struct Game : virtual kern::Blueprint, virtual car, virtual car_cpp, virtual city_stream, virtual city_stream_cpp, virtual combat_art_cpp, virtual combat_audio_cpp, virtual common, virtual drone_cpp, virtual engine_app_app, virtual engine_geo_mesh_data, virtual engine_math_math, virtual engine_physics_physics, virtual equipment, virtual equipment_cpp, virtual game, virtual game_combat, virtual game_controls, virtual game_cpp, virtual game_destruction_sync, virtual game_destruction_sync_cpp, virtual game_equipment_cpp, virtual game_gaps, virtual game_gaps_cpp, virtual game_lamps_cpp, virtual game_life, virtual game_life_cpp, virtual game_look, virtual game_look_cpp, virtual game_net, virtual game_projsync, virtual game_swarm_cpp, virtual game_toss, virtual game_town_cpp, virtual game_townsync, virtual game_traffic_cpp, virtual game_ui, virtual game_vehicle_damage_cpp, virtual game_weapons_cpp, virtual game_world, virtual game_worldsync_cpp, virtual gaps_rules, virtual gaps_rules_cpp, virtual gps_live, virtual gps_live_cpp, virtual guided, virtual interiors_cpp, virtual jet, virtual lamps, virtual life, virtual life_cpp, virtual look, virtual mp_art, virtual mp_art_cpp, virtual mp_rules_cpp, virtual net_session, virtual net_session_cpp, virtual nostr_cpp, virtual people_art_cpp, virtual people_cpp, virtual person_hit, virtual ragdoll, virtual ragdoll_cpp, virtual road_graph_cpp, virtual settings, virtual settings_cpp, virtual terrain, virtual terrain_cpp, virtual tiles_cpp, virtual touch_play_cpp, virtual vehicle_art_cpp, virtual world_marks_cpp {
  // ---- variables ----
  PadState pad = PadState{false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, false};
  List<DestroySync> dsync_ = List<DestroySync>{};
  TossState toss_ = TossState{List<TossState_Own>{}, List<TossState_Remote>{}, 0.0, 0, 0};
  List<ProjSync> projsync_ = List<ProjSync>{};
  Int proj_serial_ = 0;
  List<TownSync> townsync_ = List<TownSync>{};
  Bool puppet_world_ = false;
  Int world_sent_ = 0;
  Int world_got_ = 0;
  /// client: the host's index each of our cars plays (-1 none)
  List<Int> car_net_ = List<Int>{};
  /// client: the host's poses of each of his cars
  List<mp_PoseTimeline> car_line_ = List<mp_PoseTimeline>{};
  /// client: the host's wall clock minus our real clock (kern::now_ms)
  Float host_epoch_off_ = 0.0;
  Bool host_epoch_ok_ = false;
  Map<Int, Game_RemoteSwarm> swarms_ = Map<Int, Game_RemoteSwarm>{};
  Int swarm_serial_ = 0;
  List<Game_SwarmSent> swarm_sent_ = List<Game_SwarmSent>{};
  List<Game_SwarmFallen> swarm_fallen_ = List<Game_SwarmFallen>{};
  List<WorldLife> life_ = List<WorldLife>{};
  /// updateRocketLaser: the dot's source - a finger on the picture > the point it was lifted at (while a guided rocket flies) > the crosshair
  Bool laser_finger_ = false;
  /// updateRocketLaser: the dot's source - a finger on the picture > the point it was lifted at (while a guided rocket flies) > the crosshair
  Bool laser_pinned_ = false;
  /// updateRocketLaser: the dot's source - a finger on the picture > the point it was lifted at (while a guided rocket flies) > the crosshair
  Bool laser_valid_ = false;
  Float laser_fx_ = 0.0;
  Float laser_fy_ = 0.0;
  Vector laser_pin_ = Vector{0.0, 0.0, 0.0};
  Vector laser_dot_ = Vector{0.0, 0.0, 0.0};
  Fixed<Game_BounceFx, 8> bounce_fx_ = List<Game_BounceFx>{};
  Int bounce_fx_next_ = 0;
  List<GapsState> gaps_ = List<GapsState>{};
  Int engine_ = 0;
  /// the page runs a WebXR session ("vr 1")
  Bool vr_ = false;
  Fixed<Game_VrHand, 2> vr_hands_ = List<Game_VrHand>{};
  /// a rumble the page plays on the controller (vr json, host.mjs)
  Fixed<Float, 2> vr_pulse_ = List<Float>{0.0, 0.0};
  Bool vr_menu_open_ = false;
  Int vr_menu_hover_ = -1;
  Int vr_menu_page_ = 0;
  Bool vr_y_fired_ = false;
  Bool vr_near_vehicle_ = false;
  Float vr_y_held_ = 0.0;
  /// the wrist menu's KAMERA (game_controls.cpp)
  Bool vr_scoped_ = false;
  Text vr_action_ = "";
  Int vr_scope_tex_ = 0;
  Int vr_scope_mesh_ = 0;
  Int world_ = 0;
  CityStream city_ = CityStream{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 2600, 1, List<Vector>{}, 1, false, 0, 0, List<Vec4>{}, CityStream_NearCollision{physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, List<Int64_Int_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0}, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, 0.0, 0.0, 0.0, "", 0.0, 0, 0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, Map<Int, CityStream_Damage>{}, List<Int>{}, false, 0, 0, GeoProjector{0.0, 0.0, 1.0}, TileKey{14, 0, 0}, Map<Int64, List<CityStream_Tile>>{}, List<TileKey>{}, List<Lane>{}, false, 0, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<Int64, Terrain_Image>{}, List<TileKey>{}, Vector{1000000000.0, 1000000000.0, 0.0}, 0, 0, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0, false, Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<Int64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<Int64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<Int64, Terrain_RenderTile>{}, Map<Int64, Terrain_CollisionTile>{}, Map<Int64, Int>{}, Map<Int64, Bool>{}, Set<Int64>{}, Set<Int64>{}, Set<Int64>{}, List<Int64>{}, List<Int64>{}, List<Int64>{}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false}}, List<Vec4>{}, Int64{0x0}, -1};
  VehicleArt art_ = VehicleArt{0, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}};
  Int soldier_ = 0;
  Int drone_actor_ = 0;
  Int jet_actor_ = 0;
  Int chute_actor_ = 0;
  Drone drone_ = Drone{DroneParams{0.225, 0.127, 2300.0, 0.08, 1.1e-05, 0.105, 0.012, 45.0, 45.0, 4.0, 1.5, 0.055, 180.0, 1.225, 0.35, 0.18, Vector{0.0, 0.0, 0.0}, 0.18, 0.035, 0.005, 0.2, 0.03, 0.02, 0.03, 0.1, 0.015, -0.015, -0.07, 0.028}, fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0, false, false, false, false, false, false, false, 0.0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.39, fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}, 0.0, false}, fc_RC{List<Int>{}, false, false, false, Int64{0x0}}, fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false}, Int64{0x0}, 0, false}, List<Vector>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 1.0, 16.8, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, List<Int>{1000, 1000, 1000, 1000}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0.0, 1, 0.01, 0.01, Vector{0.0, 0.0, 0.0}, 1, true, Rng{1168318247, 0.0, false}, Rng{1905503009, 0.0, false}, Rng{1168232869, 0.0, false}, List<Float>{0.08, -0.05, 0.04}, List<Float>{0.001, -0.0015, 0.002}, 0, false, DroneTelemetry{false, true, false, false, false, 0, List<Int>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, List<Float>{}}};
  Jet jet_ = Jet{true, true, true, 1.0, 1.0, JetSpec{12000.0, Vector{100000.0, 18000.0, 115000.0}, 26.0, 9.2, 3.3, 105000.0, 185000.0, 0.34, 0.3, 0.1, 4.0, 0.42, 0.022, 0.14, -0.8, 0.02, 0.15, 0.0, -0.4, -12.0, -0.9, -0.06, -0.35, 0.08, 0.14, 0.12, -0.25, -0.03, 0.07, 0.48, 0.48, 0.5, 7.5, Vector{70000.0, 45000.0, 150000.0}, 1.0, List<JetSpec_Gear>{JetSpec_Gear{Vector{0.0, 5.2, -0.6}, 0.212, true}, JetSpec_Gear{Vector{1.9, -1.4, -0.6}, 0.394, false}, JetSpec_Gear{Vector{-1.9, -1.4, -0.6}, 0.394, false}}, 1.6, 2.05, 0.15, 0.4, 0.6, 0.8, 0.02, 0.5, 9.0, 3.0, 0.38, 3.4, 1.7, 22.0, 10.0, 6.0, 0.35, 85.0, 50.0, 1.05, 1.71}, Jet_Mode::hover, 0.0, 0.0, 0.0, 1.5707963267948966, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, true, false, true, 0, 0.0, JetTelemetry{0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0}};
  List<TrafficCar> cars_ = List<TrafficCar>{};
  Mode mode_ = Mode::foot;
  /// the car the player drives
  Int car_index_ = -1;
  Bool drone_out_ = false;
  Bool jet_placed_ = false;
  Bool world_placed_ = false;
  Bool chute_open_ = false;
  Float chute_time_ = 0.0;
  Vector chute_steer_ = Vector{0.0, 0.0, 0.0};
  DroneSticks sticks_ = DroneSticks{0.0, 0.0, 0.0, 0.0, 0.0, 1.2, false, true, false};
  /// drone: ARM in the air lands it (simulator.mjs)
  Bool landing_ = false;
  /// 0 fpv, 1 follow, 2 third (the page restores the player's last choice; FPV first)
  Int drone_cam_ = 0;
  Bool car_cockpit_ = false;
  Bool jet_cockpit_ = false;
  Camera cam_ = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Vector cam_anchor_ = Vector{0.0, 0.0, 0.0};
  Vector cam_vel_ = Vector{0.0, 0.0, 0.0};
  Vector cam_heading_ = Vector{1.0, 0.0, 0.0};
  Bool rig_init_ = false;
  Float head_yaw_ = 0.0;
  Float head_pitch_ = 0.0;
  Float dpr_ = 1.0;
  /// every mode's touch controls (touch_play.h)
  TouchPlay pad_touch_ = TouchPlay{960.0, 540.0, 0.0, 0.0, 0.0, 0.0, TouchLayout{TouchMode::off, true, "", false, false, false, 0, -1.0, false, false, false, true, false, false, false, false, false, false, false, false}, List<TouchBtn>{}, List<TouchPlay_Finger>{}, List<Text>{}, List<Text>{}, List<TouchFire>{}, -1, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, 0.0, false, List<TouchPlay_DomRect>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  Bool touch_mode_ = false;
  Float frame_w_ = 960.0;
  Float frame_h_ = 540.0;
  /// the town
  People people_ = People{List<People_Person>{}, List<CrimeReport>{}, List<People_Death>{}, List<People_Fall>{}, List<People_RemoteHit>{}, List<People_Hurt>{}, false, false, 60, 1, 0, 0, 170.0, 190.0};
  PeopleArt people_art_ = PeopleArt{0, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}};
  CombatArt combat_art_ = CombatArt{0, List<CombatArt_Part>{}};
  CombatState combat_ = CombatState{combat_MilitaryClock{false, -1.0, -1.0, 0.0}, 0, 0, 0, 0.0, List<Heli>{}, List<Tank>{}, List<Shell>{}, List<SoldierAi>{}, List<Float>{}, true, List<FlakGun>{}, List<FlakRound>{}, List<Puff>{}, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0, 0, List<FighterJet>{}, List<Bomb>{}, List<Blast>{}, List<Flare>{}, 50.0, -1000000000.0, List<Tracer>{}, List<Debris>{}, List<Int>{}, List<Bool>{}, List<Bool>{}, false, 0, 0, 0, combat_ZombieRounds{false, 0, 0, 0, 0.0, -1.0}, List<ZombieAi>{}, true, false, -99.0, List<EnemyAi>{}, List<EnemyRocket>{}, combat_Chaos{0, 1, -1.0, 0.0}, -99.0, -1.0, 0, combat_Fun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, -1.0, combat_Race{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}, false, Vector{0.0, 0.0, 0.0}, "", "", -99.0, -99.0, false, 0.0};
  /// GUIDED missiles in flight (game_guided.cpp)
  List<guided_Flight> guided_ = List<guided_Flight>{};
  Fixed<Int, 16> sfx_cursor_ = List<Int>{};
  Bool combat_audio_ready_ = false;
  RoadGraph roads_ = RoadGraph{Int64{0xffffffffffffffff}, List<RoadGraph_Node>{}, Map<Int64, Int>{}};
  List<PoliceUnit> units_ = List<PoliceUnit>{};
  Fixed<PoliceDrone, 5> pdrones_ = List<PoliceDrone>{};
  Float next_drone_wave_ = -1.0;
  Bool drone_wave_live_ = false;
  /// weapons
  List<Projectile> projectiles_ = List<Projectile>{};
  weapons_Id foot_weapon_ = weapons_Id::glock;
  Fixed<Int, 16> ammo_mag_ = List<Int>{};
  Fixed<Int, 16> ammo_reserve_ = List<Int>{};
  Int grenades_ = 4;
  Float next_attack_ = 0.0;
  Float next_grenade_ = 0.0;
  /// multiplayer
  net_Session net_ = net_Session{net_Session_Role::none, 0, 0, 0, net_Address{Int64{0x0}}, List<net_Remote>{}, List<net_Hit>{}, List<net_Hit>{}, List<net_Kill>{}, List<net_Fx>{}, List<net_Swarm>{}, List<net_SwarmHit>{}, List<Int_row>{}, List<net_PingEvent>{}, List<net_VoiceFrame>{}, List<Int>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, false, false, 0.0, 0, 0.0, List<Int>{}, Vector{0.0, 0.0, 0.0}, 255, net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_Scoreboard{List<mp_Score>{}}, false, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_SwarmParts>{}, 0, 0, false, List<Int_Float_pair>{}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, 0, "Aenderungen nur einstimmig", 0, 0, 0, seats_Table{List<seats_Row>{}, 0}, 0.0};
  /// pings, feed, voice, big map (game_net.h)
  List<MpLocal> mp_ = List<MpLocal>{};
  List<Game_Lamp> lamps_ = List<Game_Lamp>{};
  Float night_ = 0.0;
  Bool show_lamps_ = true;
  /// jet: jet_mode.mjs HP_MAX 400
  Float star_since_ = -1.0;
  /// jet: jet_mode.mjs HP_MAX 400
  Float last_wave_ = -99.0;
  /// jet: jet_mode.mjs HP_MAX 400
  Float drone_hp_ = 100.0;
  /// jet: jet_mode.mjs HP_MAX 400
  Float jet_hp_ = 400.0;
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
  Float time_ = 0.0;
  Rng rng_ = Rng{20261029, 0.0, false};
  /// audio
  Int mixer_ = 0;
  Int bank_ = 0;
  List<Float> audio_out_ = List<Float>{};
  /// the original's sound on the page (game_audio_web.cpp): events and state for web/audio.mjs
  Bool web_audio_ = false;
  Text osfx_ = "";
  /// start parameters
  Float lon_ = 9.1766;
  /// start parameters
  Float lat_ = 47.66;
  Bool mobile_ = false;
  /// menus, settings, start, equipment, presentation (game_ui.h)
  UiState ui_ = UiState{Settings{List<Float>{}, 1}, SettingsTable{List<Option>{}, List<OptSpec>{}}, 0, false, false, 0, false, 0.0, 0.0, 0, 0, 0, Vector{1000000000.0, 1000000000.0, 0.0}, 0.0, -1, -1, GpsLive{GpsLive_View::eye, 0, 0, false, false, false, false, 0.0, -1000000000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}}, 0, "", Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, 0, 0, 0, List<Vector>{}, false, 0.0, false, Jetpack{1.0, false, false, 0, 0.0, -1.0, -99.0}, false, false, 0.0, List<Int64>{}, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, FlightLog{List<Flight>{}, List<FlightLog_Event>{}, Flight{0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", List<Vector>{}}, false, false, Vector{0.0, 0.0, 0.0}, 0.0, -1000000000.0, 1, 0}, "", MusicSynth{0.0, 0, -2, -2, 0.0, 0.0, 1.0, Int64{0x0}, 0, 1, 0, 1, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, List<MusicSynth_Voice>{}, 0.0, 0.0, List<Float>{}, 0}, 0, true, "", "", 99.0, Vector{0.0, 0.0, 0.0}, false, 0};
  /// performance and the look (game_look.h)
  LookState look_ = LookState{perf_Meter{0.0, 0.0, List<Float>{}, List<Float>{}, 0.0, 0.0, -1.0}, perf_Governor{4, 4, true, 0.016666666666666666, 0.0, 0.0, 0.0, 0.0, List<Float>{}, List<Float>{}, 0}, false, false, false, -1, 0, 170.0, 220.0, look_Shake{0.0, 17.3}, look_Stabilizer{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, look_SpeedLines{List<look_SpeedLines_Streak>{}, false, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, Rng{24301, 0.0, false}}, Vector{0.0, 0.0, 0.0}, false, -1, 0.4, 0.0, 0, 16, List<LookState_Blast>{}, List<LookState_Glint>{}, -1.0, -99.0, List<Vector>{}, 0, 0, List<Float>{}, List<Float>{}, List<Int>{}, List<Int>{}, List<Int>{}, List<Int>{}, false, -1, -99.0, 0.0, 0.0, 0.0, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, -30.0}, -99.0, 30.0, -99.0, false, -1, 0, List<Int>{}};
  /// M3 life (last: destroyed before world_)
  List<GameLife> glife_ = List<GameLife>{};
  /// (of the program: combat::kContracts)
  Int combat_kContracts = 0;
  /// (of the program: combat::kGates)
  Int combat_kGates = 0;
  /// (kept by combat_rules)
  List<combat_Contract> combat_rules_g_contracts = List<combat_Contract>{};
  /// (kept by combat_rules)
  List<combat_EnemySpec> combat_rules_g_enemies = List<combat_EnemySpec>{};
  /// (kept by combat_rules)
  List<combat_Gate> combat_rules_g_gates = List<combat_Gate>{};
  /// (kept by life_draw_ragdolls)
  List<Ragdolls_Draw> life_draw_ragdolls_bones = List<Ragdolls_Draw>{};
  /// (kept by mp_rules)
  List<Int> mp_rules_g_palette = List<Int>{};
  /// (kept by sfx_id)
  List<Text> sfx_id_ids = List<Text>{};
  KERN_CONST combat_EnemySpec kNoEnemy = combat_EnemySpec{"", 1.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0, 1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}};
  KERN_CONST combat_Contract kNoContract = combat_Contract{"", "", "", 1, 0};
  KERN_CONST combat_Gate kNoGate = combat_Gate{"", Vector{0.0, 0.0, 0.0}, 1.0, 1.0, false};
  KERN_CONST Float kRespawnCooldown = 3.0;
  /// The number of each value of net_PlayerFlag, in their order
  KERN_CONST List<Int> net_PlayerFlag_numbers = List<Int>{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
  KERN_CONST Float kRad = 57.29577951308238;
  KERN_CONST Float kPi = 3.141592653589793;
  /// m: the safe margin a player is put down over the ground
  KERN_CONST Float kSpawnLift = 0.35;
  KERN_CONST Float kDeg = 0.017453292519943295;
  /// player_vehicle_runtime_v2.mjs: the drone starts 2.9 m ahead
  KERN_CONST Float kDroneAhead = 2.9;
  /// jet_mode.mjs
  KERN_CONST Float kEjectUp = 28.0;
  /// jet_mode.mjs
  KERN_CONST Float kChuteOpen = 1.1;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSink = 6.5;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSteer = 4.0;
  KERN_CONST Fixed<Int, 11> kVariants = List<Int>{3, 4, 4, 4, 2, 4, 2, 3, 4, 3, 2};
  KERN_CONST Int kSampleRate = 44100;
  KERN_CONST Fixed<MusicSynth_Station, 5> kStations = List<MusicSynth_Station>{MusicSynth_Station{"KERN FM", "Synthwave", 100.0, 11}, MusicSynth_Station{"BODENSEE BEATS", "House", 124.0, 23}, MusicSynth_Station{"LOFI LAGUNE", "Lo-Fi", 78.0, 37}, MusicSynth_Station{"8-BIT RADIO", "Chiptune", 144.0, 41}, MusicSynth_Station{"NACHTSCHICHT", "Techno", 132.0, 53}};
  KERN_CONST Int kMenu = -1;
  KERN_CONST Float kG = 9.80665;
  /// GRENADE_RESTITUTION
  KERN_CONST Float kRestitution = 0.58;
  /// GRENADE_SURFACE_DAMPING
  KERN_CONST Float kSurfaceDamping = 0.88;
  /// GRENADE_MAX_BOUNCES
  KERN_CONST Int kMaxBounces = 7;
  /// gameplay_final_runtime_v2.mjs
  KERN_CONST Float kGrenadeR = 0.05265;
  /// ROCKET_FOOT_TTL_MS
  KERN_CONST Float kRocketTtl = 9.0;
  /// GRENADE_TTL_MS
  KERN_CONST Float kGrenadeTtl = 5.6;
  /// ROCKET_LASER_RANGE_M
  KERN_CONST Float kLaserRange = 420.0;
  /// ROCKET_TURN_RAD_S
  KERN_CONST Float kRocketTurn = 2.8;
  /// ROCKET_FOOT_SPEED_MPS, ROCKET_FOOT_ACCEL_MPS2
  KERN_CONST Float kRocketTop = 40.0;
  /// ROCKET_FOOT_SPEED_MPS, ROCKET_FOOT_ACCEL_MPS2
  KERN_CONST Float kRocketAccel = 55.0;
  /// GRENADE_GRAVITY_MPS2
  KERN_CONST Float kGrenadeGravity = 9.81;
  /// GRENADE_FLOOR_NORMAL_Z
  KERN_CONST Float kFloorNormalZ = 0.58;
  /// SpawnCity's numbers. Damage of the hitscan guns is SpawnCity's multiplayer damage (VS_WEAPON_DAMAGE smg 13, glock 26, sniper 95) times the zone (head 3, torso 1, legs .65).
  KERN_CONST Fixed<weapons_Spec, 14> kTable = List<weapons_Spec>{weapons_Spec{"MP", 55.0, 180.0, 13.0, 0.0, 240.0, 1}, weapons_Spec{"Glock", 110.0, 240.0, 26.0, 0.0, 0.0, 2}, weapons_Spec{"Sniper .50", 1150.0, 1500.0, 95.0, 0.0, 850.0, 3}, weapons_Spec{"Granatwerfer 40 mm", 760.0, 300.0, 125.0, 8.5, 40.0, 5}, weapons_Spec{"Raketenwerfer", 900.0, 420.0, 160.0, 9.5, 40.0, 6}, weapons_Spec{"Faeuste", 300.0, 1.8, 24.0, 0.0, 0.0, 7}, weapons_Spec{"Messer", 380.0, 1.6, 45.0, 0.0, 0.0, 8}, weapons_Spec{"Handgranate", 4000.0, 40.0, 125.0, 8.5, 0.0, 0}, weapons_Spec{"Drohnen-MG", 92.0, 650.0, 13.0, 0.0, 0.0, 0}, weapons_Spec{"Drohnen-Raketen", 950.0, 220.0, 100.0, 8.0, 31.0, 0}, weapons_Spec{"GUIDED", 5000.0, 600.0, 34.0, 4.0, 34.0, 0}, weapons_Spec{"Bordkanone", 55.0, 1800.0, 30.0, 0.0, 0.0, 0}, weapons_Spec{"Luft-Rakete", 2400.0, 1300.0, 120.0, 8.0, 260.0, 0}, weapons_Spec{"Bombe", 450.0, 0.0, 170.0, 17.0, 0.0, 0}};
  /// vertex locations 14 and 15
  KERN_CONST Int kMaxExtensionAttributes = 2;
  /// jet_mode.mjs HP_MAX
  KERN_CONST Float kJetHpMax = 400.0;
  /// startWorldCriticalDamage delayMs 12000 (jet)
  KERN_CONST Float kJetBurnS = 12.0;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kPoleH = 6.2;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kArm = 1.42;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kHeadZ = 6.0;
  /// world.* switches that make the world (friendly fire is the rule voted on: game_rules_sync)
  KERN_CONST Fixed<Opt, 13> kWorldRules = List<Opt>{Opt::world_people, Opt::world_traffic, Opt::world_dogs, Opt::world_cats, Opt::world_birds, Opt::world_zombies, Opt::world_police, Opt::world_military, Opt::world_flak, Opt::world_jetpack, Opt::world_terrain, Opt::world_guided, Opt::world_always_day};
  /// a seated passenger: hips 0.5 m over the road instead of 0.92 (game_seats.cpp)
  KERN_CONST Float kSeatDrop = 0.42;
  KERN_CONST Float kArriveM = 25.0;
  /// world_map_full.mjs: a new route at most every 10 s
  KERN_CONST Float kNavRerouteMs = 10000.0;
  /// The number of each value of net_SwarmFlag, in their order
  KERN_CONST List<Int> net_SwarmFlag_numbers = List<Int>{128, 1, 2, 4, 3};
  KERN_CONST Float kFallenMs = 1500.0;
  KERN_CONST Int kPerson = 0;
  KERN_CONST Int kBird = 3;
  KERN_CONST Int kCow = 1;
  KERN_CONST Int kPet = 2;
  KERN_CONST Float kSpawnMax = 125.0;
  KERN_CONST Float kSpawnMin = 70.0;
  KERN_CONST Int kPlanksMax = 6;
  KERN_CONST Vector kTowYellow = Vector{0.9490196078431372, 0.7176470588235294, 0.0196078431372549};
  /// s of Y held = RESET
  KERN_CONST Float kResetHold = 1.0;
  /// Magazines and spare rounds on foot (gameplay_final_runtime_v2.mjs WEAPONS).
  KERN_CONST Fixed<Loadout, 7> kFoot = List<Loadout>{Loadout{weapons_Id::smg, 30, 150}, Loadout{weapons_Id::glock, 17, 85}, Loadout{weapons_Id::sniper, 5, 25}, Loadout{weapons_Id::grenade_launcher, 6, 18}, Loadout{weapons_Id::rocket, 1, 6}, Loadout{weapons_Id::fists, -1, -1}, Loadout{weapons_Id::knife, -1, -1}};
  /// hand_grenade.mjs FUSE_MS 4000
  KERN_CONST Float kGrenadeFuse = 4.0;
  /// hand_grenade.mjs FUSE_MS 4000
  KERN_CONST Float kGrenadeThrow = 14.0;
  KERN_CONST Float kWaterDensity = 1000.0;

  // ---- functions ----
  struct enemy_specResult { combat_EnemySpec Result = combat_EnemySpec{"", 0.0, 0.0, 0.0, 0.0, 0, 0.0, 0.0, 0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}; };
  KERN_PURE enemy_specResult enemy_spec(combat_EnemyType t = combat_EnemyType::cop) {
    return {logic_select<combat_EnemySpec>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(enum_index<combat_EnemyType>(t)), kern::to<Float>(list_length<combat_EnemySpec>(combat_rules_g_enemies))), 0.5), combat_rules_g_enemies[enum_index<combat_EnemyType>(t)], kNoEnemy)};
  }

  struct contractResult { combat_Contract Result = combat_Contract{"", "", "", 0, 0}; };
  KERN_PURE contractResult contract(Int index = 0) {
    return {logic_select<combat_Contract>(math_compare<">">(kern::to<Float>(combat_kContracts), 0.0), combat_rules_g_contracts[kern::to<Int>(kern::formula([](Float index, Float combat_kContracts) -> Float { return kern::math::mod((kern::math::mod(index, combat_kContracts) + combat_kContracts), combat_kContracts); }, kern::to<Float>(index), kern::to<Float>(combat_kContracts)))], kNoContract)};
  }

  struct gateResult { combat_Gate Result = combat_Gate{"", Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false}; };
  KERN_PURE gateResult gate(Int i = 0) {
    return {logic_select<combat_Gate>(math_compare<">">(kern::to<Float>(combat_kGates), 0.0), combat_rules_g_gates[kern::to<Int>(kern::formula([](Float i, Float combat_kGates) -> Float { return kern::math::mod((kern::math::mod(i, combat_kGates) + combat_kGates), combat_kGates); }, kern::to<Float>(i), kern::to<Float>(combat_kGates)))], kNoGate)};
  }

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
    engine_game_world_physics(n1048586, world_);
    physics = n1048586.result;
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

  struct web_audio_stateResult { Text Result = ""; };
  web_audio_stateResult web_audio_state() {
    Text s = "";
    Text b = "";
    Vector fwd = Vector{0.0, 0.0, 0.0};
    game_SoldierState ss = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0};
    Bool have = false;
    Bool soldier_state = false;
    Vector feet = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool air = false;
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool first = false;
    Bool skip = false;
    Bool first_2 = false;
    Bool skip_2 = false;
    Bool first_3 = false;
    Bool skip_3 = false;
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip_4 = false;
    Float now = 0.0;
    Float sim_time = 0.0;
    Bool first_4 = false;
    Bool first_5 = false;
    Bool skip_5 = false;
    Bool first_6 = false;
    Bool skip_6 = false;
    Int kind = 0;
    s = "";
    b = "";
    fwd = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{0.0, 1.0, 0.0});
    b = text_format<"\"paused\":%d,\"mode\":\"%s\",\"fwd\":[%.3f,%.3f,%.3f],\"L\":">(511, kern::to<Int>(kern::formula([](Float paused) -> Float { return ((paused != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(ui_.paused))), game_cpp::mode_name(mode_).Result, vector_break(fwd).x, vector_break(fwd).y, vector_break(fwd).z);
    s = text_append(s, b, "");
    web_audio_state_v3(b, s, cam_.eye);
    b = text_format<",\"set\":{\"soundEnabled\":%d,\"musicEnabled\":%d,\"musicVolume\":%.0f,\"droneVolume\":%.0f,\"shotsVolume\":%.0f,\"fxVolume\":%.0f,\"footstepsVolume\":%.0f,\"vehicleVolume\":%.0f,\"ambientVolume\":%.0f}">(511, kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::audio_on).Result))), kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::music_on).Result))), settings::Settings_get(ui_.settings, Opt::music_volume).Result, settings::Settings_get(ui_.settings, Opt::audio_drone).Result, settings::Settings_get(ui_.settings, Opt::audio_shots).Result, settings::Settings_get(ui_.settings, Opt::audio_fx).Result, settings::Settings_get(ui_.settings, Opt::audio_steps).Result, settings::Settings_get(ui_.settings, Opt::audio_vehicle).Result, settings::Settings_get(ui_.settings, Opt::audio_ambient).Result);
    s = text_append(s, b, "");
    ss = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0};
    engine_game_world_soldier_state(n2097163, world_, soldier_, ss);
    soldier_state = n2097163.result;
    ss = n2097163.out;
    have = soldier_state;
    n2097164 = matrix_of(soldier_);
    a = n2097164.Result;
    feet = engine_math_math::Mat4_translation(a).Result;
    air = logic_and(enum_equal<Mode>(mode_, Mode::foot), math_compare<">">(kern::formula([](Float z, Float the_result) -> Float { return (z - the_result); }, vector_break(feet).z, city_stream_cpp::CityStream_ground_height(city_, vector_break(feet).x, vector_break(feet).y).Result), 1.15));
    b = text_format<",\"walk\":{\"dead\":%d,\"air\":%d,\"p\":">(511, kern::to<Int>(kern::formula([](Float have, Float dead) -> Float { return (((have != 0.0) && (dead != 0.0)) ? 1.0 : 0.0); }, kern::to<Float>(have), kern::to<Float>(ss.dead))), kern::to<Int>(kern::formula([](Float air) -> Float { return ((air != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(air))));
    s = text_append(s, b, "");
    web_audio_state_v3(b, s, logic_select<Vector>(enum_equal<Mode>(mode_, Mode::foot), cam_.eye, feet));
    s = text_append(s, "}", "");
    if (drone_out_) {
      b = text_format<",\"drone\":{\"st\":%u,\"arm\":%d,\"d\":%.4f,\"w\":[%.2f,%.2f,%.2f,%.2f],\"mt\":[%.5f,%.5f,%.5f,%.5f],\"pt\":[%.5f,%.5f,%.5f,%.5f],\"p\":">(511, drone_.tele_.state, kern::to<Int>(kern::formula([](Float arm) -> Float { return ((arm != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(sticks_.arm))), drone_.p_.prop_d, drone_.tele_.omega[0], drone_.tele_.omega[1], drone_.tele_.omega[2], drone_.tele_.omega[3], drone_.tele_.motor_torque[0], drone_.tele_.motor_torque[1], drone_.tele_.motor_torque[2], drone_.tele_.motor_torque[3], drone_.tele_.prop_torque[0], drone_.tele_.prop_torque[1], drone_.tele_.prop_torque[2], drone_.tele_.prop_torque[3]);
      s = text_append(s, b, "");
      n2097165 = matrix_of(drone_actor_);
      a_2 = n2097165.Result;
      web_audio_state_v3(b, s, engine_math_math::Mat4_translation(a_2).Result);
      s = text_append(s, "}", "");
    }
    if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0))) {
      b = text_format<",\"car\":{\"id\":%d,\"v\":%.2f,\"th\":%.3f}">(511, car_index_, kern::formula([](Float the_result) -> Float { return kern::math::abs(the_result); }, car::Car_forward_speed(cars_[car_index_].car[0]).Result), car::Car_throttle(cars_[car_index_].car[0]).Result);
      s = text_append(s, b, "");
    }
    b = text_format<",\"jp\":[%d,%.2f],\"rj\":[">(511, kern::to<Int>(kern::formula([](Float thrusting, Float paused) -> Float { return (((thrusting != 0.0) && (!(paused != 0.0))) ? 1.0 : 0.0); }, kern::to<Float>(ui_.jetpack.thrusting), kern::to<Float>(ui_.paused))), ui_.jet_acc);
    s = text_append(s, b, "");
    first = true;
    for (auto it2097166 = flow_for(n2097166, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it2097166.next();) {
      skip = false;
      if (logic_not(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n2097166.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetpack)))), 0.5))) {
        skip = true;
      }
      if (logic_not(skip)) {
        b = text_format<"%s[%zu,%.2f,%.2f,%.2f]">(511, logic_select<Text>(first, "", ","), n2097166.index, vector_break(net_.remotes_[n2097166.index].shown).x, vector_break(net_.remotes_[n2097166.index].shown).y, kern::formula([](Float z) -> Float { return (z - 0.6); }, vector_break(net_.remotes_[n2097166.index].shown).z));
        s = text_append(s, b, "");
        first = false;
      }
    }
    s = text_append(s, "],\"rp\":[", "");
    first_2 = true;
    for (auto it2097167 = flow_for(n2097167, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it2097167.next();) {
      skip_2 = false;
      if (logic_not(enum_equal<net_PlayerMode>(net_.remotes_[n2097167.index].state.mode, net_PlayerMode::foot))) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        b = text_format<"%s[%u,%.2f,%.2f,%.2f,%d]">(511, logic_select<Text>(first_2, "", ","), net_.remotes_[n2097167.index].state.slot, vector_break(net_.remotes_[n2097167.index].person_shown).x, vector_break(net_.remotes_[n2097167.index].person_shown).y, vector_break(net_.remotes_[n2097167.index].person_shown).z, kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(net_.remotes_[n2097167.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetpack))))));
        s = text_append(s, b, "");
        first_2 = false;
      }
    }
    s = text_append(s, "],\"pc\":[", "");
    first_3 = true;
    for (auto it2097168 = flow_for(n2097168, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it2097168.next();) {
      skip_3 = false;
      if (logic_or(logic_or(logic_or(math_compare<"<">(kern::to<Float>(units_[n2097168.index].car), 0.0), enum_equal<PoliceUnit_State>(units_[n2097168.index].state, PoliceUnit_State::abandoned)), enum_equal<PoliceUnit_State>(units_[n2097168.index].state, PoliceUnit_State::wreck)), enum_equal<PoliceUnit_State>(units_[n2097168.index].state, PoliceUnit_State::leave))) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(car_index_), kern::to<Float>(units_[n2097168.index].car)))) {
          skip_3 = true;
        }
        if (logic_not(skip_3)) {
          if (logic_not(first_3)) {
            s = text_append(s, ",", "");
          }
          n2097169 = matrix_of(cars_[units_[n2097168.index].car].actor);
          a_3 = n2097169.Result;
          web_audio_state_v3(b, s, vector_add(engine_math_math::Mat4_translation(a_3).Result, Vector{0.0, 0.0, 0.9}));
          first_3 = false;
        }
      }
    }
    s = text_append(s, "],\"pd\":[", "");
    first_3 = true;
    for (auto it2097170 = flow_for(n2097170, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it2097170.next();) {
      skip_4 = false;
      if (logic_or(logic_or(logic_not(pdrones_[n2097170.index].active), pdrones_[n2097170.index].retreating), pdrones_[n2097170.index].emp)) {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        if (logic_not(first_3)) {
          s = text_append(s, ",", "");
        }
        web_audio_state_v3(b, s, pdrones_[n2097170.index].pos);
        first_3 = false;
      }
    }
    s = text_append(s, "],\"fj\":[", "");
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    first_4 = true;
    for (auto it2097171 = flow_for(n2097171, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<FighterJet>(combat_.jets))))); it2097171.next();) {
      b = text_format<"%s[%.2f,%.2f,%.2f,%.3f]">(511, logic_select<Text>(first_4, "", ","), combat_.jets[n2097171.index].x, combat_.jets[n2097171.index].y0, combat_.jets[n2097171.index].z, kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, combat_.jets[n2097171.index].t0));
      s = text_append(s, b, "");
      first_4 = false;
    }
    s = text_append(s, "]", "");
    if (logic_and(enum_equal<Mode>(mode_, Mode::jet), jet_placed_)) {
      b = text_format<",\"jet\":{\"flight\":%d,\"lever\":%.3f,\"nozzle\":%.3f,\"thrust\":%.0f,\"dry\":%.0f,\"cockpit\":%d}">(511, kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(enum_equal<Jet_Mode>(jet::Jet_mode(jet_).Result, Jet_Mode::flight)))), jet::Jet_lever(jet_).Result, jet::Jet_nozzle(jet_).Result, jet::Jet_thrust(jet_).Result, jet_.s_.thrust_dry, kern::to<Int>(kern::formula([](Float jet_cockpit_) -> Float { return ((jet_cockpit_ != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(jet_cockpit_))));
      s = text_append(s, b, "");
    }
    s = text_append(s, ",\"gm\":[", "");
    first_5 = true;
    for (auto it2097172 = flow_for(n2097172, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))); it2097172.next();) {
      skip_5 = false;
      if (logic_not(guided_[n2097172.index].m.motor)) {
        skip_5 = true;
      }
      if (logic_not(skip_5)) {
        b = text_format<"%s[%.3f,%.2f,%.2f,%.2f]">(511, logic_select<Text>(first_5, "", ","), guided_[n2097172.index].m.born, vector_break(guided_[n2097172.index].m.p).x, vector_break(guided_[n2097172.index].m.p).y, vector_break(guided_[n2097172.index].m.p).z);
        s = text_append(s, b, "");
        first_5 = false;
      }
    }
    s = text_append(s, "],\"rk\":[", "");
    first_6 = true;
    for (auto it2097173 = flow_for(n2097173, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it2097173.next();) {
      skip_6 = false;
      if (logic_or(logic_not(projectiles_[n2097173.index].alive), projectiles_[n2097173.index].guided)) {
        skip_6 = true;
      }
      if (logic_not(skip_6)) {
        kind = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((a != 0.0) ? 1.0 : ((a_2 != 0.0) ? 2.0 : ((a_3 != 0.0) ? 3.0 : 0.0))); }, kern::to<Float>(enum_equal<weapons_Id>(projectiles_[n2097173.index].id, weapons_Id::rocket)), kern::to<Float>(enum_equal<weapons_Id>(projectiles_[n2097173.index].id, weapons_Id::drone_rockets)), kern::to<Float>(enum_equal<weapons_Id>(projectiles_[n2097173.index].id, weapons_Id::jet_missile))));
        if (logic_not(math_compare<"!=">(kern::to<Float>(kind), 0.0))) {
          skip_6 = true;
        }
        if (logic_not(skip_6)) {
          b = text_format<"%s[%.3f,%.2f,%.2f,%.2f,%d]">(511, logic_select<Text>(first_6, "", ","), kern::formula([](Float a, Float kind) -> Float { return (a + (0.0001 * kind)); }, projectiles_[n2097173.index].born, kern::to<Float>(kind)), vector_break(projectiles_[n2097173.index].pos).x, vector_break(projectiles_[n2097173.index].pos).y, vector_break(projectiles_[n2097173.index].pos).z, kind);
          s = text_append(s, b, "");
          first_6 = false;
        }
      }
    }
    b = text_format<"],\"gps\":%d">(511, kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(gps_live::GpsLive_active(ui_.gps).Result))));
    s = text_append(s, b, "");
    return {s};
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
        n4194317 = vr_hand_ray(from, dir);
        both_2 = n4194317.Result;
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

  struct Game_ctl_view_json_screenResult { Bool Result = false; };
  Game_ctl_view_json_screenResult Game_ctl_view_json_screen(Float& x, Float& y, Vector p = Vector{0.0, 0.0, 0.0}) {
    Float nx = 0.0;
    Float ny = 0.0;
    Float depth = 0.0;
    Bool on = false;
    nx = 0.0;
    ny = 0.0;
    depth = 0.0;
    n4194327 = project_screen(nx, ny, depth, p);
    on = n4194327.Result;
    x = kern::formula([](Float nx, Float frame_w_) -> Float { return (kern::math::div((nx + 1.0), 2.0) * frame_w_); }, nx, frame_w_);
    y = kern::formula([](Float ny, Float frame_h_) -> Float { return (kern::math::div((1.0 - ny), 2.0) * frame_h_); }, ny, frame_h_);
    return {logic_and(on, math_compare<">">(depth, 0.0))};
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

  void set_touch_frame(Float w = 0.0, Float h = 0.0, Float sl = 0.0, Float st = 0.0, Float sr = 0.0, Float sb = 0.0, Float dpr = 0.0) {
    frame_w_ = w;
    frame_h_ = h;
    dpr_ = kern::formula([](Float dpr) -> Float { return ((dpr > 0.0) ? dpr : 1.0); }, dpr);
    touch_play_cpp::TouchPlay_set_frame(pad_touch_, w, h, sl, st, sr, sb);
  }

  struct touch_view_jsonResult { Text Result = ""; };
  touch_view_jsonResult touch_view_json() {
    Text raw = "";
    Text out = "";
    Int c = 0;
    if (logic_not(touch_mode_)) {
      return {"null"};
    }
    n4194342 = touch_play_cpp::TouchPlay_view_json(pad_touch_);
    raw = n4194342.Result;
    out = "";
    out = text_reserve(out, kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 16.0); }, kern::to<Float>(text_length(raw)))));
    for (auto it4194343 = flow_for(n4194343, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(raw))))); it4194343.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(raw, n4194343.index))));
      if (math_compare<"==">(kern::to<Float>(c), 10.0)) {
        out = text_append(out, "\\n", "");
      } else if (math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c) -> Float { return kern::math::band(c, 255.0); }, kern::to<Float>(c))))), 32.0)) {
        out = text_append(out, text_of_code(32), "");
      } else {
        out = text_append(out, text_of_code(c), "");
      }
    }
    return {out};
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
    engine_game_world_actor_matrix(n5242890, world_, a, m);
    actor_matrix = n5242890.result;
    m = n5242890.out;
    return {m};
  }

  struct player_positionResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  player_positionResult player_position() {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector p = Vector{0.0, 0.0, 0.0};
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Bool both = false;
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0))) {
      n5242891 = matrix_of(cars_[car_index_].actor);
      a = n5242891.Result;
      return {engine_math_math::Mat4_translation(a).Result};
    }
    if (enum_equal<Mode>(mode_, Mode::jet)) {
      n5242892 = matrix_of(jet_actor_);
      a_2 = n5242892.Result;
      return {engine_math_math::Mat4_translation(a_2).Result};
    }
    if (enum_equal<Mode>(mode_, Mode::parachute)) {
      n5242893 = matrix_of(chute_actor_);
      a_3 = n5242893.Result;
      return {engine_math_math::Mat4_translation(a_3).Result};
    }
    if (gaps_riding().Result) {
      p = Vector{0.0, 0.0, 0.0};
      q = Quat{0.0, 0.0, 0.0, 1.0};
      both = logic_not(gaps_ride_car().Result);
      if (both) {
        n5242895 = gaps_ride_pose(p, q);
        both = n5242895.Result;
      }
      if (both) {
        return {p};
      }
    }
    n5242896 = matrix_of(soldier_);
    a_4 = n5242896.Result;
    return {engine_math_math::Mat4_translation(a_4).Result};
  }

  struct focus_positionResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  focus_positionResult focus_position() {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_and(enum_equal<Mode>(mode_, Mode::drone), drone_out_)) {
      n5242897 = matrix_of(drone_actor_);
      a = n5242897.Result;
      return {engine_math_math::Mat4_translation(a).Result};
    }
    n5242898 = player_position();
    return {n5242898.Result};
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
    for (auto it5242899 = flow_for(n5242899, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it5242899.next();) {
      skip = false;
      if (logic_or(logic_not(city_.lanes_[n5242899.index].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(city_.lanes_[n5242899.index].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(city_.lanes_[n5242899.index].points))), 0.5))) {
          a = city_.lanes_[n5242899.index].points[k];
          b = city_.lanes_[n5242899.index].points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))];
          ab = vector_sub(b, a);
          l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(ab).x, vector_break(ab).y);
          t = kern::formula([](Float l2, Float a) -> Float { return ((l2 > 1e-06) ? a : 0.0); }, l2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(c).x, vector_break(a).x, vector_break(ab).x, vector_break(c).y, vector_break(a).y, vector_break(ab).y, l2), 0.0, 1.0));
          q = vector_add(a, vector_scale(ab, t));
          if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(q).x, vector_break(c).y, vector_break(q).y), kern::formula([](Float reach, Float a) -> Float { return (reach + (a * 0.5)); }, reach, city_.lanes_[n5242899.index].width))) {
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
    engine_game_world_set_mode(n5242903, world_, game_SimMode::simulate);
    set_mode = n5242903.result;
  }

  struct standing_feetResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  standing_feetResult standing_feet(Vector feet = Vector{0.0, 0.0, 0.0}) {
    Float ground = 0.0;
    the_RayResult_ r = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics = 0;
    Float z = 0.0;
    ground = city_stream_cpp::CityStream_ground_height(city_, vector_break(feet).x, vector_break(feet).y).Result;
    engine_game_world_physics(n5242905, world_);
    physics = n5242905.result;
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
    n5242906 = standing_feet(feet);
    engine_game_world_teleport(n5242907, world_, soldier_, vector_add(n5242906.Result, vector_make(0.0, 0.0, kern::formula([](Float kSpawnLift) -> Float { return (0.73 + kSpawnLift); }, kSpawnLift))), list_make<Vector, "1">(rot));
    teleport = n5242907.result;
    engine_game_world_set_actor_velocity(n5242908, world_, soldier_, Vector{0.0, 0.0, 0.0});
    set_actor_velocity = n5242908.result;
    spawn_guard_ = vector_make(vector_break(feet).x, vector_break(feet).y, yaw_deg);
    sim_time = engine_game_world_sim_time(world_);
    spawn_guard_until_ = kern::formula([](Float sim_time) -> Float { return (sim_time + 20.0); }, sim_time);
    spawn_guard_ready_at_ = -1.0;
  }

  void show_soldier(Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw_deg = 0.0) {
    Bool actor_alive = false;
    Bool set_mode = false;
    Vector rot = Vector{0.0, 0.0, 0.0};
    actor_alive = engine_game_world_actor_alive(world_, soldier_);
    if (logic_not(actor_alive)) {
      engine_game_world_set_alive(world_, soldier_, true);
    }
    engine_game_world_set_mode(n5242904, world_, game_SimMode::play);
    set_mode = n5242904.result;
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
    n5242909 = matrix_of(soldier_);
    a = n5242909.Result;
    at = engine_math_math::Mat4_translation(a).Result;
    if (logic_and(math_compare<">">(now, spawn_guard_until_), logic_or(math_compare<">=">(spawn_guard_ready_at_, 0.0), terrain_cpp::Terrain_ground_ready(city_.terrain_, vector_break(at).x, vector_break(at).y).Result))) {
      spawn_guard_until_ = -1.0;
      return;
    }
    n5242910 = matrix_of(soldier_);
    a_2 = n5242910.Result;
    c = engine_math_math::Mat4_translation(a_2).Result;
    feet = vector_make(vector_break(c).x, vector_break(c).y, kern::formula([](Float z) -> Float { return (z - 0.73); }, vector_break(c).z));
    ready = terrain_cpp::Terrain_ground_ready(city_.terrain_, vector_break(feet).x, vector_break(feet).y).Result;
    n5242911 = standing_feet(feet);
    stand = n5242911.Result;
    if (logic_not(ready)) {
      hold = vector_add(stand, vector_make(0.0, 0.0, kSpawnLift));
      if (math_compare<">">(kern::formula([](Float z, Float z_2) -> Float { return kern::math::abs((z - z_2)); }, vector_break(feet).z, vector_break(hold).z), 0.05)) {
        rot = vector_make(0.0, 0.0, vector_break(spawn_guard_).z);
        engine_game_world_teleport(n5242912, world_, soldier_, vector_add(hold, Vector{0.0, 0.0, 0.73}), list_make<Vector, "1">(rot));
        teleport = n5242912.result;
      }
      engine_game_world_set_actor_velocity(n5242913, world_, soldier_, Vector{0.0, 0.0, 0.0});
      set_actor_velocity = n5242913.result;
      spawn_guard_ready_at_ = -1.0;
      return;
    }
    if (math_compare<"<">(spawn_guard_ready_at_, 0.0)) {
      spawn_guard_ready_at_ = now;
    }
    if (math_compare<"<">(vector_break(feet).z, kern::formula([](Float z) -> Float { return (z - 0.25); }, vector_break(stand).z))) {
      engine_game_world_teleport(n5242914, world_, soldier_, vector_add(stand, Vector{0.0, 0.0, 0.83}), List<Vector>{});
      teleport_2 = n5242914.result;
      engine_game_world_set_actor_velocity(n5242915, world_, soldier_, Vector{0.0, 0.0, 0.0});
      set_actor_velocity_2 = n5242915.result;
    }
    if (math_compare<">">(kern::formula([](Float now, Float spawn_guard_ready_at_) -> Float { return (now - spawn_guard_ready_at_); }, now, spawn_guard_ready_at_), 2.0)) {
      spawn_guard_until_ = -1.0;
    }
  }

  void toggle_drone() {
    Bool set_mode = false;
    Float sim_time = 0.0;
    Float sim_time_2 = 0.0;
    Vector feet = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float yaw = 0.0;
    Float camera_yaw = 0.0;
    Vector at = Vector{0.0, 0.0, 0.0};
    List<Float> list = List<Float>{};
    Int at_2 = 0;
    Int count = 0;
    Bool stop_2 = false;
    Float turn = 0.0;
    Vector c = Vector{0.0, 0.0, 0.0};
    Bool open = false;
    Int physics = 0;
    Int physics_2 = 0;
    Int physics_3 = 0;
    Bool both = false;
    the_RayResult_ g = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics_4 = 0;
    Vector rot = Vector{0.0, 0.0, 0.0};
    Float camera_yaw_2 = 0.0;
    Bool teleport = false;
    Bool set_actor_velocity = false;
    Bool set_mode_2 = false;
    if (gaps_riding().Result) {
      return;
    }
    if (enum_equal<Mode>(mode_, Mode::drone)) {
      engine_game_world_set_alive(world_, drone_actor_, false);
      drone_out_ = false;
      engine_game_world_set_mode(n5242916, world_, game_SimMode::play);
      set_mode = n5242916.result;
      mode_ = Mode::foot;
      return;
    }
    if (logic_not(enum_equal<Mode>(mode_, Mode::foot))) {
      return;
    }
    sim_time = engine_game_world_sim_time(world_);
    if (math_compare<"<">(sim_time, combat_.drone_ready_at)) {
      sim_time_2 = engine_game_world_sim_time(world_);
      toast_ = text_append(text_append("Neue Drohne in ", text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float drone_ready_at, Float sim_time_2) -> Float { return kern::math::trunc(kern::math::ceil((drone_ready_at - sim_time_2))); }, combat_.drone_ready_at, sim_time_2))), ""), " s", "");
      toast_age_ = 0.0;
      return;
    }
    n5242917 = matrix_of(soldier_);
    a = n5242917.Result;
    feet = vector_sub(engine_math_math::Mat4_translation(a).Result, Vector{0.0, 0.0, 0.73});
    camera_yaw = engine_game_world_camera_yaw(world_);
    yaw = kern::formula([](Float camera_yaw, Float kDeg) -> Float { return (camera_yaw * kDeg); }, camera_yaw, kDeg);
    at = vector_add(feet, vector_scale(vector_make(kern::formula([](Float yaw) -> Float { return kern::math::cos(yaw); }, yaw), kern::formula([](Float yaw) -> Float { return kern::math::sin(yaw); }, yaw), 0.0), kDroneAhead));
    list = list_make<Float, "4">(0.0, kern::formula([](Float kPi) -> Float { return (0.5 * kPi); }, kPi), kern::formula([](Float kPi) -> Float { return ((-0.5) * kPi); }, kPi), kPi);
    count = list_length<Float>(list);
    at_2 = 0;
    stop_2 = false;
    while (loop(logic_and(logic_not(stop_2), math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(count))))) {
      turn = list[at_2];
      c = vector_add(feet, vector_scale(vector_make(kern::formula([](Float yaw, Float turn) -> Float { return kern::math::cos((yaw + turn)); }, yaw, turn), kern::formula([](Float yaw, Float turn) -> Float { return kern::math::sin((yaw + turn)); }, yaw, turn), 0.0), kDroneAhead));
      engine_game_world_physics(n5242918, world_);
      physics = n5242918.result;
      both = logic_not(struct_break<the_RayResult_>(common::cast(physics, vector_add(c, Vector{0.0, 0.0, 0.3}), Vector{0.0, 0.0, 1.0}, 25.0, body(soldier_).Result, Int64{0xffffffffffffffff}).Result).hit);
      if (both) {
        engine_game_world_physics(n5242920, world_);
        physics_3 = n5242920.result;
        both = logic_not(struct_break<the_RayResult_>(common::cast(physics_3, vector_add(feet, Vector{0.0, 0.0, 0.3}), logic_select<Vector>(math_compare<">">(vector_length(vector_sub(c, feet)), 9.999999717180685e-10), vector_normalize(vector_sub(c, feet)), Vector{0.0, 0.0, 1.0}), kern::formula([](Float kDroneAhead) -> Float { return (kDroneAhead + 0.3); }, kDroneAhead), body(soldier_).Result, Int64{0xffffffffffffffff}).Result).hit);
      }
      open = both;
      if (open) {
        at = c;
        stop_2 = true;
      }
      if (logic_not(stop_2)) {
        at_2 += 1;
      }
    }
    engine_game_world_physics(n5242921, world_);
    physics_4 = n5242921.result;
    g = common::cast(physics_4, vector_add(at, Vector{0.0, 0.0, 3.0}), Vector{0.0, 0.0, -1.0}, 10.0, body(soldier_).Result, Int64{0xffffffffffffffff}).Result;
    at = vector_make(vector_break(at).x, vector_break(at).y, kern::formula([](Float hit, Float z, Float z_2) -> Float { return (((hit != 0.0) ? z : z_2) + 0.0225); }, kern::to<Float>(g.hit), vector_break(g.point).z, vector_break(feet).z));
    engine_game_world_set_alive(world_, drone_actor_, true);
    camera_yaw_2 = engine_game_world_camera_yaw(world_);
    rot = vector_make(0.0, 0.0, kern::formula([](Float camera_yaw_2) -> Float { return (camera_yaw_2 + 180.0); }, camera_yaw_2));
    engine_game_world_teleport(n5242922, world_, drone_actor_, at, list_make<Vector, "1">(rot));
    teleport = n5242922.result;
    engine_game_world_set_actor_velocity(n5242923, world_, drone_actor_, Vector{0.0, 0.0, 0.0});
    set_actor_velocity = n5242923.result;
    drone_cpp::Drone_reset(drone_);
    sticks_ = struct_make<DroneSticks>(0.0, 0.0, 0.0, 0.0, 0.0, 1.2, false, true, false);
    sticks_.clearance = 1.2;
    landing_ = false;
    drone_out_ = true;
    engine_game_world_set_mode(n5242924, world_, game_SimMode::simulate);
    set_mode_2 = n5242924.result;
    mode_ = Mode::drone;
    rig_init_ = false;
  }

  void eject() {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    n5242928 = state_of(jet_actor_);
    s = n5242928.Result;
    jet_.pilot = false;
    engine_game_world_set_alive(world_, chute_actor_, true);
    engine_game_world_teleport(n5242929, world_, chute_actor_, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 2.8}).Result), List<Vector>{});
    teleport = n5242929.result;
    engine_game_world_set_actor_velocity(n5242930, world_, chute_actor_, vector_add(s.velocity, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kEjectUp)).Result));
    set_actor_velocity = n5242930.result;
    chute_time_ = 0.0;
    chute_open_ = false;
    mode_ = Mode::parachute;
    rig_init_ = false;
    toast_ = "Schleudersitz!";
    toast_age_ = 0.0;
  }

  void exit_vehicle() {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector left = Vector{0.0, 0.0, 0.0};
    Float side = 0.0;
    Vector out = Vector{0.0, 0.0, 0.0};
    Int physics = 0;
    BodyState s_2 = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector right = Vector{0.0, 0.0, 0.0};
    Vector out_2 = Vector{0.0, 0.0, 0.0};
    if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0))) {
      n5242925 = state_of(cars_[car_index_].actor);
      s = n5242925.Result;
      left = engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 1.0, 0.0}).Result;
      side = kern::formula([](Float y) -> Float { return (y + 0.9); }, vector_break(cars_[car_index_].car[0].s_.half).y);
      out = vector_add(s.position, vector_scale(left, side));
      engine_game_world_physics(n5242926, world_);
      physics = n5242926.result;
      if (struct_break<the_RayResult_>(common::cast(physics, s.position, left, kern::formula([](Float side) -> Float { return (side + 0.5); }, side), body(cars_[car_index_].actor).Result, Int64{0xffffffffffffffff}).Result).hit) {
        out = vector_sub(s.position, vector_scale(left, side));
      }
      out = vector_make(vector_break(out).x, vector_break(out).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(out).x, vector_break(out).y).Result);
      show_soldier(out, kern::formula([](Float the_result, Float kDeg) -> Float { return kern::math::div(the_result, kDeg); }, common::heading_of(engine_math_math::Quat_rotate(s.rotation, Vector{1.0, 0.0, 0.0}).Result).Result, kDeg));
      cars_[car_index_].input = struct_make<CarInput>(0.0, 0.0, false, 32.0, 9.0);
      cars_[car_index_].input.handbrake = true;
      car_index_ = -1;
      mode_ = Mode::foot;
    } else if (enum_equal<Mode>(mode_, Mode::jet)) {
      n5242927 = state_of(jet_actor_);
      s_2 = n5242927.Result;
      if (logic_and(math_compare<">=">(kern::to<Float>(jet_.tele_.contacts), 2.0), math_compare<"<">(vector_length(s_2.velocity), 8.0))) {
        right = engine_math_math::Quat_rotate(s_2.rotation, Vector{1.0, 0.0, 0.0}).Result;
        out_2 = vector_sub(s_2.position, vector_scale(right, 7.5));
        out_2 = vector_make(vector_break(out_2).x, vector_break(out_2).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(out_2).x, vector_break(out_2).y).Result);
        jet_.pilot = false;
        jet_.engine_on = false;
        show_soldier(out_2, kern::formula([](Float the_result, Float kDeg) -> Float { return kern::math::div(the_result, kDeg); }, common::heading_of(engine_math_math::Quat_rotate(s_2.rotation, Vector{0.0, 1.0, 0.0}).Result).Result, kDeg));
        mode_ = Mode::foot;
      } else {
        eject();
      }
    } else if (enum_equal<Mode>(mode_, Mode::drone)) {
      toggle_drone();
    }
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
    n5242935 = common::Rng_next(rng_);
    start_2 = n5242935.Result;
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
    engine_game_world_physics(n5242936, world_);
    physics = n5242936.result;
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

  struct manage_traffic_nearestResult { Float Result = 0.0; };
  KERN_PURE manage_traffic_nearestResult manage_traffic_nearest(const List<Vector>& mates, Vector me = Vector{0.0, 0.0, 0.0}, Vector p = Vector{0.0, 0.0, 0.0}) {
    Float d = 0.0;
    d = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(p).x, vector_break(me).x, vector_break(p).y, vector_break(me).y);
    for (auto it5242938 = flow_for(n5242938, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(mates))))); it5242938.next();) {
      d = kern::formula([](Float d, Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::min(d, kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2)))); }, d, vector_break(p).x, vector_break(mates[n5242938.index]).x, vector_break(p).y, vector_break(mates[n5242938.index]).y);
    }
    return {d};
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
    engine_game_world_physics(n5242940, world_);
    physics = n5242940.result;
    cast_sphere = engine_world_cast_sphere(physics, anchor, 0.17, d, engine_physics_physics::QueryFilter_new(Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}).Result, ignore);
    h = cast_sphere;
    return {logic_select<Vector>(h.hit, vector_add(anchor, vector_scale(d, kern::formula([](Float fraction) -> Float { return kern::math::max(0.0, (fraction - 0.02)); }, h.fraction))), want)};
  }

  struct audio_renderResult { List<Float> Result = List<Float>{}; };
  audio_renderResult audio_render(Int frames = 0) {
    Bool ready = false;
    Int at = 0;
    Int last = 0;
    Float fill = 0.0;
    Int first = 0;
    Int last_2 = 0;
    Float fill_2 = 0.0;
    Int first_2 = 0;
    Int last_3 = 0;
    list_resize(audio_out_, kern::to<Int>(kern::formula([](Float frames) -> Float { return (frames * 2.0); }, kern::to<Float>(frames))), 0.0);
    ready = engine_mixer_ready(mixer_);
    if (ready) {
      at = 0;
      engine_mixer_render(n5242947, mixer_, frames);
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(n5242947.out))));
      for (auto it5242948 = flow_for(n5242948, 0, last); it5242948.next();) {
        audio_out_[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(at), kern::to<Float>(n5242948.index)))] = list_get<Float>(n5242947.out, n5242948.index);
      }
      if (web_audio_) {
        fill = 0.0;
        first = 0;
        last_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(audio_out_))));
        for (auto it5242949 = flow_for(n5242949, first, last_2); it5242949.next();) {
          audio_out_[n5242949.index] = fill;
        }
      } else {
        ui_render_music(audio_out_, frames);
      }
    } else {
      fill_2 = 0.0;
      first_2 = 0;
      last_3 = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(audio_out_))));
      for (auto it5242950 = flow_for(n5242950, first_2, last_3); it5242950.next();) {
        audio_out_[n5242950.index] = fill_2;
      }
    }
    mix_voice(audio_out_, frames);
    return {audio_out_};
  }

  void dsync_ready() {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<DestroySync>(dsync_)), 0.0))) {
      dsync_ = list_make<DestroySync, "1">(DestroySync{1, 0.0, 0.0, List<Terrain_CraterRec>{}, List<DestroySync_Mine>{}, List<CityStream_DamageRec>{}, List<Int_Float_pair>{}, List<Int>{}, 0, -99.0, false, false, 0, 0, 0});
    }
  }

  struct dsync_update_matchResult { Bool Result = false; };
  KERN_PURE dsync_update_matchResult dsync_update_match(const CityStream_DamageRec& q, Int seed = 0) {
    return {math_compare<"==">(kern::to<Float>(q.seed), kern::to<Float>(seed))};
  }

  struct dsync_update_match_2Result { Bool Result = false; };
  KERN_PURE dsync_update_match_2Result dsync_update_match_2(const Int_Float_pair& list_2_n6291472_index, const Int_Float_pair& q) {
    return {math_compare<"==">(kern::to<Float>(q.first), kern::to<Float>(list_2_n6291472_index.first))};
  }

  struct dsync_jsonResult { Text Result = ""; };
  dsync_jsonResult dsync_json() {
    List<Terrain_CraterRec> c = List<Terrain_CraterRec>{};
    List<CityStream_DamageRec> dm = List<CityStream_DamageRec>{};
    Text b = "";
    dsync_ready();
    c = List<Terrain_CraterRec>{};
    terrain_cpp::the_Terrain_crater_list_(city_.terrain_, c);
    dm = List<CityStream_DamageRec>{};
    city_stream_cpp::the_CityStream_damage_list_(city_, dm);
    b = "";
    n6291477 = game_destruction_sync_cpp::digest_of(c, dm);
    b = text_format<"{\"epoch\":%u,\"craters\":%zu,\"damaged\":%zu,\"digest\":%u,\"sent\":%u,\"got\":%u,\"asks\":%u,\"pending\":%zu,\"mismatches\":%u}">(255, dsync_[0].epoch, list_length<Terrain_CraterRec>(c), list_length<CityStream_DamageRec>(dm), n6291477.Result, dsync_[0].sent_states, dsync_[0].got_states, dsync_[0].asks, list_length<DestroySync_Mine>(dsync_[0].mine), dsync_[0].mismatches);
    return {b};
  }

  struct details_command_startsResult { Bool Result = false; };
  KERN_PURE details_command_startsResult details_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct sfx_idResult { Text Result = ""; };
  /// "sc:<kind><variant>" for every sound of the bank (built once).
  sfx_idResult sfx_id(combat_audio_Kind k = combat_audio_Kind::shot, Int v = 0) {
    if (math_compare<"==">(kern::to<Float>(list_length<Text>(sfx_id_ids)), 0.0)) {
      for (auto it7340044 = flow_for(n7340044, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<combat_audio_Kind>(combat_audio_Kind::count))))); it7340044.next();) {
        for (auto it7340045 = flow_for(n7340045, 0, 3); it7340045.next();) {
          list_add(sfx_id_ids, text_append(text_append("sc:", combat_audio_cpp::kind_name(enum_from_index<combat_audio_Kind>(n7340044.index).value).Result, ""), text_format<"%u">(-1, n7340045.index), ""));
        }
      }
    }
    return {sfx_id_ids[kern::to<Int>(kern::formula([](Float a, Float v) -> Float { return ((a * 4.0) + v); }, kern::to<Float>(enum_index<combat_audio_Kind>(k)), kern::to<Float>(v)))]};
  }

  void combat_audio_init() {
    combat_audio_Kind k = combat_audio_Kind::shot;
    List<Float> pcm = List<Float>{};
    Text id = "";
    Bool set_pcm = false;
    if (logic_or(combat_audio_ready_, logic_not(math_compare<"!=">(kern::to<Float>(bank_), 0.0)))) {
      return;
    }
    for (auto it7340046 = flow_for(n7340046, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<combat_audio_Kind>(combat_audio_Kind::count))))); it7340046.next();) {
      k = enum_from_index<combat_audio_Kind>(n7340046.index).value;
      for (auto it7340047 = flow_for(n7340047, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_get<Int>(kVariants, n7340046.index))))); it7340047.next();) {
        n7340048 = combat_audio_cpp::render(k, n7340047.index, kSampleRate);
        pcm = n7340048.Result;
        n7340049 = sfx_id(k, n7340047.index);
        id = n7340049.Result;
        engine_sound_bank_request(bank_, id);
        engine_sound_bank_set_pcm(n7340050, bank_, id, pcm, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(pcm)))), 1, kSampleRate);
        set_pcm = n7340050.result;
      }
    }
    combat_audio_ready_ = true;
  }

  struct combat_sfxResult { Text Result = ""; };
  combat_sfxResult combat_sfx(combat_audio_Kind k = combat_audio_Kind::shot) {
    Int i = 0;
    Int v = 0;
    Int was = 0;
    i = enum_index<combat_audio_Kind>(k);
    was = sfx_cursor_[i];
    sfx_cursor_[i] = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(sfx_cursor_[i])));
    v = kern::to<Int>(kern::formula([](Float was, Float a) -> Float { return kern::math::mod(was, a); }, kern::to<Float>(was), kern::to<Float>(list_get<Int>(kVariants, i))));
    n7340051 = sfx_id(k, v);
    return {n7340051.Result};
  }

  void ui_audio_init() {
    Int m = 0;
    Int rate = 0;
    Int sample_rate = 0;
    audio_SynthPatch roar = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch rumble = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip = 0;
    audio_SynthPatch thump = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch burst = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch click = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    List<Float> w = List<Float>{};
    List<audio_SynthPatch> list = List<audio_SynthPatch>{};
    List<Float> synthesize = List<Float>{};
    Int adopt_clip_2 = 0;
    audio_SynthPatch body_2 = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch low = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    List<Float> w_2 = List<Float>{};
    List<audio_SynthPatch> list_2 = List<audio_SynthPatch>{};
    List<Float> synthesize_2 = List<Float>{};
    Int adopt_clip_3 = 0;
    audio_SynthPatch hum = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip_4 = 0;
    List<Float> birds = List<Float>{};
    List<Float> made = List<Float>{};
    Rng r = Rng{2654435769, 0.0, false};
    audio_SynthPatch chirp = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    List<Float> synthesize_3 = List<Float>{};
    Int adopt_clip_5 = 0;
    audio_SynthPatch cricket = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch pulse = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Int adopt_clip_6 = 0;
    m = mixer_;
    sample_rate = engine_mixer_sample_rate(m);
    rate = sample_rate;
    equipment_cpp::MusicSynth_init(ui_.music, rate);
    roar = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    roar.wave = audio_Wave::noise;
    roar.lowpass = 1800.0;
    roar.highpass = 140.0;
    roar.volume = 0.7;
    roar.tremolo_depth = 0.35;
    roar.tremolo_rate = 23.0;
    roar.seed = 77;
    rumble = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    rumble.wave = audio_Wave::pink;
    rumble.lowpass = 260.0;
    rumble.volume = 0.6;
    rumble.seed = 78;
    n8388618 = game_equipment_cpp::render_loop(list_make<audio_SynthPatch, "2">(roar, rumble), rate, 1.5, 0.8);
    engine_mixer_adopt_clip(n8388619, m, n8388618.Result, 1, "spawncity:jetpack", true);
    adopt_clip = n8388619.result;
    ui_.jet_clip = adopt_clip;
    thump = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    thump.wave = audio_Wave::sine;
    thump.frequency = 120.0;
    thump.slide = -3.0;
    thump.sustain = 0.05;
    thump.release = 0.25;
    thump.volume = 0.9;
    burst = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    burst.wave = audio_Wave::noise;
    burst.lowpass = 2400.0;
    burst.lowpass_sweep = -2.0;
    burst.sustain = 0.08;
    burst.release = 0.3;
    burst.volume = 0.5;
    click = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    click.wave = audio_Wave::square;
    click.frequency = 2400.0;
    click.sustain = 0.004;
    click.release = 0.01;
    click.volume = 0.2;
    w = List<Float>{};
    list = list_make<audio_SynthPatch, "3">(thump, burst, click);
    for (auto it8388620 = flow_for(n8388620, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<audio_SynthPatch>(list))))); it8388620.next();) {
      synthesize = engine_audio_synthesize(list[n8388620.index], rate);
      engine_audio_mix_into(n8388621, w, synthesize, 0.0, rate, 1.0);
      w = n8388621.dst;
    }
    engine_audio_normalize(n8388622, w, 0.9);
    w = n8388622.samples;
    engine_mixer_adopt_clip(n8388623, m, w, 1, "spawncity:jetpack-ignition", false);
    adopt_clip_2 = n8388623.result;
    ui_.whoomp_clip = adopt_clip_2;
    body_2 = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    body_2.wave = audio_Wave::noise;
    body_2.lowpass = 560.0;
    body_2.highpass = 55.0;
    body_2.sustain = 0.03;
    body_2.release = 0.16;
    body_2.volume = 0.8;
    low = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    low.wave = audio_Wave::sine;
    low.frequency = 70.0;
    low.slide = -1.5;
    low.sustain = 0.03;
    low.release = 0.18;
    w_2 = List<Float>{};
    list_2 = list_make<audio_SynthPatch, "2">(body_2, low);
    for (auto it8388624 = flow_for(n8388624, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<audio_SynthPatch>(list_2))))); it8388624.next();) {
      synthesize_2 = engine_audio_synthesize(list_2[n8388624.index], rate);
      engine_audio_mix_into(n8388625, w_2, synthesize_2, 0.0, rate, 1.0);
      w_2 = n8388625.dst;
    }
    engine_audio_normalize(n8388626, w_2, 0.8);
    w_2 = n8388626.samples;
    engine_mixer_adopt_clip(n8388627, m, w_2, 1, "spawncity:impact", false);
    adopt_clip_3 = n8388627.result;
    ui_.thump_clip = adopt_clip_3;
    hum = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    hum.wave = audio_Wave::pink;
    hum.lowpass = 420.0;
    hum.highpass = 40.0;
    hum.volume = 0.7;
    hum.tremolo_depth = 0.15;
    hum.tremolo_rate = 0.13;
    n8388628 = game_equipment_cpp::render_loop(list_make<audio_SynthPatch, "1">(hum), rate, 6.0, 0.6);
    engine_mixer_adopt_clip(n8388629, m, n8388628.Result, 1, "spawncity:city", true);
    adopt_clip_4 = n8388629.result;
    ui_.city_clip = adopt_clip_4;
    made = List<Float>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float rate) -> Float { return (rate * 6.0); }, kern::to<Float>(rate))), 0.0);
    birds = made;
    r = struct_make<Rng>(4711, 0.0, false);
    for (auto it8388630 = flow_for(n8388630, 0, 13); it8388630.next();) {
      chirp = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
      chirp.wave = audio_Wave::sine;
      n8388631 = common::Rng_unit(r);
      chirp.frequency = kern::formula([](Float the_result) -> Float { return (2600.0 + (1800.0 * the_result)); }, n8388631.Result);
      n8388632 = common::Rng_unit(r);
      chirp.slide = kern::formula([](Float the_result) -> Float { return ((the_result < 0.5) ? 3.0 : (-2.0)); }, n8388632.Result);
      chirp.vibrato_depth = 2.0;
      chirp.vibrato_rate = 28.0;
      chirp.attack = 0.005;
      n8388633 = common::Rng_unit(r);
      chirp.sustain = kern::formula([](Float the_result) -> Float { return (0.03 + (0.05 * the_result)); }, n8388633.Result);
      chirp.release = 0.04;
      n8388634 = common::Rng_unit(r);
      chirp.repeat = kern::formula([](Float the_result) -> Float { return ((the_result < 0.5) ? 0.09 : 0.0); }, n8388634.Result);
      n8388635 = common::Rng_unit(r);
      chirp.volume = kern::formula([](Float the_result) -> Float { return (0.25 + (0.25 * the_result)); }, n8388635.Result);
      synthesize_3 = engine_audio_synthesize(chirp, rate);
      n8388636 = common::Rng_unit(r);
      engine_audio_mix_into(n8388637, birds, synthesize_3, kern::formula([](Float the_result) -> Float { return (5.6 * the_result); }, n8388636.Result), rate, 1.0);
      birds = n8388637.dst;
    }
    list_resize(birds, kern::to<Int>(kern::formula([](Float rate) -> Float { return (rate * 6.0); }, kern::to<Float>(rate))), 0.0);
    engine_audio_make_seamless(n8388638, birds, rate, 0.2);
    birds = n8388638.samples;
    engine_audio_normalize(n8388639, birds, 0.5);
    birds = n8388639.samples;
    engine_mixer_adopt_clip(n8388640, m, birds, 1, "spawncity:birds", true);
    adopt_clip_5 = n8388640.result;
    ui_.birds_clip = adopt_clip_5;
    cricket = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    cricket.wave = audio_Wave::sine;
    cricket.frequency = 4300.0;
    cricket.tremolo_depth = 1.0;
    cricket.tremolo_rate = 32.0;
    cricket.volume = 0.3;
    pulse = cricket;
    pulse.frequency = 4520.0;
    pulse.tremolo_rate = 2.5;
    pulse.volume = 0.2;
    n8388641 = game_equipment_cpp::render_loop(list_make<audio_SynthPatch, "2">(cricket, pulse), rate, 4.0, 0.35);
    engine_mixer_adopt_clip(n8388642, m, n8388641.Result, 1, "spawncity:crickets", true);
    adopt_clip_6 = n8388642.result;
    ui_.crickets_clip = adopt_clip_6;
  }

  void ui_radio(Int dir = 0) {
    Int n = 0;
    Text b = "";
    n = equipment_cpp::MusicSynth_station_count().Result;
    if (logic_not(ui_.radio_on)) {
      ui_.radio_on = true;
      ui_.station = kern::to<Int>(kern::formula([](Float dir, Float n) -> Float { return ((dir > 0.0) ? 0.0 : (n - 1.0)); }, kern::to<Float>(dir), kern::to<Float>(n)));
    } else if (logic_or(logic_and(math_compare<">">(kern::to<Float>(dir), 0.0), math_compare<"==">(kern::to<Float>(ui_.station), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n))))))), logic_and(math_compare<"<">(kern::to<Float>(dir), 0.0), math_compare<"==">(kern::to<Float>(ui_.station), 0.0)))) {
      ui_.radio_on = false;
    } else {
      ui_.station = kern::to<Int>(kern::formula([](Float station, Float dir, Float n) -> Float { return kern::math::mod(((station + dir) + n), n); }, kern::to<Float>(ui_.station), kern::to<Float>(dir), kern::to<Float>(n)));
    }
    if (ui_.radio_on) {
      ui_.banner = text_append("\360\237\223\273 ", struct_break<MusicSynth_Station>(list_get<MusicSynth_Station>(kStations, ui_.station)).name, "");
      b = "";
      b = text_format<"%d/%d \302\267 %s">(95, kern::to<Int>(kern::formula([](Float station) -> Float { return (station + 1.0); }, kern::to<Float>(ui_.station))), n, struct_break<MusicSynth_Station>(list_get<MusicSynth_Station>(kStations, ui_.station)).genre);
      ui_.banner_sub = b;
    } else {
      ui_.banner = "\360\237\223\273 RADIO AUS";
      ui_.banner_sub = "";
    }
    ui_.banner_age = 0.0;
  }

  void ui_equipment(Float dt = 0.0) {
    FlightSample s = FlightSample{0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1.0, 0.0, false};
    Float sim_time = 0.0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    Float sim_time_2 = 0.0;
    Bool in_car = false;
    Bool next = false;
    Int input = 0;
    Bool pressed = false;
    Text a_2 = "";
    Text a_3 = "";
    Bool either = false;
    Bool prev = false;
    Int input_2 = 0;
    Bool pressed_2 = false;
    Vector v = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity_2 = Vector{0.0, 0.0, 0.0};
    Float dv = 0.0;
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool ready = false;
    Bool ready_2 = false;
    Bool both = false;
    audio_PlayParams p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
    Int64 play = Int64{0x0};
    if (drone_out_) {
      s = FlightSample{0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1.0, 0.0, false};
      sim_time = engine_game_world_sim_time(world_);
      s.t = sim_time;
      n8388645 = matrix_of(drone_actor_);
      a = n8388645.Result;
      s.pos = engine_math_math::Mat4_translation(a).Result;
      the_actor_velocity = engine_game_world_actor_velocity(world_, drone_actor_);
      s.vel = the_actor_velocity;
      s.agl = drone_.tele_.agl;
      s.volts = drone_.tele_.battery_v;
      s.armed = drone_.tele_.armed;
      equipment_cpp::FlightLog_observe(ui_.the_log, s, logic_select<Text>(drone_.tele_.fault, "FC_FAULT", "FC_DISARM"));
    } else if (equipment::FlightLog_flying(ui_.the_log).Result) {
      equipment_cpp::FlightLog_finish(ui_.the_log, logic_select<Text>(math_compare<"<=">(drone_hp_, 0.0), "DRONE_DESTROYED", "DRONE_STOWED"));
    }
    if (logic_and(net_session::Session_active(net_).Result, logic_not(text_equal(net_session_cpp::Session_status(net_).Result, ui_.net_status)))) {
      ui_.net_status = net_session_cpp::Session_status(net_).Result;
      sim_time_2 = engine_game_world_sim_time(world_);
      equipment_cpp::FlightLog_network(ui_.the_log, ui_.net_status, sim_time_2);
    }
    in_car = logic_and(enum_equal<Mode>(mode_, Mode::car), logic_not(ui_.paused));
    if (logic_and(logic_and(in_car, math_compare<"!=">(kern::to<Float>(engine_), 0.0)), logic_not(web_audio_))) {
      engine_engine_input(n8388646, engine_);
      input = n8388646.result;
      pressed = engine_input_pressed(input, Key::n);
      either = logic_or(pressed, game::PadState_hit(pad, enum_index<PadButtonIndex>(PadButtonIndex::kPadRight)).Result);
      if (logic_not(either)) {
        a_3 = "radio";
        n8388647 = touch_play_cpp::TouchPlay_take_pressed(pad_touch_, a_3);
        either = n8388647.Result;
      }
      next = either;
      engine_engine_input(n8388648, engine_);
      input_2 = n8388648.result;
      pressed_2 = engine_input_pressed(input_2, Key::b);
      prev = logic_or(pressed_2, game::PadState_hit(pad, enum_index<PadButtonIndex>(PadButtonIndex::kPadLeft)).Result);
      if (logic_or(next, prev)) {
        ui_radio(kern::to<Int>(kern::formula([](Float next) -> Float { return ((next != 0.0) ? 1.0 : (-1.0)); }, kern::to<Float>(next))));
      }
    }
    ui_.banner_age = kern::formula([](Float banner_age, Float dt) -> Float { return (banner_age + dt); }, ui_.banner_age, dt);
    if (logic_and(logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0)), logic_not(ui_.paused))) {
      the_actor_velocity_2 = engine_game_world_actor_velocity(world_, cars_[car_index_].actor);
      v = the_actor_velocity_2;
      if (ui_.car_vel_valid) {
        dv = kern::formula([](Float a, Float dt) -> Float { return (a - ((9.81 * dt) * 1.5)); }, vector_length(vector_sub(v, ui_.car_vel)), dt);
        if (math_compare<">">(dv, 1.8)) {
          n8388649 = matrix_of(cars_[car_index_].actor);
          a_4 = n8388649.Result;
          osfx("world-physics-impact", engine_math_math::Mat4_translation(a_4).Result, dv, 0.0, 0.0);
        }
        both = math_compare<">">(dv, 1.8);
        if (both) {
          ready_2 = engine_mixer_ready(mixer_);
          both = ready_2;
        }
        if (logic_and(both, math_compare<"!=">(kern::to<Float>(ui_.thump_clip), 0.0))) {
          p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
          p.volume = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float dv) -> Float { return (0.25 + (dv * 0.06)); }, dv), 0.3, 1.0));
          p.pitch = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float dv) -> Float { return (1.2 - (dv * 0.03)); }, dv), 0.6, 1.2));
          p.bus = audio_Bus::sfx;
          engine_mixer_play(n8388650, mixer_, ui_.thump_clip, p);
          play = n8388650.result;
          ui_.thumps = kern::to<Int>(kern::formula([](Float thumps) -> Float { return kern::math::wrap32(thumps + 1.0); }, kern::to<Float>(ui_.thumps)));
          engine_game_world_shake_camera(world_, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float dv) -> Float { return kern::math::div((dv - 2.5), 14.0); }, dv), 0.0, 0.7)));
        }
      }
      ui_.car_vel = v;
      ui_.car_vel_valid = true;
    } else {
      ui_.car_vel_valid = false;
    }
  }

  void ui_mix_equipment(Float dt = 0.0) {
    Int m = 0;
    Bool burn = false;
    Int64 a = Int64{0x0};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool on = false;
    Float high = 0.0;
    Bool live = false;
    Int64 a_3 = Int64{0x0};
    Int64 a_4 = Int64{0x0};
    Int64 a_5 = Int64{0x0};
    Float vol = 0.0;
    m = mixer_;
    burn = logic_and(ui_.jetpack.thrusting, logic_not(ui_.paused));
    a = int64_of_int(ui_.jet_voice);
    n8388651 = matrix_of(soldier_);
    a_2 = n8388651.Result;
    game_equipment_cpp::drive_loop(a, m, ui_.jet_clip, kern::formula([](Float burn) -> Float { return ((burn != 0.0) ? 0.75 : 0.0); }, kern::to<Float>(burn)), audio_Bus::sfx, true, vector_sub(engine_math_math::Mat4_translation(a_2).Result, Vector{0.0, 0.0, 0.6}));
    ui_.jet_voice = int64_to_int(a);
    for (auto it8388652 = flow_for(n8388652, 0, 7); it8388652.next();) {
      on = logic_and(logic_and(math_compare<">">(kern::formula([](Float index, Float a) -> Float { return ((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a)); }, kern::to<Float>(n8388652.index), kern::to<Float>(list_length<net_Remote>(net_.remotes_))), 0.5), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n8388652.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetpack)))), 0.5)), logic_not(ui_.paused));
      game_equipment_cpp::drive_loop(ui_.remote_jet_voice[n8388652.index], m, ui_.jet_clip, kern::formula([](Float on) -> Float { return ((on != 0.0) ? 0.7 : 0.0); }, kern::to<Float>(on)), audio_Bus::sfx, true, logic_select<Vector>(on, vector_sub(net_.remotes_[n8388652.index].shown, Vector{0.0, 0.0, 0.6}), Vector{0.0, 0.0, 0.0}));
    }
    high = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float z, Float the_result) -> Float { return (1.0 - kern::math::div((z - the_result), 150.0)); }, vector_break(cam_.eye).z, city_stream_cpp::CityStream_ground_height(city_, vector_break(cam_.eye).x, vector_break(cam_.eye).y).Result), 0.15, 1.0));
    live = logic_and(world_placed_, logic_not(ui_.paused));
    a_3 = int64_of_int(ui_.city_voice);
    game_equipment_cpp::drive_loop(a_3, m, ui_.city_clip, kern::formula([](Float live, Float high, Float a) -> Float { return ((live != 0.0) ? ((0.32 * high) * ((a != 0.0) ? 0.7 : 1.0)) : 0.0); }, kern::to<Float>(live), high, kern::to<Float>(city_stream::CityStream_offline(city_).Result)), audio_Bus::ambience, false, Vector{0.0, 0.0, 0.0});
    ui_.city_voice = int64_to_int(a_3);
    a_4 = int64_of_int(ui_.birds_voice);
    game_equipment_cpp::drive_loop(a_4, m, ui_.birds_clip, kern::formula([](Float live, Float high, Float night_) -> Float { return ((live != 0.0) ? ((0.28 * high) * (1.0 - night_)) : 0.0); }, kern::to<Float>(live), high, night_), audio_Bus::ambience, false, Vector{0.0, 0.0, 0.0});
    ui_.birds_voice = int64_to_int(a_4);
    a_5 = int64_of_int(ui_.crickets_voice);
    game_equipment_cpp::drive_loop(a_5, m, ui_.crickets_clip, kern::formula([](Float live, Float high, Float night_) -> Float { return ((live != 0.0) ? ((0.22 * high) * night_) : 0.0); }, kern::to<Float>(live), high, night_), audio_Bus::ambience, false, Vector{0.0, 0.0, 0.0});
    ui_.crickets_voice = int64_to_int(a_5);
    vol = kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 100.0); }, settings::Settings_get(ui_.settings, Opt::music_volume).Result);
    if (logic_or(logic_not(settings::Settings_on(ui_.settings, Opt::music_on).Result), logic_not(settings::Settings_on(ui_.settings, Opt::audio_on).Result))) {
      equipment_cpp::MusicSynth_select(ui_.music, -2, 0.0);
    } else if (logic_and(logic_and(enum_equal<Mode>(mode_, Mode::car), logic_not(ui_.paused)), ui_.radio_on)) {
      equipment_cpp::MusicSynth_select(ui_.music, ui_.station, kern::formula([](Float vol) -> Float { return (kern::math::max(0.55, vol) * 0.85); }, vol));
    } else if (logic_and(ui_.paused, math_compare<">">(vol, 0.0))) {
      equipment_cpp::MusicSynth_select(ui_.music, kMenu, kern::formula([](Float vol) -> Float { return (vol * 0.45); }, vol));
    } else {
      equipment_cpp::MusicSynth_select(ui_.music, -2, 0.0);
    }
  }

  void ui_render_music(List<Float>& out, Int frames = 0) {
    equipment_cpp::MusicSynth_render_add(ui_.music, out, frames);
  }

  struct ui_equipment_commandResult { Bool Result = false; };
  ui_equipment_commandResult ui_equipment_command(const Text& cmd, Text& out) {
    Text rest = "";
    Fixed<Text, 4> part = Fixed<Text, 4>{};
    Int sp = 0;
    Fixed<Int, 32> sk = Fixed<Int, 32>{};
    Fixed<Int, 32> aux = Fixed<Int, 32>{};
    Bool either = false;
    Text a = "";
    Text b = "";
    Text b_2 = "";
    Bool push_actor = false;
    if (text_equal(cmd, "logbook")) {
      out = equipment_cpp::FlightLog_json(ui_.the_log).Result;
      return {true};
    }
    if (math_compare<"==">(kern::to<Float>(text_rfind(cmd, "nostr event ", 0)), 0.0)) {
      rest = text_sub(cmd, 12, -1);
      part = List<Text>{};
      for (auto it8388653 = flow_for(n8388653, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Text>(part))))); it8388653.next();) {
        sp = text_find(rest, " ", 0);
        if (math_compare<"==">(kern::to<Float>(sp), -1.0)) {
          out = "usage: nostr event <created_at> <secret> <aux> <tags> <content>";
          return {true};
        }
        part[n8388653.index] = text_sub(rest, 0, sp);
        rest = text_sub(rest, kern::to<Int>(kern::formula([](Float sp) -> Float { return (sp + 1.0); }, kern::to<Float>(sp))), -1);
      }
      sk = List<Int>{};
      aux = List<Int>{};
      n8388654 = nostr_cpp::from_hex(sk, part[1]);
      either = logic_not(n8388654.Result);
      if (logic_not(either)) {
        n8388656 = nostr_cpp::from_hex(aux, part[2]);
        either = logic_not(n8388656.Result);
      }
      if (logic_or(logic_or(either, math_compare<"==">(kern::to<Float>(text_length(part[3])), 0.0)), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(part[3], 0)))))), 91.0))) {
        out = "bad key or tags";
        return {true};
      }
      n8388657 = nostr_cpp::event_json(sk, aux, out, text_parse_int(part[0], 0, 10).value, 30078, part[3], rest);
      if (logic_not(n8388657.Result)) {
        out = "sign failed";
      }
      return {true};
    }
    if (math_compare<"==">(kern::to<Float>(text_rfind(cmd, "radio", 0)), 0.0)) {
      a = logic_select<Text>(math_compare<">">(kern::to<Float>(text_length(cmd)), 6.0), text_sub(cmd, 6, -1), "next");
      if (text_equal(a, "next")) {
        ui_radio(1);
      } else if (text_equal(a, "prev")) {
        ui_radio(-1);
      } else if (text_equal(a, "off")) {
        ui_.radio_on = false;
        ui_.banner = "\360\237\223\273 RADIO AUS";
        ui_.banner_sub = "";
        ui_.banner_age = 0.0;
      } else if (text_equal(a, "toggle")) {
        ui_.radio_on = logic_not(ui_.radio_on);
        if (ui_.radio_on) {
          ui_radio(0);
        } else {
          ui_.banner = "\360\237\223\273 RADIO AUS";
          ui_.banner_sub = "";
          ui_.banner_age = 0.0;
        }
      } else if (text_equal(a, "on")) {
        ui_.radio_on = true;
        ui_radio(0);
      }
      out = logic_select<Text>(ui_.radio_on, struct_break<MusicSynth_Station>(list_get<MusicSynth_Station>(kStations, ui_.station)).name, "off");
      return {true};
    }
    if (text_equal(cmd, "music")) {
      b = "";
      b = text_format<"{\"programme\":%d,\"level\":%.3f,\"peak\":%.3f}">(95, equipment::MusicSynth_programme(ui_.music).Result, equipment::MusicSynth_level(ui_.music).Result, ui_.music.peak);
      out = b;
      return {true};
    }
    if (text_equal(cmd, "jetpack")) {
      b_2 = "";
      b_2 = text_format<"{\"fuel\":%.2f,\"thrusting\":%s,\"ignitions\":%u}">(127, ui_.jetpack.fuel, logic_select<Text>(ui_.jetpack.thrusting, "true", "false"), ui_.jetpack.ignitions);
      out = b_2;
      return {true};
    }
    if (text_equal(cmd, "jetpack burn")) {
      if (logic_not(enum_equal<Mode>(mode_, Mode::foot))) {
        out = "not on foot";
        return {true};
      }
      engine_game_world_push_actor(n8388658, world_, soldier_, Vector{0.0, 0.0, 270.0}, true);
      push_actor = n8388658.result;
      ui_.jet_pending = true;
      out = "ok";
      return {true};
    }
    return {false};
  }

  struct ui_equipment_jsonResult { Text Result = ""; };
  ui_equipment_jsonResult ui_equipment_json() {
    Text s = "";
    Text b = "";
    s = "";
    b = "";
    if (logic_and(enum_equal<Mode>(mode_, Mode::foot), settings::Settings_on(ui_.settings, Opt::world_jetpack).Result)) {
      b = text_format<",\"jetpack\":{\"fuel\":%.2f,\"thrusting\":%s,\"ignitions\":%u}">(255, ui_.jetpack.fuel, logic_select<Text>(ui_.jetpack.thrusting, "true", "false"), ui_.jetpack.ignitions);
      s = text_append(s, b, "");
    }
    b = text_format<",\"logbook\":{\"serial\":%u,\"flights\":%zu,\"flying\":%s}">(255, equipment::FlightLog_serial(ui_.the_log).Result, list_length<Flight>(ui_.the_log.flights_), logic_select<Text>(equipment::FlightLog_flying(ui_.the_log).Result, "true", "false"));
    s = text_append(s, b, "");
    if (enum_equal<Mode>(mode_, Mode::car)) {
      b = text_format<",\"radio\":{\"on\":%s,\"station\":\"%s\",\"index\":%d}">(255, logic_select<Text>(ui_.radio_on, "true", "false"), logic_select<Text>(ui_.radio_on, struct_break<MusicSynth_Station>(list_get<MusicSynth_Station>(kStations, ui_.station)).name, ""), ui_.station);
      s = text_append(s, b, "");
    }
    if (math_compare<"<">(ui_.banner_age, 2.6)) {
      s = text_append(s, text_append(text_append(text_append(text_append(",\"banner\":\"", ui_.banner, ""), "\",\"bannerSub\":\"", ""), ui_.banner_sub, ""), "\"", ""), "");
    }
    return {s};
  }

  struct update_debris_matchResult { Bool Result = false; };
  KERN_PURE update_debris_matchResult update_debris_match(const Debris& d, Float now = 0.0) {
    return {math_compare<">">(kern::formula([](Float now, Float born) -> Float { return (now - born); }, now, d.born), d.life)};
  }

  void update_debris(Float dt = 0.0) {
    Float now = 0.0;
    Float sim_time = 0.0;
    Quat a = Quat{0.0, 0.0, 0.0, 1.0};
    Float the_floor = 0.0;
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    Int first = 0;
    Int last = 0;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    for (auto it9437200 = flow_for(n9437200, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Debris>(combat_.debris))))); it9437200.next();) {
      combat_.debris[n9437200.index].v = vector_make(vector_break(combat_.debris[n9437200.index].v).x, vector_break(combat_.debris[n9437200.index].v).y, kern::formula([](Float z, Float kG, Float dt) -> Float { return (z - (kG * dt)); }, vector_break(combat_.debris[n9437200.index].v).z, kG, dt));
      combat_.debris[n9437200.index].p = vector_add(combat_.debris[n9437200.index].p, vector_scale(combat_.debris[n9437200.index].v, dt));
      a = engine_math_math::Quat_axis_angle(logic_select<Vector>(math_compare<">">(vector_length(combat_.debris[n9437200.index].spin), 9.999999717180685e-10), vector_normalize(combat_.debris[n9437200.index].spin), Vector{0.0, 0.0, 1.0}), kern::formula([](Float a, Float dt) -> Float { return (a * dt); }, vector_length(combat_.debris[n9437200.index].spin), dt)).Result;
      combat_.debris[n9437200.index].q = common::qnormalize(engine_math_math::Quat_mul(combat_.debris[n9437200.index].q, a).Result).Result;
      the_floor = kern::formula([](Float the_result, Float z) -> Float { return ((the_result + 0.06) + (z * 0.5)); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(combat_.debris[n9437200.index].p).x, vector_break(combat_.debris[n9437200.index].p).y).Result, vector_break(combat_.debris[n9437200.index].size).z);
      if (math_compare<"<">(vector_break(combat_.debris[n9437200.index].p).z, the_floor)) {
        combat_.debris[n9437200.index].p = vector_make(vector_break(combat_.debris[n9437200.index].p).x, vector_break(combat_.debris[n9437200.index].p).y, the_floor);
        if (math_compare<"<">(vector_break(combat_.debris[n9437200.index].v).z, 0.0)) {
          combat_.debris[n9437200.index].v = vector_make(vector_break(combat_.debris[n9437200.index].v).x, vector_break(combat_.debris[n9437200.index].v).y, kern::formula([](Float z) -> Float { return ((-z) * 0.3); }, vector_break(combat_.debris[n9437200.index].v).z));
        }
        combat_.debris[n9437200.index].v = vector_make(kern::formula([](Float x) -> Float { return (x * 0.76); }, vector_break(combat_.debris[n9437200.index].v).x), vector_break(combat_.debris[n9437200.index].v).y, vector_break(combat_.debris[n9437200.index].v).z);
        combat_.debris[n9437200.index].v = vector_make(vector_break(combat_.debris[n9437200.index].v).x, kern::formula([](Float y) -> Float { return (y * 0.76); }, vector_break(combat_.debris[n9437200.index].v).y), vector_break(combat_.debris[n9437200.index].v).z);
        combat_.debris[n9437200.index].spin = vector_scale(combat_.debris[n9437200.index].spin, 0.6);
      }
    }
    at = 0;
    kept = at;
    end = list_length<Debris>(combat_.debris);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(update_debris_match(combat_.debris[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n9437201, combat_.debris[at]);
          combat_.debris[kept] = n9437201.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(combat_.debris, kept, Debris{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 6.5});
    if (math_compare<">">(kern::formula([](Float a) -> Float { return ((63.0 < 0.0) != (a < 0.0) ? (a < 0.0) : (63.0 < a)); }, kern::to<Float>(list_length<Debris>(combat_.debris))), 0.5)) {
      first = 0;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 63.0); }, kern::to<Float>(list_length<Debris>(combat_.debris))))))));
      for (auto it9437202 = flow_for(n9437202, first, last); it9437202.next();) {
        list_remove_at(combat_.debris, first);
      }
    }
  }

  struct combat_command_startsResult { Bool Result = false; };
  KERN_PURE combat_command_startsResult combat_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct soft_target_nearResult { Bool Result = false; };
  KERN_PURE soft_target_nearResult soft_target_near(Vector p = Vector{0.0, 0.0, 0.0}) {
    Bool skip = false;
    Bool skip_2 = false;
    Float dx = 0.0;
    Float dy = 0.0;
    if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
      for (auto it10485770 = flow_for(n10485770, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(gaps_[0].lamps.lamps_))))); it10485770.next();) {
        skip = false;
        if (math_compare<"==">(kern::to<Float>(gaps_[0].lamps.lamps_[n10485770.index].state), 2.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_and(logic_and(logic_and(math_compare<"<">(kern::formula([](Float x, Float x_2) -> Float { return kern::math::abs((x - x_2)); }, vector_break(gaps_[0].lamps.lamps_[n10485770.index].foot).x, vector_break(p).x), 0.35), math_compare<"<">(kern::formula([](Float y, Float y_2) -> Float { return kern::math::abs((y - y_2)); }, vector_break(gaps_[0].lamps.lamps_[n10485770.index].foot).y, vector_break(p).y), 0.35)), math_compare<">">(vector_break(p).z, vector_break(gaps_[0].lamps.lamps_[n10485770.index].foot).z)), math_compare<"<">(vector_break(p).z, kern::formula([](Float z) -> Float { return (z + 6.4); }, vector_break(gaps_[0].lamps.lamps_[n10485770.index].foot).z)))) {
            return {true};
          }
          if (logic_and(logic_and(math_compare<"<">(kern::formula([](Float x, Float x_2) -> Float { return kern::math::abs((x - x_2)); }, vector_break(gaps_[0].lamps.lamps_[n10485770.index].head).x, vector_break(p).x), 0.45), math_compare<"<">(kern::formula([](Float y, Float y_2) -> Float { return kern::math::abs((y - y_2)); }, vector_break(gaps_[0].lamps.lamps_[n10485770.index].head).y, vector_break(p).y), 0.45)), math_compare<"<">(kern::formula([](Float z, Float z_2) -> Float { return kern::math::abs(((z + 6.0) - z_2)); }, vector_break(gaps_[0].lamps.lamps_[n10485770.index].foot).z, vector_break(p).z), 0.4))) {
            return {true};
          }
        }
      }
    }
    if (logic_and(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0), glife_[0].ready)) {
      for (auto it10485771 = flow_for(n10485771, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_))))); it10485771.next();) {
        skip_2 = false;
        if (glife_[0].life.cows_[n10485771.index].dead) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          dx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(p).x, vector_break(glife_[0].life.cows_[n10485771.index].pos).x);
          dy = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(p).y, vector_break(glife_[0].life.cows_[n10485771.index].pos).y);
          if (logic_and(math_compare<"<">(kern::formula([](Float dx, Float dy) -> Float { return ((dx * dx) + (dy * dy)); }, dx, dy), 1.1), math_compare<"<">(vector_break(p).z, kern::formula([](Float the_result) -> Float { return (the_result + 1.7); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(glife_[0].life.cows_[n10485771.index].pos).x, vector_break(glife_[0].life.cows_[n10485771.index].pos).y).Result)))) {
            return {true};
          }
        }
      }
    }
    return {false};
  }

  struct car_near_pointResult { Bool Result = false; };
  car_near_pointResult car_near_point(Vector point = Vector{0.0, 0.0, 0.0}) {
    Bool skip = false;
    BodyState b = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector local = Vector{0.0, 0.0, 0.0};
    Vector h = Vector{0.0, 0.0, 0.0};
    for (auto it10485772 = flow_for(n10485772, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it10485772.next();) {
      skip = false;
      if (logic_or(logic_not(cars_[n10485772.index].placed), cars_[n10485772.index].wreck)) {
        skip = true;
      }
      if (logic_not(skip)) {
        n10485773 = state_of(cars_[n10485772.index].actor);
        b = n10485773.Result;
        if (math_compare<">">(vector_length(vector_sub(b.position, point)), 6.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          local = common::to_local(b.rotation, vector_sub(point, b.position)).Result;
          h = cars_[n10485772.index].car[0].s_.half;
          if (logic_and(logic_and(math_compare<"<">(kern::formula([](Float x) -> Float { return kern::math::abs(x); }, vector_break(local).x), kern::formula([](Float x) -> Float { return (x + 0.4); }, vector_break(h).x)), math_compare<"<">(kern::formula([](Float y) -> Float { return kern::math::abs(y); }, vector_break(local).y), kern::formula([](Float y) -> Float { return (y + 0.4); }, vector_break(h).y))), math_compare<"<">(kern::formula([](Float z) -> Float { return kern::math::abs(z); }, vector_break(local).z), 2.0))) {
            return {true};
          }
        }
      }
    }
    return {false};
  }

  struct round_meets_mateResult { Bool Result = false; };
  /// A round meets a mate: his body as drawn (or the sphere round his vehicle) on the segment sets it off right there - grenades and rockets do not fly through other players.
  round_meets_mateResult round_meets_mate(Vector& at, Vector from = Vector{0.0, 0.0, 0.0}, Vector to = Vector{0.0, 0.0, 0.0}) {
    Float t = 0.0;
    mp_Zone z = mp_Zone::torso;
    t = 2.0;
    z = mp_Zone::torso;
    n10485774 = remote_on_segment(t, z, from, to);
    if (logic_or(math_compare<"<">(kern::to<Float>(n10485774.Result), 0.0), math_compare<">">(t, 1.0))) {
      return {false};
    }
    at = vector_add(from, vector_scale(vector_sub(to, from), t));
    return {true};
  }

  void grenade_bounce(Projectile& p, Vector point = Vector{0.0, 0.0, 0.0}, Vector n = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    Vector n_ = Vector{0.0, 0.0, 0.0};
    Float impact = 0.0;
    Float vn = 0.0;
    Float sp = 0.0;
    Int was = 0;
    n_ = n;
    impact = vector_length(p.vel);
    if (math_compare<">">(vector_dot(p.vel, n_), 0.0)) {
      n_ = vector_scale(n_, -1.0);
    }
    vn = vector_dot(p.vel, n_);
    if (math_compare<"<">(vn, 0.0)) {
      p.vel = vector_sub(p.vel, vector_scale(n_, kern::formula([](Float kRestitution, Float vn) -> Float { return ((1.0 + kRestitution) * vn); }, kRestitution, vn)));
    }
    p.vel = vector_scale(p.vel, kSurfaceDamping);
    p.bounces = kern::to<Int>(kern::formula([](Float kMaxBounces, Float bounces) -> Float { return kern::math::min(kMaxBounces, (bounces + 1.0)); }, kern::to<Float>(kMaxBounces), kern::to<Float>(p.bounces)));
    p.resting = false;
    p.next_collision = kern::formula([](Float now) -> Float { return (now + 0.036); }, now);
    p.pos = vector_add(point, vector_scale(n_, kern::formula([](Float kGrenadeR) -> Float { return (kGrenadeR + 0.014); }, kGrenadeR)));
    sp = vector_length(p.vel);
    if (math_compare<">">(sp, 0.05)) {
      p.pos = vector_add(p.pos, vector_scale(p.vel, kern::formula([](Float sp) -> Float { return kern::math::div(0.02, sp); }, sp)));
    }
    osfx("gameplay_final_runtime_v2.audioGrenadeBounce", point, impact, kern::to<Float>(p.bounces), 0.0);
    if (enum_equal<weapons_Id>(p.id, weapons_Id::grenade_launcher)) {
      was = bounce_fx_next_;
      bounce_fx_next_ = kern::to<Int>(kern::formula([](Float bounce_fx_next_) -> Float { return kern::math::wrap32(bounce_fx_next_ + 1.0); }, kern::to<Float>(bounce_fx_next_)));
      bounce_fx_[kern::to<Int>(kern::formula([](Float was) -> Float { return kern::math::mod(was, 8.0); }, kern::to<Float>(was)))] = struct_make<Game_BounceFx>(vector_add(point, vector_scale(n_, 0.008)), n_, now, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float impact) -> Float { return kern::math::div(impact, 12.0); }, impact), 0.55, 1.35)));
    }
    n10485775 = combat_sfx(combat_audio_Kind::bounce);
    engine_game_world_play_sound(world_, n10485775.Result, point, kern::formula([](Float a) -> Float { return (kern::math::div(a, 0.44) * 0.5); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float impact) -> Float { return (0.16 + (impact * 0.018)); }, impact), 0.16, 0.44)), kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float bounces, Float impact) -> Float { return ((0.92 + (bounces * 0.035)) + (0.02 * kern::math::min(impact, 10.0))); }, kern::to<Float>(p.bounces), impact), 0.88, 1.32)), true, 0);
    if (logic_and(math_compare<">=">(kern::to<Float>(p.bounces), kern::to<Float>(kMaxBounces)), math_compare<"<">(vector_length(p.vel), 2.8))) {
      p.vel = vector_scale(p.vel, 0.38);
    }
  }

  struct rocket_laser_dotResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  rocket_laser_dotResult rocket_laser_dot() {
    Bool flying = false;
    Vector from = Vector{0.0, 0.0, 0.0};
    Vector ad = Vector{0.0, 0.0, 0.0};
    Bool xr = false;
    Bool xr_2 = false;
    Bool both = false;
    Vector xr_eye = Vector{0.0, 0.0, 0.0};
    Vector xr_forward = Vector{0.0, 0.0, 0.0};
    the_RayResult_ r = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics = 0;
    flying = false;
    for (auto it10485776 = flow_for(n10485776, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it10485776.next();) {
      flying = logic_or(flying, logic_and(logic_and(logic_and(projectiles_[n10485776.index].alive, projectiles_[n10485776.index].by_player), enum_equal<weapons_Id>(projectiles_[n10485776.index].id, weapons_Id::rocket)), projectiles_[n10485776.index].guided));
    }
    if (logic_not(flying)) {
      laser_pinned_ = false;
    }
    if (laser_pinned_) {
      laser_valid_ = true;
      laser_dot_ = laser_pin_;
      return {laser_dot_};
    }
    from = Vector{0.0, 0.0, 0.0};
    ad = Vector{0.0, 0.0, 0.0};
    both = math_compare<"!=">(kern::to<Float>(engine_), 0.0);
    if (both) {
      xr_2 = engine_engine_xr(engine_);
      both = xr_2;
    }
    if (both) {
      xr_eye = engine_engine_xr_eye(engine_);
      from = xr_eye;
      xr_forward = engine_engine_xr_forward(engine_);
      ad = xr_forward;
    } else if (laser_finger_) {
      n10485777 = ctl_screen_ray(from, ad, laser_fx_, laser_fy_);
    } else {
      ad = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{1.0, 0.0, 0.0});
      from = cam_.eye;
    }
    engine_game_world_physics(n10485778, world_);
    physics = n10485778.result;
    r = common::cast(physics, from, ad, kLaserRange, body(soldier_).Result, Int64{0xffffffffffffffff}).Result;
    laser_valid_ = r.hit;
    laser_dot_ = logic_select<Vector>(r.hit, r.point, vector_add(from, vector_scale(ad, kLaserRange)));
    return {laser_dot_};
  }

  void rocket_smoke_emit(Projectile& p, Float now = 0.0) {
    Float l = 0.0;
    Int was = 0;
    Int was_2 = 0;
    Int was_3 = 0;
    Int was_4 = 0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), math_compare<"<">(kern::formula([](Float now, Float smoke_at) -> Float { return (now - smoke_at); }, now, p.smoke_at), 0.025))) {
      return;
    }
    p.smoke_at = now;
    l = kern::formula([](Float a) -> Float { return kern::math::max(0.001, a); }, vector_length(p.vel));
    was = gaps_[0].rocket_cursor;
    gaps_[0].rocket_cursor = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(gaps_[0].rocket_cursor)));
    gaps_[0].rocket_smoke[kern::to<Int>(kern::formula([](Float was, Float a) -> Float { return kern::math::mod(was, a); }, kern::to<Float>(was), kern::to<Float>(list_length<GapsState_RocketPuff>(gaps_[0].rocket_smoke))))].p = vector_add(vector_sub(p.pos, vector_scale(p.vel, kern::formula([](Float l) -> Float { return kern::math::div(0.42, l); }, l))), vector_make(kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.05); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mul32(a, 3.0) + 1.0); }, kern::to<Float>(gaps_[0].rocket_cursor)))).Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.05); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mul32(a, 3.0) + 2.0); }, kern::to<Float>(gaps_[0].rocket_cursor)))).Result), 0.0));
    was_2 = gaps_[0].rocket_cursor;
    gaps_[0].rocket_cursor = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(gaps_[0].rocket_cursor)));
    gaps_[0].rocket_smoke[kern::to<Int>(kern::formula([](Float was_2, Float a) -> Float { return kern::math::mod(was_2, a); }, kern::to<Float>(was_2), kern::to<Float>(list_length<GapsState_RocketPuff>(gaps_[0].rocket_smoke))))].born = now;
    was_3 = gaps_[0].rocket_cursor;
    gaps_[0].rocket_cursor = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(gaps_[0].rocket_cursor)));
    gaps_[0].rocket_smoke[kern::to<Int>(kern::formula([](Float was_3, Float a) -> Float { return kern::math::mod(was_3, a); }, kern::to<Float>(was_3), kern::to<Float>(list_length<GapsState_RocketPuff>(gaps_[0].rocket_smoke))))].life = kern::formula([](Float the_result) -> Float { return (1.1 + (the_result * 0.6)); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mul32(a, 3.0) + 3.0); }, kern::to<Float>(gaps_[0].rocket_cursor)))).Result);
    was_4 = gaps_[0].rocket_cursor;
    gaps_[0].rocket_cursor = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(gaps_[0].rocket_cursor)));
    gaps_[0].rocket_smoke[kern::to<Int>(kern::formula([](Float was_4, Float a) -> Float { return kern::math::mod(was_4, a); }, kern::to<Float>(was_4), kern::to<Float>(list_length<GapsState_RocketPuff>(gaps_[0].rocket_smoke))))].rot = kern::formula([](Float the_result) -> Float { return (the_result * 6.28); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mul32(a, 5.0) + 4.0); }, kern::to<Float>(gaps_[0].rocket_cursor)))).Result);
  }

  struct foot_round_stepResult { Bool Result = false; };
  foot_round_stepResult foot_round_step(Projectile& p, Vector& at, Float dt = 0.0, Float now = 0.0) {
    Int n = 0;
    Float sdt = 0.0;
    Bool hand = false;
    Int i = 0;
    Bool stop_2 = false;
    Bool skip = false;
    Bool laser = false;
    Float sp = 0.0;
    Vector dir = Vector{0.0, 0.0, 0.0};
    Vector dot_at = Vector{0.0, 0.0, 0.0};
    Vector to = Vector{0.0, 0.0, 0.0};
    Float d = 0.0;
    Vector want = Vector{0.0, 0.0, 0.0};
    Float ang = 0.0;
    Float max_turn = 0.0;
    Float turn = 0.0;
    Vector perp = Vector{0.0, 0.0, 0.0};
    Float trav = 0.0;
    the_RayResult_ r = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics = 0;
    Vector seg_to = Vector{0.0, 0.0, 0.0};
    Vector next = Vector{0.0, 0.0, 0.0};
    Float speed = 0.0;
    Float ground = 0.0;
    Vector dir_2 = Vector{0.0, 0.0, 0.0};
    Float travel = 0.0;
    the_RayResult_ r_2 = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics_2 = 0;
    Bool the_floor = false;
    Bool both = false;
    Vector seg_to_2 = Vector{0.0, 0.0, 0.0};
    Vector next_2 = Vector{0.0, 0.0, 0.0};
    Float floor_z = 0.0;
    Float horizontal = 0.0;
    Float down = 0.0;
    at = p.pos;
    if (logic_or(math_compare<">=">(kern::formula([](Float now, Float born) -> Float { return (now - born); }, now, p.born), p.fuse), math_compare<">">(kern::formula([](Float now, Float born) -> Float { return (now - born); }, now, p.born), kern::formula([](Float a, Float kRocketTtl, Float kGrenadeTtl) -> Float { return ((a != 0.0) ? kRocketTtl : kGrenadeTtl); }, kern::to<Float>(enum_equal<weapons_Id>(p.id, weapons_Id::rocket)), kRocketTtl, kGrenadeTtl)))) {
      return {true};
    }
    n = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float dt) -> Float { return kern::math::trunc(kern::math::ceil(kern::math::div(dt, 0.014))); }, dt)))), 1.0, 4.0)));
    sdt = kern::formula([](Float dt, Float n) -> Float { return kern::math::div(dt, n); }, dt, kern::to<Float>(n));
    hand = enum_equal<weapons_Id>(p.id, weapons_Id::hand_grenade);
    i = 0;
    stop_2 = false;
    while (loop(logic_and(logic_not(stop_2), math_compare<"<">(kern::to<Float>(i), kern::to<Float>(n))))) {
      skip = false;
      if (enum_equal<weapons_Id>(p.id, weapons_Id::rocket)) {
        laser = logic_and(logic_and(enum_equal<Mode>(mode_, Mode::foot), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::rocket)), logic_not(player_dead_));
        if (logic_and(p.guided, logic_not(laser))) {
          p.guided = false;
        }
        if (logic_not(p.guided)) {
          p.vel = vector_make(vector_break(p.vel).x, vector_break(p.vel).y, kern::formula([](Float z, Float sdt) -> Float { return (z - (1.2 * sdt)); }, vector_break(p.vel).z, sdt));
        }
        sp = vector_length(p.vel);
        dir = vector_scale(p.vel, kern::formula([](Float sp) -> Float { return kern::math::div(1.0, kern::math::max(0.001, sp)); }, sp));
        if (p.guided) {
          n10485779 = rocket_laser_dot();
          dot_at = n10485779.Result;
          to = vector_sub(dot_at, p.pos);
          d = vector_length(to);
          if (math_compare<">=">(d, 0.5)) {
            want = vector_scale(to, kern::formula([](Float d) -> Float { return kern::math::div(1.0, d); }, d));
            ang = kern::formula([](Float a) -> Float { return kern::math::acos((a < (-1.0) ? (-1.0) : (1.0 < a ? 1.0 : a))); }, vector_dot(dir, want));
            max_turn = kern::formula([](Float kRocketTurn, Float sdt) -> Float { return (kRocketTurn * sdt); }, kRocketTurn, sdt);
            if (math_compare<">=">(ang, 0.0001)) {
              turn = kern::formula([](Float ang, Float max_turn) -> Float { return kern::math::min(ang, max_turn); }, ang, max_turn);
              perp = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(want, vector_scale(dir, kern::formula([](Float ang) -> Float { return kern::math::cos(ang); }, ang)))), 9.999999717180685e-10), vector_normalize(vector_sub(want, vector_scale(dir, kern::formula([](Float ang) -> Float { return kern::math::cos(ang); }, ang)))), dir);
              dir = logic_select<Vector>(math_compare<">">(vector_length(vector_add(vector_scale(dir, kern::formula([](Float turn) -> Float { return kern::math::cos(turn); }, turn)), vector_scale(perp, kern::formula([](Float turn) -> Float { return kern::math::sin(turn); }, turn)))), 9.999999717180685e-10), vector_normalize(vector_add(vector_scale(dir, kern::formula([](Float turn) -> Float { return kern::math::cos(turn); }, turn)), vector_scale(perp, kern::formula([](Float turn) -> Float { return kern::math::sin(turn); }, turn)))), dir);
            }
          }
        }
        sp = kern::formula([](Float kRocketTop, Float sp, Float kRocketAccel, Float sdt) -> Float { return kern::math::min(kRocketTop, (sp + (kRocketAccel * sdt))); }, kRocketTop, sp, kRocketAccel, sdt);
        p.vel = vector_scale(dir, sp);
        trav = kern::formula([](Float sp, Float sdt) -> Float { return (sp * sdt); }, sp, sdt);
        rocket_smoke_emit(p, now);
        engine_game_world_physics(n10485780, world_);
        physics = n10485780.result;
        r = common::cast(physics, p.pos, dir, kern::formula([](Float trav) -> Float { return (trav + 0.2); }, trav), p.ignore, Int64{0xffffffffffffffff}).Result;
        if (logic_and(r.hit, math_compare<"<=">(r.distance, kern::formula([](Float trav) -> Float { return (trav + 0.2); }, trav)))) {
          at = r.point;
          return {true};
        }
        seg_to = vector_add(p.pos, vector_scale(dir, trav));
        n10485781 = people_cpp::People_shot(people_, seg_to, p.pos, 0.0, false, now, -1.0, 0.0);
        if (math_compare<">=">(kern::to<Float>(n10485781.Result), 0.0)) {
          at = seg_to;
          return {true};
        }
        n10485782 = round_meets_mate(at, p.pos, seg_to);
        if (n10485782.Result) {
          return {true};
        }
        if (soft_target_near(p.pos).Result) {
          at = p.pos;
          return {true};
        }
        for (auto it10485783 = flow_for(n10485783, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it10485783.next();) {
          if (logic_and(pdrones_[n10485783.index].active, math_compare<"<">(vector_length(vector_sub(pdrones_[n10485783.index].pos, p.pos)), kern::formula([](Float trav) -> Float { return (1.2 + trav); }, trav)))) {
            at = pdrones_[n10485783.index].pos;
            return {true};
          }
        }
        next = vector_add(p.pos, vector_scale(dir, trav));
        if (math_compare<"<=">(vector_break(next).z, kern::formula([](Float the_result) -> Float { return (the_result + 0.08); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(next).x, vector_break(next).y).Result))) {
          at = next;
          return {true};
        }
        p.pos = next;
        at = next;
        skip = true;
      }
      if (logic_not(logic_or(stop_2, skip))) {
        if (p.resting) {
          p.pos = vector_make(vector_break(p.pos).x, vector_break(p.pos).y, kern::formula([](Float the_result, Float kGrenadeR) -> Float { return (the_result + kGrenadeR); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(p.pos).x, vector_break(p.pos).y).Result, kGrenadeR));
          stop_2 = true;
        }
        if (logic_not(logic_or(stop_2, skip))) {
          p.vel = vector_make(vector_break(p.vel).x, vector_break(p.vel).y, kern::formula([](Float z, Float kGrenadeGravity, Float sdt) -> Float { return (z - (kGrenadeGravity * sdt)); }, vector_break(p.vel).z, kGrenadeGravity, sdt));
          speed = vector_length(p.vel);
          if (math_compare<"<">(speed, 0.05)) {
            ground = city_stream_cpp::CityStream_ground_height(city_, vector_break(p.pos).x, vector_break(p.pos).y).Result;
            if (math_compare<">">(vector_break(p.pos).z, kern::formula([](Float ground, Float kGrenadeR) -> Float { return ((ground + kGrenadeR) + 0.08); }, ground, kGrenadeR))) {
              p.vel = vector_make(vector_break(p.vel).x, vector_break(p.vel).y, -1.15);
            } else {
              p.pos = vector_make(vector_break(p.pos).x, vector_break(p.pos).y, kern::formula([](Float ground, Float kGrenadeR) -> Float { return (ground + kGrenadeR); }, ground, kGrenadeR));
              p.vel = Vector{0.0, 0.0, 0.0};
              p.resting = true;
              stop_2 = true;
            }
            if (logic_not(logic_or(stop_2, skip))) {
              speed = vector_length(p.vel);
            }
          }
          if (logic_not(logic_or(stop_2, skip))) {
            dir_2 = vector_scale(p.vel, kern::formula([](Float speed) -> Float { return kern::math::div(1.0, kern::math::max(0.001, speed)); }, speed));
            travel = kern::formula([](Float speed, Float sdt) -> Float { return kern::math::max(0.001, (speed * sdt)); }, speed, sdt);
            if (math_compare<">=">(now, p.next_collision)) {
              engine_game_world_physics(n10485784, world_);
              physics_2 = n10485784.result;
              r_2 = common::cast(physics_2, p.pos, dir_2, kern::formula([](Float travel, Float kGrenadeR) -> Float { return ((travel + kGrenadeR) + 0.035); }, travel, kGrenadeR), p.ignore, Int64{0xffffffffffffffff}).Result;
              if (logic_and(r_2.hit, math_compare<"<=">(r_2.distance, kern::formula([](Float travel, Float kGrenadeR) -> Float { return ((travel + kGrenadeR) + 0.018); }, travel, kGrenadeR)))) {
                both = math_compare<">=">(vector_break(r_2.normal).z, kFloorNormalZ);
                if (both) {
                  n10485786 = car_near_point(r_2.point);
                  both = logic_not(n10485786.Result);
                }
                the_floor = both;
                if (logic_and(logic_not(hand), logic_not(the_floor))) {
                  at = r_2.point;
                  return {true};
                }
                grenade_bounce(p, r_2.point, r_2.normal, now);
                skip = true;
              }
            }
            if (logic_not(logic_or(stop_2, skip))) {
              seg_to_2 = vector_add(p.pos, vector_scale(dir_2, kern::formula([](Float travel, Float kGrenadeR) -> Float { return (travel + kGrenadeR); }, travel, kGrenadeR)));
              n10485787 = people_cpp::People_shot(people_, seg_to_2, p.pos, 0.0, false, now, -1.0, 0.0);
              if (math_compare<">=">(kern::to<Float>(n10485787.Result), 0.0)) {
                at = seg_to_2;
                return {true};
              }
              n10485788 = round_meets_mate(at, p.pos, seg_to_2);
              if (n10485788.Result) {
                return {true};
              }
              if (logic_not(hand)) {
                if (soft_target_near(p.pos).Result) {
                  at = p.pos;
                  return {true};
                }
                for (auto it10485789 = flow_for(n10485789, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it10485789.next();) {
                  if (logic_and(pdrones_[n10485789.index].active, math_compare<"<">(vector_length(vector_sub(pdrones_[n10485789.index].pos, p.pos)), kern::formula([](Float travel) -> Float { return (1.2 + travel); }, travel)))) {
                    at = pdrones_[n10485789.index].pos;
                    return {true};
                  }
                }
              }
              next_2 = vector_add(p.pos, vector_scale(dir_2, travel));
              floor_z = kern::formula([](Float the_result, Float kGrenadeR) -> Float { return (the_result + kGrenadeR); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(next_2).x, vector_break(next_2).y).Result, kGrenadeR);
              if (math_compare<"<=">(vector_break(next_2).z, floor_z)) {
                next_2 = vector_make(vector_break(next_2).x, vector_break(next_2).y, floor_z);
                horizontal = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(p.vel).x, vector_break(p.vel).y);
                down = kern::formula([](Float z) -> Float { return kern::math::abs(z); }, vector_break(p.vel).z);
                if (logic_and(logic_and(math_compare<">=">(kern::to<Float>(p.bounces), 3.0), math_compare<"<">(down, 1.15)), math_compare<"<">(horizontal, 1.35))) {
                  p.pos = next_2;
                  p.vel = Vector{0.0, 0.0, 0.0};
                  p.resting = true;
                  stop_2 = true;
                }
                if (logic_not(logic_or(stop_2, skip))) {
                  grenade_bounce(p, next_2, Vector{0.0, 0.0, 1.0}, now);
                  skip = true;
                }
              }
              if (logic_not(logic_or(stop_2, skip))) {
                p.pos = next_2;
              }
            }
          }
        }
      }
      if (logic_not(stop_2)) {
        i += 1;
      }
    }
    if (logic_and(logic_and(logic_and(logic_not(enum_equal<weapons_Id>(p.id, weapons_Id::rocket)), logic_not(p.resting)), math_compare<">">(vector_break(p.pos).z, kern::formula([](Float the_result, Float kGrenadeR) -> Float { return ((the_result + kGrenadeR) + 0.12); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(p.pos).x, vector_break(p.pos).y).Result, kGrenadeR))), math_compare<"<">(vector_length(p.vel), 0.6))) {
      p.vel = vector_make(vector_break(p.vel).x, vector_break(p.vel).y, kern::formula([](Float z) -> Float { return kern::math::min(z, (-1.15)); }, vector_break(p.vel).z));
    }
    at = p.pos;
    return {false};
  }

  struct foot_remote_detonateResult { Bool Result = false; };
  foot_remote_detonateResult foot_remote_detonate(Float now = 0.0) {
    Bool any = false;
    any = false;
    for (auto it10485790 = flow_for(n10485790, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it10485790.next();) {
      if (logic_and(logic_and(projectiles_[n10485790.index].alive, projectiles_[n10485790.index].by_player), logic_or(enum_equal<weapons_Id>(projectiles_[n10485790.index].id, weapons_Id::grenade_launcher), enum_equal<weapons_Id>(projectiles_[n10485790.index].id, weapons_Id::rocket)))) {
        projectiles_[n10485790.index].fuse = 0.0;
        projectiles_[n10485790.index].born = kern::formula([](Float now) -> Float { return (now - 1.0); }, now);
        any = true;
      }
    }
    if (any) {
      next_attack_ = kern::formula([](Float now, Float interval_ms) -> Float { return (now + (kern::math::div(interval_ms, 1000.0) * 0.5)); }, now, struct_break<weapons_Spec>(list_get<weapons_Spec>(kTable, enum_index<weapons_Id>(weapons_Id::grenade_launcher))).interval_ms);
    }
    return {any};
  }

  void foot_rocket_launched() {
    Int i = 0;
    laser_pinned_ = false;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Projectile>(projectiles_))), 0.5))) {
      if (logic_and(enum_equal<weapons_Id>(projectiles_[i].id, weapons_Id::rocket), projectiles_[i].by_player)) {
        projectiles_[i].guided = false;
      }
      i += 1;
    }
    if (logic_and(logic_not(math_compare<"==">(kern::to<Float>(list_length<Projectile>(projectiles_)), 0.0)), enum_equal<weapons_Id>(projectiles_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))].id, weapons_Id::rocket))) {
      projectiles_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))].guided = true;
    }
  }

  struct Game_foot_rounds_update_ownResult { Bool Result = false; };
  /// the player's shots on foot: Kern's soldier, or the game's touch shot through the finger (from the eye)
  Game_foot_rounds_update_ownResult Game_foot_rounds_update_own(const game_ShotEvent& s) {
    return {logic_or(math_compare<"==">(kern::to<Float>(s.shooter), kern::to<Float>(soldier_)), logic_and(logic_and(math_compare<"==">(kern::to<Float>(s.shooter), 0.0), enum_equal<Mode>(mode_, Mode::foot)), math_compare<"<">(vector_length(vector_sub(s.from, cam_.eye)), 0.5)))};
  }

  struct gaps_sniper_rounds_matchResult { Bool Result = false; };
  KERN_PURE gaps_sniper_rounds_matchResult gaps_sniper_rounds_match(const GapsState_SniperRound& r, Float now = 0.0) {
    return {math_compare<">=">(now, r.due)};
  }

  void gaps_sniper_rounds(const List<game_ShotEvent>& shots, List<game_ShotEvent>& arrived, Float now = 0.0) {
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0))) {
      for (auto it10485803 = flow_for(n10485803, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_ShotEvent>(shots))))); it10485803.next();) {
        if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(shots[n10485803.index].shooter), kern::to<Float>(soldier_)), enum_equal<Mode>(mode_, Mode::foot)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper))) {
          list_add(arrived, shots[n10485803.index]);
        }
      }
      return;
    }
    for (auto it10485804 = flow_for(n10485804, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_ShotEvent>(shots))))); it10485804.next();) {
      if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(shots[n10485804.index].shooter), kern::to<Float>(soldier_)), enum_equal<Mode>(mode_, Mode::foot)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper))) {
        list_add(gaps_[0].sniper_rounds, struct_make<GapsState_SniperRound>(shots[n10485804.index], kern::formula([](Float now, Float a) -> Float { return (now + kern::math::div(a, 850.0)); }, now, vector_length(vector_sub(shots[n10485804.index].to, shots[n10485804.index].from)))));
      }
    }
    for (auto it10485805 = flow_for(n10485805, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_SniperRound>(gaps_[0].sniper_rounds))))); it10485805.next();) {
      if (math_compare<">=">(now, gaps_[0].sniper_rounds[n10485805.index].due)) {
        list_add(arrived, gaps_[0].sniper_rounds[n10485805.index].s);
      }
    }
    at = 0;
    kept = at;
    end = list_length<GapsState_SniperRound>(gaps_[0].sniper_rounds);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(gaps_sniper_rounds_match(gaps_[0].sniper_rounds[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n10485806, gaps_[0].sniper_rounds[at]);
          gaps_[0].sniper_rounds[kept] = n10485806.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(gaps_[0].sniper_rounds, kept, GapsState_SniperRound{game_ShotEvent{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, false, false, Vector{0.0, 0.0, 1.0}, 0}, 0.0});
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
    engine_renderer_allocate_id(n11534347, renderer);
    allocate_id = n11534347.result;
    p.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    renderer_2 = engine_engine_renderer(e);
    engine_upsert(n11534348, renderer_2, p.id, disc, look, parked.m, List<Float>{});
    upsert = n11534348.result;
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
    n11534346 = game_gaps_cpp::smoke_disc();
    disc = n11534346.Result;
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
    engine_renderer_allocate_id(n11534349, renderer_3);
    allocate_id = n11534349.result;
    gaps_[0].trail_part.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    renderer_4 = engine_engine_renderer(e);
    engine_upsert(n11534350, renderer_4, gaps_[0].trail_part.id, disc, look, parked.m, List<Float>{});
    upsert = n11534350.result;
    if (logic_not(upsert)) {
      renderer_5 = engine_engine_renderer(e);
      engine_renderer_release_id(renderer_5, gaps_[0].trail_part.id);
      gaps_[0].trail_part.id = 0;
    }
    m.color = engine_math_math::Vec4_new(0.847, 0.847, 0.847, 1.0).Result;
    look.material = m;
    renderer_6 = engine_engine_renderer(e);
    engine_renderer_allocate_id(n11534351, renderer_6);
    allocate_id_2 = n11534351.result;
    gaps_[0].rocket_part.id = allocate_id_2;
    renderer_7 = engine_engine_renderer(e);
    engine_upsert(n11534352, renderer_7, gaps_[0].rocket_part.id, disc, look, parked.m, List<Float>{});
    upsert_2 = n11534352.result;
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
    for (auto it11534353 = flow_for(n11534353, 0, 1); it11534353.next();) {
      for (auto it11534354 = flow_for(n11534354, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float rate) -> Float { return (rate * 0.05); }, kern::to<Float>(rate)))))))); it11534354.next();) {
        n = kern::to<Int>(kern::formula([](Float index, Float rate, Float index_2) -> Float { return (kern::math::trunc(((index * 0.09) * rate)) + index_2); }, kern::to<Float>(n11534353.index), kern::to<Float>(rate), kern::to<Float>(n11534354.index)));
        if (math_compare<">">(kern::formula([](Float n, Float a) -> Float { return ((n < 0.0) != (a < 0.0) ? (a < 0.0) : (n < a)); }, kern::to<Float>(n), kern::to<Float>(list_length<Float>(lock))), 0.5)) {
          lock[n] = kern::formula([](Float index, Float rate) -> Float { return ((kern::math::mod(kern::math::div((index * 1760.0), rate), 1.0) < 0.5) ? 0.05 : (-0.05)); }, kern::to<Float>(n11534354.index), kern::to<Float>(rate));
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
    engine_sound_bank_set_pcm(n11534355, bank_, "sc:locktone", lock, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(lock)))), 1, rate);
    set_pcm = n11534355.result;
    engine_sound_bank_request(bank_, "sc:guidedbang");
    engine_sound_bank_set_pcm(n11534356, bank_, "sc:guidedbang", bang, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(bang)))), 1, rate);
    set_pcm_2 = n11534356.result;
    gaps_[0].guided_sounds = true;
  }

  struct gaps_guided_matchResult { Bool Result = false; };
  KERN_PURE gaps_guided_matchResult gaps_guided_match(const GapsState_MotorVoice& v) {
    return {logic_not(v.alive)};
  }

  void gaps_guided(Float dt = 0.0) {
    Float now = 0.0;
    Float sim_time = 0.0;
    Bool ready = false;
    Int rate = 0;
    Int sample_rate = 0;
    List<Float> d = List<Float>{};
    List<Float> made = List<Float>{};
    Int s = 0;
    Float b = 0.0;
    Biquad bp = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    Float w = 0.0;
    Float c = 0.0;
    Float al = 0.0;
    Float a0 = 0.0;
    Int i = 0;
    Float w_2 = 0.0;
    Float saw = 0.0;
    Int adopt_clip = 0;
    Bool skip = false;
    Int voice = 0;
    Int i_2 = 0;
    Bool stop_2 = false;
    audio_PlayParams p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
    Int64 play = Int64{0x0};
    Float t = 0.0;
    Float vol = 0.0;
    Float sp = 0.0;
    Int was = 0;
    Int was_2 = 0;
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    Bool skip_2 = false;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    ready = engine_mixer_ready(mixer_);
    if (logic_and(ready, logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].motor_clip), 0.0)))) {
      sample_rate = engine_mixer_sample_rate(mixer_);
      rate = sample_rate;
      made = List<Float>{};
      list_resize(made, kern::to<Int>(kern::formula([](Float rate) -> Float { return (rate * 2.0); }, kern::to<Float>(rate))), 0.0);
      d = made;
      s = 5369127;
      b = 0.0;
      bp = Biquad{1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
      w = kern::formula([](Float kPi, Float rate) -> Float { return kern::math::div(((2.0 * kPi) * 700.0), rate); }, kPi, kern::to<Float>(rate));
      c = kern::formula([](Float w) -> Float { return kern::math::cos(w); }, w);
      al = kern::formula([](Float w) -> Float { return kern::math::div(kern::math::sin(w), (2.0 * 0.7)); }, w);
      a0 = kern::formula([](Float al) -> Float { return (1.0 + al); }, al);
      bp.b0 = kern::formula([](Float al, Float a0) -> Float { return kern::math::div(al, a0); }, al, a0);
      bp.b1 = 0.0;
      bp.b2 = kern::formula([](Float al, Float a0) -> Float { return kern::math::div((-al), a0); }, al, a0);
      bp.a1 = kern::formula([](Float c, Float a0) -> Float { return kern::math::div(((-2.0) * c), a0); }, c, a0);
      bp.a2 = kern::formula([](Float al, Float a0) -> Float { return kern::math::div((1.0 - al), a0); }, al, a0);
      i = 0;
      while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(d))), 0.5))) {
        s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::wrap32(kern::math::shl(s, 13.0))); }, kern::to<Float>(s)));
        s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::shr(s, 17.0)); }, kern::to<Float>(s)));
        s = kern::to<Int>(kern::formula([](Float s) -> Float { return kern::math::bxor(s, kern::math::wrap32(kern::math::shl(s, 5.0))); }, kern::to<Float>(s)));
        w_2 = kern::formula([](Float s) -> Float { return (kern::math::div(s, 2147483648.0) - 1.0); }, kern::to<Float>(s));
        b = kern::formula([](Float b, Float w_2) -> Float { return ((0.96 * b) + (0.04 * w_2)); }, b, w_2);
        saw = kern::formula([](Float i, Float rate) -> Float { return ((2.0 * kern::math::mod(kern::math::div(((i < 0.0 ? i + 18446744073709551616.0 : i) * 1300.0), rate), 1.0)) - 1.0); }, kern::to<Float>(i), kern::to<Float>(rate));
        n11534357 = game_gaps_cpp::Biquad_run(bp, kern::formula([](Float w_2, Float b) -> Float { return ((w_2 * 0.5) + (b * 2.0)); }, w_2, b));
        d[i] = kern::formula([](Float the_result, Float saw) -> Float { return (the_result + (saw * 0.05)); }, n11534357.Result, saw);
        i += 1;
      }
      engine_audio_make_seamless(n11534358, d, rate, 0.05);
      d = n11534358.samples;
      engine_audio_normalize(n11534359, d, 0.8);
      d = n11534359.samples;
      engine_mixer_adopt_clip(n11534360, mixer_, d, 1, "spawncity:guided-motor", true);
      adopt_clip = n11534360.result;
      gaps_[0].motor_clip = adopt_clip;
    }
    for (auto it11534361 = flow_for(n11534361, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_MotorVoice>(gaps_[0].motors))))); it11534361.next();) {
      gaps_[0].motors[n11534361.index].alive = false;
    }
    for (auto it11534362 = flow_for(n11534362, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))); it11534362.next();) {
      skip = false;
      voice = list_length<GapsState_MotorVoice>(gaps_[0].motors);
      i_2 = 0;
      stop_2 = false;
      while (loop(logic_and(logic_not(stop_2), math_compare<">">(kern::formula([](Float i_2, Float a) -> Float { return ((i_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_2 < a)); }, kern::to<Float>(i_2), kern::to<Float>(list_length<GapsState_MotorVoice>(gaps_[0].motors))), 0.5)))) {
        if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a == a_2); }, gaps_[0].motors[i_2].born, guided_[n11534362.index].m.born), 0.5)) {
          voice = i_2;
          stop_2 = true;
        }
        if (logic_not(stop_2)) {
          i_2 += 1;
        }
      }
      if (math_compare<"==">(kern::to<Float>(voice), kern::to<Float>(list_length<GapsState_MotorVoice>(gaps_[0].motors)))) {
        list_add(gaps_[0].motors, struct_make<GapsState_MotorVoice>(guided_[n11534362.index].m.born, -1.0, 0, false));
      }
      gaps_[0].motors[voice].alive = true;
      if (logic_not(guided_[n11534362.index].m.motor)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (math_compare<"<">(gaps_[0].motors[voice].lit, 0.0)) {
          gaps_[0].motors[voice].lit = now;
          gaps_guided_sounds();
          engine_game_world_play_sound(world_, "sc:guidedbang", cam_.eye, 1.0, 1.0, false, 0);
          if (math_compare<"!=">(kern::to<Float>(gaps_[0].motor_clip), 0.0)) {
            p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
            p.the_loop = true;
            p.spatial = true;
            p.position = guided_[n11534362.index].m.p;
            p.velocity = guided_[n11534362.index].m.v;
            p.min_distance = 6.0;
            p.max_distance = 600.0;
            p.volume = 0.5;
            p.bus = audio_Bus::sfx;
            p.fade_in = 0.06;
            engine_mixer_play(n11534363, mixer_, gaps_[0].motor_clip, p);
            play = n11534363.result;
            gaps_[0].motors[voice].v = int64_to_int(play);
          }
        }
        if (math_compare<"!=">(kern::to<Float>(gaps_[0].motors[voice].v), 0.0)) {
          t = kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, gaps_[0].motors[voice].lit);
          vol = kern::formula([](Float t) -> Float { return ((t < 0.06) ? kern::math::div((0.5 * t), 0.06) : (0.5 * kern::math::pow(kern::math::div(0.28, 0.5), kern::math::min(1.0, kern::math::div((t - 0.06), 0.54))))); }, t);
          engine_mixer_set_volume(mixer_, gaps_[0].motors[voice].v, vol, 0.03);
          engine_mixer_set_position(mixer_, gaps_[0].motors[voice].v, guided_[n11534362.index].m.p, guided_[n11534362.index].m.v);
        }
        sp = vector_length(guided_[n11534362.index].m.v);
        was = gaps_[0].trail_cursor;
        gaps_[0].trail_cursor = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(gaps_[0].trail_cursor)));
        gaps_[0].trail[kern::to<Int>(kern::formula([](Float was, Float a) -> Float { return kern::math::mod(was, a); }, kern::to<Float>(was), kern::to<Float>(list_length<GapsState_TrailPuff>(gaps_[0].trail))))].p = vector_add(vector_sub(guided_[n11534362.index].m.p, vector_scale(guided_[n11534362.index].m.v, kern::formula([](Float sp) -> Float { return kern::math::div(0.9, kern::math::max(1.0, sp)); }, sp))), vector_make(kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.2); }, gaps_rules_cpp::unit_hash(gaps_[0].trail_cursor).Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.2); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 7.0); }, kern::to<Float>(gaps_[0].trail_cursor)))).Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.2); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 13.0); }, kern::to<Float>(gaps_[0].trail_cursor)))).Result)));
        was_2 = gaps_[0].trail_cursor;
        gaps_[0].trail_cursor = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(gaps_[0].trail_cursor)));
        gaps_[0].trail[kern::to<Int>(kern::formula([](Float was_2, Float a) -> Float { return kern::math::mod(was_2, a); }, kern::to<Float>(was_2), kern::to<Float>(list_length<GapsState_TrailPuff>(gaps_[0].trail))))].born = now;
      }
    }
    for (auto it11534364 = flow_for(n11534364, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_MotorVoice>(gaps_[0].motors))))); it11534364.next();) {
      if (logic_and(logic_not(gaps_[0].motors[n11534364.index].alive), math_compare<"!=">(kern::to<Float>(gaps_[0].motors[n11534364.index].v), 0.0))) {
        engine_mixer_stop(mixer_, gaps_[0].motors[n11534364.index].v, 0.05);
      }
    }
    at = 0;
    kept = at;
    end = list_length<GapsState_MotorVoice>(gaps_[0].motors);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(gaps_guided_match(gaps_[0].motors[at]).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n11534365, gaps_[0].motors[at]);
          gaps_[0].motors[kept] = n11534365.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(gaps_[0].motors, kept, GapsState_MotorVoice{0.0, -1.0, 0, false});
    for (auto it11534366 = flow_for(n11534366, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_TrailPuff>(gaps_[0].trail))))); it11534366.next();) {
      skip_2 = false;
      if (math_compare<"<">(gaps_[0].trail[n11534366.index].born, 0.0)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        if (math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, gaps_[0].trail[n11534366.index].born), 2.6)) {
          gaps_[0].trail[n11534366.index].born = -1.0;
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          gaps_[0].trail[n11534366.index].p = vector_make(vector_break(gaps_[0].trail[n11534366.index].p).x, vector_break(gaps_[0].trail[n11534366.index].p).y, kern::formula([](Float z, Float a) -> Float { return (z + ((0.006 * 60.0) * a)); }, vector_break(gaps_[0].trail[n11534366.index].p).z, gaps_[0].frame_dt));
        }
      }
    }
  }

  void gaps_jet_damage(Float amount = 0.0) {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float now = 0.0;
    Float sim_time = 0.0;
    Text a_2 = "";
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Text a_4 = "";
    Text a_5 = "";
    if (logic_or(logic_not(math_compare<">">(amount, 0.0)), logic_not(jet_placed_))) {
      return;
    }
    if (logic_and(math_compare<">">(amount, 8.0), enum_equal<Mode>(mode_, Mode::jet))) {
      n11534369 = matrix_of(jet_actor_);
      a = n11534369.Result;
      osfx("jet_mode.knock", engine_math_math::Mat4_translation(a).Result, 0.0, 0.0, 0.0);
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    jet_hp_ = kern::formula([](Float jet_hp_, Float kJetHpMax, Float amount) -> Float { return kern::math::max(0.0, (kern::math::min(jet_hp_, kJetHpMax) - amount)); }, jet_hp_, kJetHpMax, amount);
    jet_.thrust_scale = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float jet_hp_, Float kJetHpMax) -> Float { return (0.35 + (kern::math::div(jet_hp_, (kJetHpMax * 0.5)) * 0.65)); }, jet_hp_, kJetHpMax), 0.35, 1.0));
    look_.shake.trauma = kern::formula([](Float trauma, Float amount) -> Float { return kern::math::min(1.0, (trauma + kern::math::min(0.6, kern::math::div(amount, 120.0)))); }, look_.shake.trauma, amount);
    if (logic_and(logic_and(math_compare<"<=">(jet_hp_, kern::formula([](Float kJetHpMax) -> Float { return (kJetHpMax * 0.15); }, kJetHpMax)), math_compare<">">(jet_hp_, 0.0)), logic_not(gaps_[0].jet_burning))) {
      gaps_[0].jet_burning = true;
      a_2 = "jet";
      n11534370 = matrix_of(jet_actor_);
      a_3 = n11534370.Result;
      n11534371 = gaps_rules_cpp::SmokeField_start(gaps_[0].smoke, a_2, engine_math_math::Mat4_translation(a_3).Result, 2.2, kJetBurnS, now);
      toast_ = "JET BRENNT";
      toast_age_ = 0.0;
    }
    if (math_compare<"<=">(jet_hp_, 0.0)) {
      gaps_[0].jet_burning = true;
      a_4 = "jet";
      n11534372 = gaps_rules_cpp::SmokeField_accelerate(gaps_[0].smoke, a_4, now, now);
      a_5 = "jet";
      gaps_rules_cpp::SmokeField_stop(gaps_[0].smoke, a_5);
    }
  }

  void gaps_camera(Camera& c) {
    Bool both = false;
    Bool collided = false;
    Float the_floor = 0.0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float h = 0.0;
    Float min_h = 0.0;
    Bool bus = false;
    Float lift = 0.0;
    both = logic_and(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0), gaps_[0].riding);
    if (both) {
      n11534382 = gaps_ride_camera(c);
      both = n11534382.Result;
    }
    if (both) {
      return;
    }
    if (logic_or(logic_or(logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(world_placed_)), enum_equal<Mode>(mode_, Mode::jet)), enum_equal<Mode>(mode_, Mode::parachute)), enum_equal<Mode>(mode_, Mode::foot))) {
      return;
    }
    collided = false;
    the_floor = kern::formula([](Float the_result) -> Float { return (the_result + 0.12); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(c.eye).x, vector_break(c.eye).y).Result);
    if (math_compare<"<">(vector_break(c.eye).z, the_floor)) {
      c.target = vector_make(vector_break(c.target).x, vector_break(c.target).y, kern::formula([](Float z, Float the_floor, Float z_2) -> Float { return (z + (the_floor - z_2)); }, vector_break(c.target).z, the_floor, vector_break(c.eye).z));
      c.eye = vector_make(vector_break(c.eye).x, vector_break(c.eye).y, the_floor);
      collided = true;
    }
    gaps_[0].camera_lifted = false;
    if (logic_and(logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0)), logic_not(car_cockpit_))) {
      n11534383 = matrix_of(cars_[car_index_].actor);
      a_2 = n11534383.Result;
      a = engine_math_math::Mat4_translation(a_2).Result;
      h = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c.eye).x, vector_break(a).x, vector_break(c.eye).y, vector_break(a).y);
      min_h = 3.2;
      bus = math_compare<">">(cars_[car_index_].car[0].s_.radius, 0.4);
      if (logic_or(math_compare<"<">(h, kern::formula([](Float min_h, Float bus) -> Float { return ((min_h * ((bus != 0.0) ? 1.75 : 1.0)) * 0.999); }, min_h, kern::to<Float>(bus))), collided)) {
        lift = kern::formula([](Float z, Float min_h, Float h) -> Float { return ((z + 1.25) + ((min_h - kern::math::min(h, min_h)) * 0.75)); }, vector_break(a).z, min_h, h);
        if (math_compare<"<">(vector_break(c.eye).z, lift)) {
          c.eye = vector_make(vector_break(c.eye).x, vector_break(c.eye).y, lift);
        }
        c.target = vector_add(a, Vector{0.0, 0.0, 0.15});
        c.up = Vector{0.0, 0.0, 1.0};
        gaps_[0].camera_lifted = true;
      }
      c.near = kern::formula([](Float near) -> Float { return (near < 0.045 ? 0.045 : (0.08 < near ? 0.08 : near)); }, c.near);
    }
  }

  struct gaps_drone_matrixResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  gaps_drone_matrixResult gaps_drone_matrix() {
    Mat4 raw = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector p = Vector{0.0, 0.0, 0.0};
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    n11534384 = matrix_of(drone_actor_);
    raw = n11534384.Result;
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(enum_equal<Mode>(mode_, Mode::drone))), math_compare<"==">(kern::to<Float>(drone_cam_), 0.0))) {
      if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
        gaps_rules::VisualPose_invalidate(gaps_[0].drone_visual);
      }
      return {raw};
    }
    p = Vector{0.0, 0.0, 0.0};
    q = Quat{0.0, 0.0, 0.0, 1.0};
    n11534385 = state_of(drone_actor_);
    gaps_rules_cpp::VisualPose_update(gaps_[0].drone_visual, p, q, engine_math_math::Mat4_translation(raw).Result, struct_break<BodyState>(n11534385.Result).rotation, cam_anchor_, kern::to<Int>(kern::formula([](Float drone_cam_) -> Float { return ((drone_cam_ == 2.0) ? 2.0 : 1.0); }, kern::to<Float>(drone_cam_))), gaps_[0].frame_dt);
    return {engine_math_math::Mat4_trs(p, q, Vector{1.0, 1.0, 1.0}).Result};
  }

  struct gaps_command_startsResult { Bool Result = false; };
  KERN_PURE gaps_command_startsResult gaps_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  void draw_guided() {
    for (auto it12582923 = flow_for(n12582923, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))); it12582923.next();) {
      people_art_cpp::PeopleArt_projectile(people_art_, guided_[n12582923.index].m.p, guided_[n12582923.index].m.v, 0.06, 0.9, guided_[n12582923.index].m.motor);
    }
  }

  void gaps_lamps_init_make(GapsState_Part& p, const MeshData& mesh, Int r = 0, Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}) {
    Int r_ = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool upsert = false;
    r_ = r;
    engine_renderer_allocate_id(n13631498, r_);
    allocate_id = n13631498.result;
    p.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    engine_upsert(n13631499, r_, p.id, mesh, look, parked.m, List<Float>{});
    upsert = n13631499.result;
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
    engine_primitives_append(n13631500, pole, cylinder, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    pole = n13631500.dst;
    box = engine_primitives_box(Vector{1.5, 0.09, 0.09}, steel);
    engine_primitives_append(n13631501, pole, box, engine_math_math::Mat4_translate(vector_make(0.72, 0.0, kern::formula([](Float kPoleH) -> Float { return (kPoleH - 0.1); }, kPoleH))).Result);
    pole = n13631501.dst;
    box_2 = engine_primitives_box(Vector{0.6, 0.26, 0.14}, head);
    engine_primitives_append(n13631502, pole, box_2, engine_math_math::Mat4_translate(vector_make(kArm, 0.0, kHeadZ)).Result);
    pole = n13631502.dst;
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
    engine_sound_bank_set_pcm(n13631503, bank_, "sc:lampglass", glass, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(glass)))), 1, rate);
    set_pcm = n13631503.result;
    engine_sound_bank_request(bank_, "sc:lampclang");
    engine_sound_bank_set_pcm(n13631504, bank_, "sc:lampclang", clang, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(clang)))), 1, rate);
    set_pcm_2 = n13631504.result;
    gaps_[0].lamp_sounds = true;
  }

  void gaps_lamps_mirror() {
    list_clear(lamps_);
    for (auto it13631507 = flow_for(n13631507, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(gaps_[0].lamps.lamps_))))); it13631507.next();) {
      list_add(lamps_, struct_make<Game_Lamp>(gaps_[0].lamps.lamps_[n13631507.index].foot, gaps_[0].lamps.lamps_[n13631507.index].yaw, gaps_[0].lamps.lamps_[n13631507.index].lane, gaps_[0].lamps.lamps_[n13631507.index].state));
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
    engine_game_world_physics(n13631511, world_);
    physics = n13631511.result;
    w = physics;
    for (auto it13631512 = flow_for(n13631512, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<physics_Body>(gaps_[0].fallen))))); it13631512.next();) {
      body_valid = engine_world_body_valid(w, gaps_[0].fallen[n13631512.index]);
      if (body_valid) {
        engine_world_destroy_body(w, gaps_[0].fallen[n13631512.index]);
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

  void Game_gaps_lamps_draw_show(const GapsState_Part& p) {
    Bool set_visible = false;
    Bool update_instances = false;
    Bool set_visible_2 = false;
    if (logic_or(logic_not(math_compare<"!=">(kern::to<Float>(p.id), 0.0)), logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].renderer), 0.0)))) {
      return;
    }
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(p.transforms)), 0.0)) {
      engine_renderer_set_visible(n13631517, gaps_[0].renderer, p.id, false);
      set_visible = n13631517.result;
      return;
    }
    engine_renderer_update_instances(n13631518, gaps_[0].renderer, p.id, p.transforms, p.tints);
    update_instances = n13631518.result;
    engine_renderer_set_visible(n13631519, gaps_[0].renderer, p.id, true);
    set_visible_2 = n13631519.result;
  }

  void gaps_lamps_draw() {
    Float glow = 0.0;
    Bool skip = false;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector c = Vector{0.0, 0.0, 0.0};
    Float rr = 0.0;
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int w = 0;
    Int physics = 0;
    Bool skip_2 = false;
    Bool body_valid = false;
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0))) {
      return;
    }
    gaps_lamps_draw_clear(gaps_[0].lamp_pole);
    gaps_lamps_draw_clear(gaps_[0].lamp_glow);
    gaps_lamps_draw_clear(gaps_[0].lamp_pool);
    gaps_lamps_draw_clear(gaps_[0].lamp_fallen);
    if (logic_and(world_placed_, show_lamps_)) {
      glow = look::lamp_glow(night_).Result;
      for (auto it13631513 = flow_for(n13631513, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(gaps_[0].lamps.lamps_))))); it13631513.next();) {
        skip = false;
        if (math_compare<"==">(kern::to<Float>(gaps_[0].lamps.lamps_[n13631513.index].state), 2.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_or(math_compare<">">(kern::formula([](Float x, Float x_2) -> Float { return kern::math::abs((x - x_2)); }, vector_break(gaps_[0].lamps.lamps_[n13631513.index].foot).x, vector_break(cam_.eye).x), kern::formula([](Float lamp_m, Float the_result) -> Float { return (lamp_m * kern::math::min(the_result, 3.0)); }, look_.lamp_m, view_zoom().Result)), math_compare<">">(kern::formula([](Float y, Float y_2) -> Float { return kern::math::abs((y - y_2)); }, vector_break(gaps_[0].lamps.lamps_[n13631513.index].foot).y, vector_break(cam_.eye).y), kern::formula([](Float lamp_m, Float the_result) -> Float { return (lamp_m * kern::math::min(the_result, 3.0)); }, look_.lamp_m, view_zoom().Result)))) {
            skip = true;
          }
          if (logic_not(skip)) {
            m = game_lamps_cpp::yaw_at(gaps_[0].lamps.lamps_[n13631513.index].foot, gaps_[0].lamps.lamps_[n13631513.index].yaw).Result;
            gaps_lamps_draw_put(gaps_[0].lamp_pole, m, engine_math_math::Vec4_new(1.0, 1.0, 1.0, 1.0).Result);
            if (logic_and(math_compare<"==">(kern::to<Float>(gaps_[0].lamps.lamps_[n13631513.index].state), 0.0), math_compare<">">(glow, 0.01))) {
              a_2 = engine_math_math::Mat4_translate(vector_make(kArm, 0.0, kern::formula([](Float kHeadZ) -> Float { return (kHeadZ - 0.075); }, kHeadZ))).Result;
              a = engine_math_math::Mat4_mul(m, a_2).Result;
              gaps_lamps_draw_put(gaps_[0].lamp_glow, a, engine_math_math::Vec4_new(glow, glow, glow, 1.0).Result);
              c = vector_make(vector_break(gaps_[0].lamps.lamps_[n13631513.index].head).x, vector_break(gaps_[0].lamps.lamps_[n13631513.index].head).y, kern::formula([](Float the_result) -> Float { return (the_result + 0.14); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(gaps_[0].lamps.lamps_[n13631513.index].head).x, vector_break(gaps_[0].lamps.lamps_[n13631513.index].head).y).Result));
              for (auto it13631514 = flow_for(n13631514, 0, 3); it13631514.next();) {
                rr = kern::formula([](Float index) -> Float { return (13.0 * (1.0 - (index * 0.22))); }, kern::to<Float>(n13631514.index));
                a_4 = engine_math_math::Mat4_translate(vector_add(c, vector_make(0.0, 0.0, kern::formula([](Float index) -> Float { return (index * 0.004); }, kern::to<Float>(n13631514.index))))).Result;
                a_5 = engine_math_math::Mat4_scale(vector_make(rr, rr, 1.0)).Result;
                a_3 = engine_math_math::Mat4_mul(a_4, a_5).Result;
                gaps_lamps_draw_put(gaps_[0].lamp_pool, a_3, engine_math_math::Vec4_new(1.0, 1.0, 1.0, kern::formula([](Float glow) -> Float { return ((glow * 0.55) * 0.22); }, glow)).Result);
              }
            }
          }
        }
      }
      engine_game_world_physics(n13631515, world_);
      physics = n13631515.result;
      w = physics;
      for (auto it13631516 = flow_for(n13631516, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<physics_Body>(gaps_[0].fallen))))); it13631516.next();) {
        skip_2 = false;
        body_valid = engine_world_body_valid(w, gaps_[0].fallen[n13631516.index]);
        if (logic_not(body_valid)) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          s = common::read_body(w, gaps_[0].fallen[n13631516.index]).Result;
          a_7 = engine_math_math::Mat4_trs(s.position, s.rotation, Vector{1.0, 1.0, 1.0}).Result;
          a_8 = engine_math_math::Mat4_translate(vector_make(0.0, 0.0, kern::formula([](Float kPoleH) -> Float { return kern::math::div((-kPoleH), 2.0); }, kPoleH))).Result;
          a_6 = engine_math_math::Mat4_mul(a_7, a_8).Result;
          gaps_lamps_draw_put(gaps_[0].lamp_fallen, a_6, engine_math_math::Vec4_new(1.0, 1.0, 1.0, 1.0).Result);
        }
      }
    }
    Game_gaps_lamps_draw_show(gaps_[0].lamp_pole);
    Game_gaps_lamps_draw_show(gaps_[0].lamp_glow);
    Game_gaps_lamps_draw_show(gaps_[0].lamp_pool);
    Game_gaps_lamps_draw_show(gaps_[0].lamp_fallen);
  }

  void life_draw_people() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Fixed<Int, 5> skins = Fixed<Int, 5>{};
    Bool skip = false;
    People_Person p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    Int value = 0;
    Int value_2 = 0;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0))) {
      return;
    }
    if (logic_not(life::Life_has_chain(glife_[0].life).Result)) {
      return;
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    skins = list_make<Int, "5">(14725260, 12881516, 11038802, 8079923, 13804158);
    for (auto it14680074 = flow_for(n14680074, 0, 9); it14680074.next();) {
      skip = false;
      if (logic_or(logic_not(glife_[0].life.chain_.runners[n14680074.index].alive), math_compare<">">(vector_length(vector_sub(glife_[0].life.chain_.runners[n14680074.index].pos, cam_.eye)), 220.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
        p.pos = glife_[0].life.chain_.runners[n14680074.index].pos;
        p.yaw = glife_[0].life.chain_.runners[n14680074.index].yaw_s;
        p.v = 2.9;
        p.phase = glife_[0].life.chain_.runners[n14680074.index].phase;
        value = 14221114;
        p.shirt = vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value)));
        value_2 = 1845818;
        p.pants = vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value_2)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value_2)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value_2)));
        p.skin = vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(skins[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::mod(index, 5.0); }, kern::to<Float>(n14680074.index)))])), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(skins[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::mod(index, 5.0); }, kern::to<Float>(n14680074.index)))])), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(skins[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::mod(index, 5.0); }, kern::to<Float>(n14680074.index)))])));
        people_art_cpp::PeopleArt_person(people_art_, p, now);
      }
    }
  }

  struct life_hides_personResult { Bool Result = false; };
  KERN_PURE life_hides_personResult life_hides_person(Int index = 0) {
    return {logic_and(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0), ragdoll_cpp::Ragdolls_has(glife_[0].life.ragdolls_, int64_add(int64_of_int(index), Int64{0x1})).Result)};
  }

  struct life_shotResult { Bool Result = false; };
  life_shotResult life_shot(Vector& to, Vector from = Vector{0.0, 0.0, 0.0}, Float damage = 0.0) {
    Float now = 0.0;
    Float sim_time = 0.0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)), logic_not(glife_[0].ready))) {
      return {false};
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    life_cpp::Life_heard_shot(glife_[0].life, from, to, now);
    n14680075 = life_cpp::Life_shot(glife_[0].life, to, glife_[0].out, from, damage, now, false);
    return {n14680075.Result};
  }

  struct life_punchResult { Bool Result = false; };
  life_punchResult life_punch(Vector& to, Vector from = Vector{0.0, 0.0, 0.0}) {
    Float sim_time = 0.0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)), logic_not(glife_[0].ready))) {
      return {false};
    }
    sim_time = engine_game_world_sim_time(world_);
    n14680077 = life_cpp::Life_shot(glife_[0].life, to, glife_[0].out, from, 6.0, sim_time, true);
    return {n14680077.Result};
  }

  void life_net_fx(const net_Fx& f) {
    Float sim_time = 0.0;
    Vector dv = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    Bool set_actor_velocity = false;
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)), logic_not(glife_[0].ready)), logic_not(math_compare<"!=">(kern::to<Float>(world_), 0.0)))) {
      return;
    }
    if (math_compare<"==">(kern::to<Float>(f.kind), kern::to<Float>(enum_index<net_Fx_Kind>(net_Fx_Kind::cow_death)))) {
      sim_time = engine_game_world_sim_time(world_);
      life_cpp::Life_remote_cow_death(glife_[0].life, glife_[0].out, f.p, vector_scale(f.dir, 600.0), sim_time);
    } else if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(f.kind), kern::to<Float>(enum_index<net_Fx_Kind>(net_Fx_Kind::push))), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::trunc((x + 0.5)); }, vector_break(f.dir).x)))), kern::to<Float>(net_session::Session_self_slot(net_).Result))), enum_equal<Mode>(mode_, Mode::foot))) {
      dv = vector_make(kern::formula([](Float x) -> Float { return (x < (-40.0) ? (-40.0) : (40.0 < x ? 40.0 : x)); }, vector_break(f.p).x), kern::formula([](Float y) -> Float { return (y < (-40.0) ? (-40.0) : (40.0 < y ? 40.0 : y)); }, vector_break(f.p).y), kern::formula([](Float z) -> Float { return (z < (-40.0) ? (-40.0) : (40.0 < z ? 40.0 : z)); }, vector_break(f.p).z));
      the_actor_velocity = engine_game_world_actor_velocity(world_, soldier_);
      engine_game_world_set_actor_velocity(n14680078, world_, soldier_, vector_add(the_actor_velocity, dv));
      set_actor_velocity = n14680078.result;
      if (math_compare<">">(f.a, 0.0)) {
        n14680079 = player_position();
        engine_game_world_damage(world_, soldier_, kern::formula([](Float a) -> Float { return kern::math::min(100.0, a); }, f.a), 0, false, vector_sub(n14680079.Result, dv));
      }
    }
  }

  void life_hud(Text& s) {
    Float now = 0.0;
    Float sim_time = 0.0;
    Int dead = 0;
    Int air = 0;
    Int birds = 0;
    Int pigeons = 0;
    Int joggers = 0;
    Text b = "";
    Bool first = false;
    Float aspect = 0.0;
    Mat4 vp = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip = false;
    Vec4 c = Vec4{0.0, 0.0, 0.0, 0.0};
    Float x = 0.0;
    Float y = 0.0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)), logic_not(glife_[0].ready))) {
      return;
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    dead = 0;
    air = 0;
    birds = 0;
    for (auto it14680080 = flow_for(n14680080, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_))))); it14680080.next();) {
      dead += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(glife_[0].life.cows_[n14680080.index].dead)));
    }
    for (auto it14680081 = flow_for(n14680081, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it14680081.next();) {
      air += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(glife_[0].life.flocks_[n14680081.index].air)));
    }
    pigeons = 0;
    for (auto it14680082 = flow_for(n14680082, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it14680082.next();) {
      pigeons += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Pigeon>(glife_[0].life.flocks_[n14680082.index].birds))));
    }
    for (auto it14680083 = flow_for(n14680083, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Bird>(glife_[0].life.birds_))))); it14680083.next();) {
      birds += kern::to<Int>(kern::formula([](Float now, Float a) -> Float { return kern::math::wrap32(((now >= a) ? 1.0 : 0.0)); }, now, glife_[0].life.birds_[n14680083.index].dead_until));
    }
    joggers = 0;
    if (life::Life_has_chain(glife_[0].life).Result) {
      for (auto it14680084 = flow_for(n14680084, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Runner>(glife_[0].life.chain_.runners))))); it14680084.next();) {
        joggers += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(glife_[0].life.chain_.runners[n14680084.index].alive)));
      }
    }
    b = "";
    b = text_format<",\"life\":{\"cows\":%u,\"cowsDead\":%u,\"cowHits\":%u,\"flocks\":%u,\"flocksAir\":%u,\"pigeons\":%u,\"birds\":%u,\"birdHits\":%u,\"trees\":%u,\"joggers\":%u,\"joggerStreak\":%u,\"ragdolls\":%u,\"ragdollSpawns\":%u,\"falls\":%u,\"getups\":%u,\"shouts\":%u,\"honks\":%u">(639, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_)))), dead, life::Life_cow_hits(glife_[0].life).Result, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_)))), air, pigeons, birds, life::Life_bird_hits(glife_[0].life).Result, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Tree>(glife_[0].life.trees_)))), joggers, life::Life_jogger_streak(glife_[0].life).Result, ragdoll_cpp::Ragdolls_active(glife_[0].life.ragdolls_).Result, ragdoll::Ragdolls_spawned(glife_[0].life.ragdolls_).Result, glife_[0].falls, glife_[0].getups, life::Life_shouts(glife_[0].life).Result, life::Life_honks(glife_[0].life).Result);
    s = text_append(s, b, "");
    if (math_compare<"<">(now, glife_[0].banner_until)) {
      s = text_append(s, text_append(text_append(",\"banner\":\"", game_life_cpp::json_escape(glife_[0].banner).Result, ""), "\"", ""), "");
    }
    s = text_append(s, ",\"say\":[", "");
    for (auto it14680085 = flow_for(n14680085, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<LifeOut_Say>(glife_[0].speech))))); it14680085.next();) {
      b = text_format<"%s[\"%s\",%.2f,%.2f,%.2f,%.2f]">(639, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n14680085.index), 0.0), ",", ""), game_life_cpp::json_escape(glife_[0].speech[n14680085.index].text).Result, glife_[0].speech[n14680085.index].pitch, glife_[0].speech[n14680085.index].rate, glife_[0].speech[n14680085.index].volume, glife_[0].speech[n14680085.index].delay);
      s = text_append(s, b, "");
    }
    list_clear(glife_[0].speech);
    s = text_append(s, "],\"bubbles\":[", "");
    first = true;
    aspect = kern::formula([](Float frame_h_, Float frame_w_) -> Float { return ((frame_h_ > 0.0) ? kern::math::div(frame_w_, frame_h_) : kern::math::div(16.0, 9.0)); }, frame_h_, frame_w_);
    a = engine_math_math::projection_matrix(cam_, aspect).Result;
    n14680086 = engine_math_math::Mat4_look_at_2(cam_.eye, cam_.target, cam_.up);
    a_2 = n14680086.Result;
    vp = engine_math_math::Mat4_mul(a, a_2).Result;
    for (auto it14680087 = flow_for(n14680087, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Voice>(glife_[0].life.voices_))))); it14680087.next();) {
      skip = false;
      c = engine_math_math::Mat4_transform(vp, engine_math_math::Vec4_new(vector_break(glife_[0].life.voices_[n14680087.index].at).x, vector_break(glife_[0].life.voices_[n14680087.index].at).y, vector_break(glife_[0].life.voices_[n14680087.index].at).z, 1.0).Result).Result;
      if (math_compare<"<=">(c.w, 0.1)) {
        skip = true;
      }
      if (logic_not(skip)) {
        x = kern::formula([](Float x, Float w) -> Float { return kern::math::div(x, w); }, c.x, c.w);
        y = kern::formula([](Float y, Float w) -> Float { return kern::math::div(y, w); }, c.y, c.w);
        if (logic_or(math_compare<">">(kern::formula([](Float x) -> Float { return kern::math::abs(x); }, x), 1.1), math_compare<">">(kern::formula([](Float y) -> Float { return kern::math::abs(y); }, y), 1.1))) {
          skip = true;
        }
        if (logic_not(skip)) {
          b = text_format<"%s[%.4f,%.4f,\"%s\",\"%s\",%s]">(639, logic_select<Text>(first, "", ","), kern::formula([](Float x) -> Float { return kern::math::div((x + 1.0), 2.0); }, x), kern::formula([](Float y) -> Float { return kern::math::div((1.0 - y), 2.0); }, y), game_life_cpp::json_escape(glife_[0].life.voices_[n14680087.index].text).Result, glife_[0].life.voices_[n14680087.index].mood, logic_select<Text>(math_compare<"<">(kern::formula([](Float a, Float now) -> Float { return (a - now); }, glife_[0].life.voices_[n14680087.index].until, now), 0.35), "true", "false"));
          s = text_append(s, b, "");
          first = false;
        }
      }
    }
    s = text_append(s, "]", "");
    interiors_cpp::Interiors_hud(glife_[0].interiors, s);
    s = text_append(s, "}", "");
  }

  struct life_command_startsResult { Bool Result = false; };
  KERN_PURE life_command_startsResult life_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  void life_draw_ragdolls() {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0))) {
      return;
    }
    list_clear(life_draw_ragdolls_bones);
    ragdoll_cpp::Ragdolls_draw(glife_[0].life.ragdolls_, life_draw_ragdolls_bones);
    people_art_cpp::PeopleArt_ragdolls(people_art_, life_draw_ragdolls_bones, glife_[0].life.ragdolls_);
  }

  void look_view_models_make(const MeshData& d, Int& id, Int r = 0, Int n15728651_index = 0, Bool g = false) {
    Int r_ = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    r_ = r;
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(d.positions)), 0.0)) {
      return;
    }
    engine_renderer_allocate_id(n15728653, r_);
    allocate_id = n15728653.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = game_look_cpp::vm_material(g, math_compare<"==">(kern::to<Float>(n15728651_index), 1.0)).Result;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    engine_upsert(n15728654, r_, id, d, look, List<Float>{}, List<Float>{});
    upsert = n15728654.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, id);
      id = 0;
    }
  }

  struct look_command_startsResult { Bool Result = false; };
  KERN_PURE look_command_startsResult look_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
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
    n16777226 = matrix_of(a);
    a_2 = n16777226.Result;
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

  struct step_soldier_matchResult { Bool Result = false; };
  KERN_PURE step_soldier_matchResult step_soldier_match(Float now = 0.0, Float combat_kBurstWindowS_2 = 0.0, Float t = 0.0) {
    return {math_compare<">=">(kern::formula([](Float now, Float t) -> Float { return (now - t); }, now, t), combat_kBurstWindowS_2)};
  }

  void mp_local_ready() {
    Int renderer = 0;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<MpLocal>(mp_)), 0.0))) {
      mp_ = list_make<MpLocal, "1">(MpLocal{MpArt{0, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}}, List<Float>{}, false, mp_PingBoard{List<mp_Ping>{}}, List<MpLocal_Tracer>{}, List<MpLocal_Flash>{}, List<MpLocal_Feed>{}, -1000000000000.0, -1000000000000.0, -1000000000000.0, 0.0, -1000000000000.0, false, false, 0, Vector{0.0, 0.0, 0.0}, 254, 0, 0, -1.0, false, Vector{0.0, 0.0, 0.0}, mp_VoiceCapture{List<Float>{}, List<Int_row>{}, 0.0}, false, false, false, 0, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, "", List<Vector>{}, false, -1000000000000.0, 0, "", 0, false, false});
    }
    if (logic_and(logic_not(mp_art::MpArt_ready(mp_[0].art).Result), math_compare<"!=">(kern::to<Float>(engine_), 0.0))) {
      renderer = engine_engine_renderer(engine_);
      n17825802 = mp_art_cpp::MpArt_init(mp_[0].art, renderer);
    }
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
    for (auto it17825803 = flow_for(n17825803, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Opt>(list))))); it17825803.next();) {
      w = list[n17825803.index];
      if (enum_equal<Opt>(w, o)) {
        return {true};
      }
    }
    return {false};
  }

  struct world_rules_jsonResult { Text Result = ""; };
  KERN_PURE world_rules_jsonResult world_rules_json() {
    Text o = "";
    Bool first = false;
    List<Opt> list = List<Opt>{};
    Opt w = Opt::mouse_sensitivity;
    Text key = "";
    o = logic_select<Text>(logic_and(net_session::Session_active(net_).Result, logic_not(net_session::Session_hosting(net_).Result)), ",\"worldLocked\":true,\"worldRules\":{", ",\"worldLocked\":false,\"worldRules\":{");
    first = true;
    list = kWorldRules;
    for (auto it17825805 = flow_for(n17825805, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Opt>(list))))); it17825805.next();) {
      w = list[n17825805.index];
      key = struct_break<OptSpec>(settings_cpp::opt_spec(ui_.settings_table, w).Result).key;
      o = text_append(o, logic_select<Text>(first, "\"", ",\""), "");
      for (auto it17825806 = flow_for(n17825806, 6, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(text_length(key))))); it17825806.next();) {
        o = text_append(o, text_of_code(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(key, n17825806.index))))), "");
      }
      o = text_append(o, logic_select<Text>(settings::Settings_on(ui_.settings, w).Result, "\":true", "\":false"), "");
      first = false;
    }
    return {text_append(o, "}", "")};
  }

  void mp_input(Int e = 0, Float dt = 0.0) {
    mp_local_ready();
    if (mp_[0].map_open) {
      pad.rt = 0.0;
      pad.lt = pad.rt;
      pad.ry = pad.lt;
      pad.rx = pad.ry;
      pad.ly = pad.rx;
      pad.lx = pad.ly;
      pad.pressed = 0;
      pad.buttons = pad.pressed;
    }
  }

  void voice_capture(const List<Float>& samples, Int rate = 0) {
    mp_local_ready();
    if (logic_and(mp_[0].mic, logic_not(mp_[0].muted))) {
      mp_rules_cpp::VoiceCapture_push(mp_[0].capture, samples, rate);
    }
  }

  void mix_voice(List<Float>& out, Int frames = 0) {
    Bool ready = false;
    Bool ready_2 = false;
    Bool either = false;
    Int sample_rate = 0;
    either = logic_or(logic_not(net_session::Session_active(net_).Result), logic_not(math_compare<">">(kern::to<Float>(list_length<MpLocal>(mp_)), 0.0)));
    if (logic_not(either)) {
      ready_2 = engine_mixer_ready(mixer_);
      either = logic_not(ready_2);
    }
    if (either) {
      return;
    }
    for (auto it17825811 = flow_for(n17825811, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825811.next();) {
      sample_rate = engine_mixer_sample_rate(mixer_);
      n17825812 = mp_rules_cpp::VoiceStream_render(net_.remotes_[n17825811.index].voice, out, frames, sample_rate);
    }
  }

  struct remote_on_segment_zonesResult { Bool Result = false; };
  remote_on_segment_zonesResult remote_on_segment_zones(Float& t_hit, mp_Zone& z, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float speed = 0.0) {
    BodyPart part = BodyPart::none;
    n17825813 = person_hit::hit(t_hit, feet, yaw, speed, from, seg, 1.16, 1.0);
    part = n17825813.Result;
    z = logic_select<mp_Zone>(enum_equal<BodyPart>(part, BodyPart::head), mp_Zone::head, logic_select<mp_Zone>(enum_equal<BodyPart>(part, BodyPart::legs), mp_Zone::legs, mp_Zone::torso));
    return {logic_not(enum_equal<BodyPart>(part, BodyPart::none))};
  }

  struct remote_on_segmentResult { Int Result = 0; };
  /// The mate closest along a segment (his body as it is drawn, a sphere around his vehicle): -1 for none; `t_out` the share of the segment where it touches him. Rounds, blows, grenades and rockets all ask this - a mate's body stops them on every machine as a passer-by's does.
  remote_on_segmentResult remote_on_segment(Float& t_out, mp_Zone& zone_out, Vector from = Vector{0.0, 0.0, 0.0}, Vector to = Vector{0.0, 0.0, 0.0}) {
    Vector seg = Vector{0.0, 0.0, 0.0};
    Float l2 = 0.0;
    Int best = 0;
    Float best_t = 0.0;
    mp_Zone best_zone = mp_Zone::torso;
    Bool skip = false;
    Float t = 0.0;
    mp_Zone z = mp_Zone::torso;
    Vector feet = Vector{0.0, 0.0, 0.0};
    Float t_2 = 0.0;
    mp_Zone z_2 = mp_Zone::torso;
    Float t_3 = 0.0;
    mp_Zone z_3 = mp_Zone::torso;
    Bool person = false;
    Vector c = Vector{0.0, 0.0, 0.0};
    Float radius = 0.0;
    Float t_4 = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Float d = 0.0;
    Vector q = Vector{0.0, 0.0, 0.0};
    t_out = 2.0;
    zone_out = mp_Zone::torso;
    if (logic_not(net_session::Session_active(net_).Result)) {
      return {-1};
    }
    seg = vector_sub(to, from);
    l2 = kern::formula([](Float a) -> Float { return kern::math::max(1e-06, a); }, vector_dot(seg, seg));
    best = -1;
    best_t = 2.0;
    best_zone = mp_Zone::torso;
    for (auto it17825814 = flow_for(n17825814, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825814.next();) {
      skip = false;
      if (math_compare<">">(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return (kern::math::band(a, kern::math::bor(a_2, a_3)) != 0.0); }, kern::to<Float>(net_.remotes_[n17825814.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead))), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_and(gaps_riding().Result, math_compare<"==">(kern::to<Float>(net_.remotes_[n17825814.index].state.slot), kern::to<Float>(gaps_[0].ride_owner)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (enum_equal<net_PlayerMode>(net_.remotes_[n17825814.index].state.mode, net_PlayerMode::foot)) {
            t = 2.0;
            z = mp_Zone::torso;
            feet = logic_select<Vector>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825814.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated)))), 0.5), vector_sub(net_.remotes_[n17825814.index].person_shown, vector_make(0.0, 0.0, kSeatDrop)), net_.remotes_[n17825814.index].person_shown);
            n17825815 = remote_on_segment_zones(t, z, from, seg, feet, net_.remotes_[n17825814.index].state.avatar_yaw, net_.remotes_[n17825814.index].state.speed);
            if (logic_and(n17825815.Result, math_compare<"<">(t, best_t))) {
              best_t = t;
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825814.index)));
              best_zone = z;
            }
            skip = true;
          }
          if (logic_not(skip)) {
            if (enum_equal<net_PlayerMode>(net_.remotes_[n17825814.index].state.mode, net_PlayerMode::drone)) {
              t_2 = 2.0;
              z_2 = mp_Zone::torso;
              n17825816 = remote_on_segment_zones(t_2, z_2, from, seg, net_.remotes_[n17825814.index].person_shown, net_.remotes_[n17825814.index].state.avatar_yaw, 0.0);
              if (logic_and(n17825816.Result, math_compare<"<">(t_2, best_t))) {
                best_t = t_2;
                best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825814.index)));
                best_zone = z_2;
              }
            }
            if (enum_equal<net_PlayerMode>(net_.remotes_[n17825814.index].state.mode, net_PlayerMode::parachute)) {
              t_3 = 2.0;
              z_3 = mp_Zone::torso;
              n17825817 = remote_on_segment_zones(t_3, z_3, from, seg, vector_sub(net_.remotes_[n17825814.index].shown, Vector{0.0, 0.0, 1.75}), net_.remotes_[n17825814.index].state.yaw, 0.0);
              if (logic_and(n17825817.Result, math_compare<"<">(t_3, best_t))) {
                best_t = t_3;
                best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825814.index)));
                best_zone = z_3;
              }
            }
            person = enum_equal<net_PlayerMode>(net_.remotes_[n17825814.index].state.mode, net_PlayerMode::foot);
            c = logic_select<Vector>(person, vector_add(net_.remotes_[n17825814.index].person_shown, vector_make(0.0, 0.0, kern::formula([](Float a, Float a_2, Float kSeatDrop) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? (0.73 - kSeatDrop) : 0.73); }, kern::to<Float>(net_.remotes_[n17825814.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated))), kSeatDrop))), net_.remotes_[n17825814.index].shown);
            radius = 0.522;
            switch (flow_switch_enum<net_PlayerMode>(net_.remotes_[n17825814.index].state.mode)) {
              case net_PlayerMode::foot: {
                c = vector_add(c, Vector{0.0, 0.0, 0.2});
              } break;
              case net_PlayerMode::drone: {
                radius = 0.3;
              } break;
              case net_PlayerMode::car: {
                radius = kern::formula([](Float a, Float a_2) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? 3.2 : 2.0); }, kern::to<Float>(net_.remotes_[n17825814.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kBus))));
              } break;
              case net_PlayerMode::jet: {
                radius = 5.5;
              } break;
              case net_PlayerMode::parachute: {
                radius = 0.6;
              } break;
            }
            t_4 = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a, Float l2) -> Float { return kern::math::div(a, l2); }, vector_dot(vector_sub(c, from), seg), l2), 0.0, 1.0));
            p = vector_add(from, vector_scale(seg, t_4));
            d = vector_length(vector_sub(p, c));
            if (person) {
              q = vector_make(vector_break(c).x, vector_break(c).y, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, vector_break(p).z, kern::formula([](Float z) -> Float { return (z - 0.93); }, vector_break(c).z), kern::formula([](Float z) -> Float { return (z + 0.9); }, vector_break(c).z))));
              d = vector_length(vector_sub(p, q));
            }
            if (logic_and(math_compare<"<">(d, radius), math_compare<"<">(t_4, best_t))) {
              best_t = t_4;
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825814.index)));
              best_zone = mp_Zone::torso;
            }
          }
        }
      }
    }
    t_out = best_t;
    zone_out = best_zone;
    return {best};
  }

  /// jet_mode.mjs makeChute: under the canopy hangs the player's own character, both arms up on the risers, legs a little apart - ours (seen in the third-person camera) and every mate's
  void Game_draw_remote_people_pilot(Float now = 0.0, Vector harness = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Vector shirt = Vector{0.0, 0.0, 0.0}) {
    People_Person p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    p.kind = People_Kind::citizen;
    p.shirt = shirt;
    p.pants = Vector{0.12, 0.13, 0.16};
    p.skin = Vector{0.86, 0.66, 0.5};
    p.pos = vector_sub(harness, Vector{0.0, 0.0, 0.9});
    p.yaw = yaw;
    p.v = 0.0;
    p.hands_up_until = kern::formula([](Float now) -> Float { return (now + 1.0); }, now);
    people_art_cpp::PeopleArt_person(people_art_, p, now);
  }

  void update_nav() {
    Vector p3 = Vector{0.0, 0.0, 0.0};
    Vector me = Vector{0.0, 0.0, 0.0};
    Float now = 0.0;
    Bool routable = false;
    List<Vector> path = List<Vector>{};
    Float lon = 0.0;
    Float lat = 0.0;
    Text b = "";
    if (logic_not(mp_[0].has_dest)) {
      return;
    }
    n17825820 = focus_position();
    p3 = n17825820.Result;
    me = vector_make(vector_break(p3).x, vector_break(p3).y, 0.0);
    if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(mp_[0].dest).x, vector_break(me).x, vector_break(mp_[0].dest).y, vector_break(me).y), kArriveM)) {
      mp_[0].has_dest = false;
      list_clear(mp_[0].route);
      toast_ = text_append("Ziel erreicht: ", mp_[0].dest_name, "");
      toast_age_ = 0.0;
      return;
    }
    now = kern::formula([](Float time_) -> Float { return (time_ * 1000.0); }, time_);
    routable = logic_or(enum_equal<Mode>(mode_, Mode::foot), enum_equal<Mode>(mode_, Mode::car));
    if (logic_or(logic_not(routable), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, mp_[0].route_at), kNavRerouteMs))) {
      return;
    }
    mp_[0].route_at = now;
    if (logic_not(int64_equal(roads_.built_from, road_graph_cpp::lane_signature(city_.lanes_).Result))) {
      road_graph_cpp::RoadGraph_rebuild(roads_, city_.lanes_);
    }
    path = List<Vector>{};
    n17825821 = road_graph_cpp::RoadGraph_path(roads_, path, me, mp_[0].dest);
    if (logic_and(logic_and(n17825821.Result, math_compare<">=">(kern::to<Float>(list_length<Vector>(path)), 2.0)), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(path[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(path))))]).x, vector_break(mp_[0].dest).x, vector_break(path[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(path))))]).y, vector_break(mp_[0].dest).y), 60.0))) {
      list_add(path, mp_[0].dest);
      mp_[0].route = path;
      mp_[0].route_streets = true;
      return;
    }
    if (logic_not(city_stream::CityStream_has_real_data(city_).Result)) {
      mp_[0].route = list_make<Vector, "2">(me, mp_[0].dest);
      mp_[0].route_streets = false;
      return;
    }
    lon = 0.0;
    lat = 0.0;
    tiles_cpp::unproject(city_.proj_, lon, lat, vector_break(me).x, vector_break(me).y);
    b = "";
    mp_[0].fetch_id = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(mp_[0].fetch_id)));
    b = text_format<"{\"id\":%u,\"from\":[%.6f,%.6f],\"to\":[%.6f,%.6f],\"profile\":\"%s\"}">(199, mp_[0].fetch_id, lon, lat, mp_[0].dest_lon, mp_[0].dest_lat, logic_select<Text>(enum_equal<Mode>(mode_, Mode::foot), "foot", "car"));
    mp_[0].fetch_json = b;
    if (math_compare<"==">(kern::to<Float>(list_length<Vector>(mp_[0].route)), 0.0)) {
      mp_[0].route = list_make<Vector, "2">(me, mp_[0].dest);
    }
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

  struct net_command_startsResult { Bool Result = false; };
  KERN_PURE net_command_startsResult net_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  void projsync_ready() {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<ProjSync>(projsync_)), 0.0))) {
      projsync_ = list_make<ProjSync, "1">(ProjSync{Map<Int, ProjSync_Ghost>{}, List<Int_Int_pair>{}, 0.0, 0, 0});
    }
  }

  void projsync_blast(Int owner = 0, Vector at = Vector{0.0, 0.0, 0.0}) {
    Int key = 0;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<ProjSync>(projsync_)), 0.0))) {
      return;
    }
    for (auto it18874378 = flow_for(n18874378, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it18874378.next();) {
      if (logic_and(logic_and(logic_and(projectiles_[n18874378.index].ghost, projectiles_[n18874378.index].alive), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::shr(a, 16.0); }, kern::to<Float>(projectiles_[n18874378.index].net))))), kern::to<Float>(owner))), math_compare<"<">(vector_length(vector_sub(projectiles_[n18874378.index].pos, at)), 8.0))) {
        projectiles_[n18874378.index].alive = false;
        key = projectiles_[n18874378.index].net;
        map_remove(n18874379, projsync_[0].ghosts, key);
      }
    }
  }

  struct gaps_ridingResult { Bool Result = false; };
  KERN_PURE gaps_ridingResult gaps_riding() {
    return {logic_and(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0), gaps_[0].riding)};
  }

  struct gaps_ride_carResult { Bool Result = false; };
  KERN_PURE gaps_ride_carResult gaps_ride_car() {
    return {logic_and(logic_and(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0), gaps_[0].riding), gaps_[0].ride_car)};
  }

  struct gaps_ride_poseResult { Bool Result = false; };
  gaps_ride_poseResult gaps_ride_pose(Vector& p, Quat& q) {
    Int at = 0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(gaps_[0].riding))) {
      return {false};
    }
    at = net_session_cpp::Session_remote_index(net_, gaps_[0].ride_owner).Result;
    if (math_compare<"<">(kern::to<Float>(at), 0.0)) {
      return {false};
    }
    if (logic_or(enum_equal<net_PlayerMode>(net_.remotes_[at].state.mode, net_PlayerMode::jet), enum_equal<net_PlayerMode>(net_.remotes_[at].state.mode, net_PlayerMode::car))) {
      p = net_.remotes_[at].shown;
      q = net_.remotes_[at].shown_rotation;
      return {true};
    }
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[at].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetOut)))), 0.5)) {
      p = net_.remotes_[at].state.jet_pos;
      q = net_.remotes_[at].state.jet_rot;
      return {true};
    }
    return {false};
  }

  struct gaps_ride_cameraResult { Bool Result = false; };
  gaps_ride_cameraResult gaps_ride_camera(Camera& c) {
    Vector p = Vector{0.0, 0.0, 0.0};
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Int owner_at = 0;
    if (logic_and(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0), gaps_[0].ride_car)) {
      return {false};
    }
    p = Vector{0.0, 0.0, 0.0};
    q = Quat{0.0, 0.0, 0.0, 1.0};
    n19922957 = gaps_ride_pose(p, q);
    if (logic_not(n19922957.Result)) {
      return {false};
    }
    owner_at = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? a_2 : (-1.0)); }, kern::to<Float>(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), kern::to<Float>(net_session_cpp::Session_remote_index(net_, gaps_[0].ride_owner).Result)));
    if (logic_and(math_compare<">=">(kern::to<Float>(owner_at), 0.0), enum_equal<net_PlayerMode>(net_.remotes_[owner_at].state.mode, net_PlayerMode::car))) {
      c.eye = vector_add(p, engine_math_math::Quat_rotate(q, Vector{0.04, -0.36, 0.7}).Result);
      c.target = vector_add(c.eye, vector_scale(engine_math_math::Quat_rotate(q, Vector{1.0, 0.0, 0.0}).Result, 10.0));
      c.up = engine_math_math::Quat_rotate(q, Vector{0.0, 0.0, 1.0}).Result;
      c.fov = kern::formula([](Float kDeg) -> Float { return (80.0 * kDeg); }, kDeg);
      c.near = 0.04;
      c.far = 2400.0;
      return {true};
    }
    c.eye = vector_add(p, engine_math_math::Quat_rotate(q, Vector{0.48, 2.5, 1.25}).Result);
    c.target = vector_add(c.eye, vector_scale(engine_math_math::Quat_rotate(q, Vector{0.0, 1.0, 0.0}).Result, 10.0));
    c.up = engine_math_math::Quat_rotate(q, Vector{0.0, 0.0, 1.0}).Result;
    c.fov = kern::formula([](Float kDeg) -> Float { return (72.0 * kDeg); }, kDeg);
    c.near = 0.5;
    c.far = 6000.0;
    return {true};
  }

  void swarm_clear() {
    for (auto it20971530 = flow_for(n20971530, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Game_RemoteSwarm>(swarms_))))); it20971530.next();) {
      for (auto it20971531 = flow_for(n20971531, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_RemoteUnit>(swarms_[n20971530.index].units))))); it20971531.next();) {
        if (math_compare<">=">(kern::to<Float>(swarms_[n20971530.index].units[n20971531.index].person), 0.0)) {
          people_.people_[swarms_[n20971530.index].units[n20971531.index].person].state = People_State::gone;
        }
      }
    }
    map_clear(swarms_);
    list_clear(swarm_sent_);
    list_clear(swarm_fallen_);
  }

  struct Game_swarm_collect_next_idResult { Int Result = 0; };
  Game_swarm_collect_next_idResult Game_swarm_collect_next_id() {
    swarm_serial_ = kern::to<Int>(kern::formula([](Float swarm_serial_) -> Float { return kern::math::wrap32(swarm_serial_ + 1.0); }, kern::to<Float>(swarm_serial_)));
    if (math_compare<"==">(kern::to<Float>(swarm_serial_), 0.0)) {
      swarm_serial_ = kern::to<Int>(kern::formula([](Float swarm_serial_) -> Float { return kern::math::wrap32(swarm_serial_ + 1.0); }, kern::to<Float>(swarm_serial_)));
    }
    return {swarm_serial_};
  }

  struct swarm_collect_matchResult { Bool Result = false; };
  KERN_PURE swarm_collect_matchResult swarm_collect_match(const Game_SwarmSent& swarm_sent_n20971548_index, const Game_SwarmSent& s) {
    return {logic_and(math_compare<"==">(kern::to<Float>(s.unit.id), kern::to<Float>(swarm_sent_n20971548_index.unit.id)), enum_equal<net_SwarmKind>(s.unit.kind, swarm_sent_n20971548_index.unit.kind))};
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
    for (auto it20971532 = flow_for(n20971532, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<ZombieAi>(combat_.zombie_ai))))); it20971532.next();) {
      skip = false;
      if (math_compare<"<">(kern::to<Float>(combat_.zombie_ai[n20971532.index].person), 0.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.zombie_ai[n20971532.index].id), 0.0))) {
          n20971533 = Game_swarm_collect_next_id();
          combat_.zombie_ai[n20971532.index].id = n20971533.Result;
        }
        list_add(out, struct_make<net_SwarmUnit>(combat_.zombie_ai[n20971532.index].id, net_SwarmKind::zombie, kern::to<Int>(kern::formula([](Float st, Float a) -> Float { return kern::math::band((((st - a) < 0.6) ? 1.0 : 0.0), 255.0); }, st, combat_.zombie_ai[n20971532.index].swing)), people_.people_[combat_.zombie_ai[n20971532.index].person].pos, common::wrap_pi(people_.people_[combat_.zombie_ai[n20971532.index].person].yaw).Result, people_.people_[combat_.zombie_ai[n20971532.index].person].v));
      }
    }
    for (auto it20971534 = flow_for(n20971534, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Heli>(combat_.helis))))); it20971534.next();) {
      skip_2 = false;
      if (combat_.helis[n20971534.index].dead) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.helis[n20971534.index].id), 0.0))) {
          n20971535 = Game_swarm_collect_next_id();
          combat_.helis[n20971534.index].id = n20971535.Result;
        }
        list_add(out, struct_make<net_SwarmUnit>(combat_.helis[n20971534.index].id, net_SwarmKind::heli, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(((a != 0.0) ? 1.0 : 0.0), 255.0); }, kern::to<Float>(enum_equal<Heli_State>(combat_.helis[n20971534.index].state, Heli_State::down)))), combat_.helis[n20971534.index].p, common::wrap_pi(combat_.helis[n20971534.index].yaw).Result, 0.0));
      }
    }
    for (auto it20971536 = flow_for(n20971536, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Tank>(combat_.tanks))))); it20971536.next();) {
      skip_3 = false;
      if (combat_.tanks[n20971536.index].dead) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.tanks[n20971536.index].id), 0.0))) {
          n20971537 = Game_swarm_collect_next_id();
          combat_.tanks[n20971536.index].id = n20971537.Result;
        }
        list_add(out, struct_make<net_SwarmUnit>(combat_.tanks[n20971536.index].id, net_SwarmKind::tank, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(((a != 0.0) ? 1.0 : 0.0), 255.0); }, kern::to<Float>(combat_.tanks[n20971536.index].wreck))), combat_.tanks[n20971536.index].p, common::wrap_pi(combat_.tanks[n20971536.index].yaw).Result, common::wrap_pi(combat_.tanks[n20971536.index].turret).Result));
      }
    }
    for (auto it20971538 = flow_for(n20971538, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<SoldierAi>(combat_.soldiers))))); it20971538.next();) {
      skip_4 = false;
      if (math_compare<"<">(kern::to<Float>(combat_.soldiers[n20971538.index].person), 0.0)) {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.soldiers[n20971538.index].id), 0.0))) {
          n20971539 = Game_swarm_collect_next_id();
          combat_.soldiers[n20971538.index].id = n20971539.Result;
        }
        if (logic_or(enum_equal<People_State>(people_.people_[combat_.soldiers[n20971538.index].person].state, People_State::down), enum_equal<People_State>(people_.people_[combat_.soldiers[n20971538.index].person].state, People_State::gone))) {
          skip_4 = true;
        }
        if (logic_not(skip_4)) {
          anim = kern::to<Int>(kern::formula([](Float a, Float st, Float a_2, Float a_3) -> Float { return kern::math::band((((a != 0.0) || ((st - a_2) < 0.5)) ? 1.0 : ((a_3 > 3.0) ? 2.0 : ((a_3 > 0.0) ? 3.0 : 0.0))), 255.0); }, kern::to<Float>(combat_.soldiers[n20971538.index].los), st, combat_.soldiers[n20971538.index].last_shot, people_.people_[combat_.soldiers[n20971538.index].person].v));
          list_add(out, struct_make<net_SwarmUnit>(combat_.soldiers[n20971538.index].id, net_SwarmKind::soldier, anim, people_.people_[combat_.soldiers[n20971538.index].person].pos, common::wrap_pi(people_.people_[combat_.soldiers[n20971538.index].person].yaw).Result, people_.people_[combat_.soldiers[n20971538.index].person].v));
        }
      }
    }
    for (auto it20971540 = flow_for(n20971540, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it20971540.next();) {
      skip_5 = false;
      if (math_compare<"<">(kern::to<Float>(units_[n20971540.index].car), 0.0)) {
        skip_5 = true;
      }
      if (logic_not(skip_5)) {
        for (auto it20971541 = flow_for(n20971541, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit_Cop>(units_[n20971540.index].cops))))); it20971541.next();) {
          skip_6 = false;
          if (logic_or(math_compare<"<">(kern::to<Float>(units_[n20971540.index].cops[n20971541.index].person), 0.0), units_[n20971540.index].cops[n20971541.index].inside)) {
            skip_6 = true;
          }
          if (logic_not(skip_6)) {
            p_at = units_[n20971540.index].cops[n20971541.index].person;
            if (logic_or(logic_or(math_compare<"!=">(kern::to<Float>(people_.people_[p_at].seed), kern::to<Float>(units_[n20971540.index].cops[n20971541.index].seed)), enum_equal<People_State>(people_.people_[p_at].state, People_State::down)), enum_equal<People_State>(people_.people_[p_at].state, People_State::gone))) {
              skip_6 = true;
            }
            if (logic_not(skip_6)) {
              if (logic_not(math_compare<"!=">(kern::to<Float>(units_[n20971540.index].cops[n20971541.index].id), 0.0))) {
                n20971542 = Game_swarm_collect_next_id();
                units_[n20971540.index].cops[n20971541.index].id = n20971542.Result;
              }
              flags = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::band(kern::math::bor(kern::math::wrap32(((a != 0.0) ? a_2 : 0.0)), kern::math::wrap32(kern::math::shl(kern::math::mod(a_3, 5.0), a_2))), 255.0); }, kern::to<Float>(people_.people_[p_at].aiming), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims))), kern::to<Float>(units_[n20971540.index].cops[n20971541.index].seed)));
              list_add(out, struct_make<net_SwarmUnit>(units_[n20971540.index].cops[n20971541.index].id, net_SwarmKind::officer, flags, people_.people_[p_at].pos, common::wrap_pi(people_.people_[p_at].yaw).Result, people_.people_[p_at].v));
              list_add(sent, struct_make<Game_SwarmSent>(out[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_SwarmUnit>(out))))], units_[n20971540.index].cops[n20971541.index].person, people_.people_[p_at].seed));
            }
          }
        }
      }
    }
    for (auto it20971543 = flow_for(n20971543, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it20971543.next();) {
      skip_7 = false;
      either = logic_or(logic_not(cars_[n20971543.index].police), logic_not(cars_[n20971543.index].placed));
      if (logic_not(either)) {
        actor_alive_2 = engine_game_world_actor_alive(world_, cars_[n20971543.index].actor);
        either = logic_not(actor_alive_2);
      }
      if (logic_or(either, logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(car_index_), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n20971543.index))))))))) {
        skip_7 = true;
      }
      if (logic_not(skip_7)) {
        n20971544 = state_of(cars_[n20971543.index].actor);
        s = n20971544.Result;
        f = engine_math_math::Quat_rotate(s.rotation, Vector{1.0, 0.0, 0.0}).Result;
        lights = logic_and(logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(cars_[n20971543.index].unit), 0.0), math_compare<"<">(kern::to<Float>(cars_[n20971543.index].unit), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<PoliceUnit>(units_)))))))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n20971543.index].unit].state, PoliceUnit_State::abandoned))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n20971543.index].unit].state, PoliceUnit_State::wreck))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n20971543.index].unit].state, PoliceUnit_State::leave)));
        flags_2 = kern::to<Int>(kern::formula([](Float lights, Float a, Float a_2, Float a_3, Float a_4, Float a_5) -> Float { return kern::math::band(kern::math::bor(kern::math::bor(((lights != 0.0) ? a : 0.0), ((a_2 != 0.0) ? a_3 : 0.0)), ((a_4 >= 0.0) ? a_5 : 0.0)), 255.0); }, kern::to<Float>(lights), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims))), kern::to<Float>(cars_[n20971543.index].wreck), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserWreck))), cars_[n20971543.index].burn_until, kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserBurns)))));
        list_add(out, struct_make<net_SwarmUnit>(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::band((index + 1.0), 65535.0); }, kern::to<Float>(n20971543.index))), net_SwarmKind::cruiser, flags_2, s.position, kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(f).y, vector_break(f).x), car::Car_steer_angle(cars_[n20971543.index].car[0]).Result));
      }
    }
    for (auto it20971545 = flow_for(n20971545, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it20971545.next();) {
      if (pdrones_[n20971545.index].active) {
        list_add(out, struct_make<net_SwarmUnit>(game_swarm_cpp::drone_id(pdrones_[n20971545.index]).Result, net_SwarmKind::police_drone, kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(((a != 0.0) ? a_2 : 0.0), 255.0); }, kern::to<Float>(pdrones_[n20971545.index].emp), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims))))), pdrones_[n20971545.index].pos, common::wrap_pi(pdrones_[n20971545.index].yaw).Result, 0.0));
      }
    }
    for (auto it20971546 = flow_for(n20971546, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<EnemyAi>(combat_.enemies))))); it20971546.next();) {
      skip_8 = false;
      if (math_compare<"<">(kern::to<Float>(combat_.enemies[n20971546.index].person), 0.0)) {
        skip_8 = true;
      }
      if (logic_not(skip_8)) {
        p_at_2 = combat_.enemies[n20971546.index].person;
        if (logic_or(logic_or(logic_not(enum_equal<People_Kind>(people_.people_[p_at_2].kind, People_Kind::enemy)), enum_equal<People_State>(people_.people_[p_at_2].state, People_State::down)), enum_equal<People_State>(people_.people_[p_at_2].state, People_State::gone))) {
          skip_8 = true;
        }
        if (logic_not(skip_8)) {
          if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.enemies[n20971546.index].id), 0.0))) {
            n20971547 = Game_swarm_collect_next_id();
            combat_.enemies[n20971546.index].id = n20971547.Result;
          }
          flags_3 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::band(kern::math::bor(kern::math::band(a, a_2), ((a_3 != 0.0) ? a_4 : 0.0)), 255.0); }, kern::to<Float>(enum_index<combat_EnemyType>(combat_.enemies[n20971546.index].type)), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kEnemyTypeMask))), kern::to<Float>(people_.people_[p_at_2].aiming), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserBurns)))));
          list_add(out, struct_make<net_SwarmUnit>(combat_.enemies[n20971546.index].id, net_SwarmKind::enemy, flags_3, people_.people_[p_at_2].pos, common::wrap_pi(people_.people_[p_at_2].yaw).Result, people_.people_[p_at_2].v));
          list_add(sent, struct_make<Game_SwarmSent>(out[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_SwarmUnit>(out))))], combat_.enemies[n20971546.index].person, people_.people_[p_at_2].seed));
        }
      }
    }
    for (auto it20971548 = flow_for(n20971548, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_SwarmSent>(swarm_sent_))))); it20971548.next();) {
      skip_9 = false;
      at = 0;
      end = list_length<Game_SwarmSent>(sent);
      go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      while (loop(go)) {
        if (swarm_collect_match(swarm_sent_[n20971548.index], sent[at]).Result) {
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
        if (logic_or(math_compare<"<">(kern::to<Float>(swarm_sent_[n20971548.index].person), 0.0), math_compare<">=">(kern::to<Float>(swarm_sent_[n20971548.index].person), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<People_Person>(people_.people_))))))))) {
          skip_9 = true;
        }
        if (logic_not(skip_9)) {
          if (logic_or(math_compare<"!=">(kern::to<Float>(people_.people_[swarm_sent_[n20971548.index].person].seed), kern::to<Float>(swarm_sent_[n20971548.index].seed)), logic_not(enum_equal<People_State>(people_.people_[swarm_sent_[n20971548.index].person].state, People_State::down)))) {
            skip_9 = true;
          }
          if (logic_not(skip_9)) {
            u = swarm_sent_[n20971548.index].unit;
            u.flags = kern::to<Int>(kern::formula([](Float flags, Float a) -> Float { return kern::math::bor(flags, a); }, kern::to<Float>(u.flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kUnitDead)))));
            u.p = people_.people_[swarm_sent_[n20971548.index].person].pos;
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
          var_take(n20971549, swarm_fallen_[at_2]);
          swarm_fallen_[kept] = n20971549.value;
        }
        kept += 1;
      }
      at_2 += 1;
    }
    list_resize(swarm_fallen_, kept, Game_SwarmFallen{net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, 0.0});
    for (auto it20971550 = flow_for(n20971550, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_SwarmFallen>(swarm_fallen_))))); it20971550.next();) {
      list_add(out, swarm_fallen_[n20971550.index].unit);
    }
  }

  void swarm_draw() {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    combat_EnemyType type = combat_EnemyType::cop;
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Quat a_13 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_14 = Quat{0.0, 0.0, 0.0, 1.0};
    for (auto it20971559 = flow_for(n20971559, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Game_RemoteSwarm>(swarms_))))); it20971559.next();) {
      for (auto it20971560 = flow_for(n20971560, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_RemoteUnit>(swarms_[n20971559.index].units))))); it20971560.next();) {
        if (enum_equal<net_SwarmKind>(swarms_[n20971559.index].units[n20971560.index].kind, net_SwarmKind::heli)) {
          a_2 = engine_math_math::Mat4_translate(swarms_[n20971559.index].units[n20971560.index].p).Result;
          a_3 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(swarms_[n20971559.index].units[n20971560.index].yaw).Result, Vector{1.0, 1.0, 1.0}).Result;
          a = engine_math_math::Mat4_mul(a_2, a_3).Result;
          combat_art_cpp::CombatArt_heli(combat_art_, a, swarms_[n20971559.index].units[n20971560.index].rotor, math_compare<"!=">(kern::to<Float>(swarms_[n20971559.index].units[n20971560.index].flags), 0.0));
        } else if (enum_equal<net_SwarmKind>(swarms_[n20971559.index].units[n20971560.index].kind, net_SwarmKind::tank)) {
          a_5 = engine_math_math::Mat4_translate(vector_make(vector_break(swarms_[n20971559.index].units[n20971560.index].p).x, vector_break(swarms_[n20971559.index].units[n20971560.index].p).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(swarms_[n20971559.index].units[n20971560.index].p).x, vector_break(swarms_[n20971559.index].units[n20971560.index].p).y).Result)).Result;
          a_6 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(swarms_[n20971559.index].units[n20971560.index].yaw).Result, Vector{1.0, 1.0, 1.0}).Result;
          a_4 = engine_math_math::Mat4_mul(a_5, a_6).Result;
          combat_art_cpp::CombatArt_tank(combat_art_, a_4, swarms_[n20971559.index].units[n20971560.index].aux, math_compare<"!=">(kern::to<Float>(swarms_[n20971559.index].units[n20971560.index].flags), 0.0));
        } else if (logic_and(enum_equal<net_SwarmKind>(swarms_[n20971559.index].units[n20971560.index].kind, net_SwarmKind::enemy), math_compare<">=">(kern::to<Float>(swarms_[n20971559.index].units[n20971560.index].person), 0.0))) {
          type = enum_from_index<combat_EnemyType>(people_.people_[swarms_[n20971559.index].units[n20971560.index].person].role).value;
          a_8 = engine_math_math::Mat4_translate(vector_add(people_.people_[swarms_[n20971559.index].units[n20971560.index].person].pos, Vector{0.0, 0.0, 1.4})).Result;
          a_9 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(people_.people_[swarms_[n20971559.index].units[n20971560.index].person].yaw).Result, Vector{0.27, 0.45, 0.06}).Result;
          a_7 = engine_math_math::Mat4_mul(a_8, a_9).Result;
          combat_art_cpp::CombatArt_put(combat_art_, a_7, CombatArt_Shape::glow_box, vector_scale(struct_break<combat_EnemySpec>(enemy_spec(type).Result).stripe, 1.6));
          if (enum_equal<combat_EnemyType>(type, combat_EnemyType::heavy)) {
            a_11 = engine_math_math::Mat4_translate(vector_add(people_.people_[swarms_[n20971559.index].units[n20971560.index].person].pos, vector_make(kern::formula([](Float a) -> Float { return (kern::math::cos(a) * 0.1); }, people_.people_[swarms_[n20971559.index].units[n20971560.index].person].yaw), kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.1); }, people_.people_[swarms_[n20971559.index].units[n20971560.index].person].yaw), 1.62))).Result;
            a_13 = common::yaw_quat(people_.people_[swarms_[n20971559.index].units[n20971560.index].person].yaw).Result;
            a_14 = engine_math_math::Quat_axis_angle(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
            a_12 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_mul(a_13, a_14).Result, Vector{0.26, 0.26, 1.1}).Result;
            a_10 = engine_math_math::Mat4_mul(a_11, a_12).Result;
            combat_art_cpp::CombatArt_put(combat_art_, a_10, CombatArt_Shape::cylinder, Vector{0.1, 0.1, 0.13});
          }
        }
      }
    }
  }

  void swarm_draw_town() {
    Float now = 0.0;
    Float sim_time = 0.0;
    physics_Pose rest = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    CarSpec a = CarSpec{"car", Vector{1.78, 0.82, 0.36}, 1350.0, -0.06, 0.15, 0.34, 0.22, 22.0, -0.26, List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}, -0.15, 0.11, 0.06, 550.0, 850.0, 140000.0, 0.15, 15.0, 0.68, 0.013, 1.3, 2400.0, 3200.0, 30.0, 0.58, 430.0, 600.0, 6.0, 1.1, 2.8, 3000.0, 95.0, List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}, 0.71, 0.54};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector v = Vector{0.0, 0.0, 0.0};
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    for (auto it20971563 = flow_for(n20971563, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Game_RemoteSwarm>(swarms_))))); it20971563.next();) {
      for (auto it20971564 = flow_for(n20971564, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_RemoteUnit>(swarms_[n20971563.index].units))))); it20971564.next();) {
        if (enum_equal<net_SwarmKind>(swarms_[n20971563.index].units[n20971564.index].kind, net_SwarmKind::cruiser)) {
          a = car::CarSpec_car().Result;
          rest = car_rest_pose(a, vector_break(swarms_[n20971563.index].units[n20971564.index].p).x, vector_break(swarms_[n20971563.index].units[n20971564.index].p).y, swarms_[n20971563.index].units[n20971564.index].yaw).Result;
          a_2 = engine_math_math::Mat4_trs(rest.position, rest.rotation, Vector{1.0, 1.0, 1.0}).Result;
          people_art_cpp::PeopleArt_light_bar(people_art_, a_2, math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(swarms_[n20971563.index].units[n20971564.index].flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims)))), 0.5), now);
          if (logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(swarms_[n20971563.index].units[n20971564.index].flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserWreck)))), 0.5), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, swarms_[n20971563.index].units[n20971564.index].wreck_at), 40.0))) {
            people_art_cpp::PeopleArt_fire(people_art_, vector_add(rest.position, Vector{0.0, 0.0, 0.4}), 0.35, now);
          }
        } else if (enum_equal<net_SwarmKind>(swarms_[n20971563.index].units[n20971564.index].kind, net_SwarmKind::police_drone)) {
          v = vector_scale(vector_sub(swarms_[n20971563.index].units[n20971564.index].tp, swarms_[n20971563.index].units[n20971564.index].p), 10.0);
          people_art_cpp::PeopleArt_police_drone(people_art_, swarms_[n20971563.index].units[n20971564.index].p, swarms_[n20971563.index].units[n20971564.index].yaw, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float y) -> Float { return ((-y) * 0.04); }, vector_break(v).y), -0.3, 0.3)), kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x) -> Float { return (x * 0.04); }, vector_break(v).x), -0.3, 0.3)), logic_not(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(swarms_[n20971563.index].units[n20971564.index].flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims)))), 0.5)), now, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mod(a, 5.0)); }, kern::to<Float>(swarms_[n20971563.index].units[n20971564.index].id))));
        }
      }
    }
  }

  /// The hand took something (game_vr.cpp vr_grab): ours to stream until it lies still.
  void toss_own(Int kind = 0, Int local = 0, Int64 id = Int64{0x0}) {
    Int64 net_id = Int64{0x0};
    Float sim_time = 0.0;
    if (logic_not(net_session::Session_active(net_).Result)) {
      return;
    }
    net_id = id;
    if (math_compare<"==">(kern::to<Float>(kind), kern::to<Float>(kPerson))) {
      if (logic_or(math_compare<"<">(kern::to<Float>(local), 0.0), math_compare<">=">(kern::to<Float>(local), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<People_Person>(people_.people_))))))))) {
        return;
      }
      net_id = logic_select<Int64>(net_session::Session_hosting(net_).Result, int64_of_int(local), int64_of_int(people_.people_[local].net));
      if (logic_and(logic_not(net_session::Session_hosting(net_).Result), math_compare<"<">(kern::to<Float>(people_.people_[local].net), 0.0))) {
        return;
      }
    } else if (math_compare<"==">(kern::to<Float>(kind), kern::to<Float>(kBird))) {
      net_id = int64_or(int64_shift_left(int64_of_int(net_session::Session_self_slot(net_).Result), 32), int64_of_int(kern::to<Int>(kern::formula([](Float local) -> Float { return kern::math::wrap32(local); }, kern::to<Float>(local)))));
    }
    for (auto it22020106 = flow_for(n22020106, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TossState_Own>(toss_.own))))); it22020106.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(toss_.own[n22020106.index].kind), kern::to<Float>(kind)), int64_equal(toss_.own[n22020106.index].id, net_id))) {
        toss_.own[n22020106.index].still = 0.0;
        toss_.own[n22020106.index].done_sent = 0;
        toss_.own[n22020106.index].local = local;
        return;
      }
    }
    sim_time = engine_game_world_sim_time(world_);
    list_add(toss_.own, struct_make<TossState_Own>(kind, net_id, local, 0.0, sim_time, 0));
  }

  void toss_own_body(physics_Body b = physics_Body{Int64{0x0}}) {
    Int64 id = Int64{0x0};
    Int a = 0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)), logic_not(net_session::Session_active(net_).Result))) {
      return;
    }
    id = Int64{0x0};
    n22020107 = life::Life_animal_of(glife_[0].life, id, b);
    a = n22020107.Result;
    if (math_compare<"==">(kern::to<Float>(a), 1.0)) {
      toss_own(kCow, -1, id);
    } else if (math_compare<"==">(kern::to<Float>(a), 2.0)) {
      toss_own(kPet, kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(id)))), id);
    }
  }

  struct toss_jsonResult { Text Result = ""; };
  toss_jsonResult toss_json() {
    Text b = "";
    Int ghosts = 0;
    Int rounds = 0;
    Vector gp = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    b = "";
    ghosts = 0;
    rounds = 0;
    gp = Vector{0.0, 0.0, 0.0};
    for (auto it22020108 = flow_for(n22020108, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it22020108.next();) {
      skip = false;
      if (logic_not(projectiles_[n22020108.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (projectiles_[n22020108.index].ghost) {
          ghosts += 1;
          gp = projectiles_[n22020108.index].pos;
        } else {
          rounds += 1;
        }
      }
    }
    b = text_format<"{\"own\":%zu,\"remote\":%zu,\"sent\":%u,\"got\":%u,\"rounds\":%u,\"ghosts\":%u,\"ghost\":[%.2f,%.2f,%.2f]}">(255, list_length<TossState_Own>(toss_.own), list_length<TossState_Remote>(toss_.remote), toss_.sent, toss_.got, rounds, ghosts, vector_break(gp).x, vector_break(gp).y, vector_break(gp).z);
    return {b};
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
    engine_game_world_physics(n23068682, world_);
    physics = n23068682.result;
    return {logic_not(struct_break<the_RayResult_>(common::cast(physics, a, vector_scale(d, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l)), l, body(me).Result, Int64{0x1}).Result).hit)};
  }

  void spawn_police_unit() {
    Int car = 0;
    Int i = 0;
    Bool stop_2 = false;
    Vector me = Vector{0.0, 0.0, 0.0};
    Vector p = Vector{0.0, 0.0, 0.0};
    Float heading = 0.0;
    Bool found = false;
    Int attempt = 0;
    Bool stop_3 = false;
    Bool skip = false;
    Float d = 0.0;
    Int physics = 0;
    Float to_player = 0.0;
    Vector pos = Vector{0.0, 0.0, 0.0};
    Bool actor_alive = false;
    Vector rot = Vector{0.0, 0.0, 0.0};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    PoliceUnit u = PoliceUnit{-1, PoliceUnit_State::pursuit, List<PoliceUnit_Cop>{}, List<Vector>{}, -99.0, -1.0, 0.0, -1.0, -1.0, 17.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, -1.0};
    Float sim_time = 0.0;
    Int slot = 0;
    Int i_2 = 0;
    Bool stop_4 = false;
    Float sim_time_2 = 0.0;
    car = -1;
    i = 0;
    stop_2 = false;
    while (loop(logic_and(logic_not(stop_2), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<TrafficCar>(cars_))), 0.5)))) {
      if (logic_and(logic_and(cars_[i].police, math_compare<"<">(kern::to<Float>(cars_[i].unit), 0.0)), logic_not(cars_[i].placed))) {
        car = kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32s(i); }, kern::to<Float>(i)));
        stop_2 = true;
      }
      if (logic_not(stop_2)) {
        i += 1;
      }
    }
    if (math_compare<"<">(kern::to<Float>(car), 0.0)) {
      return;
    }
    n23068690 = player_position();
    me = n23068690.Result;
    p = Vector{0.0, 0.0, 0.0};
    heading = 0.0;
    found = false;
    attempt = 0;
    stop_3 = false;
    while (loop(logic_and(logic_not(stop_3), logic_and(math_compare<"<">(kern::to<Float>(attempt), 12.0), logic_not(found))))) {
      skip = false;
      n23068691 = city_stream_cpp::CityStream_street_point(city_, rng_, p, heading, me, kSpawnMax);
      if (logic_not(n23068691.Result)) {
        stop_3 = true;
      }
      if (logic_not(logic_or(stop_3, skip))) {
        d = game_town_cpp::dist2d(p, me).Result;
        if (math_compare<"<">(d, kSpawnMin)) {
          skip = true;
        }
        if (logic_not(logic_or(stop_3, skip))) {
          engine_game_world_physics(n23068692, world_);
          physics = n23068692.result;
          if (struct_break<the_RayResult_>(common::cast(physics, vector_add(p, Vector{0.0, 0.0, 3.5}), Vector{0.0, 0.0, -1.0}, 3.0, struct_make<physics_Body>(Int64{0x0}), Int64{0xffffffffffffffff}).Result).hit) {
            skip = true;
          }
          if (logic_not(logic_or(stop_3, skip))) {
            found = true;
          }
        }
      }
      if (logic_not(stop_3)) {
        attempt += 1;
      }
    }
    if (logic_not(found)) {
      return;
    }
    to_player = kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(me).y, vector_break(p).y, vector_break(me).x, vector_break(p).x);
    if (math_compare<">">(kern::formula([](Float the_result) -> Float { return kern::math::abs(the_result); }, common::wrap_pi(kern::formula([](Float to_player, Float heading) -> Float { return (to_player - heading); }, to_player, heading)).Result), kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi))) {
      heading = common::wrap_pi(kern::formula([](Float heading, Float kPi) -> Float { return (heading + kPi); }, heading, kPi)).Result;
    }
    pos = vector_make(vector_break(p).x, vector_break(p).y, kern::formula([](Float the_result, Float the_result_2) -> Float { return ((the_result + the_result_2) + 0.05); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(p).x, vector_break(p).y).Result, car::Car_ground_offset(cars_[car].car[0].s_).Result));
    actor_alive = engine_game_world_actor_alive(world_, cars_[car].actor);
    if (logic_not(actor_alive)) {
      engine_game_world_set_alive(world_, cars_[car].actor, true);
    }
    rot = vector_make(0.0, 0.0, kern::formula([](Float heading, Float kDeg) -> Float { return kern::math::div(heading, kDeg); }, heading, kDeg));
    engine_game_world_teleport(n23068693, world_, cars_[car].actor, pos, list_make<Vector, "1">(rot));
    teleport = n23068693.result;
    engine_game_world_set_actor_velocity(n23068694, world_, cars_[car].actor, Vector{0.0, 0.0, 0.0});
    set_actor_velocity = n23068694.result;
    car_cpp::Car_reset(cars_[car].car[0]);
    cars_[car].placed = true;
    cars_[car].driver = true;
    cars_[car].hp = car_max_hp(cars_[car]).Result;
    cars_[car].wreck = false;
    cars_[car].hit_by_player = false;
    cars_[car].burn_until = -1.0;
    cars_[car].lane = -1;
    u = PoliceUnit{-1, PoliceUnit_State::pursuit, List<PoliceUnit_Cop>{}, List<Vector>{}, -99.0, -1.0, 0.0, -1.0, -1.0, 17.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, -1.0};
    u.car = car;
    u.state = PoliceUnit_State::pursuit;
    for (auto it23068695 = flow_for(n23068695, 0, 1); it23068695.next();) {
      n23068696 = common::Rng_next(rng_);
      u.cops[n23068695.index].seed = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::bor(the_result, 1.0); }, kern::to<Float>(n23068696.Result)));
      u.cops[n23068695.index].inside = true;
      u.cops[n23068695.index].seat = kern::to<Int>(kern::formula([](Float index) -> Float { return ((index != 0.0) ? (-1.0) : 1.0); }, kern::to<Float>(n23068695.index)));
    }
    u.progress_at = pos;
    sim_time = engine_game_world_sim_time(world_);
    u.progress_t = sim_time;
    slot = -1;
    i_2 = 0;
    stop_4 = false;
    while (loop(logic_and(logic_not(stop_4), math_compare<">">(kern::formula([](Float i_2, Float a) -> Float { return ((i_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_2 < a)); }, kern::to<Float>(i_2), kern::to<Float>(list_length<PoliceUnit>(units_))), 0.5)))) {
      if (math_compare<"<">(kern::to<Float>(units_[i_2].car), 0.0)) {
        slot = kern::to<Int>(kern::formula([](Float i_2) -> Float { return kern::math::wrap32s(i_2); }, kern::to<Float>(i_2)));
        stop_4 = true;
      }
      if (logic_not(stop_4)) {
        i_2 += 1;
      }
    }
    if (math_compare<"<">(kern::to<Float>(slot), 0.0)) {
      list_add(units_, u);
      slot = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s((a - 1.0)); }, kern::to<Float>(list_length<PoliceUnit>(units_))));
    } else {
      units_[slot] = u;
    }
    cars_[car].unit = slot;
    sim_time_2 = engine_game_world_sim_time(world_);
    last_wave_ = sim_time_2;
  }

  void Game_drive_police_brake(Float vf = 0.0, Int c_at = 0) {
    CarInput in = CarInput{0.0, 0.0, false, 32.0, 9.0};
    in = CarInput{0.0, 0.0, false, 32.0, 9.0};
    in.handbrake = true;
    in.pedal = kern::formula([](Float vf) -> Float { return ((kern::math::abs(vf) > 0.5) ? (-kern::math::sign(vf)) : 0.0); }, vf);
    cars_[c_at].input = in;
  }

  void Game_drive_police_exit_car(PoliceUnit& u, Vector fwd = Vector{0.0, 0.0, 0.0}, Vector pos = Vector{0.0, 0.0, 0.0}, Int unit = 0, Float now = 0.0) {
    Float yaw = 0.0;
    Vector left = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    Vector at = Vector{0.0, 0.0, 0.0};
    yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(fwd).y, vector_break(fwd).x);
    left = vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(fwd).y), vector_break(fwd).x, 0.0);
    for (auto it23068701 = flow_for(n23068701, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit_Cop>(u.cops))))); it23068701.next();) {
      skip = false;
      if (logic_or(logic_not(u.cops[n23068701.index].inside), logic_not(math_compare<"!=">(kern::to<Float>(u.cops[n23068701.index].seed), 0.0)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        at = vector_add(vector_add(pos, vector_scale(fwd, 0.25)), vector_scale(left, kern::formula([](Float a) -> Float { return (1.35 * a); }, kern::to<Float>(u.cops[n23068701.index].seat))));
        at = vector_make(vector_break(at).x, vector_break(at).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(at).x, vector_break(at).y).Result);
        n23068702 = line_of_sight(vector_add(pos, Vector{0.0, 0.0, 1.0}), vector_add(at, Vector{0.0, 0.0, 1.0}));
        if (logic_not(n23068702.Result)) {
          at = vector_sub(pos, vector_scale(fwd, 2.6));
        }
        at = vector_make(vector_break(at).x, vector_break(at).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(at).x, vector_break(at).y).Result);
        n23068703 = people_cpp::People_add_officer(people_, at, kern::formula([](Float yaw, Float a, Float kPi) -> Float { return (yaw + kern::math::div((a * kPi), 2.0)); }, yaw, kern::to<Float>(u.cops[n23068701.index].seat), kPi), unit, u.cops[n23068701.index].seed);
        u.cops[n23068701.index].person = n23068703.Result;
        u.cops[n23068701.index].inside = false;
        n23068704 = common::Rng_unit(rng_);
        u.cops[n23068701.index].next_shot = kern::formula([](Float now, Float the_result) -> Float { return ((now + 0.7) + (the_result * 0.5)); }, now, n23068704.Result);
        u.cops[n23068701.index].los = false;
        u.cops[n23068701.index].los_at = -1.0;
      }
    }
    u.state = PoliceUnit_State::engage;
  }

  struct request_respawnResult { Bool Result = false; };
  request_respawnResult request_respawn() {
    Bool respawn_now = false;
    if (logic_or(logic_not(player_dead_), math_compare<">">(respawn_wait().Result, 0.0))) {
      return {false};
    }
    engine_game_world_respawn_now(n23068709, world_, soldier_);
    respawn_now = n23068709.result;
    return {respawn_now};
  }

  struct map_jsonResult { Text Result = ""; };
  map_jsonResult map_json() {
    Vector me = Vector{0.0, 0.0, 0.0};
    Float range = 0.0;
    Text o = "";
    Text b = "";
    Vector look = Vector{0.0, 0.0, 0.0};
    Bool first = false;
    Bool skip = false;
    Bool near = false;
    Int k = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l2 = 0.0;
    Float t = 0.0;
    Bool skip_2 = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip_3 = false;
    Vector j = Vector{0.0, 0.0, 0.0};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_not(world_placed_)) {
      return {"{}"};
    }
    n23068711 = focus_position();
    me = n23068711.Result;
    range = 180.0;
    o = "";
    b = "";
    look = vector_sub(cam_.target, cam_.eye);
    b = text_format<"{\"me\":[%.1f,%.1f,%.3f],\"lanes\":[">(95, vector_break(me).x, vector_break(me).y, kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(look).y, vector_break(look).x));
    o = text_append(o, b, "");
    first = true;
    for (auto it23068712 = flow_for(n23068712, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it23068712.next();) {
      skip = false;
      if (logic_not(city_.lanes_[n23068712.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        near = false;
        k = 0;
        while (loop(logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(city_.lanes_[n23068712.index].points))), 0.5), logic_not(near)))) {
          a = city_.lanes_[n23068712.index].points[k];
          d = vector_sub(city_.lanes_[n23068712.index].points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))], a);
          l2 = kern::formula([](Float x, Float y) -> Float { return kern::math::max(1e-06, ((x * x) + (y * y))); }, vector_break(d).x, vector_break(d).y);
          t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(me).x, vector_break(a).x, vector_break(d).x, vector_break(me).y, vector_break(a).y, vector_break(d).y, l2), 0.0, 1.0));
          near = math_compare<"<">(kern::formula([](Float x, Float x_2, Float t, Float x_3, Float y, Float y_2, Float y_3) -> Float { return kern::math::sqrt((((x + (x_2 * t)) - x_3) * ((x + (x_2 * t)) - x_3)) + (((y + (y_2 * t)) - y_3) * ((y + (y_2 * t)) - y_3))); }, vector_break(a).x, vector_break(d).x, t, vector_break(me).x, vector_break(a).y, vector_break(d).y, vector_break(me).y), range);
          k += 1;
        }
        if (logic_not(near)) {
          skip = true;
        }
        if (logic_not(skip)) {
          o = text_append(o, logic_select<Text>(first, "[", ",["), "");
          first = false;
          for (auto it23068713 = flow_for(n23068713, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(city_.lanes_[n23068712.index].points))))); it23068713.next();) {
            b = text_format<"%s%.0f,%.0f">(95, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n23068713.index), 0.0), ",", ""), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(city_.lanes_[n23068712.index].points[n23068713.index]).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(city_.lanes_[n23068712.index].points[n23068713.index]).y, vector_break(me).y));
            o = text_append(o, b, "");
          }
          o = text_append(o, "]", "");
        }
      }
    }
    o = text_append(o, "],\"police\":[", "");
    first = true;
    for (auto it23068714 = flow_for(n23068714, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it23068714.next();) {
      skip_2 = false;
      if (math_compare<"<">(kern::to<Float>(units_[n23068714.index].car), 0.0)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        n23068715 = matrix_of(cars_[units_[n23068714.index].car].actor);
        a_2 = n23068715.Result;
        p = engine_math_math::Mat4_translation(a_2).Result;
        b = text_format<"%s[%.0f,%.0f]">(95, logic_select<Text>(first, "", ","), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(p).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(p).y, vector_break(me).y));
        o = text_append(o, b, "");
        first = false;
      }
    }
    for (auto it23068716 = flow_for(n23068716, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it23068716.next();) {
      skip_3 = false;
      if (logic_not(pdrones_[n23068716.index].active)) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        b = text_format<"%s[%.0f,%.0f]">(95, logic_select<Text>(first, "", ","), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(pdrones_[n23068716.index].pos).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(pdrones_[n23068716.index].pos).y, vector_break(me).y));
        o = text_append(o, b, "");
        first = false;
      }
    }
    o = text_append(o, "],\"players\":[", "");
    first = true;
    for (auto it23068717 = flow_for(n23068717, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it23068717.next();) {
      b = text_format<"%s[%.0f,%.0f]">(95, logic_select<Text>(first, "", ","), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(net_.remotes_[n23068717.index].shown).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(net_.remotes_[n23068717.index].shown).y, vector_break(me).y));
      o = text_append(o, b, "");
      first = false;
    }
    o = text_append(o, "]", "");
    if (logic_and(jet_placed_, logic_not(enum_equal<Mode>(mode_, Mode::jet)))) {
      n23068718 = matrix_of(jet_actor_);
      a_3 = n23068718.Result;
      j = engine_math_math::Mat4_translation(a_3).Result;
      b = text_format<",\"jet\":[%.0f,%.0f]">(95, kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(j).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(j).y, vector_break(me).y));
      o = text_append(o, b, "");
    }
    return {text_append(o, "}", "")};
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
    Int64 key_2 = Int64{0x0};
    List<Int> t = List<Int>{};
    Int64 key_3 = Int64{0x0};
    List<Int> put = List<Int>{};
    Int count = 0;
    Int fill = 0;
    List<Int> t_2 = List<Int>{};
    Int64 key_4 = Int64{0x0};
    List<Int> put_2 = List<Int>{};
    key = r.key;
    map_add(n24117258, gaps_[0].planks, key);
    key_2 = r.key;
    map_add(n24117259, gaps_[0].planks, key_2);
    if (math_compare<"!=">(kern::to<Float>(list_length<Int>(gaps_[0].planks[n24117259.index])), kern::to<Float>(r.doors))) {
      key_3 = r.key;
      map_add(n24117260, gaps_[0].planks, key_3);
      put = t;
      t = gaps_[0].planks[n24117260.index];
      count = r.doors;
      fill = kPlanksMax;
      list_clear(t);
      list_resize(t, count, fill);
      gaps_[0].planks[n24117260.index] = put;
    }
    key_4 = r.key;
    map_add(n24117261, gaps_[0].planks, key_4);
    put_2 = t_2;
    t_2 = gaps_[0].planks[n24117261.index];
    t_2[r.door] = r.planks;
    gaps_[0].planks[n24117261.index] = put_2;
  }

  struct townsync_jsonResult { Text Result = ""; };
  townsync_jsonResult townsync_json() {
    Int open_doors = 0;
    Int n = 0;
    Text b = "";
    townsync_ready();
    open_doors = 0;
    if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
      for (auto it24117265 = flow_for(n24117265, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<Int>>(gaps_[0].planks))))); it24117265.next();) {
        for (auto it24117266 = flow_for(n24117266, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(gaps_[0].planks[n24117265.index]))))); it24117266.next();) {
          n = gaps_[0].planks[n24117265.index][n24117266.index];
          open_doors += kern::to<Int>(math_compare<"<">(kern::to<Float>(n), kern::to<Float>(kPlanksMax)));
        }
      }
    }
    b = "";
    b = text_format<"{\"epoch\":%u,\"lamps\":%zu,\"doors\":%zu,\"marks\":%zu,\"carHits\":[%u,%u,%u],\"personHits\":%u,\"sent\":%u,\"got\":%u,\"asks\":%u,\"refused\":%u}">(255, townsync_[0].epoch, kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? a_2 : 0.0); }, kern::to<Float>(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), kern::to<Float>(lamps::Field_broken(gaps_[0].lamps).Result))), open_doors, list_length<town_MarkRec>(townsync_[0].marks), townsync_[0].car_hits[0], townsync_[0].car_hits[1], townsync_[0].car_hits[2], townsync_[0].person_hits, townsync_[0].sent_states, townsync_[0].got_states, townsync_[0].asks, net_session::Session_send_refused(net_).Result);
    return {b};
  }

  struct traffic_lane_distanceResult { Float Result = 0.0; };
  traffic_lane_distanceResult traffic_lane_distance(const TrafficCar& c) {
    Vector p = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_or(math_compare<"<">(kern::to<Float>(c.lane), 0.0), math_compare<">=">(kern::to<Float>(c.lane), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Lane>(city_.lanes_))))))))) {
      return {-1.0};
    }
    n25165834 = matrix_of(c.actor);
    a = n25165834.Result;
    p = engine_math_math::Mat4_translation(a).Result;
    return {game_traffic_cpp::lane_distance(city_.lanes_[c.lane], vector_make(vector_break(p).x, vector_break(p).y, 0.0)).Result};
  }

  void eject_driver(Int ci = 0) {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float yaw = 0.0;
    Vector side = Vector{0.0, 0.0, 0.0};
    Vector at = Vector{0.0, 0.0, 0.0};
    Int physics = 0;
    Int p = 0;
    Float now = 0.0;
    Float sim_time = 0.0;
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0)), math_compare<"<">(kern::to<Float>(ci), 0.0)), math_compare<">=">(kern::to<Float>(ci), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<TrafficCar>(cars_))))))))) {
      return;
    }
    n25165841 = state_of(cars_[ci].actor);
    s = n25165841.Result;
    yaw = common::heading_of(engine_math_math::Quat_rotate(s.rotation, Vector{1.0, 0.0, 0.0}).Result).Result;
    side = vector_make(kern::formula([](Float yaw) -> Float { return (-kern::math::sin(yaw)); }, yaw), kern::formula([](Float yaw) -> Float { return kern::math::cos(yaw); }, yaw), 0.0);
    at = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x + (x_2 * 1.8)); }, vector_break(s.position).x, vector_break(side).x), kern::formula([](Float y, Float y_2) -> Float { return (y + (y_2 * 1.8)); }, vector_break(s.position).y, vector_break(side).y), 0.0);
    engine_game_world_physics(n25165842, world_);
    physics = n25165842.result;
    if (struct_break<the_RayResult_>(common::cast(physics, s.position, vector_make(vector_break(side).x, vector_break(side).y, 0.0), 2.3, body(cars_[ci].actor).Result, Int64{0xffffffffffffffff}).Result).hit) {
      at = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - (x_2 * 1.8)); }, vector_break(s.position).x, vector_break(side).x), kern::formula([](Float y, Float y_2) -> Float { return (y - (y_2 * 1.8)); }, vector_break(s.position).y, vector_break(side).y), 0.0);
    }
    at = vector_make(vector_break(at).x, vector_break(at).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(at).x, vector_break(at).y).Result);
    n25165843 = common::Rng_next(rng_);
    n25165844 = people_cpp::People_add_citizen(people_, at, kern::formula([](Float yaw, Float kPi) -> Float { return (yaw + kern::math::div(kPi, 2.0)); }, yaw, kPi), kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::bor(the_result, 1.0); }, kern::to<Float>(n25165843.Result))));
    p = n25165844.Result;
    if (logic_and(logic_and(math_compare<">=">(kern::to<Float>(cars_[ci].lane), 0.0), math_compare<"<">(kern::to<Float>(cars_[ci].lane), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Lane>(city_.lanes_)))))))), city_.lanes_[cars_[ci].lane].alive)) {
      people_.people_[p].lane = cars_[ci].lane;
      people_.people_[p].seg = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return (a - ((a_2 > 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(cars_[ci].index), kern::to<Float>(cars_[ci].dir))))), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::wrap32s(a) - 2.0); }, kern::to<Float>(list_length<Vector>(city_.lanes_[cars_[ci].lane].points)))))))));
      n25165845 = common::Rng_unit(rng_);
      people_.people_[p].dir = kern::to<Int>(kern::formula([](Float the_result) -> Float { return ((the_result < 0.5) ? 1.0 : (-1.0)); }, n25165845.Result));
      people_.people_[p].side = 1.0;
      people_.people_[p].off = kern::formula([](Float a) -> Float { return ((a * 0.5) + 1.7); }, city_.lanes_[cars_[ci].lane].width);
      people_.people_[p].along = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(at).x, vector_break(city_.lanes_[cars_[ci].lane].points[people_.people_[p].seg]).x, vector_break(at).y, vector_break(city_.lanes_[cars_[ci].lane].points[people_.people_[p].seg]).y);
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    n25165846 = common::Rng_unit(rng_);
    if (math_compare<"<">(n25165846.Result, 0.5)) {
      people_cpp::People_frighten(people_, s.position, 4.0, now, 1.2);
    } else {
      people_.people_[p].state = People_State::wait;
      people_.people_[p].wait_until = kern::formula([](Float now) -> Float { return (now + 2.6); }, now);
      people_.people_[p].yaw = kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(s.position).y, vector_break(at).y, vector_break(s.position).x, vector_break(at).x);
      list_add(life_[0].delayed_fright, struct_make<WorldLife_Fright>(p, kern::formula([](Float now) -> Float { return (now + 2.6); }, now), s.position));
    }
    life_[0].ejected = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(life_[0].ejected)));
  }

  void draw_world_vehicles() {
    Int w = 0;
    Int physics = 0;
    Bool skip = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool either = false;
    physics_Pose p = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0))) {
      return;
    }
    engine_game_world_physics(n25165851, world_);
    physics = n25165851.result;
    w = physics;
    for (auto it25165852 = flow_for(n25165852, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))))); it25165852.next();) {
      skip = false;
      either = logic_not(engine_physics_physics::Body_valid(life_[0].trucks[n25165852.index].body).Result);
      if (logic_not(either)) {
        body_valid_2 = engine_world_body_valid(w, life_[0].trucks[n25165852.index].body);
        either = logic_not(body_valid_2);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        pose = engine_world_pose(w, life_[0].trucks[n25165852.index].body);
        p = pose;
        m = engine_math_math::Mat4_trs(p.position, p.rotation, Vector{1.0, 1.0, 1.0}).Result;
        vehicle_art_cpp::VehicleArt_car(art_, m, life_[0].trucks[n25165852.index].car[0], kTowYellow, false, math_compare<"<">(life_[0].trucks[n25165852.index].input.pedal, -0.05));
      }
    }
  }

  struct car_towedResult { Bool Result = false; };
  KERN_PURE car_towedResult car_towed(Int car = 0) {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0))) {
      return {false};
    }
    for (auto it25165859 = flow_for(n25165859, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))))); it25165859.next();) {
      if (math_compare<"==">(kern::to<Float>(life_[0].trucks[n25165859.index].job), kern::to<Float>(car))) {
        return {true};
      }
    }
    return {false};
  }

  struct car_rest_poseResult { physics_Pose Result = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE car_rest_poseResult car_rest_pose(const CarSpec& s, Float x = 0.0, Float y = 0.0, Float yaw = 0.0) {
    Float cy = 0.0;
    Float sy = 0.0;
    Fixed<Float, 4> h = Fixed<Float, 4>{};
    Float wx = 0.0;
    Float wy = 0.0;
    Float front = 0.0;
    Float rear = 0.0;
    Float left = 0.0;
    Float right = 0.0;
    Float xf = 0.0;
    Float xr = 0.0;
    Float track = 0.0;
    Float pitch = 0.0;
    Float roll = 0.0;
    Float z0 = 0.0;
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_2 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_3 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_4 = Quat{0.0, 0.0, 0.0, 1.0};
    cy = kern::formula([](Float yaw) -> Float { return kern::math::cos(yaw); }, yaw);
    sy = kern::formula([](Float yaw) -> Float { return kern::math::sin(yaw); }, yaw);
    h = List<Float>{};
    for (auto it26214410 = flow_for(n26214410, 0, 3); it26214410.next();) {
      wx = kern::formula([](Float x, Float cy, Float a, Float sy, Float a_2) -> Float { return ((x + (cy * a)) - (sy * a_2)); }, x, cy, s.wheels[n26214410.index].x, sy, s.wheels[n26214410.index].y);
      wy = kern::formula([](Float y, Float sy, Float a, Float cy, Float a_2) -> Float { return ((y + (sy * a)) + (cy * a_2)); }, y, sy, s.wheels[n26214410.index].x, cy, s.wheels[n26214410.index].y);
      h[n26214410.index] = city_stream_cpp::CityStream_ground_height(city_, wx, wy).Result;
    }
    front = kern::formula([](Float a, Float a_2) -> Float { return kern::math::div((a + a_2), 2.0); }, h[0], h[1]);
    rear = kern::formula([](Float a, Float a_2) -> Float { return kern::math::div((a + a_2), 2.0); }, h[2], h[3]);
    left = kern::formula([](Float a, Float a_2) -> Float { return kern::math::div((a + a_2), 2.0); }, h[0], h[2]);
    right = kern::formula([](Float a, Float a_2) -> Float { return kern::math::div((a + a_2), 2.0); }, h[1], h[3]);
    xf = s.wheels[0].x;
    xr = s.wheels[2].x;
    track = kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, s.wheels[0].y, s.wheels[1].y);
    pitch = kern::formula([](Float front, Float rear, Float xf, Float xr) -> Float { return kern::math::atan2((front - rear), (xf - xr)); }, front, rear, xf, xr);
    roll = kern::formula([](Float left, Float right, Float track) -> Float { return kern::math::atan2((left - right), track); }, left, right, track);
    z0 = kern::formula([](Float rear, Float front, Float xr, Float xf) -> Float { return (rear + kern::math::div(((front - rear) * (0.0 - xr)), (xf - xr))); }, rear, front, xr, xf);
    a_2 = common::yaw_quat(yaw).Result;
    a_3 = engine_math_math::Quat_axis_angle(Vector{0.0, 1.0, 0.0}, kern::formula([](Float pitch) -> Float { return (-pitch); }, pitch)).Result;
    a = engine_math_math::Quat_mul(a_2, a_3).Result;
    a_4 = engine_math_math::Quat_axis_angle(Vector{1.0, 0.0, 0.0}, roll).Result;
    q = common::qnormalize(engine_math_math::Quat_mul(a, a_4).Result).Result;
    return {struct_make<physics_Pose>(vector_make(x, y, kern::formula([](Float z0, Float the_result, Float z) -> Float { return (z0 + kern::math::div(the_result, kern::math::max(0.5, z))); }, z0, car::Car_ground_offset(s).Result, vector_break(engine_math_math::Quat_rotate(q, Vector{0.0, 0.0, 1.0}).Result).z)), q)};
  }

  void traffic_solid(TrafficCar& c) {
    physics_Body b = physics_Body{Int64{0x0}};
    Int physics = 0;
    Int physics_2 = 0;
    Bool body_valid = false;
    Bool both = false;
    Int physics_3 = 0;
    c.far = false;
    if (math_compare<"==">(kern::to<Float>(list_length<Int64>(c.solid_masks)), 0.0)) {
      return;
    }
    b = body(c.actor).Result;
    both = engine_physics_physics::Body_valid(b).Result;
    if (both) {
      engine_game_world_physics(n26214412, world_);
      physics_2 = n26214412.result;
      body_valid = engine_world_body_valid(physics_2, b);
      both = body_valid;
    }
    if (both) {
      engine_game_world_physics(n26214413, world_);
      physics_3 = n26214413.result;
      engine_world_set_body_masks(physics_3, b, c.solid_masks);
    }
    list_clear(c.solid_masks);
  }

  void traffic_make_far(TrafficCar& c) {
    Int w = 0;
    Int physics = 0;
    physics_Body b = physics_Body{Int64{0x0}};
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool either = false;
    Fixed<Int64, 1> nothing = Fixed<Int64, 1>{};
    physics_BodyType body_type = physics_BodyType::fixed;
    engine_game_world_physics(n26214414, world_);
    physics = n26214414.result;
    w = physics;
    b = body(c.actor).Result;
    either = logic_not(engine_physics_physics::Body_valid(b).Result);
    if (logic_not(either)) {
      body_valid_2 = engine_world_body_valid(w, b);
      either = logic_not(body_valid_2);
    }
    if (either) {
      c.far = false;
      return;
    }
    if (math_compare<"==">(kern::to<Float>(list_length<Int64>(c.solid_masks)), 0.0)) {
      engine_world_body_masks(n26214415, w, b, c.solid_masks);
      c.solid_masks = n26214415.out;
      nothing = list_make<Int64, "1">(Int64{0x8});
      engine_world_set_body_masks(w, b, nothing);
    }
    body_type = engine_world_body_type(w, b);
    if (logic_not(enum_equal<physics_BodyType>(body_type, physics_BodyType::kinematic))) {
      engine_world_set_body_type(w, b, physics_BodyType::kinematic);
    }
    engine_world_set_linear_velocity(w, b, Vector{0.0, 0.0, 0.0});
    engine_world_set_angular_velocity(w, b, Vector{0.0, 0.0, 0.0});
    car_cpp::Car_detach(c.car[0]);
    c.far = true;
  }

  struct ui_command_startsResult { Bool Result = false; };
  KERN_PURE ui_command_startsResult ui_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
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
      engine_renderer_allocate_id(n27262988, r_);
      allocate_id = n27262988.result;
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
    engine_upsert(n27262989, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n27262989.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, id);
      id = 0;
    }
  }

  /// settings_help.mjs worldGrid (1 m reference grid around the player) and box3dColliderDebug (Box3D's collision shapes as wire frames). Visual only.
  void ui_draw_helpers() {
    Int r = 0;
    Int renderer = 0;
    Bool grid = false;
    Vector me = Vector{0.0, 0.0, 0.0};
    Float cx = 0.0;
    Float cy = 0.0;
    List<Float> p = List<Float>{};
    Int n = 0;
    Float a = 0.0;
    Float b = 0.0;
    Float o = 0.0;
    Int place = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    Float item_5 = 0.0;
    Float item_6 = 0.0;
    Int place_2 = 0;
    Float item_7 = 0.0;
    Float item_8 = 0.0;
    Float item_9 = 0.0;
    Float item_10 = 0.0;
    Float item_11 = 0.0;
    Float item_12 = 0.0;
    List<Float> a_2 = List<Float>{};
    Bool set_visible = false;
    List<Float> pos = List<Float>{};
    List<Float> col = List<Float>{};
    Bool set_visible_2 = false;
    renderer = engine_engine_renderer(engine_);
    r = renderer;
    grid = logic_and(settings::Settings_on(ui_.settings, Opt::view_grid).Result, world_placed_);
    if (grid) {
      n27262990 = player_position();
      me = n27262990.Result;
      cx = kern::formula([](Float x) -> Float { return kern::math::round(x); }, vector_break(me).x);
      cy = kern::formula([](Float y) -> Float { return kern::math::round(y); }, vector_break(me).y);
      if (logic_or(logic_or(logic_not(math_compare<"!=">(kern::to<Float>(ui_.grid_mesh), 0.0)), math_compare<">=">(kern::formula([](Float cx, Float x) -> Float { return kern::math::abs((cx - x)); }, cx, vector_break(ui_.grid_at).x), 1.0)), math_compare<">=">(kern::formula([](Float cy, Float y) -> Float { return kern::math::abs((cy - y)); }, cy, vector_break(ui_.grid_at).y), 1.0))) {
        ui_.grid_at = vector_make(cx, cy, 0.0);
        p = List<Float>{};
        n = 20;
        for (auto it27262991 = flow_for(n27262991, kern::to<Int>(kern::formula([](Float n) -> Float { return (-n); }, kern::to<Float>(n))), n); it27262991.next();) {
          for (auto it27262992 = flow_for(n27262992, kern::to<Int>(kern::formula([](Float n) -> Float { return (-n); }, kern::to<Float>(n))), kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it27262992.next();) {
            a = kern::to<Float>(n27262992.index);
            b = kern::formula([](Float index) -> Float { return (index + 1.0); }, kern::to<Float>(n27262992.index));
            o = kern::to<Float>(n27262991.index);
            place = list_length<Float>(p);
            item = kern::formula([](Float cx, Float a) -> Float { return (cx + a); }, cx, a);
            item_2 = kern::formula([](Float cy, Float o) -> Float { return (cy + o); }, cy, o);
            item_3 = kern::formula([](Float the_result) -> Float { return (the_result + 0.03); }, city_stream_cpp::CityStream_ground_height(city_, kern::formula([](Float cx, Float a) -> Float { return (cx + a); }, cx, a), kern::formula([](Float cy, Float o) -> Float { return (cy + o); }, cy, o)).Result);
            item_4 = kern::formula([](Float cx, Float b) -> Float { return (cx + b); }, cx, b);
            item_5 = kern::formula([](Float cy, Float o) -> Float { return (cy + o); }, cy, o);
            item_6 = kern::formula([](Float the_result) -> Float { return (the_result + 0.03); }, city_stream_cpp::CityStream_ground_height(city_, kern::formula([](Float cx, Float b) -> Float { return (cx + b); }, cx, b), kern::formula([](Float cy, Float o) -> Float { return (cy + o); }, cy, o)).Result);
            list_insert_list(p, place, list_make<Float, "6">(item, item_2, item_3, item_4, item_5, item_6));
            place_2 = list_length<Float>(p);
            item_7 = kern::formula([](Float cx, Float o) -> Float { return (cx + o); }, cx, o);
            item_8 = kern::formula([](Float cy, Float a) -> Float { return (cy + a); }, cy, a);
            item_9 = kern::formula([](Float the_result) -> Float { return (the_result + 0.03); }, city_stream_cpp::CityStream_ground_height(city_, kern::formula([](Float cx, Float o) -> Float { return (cx + o); }, cx, o), kern::formula([](Float cy, Float a) -> Float { return (cy + a); }, cy, a)).Result);
            item_10 = kern::formula([](Float cx, Float o) -> Float { return (cx + o); }, cx, o);
            item_11 = kern::formula([](Float cy, Float b) -> Float { return (cy + b); }, cy, b);
            item_12 = kern::formula([](Float the_result) -> Float { return (the_result + 0.03); }, city_stream_cpp::CityStream_ground_height(city_, kern::formula([](Float cx, Float o) -> Float { return (cx + o); }, cx, o), kern::formula([](Float cy, Float b) -> Float { return (cy + b); }, cy, b)).Result);
            list_insert_list(p, place_2, list_make<Float, "6">(item_7, item_8, item_9, item_10, item_11, item_12));
          }
        }
        a_2 = List<Float>{};
        ui_draw_helpers_line_mesh(ui_.grid_mesh, p, a_2, r, false, Vector{0.55, 0.85, 1.0});
      }
    } else if (math_compare<"!=">(kern::to<Float>(ui_.grid_mesh), 0.0)) {
      engine_renderer_set_visible(n27262993, r, ui_.grid_mesh, false);
      set_visible = n27262993.result;
      ui_.grid_at = Vector{1000000000.0, 1000000000.0, 0.0};
    }
    if (logic_and(settings::Settings_on(ui_.settings, Opt::debug_colliders).Result, world_placed_)) {
      ui_.collider_clock = kern::formula([](Float collider_clock) -> Float { return (collider_clock + 1.0); }, ui_.collider_clock);
      if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float collider_clock) -> Float { return kern::math::mod(kern::math::trunc(collider_clock), 6.0); }, ui_.collider_clock)))), 0.0)) {
        pos = List<Float>{};
        col = List<Float>{};
        engine_game_world_debug_lines(n27262994, world_, true, false, false, pos, col);
        pos = n27262994.positions;
        col = n27262994.colors;
        ui_draw_helpers_line_mesh(ui_.collider_mesh, pos, col, r, true, Vector{0.0, 0.0, 0.0});
      }
    } else if (math_compare<"!=">(kern::to<Float>(ui_.collider_mesh), 0.0)) {
      engine_renderer_set_visible(n27262995, r, ui_.collider_mesh, false);
      set_visible_2 = n27262995.result;
    }
  }

  void ui_camera(Float dt = 0.0) {
    Vector p = Vector{0.0, 0.0, 0.0};
    Float h = 0.0;
    Float fx = 0.0;
    Float fy = 0.0;
    Float gz = 0.0;
    Float fast = 0.0;
    Vector eye = Vector{0.0, 0.0, 0.0};
    Vector look = Vector{0.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 0.0};
    Float fov = 0.0;
    Float back = 0.0;
    Float high = 0.0;
    if (logic_or(logic_or(logic_or(logic_not(gps_live::GpsLive_active(ui_.gps).Result), logic_not(gps_live::GpsLive_has_fix(ui_.gps).Result)), enum_equal<GpsLive_View>(ui_.gps.view, GpsLive_View::eye)), logic_not(enum_equal<Mode>(mode_, Mode::foot)))) {
      return;
    }
    p = gps_live::GpsLive_shown(ui_.gps).Result;
    h = gps_live::GpsLive_heading(ui_.gps).Result;
    fx = kern::formula([](Float h) -> Float { return kern::math::sin(h); }, h);
    fy = kern::formula([](Float h) -> Float { return kern::math::cos(h); }, h);
    gz = city_stream_cpp::CityStream_ground_height(city_, vector_break(p).x, vector_break(p).y).Result;
    fast = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 12.0); }, gps_live::GpsLive_speed(ui_.gps).Result), 0.0, 1.0));
    eye = Vector{0.0, 0.0, 0.0};
    look = Vector{0.0, 0.0, 0.0};
    up = Vector{0.0, 0.0, 1.0};
    fov = 62.0;
    if (enum_equal<GpsLive_View>(ui_.gps.view, GpsLive_View::follow)) {
      back = kern::formula([](Float fast) -> Float { return (9.0 + (10.0 * fast)); }, fast);
      high = kern::formula([](Float fast) -> Float { return (6.0 + (6.0 * fast)); }, fast);
      eye = vector_make(kern::formula([](Float x, Float fx, Float back) -> Float { return (x - (fx * back)); }, vector_break(p).x, fx, back), kern::formula([](Float y, Float fy, Float back) -> Float { return (y - (fy * back)); }, vector_break(p).y, fy, back), kern::formula([](Float gz, Float high) -> Float { return (gz + high); }, gz, high));
      look = vector_make(kern::formula([](Float x, Float fx, Float fast) -> Float { return (x + (fx * (6.0 + (10.0 * fast)))); }, vector_break(p).x, fx, fast), kern::formula([](Float y, Float fy, Float fast) -> Float { return (y + (fy * (6.0 + (10.0 * fast)))); }, vector_break(p).y, fy, fast), kern::formula([](Float gz) -> Float { return (gz + 1.0); }, gz));
    } else {
      eye = vector_make(kern::formula([](Float x, Float fx) -> Float { return (x - (fx * 0.01)); }, vector_break(p).x, fx), kern::formula([](Float y, Float fy) -> Float { return (y - (fy * 0.01)); }, vector_break(p).y, fy), kern::formula([](Float gz, Float fast) -> Float { return ((gz + 90.0) + (160.0 * fast)); }, gz, fast));
      look = vector_make(vector_break(p).x, vector_break(p).y, gz);
      up = vector_make(fx, fy, 0.0);
      fov = 55.0;
    }
    if (logic_not(ui_.gps_cam_init)) {
      ui_.gps_cam_eye = eye;
      ui_.gps_cam_look = look;
      ui_.gps_cam_init = true;
    }
    ui_.gps_cam_eye = vector_add(ui_.gps_cam_eye, vector_scale(vector_sub(eye, ui_.gps_cam_eye), 0.18));
    ui_.gps_cam_look = vector_add(ui_.gps_cam_look, vector_scale(vector_sub(look, ui_.gps_cam_look), 0.25));
    cam_.eye = ui_.gps_cam_eye;
    cam_.target = ui_.gps_cam_look;
    cam_.up = up;
    cam_.fov = kern::formula([](Float fov, Float kDeg) -> Float { return (fov * kDeg); }, fov, kDeg);
    cam_.near = 0.25;
  }

  void ui_before_input(Int e = 0) {
    ui_.head_yaw_before = head_yaw_;
    ui_.head_pitch_before = head_pitch_;
  }

  void ui_clear_police() {
    for (auto it27262996 = flow_for(n27262996, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it27262996.next();) {
      if (logic_and(math_compare<">=">(kern::to<Float>(units_[n27262996.index].car), 0.0), math_compare<"!=">(kern::to<Float>(units_[n27262996.index].car), kern::to<Float>(car_index_)))) {
        engine_game_world_set_alive(world_, cars_[units_[n27262996.index].car].actor, false);
        cars_[units_[n27262996.index].car].placed = false;
      }
      if (math_compare<">=">(kern::to<Float>(units_[n27262996.index].car), 0.0)) {
        cars_[units_[n27262996.index].car].unit = -1;
      }
    }
    list_clear(units_);
    for (auto it27262997 = flow_for(n27262997, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it27262997.next();) {
      pdrones_[n27262997.index].active = false;
    }
    drone_wave_live_ = false;
    next_drone_wave_ = -1.0;
    star_since_ = -1.0;
  }

  void ui_mix_cues() {
    Float fx = 0.0;
    List<game_SoundCue> list = List<game_SoundCue>{};
    List<game_SoundCue> sound_cues = List<game_SoundCue>{};
    fx = kern::formula([](Float the_result) -> Float { return kern::math::max(0.01, kern::math::div(the_result, 72.0)); }, settings::Settings_get(ui_.settings, Opt::audio_fx).Result);
    engine_game_world_sound_cues(n27262999, world_);
    sound_cues = n27262999.result;
    list = sound_cues;
    for (auto it27263000 = flow_for(n27263000, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_SoundCue>(list))))); it27263000.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(list[n27263000.index].actor), kern::to<Float>(soldier_)), logic_or(logic_or(text_equal(list[n27263000.index].sound, "sc:shot0"), text_equal(list[n27263000.index].sound, "sc:pistol0")), text_equal(list[n27263000.index].sound, "builtin:sniper")))) {
        osfx("foot.shot", list[n27263000.index].position, kern::to<Float>(enum_index<weapons_Id>(foot_weapon_)), 0.0, 0.0);
      }
      if (logic_or(text_equal(list[n27263000.index].sound, "builtin:step"), text_equal(list[n27263000.index].sound, "builtin:land"))) {
        list[n27263000.index].volume = kern::formula([](Float a, Float the_result, Float fx) -> Float { return (a * kern::math::div(kern::math::div(the_result, 30.0), fx)); }, list[n27263000.index].volume, settings::Settings_get(ui_.settings, Opt::audio_steps).Result, fx);
      } else if (logic_or(logic_or(logic_or(logic_or(text_equal(list[n27263000.index].sound, "builtin:pistol"), text_equal(list[n27263000.index].sound, "builtin:smg")), text_equal(list[n27263000.index].sound, "builtin:sniper")), text_equal(list[n27263000.index].sound, "builtin:rifle")), text_equal(list[n27263000.index].sound, "builtin:shotgun"))) {
        list[n27263000.index].volume = kern::formula([](Float a, Float the_result, Float fx) -> Float { return (a * kern::math::div(kern::math::div(the_result, 82.0), fx)); }, list[n27263000.index].volume, settings::Settings_get(ui_.settings, Opt::audio_shots).Result, fx);
      }
    }
  }

  struct ui_hud_jsonResult { Text Result = ""; };
  ui_hud_jsonResult ui_hud_json() {
    Text b = "";
    Text s = "";
    Text view = "";
    b = "";
    s = "";
    b = text_format<",\"ui\":{\"placed\":%s,\"paused\":%s,\"settings\":%u,\"resets\":%u,\"moves\":%u,\"origin\":[%.6f,%.6f],\"northUp\":%s,\"assists\":%u,\"audioOn\":%s,\"musicOn\":%s}">(511, logic_select<Text>(world_placed_, "true", "false"), logic_select<Text>(ui_.paused, "true", "false"), settings::Settings_version(ui_.settings).Result, ui_.resets, ui_.moves, lat_, lon_, logic_select<Text>(settings::Settings_on(ui_.settings, Opt::view_minimap_north).Result, "true", "false"), ui_.assists, logic_select<Text>(settings::Settings_on(ui_.settings, Opt::audio_on).Result, "true", "false"), logic_select<Text>(settings::Settings_on(ui_.settings, Opt::music_on).Result, "true", "false"));
    s = text_append(s, b, "");
    if (gps_live::GpsLive_active(ui_.gps).Result) {
      view = logic_select<Text>(enum_equal<GpsLive_View>(ui_.gps.view, GpsLive_View::eye), "AUGE", logic_select<Text>(enum_equal<GpsLive_View>(ui_.gps.view, GpsLive_View::follow), "FOLGEN", "KARTE"));
      b = text_format<",\"gps\":{\"fix\":%s,\"activity\":\"%s\",\"speed\":%.2f,\"distance\":%.1f,\"time\":%.0f,\"accuracy\":%.0f,\"view\":\"%s\",\"accepted\":%u,\"rejected\":%u}">(511, logic_select<Text>(gps_live::GpsLive_has_fix(ui_.gps).Result, "true", "false"), gps_live_cpp::GpsLive_activity_label(ui_.gps).Result, gps_live::GpsLive_speed(ui_.gps).Result, gps_live::GpsLive_distance(ui_.gps).Result, gps_live::GpsLive_elapsed(ui_.gps, time_).Result, gps_live::GpsLive_accuracy(ui_.gps).Result, view, ui_.gps.accepted, ui_.gps.rejected);
      s = text_append(s, b, "");
    }
    n27263002 = ui_equipment_json();
    return {text_append(s, n27263002.Result, "")};
  }

  struct car_max_hpResult { Float Result = 0.0; };
  KERN_PURE car_max_hpResult car_max_hp(const TrafficCar& c) {
    return {kern::formula([](Float police, Float a) -> Float { return ((police != 0.0) ? 300.0 : ((a != 0.0) ? 320.0 : 180.0)); }, kern::to<Float>(c.police), kern::to<Float>(game_vehicle_damage_cpp::is_bus(c).Result))};
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

  struct vr_menu_jsonResult { Text Result = ""; };
  vr_menu_jsonResult vr_menu_json() {
    Text b = "";
    b = "";
    b = text_format<",\"vr\":{\"menu\":%d,\"page\":%d,\"hover\":%d,\"arms\":%d,\"smooth\":%d,\"level\":%d,\"tunnel\":%d,\"day\":%d,\"sound\":%d,\"music\":%d,\"rocket\":%d,\"reset\":%.2f,\"pulse\":[%.2f,%.2f],\"near\":%d,\"mic\":%d,\"cam\":\"%s\",\"action\":\"%s\"}">(479, kern::to<Int>(kern::formula([](Float vr_menu_open_) -> Float { return ((vr_menu_open_ != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(vr_menu_open_))), vr_menu_page_, vr_menu_hover_, kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::vr_arms).Result))), kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::vr_smooth_turn).Result))), kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::vr_level).Result))), kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::vr_tunnel).Result))), kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::world_always_day).Result))), kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::audio_on).Result))), kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::music_on).Result))), kern::to<Int>(kern::formula([](Float rocket) -> Float { return ((rocket != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(ctl_.rocket))), kern::formula([](Float vr_y_fired_, Float a) -> Float { return ((vr_y_fired_ != 0.0) ? 1.0 : a); }, kern::to<Float>(vr_y_fired_), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float vr_y_held_, Float kResetHold) -> Float { return kern::math::div(vr_y_held_, kResetHold); }, vr_y_held_, kResetHold), 0.0, 1.0)), vr_pulse_[0], vr_pulse_[1], kern::to<Int>(kern::formula([](Float vr_near_vehicle_) -> Float { return ((vr_near_vehicle_ != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(vr_near_vehicle_))), kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(voice_mic_on().Result))), logic_select<Text>(enum_equal<Mode>(mode_, Mode::drone), logic_select<Text>(math_compare<"==">(kern::to<Float>(drone_cam_), 0.0), "FPV", logic_select<Text>(math_compare<"==">(kern::to<Float>(drone_cam_), 1.0), "FOLGEN", "AUSSEN")), logic_select<Text>(enum_equal<Mode>(mode_, Mode::car), logic_select<Text>(car_cockpit_, "INNEN", "AUSSEN"), logic_select<Text>(enum_equal<Mode>(mode_, Mode::jet), logic_select<Text>(jet_cockpit_, "COCKPIT", "AUSSEN"), ""))), vr_action_);
    vr_action_ = "";
    vr_pulse_[1] = 0.0;
    vr_pulse_[0] = vr_pulse_[1];
    return {b};
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
    n29360141 = vr_hand_matrix(m, 1);
    if (logic_not(n29360141.Result)) {
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
      n29360143 = vr_hand_matrix(m, 1);
      both_2 = n29360143.Result;
    }
    on = both_2;
    engine_engine_set_xr_scope(engine_, false);
    if (logic_not(on)) {
      if (math_compare<"!=">(kern::to<Float>(vr_scope_mesh_), 0.0)) {
        engine_renderer_set_visible(n29360144, r, vr_scope_mesh_, false);
        set_visible = n29360144.result;
      }
      return;
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(vr_scope_tex_), 0.0))) {
      fbo = 0;
      engine_renderer_create_render_target(n29360145, r, 512, fbo);
      create_render_target = n29360145.result;
      fbo = n29360145.framebuffer;
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
      for (auto it29360146 = flow_for(n29360146, 0, kSeg); it29360146.next();) {
        a = kern::formula([](Float index, Float kSeg, Float kPi) -> Float { return ((kern::math::div(index, kSeg) * 2.0) * kPi); }, kern::to<Float>(n29360146.index), kern::to<Float>(kSeg), kPi);
        engine_geo_mesh_data::MeshData_push(d, vector_make(kern::formula([](Float a) -> Float { return (kern::math::cos(a) * 0.5); }, a), 0.0, kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.5); }, a)), n, Vector{1.0, 1.0, 1.0}, vector_make(kern::formula([](Float a) -> Float { return (0.5 + (kern::math::cos(a) * 0.5)); }, a), kern::formula([](Float a) -> Float { return (0.5 - (kern::math::sin(a) * 0.5)); }, a), 0.0));
        if (math_compare<">">(kern::to<Float>(n29360146.index), 0.0)) {
          place = list_length<Int>(d.indices);
          item = 0;
          item_2 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n29360146.index)));
          item_3 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32((index + 1.0)); }, kern::to<Float>(n29360146.index)));
          list_insert_list(d.indices, place, list_make<Int, "3">(item, item_2, item_3));
        }
      }
      engine_renderer_allocate_id(n29360147, r);
      allocate_id = n29360147.result;
      vr_scope_mesh_ = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
      look.material.unlit = true;
      look.material.double_sided = true;
      look.material.cast_shadow = false;
      look.material.frustum_culled = false;
      look.extra_slot = kMaxExtensionAttributes;
      look.texture = vr_scope_tex_;
      engine_upsert(n29360148, r, vr_scope_mesh_, d, look, List<Float>{}, List<Float>{});
      upsert = n29360148.result;
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
    engine_renderer_update_instances(n29360149, r, vr_scope_mesh_, lm, List<Float>{});
    update_instances = n29360149.result;
    engine_renderer_set_visible(n29360150, r, vr_scope_mesh_, true);
    set_visible_2 = n29360150.result;
  }

  void vr_update_view() {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool both = false;
    if (math_compare<"!=">(kern::to<Float>(engine_), 0.0)) {
      vr_scope_update();
    }
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    both = logic_and(logic_and(vr_, enum_equal<Mode>(mode_, Mode::foot)), logic_not(game_weapon_in_hand().Result));
    if (both) {
      n29360152 = vr_hand_matrix(m, 1);
      both = n29360152.Result;
    }
    if (both) {
      engine_game_world_set_view_hand(world_, list_make<Mat4, "1">(m));
    } else {
      engine_game_world_set_view_hand(world_, List<Mat4>{});
    }
    if (math_compare<"!=">(kern::to<Float>(engine_), 0.0)) {
      engine_engine_set_xr_follow_head(engine_, logic_and(logic_and(vr_, enum_equal<Mode>(mode_, Mode::foot)), logic_not(player_dead_)));
      engine_engine_set_xr_hold_room(engine_, logic_and(vr_, player_dead_));
      engine_engine_set_xr_full_room(engine_, logic_and(logic_and(logic_and(vr_, logic_not(player_dead_)), logic_not(settings::Settings_on(ui_.settings, Opt::vr_level).Result)), logic_or(logic_and(enum_equal<Mode>(mode_, Mode::jet), jet_cockpit_), logic_and(enum_equal<Mode>(mode_, Mode::drone), math_compare<"==">(kern::to<Float>(drone_cam_), 0.0)))));
    }
  }

  void init_weapons() {
    List<Loadout> list = List<Loadout>{};
    list = kFoot;
    for (auto it30408714 = flow_for(n30408714, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Loadout>(list))))); it30408714.next();) {
      ammo_mag_[enum_index<weapons_Id>(list[n30408714.index].id)] = list[n30408714.index].magazine;
      ammo_reserve_[enum_index<weapons_Id>(list[n30408714.index].id)] = list[n30408714.index].reserve;
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
    engine_game_world_soldier_state(n30408715, world_, soldier_, st);
    soldier_state = n30408715.result;
    st = n30408715.out;
    if (logic_and(logic_and(soldier_state, game_weapons_cpp::hitscan_on_foot(foot_weapon_).Result), math_compare<">=">(kern::to<Float>(st.ammo), 0.0))) {
      ammo_mag_[enum_index<weapons_Id>(foot_weapon_)] = st.ammo;
      ammo_reserve_[enum_index<weapons_Id>(foot_weapon_)] = st.reserve;
    }
    foot_weapon_ = id;
    s = list_get<weapons_Spec>(kTable, enum_index<weapons_Id>(id));
    kind = logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::smg), "rifle", logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::sniper), "sniper", logic_select<Text>(logic_or(enum_equal<weapons_Id>(id, weapons_Id::grenade_launcher), enum_equal<weapons_Id>(id, weapons_Id::rocket)), "shotgun", "pistol")));
    engine_game_world_set_property_text(n30408716, world_, soldier_, "components.weapon.kind", kind);
    set_property_text = n30408716.result;
    if (game_weapons_cpp::hitscan_on_foot(id).Result) {
      engine_game_world_set_property_number(n30408717, world_, soldier_, "components.weapon.damage", s.damage);
      set_property_number = n30408717.result;
      engine_game_world_set_property_number(n30408718, world_, soldier_, "components.weapon.fireRate", kern::formula([](Float interval_ms) -> Float { return kern::math::div(60000.0, interval_ms); }, s.interval_ms));
      set_property_number_2 = n30408718.result;
      engine_game_world_set_property_flag(n30408719, world_, soldier_, "components.weapon.automatic", enum_equal<weapons_Id>(id, weapons_Id::smg));
      set_property_flag = n30408719.result;
      engine_game_world_set_property_number(n30408720, world_, soldier_, "components.weapon.range", s.range_m);
      set_property_number_3 = n30408720.result;
      engine_game_world_set_property_number(n30408721, world_, soldier_, "components.weapon.headshot", 3.0);
      set_property_number_4 = n30408721.result;
      list = kFoot;
      for (auto it30408722 = flow_for(n30408722, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Loadout>(list))))); it30408722.next();) {
        if (enum_equal<weapons_Id>(list[n30408722.index].id, id)) {
          engine_game_world_set_property_number(n30408723, world_, soldier_, "components.weapon.magazine", kern::to<Float>(list[n30408722.index].magazine));
          set_property_number_5 = n30408723.result;
        }
      }
      engine_game_world_set_property_text(n30408724, world_, soldier_, "components.weapon.fireSound", logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::smg), "sc:shot0", logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::sniper), "builtin:sniper", "sc:pistol0")));
      set_property_text_2 = n30408724.result;
      engine_game_world_set_property_number(n30408725, world_, soldier_, "components.weapon.pellets", 1.0);
      set_property_number_6 = n30408725.result;
      engine_game_world_set_property_number(n30408726, world_, soldier_, "components.weapon.aimFov", kern::formula([](Float a) -> Float { return ((a != 0.0) ? 8.09 : 42.6); }, kern::to<Float>(enum_equal<weapons_Id>(id, weapons_Id::sniper))));
      set_property_number_7 = n30408726.result;
      engine_game_world_set_property_number(n30408727, world_, soldier_, "components.weapon.spread", 0.0);
      set_property_number_8 = n30408727.result;
      engine_game_world_set_property_number(n30408728, world_, soldier_, "components.weapon.aimSpread", 0.0);
      set_property_number_9 = n30408728.result;
      engine_game_world_set_property_number(n30408729, world_, soldier_, "components.weapon.recoil", 0.0);
      set_property_number_10 = n30408729.result;
      engine_game_world_soldier_state(n30408730, world_, soldier_, st);
      soldier_state_2 = n30408730.result;
      st = n30408730.out;
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
    for (auto it30408731 = flow_for(n30408731, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it30408731.next();) {
      if (enum_equal<weapons_Id>(struct_break<Loadout>(list_get<Loadout>(kFoot, n30408731.index)).id, foot_weapon_)) {
        cur = n30408731.index;
      }
    }
    select_foot_weapon(struct_break<Loadout>(list_get<Loadout>(kFoot, kern::to<Int>(kern::formula([](Float cur, Float step, Float n) -> Float { return kern::math::mod((kern::math::mod((cur + step), n) + n), n); }, kern::to<Float>(cur), kern::to<Float>(step), kern::to<Float>(n))))).id);
  }

  struct game_weapon_in_handResult { Bool Result = false; };
  KERN_PURE game_weapon_in_handResult game_weapon_in_hand() {
    return {logic_and(enum_equal<Mode>(mode_, Mode::foot), logic_not(game_weapons_cpp::hitscan_on_foot(foot_weapon_).Result))};
  }

  struct aim_rayResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  aim_rayResult aim_ray(Vector& dir) {
    Bool xr = false;
    Bool xr_2 = false;
    Bool both = false;
    Bool xr_3 = false;
    Bool xr_4 = false;
    Bool both_2 = false;
    Bool either = false;
    Vector from = Vector{0.0, 0.0, 0.0};
    either = ctl_.aim_point;
    if (logic_not(either)) {
      both_2 = math_compare<"!=">(kern::to<Float>(engine_), 0.0);
      if (both_2) {
        xr_4 = engine_engine_xr(engine_);
        both_2 = xr_4;
      }
      either = both_2;
    }
    if (either) {
      from = Vector{0.0, 0.0, 0.0};
      n30408732 = ctl_screen_ray(from, dir, ctl_.aim_x, ctl_.aim_y);
      return {from};
    }
    dir = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{1.0, 0.0, 0.0});
    return {cam_.eye};
  }

  void fire_projectile(weapons_Id id = weapons_Id::smg, Vector from = Vector{0.0, 0.0, 0.0}, Vector vel = Vector{0.0, 0.0, 0.0}, Bool by_player = false, physics_Body ignore = physics_Body{Int64{0x0}}) {
    Projectile p = Projectile{weapons_Id::grenade_launcher, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 12.0, true, false, false, true, 0, false, 0.0, -1.0, physics_Body{Int64{0x0}}, 0, false};
    Float sim_time = 0.0;
    Text snd = "";
    Float sim_time_2 = 0.0;
    p = Projectile{weapons_Id::grenade_launcher, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 12.0, true, false, false, true, 0, false, 0.0, -1.0, physics_Body{Int64{0x0}}, 0, false};
    p.id = id;
    p.pos = from;
    p.vel = vel;
    sim_time = engine_game_world_sim_time(world_);
    p.born = sim_time;
    p.by_player = by_player;
    p.ignore = ignore;
    p.gravity = logic_or(logic_or(enum_equal<weapons_Id>(id, weapons_Id::grenade_launcher), enum_equal<weapons_Id>(id, weapons_Id::hand_grenade)), enum_equal<weapons_Id>(id, weapons_Id::jet_bomb));
    p.fuse = kern::formula([](Float a, Float a_2, Float kGrenadeFuse, Float a_3, Float a_4, Float a_5) -> Float { return ((a != 0.0) ? 5.0 : ((a_2 != 0.0) ? kGrenadeFuse : ((a_3 != 0.0) ? 7.2 : ((a_4 != 0.0) ? 2.8 : ((a_5 != 0.0) ? 9.0 : 12.0))))); }, kern::to<Float>(enum_equal<weapons_Id>(id, weapons_Id::jet_missile)), kern::to<Float>(enum_equal<weapons_Id>(id, weapons_Id::hand_grenade)), kGrenadeFuse, kern::to<Float>(enum_equal<weapons_Id>(id, weapons_Id::drone_rockets)), kern::to<Float>(enum_equal<weapons_Id>(id, weapons_Id::grenade_launcher)), kern::to<Float>(enum_equal<weapons_Id>(id, weapons_Id::rocket)));
    if (enum_equal<weapons_Id>(id, weapons_Id::hand_grenade)) {
      p.next_collision = kern::formula([](Float born) -> Float { return (born + 0.06); }, p.born);
    }
    p.guided = enum_equal<weapons_Id>(id, weapons_Id::drone_guided);
    proj_serial_ = kern::to<Int>(kern::formula([](Float proj_serial_) -> Float { return kern::math::wrap32(proj_serial_ + 1.0); }, kern::to<Float>(proj_serial_)));
    p.net = kern::to<Int>(kern::formula([](Float proj_serial_) -> Float { return kern::math::band(proj_serial_, 32767.0); }, kern::to<Float>(proj_serial_)));
    list_add(projectiles_, p);
    snd = logic_select<Text>(logic_or(enum_equal<weapons_Id>(id, weapons_Id::hand_grenade), enum_equal<weapons_Id>(id, weapons_Id::jet_bomb)), "builtin:jump", "builtin:launcher");
    engine_game_world_play_sound(world_, snd, from, 0.9, kern::formula([](Float a) -> Float { return ((a != 0.0) ? 0.8 : 1.0); }, kern::to<Float>(enum_equal<weapons_Id>(id, weapons_Id::jet_missile))), true, 0);
    osfx("launch", from, kern::to<Float>(enum_index<weapons_Id>(id)), kern::formula([](Float by_player) -> Float { return ((by_player != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(by_player)), 0.0);
    if (by_player) {
      sim_time_2 = engine_game_world_sim_time(world_);
      people_cpp::People_frighten(people_, from, 18.0, sim_time_2, 1.0);
    }
  }

  void throw_grenade() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Vector dir = Vector{0.0, 0.0, 0.0};
    Vector eye = Vector{0.0, 0.0, 0.0};
    Vector v = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    if (logic_or(logic_or(math_compare<"<=">(kern::to<Float>(grenades_), 0.0), math_compare<"<">(now, next_grenade_)), player_dead_)) {
      toast_ = logic_select<Text>(math_compare<"<=">(kern::to<Float>(grenades_), 0.0), "Keine Granaten mehr", "");
      toast_age_ = kern::formula([](Float grenades_) -> Float { return ((grenades_ <= 0.0) ? 0.0 : 99.0); }, kern::to<Float>(grenades_));
      return;
    }
    grenades_ = kern::to<Int>(kern::formula([](Float grenades_) -> Float { return (grenades_ - 1.0); }, kern::to<Float>(grenades_)));
    next_grenade_ = kern::formula([](Float now, Float interval_ms) -> Float { return (now + kern::math::div(interval_ms, 1000.0)); }, now, struct_break<weapons_Spec>(list_get<weapons_Spec>(kTable, enum_index<weapons_Id>(weapons_Id::hand_grenade))).interval_ms);
    dir = Vector{0.0, 0.0, 0.0};
    n30408743 = aim_ray(dir);
    eye = n30408743.Result;
    the_actor_velocity = engine_game_world_actor_velocity(world_, soldier_);
    v = vector_add(vector_add(vector_scale(dir, kGrenadeThrow), Vector{0.0, 0.0, 3.5}), the_actor_velocity);
    fire_projectile(weapons_Id::hand_grenade, vector_add(eye, vector_scale(dir, 0.5)), v, true, body(soldier_).Result);
  }

  void draw_projectiles() {
    for (auto it30408749 = flow_for(n30408749, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it30408749.next();) {
      switch (flow_switch_enum<weapons_Id>(projectiles_[n30408749.index].id)) {
        case weapons_Id::smg: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::glock: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::sniper: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::grenade_launcher: {
        } break;
        case weapons_Id::rocket: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::fists: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::knife: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::hand_grenade: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.05, 0.14, false);
        } break;
        case weapons_Id::drone_gun: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::drone_rockets: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::drone_guided: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::jet_cannon: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::jet_missile: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.12, 2.8, true);
        } break;
        case weapons_Id::jet_bomb: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408749.index].pos, projectiles_[n30408749.index].vel, 0.25, 1.9, false);
        } break;
      }
    }
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

  void Game_step_world_water(Int w = 0, Int slot = 0, Int a = 0, Vector half = Vector{0.0, 0.0, 0.0}, Bool drone = false) {
    Int w_ = 0;
    Bool actor_alive = false;
    Bool actor_alive_2 = false;
    Bool either = false;
    physics_Body b = physics_Body{Int64{0x0}};
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float sub = 0.0;
    Float m = 0.0;
    Float mass = 0.0;
    Vector flow = Vector{0.0, 0.0, 0.0};
    Vector rel = Vector{0.0, 0.0, 0.0};
    Float speed = 0.0;
    Float k = 0.0;
    Float volume = 0.0;
    Float lift = 0.0;
    w_ = w;
    either = logic_or(math_compare<">=">(kern::to<Float>(slot), 32.0), logic_not(math_compare<"!=">(kern::to<Float>(a), 0.0)));
    if (logic_not(either)) {
      actor_alive_2 = engine_game_world_actor_alive(world_, a);
      either = logic_not(actor_alive_2);
    }
    if (either) {
      return;
    }
    b = body(a).Result;
    if (logic_not(engine_physics_physics::Body_valid(b).Result)) {
      return;
    }
    s = common::read_body(w_, b).Result;
    sub = 0.0;
    if (terrain_cpp::Terrain_water_at(city_.terrain_, vector_break(s.position).x, vector_break(s.position).y).Result) {
      sub = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float the_result, Float z, Float z_2) -> Float { return kern::math::div((the_result - (z - z_2)), (2.0 * z_2)); }, terrain_cpp::Terrain_water_level(city_.terrain_, vector_break(s.position).x, vector_break(s.position).y).Result, vector_break(s.position).z, vector_break(half).z), 0.0, 1.0));
    }
    if (math_compare<">">(sub, 0.0)) {
      mass = engine_world_mass(w_, b);
      m = mass;
      flow = terrain_cpp::Terrain_flow_at(city_.terrain_, vector_break(s.position).x, vector_break(s.position).y).Result;
      rel = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(s.velocity).x, vector_break(flow).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(s.velocity).y, vector_break(flow).y), vector_break(s.velocity).z);
      speed = vector_length(rel);
      k = kern::formula([](Float sub, Float speed, Float m) -> Float { return (((sub * (1.6 + (0.9 * speed))) * m) * 0.35); }, sub, speed, m);
      volume = kern::formula([](Float x, Float y, Float z) -> Float { return (((8.0 * x) * y) * z); }, vector_break(half).x, vector_break(half).y, vector_break(half).z);
      lift = kern::formula([](Float drone, Float kWaterDensity, Float kG, Float volume, Float sub) -> Float { return ((drone != 0.0) ? 0.0 : ((((kWaterDensity * kG) * volume) * sub) * 0.62)); }, kern::to<Float>(drone), kWaterDensity, kG, volume, sub);
      engine_world_apply_force_to_center(w_, b, vector_make(kern::formula([](Float x, Float k) -> Float { return ((-x) * k); }, vector_break(rel).x, k), kern::formula([](Float y, Float k) -> Float { return ((-y) * k); }, vector_break(rel).y, k), kern::formula([](Float lift, Float z, Float k) -> Float { return (lift - ((z * k) * 1.4)); }, lift, vector_break(rel).z, k)));
      engine_world_apply_torque(w_, b, vector_make(kern::formula([](Float x, Float m, Float sub) -> Float { return ((((-x) * m) * sub) * 0.8); }, vector_break(s.angular).x, m, sub), kern::formula([](Float y, Float m, Float sub) -> Float { return ((((-y) * m) * sub) * 0.8); }, vector_break(s.angular).y, m, sub), kern::formula([](Float z, Float m, Float sub) -> Float { return ((((-z) * m) * sub) * 0.5); }, vector_break(s.angular).z, m, sub)));
      if (logic_and(math_compare<"<=">(life_[0].in_water[slot], 0.0), math_compare<">">(kern::formula([](Float z) -> Float { return kern::math::abs(z); }, vector_break(s.velocity).z), 1.5))) {
        list_add(life_[0].pending_splashes, struct_make<WorldLife_Splash>(vector_make(vector_break(s.position).x, vector_break(s.position).y, terrain_cpp::Terrain_water_level(city_.terrain_, vector_break(s.position).x, vector_break(s.position).y).Result), kern::formula([](Float z, Float x) -> Float { return kern::math::min(6.0, ((0.4 * kern::math::abs(z)) * (1.0 + x))); }, vector_break(s.velocity).z, vector_break(half).x)));
      }
    }
    life_[0].in_water[slot] = sub;
  }

  void world_bullet(Vector at = Vector{0.0, 0.0, 0.0}) {
    if (math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0)) {
      n31457293 = world_marks_cpp::Marks_bullet(life_[0].marks, city_, rng_, at);
    }
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
    for (auto it32505866 = flow_for(n32505866, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it32505866.next();) {
      skip = false;
      if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n32505866.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        list_add(out, logic_select<Vector>(logic_or(enum_equal<net_PlayerMode>(net_.remotes_[n32505866.index].state.mode, net_PlayerMode::foot), enum_equal<net_PlayerMode>(net_.remotes_[n32505866.index].state.mode, net_PlayerMode::drone)), net_.remotes_[n32505866.index].person_shown, net_.remotes_[n32505866.index].shown));
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

  void car_body_dynamic(TrafficCar& c) {
    Int w = 0;
    Int physics = 0;
    physics_Body b = physics_Body{Int64{0x0}};
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool either = false;
    physics_BodyType body_type = physics_BodyType::fixed;
    physics_BodyType body_type_2 = physics_BodyType::fixed;
    Bool either_2 = false;
    RigidSpec rs = RigidSpec{List<ColliderSpec>{}, MassSpec{1.0, Vector{0.0, 0.0, 0.0}, List<Float>{1.0, 1.0, 1.0, 0.0, 0.0, 0.0}}, 0.0, 0.0, false};
    engine_game_world_physics(n32505867, world_);
    physics = n32505867.result;
    w = physics;
    b = body(c.actor).Result;
    either = logic_not(engine_physics_physics::Body_valid(b).Result);
    if (logic_not(either)) {
      body_valid_2 = engine_world_body_valid(w, b);
      either = logic_not(body_valid_2);
    }
    either_2 = either;
    if (logic_not(either_2)) {
      body_type_2 = engine_world_body_type(w, b);
      either_2 = enum_equal<physics_BodyType>(body_type_2, physics_BodyType::dynamic);
    }
    if (either_2) {
      return;
    }
    engine_world_set_body_type(w, b, physics_BodyType::dynamic);
    n32505868 = car_cpp::Car_rigid_spec(c.car[0].s_);
    rs = n32505868.Result;
    engine_world_set_mass_data(w, b, rs.mass.mass, rs.mass.center, rs.mass.inertia);
    engine_world_set_awake(w, b, true);
    car_cpp::Car_reset(c.car[0]);
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

  struct the_worldsync_list_Result { Text Result = ""; };
  the_worldsync_list_Result the_worldsync_list_() {
    Text o = "";
    Text b = "";
    Bool first = false;
    Vector me = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    Bool actor_alive = false;
    Bool actor_alive_2 = false;
    Bool either = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip_2 = false;
    Int flocks = 0;
    Int pigeons = 0;
    Int pigeons_dead = 0;
    Int cows = 0;
    Bool skip_3 = false;
    Bool skip_4 = false;
    o = text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append("{\"puppet\":", logic_select<Text>(puppet_world_, "1", "0"), ""), ",\"refused\":", ""), text_format<"%u">(-1, net_session::Session_send_refused(net_).Result), ""), ",\"sent\":", ""), text_format<"%u">(-1, world_sent_), ""), ",\"got\":", ""), text_format<"%u">(-1, world_got_), ""), ",\"mates\":", ""), text_format<"%llu">(-1, list_length<Vector>(mate_foci().Result)), ""), ",\"cars\":[", "");
    b = "";
    first = true;
    n32505873 = focus_position();
    me = n32505873.Result;
    for (auto it32505874 = flow_for(n32505874, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it32505874.next();) {
      skip = false;
      either = logic_or(logic_not(cars_[n32505874.index].placed), cars_[n32505874.index].police);
      if (logic_not(either)) {
        actor_alive_2 = engine_game_world_actor_alive(world_, cars_[n32505874.index].actor);
        either = logic_not(actor_alive_2);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        n32505875 = matrix_of(cars_[n32505874.index].actor);
        a = n32505875.Result;
        p = engine_math_math::Mat4_translation(a).Result;
        if (math_compare<">">(game_worldsync_cpp::dist2d(p, me).Result, 260.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          b = text_format<"%s[%.1f,%.1f,%u]">(95, logic_select<Text>(first, "", ","), vector_break(p).x, vector_break(p).y, kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::bor(kern::math::bor(kern::math::wrap32s(kern::math::shl(a, 16.0)), kern::math::wrap32s(kern::math::shl(a_2, 8.0))), a_3); }, kern::to<Float>(game_worldsync_cpp::col8(vector_break(cars_[n32505874.index].color).x).Result), kern::to<Float>(game_worldsync_cpp::col8(vector_break(cars_[n32505874.index].color).y).Result), kern::to<Float>(game_worldsync_cpp::col8(vector_break(cars_[n32505874.index].color).z).Result))));
          o = text_append(o, b, "");
          first = false;
        }
      }
    }
    o = text_append(o, "],\"people\":[", "");
    first = true;
    for (auto it32505876 = flow_for(n32505876, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<People_Person>(people_.people_))))); it32505876.next();) {
      skip_2 = false;
      if (logic_or(logic_or(logic_not(enum_equal<People_Kind>(people_.people_[n32505876.index].kind, People_Kind::citizen)), enum_equal<People_State>(people_.people_[n32505876.index].state, People_State::gone)), math_compare<">">(game_worldsync_cpp::dist2d(people_.people_[n32505876.index].pos, me).Result, 200.0))) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        b = text_format<"%s[%u,%.1f,%.1f]">(95, logic_select<Text>(first, "", ","), people_.people_[n32505876.index].seed, vector_break(people_.people_[n32505876.index].pos).x, vector_break(people_.people_[n32505876.index].pos).y);
        o = text_append(o, b, "");
        first = false;
      }
    }
    o = text_append(o, "],\"pets\":[", "");
    first = true;
    flocks = 0;
    pigeons = 0;
    pigeons_dead = 0;
    cows = 0;
    if (math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)) {
      for (auto it32505877 = flow_for(n32505877, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(glife_[0].life.pets_))))); it32505877.next();) {
        skip_3 = false;
        if (logic_or(logic_not(glife_[0].life.pets_[n32505877.index].shown), math_compare<">">(game_worldsync_cpp::dist2d(glife_[0].life.pets_[n32505877.index].pos, me).Result, 200.0))) {
          skip_3 = true;
        }
        if (logic_not(skip_3)) {
          b = text_format<"%s[%zu,%.2f,%.2f,%d]">(95, logic_select<Text>(first, "", ","), n32505877.index, vector_break(glife_[0].life.pets_[n32505877.index].pos).x, vector_break(glife_[0].life.pets_[n32505877.index].pos).y, kern::to<Int>(kern::formula([](Float a) -> Float { return ((a >= 0.0) ? 1.0 : 0.0); }, glife_[0].life.pets_[n32505877.index].dead_at)));
          o = text_append(o, b, "");
          first = false;
        }
      }
      for (auto it32505878 = flow_for(n32505878, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it32505878.next();) {
        skip_4 = false;
        if (math_compare<">">(game_worldsync_cpp::dist2d(vector_make(vector_break(glife_[0].life.flocks_[n32505878.index].at).x, vector_break(glife_[0].life.flocks_[n32505878.index].at).y, 0.0), me).Result, 250.0)) {
          skip_4 = true;
        }
        if (logic_not(skip_4)) {
          flocks += 1;
          for (auto it32505879 = flow_for(n32505879, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pigeon>(glife_[0].life.flocks_[n32505878.index].birds))))); it32505879.next();) {
            pigeons += 1;
            pigeons_dead += kern::to<Int>(math_compare<">=">(glife_[0].life.flocks_[n32505878.index].birds[n32505879.index].dead_at, 0.0));
          }
        }
      }
      for (auto it32505880 = flow_for(n32505880, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_))))); it32505880.next();) {
        cows += kern::to<Int>(math_compare<"<">(game_worldsync_cpp::dist2d(glife_[0].life.cows_[n32505880.index].pos, me).Result, 250.0));
      }
    }
    o = text_append(o, "],\"flockList\":[", "");
    first = true;
    if (math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)) {
      for (auto it32505881 = flow_for(n32505881, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it32505881.next();) {
        b = text_format<"%s[%u,%zu,%.0f,%.0f,%.0f]">(95, logic_select<Text>(first, "", ","), glife_[0].life.flocks_[n32505881.index].id, list_length<Life_Pigeon>(glife_[0].life.flocks_[n32505881.index].birds), vector_break(glife_[0].life.flocks_[n32505881.index].at).x, vector_break(glife_[0].life.flocks_[n32505881.index].at).y, game_worldsync_cpp::dist2d(vector_make(vector_break(glife_[0].life.flocks_[n32505881.index].at).x, vector_break(glife_[0].life.flocks_[n32505881.index].at).y, 0.0), me).Result);
        o = text_append(o, b, "");
        first = false;
      }
    }
    b = text_format<"],\"me\":[%.0f,%.0f],\"flocks\":%u,\"pigeons\":%u,\"pigeonsDead\":%u,\"cows\":%u,\"epoch\":%.3f}">(95, vector_break(me).x, vector_break(me).y, flocks, pigeons, pigeons_dead, cows, kern::formula([](Float the_result) -> Float { return kern::math::mod(the_result, 100000.0); }, life_epoch().Result));
    return {text_append(o, b, "")};
  }

  struct puppet_car_stepResult { Bool Result = false; };
  puppet_car_stepResult puppet_car_step(TrafficCar& c, Int i = 0, Float h = 0.0) {
    physics_Body b = physics_Body{Int64{0x0}};
    Int w = 0;
    Int physics = 0;
    mp_PoseSample s = mp_PoseSample{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, false, false, false};
    Float now_ms = 0.0;
    physics_BodyType body_type = physics_BodyType::fixed;
    BodyState here = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    if (logic_or(logic_or(logic_or(logic_not(puppet_world_), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (!((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a))); }, kern::to<Float>(i), kern::to<Float>(list_length<Int>(car_net_))), 0.5)), math_compare<"<">(kern::to<Float>(car_net_[i]), 0.0)), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32s(i); }, kern::to<Float>(i))))), kern::to<Float>(car_index_)))) {
      return {false};
    }
    b = body(c.actor).Result;
    if (logic_or(logic_not(engine_physics_physics::Body_valid(b).Result), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (!((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a))); }, kern::to<Float>(i), kern::to<Float>(list_length<mp_PoseTimeline>(car_line_))), 0.5))) {
      return {true};
    }
    engine_game_world_physics(n32505882, world_);
    physics = n32505882.result;
    w = physics;
    now_ms = engine_now_ms();
    s = mp_rules_cpp::PoseTimeline_sample(car_line_[i], now_ms).Result;
    if (logic_not(s.valid)) {
      return {true};
    }
    body_type = engine_world_body_type(w, b);
    if (logic_not(enum_equal<physics_BodyType>(body_type, physics_BodyType::kinematic))) {
      engine_world_set_body_type(w, b, physics_BodyType::kinematic);
    }
    here = common::read_body(w, b).Result;
    if (math_compare<">">(vector_length(vector_sub(s.p, here.position)), 8.0)) {
      engine_world_set_pose(w, b, struct_make<physics_Pose>(s.p, s.q));
      engine_world_set_linear_velocity(w, b, Vector{0.0, 0.0, 0.0});
      engine_world_set_angular_velocity(w, b, Vector{0.0, 0.0, 0.0});
    } else {
      engine_world_set_target_pose(w, b, struct_make<physics_Pose>(s.p, s.q), h);
    }
    linear_velocity = engine_world_linear_velocity(w, b);
    car::Car_roll(c.car[0], vector_dot(linear_velocity, engine_math_math::Quat_rotate(s.q, Vector{1.0, 0.0, 0.0}).Result), h);
    return {true};
  }

  struct paletteResult { List<Int> Result = List<Int>{}; };
  KERN_PURE paletteResult palette() {
    return {mp_rules_g_palette};
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n1048586;
  struct { Bool result = false; game_SoldierState out = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0}; } n2097163;
  matrix_ofResult n2097164;
  matrix_ofResult n2097165;
  struct : kern::Node<2097166> { Int index = 0; } n2097166;
  struct : kern::Node<2097167> { Int index = 0; } n2097167;
  struct : kern::Node<2097168> { Int index = 0; } n2097168;
  matrix_ofResult n2097169;
  struct : kern::Node<2097170> { Int index = 0; } n2097170;
  struct : kern::Node<2097171> { Int index = 0; } n2097171;
  struct : kern::Node<2097172> { Int index = 0; } n2097172;
  struct : kern::Node<2097173> { Int index = 0; } n2097173;
  vr_hand_rayResult n4194317;
  project_screenResult n4194327;
  touch_play_cpp::TouchPlay_view_jsonResult n4194342;
  struct : kern::Node<4194343> { Int index = 0; } n4194343;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n5242890;
  matrix_ofResult n5242891;
  matrix_ofResult n5242892;
  matrix_ofResult n5242893;
  gaps_ride_poseResult n5242895;
  matrix_ofResult n5242896;
  matrix_ofResult n5242897;
  player_positionResult n5242898;
  struct : kern::Node<5242899> { Int index = 0; } n5242899;
  struct { Bool result = false; } n5242903;
  struct { Bool result = false; } n5242904;
  struct { Int result = 0; } n5242905;
  standing_feetResult n5242906;
  struct { Bool result = false; } n5242907;
  struct { Bool result = false; } n5242908;
  matrix_ofResult n5242909;
  matrix_ofResult n5242910;
  standing_feetResult n5242911;
  struct { Bool result = false; } n5242912;
  struct { Bool result = false; } n5242913;
  struct { Bool result = false; } n5242914;
  struct { Bool result = false; } n5242915;
  struct { Bool result = false; } n5242916;
  matrix_ofResult n5242917;
  struct { Int result = 0; } n5242918;
  struct { Int result = 0; } n5242920;
  struct { Int result = 0; } n5242921;
  struct { Bool result = false; } n5242922;
  struct { Bool result = false; } n5242923;
  struct { Bool result = false; } n5242924;
  state_ofResult n5242925;
  struct { Int result = 0; } n5242926;
  state_ofResult n5242927;
  state_ofResult n5242928;
  struct { Bool result = false; } n5242929;
  struct { Bool result = false; } n5242930;
  common::Rng_nextResult n5242935;
  struct { Int result = 0; } n5242936;
  struct : kern::Node<5242938> { Int index = 0; } n5242938;
  struct { Int result = 0; } n5242940;
  struct { List<Float> out = List<Float>{}; } n5242947;
  struct : kern::Node<5242948> { Int index = 0; } n5242948;
  struct : kern::Node<5242949> { Int index = 0; } n5242949;
  struct : kern::Node<5242950> { Int index = 0; } n5242950;
  game_destruction_sync_cpp::digest_ofResult n6291477;
  struct : kern::Node<7340044> { Int index = 0; } n7340044;
  struct : kern::Node<7340045> { Int index = 0; } n7340045;
  struct : kern::Node<7340046> { Int index = 0; } n7340046;
  struct : kern::Node<7340047> { Int index = 0; } n7340047;
  combat_audio_cpp::renderResult n7340048;
  sfx_idResult n7340049;
  struct { Bool result = false; } n7340050;
  sfx_idResult n7340051;
  game_equipment_cpp::render_loopResult n8388618;
  struct { Int result = 0; } n8388619;
  struct : kern::Node<8388620> { Int index = 0; } n8388620;
  struct { List<Float> dst = List<Float>{}; } n8388621;
  struct { List<Float> samples = List<Float>{}; } n8388622;
  struct { Int result = 0; } n8388623;
  struct : kern::Node<8388624> { Int index = 0; } n8388624;
  struct { List<Float> dst = List<Float>{}; } n8388625;
  struct { List<Float> samples = List<Float>{}; } n8388626;
  struct { Int result = 0; } n8388627;
  game_equipment_cpp::render_loopResult n8388628;
  struct { Int result = 0; } n8388629;
  struct : kern::Node<8388630> { Int index = 0; } n8388630;
  common::Rng_unitResult n8388631;
  common::Rng_unitResult n8388632;
  common::Rng_unitResult n8388633;
  common::Rng_unitResult n8388634;
  common::Rng_unitResult n8388635;
  common::Rng_unitResult n8388636;
  struct { List<Float> dst = List<Float>{}; } n8388637;
  struct { List<Float> samples = List<Float>{}; } n8388638;
  struct { List<Float> samples = List<Float>{}; } n8388639;
  struct { Int result = 0; } n8388640;
  game_equipment_cpp::render_loopResult n8388641;
  struct { Int result = 0; } n8388642;
  matrix_ofResult n8388645;
  struct { Int result = 0; } n8388646;
  touch_play_cpp::TouchPlay_take_pressedResult n8388647;
  struct { Int result = 0; } n8388648;
  matrix_ofResult n8388649;
  struct { Int64 result = Int64{0x0}; } n8388650;
  matrix_ofResult n8388651;
  struct : kern::Node<8388652> { Int index = 0; } n8388652;
  struct : kern::Node<8388653> { Int index = 0; } n8388653;
  nostr_cpp::from_hexResult n8388654;
  nostr_cpp::from_hexResult n8388656;
  nostr_cpp::event_jsonResult n8388657;
  struct { Bool result = false; } n8388658;
  struct : kern::Node<9437200> { Int index = 0; } n9437200;
  struct { Debris value = Debris{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 6.5}; } n9437201;
  struct : kern::Node<9437202> { Int index = 0; } n9437202;
  struct : kern::Node<10485770> { Int index = 0; } n10485770;
  struct : kern::Node<10485771> { Int index = 0; } n10485771;
  struct : kern::Node<10485772> { Int index = 0; } n10485772;
  state_ofResult n10485773;
  remote_on_segmentResult n10485774;
  combat_sfxResult n10485775;
  struct : kern::Node<10485776> { Int index = 0; } n10485776;
  ctl_screen_rayResult n10485777;
  struct { Int result = 0; } n10485778;
  rocket_laser_dotResult n10485779;
  struct { Int result = 0; } n10485780;
  people_cpp::People_shotResult n10485781;
  round_meets_mateResult n10485782;
  struct : kern::Node<10485783> { Int index = 0; } n10485783;
  struct { Int result = 0; } n10485784;
  car_near_pointResult n10485786;
  people_cpp::People_shotResult n10485787;
  round_meets_mateResult n10485788;
  struct : kern::Node<10485789> { Int index = 0; } n10485789;
  struct : kern::Node<10485790> { Int index = 0; } n10485790;
  struct : kern::Node<10485803> { Int index = 0; } n10485803;
  struct : kern::Node<10485804> { Int index = 0; } n10485804;
  struct : kern::Node<10485805> { Int index = 0; } n10485805;
  struct { GapsState_SniperRound value = GapsState_SniperRound{game_ShotEvent{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, false, false, Vector{0.0, 0.0, 1.0}, 0}, 0.0}; } n10485806;
  game_gaps_cpp::smoke_discResult n11534346;
  struct { Int result = 0; } n11534347;
  struct { Bool result = false; } n11534348;
  struct { Int result = 0; } n11534349;
  struct { Bool result = false; } n11534350;
  struct { Int result = 0; } n11534351;
  struct { Bool result = false; } n11534352;
  struct : kern::Node<11534353> { Int index = 0; } n11534353;
  struct : kern::Node<11534354> { Int index = 0; } n11534354;
  struct { Bool result = false; } n11534355;
  struct { Bool result = false; } n11534356;
  game_gaps_cpp::Biquad_runResult n11534357;
  struct { List<Float> samples = List<Float>{}; } n11534358;
  struct { List<Float> samples = List<Float>{}; } n11534359;
  struct { Int result = 0; } n11534360;
  struct : kern::Node<11534361> { Int index = 0; } n11534361;
  struct : kern::Node<11534362> { Int index = 0; } n11534362;
  struct { Int64 result = Int64{0x0}; } n11534363;
  struct : kern::Node<11534364> { Int index = 0; } n11534364;
  struct { GapsState_MotorVoice value = GapsState_MotorVoice{0.0, -1.0, 0, false}; } n11534365;
  struct : kern::Node<11534366> { Int index = 0; } n11534366;
  matrix_ofResult n11534369;
  matrix_ofResult n11534370;
  gaps_rules_cpp::SmokeField_startResult n11534371;
  gaps_rules_cpp::SmokeField_accelerateResult n11534372;
  gaps_ride_cameraResult n11534382;
  matrix_ofResult n11534383;
  matrix_ofResult n11534384;
  state_ofResult n11534385;
  struct : kern::Node<12582923> { Int index = 0; } n12582923;
  struct { Int result = 0; } n13631498;
  struct { Bool result = false; } n13631499;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n13631500;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n13631501;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n13631502;
  struct { Bool result = false; } n13631503;
  struct { Bool result = false; } n13631504;
  struct : kern::Node<13631507> { Int index = 0; } n13631507;
  struct { Int result = 0; } n13631511;
  struct : kern::Node<13631512> { Int index = 0; } n13631512;
  struct : kern::Node<13631513> { Int index = 0; } n13631513;
  struct : kern::Node<13631514> { Int index = 0; } n13631514;
  struct { Int result = 0; } n13631515;
  struct : kern::Node<13631516> { Int index = 0; } n13631516;
  struct { Bool result = false; } n13631517;
  struct { Bool result = false; Bool changed = false; } n13631518;
  struct { Bool result = false; } n13631519;
  struct : kern::Node<14680074> { Int index = 0; } n14680074;
  life_cpp::Life_shotResult n14680075;
  life_cpp::Life_shotResult n14680077;
  struct { Bool result = false; } n14680078;
  player_positionResult n14680079;
  struct : kern::Node<14680080> { Int index = 0; } n14680080;
  struct : kern::Node<14680081> { Int index = 0; } n14680081;
  struct : kern::Node<14680082> { Int index = 0; } n14680082;
  struct : kern::Node<14680083> { Int index = 0; } n14680083;
  struct : kern::Node<14680084> { Int index = 0; } n14680084;
  struct : kern::Node<14680085> { Int index = 0; } n14680085;
  engine_math_math::Mat4_look_at_2Result n14680086;
  struct : kern::Node<14680087> { Int index = 0; } n14680087;
  struct { Int result = 0; } n15728653;
  struct { Bool result = false; } n15728654;
  matrix_ofResult n16777226;
  mp_art_cpp::MpArt_initResult n17825802;
  struct : kern::Node<17825803> { Int index = 0; } n17825803;
  struct : kern::Node<17825805> { Int index = 0; } n17825805;
  struct : kern::Node<17825806> { Int index = 0; } n17825806;
  struct : kern::Node<17825811> { Int index = 0; } n17825811;
  mp_rules_cpp::VoiceStream_renderResult n17825812;
  person_hit::hitResult n17825813;
  struct : kern::Node<17825814> { Int index = 0; } n17825814;
  remote_on_segment_zonesResult n17825815;
  remote_on_segment_zonesResult n17825816;
  remote_on_segment_zonesResult n17825817;
  focus_positionResult n17825820;
  road_graph_cpp::RoadGraph_pathResult n17825821;
  struct : kern::Node<18874378> { Int index = 0; } n18874378;
  struct { Bool removed = false; } n18874379;
  gaps_ride_poseResult n19922957;
  struct : kern::Node<20971530> { Int index = 0; } n20971530;
  struct : kern::Node<20971531> { Int index = 0; } n20971531;
  struct : kern::Node<20971532> { Int index = 0; } n20971532;
  Game_swarm_collect_next_idResult n20971533;
  struct : kern::Node<20971534> { Int index = 0; } n20971534;
  Game_swarm_collect_next_idResult n20971535;
  struct : kern::Node<20971536> { Int index = 0; } n20971536;
  Game_swarm_collect_next_idResult n20971537;
  struct : kern::Node<20971538> { Int index = 0; } n20971538;
  Game_swarm_collect_next_idResult n20971539;
  struct : kern::Node<20971540> { Int index = 0; } n20971540;
  struct : kern::Node<20971541> { Int index = 0; } n20971541;
  Game_swarm_collect_next_idResult n20971542;
  struct : kern::Node<20971543> { Int index = 0; } n20971543;
  state_ofResult n20971544;
  struct : kern::Node<20971545> { Int index = 0; } n20971545;
  struct : kern::Node<20971546> { Int index = 0; } n20971546;
  Game_swarm_collect_next_idResult n20971547;
  struct : kern::Node<20971548> { Int index = 0; } n20971548;
  struct { Game_SwarmFallen value = Game_SwarmFallen{net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, 0.0}; } n20971549;
  struct : kern::Node<20971550> { Int index = 0; } n20971550;
  struct : kern::Node<20971559> { Int index = 0; } n20971559;
  struct : kern::Node<20971560> { Int index = 0; } n20971560;
  struct : kern::Node<20971563> { Int index = 0; } n20971563;
  struct : kern::Node<20971564> { Int index = 0; } n20971564;
  struct : kern::Node<22020106> { Int index = 0; } n22020106;
  life::Life_animal_ofResult n22020107;
  struct : kern::Node<22020108> { Int index = 0; } n22020108;
  struct { Int result = 0; } n23068682;
  player_positionResult n23068690;
  city_stream_cpp::CityStream_street_pointResult n23068691;
  struct { Int result = 0; } n23068692;
  struct { Bool result = false; } n23068693;
  struct { Bool result = false; } n23068694;
  struct : kern::Node<23068695> { Int index = 0; } n23068695;
  common::Rng_nextResult n23068696;
  struct : kern::Node<23068701> { Int index = 0; } n23068701;
  line_of_sightResult n23068702;
  people_cpp::People_add_officerResult n23068703;
  common::Rng_unitResult n23068704;
  struct { Bool result = false; } n23068709;
  focus_positionResult n23068711;
  struct : kern::Node<23068712> { Int index = 0; } n23068712;
  struct : kern::Node<23068713> { Int index = 0; } n23068713;
  struct : kern::Node<23068714> { Int index = 0; } n23068714;
  matrix_ofResult n23068715;
  struct : kern::Node<23068716> { Int index = 0; } n23068716;
  struct : kern::Node<23068717> { Int index = 0; } n23068717;
  matrix_ofResult n23068718;
  struct { Bool added = false; Int index = 0; } n24117258;
  struct { Bool added = false; Int index = 0; } n24117259;
  struct { Bool added = false; Int index = 0; } n24117260;
  struct { Bool added = false; Int index = 0; } n24117261;
  struct : kern::Node<24117265> { Int index = 0; } n24117265;
  struct : kern::Node<24117266> { Int index = 0; } n24117266;
  matrix_ofResult n25165834;
  state_ofResult n25165841;
  struct { Int result = 0; } n25165842;
  common::Rng_nextResult n25165843;
  people_cpp::People_add_citizenResult n25165844;
  common::Rng_unitResult n25165845;
  common::Rng_unitResult n25165846;
  struct { Int result = 0; } n25165851;
  struct : kern::Node<25165852> { Int index = 0; } n25165852;
  struct : kern::Node<25165859> { Int index = 0; } n25165859;
  struct : kern::Node<26214410> { Int index = 0; } n26214410;
  struct { Int result = 0; } n26214412;
  struct { Int result = 0; } n26214413;
  struct { Int result = 0; } n26214414;
  struct { List<Int64> out = List<Int64>{}; } n26214415;
  struct { Int result = 0; } n27262988;
  struct { Bool result = false; } n27262989;
  player_positionResult n27262990;
  struct : kern::Node<27262991> { Int index = 0; } n27262991;
  struct : kern::Node<27262992> { Int index = 0; } n27262992;
  struct { Bool result = false; } n27262993;
  struct { List<Float> positions = List<Float>{}; List<Float> colors = List<Float>{}; } n27262994;
  struct { Bool result = false; } n27262995;
  struct : kern::Node<27262996> { Int index = 0; } n27262996;
  struct : kern::Node<27262997> { Int index = 0; } n27262997;
  struct { List<game_SoundCue> result = List<game_SoundCue>{}; } n27262999;
  struct : kern::Node<27263000> { Int index = 0; } n27263000;
  ui_equipment_jsonResult n27263002;
  vr_hand_matrixResult n29360141;
  vr_hand_matrixResult n29360143;
  struct { Bool result = false; } n29360144;
  struct { Int result = 0; Int framebuffer = 0; } n29360145;
  struct : kern::Node<29360146> { Int index = 0; } n29360146;
  struct { Int result = 0; } n29360147;
  struct { Bool result = false; } n29360148;
  struct { Bool result = false; Bool changed = false; } n29360149;
  struct { Bool result = false; } n29360150;
  vr_hand_matrixResult n29360152;
  struct : kern::Node<30408714> { Int index = 0; } n30408714;
  struct { Bool result = false; game_SoldierState out = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0}; } n30408715;
  struct { Bool result = false; } n30408716;
  struct { Bool result = false; } n30408717;
  struct { Bool result = false; } n30408718;
  struct { Bool result = false; } n30408719;
  struct { Bool result = false; } n30408720;
  struct { Bool result = false; } n30408721;
  struct : kern::Node<30408722> { Int index = 0; } n30408722;
  struct { Bool result = false; } n30408723;
  struct { Bool result = false; } n30408724;
  struct { Bool result = false; } n30408725;
  struct { Bool result = false; } n30408726;
  struct { Bool result = false; } n30408727;
  struct { Bool result = false; } n30408728;
  struct { Bool result = false; } n30408729;
  struct { Bool result = false; game_SoldierState out = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0}; } n30408730;
  struct : kern::Node<30408731> { Int index = 0; } n30408731;
  ctl_screen_rayResult n30408732;
  aim_rayResult n30408743;
  struct : kern::Node<30408749> { Int index = 0; } n30408749;
  world_marks_cpp::Marks_bulletResult n31457293;
  struct : kern::Node<32505866> { Int index = 0; } n32505866;
  struct { Int result = 0; } n32505867;
  car_cpp::Car_rigid_specResult n32505868;
  focus_positionResult n32505873;
  struct : kern::Node<32505874> { Int index = 0; } n32505874;
  matrix_ofResult n32505875;
  struct : kern::Node<32505876> { Int index = 0; } n32505876;
  struct : kern::Node<32505877> { Int index = 0; } n32505877;
  struct : kern::Node<32505878> { Int index = 0; } n32505878;
  struct : kern::Node<32505879> { Int index = 0; } n32505879;
  struct : kern::Node<32505880> { Int index = 0; } n32505880;
  struct : kern::Node<32505881> { Int index = 0; } n32505881;
  struct { Int result = 0; } n32505882;
};
