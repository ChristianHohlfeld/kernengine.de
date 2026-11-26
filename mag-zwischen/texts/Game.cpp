struct Game : virtual kern::Blueprint, virtual blueprint_call, virtual car, virtual car_cpp, virtual city_stream, virtual city_stream_cpp, virtual combat_art_cpp, virtual combat_audio, virtual combat_audio_cpp, virtual combat_rules_cpp, virtual common, virtual controls_cpp, virtual destruction, virtual drone_cpp, virtual engine_app_app, virtual engine_geo_mesh_data, virtual engine_input_actions, virtual engine_math_math, virtual engine_physics_physics, virtual equipment, virtual equipment_cpp, virtual game, virtual game_barricades_cpp, virtual game_combat, virtual game_controls, virtual game_cpp, virtual game_destruction_sync, virtual game_destruction_sync_cpp, virtual game_equipment_cpp, virtual game_gaps, virtual game_gaps_cpp, virtual game_guided_cpp, virtual game_lamps_cpp, virtual game_life, virtual game_life_cpp, virtual game_look, virtual game_look_cpp, virtual game_net, virtual game_projsync, virtual game_projsync_cpp, virtual game_swarm_cpp, virtual game_toss, virtual game_toss_cpp, virtual game_town_cpp, virtual game_townsync, virtual game_traffic_cpp, virtual game_ui, virtual game_vehicle_damage_cpp, virtual game_weapons_cpp, virtual game_world, virtual game_worldsync_cpp, virtual gaps_rules, virtual gaps_rules_cpp, virtual gps_live, virtual gps_live_cpp, virtual guided, virtual interiors, virtual interiors_cpp, virtual jet, virtual lamps, virtual lamps_cpp, virtual life, virtual life_art_cpp, virtual life_cpp, virtual look, virtual look_cpp, virtual mp_art, virtual mp_art_cpp, virtual mp_rules_cpp, virtual net_session, virtual net_session_cpp, virtual nostr_cpp, virtual people, virtual people_art_cpp, virtual people_cpp, virtual perf, virtual perf_cpp, virtual person_hit, virtual ragdoll, virtual ragdoll_cpp, virtual road_graph_cpp, virtual settings, virtual settings_cpp, virtual sound_fx_cpp, virtual terrain, virtual terrain_cpp, virtual tiles_cpp, virtual touch_play_cpp, virtual town_sync_cpp, virtual vehicle_art_cpp, virtual wanted_cpp, virtual weapons, virtual wire, virtual world_marks_cpp {
  // ---- variables ----
  PadState pad = PadState{false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, false};
  List<DestroySync> dsync_ = List<DestroySync>{};
  I32 toss_test_ = -1;
  TossState toss_ = TossState{List<TossState_Own>{}, List<TossState_Remote>{}, 0.0, 0, 0};
  List<ProjSync> projsync_ = List<ProjSync>{};
  U32 proj_serial_ = 0;
  List<TownSync> townsync_ = List<TownSync>{};
  Bool puppet_world_ = false;
  U32 world_sent_ = 0;
  U32 world_got_ = 0;
  /// client: the host's index each of our cars plays (-1 none)
  List<I32> car_net_ = List<I32>{};
  /// client: the host's poses of each of his cars
  List<mp_PoseTimeline> car_line_ = List<mp_PoseTimeline>{};
  /// client: the host's wall clock minus our real clock (kern::now_ms)
  Float host_epoch_off_ = 0.0;
  Bool host_epoch_ok_ = false;
  Map<Int, Game_RemoteSwarm> swarms_ = Map<Int, Game_RemoteSwarm>{};
  U16 swarm_serial_ = 0;
  List<Game_SwarmSent> swarm_sent_ = List<Game_SwarmSent>{};
  List<Game_SwarmFallen> swarm_fallen_ = List<Game_SwarmFallen>{};
  /// the mates' cruisers are drawn with it
  List<Car> swarm_cruiser_ = List<Car>{};
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
  List<Game_VisualBlast> visual_blasts_ = List<Game_VisualBlast>{};
  Fixed<Game_BounceFx, 8> bounce_fx_ = List<Game_BounceFx>{};
  U32 bounce_fx_next_ = 0;
  List<GapsState> gaps_ = List<GapsState>{};
  Object<kern::Engine> engine_ = Object<kern::Engine>{};
  /// the page runs a WebXR session ("vr 1")
  Bool vr_ = false;
  Fixed<Game_VrHand, 2> vr_hands_ = List<Game_VrHand>{};
  /// a rumble the page plays on the controller (vr json, host.mjs)
  Fixed<Float, 2> vr_pulse_ = List<Float>{0.0, 0.0};
  Bool vr_menu_open_ = false;
  Bool vr_menu_valid_ = false;
  I32 vr_menu_hover_ = -1;
  I32 vr_menu_page_ = 0;
  Bool vr_y_fired_ = false;
  Bool vr_near_vehicle_ = false;
  Float vr_y_held_ = 0.0;
  /// the wrist menu's KAMERA (game_controls.cpp)
  Bool vr_scoped_ = false;
  Vector vr_menu_c_ = Vector{0.0, 0.0, 0.0};
  Vector vr_menu_r_ = Vector{0.0, 0.0, 0.0};
  Vector vr_menu_u_ = Vector{0.0, 0.0, 0.0};
  Vector vr_menu_n_ = Vector{0.0, 0.0, 0.0};
  Float vr_menu_w_ = 1.0;
  Float vr_menu_h_ = 1.0;
  Text vr_action_ = "";
  U32 vr_scope_tex_ = 0;
  U32 vr_scope_mesh_ = 0;
  /// the headset HUD panel (the page's texture, "vr hud <id>")
  U32 vr_hud_tex_ = 0;
  /// the headset HUD panel (the page's texture, "vr hud <id>")
  U32 vr_hud_mesh_ = 0;
  Object<kern::game::GameWorld> world_ = Object<kern::game::GameWorld>{};
  CityStream city_ = CityStream{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 2600, 1, List<Vector>{}, 1, false, 0, 0, List<Vec4>{}, CityStream_NearCollision{physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, List<Int64_Int_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0}, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, 0.0, 0.0, 0.0, "", 0.0, 0, 0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, Map<Int, CityStream_Damage>{}, List<U32>{}, false, Object<kern::Renderer>{}, Object<kern::physics::World>{}, GeoProjector{0.0, 0.0, 1.0}, TileKey{14, 0, 0}, Map<Int64, List<CityStream_Tile>>{}, List<TileKey>{}, List<Lane>{}, false, 0, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<Int64, Terrain_Image>{}, List<TileKey>{}, Vector{1000000000.0, 1000000000.0, 0.0}, Object<kern::Renderer>{}, Object<kern::physics::World>{}, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0, false, Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<Int64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<Int64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<Int64, Terrain_RenderTile>{}, Map<Int64, Terrain_CollisionTile>{}, Map<Int64, Int>{}, Map<Int64, Bool>{}, Set<Int64>{}, Set<Int64>{}, Set<Int64>{}, List<Int64>{}, List<Int64>{}, List<Int64>{}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false}}, List<Vec4>{}, Int64{0x0}, -1};
  VehicleArt art_ = VehicleArt{Object<kern::Renderer>{}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}, VehicleArt_Part{0, List<Float>{}, List<Float>{}, false}};
  U32 soldier_ = 0;
  U32 drone_actor_ = 0;
  U32 jet_actor_ = 0;
  U32 chute_actor_ = 0;
  Drone drone_ = Drone{DroneParams{0.225, 0.127, 2300.0, 0.08, 1.1e-05, 0.105, 0.012, 45.0, 45.0, 4.0, 1.5, 0.055, 180.0, 1.225, 0.35, 0.18, Vector{0.0, 0.0, 0.0}, 0.18, 0.035, 0.005, 0.2, 0.03, 0.02, 0.03, 0.1, 0.015, -0.015, -0.07, 0.028}, fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0, false, false, false, false, false, false, false, 0.0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.39, fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}, 0.0, false}, fc_RC{List<U16>{}, false, false, false, Int64{0x0}}, fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false}, Int64{0x0}, 0, false}, List<Vector>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 1.0, 16.8, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, List<U16>{1000, 1000, 1000, 1000}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0.0, 1, 0.01, 0.01, Vector{0.0, 0.0, 0.0}, 1, true, Rng{1168318247, 0.0, false}, Rng{1905503009, 0.0, false}, Rng{1168232869, 0.0, false}, List<Float>{0.08, -0.05, 0.04}, List<Float>{0.001, -0.0015, 0.002}, 0, false, DroneTelemetry{false, true, false, false, false, 0, List<U16>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, List<Float>{}}};
  Jet jet_ = Jet{true, true, true, 1.0, 1.0, JetSpec{12000.0, Vector{100000.0, 18000.0, 115000.0}, 26.0, 9.2, 3.3, 105000.0, 185000.0, 0.34, 0.3, 0.1, 4.0, 0.42, 0.022, 0.14, -0.8, 0.02, 0.15, 0.0, -0.4, -12.0, -0.9, -0.06, -0.35, 0.08, 0.14, 0.12, -0.25, -0.03, 0.07, 0.48, 0.48, 0.5, 7.5, Vector{70000.0, 45000.0, 150000.0}, 1.0, List<JetSpec_Gear>{JetSpec_Gear{Vector{0.0, 5.2, -0.6}, 0.212, true}, JetSpec_Gear{Vector{1.9, -1.4, -0.6}, 0.394, false}, JetSpec_Gear{Vector{-1.9, -1.4, -0.6}, 0.394, false}}, 1.6, 2.05, 0.15, 0.4, 0.6, 0.8, 0.02, 0.5, 9.0, 3.0, 0.38, 3.4, 1.7, 22.0, 10.0, 6.0, 0.35, 85.0, 50.0, 1.05, 1.71}, Jet_Mode::hover, 0.0, 0.0, 0.0, 1.5707963267948966, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, true, false, true, 0, 0.0, JetTelemetry{0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0, 0.0, 0.0, 0.0, 0.0}};
  List<TrafficCar> cars_ = List<TrafficCar>{};
  Mode mode_ = Mode::foot;
  /// the car the player drives
  I32 car_index_ = -1;
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
  I32 drone_cam_ = 0;
  Bool car_cockpit_ = false;
  Bool jet_cockpit_ = false;
  Float fpv_tilt_deg_ = -15.0;
  Float fpv_fov_deg_ = 105.0;
  Camera cam_ = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Bool cam_valid_ = false;
  Vector cam_anchor_ = Vector{0.0, 0.0, 0.0};
  Vector cam_vel_ = Vector{0.0, 0.0, 0.0};
  Vector cam_heading_ = Vector{1.0, 0.0, 0.0};
  Bool rig_init_ = false;
  Float head_yaw_ = 0.0;
  Float head_pitch_ = 0.0;
  Float dpr_ = 1.0;
  /// input devices
  TouchPlay pad_touch_ = TouchPlay{960.0, 540.0, 0.0, 0.0, 0.0, 0.0, TouchLayout{TouchMode::off, true, "", false, false, false, 0, -1.0, false, false, false, true, false, false, false, false, false, false, false, false}, List<TouchBtn>{}, List<TouchPlay_Finger>{}, List<Text>{}, List<Text>{}, List<TouchFire>{}, -1, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, 0.0, false, List<TouchPlay_DomRect>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  Bool touch_mode_ = false;
  Float frame_w_ = 960.0;
  Float frame_h_ = 540.0;
  game_PlayInput play_in_ = game_PlayInput{0.0, 0.0, 0.0, false, false, false, false, 0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, false, 0.0, false, false, false, false, false, 0.0, 0.0, false, 0.0};
  /// the town
  People people_ = People{List<People_Person>{}, List<CrimeReport>{}, List<People_Death>{}, List<People_Fall>{}, List<People_RemoteHit>{}, List<People_Hurt>{}, false, false, 60, 1, 0, 0, 170.0, 190.0};
  PeopleArt people_art_ = PeopleArt{Object<kern::Renderer>{}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}};
  CombatArt combat_art_ = CombatArt{Object<kern::Renderer>{}, List<CombatArt_Part>{}};
  /// buildings broken by weapons (destruction.h)
  Destruction destruction_ = Destruction{Object<kern::Renderer>{}, Map<Int, Float>{}, List<Int_Float_pair>{}, List<Destruction_Piece>{}, List<Destruction_Torn>{}, Destruction_Stats{0, 0, 0, 0}};
  CombatState combat_ = CombatState{combat_MilitaryClock{false, -1.0, -1.0, 0.0}, 0, 0, 0, 0.0, List<Heli>{}, List<Tank>{}, List<Shell>{}, List<SoldierAi>{}, List<Float>{}, true, List<FlakGun>{}, List<FlakRound>{}, List<Puff>{}, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0, 0, List<FighterJet>{}, List<Bomb>{}, List<Blast>{}, List<Flare>{}, 50.0, -1000000000.0, List<Tracer>{}, List<Debris>{}, List<U8>{}, List<Bool>{}, List<Bool>{}, false, 0, 0, 0, combat_ZombieRounds{false, 0, 0, 0, 0.0, -1.0}, List<ZombieAi>{}, true, false, -99.0, List<EnemyAi>{}, List<EnemyRocket>{}, combat_Chaos{0, 1, -1.0, 0.0}, -99.0, -1.0, 0, combat_Fun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, -1.0, combat_Race{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}, false, Vector{0.0, 0.0, 0.0}, "", "", -99.0, -99.0, false, 0.0};
  /// GUIDED missiles in flight (game_guided.cpp)
  List<guided_Flight> guided_ = List<guided_Flight>{};
  /// the first press marks, the second launches (game_guided.cpp)
  guided_Target guided_mark_ = guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"};
  Vector guided_mark_at_ = Vector{0.0, 0.0, 0.0};
  Float guided_mark_time_ = -99.0;
  Bool guided_marked_ = false;
  Fixed<U32, 16> sfx_cursor_ = List<U32>{};
  Bool combat_audio_ready_ = false;
  RoadGraph roads_ = RoadGraph{Int64{0xffffffffffffffff}, List<RoadGraph_Node>{}, Map<Int64, Int>{}};
  List<PoliceUnit> units_ = List<PoliceUnit>{};
  Fixed<PoliceDrone, 5> pdrones_ = List<PoliceDrone>{};
  Float next_drone_wave_ = -1.0;
  Bool drone_wave_live_ = false;
  /// weapons
  List<Projectile> projectiles_ = List<Projectile>{};
  weapons_Id foot_weapon_ = weapons_Id::glock;
  Fixed<I32, 16> ammo_mag_ = List<I32>{};
  Fixed<I32, 16> ammo_reserve_ = List<I32>{};
  I32 grenades_ = 4;
  /// jet_mode.mjs camOff (the chase camera's eased offset) // tests: distance of the car "mode car" took // jet_mode.mjs rocketSide: the pods fire in turn
  Vector jet_cam_off_ = Vector{0.0, 0.0, 0.0};
  Float next_attack_ = 0.0;
  Float next_grenade_ = 0.0;
  /// multiplayer
  net_Session net_ = net_Session{net_Session_Role::none, Object<kern::net::HostTransport>{}, Object<kern::net::Server>{}, Object<kern::net::Client>{}, net_Address{Int64{0x0}}, List<net_Remote>{}, List<net_Hit>{}, List<net_Hit>{}, List<net_Kill>{}, List<net_Fx>{}, List<net_Swarm>{}, List<net_SwarmHit>{}, List<U8_row>{}, List<net_PingEvent>{}, List<net_VoiceFrame>{}, List<U8>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, false, false, 0.0, 0, 0.0, List<U8>{}, Vector{0.0, 0.0, 0.0}, 255, net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_Scoreboard{List<mp_Score>{}}, false, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_SwarmParts>{}, 0, 0, false, List<Int_Float_pair>{}, mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}, mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}, 0, "Aenderungen nur einstimmig", 0, 0, 0, seats_Table{List<seats_Row>{}, 0}, 0.0};
  List<Car> remote_car_ = List<Car>{};
  List<Car> remote_bus_ = List<Car>{};
  /// pings, feed, voice, big map (game_net.h)
  List<MpLocal> mp_ = List<MpLocal>{};
  List<Game_Lamp> lamps_ = List<Game_Lamp>{};
  /// where the ground changed this frame (city_stream.h take_ground_changes)
  List<Vec4> ground_changes_ = List<Vec4>{};
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
  /// world_spawn_guard (Game::spawn_guard)
  Bool spawn_checked_ = false;
  /// audio
  Object<kern::audio::Mixer> mixer_ = Object<kern::audio::Mixer>{};
  Object<kern::game::SoundBank> bank_ = Object<kern::game::SoundBank>{};
  Object<kern::game::GameAudio> game_audio_ = Object<kern::game::GameAudio>{};
  SoundFx fx_ = SoundFx{Object<kern::audio::Mixer>{}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}};
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
  UiState ui_ = UiState{Settings{List<Float>{}, 1}, SettingsTable{List<Option>{}, List<OptSpec>{}}, 0, false, false, 0, false, 0.0, 0.0, 0, 0, 0, Vector{1000000000.0, 1000000000.0, 0.0}, 0.0, -1, -1, GpsLive{GpsLive_View::eye, 0, 0, false, false, false, false, 0.0, -1000000000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}}, 0, "", Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, 0, 0, 0, List<Vector>{}, false, 0.0, false, Jetpack{1.0, false, false, 0, 0.0, -1.0, -99.0}, false, false, 0.0, List<Int64>{}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, 0, 0, 0, 0, 0, 0, FlightLog{List<Flight>{}, List<FlightLog_Event>{}, Flight{0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", List<Vector>{}}, false, false, Vector{0.0, 0.0, 0.0}, 0.0, -1000000000.0, 1, 0}, "", MusicSynth{0.0, 0, -2, -2, 0.0, 0.0, 1.0, Int64{0x0}, 0, 1, 0, 1, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, List<MusicSynth_Voice>{}, 0.0, 0.0, List<Float>{}, 0}, 0, true, "", "", 99.0, Vector{0.0, 0.0, 0.0}, false, 0};
  /// performance and the look (game_look.h)
  LookState look_ = LookState{perf_Meter{0.0, 0.0, List<Float>{}, List<Float>{}, 0.0, 0.0, -1.0}, perf_Governor{4, 4, true, 0.016666666666666666, 0.0, 0.0, 0.0, 0.0, List<Float>{}, List<Float>{}, 0}, false, false, false, -1, 0, 170.0, 220.0, look_Shake{0.0, 17.3}, look_Stabilizer{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, look_SpeedLines{List<look_SpeedLines_Streak>{}, false, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, Rng{24301, 0.0, false}}, Vector{0.0, 0.0, 0.0}, false, -1, 0.4, 0.0, Object<kern::LightPool>{}, 16, List<LookState_Blast>{}, List<LookState_Glint>{}, -1.0, -99.0, List<Vector>{}, 0, 0, List<Float>{}, List<Float>{}, List<U32>{}, List<U32>{}, List<U32>{}, List<U32>{}, false, -1, -99.0, 0.0, 0.0, 0.0, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, -30.0}, -99.0, 30.0, -99.0, false, -1, 0, List<U32>{}};
  /// M3 life (last: destroyed before world_)
  List<GameLife> glife_ = List<GameLife>{};
  /// (of the program: combat::kAlertS)
  Float combat_kAlertS = 0.0;
  /// (of the program: combat::kAmbientMaxS)
  Float combat_kAmbientMaxS = 0.0;
  /// (of the program: combat::kAmbientMinS)
  Float combat_kAmbientMinS = 0.0;
  /// (of the program: combat::kBombDamage)
  Float combat_kBombDamage = 0.0;
  /// (of the program: combat::kBombRadius)
  Float combat_kBombRadius = 0.0;
  /// (of the program: combat::kBurstWindowS)
  Float combat_kBurstWindowS = 0.0;
  /// (of the program: combat::kComboS)
  Float combat_kComboS = 0.0;
  /// (of the program: combat::kContracts)
  I32 combat_kContracts = 0;
  /// (of the program: combat::kCourseScale)
  Float combat_kCourseScale = 0.0;
  /// (of the program: combat::kEndStars)
  I32 combat_kEndStars = 0;
  /// (of the program: combat::kEnemySpawnS)
  Float combat_kEnemySpawnS = 0.0;
  /// (of the program: combat::kFirstReinforceS)
  Float combat_kFirstReinforceS = 0.0;
  /// (of the program: combat::kFlakBurstR)
  Float combat_kFlakBurstR = 0.0;
  /// (of the program: combat::kFlakCell)
  Float combat_kFlakCell = 0.0;
  /// (of the program: combat::kFlakChance)
  Float combat_kFlakChance = 0.0;
  /// (of the program: combat::kFlakHp)
  Float combat_kFlakHp = 0.0;
  /// (of the program: combat::kFlakMinAgl)
  Float combat_kFlakMinAgl = 0.0;
  /// (of the program: combat::kFlakMuzzle)
  Float combat_kFlakMuzzle = 0.0;
  /// (of the program: combat::kFlakOff)
  Float combat_kFlakOff = 0.0;
  /// (of the program: combat::kFlakOn)
  Float combat_kFlakOn = 0.0;
  /// (of the program: combat::kFlakRange)
  Float combat_kFlakRange = 0.0;
  /// (of the program: combat::kFlakRps)
  Float combat_kFlakRps = 0.0;
  /// (of the program: combat::kFlakSpread)
  Float combat_kFlakSpread = 0.0;
  /// (of the program: combat::kFunComboS)
  Float combat_kFunComboS = 0.0;
  /// (of the program: combat::kGangGunDmg)
  Float combat_kGangGunDmg = 0.0;
  /// (of the program: combat::kGates)
  I32 combat_kGates = 0;
  /// (of the program: combat::kHeliDmg)
  Float combat_kHeliDmg = 0.0;
  /// (of the program: combat::kHeliDmgDrone)
  Float combat_kHeliDmgDrone = 0.0;
  /// (of the program: combat::kHeliHp)
  Float combat_kHeliHp = 0.0;
  /// (of the program: combat::kJetAltMax)
  Float combat_kJetAltMax = 0.0;
  /// (of the program: combat::kJetAltMin)
  Float combat_kJetAltMin = 0.0;
  /// (of the program: combat::kJetSpan)
  Float combat_kJetSpan = 0.0;
  /// (of the program: combat::kJetSpeed)
  Float combat_kJetSpeed = 0.0;
  /// (of the program: combat::kMaxSoldiers)
  I32 combat_kMaxSoldiers = 0;
  /// (of the program: combat::kMilHit)
  Float combat_kMilHit = 0.0;
  /// (of the program: combat::kMomentumTarget)
  I32 combat_kMomentumTarget = 0;
  /// (of the program: combat::kOverdriveS)
  Float combat_kOverdriveS = 0.0;
  /// (of the program: combat::kPunchDmg)
  Float combat_kPunchDmg = 0.0;
  /// (of the program: combat::kRaceLaps)
  I32 combat_kRaceLaps = 0;
  /// (of the program: combat::kReinforceS)
  Float combat_kReinforceS = 0.0;
  /// (of the program: combat::kShellDmg)
  Float combat_kShellDmg = 0.0;
  /// (of the program: combat::kSoldierBursts)
  I32 combat_kSoldierBursts = 0;
  /// (of the program: combat::kSoldierDmg)
  Float combat_kSoldierDmg = 0.0;
  /// (of the program: combat::kSoldierDmgDrone)
  Float combat_kSoldierDmgDrone = 0.0;
  /// (of the program: combat::kSoldierHp)
  Float combat_kSoldierHp = 0.0;
  /// (of the program: combat::kSoldierReactS)
  Float combat_kSoldierReactS = 0.0;
  /// (of the program: combat::kSoundMps)
  Float combat_kSoundMps = 0.0;
  /// (of the program: combat::kSquad)
  I32 combat_kSquad = 0;
  /// (of the program: combat::kStabDmg)
  Float combat_kStabDmg = 0.0;
  /// (of the program: combat::kStick)
  I32 combat_kStick = 0;
  /// (of the program: combat::kStickLen)
  Float combat_kStickLen = 0.0;
  /// (of the program: combat::kStrikeCooldownS)
  Float combat_kStrikeCooldownS = 0.0;
  /// (of the program: combat::kTankHp)
  Float combat_kTankHp = 0.0;
  /// (of the program: combat::kTriggerExtremeHeat)
  Float combat_kTriggerExtremeHeat = 0.0;
  /// (of the program: combat::kTriggerHeat)
  Float combat_kTriggerHeat = 0.0;
  /// (of the program: combat::kTriggerHoldS)
  Float combat_kTriggerHoldS = 0.0;
  /// (of the program: combat::kZombieCap)
  I32 combat_kZombieCap = 0;
  /// (of the program: combat::kZombieHit)
  Float combat_kZombieHit = 0.0;
  /// (of the program: combat::kZombieHitPts)
  I32 combat_kZombieHitPts = 0;
  /// (of the program: combat::kZombieKillPts)
  I32 combat_kZombieKillPts = 0;
  /// (of the program: combat::kZombieMelee)
  Float combat_kZombieMelee = 0.0;
  /// (of the program: combat::kZombieMeleeS)
  Float combat_kZombieMeleeS = 0.0;
  /// (of the program: combat::kZombieSpawnMax)
  Float combat_kZombieSpawnMax = 0.0;
  /// (of the program: combat::kZombieSpawnMin)
  Float combat_kZombieSpawnMin = 0.0;
  /// (kept by combat_rules)
  List<combat_ContractRow> combat_rules_g_contract_rows = List<combat_ContractRow>{};
  /// (kept by combat_rules)
  List<combat_Contract> combat_rules_g_contracts = List<combat_Contract>{};
  /// (kept by combat_rules)
  List<combat_EnemySpec> combat_rules_g_enemies = List<combat_EnemySpec>{};
  /// (kept by combat_rules)
  List<combat_EnemyRow> combat_rules_g_enemy_rows = List<combat_EnemyRow>{};
  /// (kept by combat_rules)
  List<combat_GateRow> combat_rules_g_gate_rows = List<combat_GateRow>{};
  /// (kept by combat_rules)
  List<combat_Gate> combat_rules_g_gates = List<combat_Gate>{};
  /// (kept by life_draw_ragdolls)
  List<Ragdolls_Draw> life_draw_ragdolls_bones = List<Ragdolls_Draw>{};
  /// (of the program: mp::kBallotMs)
  Float mp_kBallotMs = 0.0;
  /// (of the program: mp::kPingHoldMs)
  Float mp_kPingHoldMs = 0.0;
  /// (of the program: mp::kPingLifeMs)
  Float mp_kPingLifeMs = 0.0;
  /// (of the program: mp::kPingRemoveM)
  Float mp_kPingRemoveM = 0.0;
  /// (of the program: mp::kRegenDelayMs)
  Float mp_kRegenDelayMs = 0.0;
  /// (of the program: mp::kRegenHpPerS)
  Float mp_kRegenHpPerS = 0.0;
  /// (of the program: mp::kRegenTickMs)
  Float mp_kRegenTickMs = 0.0;
  /// (of the program: mp::kRespawnCooldownMs)
  Float mp_kRespawnCooldownMs = 0.0;
  /// (of the program: mp::kRespawnRadiusMax)
  Float mp_kRespawnRadiusMax = 0.0;
  /// (of the program: mp::kRespawnRadiusMin)
  Float mp_kRespawnRadiusMin = 0.0;
  /// (of the program: mp::kSpawnProtectMs)
  Float mp_kSpawnProtectMs = 0.0;
  /// (of the program: mp::kVoiceRadius)
  Float mp_kVoiceRadius = 0.0;
  /// (kept by mp_rules)
  List<U32> mp_rules_g_palette = List<U32>{};
  /// (kept by sfx_id)
  List<Text> sfx_id_ids = List<Text>{};
  KERN_CONST combat_EnemySpec kNoEnemy = combat_EnemySpec{"", 1.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0, 1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}};
  KERN_CONST combat_Contract kNoContract = combat_Contract{"", "", "", 1, 0};
  KERN_CONST combat_Gate kNoGate = combat_Gate{"", Vector{0.0, 0.0, 0.0}, 1.0, 1.0, false};
  KERN_CONST Float kRespawnCooldown = 3.0;
  /// m: the safe margin a player is put down over the ground
  KERN_CONST Float kSpawnLift = 0.35;
  /// The number of each value of net_PlayerFlag, in their order
  KERN_CONST List<Int> net_PlayerFlag_numbers = List<Int>{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
  /// building_interiors.mjs PLANKS
  KERN_CONST I32 kPlanks = 6;
  /// zombie_nights.mjs PLANK_MS
  KERN_CONST Float kPlankS = 3.4;
  /// nearDoor
  KERN_CONST Float kRepairM = 3.2;
  /// REPAIR_COST
  KERN_CONST I32 kRepairCost = 10;
  /// DOOR_W
  KERN_CONST Float kDoorW = 1.8;
  /// vertex locations 14 and 15
  KERN_CONST U32 kMaxExtensionAttributes = 2;
  KERN_CONST Float kRad = 57.29577951308238;
  KERN_CONST Float kPi = 3.141592653589793;
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
  /// building records per message (a full state is split)
  KERN_CONST Int kPerMessage = 400;
  KERN_CONST U8 kState = 2;
  /// The constant kVariants of the Blueprint combat_audio_cpp
  KERN_CONST Fixed<U32, 11> kVariants = List<U32>{3, 4, 4, 4, 2, 4, 2, 3, 4, 3, 2};
  /// The constant kSampleRate of the Blueprint combat_audio_cpp
  KERN_CONST U32 kSampleRate = 44100;
  KERN_CONST Fixed<MusicSynth_Station, 5> kStations = List<MusicSynth_Station>{MusicSynth_Station{"KERN FM", "Synthwave", 100.0, 11}, MusicSynth_Station{"BODENSEE BEATS", "House", 124.0, 23}, MusicSynth_Station{"LOFI LAGUNE", "Lo-Fi", 78.0, 37}, MusicSynth_Station{"8-BIT RADIO", "Chiptune", 144.0, 41}, MusicSynth_Station{"NACHTSCHICHT", "Techno", 132.0, 53}};
  KERN_CONST I32 kMenu = -1;
  KERN_CONST Float kG = 9.80665;
  /// GRENADE_RESTITUTION
  KERN_CONST Float kRestitution = 0.58;
  /// GRENADE_SURFACE_DAMPING
  KERN_CONST Float kSurfaceDamping = 0.88;
  /// GRENADE_MAX_BOUNCES
  KERN_CONST I32 kMaxBounces = 7;
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
  /// jet_mode.mjs HP_MAX
  KERN_CONST Float kJetHpMax = 400.0;
  /// startWorldCriticalDamage delayMs 12000 (jet)
  KERN_CONST Float kJetBurnS = 12.0;
  KERN_CONST Float kGuidedMarkSeconds = 8.0;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kPoleH = 6.2;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kArm = 1.42;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kHeadZ = 6.0;
  KERN_CONST I32 kMobileStart = 2;
  KERN_CONST I32 kDesktopStart = 4;
  KERN_CONST I32 kMobileMax = 3;
  KERN_CONST I32 kTiers = 5;
  KERN_CONST Fixed<perf_Tier, 5> kTier = List<perf_Tier>{perf_Tier{"minimal", 16, 35.0, 1, 0, 70.0, 90.0, 2, 0.6}, perf_Tier{"niedrig", 30, 50.0, 4, 0, 110.0, 140.0, 3, 0.75}, perf_Tier{"mittel", 44, 70.0, 4, 1, 140.0, 180.0, 4, 1.0}, perf_Tier{"hoch", 64, 90.0, 9, 2, 170.0, 220.0, 6, 1.0}, perf_Tier{"maximal", 80, 110.0, 9, 3, 170.0, 220.0, 10, 1.0}};
  /// ---- day and night: one clock (480 s per day, SpawnCity) the game owns ----
  KERN_CONST Float kDayLength = 480.0;
  KERN_CONST Fixed<Text, 4> kVmNames = List<Text>{"BOOMSTICK 40 mm", "ROCKET LAUNCHER", "FISTS", "KNIFE"};
  /// world.* switches that make the world (friendly fire is the rule voted on: game_rules_sync)
  KERN_CONST Fixed<Opt, 13> kWorldRules = List<Opt>{Opt::world_people, Opt::world_traffic, Opt::world_dogs, Opt::world_cats, Opt::world_birds, Opt::world_zombies, Opt::world_police, Opt::world_military, Opt::world_flak, Opt::world_jetpack, Opt::world_terrain, Opt::world_guided, Opt::world_always_day};
  /// mp_fun_hud.mjs: a kill line stays 5.2 s
  KERN_CONST Float kFeedMs = 5200.0;
  KERN_CONST Float kArriveM = 25.0;
  /// world_map_full.mjs: a new route at most every 10 s
  KERN_CONST Float kNavRerouteMs = 10000.0;
  /// a seated passenger: hips 0.5 m over the road instead of 0.92 (game_seats.cpp)
  KERN_CONST Float kSeatDrop = 0.42;
  KERN_CONST Float game_projsync_kSendEvery = 0.05;
  KERN_CONST U8 kGone = 1;
  KERN_CONST Float game_projsync_kQuiet = 0.6;
  /// The number of each value of net_SwarmFlag, in their order
  KERN_CONST List<Int> net_SwarmFlag_numbers = List<Int>{128, 1, 2, 4, 3};
  KERN_CONST Float kFallenMs = 1500.0;
  /// game_net.cpp / game.cpp draw_vehicles
  KERN_CONST Vector kWreckBlack = Vector{0.055, 0.05, 0.045};
  /// game_net.cpp / game.cpp draw_vehicles
  KERN_CONST Vector kCruiserWhite = Vector{0.93, 0.94, 0.96};
  KERN_CONST U8 kPerson = 0;
  KERN_CONST U8 kBird = 3;
  KERN_CONST U8 kCow = 1;
  KERN_CONST U8 kPet = 2;
  KERN_CONST U8 kHeld = 1;
  KERN_CONST U8 kDone = 2;
  KERN_CONST Float game_toss_kSendEvery = 0.05;
  KERN_CONST Float game_toss_kQuiet = 0.5;
  KERN_CONST Float kCarCritical = 0.2;
  KERN_CONST Float kCarBurn = 2.45;
  KERN_CONST Float game_town_kSpawnMax = 125.0;
  KERN_CONST Float game_town_kSpawnMin = 70.0;
  KERN_CONST U8 kDoorsMax = 64;
  /// the blasts kept (scorches 40, soot 48, breaches 64 - world_marks.h)
  KERN_CONST Int kMarks = 64;
  KERN_CONST U8 kPlanksMax = 6;
  KERN_CONST Float kGiveUpS = 70.0;
  KERN_CONST Float kDriveSpeed = 14.0;
  KERN_CONST Float kHookM = 7.5;
  KERN_CONST Float kHookS = 1.8;
  KERN_CONST Float kTowSpeed = 9.0;
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
  struct Race_GateAtResult { combat_GateRow gate = combat_GateRow{"", Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false}; };
  /// (of the Blueprint of the actor Race, called on it)
  KERN_EXTERN_AS("GateAt") Race_GateAtResult Race_GateAt(Int index = 0);

  struct bind_militaryResult { Bool Result = false; };
  /// ---------------------------------------------------------------- "Military"
  bind_militaryResult bind_military() {
    Bool runs = false;
    runs = func_has_blueprint(actor_find("Military"));
    combat_kGangGunDmg = 0.0;
    combat_kStabDmg = combat_kGangGunDmg;
    combat_kPunchDmg = combat_kStabDmg;
    combat_kStick = kern::to<Int>(combat_kPunchDmg);
    combat_kStickLen = kern::to<Float>(combat_kStick);
    combat_kBombDamage = combat_kStickLen;
    combat_kBombRadius = combat_kBombDamage;
    combat_kStrikeCooldownS = combat_kBombRadius;
    combat_kAmbientMaxS = combat_kStrikeCooldownS;
    combat_kAmbientMinS = combat_kAmbientMaxS;
    combat_kJetSpan = combat_kAmbientMinS;
    combat_kJetAltMax = combat_kJetSpan;
    combat_kJetAltMin = combat_kJetAltMax;
    combat_kSoundMps = combat_kJetAltMin;
    combat_kJetSpeed = combat_kSoundMps;
    combat_kFlakHp = combat_kJetSpeed;
    combat_kFlakBurstR = combat_kFlakHp;
    combat_kFlakSpread = combat_kFlakBurstR;
    combat_kFlakOff = combat_kFlakSpread;
    combat_kFlakOn = combat_kFlakOff;
    combat_kFlakRps = combat_kFlakOn;
    combat_kFlakMuzzle = combat_kFlakRps;
    combat_kFlakMinAgl = combat_kFlakMuzzle;
    combat_kFlakRange = combat_kFlakMinAgl;
    combat_kFlakChance = combat_kFlakRange;
    combat_kFlakCell = combat_kFlakChance;
    combat_kSoldierBursts = kern::to<Int>(combat_kFlakCell);
    combat_kBurstWindowS = kern::to<Float>(combat_kSoldierBursts);
    combat_kSoldierReactS = combat_kBurstWindowS;
    combat_kShellDmg = combat_kSoldierReactS;
    combat_kSoldierDmgDrone = combat_kShellDmg;
    combat_kSoldierDmg = combat_kSoldierDmgDrone;
    combat_kHeliDmgDrone = combat_kSoldierDmg;
    combat_kHeliDmg = combat_kHeliDmgDrone;
    combat_kTankHp = combat_kHeliDmg;
    combat_kHeliHp = combat_kTankHp;
    combat_kMilHit = combat_kHeliHp;
    combat_kSoldierHp = combat_kMilHit;
    combat_kMaxSoldiers = kern::to<Int>(combat_kSoldierHp);
    combat_kSquad = combat_kMaxSoldiers;
    combat_kEndStars = combat_kSquad;
    combat_kFirstReinforceS = kern::to<Float>(combat_kEndStars);
    combat_kReinforceS = combat_kFirstReinforceS;
    combat_kAlertS = combat_kReinforceS;
    combat_kTriggerExtremeHeat = combat_kAlertS;
    combat_kTriggerHoldS = combat_kTriggerExtremeHeat;
    combat_kTriggerHeat = combat_kTriggerHoldS;
    if (logic_not(runs)) {
      return {false};
    }
    combat_kTriggerHeat = func_get_on<"TriggerHeat", Float>(actor_find("Military"));
    combat_kTriggerHoldS = func_get_on<"TriggerHoldS", Float>(actor_find("Military"));
    combat_kTriggerExtremeHeat = func_get_on<"TriggerExtremeHeat", Float>(actor_find("Military"));
    combat_kAlertS = func_get_on<"AlertS", Float>(actor_find("Military"));
    combat_kReinforceS = func_get_on<"ReinforceS", Float>(actor_find("Military"));
    combat_kFirstReinforceS = func_get_on<"FirstReinforceS", Float>(actor_find("Military"));
    combat_kEndStars = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(a); }, func_get_on<"EndStars", Float>(actor_find("Military"))));
    combat_kSquad = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(a); }, func_get_on<"Squad", Float>(actor_find("Military"))));
    combat_kMaxSoldiers = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(a); }, func_get_on<"MaxSoldiers", Float>(actor_find("Military"))));
    combat_kSoldierHp = func_get_on<"SoldierHp", Float>(actor_find("Military"));
    combat_kMilHit = func_get_on<"MilHit", Float>(actor_find("Military"));
    combat_kHeliHp = func_get_on<"HeliHp", Float>(actor_find("Military"));
    combat_kTankHp = func_get_on<"TankHp", Float>(actor_find("Military"));
    combat_kHeliDmg = func_get_on<"HeliDmg", Float>(actor_find("Military"));
    combat_kHeliDmgDrone = func_get_on<"HeliDmgDrone", Float>(actor_find("Military"));
    combat_kSoldierDmg = func_get_on<"SoldierDmg", Float>(actor_find("Military"));
    combat_kSoldierDmgDrone = func_get_on<"SoldierDmgDrone", Float>(actor_find("Military"));
    combat_kShellDmg = func_get_on<"ShellDmg", Float>(actor_find("Military"));
    combat_kSoldierReactS = func_get_on<"SoldierReactS", Float>(actor_find("Military"));
    combat_kBurstWindowS = func_get_on<"BurstWindowS", Float>(actor_find("Military"));
    combat_kSoldierBursts = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(a); }, func_get_on<"SoldierBursts", Float>(actor_find("Military"))));
    combat_kFlakCell = func_get_on<"FlakCell", Float>(actor_find("Military"));
    combat_kFlakChance = func_get_on<"FlakChance", Float>(actor_find("Military"));
    combat_kFlakRange = func_get_on<"FlakRange", Float>(actor_find("Military"));
    combat_kFlakMinAgl = func_get_on<"FlakMinAgl", Float>(actor_find("Military"));
    combat_kFlakMuzzle = func_get_on<"FlakMuzzle", Float>(actor_find("Military"));
    combat_kFlakRps = func_get_on<"FlakRps", Float>(actor_find("Military"));
    combat_kFlakOn = func_get_on<"FlakOn", Float>(actor_find("Military"));
    combat_kFlakOff = func_get_on<"FlakOff", Float>(actor_find("Military"));
    combat_kFlakSpread = func_get_on<"FlakSpread", Float>(actor_find("Military"));
    combat_kFlakBurstR = func_get_on<"FlakBurstR", Float>(actor_find("Military"));
    combat_kFlakHp = func_get_on<"FlakHp", Float>(actor_find("Military"));
    combat_kJetSpeed = func_get_on<"JetSpeed", Float>(actor_find("Military"));
    combat_kSoundMps = func_get_on<"SoundMps", Float>(actor_find("Military"));
    combat_kJetAltMin = func_get_on<"JetAltMin", Float>(actor_find("Military"));
    combat_kJetAltMax = func_get_on<"JetAltMax", Float>(actor_find("Military"));
    combat_kJetSpan = func_get_on<"JetSpan", Float>(actor_find("Military"));
    combat_kAmbientMinS = func_get_on<"AmbientMinS", Float>(actor_find("Military"));
    combat_kAmbientMaxS = func_get_on<"AmbientMaxS", Float>(actor_find("Military"));
    combat_kStrikeCooldownS = func_get_on<"StrikeCooldownS", Float>(actor_find("Military"));
    combat_kBombRadius = func_get_on<"BombRadius", Float>(actor_find("Military"));
    combat_kBombDamage = func_get_on<"BombDamage", Float>(actor_find("Military"));
    combat_kStickLen = func_get_on<"StickLen", Float>(actor_find("Military"));
    combat_kStick = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(a); }, func_get_on<"Stick", Float>(actor_find("Military"))));
    combat_kPunchDmg = func_get_on<"PunchDmg", Float>(actor_find("Military"));
    combat_kStabDmg = func_get_on<"StabDmg", Float>(actor_find("Military"));
    combat_kGangGunDmg = func_get_on<"GangGunDmg", Float>(actor_find("Military"));
    return {true};
  }

  struct bind_zombiesResult { Bool Result = false; };
  /// ---------------------------------------------------------------- "Zombies"
  bind_zombiesResult bind_zombies() {
    Bool runs = false;
    runs = func_has_blueprint(actor_find("Zombies"));
    combat_kZombieKillPts = 0;
    combat_kZombieHitPts = combat_kZombieKillPts;
    combat_kZombieSpawnMax = kern::to<Float>(combat_kZombieHitPts);
    combat_kZombieSpawnMin = combat_kZombieSpawnMax;
    combat_kZombieMeleeS = combat_kZombieSpawnMin;
    combat_kZombieMelee = combat_kZombieMeleeS;
    combat_kZombieHit = combat_kZombieMelee;
    combat_kZombieCap = kern::to<Int>(combat_kZombieHit);
    if (logic_not(runs)) {
      return {false};
    }
    combat_kZombieCap = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(a); }, func_get_on<"ZombieCap", Float>(actor_find("Zombies"))));
    combat_kZombieHit = func_get_on<"ZombieHit", Float>(actor_find("Zombies"));
    combat_kZombieMelee = func_get_on<"ZombieMelee", Float>(actor_find("Zombies"));
    combat_kZombieMeleeS = func_get_on<"ZombieMeleeS", Float>(actor_find("Zombies"));
    combat_kZombieSpawnMin = func_get_on<"ZombieSpawnMin", Float>(actor_find("Zombies"));
    combat_kZombieSpawnMax = func_get_on<"ZombieSpawnMax", Float>(actor_find("Zombies"));
    combat_kZombieHitPts = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(a); }, func_get_on<"ZombieHitPts", Float>(actor_find("Zombies"))));
    combat_kZombieKillPts = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(a); }, func_get_on<"ZombieKillPts", Float>(actor_find("Zombies"))));
    return {true};
  }

  struct bind_sandboxResult { Bool Result = false; };
  /// ---------------------------------------------------------------- "Sandbox"
  bind_sandboxResult bind_sandbox() {
    Bool runs = false;
    runs = func_has_blueprint(actor_find("Sandbox"));
    list_clear(combat_rules_g_enemies);
    combat_kEnemySpawnS = func_get_on<"EnemySpawnS", Float>(actor_find("Sandbox"));
    combat_kComboS = func_get_on<"ComboS", Float>(actor_find("Sandbox"));
    combat_rules_g_enemy_rows = func_get_on<"Enemies", List<combat_EnemyRow>>(actor_find("Sandbox"));
    if (logic_or(logic_not(runs), math_compare<"<">(kern::to<Float>(list_length<combat_EnemyRow>(combat_rules_g_enemy_rows)), 3.0))) {
      list_clear(combat_rules_g_enemy_rows);
      return {false};
    }
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<combat_EnemyRow>(combat_rules_g_enemy_rows))))); it10.next();) {
      list_add(combat_rules_g_enemies, struct_make<combat_EnemySpec>(combat_rules_g_enemy_rows[n10.index].name, combat_rules_g_enemy_rows[n10.index].hp, combat_rules_g_enemy_rows[n10.index].speed, combat_rules_g_enemy_rows[n10.index].range, combat_rules_g_enemy_rows[n10.index].shot_s, combat_rules_g_enemy_rows[n10.index].burst, combat_rules_g_enemy_rows[n10.index].dmg, combat_rules_g_enemy_rows[n10.index].acc, combat_rules_g_enemy_rows[n10.index].score, combat_rules_g_enemy_rows[n10.index].scale, combat_rules_g_enemy_rows[n10.index].color, combat_rules_g_enemy_rows[n10.index].stripe));
    }
    return {true};
  }

  struct enemy_specResult { combat_EnemySpec Result = combat_EnemySpec{"", 0.0, 0.0, 0.0, 0.0, 0, 0.0, 0.0, 0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}; };
  KERN_PURE enemy_specResult enemy_spec(combat_EnemyType t = combat_EnemyType::cop) {
    return {logic_select<combat_EnemySpec>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(enum_index<combat_EnemyType>(t)), kern::to<Float>(list_length<combat_EnemySpec>(combat_rules_g_enemies))), 0.5), combat_rules_g_enemies[enum_index<combat_EnemyType>(t)], kNoEnemy)};
  }

  struct bind_funResult { Bool Result = false; };
  /// ---------------------------------------------------------------- "Fun"
  bind_funResult bind_fun() {
    Bool runs = false;
    runs = func_has_blueprint(actor_find("Fun"));
    list_clear(combat_rules_g_contracts);
    combat_kContracts = 0;
    combat_kFunComboS = func_get_on<"FunComboS", Float>(actor_find("Fun"));
    combat_kOverdriveS = func_get_on<"OverdriveS", Float>(actor_find("Fun"));
    combat_kMomentumTarget = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(a); }, func_get_on<"MomentumTarget", Float>(actor_find("Fun"))));
    combat_rules_g_contract_rows = func_get_on<"Contracts", List<combat_ContractRow>>(actor_find("Fun"));
    if (logic_or(logic_not(runs), math_compare<"==">(kern::to<Float>(list_length<combat_ContractRow>(combat_rules_g_contract_rows)), 0.0))) {
      return {false};
    }
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<combat_ContractRow>(combat_rules_g_contract_rows))))); it11.next();) {
      list_add(combat_rules_g_contracts, struct_make<combat_Contract>(combat_rules_g_contract_rows[n11.index].id, combat_rules_g_contract_rows[n11.index].name, combat_rules_g_contract_rows[n11.index].hint, combat_rules_g_contract_rows[n11.index].target, combat_rules_g_contract_rows[n11.index].reward));
    }
    combat_kContracts = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<combat_Contract>(combat_rules_g_contracts))));
    return {true};
  }

  struct contractResult { combat_Contract Result = combat_Contract{"", "", "", 0, 0}; };
  KERN_PURE contractResult contract(I32 index = 0) {
    return {logic_select<combat_Contract>(math_compare<">">(kern::to<Float>(combat_kContracts), 0.0), combat_rules_g_contracts[kern::to<Int>(kern::formula([](Float index, Float combat_kContracts) -> Float { return kern::math::mod((kern::math::mod(index, combat_kContracts) + combat_kContracts), combat_kContracts); }, kern::to<Float>(index), kern::to<Float>(combat_kContracts)))], kNoContract)};
  }

  struct bind_raceResult { Bool Result = false; };
  /// ---------------------------------------------------------------- "Race"
  bind_raceResult bind_race() {
    Bool runs = false;
    I32 n = 0;
    combat_GateRow g = combat_GateRow{"", Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false};
    runs = func_has_blueprint(actor_find("Race"));
    list_clear(combat_rules_g_gate_rows);
    list_clear(combat_rules_g_gates);
    combat_kGates = 0;
    combat_kRaceLaps = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(a); }, func_get_on<"RaceLaps", Float>(actor_find("Race"))));
    combat_kCourseScale = func_get_on<"CourseScale", Float>(actor_find("Race"));
    n = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<combat_GateRow>(func_get_on<"Gates", List<combat_GateRow>>(actor_find("Race"))))));
    if (logic_or(logic_not(runs), math_compare<"<">(kern::to<Float>(n), 2.0))) {
      return {false};
    }
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it12.next();) {
      g = combat_GateRow{"", Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false};
      func_call_on<"Race_GateAt">(actor_find("Race"), kern::out(n13.gate), n12.index);
      g = n13.gate;
      list_add(combat_rules_g_gate_rows, g);
    }
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<combat_GateRow>(combat_rules_g_gate_rows))))); it14.next();) {
      list_add(combat_rules_g_gates, struct_make<combat_Gate>(combat_rules_g_gate_rows[n14.index].name, combat_rules_g_gate_rows[n14.index].center, combat_rules_g_gate_rows[n14.index].width, combat_rules_g_gate_rows[n14.index].height, combat_rules_g_gate_rows[n14.index].start));
    }
    combat_kGates = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<combat_Gate>(combat_rules_g_gates))));
    return {true};
  }

  struct gateResult { combat_Gate Result = combat_Gate{"", Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false}; };
  KERN_PURE gateResult gate(I32 i = 0) {
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
  KERN_PURE bodyResult body(U32 a = 0) {
    physics_Body actor_body = physics_Body{Int64{0x0}};
    actor_body = engine_game_world_actor_body(world_, a);
    return {actor_body};
  }

  struct state_ofResult { BodyState Result = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}; };
  state_ofResult state_of(U32 a = 0) {
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
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
    I32 kind = 0;
    s = "";
    b = "";
    fwd = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{0.0, 1.0, 0.0});
    b = text_format<"\"paused\":%d,\"mode\":\"%s\",\"fwd\":[%.3f,%.3f,%.3f],\"L\":">(511, kern::to<Int>(kern::formula([](Float paused) -> Float { return ((paused != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(ui_.paused))), game_cpp::mode_name(mode_).Result, vector_break(fwd).x, vector_break(fwd).y, vector_break(fwd).z);
    s = text_append(s, b, "");
    web_audio_state_v3(b, s, cam_.eye);
    b = text_format<",\"set\":{\"soundEnabled\":%d,\"musicEnabled\":%d,\"musicVolume\":%.0f,\"droneVolume\":%.0f,\"shotsVolume\":%.0f,\"fxVolume\":%.0f,\"footstepsVolume\":%.0f,\"vehicleVolume\":%.0f,\"ambientVolume\":%.0f}">(511, kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::audio_on).Result))), kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::music_on).Result))), settings::Settings_get(ui_.settings, Opt::music_volume).Result, settings::Settings_get(ui_.settings, Opt::audio_drone).Result, settings::Settings_get(ui_.settings, Opt::audio_shots).Result, settings::Settings_get(ui_.settings, Opt::audio_fx).Result, settings::Settings_get(ui_.settings, Opt::audio_steps).Result, settings::Settings_get(ui_.settings, Opt::audio_vehicle).Result, settings::Settings_get(ui_.settings, Opt::audio_ambient).Result);
    s = text_append(s, b, "");
    ss = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0};
    engine_game_world_soldier_state(n2097162, world_, soldier_, ss);
    soldier_state = n2097162.result;
    ss = n2097162.out;
    have = soldier_state;
    n2097163 = matrix_of(soldier_);
    a = n2097163.Result;
    feet = engine_math_math::Mat4_translation(a).Result;
    air = logic_and(enum_equal<Mode>(mode_, Mode::foot), math_compare<">">(kern::formula([](Float z, Float the_result) -> Float { return (z - the_result); }, vector_break(feet).z, city_stream_cpp::CityStream_ground_height(city_, vector_break(feet).x, vector_break(feet).y).Result), 1.15));
    b = text_format<",\"walk\":{\"dead\":%d,\"air\":%d,\"p\":">(511, kern::to<Int>(kern::formula([](Float have, Float dead) -> Float { return (((have != 0.0) && (dead != 0.0)) ? 1.0 : 0.0); }, kern::to<Float>(have), kern::to<Float>(ss.dead))), kern::to<Int>(kern::formula([](Float air) -> Float { return ((air != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(air))));
    s = text_append(s, b, "");
    web_audio_state_v3(b, s, logic_select<Vector>(enum_equal<Mode>(mode_, Mode::foot), cam_.eye, feet));
    s = text_append(s, "}", "");
    if (drone_out_) {
      b = text_format<",\"drone\":{\"st\":%u,\"arm\":%d,\"d\":%.4f,\"w\":[%.2f,%.2f,%.2f,%.2f],\"mt\":[%.5f,%.5f,%.5f,%.5f],\"pt\":[%.5f,%.5f,%.5f,%.5f],\"p\":">(511, drone_.tele_.state, kern::to<Int>(kern::formula([](Float arm) -> Float { return ((arm != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(sticks_.arm))), drone_.p_.prop_d, drone_.tele_.omega[0], drone_.tele_.omega[1], drone_.tele_.omega[2], drone_.tele_.omega[3], drone_.tele_.motor_torque[0], drone_.tele_.motor_torque[1], drone_.tele_.motor_torque[2], drone_.tele_.motor_torque[3], drone_.tele_.prop_torque[0], drone_.tele_.prop_torque[1], drone_.tele_.prop_torque[2], drone_.tele_.prop_torque[3]);
      s = text_append(s, b, "");
      n2097164 = matrix_of(drone_actor_);
      a_2 = n2097164.Result;
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
    for (auto it2097165 = flow_for(n2097165, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it2097165.next();) {
      skip = false;
      if (logic_not(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n2097165.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetpack)))), 0.5))) {
        skip = true;
      }
      if (logic_not(skip)) {
        b = text_format<"%s[%zu,%.2f,%.2f,%.2f]">(511, logic_select<Text>(first, "", ","), n2097165.index, vector_break(net_.remotes_[n2097165.index].shown).x, vector_break(net_.remotes_[n2097165.index].shown).y, kern::formula([](Float z) -> Float { return (z - 0.6); }, vector_break(net_.remotes_[n2097165.index].shown).z));
        s = text_append(s, b, "");
        first = false;
      }
    }
    s = text_append(s, "],\"rp\":[", "");
    first_2 = true;
    for (auto it2097166 = flow_for(n2097166, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it2097166.next();) {
      skip_2 = false;
      if (logic_not(enum_equal<net_PlayerMode>(net_.remotes_[n2097166.index].state.mode, net_PlayerMode::foot))) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        b = text_format<"%s[%u,%.2f,%.2f,%.2f,%d]">(511, logic_select<Text>(first_2, "", ","), net_.remotes_[n2097166.index].state.slot, vector_break(net_.remotes_[n2097166.index].person_shown).x, vector_break(net_.remotes_[n2097166.index].person_shown).y, vector_break(net_.remotes_[n2097166.index].person_shown).z, kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(net_.remotes_[n2097166.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetpack))))));
        s = text_append(s, b, "");
        first_2 = false;
      }
    }
    s = text_append(s, "],\"pc\":[", "");
    first_3 = true;
    for (auto it2097167 = flow_for(n2097167, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it2097167.next();) {
      skip_3 = false;
      if (logic_or(logic_or(logic_or(math_compare<"<">(kern::to<Float>(units_[n2097167.index].car), 0.0), enum_equal<PoliceUnit_State>(units_[n2097167.index].state, PoliceUnit_State::abandoned)), enum_equal<PoliceUnit_State>(units_[n2097167.index].state, PoliceUnit_State::wreck)), enum_equal<PoliceUnit_State>(units_[n2097167.index].state, PoliceUnit_State::leave))) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(car_index_), kern::to<Float>(units_[n2097167.index].car)))) {
          skip_3 = true;
        }
        if (logic_not(skip_3)) {
          if (logic_not(first_3)) {
            s = text_append(s, ",", "");
          }
          n2097168 = matrix_of(cars_[units_[n2097167.index].car].actor);
          a_3 = n2097168.Result;
          web_audio_state_v3(b, s, vector_add(engine_math_math::Mat4_translation(a_3).Result, Vector{0.0, 0.0, 0.9}));
          first_3 = false;
        }
      }
    }
    s = text_append(s, "],\"pd\":[", "");
    first_3 = true;
    for (auto it2097169 = flow_for(n2097169, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it2097169.next();) {
      skip_4 = false;
      if (logic_or(logic_or(logic_not(pdrones_[n2097169.index].active), pdrones_[n2097169.index].retreating), pdrones_[n2097169.index].emp)) {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        if (logic_not(first_3)) {
          s = text_append(s, ",", "");
        }
        web_audio_state_v3(b, s, pdrones_[n2097169.index].pos);
        first_3 = false;
      }
    }
    s = text_append(s, "],\"fj\":[", "");
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    first_4 = true;
    for (auto it2097170 = flow_for(n2097170, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<FighterJet>(combat_.jets))))); it2097170.next();) {
      b = text_format<"%s[%.2f,%.2f,%.2f,%.3f]">(511, logic_select<Text>(first_4, "", ","), combat_.jets[n2097170.index].x, combat_.jets[n2097170.index].y0, combat_.jets[n2097170.index].z, kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, combat_.jets[n2097170.index].t0));
      s = text_append(s, b, "");
      first_4 = false;
    }
    s = text_append(s, "]", "");
    if (logic_and(enum_equal<Mode>(mode_, Mode::jet), jet_placed_)) {
      b = text_format<",\"jet\":{\"flight\":%d,\"lever\":%.3f,\"nozzle\":%.3f,\"thrust\":%.0f,\"dry\":%.0f,\"cockpit\":%d}">(511, kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(enum_index<Jet_Mode>(jet::Jet_mode(jet_).Result)), kern::to<Float>(enum_index<Jet_Mode>(Jet_Mode::flight)))))), jet::Jet_lever(jet_).Result, jet::Jet_nozzle(jet_).Result, jet::Jet_thrust(jet_).Result, jet_.s_.thrust_dry, kern::to<Int>(kern::formula([](Float jet_cockpit_) -> Float { return ((jet_cockpit_ != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(jet_cockpit_))));
      s = text_append(s, b, "");
    }
    s = text_append(s, ",\"gm\":[", "");
    first_5 = true;
    for (auto it2097171 = flow_for(n2097171, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))); it2097171.next();) {
      skip_5 = false;
      if (logic_not(guided_[n2097171.index].m.motor)) {
        skip_5 = true;
      }
      if (logic_not(skip_5)) {
        b = text_format<"%s[%.3f,%.2f,%.2f,%.2f]">(511, logic_select<Text>(first_5, "", ","), guided_[n2097171.index].m.born, vector_break(guided_[n2097171.index].m.p).x, vector_break(guided_[n2097171.index].m.p).y, vector_break(guided_[n2097171.index].m.p).z);
        s = text_append(s, b, "");
        first_5 = false;
      }
    }
    s = text_append(s, "],\"rk\":[", "");
    first_6 = true;
    for (auto it2097172 = flow_for(n2097172, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it2097172.next();) {
      skip_6 = false;
      if (logic_or(logic_not(projectiles_[n2097172.index].alive), projectiles_[n2097172.index].guided)) {
        skip_6 = true;
      }
      if (logic_not(skip_6)) {
        kind = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((a != 0.0) ? 1.0 : ((a_2 != 0.0) ? 2.0 : ((a_3 != 0.0) ? 3.0 : 0.0))); }, kern::to<Float>(enum_equal<weapons_Id>(projectiles_[n2097172.index].id, weapons_Id::rocket)), kern::to<Float>(enum_equal<weapons_Id>(projectiles_[n2097172.index].id, weapons_Id::drone_rockets)), kern::to<Float>(enum_equal<weapons_Id>(projectiles_[n2097172.index].id, weapons_Id::jet_missile))));
        if (logic_not(math_compare<"!=">(kern::to<Float>(kind), 0.0))) {
          skip_6 = true;
        }
        if (logic_not(skip_6)) {
          b = text_format<"%s[%.3f,%.2f,%.2f,%.2f,%d]">(511, logic_select<Text>(first_6, "", ","), kern::formula([](Float a, Float kind) -> Float { return (a + (0.0001 * kind)); }, projectiles_[n2097172.index].born, kern::to<Float>(kind)), vector_break(projectiles_[n2097172.index].pos).x, vector_break(projectiles_[n2097172.index].pos).y, vector_break(projectiles_[n2097172.index].pos).z, kind);
          s = text_append(s, b, "");
          first_6 = false;
        }
      }
    }
    b = text_format<"],\"gps\":%d">(511, kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(gps_live::GpsLive_active(ui_.gps).Result))));
    s = text_append(s, b, "");
    return {s};
  }

  struct gaps_barricade_doorsResult { Bool Result = false; };
  gaps_barricade_doorsResult gaps_barricade_doors(List<GapsState_DoorView>& out) {
    Int64 key = Int64{0x0};
    Int64 key_2 = Int64{0x0};
    Int64 key_3 = Int64{0x0};
    List<U8> t = List<U8>{};
    Int64 key_4 = Int64{0x0};
    List<U8> put = List<U8>{};
    Int count = 0;
    U8 fill = 0;
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
    map_add(n3145738, gaps_[0].planks, key_2);
    key_3 = key;
    map_add(n3145739, gaps_[0].planks, key_3);
    if (math_compare<"!=">(kern::to<Float>(list_length<U8>(gaps_[0].planks[n3145739.index])), kern::to<Float>(list_length<Interiors_Door>(glife_[0].interiors.plan_.doors)))) {
      key_4 = key;
      map_add(n3145740, gaps_[0].planks, key_4);
      t = gaps_[0].planks[n3145740.index];
      count = list_length<Interiors_Door>(glife_[0].interiors.plan_.doors);
      fill = kern::to<Int>(kern::formula([](Float kPlanks) -> Float { return kern::math::band(kPlanks, 255.0); }, kern::to<Float>(kPlanks)));
      list_clear(t);
      list_resize(t, count, fill);
      put = t;
      gaps_[0].planks[n3145740.index] = put;
    }
    if (math_compare<">">(kern::to<Float>(map_length<Int64, List<U8>>(gaps_[0].planks)), 400.0)) {
      it = 0;
      stop_2 = false;
      while (loop(logic_and(logic_not(stop_2), logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<U8>>(gaps_[0].planks))))))) {
        if (logic_not(int64_equal(map_key_at<Int64, List<U8>>(gaps_[0].planks, it), key))) {
          at = it;
          map_remove_at(gaps_[0].planks, at);
          stop_2 = true;
        }
        if (logic_not(stop_2)) {
          it += 1;
        }
      }
    }
    for (auto it3145741 = flow_for(n3145741, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(glife_[0].interiors.plan_.doors))))); it3145741.next();) {
      key_5 = key;
      map_add(n3145742, gaps_[0].planks, key_5);
      list_add(out, struct_make<GapsState_DoorView>(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n3145741.index))), glife_[0].interiors.plan_.doors[n3145741.index].c, glife_[0].interiors.plan_.doors[n3145741.index].d, glife_[0].interiors.plan_.doors[n3145741.index].n, list_get<U8>(gaps_[0].planks[n3145742.index], n3145741.index), kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((a != 0.0) ? a_2 : a_3); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<Float>(glife_[0].interiors.plan_.levels)), 0.0)), glife_[0].interiors.plan_.base, glife_[0].interiors.plan_.levels[0]), key));
    }
    return {true};
  }

  struct gaps_zombie_goalResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  gaps_zombie_goalResult gaps_zombie_goal(Bool& walled, Bool& hold, I32 person = 0, Vector pos = Vector{0.0, 0.0, 0.0}, Vector player = Vector{0.0, 0.0, 0.0}, Float now = 0.0) {
    List<GapsState_DoorView> doors = List<GapsState_DoorView>{};
    Int key = 0;
    Int z_row = 0;
    Bool p_in = false;
    Int nearest = 0;
    Float dd = 0.0;
    Float k = 0.0;
    GapsState_DoorView door = GapsState_DoorView{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 6, 0.0, Int64{0x0}};
    Vector o = Vector{0.0, 0.0, 0.0};
    Int64 key_2 = Int64{0x0};
    Int door_next_row = 0;
    Int64 key_3 = Int64{0x0};
    Int st_row = 0;
    Object<kern::game::GameWorld> arg = Object<kern::game::GameWorld>{};
    walled = false;
    hold = false;
    doors = List<GapsState_DoorView>{};
    n3145743 = gaps_barricade_doors(doors);
    if (logic_not(n3145743.Result)) {
      return {player};
    }
    key = person;
    map_add(n3145744, gaps_[0].zombie_doors, key);
    z_row = n3145744.index;
    p_in = interiors_cpp::Interiors_inside(glife_[0].interiors, vector_add(player, Vector{0.0, 0.0, 0.3})).Result;
    if (gaps_[0].zombie_doors[z_row].entering) {
      if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(gaps_[0].zombie_doors[z_row].enter_at).x, vector_break(pos).x, vector_break(gaps_[0].zombie_doors[z_row].enter_at).y, vector_break(pos).y), 0.25)) {
        gaps_[0].zombie_doors[z_row].entering = false;
        gaps_[0].zombie_doors[z_row].inside = interiors_cpp::Interiors_inside(glife_[0].interiors, vector_add(pos, Vector{0.0, 0.0, 0.3})).Result;
      } else {
        return {vector_make(vector_break(gaps_[0].zombie_doors[z_row].enter_at).x, vector_break(gaps_[0].zombie_doors[z_row].enter_at).y, vector_break(pos).z)};
      }
    }
    if (gaps_[0].zombie_doors[z_row].inside) {
      if (logic_not(interiors_cpp::Interiors_inside(glife_[0].interiors, vector_add(pos, Vector{0.0, 0.0, 0.3})).Result)) {
        gaps_[0].zombie_doors[z_row].inside = false;
      }
      return {player};
    }
    if (logic_not(p_in)) {
      return {player};
    }
    nearest = list_length<GapsState_DoorView>(doors);
    dd = 1000000000.0;
    for (auto it3145745 = flow_for(n3145745, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<GapsState_DoorView>(doors))))); it3145745.next();) {
      k = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(pos).x, vector_break(doors[n3145745.index].c).x, vector_break(pos).y, vector_break(doors[n3145745.index].c).y);
      if (math_compare<"<">(k, dd)) {
        dd = k;
        nearest = n3145745.index;
      }
    }
    if (math_compare<"==">(kern::to<Float>(nearest), kern::to<Float>(list_length<GapsState_DoorView>(doors)))) {
      return {player};
    }
    door = doors[nearest];
    walled = math_compare<">">(kern::to<Float>(door.planks), 0.0);
    o = vector_add(door.c, vector_scale(door.n, 1.15));
    if (math_compare<">=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(o).x, vector_break(pos).x, vector_break(o).y, vector_break(pos).y), 0.9)) {
      return {vector_make(vector_break(o).x, vector_break(o).y, vector_break(pos).z)};
    }
    if (math_compare<">">(kern::to<Float>(door.planks), 0.0)) {
      hold = true;
      key_2 = int64_add(int64_mul(door.key, Int64{0x1f}), int64_of_int(door.i));
      map_add(n3145746, gaps_[0].door_next_break, key_2);
      door_next_row = n3145746.index;
      if (logic_and(math_compare<">=">(now, gaps_[0].zombie_doors[z_row].next_plank), math_compare<">=">(now, gaps_[0].door_next_break[door_next_row]))) {
        n3145747 = common::Rng_unit(rng_);
        gaps_[0].zombie_doors[z_row].next_plank = kern::formula([](Float now, Float kPlankS, Float the_result) -> Float { return (now + (kPlankS * (0.8 + (the_result * 0.5)))); }, now, kPlankS, n3145747.Result);
        gaps_[0].door_next_break[door_next_row] = kern::formula([](Float now, Float kPlankS) -> Float { return (now + (kPlankS * 0.9)); }, now, kPlankS);
        key_3 = door.key;
        map_add(n3145748, gaps_[0].planks, key_3);
        st_row = n3145748.index;
        if (logic_and(math_compare<"<">(kern::to<Float>(door.i), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<U8>(gaps_[0].planks[st_row]))))))), math_compare<">">(kern::to<Float>(gaps_[0].planks[st_row][door.i]), 0.0))) {
          gaps_[0].planks[st_row][door.i] = kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(gaps_[0].planks[st_row][door.i])));
          townsync_plank(door.key, kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::band(i, 255.0); }, kern::to<Float>(door.i))), kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(list_length<U8>(gaps_[0].planks[st_row])))), -1);
        }
        arg = world_;
        n3145749 = combat_sfx(combat_audio_Kind::bounce);
        engine_game_world_play_sound(arg, n3145749.Result, vector_make(vector_break(door.c).x, vector_break(door.c).y, kern::formula([](Float the_floor) -> Float { return (the_floor + 1.0); }, door.the_floor)), 0.5, 0.55, true, 0);
        gaps_[0].planks_broken = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(gaps_[0].planks_broken)));
      }
      return {vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - (x_2 * 0.2)); }, vector_break(o).x, vector_break(door.n).x), kern::formula([](Float y, Float y_2) -> Float { return (y - (y_2 * 0.2)); }, vector_break(o).y, vector_break(door.n).y), vector_break(pos).z)};
    }
    gaps_[0].zombie_doors[z_row].entering = true;
    gaps_[0].zombie_doors[z_row].enter_at = vector_sub(door.c, vector_scale(door.n, 1.8));
    return {vector_make(vector_break(gaps_[0].zombie_doors[z_row].enter_at).x, vector_break(gaps_[0].zombie_doors[z_row].enter_at).y, vector_break(pos).z)};
  }

  struct gaps_repair_barricadeResult { Bool Result = false; };
  gaps_repair_barricadeResult gaps_repair_barricade() {
    List<GapsState_DoorView> doors = List<GapsState_DoorView>{};
    Vector me = Vector{0.0, 0.0, 0.0};
    Int nearest = 0;
    Float bd = 0.0;
    Float k = 0.0;
    GapsState_DoorView best = GapsState_DoorView{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 6, 0.0, Int64{0x0}};
    List<U8> t = List<U8>{};
    Int64 key = Int64{0x0};
    List<U8> put = List<U8>{};
    Int64 key_2 = Int64{0x0};
    Int64 arg = Int64{0x0};
    U8 arg_2 = 0;
    Object<kern::game::GameWorld> arg_3 = Object<kern::game::GameWorld>{};
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(combat_.zombies.active))) {
      return {false};
    }
    doors = List<GapsState_DoorView>{};
    n3145750 = gaps_barricade_doors(doors);
    if (logic_not(n3145750.Result)) {
      return {false};
    }
    n3145751 = player_position();
    me = n3145751.Result;
    nearest = list_length<GapsState_DoorView>(doors);
    bd = kRepairM;
    for (auto it3145752 = flow_for(n3145752, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<GapsState_DoorView>(doors))))); it3145752.next();) {
      k = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(me).x, vector_break(doors[n3145752.index].c).x, vector_break(me).y, vector_break(doors[n3145752.index].c).y);
      if (math_compare<"<">(k, bd)) {
        bd = k;
        nearest = n3145752.index;
      }
    }
    if (logic_or(logic_or(math_compare<"==">(kern::to<Float>(nearest), kern::to<Float>(list_length<GapsState_DoorView>(doors))), math_compare<">=">(kern::to<Float>(doors[nearest].planks), kern::to<Float>(kPlanks))), math_compare<"<">(kern::to<Float>(combat_.zombies.points), kern::to<Float>(kRepairCost)))) {
      return {false};
    }
    best = doors[nearest];
    key = best.key;
    map_add(n3145753, gaps_[0].planks, key);
    t = gaps_[0].planks[n3145753.index];
    t[best.i] = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(list_get<U8>(gaps_[0].planks[n3145753.index], best.i))));
    put = t;
    gaps_[0].planks[n3145753.index] = put;
    arg = best.key;
    arg_2 = kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::band(i, 255.0); }, kern::to<Float>(best.i)));
    key_2 = best.key;
    map_add(n3145754, gaps_[0].planks, key_2);
    townsync_plank(arg, arg_2, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(list_length<U8>(gaps_[0].planks[n3145754.index])))), 1);
    combat_.zombies.points = kern::to<Int>(kern::formula([](Float points, Float kRepairCost) -> Float { return (points - kRepairCost); }, kern::to<Float>(combat_.zombies.points), kern::to<Float>(kRepairCost)));
    arg_3 = world_;
    n3145755 = combat_sfx(combat_audio_Kind::bounce);
    engine_game_world_play_sound(arg_3, n3145755.Result, cam_.eye, 0.4, 0.7, false, 0);
    osfx("zombie_nights.repairKnock", cam_.eye, 0.0, 0.0, 0.0);
    return {true};
  }

  struct gaps_repair_offerResult { Bool Result = false; };
  gaps_repair_offerResult gaps_repair_offer() {
    Int it = 0;
    Vector me = Vector{0.0, 0.0, 0.0};
    I32 n = 0;
    if (logic_or(logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(combat_.zombies.active)), logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0))), logic_not(interiors::Interiors_active(glife_[0].interiors).Result))) {
      return {false};
    }
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<U8>>(gaps_[0].planks, game_barricades_cpp::building_key(glife_[0].interiors.plan_).Result).index), kern::to<Float>(map_length<Int64, List<U8>>(gaps_[0].planks))));
    n3145756 = player_position();
    me = n3145756.Result;
    for (auto it3145757 = flow_for(n3145757, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(glife_[0].interiors.plan_.doors))))); it3145757.next();) {
      n = kern::to<Int>(kern::formula([](Float a, Float index, Float a_2, Float kPlanks, Float a_3) -> Float { return (((a != 0.0) || (index >= a_2)) ? kPlanks : a_3); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<U8>>(gaps_[0].planks)))), kern::to<Float>(n3145757.index), kern::to<Float>(list_length<U8>(gaps_[0].planks[it])), kern::to<Float>(kPlanks), kern::to<Float>(gaps_[0].planks[it][n3145757.index])));
      if (logic_and(math_compare<"<">(kern::to<Float>(n), kern::to<Float>(kPlanks)), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(me).x, vector_break(glife_[0].interiors.plan_.doors[n3145757.index].c).x, vector_break(me).y, vector_break(glife_[0].interiors.plan_.doors[n3145757.index].c).y), kRepairM))) {
        return {true};
      }
    }
    return {false};
  }

  void gaps_barricades_draw() {
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData box = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
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
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(object_valid<kern::Renderer>(gaps_[0].renderer)))) {
      return;
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].plank_part.id), 0.0))) {
      m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
      m.roughness = 0.85;
      box_2 = engine_primitives_box(vector_make(kern::formula([](Float kDoorW) -> Float { return (kDoorW + 0.5); }, kDoorW), 0.06, 0.2), Vector{0.42, 0.29, 0.17});
      box = box_2;
      engine_renderer_allocate_id(n3145758, gaps_[0].renderer);
      allocate_id = n3145758.result;
      gaps_[0].plank_part.id = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
      look.material = m;
      look.extra_slot = kMaxExtensionAttributes;
      look.visible = false;
      engine_upsert(n3145759, gaps_[0].renderer, gaps_[0].plank_part.id, box, look, List<Float>{}, List<Float>{});
      upsert = n3145759.result;
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
      n3145761 = gaps_barricade_doors(doors);
      both = n3145761.Result;
    }
    if (both) {
      for (auto it3145762 = flow_for(n3145762, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_DoorView>(doors))))); it3145762.next();) {
        yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(doors[n3145762.index].d).y, vector_break(doors[n3145762.index].d).x);
        for (auto it3145763 = flow_for(n3145763, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(doors[n3145762.index].planks)))); it3145763.next();) {
          along = kern::formula([](Float index) -> Float { return ((kern::math::mod(index, 2.0) != 0.0) ? 0.06 : (-0.06)); }, kern::to<Float>(n3145763.index));
          tilt = kern::formula([](Float index) -> Float { return (((kern::math::mod(index, 2.0) != 0.0) ? 1.0 : (-1.0)) * (0.08 + (0.05 * kern::math::mod(index, 3.0)))); }, kern::to<Float>(n3145763.index));
          at = vector_make(kern::formula([](Float x, Float x_2, Float along, Float x_3) -> Float { return ((x + (x_2 * along)) + (x_3 * 0.22)); }, vector_break(doors[n3145762.index].c).x, vector_break(doors[n3145762.index].d).x, along, vector_break(doors[n3145762.index].n).x), kern::formula([](Float y, Float y_2, Float along, Float y_3) -> Float { return ((y + (y_2 * along)) + (y_3 * 0.22)); }, vector_break(doors[n3145762.index].c).y, vector_break(doors[n3145762.index].d).y, along, vector_break(doors[n3145762.index].n).y), kern::formula([](Float a, Float index) -> Float { return ((a + 0.35) + (index * 0.36)); }, doors[n3145762.index].the_floor, kern::to<Float>(n3145763.index)));
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
      engine_renderer_set_visible(n3145764, gaps_[0].renderer, gaps_[0].plank_part.id, false);
      set_visible = n3145764.result;
      return;
    }
    engine_renderer_update_instances(n3145765, gaps_[0].renderer, gaps_[0].plank_part.id, gaps_[0].plank_part.transforms, gaps_[0].plank_part.tints);
    update_instances = n3145765.result;
    engine_renderer_set_visible(n3145766, gaps_[0].renderer, gaps_[0].plank_part.id, true);
    set_visible_2 = n3145766.result;
  }

  struct Game_ctl_discrete_phResult { Bool Result = false; };
  Game_ctl_discrete_phResult Game_ctl_discrete_ph(I32 b = 0) {
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
    both = object_valid<kern::Engine>(engine_);
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
    both = object_valid<kern::Engine>(engine_);
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
    n4194365 = touch_play_cpp::TouchPlay_view_json(pad_touch_);
    raw = n4194365.Result;
    out = "";
    out = text_reserve(out, kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 16.0); }, kern::to<Float>(text_length(raw)))));
    for (auto it4194366 = flow_for(n4194366, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(raw))))); it4194366.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(raw, n4194366.index))));
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

  void key_down_event() {
    controls_cpp::Devices_key(ctl_.dev);
  }

  /// What has bodies in the world and is declared before world_ goes before it (in the order it would have gone): the world's state (the cars of its tow trucks), then the cruiser the mates' cruisers are drawn with.
  void Game_end() {
    Int count = 0;
    Int at = 0;
    if (math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0)) {
      count = list_length<WorldLife_TowTruck>(life_[0].trucks);
      for (auto it5242894 = flow_for(n5242894, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(count)))); it5242894.next();) {
        at = n5242894.index;
        if (math_compare<">">(kern::to<Float>(list_length<Car>(life_[0].trucks[at].car)), 0.0)) {
          car_cpp::Car_end(life_[0].trucks[at].car[0]);
        }
      }
    }
    life_ = List<WorldLife>{};
    if (math_compare<">">(kern::to<Float>(list_length<Car>(swarm_cruiser_)), 0.0)) {
      car_cpp::Car_end(swarm_cruiser_[0]);
    }
    swarm_cruiser_ = List<Car>{};
  }

  void stop() {
    Int count = 0;
    Int at = 0;
    Int count_2 = 0;
    Int at_2 = 0;
    Object<kern::game::GameWorld> old = Object<kern::game::GameWorld>{};
    if (math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)) {
      ragdoll_cpp::Ragdolls_end(glife_[0].life.ragdolls_);
    }
    glife_ = List<GameLife>{};
    if (math_compare<">">(kern::to<Float>(list_length<Car>(remote_bus_)), 0.0)) {
      car_cpp::Car_end(remote_bus_[0]);
    }
    remote_bus_ = List<Car>{};
    if (math_compare<">">(kern::to<Float>(list_length<Car>(remote_car_)), 0.0)) {
      car_cpp::Car_end(remote_car_[0]);
    }
    remote_car_ = List<Car>{};
    count = list_length<TrafficCar>(cars_);
    for (auto it5242897 = flow_for(n5242897, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(count)))); it5242897.next();) {
      at = n5242897.index;
      if (math_compare<">">(kern::to<Float>(list_length<Car>(cars_[at].car)), 0.0)) {
        car_cpp::Car_end(cars_[at].car[0]);
      }
    }
    list_clear(cars_);
    if (math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0)) {
      count_2 = list_length<WorldLife_TowTruck>(life_[0].trucks);
      for (auto it5242898 = flow_for(n5242898, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(count_2)))); it5242898.next();) {
        at_2 = n5242898.index;
        if (math_compare<">">(kern::to<Float>(list_length<Car>(life_[0].trucks[at_2].car)), 0.0)) {
          car_cpp::Car_end(life_[0].trucks[at_2].car[0]);
        }
      }
    }
    life_ = List<WorldLife>{};
    if (math_compare<">">(kern::to<Float>(list_length<Car>(swarm_cruiser_)), 0.0)) {
      car_cpp::Car_end(swarm_cruiser_[0]);
    }
    swarm_cruiser_ = List<Car>{};
    old = world_;
    world_ = Object<kern::game::GameWorld>{};
    if (object_valid<kern::game::GameWorld>(old)) {
      engine_game_world_release(old);
    }
  }

  struct matrix_ofResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  matrix_ofResult matrix_of(U32 a = 0) {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool actor_matrix = false;
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    engine_game_world_actor_matrix(n5242899, world_, a, m);
    actor_matrix = n5242899.result;
    m = n5242899.out;
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
      n5242900 = matrix_of(cars_[car_index_].actor);
      a = n5242900.Result;
      return {engine_math_math::Mat4_translation(a).Result};
    }
    if (enum_equal<Mode>(mode_, Mode::jet)) {
      n5242901 = matrix_of(jet_actor_);
      a_2 = n5242901.Result;
      return {engine_math_math::Mat4_translation(a_2).Result};
    }
    if (enum_equal<Mode>(mode_, Mode::parachute)) {
      n5242902 = matrix_of(chute_actor_);
      a_3 = n5242902.Result;
      return {engine_math_math::Mat4_translation(a_3).Result};
    }
    if (gaps_riding().Result) {
      p = Vector{0.0, 0.0, 0.0};
      q = Quat{0.0, 0.0, 0.0, 1.0};
      both = logic_not(gaps_ride_car().Result);
      if (both) {
        n5242904 = gaps_ride_pose(p, q);
        both = n5242904.Result;
      }
      if (both) {
        return {p};
      }
    }
    n5242905 = matrix_of(soldier_);
    a_4 = n5242905.Result;
    return {engine_math_math::Mat4_translation(a_4).Result};
  }

  struct focus_positionResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  focus_positionResult focus_position() {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_and(enum_equal<Mode>(mode_, Mode::drone), drone_out_)) {
      n5242906 = matrix_of(drone_actor_);
      a = n5242906.Result;
      return {engine_math_math::Mat4_translation(a).Result};
    }
    n5242907 = player_position();
    return {n5242907.Result};
  }

  struct Game_clear_spot_on_roadResult { Bool Result = false; };
  Game_clear_spot_on_roadResult Game_clear_spot_on_road(Bool off_road = false, Vector half = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}) {
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
    for (auto it5242908 = flow_for(n5242908, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it5242908.next();) {
      skip = false;
      if (logic_or(logic_not(city_.lanes_[n5242908.index].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(city_.lanes_[n5242908.index].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(city_.lanes_[n5242908.index].points))), 0.5))) {
          a = city_.lanes_[n5242908.index].points[k];
          b = city_.lanes_[n5242908.index].points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))];
          ab = vector_sub(b, a);
          l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(ab).x, vector_break(ab).y);
          t = kern::formula([](Float l2, Float a) -> Float { return ((l2 > 1e-06) ? a : 0.0); }, l2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(c).x, vector_break(a).x, vector_break(ab).x, vector_break(c).y, vector_break(a).y, vector_break(ab).y, l2), 0.0, 1.0));
          q = vector_add(a, vector_scale(ab, t));
          if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(q).x, vector_break(c).y, vector_break(q).y), kern::formula([](Float reach, Float a) -> Float { return (reach + (a * 0.5)); }, reach, city_.lanes_[n5242908.index].width))) {
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
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Float r = 0.0;
    I32 steps = 0;
    Float a = 0.0;
    Vector c = Vector{0.0, 0.0, 0.0};
    Bool ok = false;
    I32 ix = 0;
    I32 iy = 0;
    Vector o = Vector{0.0, 0.0, 0.0};
    the_RayResult_ h = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Bool both = false;
    engine_game_world_physics(n5242909, world_);
    physics = n5242909.result;
    w = physics;
    r = 0.0;
    while (loop(math_compare<"<=">(r, search))) {
      steps = kern::to<Int>(kern::formula([](Float r, Float kPi) -> Float { return ((r == 0.0) ? 1.0 : kern::math::trunc(kern::math::div(((2.0 * kPi) * r), 8.0))); }, r, kPi));
      for (auto it5242910 = flow_for(n5242910, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(steps)))); it5242910.next();) {
        a = kern::formula([](Float kPi, Float index, Float steps) -> Float { return kern::math::div(((2.0 * kPi) * index), steps); }, kPi, kern::to<Float>(n5242910.index), kern::to<Float>(steps));
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
        both = ok;
        if (both) {
          n5242912 = Game_clear_spot_on_road(off_road, half, vector_make(vector_break(c).x, vector_break(c).y, 0.0));
          both = logic_not(n5242912.Result);
        }
        if (both) {
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
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Float z = 0.0;
    ground = city_stream_cpp::CityStream_ground_height(city_, vector_break(feet).x, vector_break(feet).y).Result;
    engine_game_world_physics(n5242922, world_);
    physics = n5242922.result;
    r = common::cast(physics, vector_make(vector_break(feet).x, vector_break(feet).y, kern::formula([](Float z, Float ground) -> Float { return (kern::math::max(z, ground) + 1.0); }, vector_break(feet).z, ground)), Vector{0.0, 0.0, -1.0}, kern::formula([](Float z, Float ground) -> Float { return ((1.0 + kern::math::abs((z - ground))) + 1.0); }, vector_break(feet).z, ground), body(soldier_).Result, Int64{0x1}).Result;
    z = ground;
    if (logic_and(logic_and(r.hit, math_compare<">">(vector_break(r.normal).z, 0.6)), math_compare<">">(vector_break(r.point).z, kern::formula([](Float ground) -> Float { return (ground - 0.05); }, ground)))) {
      z = vector_break(r.point).z;
    }
    return {vector_make(vector_break(feet).x, vector_break(feet).y, kern::formula([](Float z) -> Float { return (z + 0.02); }, z))};
  }

  void put_down(Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw_deg = 0.0) {
    Vector rot = Vector{0.0, 0.0, 0.0};
    Object<kern::game::GameWorld> arg = Object<kern::game::GameWorld>{};
    Int arg_2 = 0;
    Bool teleport = false;
    Bool set_actor_velocity = false;
    Float sim_time = 0.0;
    rot = vector_make(0.0, 0.0, yaw_deg);
    arg = world_;
    arg_2 = soldier_;
    n5242923 = standing_feet(feet);
    engine_game_world_teleport(n5242924, arg, arg_2, vector_add(n5242923.Result, vector_make(0.0, 0.0, kern::formula([](Float kSpawnLift) -> Float { return (0.73 + kSpawnLift); }, kSpawnLift))), list_make<Vector, "1">(rot));
    teleport = n5242924.result;
    engine_game_world_set_actor_velocity(n5242925, world_, soldier_, Vector{0.0, 0.0, 0.0});
    set_actor_velocity = n5242925.result;
    spawn_guard_ = vector_make(vector_break(feet).x, vector_break(feet).y, yaw_deg);
    sim_time = engine_game_world_sim_time(world_);
    spawn_guard_until_ = kern::formula([](Float sim_time) -> Float { return (sim_time + 20.0); }, sim_time);
    spawn_guard_ready_at_ = -1.0;
  }

  void spawn_guard() {
    Vector jp = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Game_Spot here = Game_Spot{Vector{0.0, 0.0, 0.0}, 0.0};
    Game_Spot there = Game_Spot{Vector{0.0, 0.0, 0.0}, 0.0};
    Bool both = false;
    Vector me0 = Vector{0.0, 0.0, 0.0};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector rot = Vector{0.0, 0.0, 0.0};
    Bool teleport = false;
    Vector me = Vector{0.0, 0.0, 0.0};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector p = Vector{0.0, 0.0, 0.0};
    Float heading = 0.0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Fixed<Float, 3> items = Fixed<Float, 3>{};
    Float r = 0.0;
    if (logic_or(logic_or(spawn_checked_, logic_not(enum_equal<Mode>(mode_, Mode::foot))), math_compare<"<">(city_.stats_.t_collision, 0.0))) {
      return;
    }
    spawn_checked_ = true;
    if (logic_and(logic_and(jet_placed_, logic_not(enum_equal<Mode>(mode_, Mode::jet))), logic_not(jet_.pilot))) {
      n5242913 = matrix_of(jet_actor_);
      a = n5242913.Result;
      jp = engine_math_math::Mat4_translation(a).Result;
      here = struct_make<Game_Spot>(Vector{0.0, 0.0, 0.0}, 0.0);
      there = struct_make<Game_Spot>(Vector{0.0, 0.0, 0.0}, 0.0);
      n5242914 = clear_spot(here, jp, Vector{16.0, 20.0, 0.0}, 0.0, true);
      both = logic_not(n5242914.Result);
      if (both) {
        n5242915 = clear_spot(there, jp, Vector{16.0, 20.0, 0.0}, 260.0, true);
        both = n5242915.Result;
      }
      if (both) {
        n5242916 = matrix_of(soldier_);
        a_2 = n5242916.Result;
        me0 = engine_math_math::Mat4_translation(a_2).Result;
        rot = vector_make(0.0, 0.0, kern::formula([](Float y, Float y_2, Float x, Float x_2, Float kDeg) -> Float { return (kern::math::div(kern::math::atan2((y - y_2), (x - x_2)), kDeg) - 90.0); }, vector_break(me0).y, vector_break(there.p).y, vector_break(me0).x, vector_break(there.p).x, kDeg));
        engine_game_world_teleport(n5242917, world_, jet_actor_, vector_add(there.p, Vector{0.0, 0.0, 2.1}), list_make<Vector, "1">(rot));
        teleport = n5242917.result;
      }
    }
    n5242918 = matrix_of(soldier_);
    a_3 = n5242918.Result;
    me = engine_math_math::Mat4_translation(a_3).Result;
    if (logic_not(city_stream_cpp::CityStream_building_at(city_, vector_make(vector_break(me).x, vector_break(me).y, 0.0)).Result)) {
      return;
    }
    p = Vector{0.0, 0.0, 0.0};
    heading = 0.0;
    item = 40.0;
    item_2 = 80.0;
    item_3 = 160.0;
    items = list_make<Float, "3">(item, item_2, item_3);
    for (auto it5242919 = flow_for(n5242919, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it5242919.next();) {
      r = items[n5242919.index];
      for (auto it5242920 = flow_for(n5242920, 0, 5); it5242920.next();) {
        n5242921 = city_stream_cpp::CityStream_street_point(city_, rng_, p, heading, me, r);
        if (logic_and(n5242921.Result, logic_not(city_stream_cpp::CityStream_building_at(city_, vector_make(vector_break(p).x, vector_break(p).y, 0.0)).Result))) {
          put_down(p, kern::formula([](Float heading, Float kDeg) -> Float { return kern::math::div(heading, kDeg); }, heading, kDeg));
          return;
        }
      }
    }
  }

  struct Game_place_world_takenResult { Bool Result = false; };
  Game_place_world_takenResult Game_place_world_taken(Vector at = Vector{0.0, 0.0, 0.0}) {
    for (auto it5242927 = flow_for(n5242927, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it5242927.next();) {
      if (logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n5242927.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetOut)))), 0.5), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(net_.remotes_[n5242927.index].state.jet_pos).x, vector_break(at).x, vector_break(net_.remotes_[n5242927.index].state.jet_pos).y, vector_break(at).y), 24.0))) {
        return {true};
      }
    }
    return {false};
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
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    Fixed<Float, 4> items = Fixed<Float, 4>{};
    Int at_2 = 0;
    Int count = 0;
    Bool stop_2 = false;
    Float turn = 0.0;
    Vector c = Vector{0.0, 0.0, 0.0};
    Bool open = false;
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Object<kern::physics::World> physics_2 = Object<kern::physics::World>{};
    Bool both = false;
    the_RayResult_ g = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Object<kern::physics::World> physics_3 = Object<kern::physics::World>{};
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
      engine_game_world_set_mode(n5242933, world_, game_SimMode::play);
      set_mode = n5242933.result;
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
    n5242934 = matrix_of(soldier_);
    a = n5242934.Result;
    feet = vector_sub(engine_math_math::Mat4_translation(a).Result, Vector{0.0, 0.0, 0.73});
    camera_yaw = engine_game_world_camera_yaw(world_);
    yaw = kern::formula([](Float camera_yaw, Float kDeg) -> Float { return (camera_yaw * kDeg); }, camera_yaw, kDeg);
    at = vector_add(feet, vector_scale(vector_make(kern::formula([](Float yaw) -> Float { return kern::math::cos(yaw); }, yaw), kern::formula([](Float yaw) -> Float { return kern::math::sin(yaw); }, yaw), 0.0), kDroneAhead));
    item = 0.0;
    item_2 = kern::formula([](Float kPi) -> Float { return (0.5 * kPi); }, kPi);
    item_3 = kern::formula([](Float kPi) -> Float { return ((-0.5) * kPi); }, kPi);
    item_4 = kPi;
    items = list_make<Float, "4">(item, item_2, item_3, item_4);
    count = list_length<Float>(items);
    at_2 = 0;
    stop_2 = false;
    while (loop(logic_and(logic_not(stop_2), math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(count))))) {
      turn = items[at_2];
      c = vector_add(feet, vector_scale(vector_make(kern::formula([](Float yaw, Float turn) -> Float { return kern::math::cos((yaw + turn)); }, yaw, turn), kern::formula([](Float yaw, Float turn) -> Float { return kern::math::sin((yaw + turn)); }, yaw, turn), 0.0), kDroneAhead));
      engine_game_world_physics(n5242935, world_);
      physics = n5242935.result;
      both = logic_not(struct_break<the_RayResult_>(common::cast(physics, vector_add(c, Vector{0.0, 0.0, 0.3}), Vector{0.0, 0.0, 1.0}, 25.0, body(soldier_).Result, Int64{0xffffffffffffffff}).Result).hit);
      if (both) {
        engine_game_world_physics(n5242936, world_);
        physics_2 = n5242936.result;
        both = logic_not(struct_break<the_RayResult_>(common::cast(physics_2, vector_add(feet, Vector{0.0, 0.0, 0.3}), logic_select<Vector>(math_compare<">">(vector_length(vector_sub(c, feet)), 9.999999717180685e-10), vector_normalize(vector_sub(c, feet)), Vector{0.0, 0.0, 1.0}), kern::formula([](Float kDroneAhead) -> Float { return (kDroneAhead + 0.3); }, kDroneAhead), body(soldier_).Result, Int64{0xffffffffffffffff}).Result).hit);
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
    engine_game_world_physics(n5242937, world_);
    physics_3 = n5242937.result;
    g = common::cast(physics_3, vector_add(at, Vector{0.0, 0.0, 3.0}), Vector{0.0, 0.0, -1.0}, 10.0, body(soldier_).Result, Int64{0xffffffffffffffff}).Result;
    at = vector_make(vector_break(at).x, vector_break(at).y, kern::formula([](Float hit, Float z, Float z_2) -> Float { return (((hit != 0.0) ? z : z_2) + 0.0225); }, kern::to<Float>(g.hit), vector_break(g.point).z, vector_break(feet).z));
    engine_game_world_set_alive(world_, drone_actor_, true);
    camera_yaw_2 = engine_game_world_camera_yaw(world_);
    rot = vector_make(0.0, 0.0, kern::formula([](Float camera_yaw_2) -> Float { return (camera_yaw_2 + 180.0); }, camera_yaw_2));
    engine_game_world_teleport(n5242938, world_, drone_actor_, at, list_make<Vector, "1">(rot));
    teleport = n5242938.result;
    engine_game_world_set_actor_velocity(n5242939, world_, drone_actor_, Vector{0.0, 0.0, 0.0});
    set_actor_velocity = n5242939.result;
    drone_cpp::Drone_reset(drone_);
    sticks_ = struct_make<DroneSticks>(0.0, 0.0, 0.0, 0.0, 0.0, 1.2, false, true, false);
    sticks_.clearance = 1.2;
    landing_ = false;
    drone_out_ = true;
    engine_game_world_set_mode(n5242940, world_, game_SimMode::simulate);
    set_mode_2 = n5242940.result;
    mode_ = Mode::drone;
    rig_init_ = false;
  }

  void hide_soldier() {
    Bool set_mode = false;
    engine_game_world_set_alive(world_, soldier_, false);
    engine_game_world_set_mode(n5242941, world_, game_SimMode::simulate);
    set_mode = n5242941.result;
  }

  void show_soldier(Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw_deg = 0.0) {
    Bool actor_alive = false;
    Bool set_mode = false;
    Vector rot = Vector{0.0, 0.0, 0.0};
    actor_alive = engine_game_world_actor_alive(world_, soldier_);
    if (logic_not(actor_alive)) {
      engine_game_world_set_alive(world_, soldier_, true);
    }
    engine_game_world_set_mode(n5242942, world_, game_SimMode::play);
    set_mode = n5242942.result;
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
    n5242943 = matrix_of(soldier_);
    a = n5242943.Result;
    at = engine_math_math::Mat4_translation(a).Result;
    if (logic_and(math_compare<">">(now, spawn_guard_until_), logic_or(math_compare<">=">(spawn_guard_ready_at_, 0.0), terrain_cpp::Terrain_ground_ready(city_.terrain_, vector_break(at).x, vector_break(at).y).Result))) {
      spawn_guard_until_ = -1.0;
      return;
    }
    n5242944 = matrix_of(soldier_);
    a_2 = n5242944.Result;
    c = engine_math_math::Mat4_translation(a_2).Result;
    feet = vector_make(vector_break(c).x, vector_break(c).y, kern::formula([](Float z) -> Float { return (z - 0.73); }, vector_break(c).z));
    ready = terrain_cpp::Terrain_ground_ready(city_.terrain_, vector_break(feet).x, vector_break(feet).y).Result;
    n5242945 = standing_feet(feet);
    stand = n5242945.Result;
    if (logic_not(ready)) {
      hold = vector_add(stand, vector_make(0.0, 0.0, kSpawnLift));
      if (math_compare<">">(kern::formula([](Float z, Float z_2) -> Float { return kern::math::abs((z - z_2)); }, vector_break(feet).z, vector_break(hold).z), 0.05)) {
        rot = vector_make(0.0, 0.0, vector_break(spawn_guard_).z);
        engine_game_world_teleport(n5242946, world_, soldier_, vector_add(hold, Vector{0.0, 0.0, 0.73}), list_make<Vector, "1">(rot));
        teleport = n5242946.result;
      }
      engine_game_world_set_actor_velocity(n5242947, world_, soldier_, Vector{0.0, 0.0, 0.0});
      set_actor_velocity = n5242947.result;
      spawn_guard_ready_at_ = -1.0;
      return;
    }
    if (math_compare<"<">(spawn_guard_ready_at_, 0.0)) {
      spawn_guard_ready_at_ = now;
    }
    if (math_compare<"<">(vector_break(feet).z, kern::formula([](Float z) -> Float { return (z - 0.25); }, vector_break(stand).z))) {
      engine_game_world_teleport(n5242948, world_, soldier_, vector_add(stand, Vector{0.0, 0.0, 0.83}), List<Vector>{});
      teleport_2 = n5242948.result;
      engine_game_world_set_actor_velocity(n5242949, world_, soldier_, Vector{0.0, 0.0, 0.0});
      set_actor_velocity_2 = n5242949.result;
    }
    if (math_compare<">">(kern::formula([](Float now, Float spawn_guard_ready_at_) -> Float { return (now - spawn_guard_ready_at_); }, now, spawn_guard_ready_at_), 2.0)) {
      spawn_guard_until_ = -1.0;
    }
  }

  void switch_mode(Mode m = Mode::foot, I32 vehicle = 0) {
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Bool body_valid = false;
    Object<kern::physics::World> physics_2 = Object<kern::physics::World>{};
    if (logic_and(enum_equal<Mode>(m, mode_), logic_or(logic_not(enum_equal<Mode>(m, Mode::car)), math_compare<"==">(kern::to<Float>(vehicle), kern::to<Float>(car_index_))))) {
      return;
    }
    if (enum_equal<Mode>(mode_, Mode::drone)) {
      toggle_drone();
    }
    switch (flow_switch_enum<Mode>(m)) {
      case Mode::foot: {
      } break;
      case Mode::drone: {
        toggle_drone();
        return;
      } break;
      case Mode::car: {
        if (logic_and(cars_[vehicle].driver, cars_[vehicle].placed)) {
          wanted_cpp::report(wanted_Crime::carjacking, true);
          eject_driver(vehicle);
          toast_ = "Auto geklaut! Der Fahrer ist raus.";
          toast_age_ = 0.0;
        }
        cars_[vehicle].driver = false;
        car_index_ = vehicle;
        engine_game_world_physics(n5242950, world_);
        physics = n5242950.result;
        body_valid = engine_world_body_valid(physics, body(cars_[vehicle].actor).Result);
        if (body_valid) {
          engine_game_world_physics(n5242951, world_);
          physics_2 = n5242951.result;
          engine_world_set_awake(physics_2, body(cars_[vehicle].actor).Result, true);
        }
        hide_soldier();
        car_cockpit_ = vr_;
      } break;
      case Mode::jet: {
        hide_soldier();
        jet_.pilot = true;
        jet_.engine_on = true;
      } break;
      case Mode::parachute: {
      } break;
    }
    mode_ = m;
    rig_init_ = false;
    head_pitch_ = 0.0;
    head_yaw_ = head_pitch_;
  }

  void eject() {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    n5242955 = state_of(jet_actor_);
    s = n5242955.Result;
    jet_.pilot = false;
    engine_game_world_set_alive(world_, chute_actor_, true);
    engine_game_world_teleport(n5242956, world_, chute_actor_, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 2.8}).Result), List<Vector>{});
    teleport = n5242956.result;
    engine_game_world_set_actor_velocity(n5242957, world_, chute_actor_, vector_add(s.velocity, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kEjectUp)).Result));
    set_actor_velocity = n5242957.result;
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
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    BodyState s_2 = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector right = Vector{0.0, 0.0, 0.0};
    Vector out_2 = Vector{0.0, 0.0, 0.0};
    if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0))) {
      n5242952 = state_of(cars_[car_index_].actor);
      s = n5242952.Result;
      left = engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 1.0, 0.0}).Result;
      side = kern::formula([](Float y) -> Float { return (y + 0.9); }, vector_break(cars_[car_index_].car[0].s_.half).y);
      out = vector_add(s.position, vector_scale(left, side));
      engine_game_world_physics(n5242953, world_);
      physics = n5242953.result;
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
      n5242954 = state_of(jet_actor_);
      s_2 = n5242954.Result;
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
    I32 n = 0;
    Int L_at = 0;
    Vector end = Vector{0.0, 0.0, 0.0};
    I32 next = 0;
    I32 next_dir = 0;
    U32 start_2 = 0;
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
    n5242962 = common::Rng_next(rng_);
    start_2 = n5242962.Result;
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
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    physics_Body b = physics_Body{Int64{0x0}};
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float m = 0.0;
    Float mass = 0.0;
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector want = Vector{0.0, 0.0, 0.0};
    engine_game_world_physics(n5242963, world_);
    physics = n5242963.result;
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
    for (auto it5242965 = flow_for(n5242965, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(mates))))); it5242965.next();) {
      d = kern::formula([](Float d, Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::min(d, kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2)))); }, d, vector_break(p).x, vector_break(mates[n5242965.index]).x, vector_break(p).y, vector_break(mates[n5242965.index]).y);
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
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    physics_RayHit cast_sphere = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    d = vector_sub(want, anchor);
    engine_game_world_physics(n5242967, world_);
    physics = n5242967.result;
    cast_sphere = engine_world_cast_sphere(physics, anchor, 0.17, d, engine_physics_physics::QueryFilter_new(Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}).Result, ignore);
    h = cast_sphere;
    return {logic_select<Vector>(h.hit, vector_add(anchor, vector_scale(d, kern::formula([](Float fraction) -> Float { return kern::math::max(0.0, (fraction - 0.02)); }, h.fraction))), want)};
  }

  void update_camera(Object<kern::Engine> e = Object<kern::Engine>{}, Float dt = 0.0) {
    Camera c = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
    Float aspect = 0.0;
    Float aspect_2 = 0.0;
    Bool update_camera_2 = false;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector pos = Vector{0.0, 0.0, 0.0};
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 0.0};
    Vector vel = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    Float t = 0.0;
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector u = Vector{0.0, 0.0, 0.0};
    Vector h = Vector{0.0, 0.0, 0.0};
    Float hl = 0.0;
    Bool third = false;
    Float back = 0.0;
    Float upm = 0.0;
    Float ahead = 0.0;
    Float look_up = 0.0;
    Vector hd = Vector{0.0, 0.0, 0.0};
    Vector want = Vector{0.0, 0.0, 0.0};
    Bool left = false;
    Mat4 m_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector pos_2 = Vector{0.0, 0.0, 0.0};
    Vector fwd_2 = Vector{0.0, 0.0, 0.0};
    Bool bus = false;
    Mat4 seat = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector look = Vector{0.0, 0.0, 0.0};
    Float zoom = 0.0;
    Vector hd_2 = Vector{0.0, 0.0, 0.0};
    Vector look_dir = Vector{0.0, 0.0, 0.0};
    Quat a_2 = Quat{0.0, 0.0, 0.0, 1.0};
    Float cz = 0.0;
    Vector tgt = Vector{0.0, 0.0, 0.0};
    Vector want_2 = Vector{0.0, 0.0, 0.0};
    Mat4 m_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    BodyState s_2 = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector pos_3 = Vector{0.0, 0.0, 0.0};
    Vector nose = Vector{0.0, 0.0, 0.0};
    Vector up_2 = Vector{0.0, 0.0, 0.0};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float sp = 0.0;
    Float dist = 0.0;
    Vector want_3 = Vector{0.0, 0.0, 0.0};
    Vector pos_4 = Vector{0.0, 0.0, 0.0};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    c = cam_;
    c.near = 0.05;
    c.far = 2400.0;
    aspect_2 = engine_engine_aspect(e);
    aspect = aspect_2;
    if (logic_not(world_placed_)) {
      c.eye = vector_make(0.0, 0.0, kern::formula([](Float the_result) -> Float { return (the_result + 1.68); }, city_stream_cpp::CityStream_ground_height(city_, 0.0, 0.0).Result));
      c.target = vector_add(c.eye, Vector{10.0, 0.0, 0.0});
      c.up = Vector{0.0, 0.0, 1.0};
      c.fov = kern::formula([](Float kDeg) -> Float { return (50.0 * kDeg); }, kDeg);
      cam_ = c;
      return;
    }
    switch (flow_switch_enum<Mode>(mode_)) {
      case Mode::foot: {
        engine_game_world_update_camera(n5242968, world_, c, dt, play_in_);
        update_camera_2 = n5242968.result;
        c = n5242968.camera;
        cam_valid_ = update_camera_2;
      } break;
      case Mode::drone: {
        n5242969 = matrix_of(drone_actor_);
        m = n5242969.Result;
        pos = engine_math_math::Mat4_translation(m).Result;
        n5242970 = state_of(drone_actor_);
        q = struct_break<BodyState>(n5242970.Result).rotation;
        fwd = engine_math_math::Quat_rotate(q, Vector{-1.0, 0.0, 0.0}).Result;
        up = engine_math_math::Quat_rotate(q, Vector{0.0, 0.0, 1.0}).Result;
        the_actor_velocity = engine_game_world_actor_velocity(world_, drone_actor_);
        vel = the_actor_velocity;
        if (math_compare<"==">(kern::to<Float>(drone_cam_), 0.0)) {
          t = kern::formula([](Float fpv_tilt_deg_, Float kDeg) -> Float { return (fpv_tilt_deg_ * kDeg); }, fpv_tilt_deg_, kDeg);
          f = vector_add(vector_scale(fwd, kern::formula([](Float t) -> Float { return kern::math::cos(t); }, t)), vector_scale(up, kern::formula([](Float t) -> Float { return kern::math::sin(t); }, t)));
          u = vector_sub(vector_scale(up, kern::formula([](Float t) -> Float { return kern::math::cos(t); }, t)), vector_scale(fwd, kern::formula([](Float t) -> Float { return kern::math::sin(t); }, t)));
          c.eye = vector_add(vector_add(pos, vector_scale(fwd, 0.102)), vector_scale(up, 0.073));
          c.target = vector_add(c.eye, vector_scale(f, 4.0));
          c.up = u;
          c.fov = kern::formula([](Float fpv_fov_deg_, Float kDeg) -> Float { return (fpv_fov_deg_ * kDeg); }, fpv_fov_deg_, kDeg);
          c.near = 0.02;
        } else {
          h = vector_make(vector_break(fwd).x, vector_break(fwd).y, 0.0);
          hl = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt(((x * x) + (y * y))); }, vector_break(h).x, vector_break(h).y);
          h = logic_select<Vector>(math_compare<">">(hl, 0.2), vector_scale(h, kern::formula([](Float hl) -> Float { return kern::math::div(1.0, hl); }, hl)), cam_heading_);
          third = math_compare<"==">(kern::to<Float>(drone_cam_), 2.0);
          Game_update_camera_rig(dt, pos, vel, h, kern::formula([](Float third) -> Float { return ((third != 0.0) ? 3.6 : 4.8); }, kern::to<Float>(third)), kern::formula([](Float third) -> Float { return ((third != 0.0) ? 5.2 : 7.0); }, kern::to<Float>(third)), kern::formula([](Float third) -> Float { return ((third != 0.0) ? 3.8 : 7.5); }, kern::to<Float>(third)), kern::formula([](Float third) -> Float { return ((third != 0.0) ? 0.36 : 0.18); }, kern::to<Float>(third)));
          back = kern::formula([](Float third) -> Float { return ((third != 0.0) ? kern::math::div((1.5 * 2.25), 2.4847) : 1.65); }, kern::to<Float>(third));
          upm = kern::formula([](Float third) -> Float { return ((third != 0.0) ? kern::math::div((1.5 * 1.05), 2.4847) : 0.78); }, kern::to<Float>(third));
          ahead = kern::formula([](Float third) -> Float { return ((third != 0.0) ? 0.55 : 0.38); }, kern::to<Float>(third));
          look_up = kern::formula([](Float third) -> Float { return ((third != 0.0) ? 0.18 : 0.1); }, kern::to<Float>(third));
          hd = vector_make(vector_break(cam_heading_).x, vector_break(cam_heading_).y, 0.0);
          want = vector_add(vector_sub(cam_anchor_, vector_scale(hd, back)), vector_make(0.0, 0.0, upm));
          n5242971 = Game_update_camera_collide(cam_anchor_, want, body(drone_actor_).Result);
          c.eye = n5242971.Result;
          c.target = vector_add(vector_add(cam_anchor_, vector_scale(hd, ahead)), vector_make(0.0, 0.0, look_up));
          c.up = Vector{0.0, 0.0, 1.0};
          c.fov = kern::formula([](Float third, Float a, Float a_2, Float kDeg) -> Float { return (((third != 0.0) ? a : a_2) * kDeg); }, kern::to<Float>(third), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float fpv_fov_deg_) -> Float { return kern::math::div((62.0 * fpv_fov_deg_), 105.0); }, fpv_fov_deg_), 35.0, 100.0), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float fpv_fov_deg_) -> Float { return kern::math::div((52.0 * fpv_fov_deg_), 105.0); }, fpv_fov_deg_), 30.0, 90.0), kDeg);
          c.near = 0.06;
        }
      } break;
      case Mode::car: {
        left = false;
        if (math_compare<"<">(kern::to<Float>(car_index_), 0.0)) {
          left = true;
        }
        if (logic_not(left)) {
          n5242972 = matrix_of(cars_[car_index_].actor);
          m_2 = n5242972.Result;
          n5242973 = state_of(cars_[car_index_].actor);
          s = n5242973.Result;
          pos_2 = engine_math_math::Mat4_translation(m_2).Result;
          fwd_2 = engine_math_math::Quat_rotate(s.rotation, Vector{1.0, 0.0, 0.0}).Result;
          bus = math_compare<">">(cars_[car_index_].car[0].s_.radius, 0.4);
          if (car_cockpit_) {
            a = engine_math_math::Mat4_translate(logic_select<Vector>(bus, Vector{3.3, 0.72, 0.5699999999999998}, Vector{0.04, 0.36, 0.7})).Result;
            seat = engine_math_math::Mat4_mul(m_2, a).Result;
            c.eye = engine_math_math::Mat4_translation(seat).Result;
            look = engine_math_math::Quat_rotate(s.rotation, vector_make(kern::formula([](Float head_yaw_, Float head_pitch_) -> Float { return (kern::math::cos(head_yaw_) * kern::math::cos(head_pitch_)); }, head_yaw_, head_pitch_), kern::formula([](Float head_yaw_, Float head_pitch_) -> Float { return (kern::math::sin(head_yaw_) * kern::math::cos(head_pitch_)); }, head_yaw_, head_pitch_), kern::formula([](Float head_pitch_) -> Float { return kern::math::sin(head_pitch_); }, head_pitch_))).Result;
            c.target = vector_add(c.eye, vector_scale(look, 5.0));
            c.up = engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 1.0}).Result;
            c.fov = kern::formula([](Float kDeg) -> Float { return (80.0 * kDeg); }, kDeg);
            c.near = kern::formula([](Float bus) -> Float { return ((bus != 0.0) ? 0.22 : 0.04); }, kern::to<Float>(bus));
          } else {
            zoom = kern::formula([](Float bus) -> Float { return ((bus != 0.0) ? 1.75 : 1.0); }, kern::to<Float>(bus));
            Game_update_camera_rig(dt, pos_2, s.velocity, vector_make(vector_break(fwd_2).x, vector_break(fwd_2).y, 0.0), 9.0, 9.0, 6.0, 0.6);
            hd_2 = logic_select<Vector>(math_compare<">">(vector_length(vector_make(vector_break(cam_heading_).x, vector_break(cam_heading_).y, 0.0)), 9.999999717180685e-10), vector_normalize(vector_make(vector_break(cam_heading_).x, vector_break(cam_heading_).y, 0.0)), Vector{0.0, 0.0, 1.0});
            a_2 = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, head_yaw_).Result;
            look_dir = engine_math_math::Quat_rotate(a_2, hd_2).Result;
            cz = ctl_.car_zoom;
            tgt = vector_add(vector_add(cam_anchor_, vector_scale(look_dir, 1.05)), Vector{0.0, 0.0, 1.2});
            want_2 = vector_add(vector_sub(tgt, vector_scale(look_dir, kern::formula([](Float zoom, Float cz) -> Float { return ((5.4 * zoom) * cz); }, zoom, cz))), vector_make(0.0, 0.0, kern::formula([](Float zoom, Float cz) -> Float { return ((2.15 * zoom) * (0.55 + (0.45 * cz))); }, zoom, cz)));
            n5242974 = Game_update_camera_collide(tgt, want_2, body(cars_[car_index_].actor).Result);
            c.eye = n5242974.Result;
            c.target = tgt;
            c.up = Vector{0.0, 0.0, 1.0};
            c.fov = kern::formula([](Float the_result, Float kDeg) -> Float { return (kern::math::min(80.0, (70.0 + (kern::math::abs(the_result) * 0.25))) * kDeg); }, car::Car_forward_speed(cars_[car_index_].car[0]).Result, kDeg);
          }
        }
      } break;
      case Mode::jet: {
        n5242975 = matrix_of(jet_actor_);
        m_3 = n5242975.Result;
        n5242976 = state_of(jet_actor_);
        s_2 = n5242976.Result;
        pos_3 = engine_math_math::Mat4_translation(m_3).Result;
        nose = engine_math_math::Quat_rotate(s_2.rotation, Vector{0.0, 1.0, 0.0}).Result;
        up_2 = engine_math_math::Quat_rotate(s_2.rotation, Vector{0.0, 0.0, 1.0}).Result;
        if (jet_cockpit_) {
          a_4 = engine_math_math::Mat4_translate(Vector{0.0, 3.5, 1.42}).Result;
          a_3 = engine_math_math::Mat4_mul(m_3, a_4).Result;
          c.eye = engine_math_math::Mat4_translation(a_3).Result;
          c.target = vector_add(c.eye, vector_scale(nose, 10.0));
          c.up = up_2;
          c.near = 0.5;
        } else {
          sp = vector_length(s_2.velocity);
          dist = kern::formula([](Float sp) -> Float { return (22.0 + kern::math::min(14.0, kern::math::div(sp, 40.0))); }, sp);
          want_3 = vector_add(vector_scale(nose, kern::formula([](Float dist) -> Float { return (-dist); }, dist)), vector_scale(up_2, 6.5));
          if (logic_not(rig_init_)) {
            jet_cam_off_ = want_3;
            rig_init_ = true;
          }
          jet_cam_off_ = vector_add(jet_cam_off_, vector_scale(vector_sub(want_3, jet_cam_off_), kern::formula([](Float dt) -> Float { return (1.0 - kern::math::exp(((-kern::math::min(dt, 0.1)) * 6.0))); }, dt)));
          n5242977 = Game_update_camera_collide(vector_add(pos_3, vector_scale(up_2, 2.0)), vector_add(pos_3, jet_cam_off_), body(jet_actor_).Result);
          c.eye = n5242977.Result;
          c.target = vector_add(pos_3, vector_scale(nose, 30.0));
          c.up = logic_select<Vector>(math_compare<">">(vector_length(vector_add(up_2, vector_scale(vector_sub(Vector{0.0, 0.0, 1.0}, up_2), 0.35))), 9.999999717180685e-10), vector_normalize(vector_add(up_2, vector_scale(vector_sub(Vector{0.0, 0.0, 1.0}, up_2), 0.35))), Vector{0.0, 0.0, 1.0});
          c.near = 1.0;
        }
        c.fov = kern::formula([](Float a, Float kDeg) -> Float { return (a * kDeg); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a) -> Float { return (66.0 + kern::math::div(a, 30.0)); }, vector_length(s_2.velocity)), 66.0, 90.0), kDeg);
        c.far = 6000.0;
      } break;
      case Mode::parachute: {
        n5242978 = matrix_of(chute_actor_);
        a_5 = n5242978.Result;
        pos_4 = engine_math_math::Mat4_translation(a_5).Result;
        n5242979 = Game_update_camera_collide(pos_4, vector_add(pos_4, Vector{0.0, -14.0, 7.0}), body(chute_actor_).Result);
        c.eye = n5242979.Result;
        c.target = vector_add(pos_4, Vector{0.0, 0.0, 2.0});
        c.up = Vector{0.0, 0.0, 1.0};
        c.fov = kern::formula([](Float kDeg) -> Float { return (70.0 * kDeg); }, kDeg);
      } break;
    }
    ctl_camera(c);
    gaps_camera(c);
    cam_ = c;
  }

  struct audio_initResult { Bool Result = false; };
  audio_initResult audio_init(U32 rate = 0) {
    Bool ready = false;
    Bool init = false;
    Object<kern::game::SoundBank> old = Object<kern::game::SoundBank>{};
    Object<kern::game::SoundBank> sound_bank = Object<kern::game::SoundBank>{};
    Object<kern::game::GameAudio> old_2 = Object<kern::game::GameAudio>{};
    Object<kern::game::GameAudio> game_audio = Object<kern::game::GameAudio>{};
    ready = engine_mixer_ready(mixer_);
    if (ready) {
      return {true};
    }
    engine_mixer_init(n5242980, mixer_, rate, 64);
    init = n5242980.result;
    if (logic_not(init)) {
      return {false};
    }
    old = bank_;
    engine_sound_bank_make(n5242981, mixer_);
    sound_bank = n5242981.result;
    bank_ = sound_bank;
    if (object_valid<kern::game::SoundBank>(old)) {
      engine_sound_bank_release(old);
    }
    old_2 = game_audio_;
    engine_game_audio_make(n5242982, mixer_, bank_);
    game_audio = n5242982.result;
    game_audio_ = game_audio;
    if (object_valid<kern::game::GameAudio>(old_2)) {
      engine_game_audio_release(old_2);
    }
    if (object_valid<kern::game::GameWorld>(world_)) {
      engine_sound_bank_request_world(bank_, world_);
    }
    n5242983 = sound_fx_cpp::SoundFx_init(fx_, mixer_);
    ui_audio_init();
    combat_audio_init();
    return {true};
  }

  struct audio_renderResult { List<Float> Result = List<Float>{}; };
  audio_renderResult audio_render(U32 frames = 0) {
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
      engine_mixer_render(n5242984, mixer_, frames);
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(n5242984.out))));
      for (auto it5242985 = flow_for(n5242985, 0, last); it5242985.next();) {
        audio_out_[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(at), kern::to<Float>(n5242985.index)))] = list_get<Float>(n5242984.out, n5242985.index);
      }
      if (web_audio_) {
        fill = 0.0;
        first = 0;
        last_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(audio_out_))));
        for (auto it5242986 = flow_for(n5242986, first, last_2); it5242986.next();) {
          audio_out_[n5242986.index] = fill;
        }
      } else {
        ui_render_music(audio_out_, frames);
      }
    } else {
      fill_2 = 0.0;
      first_2 = 0;
      last_3 = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(audio_out_))));
      for (auto it5242987 = flow_for(n5242987, first_2, last_3); it5242987.next();) {
        audio_out_[n5242987.index] = fill_2;
      }
    }
    mix_voice(audio_out_, frames);
    return {audio_out_};
  }

  void dsync_ready() {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<DestroySync>(dsync_)), 0.0))) {
      dsync_ = list_make<DestroySync, "1">(DestroySync{1, 0.0, 0.0, List<Terrain_CraterRec>{}, List<DestroySync_Mine>{}, List<CityStream_DamageRec>{}, List<Int_Float_pair>{}, List<U8>{}, 0, -99.0, false, false, 0, 0, 0});
    }
  }

  void dsync_send_full(I32 to = 0) {
    List<Terrain_CraterRec> c = List<Terrain_CraterRec>{};
    List<CityStream_DamageRec> dm = List<CityStream_DamageRec>{};
    List<Int_Float_pair> pt = List<Int_Float_pair>{};
    Int parts = 0;
    List<U8> o = List<U8>{};
    Int a = 0;
    Int b = 0;
    dsync_ready();
    c = List<Terrain_CraterRec>{};
    terrain_cpp::the_Terrain_crater_list_(city_.terrain_, c);
    dm = List<CityStream_DamageRec>{};
    city_stream_cpp::the_CityStream_damage_list_(city_, dm);
    pt = List<Int_Float_pair>{};
    destruction::the_Destruction_points_list_(destruction_, pt);
    parts = kern::to<Int>(kern::formula([](Float a, Float a_2, Float kPerMessage) -> Float { return kern::math::max(1.0, kern::math::trunc(kern::math::div(((kern::math::max(a, a_2) + kPerMessage) - 1.0), kPerMessage))); }, kern::to<Float>(list_length<CityStream_DamageRec>(dm)), kern::to<Float>(list_length<Int_Float_pair>(pt)), kern::to<Float>(kPerMessage)));
    for (auto it6291466 = flow_for(n6291466, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(parts)))); it6291466.next();) {
      o = List<U8>{};
      wire::put_u8(o, kState);
      wire::put_u32(o, dsync_[0].epoch);
      wire::put_u8(o, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::band(((index == 0.0) ? 1.0 : 0.0), 255.0); }, kern::to<Float>(n6291466.index))));
      game_destruction_sync_cpp::put_craters(o, c);
      a = kern::to<Int>(kern::formula([](Float a, Float index, Float kPerMessage) -> Float { return kern::math::min(a, (index * kPerMessage)); }, kern::to<Float>(list_length<CityStream_DamageRec>(dm)), kern::to<Float>(n6291466.index), kern::to<Float>(kPerMessage)));
      b = kern::to<Int>(kern::formula([](Float a, Float index, Float kPerMessage) -> Float { return kern::math::min(a, (index * kPerMessage)); }, kern::to<Float>(list_length<Int_Float_pair>(pt)), kern::to<Float>(n6291466.index), kern::to<Float>(kPerMessage)));
      game_destruction_sync_cpp::put_damage(o, dm, a, kern::to<Int>(kern::formula([](Float kPerMessage, Float a, Float a_2) -> Float { return kern::math::min(kPerMessage, (a - a_2)); }, kern::to<Float>(kPerMessage), kern::to<Float>(list_length<CityStream_DamageRec>(dm)), kern::to<Float>(a))));
      game_destruction_sync_cpp::put_points(o, pt, b, kern::to<Int>(kern::formula([](Float kPerMessage, Float a, Float b) -> Float { return kern::math::min(kPerMessage, (a - b)); }, kern::to<Float>(kPerMessage), kern::to<Float>(list_length<Int_Float_pair>(pt)), kern::to<Float>(b))));
      if (math_compare<"<">(kern::to<Float>(to), 0.0)) {
        net_session_cpp::Session_destroy_to_all(net_, o, true);
      } else {
        net_session_cpp::Session_destroy_to(net_, o, kern::to<Int>(kern::formula([](Float to) -> Float { return kern::math::band(to, 255.0); }, kern::to<Float>(to))));
      }
      dsync_[0].sent_states = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(dsync_[0].sent_states)));
    }
    if (math_compare<"<">(kern::to<Float>(to), 0.0)) {
      dsync_[0].sent = c;
    }
  }

  void dsync_reset_epoch() {
    dsync_ready();
    if (net_session::Session_hosting(net_).Result) {
      dsync_[0].epoch = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(dsync_[0].epoch)));
      dsync_send_full(-1);
    }
  }

  struct dsync_update_matchResult { Bool Result = false; };
  KERN_PURE dsync_update_matchResult dsync_update_match(const CityStream_DamageRec& q, U32 seed = 0) {
    return {math_compare<"==">(kern::to<Float>(q.seed), kern::to<Float>(seed))};
  }

  struct dsync_update_match_2Result { Bool Result = false; };
  KERN_PURE dsync_update_match_2Result dsync_update_match_2(const Int_Float_pair& list_2_n6291473_index, const Int_Float_pair& q) {
    return {math_compare<"==">(kern::to<Float>(q.first), kern::to<Float>(list_2_n6291473_index.first))};
  }

  struct dsync_jsonResult { Text Result = ""; };
  dsync_jsonResult dsync_json() {
    List<Terrain_CraterRec> c = List<Terrain_CraterRec>{};
    List<CityStream_DamageRec> dm = List<CityStream_DamageRec>{};
    Text b = "";
    Int arg = 0;
    Int arg_2 = 0;
    Int arg_3 = 0;
    dsync_ready();
    c = List<Terrain_CraterRec>{};
    terrain_cpp::the_Terrain_crater_list_(city_.terrain_, c);
    dm = List<CityStream_DamageRec>{};
    city_stream_cpp::the_CityStream_damage_list_(city_, dm);
    b = "";
    arg = dsync_[0].epoch;
    arg_2 = list_length<Terrain_CraterRec>(c);
    arg_3 = list_length<CityStream_DamageRec>(dm);
    n6291484 = game_destruction_sync_cpp::digest_of(c, dm);
    b = text_format<"{\"epoch\":%u,\"craters\":%zu,\"damaged\":%zu,\"digest\":%u,\"sent\":%u,\"got\":%u,\"asks\":%u,\"pending\":%zu,\"mismatches\":%u}">(255, arg, arg_2, arg_3, n6291484.Result, dsync_[0].sent_states, dsync_[0].got_states, dsync_[0].asks, list_length<DestroySync_Mine>(dsync_[0].mine), dsync_[0].mismatches);
    return {b};
  }

  struct details_command_startsResult { Bool Result = false; };
  KERN_PURE details_command_startsResult details_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct sfx_idResult { Text Result = ""; };
  /// "sc:<kind><variant>" for every sound of the bank (built once).
  sfx_idResult sfx_id(combat_audio_Kind k = combat_audio_Kind::shot, U32 v = 0) {
    if (math_compare<"==">(kern::to<Float>(list_length<Text>(sfx_id_ids)), 0.0)) {
      for (auto it7340047 = flow_for(n7340047, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<combat_audio_Kind>(combat_audio_Kind::count))))); it7340047.next();) {
        for (auto it7340048 = flow_for(n7340048, 0, 3); it7340048.next();) {
          list_add(sfx_id_ids, text_append(text_append("sc:", combat_audio_cpp::kind_name(enum_from_index<combat_audio_Kind>(n7340047.index).value).Result, ""), text_format<"%u">(-1, n7340048.index), ""));
        }
      }
    }
    return {sfx_id_ids[kern::to<Int>(kern::formula([](Float a, Float v) -> Float { return ((a * 4.0) + v); }, kern::to<Float>(enum_index<combat_audio_Kind>(k)), kern::to<Float>(v)))]};
  }

  void combat_audio_init() {
    combat_audio_Kind k = combat_audio_Kind::shot;
    List<Float> pcm = List<Float>{};
    List<Float> list = List<Float>{};
    Float x = 0.0;
    Text id = "";
    Bool set_pcm = false;
    if (logic_or(combat_audio_ready_, logic_not(object_valid<kern::game::SoundBank>(bank_)))) {
      return;
    }
    for (auto it7340049 = flow_for(n7340049, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<combat_audio_Kind>(combat_audio_Kind::count))))); it7340049.next();) {
      k = enum_from_index<combat_audio_Kind>(n7340049.index).value;
      for (auto it7340050 = flow_for(n7340050, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(int64_to_int(int64_of_int(list_get<U32>(kVariants, n7340049.index))))))); it7340050.next();) {
        pcm = List<Float>{};
        n7340051 = combat_audio_cpp::render(k, n7340050.index, kSampleRate);
        list = n7340051.Result;
        for (auto it7340052 = flow_for(n7340052, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(list))))); it7340052.next();) {
          x = list[n7340052.index];
          list_add(pcm, x);
        }
        n7340053 = sfx_id(k, n7340050.index);
        id = n7340053.Result;
        engine_sound_bank_request(bank_, id);
        engine_sound_bank_set_pcm(n7340054, bank_, id, pcm, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(pcm)))), 1, int64_low(int64_of_int(kSampleRate)));
        set_pcm = n7340054.result;
      }
    }
    combat_audio_ready_ = true;
  }

  struct combat_sfxResult { Text Result = ""; };
  combat_sfxResult combat_sfx(combat_audio_Kind k = combat_audio_Kind::shot) {
    U8 i = 0;
    U32 v = 0;
    Int was = 0;
    i = enum_index<combat_audio_Kind>(k);
    was = sfx_cursor_[i];
    sfx_cursor_[i] = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(sfx_cursor_[i])));
    v = int64_low(int64_rem(int64_of_int(was), int64_of_int(list_get<U32>(kVariants, i))));
    n7340055 = sfx_id(k, v);
    return {n7340055.Result};
  }

  void ui_audio_init() {
    Object<kern::audio::Mixer> m = Object<kern::audio::Mixer>{};
    U32 rate = 0;
    Int sample_rate = 0;
    audio_SynthPatch roar = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch rumble = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Object<kern::audio::Mixer> arg = Object<kern::audio::Mixer>{};
    Int adopt_clip = 0;
    audio_SynthPatch thump = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch burst = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch click = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    List<Float> w = List<Float>{};
    audio_SynthPatch item = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch item_2 = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch item_3 = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Fixed<audio_SynthPatch, 3> items = Fixed<audio_SynthPatch, 3>{};
    List<Float> synthesize = List<Float>{};
    Int adopt_clip_2 = 0;
    audio_SynthPatch body_2 = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch low = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    List<Float> w_2 = List<Float>{};
    audio_SynthPatch item_4 = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch item_5 = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Fixed<audio_SynthPatch, 2> items_2 = Fixed<audio_SynthPatch, 2>{};
    List<Float> synthesize_2 = List<Float>{};
    Int adopt_clip_3 = 0;
    audio_SynthPatch hum = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Object<kern::audio::Mixer> arg_2 = Object<kern::audio::Mixer>{};
    Int adopt_clip_4 = 0;
    List<Float> birds = List<Float>{};
    List<Float> made = List<Float>{};
    Rng r = Rng{2654435769, 0.0, false};
    audio_SynthPatch chirp = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    List<Float> synthesize_3 = List<Float>{};
    List<Float> arg_3 = List<Float>{};
    Int adopt_clip_5 = 0;
    audio_SynthPatch cricket = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    audio_SynthPatch pulse = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    Object<kern::audio::Mixer> arg_4 = Object<kern::audio::Mixer>{};
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
    arg = m;
    n8388618 = game_equipment_cpp::render_loop(list_make<audio_SynthPatch, "2">(roar, rumble), rate, 1.5, 0.8);
    engine_mixer_adopt_clip(n8388619, arg, n8388618.Result, 1, "spawncity:jetpack", true);
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
    item = thump;
    item_2 = burst;
    item_3 = click;
    items = list_make<audio_SynthPatch, "3">(item, item_2, item_3);
    for (auto it8388620 = flow_for(n8388620, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<audio_SynthPatch>(items))))); it8388620.next();) {
      synthesize = engine_audio_synthesize(items[n8388620.index], rate);
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
    item_4 = body_2;
    item_5 = low;
    items_2 = list_make<audio_SynthPatch, "2">(item_4, item_5);
    for (auto it8388624 = flow_for(n8388624, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<audio_SynthPatch>(items_2))))); it8388624.next();) {
      synthesize_2 = engine_audio_synthesize(items_2[n8388624.index], rate);
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
    arg_2 = m;
    n8388628 = game_equipment_cpp::render_loop(list_make<audio_SynthPatch, "1">(hum), rate, 6.0, 0.6);
    engine_mixer_adopt_clip(n8388629, arg_2, n8388628.Result, 1, "spawncity:city", true);
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
      arg_3 = birds;
      n8388636 = common::Rng_unit(r);
      engine_audio_mix_into(n8388637, arg_3, synthesize_3, kern::formula([](Float the_result) -> Float { return (5.6 * the_result); }, n8388636.Result), rate, 1.0);
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
    arg_4 = m;
    n8388641 = game_equipment_cpp::render_loop(list_make<audio_SynthPatch, "2">(cricket, pulse), rate, 4.0, 0.35);
    engine_mixer_adopt_clip(n8388642, arg_4, n8388641.Result, 1, "spawncity:crickets", true);
    adopt_clip_6 = n8388642.result;
    ui_.crickets_clip = adopt_clip_6;
  }

  void ui_radio(I32 dir = 0) {
    I32 n = 0;
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
    Bool both = false;
    Float sim_time_2 = 0.0;
    Bool in_car = false;
    Bool next = false;
    Object<kern::Input> input = Object<kern::Input>{};
    Bool pressed = false;
    Text a_2 = "";
    Text a_3 = "";
    Bool either = false;
    Bool prev = false;
    Object<kern::Input> input_2 = Object<kern::Input>{};
    Bool pressed_2 = false;
    Vector v = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity_2 = Vector{0.0, 0.0, 0.0};
    Float dv = 0.0;
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool ready = false;
    Bool ready_2 = false;
    Bool both_2 = false;
    audio_PlayParams p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
    Int64 play = Int64{0x0};
    if (drone_out_) {
      s = FlightSample{0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1.0, 0.0, false};
      sim_time = engine_game_world_sim_time(world_);
      s.t = sim_time;
      n8388643 = matrix_of(drone_actor_);
      a = n8388643.Result;
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
    both = net_session::Session_active(net_).Result;
    if (both) {
      both = logic_not(text_equal(net_session_cpp::Session_status(net_).Result, ui_.net_status));
    }
    if (both) {
      ui_.net_status = net_session_cpp::Session_status(net_).Result;
      sim_time_2 = engine_game_world_sim_time(world_);
      equipment_cpp::FlightLog_network(ui_.the_log, ui_.net_status, sim_time_2);
    }
    in_car = logic_and(enum_equal<Mode>(mode_, Mode::car), logic_not(ui_.paused));
    if (logic_and(logic_and(in_car, object_valid<kern::Engine>(engine_)), logic_not(web_audio_))) {
      engine_engine_input(n8388644, engine_);
      input = n8388644.result;
      pressed = engine_input_pressed(input, Key::n);
      either = logic_or(pressed, game::PadState_hit(pad, enum_index<PadButtonIndex>(PadButtonIndex::kPadRight)).Result);
      if (logic_not(either)) {
        a_3 = "radio";
        n8388645 = touch_play_cpp::TouchPlay_take_pressed(pad_touch_, a_3);
        either = n8388645.Result;
      }
      next = either;
      engine_engine_input(n8388646, engine_);
      input_2 = n8388646.result;
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
          n8388647 = matrix_of(cars_[car_index_].actor);
          a_4 = n8388647.Result;
          osfx("world-physics-impact", engine_math_math::Mat4_translation(a_4).Result, dv, 0.0, 0.0);
        }
        both_2 = math_compare<">">(dv, 1.8);
        if (both_2) {
          ready_2 = engine_mixer_ready(mixer_);
          both_2 = ready_2;
        }
        if (logic_and(both_2, math_compare<"!=">(kern::to<Float>(ui_.thump_clip), 0.0))) {
          p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
          p.volume = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float dv) -> Float { return (0.25 + (dv * 0.06)); }, dv), 0.3, 1.0));
          p.pitch = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float dv) -> Float { return (1.2 - (dv * 0.03)); }, dv), 0.6, 1.2));
          p.bus = audio_Bus::sfx;
          engine_mixer_play(n8388648, mixer_, ui_.thump_clip, p);
          play = n8388648.result;
          ui_.thumps = kern::to<Int>(kern::formula([](Float thumps) -> Float { return (thumps + 1.0); }, kern::to<Float>(ui_.thumps)));
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
    Object<kern::audio::Mixer> m = Object<kern::audio::Mixer>{};
    Bool burn = false;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Object<kern::audio::Mixer> arg = Object<kern::audio::Mixer>{};
    U32 arg_2 = 0;
    Float arg_3 = 0.0;
    audio_Bus arg_4 = audio_Bus::master;
    Bool on = false;
    Float high = 0.0;
    Bool live = false;
    Float vol = 0.0;
    m = mixer_;
    burn = logic_and(ui_.jetpack.thrusting, logic_not(ui_.paused));
    arg = m;
    arg_2 = ui_.jet_clip;
    arg_3 = kern::formula([](Float burn) -> Float { return ((burn != 0.0) ? 0.75 : 0.0); }, kern::to<Float>(burn));
    arg_4 = audio_Bus::sfx;
    n8388649 = matrix_of(soldier_);
    a = n8388649.Result;
    game_equipment_cpp::drive_loop(ui_.jet_voice, arg, arg_2, arg_3, arg_4, true, vector_sub(engine_math_math::Mat4_translation(a).Result, Vector{0.0, 0.0, 0.6}));
    for (auto it8388650 = flow_for(n8388650, 0, 7); it8388650.next();) {
      on = logic_and(logic_and(math_compare<">">(kern::formula([](Float index, Float a) -> Float { return ((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a)); }, kern::to<Float>(n8388650.index), kern::to<Float>(list_length<net_Remote>(net_.remotes_))), 0.5), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n8388650.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetpack)))), 0.5)), logic_not(ui_.paused));
      game_equipment_cpp::drive_loop(ui_.remote_jet_voice[n8388650.index], m, ui_.jet_clip, kern::formula([](Float on) -> Float { return ((on != 0.0) ? 0.7 : 0.0); }, kern::to<Float>(on)), audio_Bus::sfx, true, logic_select<Vector>(on, vector_sub(net_.remotes_[n8388650.index].shown, Vector{0.0, 0.0, 0.6}), Vector{0.0, 0.0, 0.0}));
    }
    high = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float z, Float the_result) -> Float { return (1.0 - kern::math::div((z - the_result), 150.0)); }, vector_break(cam_.eye).z, city_stream_cpp::CityStream_ground_height(city_, vector_break(cam_.eye).x, vector_break(cam_.eye).y).Result), 0.15, 1.0));
    live = logic_and(world_placed_, logic_not(ui_.paused));
    game_equipment_cpp::drive_loop(ui_.city_voice, m, ui_.city_clip, kern::formula([](Float live, Float high, Float a) -> Float { return ((live != 0.0) ? ((0.32 * high) * ((a != 0.0) ? 0.7 : 1.0)) : 0.0); }, kern::to<Float>(live), high, kern::to<Float>(city_stream::CityStream_offline(city_).Result)), audio_Bus::ambience, false, Vector{0.0, 0.0, 0.0});
    game_equipment_cpp::drive_loop(ui_.birds_voice, m, ui_.birds_clip, kern::formula([](Float live, Float high, Float night_) -> Float { return ((live != 0.0) ? ((0.28 * high) * (1.0 - night_)) : 0.0); }, kern::to<Float>(live), high, night_), audio_Bus::ambience, false, Vector{0.0, 0.0, 0.0});
    game_equipment_cpp::drive_loop(ui_.crickets_voice, m, ui_.crickets_clip, kern::formula([](Float live, Float high, Float night_) -> Float { return ((live != 0.0) ? ((0.22 * high) * night_) : 0.0); }, kern::to<Float>(live), high, night_), audio_Bus::ambience, false, Vector{0.0, 0.0, 0.0});
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

  void ui_render_music(List<Float>& out, U32 frames = 0) {
    equipment_cpp::MusicSynth_render_add(ui_.music, out, frames);
  }

  struct ui_equipment_commandResult { Bool Result = false; };
  ui_equipment_commandResult ui_equipment_command(const Text& cmd, Text& out) {
    Text rest = "";
    Fixed<Text, 4> part = Fixed<Text, 4>{};
    Int sp = 0;
    Fixed<U8, 32> sk = Fixed<U8, 32>{};
    Fixed<U8, 32> aux = Fixed<U8, 32>{};
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
      for (auto it8388651 = flow_for(n8388651, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Text>(part))))); it8388651.next();) {
        sp = text_find(rest, " ", 0);
        if (math_compare<"==">(kern::to<Float>(sp), -1.0)) {
          out = "usage: nostr event <created_at> <secret> <aux> <tags> <content>";
          return {true};
        }
        part[n8388651.index] = text_sub(rest, 0, sp);
        rest = text_sub(rest, kern::to<Int>(kern::formula([](Float sp) -> Float { return (sp + 1.0); }, kern::to<Float>(sp))), -1);
      }
      sk = List<U8>{};
      aux = List<U8>{};
      n8388652 = nostr_cpp::from_hex(sk, part[1]);
      either = logic_not(n8388652.Result);
      if (logic_not(either)) {
        n8388654 = nostr_cpp::from_hex(aux, part[2]);
        either = logic_not(n8388654.Result);
      }
      if (logic_or(logic_or(either, math_compare<"==">(kern::to<Float>(text_length(part[3])), 0.0)), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(part[3], 0)))))), 91.0))) {
        out = "bad key or tags";
        return {true};
      }
      n8388655 = nostr_cpp::event_json(sk, aux, out, text_parse_int(part[0], 0, 10).value, 30078, part[3], rest);
      if (logic_not(n8388655.Result)) {
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
      engine_game_world_push_actor(n8388656, world_, soldier_, Vector{0.0, 0.0, 270.0}, true);
      push_actor = n8388656.result;
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

  void Game_update_scores_pop(const Text& t, Float now = 0.0) {
    if (logic_not(math_compare<"==">(kern::to<Float>(text_length(t)), 0.0))) {
      combat_.pop = t;
      combat_.pop_at = now;
    }
  }

  /// (a label of none - an award without a word on the screen - is the empty one)
  void Game_update_scores_award(Float now = 0.0, I32 pts = 0, Text label = "") {
    Text a = "";
    n9437212 = combat_rules_cpp::Chaos_award(combat_.chaos, pts, now);
    if (logic_not(math_compare<"==">(kern::to<Float>(text_length(label)), 0.0))) {
      a = logic_select<Text>(math_compare<">">(kern::to<Float>(combat_.chaos.combo), 1.0), text_append(text_append(label, "  x", ""), text_format<"%d">(-1, combat_.chaos.combo), ""), label);
      Game_update_scores_pop(a, now);
    }
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
    for (auto it9437215 = flow_for(n9437215, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Debris>(combat_.debris))))); it9437215.next();) {
      combat_.debris[n9437215.index].v = vector_make(vector_break(combat_.debris[n9437215.index].v).x, vector_break(combat_.debris[n9437215.index].v).y, kern::formula([](Float z, Float kG, Float dt) -> Float { return (z - (kG * dt)); }, vector_break(combat_.debris[n9437215.index].v).z, kG, dt));
      combat_.debris[n9437215.index].p = vector_add(combat_.debris[n9437215.index].p, vector_scale(combat_.debris[n9437215.index].v, dt));
      a = engine_math_math::Quat_axis_angle(logic_select<Vector>(math_compare<">">(vector_length(combat_.debris[n9437215.index].spin), 9.999999717180685e-10), vector_normalize(combat_.debris[n9437215.index].spin), Vector{0.0, 0.0, 1.0}), kern::formula([](Float a, Float dt) -> Float { return (a * dt); }, vector_length(combat_.debris[n9437215.index].spin), dt)).Result;
      combat_.debris[n9437215.index].q = common::qnormalize(engine_math_math::Quat_mul(combat_.debris[n9437215.index].q, a).Result).Result;
      the_floor = kern::formula([](Float the_result, Float z) -> Float { return ((the_result + 0.06) + (z * 0.5)); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(combat_.debris[n9437215.index].p).x, vector_break(combat_.debris[n9437215.index].p).y).Result, vector_break(combat_.debris[n9437215.index].size).z);
      if (math_compare<"<">(vector_break(combat_.debris[n9437215.index].p).z, the_floor)) {
        combat_.debris[n9437215.index].p = vector_make(vector_break(combat_.debris[n9437215.index].p).x, vector_break(combat_.debris[n9437215.index].p).y, the_floor);
        if (math_compare<"<">(vector_break(combat_.debris[n9437215.index].v).z, 0.0)) {
          combat_.debris[n9437215.index].v = vector_make(vector_break(combat_.debris[n9437215.index].v).x, vector_break(combat_.debris[n9437215.index].v).y, kern::formula([](Float z) -> Float { return ((-z) * 0.3); }, vector_break(combat_.debris[n9437215.index].v).z));
        }
        combat_.debris[n9437215.index].v = vector_make(kern::formula([](Float x) -> Float { return (x * 0.76); }, vector_break(combat_.debris[n9437215.index].v).x), vector_break(combat_.debris[n9437215.index].v).y, vector_break(combat_.debris[n9437215.index].v).z);
        combat_.debris[n9437215.index].v = vector_make(vector_break(combat_.debris[n9437215.index].v).x, kern::formula([](Float y) -> Float { return (y * 0.76); }, vector_break(combat_.debris[n9437215.index].v).y), vector_break(combat_.debris[n9437215.index].v).z);
        combat_.debris[n9437215.index].spin = vector_scale(combat_.debris[n9437215.index].spin, 0.6);
      }
    }
    at = 0;
    kept = at;
    end = list_length<Debris>(combat_.debris);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(update_debris_match(combat_.debris[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n9437216, combat_.debris[at]);
          combat_.debris[kept] = n9437216.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(combat_.debris, kept, Debris{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 6.5});
    if (math_compare<">">(kern::formula([](Float a) -> Float { return ((63.0 < 0.0) != (a < 0.0) ? (a < 0.0) : (63.0 < a)); }, kern::to<Float>(list_length<Debris>(combat_.debris))), 0.5)) {
      first = 0;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 63.0); }, kern::to<Float>(list_length<Debris>(combat_.debris))))))));
      for (auto it9437217 = flow_for(n9437217, first, last); it9437217.next();) {
        list_remove_at(combat_.debris, first);
      }
    }
  }

  struct combat_command_startsResult { Bool Result = false; };
  KERN_PURE combat_command_startsResult combat_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct soft_target_nearResult { Bool Result = false; };
  soft_target_nearResult soft_target_near(Vector p = Vector{0.0, 0.0, 0.0}) {
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
    Object<kern::game::GameWorld> arg = Object<kern::game::GameWorld>{};
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
      bounce_fx_next_ = kern::to<Int>(kern::formula([](Float bounce_fx_next_) -> Float { return (bounce_fx_next_ + 1.0); }, kern::to<Float>(bounce_fx_next_)));
      bounce_fx_[kern::to<Int>(kern::formula([](Float was) -> Float { return kern::math::mod(was, 8.0); }, kern::to<Float>(was)))] = struct_make<Game_BounceFx>(vector_add(point, vector_scale(n_, 0.008)), n_, now, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float impact) -> Float { return kern::math::div(impact, 12.0); }, impact), 0.55, 1.35)));
    }
    arg = world_;
    n10485775 = combat_sfx(combat_audio_Kind::bounce);
    engine_game_world_play_sound(arg, n10485775.Result, point, kern::formula([](Float a) -> Float { return (kern::math::div(a, 0.44) * 0.5); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float impact) -> Float { return (0.16 + (impact * 0.018)); }, impact), 0.16, 0.44)), kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float bounces, Float impact) -> Float { return ((0.92 + (bounces * 0.035)) + (0.02 * kern::math::min(impact, 10.0))); }, kern::to<Float>(p.bounces), impact), 0.88, 1.32)), true, 0);
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
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
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
    both = object_valid<kern::Engine>(engine_);
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
    Int r_at = 0;
    Int was = 0;
    Float l = 0.0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), math_compare<"<">(kern::formula([](Float now, Float smoke_at) -> Float { return (now - smoke_at); }, now, p.smoke_at), 0.025))) {
      return;
    }
    p.smoke_at = now;
    was = gaps_[0].rocket_cursor;
    gaps_[0].rocket_cursor = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(gaps_[0].rocket_cursor)));
    r_at = kern::to<Int>(kern::formula([](Float was, Float a) -> Float { return kern::math::mod(was, a); }, kern::to<Float>(was), kern::to<Float>(list_length<GapsState_RocketPuff>(gaps_[0].rocket_smoke))));
    l = kern::formula([](Float a) -> Float { return kern::math::max(0.001, a); }, vector_length(p.vel));
    gaps_[0].rocket_smoke[r_at].p = vector_add(vector_sub(p.pos, vector_scale(p.vel, kern::formula([](Float l) -> Float { return kern::math::div(0.42, l); }, l))), vector_make(kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.05); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mul32(a, 3.0) + 1.0); }, kern::to<Float>(gaps_[0].rocket_cursor)))).Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.05); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mul32(a, 3.0) + 2.0); }, kern::to<Float>(gaps_[0].rocket_cursor)))).Result), 0.0));
    gaps_[0].rocket_smoke[r_at].born = now;
    gaps_[0].rocket_smoke[r_at].life = kern::formula([](Float the_result) -> Float { return (1.1 + (the_result * 0.6)); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mul32(a, 3.0) + 3.0); }, kern::to<Float>(gaps_[0].rocket_cursor)))).Result);
    gaps_[0].rocket_smoke[r_at].rot = kern::formula([](Float the_result) -> Float { return (the_result * 6.28); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mul32(a, 5.0) + 4.0); }, kern::to<Float>(gaps_[0].rocket_cursor)))).Result);
  }

  struct foot_round_stepResult { Bool Result = false; };
  foot_round_stepResult foot_round_step(Projectile& p, Vector& at, Float dt = 0.0, Float now = 0.0) {
    I32 n = 0;
    Float sdt = 0.0;
    Bool hand = false;
    I32 i = 0;
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
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Vector seg_to = Vector{0.0, 0.0, 0.0};
    Vector next = Vector{0.0, 0.0, 0.0};
    Float speed = 0.0;
    Float ground = 0.0;
    Vector dir_2 = Vector{0.0, 0.0, 0.0};
    Float travel = 0.0;
    the_RayResult_ r_2 = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Object<kern::physics::World> physics_2 = Object<kern::physics::World>{};
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
        n10485783 = soft_target_near(p.pos);
        if (n10485783.Result) {
          at = p.pos;
          return {true};
        }
        for (auto it10485784 = flow_for(n10485784, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it10485784.next();) {
          if (logic_and(pdrones_[n10485784.index].active, math_compare<"<">(vector_length(vector_sub(pdrones_[n10485784.index].pos, p.pos)), kern::formula([](Float trav) -> Float { return (1.2 + trav); }, trav)))) {
            at = pdrones_[n10485784.index].pos;
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
              engine_game_world_physics(n10485785, world_);
              physics_2 = n10485785.result;
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
                n10485789 = soft_target_near(p.pos);
                if (n10485789.Result) {
                  at = p.pos;
                  return {true};
                }
                for (auto it10485790 = flow_for(n10485790, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it10485790.next();) {
                  if (logic_and(pdrones_[n10485790.index].active, math_compare<"<">(vector_length(vector_sub(pdrones_[n10485790.index].pos, p.pos)), kern::formula([](Float travel) -> Float { return (1.2 + travel); }, travel)))) {
                    at = pdrones_[n10485790.index].pos;
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
    for (auto it10485791 = flow_for(n10485791, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it10485791.next();) {
      if (logic_and(logic_and(projectiles_[n10485791.index].alive, projectiles_[n10485791.index].by_player), logic_or(enum_equal<weapons_Id>(projectiles_[n10485791.index].id, weapons_Id::grenade_launcher), enum_equal<weapons_Id>(projectiles_[n10485791.index].id, weapons_Id::rocket)))) {
        projectiles_[n10485791.index].fuse = 0.0;
        projectiles_[n10485791.index].born = kern::formula([](Float now) -> Float { return (now - 1.0); }, now);
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

  struct gaps_sniper_rounds_matchResult { Bool Result = false; };
  KERN_PURE gaps_sniper_rounds_matchResult gaps_sniper_rounds_match(const GapsState_SniperRound& r, Float now = 0.0) {
    return {math_compare<">=">(now, r.due)};
  }

  void gaps_sniper_rounds(const List<game_ShotEvent>& shots, List<game_ShotEvent>& arrived, Float now = 0.0) {
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0))) {
      for (auto it10485792 = flow_for(n10485792, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_ShotEvent>(shots))))); it10485792.next();) {
        if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(shots[n10485792.index].shooter), kern::to<Float>(soldier_)), enum_equal<Mode>(mode_, Mode::foot)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper))) {
          list_add(arrived, shots[n10485792.index]);
        }
      }
      return;
    }
    for (auto it10485793 = flow_for(n10485793, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_ShotEvent>(shots))))); it10485793.next();) {
      if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(shots[n10485793.index].shooter), kern::to<Float>(soldier_)), enum_equal<Mode>(mode_, Mode::foot)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper))) {
        list_add(gaps_[0].sniper_rounds, struct_make<GapsState_SniperRound>(shots[n10485793.index], kern::formula([](Float now, Float a) -> Float { return (now + kern::math::div(a, 850.0)); }, now, vector_length(vector_sub(shots[n10485793.index].to, shots[n10485793.index].from)))));
      }
    }
    for (auto it10485794 = flow_for(n10485794, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_SniperRound>(gaps_[0].sniper_rounds))))); it10485794.next();) {
      if (math_compare<">=">(now, gaps_[0].sniper_rounds[n10485794.index].due)) {
        list_add(arrived, gaps_[0].sniper_rounds[n10485794.index].s);
      }
    }
    at = 0;
    kept = at;
    end = list_length<GapsState_SniperRound>(gaps_[0].sniper_rounds);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(gaps_sniper_rounds_match(gaps_[0].sniper_rounds[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n10485795, gaps_[0].sniper_rounds[at]);
          gaps_[0].sniper_rounds[kept] = n10485795.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(gaps_[0].sniper_rounds, kept, GapsState_SniperRound{game_ShotEvent{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, false, false, Vector{0.0, 0.0, 1.0}, 0}, 0.0});
  }

  void gaps_init_make_part(const MeshData& disc, GapsState_Part& p, Object<kern::Engine> e = Object<kern::Engine>{}, Bool ember = false) {
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Object<kern::Renderer> renderer_2 = Object<kern::Renderer>{};
    Bool upsert = false;
    Object<kern::Renderer> renderer_3 = Object<kern::Renderer>{};
    m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
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
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
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

  void gaps_init(Object<kern::Engine> e = Object<kern::Engine>{}) {
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    MeshData disc = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Object<kern::Renderer> renderer_2 = Object<kern::Renderer>{};
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Object<kern::Renderer> renderer_3 = Object<kern::Renderer>{};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Object<kern::Renderer> renderer_4 = Object<kern::Renderer>{};
    Bool upsert = false;
    Object<kern::Renderer> renderer_5 = Object<kern::Renderer>{};
    Object<kern::Renderer> renderer_6 = Object<kern::Renderer>{};
    Int allocate_id_2 = 0;
    Object<kern::Renderer> renderer_7 = Object<kern::Renderer>{};
    Bool upsert_2 = false;
    Object<kern::Renderer> renderer_8 = Object<kern::Renderer>{};
    gaps_ = list_make<GapsState, "1">(GapsState{gaps_SirenVoice{false, 0.0, 1.0, 0.0}, gaps_SirenVoice{false, 0.0, 1.0, 0.0}, 0, 0, 0, 0, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, 0.0, 0.0, 0.0, List<Float>{-1.0, -1.0, -1.0, -1.0, -1.0}, gaps_SmokeField{List<gaps_SmokeEffect>{}, List<gaps_Puff>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, Object<kern::Renderer>{}, List<Float>{}, false, -1.0, -99.0, -99.0, 0, List<GapsState_Burst>{}, 0, -1.0, 0, 0, 0, false, gaps_VisualPose{false, 0, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, 0.016666666666666666, 0, 0, List<GapsState_JetVoice>{}, 0, List<GapsState_TrailPuff>{}, 0, List<GapsState_MotorVoice>{}, 0, false, GapsState_Part{0, List<Float>{}, List<Float>{}}, 0.0, 0, false, Map<Int64, List<U8>>{}, Map<Int64, Float>{}, Map<Int, GapsState_ZombieDoor>{}, 0, GapsState_Part{0, List<Float>{}, List<Float>{}}, false, false, false, -10.0, 0, 0, -1.0, 0.0, lamps_Field{List<lamps_Lamp>{}, Set<Int64>{}, Map<Int64, Int>{}, Map<Int64, List<I32>>{}}, 4294967295, false, false, -1.0, 0, List<physics_Body>{}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, List<Float>{-99.0, -99.0}, -99.0, -1.0, false, 0.0, 0.0, List<GapsState_RocketPuff>{}, 0, 0, List<GapsState_SniperRound>{}, GapsState_Part{0, List<Float>{}, List<Float>{}}});
    gaps_[0].smoke = gaps_rules::SmokeField_of(kern::to<Int>(kern::formula([](Float mobile_) -> Float { return kern::math::wrap32(((mobile_ != 0.0) ? 48.0 : 78.0)); }, kern::to<Float>(mobile_)))).Result;
    renderer = engine_engine_renderer(e);
    gaps_[0].renderer = renderer;
    n11534346 = game_gaps_cpp::smoke_disc();
    disc = n11534346.Result;
    gaps_init_make_part(disc, gaps_[0].disc, e, false);
    gaps_init_make_part(disc, gaps_[0].ember, e, true);
    renderer_2 = engine_engine_renderer(e);
    gaps_lamps_init(renderer_2);
    m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    m.alpha = AlphaMode::blend;
    m.unlit = true;
    m.double_sided = true;
    m.cast_shadow = false;
    m.color = engine_math_math::Vec4_new(0.851, 0.863, 0.875, 1.0).Result;
    renderer_3 = engine_engine_renderer(e);
    engine_renderer_allocate_id(n11534349, renderer_3);
    allocate_id = n11534349.result;
    gaps_[0].trail_part.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
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
    U32 rate = 0;
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
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), gaps_[0].guided_sounds), logic_not(object_valid<kern::game::SoundBank>(bank_)))) {
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
    U32 rate = 0;
    Int sample_rate = 0;
    List<Float> d = List<Float>{};
    List<Float> made = List<Float>{};
    U32 s = 0;
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
    Int pf_at = 0;
    Int was = 0;
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
        list_add(gaps_[0].motors, struct_make<GapsState_MotorVoice>(guided_[n11534362.index].m.born, -1.0, Int64{0x0}, false));
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
            gaps_[0].motors[voice].v = play;
          }
        }
        if (logic_not(int64_equal(gaps_[0].motors[voice].v, Int64{0x0}))) {
          t = kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, gaps_[0].motors[voice].lit);
          vol = kern::formula([](Float t) -> Float { return ((t < 0.06) ? kern::math::div((0.5 * t), 0.06) : (0.5 * kern::math::pow(kern::math::div(0.28, 0.5), kern::math::min(1.0, kern::math::div((t - 0.06), 0.54))))); }, t);
          engine_mixer_set_volume(mixer_, gaps_[0].motors[voice].v, vol, 0.03);
          engine_mixer_set_position(mixer_, gaps_[0].motors[voice].v, guided_[n11534362.index].m.p, guided_[n11534362.index].m.v);
        }
        sp = vector_length(guided_[n11534362.index].m.v);
        was = gaps_[0].trail_cursor;
        gaps_[0].trail_cursor = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(gaps_[0].trail_cursor)));
        pf_at = kern::to<Int>(kern::formula([](Float was, Float a) -> Float { return kern::math::mod(was, a); }, kern::to<Float>(was), kern::to<Float>(list_length<GapsState_TrailPuff>(gaps_[0].trail))));
        gaps_[0].trail[pf_at].p = vector_add(vector_sub(guided_[n11534362.index].m.p, vector_scale(guided_[n11534362.index].m.v, kern::formula([](Float sp) -> Float { return kern::math::div(0.9, kern::math::max(1.0, sp)); }, sp))), vector_make(kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.2); }, gaps_rules_cpp::unit_hash(gaps_[0].trail_cursor).Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.2); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 7.0); }, kern::to<Float>(gaps_[0].trail_cursor)))).Result), kern::formula([](Float the_result) -> Float { return ((the_result - 0.5) * 0.2); }, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 13.0); }, kern::to<Float>(gaps_[0].trail_cursor)))).Result)));
        gaps_[0].trail[pf_at].born = now;
      }
    }
    for (auto it11534364 = flow_for(n11534364, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_MotorVoice>(gaps_[0].motors))))); it11534364.next();) {
      if (logic_and(logic_not(gaps_[0].motors[n11534364.index].alive), logic_not(int64_equal(gaps_[0].motors[n11534364.index].v, Int64{0x0})))) {
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
    list_resize(gaps_[0].motors, kept, GapsState_MotorVoice{0.0, -1.0, Int64{0x0}, false});
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
      n11534381 = matrix_of(jet_actor_);
      a = n11534381.Result;
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
      n11534382 = matrix_of(jet_actor_);
      a_3 = n11534382.Result;
      n11534383 = gaps_rules_cpp::SmokeField_start(gaps_[0].smoke, a_2, engine_math_math::Mat4_translation(a_3).Result, 2.2, kJetBurnS, now);
      toast_ = "JET BRENNT";
      toast_age_ = 0.0;
    }
    if (math_compare<"<=">(jet_hp_, 0.0)) {
      gaps_[0].jet_burning = true;
      a_4 = "jet";
      n11534384 = gaps_rules_cpp::SmokeField_accelerate(gaps_[0].smoke, a_4, now, now);
      a_5 = "jet";
      gaps_rules_cpp::SmokeField_stop(gaps_[0].smoke, a_5);
    }
  }

  struct gaps_jet_audio_matchResult { Bool Result = false; };
  KERN_PURE gaps_jet_audio_matchResult gaps_jet_audio_match(const GapsState_JetVoice& v) {
    return {logic_not(v.alive)};
  }

  void gaps_jet_audio(Float dt = 0.0) {
    Bool ready = false;
    Bool ready_2 = false;
    Bool either = false;
    U32 rate = 0;
    Int sample_rate = 0;
    Object<kern::audio::Mixer> arg = Object<kern::audio::Mixer>{};
    Int adopt_clip = 0;
    Object<kern::audio::Mixer> arg_2 = Object<kern::audio::Mixer>{};
    Int adopt_clip_2 = 0;
    Float now = 0.0;
    Float sim_time = 0.0;
    Vector L = Vector{0.0, 0.0, 0.0};
    Vector chosen = Vector{0.0, 0.0, 0.0};
    Bool silent = false;
    Int v_at = 0;
    Int at = 0;
    Int count = 0;
    Bool stop_2 = false;
    Float te = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Float d = 0.0;
    Vector dir = Vector{0.0, 0.0, 0.0};
    Float vr = 0.0;
    Float doppler = 0.0;
    Bool started = false;
    Float gain = 0.0;
    Float cutoff = 0.0;
    Float w = 0.0;
    Float rate_k = 0.0;
    U32 clip = 0;
    Float vol = 0.0;
    U32 clip_2 = 0;
    Float vol_2 = 0.0;
    Bool playing = false;
    Bool playing_2 = false;
    Bool either_2 = false;
    audio_PlayParams pp = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
    Int64 play = Int64{0x0};
    Bool playing_3 = false;
    Bool playing_4 = false;
    Bool either_3 = false;
    audio_PlayParams pp_2 = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
    Int64 play_2 = Int64{0x0};
    Int at_2 = 0;
    Int kept = 0;
    Int end = 0;
    either = logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0));
    if (logic_not(either)) {
      ready_2 = engine_mixer_ready(mixer_);
      either = logic_not(ready_2);
    }
    if (logic_or(either, logic_not(object_valid<kern::game::GameWorld>(world_)))) {
      return;
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].jet_roar), 0.0))) {
      sample_rate = engine_mixer_sample_rate(mixer_);
      rate = sample_rate;
      arg = mixer_;
      n11534385 = game_gaps_cpp::jet_roar_pcm(rate, 9000.0);
      engine_mixer_adopt_clip(n11534386, arg, n11534385.Result, 1, "spawncity:jetroar", true);
      adopt_clip = n11534386.result;
      gaps_[0].jet_roar = adopt_clip;
      arg_2 = mixer_;
      n11534387 = game_gaps_cpp::jet_roar_pcm(rate, 900.0);
      engine_mixer_adopt_clip(n11534388, arg_2, n11534387.Result, 1, "spawncity:jetroar-far", true);
      adopt_clip_2 = n11534388.result;
      gaps_[0].jet_roar_dull = adopt_clip_2;
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    for (auto it11534389 = flow_for(n11534389, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_JetVoice>(gaps_[0].jet_voices))))); it11534389.next();) {
      gaps_[0].jet_voices[n11534389.index].alive = false;
    }
    if (enum_equal<Mode>(mode_, Mode::foot)) {
      n11534390 = player_position();
      chosen = n11534390.Result;
    } else {
      chosen = cam_.eye;
    }
    L = chosen;
    silent = logic_or(ui_.paused, logic_not(settings::Settings_on(ui_.settings, Opt::audio_on).Result));
    for (auto it11534391 = flow_for(n11534391, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<FighterJet>(combat_.jets))))); it11534391.next();) {
      v_at = -1;
      count = list_length<GapsState_JetVoice>(gaps_[0].jet_voices);
      at = 0;
      stop_2 = false;
      while (loop(logic_and(logic_not(stop_2), math_compare<"<">(kern::to<Float>(at), kern::to<Float>(count))))) {
        if (logic_and(logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a == a_2); }, gaps_[0].jet_voices[at].t0, combat_.jets[n11534391.index].t0), 0.5), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a == a_2); }, gaps_[0].jet_voices[at].x, combat_.jets[n11534391.index].x), 0.5)), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (a == a_2); }, gaps_[0].jet_voices[at].y0, combat_.jets[n11534391.index].y0), 0.5))) {
          v_at = at;
          stop_2 = true;
        }
        if (logic_not(stop_2)) {
          at += 1;
        }
      }
      if (logic_not(math_compare<">=">(kern::to<Float>(v_at), 0.0))) {
        gaps_[0].jet_voice_count = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(gaps_[0].jet_voice_count)));
        list_add(gaps_[0].jet_voices, struct_make<GapsState_JetVoice>(combat_.jets[n11534391.index].x, combat_.jets[n11534391.index].y0, combat_.jets[n11534391.index].z, combat_.jets[n11534391.index].t0, gaps_rules_cpp::unit_hash(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::mul32(a, 2654435761.0); }, kern::to<Float>(gaps_[0].jet_voice_count)))).Result, Int64{0x0}, Int64{0x0}, false));
        v_at = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_JetVoice>(gaps_[0].jet_voices))));
      }
      gaps_[0].jet_voices[v_at].alive = true;
      te = now;
      p = Vector{0.0, 0.0, 0.0};
      for (auto it11534392 = flow_for(n11534392, 0, 3); it11534392.next();) {
        p = vector_make(combat_.jets[n11534391.index].x, kern::formula([](Float a, Float combat_kJetSpeed, Float te, Float a_2) -> Float { return (a + (combat_kJetSpeed * (te - a_2))); }, combat_.jets[n11534391.index].y0, combat_kJetSpeed, te, combat_.jets[n11534391.index].t0), combat_.jets[n11534391.index].z);
        te = kern::formula([](Float now, Float a, Float combat_kSoundMps) -> Float { return (now - kern::math::div(a, combat_kSoundMps)); }, now, vector_length(vector_sub(p, L)), combat_kSoundMps);
      }
      p = vector_make(combat_.jets[n11534391.index].x, kern::formula([](Float a, Float combat_kJetSpeed, Float te, Float a_2) -> Float { return (a + (combat_kJetSpeed * (te - a_2))); }, combat_.jets[n11534391.index].y0, combat_kJetSpeed, te, combat_.jets[n11534391.index].t0), combat_.jets[n11534391.index].z);
      d = kern::formula([](Float a) -> Float { return kern::math::max(8.0, a); }, vector_length(vector_sub(p, L)));
      dir = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(L, p)), 9.999999717180685e-10), vector_normalize(vector_sub(L, p)), Vector{0.0, 1.0, 0.0});
      vr = kern::formula([](Float combat_kJetSpeed, Float y) -> Float { return (combat_kJetSpeed * y); }, combat_kJetSpeed, vector_break(dir).y);
      doppler = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float combat_kSoundMps, Float vr) -> Float { return kern::math::div(combat_kSoundMps, (combat_kSoundMps - vr)); }, combat_kSoundMps, vr), 0.45, 2.6));
      started = math_compare<">=">(te, kern::formula([](Float a) -> Float { return (a - 1.5); }, combat_.jets[n11534391.index].t0));
      gain = kern::formula([](Float silent, Float started, Float d) -> Float { return (((silent != 0.0) || (!(started != 0.0))) ? 0.0 : ((kern::math::min(1.8, kern::math::pow(kern::math::div(85.0, d), 1.35)) * 0.9) * 0.85)); }, kern::to<Float>(silent), kern::to<Float>(started), d);
      cutoff = kern::formula([](Float d) -> Float { return kern::math::min(9000.0, (900.0 + kern::math::div(60000.0, d))); }, d);
      w = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float cutoff) -> Float { return kern::math::div((cutoff - 900.0), (9000.0 - 900.0)); }, cutoff), 0.0, 1.0));
      rate_k = kern::formula([](Float doppler, Float a) -> Float { return (doppler * (0.92 + (a * 0.16))); }, doppler, gaps_[0].jet_voices[v_at].pitch);
      clip = gaps_[0].jet_roar;
      vol = kern::formula([](Float gain, Float w) -> Float { return (gain * w); }, gain, w);
      clip_2 = gaps_[0].jet_roar_dull;
      vol_2 = kern::formula([](Float gain, Float w) -> Float { return (gain * (1.0 - w)); }, gain, w);
      either_2 = logic_not(logic_not(int64_equal(gaps_[0].jet_voices[v_at].bright, Int64{0x0})));
      if (logic_not(either_2)) {
        playing_2 = engine_mixer_playing(mixer_, gaps_[0].jet_voices[v_at].bright);
        either_2 = logic_not(playing_2);
      }
      if (either_2) {
        pp = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
        pp.the_loop = true;
        pp.volume = vol;
        pp.pitch = rate_k;
        pp.bus = audio_Bus::sfx;
        pp.start = kern::formula([](Float a) -> Float { return (a * 3.0); }, gaps_[0].jet_voices[v_at].pitch);
        engine_mixer_play(n11534393, mixer_, clip, pp);
        play = n11534393.result;
        gaps_[0].jet_voices[v_at].bright = play;
      } else {
        engine_mixer_set_volume(mixer_, gaps_[0].jet_voices[v_at].bright, vol, 0.05);
        engine_mixer_set_pitch(mixer_, gaps_[0].jet_voices[v_at].bright, kern::formula([](Float rate_k) -> Float { return (rate_k < 0.05 ? 0.05 : (8.0 < rate_k ? 8.0 : rate_k)); }, rate_k));
      }
      either_3 = logic_not(logic_not(int64_equal(gaps_[0].jet_voices[v_at].dull, Int64{0x0})));
      if (logic_not(either_3)) {
        playing_4 = engine_mixer_playing(mixer_, gaps_[0].jet_voices[v_at].dull);
        either_3 = logic_not(playing_4);
      }
      if (either_3) {
        pp_2 = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
        pp_2.the_loop = true;
        pp_2.volume = vol_2;
        pp_2.pitch = rate_k;
        pp_2.bus = audio_Bus::sfx;
        pp_2.start = kern::formula([](Float a) -> Float { return (a * 3.0); }, gaps_[0].jet_voices[v_at].pitch);
        engine_mixer_play(n11534394, mixer_, clip_2, pp_2);
        play_2 = n11534394.result;
        gaps_[0].jet_voices[v_at].dull = play_2;
      } else {
        engine_mixer_set_volume(mixer_, gaps_[0].jet_voices[v_at].dull, vol_2, 0.05);
        engine_mixer_set_pitch(mixer_, gaps_[0].jet_voices[v_at].dull, kern::formula([](Float rate_k) -> Float { return (rate_k < 0.05 ? 0.05 : (8.0 < rate_k ? 8.0 : rate_k)); }, rate_k));
      }
    }
    for (auto it11534395 = flow_for(n11534395, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_JetVoice>(gaps_[0].jet_voices))))); it11534395.next();) {
      if (logic_not(gaps_[0].jet_voices[n11534395.index].alive)) {
        if (logic_not(int64_equal(gaps_[0].jet_voices[n11534395.index].bright, Int64{0x0}))) {
          engine_mixer_stop(mixer_, gaps_[0].jet_voices[n11534395.index].bright, 0.3);
        }
        if (logic_not(int64_equal(gaps_[0].jet_voices[n11534395.index].dull, Int64{0x0}))) {
          engine_mixer_stop(mixer_, gaps_[0].jet_voices[n11534395.index].dull, 0.3);
        }
      }
    }
    at_2 = 0;
    kept = at_2;
    end = list_length<GapsState_JetVoice>(gaps_[0].jet_voices);
    while (loop(math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end)))) {
      if (logic_not(gaps_jet_audio_match(gaps_[0].jet_voices[at_2]).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at_2))) {
          var_take(n11534396, gaps_[0].jet_voices[at_2]);
          gaps_[0].jet_voices[kept] = n11534396.value;
        }
        kept += 1;
      }
      at_2 += 1;
    }
    list_resize(gaps_[0].jet_voices, kept, GapsState_JetVoice{0.0, 0.0, 0.0, 0.0, 0.0, Int64{0x0}, Int64{0x0}, false});
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
      n11534398 = gaps_ride_camera(c);
      both = n11534398.Result;
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
      n11534399 = matrix_of(cars_[car_index_].actor);
      a_2 = n11534399.Result;
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
    Vector arg = Vector{0.0, 0.0, 0.0};
    n11534400 = matrix_of(drone_actor_);
    raw = n11534400.Result;
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(enum_equal<Mode>(mode_, Mode::drone))), math_compare<"==">(kern::to<Float>(drone_cam_), 0.0))) {
      if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
        gaps_rules::VisualPose_invalidate(gaps_[0].drone_visual);
      }
      return {raw};
    }
    p = Vector{0.0, 0.0, 0.0};
    q = Quat{0.0, 0.0, 0.0, 1.0};
    arg = engine_math_math::Mat4_translation(raw).Result;
    n11534401 = state_of(drone_actor_);
    gaps_rules_cpp::VisualPose_update(gaps_[0].drone_visual, p, q, arg, struct_break<BodyState>(n11534401.Result).rotation, cam_anchor_, kern::to<Int>(kern::formula([](Float drone_cam_) -> Float { return ((drone_cam_ == 2.0) ? 2.0 : 1.0); }, kern::to<Float>(drone_cam_))), gaps_[0].frame_dt);
    return {engine_math_math::Mat4_trs(p, q, Vector{1.0, 1.0, 1.0}).Result};
  }

  void gaps_draw() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Float sim_time_2 = 0.0;
    Float chosen = 0.0;
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector right = Vector{0.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int place = 0;
    Int place_2 = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    Mat4 m_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int place_3 = 0;
    Int place_4 = 0;
    Float item_5 = 0.0;
    Float item_6 = 0.0;
    Float item_7 = 0.0;
    Float item_8 = 0.0;
    Bool skip_2 = false;
    Mat4 m_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int place_5 = 0;
    Int place_6 = 0;
    Float item_9 = 0.0;
    Float item_10 = 0.0;
    Float item_11 = 0.0;
    Float item_12 = 0.0;
    Bool skip_3 = false;
    Float t = 0.0;
    Mat4 m_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int place_7 = 0;
    Int place_8 = 0;
    Float item_13 = 0.0;
    Float item_14 = 0.0;
    Float item_15 = 0.0;
    Float item_16 = 0.0;
    Bool skip_4 = false;
    Bool set_visible = false;
    Bool update_instances = false;
    Bool set_visible_2 = false;
    Bool set_visible_3 = false;
    Bool update_instances_2 = false;
    Bool set_visible_4 = false;
    Bool set_visible_5 = false;
    Bool update_instances_3 = false;
    Bool set_visible_6 = false;
    Bool set_visible_7 = false;
    Bool update_instances_4 = false;
    Bool set_visible_8 = false;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(object_valid<kern::Renderer>(gaps_[0].renderer)))) {
      return;
    }
    gaps_barricades_draw();
    if (object_valid<kern::game::GameWorld>(world_)) {
      sim_time_2 = engine_game_world_sim_time(world_);
      chosen = sim_time_2;
    } else {
      chosen = 0.0;
    }
    now = kern::formula([](Float chosen) -> Float { return chosen; }, chosen);
    f = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{1.0, 0.0, 0.0});
    right = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(f, Vector{0.0, 0.0, 1.0})), 9.999999717180685e-10), vector_normalize(vector_cross(f, Vector{0.0, 0.0, 1.0})), Vector{1.0, 0.0, 0.0});
    up = vector_cross(right, f);
    list_clear(gaps_[0].disc.transforms);
    list_clear(gaps_[0].disc.tints);
    list_clear(gaps_[0].ember.transforms);
    list_clear(gaps_[0].ember.tints);
    for (auto it11534402 = flow_for(n11534402, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_Puff>(gaps_[0].smoke.puffs_))))); it11534402.next();) {
      skip = false;
      if (logic_not(gaps_[0].smoke.puffs_[n11534402.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (gaps_[0].smoke.puffs_[n11534402.index].ember) {
          m = game_gaps_cpp::billboard(gaps_[0].smoke.puffs_[n11534402.index].pos, right, up, vector_scale(f, -1.0), kern::formula([](Float the_result) -> Float { return (the_result * 0.5); }, gaps_rules_cpp::SmokeField_size(gaps_[0].smoke.puffs_[n11534402.index], now).Result), gaps_[0].smoke.puffs_[n11534402.index].rotation).Result;
          place = list_length<Float>(gaps_[0].ember.transforms);
          list_insert_list(gaps_[0].ember.transforms, place, m.m);
          place_2 = list_length<Float>(gaps_[0].ember.tints);
          item = vector_break(gaps_[0].smoke.puffs_[n11534402.index].color).x;
          item_2 = vector_break(gaps_[0].smoke.puffs_[n11534402.index].color).y;
          item_3 = vector_break(gaps_[0].smoke.puffs_[n11534402.index].color).z;
          item_4 = gaps_[0].smoke.puffs_[n11534402.index].opacity;
          list_insert_list(gaps_[0].ember.tints, place_2, list_make<Float, "4">(item, item_2, item_3, item_4));
        } else {
          m_2 = game_gaps_cpp::billboard(gaps_[0].smoke.puffs_[n11534402.index].pos, right, up, vector_scale(f, -1.0), kern::formula([](Float the_result) -> Float { return (the_result * 0.5); }, gaps_rules_cpp::SmokeField_size(gaps_[0].smoke.puffs_[n11534402.index], now).Result), gaps_[0].smoke.puffs_[n11534402.index].rotation).Result;
          place_3 = list_length<Float>(gaps_[0].disc.transforms);
          list_insert_list(gaps_[0].disc.transforms, place_3, m_2.m);
          place_4 = list_length<Float>(gaps_[0].disc.tints);
          item_5 = vector_break(gaps_[0].smoke.puffs_[n11534402.index].color).x;
          item_6 = vector_break(gaps_[0].smoke.puffs_[n11534402.index].color).y;
          item_7 = vector_break(gaps_[0].smoke.puffs_[n11534402.index].color).z;
          item_8 = gaps_[0].smoke.puffs_[n11534402.index].opacity;
          list_insert_list(gaps_[0].disc.tints, place_4, list_make<Float, "4">(item_5, item_6, item_7, item_8));
        }
      }
    }
    list_clear(gaps_[0].trail_part.transforms);
    list_clear(gaps_[0].trail_part.tints);
    for (auto it11534403 = flow_for(n11534403, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_TrailPuff>(gaps_[0].trail))))); it11534403.next();) {
      skip_2 = false;
      if (math_compare<"<">(gaps_[0].trail[n11534403.index].born, 0.0)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        m_3 = game_gaps_cpp::billboard(gaps_[0].trail[n11534403.index].p, right, up, vector_scale(f, -1.0), 0.8, 0.0).Result;
        place_5 = list_length<Float>(gaps_[0].trail_part.transforms);
        list_insert_list(gaps_[0].trail_part.transforms, place_5, m_3.m);
        place_6 = list_length<Float>(gaps_[0].trail_part.tints);
        item_9 = 1.0;
        item_10 = 1.0;
        item_11 = 1.0;
        item_12 = 0.42;
        list_insert_list(gaps_[0].trail_part.tints, place_6, list_make<Float, "4">(item_9, item_10, item_11, item_12));
      }
    }
    list_clear(gaps_[0].rocket_part.transforms);
    list_clear(gaps_[0].rocket_part.tints);
    for (auto it11534404 = flow_for(n11534404, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_RocketPuff>(gaps_[0].rocket_smoke))))); it11534404.next();) {
      skip_3 = false;
      if (math_compare<"<">(gaps_[0].rocket_smoke[n11534404.index].born, 0.0)) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float a, Float a_2) -> Float { return kern::math::div((now - a), a_2); }, now, gaps_[0].rocket_smoke[n11534404.index].born, gaps_[0].rocket_smoke[n11534404.index].life), 0.0, 1.0));
        m_4 = game_gaps_cpp::billboard(gaps_[0].rocket_smoke[n11534404.index].p, right, up, vector_scale(f, -1.0), kern::formula([](Float t) -> Float { return ((0.18 + (1.1 * kern::math::sqrt(t))) * 0.5); }, t), gaps_[0].rocket_smoke[n11534404.index].rot).Result;
        place_7 = list_length<Float>(gaps_[0].rocket_part.transforms);
        list_insert_list(gaps_[0].rocket_part.transforms, place_7, m_4.m);
        place_8 = list_length<Float>(gaps_[0].rocket_part.tints);
        item_13 = 1.0;
        item_14 = 1.0;
        item_15 = 1.0;
        item_16 = kern::formula([](Float t) -> Float { return (0.7 * kern::math::pow((1.0 - t), 1.6)); }, t);
        list_insert_list(gaps_[0].rocket_part.tints, place_8, list_make<Float, "4">(item_13, item_14, item_15, item_16));
      }
    }
    skip_4 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].disc.id), 0.0))) {
      skip_4 = true;
    }
    if (logic_not(skip_4)) {
      if (math_compare<"==">(kern::to<Float>(list_length<Float>(gaps_[0].disc.transforms)), 0.0)) {
        engine_renderer_set_visible(n11534405, gaps_[0].renderer, gaps_[0].disc.id, false);
        set_visible = n11534405.result;
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        engine_renderer_update_instances(n11534406, gaps_[0].renderer, gaps_[0].disc.id, gaps_[0].disc.transforms, gaps_[0].disc.tints);
        update_instances = n11534406.result;
        engine_renderer_set_visible(n11534407, gaps_[0].renderer, gaps_[0].disc.id, true);
        set_visible_2 = n11534407.result;
      }
    }
    skip_4 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].ember.id), 0.0))) {
      skip_4 = true;
    }
    if (logic_not(skip_4)) {
      if (math_compare<"==">(kern::to<Float>(list_length<Float>(gaps_[0].ember.transforms)), 0.0)) {
        engine_renderer_set_visible(n11534408, gaps_[0].renderer, gaps_[0].ember.id, false);
        set_visible_3 = n11534408.result;
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        engine_renderer_update_instances(n11534409, gaps_[0].renderer, gaps_[0].ember.id, gaps_[0].ember.transforms, gaps_[0].ember.tints);
        update_instances_2 = n11534409.result;
        engine_renderer_set_visible(n11534410, gaps_[0].renderer, gaps_[0].ember.id, true);
        set_visible_4 = n11534410.result;
      }
    }
    skip_4 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].trail_part.id), 0.0))) {
      skip_4 = true;
    }
    if (logic_not(skip_4)) {
      if (math_compare<"==">(kern::to<Float>(list_length<Float>(gaps_[0].trail_part.transforms)), 0.0)) {
        engine_renderer_set_visible(n11534411, gaps_[0].renderer, gaps_[0].trail_part.id, false);
        set_visible_5 = n11534411.result;
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        engine_renderer_update_instances(n11534412, gaps_[0].renderer, gaps_[0].trail_part.id, gaps_[0].trail_part.transforms, gaps_[0].trail_part.tints);
        update_instances_3 = n11534412.result;
        engine_renderer_set_visible(n11534413, gaps_[0].renderer, gaps_[0].trail_part.id, true);
        set_visible_6 = n11534413.result;
      }
    }
    skip_4 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].rocket_part.id), 0.0))) {
      skip_4 = true;
    }
    if (logic_not(skip_4)) {
      if (math_compare<"==">(kern::to<Float>(list_length<Float>(gaps_[0].rocket_part.transforms)), 0.0)) {
        engine_renderer_set_visible(n11534414, gaps_[0].renderer, gaps_[0].rocket_part.id, false);
        set_visible_7 = n11534414.result;
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        engine_renderer_update_instances(n11534415, gaps_[0].renderer, gaps_[0].rocket_part.id, gaps_[0].rocket_part.transforms, gaps_[0].rocket_part.tints);
        update_instances_4 = n11534415.result;
        engine_renderer_set_visible(n11534416, gaps_[0].renderer, gaps_[0].rocket_part.id, true);
        set_visible_8 = n11534416.result;
      }
    }
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
        n12582922 = matrix_of(cars_[t.index].actor);
        a = n12582922.Result;
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
        for (auto it12582923 = flow_for(n12582923, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it12582923.next();) {
          if (math_compare<"==">(kern::to<Float>(net_.remotes_[n12582923.index].state.slot), kern::to<Float>(t.slot))) {
            at = vector_add(net_.remotes_[n12582923.index].shown, vector_make(0.0, 0.0, kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.1 : 0.0); }, kern::to<Float>(enum_equal<net_PlayerMode>(net_.remotes_[n12582923.index].state.mode, net_PlayerMode::foot)))));
            return {true};
          }
        }
        return {false};
      } break;
    }
  }

  void draw_guided() {
    for (auto it12582929 = flow_for(n12582929, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))); it12582929.next();) {
      people_art_cpp::PeopleArt_projectile(people_art_, guided_[n12582929.index].m.p, guided_[n12582929.index].m.v, 0.06, 0.9, guided_[n12582929.index].m.motor);
    }
  }

  void draw_guided_mark() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Vector at = Vector{0.0, 0.0, 0.0};
    Bool both = false;
    Float pulse = 0.0;
    Float d = 0.0;
    Float r = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector ax = Vector{0.0, 0.0, 0.0};
    Float al = 0.0;
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float g = 0.0;
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    if (logic_or(logic_or(logic_not(guided_marked_), math_compare<">=">(kern::formula([](Float now, Float guided_mark_time_) -> Float { return (now - guided_mark_time_); }, now, guided_mark_time_), kGuidedMarkSeconds)), logic_not(drone_out_))) {
      return;
    }
    at = guided_mark_at_;
    both = logic_not(enum_equal<guided_TargetKind>(guided_mark_.kind, guided_TargetKind::point));
    if (both) {
      n12582930 = guided_target_at(guided_mark_, at);
      both = logic_not(n12582930.Result);
    }
    if (both) {
      return;
    }
    pulse = kern::formula([](Float now) -> Float { return (0.85 + (0.15 * kern::math::sin((now * 12.0)))); }, now);
    d = kern::formula([](Float a) -> Float { return kern::math::max(1.0, a); }, vector_length(vector_sub(at, cam_.eye)));
    r = kern::formula([](Float d, Float pulse) -> Float { return (kern::math::max(1.2, (d * 0.025)) * pulse); }, d, pulse);
    n = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.eye, at)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.eye, at)), Vector{0.0, 0.0, 1.0});
    ax = vector_cross(Vector{0.0, 0.0, 1.0}, n);
    al = vector_length(ax);
    q = logic_select<Quat>(math_compare<">">(al, 0.0001), engine_math_math::Quat_axis_angle(vector_scale(ax, kern::formula([](Float al) -> Float { return kern::math::div(1.0, al); }, al)), kern::formula([](Float z) -> Float { return kern::math::acos((z < (-1.0) ? (-1.0) : (1.0 < z ? 1.0 : z))); }, vector_break(n).z)).Result, struct_make<Quat>(0.0, 0.0, 0.0, 1.0));
    a_3 = engine_math_math::Mat4_translate(at).Result;
    a_4 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, q, Vector{1.0, 1.0, 1.0}).Result;
    a_2 = engine_math_math::Mat4_mul(a_3, a_4).Result;
    a_5 = engine_math_math::Mat4_scale(vector_make(r, r, 0.05)).Result;
    a = engine_math_math::Mat4_mul(a_2, a_5).Result;
    combat_art_cpp::CombatArt_put_2(combat_art_, a, CombatArt_Shape::fade_ring, vector_scale(Vector{1.0, 0.12, 0.08}, 3.0), 0.85);
    g = kern::formula([](Float the_result) -> Float { return (the_result + 0.08); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(at).x, vector_break(at).y).Result);
    a_7 = engine_math_math::Mat4_translate(vector_make(vector_break(at).x, vector_break(at).y, g)).Result;
    a_8 = engine_math_math::Mat4_scale(vector_make(kern::formula([](Float r) -> Float { return (r * 1.4); }, r), kern::formula([](Float r) -> Float { return (r * 1.4); }, r), 0.05)).Result;
    a_6 = engine_math_math::Mat4_mul(a_7, a_8).Result;
    combat_art_cpp::CombatArt_put_2(combat_art_, a_6, CombatArt_Shape::fade_ring, vector_scale(Vector{1.0, 0.12, 0.08}, 2.5), 0.6);
  }

  void gaps_lamps_init_make(GapsState_Part& p, const MeshData& mesh, Object<kern::Renderer> r = Object<kern::Renderer>{}, Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool upsert = false;
    r_ = r;
    engine_renderer_allocate_id(n13631498, r_);
    allocate_id = n13631498.result;
    p.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
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

  void gaps_lamps_init(Object<kern::Renderer> r = Object<kern::Renderer>{}) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Vector steel = Vector{0.0, 0.0, 0.0};
    Vector head = Vector{0.0, 0.0, 0.0};
    MeshData pole = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material pm = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Material lens = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material pool = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    r_ = r;
    steel = game_lamps_cpp::rgb(2829873).Result;
    head = game_lamps_cpp::rgb(2763304).Result;
    pole = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    cylinder = engine_primitives_cylinder(0.09, kPoleH, 7, steel);
    engine_primitives_append(n13631500, pole, cylinder, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    pole = n13631500.dst;
    box = engine_primitives_box(Vector{1.5, 0.09, 0.09}, steel);
    engine_primitives_append(n13631501, pole, box, engine_math_math::Mat4_translate(vector_make(0.72, 0.0, kern::formula([](Float kPoleH) -> Float { return (kPoleH - 0.1); }, kPoleH))).Result);
    pole = n13631501.dst;
    box_2 = engine_primitives_box(Vector{0.6, 0.26, 0.14}, head);
    engine_primitives_append(n13631502, pole, box_2, engine_math_math::Mat4_translate(vector_make(kArm, 0.0, kHeadZ)).Result);
    pole = n13631502.dst;
    pm = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    pm.roughness = 0.55;
    pm.metalness = 0.6;
    gaps_lamps_init_make(gaps_[0].lamp_pole, pole, r_, pm);
    gaps_lamps_init_make(gaps_[0].lamp_fallen, pole, r_, pm);
    lens = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    lens.unlit = true;
    lens.cast_shadow = false;
    lens.emissive = vector_scale(game_lamps_cpp::rgb(16757594).Result, 3.2);
    box_3 = engine_primitives_box(Vector{0.6, 0.26, 0.02}, Vector{1.0, 1.0, 1.0});
    a = box_3;
    gaps_lamps_init_make(gaps_[0].lamp_glow, a, r_, lens);
    pool = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
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
    U32 rate = 0;
    List<Float> glass = List<Float>{};
    List<Float> clang = List<Float>{};
    Bool set_pcm = false;
    Bool set_pcm_2 = false;
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), gaps_[0].lamp_sounds), logic_not(object_valid<kern::game::SoundBank>(bank_)))) {
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
    for (auto it13631508 = flow_for(n13631508, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(gaps_[0].lamps.lamps_))))); it13631508.next();) {
      list_add(lamps_, struct_make<Game_Lamp>(gaps_[0].lamps.lamps_[n13631508.index].foot, gaps_[0].lamps.lamps_[n13631508.index].yaw, gaps_[0].lamps.lamps_[n13631508.index].lane, gaps_[0].lamps.lamps_[n13631508.index].state));
    }
    gaps_[0].lamps_dirty = false;
  }

  void gaps_lamps_place() {
    List<Int64> tiles = List<Int64>{};
    List<lamps_Field_Spot> spots = List<lamps_Field_Spot>{};
    Int64 tile = Int64{0x0};
    List<MapFeatures_Spot> list = List<MapFeatures_Spot>{};
    if (math_compare<"!=">(kern::to<Float>(gaps_[0].lamp_serial), kern::to<Float>(city_stream::CityStream_furniture_serial(city_).Result))) {
      gaps_[0].lamp_serial = city_stream::CityStream_furniture_serial(city_).Result;
      tiles = List<Int64>{};
      city_stream_cpp::CityStream_furniture(city_, tiles);
      spots = List<lamps_Field_Spot>{};
      for (auto it13631505 = flow_for(n13631505, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int64>(tiles))))); it13631505.next();) {
        tile = tiles[n13631505.index];
        list = struct_break<MapFeatures>(city_stream_cpp::CityStream_features(city_, tile).Result).lamps;
        for (auto it13631506 = flow_for(n13631506, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Spot>(list))))); it13631506.next();) {
          if (logic_not(terrain_cpp::Terrain_water_at(city_.terrain_, vector_break(list[n13631506.index].p).x, vector_break(list[n13631506.index].p).y).Result)) {
            list_add(spots, struct_make<lamps_Field_Spot>(vector_break(list[n13631506.index].p).x, vector_break(list[n13631506.index].p).y, list[n13631506.index].a, city_stream_cpp::CityStream_ground_height(city_, vector_break(list[n13631506.index].p).x, vector_break(list[n13631506.index].p).y).Result));
          }
        }
      }
      lamps_cpp::Field_rebuild(gaps_[0].lamps, spots);
      gaps_[0].lamps_dirty = true;
    }
    for (auto it13631507 = flow_for(n13631507, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vec4>(ground_changes_))))); it13631507.next();) {
      lamps_cpp::Field_reground(gaps_[0].lamps, city_, ground_changes_[n13631507.index].x, ground_changes_[n13631507.index].y, ground_changes_[n13631507.index].z, ground_changes_[n13631507.index].w);
      gaps_[0].lamps_dirty = true;
    }
    if (gaps_[0].lamps_dirty) {
      gaps_lamps_mirror();
    }
  }

  void gaps_lamps_reset() {
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Bool body_valid = false;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0))) {
      return;
    }
    engine_game_world_physics(n13631519, world_);
    physics = n13631519.result;
    w = physics;
    for (auto it13631520 = flow_for(n13631520, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<physics_Body>(gaps_[0].fallen))))); it13631520.next();) {
      body_valid = engine_world_body_valid(w, gaps_[0].fallen[n13631520.index]);
      if (body_valid) {
        engine_world_destroy_body(w, gaps_[0].fallen[n13631520.index]);
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
    if (logic_or(logic_not(math_compare<"!=">(kern::to<Float>(p.id), 0.0)), logic_not(object_valid<kern::Renderer>(gaps_[0].renderer)))) {
      return;
    }
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(p.transforms)), 0.0)) {
      engine_renderer_set_visible(n13631525, gaps_[0].renderer, p.id, false);
      set_visible = n13631525.result;
      return;
    }
    engine_renderer_update_instances(n13631526, gaps_[0].renderer, p.id, p.transforms, p.tints);
    update_instances = n13631526.result;
    engine_renderer_set_visible(n13631527, gaps_[0].renderer, p.id, true);
    set_visible_2 = n13631527.result;
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
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
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
      for (auto it13631521 = flow_for(n13631521, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(gaps_[0].lamps.lamps_))))); it13631521.next();) {
        skip = false;
        if (math_compare<"==">(kern::to<Float>(gaps_[0].lamps.lamps_[n13631521.index].state), 2.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_or(math_compare<">">(kern::formula([](Float x, Float x_2) -> Float { return kern::math::abs((x - x_2)); }, vector_break(gaps_[0].lamps.lamps_[n13631521.index].foot).x, vector_break(cam_.eye).x), kern::formula([](Float lamp_m, Float the_result) -> Float { return (lamp_m * kern::math::min(the_result, 3.0)); }, look_.lamp_m, view_zoom().Result)), math_compare<">">(kern::formula([](Float y, Float y_2) -> Float { return kern::math::abs((y - y_2)); }, vector_break(gaps_[0].lamps.lamps_[n13631521.index].foot).y, vector_break(cam_.eye).y), kern::formula([](Float lamp_m, Float the_result) -> Float { return (lamp_m * kern::math::min(the_result, 3.0)); }, look_.lamp_m, view_zoom().Result)))) {
            skip = true;
          }
          if (logic_not(skip)) {
            m = game_lamps_cpp::yaw_at(gaps_[0].lamps.lamps_[n13631521.index].foot, gaps_[0].lamps.lamps_[n13631521.index].yaw).Result;
            gaps_lamps_draw_put(gaps_[0].lamp_pole, m, engine_math_math::Vec4_new(1.0, 1.0, 1.0, 1.0).Result);
            if (logic_and(math_compare<"==">(kern::to<Float>(gaps_[0].lamps.lamps_[n13631521.index].state), 0.0), math_compare<">">(glow, 0.01))) {
              a_2 = engine_math_math::Mat4_translate(vector_make(kArm, 0.0, kern::formula([](Float kHeadZ) -> Float { return (kHeadZ - 0.075); }, kHeadZ))).Result;
              a = engine_math_math::Mat4_mul(m, a_2).Result;
              gaps_lamps_draw_put(gaps_[0].lamp_glow, a, engine_math_math::Vec4_new(glow, glow, glow, 1.0).Result);
              c = vector_make(vector_break(gaps_[0].lamps.lamps_[n13631521.index].head).x, vector_break(gaps_[0].lamps.lamps_[n13631521.index].head).y, kern::formula([](Float the_result) -> Float { return (the_result + 0.14); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(gaps_[0].lamps.lamps_[n13631521.index].head).x, vector_break(gaps_[0].lamps.lamps_[n13631521.index].head).y).Result));
              for (auto it13631522 = flow_for(n13631522, 0, 3); it13631522.next();) {
                rr = kern::formula([](Float index) -> Float { return (13.0 * (1.0 - (index * 0.22))); }, kern::to<Float>(n13631522.index));
                a_4 = engine_math_math::Mat4_translate(vector_add(c, vector_make(0.0, 0.0, kern::formula([](Float index) -> Float { return (index * 0.004); }, kern::to<Float>(n13631522.index))))).Result;
                a_5 = engine_math_math::Mat4_scale(vector_make(rr, rr, 1.0)).Result;
                a_3 = engine_math_math::Mat4_mul(a_4, a_5).Result;
                gaps_lamps_draw_put(gaps_[0].lamp_pool, a_3, engine_math_math::Vec4_new(1.0, 1.0, 1.0, kern::formula([](Float glow) -> Float { return ((glow * 0.55) * 0.22); }, glow)).Result);
              }
            }
          }
        }
      }
      engine_game_world_physics(n13631523, world_);
      physics = n13631523.result;
      w = physics;
      for (auto it13631524 = flow_for(n13631524, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<physics_Body>(gaps_[0].fallen))))); it13631524.next();) {
        skip_2 = false;
        body_valid = engine_world_body_valid(w, gaps_[0].fallen[n13631524.index]);
        if (logic_not(body_valid)) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          s = common::read_body(w, gaps_[0].fallen[n13631524.index]).Result;
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

  struct life_update_slotResult { Int Result = 0; };
  /// (the place of this car's entry in a list of (car, time): found, or added with 0)
  life_update_slotResult life_update_slot(const LifeCar& v_cars_n14680100_index, List<Int_Float_pair>& l) {
    for (auto it14680101 = flow_for(n14680101, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Int_Float_pair>(l))))); it14680101.next();) {
      if (math_compare<"==">(kern::to<Float>(l[n14680101.index].first), kern::to<Float>(v_cars_n14680100_index.car))) {
        return {n14680101.index};
      }
    }
    list_add(l, struct_make<Int_Float_pair>(v_cars_n14680100_index.car, 0.0));
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Int_Float_pair>(l))))};
  }

  struct life_update_blocksResult { Bool Result = false; };
  KERN_PURE life_update_blocksResult life_update_blocks(const LifeCar& v_cars_n14680100_index, Vector q = Vector{0.0, 0.0, 0.0}) {
    Vector l = Vector{0.0, 0.0, 0.0};
    l = common::to_local(v_cars_n14680100_index.rotation, vector_sub(q, v_cars_n14680100_index.position)).Result;
    return {logic_and(logic_and(logic_and(math_compare<">">(vector_break(l).x, vector_break(v_cars_n14680100_index.half).x), math_compare<"<">(vector_break(l).x, kern::formula([](Float x) -> Float { return (x + 7.0); }, vector_break(v_cars_n14680100_index.half).x))), math_compare<"<">(kern::formula([](Float y) -> Float { return kern::math::abs(y); }, vector_break(l).y), 1.6)), math_compare<"<">(kern::formula([](Float z) -> Float { return kern::math::abs(z); }, vector_break(l).z), 3.0))};
  }

  void life_draw() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Float sim_time_2 = 0.0;
    Float chosen = 0.0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)), logic_not(glife_[0].art_ready))) {
      return;
    }
    if (object_valid<kern::game::GameWorld>(world_)) {
      sim_time_2 = engine_game_world_sim_time(world_);
      chosen = sim_time_2;
    } else {
      chosen = 0.0;
    }
    now = kern::formula([](Float chosen) -> Float { return chosen; }, chosen);
    life_art_cpp::LifeArt_begin(glife_[0].art);
    if (world_placed_) {
      life_art_cpp::LifeArt_life(glife_[0].art, glife_[0].life, now, cam_.eye, kern::formula([](Float the_result) -> Float { return (170.0 * kern::math::min(the_result, 3.0)); }, view_zoom().Result));
      interiors_cpp::Interiors_draw(glife_[0].interiors, glife_[0].art, now, cam_.eye);
    }
    life_art_cpp::LifeArt_end(glife_[0].art);
  }

  void life_draw_people() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Fixed<U32, 5> skins = Fixed<U32, 5>{};
    Bool skip = false;
    People_Person p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    Int value = 0;
    Int value_2 = 0;
    Int value_3 = 0;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0))) {
      return;
    }
    if (logic_not(life::Life_has_chain(glife_[0].life).Result)) {
      return;
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    skins = list_make<U32, "5">(14725260, 12881516, 11038802, 8079923, 13804158);
    for (auto it14680108 = flow_for(n14680108, 0, 9); it14680108.next();) {
      skip = false;
      if (logic_or(logic_not(glife_[0].life.chain_.runners[n14680108.index].alive), math_compare<">">(vector_length(vector_sub(glife_[0].life.chain_.runners[n14680108.index].pos, cam_.eye)), 220.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
        p.pos = glife_[0].life.chain_.runners[n14680108.index].pos;
        p.yaw = glife_[0].life.chain_.runners[n14680108.index].yaw_s;
        p.v = 2.9;
        p.phase = glife_[0].life.chain_.runners[n14680108.index].phase;
        value = 14221114;
        p.shirt = vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value)));
        value_2 = 1845818;
        p.pants = vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value_2)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value_2)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value_2)));
        value_3 = skins[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::mod(index, 5.0); }, kern::to<Float>(n14680108.index)))];
        p.skin = vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value_3)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value_3)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value_3)));
        people_art_cpp::PeopleArt_person(people_art_, p, now);
      }
    }
  }

  struct life_hides_personResult { Bool Result = false; };
  KERN_PURE life_hides_personResult life_hides_person(I32 index = 0) {
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
    n14680109 = life_cpp::Life_shot(glife_[0].life, to, glife_[0].out, from, damage, now, false);
    return {n14680109.Result};
  }

  struct life_punchResult { Bool Result = false; };
  life_punchResult life_punch(Vector& to, Vector from = Vector{0.0, 0.0, 0.0}) {
    Float sim_time = 0.0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)), logic_not(glife_[0].ready))) {
      return {false};
    }
    sim_time = engine_game_world_sim_time(world_);
    n14680111 = life_cpp::Life_shot(glife_[0].life, to, glife_[0].out, from, 6.0, sim_time, true);
    return {n14680111.Result};
  }

  void life_net_fx(const net_Fx& f) {
    Float sim_time = 0.0;
    Vector arg = Vector{0.0, 0.0, 0.0};
    Vector dv = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    Object<kern::game::GameWorld> arg_2 = Object<kern::game::GameWorld>{};
    Bool set_actor_velocity = false;
    Object<kern::game::GameWorld> arg_3 = Object<kern::game::GameWorld>{};
    Int arg_4 = 0;
    Float arg_5 = 0.0;
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)), logic_not(glife_[0].ready)), logic_not(object_valid<kern::game::GameWorld>(world_)))) {
      return;
    }
    if (math_compare<"==">(kern::to<Float>(f.kind), kern::to<Float>(enum_index<net_Fx_Kind>(net_Fx_Kind::cow_death)))) {
      arg = vector_scale(f.dir, 600.0);
      sim_time = engine_game_world_sim_time(world_);
      life_cpp::Life_remote_cow_death(glife_[0].life, glife_[0].out, f.p, arg, sim_time);
    } else if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(f.kind), kern::to<Float>(enum_index<net_Fx_Kind>(net_Fx_Kind::push))), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::trunc((x + 0.5)); }, vector_break(f.dir).x)))), kern::to<Float>(net_session::Session_self_slot(net_).Result))), enum_equal<Mode>(mode_, Mode::foot))) {
      dv = vector_make(kern::formula([](Float x) -> Float { return (x < (-40.0) ? (-40.0) : (40.0 < x ? 40.0 : x)); }, vector_break(f.p).x), kern::formula([](Float y) -> Float { return (y < (-40.0) ? (-40.0) : (40.0 < y ? 40.0 : y)); }, vector_break(f.p).y), kern::formula([](Float z) -> Float { return (z < (-40.0) ? (-40.0) : (40.0 < z ? 40.0 : z)); }, vector_break(f.p).z));
      arg_2 = world_;
      the_actor_velocity = engine_game_world_actor_velocity(world_, soldier_);
      engine_game_world_set_actor_velocity(n14680112, arg_2, soldier_, vector_add(the_actor_velocity, dv));
      set_actor_velocity = n14680112.result;
      if (math_compare<">">(f.a, 0.0)) {
        arg_3 = world_;
        arg_4 = soldier_;
        arg_5 = kern::formula([](Float a) -> Float { return kern::math::min(100.0, a); }, f.a);
        n14680113 = player_position();
        engine_game_world_damage(arg_3, arg_4, arg_5, 0, false, vector_sub(n14680113.Result, dv));
      }
    }
  }

  void life_hud(Text& s) {
    Float now = 0.0;
    Float sim_time = 0.0;
    U32 dead = 0;
    U32 air = 0;
    U32 birds = 0;
    U32 pigeons = 0;
    U32 joggers = 0;
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
    for (auto it14680114 = flow_for(n14680114, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_))))); it14680114.next();) {
      dead += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(glife_[0].life.cows_[n14680114.index].dead)));
    }
    for (auto it14680115 = flow_for(n14680115, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it14680115.next();) {
      air += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(glife_[0].life.flocks_[n14680115.index].air)));
    }
    pigeons = 0;
    for (auto it14680116 = flow_for(n14680116, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it14680116.next();) {
      pigeons += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Pigeon>(glife_[0].life.flocks_[n14680116.index].birds))));
    }
    for (auto it14680117 = flow_for(n14680117, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Bird>(glife_[0].life.birds_))))); it14680117.next();) {
      birds += kern::to<Int>(kern::formula([](Float now, Float a) -> Float { return kern::math::wrap32(((now >= a) ? 1.0 : 0.0)); }, now, glife_[0].life.birds_[n14680117.index].dead_until));
    }
    joggers = 0;
    if (life::Life_has_chain(glife_[0].life).Result) {
      for (auto it14680118 = flow_for(n14680118, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Runner>(glife_[0].life.chain_.runners))))); it14680118.next();) {
        joggers += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(glife_[0].life.chain_.runners[n14680118.index].alive)));
      }
    }
    b = "";
    b = text_format<",\"life\":{\"cows\":%u,\"cowsDead\":%u,\"cowHits\":%u,\"flocks\":%u,\"flocksAir\":%u,\"pigeons\":%u,\"birds\":%u,\"birdHits\":%u,\"trees\":%u,\"joggers\":%u,\"joggerStreak\":%u,\"ragdolls\":%u,\"ragdollSpawns\":%u,\"falls\":%u,\"getups\":%u,\"shouts\":%u,\"honks\":%u">(639, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_)))), dead, life::Life_cow_hits(glife_[0].life).Result, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_)))), air, pigeons, birds, life::Life_bird_hits(glife_[0].life).Result, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Tree>(glife_[0].life.trees_)))), joggers, life::Life_jogger_streak(glife_[0].life).Result, ragdoll_cpp::Ragdolls_active(glife_[0].life.ragdolls_).Result, ragdoll::Ragdolls_spawned(glife_[0].life.ragdolls_).Result, glife_[0].falls, glife_[0].getups, life::Life_shouts(glife_[0].life).Result, life::Life_honks(glife_[0].life).Result);
    s = text_append(s, b, "");
    if (math_compare<"<">(now, glife_[0].banner_until)) {
      s = text_append(s, text_append(text_append(",\"banner\":\"", game_life_cpp::json_escape(glife_[0].banner).Result, ""), "\"", ""), "");
    }
    s = text_append(s, ",\"say\":[", "");
    for (auto it14680119 = flow_for(n14680119, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<LifeOut_Say>(glife_[0].speech))))); it14680119.next();) {
      b = text_format<"%s[\"%s\",%.2f,%.2f,%.2f,%.2f]">(639, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n14680119.index), 0.0), ",", ""), game_life_cpp::json_escape(glife_[0].speech[n14680119.index].text).Result, glife_[0].speech[n14680119.index].pitch, glife_[0].speech[n14680119.index].rate, glife_[0].speech[n14680119.index].volume, glife_[0].speech[n14680119.index].delay);
      s = text_append(s, b, "");
    }
    list_clear(glife_[0].speech);
    s = text_append(s, "],\"bubbles\":[", "");
    first = true;
    aspect = kern::formula([](Float frame_h_, Float frame_w_) -> Float { return ((frame_h_ > 0.0) ? kern::math::div(frame_w_, frame_h_) : kern::math::div(16.0, 9.0)); }, frame_h_, frame_w_);
    a = engine_math_math::projection_matrix(cam_, aspect).Result;
    n14680120 = engine_math_math::Mat4_look_at_2(cam_.eye, cam_.target, cam_.up);
    a_2 = n14680120.Result;
    vp = engine_math_math::Mat4_mul(a, a_2).Result;
    for (auto it14680121 = flow_for(n14680121, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Voice>(glife_[0].life.voices_))))); it14680121.next();) {
      skip = false;
      c = engine_math_math::Mat4_transform(vp, engine_math_math::Vec4_new(vector_break(glife_[0].life.voices_[n14680121.index].at).x, vector_break(glife_[0].life.voices_[n14680121.index].at).y, vector_break(glife_[0].life.voices_[n14680121.index].at).z, 1.0).Result).Result;
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
          b = text_format<"%s[%.4f,%.4f,\"%s\",\"%s\",%s]">(639, logic_select<Text>(first, "", ","), kern::formula([](Float x) -> Float { return kern::math::div((x + 1.0), 2.0); }, x), kern::formula([](Float y) -> Float { return kern::math::div((1.0 - y), 2.0); }, y), game_life_cpp::json_escape(glife_[0].life.voices_[n14680121.index].text).Result, glife_[0].life.voices_[n14680121.index].mood, logic_select<Text>(math_compare<"<">(kern::formula([](Float a, Float now) -> Float { return (a - now); }, glife_[0].life.voices_[n14680121.index].until, now), 0.35), "true", "false"));
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

  struct look_people_budgetResult { I32 Result = 0; };
  KERN_PURE look_people_budgetResult look_people_budget() {
    return {kern::to<Int>(kern::formula([](Float a, Float mobile_) -> Float { return ((a != 0.0) ? ((mobile_ != 0.0) ? 68.0 : 100.0) : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::world_people).Result), kern::to<Float>(mobile_)))};
  }

  void look_apply_tier() {
    I32 t = 0;
    perf_Tier q = perf_Tier{"", 0, 0.0, 0, 0, 0.0, 0.0, 0, 0.0};
    Object<kern::LightPool> old = Object<kern::LightPool>{};
    Object<kern::LightPool> light_pool = Object<kern::LightPool>{};
    Object<kern::Renderer> r = Object<kern::Renderer>{};
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    AoOptions ao = AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true};
    t = perf::Governor_tier(look_.gov).Result;
    q = list_get<perf_Tier>(kTier, t);
    people::People_set_budget(people_, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(look_people_budget().Result))));
    look_.lamp_m = q.lamp_m;
    look_.people_m = q.people_m;
    if (math_compare<"!=">(kern::to<Float>(look_.pool_size), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float lights) -> Float { return kern::math::wrap32(lights); }, kern::to<Float>(q.lights))))))) {
      look_.pool_size = kern::to<Int>(kern::formula([](Float lights) -> Float { return kern::math::wrap32(lights); }, kern::to<Float>(q.lights)));
      old = look_.lights;
      engine_light_pool_make(n15728650, look_.pool_size, 96);
      light_pool = n15728650.result;
      look_.lights = light_pool;
      if (object_valid<kern::LightPool>(old)) {
        engine_light_pool_release(old);
      }
    }
    if (object_valid<kern::Engine>(engine_)) {
      renderer = engine_engine_renderer(engine_);
      r = renderer;
      engine_renderer_set_shadow_filter(r, q.shadow_filter);
      ao = AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true};
      ao.enabled = logic_and(false, math_compare<">">(kern::to<Float>(q.ao), 0.0));
      ao.quality = logic_select<AoQuality>(math_compare<">=">(kern::to<Float>(q.ao), 2.0), AoQuality::medium, AoQuality::low);
      ao.half_resolution = math_compare<"<">(kern::to<Float>(q.ao), 3.0);
      engine_renderer_set_ambient_occlusion(r, ao);
    }
    if (logic_and(logic_and(math_compare<">=">(kern::to<Float>(look_.applied_tier), 0.0), math_compare<"!=">(kern::to<Float>(look_.applied_tier), kern::to<Float>(t))), perf::Governor_automatic(look_.gov).Result)) {
      toast_ = text_append("Grafik automatisch: ", q.name, "");
      toast_age_ = 0.0;
    }
    look_.applied_tier = t;
  }

  void look_begin(const Frame& f, Object<kern::Engine> e = Object<kern::Engine>{}) {
    Bool has_arg = false;
    Float q = 0.0;
    Float arg_number = 0.0;
    Bool live = false;
    perf::Meter_begin_frame(look_.meter, game_look_cpp::clock_ms().Result);
    if (logic_not(look_.started)) {
      look_.started = true;
      perf_cpp::Governor_reset(look_.gov, kern::to<Int>(kern::formula([](Float mobile_, Float kMobileStart, Float kDesktopStart) -> Float { return ((mobile_ != 0.0) ? kMobileStart : kDesktopStart); }, kern::to<Float>(mobile_), kern::to<Float>(kMobileStart), kern::to<Float>(kDesktopStart))), kern::to<Int>(kern::formula([](Float mobile_, Float kMobileMax, Float kTiers) -> Float { return ((mobile_ != 0.0) ? kMobileMax : (kTiers - 1.0)); }, kern::to<Float>(mobile_), kern::to<Float>(kMobileMax), kern::to<Float>(kTiers))));
      has_arg = engine_engine_has_arg(e, "quality");
      if (has_arg) {
        arg_number = engine_engine_arg_number(e, "quality", -1.0);
        q = arg_number;
        if (logic_and(math_compare<">=">(q, 0.0), math_compare<"<">(q, kern::to<Float>(kTiers)))) {
          perf_cpp::Governor_set_fixed(look_.gov, kern::to<Int>(kern::formula([](Float q) -> Float { return kern::math::trunc(q); }, q)));
        }
      }
      look_apply_tier();
    }
    live = logic_and(world_placed_, logic_not(ui_.paused));
    if (live) {
      look_.tod = kern::formula([](Float tod, Float dt, Float kDayLength) -> Float { return (tod + kern::math::div(dt, kDayLength)); }, look_.tod, f.dt, kDayLength);
      look_.tod = kern::formula([](Float tod) -> Float { return (tod - kern::math::floor(tod)); }, look_.tod);
      if (settings::Settings_on(ui_.settings, Opt::world_always_day).Result) {
        look_.tod = 0.42;
      }
      n15728651 = perf_cpp::Governor_update(look_.gov, f.dt);
      if (n15728651.Result) {
        look_apply_tier();
      }
    }
    look_.sync_clock = kern::formula([](Float sync_clock, Float dt) -> Float { return (sync_clock + dt); }, look_.sync_clock, f.dt);
    if (logic_and(logic_and(net_session::Session_active(net_).Result, logic_not(net_session::Session_hosting(net_).Result)), math_compare<">">(look_.sync_clock, 1.0))) {
      look_.sync_clock = 0.0;
      for (auto it15728652 = flow_for(n15728652, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it15728652.next();) {
        if (logic_and(math_compare<"==">(kern::to<Float>(net_.remotes_[n15728652.index].state.slot), 255.0), math_compare<">=">(net_.remotes_[n15728652.index].state.day, 0.0))) {
          look_.tod = look_cpp::follow_phase(look_.tod, net_.remotes_[n15728652.index].state.day).Result;
        }
      }
    }
    if (math_compare<"!=">(kern::to<Float>(ui_.applied), kern::to<Float>(look_.applied_settings))) {
      look_.applied_settings = ui_.applied;
      people::People_set_budget(people_, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(look_people_budget().Result))));
    }
  }

  void look_lap(perf_Slot s = perf_Slot::logic) {
    perf_cpp::Meter_lap(look_.meter, s, game_look_cpp::clock_ms().Result);
  }

  struct view_zoomResult { Float Result = 0.0; };
  /// world_procedural_population.mjs PERSON_SLOTS 100 (phone 68): the original does not scale the crowd with the quality level
  KERN_PURE view_zoomResult view_zoom() {
    Float base = 0.0;
    Float t = 0.0;
    if (logic_not(enum_equal<Mode>(mode_, Mode::foot))) {
      return {1.0};
    }
    base = kern::formula([](Float fov, Float the_result, Float kDeg) -> Float { return kern::math::max(fov, (the_result * kDeg)); }, cam_.fov, settings::Settings_get(ui_.settings, Opt::cam_fov).Result, kDeg);
    t = kern::formula([](Float fov) -> Float { return kern::math::tan((kern::math::max(0.05, fov) * 0.5)); }, cam_.fov);
    return {kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float base, Float t) -> Float { return kern::math::div(kern::math::tan((kern::math::min(base, 3.0) * 0.5)), kern::math::max(0.001, t)); }, base, t), 1.0, 12.0))};
  }

  void look_camera(Float dt = 0.0) {
    Vector v = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    Vector angles = Vector{0.0, 0.0, 0.0};
    Vector offset = Vector{0.0, 0.0, 0.0};
    if (math_compare<"!=">(kern::to<Float>(enum_index<Mode>(mode_)), kern::to<Float>(look_.last_mode))) {
      look_.last_mode = enum_index<Mode>(mode_);
      look::Stabilizer_reset(look_.stab);
      look_.vel_valid = false;
    }
    if (logic_not(world_placed_)) {
      return;
    }
    if (logic_and(enum_equal<Mode>(mode_, Mode::drone), math_compare<"==">(kern::to<Float>(drone_cam_), 0.0))) {
      look_cpp::Stabilizer_apply(look_.stab, cam_, dt);
    } else {
      look::Stabilizer_reset(look_.stab);
    }
    if (logic_or(logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0)), enum_equal<Mode>(mode_, Mode::drone))) {
      the_actor_velocity = engine_game_world_actor_velocity(world_, kern::to<Int>(kern::formula([](Float a, Float a_2, Float drone_actor_) -> Float { return ((a != 0.0) ? a_2 : drone_actor_); }, kern::to<Float>(enum_equal<Mode>(mode_, Mode::car)), kern::to<Float>(cars_[car_index_].actor), kern::to<Float>(drone_actor_))));
      v = the_actor_velocity;
      if (logic_and(look_.vel_valid, logic_not(ui_.paused))) {
        look_cpp::Shake_add(look_.shake, look::impact_trauma(vector_length(vector_sub(v, look_.last_vel))).Result, 1.0);
      }
      look_.last_vel = v;
      look_.vel_valid = true;
    } else {
      look_.vel_valid = false;
    }
    angles = Vector{0.0, 0.0, 0.0};
    offset = Vector{0.0, 0.0, 0.0};
    n15728653 = look_cpp::Shake_sample(look_.shake, angles, offset, dt, time_, kern::formula([](Float a) -> Float { return ((a != 0.0) ? 0.85 : 1.0); }, kern::to<Float>(enum_equal<Mode>(mode_, Mode::drone))));
    if (logic_and(n15728653.Result, logic_not(enum_equal<Mode>(mode_, Mode::foot)))) {
      look_cpp::shake_camera(cam_, angles, offset);
    }
  }

  void look_vr_hud() {
    Object<kern::Renderer> r = Object<kern::Renderer>{};
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    Bool on = false;
    Bool xr = false;
    Bool xr_2 = false;
    Bool both = false;
    Bool set_visible = false;
    MeshData q = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    Vector c = Vector{0.0, 0.0, 0.0};
    Vector rt = Vector{0.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 0.0};
    Vector to_eye = Vector{0.0, 0.0, 0.0};
    Float w = 0.0;
    Float h = 0.0;
    Mat4 lh = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector hx = Vector{0.0, 0.0, 0.0};
    Vector hy = Vector{0.0, 0.0, 0.0};
    Vector hz = Vector{0.0, 0.0, 0.0};
    Vector xr_eye = Vector{0.0, 0.0, 0.0};
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector xr_forward = Vector{0.0, 0.0, 0.0};
    Vector xr_eye_2 = Vector{0.0, 0.0, 0.0};
    Vector xr_eye_3 = Vector{0.0, 0.0, 0.0};
    Fixed<Float, 16> m = Fixed<Float, 16>{};
    Bool update_instances = false;
    Bool set_visible_2 = false;
    renderer = engine_engine_renderer(engine_);
    r = renderer;
    both = vr_;
    if (both) {
      xr_2 = engine_engine_xr(engine_);
      both = xr_2;
    }
    on = logic_and(both, math_compare<"!=">(kern::to<Float>(vr_hud_tex_), 0.0));
    if (logic_not(on)) {
      if (math_compare<"!=">(kern::to<Float>(vr_hud_mesh_), 0.0)) {
        engine_renderer_set_visible(n15728662, r, vr_hud_mesh_, false);
        set_visible = n15728662.result;
      }
      return;
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(vr_hud_mesh_), 0.0))) {
      q = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
      engine_geo_mesh_data::MeshData_push(q, Vector{-0.5, 0.0, 0.5}, Vector{0.0, -1.0, 0.0}, Vector{1.0, 1.0, 1.0}, Vector{0.0, 0.0, 0.0});
      engine_geo_mesh_data::MeshData_push(q, Vector{0.5, 0.0, 0.5}, Vector{0.0, -1.0, 0.0}, Vector{1.0, 1.0, 1.0}, Vector{1.0, 0.0, 0.0});
      engine_geo_mesh_data::MeshData_push(q, Vector{0.5, 0.0, -0.5}, Vector{0.0, -1.0, 0.0}, Vector{1.0, 1.0, 1.0}, Vector{1.0, 1.0, 0.0});
      engine_geo_mesh_data::MeshData_push(q, Vector{-0.5, 0.0, -0.5}, Vector{0.0, -1.0, 0.0}, Vector{1.0, 1.0, 1.0}, Vector{0.0, 1.0, 0.0});
      q.indices = list_make<U32, "6">(0, 2, 1, 0, 3, 2);
      engine_renderer_allocate_id(n15728663, r);
      allocate_id = n15728663.result;
      vr_hud_mesh_ = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
      look.material.unlit = true;
      look.material.double_sided = true;
      look.material.cast_shadow = false;
      look.material.frustum_culled = false;
      look.material.alpha = AlphaMode::blend;
      look.extra_slot = kMaxExtensionAttributes;
      look.texture = vr_hud_tex_;
      engine_upsert(n15728664, r, vr_hud_mesh_, q, look, List<Float>{}, List<Float>{});
      upsert = n15728664.result;
      if (logic_not(upsert)) {
        engine_renderer_release_id(r, vr_hud_mesh_);
        vr_hud_mesh_ = 0;
        return;
      }
    }
    c = Vector{0.0, 0.0, 0.0};
    rt = Vector{0.0, 0.0, 0.0};
    up = Vector{0.0, 0.0, 0.0};
    to_eye = Vector{0.0, 0.0, 0.0};
    w = 0.2;
    h = 0.15;
    lh = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    n15728665 = vr_hand_matrix(lh, 0);
    if (n15728665.Result) {
      hx = logic_select<Vector>(math_compare<">">(vector_length(vector_make(lh.m[0], lh.m[1], lh.m[2])), 9.999999717180685e-10), vector_normalize(vector_make(lh.m[0], lh.m[1], lh.m[2])), Vector{1.0, 0.0, 0.0});
      hy = logic_select<Vector>(math_compare<">">(vector_length(vector_make(lh.m[4], lh.m[5], lh.m[6])), 9.999999717180685e-10), vector_normalize(vector_make(lh.m[4], lh.m[5], lh.m[6])), Vector{0.0, 0.0, 1.0});
      hz = logic_select<Vector>(math_compare<">">(vector_length(vector_make(lh.m[8], lh.m[9], lh.m[10])), 9.999999717180685e-10), vector_normalize(vector_make(lh.m[8], lh.m[9], lh.m[10])), Vector{0.0, 1.0, 0.0});
      c = vector_add(vector_add(engine_math_math::Mat4_translation(lh).Result, vector_scale(hy, 0.06)), vector_scale(hz, 0.09));
      if (vr_menu_open_) {
        w = 0.32;
        h = 0.24;
        c = vector_add(c, vector_scale(hy, 0.06));
      }
      xr_eye = engine_engine_xr_eye(engine_);
      to_eye = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(xr_eye, c)), 9.999999717180685e-10), vector_normalize(vector_sub(xr_eye, c)), hy);
      rt = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(Vector{0.0, 0.0, 1.0}, to_eye)), 9.999999717180685e-10), vector_normalize(vector_cross(Vector{0.0, 0.0, 1.0}, to_eye)), Vector{1.0, 0.0, 0.0});
      up = vector_cross(to_eye, rt);
    } else {
      xr_forward = engine_engine_xr_forward(engine_);
      f = xr_forward;
      f = vector_make(vector_break(f).x, vector_break(f).y, 0.0);
      f = logic_select<Vector>(math_compare<">">(vector_length(f), 9.999999717180685e-10), vector_normalize(f), Vector{1.0, 0.0, 0.0});
      rt = vector_make(vector_break(f).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(f).x), 0.0);
      xr_eye_2 = engine_engine_xr_eye(engine_);
      c = vector_sub(vector_add(xr_eye_2, vector_scale(f, 1.1)), Vector{0.0, 0.0, 0.85});
      xr_eye_3 = engine_engine_xr_eye(engine_);
      to_eye = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(xr_eye_3, c)), 9.999999717180685e-10), vector_normalize(vector_sub(xr_eye_3, c)), vector_scale(f, -1.0));
      rt = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(Vector{0.0, 0.0, 1.0}, to_eye)), 9.999999717180685e-10), vector_normalize(vector_cross(Vector{0.0, 0.0, 1.0}, to_eye)), rt);
      up = vector_cross(to_eye, rt);
      w = 0.48;
      h = 0.36;
    }
    vr_menu_valid_ = true;
    vr_menu_c_ = c;
    vr_menu_r_ = rt;
    vr_menu_u_ = up;
    vr_menu_n_ = to_eye;
    vr_menu_w_ = w;
    vr_menu_h_ = h;
    m = list_make<Float, "16">(kern::formula([](Float x, Float w) -> Float { return (x * w); }, vector_break(rt).x, w), kern::formula([](Float y, Float w) -> Float { return (y * w); }, vector_break(rt).y, w), kern::formula([](Float z, Float w) -> Float { return (z * w); }, vector_break(rt).z, w), 0.0, kern::formula([](Float x) -> Float { return (-x); }, vector_break(to_eye).x), kern::formula([](Float y) -> Float { return (-y); }, vector_break(to_eye).y), kern::formula([](Float z) -> Float { return (-z); }, vector_break(to_eye).z), 0.0, kern::formula([](Float x, Float h) -> Float { return (x * h); }, vector_break(up).x, h), kern::formula([](Float y, Float h) -> Float { return (y * h); }, vector_break(up).y, h), kern::formula([](Float z, Float h) -> Float { return (z * h); }, vector_break(up).z, h), 0.0, vector_break(c).x, vector_break(c).y, vector_break(c).z, 1.0);
    engine_renderer_update_instances(n15728666, r, vr_hud_mesh_, m, List<Float>{});
    update_instances = n15728666.result;
    engine_renderer_set_visible(n15728667, r, vr_hud_mesh_, true);
    set_visible_2 = n15728667.result;
  }

  void look_view_models_make(const MeshData& d, U32& id, Object<kern::Renderer> r = Object<kern::Renderer>{}, I32 n15728668_index = 0, Bool g = false) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    r_ = r;
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(d.positions)), 0.0)) {
      return;
    }
    engine_renderer_allocate_id(n15728670, r_);
    allocate_id = n15728670.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    look.material = game_look_cpp::vm_material(g, math_compare<"==">(kern::to<Float>(n15728668_index), 1.0)).Result;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    engine_upsert(n15728671, r_, id, d, look, List<Float>{}, List<Float>{});
    upsert = n15728671.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, id);
      id = 0;
    }
  }

  struct look_hud_jsonResult { Text Result = ""; };
  look_hud_jsonResult look_hud_json() {
    perf_Tier q = perf_Tier{"", 0, 0.0, 0, 0, 0.0, 0.0, 0, 0.0};
    Float hours = 0.0;
    I32 hh = 0;
    I32 mm = 0;
    Text b = "";
    q = list_get<perf_Tier>(kTier, perf::Governor_tier(look_.gov).Result);
    hours = kern::formula([](Float tod) -> Float { return (tod * 24.0); }, look_.tod);
    hh = kern::to<Int>(kern::formula([](Float hours) -> Float { return kern::math::mod(kern::math::trunc(hours), 24.0); }, hours));
    mm = kern::to<Int>(kern::formula([](Float hours) -> Float { return kern::math::trunc(((hours - kern::math::floor(hours)) * 60.0)); }, hours));
    b = "";
    b = text_format<",\"perf\":{\"fps\":%.1f,\"tier\":%d,\"tierName\":\"%s\",\"auto\":%s,\"show\":%s,\"changes\":%u,\"lights\":%u,\"ms\":{\"logic\":%.2f,\"physics\":%.2f,\"stream\":%.2f,\"draw\":%.2f,\"update\":%.2f,\"render\":%.2f,\"gpu\":%.2f}},\"renderScale\":%.2f,\"timeOfDay\":%.4f,\"clock\":\"%02d:%02d\",\"night\":%.2f,\"look\":{\"shake\":%.2f,\"speedLines\":%.2f,\"fpvStab\":%s,\"viewModel\":\"%s\",\"stereo\":false}">(719, perf::Governor_fps(look_.gov).Result, perf::Governor_tier(look_.gov).Result, q.name, logic_select<Text>(perf::Governor_automatic(look_.gov).Result, "true", "false"), logic_select<Text>(look_.show_perf, "true", "false"), perf::Governor_changes(look_.gov).Result, look_.lights_used, perf::Meter_ms(look_.meter, perf_Slot::logic).Result, perf::Meter_ms(look_.meter, perf_Slot::physics).Result, perf::Meter_ms(look_.meter, perf_Slot::stream).Result, perf::Meter_ms(look_.meter, perf_Slot::draw).Result, perf::Meter_update_ms(look_.meter).Result, perf::Meter_render_ms(look_.meter).Result, perf::Meter_gpu_ms(look_.meter).Result, q.render_scale, look_.tod, hh, mm, night_, look_.shake.trauma, look_.lines.k, logic_select<Text>(look_.stab.init, "true", "false"), logic_select<Text>(math_compare<">=">(kern::to<Float>(look_.vm_shown), 0.0), list_get<Text>(kVmNames, look_.vm_shown), ""));
    return {b};
  }

  struct look_command_startsResult { Bool Result = false; };
  KERN_PURE look_command_startsResult look_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct combat_targetResult { Bool Result = false; };
  combat_targetResult combat_target(Vector& at, Vector& vel, Bool& drone, Bool& air) {
    U32 a = 0;
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

  struct update_flak_matchResult { Bool Result = false; };
  KERN_PURE update_flak_matchResult update_flak_match(const FlakGun& g, Int64 cx = Int64{0x0}, Int64 dx = Int64{0x0}, Int64 cy = Int64{0x0}, Int64 dy = Int64{0x0}) {
    return {logic_and(int64_equal(g.cx, int64_add(cx, dx)), int64_equal(g.cy, int64_add(cy, dy)))};
  }

  void launch_jets(Float cx = 0.0, Float cy = 0.0, Bool strike = false, Float at = 0.0, Float alt = 0.0, Float offset = 0.0, Bool remote = false) {
    Float z = 0.0;
    FighterJet j = FighterJet{0.0, 0.0, 0.0, 0.0, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false};
    z = kern::formula([](Float the_result, Float alt) -> Float { return (the_result + alt); }, city_stream_cpp::CityStream_ground_height(city_, cx, cy).Result, alt);
    for (auto it16777281 = flow_for(n16777281, 0, 1); it16777281.next();) {
      j = FighterJet{0.0, 0.0, 0.0, 0.0, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false};
      j.x = kern::formula([](Float cx, Float offset, Float index) -> Float { return ((cx + offset) - (index * 26.0)); }, cx, offset, kern::to<Float>(n16777281.index));
      j.y0 = kern::formula([](Float cy, Float combat_kJetSpan, Float index) -> Float { return ((cy - combat_kJetSpan) - (index * 42.0)); }, cy, combat_kJetSpan, kern::to<Float>(n16777281.index));
      j.z = kern::formula([](Float z, Float index) -> Float { return (z + (index * 6.0)); }, z, kern::to<Float>(n16777281.index));
      j.t0 = at;
      j.strike = logic_and(strike, math_compare<"==">(kern::to<Float>(n16777281.index), 0.0));
      j.target = vector_make(cx, cy, 0.0);
      j.remote = remote;
      list_add(combat_.jets, j);
    }
  }

  struct update_jets_matchResult { Bool Result = false; };
  KERN_PURE update_jets_matchResult update_jets_match(const FighterJet& j, Float combat_kJetSpeed_2 = 0.0, Float now = 0.0, Float combat_kJetSpan_2 = 0.0) {
    return {math_compare<">">(kern::formula([](Float y0, Float combat_kJetSpeed_2, Float now, Float t0) -> Float { return (y0 + (combat_kJetSpeed_2 * (now - t0))); }, j.y0, combat_kJetSpeed_2, now, j.t0), kern::formula([](Float y0, Float combat_kJetSpan_2) -> Float { return (y0 + (2.0 * combat_kJetSpan_2)); }, j.y0, combat_kJetSpan_2))};
  }

  struct draw_military_matchResult { Bool Result = false; };
  /// updateBlasts: hot core (sphere r .45, x .5 .. 5, opacity .9 (1-t)) and flat ring (inner .6 outer 1, x .7 .. 7.7, opacity .78 (1-t)^1.6), e = 1 - (1-t)^3 over 540 ms, additive orange / gold
  KERN_PURE draw_military_matchResult draw_military_match(const Game_VisualBlast& b, Float now = 0.0) {
    return {logic_or(math_compare<">=">(kern::formula([](Float now, Float born) -> Float { return (now - born); }, now, b.born), 0.54), math_compare<"<">(now, b.born))};
  }

  struct draw_military_match_2Result { Bool Result = false; };
  KERN_PURE draw_military_match_2Result draw_military_match_2(const Tracer& t, Float now = 0.0) {
    return {math_compare<">">(now, t.until)};
  }

  void draw_military() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Quat a_4 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_5 = Quat{0.0, 0.0, 0.0, 1.0};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float a_9 = 0.0;
    Float s = 0.0;
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector p = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    Float u = 0.0;
    Vector p_2 = Vector{0.0, 0.0, 0.0};
    Mat4 a_13 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_14 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_15 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float t = 0.0;
    Float e = 0.0;
    Float s_2 = 0.0;
    Mat4 a_16 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_17 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_18 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float s_3 = 0.0;
    Mat4 a_19 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_20 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_21 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float k = 0.0;
    Float len = 0.0;
    Float pulse = 0.0;
    Mat4 a_22 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_23 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_24 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_25 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_26 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_27 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float t_2 = 0.0;
    Float until = 0.0;
    Float kRunLen = 0.0;
    Float pulse_2 = 0.0;
    Float fade = 0.0;
    Quat arm_l = Quat{0.0, 0.0, 0.0, 1.0};
    Quat arm_r = Quat{0.0, 0.0, 0.0, 1.0};
    Bool skip_2 = false;
    Float u_2 = 0.0;
    Float y = 0.0;
    Float a_28 = 0.0;
    Vector c = Vector{0.0, 0.0, 0.0};
    Mat4 a_29 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_30 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_31 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_32 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_33 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_34 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_35 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_36 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_37 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_38 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    Float t_3 = 0.0;
    Float e_2 = 0.0;
    Float hot = 0.0;
    Float ring = 0.0;
    Mat4 a_39 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_40 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_41 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_42 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_43 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_44 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip_3 = false;
    Float r = 0.0;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_45 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_46 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_47 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_48 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float the_band = 0.0;
    Float tube = 0.0;
    Mat4 a_49 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_50 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_51 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_52 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_53 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_54 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_55 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_56 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip_4 = false;
    Float t_4 = 0.0;
    Float e_3 = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector ax = Vector{0.0, 0.0, 0.0};
    Float al = 0.0;
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Float ring_2 = 0.0;
    Float flash = 0.0;
    Mat4 a_57 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_58 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_59 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_60 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_61 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_62 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_63 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_64 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector dot = Vector{0.0, 0.0, 0.0};
    Vector to_eye = Vector{0.0, 0.0, 0.0};
    Float dist = 0.0;
    Float size = 0.0;
    Vector n_2 = Vector{0.0, 0.0, 0.0};
    Vector at_2 = Vector{0.0, 0.0, 0.0};
    Vector ax_2 = Vector{0.0, 0.0, 0.0};
    Float al_2 = 0.0;
    Quat q_2 = Quat{0.0, 0.0, 0.0, 1.0};
    Mat4 a_65 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_66 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_67 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_68 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_69 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_70 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int at_3 = 0;
    Int kept_2 = 0;
    Int end_2 = 0;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    for (auto it16777291 = flow_for(n16777291, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Heli>(combat_.helis))))); it16777291.next();) {
      a_2 = engine_math_math::Mat4_translate(combat_.helis[n16777291.index].p).Result;
      a_4 = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, combat_.helis[n16777291.index].yaw).Result;
      a_5 = engine_math_math::Quat_axis_angle(Vector{1.0, 0.0, 0.0}, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a) -> Float { return ((-a) * 0.004); }, vector_length(combat_.helis[n16777291.index].v)), -0.25, 0.0))).Result;
      a_3 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_mul(a_4, a_5).Result, Vector{1.0, 1.0, 1.0}).Result;
      a = engine_math_math::Mat4_mul(a_2, a_3).Result;
      combat_art_cpp::CombatArt_heli(combat_art_, a, combat_.helis[n16777291.index].rotor, enum_equal<Heli_State>(combat_.helis[n16777291.index].state, Heli_State::down));
    }
    for (auto it16777292 = flow_for(n16777292, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Tank>(combat_.tanks))))); it16777292.next();) {
      a_7 = engine_math_math::Mat4_translate(combat_.tanks[n16777292.index].p).Result;
      a_8 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(combat_.tanks[n16777292.index].yaw).Result, Vector{1.0, 1.0, 1.0}).Result;
      a_6 = engine_math_math::Mat4_mul(a_7, a_8).Result;
      combat_art_cpp::CombatArt_tank(combat_art_, a_6, combat_.tanks[n16777292.index].turret, combat_.tanks[n16777292.index].wreck);
    }
    swarm_draw();
    for (auto it16777293 = flow_for(n16777293, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Tank>(combat_.tanks))))); it16777293.next();) {
      if (logic_and(combat_.tanks[n16777293.index].wreck, math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, combat_.tanks[n16777293.index].wreck_at), 40.0))) {
        people_art_cpp::PeopleArt_fire(people_art_, vector_add(combat_.tanks[n16777293.index].p, Vector{0.0, 0.0, 1.6}), 0.9, now);
      }
    }
    for (auto it16777294 = flow_for(n16777294, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<FlakGun>(combat_.flak))))); it16777294.next();) {
      if (math_compare<"<">(vector_length(vector_sub(combat_.flak[n16777294.index].p, cam_.eye)), 900.0)) {
        combat_art_cpp::CombatArt_flak(combat_art_, combat_.flak[n16777294.index].p, combat_.flak[n16777294.index].base_yaw, combat_.flak[n16777294.index].yaw, combat_.flak[n16777294.index].pitch, combat_.flak[n16777294.index].dead, math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, combat_.flak[n16777294.index].flash_at), 0.045));
      }
    }
    for (auto it16777295 = flow_for(n16777295, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<FlakRound>(combat_.flak_rounds))))); it16777295.next();) {
      combat_art_cpp::CombatArt_beam(combat_art_, combat_.flak_rounds[n16777295.index].p, vector_sub(combat_.flak_rounds[n16777295.index].p, vector_scale(logic_select<Vector>(math_compare<">">(vector_length(combat_.flak_rounds[n16777295.index].v), 9.999999717180685e-10), vector_normalize(combat_.flak_rounds[n16777295.index].v), Vector{0.0, 0.0, 1.0}), kern::formula([](Float a) -> Float { return kern::math::min(14.0, (a * 0.016)); }, vector_length(combat_.flak_rounds[n16777295.index].v)))), 0.12, Vector{3.2, 1.8, 0.6});
    }
    for (auto it16777296 = flow_for(n16777296, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Puff>(combat_.puffs))))); it16777296.next();) {
      a_9 = kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, combat_.puffs[n16777296.index].born);
      s = kern::formula([](Float a_9) -> Float { return ((1.2 + (a_9 * 2.2)) * 2.0); }, a_9);
      a_11 = engine_math_math::Mat4_translate(vector_add(combat_.puffs[n16777296.index].p, vector_make(0.0, 0.0, kern::formula([](Float a_9) -> Float { return (a_9 * 0.6); }, a_9)))).Result;
      a_12 = engine_math_math::Mat4_scale(vector_make(s, s, s)).Result;
      a_10 = engine_math_math::Mat4_mul(a_11, a_12).Result;
      combat_art_cpp::CombatArt_put(combat_art_, a_10, CombatArt_Shape::smoke, Vector{0.14, 0.13, 0.12});
    }
    for (auto it16777297 = flow_for(n16777297, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<FighterJet>(combat_.jets))))); it16777297.next();) {
      p = vector_make(combat_.jets[n16777297.index].x, kern::formula([](Float a, Float combat_kJetSpeed, Float now, Float a_2) -> Float { return (a + (combat_kJetSpeed * (now - a_2))); }, combat_.jets[n16777297.index].y0, combat_kJetSpeed, now, combat_.jets[n16777297.index].t0), combat_.jets[n16777297.index].z);
      if (math_compare<"<">(vector_length(vector_sub(p, cam_.eye)), 2400.0)) {
        combat_art_cpp::CombatArt_fighter(combat_art_, p, kern::formula([](Float now, Float a) -> Float { return (0.8 + (0.5 * kern::math::abs(kern::math::sin(((now * 37.0) + a))))); }, now, combat_.jets[n16777297.index].x));
      }
    }
    for (auto it16777298 = flow_for(n16777298, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Bomb>(combat_.bombs))))); it16777298.next();) {
      skip = false;
      u = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float a, Float a_2) -> Float { return kern::math::div((now - a), a_2); }, now, combat_.bombs[n16777298.index].t0, combat_.bombs[n16777298.index].dur), 0.0, 1.0));
      if (math_compare<"<">(now, combat_.bombs[n16777298.index].t0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        p_2 = vector_add(combat_.bombs[n16777298.index].from, vector_scale(vector_sub(combat_.bombs[n16777298.index].to, combat_.bombs[n16777298.index].from), u));
        p_2 = vector_make(vector_break(p_2).x, vector_break(p_2).y, kern::formula([](Float z, Float z_2, Float u) -> Float { return (z + (((z_2 - z) * u) * u)); }, vector_break(combat_.bombs[n16777298.index].from).z, vector_break(combat_.bombs[n16777298.index].to).z, u));
        a_14 = engine_math_math::Mat4_translate(p_2).Result;
        a_15 = engine_math_math::Mat4_scale(Vector{0.44, 0.44, 1.5}).Result;
        a_13 = engine_math_math::Mat4_mul(a_14, a_15).Result;
        combat_art_cpp::CombatArt_put(combat_art_, a_13, CombatArt_Shape::cylinder, Vector{0.23, 0.25, 0.21});
      }
    }
    for (auto it16777299 = flow_for(n16777299, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Blast>(combat_.blasts))))); it16777299.next();) {
      t = kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, combat_.blasts[n16777299.index].born);
      e = kern::formula([](Float t) -> Float { return (1.0 - kern::math::exp(((-t) * 7.0))); }, t);
      if (math_compare<"<">(t, 0.62)) {
        s_2 = kern::formula([](Float e, Float a) -> Float { return (((2.0 + (11.0 * e)) * 2.0) * a); }, e, combat_.blasts[n16777299.index].scale);
        a_17 = engine_math_math::Mat4_translate(combat_.blasts[n16777299.index].p).Result;
        a_18 = engine_math_math::Mat4_scale(vector_make(s_2, s_2, s_2)).Result;
        a_16 = engine_math_math::Mat4_mul(a_17, a_18).Result;
        combat_art_cpp::CombatArt_put(combat_art_, a_16, CombatArt_Shape::glow_sphere, vector_scale(Vector{3.0, 1.8, 0.7}, kern::formula([](Float t) -> Float { return (1.0 - (t * 1.6)); }, t)));
      }
      s_3 = kern::formula([](Float t, Float a) -> Float { return (((3.0 + (12.0 * kern::math::min(1.0, (t * 0.8)))) * 2.0) * a); }, t, combat_.blasts[n16777299.index].scale);
      a_20 = engine_math_math::Mat4_translate(vector_add(combat_.blasts[n16777299.index].p, vector_make(0.0, 0.0, kern::formula([](Float t) -> Float { return (t * 2.2); }, t)))).Result;
      a_21 = engine_math_math::Mat4_scale(vector_make(s_3, s_3, kern::formula([](Float s_3) -> Float { return (s_3 * 0.8); }, s_3))).Result;
      a_19 = engine_math_math::Mat4_mul(a_20, a_21).Result;
      combat_art_cpp::CombatArt_put(combat_art_, a_19, CombatArt_Shape::smoke, vector_add(vector_scale(Vector{0.17, 0.15, 0.14}, kern::formula([](Float t) -> Float { return (1.0 - kern::math::div(t, 6.0)); }, t)), Vector{0.05, 0.05, 0.05}));
    }
    for (auto it16777300 = flow_for(n16777300, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Flare>(combat_.flares))))); it16777300.next();) {
      k = kern::formula([](Float now, Float a) -> Float { return kern::math::min(1.0, kern::math::div((now - a), 0.42)); }, now, combat_.flares[n16777300.index].born);
      len = kern::formula([](Float k) -> Float { return (420.0 * (1.0 - (((1.0 - k) * (1.0 - k)) * (1.0 - k)))); }, k);
      pulse = kern::formula([](Float now) -> Float { return (0.75 + (0.25 * kern::math::sin((now * 18.0)))); }, now);
      a_23 = engine_math_math::Mat4_translate(vector_add(combat_.flares[n16777300.index].p, vector_make(0.0, 0.0, kern::formula([](Float len) -> Float { return (420.0 - (len * 0.5)); }, len)))).Result;
      a_24 = engine_math_math::Mat4_scale(vector_make(0.44, 0.44, len)).Result;
      a_22 = engine_math_math::Mat4_mul(a_23, a_24).Result;
      combat_art_cpp::CombatArt_put(combat_art_, a_22, CombatArt_Shape::glow_box, vector_scale(Vector{3.0, 0.3, 0.2}, pulse));
      a_26 = engine_math_math::Mat4_translate(vector_add(combat_.flares[n16777300.index].p, Vector{0.0, 0.0, 0.3})).Result;
      a_27 = engine_math_math::Mat4_scale(Vector{0.7, 0.7, 0.7}).Result;
      a_25 = engine_math_math::Mat4_mul(a_26, a_27).Result;
      combat_art_cpp::CombatArt_put(combat_art_, a_25, CombatArt_Shape::glow_sphere, Vector{4.0, 0.4, 0.2});
      t_2 = kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, combat_.flares[n16777300.index].born);
      until = 5.2;
      if (logic_and(math_compare<">=">(t_2, 0.0), math_compare<"<">(t_2, until))) {
        kRunLen = 170.0;
        pulse_2 = kern::formula([](Float t_2) -> Float { return (0.55 + (0.45 * kern::math::sin((t_2 * 9.0)))); }, t_2);
        fade = kern::formula([](Float t_2, Float until) -> Float { return (kern::math::min(1.0, kern::math::div(t_2, 0.4)) * kern::math::min(1.0, kern::math::div((until - t_2), 0.6))); }, t_2, until);
        arm_l = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, 0.785398).Result;
        arm_r = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, -0.785398).Result;
        for (auto it16777301 = flow_for(n16777301, 0, 8); it16777301.next();) {
          skip_2 = false;
          u_2 = kern::formula([](Float index, Float t_2) -> Float { return kern::math::mod((kern::math::div(index, 9.0) + (t_2 * 0.35)), 1.0); }, kern::to<Float>(n16777301.index), t_2);
          y = kern::formula([](Float y, Float kRunLen, Float u_2) -> Float { return ((y - kern::math::div(kRunLen, 2.0)) + (u_2 * kRunLen)); }, vector_break(combat_.flares[n16777300.index].p).y, kRunLen, u_2);
          a_28 = kern::formula([](Float pulse_2, Float fade, Float u_2, Float kPi) -> Float { return (((0.75 * pulse_2) * fade) * kern::math::sin((u_2 * kPi))); }, pulse_2, fade, u_2, kPi);
          if (math_compare<"<=">(a_28, 0.01)) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            c = vector_make(vector_break(combat_.flares[n16777300.index].p).x, y, kern::formula([](Float the_result) -> Float { return (the_result + 0.35); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(combat_.flares[n16777300.index].p).x, y).Result));
            a_31 = engine_math_math::Mat4_translate(vector_add(c, vector_scale(Vector{-1.1, 0.0, 0.0}, 2.0))).Result;
            a_32 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, arm_l, Vector{1.0, 1.0, 1.0}).Result;
            a_30 = engine_math_math::Mat4_mul(a_31, a_32).Result;
            a_33 = engine_math_math::Mat4_scale(Vector{6.2, 1.3, 0.05}).Result;
            a_29 = engine_math_math::Mat4_mul(a_30, a_33).Result;
            combat_art_cpp::CombatArt_put_2(combat_art_, a_29, CombatArt_Shape::fade_sphere, vector_scale(Vector{0.25, 0.66, 1.0}, 3.0), a_28);
            a_36 = engine_math_math::Mat4_translate(vector_add(c, vector_scale(Vector{1.1, 0.0, 0.0}, 2.0))).Result;
            a_37 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, arm_r, Vector{1.0, 1.0, 1.0}).Result;
            a_35 = engine_math_math::Mat4_mul(a_36, a_37).Result;
            a_38 = engine_math_math::Mat4_scale(Vector{6.2, 1.3, 0.05}).Result;
            a_34 = engine_math_math::Mat4_mul(a_35, a_38).Result;
            combat_art_cpp::CombatArt_put_2(combat_art_, a_34, CombatArt_Shape::fade_sphere, vector_scale(Vector{0.25, 0.66, 1.0}, 3.0), a_28);
          }
        }
      }
    }
    at = 0;
    kept = at;
    end = list_length<Game_VisualBlast>(visual_blasts_);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(draw_military_match(visual_blasts_[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n16777302, visual_blasts_[at]);
          visual_blasts_[kept] = n16777302.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(visual_blasts_, kept, Game_VisualBlast{Vector{0.0, 0.0, 0.0}, 0.0, 1.0});
    for (auto it16777303 = flow_for(n16777303, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_VisualBlast>(visual_blasts_))))); it16777303.next();) {
      t_3 = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float a) -> Float { return kern::math::div((now - a), 0.54); }, now, visual_blasts_[n16777303.index].born), 0.0, 1.0));
      e_2 = kern::formula([](Float t_3) -> Float { return (1.0 - (((1.0 - t_3) * (1.0 - t_3)) * (1.0 - t_3))); }, t_3);
      hot = kern::formula([](Float a, Float e_2) -> Float { return ((a * (0.5 + (4.5 * e_2))) * 0.9); }, visual_blasts_[n16777303.index].scale, e_2);
      ring = kern::formula([](Float a, Float e_2) -> Float { return (a * (0.7 + (7.0 * e_2))); }, visual_blasts_[n16777303.index].scale, e_2);
      a_40 = engine_math_math::Mat4_translate(visual_blasts_[n16777303.index].p).Result;
      a_41 = engine_math_math::Mat4_scale(vector_make(hot, hot, hot)).Result;
      a_39 = engine_math_math::Mat4_mul(a_40, a_41).Result;
      combat_art_cpp::CombatArt_put_2(combat_art_, a_39, CombatArt_Shape::fade_sphere, vector_scale(Vector{1.0, 0.33, 0.056}, 3.0), kern::formula([](Float t_3) -> Float { return (0.9 * (1.0 - t_3)); }, t_3));
      a_43 = engine_math_math::Mat4_translate(visual_blasts_[n16777303.index].p).Result;
      a_44 = engine_math_math::Mat4_scale(vector_make(ring, ring, ring)).Result;
      a_42 = engine_math_math::Mat4_mul(a_43, a_44).Result;
      combat_art_cpp::CombatArt_put_2(combat_art_, a_42, CombatArt_Shape::fade_ring, vector_scale(Vector{1.0, 0.64, 0.19}, 3.0), kern::formula([](Float t_3) -> Float { return (0.78 * kern::math::pow((1.0 - t_3), 1.6)); }, t_3));
    }
    for (auto it16777304 = flow_for(n16777304, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it16777304.next();) {
      skip_3 = false;
      if (logic_not(enum_equal<weapons_Id>(projectiles_[n16777304.index].id, weapons_Id::grenade_launcher))) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        r = 0.05265;
        a_45 = engine_math_math::Mat4_translate(projectiles_[n16777304.index].pos).Result;
        a_46 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(logic_select<Vector>(math_compare<">">(vector_length(vector_cross(Vector{0.0, 0.0, 1.0}, logic_select<Vector>(math_compare<">">(vector_length(projectiles_[n16777304.index].vel), 9.999999717180685e-10), vector_normalize(projectiles_[n16777304.index].vel), Vector{1.0, 0.0, 0.0}))), 9.999999717180685e-10), vector_normalize(vector_cross(Vector{0.0, 0.0, 1.0}, logic_select<Vector>(math_compare<">">(vector_length(projectiles_[n16777304.index].vel), 9.999999717180685e-10), vector_normalize(projectiles_[n16777304.index].vel), Vector{1.0, 0.0, 0.0}))), Vector{1.0, 0.0, 0.0}), kern::formula([](Float z) -> Float { return kern::math::acos((z < (-1.0) ? (-1.0) : (1.0 < z ? 1.0 : z))); }, vector_break(logic_select<Vector>(math_compare<">">(vector_length(projectiles_[n16777304.index].vel), 9.999999717180685e-10), vector_normalize(projectiles_[n16777304.index].vel), Vector{1.0, 0.0, 0.0})).z)).Result, Vector{1.0, 1.0, 1.0}).Result;
        m = engine_math_math::Mat4_mul(a_45, a_46).Result;
        a_48 = engine_math_math::Mat4_scale(vector_make(kern::formula([](Float r) -> Float { return (2.0 * r); }, r), kern::formula([](Float r) -> Float { return (2.0 * r); }, r), kern::formula([](Float r) -> Float { return (2.0 * r); }, r))).Result;
        a_47 = engine_math_math::Mat4_mul(m, a_48).Result;
        combat_art_cpp::CombatArt_put(combat_art_, a_47, CombatArt_Shape::sphere, Vector{0.006, 0.014, 0.009});
        the_band = kern::formula([](Float r) -> Float { return kern::math::div((r * 1.02), 0.8); }, r);
        tube = 0.0324;
        a_50 = engine_math_math::Mat4_scale(vector_make(the_band, the_band, kern::formula([](Float tube) -> Float { return (tube * 0.25); }, tube))).Result;
        a_49 = engine_math_math::Mat4_mul(m, a_50).Result;
        combat_art_cpp::CombatArt_put_2(combat_art_, a_49, CombatArt_Shape::fade_ring, vector_scale(Vector{1.0, 0.42, 0.06}, 3.0), 1.0);
        a_53 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(Vector{1.0, 0.0, 0.0}, 1.5708).Result, Vector{1.0, 1.0, 1.0}).Result;
        a_52 = engine_math_math::Mat4_mul(m, a_53).Result;
        a_54 = engine_math_math::Mat4_scale(vector_make(the_band, the_band, kern::formula([](Float tube) -> Float { return (tube * 0.2); }, tube))).Result;
        a_51 = engine_math_math::Mat4_mul(a_52, a_54).Result;
        combat_art_cpp::CombatArt_put_2(combat_art_, a_51, CombatArt_Shape::fade_ring, vector_scale(Vector{1.0, 0.42, 0.06}, 3.0), 1.0);
        a_56 = engine_math_math::Mat4_scale(vector_make(kern::formula([](Float r) -> Float { return ((2.0 * r) * 1.28); }, r), kern::formula([](Float r) -> Float { return ((2.0 * r) * 1.28); }, r), kern::formula([](Float r) -> Float { return ((2.0 * r) * 1.28); }, r))).Result;
        a_55 = engine_math_math::Mat4_mul(m, a_56).Result;
        combat_art_cpp::CombatArt_put_2(combat_art_, a_55, CombatArt_Shape::fade_sphere, vector_scale(Vector{0.0, 1.0, 0.13}, 3.2), 0.42);
      }
    }
    for (auto it16777305 = flow_for(n16777305, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_BounceFx>(bounce_fx_))))); it16777305.next();) {
      skip_4 = false;
      t_4 = kern::formula([](Float now, Float a) -> Float { return kern::math::div((now - a), 0.18); }, now, bounce_fx_[n16777305.index].born);
      if (logic_or(math_compare<"<">(t_4, 0.0), math_compare<">=">(t_4, 1.0))) {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        e_3 = kern::formula([](Float t_4) -> Float { return (1.0 - (((1.0 - t_4) * (1.0 - t_4)) * (1.0 - t_4))); }, t_4);
        n = logic_select<Vector>(math_compare<">">(vector_length(bounce_fx_[n16777305.index].n), 9.999999717180685e-10), vector_normalize(bounce_fx_[n16777305.index].n), Vector{0.0, 0.0, 1.0});
        ax = vector_cross(Vector{0.0, 0.0, 1.0}, n);
        al = vector_length(ax);
        q = logic_select<Quat>(math_compare<">">(al, 0.0001), engine_math_math::Quat_axis_angle(vector_scale(ax, kern::formula([](Float al) -> Float { return kern::math::div(1.0, al); }, al)), kern::formula([](Float z) -> Float { return kern::math::acos((z < (-1.0) ? (-1.0) : (1.0 < z ? 1.0 : z))); }, vector_break(n).z)).Result, logic_select<Quat>(math_compare<"<">(vector_break(n).z, 0.0), engine_math_math::Quat_axis_angle(Vector{1.0, 0.0, 0.0}, kPi).Result, struct_make<Quat>(0.0, 0.0, 0.0, 1.0)));
        ring_2 = kern::formula([](Float a, Float e_3) -> Float { return kern::math::div(((a * (0.75 + (3.1 * e_3))) * 0.049), 0.8); }, bounce_fx_[n16777305.index].strength, e_3);
        flash = kern::formula([](Float a, Float e_3) -> Float { return ((a * (1.0 + (0.7 * e_3))) * 0.04); }, bounce_fx_[n16777305.index].strength, e_3);
        a_59 = engine_math_math::Mat4_translate(bounce_fx_[n16777305.index].p).Result;
        a_60 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, q, Vector{1.0, 1.0, 1.0}).Result;
        a_58 = engine_math_math::Mat4_mul(a_59, a_60).Result;
        a_61 = engine_math_math::Mat4_scale(vector_make(ring_2, ring_2, 0.011249999999999998)).Result;
        a_57 = engine_math_math::Mat4_mul(a_58, a_61).Result;
        combat_art_cpp::CombatArt_put_2(combat_art_, a_57, CombatArt_Shape::fade_ring, vector_scale(Vector{1.0, 0.7, 0.28}, 3.0), kern::formula([](Float t_4) -> Float { return (0.72 * kern::math::pow((1.0 - t_4), 1.35)); }, t_4));
        a_63 = engine_math_math::Mat4_translate(bounce_fx_[n16777305.index].p).Result;
        a_64 = engine_math_math::Mat4_scale(vector_make(flash, flash, flash)).Result;
        a_62 = engine_math_math::Mat4_mul(a_63, a_64).Result;
        combat_art_cpp::CombatArt_put_2(combat_art_, a_62, CombatArt_Shape::fade_sphere, vector_scale(Vector{0.96, 1.0, 0.97}, 3.0), kern::formula([](Float t_4) -> Float { return (0.95 * kern::math::pow((1.0 - t_4), 2.2)); }, t_4));
      }
    }
    if (logic_and(logic_and(logic_and(world_placed_, enum_equal<Mode>(mode_, Mode::foot)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::rocket)), logic_not(player_dead_))) {
      n16777306 = rocket_laser_dot();
      dot = n16777306.Result;
      if (laser_valid_) {
        to_eye = vector_sub(cam_.eye, dot);
        dist = kern::formula([](Float a) -> Float { return kern::math::max(0.001, a); }, vector_length(to_eye));
        size = kern::formula([](Float dist, Float now) -> Float { return (kern::math::max(0.14, (dist * 0.016)) * (0.9 + (0.1 * kern::math::sin((now * 45.0))))); }, dist, now);
        n_2 = vector_scale(to_eye, kern::formula([](Float dist) -> Float { return kern::math::div(1.0, dist); }, dist));
        at_2 = vector_add(dot, vector_scale(n_2, kern::formula([](Float dist, Float size) -> Float { return (kern::math::min(0.15, (0.03 + (dist * 0.002))) + (size * 0.3)); }, dist, size)));
        ax_2 = vector_cross(Vector{0.0, 0.0, 1.0}, n_2);
        al_2 = vector_length(ax_2);
        q_2 = logic_select<Quat>(math_compare<">">(al_2, 0.0001), engine_math_math::Quat_axis_angle(vector_scale(ax_2, kern::formula([](Float al_2) -> Float { return kern::math::div(1.0, al_2); }, al_2)), kern::formula([](Float z) -> Float { return kern::math::acos((z < (-1.0) ? (-1.0) : (1.0 < z ? 1.0 : z))); }, vector_break(n_2).z)).Result, struct_make<Quat>(0.0, 0.0, 0.0, 1.0));
        a_66 = engine_math_math::Mat4_translate(at_2).Result;
        a_67 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, q_2, vector_make(size, size, kern::formula([](Float size) -> Float { return (size * 0.25); }, size))).Result;
        a_65 = engine_math_math::Mat4_mul(a_66, a_67).Result;
        combat_art_cpp::CombatArt_put(combat_art_, a_65, CombatArt_Shape::glow_sphere, Vector{5.0, 0.15, 0.08});
        a_69 = engine_math_math::Mat4_translate(vector_add(at_2, vector_scale(n_2, 0.01))).Result;
        a_70 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, q_2, vector_make(kern::formula([](Float size) -> Float { return (size * 2.3); }, size), kern::formula([](Float size) -> Float { return (size * 2.3); }, size), kern::formula([](Float size) -> Float { return (size * 0.3); }, size))).Result;
        a_68 = engine_math_math::Mat4_mul(a_69, a_70).Result;
        combat_art_cpp::CombatArt_put_2(combat_art_, a_68, CombatArt_Shape::fade_sphere, vector_scale(Vector{1.0, 0.05, 0.03}, 2.5), 0.42);
      }
    }
    for (auto it16777307 = flow_for(n16777307, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Tracer>(combat_.tracers))))); it16777307.next();) {
      if (math_compare<"<=">(now, combat_.tracers[n16777307.index].until)) {
        combat_art_cpp::CombatArt_beam(combat_art_, combat_.tracers[n16777307.index].a, combat_.tracers[n16777307.index].b, 0.06, combat_.tracers[n16777307.index].color);
      }
    }
    at_3 = 0;
    kept_2 = at_3;
    end_2 = list_length<Tracer>(combat_.tracers);
    while (loop(math_compare<"<">(kern::to<Float>(at_3), kern::to<Float>(end_2)))) {
      if (logic_not(draw_military_match_2(combat_.tracers[at_3], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept_2), kern::to<Float>(at_3))) {
          var_take(n16777308, combat_.tracers[at_3]);
          combat_.tracers[kept_2] = n16777308.value;
        }
        kept_2 += 1;
      }
      at_3 += 1;
    }
    list_resize(combat_.tracers, kept_2, Tracer{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{3.0, 2.4, 1.2}});
  }

  void mp_local_ready() {
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<MpLocal>(mp_)), 0.0))) {
      mp_ = list_make<MpLocal, "1">(MpLocal{MpArt{Object<kern::Renderer>{}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}, MpArt_Part{0, List<Float>{}, List<Float>{}}}, List<Float>{}, false, mp_PingBoard{List<mp_Ping>{}}, List<MpLocal_Tracer>{}, List<MpLocal_Flash>{}, List<MpLocal_Feed>{}, -1000000000000.0, -1000000000000.0, -1000000000000.0, 0.0, -1000000000000.0, false, false, 0, Vector{0.0, 0.0, 0.0}, 254, 0, 0, -1.0, false, Vector{0.0, 0.0, 0.0}, mp_VoiceCapture{List<Float>{}, List<U8_row>{}, 0.0}, false, false, false, 0, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, "", List<Vector>{}, false, -1000000000000.0, 0, "", 0, false, false});
    }
    if (logic_and(logic_not(mp_art::MpArt_ready(mp_[0].art).Result), object_valid<kern::Engine>(engine_))) {
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

  void net_world_rules() {
    U16 mask = 0;
    U16 mask_2 = 0;
    Float v = 0.0;
    mp_local_ready();
    if (logic_not(net_session::Session_active(net_).Result)) {
      if (mp_[0].world_saved) {
        for (auto it17825804 = flow_for(n17825804, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Opt>(kWorldRules))))); it17825804.next();) {
          settings_cpp::Settings_set(ui_.settings, ui_.settings_table, list_get<Opt>(kWorldRules, n17825804.index), mp_[0].world_own[n17825804.index]);
        }
        mp_[0].world_saved = false;
      }
      return;
    }
    if (net_session::Session_hosting(net_).Result) {
      mask = 0;
      for (auto it17825805 = flow_for(n17825805, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Opt>(kWorldRules))))); it17825805.next();) {
        if (settings::Settings_on(ui_.settings, list_get<Opt>(kWorldRules, n17825805.index)).Result) {
          mask = kern::to<Int>(kern::formula([](Float mask, Float index) -> Float { return kern::math::band(kern::math::bor(mask, kern::math::band(kern::math::wrap32(kern::math::shl(1.0, index)), 65535.0)), 65535.0); }, kern::to<Float>(mask), kern::to<Float>(n17825805.index)));
        }
      }
      net_session_cpp::Session_set_world_rules(net_, mask);
      return;
    }
    mask_2 = 0;
    n17825806 = net_session::Session_world_rules(net_, mask_2);
    if (logic_not(n17825806.Result)) {
      return;
    }
    if (logic_not(mp_[0].world_saved)) {
      for (auto it17825807 = flow_for(n17825807, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Opt>(kWorldRules))))); it17825807.next();) {
        mp_[0].world_own[n17825807.index] = settings::Settings_get(ui_.settings, list_get<Opt>(kWorldRules, n17825807.index)).Result;
      }
      mp_[0].world_saved = true;
    }
    for (auto it17825808 = flow_for(n17825808, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Opt>(kWorldRules))))); it17825808.next();) {
      v = kern::formula([](Float mask_2, Float index) -> Float { return ((kern::math::band(kern::math::wrap32(kern::math::shr(mask_2, index)), 1.0) != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(mask_2), kern::to<Float>(n17825808.index));
      if (math_compare<">">(kern::formula([](Float the_result, Float v) -> Float { return (the_result != v); }, settings::Settings_get(ui_.settings, list_get<Opt>(kWorldRules, n17825808.index)).Result, v), 0.5)) {
        settings_cpp::Settings_set(ui_.settings, ui_.settings_table, list_get<Opt>(kWorldRules, n17825808.index), v);
      }
    }
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
    for (auto it17825809 = flow_for(n17825809, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Opt>(list))))); it17825809.next();) {
      w = list[n17825809.index];
      key = struct_break<OptSpec>(settings_cpp::opt_spec(ui_.settings_table, w).Result).key;
      o = text_append(o, logic_select<Text>(first, "\"", ",\""), "");
      for (auto it17825810 = flow_for(n17825810, 6, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(text_length(key))))); it17825810.next();) {
        o = text_append(o, text_of_code(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(key, n17825810.index))))), "");
      }
      o = text_append(o, logic_select<Text>(settings::Settings_on(ui_.settings, w).Result, "\":true", "\":false"), "");
      first = false;
    }
    return {text_append(o, "}", "")};
  }

  struct my_stateResult { net_PlayerState Result = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}; };
  my_stateResult my_state() {
    net_PlayerState s = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255};
    U32 a = 0;
    BodyState b = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float camera_yaw = 0.0;
    Float camera_yaw_2 = 0.0;
    Float camera_pitch = 0.0;
    Bool xr = false;
    Bool xr_2 = false;
    Bool both = false;
    Vector from = Vector{0.0, 0.0, 0.0};
    Vector dir = Vector{0.0, 0.0, 0.0};
    Bool both_2 = false;
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector xr_forward = Vector{0.0, 0.0, 0.0};
    game_Hud h = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
    game_Hud hud = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
    Float tnow = 0.0;
    Float sim_time = 0.0;
    Vector feet = Vector{0.0, 0.0, 0.0};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool xr_3 = false;
    Bool xr_4 = false;
    Bool both_3 = false;
    Mat4 hm = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool any = false;
    Vector d = Vector{0.0, 0.0, 0.0};
    U16 f_2 = 0;
    BodyState d_2 = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    BodyState j = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector rp = Vector{0.0, 0.0, 0.0};
    Quat rq = Quat{0.0, 0.0, 0.0, 1.0};
    s = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255};
    s.mode = enum_from_index<net_PlayerMode>(enum_index<Mode>(mode_)).value;
    a = kern::to<Int>(kern::formula([](Float a, Float car_index_, Float a_2, Float a_3, Float jet_actor_, Float a_4, Float drone_actor_, Float a_5, Float chute_actor_, Float soldier_) -> Float { return (((a != 0.0) && (car_index_ >= 0.0)) ? a_2 : ((a_3 != 0.0) ? jet_actor_ : ((a_4 != 0.0) ? drone_actor_ : ((a_5 != 0.0) ? chute_actor_ : soldier_)))); }, kern::to<Float>(enum_equal<Mode>(mode_, Mode::car)), kern::to<Float>(car_index_), kern::to<Float>(cars_[car_index_].actor), kern::to<Float>(enum_equal<Mode>(mode_, Mode::jet)), kern::to<Float>(jet_actor_), kern::to<Float>(enum_equal<Mode>(mode_, Mode::drone)), kern::to<Float>(drone_actor_), kern::to<Float>(enum_equal<Mode>(mode_, Mode::parachute)), kern::to<Float>(chute_actor_), kern::to<Float>(soldier_)));
    n17825811 = state_of(a);
    b = n17825811.Result;
    s.position = b.position;
    s.velocity = b.velocity;
    s.rotation = b.rotation;
    n17825812 = matrix_of(soldier_);
    a_2 = n17825812.Result;
    s.avatar = vector_sub(engine_math_math::Mat4_translation(a_2).Result, Vector{0.0, 0.0, 0.73});
    camera_yaw = engine_game_world_camera_yaw(world_);
    s.avatar_yaw = kern::formula([](Float camera_yaw, Float kDeg) -> Float { return (camera_yaw * kDeg); }, camera_yaw, kDeg);
    camera_yaw_2 = engine_game_world_camera_yaw(world_);
    s.yaw = kern::formula([](Float camera_yaw_2, Float kDeg) -> Float { return (camera_yaw_2 * kDeg); }, camera_yaw_2, kDeg);
    camera_pitch = engine_game_world_camera_pitch(world_);
    s.pitch = kern::formula([](Float camera_pitch, Float kDeg) -> Float { return (camera_pitch * kDeg); }, camera_pitch, kDeg);
    both = object_valid<kern::Engine>(engine_);
    if (both) {
      xr_2 = engine_engine_xr(engine_);
      both = xr_2;
    }
    if (both) {
      from = Vector{0.0, 0.0, 0.0};
      dir = Vector{0.0, 0.0, 0.0};
      both_2 = enum_equal<Mode>(mode_, Mode::foot);
      if (both_2) {
        n17825813 = vr_hand_ray(from, dir);
        both_2 = n17825813.Result;
      }
      if (both_2) {
        s.yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(dir).y, vector_break(dir).x);
        s.pitch = kern::formula([](Float z) -> Float { return (-kern::math::asin((z < (-1.0) ? (-1.0) : (1.0 < z ? 1.0 : z)))); }, vector_break(dir).z);
      } else {
        xr_forward = engine_engine_xr_forward(engine_);
        f = xr_forward;
        s.pitch = kern::formula([](Float z) -> Float { return (-kern::math::asin((z < (-1.0) ? (-1.0) : (1.0 < z ? 1.0 : z)))); }, vector_break(f).z);
      }
    }
    s.speed = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(b.velocity).x, vector_break(b.velocity).y);
    hud = engine_game_world_hud(world_);
    h = hud;
    s.health = kern::formula([](Float health, Float a) -> Float { return ((health >= 0.0) ? a : 100.0); }, h.health, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float health, Float max_health) -> Float { return (kern::math::div(health, kern::math::max(1.0, max_health)) * 100.0); }, h.health, h.max_health), 0.0, 100.0));
    n17825814 = wanted_cpp::stars();
    s.stars = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::band(the_result, 255.0); }, kern::to<Float>(n17825814.Result)));
    s.weapon = enum_index<weapons_Id>(foot_weapon_);
    s.hands = 0;
    for (auto it17825815 = flow_for(n17825815, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<I8>(s.hand))))); it17825815.next();) {
      s.hand[n17825815.index] = 0;
    }
    if (logic_and(enum_equal<Mode>(mode_, Mode::foot), logic_not(player_dead_))) {
      sim_time = engine_game_world_sim_time(world_);
      tnow = sim_time;
      n17825816 = matrix_of(soldier_);
      a_3 = n17825816.Result;
      feet = vector_sub(engine_math_math::Mat4_translation(a_3).Result, Vector{0.0, 0.0, 0.73});
      both_3 = object_valid<kern::Engine>(engine_);
      if (both_3) {
        xr_4 = engine_engine_xr(engine_);
        both_3 = xr_4;
      }
      if (logic_and(both_3, vr_)) {
        hm = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
        any = false;
        for (auto it17825817 = flow_for(n17825817, 0, 1); it17825817.next();) {
          n17825818 = vr_hand_matrix(hm, n17825817.index);
          if (n17825818.Result) {
            d = vector_sub(engine_math_math::Mat4_translation(hm).Result, feet);
            s.hand[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index * 3.0) + 0.0); }, kern::to<Float>(n17825817.index)))] = kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(a), 255.0), 128.0) - 128.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x) -> Float { return (x * 100.0); }, vector_break(d).x), -127.0, 127.0)));
            s.hand[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index * 3.0) + 1.0); }, kern::to<Float>(n17825817.index)))] = kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(a), 255.0), 128.0) - 128.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float y) -> Float { return (y * 100.0); }, vector_break(d).y), -127.0, 127.0)));
            s.hand[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index * 3.0) + 2.0); }, kern::to<Float>(n17825817.index)))] = kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(a), 255.0), 128.0) - 128.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float z) -> Float { return ((z * 50.0) - 127.0); }, vector_break(d).z), -127.0, 127.0)));
            any = true;
          }
        }
        if (any) {
          s.hands = kern::to<Int>(kern::formula([](Float hands) -> Float { return kern::math::bor(hands, 1.0); }, kern::to<Float>(s.hands)));
        }
      } else if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
        if (math_compare<"<">(kern::formula([](Float tnow, Float a) -> Float { return (tnow - a); }, tnow, gaps_[0].fist_last[0]), 0.22)) {
          s.hands = kern::to<Int>(kern::formula([](Float hands) -> Float { return kern::math::bor(hands, 2.0); }, kern::to<Float>(s.hands)));
        }
        if (math_compare<"<">(kern::formula([](Float tnow, Float a) -> Float { return (tnow - a); }, tnow, gaps_[0].fist_last[1]), 0.22)) {
          s.hands = kern::to<Int>(kern::formula([](Float hands) -> Float { return kern::math::bor(hands, 4.0); }, kern::to<Float>(s.hands)));
        }
        if (logic_and(enum_equal<weapons_Id>(foot_weapon_, weapons_Id::knife), math_compare<"<">(kern::formula([](Float tnow, Float vm_fire_at) -> Float { return (tnow - vm_fire_at); }, tnow, look_.vm_fire_at), 0.3))) {
          s.hands = kern::to<Int>(kern::formula([](Float hands) -> Float { return kern::math::bor(hands, 8.0); }, kern::to<Float>(s.hands)));
        }
      }
    }
    s.car = 255;
    if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0))) {
      if (net_session::Session_hosting(net_).Result) {
        s.car = kern::to<Int>(kern::formula([](Float car_index_) -> Float { return kern::math::band(kern::math::min(car_index_, 254.0), 255.0); }, kern::to<Float>(car_index_)));
      } else if (logic_and(logic_and(puppet_world_, math_compare<">">(kern::formula([](Float car_index_, Float a) -> Float { return ((car_index_ < 0.0) != (a < 0.0) ? (a < 0.0) : (car_index_ < a)); }, kern::to<Float>(car_index_), kern::to<Float>(list_length<I32>(car_net_))), 0.5)), math_compare<">=">(kern::to<Float>(car_net_[car_index_]), 0.0))) {
        s.car = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(car_net_[car_index_])));
      }
    }
    s.day = look_.tod;
    s.thrust = kern::formula([](Float the_result, Float thrust_max) -> Float { return kern::math::div(the_result, kern::math::max(1.0, thrust_max)); }, jet::Jet_thrust(jet_).Result, jet_.s_.thrust_max);
    f_2 = 0;
    if (player_dead_) {
      f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead)))));
    }
    if (drone_.tele_.armed) {
      f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kArmed)))));
    }
    if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0))) {
      if (math_compare<">">(cars_[car_index_].car[0].s_.radius, 0.4)) {
        f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kBus)))));
      }
      if (cars_[car_index_].police) {
        f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPoliceCar)))));
      }
      s.color = kern::to<Int>(kern::formula([](Float car_index_) -> Float { return kern::math::band(kern::math::mod(car_index_, 8.0), 255.0); }, kern::to<Float>(car_index_)));
    }
    if (jet::Jet_gear_down(jet_).Result) {
      f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kGearDown)))));
    }
    if (logic_and(drone_out_, logic_not(enum_equal<Mode>(mode_, Mode::drone)))) {
      f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDroneOut)))));
      n17825819 = state_of(drone_actor_);
      d_2 = n17825819.Result;
      s.drone_pos = d_2.position;
      s.drone_rot = d_2.rotation;
    }
    if (logic_and(jet_placed_, logic_not(enum_equal<Mode>(mode_, Mode::jet)))) {
      f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetOut)))));
      n17825820 = state_of(jet_actor_);
      j = n17825820.Result;
      s.jet_pos = j.position;
      s.jet_rot = j.rotation;
    }
    if (logic_and(logic_and(math_compare<">">(kern::to<Float>(list_length<MpLocal>(mp_)), 0.0), mp_[0].mic), logic_not(mp_[0].muted))) {
      f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kTalking)))));
    }
    if (logic_and(ui_.jetpack.thrusting, enum_equal<Mode>(mode_, Mode::foot))) {
      f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetpack)))));
    }
    rp = Vector{0.0, 0.0, 0.0};
    if (logic_and(gaps_riding().Result, logic_not(logic_and(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0), gaps_[0].ride_car)))) {
      rq = Quat{0.0, 0.0, 0.0, 1.0};
      n17825821 = gaps_ride_pose(rp, rq);
      if (n17825821.Result) {
        s.position = rp;
        s.avatar = rp;
        s.rotation = rq;
        s.velocity = Vector{0.0, 0.0, 0.0};
        s.speed = 0.0;
      }
      f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))));
    }
    if (gaps_ride_car().Result) {
      f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated)))));
    }
    s.flags = f_2;
    return {s};
  }

  struct update_net_matchResult { Bool Result = false; };
  KERN_PURE update_net_matchResult update_net_match(const MpLocal_Feed& f, Float now = 0.0) {
    return {math_compare<">">(kern::formula([](Float now, Float at) -> Float { return (now - at); }, now, f.at), kern::formula([](Float kFeedMs) -> Float { return (kFeedMs + 450.0); }, kFeedMs))};
  }

  struct update_net_match_2Result { Bool Result = false; };
  KERN_PURE update_net_match_2Result update_net_match_2(const MpLocal_Tracer& t) {
    return {math_compare<"<=">(t.left, 0.0)};
  }

  struct update_net_match_3Result { Bool Result = false; };
  KERN_PURE update_net_match_3Result update_net_match_3(const MpLocal_Flash& f, Float now = 0.0) {
    return {math_compare<">">(kern::formula([](Float now, Float born) -> Float { return (now - born); }, now, f.born), f.life)};
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
    n17825829 = focus_position();
    p3 = n17825829.Result;
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
    n17825830 = road_graph_cpp::RoadGraph_path(roads_, path, me, mp_[0].dest);
    if (logic_and(logic_and(n17825830.Result, math_compare<">=">(kern::to<Float>(list_length<Vector>(path)), 2.0)), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(path[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(path))))]).x, vector_break(mp_[0].dest).x, vector_break(path[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(path))))]).y, vector_break(mp_[0].dest).y), 60.0))) {
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
    mp_[0].fetch_id = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(mp_[0].fetch_id)));
    b = text_format<"{\"id\":%u,\"from\":[%.6f,%.6f],\"to\":[%.6f,%.6f],\"profile\":\"%s\"}">(199, mp_[0].fetch_id, lon, lat, mp_[0].dest_lon, mp_[0].dest_lat, logic_select<Text>(enum_equal<Mode>(mode_, Mode::foot), "foot", "car"));
    mp_[0].fetch_json = b;
    if (math_compare<"==">(kern::to<Float>(list_length<Vector>(mp_[0].route)), 0.0)) {
      mp_[0].route = list_make<Vector, "2">(me, mp_[0].dest);
    }
  }

  void mp_input(Object<kern::Engine> e = Object<kern::Engine>{}, Float dt = 0.0) {
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

  void voice_capture(List<Float> samples = List<Float>{}, U32 rate = 0) {
    mp_local_ready();
    if (logic_and(mp_[0].mic, logic_not(mp_[0].muted))) {
      mp_rules_cpp::VoiceCapture_push(mp_[0].capture, samples, rate);
    }
  }

  void mix_voice(List<Float>& out, U32 frames = 0) {
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
    for (auto it17825831 = flow_for(n17825831, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825831.next();) {
      sample_rate = engine_mixer_sample_rate(mixer_);
      n17825832 = mp_rules_cpp::VoiceStream_render(net_.remotes_[n17825831.index].voice, out, frames, sample_rate);
    }
  }

  struct remote_on_segment_zonesResult { Bool Result = false; };
  remote_on_segment_zonesResult remote_on_segment_zones(Float& t_hit, mp_Zone& z, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float speed = 0.0) {
    Float t = 0.0;
    BodyPart part = BodyPart::none;
    t = t_hit;
    n17825833 = person_hit::hit(t, feet, blueprint_call::decimal(yaw).Result, blueprint_call::decimal(speed).Result, from, seg, 1.16, 0.0);
    part = n17825833.Result;
    t_hit = t;
    z = logic_select<mp_Zone>(enum_equal<BodyPart>(part, BodyPart::head), mp_Zone::head, logic_select<mp_Zone>(enum_equal<BodyPart>(part, BodyPart::legs), mp_Zone::legs, mp_Zone::torso));
    return {logic_not(enum_equal<BodyPart>(part, BodyPart::none))};
  }

  struct remote_on_segmentResult { I32 Result = 0; };
  /// The mate closest along a segment (his body as it is drawn, a sphere around his vehicle): -1 for none; `t_out` the share of the segment where it touches him. Rounds, blows, grenades and rockets all ask this - a mate's body stops them on every machine as a passer-by's does.
  remote_on_segmentResult remote_on_segment(Float& t_out, mp_Zone& zone_out, Vector from = Vector{0.0, 0.0, 0.0}, Vector to = Vector{0.0, 0.0, 0.0}) {
    Vector seg = Vector{0.0, 0.0, 0.0};
    Float l2 = 0.0;
    I32 best = 0;
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
    for (auto it17825834 = flow_for(n17825834, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825834.next();) {
      skip = false;
      if (math_compare<">">(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return (kern::math::band(a, kern::math::bor(a_2, a_3)) != 0.0); }, kern::to<Float>(net_.remotes_[n17825834.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead))), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_and(gaps_riding().Result, math_compare<"==">(kern::to<Float>(net_.remotes_[n17825834.index].state.slot), kern::to<Float>(gaps_[0].ride_owner)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (enum_equal<net_PlayerMode>(net_.remotes_[n17825834.index].state.mode, net_PlayerMode::foot)) {
            t = 2.0;
            z = mp_Zone::torso;
            feet = logic_select<Vector>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825834.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated)))), 0.5), vector_sub(net_.remotes_[n17825834.index].person_shown, vector_make(0.0, 0.0, kSeatDrop)), net_.remotes_[n17825834.index].person_shown);
            n17825835 = remote_on_segment_zones(t, z, from, seg, feet, net_.remotes_[n17825834.index].state.avatar_yaw, net_.remotes_[n17825834.index].state.speed);
            if (logic_and(n17825835.Result, math_compare<"<">(t, best_t))) {
              best_t = t;
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825834.index)));
              best_zone = z;
            }
            skip = true;
          }
          if (logic_not(skip)) {
            if (enum_equal<net_PlayerMode>(net_.remotes_[n17825834.index].state.mode, net_PlayerMode::drone)) {
              t_2 = 2.0;
              z_2 = mp_Zone::torso;
              n17825836 = remote_on_segment_zones(t_2, z_2, from, seg, net_.remotes_[n17825834.index].person_shown, net_.remotes_[n17825834.index].state.avatar_yaw, 0.0);
              if (logic_and(n17825836.Result, math_compare<"<">(t_2, best_t))) {
                best_t = t_2;
                best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825834.index)));
                best_zone = z_2;
              }
            }
            if (enum_equal<net_PlayerMode>(net_.remotes_[n17825834.index].state.mode, net_PlayerMode::parachute)) {
              t_3 = 2.0;
              z_3 = mp_Zone::torso;
              n17825837 = remote_on_segment_zones(t_3, z_3, from, seg, vector_sub(net_.remotes_[n17825834.index].shown, Vector{0.0, 0.0, 1.75}), net_.remotes_[n17825834.index].state.yaw, 0.0);
              if (logic_and(n17825837.Result, math_compare<"<">(t_3, best_t))) {
                best_t = t_3;
                best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825834.index)));
                best_zone = z_3;
              }
            }
            person = enum_equal<net_PlayerMode>(net_.remotes_[n17825834.index].state.mode, net_PlayerMode::foot);
            c = logic_select<Vector>(person, vector_add(net_.remotes_[n17825834.index].person_shown, vector_make(0.0, 0.0, kern::formula([](Float a, Float a_2, Float kSeatDrop) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? (0.73 - kSeatDrop) : 0.73); }, kern::to<Float>(net_.remotes_[n17825834.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated))), kSeatDrop))), net_.remotes_[n17825834.index].shown);
            radius = 0.522;
            switch (flow_switch_enum<net_PlayerMode>(net_.remotes_[n17825834.index].state.mode)) {
              case net_PlayerMode::foot: {
                c = vector_add(c, Vector{0.0, 0.0, 0.2});
              } break;
              case net_PlayerMode::drone: {
                radius = 0.3;
              } break;
              case net_PlayerMode::car: {
                radius = kern::formula([](Float a, Float a_2) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? 3.2 : 2.0); }, kern::to<Float>(net_.remotes_[n17825834.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kBus))));
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
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825834.index)));
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

  /// Screen point of a world point (vs_multiplayer.mjs renderMarker: off screen at the edge with an arrow; phones keep it off the sticks).
  void Game_mp_hud_json_marker(Float& sx, Float& sy, Bool& on, Float& angle, Float w = 0.0, Float h = 0.0, Vector p = Vector{0.0, 0.0, 0.0}) {
    Float nx = 0.0;
    Float ny = 0.0;
    Float depth = 0.0;
    Float margin = 0.0;
    Float hw = 0.0;
    Float hh = 0.0;
    Float x = 0.0;
    Float y = 0.0;
    Float k = 0.0;
    Vector q = Vector{0.0, 0.0, 0.0};
    nx = 0.0;
    ny = 0.0;
    depth = 0.0;
    n17825841 = project_screen(nx, ny, depth, p);
    on = n17825841.Result;
    margin = kern::formula([](Float w) -> Float { return kern::math::min(86.0, (w * 0.1)); }, w);
    hw = kern::formula([](Float w, Float margin) -> Float { return kern::math::max(1.0, (kern::math::div(w, 2.0) - margin)); }, w, margin);
    hh = kern::formula([](Float h, Float margin) -> Float { return kern::math::max(1.0, (kern::math::div(h, 2.0) - margin)); }, h, margin);
    x = kern::formula([](Float nx, Float w) -> Float { return kern::math::div((nx * w), 2.0); }, nx, w);
    y = kern::formula([](Float ny, Float h) -> Float { return kern::math::div(((-ny) * h), 2.0); }, ny, h);
    if (logic_not(on)) {
      k = kern::formula([](Float hw, Float x, Float hh, Float y) -> Float { return kern::math::min(kern::math::div(hw, kern::math::max(0.001, kern::math::abs(x))), kern::math::div(hh, kern::math::max(0.001, kern::math::abs(y)))); }, hw, x, hh, y);
      x = kern::formula([](Float x, Float k) -> Float { return (x * k); }, x, k);
      y = kern::formula([](Float y, Float k) -> Float { return (y * k); }, y, k);
    }
    n17825842 = mp_rules_cpp::safe_mobile_marker_point(vector_make(kern::formula([](Float w, Float x) -> Float { return (kern::math::div(w, 2.0) + x); }, w, x), kern::formula([](Float h, Float y) -> Float { return (kern::math::div(h, 2.0) + y); }, h, y), 0.0), w, h, touch_mode_);
    q = n17825842.Result;
    sx = vector_break(q).x;
    sy = vector_break(q).y;
    angle = kern::formula([](Float y, Float x, Float kDeg) -> Float { return (kern::math::div(kern::math::atan2(y, x), kDeg) - 90.0); }, y, x, kDeg);
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

  void projsync_blast(U8 owner = 0, Vector at = Vector{0.0, 0.0, 0.0}) {
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

  struct projsync_update_matchResult { Bool Result = false; };
  KERN_PURE projsync_update_matchResult projsync_update_match(const Int_Int_pair& projsync_0_sent_n18874384_index, const Game_projsync_update_Rec& r) {
    return {math_compare<"==">(kern::to<Float>(r.id), kern::to<Float>(projsync_0_sent_n18874384_index.first))};
  }

  void projsync_update(Float dt = 0.0) {
    Float now = 0.0;
    Float sim_time = 0.0;
    Float now_ms = 0.0;
    List<Game_projsync_update_Rec> recs = List<Game_projsync_update_Rec>{};
    List<Int_Int_pair> next = List<Int_Int_pair>{};
    Bool skip = false;
    Int at = 0;
    Int end = 0;
    Bool go = false;
    List<U8> o = List<U8>{};
    Int i = 0;
    Bool skip_2 = false;
    Game_projsync_update_Rec r = Game_projsync_update_Rec{0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}};
    List<net_Session_Blob> list = List<net_Session_Blob>{};
    wire_Reader in = wire_Reader{0, true};
    U32 t = 0;
    U8 n = 0;
    U8 k = 0;
    Bool stop_2 = false;
    Bool skip_3 = false;
    U16 id = 0;
    U8 kind = 0;
    U8 flags = 0;
    U32 key = 0;
    Int it = 0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Vector v = Vector{0.0, 0.0, 0.0};
    Int key_2 = 0;
    Int_Bool_pair v_2 = Int_Bool_pair{0, false};
    Projectile q = Projectile{weapons_Id::grenade_launcher, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 12.0, true, false, false, true, 0, false, 0.0, -1.0, physics_Body{Int64{0x0}}, 0, false};
    Bool skip_4 = false;
    Int it_2 = 0;
    mp_PoseSample s = mp_PoseSample{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, false, false, false};
    Int it_3 = 0;
    Int at_2 = 0;
    projsync_ready();
    if (logic_not(net_session::Session_active(net_).Result)) {
      if (logic_not(math_compare<"==">(kern::to<Float>(map_length<Int, ProjSync_Ghost>(projsync_[0].ghosts)), 0.0))) {
        for (auto it18874380 = flow_for(n18874380, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it18874380.next();) {
          if (projectiles_[n18874380.index].ghost) {
            projectiles_[n18874380.index].alive = false;
          }
        }
        map_clear(projsync_[0].ghosts);
      }
      n18874381 = net_session::Session_take_proj(net_);
      return;
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    now_ms = kern::formula([](Float now) -> Float { return (now * 1000.0); }, now);
    projsync_[0].clock = kern::formula([](Float a, Float dt) -> Float { return (a + dt); }, projsync_[0].clock, dt);
    if (math_compare<">=">(projsync_[0].clock, game_projsync_kSendEvery)) {
      projsync_[0].clock = 0.0;
      recs = List<Game_projsync_update_Rec>{};
      for (auto it18874382 = flow_for(n18874382, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it18874382.next();) {
        if (logic_and(logic_and(logic_and(projectiles_[n18874382.index].alive, logic_not(projectiles_[n18874382.index].ghost)), projectiles_[n18874382.index].by_player), math_compare<"!=">(kern::to<Float>(projectiles_[n18874382.index].net), 0.0))) {
          list_add(recs, struct_make<Game_projsync_update_Rec>(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 65535.0); }, kern::to<Float>(projectiles_[n18874382.index].net))), enum_index<weapons_Id>(projectiles_[n18874382.index].id), 0, projectiles_[n18874382.index].pos, projectiles_[n18874382.index].vel));
        }
      }
      for (auto it18874383 = flow_for(n18874383, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))); it18874383.next();) {
        list_add(recs, struct_make<Game_projsync_update_Rec>(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(kern::math::bor(32768.0, kern::math::band(kern::math::wrap32(kern::math::round((a * 1000.0))), 32767.0)), 65535.0); }, guided_[n18874383.index].m.born)), enum_index<weapons_Id>(weapons_Id::drone_guided), kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(((a != 0.0) ? 2.0 : 0.0), 255.0); }, kern::to<Float>(guided_[n18874383.index].m.motor))), guided_[n18874383.index].m.p, guided_[n18874383.index].m.v));
      }
      next = List<Int_Int_pair>{};
      for (auto it18874384 = flow_for(n18874384, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_Int_pair>(projsync_[0].sent))))); it18874384.next();) {
        skip = false;
        at = 0;
        end = list_length<Game_projsync_update_Rec>(recs);
        go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
        while (loop(go)) {
          if (projsync_update_match(projsync_[0].sent[n18874384.index], recs[at]).Result) {
            go = false;
          } else {
            at += 1;
            go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
          }
        }
        if (logic_not(math_compare<"==">(kern::to<Float>(at), kern::to<Float>(end)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (math_compare<">=">(kern::to<Float>(projsync_[0].sent[n18874384.index].second), 3.0)) {
            skip = true;
          }
          if (logic_not(skip)) {
            list_add(recs, struct_make<Game_projsync_update_Rec>(projsync_[0].sent[n18874384.index].first, 0, kGone, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}));
            list_add(next, struct_make<Int_Int_pair>(projsync_[0].sent[n18874384.index].first, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band((a + 1.0), 255.0); }, kern::to<Float>(projsync_[0].sent[n18874384.index].second)))));
          }
        }
      }
      for (auto it18874385 = flow_for(n18874385, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_projsync_update_Rec>(recs))))); it18874385.next();) {
        if (logic_not(math_compare<">">(kern::formula([](Float a, Float kGone) -> Float { return (kern::math::band(a, kGone) != 0.0); }, kern::to<Float>(recs[n18874385.index].flags), kern::to<Float>(kGone)), 0.5))) {
          list_add(next, struct_make<Int_Int_pair>(recs[n18874385.index].id, 0));
        }
      }
      projsync_[0].sent = next;
      if (logic_not(math_compare<"==">(kern::to<Float>(list_length<Game_projsync_update_Rec>(recs)), 0.0))) {
        o = List<U8>{};
        wire::put_u32(o, kern::to<Int>(kern::formula([](Float now_ms) -> Float { return kern::math::wrap32(kern::math::round(now_ms)); }, now_ms)));
        wire::put_u8(o, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(kern::math::min(a, 48.0), 255.0); }, kern::to<Float>(list_length<Game_projsync_update_Rec>(recs)))));
        i = 0;
        while (loop(logic_and(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Game_projsync_update_Rec>(recs))), 0.5), math_compare<">">(kern::formula([](Float i) -> Float { return ((i < 0.0) != (48.0 < 0.0) ? (48.0 < 0.0) : (i < 48.0)); }, kern::to<Float>(i)), 0.5)))) {
          skip_2 = false;
          r = recs[i];
          wire::put_u16(o, r.id);
          wire::put_u8(o, r.kind);
          wire::put_u8(o, r.flags);
          if (math_compare<">">(kern::formula([](Float flags, Float kGone) -> Float { return (kern::math::band(flags, kGone) != 0.0); }, kern::to<Float>(r.flags), kern::to<Float>(kGone)), 0.5)) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            wire::put_f32(o, vector_break(r.p).x);
            wire::put_f32(o, vector_break(r.p).y);
            wire::put_f32(o, vector_break(r.p).z);
            wire::put_i16(o, game_projsync_cpp::dms(vector_break(r.v).x).Result);
            wire::put_i16(o, game_projsync_cpp::dms(vector_break(r.v).y).Result);
            wire::put_i16(o, game_projsync_cpp::dms(vector_break(r.v).z).Result);
          }
          i += 1;
        }
        net_session_cpp::Session_send_proj(net_, o);
        projsync_[0].sent_msgs = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(projsync_[0].sent_msgs)));
      }
    }
    n18874386 = net_session::Session_take_proj(net_);
    list = n18874386.Result;
    for (auto it18874387 = flow_for(n18874387, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Session_Blob>(list))))); it18874387.next();) {
      in = wire_Reader{0, true};
      n18874388 = wire::Reader_get_u32(in, list[n18874387.index].data);
      t = n18874388.Result;
      n18874389 = wire::Reader_get_u8(in, list[n18874387.index].data);
      n = n18874389.Result;
      k = 0;
      stop_2 = false;
      while (loop(logic_and(logic_not(stop_2), logic_and(math_compare<"<">(kern::to<Float>(k), kern::to<Float>(n)), in.ok)))) {
        skip_3 = false;
        n18874390 = wire::Reader_get_u16(in, list[n18874387.index].data);
        id = n18874390.Result;
        n18874391 = wire::Reader_get_u8(in, list[n18874387.index].data);
        kind = n18874391.Result;
        n18874392 = wire::Reader_get_u8(in, list[n18874387.index].data);
        flags = n18874392.Result;
        key = kern::to<Int>(kern::formula([](Float a, Float id) -> Float { return kern::math::bor(kern::math::wrap32(kern::math::shl(a, 16.0)), id); }, kern::to<Float>(list[n18874387.index].from), kern::to<Float>(id)));
        if (math_compare<">">(kern::formula([](Float flags, Float kGone) -> Float { return (kern::math::band(flags, kGone) != 0.0); }, kern::to<Float>(flags), kern::to<Float>(kGone)), 0.5)) {
          it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, ProjSync_Ghost>(projsync_[0].ghosts, key).index), kern::to<Float>(map_length<Int, ProjSync_Ghost>(projsync_[0].ghosts))));
          if (logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int, ProjSync_Ghost>(projsync_[0].ghosts))))) {
            projsync_[0].ghosts[it].gone = true;
          }
          skip_3 = true;
        }
        if (logic_not(logic_or(stop_2, skip_3))) {
          p = Vector{0.0, 0.0, 0.0};
          v = Vector{0.0, 0.0, 0.0};
          n18874393 = wire::Reader_get_f32(in, list[n18874387.index].data);
          p = vector_make(n18874393.Result, vector_break(p).y, vector_break(p).z);
          n18874394 = wire::Reader_get_f32(in, list[n18874387.index].data);
          p = vector_make(vector_break(p).x, n18874394.Result, vector_break(p).z);
          n18874395 = wire::Reader_get_f32(in, list[n18874387.index].data);
          p = vector_make(vector_break(p).x, vector_break(p).y, n18874395.Result);
          n18874396 = wire::Reader_get_i16(in, list[n18874387.index].data);
          v = vector_make(kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 10.0); }, kern::to<Float>(n18874396.Result)), vector_break(v).y, vector_break(v).z);
          n18874397 = wire::Reader_get_i16(in, list[n18874387.index].data);
          v = vector_make(vector_break(v).x, kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 10.0); }, kern::to<Float>(n18874397.Result)), vector_break(v).z);
          n18874398 = wire::Reader_get_i16(in, list[n18874387.index].data);
          v = vector_make(vector_break(v).x, vector_break(v).y, kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 10.0); }, kern::to<Float>(n18874398.Result)));
          if (logic_or(logic_or(logic_or(logic_not(in.ok), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(p).x), 0.0))), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(p).y), 0.0))), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(p).z), 0.0)))) {
            stop_2 = true;
          }
          if (logic_not(logic_or(stop_2, skip_3))) {
            projsync_[0].got = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(projsync_[0].got)));
            key_2 = key;
            map_add(n18874399, projsync_[0].ghosts, key_2);
            v_2 = struct_make<Int_Bool_pair>(n18874399.index, n18874399.added);
            n18874400 = mp_rules_cpp::PoseTimeline_push(projsync_[0].ghosts[v_2.first].line, p, struct_make<Quat>(0.0, 0.0, 0.0, 1.0), v, true, kern::to<Float>(t), now_ms);
            projsync_[0].ghosts[v_2.first].v = v;
            projsync_[0].ghosts[v_2.first].heard = now;
            if (v_2.second) {
              q = Projectile{weapons_Id::grenade_launcher, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 12.0, true, false, false, true, 0, false, 0.0, -1.0, physics_Body{Int64{0x0}}, 0, false};
              q.id = enum_from_index<weapons_Id>(kind).value;
              q.pos = p;
              q.vel = v;
              q.born = now;
              q.by_player = false;
              q.ghost = true;
              q.net = key;
              q.gravity = logic_or(logic_or(enum_equal<weapons_Id>(q.id, weapons_Id::grenade_launcher), enum_equal<weapons_Id>(q.id, weapons_Id::hand_grenade)), enum_equal<weapons_Id>(q.id, weapons_Id::jet_bomb));
              q.fuse = 1000000000.0;
              list_add(projectiles_, q);
              osfx("launch", p, kern::to<Float>(kind), 0.0, 0.0);
            }
          }
        }
        if (logic_not(stop_2)) {
          k += 1;
        }
      }
    }
    for (auto it18874401 = flow_for(n18874401, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it18874401.next();) {
      skip_4 = false;
      if (logic_or(logic_not(projectiles_[n18874401.index].ghost), logic_not(projectiles_[n18874401.index].alive))) {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        it_2 = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, ProjSync_Ghost>(projsync_[0].ghosts, projectiles_[n18874401.index].net).index), kern::to<Float>(map_length<Int, ProjSync_Ghost>(projsync_[0].ghosts))));
        if (logic_or(logic_or(math_compare<"==">(kern::to<Float>(it_2), kern::to<Float>(map_length<Int, ProjSync_Ghost>(projsync_[0].ghosts))), projsync_[0].ghosts[it_2].gone), math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, projsync_[0].ghosts[it_2].heard), game_projsync_kQuiet))) {
          projectiles_[n18874401.index].alive = false;
          skip_4 = true;
        }
        if (logic_not(skip_4)) {
          s = mp_rules_cpp::PoseTimeline_sample(projsync_[0].ghosts[it_2].line, now_ms).Result;
          if (logic_not(s.valid)) {
            skip_4 = true;
          }
          if (logic_not(skip_4)) {
            projectiles_[n18874401.index].pos = s.p;
            projectiles_[n18874401.index].vel = projsync_[0].ghosts[it_2].v;
            if (logic_or(logic_or(logic_or(enum_equal<weapons_Id>(projectiles_[n18874401.index].id, weapons_Id::rocket), enum_equal<weapons_Id>(projectiles_[n18874401.index].id, weapons_Id::drone_rockets)), enum_equal<weapons_Id>(projectiles_[n18874401.index].id, weapons_Id::jet_missile)), enum_equal<weapons_Id>(projectiles_[n18874401.index].id, weapons_Id::drone_guided))) {
              rocket_smoke_emit(projectiles_[n18874401.index], now);
            }
          }
        }
      }
    }
    it_3 = 0;
    while (loop(logic_not(math_compare<"==">(kern::to<Float>(it_3), kern::to<Float>(map_length<Int, ProjSync_Ghost>(projsync_[0].ghosts)))))) {
      if (logic_or(projsync_[0].ghosts[it_3].gone, math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, projsync_[0].ghosts[it_3].heard), kern::formula([](Float game_projsync_kQuiet) -> Float { return (game_projsync_kQuiet + 1.0); }, game_projsync_kQuiet)))) {
        at_2 = it_3;
        map_remove_at(projsync_[0].ghosts, at_2);
        it_3 = at_2;
      } else {
        it_3 += 1;
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
    I32 at = 0;
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
    I32 owner_at = 0;
    if (logic_and(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0), gaps_[0].ride_car)) {
      return {false};
    }
    p = Vector{0.0, 0.0, 0.0};
    q = Quat{0.0, 0.0, 0.0, 1.0};
    n19922959 = gaps_ride_pose(p, q);
    if (logic_not(n19922959.Result)) {
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

  struct Game_swarm_collect_next_idResult { U16 Result = 0; };
  Game_swarm_collect_next_idResult Game_swarm_collect_next_id() {
    swarm_serial_ = kern::to<Int>(kern::formula([](Float swarm_serial_) -> Float { return (swarm_serial_ + 1.0); }, kern::to<Float>(swarm_serial_)));
    if (math_compare<"==">(kern::to<Float>(swarm_serial_), 0.0)) {
      swarm_serial_ = kern::to<Int>(kern::formula([](Float swarm_serial_) -> Float { return (swarm_serial_ + 1.0); }, kern::to<Float>(swarm_serial_)));
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
    U8 anim = 0;
    Bool skip_5 = false;
    Bool skip_6 = false;
    Int p_at = 0;
    U8 flags = 0;
    Bool skip_7 = false;
    Bool actor_alive = false;
    Bool actor_alive_2 = false;
    Bool either = false;
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector f = Vector{0.0, 0.0, 0.0};
    Bool lights = false;
    U8 flags_2 = 0;
    Bool skip_8 = false;
    Int p_at_2 = 0;
    U8 flags_3 = 0;
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
          anim = kern::to<Int>(kern::formula([](Float a, Float st, Float a_2, Float a_3) -> Float { return (((a != 0.0) || ((st - a_2) < 0.5)) ? 1.0 : ((a_3 > 3.0) ? 2.0 : ((a_3 > 0.0) ? 3.0 : 0.0))); }, kern::to<Float>(combat_.soldiers[n20971538.index].los), st, combat_.soldiers[n20971538.index].last_shot, people_.people_[combat_.soldiers[n20971538.index].person].v));
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

  void swarm_draw_vehicles() {
    Bool skip = false;
    CarSpec a = CarSpec{"car", Vector{1.78, 0.82, 0.36}, 1350.0, -0.06, 0.15, 0.34, 0.22, 22.0, -0.26, List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}, -0.15, 0.11, 0.06, 550.0, 850.0, 140000.0, 0.15, 15.0, 0.68, 0.013, 1.3, 2400.0, 3200.0, 30.0, 0.58, 430.0, 600.0, 6.0, 1.1, 2.8, 3000.0, 95.0, List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}, 0.71, 0.54};
    List<Car> put = List<Car>{};
    physics_Pose rest = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    for (auto it20971561 = flow_for(n20971561, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Game_RemoteSwarm>(swarms_))))); it20971561.next();) {
      for (auto it20971562 = flow_for(n20971562, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_RemoteUnit>(swarms_[n20971561.index].units))))); it20971562.next();) {
        skip = false;
        if (logic_not(enum_equal<net_SwarmKind>(swarms_[n20971561.index].units[n20971562.index].kind, net_SwarmKind::cruiser))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_not(math_compare<">">(kern::to<Float>(list_length<Car>(swarm_cruiser_)), 0.0))) {
            a = car::CarSpec_car().Result;
            put = list_make<Car, "1">(car::Car_new(a).Result);
            if (math_compare<">">(kern::to<Float>(list_length<Car>(swarm_cruiser_)), 0.0)) {
              car_cpp::Car_end(swarm_cruiser_[0]);
            }
            swarm_cruiser_ = put;
          }
          rest = car_rest_pose(swarm_cruiser_[0].s_, vector_break(swarms_[n20971561.index].units[n20971562.index].p).x, vector_break(swarms_[n20971561.index].units[n20971562.index].p).y, swarms_[n20971561.index].units[n20971562.index].yaw).Result;
          car::Car_show(swarm_cruiser_[0], swarms_[n20971561.index].units[n20971562.index].aux);
          a_2 = engine_math_math::Mat4_trs(rest.position, rest.rotation, Vector{1.0, 1.0, 1.0}).Result;
          vehicle_art_cpp::VehicleArt_car(art_, a_2, swarm_cruiser_[0], logic_select<Vector>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(swarms_[n20971561.index].units[n20971562.index].flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserWreck)))), 0.5), kWreckBlack, kCruiserWhite), false, false);
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
  void toss_own(U8 kind = 0, I32 local = 0, Int64 id = Int64{0x0}) {
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
    U8 a = 0;
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

  struct toss_update_body_okResult { Bool Result = false; };
  KERN_PURE toss_update_body_okResult toss_update_body_ok(Object<kern::physics::World> w = Object<kern::physics::World>{}, physics_Body b = physics_Body{Int64{0x0}}) {
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

  struct Game_toss_update_held_hereResult { Bool Result = false; };
  Game_toss_update_held_hereResult Game_toss_update_held_here(const TossState_Own& o) {
    Int64 id = Int64{0x0};
    U8 a = 0;
    for (auto it22020110 = flow_for(n22020110, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_VrHand>(vr_hands_))))); it22020110.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kPerson)), math_compare<"==">(kern::to<Float>(vr_hands_[n22020110.index].grab_person), kern::to<Float>(o.local)))) {
        return {true};
      }
      if (logic_and(math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kBird)), math_compare<"==">(kern::to<Float>(vr_hands_[n22020110.index].bird), kern::to<Float>(o.local)))) {
        return {true};
      }
      if (logic_and(logic_or(math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kCow)), math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kPet))), engine_physics_physics::Body_valid(vr_hands_[n22020110.index].grab).Result)) {
        id = Int64{0x0};
        n22020111 = life::Life_animal_of(glife_[0].life, id, vr_hands_[n22020110.index].grab);
        a = n22020111.Result;
        if (logic_or(logic_and(logic_and(math_compare<"==">(kern::to<Float>(a), 1.0), math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kCow))), int64_equal(id, o.id)), logic_and(logic_and(math_compare<"==">(kern::to<Float>(a), 2.0), math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kPet))), int64_equal(id, o.id)))) {
          return {true};
        }
      }
    }
    return {false};
  }

  struct toss_update_matchResult { Bool Result = false; };
  KERN_PURE toss_update_matchResult toss_update_match(const TossState_Own& o) {
    return {math_compare<">=">(kern::to<Float>(o.done_sent), 3.0)};
  }

  struct toss_update_match_2Result { Bool Result = false; };
  KERN_PURE toss_update_match_2Result toss_update_match_2(const TossState_Remote& r_3, const TossState_Remote& q2) {
    return {logic_and(logic_and(math_compare<"==">(kern::to<Float>(q2.from), kern::to<Float>(r_3.from)), math_compare<"==">(kern::to<Float>(q2.kind), kern::to<Float>(r_3.kind))), int64_equal(q2.id, r_3.id))};
  }

  struct toss_update_match_3Result { Bool Result = false; };
  KERN_PURE toss_update_match_3Result toss_update_match_3(const TossState_Remote& r, Float now = 0.0) {
    return {logic_and(logic_or(math_compare<">">(kern::formula([](Float flags, Float kDone) -> Float { return (kern::math::band(flags, kDone) != 0.0); }, kern::to<Float>(r.flags), kern::to<Float>(kDone)), 0.5), math_compare<">">(kern::formula([](Float now, Float at) -> Float { return (now - at); }, now, r.at), game_toss_kQuiet)), math_compare<">">(kern::formula([](Float now, Float at) -> Float { return (now - at); }, now, r.at), 1.5))};
  }

  void toss_update(Float dt = 0.0) {
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Float now = 0.0;
    Float sim_time = 0.0;
    List<Game_toss_update_Rec> recs = List<Game_toss_update_Rec>{};
    Bool skip = false;
    Game_toss_update_Rec r = Game_toss_update_Rec{0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool have = false;
    Bool both = false;
    physics_Body b = physics_Body{Int64{0x0}};
    physics_Pose ps = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    List<U8> o = List<U8>{};
    Int i = 0;
    Game_toss_update_Rec r_2 = Game_toss_update_Rec{0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    List<net_Session_Blob> list = List<net_Session_Blob>{};
    wire_Reader in = wire_Reader{0, true};
    U32 t_ms = 0;
    U8 n = 0;
    U8 k = 0;
    Bool stop_2 = false;
    TossState_Remote r_3 = TossState_Remote{0, 0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, -1, false, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}};
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Float ql = 0.0;
    Int it = 0;
    Int at_2 = 0;
    Int end_2 = 0;
    Bool go = false;
    Bool skip_2 = false;
    Bool live = false;
    Vector bp = Vector{0.0, 0.0, 0.0};
    mp_PoseSample smp = mp_PoseSample{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, false, false, false};
    physics_Body b_2 = physics_Body{Int64{0x0}};
    Int i_2 = 0;
    Bool stop_3 = false;
    physics_Pose ps_2 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose pose_2 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector want_p = Vector{0.0, 0.0, 0.0};
    Quat want_q = Quat{0.0, 0.0, 0.0, 1.0};
    mp_PoseSample smp_2 = mp_PoseSample{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, false, false, false};
    Vector off = Vector{0.0, 0.0, 0.0};
    Quat e = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a = Quat{0.0, 0.0, 0.0, 1.0};
    Int at_3 = 0;
    Int kept_2 = 0;
    Int end_3 = 0;
    if (logic_or(logic_not(net_session::Session_active(net_).Result), logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)))) {
      list_clear(toss_.own);
      list_clear(toss_.remote);
      n22020108 = net_session::Session_take_toss(net_);
      return;
    }
    engine_game_world_physics(n22020109, world_);
    physics = n22020109.result;
    w = physics;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    recs = List<Game_toss_update_Rec>{};
    for (auto it22020112 = flow_for(n22020112, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TossState_Own>(toss_.own))))); it22020112.next();) {
      skip = false;
      r = struct_make<Game_toss_update_Rec>(toss_.own[n22020112.index].kind, 0, toss_.own[n22020112.index].id, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, struct_make<Quat>(0.0, 0.0, 0.0, 1.0));
      have = false;
      if (math_compare<"==">(kern::to<Float>(toss_.own[n22020112.index].kind), kern::to<Float>(kBird))) {
        for (auto it22020113 = flow_for(n22020113, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_LooseBird>(glife_[0].life.loose_))))); it22020113.next();) {
          if (logic_and(math_compare<"==">(kern::to<Float>(glife_[0].life.loose_[n22020113.index].id), kern::to<Float>(toss_.own[n22020112.index].local)), logic_not(logic_not(int64_equal(glife_[0].life.loose_[n22020113.index].net, Int64{0x0}))))) {
            r.p = glife_[0].life.loose_[n22020113.index].pos;
            r.v = glife_[0].life.loose_[n22020113.index].vel;
            r.q = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, glife_[0].life.loose_[n22020113.index].yaw).Result;
            have = true;
          }
        }
        both = have;
        if (both) {
          n22020115 = Game_toss_update_held_here(toss_.own[n22020112.index]);
          both = logic_not(n22020115.Result);
        }
        if (logic_and(both, math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, toss_.own[n22020112.index].born), 0.2))) {
          toss_.own[n22020112.index].still = kern::formula([](Float a, Float dt) -> Float { return (a + dt); }, toss_.own[n22020112.index].still, dt);
        }
        if (math_compare<">">(toss_.own[n22020112.index].still, 1.0)) {
          have = false;
        }
      } else {
        b = physics_Body{Int64{0x0}};
        if (math_compare<"==">(kern::to<Float>(toss_.own[n22020112.index].kind), kern::to<Float>(kPerson))) {
          b = ragdoll_cpp::Ragdolls_bone_body(glife_[0].life.ragdolls_, int64_add(int64_of_int(toss_.own[n22020112.index].local), Int64{0x1}), Ragdolls_Bone::chest).Result;
        } else if (math_compare<"==">(kern::to<Float>(toss_.own[n22020112.index].kind), kern::to<Float>(kCow))) {
          b = life::Life_cow_body(glife_[0].life, toss_.own[n22020112.index].id).Result;
        } else {
          b = life::Life_pet_body(glife_[0].life, int64_low(toss_.own[n22020112.index].id)).Result;
        }
        if (toss_update_body_ok(w, b).Result) {
          pose = engine_world_pose(w, b);
          ps = pose;
          r.p = ps.position;
          r.q = ps.rotation;
          linear_velocity = engine_world_linear_velocity(w, b);
          r.v = linear_velocity;
          have = true;
          n22020116 = Game_toss_update_held_here(toss_.own[n22020112.index]);
          toss_.own[n22020112.index].still = kern::formula([](Float the_result, Float a, Float a_2, Float dt) -> Float { return (((!(the_result != 0.0)) && (a < 0.3)) ? (a_2 + dt) : 0.0); }, kern::to<Float>(n22020116.Result), vector_length(r.v), toss_.own[n22020112.index].still, dt);
          if (math_compare<">">(toss_.own[n22020112.index].still, 1.0)) {
            have = false;
          }
        } else if (math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, toss_.own[n22020112.index].born), 0.5)) {
          skip = true;
        }
      }
      if (logic_not(skip)) {
        n22020117 = Game_toss_update_held_here(toss_.own[n22020112.index]);
        if (n22020117.Result) {
          r.flags = kern::to<Int>(kern::formula([](Float flags, Float kHeld) -> Float { return kern::math::bor(flags, kHeld); }, kern::to<Float>(r.flags), kern::to<Float>(kHeld)));
        }
        if (logic_not(have)) {
          r.flags = kern::to<Int>(kern::formula([](Float flags, Float kDone) -> Float { return kern::math::bor(flags, kDone); }, kern::to<Float>(r.flags), kern::to<Float>(kDone)));
          toss_.own[n22020112.index].done_sent = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(toss_.own[n22020112.index].done_sent)));
        }
        list_add(recs, r);
      }
    }
    toss_.clock = kern::formula([](Float clock, Float dt) -> Float { return (clock + dt); }, toss_.clock, dt);
    if (logic_and(math_compare<">=">(toss_.clock, game_toss_kSendEvery), logic_not(math_compare<"==">(kern::to<Float>(list_length<Game_toss_update_Rec>(recs)), 0.0)))) {
      toss_.clock = 0.0;
      o = List<U8>{};
      wire::put_u32(o, kern::to<Int>(kern::formula([](Float now) -> Float { return kern::math::wrap32(kern::math::round((now * 1000.0))); }, now)));
      wire::put_u8(o, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(kern::math::min(a, 16.0), 255.0); }, kern::to<Float>(list_length<Game_toss_update_Rec>(recs)))));
      i = 0;
      while (loop(logic_and(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Game_toss_update_Rec>(recs))), 0.5), math_compare<">">(kern::formula([](Float i) -> Float { return ((i < 0.0) != (16.0 < 0.0) ? (16.0 < 0.0) : (i < 16.0)); }, kern::to<Float>(i)), 0.5)))) {
        r_2 = recs[i];
        wire::put_u8(o, r_2.kind);
        wire::put_u8(o, r_2.flags);
        wire::put_u64(o, r_2.id);
        wire::put_f32(o, vector_break(r_2.p).x);
        wire::put_f32(o, vector_break(r_2.p).y);
        wire::put_f32(o, vector_break(r_2.p).z);
        wire::put_i16(o, game_toss_cpp::q16(r_2.q.x).Result);
        wire::put_i16(o, game_toss_cpp::q16(r_2.q.y).Result);
        wire::put_i16(o, game_toss_cpp::q16(r_2.q.z).Result);
        wire::put_i16(o, game_toss_cpp::q16(r_2.q.w).Result);
        wire::put_i16(o, game_toss_cpp::cms(vector_break(r_2.v).x).Result);
        wire::put_i16(o, game_toss_cpp::cms(vector_break(r_2.v).y).Result);
        wire::put_i16(o, game_toss_cpp::cms(vector_break(r_2.v).z).Result);
        i += 1;
      }
      net_session_cpp::Session_send_toss(net_, o);
      toss_.sent = kern::to<Int>(kern::formula([](Float sent) -> Float { return (sent + 1.0); }, kern::to<Float>(toss_.sent)));
    }
    at = 0;
    kept = at;
    end = list_length<TossState_Own>(toss_.own);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(toss_update_match(toss_.own[at]).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n22020118, toss_.own[at]);
          toss_.own[kept] = n22020118.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(toss_.own, kept, TossState_Own{0, Int64{0x0}, -1, 0.0, 0.0, 0});
    n22020119 = net_session::Session_take_toss(net_);
    list = n22020119.Result;
    for (auto it22020120 = flow_for(n22020120, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Session_Blob>(list))))); it22020120.next();) {
      in = wire_Reader{0, true};
      n22020121 = wire::Reader_get_u32(in, list[n22020120.index].data);
      t_ms = n22020121.Result;
      n22020122 = wire::Reader_get_u8(in, list[n22020120.index].data);
      n = n22020122.Result;
      k = 0;
      stop_2 = false;
      while (loop(logic_and(logic_not(stop_2), logic_and(math_compare<"<">(kern::to<Float>(k), kern::to<Float>(n)), in.ok)))) {
        r_3 = TossState_Remote{0, 0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, -1, false, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}};
        r_3.from = list[n22020120.index].from;
        n22020123 = wire::Reader_get_u8(in, list[n22020120.index].data);
        r_3.kind = n22020123.Result;
        n22020124 = wire::Reader_get_u8(in, list[n22020120.index].data);
        r_3.flags = n22020124.Result;
        n22020125 = wire::Reader_get_u64(in, list[n22020120.index].data);
        r_3.id = n22020125.Result;
        n22020126 = wire::Reader_get_f32(in, list[n22020120.index].data);
        r_3.p = vector_make(n22020126.Result, vector_break(r_3.p).y, vector_break(r_3.p).z);
        n22020127 = wire::Reader_get_f32(in, list[n22020120.index].data);
        r_3.p = vector_make(vector_break(r_3.p).x, n22020127.Result, vector_break(r_3.p).z);
        n22020128 = wire::Reader_get_f32(in, list[n22020120.index].data);
        r_3.p = vector_make(vector_break(r_3.p).x, vector_break(r_3.p).y, n22020128.Result);
        q = Quat{0.0, 0.0, 0.0, 1.0};
        n22020129 = wire::Reader_get_i16(in, list[n22020120.index].data);
        q.x = game_toss_cpp::uq16(n22020129.Result).Result;
        n22020130 = wire::Reader_get_i16(in, list[n22020120.index].data);
        q.y = game_toss_cpp::uq16(n22020130.Result).Result;
        n22020131 = wire::Reader_get_i16(in, list[n22020120.index].data);
        q.z = game_toss_cpp::uq16(n22020131.Result).Result;
        n22020132 = wire::Reader_get_i16(in, list[n22020120.index].data);
        q.w = game_toss_cpp::uq16(n22020132.Result).Result;
        ql = kern::formula([](Float x, Float y, Float z, Float w) -> Float { return kern::math::sqrt(((((x * x) + (y * y)) + (z * z)) + (w * w))); }, q.x, q.y, q.z, q.w);
        r_3.q = logic_select<Quat>(math_compare<">">(ql, 1e-06), struct_make<Quat>(kern::formula([](Float x, Float ql) -> Float { return kern::math::div(x, ql); }, q.x, ql), kern::formula([](Float y, Float ql) -> Float { return kern::math::div(y, ql); }, q.y, ql), kern::formula([](Float z, Float ql) -> Float { return kern::math::div(z, ql); }, q.z, ql), kern::formula([](Float w, Float ql) -> Float { return kern::math::div(w, ql); }, q.w, ql)), struct_make<Quat>(0.0, 0.0, 0.0, 1.0));
        n22020133 = wire::Reader_get_i16(in, list[n22020120.index].data);
        r_3.v = vector_make(kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 100.0); }, kern::to<Float>(n22020133.Result)), vector_break(r_3.v).y, vector_break(r_3.v).z);
        n22020134 = wire::Reader_get_i16(in, list[n22020120.index].data);
        r_3.v = vector_make(vector_break(r_3.v).x, kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 100.0); }, kern::to<Float>(n22020134.Result)), vector_break(r_3.v).z);
        n22020135 = wire::Reader_get_i16(in, list[n22020120.index].data);
        r_3.v = vector_make(vector_break(r_3.v).x, vector_break(r_3.v).y, kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 100.0); }, kern::to<Float>(n22020135.Result)));
        if (logic_or(logic_or(logic_or(logic_not(in.ok), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(r_3.p).x), 0.0))), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(r_3.p).y), 0.0))), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, vector_break(r_3.p).z), 0.0)))) {
          stop_2 = true;
        }
        if (logic_not(stop_2)) {
          r_3.at = now;
          toss_.got = kern::to<Int>(kern::formula([](Float got) -> Float { return (got + 1.0); }, kern::to<Float>(toss_.got)));
          at_2 = 0;
          end_2 = list_length<TossState_Remote>(toss_.remote);
          go = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end_2));
          while (loop(go)) {
            if (toss_update_match_2(r_3, toss_.remote[at_2]).Result) {
              go = false;
            } else {
              at_2 += 1;
              go = math_compare<"<">(kern::to<Float>(at_2), kern::to<Float>(end_2));
            }
          }
          it = at_2;
          if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(list_length<TossState_Remote>(toss_.remote)))) {
            n22020136 = mp_rules_cpp::PoseTimeline_push(r_3.line, r_3.p, r_3.q, r_3.v, true, kern::to<Float>(t_ms), kern::formula([](Float now) -> Float { return (now * 1000.0); }, now));
            list_add(toss_.remote, r_3);
          } else {
            r_3.local = toss_.remote[it].local;
            var_take(n22020137, toss_.remote[it].line);
            toss_.remote[it].line.offset_ = struct_break<mp_PoseTimeline>(n22020137.value).offset_;
            toss_.remote[it].line.delay_ = struct_break<mp_PoseTimeline>(n22020137.value).delay_;
            toss_.remote[it].line.have_offset_ = struct_break<mp_PoseTimeline>(n22020137.value).have_offset_;
            r_3.line = n22020137.value;
            n22020138 = mp_rules_cpp::PoseTimeline_push(r_3.line, r_3.p, r_3.q, r_3.v, true, kern::to<Float>(t_ms), kern::formula([](Float now) -> Float { return (now * 1000.0); }, now));
            toss_.remote[it] = r_3;
          }
        }
        if (logic_not(stop_2)) {
          k += 1;
        }
      }
    }
    for (auto it22020139 = flow_for(n22020139, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TossState_Remote>(toss_.remote))))); it22020139.next();) {
      skip_2 = false;
      live = logic_and(logic_not(math_compare<">">(kern::formula([](Float a, Float kDone) -> Float { return (kern::math::band(a, kDone) != 0.0); }, kern::to<Float>(toss_.remote[n22020139.index].flags), kern::to<Float>(kDone)), 0.5)), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, toss_.remote[n22020139.index].at), game_toss_kQuiet));
      if (math_compare<"==">(kern::to<Float>(toss_.remote[n22020139.index].kind), kern::to<Float>(kBird))) {
        bp = toss_.remote[n22020139.index].p;
        smp = mp_rules_cpp::PoseTimeline_sample(toss_.remote[n22020139.index].line, kern::formula([](Float now) -> Float { return (now * 1000.0); }, now)).Result;
        if (smp.valid) {
          bp = smp.p;
        }
        life_cpp::Life_net_bird(glife_[0].life, toss_.remote[n22020139.index].id, bp, kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(engine_math_math::Quat_rotate(toss_.remote[n22020139.index].q, Vector{1.0, 0.0, 0.0}).Result).y, vector_break(engine_math_math::Quat_rotate(toss_.remote[n22020139.index].q, Vector{1.0, 0.0, 0.0}).Result).x), toss_.remote[n22020139.index].v, logic_and(live, math_compare<">">(kern::formula([](Float a, Float kHeld) -> Float { return (kern::math::band(a, kHeld) != 0.0); }, kern::to<Float>(toss_.remote[n22020139.index].flags), kern::to<Float>(kHeld)), 0.5)));
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        if (logic_not(live)) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          b_2 = physics_Body{Int64{0x0}};
          if (math_compare<"==">(kern::to<Float>(toss_.remote[n22020139.index].kind), kern::to<Float>(kPerson))) {
            if (math_compare<"<">(kern::to<Float>(toss_.remote[n22020139.index].local), 0.0)) {
              i_2 = 0;
              stop_3 = false;
              while (loop(logic_and(logic_not(stop_3), math_compare<">">(kern::formula([](Float i_2, Float a) -> Float { return ((i_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_2 < a)); }, kern::to<Float>(i_2), kern::to<Float>(list_length<People_Person>(people_.people_))), 0.5)))) {
                if (logic_select<Bool>(net_session::Session_hosting(net_).Result, int64_equal(int64_of_int(i_2), toss_.remote[n22020139.index].id), logic_and(math_compare<">=">(kern::to<Float>(people_.people_[i_2].net), 0.0), int64_equal(int64_of_int(people_.people_[i_2].net), toss_.remote[n22020139.index].id)))) {
                  toss_.remote[n22020139.index].local = kern::to<Int>(kern::formula([](Float i_2) -> Float { return kern::math::wrap32s(i_2); }, kern::to<Float>(i_2)));
                  stop_3 = true;
                }
                if (logic_not(stop_3)) {
                  i_2 += 1;
                }
              }
              if (math_compare<"<">(kern::to<Float>(toss_.remote[n22020139.index].local), 0.0)) {
                skip_2 = true;
              }
            }
            if (logic_not(skip_2)) {
              b_2 = ragdoll_cpp::Ragdolls_bone_body(glife_[0].life.ragdolls_, int64_add(int64_of_int(toss_.remote[n22020139.index].local), Int64{0x1}), Ragdolls_Bone::chest).Result;
              if (logic_not(toss_update_body_ok(w, b_2).Result)) {
                if (logic_not(ragdoll_cpp::Ragdolls_has(glife_[0].life.ragdolls_, int64_add(int64_of_int(toss_.remote[n22020139.index].local), Int64{0x1})).Result)) {
                  people::People_grab_knock(people_, toss_.remote[n22020139.index].local, vector_scale(toss_.remote[n22020139.index].v, 0.2), now);
                }
                skip_2 = true;
              }
            }
          } else if (math_compare<"==">(kern::to<Float>(toss_.remote[n22020139.index].kind), kern::to<Float>(kCow))) {
            n22020140 = life_cpp::Life_net_carry_cow(glife_[0].life, toss_.remote[n22020139.index].id);
            b_2 = n22020140.Result;
          } else {
            n22020141 = life_cpp::Life_net_carry_pet(glife_[0].life, int64_low(toss_.remote[n22020139.index].id));
            b_2 = n22020141.Result;
          }
          if (logic_not(skip_2)) {
            if (logic_not(toss_update_body_ok(w, b_2).Result)) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              pose_2 = engine_world_pose(w, b_2);
              ps_2 = pose_2;
              want_p = vector_add(toss_.remote[n22020139.index].p, vector_scale(toss_.remote[n22020139.index].v, kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, toss_.remote[n22020139.index].at)));
              want_q = toss_.remote[n22020139.index].q;
              smp_2 = mp_rules_cpp::PoseTimeline_sample(toss_.remote[n22020139.index].line, kern::formula([](Float now) -> Float { return (now * 1000.0); }, now)).Result;
              if (smp_2.valid) {
                want_p = smp_2.p;
                want_q = smp_2.q;
              }
              off = vector_sub(want_p, ps_2.position);
              if (math_compare<">">(vector_length(off), 6.0)) {
                engine_world_set_pose(w, b_2, struct_make<physics_Pose>(want_p, want_q));
                engine_world_set_linear_velocity(w, b_2, toss_.remote[n22020139.index].v);
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                engine_world_set_linear_velocity(w, b_2, vector_add(toss_.remote[n22020139.index].v, vector_scale(off, 10.0)));
                a = struct_make<Quat>(kern::formula([](Float x) -> Float { return (-x); }, ps_2.rotation.x), kern::formula([](Float y) -> Float { return (-y); }, ps_2.rotation.y), kern::formula([](Float z) -> Float { return (-z); }, ps_2.rotation.z), ps_2.rotation.w);
                e = engine_math_math::Quat_mul(want_q, a).Result;
                if (math_compare<"<">(e.w, 0.0)) {
                  e = struct_make<Quat>(kern::formula([](Float x) -> Float { return (-x); }, e.x), kern::formula([](Float y) -> Float { return (-y); }, e.y), kern::formula([](Float z) -> Float { return (-z); }, e.z), kern::formula([](Float w) -> Float { return (-w); }, e.w));
                }
                engine_world_set_angular_velocity(w, b_2, vector_scale(vector_scale(vector_make(e.x, e.y, e.z), 2.0), 8.0));
                engine_world_set_awake(w, b_2, true);
                if (logic_or(math_compare<"==">(kern::to<Float>(toss_.remote[n22020139.index].kind), kern::to<Float>(kCow)), math_compare<"==">(kern::to<Float>(toss_.remote[n22020139.index].kind), kern::to<Float>(kPet)))) {
                  life::Life_hold_animal(glife_[0].life, b_2);
                }
              }
            }
          }
        }
      }
    }
    at_3 = 0;
    kept_2 = at_3;
    end_3 = list_length<TossState_Remote>(toss_.remote);
    while (loop(math_compare<"<">(kern::to<Float>(at_3), kern::to<Float>(end_3)))) {
      if (logic_not(toss_update_match_3(toss_.remote[at_3], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept_2), kern::to<Float>(at_3))) {
          var_take(n22020142, toss_.remote[at_3]);
          toss_.remote[kept_2] = n22020142.value;
        }
        kept_2 += 1;
      }
      at_3 += 1;
    }
    list_resize(toss_.remote, kept_2, TossState_Remote{0, 0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, -1, false, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}});
  }

  struct toss_jsonResult { Text Result = ""; };
  toss_jsonResult toss_json() {
    Text b = "";
    U32 ghosts = 0;
    U32 rounds = 0;
    Vector gp = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    b = "";
    ghosts = 0;
    rounds = 0;
    gp = Vector{0.0, 0.0, 0.0};
    for (auto it22020155 = flow_for(n22020155, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it22020155.next();) {
      skip = false;
      if (logic_not(projectiles_[n22020155.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (projectiles_[n22020155.index].ghost) {
          ghosts += 1;
          gp = projectiles_[n22020155.index].pos;
        } else {
          rounds += 1;
        }
      }
    }
    b = text_format<"{\"own\":%zu,\"remote\":%zu,\"sent\":%u,\"got\":%u,\"rounds\":%u,\"ghosts\":%u,\"ghost\":[%.2f,%.2f,%.2f]}">(255, list_length<TossState_Own>(toss_.own), list_length<TossState_Remote>(toss_.remote), toss_.sent, toss_.got, rounds, ghosts, vector_break(gp).x, vector_break(gp).y, vector_break(gp).z);
    return {b};
  }

  struct toss_commandResult { Text Result = ""; };
  /// tests: "toss person" - the nearest citizen goes down as ours (as a hand would take him); "toss fling X Y Z" - his doll flies; "toss where I" - the chest of the host's citizen I here
  toss_commandResult toss_command(const Text& cmd) {
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Float now = 0.0;
    Float sim_time = 0.0;
    Vector me = Vector{0.0, 0.0, 0.0};
    I32 best = 0;
    Float bd = 0.0;
    Bool skip = false;
    Float d = 0.0;
    Vector v = Vector{0.0, 0.0, 0.0};
    Bool either = false;
    physics_Body bb = physics_Body{Int64{0x0}};
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    I32 id = 0;
    physics_Body b = physics_Body{Int64{0x0}};
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both_2 = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Text o = "";
    I32 id_2 = 0;
    Vector dir = Vector{0.0, 0.0, 0.0};
    Vector eye = Vector{0.0, 0.0, 0.0};
    Vector d_2 = Vector{0.0, 0.0, 0.0};
    Text o_2 = "";
    Text t = "";
    Bool first = false;
    Bool skip_2 = false;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0))) {
      return {"no life"};
    }
    engine_game_world_physics(n22020143, world_);
    physics = n22020143.result;
    w = physics;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    if (text_equal(cmd, "toss person")) {
      n22020144 = player_position();
      me = n22020144.Result;
      best = -1;
      bd = 60.0;
      for (auto it22020145 = flow_for(n22020145, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<People_Person>(people_.people_))))); it22020145.next();) {
        skip = false;
        if (logic_or(logic_or(enum_equal<People_State>(people_.people_[n22020145.index].state, People_State::down), enum_equal<People_State>(people_.people_[n22020145.index].state, People_State::gone)), math_compare<"<">(now, people_.people_[n22020145.index].knocked_until))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_and(logic_not(net_session::Session_hosting(net_).Result), math_compare<"<">(kern::to<Float>(people_.people_[n22020145.index].net), 0.0))) {
            skip = true;
          }
          if (logic_not(skip)) {
            d = vector_length(vector_sub(people_.people_[n22020145.index].pos, me));
            if (math_compare<"<">(d, bd)) {
              bd = d;
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n22020145.index)));
            }
          }
        }
      }
      if (math_compare<"<">(kern::to<Float>(best), 0.0)) {
        return {"nobody"};
      }
      people::People_grab_knock(people_, best, Vector{0.0, 0.0, 0.6}, now);
      toss_own(0, best, Int64{0x0});
      toss_test_ = best;
      return {text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float a, Float best, Float a_2) -> Float { return ((a != 0.0) ? best : a_2); }, kern::to<Float>(net_session::Session_hosting(net_).Result), kern::to<Float>(best), kern::to<Float>(people_.people_[best].net))))};
    }
    if (math_compare<"==">(kern::to<Float>(text_rfind(cmd, "toss fling ", 0)), 0.0)) {
      v = Vector{0.0, 0.0, 0.0};
      either = math_compare<"<">(kern::to<Float>(toss_test_), 0.0);
      if (logic_not(either)) {
        text_scan<"%f %f %f">(n22020147, cmd, 11, vector_break(v).x, vector_break(v).y, vector_break(v).z);
        v = vector_make(n22020147.v0, vector_break(v).y, vector_break(v).z);
        v = vector_make(vector_break(v).x, n22020147.v1, vector_break(v).z);
        v = vector_make(vector_break(v).x, vector_break(v).y, n22020147.v2);
        either = math_compare<"!=">(kern::to<Float>(n22020147.count), 3.0);
      }
      if (either) {
        return {"usage"};
      }
      for (auto it22020148 = flow_for(n22020148, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it22020148.next();) {
        bb = ragdoll_cpp::Ragdolls_bone_body(glife_[0].life.ragdolls_, int64_add(int64_of_int(toss_test_), Int64{0x1}), enum_from_index<Ragdolls_Bone>(n22020148.index).value).Result;
        both = engine_physics_physics::Body_valid(bb).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(w, bb);
          both = body_valid_2;
        }
        if (both) {
          engine_world_set_linear_velocity(w, bb, v);
          engine_world_set_awake(w, bb, true);
        }
      }
      return {"ok"};
    }
    if (math_compare<"==">(kern::to<Float>(text_rfind(cmd, "toss where ", 0)), 0.0)) {
      id = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(text_parse_int(cmd, 11, 10).value))));
      for (auto it22020149 = flow_for(n22020149, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<People_Person>(people_.people_))))); it22020149.next();) {
        if (logic_select<Bool>(net_session::Session_hosting(net_).Result, math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n22020149.index))))), kern::to<Float>(id)), math_compare<"==">(kern::to<Float>(people_.people_[n22020149.index].net), kern::to<Float>(id)))) {
          b = ragdoll_cpp::Ragdolls_bone_body(glife_[0].life.ragdolls_, int64_add(int64_of_int(n22020149.index), Int64{0x1}), Ragdolls_Bone::chest).Result;
          both_2 = engine_physics_physics::Body_valid(b).Result;
          if (both_2) {
            body_valid_4 = engine_world_body_valid(w, b);
            both_2 = body_valid_4;
          }
          if (logic_not(both_2)) {
            return {"{\"doll\":false}"};
          }
          pose = engine_world_pose(w, b);
          p = struct_break<physics_Pose>(pose).position;
          o = "";
          o = text_format<"{\"doll\":true,\"x\":%.2f,\"y\":%.2f,\"z\":%.2f}">(95, vector_break(p).x, vector_break(p).y, vector_break(p).z);
          return {o};
        }
      }
      return {"{\"none\":true}"};
    }
    if (math_compare<"==">(kern::to<Float>(text_rfind(cmd, "toss fire ", 0)), 0.0)) {
      id_2 = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(text_parse_int(cmd, 10, 10).value))));
      dir = Vector{0.0, 0.0, 0.0};
      n22020150 = aim_ray(dir);
      eye = n22020150.Result;
      d_2 = logic_select<Vector>(math_compare<">">(vector_length(vector_add(dir, Vector{0.0, 0.0, 0.35})), 9.999999717180685e-10), vector_normalize(vector_add(dir, Vector{0.0, 0.0, 0.35})), dir);
      fire_projectile(enum_from_index<weapons_Id>(id_2).value, vector_add(eye, vector_scale(d_2, 1.2)), vector_scale(d_2, kern::formula([](Float id_2) -> Float { return ((id_2 == 7.0) ? 14.0 : ((id_2 == 4.0) ? 22.0 : 40.0)); }, kern::to<Float>(id_2))), true, body(soldier_).Result);
      return {"fired"};
    }
    if (text_equal(cmd, "toss animals")) {
      o_2 = text_append(text_append("{\"epoch\":", text_format<"%f">(-1, life::Life_epoch(glife_[0].life).Result), ""), ",\"cows\":[", "");
      t = "";
      first = true;
      for (auto it22020151 = flow_for(n22020151, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_))))); it22020151.next();) {
        t = text_format<"%s[%u,%.1f,%.1f]">(95, logic_select<Text>(first, "", ","), int64_low(int64_and(glife_[0].life.cows_[n22020151.index].key, Int64{0xffff})), vector_break(glife_[0].life.cows_[n22020151.index].pos).x, vector_break(glife_[0].life.cows_[n22020151.index].pos).y);
        o_2 = text_append(o_2, t, "");
        first = false;
      }
      o_2 = text_append(o_2, "],\"pets\":[", "");
      first = true;
      for (auto it22020152 = flow_for(n22020152, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(glife_[0].life.pets_))))); it22020152.next();) {
        skip_2 = false;
        if (logic_not(glife_[0].life.pets_[n22020152.index].shown)) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          t = text_format<"%s[%zu,%.1f,%.1f]">(95, logic_select<Text>(first, "", ","), n22020152.index, vector_break(glife_[0].life.pets_[n22020152.index].pos).x, vector_break(glife_[0].life.pets_[n22020152.index].pos).y);
          o_2 = text_append(o_2, t, "");
          first = false;
        }
      }
      o_2 = text_append(o_2, "],\"birds\":[", "");
      first = true;
      for (auto it22020153 = flow_for(n22020153, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it22020153.next();) {
        for (auto it22020154 = flow_for(n22020154, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pigeon>(glife_[0].life.flocks_[n22020153.index].birds))))); it22020154.next();) {
          t = text_format<"%s[%.1f,%.1f]">(95, logic_select<Text>(first, "", ","), vector_break(glife_[0].life.flocks_[n22020153.index].birds[n22020154.index].pos).x, vector_break(glife_[0].life.flocks_[n22020153.index].birds[n22020154.index].pos).y);
          o_2 = text_append(o_2, t, "");
          first = false;
        }
      }
      return {text_append(o_2, "]}", "")};
    }
    n22020156 = toss_json();
    return {n22020156.Result};
  }

  struct line_of_sightResult { Bool Result = false; };
  line_of_sightResult line_of_sight(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}) {
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    U32 me = 0;
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
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

  void damage_car(I32 i = 0, Float amount = 0.0, Bool by_player = false, town_Cause cause = town_Cause::other) {
    Float now = 0.0;
    Float sim_time = 0.0;
    Bool bus = false;
    if (logic_or(math_compare<"<">(kern::to<Float>(i), 0.0), math_compare<">=">(kern::to<Float>(i), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<TrafficCar>(cars_))))))))) {
      return;
    }
    if (logic_or(cars_[i].wreck, logic_not(math_compare<">">(amount, 0.0)))) {
      return;
    }
    if (town_car_remote(i).Result) {
      if (by_player) {
        cars_[i].hit_by_player = true;
      }
      townsync_car_hit(i, amount, cause);
      return;
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    if (logic_and(logic_and(cars_[i].police, enum_equal<Mode>(mode_, Mode::car)), math_compare<"==">(kern::to<Float>(i), kern::to<Float>(car_index_)))) {
      return;
    }
    cars_[i].hp = kern::formula([](Float a, Float amount) -> Float { return kern::math::max(0.0, (a - amount)); }, cars_[i].hp, amount);
    bus = math_compare<">">(cars_[i].car[0].s_.radius, 0.4);
    if (math_compare<"<=">(cars_[i].hp, kern::formula([](Float the_result, Float a, Float kCarCritical) -> Float { return (the_result * ((a != 0.0) ? 0.25 : kCarCritical)); }, car_max_hp(cars_[i]).Result, kern::to<Float>(cars_[i].police), kCarCritical))) {
      if (math_compare<"<">(cars_[i].burn_until, 0.0)) {
        cars_[i].burn_until = kern::formula([](Float now, Float a, Float bus, Float kCarBurn) -> Float { return (now + ((a != 0.0) ? 2.6 : ((bus != 0.0) ? 3.1 : kCarBurn))); }, now, kern::to<Float>(cars_[i].police), kern::to<Float>(bus), kCarBurn);
      } else if (logic_or(logic_not(cars_[i].police), math_compare<"<=">(cars_[i].hp, 0.0))) {
        cars_[i].burn_until = kern::formula([](Float a, Float now, Float a_2, Float bus) -> Float { return kern::math::min(a, (now + ((a_2 != 0.0) ? 0.5 : ((bus != 0.0) ? 0.72 : 0.62)))); }, cars_[i].burn_until, now, kern::to<Float>(cars_[i].police), kern::to<Float>(bus));
      }
    }
    if (by_player) {
      cars_[i].hit_by_player = true;
    }
  }

  void spawn_police_unit() {
    I32 car = 0;
    Int i = 0;
    Bool stop_2 = false;
    Vector me = Vector{0.0, 0.0, 0.0};
    Vector p = Vector{0.0, 0.0, 0.0};
    Float heading = 0.0;
    Bool found = false;
    I32 attempt = 0;
    Bool stop_3 = false;
    Bool skip = false;
    Float d = 0.0;
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Float to_player = 0.0;
    Vector pos = Vector{0.0, 0.0, 0.0};
    Bool actor_alive = false;
    Vector rot = Vector{0.0, 0.0, 0.0};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    PoliceUnit u = PoliceUnit{-1, PoliceUnit_State::pursuit, List<PoliceUnit_Cop>{}, List<Vector>{}, -99.0, -1.0, 0.0, -1.0, -1.0, 17.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, -1.0};
    Float sim_time = 0.0;
    I32 slot = 0;
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
      n23068691 = city_stream_cpp::CityStream_street_point(city_, rng_, p, heading, me, game_town_kSpawnMax);
      if (logic_not(n23068691.Result)) {
        stop_3 = true;
      }
      if (logic_not(logic_or(stop_3, skip))) {
        d = game_town_cpp::dist2d(p, me).Result;
        if (math_compare<"<">(d, game_town_kSpawnMin)) {
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
    engine_game_world_respawn_now(n23068705, world_, soldier_);
    respawn_now = n23068705.result;
    return {respawn_now};
  }

  void update_lamps() {
    gaps_lamps_place();
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
    n23068707 = focus_position();
    me = n23068707.Result;
    range = 180.0;
    o = "";
    b = "";
    look = vector_sub(cam_.target, cam_.eye);
    b = text_format<"{\"me\":[%.1f,%.1f,%.3f],\"lanes\":[">(95, vector_break(me).x, vector_break(me).y, kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(look).y, vector_break(look).x));
    o = text_append(o, b, "");
    first = true;
    for (auto it23068708 = flow_for(n23068708, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it23068708.next();) {
      skip = false;
      if (logic_not(city_.lanes_[n23068708.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        near = false;
        k = 0;
        while (loop(logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(city_.lanes_[n23068708.index].points))), 0.5), logic_not(near)))) {
          a = city_.lanes_[n23068708.index].points[k];
          d = vector_sub(city_.lanes_[n23068708.index].points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))], a);
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
          for (auto it23068709 = flow_for(n23068709, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(city_.lanes_[n23068708.index].points))))); it23068709.next();) {
            b = text_format<"%s%.0f,%.0f">(95, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n23068709.index), 0.0), ",", ""), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(city_.lanes_[n23068708.index].points[n23068709.index]).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(city_.lanes_[n23068708.index].points[n23068709.index]).y, vector_break(me).y));
            o = text_append(o, b, "");
          }
          o = text_append(o, "]", "");
        }
      }
    }
    o = text_append(o, "],\"police\":[", "");
    first = true;
    for (auto it23068710 = flow_for(n23068710, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it23068710.next();) {
      skip_2 = false;
      if (math_compare<"<">(kern::to<Float>(units_[n23068710.index].car), 0.0)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        n23068711 = matrix_of(cars_[units_[n23068710.index].car].actor);
        a_2 = n23068711.Result;
        p = engine_math_math::Mat4_translation(a_2).Result;
        b = text_format<"%s[%.0f,%.0f]">(95, logic_select<Text>(first, "", ","), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(p).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(p).y, vector_break(me).y));
        o = text_append(o, b, "");
        first = false;
      }
    }
    for (auto it23068712 = flow_for(n23068712, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it23068712.next();) {
      skip_3 = false;
      if (logic_not(pdrones_[n23068712.index].active)) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        b = text_format<"%s[%.0f,%.0f]">(95, logic_select<Text>(first, "", ","), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(pdrones_[n23068712.index].pos).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(pdrones_[n23068712.index].pos).y, vector_break(me).y));
        o = text_append(o, b, "");
        first = false;
      }
    }
    o = text_append(o, "],\"players\":[", "");
    first = true;
    for (auto it23068713 = flow_for(n23068713, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it23068713.next();) {
      b = text_format<"%s[%.0f,%.0f]">(95, logic_select<Text>(first, "", ","), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(net_.remotes_[n23068713.index].shown).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(net_.remotes_[n23068713.index].shown).y, vector_break(me).y));
      o = text_append(o, b, "");
      first = false;
    }
    o = text_append(o, "]", "");
    if (logic_and(jet_placed_, logic_not(enum_equal<Mode>(mode_, Mode::jet)))) {
      n23068714 = matrix_of(jet_actor_);
      a_3 = n23068714.Result;
      j = engine_math_math::Mat4_translation(a_3).Result;
      b = text_format<",\"jet\":[%.0f,%.0f]">(95, kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(j).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(j).y, vector_break(me).y));
      o = text_append(o, b, "");
    }
    return {text_append(o, "}", "")};
  }

  struct minimap_json_distResult { Float Result = 0.0; };
  KERN_PURE minimap_json_distResult minimap_json_dist(const List<Vector>& pts, Vector me = Vector{0.0, 0.0, 0.0}) {
    Float best = 0.0;
    Int k = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l2 = 0.0;
    Float t = 0.0;
    best = 1000000000.0;
    k = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(pts))), 0.5))) {
      a = pts[k];
      d = vector_sub(pts[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))], a);
      l2 = kern::formula([](Float x, Float y) -> Float { return kern::math::max(1e-06, ((x * x) + (y * y))); }, vector_break(d).x, vector_break(d).y);
      t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(me).x, vector_break(a).x, vector_break(d).x, vector_break(me).y, vector_break(a).y, vector_break(d).y, l2), 0.0, 1.0));
      best = kern::formula([](Float best, Float x, Float x_2, Float t, Float x_3, Float y, Float y_2, Float y_3) -> Float { return kern::math::min(best, kern::math::sqrt((((x + (x_2 * t)) - x_3) * ((x + (x_2 * t)) - x_3)) + (((y + (y_2 * t)) - y_3) * ((y + (y_2 * t)) - y_3)))); }, best, vector_break(a).x, vector_break(d).x, t, vector_break(me).x, vector_break(a).y, vector_break(d).y, vector_break(me).y);
      k += 1;
    }
    return {best};
  }

  struct minimap_json_matchResult { Bool Result = false; };
  KERN_PURE minimap_json_matchResult minimap_json_match(const Game_minimap_json_Feat& f, Float range = 0.0) {
    return {math_compare<">">(f.d, range)};
  }

  struct minimap_json_match_2Result { Bool Result = false; };
  KERN_PURE minimap_json_match_2Result minimap_json_match_2(const Game_minimap_json_Feat& a, const Game_minimap_json_Feat& b) {
    return {math_compare<"<">(a.d, b.d)};
  }

  struct minimap_json_match_3Result { Bool Result = false; };
  /// draw order of the original: the map's layer order (water, green, roads, buildings)
  KERN_PURE minimap_json_match_3Result minimap_json_match_3(const Game_minimap_json_Feat& a, const Game_minimap_json_Feat& b) {
    return {math_compare<"<">(kern::to<Float>(game_town_cpp::map_layer(a.kind).Result), kern::to<Float>(game_town_cpp::map_layer(b.kind).Result))};
  }

  struct minimap_json_compassResult { Float Result = 0.0; };
  KERN_PURE minimap_json_compassResult minimap_json_compass(Float x = 0.0, Float y = 0.0) {
    return {kern::formula([](Float y, Float x, Float kPi) -> Float { return (90.0 - kern::math::div((kern::math::atan2(y, x) * 180.0), kPi)); }, y, x, kPi)};
  }

  /// every path thinned to <= 28 points
  void minimap_json_put(Text& b_2, Text& o, const List<Vector>& p, Vector me = Vector{0.0, 0.0, 0.0}) {
    Int step = 0;
    Int k = 0;
    step = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::max(1.0, kern::math::trunc(kern::math::div((a + 27.0), 28.0))); }, kern::to<Float>(list_length<Vector>(p))));
    k = 0;
    while (loop(math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<Vector>(p))), 0.5))) {
      b_2 = text_format<",%.1f,%.1f">(159, kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(p[k]).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(p[k]).y, vector_break(me).y));
      o = text_append(o, b_2, "");
      k += step;
    }
  }

  struct minimap_jsonResult { Text Result = ""; };
  minimap_jsonResult minimap_json() {
    Vector me = Vector{0.0, 0.0, 0.0};
    Float range = 0.0;
    List<Game_minimap_json_Feat> feats = List<Game_minimap_json_Feat>{};
    List<Int64> tiles = List<Int64>{};
    Int64 tile = Int64{0x0};
    MapFeatures f = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
    MapFeatures_Road r = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    Bool skip = false;
    MapFeatures_Area a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    Int k = 0;
    Bool skip_2 = false;
    MapFeatures_Building b = MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false};
    Vector q = Vector{0.0, 0.0, 0.0};
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    Vector look = Vector{0.0, 0.0, 0.0};
    Vector fwd = Vector{0.0, 0.0, 0.0};
    U32 a_2 = 0;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Text o = "";
    Text b_2 = "";
    Game_minimap_json_Feat f_2 = Game_minimap_json_Feat{0, false, Int64{0x0}, 0, 0.0};
    MapFeatures fs = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
    if (logic_not(world_placed_)) {
      return {"{}"};
    }
    n23068715 = focus_position();
    me = n23068715.Result;
    range = 420.0;
    feats = List<Game_minimap_json_Feat>{};
    tiles = List<Int64>{};
    city_stream_cpp::CityStream_ready_tiles(city_, tiles);
    for (auto it23068716 = flow_for(n23068716, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int64>(tiles))))); it23068716.next();) {
      tile = tiles[n23068716.index];
      f = city_stream_cpp::CityStream_features(city_, tile).Result;
      for (auto it23068717 = flow_for(n23068717, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Road>(f.roads))))); it23068717.next();) {
        r = f.roads[n23068717.index];
        if (math_compare<">=">(kern::to<Float>(list_length<Vector>(r.points)), 2.0)) {
          list_add(feats, struct_make<Game_minimap_json_Feat>(kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 119.0 : 114.0); }, kern::to<Float>(tiles_cpp::road_is_water(r.cls).Result))), false, tile, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n23068717.index))), minimap_json_dist(r.points, me).Result));
        }
      }
      for (auto it23068718 = flow_for(n23068718, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Area>(f.areas))))); it23068718.next();) {
        skip = false;
        a = f.areas[n23068718.index];
        if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(a.rings)), 0.0), math_compare<"<">(kern::to<Float>(list_length<Vector>(a.rings[0].items)), 3.0))) {
          skip = true;
        }
        if (logic_not(skip)) {
          k = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::bxor(kern::math::band(((a != 0.0) ? 119.0 : ((a_2 != 0.0) ? 0.0 : 103.0)), 255.0), 128.0) - 128.0); }, kern::to<Float>(enum_equal<AreaKind>(a.kind, AreaKind::water)), kern::to<Float>(enum_equal<AreaKind>(a.kind, AreaKind::sand))));
          if (math_compare<"!=">(kern::to<Float>(k), 0.0)) {
            list_add(feats, struct_make<Game_minimap_json_Feat>(k, true, tile, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n23068718.index))), minimap_json_dist(a.rings[0].items, me).Result));
          }
        }
      }
      for (auto it23068719 = flow_for(n23068719, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(f.buildings))))); it23068719.next();) {
        skip_2 = false;
        b = f.buildings[n23068719.index];
        if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(b.rings)), 0.0), math_compare<"<">(kern::to<Float>(list_length<Vector>(b.rings[0].items)), 3.0))) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          q = b.rings[0].items[0];
          if (logic_or(math_compare<">">(kern::formula([](Float x, Float x_2) -> Float { return kern::math::abs((x - x_2)); }, vector_break(q).x, vector_break(me).x), kern::formula([](Float range) -> Float { return (range + 300.0); }, range)), math_compare<">">(kern::formula([](Float y, Float y_2) -> Float { return kern::math::abs((y - y_2)); }, vector_break(q).y, vector_break(me).y), kern::formula([](Float range) -> Float { return (range + 300.0); }, range)))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            list_add(feats, struct_make<Game_minimap_json_Feat>(98, true, tile, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n23068719.index))), minimap_json_dist(b.rings[0].items, me).Result));
          }
        }
      }
    }
    at = 0;
    kept = at;
    end = list_length<Game_minimap_json_Feat>(feats);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(minimap_json_match(feats[at], range).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n23068720, feats[at]);
          feats[kept] = n23068720.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(feats, kept, Game_minimap_json_Feat{0, false, Int64{0x0}, 0, 0.0});
    list_sort<&Game::minimap_json_match_2>(*this, feats);
    if (math_compare<">">(kern::to<Float>(list_length<Game_minimap_json_Feat>(feats)), 80.0)) {
      list_resize(feats, 80, Game_minimap_json_Feat{0, false, Int64{0x0}, 0, 0.0});
    }
    list_sort<&Game::minimap_json_match_3>(*this, feats);
    look = vector_sub(cam_.target, cam_.eye);
    fwd = look;
    a_2 = soldier_;
    if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0))) {
      a_2 = cars_[car_index_].actor;
    } else if (enum_equal<Mode>(mode_, Mode::jet)) {
      a_2 = jet_actor_;
    } else if (logic_and(enum_equal<Mode>(mode_, Mode::drone), drone_out_)) {
      a_2 = drone_actor_;
    }
    if (math_compare<"!=">(kern::to<Float>(a_2), kern::to<Float>(soldier_))) {
      n23068721 = matrix_of(a_2);
      m = n23068721.Result;
      fwd = vector_make(m.m[0], m.m[1], 0.0);
    }
    o = "";
    b_2 = "";
    b_2 = text_format<"{\"me\":[%.1f,%.1f,%.1f],\"bearing\":%.2f,\"look\":%.2f,\"mode\":\"%s\",\"f\":[">(159, vector_break(me).x, vector_break(me).y, kern::formula([](Float z, Float the_result) -> Float { return (z - the_result); }, vector_break(me).z, city_stream_cpp::CityStream_ground_height(city_, vector_break(me).x, vector_break(me).y).Result), minimap_json_compass(vector_break(fwd).x, vector_break(fwd).y).Result, minimap_json_compass(vector_break(look).x, vector_break(look).y).Result, game_cpp::mode_name(mode_).Result);
    o = text_append(o, b_2, "");
    for (auto it23068722 = flow_for(n23068722, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Game_minimap_json_Feat>(feats))))); it23068722.next();) {
      f_2 = feats[n23068722.index];
      b_2 = text_format<"%s[\"%c%s\"">(159, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n23068722.index), 0.0), ",", ""), f_2.kind, logic_select<Text>(f_2.poly, "p", ""));
      o = text_append(o, b_2, "");
      fs = city_stream_cpp::CityStream_features(city_, f_2.tile).Result;
      if (logic_not(f_2.poly)) {
        minimap_json_put(b_2, o, fs.roads[f_2.index].points, me);
      } else if (math_compare<"==">(kern::to<Float>(f_2.kind), 98.0)) {
        minimap_json_put(b_2, o, fs.buildings[f_2.index].rings[0].items, me);
      } else {
        minimap_json_put(b_2, o, fs.areas[f_2.index].rings[0].items, me);
      }
      o = text_append(o, "]", "");
    }
    return {text_append(o, "]}", "")};
  }

  void townsync_ready() {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<TownSync>(townsync_)), 0.0))) {
      townsync_ = list_make<TownSync, "1">(TownSync{1, false, false, false, List<U8>{}, List<town_LampRec>{}, List<town_MarkRec>{}, List<town_MarkRec>{}, List<U32>{0, 0, 0}, 0, 0, 0, 0});
    }
  }

  struct town_car_remoteResult { Bool Result = false; };
  KERN_PURE town_car_remoteResult town_car_remote(I32 car = 0) {
    return {logic_and(logic_and(logic_and(logic_and(puppet_world_, math_compare<">=">(kern::to<Float>(car), 0.0)), math_compare<">">(kern::formula([](Float car, Float a) -> Float { return ((car < 0.0) != (a < 0.0) ? (a < 0.0) : (car < a)); }, kern::to<Float>(car), kern::to<Float>(list_length<I32>(car_net_))), 0.5)), math_compare<">=">(kern::to<Float>(car_net_[car]), 0.0)), logic_not(logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(car), kern::to<Float>(car_index_)))))};
  }

  void townsync_car_hit(I32 car = 0, Float amount = 0.0, town_Cause cause = town_Cause::other) {
    List<U8> a = List<U8>{};
    town_CarHit a_2 = town_CarHit{0, 0.0, town_Cause::other};
    a_2 = struct_make<town_CarHit>(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(car_net_[car]))), amount, cause);
    n24117258 = town_sync_cpp::pack(a_2);
    a = n24117258.Result;
    net_session_cpp::Session_town_to_host(net_, a);
    townsync_ready();
    townsync_[0].car_hits[enum_index<town_Cause>(cause)] = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(townsync_[0].car_hits[enum_index<town_Cause>(cause)])));
  }

  void townsync_lamp(Int64 key = Int64{0x0}, U8 state = 0) {
    if (net_session::Session_active(net_).Result) {
      townsync_ready();
      list_add(townsync_[0].lamps_out, struct_make<town_LampRec>(key, state));
    }
  }

  void townsync_plank(Int64 key = Int64{0x0}, U8 door = 0, U8 doors = 0, I32 step = 0) {
    List<U8> a = List<U8>{};
    town_PlankRec a_2 = town_PlankRec{Int64{0x0}, 0, 0, 0};
    Int64 key_2 = Int64{0x0};
    List<U8> a_3 = List<U8>{};
    town_PlankStep a_4 = town_PlankStep{Int64{0x0}, 0, 0, 0};
    if (logic_or(logic_or(logic_or(logic_or(logic_not(net_session::Session_active(net_).Result), logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0))), math_compare<"<">(kern::to<Float>(doors), 1.0)), math_compare<">">(kern::to<Float>(doors), kern::to<Float>(kDoorsMax))), math_compare<">=">(kern::to<Float>(door), kern::to<Float>(doors)))) {
      return;
    }
    if (net_session::Session_hosting(net_).Result) {
      key_2 = key;
      map_add(n24117259, gaps_[0].planks, key_2);
      a_2 = struct_make<town_PlankRec>(key, door, doors, list_get<U8>(gaps_[0].planks[n24117259.index], door));
      n24117260 = town_sync_cpp::pack_4(a_2);
      a = n24117260.Result;
      net_session_cpp::Session_town_to_all(net_, a);
    } else {
      a_4 = struct_make<town_PlankStep>(key, door, doors, kern::to<Int>(kern::formula([](Float step) -> Float { return (kern::math::bxor(kern::math::band(step, 255.0), 128.0) - 128.0); }, kern::to<Float>(step))));
      n24117261 = town_sync_cpp::pack_3(a_4);
      a_3 = n24117261.Result;
      net_session_cpp::Session_town_to_host(net_, a_3);
    }
  }

  void town_mark(Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float strength = 0.0, Bool craters = false) {
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Vector arg = Vector{0.0, 0.0, 0.0};
    Float arg_2 = 0.0;
    Float arg_3 = 0.0;
    Float sim_time = 0.0;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0))) {
      return;
    }
    arg = at;
    arg_2 = radius;
    arg_3 = strength;
    engine_game_world_physics(n24117262, world_);
    physics = n24117262.result;
    sim_time = engine_game_world_sim_time(world_);
    world_marks_cpp::Marks_blast(life_[0].marks, city_, rng_, arg, arg_2, arg_3, physics, sim_time, craters);
    townsync_ready();
    list_add(townsync_[0].marks, struct_make<town_MarkRec>(at, radius, strength));
    if (math_compare<">">(kern::formula([](Float kMarks, Float a) -> Float { return ((kMarks < 0.0) != (a < 0.0) ? (a < 0.0) : (kMarks < a)); }, kern::to<Float>(kMarks), kern::to<Float>(list_length<town_MarkRec>(townsync_[0].marks))), 0.5)) {
      list_remove_at(townsync_[0].marks, 0);
    }
  }

  struct townsync_send_full_matchResult { Bool Result = false; };
  KERN_PURE townsync_send_full_matchResult townsync_send_full_match(const town_PlankRec& a, const town_PlankRec& b) {
    return {logic_select<Bool>(logic_not(int64_equal(a.key, b.key)), int64_less(a.key, b.key), math_compare<"<">(kern::to<Float>(a.door), kern::to<Float>(b.door)))};
  }

  void townsync_send_full(I32 to = 0) {
    List<town_LampRec> lamps = List<town_LampRec>{};
    List<Int64_Int_pair> list = List<Int64_Int_pair>{};
    List<town_PlankRec> planks = List<town_PlankRec>{};
    Bool skip = false;
    List<U8_row> list_2 = List<U8_row>{};
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0))) {
      return;
    }
    townsync_ready();
    lamps = List<town_LampRec>{};
    list = lamps_cpp::the_Field_broken_list_(gaps_[0].lamps).Result;
    for (auto it24117263 = flow_for(n24117263, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int64_Int_pair>(list))))); it24117263.next();) {
      list_add(lamps, struct_make<town_LampRec>(list[n24117263.index].first, list[n24117263.index].second));
    }
    planks = List<town_PlankRec>{};
    for (auto it24117264 = flow_for(n24117264, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<U8>>(gaps_[0].planks))))); it24117264.next();) {
      skip = false;
      if (logic_or(math_compare<"==">(kern::to<Float>(list_length<U8>(gaps_[0].planks[n24117264.index])), 0.0), math_compare<">">(kern::formula([](Float kDoorsMax, Float a) -> Float { return ((kDoorsMax < 0.0) != (a < 0.0) ? (a < 0.0) : (kDoorsMax < a)); }, kern::to<Float>(kDoorsMax), kern::to<Float>(list_length<U8>(gaps_[0].planks[n24117264.index]))), 0.5))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it24117265 = flow_for(n24117265, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<U8>(gaps_[0].planks[n24117264.index]))))); it24117265.next();) {
          if (math_compare<"<">(kern::to<Float>(gaps_[0].planks[n24117264.index][n24117265.index]), kern::to<Float>(kPlanksMax))) {
            list_add(planks, struct_make<town_PlankRec>(map_key_at<Int64, List<U8>>(gaps_[0].planks, n24117264.index), kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::band(index, 255.0); }, kern::to<Float>(n24117265.index))), kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 255.0); }, kern::to<Float>(list_length<U8>(gaps_[0].planks[n24117264.index])))), gaps_[0].planks[n24117264.index][n24117265.index]));
          }
        }
      }
    }
    list_sort<&Game::townsync_send_full_match>(*this, planks);
    n24117266 = town_sync_cpp::pack_state(lamps, planks, townsync_[0].marks, townsync_[0].epoch);
    list_2 = n24117266.Result;
    for (auto it24117267 = flow_for(n24117267, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8_row>(list_2))))); it24117267.next();) {
      if (math_compare<"<">(kern::to<Float>(to), 0.0)) {
        net_session_cpp::Session_town_to_all(net_, list_2[n24117267.index].items);
      } else {
        net_session_cpp::Session_town_to(net_, list_2[n24117267.index].items, kern::to<Int>(kern::formula([](Float to) -> Float { return kern::math::band(to, 255.0); }, kern::to<Float>(to))));
      }
      townsync_[0].sent_states = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(townsync_[0].sent_states)));
    }
  }

  void townsync_reset() {
    List<U8> a = List<U8>{};
    if (logic_not(net_session::Session_active(net_).Result)) {
      return;
    }
    townsync_ready();
    list_clear(townsync_[0].lamps_out);
    if (net_session::Session_hosting(net_).Result) {
      townsync_[0].epoch = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(townsync_[0].epoch)));
      townsync_send_full(-1);
    } else {
      a = town_sync_cpp::pack_ask().Result;
      net_session_cpp::Session_town_to_host(net_, a);
      townsync_[0].asks = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(townsync_[0].asks)));
    }
  }

  struct townsync_jsonResult { Text Result = ""; };
  townsync_jsonResult townsync_json() {
    Int open_doors = 0;
    U8 n = 0;
    Text b = "";
    townsync_ready();
    open_doors = 0;
    if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
      for (auto it24117268 = flow_for(n24117268, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<U8>>(gaps_[0].planks))))); it24117268.next();) {
        for (auto it24117269 = flow_for(n24117269, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U8>(gaps_[0].planks[n24117268.index]))))); it24117269.next();) {
          n = gaps_[0].planks[n24117268.index][n24117269.index];
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

  void eject_driver(I32 ci = 0) {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float yaw = 0.0;
    Vector side = Vector{0.0, 0.0, 0.0};
    Vector at = Vector{0.0, 0.0, 0.0};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    I32 p = 0;
    Vector arg = Vector{0.0, 0.0, 0.0};
    Float arg_2 = 0.0;
    Float now = 0.0;
    Float sim_time = 0.0;
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0)), math_compare<"<">(kern::to<Float>(ci), 0.0)), math_compare<">=">(kern::to<Float>(ci), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<TrafficCar>(cars_))))))))) {
      return;
    }
    n25165835 = state_of(cars_[ci].actor);
    s = n25165835.Result;
    yaw = common::heading_of(engine_math_math::Quat_rotate(s.rotation, Vector{1.0, 0.0, 0.0}).Result).Result;
    side = vector_make(kern::formula([](Float yaw) -> Float { return (-kern::math::sin(yaw)); }, yaw), kern::formula([](Float yaw) -> Float { return kern::math::cos(yaw); }, yaw), 0.0);
    at = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x + (x_2 * 1.8)); }, vector_break(s.position).x, vector_break(side).x), kern::formula([](Float y, Float y_2) -> Float { return (y + (y_2 * 1.8)); }, vector_break(s.position).y, vector_break(side).y), 0.0);
    engine_game_world_physics(n25165836, world_);
    physics = n25165836.result;
    if (struct_break<the_RayResult_>(common::cast(physics, s.position, vector_make(vector_break(side).x, vector_break(side).y, 0.0), 2.3, body(cars_[ci].actor).Result, Int64{0xffffffffffffffff}).Result).hit) {
      at = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - (x_2 * 1.8)); }, vector_break(s.position).x, vector_break(side).x), kern::formula([](Float y, Float y_2) -> Float { return (y - (y_2 * 1.8)); }, vector_break(s.position).y, vector_break(side).y), 0.0);
    }
    at = vector_make(vector_break(at).x, vector_break(at).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(at).x, vector_break(at).y).Result);
    arg = at;
    arg_2 = kern::formula([](Float yaw, Float kPi) -> Float { return (yaw + kern::math::div(kPi, 2.0)); }, yaw, kPi);
    n25165837 = common::Rng_next(rng_);
    n25165838 = people_cpp::People_add_citizen(people_, arg, arg_2, kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::bor(the_result, 1.0); }, kern::to<Float>(n25165837.Result))));
    p = n25165838.Result;
    if (logic_and(logic_and(math_compare<">=">(kern::to<Float>(cars_[ci].lane), 0.0), math_compare<"<">(kern::to<Float>(cars_[ci].lane), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Lane>(city_.lanes_)))))))), city_.lanes_[cars_[ci].lane].alive)) {
      people_.people_[p].lane = cars_[ci].lane;
      people_.people_[p].seg = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return (a - ((a_2 > 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(cars_[ci].index), kern::to<Float>(cars_[ci].dir))))), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::wrap32s(a) - 2.0); }, kern::to<Float>(list_length<Vector>(city_.lanes_[cars_[ci].lane].points)))))))));
      n25165839 = common::Rng_unit(rng_);
      people_.people_[p].dir = kern::to<Int>(kern::formula([](Float the_result) -> Float { return ((the_result < 0.5) ? 1.0 : (-1.0)); }, n25165839.Result));
      people_.people_[p].side = 1.0;
      people_.people_[p].off = kern::formula([](Float a) -> Float { return ((a * 0.5) + 1.7); }, city_.lanes_[cars_[ci].lane].width);
      people_.people_[p].along = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(at).x, vector_break(city_.lanes_[cars_[ci].lane].points[people_.people_[p].seg]).x, vector_break(at).y, vector_break(city_.lanes_[cars_[ci].lane].points[people_.people_[p].seg]).y);
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    n25165840 = common::Rng_unit(rng_);
    if (math_compare<"<">(n25165840.Result, 0.5)) {
      people_cpp::People_frighten(people_, s.position, 4.0, now, 1.2);
    } else {
      people_.people_[p].state = People_State::wait;
      people_.people_[p].wait_until = kern::formula([](Float now) -> Float { return (now + 2.6); }, now);
      people_.people_[p].yaw = kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(s.position).y, vector_break(at).y, vector_break(s.position).x, vector_break(at).x);
      list_add(life_[0].delayed_fright, struct_make<WorldLife_Fright>(p, kern::formula([](Float now) -> Float { return (now + 2.6); }, now), s.position));
    }
    life_[0].ejected = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(life_[0].ejected)));
  }

  struct Game_step_tow_followResult { Float Result = 0.0; };
  Game_step_tow_followResult Game_step_tow_follow(const BodyState& s, CarInput& in, Int ti = 0, Float now = 0.0, Float sp = 0.0, Float yaw = 0.0, Vector fwd = Vector{0.0, 0.0, 0.0}, Vector goal = Vector{0.0, 0.0, 0.0}, Float speed = 0.0) {
    Vector p = Vector{0.0, 0.0, 0.0};
    Float d = 0.0;
    Vector tp = Vector{0.0, 0.0, 0.0};
    Float herr = 0.0;
    Float want = 0.0;
    p = vector_make(vector_break(s.position).x, vector_break(s.position).y, 0.0);
    d = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(goal).x, vector_break(p).x, vector_break(goal).y, vector_break(p).y);
    if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector>(life_[0].trucks[ti].path)), 0.0), math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, life_[0].trucks[ti].path_at), 2.5))) {
      n25165850 = road_graph_cpp::RoadGraph_path(roads_, life_[0].trucks[ti].path, p, goal);
      if (logic_not(n25165850.Result)) {
        life_[0].trucks[ti].path = list_make<Vector, "2">(p, goal);
      }
      life_[0].trucks[ti].path_at = now;
    }
    tp = road_graph_cpp::RoadGraph_pursuit_point(life_[0].trucks[ti].path, p, kern::formula([](Float sp) -> Float { return kern::math::max(7.0, (sp * 1.1)); }, sp)).Result;
    herr = common::wrap_pi(kern::formula([](Float y, Float y_2, Float x, Float x_2, Float yaw) -> Float { return (kern::math::atan2((y - y_2), (x - x_2)) - yaw); }, vector_break(tp).y, vector_break(p).y, vector_break(tp).x, vector_break(p).x, yaw)).Result;
    if (math_compare<"<">(now, life_[0].trucks[ti].recover_until)) {
      if (logic_and(math_compare<"<">(kern::formula([](Float herr) -> Float { return kern::math::abs(herr); }, herr), 0.45), math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, life_[0].trucks[ti].recover_start), 0.5))) {
        life_[0].trucks[ti].recover_until = -1.0;
      } else {
        in = struct_make<CarInput>(0.0, 0.0, false, 32.0, 9.0);
        in.pedal = -0.75;
        in.steer = kern::formula([](Float herr) -> Float { return (-kern::math::sign(((herr != 0.0) ? herr : 1.0))); }, herr);
        in.max_reverse = 4.0;
        return {d};
      }
    }
    if (logic_and(math_compare<"<">(sp, 0.7), math_compare<">">(d, 6.0))) {
      if (math_compare<"<">(life_[0].trucks[ti].stuck_since, 0.0)) {
        life_[0].trucks[ti].stuck_since = now;
      }
      if (math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, life_[0].trucks[ti].stuck_since), 2.2)) {
        life_[0].trucks[ti].recover_start = now;
        life_[0].trucks[ti].recover_until = kern::formula([](Float now) -> Float { return (now + 1.7); }, now);
        life_[0].trucks[ti].stuck_since = -1.0;
        list_clear(life_[0].trucks[ti].path);
        return {d};
      }
    } else {
      life_[0].trucks[ti].stuck_since = -1.0;
    }
    want = kern::formula([](Float d, Float speed) -> Float { return ((d < 20.0) ? kern::math::max(3.0, (d * 0.5)) : speed); }, d, speed);
    in = car_cpp::ai_drive(s.position, fwd, car::Car_forward_speed(life_[0].trucks[ti].car[0]).Result, vector_make(vector_break(tp).x, vector_break(tp).y, vector_break(s.position).z), kern::formula([](Float speed, Float want) -> Float { return kern::math::min(speed, want); }, speed, want)).Result;
    return {d};
  }

  void step_tow(Float h = 0.0) {
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Float now = 0.0;
    Float sim_time = 0.0;
    Vector me = Vector{0.0, 0.0, 0.0};
    Int ti = 0;
    Bool skip = false;
    Bool remove = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool either = false;
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Float yaw = 0.0;
    Float sp = 0.0;
    Int job_at = 0;
    CarInput in = CarInput{0.0, 0.0, false, 32.0, 9.0};
    Bool left = false;
    Vector cp = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    I32 n = 0;
    I32 k = 0;
    Float left_2 = 0.0;
    I32 prev = 0;
    Vector d = Vector{0.0, 0.0, 0.0};
    Float dl = 0.0;
    Float off = 0.0;
    Bool left_3 = false;
    Vector goal = Vector{0.0, 0.0, 0.0};
    Float d_2 = 0.0;
    Vector back = Vector{0.0, 0.0, 0.0};
    Vector rot = Vector{0.0, 0.0, 0.0};
    physics_Pose rest = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool teleport = false;
    physics_Body jb = physics_Body{Int64{0x0}};
    Bool set_actor_velocity = false;
    Bool left_4 = false;
    Float d_3 = 0.0;
    Vector back_2 = Vector{0.0, 0.0, 0.0};
    physics_Body jb_2 = physics_Body{Int64{0x0}};
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_2 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_3 = Quat{0.0, 0.0, 0.0, 1.0};
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both = false;
    Int at = 0;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0))) {
      return;
    }
    engine_game_world_physics(n25165848, world_);
    physics = n25165848.result;
    w = physics;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    n25165849 = player_position();
    me = n25165849.Result;
    if (logic_not(int64_equal(roads_.built_from, road_graph_cpp::lane_signature(city_.lanes_).Result))) {
      road_graph_cpp::RoadGraph_rebuild(roads_, city_.lanes_);
    }
    ti = 0;
    while (loop(math_compare<">">(kern::formula([](Float ti, Float a) -> Float { return ((ti < 0.0) != (a < 0.0) ? (a < 0.0) : (ti < a)); }, kern::to<Float>(ti), kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))), 0.5))) {
      skip = false;
      either = logic_not(engine_physics_physics::Body_valid(life_[0].trucks[ti].body).Result);
      if (logic_not(either)) {
        body_valid_2 = engine_world_body_valid(w, life_[0].trucks[ti].body);
        either = logic_not(body_valid_2);
      }
      remove = either;
      if (logic_not(remove)) {
        s = common::read_body(w, life_[0].trucks[ti].body).Result;
        fwd = engine_math_math::Quat_rotate(s.rotation, Vector{1.0, 0.0, 0.0}).Result;
        yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(fwd).y, vector_break(fwd).x);
        sp = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(s.velocity).x, vector_break(s.velocity).y);
        job_at = logic_select<Int>(logic_and(math_compare<">=">(kern::to<Float>(life_[0].trucks[ti].job), 0.0), math_compare<"<">(kern::to<Float>(life_[0].trucks[ti].job), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<TrafficCar>(cars_)))))))), life_[0].trucks[ti].job, -1);
        in = CarInput{0.0, 0.0, false, 32.0, 9.0};
        in.handbrake = true;
        if (logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(job_at), 0.0), logic_not(enum_equal<WorldLife_TowTruck_State>(life_[0].trucks[ti].state, WorldLife_TowTruck_State::depot))), enum_equal<Mode>(mode_, Mode::car)), math_compare<"==">(kern::to<Float>(car_index_), kern::to<Float>(life_[0].trucks[ti].job)))) {
          life_[0].trucks[ti].state = WorldLife_TowTruck_State::depot;
          life_[0].trucks[ti].towing = false;
          list_clear(life_[0].trucks[ti].path);
        }
        if (logic_and(logic_not(enum_equal<WorldLife_TowTruck_State>(life_[0].trucks[ti].state, WorldLife_TowTruck_State::depot)), math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, life_[0].trucks[ti].born), kGiveUpS))) {
          life_[0].trucks[ti].state = WorldLife_TowTruck_State::depot;
          life_[0].trucks[ti].towing = false;
          list_clear(life_[0].trucks[ti].path);
        }
        switch (flow_switch_enum<WorldLife_TowTruck_State>(life_[0].trucks[ti].state)) {
          case WorldLife_TowTruck_State::to_job: {
            left = false;
            if (logic_or(logic_not(math_compare<">=">(kern::to<Float>(job_at), 0.0)), logic_not(cars_[job_at].placed))) {
              life_[0].trucks[ti].state = WorldLife_TowTruck_State::depot;
              left = true;
            }
            if (logic_not(logic_or(left, skip))) {
              n25165851 = matrix_of(cars_[job_at].actor);
              a = n25165851.Result;
              cp = engine_math_math::Mat4_translation(a).Result;
              n25165852 = Game_step_tow_follow(s, in, ti, now, sp, yaw, fwd, vector_make(vector_break(cp).x, vector_break(cp).y, 0.0), kDriveSpeed);
              if (math_compare<"<">(n25165852.Result, kHookM)) {
                in = struct_make<CarInput>(0.0, 0.0, false, 32.0, 9.0);
                in.handbrake = true;
                in.pedal = kern::formula([](Float the_result) -> Float { return ((-0.3) * kern::math::sign(the_result)); }, car::Car_forward_speed(life_[0].trucks[ti].car[0]).Result);
                life_[0].trucks[ti].state = WorldLife_TowTruck_State::hook;
                life_[0].trucks[ti].state_at = now;
              }
            }
          } break;
          case WorldLife_TowTruck_State::hook: {
            in = struct_make<CarInput>(0.0, 0.0, false, 32.0, 9.0);
            in.handbrake = true;
            if (logic_and(math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, life_[0].trucks[ti].state_at), kHookS), math_compare<">=">(kern::to<Float>(job_at), 0.0))) {
              life_[0].trucks[ti].towing = true;
              cars_[job_at].driver = logic_not(life_[0].trucks[ti].wreck_job);
              life_[0].trucks[ti].drop = vector_make(vector_break(s.position).x, vector_break(s.position).y, 0.0);
              life_[0].trucks[ti].drop_yaw = yaw;
              if (logic_and(logic_and(logic_and(logic_not(life_[0].trucks[ti].wreck_job), math_compare<">=">(kern::to<Float>(cars_[job_at].lane), 0.0)), math_compare<"<">(kern::to<Float>(cars_[job_at].lane), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Lane>(city_.lanes_)))))))), city_.lanes_[cars_[job_at].lane].alive)) {
                n = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Vector>(city_.lanes_[cars_[job_at].lane].points))));
                k = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(cars_[job_at].index), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n))))))));
                left_2 = 30.0;
                while (loop(logic_and(logic_and(math_compare<">">(left_2, 0.0), math_compare<">=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k, Float a) -> Float { return (k + a); }, kern::to<Float>(k), kern::to<Float>(cars_[job_at].dir))))), 0.0)), math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k, Float a) -> Float { return (k + a); }, kern::to<Float>(k), kern::to<Float>(cars_[job_at].dir))))), kern::to<Float>(n))))) {
                  left_2 = kern::formula([](Float left_2, Float x, Float x_2, Float y, Float y_2) -> Float { return (left_2 - kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2)))); }, left_2, vector_break(city_.lanes_[cars_[job_at].lane].points[kern::to<Int>(kern::formula([](Float k, Float a) -> Float { return (k + a); }, kern::to<Float>(k), kern::to<Float>(cars_[job_at].dir)))]).x, vector_break(city_.lanes_[cars_[job_at].lane].points[k]).x, vector_break(city_.lanes_[cars_[job_at].lane].points[kern::to<Int>(kern::formula([](Float k, Float a) -> Float { return (k + a); }, kern::to<Float>(k), kern::to<Float>(cars_[job_at].dir)))]).y, vector_break(city_.lanes_[cars_[job_at].lane].points[k]).y);
                  k += cars_[job_at].dir;
                }
                prev = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k, Float a) -> Float { return (k - a); }, kern::to<Float>(k), kern::to<Float>(cars_[job_at].dir))))), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n))))))));
                d = vector_sub(city_.lanes_[cars_[job_at].lane].points[k], city_.lanes_[cars_[job_at].lane].points[prev]);
                dl = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y);
                life_[0].trucks[ti].drop_yaw = kern::formula([](Float dl, Float y, Float x, Float yaw) -> Float { return ((dl > 0.001) ? kern::math::atan2(y, x) : yaw); }, dl, vector_break(d).y, vector_break(d).x, yaw);
                off = kern::formula([](Float a) -> Float { return kern::math::min((a * 0.25), 1.8); }, city_.lanes_[cars_[job_at].lane].width);
                life_[0].trucks[ti].drop = vector_make(kern::formula([](Float x, Float a, Float off) -> Float { return (x + (kern::math::sin(a) * off)); }, vector_break(city_.lanes_[cars_[job_at].lane].points[k]).x, life_[0].trucks[ti].drop_yaw, off), kern::formula([](Float y, Float a, Float off) -> Float { return (y - (kern::math::cos(a) * off)); }, vector_break(city_.lanes_[cars_[job_at].lane].points[k]).y, life_[0].trucks[ti].drop_yaw, off), 0.0);
                cars_[job_at].index = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k, Float a) -> Float { return (k + a); }, kern::to<Float>(k), kern::to<Float>(cars_[job_at].dir))))), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n))))))));
              }
              life_[0].trucks[ti].state = WorldLife_TowTruck_State::tow;
              list_clear(life_[0].trucks[ti].path);
            }
          } break;
          case WorldLife_TowTruck_State::tow: {
            left_3 = false;
            if (logic_or(logic_not(math_compare<">=">(kern::to<Float>(job_at), 0.0)), logic_not(cars_[job_at].placed))) {
              life_[0].trucks[ti].state = WorldLife_TowTruck_State::depot;
              life_[0].trucks[ti].towing = false;
              left_3 = true;
            }
            if (logic_not(logic_or(left_3, skip))) {
              goal = logic_select<Vector>(life_[0].trucks[ti].wreck_job, life_[0].trucks[ti].depot, vector_make(kern::formula([](Float x, Float a) -> Float { return (x + (kern::math::cos(a) * 5.5)); }, vector_break(life_[0].trucks[ti].drop).x, life_[0].trucks[ti].drop_yaw), kern::formula([](Float y, Float a) -> Float { return (y + (kern::math::sin(a) * 5.5)); }, vector_break(life_[0].trucks[ti].drop).y, life_[0].trucks[ti].drop_yaw), 0.0));
              n25165853 = Game_step_tow_follow(s, in, ti, now, sp, yaw, fwd, goal, kTowSpeed);
              d_2 = n25165853.Result;
              if (logic_and(logic_and(logic_not(life_[0].trucks[ti].wreck_job), math_compare<"<">(d_2, 4.5)), math_compare<"<">(sp, 2.5))) {
                life_[0].trucks[ti].towing = false;
                back = vector_make(kern::formula([](Float x, Float yaw) -> Float { return (x - (kern::math::cos(yaw) * 5.1)); }, vector_break(s.position).x, yaw), kern::formula([](Float y, Float yaw) -> Float { return (y - (kern::math::sin(yaw) * 5.1)); }, vector_break(s.position).y, yaw), 0.0);
                rot = vector_make(0.0, 0.0, kern::formula([](Float yaw, Float kDeg) -> Float { return kern::math::div(yaw, kDeg); }, yaw, kDeg));
                rest = car_rest_pose(cars_[job_at].car[0].s_, vector_break(back).x, vector_break(back).y, yaw).Result;
                engine_game_world_teleport(n25165854, world_, cars_[job_at].actor, rest.position, list_make<Vector, "1">(rot));
                teleport = n25165854.result;
                jb = body(cars_[job_at].actor).Result;
                if (engine_physics_physics::Body_valid(jb).Result) {
                  engine_world_set_pose(w, jb, rest);
                }
                engine_game_world_set_actor_velocity(n25165855, world_, cars_[job_at].actor, Vector{0.0, 0.0, 0.0});
                set_actor_velocity = n25165855.result;
                car_cpp::Car_reset(cars_[job_at].car[0]);
                life_[0].trucks[ti].state = WorldLife_TowTruck_State::depot;
                life_[0].trucks[ti].state_at = now;
                list_clear(life_[0].trucks[ti].path);
                life_[0].tow_done = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(life_[0].tow_done)));
                if (math_compare<"<">(kern::to<Float>(life_[0].trucks[ti].job), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<WorldLife_Watch>(life_[0].watch)))))))) {
                  life_[0].watch[life_[0].trucks[ti].job] = struct_make<WorldLife_Watch>(kern::formula([](Float now) -> Float { return (now + 60.0); }, now), -1.0, -1.0, -1.0);
                }
              } else if (logic_and(life_[0].trucks[ti].wreck_job, logic_or(math_compare<"<">(d_2, 12.0), math_compare<">">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(s.position).x, vector_break(me).x, vector_break(s.position).y, vector_break(me).y), 220.0)))) {
                remove = true;
              }
            }
          } break;
          case WorldLife_TowTruck_State::depot: {
            left_4 = false;
            if (math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, life_[0].trucks[ti].state_at), 1.2)) {
              in = struct_make<CarInput>(0.0, 0.0, false, 32.0, 9.0);
              in.handbrake = true;
              left_4 = true;
            }
            if (logic_not(logic_or(left_4, skip))) {
              n25165856 = Game_step_tow_follow(s, in, ti, now, sp, yaw, fwd, life_[0].trucks[ti].depot, kDriveSpeed);
              d_3 = n25165856.Result;
              if (logic_or(math_compare<"<">(d_3, 12.0), math_compare<">">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(s.position).x, vector_break(me).x, vector_break(s.position).y, vector_break(me).y), 200.0))) {
                remove = true;
              }
            }
          } break;
        }
        if (logic_and(life_[0].trucks[ti].towing, math_compare<">=">(kern::to<Float>(job_at), 0.0))) {
          back_2 = vector_make(kern::formula([](Float x, Float yaw) -> Float { return (x - (kern::math::cos(yaw) * 5.1)); }, vector_break(s.position).x, yaw), kern::formula([](Float y, Float yaw) -> Float { return (y - (kern::math::sin(yaw) * 5.1)); }, vector_break(s.position).y, yaw), 0.0);
          jb_2 = body(cars_[job_at].actor).Result;
          if (engine_physics_physics::Body_valid(jb_2).Result) {
            a_2 = common::yaw_quat(yaw).Result;
            a_3 = engine_math_math::Quat_axis_angle(Vector{0.0, 1.0, 0.0}, -0.08).Result;
            q = engine_math_math::Quat_mul(a_2, a_3).Result;
            engine_world_set_pose(w, jb_2, struct_make<physics_Pose>(vector_make(vector_break(back_2).x, vector_break(back_2).y, kern::formula([](Float the_result, Float the_result_2) -> Float { return ((the_result + the_result_2) + 0.35); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(back_2).x, vector_break(back_2).y).Result, car::Car_ground_offset(cars_[job_at].car[0].s_).Result)), q));
            engine_world_set_linear_velocity(w, jb_2, vector_make(vector_break(s.velocity).x, vector_break(s.velocity).y, 0.0));
            engine_world_set_angular_velocity(w, jb_2, Vector{0.0, 0.0, 0.0});
          }
        }
        life_[0].trucks[ti].input = in;
        car_cpp::Car_step(life_[0].trucks[ti].car[0], in, w, life_[0].trucks[ti].body, h);
        if (logic_and(math_compare<"<=">(life_[0].trucks[ti].hp, 0.0), life_[0].trucks[ti].exploded)) {
          remove = true;
        }
      }
      if (remove) {
        if (logic_and(logic_and(logic_and(life_[0].trucks[ti].towing, life_[0].trucks[ti].wreck_job), math_compare<">=">(kern::to<Float>(life_[0].trucks[ti].job), 0.0)), math_compare<"<">(kern::to<Float>(life_[0].trucks[ti].job), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<TrafficCar>(cars_))))))))) {
          engine_game_world_set_alive(world_, cars_[life_[0].trucks[ti].job].actor, false);
          cars_[life_[0].trucks[ti].job].placed = false;
          cars_[life_[0].trucks[ti].job].wreck = false;
          cars_[life_[0].trucks[ti].job].hp = car_max_hp(cars_[life_[0].trucks[ti].job]).Result;
          life_[0].tow_done = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(life_[0].tow_done)));
        }
        both = engine_physics_physics::Body_valid(life_[0].trucks[ti].body).Result;
        if (both) {
          body_valid_4 = engine_world_body_valid(w, life_[0].trucks[ti].body);
          both = body_valid_4;
        }
        if (both) {
          engine_world_destroy_body(w, life_[0].trucks[ti].body);
        }
        at = ti;
        if (math_compare<">">(kern::to<Float>(list_length<Car>(life_[0].trucks[at].car)), 0.0)) {
          car_cpp::Car_end(life_[0].trucks[at].car[0]);
        }
        list_remove_at(life_[0].trucks, at);
        skip = true;
      }
      if (logic_not(skip)) {
        ti += 1;
      }
    }
  }

  void draw_world_vehicles() {
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
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
    engine_game_world_physics(n25165857, world_);
    physics = n25165857.result;
    w = physics;
    for (auto it25165858 = flow_for(n25165858, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))))); it25165858.next();) {
      skip = false;
      either = logic_not(engine_physics_physics::Body_valid(life_[0].trucks[n25165858.index].body).Result);
      if (logic_not(either)) {
        body_valid_2 = engine_world_body_valid(w, life_[0].trucks[n25165858.index].body);
        either = logic_not(body_valid_2);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        pose = engine_world_pose(w, life_[0].trucks[n25165858.index].body);
        p = pose;
        m = engine_math_math::Mat4_trs(p.position, p.rotation, Vector{1.0, 1.0, 1.0}).Result;
        vehicle_art_cpp::VehicleArt_car(art_, m, life_[0].trucks[n25165858.index].car[0], kTowYellow, false, math_compare<"<">(life_[0].trucks[n25165858.index].input.pedal, -0.05));
      }
    }
  }

  struct car_towedResult { Bool Result = false; };
  car_towedResult car_towed(I32 car = 0) {
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
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Object<kern::physics::World> physics_2 = Object<kern::physics::World>{};
    Bool body_valid = false;
    Bool both = false;
    Object<kern::physics::World> physics_3 = Object<kern::physics::World>{};
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
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
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

  void ui_draw_gps_make(U32& id, const MeshData& m, Object<kern::Renderer> r = Object<kern::Renderer>{}, Vector color = Vector{0.0, 0.0, 0.0}, Float alpha = 0.0) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Material mat = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    r_ = r;
    mat = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    mat.unlit = true;
    mat.cast_shadow = false;
    mat.color = engine_math_math::Vec4_new(vector_break(color).x, vector_break(color).y, vector_break(color).z, alpha).Result;
    mat.alpha = logic_select<AlphaMode>(math_compare<"<">(alpha, 1.0), AlphaMode::blend, AlphaMode::opaque);
    mat.double_sided = true;
    engine_renderer_allocate_id(n27262994, r_);
    allocate_id = n27262994.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    look.material = mat;
    look.visible = false;
    engine_upsert(n27262995, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n27262995.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, id);
      id = 0;
    }
  }

  void ui_draw_gps_put(const Mat4& m, Object<kern::Renderer> r = Object<kern::Renderer>{}, U32 id = 0) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Bool update_instances = false;
    r_ = r;
    if (math_compare<"!=">(kern::to<Float>(id), 0.0)) {
      engine_renderer_update_instances(n27262999, r_, id, m.m, List<Float>{});
      update_instances = n27262999.result;
    }
  }

  /// The position puck and the track line on the ground (gps_live_mode.mjs ensureScene / pushTrailPoint).
  void ui_draw_gps(Float dt = 0.0) {
    Object<kern::Renderer> r = Object<kern::Renderer>{};
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    Vector blue = Vector{0.0, 0.0, 0.0};
    MeshData a = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData a_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData a_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData wedge = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Vector c = Vector{0.0, 0.0, 0.0};
    Float a0 = 0.0;
    Float a1 = 0.0;
    Bool show = false;
    U32 item = 0;
    U32 item_2 = 0;
    U32 item_3 = 0;
    U32 item_4 = 0;
    Fixed<U32, 4> items = Fixed<U32, 4>{};
    U32 id = 0;
    Bool set_visible = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    Float z = 0.0;
    Float acc = 0.0;
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int allocate_id = 0;
    Bool set_visible_2 = false;
    MeshData line = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Vector blue_2 = Vector{0.0, 0.0, 0.0};
    Vector item_5 = Vector{0.0, 0.0, 0.0};
    Vector item_6 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 2> items_2 = Fixed<Vector, 2>{};
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    Bool set_visible_3 = false;
    Bool set_visible_4 = false;
    renderer = engine_engine_renderer(engine_);
    r = renderer;
    if (logic_not(math_compare<"!=">(kern::to<Float>(ui_.puck_acc), 0.0))) {
      blue = Vector{0.26, 0.52, 0.96};
      cylinder = engine_primitives_cylinder(1.0, 0.01, 40, Vector{1.0, 1.0, 1.0});
      a = cylinder;
      ui_draw_gps_make(ui_.puck_acc, a, r, blue, 0.16);
      cylinder_2 = engine_primitives_cylinder(0.62, 0.012, 28, Vector{1.0, 1.0, 1.0});
      a_2 = cylinder_2;
      ui_draw_gps_make(ui_.puck_ring, a_2, r, Vector{1.0, 1.0, 1.0}, 1.0);
      cylinder_3 = engine_primitives_cylinder(0.48, 0.014, 28, Vector{1.0, 1.0, 1.0});
      a_3 = cylinder_3;
      ui_draw_gps_make(ui_.puck_dot, a_3, r, blue, 1.0);
      wedge = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
      c = Vector{1.0, 1.0, 1.0};
      for (auto it27262996 = flow_for(n27262996, 0, 11); it27262996.next();) {
        a0 = kern::formula([](Float index) -> Float { return ((-0.5) + kern::math::div(index, 12.0)); }, kern::to<Float>(n27262996.index));
        a1 = kern::formula([](Float index) -> Float { return ((-0.5) + kern::math::div((index + 1.0), 12.0)); }, kern::to<Float>(n27262996.index));
        engine_geo_mesh_data::MeshData_push(wedge, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}, c, Vector{0.0, 0.0, 0.0});
        engine_geo_mesh_data::MeshData_push(wedge, vector_make(kern::formula([](Float a0) -> Float { return (kern::math::sin(a0) * 4.2); }, a0), kern::formula([](Float a0) -> Float { return (kern::math::cos(a0) * 4.2); }, a0), 0.0), Vector{0.0, 0.0, 1.0}, c, Vector{0.0, 0.0, 0.0});
        engine_geo_mesh_data::MeshData_push(wedge, vector_make(kern::formula([](Float a1) -> Float { return (kern::math::sin(a1) * 4.2); }, a1), kern::formula([](Float a1) -> Float { return (kern::math::cos(a1) * 4.2); }, a1), 0.0), Vector{0.0, 0.0, 1.0}, c, Vector{0.0, 0.0, 0.0});
      }
      ui_draw_gps_make(ui_.puck_wedge, wedge, r, blue, 0.28);
    }
    show = logic_and(logic_and(gps_live::GpsLive_active(ui_.gps).Result, gps_live::GpsLive_has_fix(ui_.gps).Result), world_placed_);
    item = ui_.puck_acc;
    item_2 = ui_.puck_ring;
    item_3 = ui_.puck_dot;
    item_4 = ui_.puck_wedge;
    items = list_make<U32, "4">(item, item_2, item_3, item_4);
    for (auto it27262997 = flow_for(n27262997, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U32>(items))))); it27262997.next();) {
      id = items[n27262997.index];
      if (math_compare<"!=">(kern::to<Float>(id), 0.0)) {
        engine_renderer_set_visible(n27262998, r, id, show);
        set_visible = n27262998.result;
      }
    }
    if (show) {
      p = gps_live::GpsLive_shown(ui_.gps).Result;
      z = kern::formula([](Float the_result) -> Float { return (the_result + 0.05); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(p).x, vector_break(p).y).Result);
      acc = kern::formula([](Float the_result) -> Float { return kern::math::max(1.0, the_result); }, gps_live::GpsLive_accuracy(ui_.gps).Result);
      a_4 = engine_math_math::Mat4_trs(vector_make(vector_break(p).x, vector_break(p).y, z), struct_make<Quat>(0.0, 0.0, 0.0, 1.0), vector_make(acc, acc, 1.0)).Result;
      ui_draw_gps_put(a_4, r, ui_.puck_acc);
      a_5 = engine_math_math::Mat4_translate(vector_make(vector_break(p).x, vector_break(p).y, kern::formula([](Float z) -> Float { return (z + 0.01); }, z))).Result;
      ui_draw_gps_put(a_5, r, ui_.puck_ring);
      a_6 = engine_math_math::Mat4_translate(vector_make(vector_break(p).x, vector_break(p).y, kern::formula([](Float z) -> Float { return (z + 0.02); }, z))).Result;
      ui_draw_gps_put(a_6, r, ui_.puck_dot);
      a_7 = engine_math_math::Mat4_trs(vector_make(vector_break(p).x, vector_break(p).y, kern::formula([](Float z) -> Float { return (z + 0.03); }, z)), engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, kern::formula([](Float the_result) -> Float { return (-the_result); }, gps_live::GpsLive_heading(ui_.gps).Result)).Result, Vector{1.0, 1.0, 1.0}).Result;
      ui_draw_gps_put(a_7, r, ui_.puck_wedge);
    }
    ui_.trail_upload = kern::formula([](Float trail_upload, Float dt) -> Float { return (trail_upload + dt); }, ui_.trail_upload, dt);
    if (logic_and(ui_.trail_dirty, math_compare<">">(ui_.trail_upload, 0.5))) {
      ui_.trail_dirty = false;
      ui_.trail_upload = 0.0;
      if (logic_not(math_compare<"!=">(kern::to<Float>(ui_.trail_mesh), 0.0))) {
        engine_renderer_allocate_id(n27263000, r);
        allocate_id = n27263000.result;
        ui_.trail_mesh = allocate_id;
      }
      if (math_compare<"<">(kern::to<Float>(list_length<Vector>(ui_.trail)), 2.0)) {
        engine_renderer_set_visible(n27263001, r, ui_.trail_mesh, false);
        set_visible_2 = n27263001.result;
      } else {
        line = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
        blue_2 = Vector{0.26, 0.52, 0.96};
        for (auto it27263002 = flow_for(n27263002, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(ui_.trail))))); it27263002.next();) {
          item_5 = ui_.trail[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n27263002.index)))];
          item_6 = ui_.trail[n27263002.index];
          items_2 = list_make<Vector, "2">(item_5, item_6);
          for (auto it27263003 = flow_for(n27263003, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(items_2))))); it27263003.next();) {
            engine_geo_mesh_data::MeshData_push(line, vector_make(vector_break(items_2[n27263003.index]).x, vector_break(items_2[n27263003.index]).y, kern::formula([](Float the_result) -> Float { return (the_result + 0.12); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(items_2[n27263003.index]).x, vector_break(items_2[n27263003.index]).y).Result)), Vector{0.0, 0.0, 1.0}, blue_2, Vector{0.0, 0.0, 0.0});
          }
        }
        look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
        look.material.lines = true;
        look.material.unlit = true;
        look.material.cast_shadow = false;
        look.material.frustum_culled = false;
        look.visible = true;
        engine_upsert(n27263004, r, ui_.trail_mesh, line, look, List<Float>{}, List<Float>{});
        upsert = n27263004.result;
        if (logic_not(upsert)) {
          engine_renderer_release_id(r, ui_.trail_mesh);
          ui_.trail_mesh = 0;
        }
      }
    }
    if (logic_and(math_compare<"!=">(kern::to<Float>(ui_.trail_mesh), 0.0), logic_not(gps_live::GpsLive_active(ui_.gps).Result))) {
      engine_renderer_set_visible(n27263005, r, ui_.trail_mesh, false);
      set_visible_3 = n27263005.result;
    } else if (logic_and(logic_and(math_compare<"!=">(kern::to<Float>(ui_.trail_mesh), 0.0), math_compare<">=">(kern::to<Float>(list_length<Vector>(ui_.trail)), 2.0)), logic_not(ui_.trail_dirty))) {
      engine_renderer_set_visible(n27263006, r, ui_.trail_mesh, true);
      set_visible_4 = n27263006.result;
    }
  }

  /// (colored: every point has its colour in col; else all are `color`)
  void ui_draw_helpers_line_mesh(U32& id, const List<Float>& pos, const List<Float>& col, Object<kern::Renderer> r = Object<kern::Renderer>{}, Bool colored = false, Vector color = Vector{0.0, 0.0, 0.0}) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Int allocate_id = 0;
    MeshData m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Int i = 0;
    Vector c = Vector{0.0, 0.0, 0.0};
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    r_ = r;
    if (logic_not(math_compare<"!=">(kern::to<Float>(id), 0.0))) {
      engine_renderer_allocate_id(n27263007, r_);
      allocate_id = n27263007.result;
      id = allocate_id;
    }
    m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Float>(pos))), 0.5))) {
      c = logic_select<Vector>(colored, vector_make(col[i], col[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))], col[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i)))]), color);
      engine_geo_mesh_data::MeshData_push(m, vector_make(pos[i], pos[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))], pos[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i)))]), Vector{0.0, 0.0, 1.0}, c, Vector{0.0, 0.0, 0.0});
      i += 3;
    }
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    look.material.lines = true;
    look.material.unlit = true;
    look.material.cast_shadow = false;
    look.material.frustum_culled = false;
    look.material.linear_colors = colored;
    look.visible = logic_not(math_compare<"==">(kern::to<Float>(list_length<Float>(pos)), 0.0));
    engine_upsert(n27263008, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n27263008.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, id);
      id = 0;
    }
  }

  /// settings_help.mjs worldGrid (1 m reference grid around the player) and box3dColliderDebug (Box3D's collision shapes as wire frames). Visual only.
  void ui_draw_helpers() {
    Object<kern::Renderer> r = Object<kern::Renderer>{};
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    Bool grid = false;
    Vector me = Vector{0.0, 0.0, 0.0};
    Float cx = 0.0;
    Float cy = 0.0;
    List<Float> p = List<Float>{};
    I32 n = 0;
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
      n27263009 = player_position();
      me = n27263009.Result;
      cx = kern::formula([](Float x) -> Float { return kern::math::round(x); }, vector_break(me).x);
      cy = kern::formula([](Float y) -> Float { return kern::math::round(y); }, vector_break(me).y);
      if (logic_or(logic_or(logic_not(math_compare<"!=">(kern::to<Float>(ui_.grid_mesh), 0.0)), math_compare<">=">(kern::formula([](Float cx, Float x) -> Float { return kern::math::abs((cx - x)); }, cx, vector_break(ui_.grid_at).x), 1.0)), math_compare<">=">(kern::formula([](Float cy, Float y) -> Float { return kern::math::abs((cy - y)); }, cy, vector_break(ui_.grid_at).y), 1.0))) {
        ui_.grid_at = vector_make(cx, cy, 0.0);
        p = List<Float>{};
        n = 20;
        for (auto it27263010 = flow_for(n27263010, kern::to<Int>(kern::formula([](Float n) -> Float { return (-n); }, kern::to<Float>(n))), n); it27263010.next();) {
          for (auto it27263011 = flow_for(n27263011, kern::to<Int>(kern::formula([](Float n) -> Float { return (-n); }, kern::to<Float>(n))), kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it27263011.next();) {
            a = kern::to<Float>(n27263011.index);
            b = kern::formula([](Float index) -> Float { return (index + 1.0); }, kern::to<Float>(n27263011.index));
            o = kern::to<Float>(n27263010.index);
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
      engine_renderer_set_visible(n27263012, r, ui_.grid_mesh, false);
      set_visible = n27263012.result;
      ui_.grid_at = Vector{1000000000.0, 1000000000.0, 0.0};
    }
    if (logic_and(settings::Settings_on(ui_.settings, Opt::debug_colliders).Result, world_placed_)) {
      ui_.collider_clock = kern::formula([](Float collider_clock) -> Float { return (collider_clock + 1.0); }, ui_.collider_clock);
      if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float collider_clock) -> Float { return kern::math::mod(kern::math::trunc(collider_clock), 6.0); }, ui_.collider_clock)))), 0.0)) {
        pos = List<Float>{};
        col = List<Float>{};
        engine_game_world_debug_lines(n27263013, world_, true, false, false, pos, col);
        pos = n27263013.positions;
        col = n27263013.colors;
        ui_draw_helpers_line_mesh(ui_.collider_mesh, pos, col, r, true, Vector{0.0, 0.0, 0.0});
      }
    } else if (math_compare<"!=">(kern::to<Float>(ui_.collider_mesh), 0.0)) {
      engine_renderer_set_visible(n27263014, r, ui_.collider_mesh, false);
      set_visible_2 = n27263014.result;
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

  void ui_before_input(Object<kern::Engine> e = Object<kern::Engine>{}) {
    ui_.head_yaw_before = head_yaw_;
    ui_.head_pitch_before = head_pitch_;
  }

  void ui_clear_police() {
    for (auto it27263015 = flow_for(n27263015, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it27263015.next();) {
      if (logic_and(math_compare<">=">(kern::to<Float>(units_[n27263015.index].car), 0.0), math_compare<"!=">(kern::to<Float>(units_[n27263015.index].car), kern::to<Float>(car_index_)))) {
        engine_game_world_set_alive(world_, cars_[units_[n27263015.index].car].actor, false);
        cars_[units_[n27263015.index].car].placed = false;
      }
      if (math_compare<">=">(kern::to<Float>(units_[n27263015.index].car), 0.0)) {
        cars_[units_[n27263015.index].car].unit = -1;
      }
    }
    list_clear(units_);
    for (auto it27263016 = flow_for(n27263016, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it27263016.next();) {
      pdrones_[n27263016.index].active = false;
    }
    drone_wave_live_ = false;
    next_drone_wave_ = -1.0;
    star_since_ = -1.0;
  }

  void ui_mix_audio(SoundState& s) {
    Object<kern::audio::Mixer> m = Object<kern::audio::Mixer>{};
    Float fx = 0.0;
    m = mixer_;
    engine_mixer_bus(n27263017, m, audio_Bus::master);
    n27263017.result.muted = logic_not(settings::Settings_on(ui_.settings, Opt::audio_on).Result);
    engine_mixer_bus(n27263018, m, audio_Bus::sfx);
    n27263018.result.volume = kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 72.0); }, settings::Settings_get(ui_.settings, Opt::audio_fx).Result);
    engine_mixer_bus(n27263019, m, audio_Bus::ambience);
    n27263019.result.volume = kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 46.0); }, settings::Settings_get(ui_.settings, Opt::audio_ambient).Result);
    fx = kern::formula([](Float the_result) -> Float { return kern::math::max(0.01, kern::math::div(the_result, 72.0)); }, settings::Settings_get(ui_.settings, Opt::audio_fx).Result);
    s.drone_mix = kern::formula([](Float the_result, Float fx) -> Float { return kern::math::div(kern::math::div(the_result, 50.0), fx); }, settings::Settings_get(ui_.settings, Opt::audio_drone).Result, fx);
    s.vehicle_mix = kern::formula([](Float the_result, Float fx) -> Float { return kern::math::div(kern::math::div(the_result, 58.0), fx); }, settings::Settings_get(ui_.settings, Opt::audio_vehicle).Result, fx);
    if (math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(ui_.paused))), kern::to<Float>(value_int(kern::to<Int>(ui_.audio_paused))))) {
      engine_mixer_set_all_paused(m, ui_.paused);
      ui_.audio_paused = ui_.paused;
    }
    ui_mix_equipment(0.0);
  }

  void ui_mix_cues() {
    Float fx = 0.0;
    fx = kern::formula([](Float the_result) -> Float { return kern::math::max(0.01, kern::math::div(the_result, 72.0)); }, settings::Settings_get(ui_.settings, Opt::audio_fx).Result);
    engine_game_world_sound_cues(n27263020, world_);
    for (auto it27263021 = flow_for(n27263021, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_SoundCue>(n27263020.result))))); it27263021.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(n27263020.result[n27263021.index].actor), kern::to<Float>(soldier_)), logic_or(logic_or(text_equal(n27263020.result[n27263021.index].sound, "sc:shot0"), text_equal(n27263020.result[n27263021.index].sound, "sc:pistol0")), text_equal(n27263020.result[n27263021.index].sound, "builtin:sniper")))) {
        osfx("foot.shot", n27263020.result[n27263021.index].position, kern::to<Float>(enum_index<weapons_Id>(foot_weapon_)), 0.0, 0.0);
      }
      if (logic_or(text_equal(n27263020.result[n27263021.index].sound, "builtin:step"), text_equal(n27263020.result[n27263021.index].sound, "builtin:land"))) {
        n27263020.result[n27263021.index].volume = kern::formula([](Float a, Float the_result, Float fx) -> Float { return (a * kern::math::div(kern::math::div(the_result, 30.0), fx)); }, n27263020.result[n27263021.index].volume, settings::Settings_get(ui_.settings, Opt::audio_steps).Result, fx);
      } else if (logic_or(logic_or(logic_or(logic_or(text_equal(n27263020.result[n27263021.index].sound, "builtin:pistol"), text_equal(n27263020.result[n27263021.index].sound, "builtin:smg")), text_equal(n27263020.result[n27263021.index].sound, "builtin:sniper")), text_equal(n27263020.result[n27263021.index].sound, "builtin:rifle")), text_equal(n27263020.result[n27263021.index].sound, "builtin:shotgun"))) {
        n27263020.result[n27263021.index].volume = kern::formula([](Float a, Float the_result, Float fx) -> Float { return (a * kern::math::div(kern::math::div(the_result, 82.0), fx)); }, n27263020.result[n27263021.index].volume, settings::Settings_get(ui_.settings, Opt::audio_shots).Result, fx);
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
    n27263022 = ui_equipment_json();
    return {text_append(s, n27263022.Result, "")};
  }

  struct car_max_hpResult { Float Result = 0.0; };
  KERN_PURE car_max_hpResult car_max_hp(const TrafficCar& c) {
    return {kern::formula([](Float police, Float a) -> Float { return ((police != 0.0) ? 300.0 : ((a != 0.0) ? 320.0 : 180.0)); }, kern::to<Float>(c.police), kern::to<Float>(game_vehicle_damage_cpp::is_bus(c).Result))};
  }

  void car_bullet(I32 i = 0, Bool by_player = false) {
    Bool sniper = false;
    Float dmg = 0.0;
    if (logic_or(math_compare<"<">(kern::to<Float>(i), 0.0), math_compare<">=">(kern::to<Float>(i), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<TrafficCar>(cars_))))))))) {
      return;
    }
    sniper = logic_and(enum_equal<Mode>(mode_, Mode::foot), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper));
    dmg = kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? 22.0 : ((a_2 != 0.0) ? 26.0 : 30.0)); }, kern::to<Float>(cars_[i].police), kern::to<Float>(game_vehicle_damage_cpp::is_bus(cars_[i]).Result));
    if (sniper) {
      dmg = kern::formula([](Float a, Float the_result) -> Float { return ((a != 0.0) ? (4.0 * 22.0) : kern::math::max(1.0, kern::math::round((the_result * 0.34)))); }, kern::to<Float>(cars_[i].police), car_max_hp(cars_[i]).Result);
    }
    damage_car(i, dmg, by_player, town_Cause::round_);
  }

  void blast_cars(Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float damage = 0.0, Bool by_player = false, Bool police_only = false) {
    Bool skip = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float r = 0.0;
    Float md = 0.0;
    Float dd = 0.0;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector ax = Vector{0.0, 0.0, 0.0};
    Vector ay = Vector{0.0, 0.0, 0.0};
    Vector az = Vector{0.0, 0.0, 0.0};
    Vector h = Vector{0.0, 0.0, 0.0};
    Vector o = Vector{0.0, 0.0, 0.0};
    Vector lo = Vector{0.0, 0.0, 0.0};
    Vector cl = Vector{0.0, 0.0, 0.0};
    Vector near = Vector{0.0, 0.0, 0.0};
    Float r_2 = 0.0;
    Float d = 0.0;
    Float f = 0.0;
    Bool both = false;
    Float arg = 0.0;
    Float arg_2 = 0.0;
    Float hits = 0.0;
    for (auto it28311562 = flow_for(n28311562, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it28311562.next();) {
      skip = false;
      if (logic_or(logic_or(logic_not(cars_[n28311562.index].placed), cars_[n28311562.index].wreck), logic_and(police_only, logic_not(cars_[n28311562.index].police)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        n28311563 = matrix_of(cars_[n28311562.index].actor);
        a = n28311563.Result;
        p = engine_math_math::Mat4_translation(a).Result;
        if (cars_[n28311562.index].police) {
          r = kern::formula([](Float radius) -> Float { return (radius < 2.0 ? 2.0 : (60.0 < radius ? 60.0 : radius)); }, radius);
          md = kern::formula([](Float damage) -> Float { return kern::math::max(20.0, damage); }, damage);
          dd = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(p).x, vector_break(at).x, vector_break(p).y, vector_break(at).y);
          if (math_compare<"<">(dd, kern::formula([](Float r) -> Float { return (r + 2.5); }, r))) {
            damage_car(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n28311562.index))), kern::formula([](Float md, Float dd, Float r) -> Float { return ((md * 2.4) * (1.0 - kern::math::div(dd, (r + 2.5)))); }, md, dd, r), by_player, town_Cause::blast);
          }
          skip = true;
        }
        if (logic_not(skip)) {
          n28311564 = matrix_of(cars_[n28311562.index].actor);
          m = n28311564.Result;
          ax = logic_select<Vector>(math_compare<">">(vector_length(engine_math_math::Mat4_transform_vector(m, Vector{1.0, 0.0, 0.0}).Result), 9.999999717180685e-10), vector_normalize(engine_math_math::Mat4_transform_vector(m, Vector{1.0, 0.0, 0.0}).Result), Vector{1.0, 0.0, 0.0});
          ay = logic_select<Vector>(math_compare<">">(vector_length(engine_math_math::Mat4_transform_vector(m, Vector{0.0, 1.0, 0.0}).Result), 9.999999717180685e-10), vector_normalize(engine_math_math::Mat4_transform_vector(m, Vector{0.0, 1.0, 0.0}).Result), Vector{0.0, 1.0, 0.0});
          az = logic_select<Vector>(math_compare<">">(vector_length(engine_math_math::Mat4_transform_vector(m, Vector{0.0, 0.0, 1.0}).Result), 9.999999717180685e-10), vector_normalize(engine_math_math::Mat4_transform_vector(m, Vector{0.0, 0.0, 1.0}).Result), Vector{0.0, 0.0, 1.0});
          h = vector_add(cars_[n28311562.index].car[0].s_.half, Vector{0.0, 0.0, 0.6});
          o = vector_sub(at, p);
          lo = vector_make(vector_dot(o, ax), vector_dot(o, ay), vector_dot(o, az));
          cl = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x < (-x_2) ? (-x_2) : (x_2 < x ? x_2 : x)); }, vector_break(lo).x, vector_break(h).x), kern::formula([](Float y, Float y_2) -> Float { return (y < (-y_2) ? (-y_2) : (y_2 < y ? y_2 : y)); }, vector_break(lo).y, vector_break(h).y), kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, vector_break(lo).z, kern::formula([](Float z) -> Float { return (-z); }, vector_break(h).z), kern::formula([](Float z) -> Float { return (z + 0.6); }, vector_break(h).z))));
          near = vector_add(vector_add(vector_add(p, vector_scale(ax, vector_break(cl).x)), vector_scale(ay, vector_break(cl).y)), vector_scale(az, vector_break(cl).z));
          r_2 = kern::formula([](Float radius) -> Float { return (radius < 2.0 ? 2.0 : (18.0 < radius ? 18.0 : radius)); }, radius);
          d = vector_length(vector_sub(at, near));
          if (math_compare<">=">(d, r_2)) {
            skip = true;
          }
          if (logic_not(skip)) {
            arg = d;
            arg_2 = r_2;
            both = math_compare<">">(d, 0.8);
            if (both) {
              n28311565 = line_of_sight(at, near);
              both = logic_not(n28311565.Result);
            }
            f = weapons::blast_factor(arg, arg_2, both).Result;
            hits = kern::formula([](Float f, Float a) -> Float { return kern::math::min(5.0, kern::math::ceil((f * ((a != 0.0) ? 4.0 : 3.0)))); }, f, kern::to<Float>(game_vehicle_damage_cpp::is_bus(cars_[n28311562.index]).Result));
            for (auto it28311566 = flow_for(n28311566, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float hits) -> Float { return kern::math::trunc(hits); }, hits))))))); it28311566.next();) {
              damage_car(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n28311562.index))), kern::formula([](Float a) -> Float { return ((a != 0.0) ? 26.0 : 30.0); }, kern::to<Float>(game_vehicle_damage_cpp::is_bus(cars_[n28311562.index]).Result)), by_player, town_Cause::blast);
            }
          }
        }
      }
    }
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
    both = object_valid<kern::Engine>(engine_);
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
      both_2 = object_valid<kern::Engine>(engine_);
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
  vr_hand_matrixResult vr_hand_matrix(Mat4& out, I32 side = 0) {
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
    either = logic_or(logic_not(vr_), logic_not(object_valid<kern::Engine>(engine_)));
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
  KERN_PURE vr_grab_validResult vr_grab_valid(Object<kern::physics::World> w = Object<kern::physics::World>{}, physics_Body b = physics_Body{Int64{0x0}}) {
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

  void vr_set_hand(I32 side = 0, Bool valid = false, Vector p = Vector{0.0, 0.0, 0.0}, Quat q = Quat{0.0, 0.0, 0.0, 1.0}) {
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
    n29360143 = vr_hand_matrix(m, 1);
    if (logic_not(n29360143.Result)) {
      return {false};
    }
    from = engine_math_math::Mat4_translation(m).Result;
    dir = logic_select<Vector>(math_compare<">">(vector_length(vector_make(kern::formula([](Float a) -> Float { return (-a); }, m.m[8]), kern::formula([](Float a) -> Float { return (-a); }, m.m[9]), kern::formula([](Float a) -> Float { return (-a); }, m.m[10]))), 9.999999717180685e-10), vector_normalize(vector_make(kern::formula([](Float a) -> Float { return (-a); }, m.m[8]), kern::formula([](Float a) -> Float { return (-a); }, m.m[9]), kern::formula([](Float a) -> Float { return (-a); }, m.m[10]))), Vector{1.0, 0.0, 0.0});
    return {true};
  }

  void vr_scope_update() {
    Object<kern::Renderer> r = Object<kern::Renderer>{};
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool on = false;
    Bool xr = false;
    Bool xr_2 = false;
    Bool both = false;
    Bool both_2 = false;
    Bool set_visible = false;
    U32 fbo = 0;
    Int create_render_target = 0;
    MeshData d = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Vector n = Vector{0.0, 0.0, 0.0};
    I32 kSeg = 0;
    Float a = 0.0;
    Int place = 0;
    U32 item = 0;
    U32 item_2 = 0;
    U32 item_3 = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
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
      n29360144 = vr_hand_matrix(m, 1);
      both_2 = n29360144.Result;
    }
    on = both_2;
    engine_engine_set_xr_scope(engine_, false);
    if (logic_not(on)) {
      if (math_compare<"!=">(kern::to<Float>(vr_scope_mesh_), 0.0)) {
        engine_renderer_set_visible(n29360145, r, vr_scope_mesh_, false);
        set_visible = n29360145.result;
      }
      return;
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(vr_scope_tex_), 0.0))) {
      fbo = 0;
      engine_renderer_create_render_target(n29360146, r, 512, fbo);
      create_render_target = n29360146.result;
      fbo = n29360146.framebuffer;
      vr_scope_tex_ = create_render_target;
      engine_engine_set_xr_scope_fbo(engine_, fbo);
      engine_engine_set_xr_scope_size(engine_, 512);
      if (logic_not(math_compare<"!=">(kern::to<Float>(vr_scope_tex_), 0.0))) {
        return;
      }
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(vr_scope_mesh_), 0.0))) {
      d = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
      n = Vector{0.0, -1.0, 0.0};
      engine_geo_mesh_data::MeshData_push(d, Vector{0.0, 0.0, 0.0}, n, Vector{1.0, 1.0, 1.0}, Vector{0.5, 0.5, 0.0});
      kSeg = 28;
      for (auto it29360147 = flow_for(n29360147, 0, kSeg); it29360147.next();) {
        a = kern::formula([](Float index, Float kSeg, Float kPi) -> Float { return ((kern::math::div(index, kSeg) * 2.0) * kPi); }, kern::to<Float>(n29360147.index), kern::to<Float>(kSeg), kPi);
        engine_geo_mesh_data::MeshData_push(d, vector_make(kern::formula([](Float a) -> Float { return (kern::math::cos(a) * 0.5); }, a), 0.0, kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.5); }, a)), n, Vector{1.0, 1.0, 1.0}, vector_make(kern::formula([](Float a) -> Float { return (0.5 + (kern::math::cos(a) * 0.5)); }, a), kern::formula([](Float a) -> Float { return (0.5 - (kern::math::sin(a) * 0.5)); }, a), 0.0));
        if (math_compare<">">(kern::to<Float>(n29360147.index), 0.0)) {
          place = list_length<U32>(d.indices);
          item = 0;
          item_2 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n29360147.index)));
          item_3 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32((index + 1.0)); }, kern::to<Float>(n29360147.index)));
          list_insert_list(d.indices, place, list_make<U32, "3">(item, item_2, item_3));
        }
      }
      engine_renderer_allocate_id(n29360148, r);
      allocate_id = n29360148.result;
      vr_scope_mesh_ = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
      look.material.unlit = true;
      look.material.double_sided = true;
      look.material.cast_shadow = false;
      look.material.frustum_culled = false;
      look.extra_slot = kMaxExtensionAttributes;
      look.texture = vr_scope_tex_;
      engine_upsert(n29360149, r, vr_scope_mesh_, d, look, List<Float>{}, List<Float>{});
      upsert = n29360149.result;
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
    engine_renderer_update_instances(n29360150, r, vr_scope_mesh_, lm, List<Float>{});
    update_instances = n29360150.result;
    engine_renderer_set_visible(n29360151, r, vr_scope_mesh_, true);
    set_visible_2 = n29360151.result;
  }

  void vr_update_view() {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool both = false;
    if (object_valid<kern::Engine>(engine_)) {
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
    if (object_valid<kern::Engine>(engine_)) {
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

  void cycle_foot_weapon(I32 step = 0) {
    I32 n = 0;
    I32 cur = 0;
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
      both_2 = object_valid<kern::Engine>(engine_);
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
    proj_serial_ = kern::to<Int>(kern::formula([](Float proj_serial_) -> Float { return (proj_serial_ + 1.0); }, kern::to<Float>(proj_serial_)));
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
    n30408750 = aim_ray(dir);
    eye = n30408750.Result;
    the_actor_velocity = engine_game_world_actor_velocity(world_, soldier_);
    v = vector_add(vector_add(vector_scale(dir, kGrenadeThrow), Vector{0.0, 0.0, 3.5}), the_actor_velocity);
    fire_projectile(weapons_Id::hand_grenade, vector_add(eye, vector_scale(dir, 0.5)), v, true, body(soldier_).Result);
  }

  void draw_projectiles() {
    for (auto it30408759 = flow_for(n30408759, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it30408759.next();) {
      switch (flow_switch_enum<weapons_Id>(projectiles_[n30408759.index].id)) {
        case weapons_Id::smg: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::glock: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::sniper: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::grenade_launcher: {
        } break;
        case weapons_Id::rocket: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::fists: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::knife: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::hand_grenade: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.05, 0.14, false);
        } break;
        case weapons_Id::drone_gun: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::drone_rockets: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::drone_guided: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::jet_cannon: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::jet_missile: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.12, 2.8, true);
        } break;
        case weapons_Id::jet_bomb: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408759.index].pos, projectiles_[n30408759.index].vel, 0.25, 1.9, false);
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

  void Game_step_world_water(Object<kern::physics::World> w = Object<kern::physics::World>{}, U32 slot = 0, U32 a = 0, Vector half = Vector{0.0, 0.0, 0.0}, Bool drone = false) {
    Object<kern::physics::World> w_ = Object<kern::physics::World>{};
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

  void step_world(Float h = 0.0) {
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Int i = 0;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0))) {
      return;
    }
    engine_game_world_physics(n31457292, world_);
    physics = n31457292.result;
    w = physics;
    if (jet_placed_) {
      Game_step_world_water(w, 0, jet_actor_, Vector{0.8, 8.1, 0.8}, false);
    }
    if (drone_out_) {
      Game_step_world_water(w, 1, drone_actor_, Vector{0.12, 0.12, 0.04}, true);
    }
    if (enum_equal<Mode>(mode_, Mode::parachute)) {
      Game_step_world_water(w, 2, chute_actor_, Vector{0.3, 0.3, 0.85}, false);
    }
    i = 0;
    while (loop(logic_and(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<TrafficCar>(cars_))), 0.5), math_compare<">">(kern::formula([](Float a) -> Float { return ((a < 0.0) != (32.0 < 0.0) ? (32.0 < 0.0) : (a < 32.0)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 3.0); }, kern::to<Float>(i)))))), 0.5)))) {
      if (cars_[i].placed) {
        Game_step_world_water(w, kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32((i + 3.0)); }, kern::to<Float>(i))), cars_[i].actor, cars_[i].car[0].s_.half, false);
      }
      i += 1;
    }
    step_tow(h);
  }

  void world_bullet(Vector at = Vector{0.0, 0.0, 0.0}) {
    if (math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0)) {
      n31457294 = world_marks_cpp::Marks_bullet(life_[0].marks, city_, rng_, at);
    }
  }

  void world_blast(Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float damage = 0.0, Bool by_player = false) {
    Bool skip = false;
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Object<kern::physics::World> physics_2 = Object<kern::physics::World>{};
    Bool body_valid = false;
    Bool either = false;
    Float d = 0.0;
    Object<kern::physics::World> physics_3 = Object<kern::physics::World>{};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0))) {
      return;
    }
    if (logic_and(terrain_cpp::Terrain_water_at(city_.terrain_, vector_break(at).x, vector_break(at).y).Result, math_compare<"<">(kern::formula([](Float z, Float the_result) -> Float { return (z - the_result); }, vector_break(at).z, terrain_cpp::Terrain_water_level(city_.terrain_, vector_break(at).x, vector_break(at).y).Result), radius))) {
      list_add(life_[0].pending_splashes, struct_make<WorldLife_Splash>(vector_make(vector_break(at).x, vector_break(at).y, terrain_cpp::Terrain_water_level(city_.terrain_, vector_break(at).x, vector_break(at).y).Result), kern::formula([](Float radius) -> Float { return (2.0 + (radius * 0.8)); }, radius)));
      return;
    }
    for (auto it31457295 = flow_for(n31457295, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))))); it31457295.next();) {
      skip = false;
      either = logic_not(engine_physics_physics::Body_valid(life_[0].trucks[n31457295.index].body).Result);
      if (logic_not(either)) {
        engine_game_world_physics(n31457297, world_);
        physics_2 = n31457297.result;
        body_valid = engine_world_body_valid(physics_2, life_[0].trucks[n31457295.index].body);
        either = logic_not(body_valid);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        engine_game_world_physics(n31457298, world_);
        physics_3 = n31457298.result;
        pose = engine_world_pose(physics_3, life_[0].trucks[n31457295.index].body);
        d = vector_length(vector_sub(struct_break<physics_Pose>(pose).position, at));
        if (math_compare<"<">(d, radius)) {
          life_[0].trucks[n31457295.index].hp = kern::formula([](Float a, Float damage, Float d, Float radius) -> Float { return (a - ((damage * 1.3) * kern::math::pow((1.0 - kern::math::div(d, radius)), 1.2))); }, life_[0].trucks[n31457295.index].hp, damage, d, radius);
        }
      }
    }
    town_mark(at, radius, kern::formula([](Float damage) -> Float { return kern::math::min(1.4, kern::math::div(damage, 110.0)); }, damage), settings::Settings_on(ui_.settings, Opt::world_terrain).Result);
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
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
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
    U32 flocks = 0;
    U32 pigeons = 0;
    U32 pigeons_dead = 0;
    U32 cows = 0;
    Bool skip_3 = false;
    Bool skip_4 = false;
    o = text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append(text_append("{\"puppet\":", logic_select<Text>(puppet_world_, "1", "0"), ""), ",\"refused\":", ""), text_format<"%u">(-1, net_session::Session_send_refused(net_).Result), ""), ",\"sent\":", ""), text_format<"%u">(-1, world_sent_), ""), ",\"got\":", ""), text_format<"%u">(-1, world_got_), ""), ",\"mates\":", ""), text_format<"%llu">(-1, list_length<Vector>(mate_foci().Result)), ""), ",\"cars\":[", "");
    b = "";
    first = true;
    n32505869 = focus_position();
    me = n32505869.Result;
    for (auto it32505870 = flow_for(n32505870, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it32505870.next();) {
      skip = false;
      either = logic_or(logic_not(cars_[n32505870.index].placed), cars_[n32505870.index].police);
      if (logic_not(either)) {
        actor_alive_2 = engine_game_world_actor_alive(world_, cars_[n32505870.index].actor);
        either = logic_not(actor_alive_2);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        n32505871 = matrix_of(cars_[n32505870.index].actor);
        a = n32505871.Result;
        p = engine_math_math::Mat4_translation(a).Result;
        if (math_compare<">">(game_worldsync_cpp::dist2d(p, me).Result, 260.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          b = text_format<"%s[%.1f,%.1f,%u]">(95, logic_select<Text>(first, "", ","), vector_break(p).x, vector_break(p).y, kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::bor(kern::math::bor(kern::math::wrap32s(kern::math::shl(a, 16.0)), kern::math::wrap32s(kern::math::shl(a_2, 8.0))), a_3); }, kern::to<Float>(game_worldsync_cpp::col8(vector_break(cars_[n32505870.index].color).x).Result), kern::to<Float>(game_worldsync_cpp::col8(vector_break(cars_[n32505870.index].color).y).Result), kern::to<Float>(game_worldsync_cpp::col8(vector_break(cars_[n32505870.index].color).z).Result))));
          o = text_append(o, b, "");
          first = false;
        }
      }
    }
    o = text_append(o, "],\"people\":[", "");
    first = true;
    for (auto it32505872 = flow_for(n32505872, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<People_Person>(people_.people_))))); it32505872.next();) {
      skip_2 = false;
      if (logic_or(logic_or(logic_not(enum_equal<People_Kind>(people_.people_[n32505872.index].kind, People_Kind::citizen)), enum_equal<People_State>(people_.people_[n32505872.index].state, People_State::gone)), math_compare<">">(game_worldsync_cpp::dist2d(people_.people_[n32505872.index].pos, me).Result, 200.0))) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        b = text_format<"%s[%u,%.1f,%.1f]">(95, logic_select<Text>(first, "", ","), people_.people_[n32505872.index].seed, vector_break(people_.people_[n32505872.index].pos).x, vector_break(people_.people_[n32505872.index].pos).y);
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
      for (auto it32505873 = flow_for(n32505873, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(glife_[0].life.pets_))))); it32505873.next();) {
        skip_3 = false;
        if (logic_or(logic_not(glife_[0].life.pets_[n32505873.index].shown), math_compare<">">(game_worldsync_cpp::dist2d(glife_[0].life.pets_[n32505873.index].pos, me).Result, 200.0))) {
          skip_3 = true;
        }
        if (logic_not(skip_3)) {
          b = text_format<"%s[%zu,%.2f,%.2f,%d]">(95, logic_select<Text>(first, "", ","), n32505873.index, vector_break(glife_[0].life.pets_[n32505873.index].pos).x, vector_break(glife_[0].life.pets_[n32505873.index].pos).y, kern::to<Int>(kern::formula([](Float a) -> Float { return ((a >= 0.0) ? 1.0 : 0.0); }, glife_[0].life.pets_[n32505873.index].dead_at)));
          o = text_append(o, b, "");
          first = false;
        }
      }
      for (auto it32505874 = flow_for(n32505874, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it32505874.next();) {
        skip_4 = false;
        if (math_compare<">">(game_worldsync_cpp::dist2d(vector_make(vector_break(glife_[0].life.flocks_[n32505874.index].at).x, vector_break(glife_[0].life.flocks_[n32505874.index].at).y, 0.0), me).Result, 250.0)) {
          skip_4 = true;
        }
        if (logic_not(skip_4)) {
          flocks += 1;
          for (auto it32505875 = flow_for(n32505875, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pigeon>(glife_[0].life.flocks_[n32505874.index].birds))))); it32505875.next();) {
            pigeons += 1;
            pigeons_dead += kern::to<Int>(math_compare<">=">(glife_[0].life.flocks_[n32505874.index].birds[n32505875.index].dead_at, 0.0));
          }
        }
      }
      for (auto it32505876 = flow_for(n32505876, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_))))); it32505876.next();) {
        cows += kern::to<Int>(math_compare<"<">(game_worldsync_cpp::dist2d(glife_[0].life.cows_[n32505876.index].pos, me).Result, 250.0));
      }
    }
    o = text_append(o, "],\"flockList\":[", "");
    first = true;
    if (math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)) {
      for (auto it32505877 = flow_for(n32505877, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it32505877.next();) {
        b = text_format<"%s[%u,%zu,%.0f,%.0f,%.0f]">(95, logic_select<Text>(first, "", ","), glife_[0].life.flocks_[n32505877.index].id, list_length<Life_Pigeon>(glife_[0].life.flocks_[n32505877.index].birds), vector_break(glife_[0].life.flocks_[n32505877.index].at).x, vector_break(glife_[0].life.flocks_[n32505877.index].at).y, game_worldsync_cpp::dist2d(vector_make(vector_break(glife_[0].life.flocks_[n32505877.index].at).x, vector_break(glife_[0].life.flocks_[n32505877.index].at).y, 0.0), me).Result);
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
    Object<kern::physics::World> w = Object<kern::physics::World>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    mp_PoseSample s = mp_PoseSample{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, false, false, false};
    Float now_ms = 0.0;
    physics_BodyType body_type = physics_BodyType::fixed;
    BodyState here = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    if (logic_or(logic_or(logic_or(logic_not(puppet_world_), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (!((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a))); }, kern::to<Float>(i), kern::to<Float>(list_length<I32>(car_net_))), 0.5)), math_compare<"<">(kern::to<Float>(car_net_[i]), 0.0)), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32s(i); }, kern::to<Float>(i))))), kern::to<Float>(car_index_)))) {
      return {false};
    }
    b = body(c.actor).Result;
    if (logic_or(logic_not(engine_physics_physics::Body_valid(b).Result), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return (!((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a))); }, kern::to<Float>(i), kern::to<Float>(list_length<mp_PoseTimeline>(car_line_))), 0.5))) {
      return {true};
    }
    engine_game_world_physics(n32505878, world_);
    physics = n32505878.result;
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

  KERN_EXPORT_AS("sc_tile_data", "i32 i32 i32 bytes at len") void sc_tile_data(I32 z = 0, I32 x = 0, I32 y = 0, List<U8> bytes = List<U8>{}, Int bytes_at = 0, U32 length = 0) {
    TileKey a = TileKey{14, 0, 0};
    List<U8> part = List<U8>{};
    a = struct_make<TileKey>(z, x, y);
    part = List<U8>{};
    list_resize(part, length, 0);
    list_copy_bytes<"u8", "u8">(part, 0, bytes, bytes_at, length);
    city_stream_cpp::CityStream_provide(city_, a, part);
  }

  KERN_EXPORT_AS("sc_tile_failed", "i32 i32 i32") void sc_tile_failed(I32 z = 0, I32 x = 0, I32 y = 0) {
    TileKey a = TileKey{14, 0, 0};
    a = struct_make<TileKey>(z, x, y);
    city_stream_cpp::CityStream_fail(city_, a);
  }

  KERN_EXPORT_AS("sc_offline", "") void sc_offline() {
    city_stream_cpp::CityStream_go_offline(city_);
  }

  /// RGBA pixels of a requested elevation tile (`length` = width x height x 4 bytes, copied).
  KERN_EXPORT_AS("sc_dem_data", "i32 i32 i32 bytes at len u32 u32") void sc_dem_data(I32 z = 0, I32 x = 0, I32 y = 0, List<U8> rgba = List<U8>{}, Int rgba_at = 0, U32 length = 0, U32 width = 0, U32 height = 0) {
    TileKey a = TileKey{14, 0, 0};
    List<U8> part = List<U8>{};
    a = struct_make<TileKey>(z, x, y);
    part = List<U8>{};
    list_resize(part, length, 0);
    list_copy_bytes<"u8", "u8">(part, 0, rgba, rgba_at, length);
    terrain_cpp::Terrain_provide(city_.terrain_, a, part, width, height);
  }

  KERN_EXPORT_AS("sc_dem_failed", "i32 i32 i32") void sc_dem_failed(I32 z = 0, I32 x = 0, I32 y = 0) {
    TileKey a = TileKey{14, 0, 0};
    a = struct_make<TileKey>(z, x, y);
    terrain_cpp::Terrain_fail(city_.terrain_, a);
  }

  /// RGBA pixels of a requested aerial tile (`length` = width x height x 4 bytes, copied).
  KERN_EXPORT_AS("sc_image_data", "i32 i32 i32 bytes at len u32 u32") void sc_image_data(I32 z = 0, I32 x = 0, I32 y = 0, List<U8> rgba = List<U8>{}, Int rgba_at = 0, U32 length = 0, U32 width = 0, U32 height = 0) {
    TileKey a = TileKey{14, 0, 0};
    List<U8> part = List<U8>{};
    a = struct_make<TileKey>(z, x, y);
    part = List<U8>{};
    list_resize(part, length, 0);
    list_copy_bytes<"u8", "u8">(part, 0, rgba, rgba_at, length);
    terrain_cpp::Terrain_provide_image(city_.terrain_, a, part, width, height);
  }

  KERN_EXPORT_AS("sc_image_failed", "i32 i32 i32") void sc_image_failed(I32 z = 0, I32 x = 0, I32 y = 0) {
    TileKey a = TileKey{14, 0, 0};
    a = struct_make<TileKey>(z, x, y);
    terrain_cpp::Terrain_fail_image(city_.terrain_, a);
  }

  /// ---- input the App host does not forward ----
  KERN_EXPORT_AS("sc_gamepad", "i32 f32 f32 f32 f32 f32 f32 u32 i32") void sc_gamepad(I32 connected = 0, Float lx = 0.0, Float ly = 0.0, Float rx = 0.0, Float ry = 0.0, Float lt = 0.0, Float rt = 0.0, U32 buttons = 0, I32 active = 0) {
    pad.pressed = kern::to<Int>(kern::formula([](Float buttons, Float buttons_2) -> Float { return kern::math::band(buttons, kern::math::wrap32(kern::math::bxor(buttons_2, -1.0))); }, kern::to<Float>(buttons), kern::to<Float>(pad.buttons)));
    pad.connected = math_compare<"!=">(kern::to<Float>(connected), 0.0);
    pad.lx = lx;
    pad.ly = ly;
    pad.rx = rx;
    pad.ry = ry;
    pad.lt = lt;
    pad.rt = rt;
    pad.buttons = buttons;
    pad.active = math_compare<"!=">(kern::to<Float>(active), 0.0);
  }

  /// VR: a controller's pointing ray in the room (WebXR local space: x right, y up, -z forward).
  KERN_EXPORT_AS("sc_vr_hand", "i32 i32 f32 f32 f32 f32 f32 f32 f32") void sc_vr_hand(I32 side = 0, I32 valid = 0, Float px = 0.0, Float py = 0.0, Float pz = 0.0, Float qx = 0.0, Float qy = 0.0, Float qz = 0.0, Float qw = 0.0) {
    vr_set_hand(side, math_compare<"!=">(kern::to<Float>(valid), 0.0), vector_make(px, py, pz), struct_make<Quat>(qx, qy, qz, qw));
  }

  /// Mouse movement under the pointer lock (movementX/Y of mousemove, CSS px, clamped +-420 per event by the page: desktop_controls.mjs MAX_EVENT_PX) and the lock state (game_controls.cpp).
  KERN_EXPORT_AS("sc_mouse", "f32 f32") void sc_mouse(Float dx = 0.0, Float dy = 0.0) {
    mouse_move(dx, dy);
  }

  KERN_EXPORT_AS("sc_pointer_lock", "i32") void sc_pointer_lock(I32 locked = 0) {
    pointer_locked(math_compare<"!=">(kern::to<Float>(locked), 0.0));
  }

  /// Viewport in CSS px, safe-area insets and the device pixel ratio.
  KERN_EXPORT_AS("sc_touch_frame", "f32 f32 f32 f32 f32 f32 f32") void sc_touch_frame(Float w = 0.0, Float h = 0.0, Float sl = 0.0, Float st = 0.0, Float sr = 0.0, Float sb = 0.0, Float dpr = 0.0) {
    set_touch_frame(w, h, sl, st, sr, sb, dpr);
  }

  /// ---- multiplayer: the page moves the datagrams (KernNet) ---- A datagram from peer `from` (split into two 32-bit halves).
  KERN_EXPORT_AS("sc_net_push", "u32 u32 bytes at len") void sc_net_push(U32 from_lo = 0, U32 from_hi = 0, List<U8> bytes = List<U8>{}, Int bytes_at = 0, U32 length = 0) {
    List<U8> part = List<U8>{};
    Int64 arg = Int64{0x0};
    arg = int64_or(int64_shift_left(int64_of_int(from_hi), 32), int64_of_int(from_lo));
    part = List<U8>{};
    list_resize(part, length, 0);
    list_copy_bytes<"u8", "u8">(part, 0, bytes, bytes_at, length);
    net_session::Session_push(net_, arg, part);
  }

  struct sc_audio_initResult { I32 Result = 0; };
  /// ---- audio: Kern's mixer renders, the page plays (KernAudio) ----
  KERN_EXPORT_AS("sc_audio_init", "u32 -> i32") sc_audio_initResult sc_audio_init(U32 rate = 0) {
    n33554442 = audio_init(rate);
    return {kern::to<Int>(kern::formula([](Float the_result) -> Float { return ((the_result != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(n33554442.Result)))};
  }

  struct bindResult { Bool Result = false; };
  bindResult bind() {
    Bool runs = false;
    runs = func_has_blueprint(actor_find("Multiplayer"));
    list_clear(mp_rules_g_palette);
    mp_kSpawnProtectMs = func_get_on<"SpawnProtectMs", Float>(actor_find("Multiplayer"));
    mp_kRegenDelayMs = func_get_on<"RegenDelayMs", Float>(actor_find("Multiplayer"));
    mp_kRegenTickMs = func_get_on<"RegenTickMs", Float>(actor_find("Multiplayer"));
    mp_kRespawnCooldownMs = func_get_on<"RespawnCooldownMs", Float>(actor_find("Multiplayer"));
    mp_kBallotMs = func_get_on<"BallotMs", Float>(actor_find("Multiplayer"));
    mp_kPingLifeMs = func_get_on<"PingLifeMs", Float>(actor_find("Multiplayer"));
    mp_kPingHoldMs = func_get_on<"PingHoldMs", Float>(actor_find("Multiplayer"));
    mp_kRegenHpPerS = func_get_on<"RegenHpPerS", Float>(actor_find("Multiplayer"));
    mp_kRespawnRadiusMin = func_get_on<"RespawnRadiusMin", Float>(actor_find("Multiplayer"));
    mp_kRespawnRadiusMax = func_get_on<"RespawnRadiusMax", Float>(actor_find("Multiplayer"));
    mp_kVoiceRadius = func_get_on<"VoiceRadius", Float>(actor_find("Multiplayer"));
    mp_kPingRemoveM = func_get_on<"PingRemoveM", Float>(actor_find("Multiplayer"));
    mp_rules_g_palette = func_get_on<"Palette", List<U32>>(actor_find("Multiplayer"));
    return {runs};
  }

  struct paletteResult { List<U32> Result = List<U32>{}; };
  KERN_PURE paletteResult palette() {
    return {mp_rules_g_palette};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { combat_GateRow gate = combat_GateRow{"", Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false}; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n1048586;
  struct { Bool result = false; game_SoldierState out = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0}; } n2097162;
  matrix_ofResult n2097163;
  matrix_ofResult n2097164;
  struct : kern::Node<2097165> { Int index = 0; } n2097165;
  struct : kern::Node<2097166> { Int index = 0; } n2097166;
  struct : kern::Node<2097167> { Int index = 0; } n2097167;
  matrix_ofResult n2097168;
  struct : kern::Node<2097169> { Int index = 0; } n2097169;
  struct : kern::Node<2097170> { Int index = 0; } n2097170;
  struct : kern::Node<2097171> { Int index = 0; } n2097171;
  struct : kern::Node<2097172> { Int index = 0; } n2097172;
  struct { Bool added = false; Int index = 0; } n3145738;
  struct { Bool added = false; Int index = 0; } n3145739;
  struct { Bool added = false; Int index = 0; } n3145740;
  struct : kern::Node<3145741> { Int index = 0; } n3145741;
  struct { Bool added = false; Int index = 0; } n3145742;
  gaps_barricade_doorsResult n3145743;
  struct { Bool added = false; Int index = 0; } n3145744;
  struct : kern::Node<3145745> { Int index = 0; } n3145745;
  struct { Bool added = false; Int index = 0; } n3145746;
  common::Rng_unitResult n3145747;
  struct { Bool added = false; Int index = 0; } n3145748;
  combat_sfxResult n3145749;
  gaps_barricade_doorsResult n3145750;
  player_positionResult n3145751;
  struct : kern::Node<3145752> { Int index = 0; } n3145752;
  struct { Bool added = false; Int index = 0; } n3145753;
  struct { Bool added = false; Int index = 0; } n3145754;
  combat_sfxResult n3145755;
  player_positionResult n3145756;
  struct : kern::Node<3145757> { Int index = 0; } n3145757;
  struct { Int result = 0; } n3145758;
  struct { Bool result = false; } n3145759;
  gaps_barricade_doorsResult n3145761;
  struct : kern::Node<3145762> { Int index = 0; } n3145762;
  struct : kern::Node<3145763> { Int index = 0; } n3145763;
  struct { Bool result = false; } n3145764;
  struct { Bool result = false; Bool changed = false; } n3145765;
  struct { Bool result = false; } n3145766;
  vr_hand_rayResult n4194317;
  touch_play_cpp::TouchPlay_view_jsonResult n4194365;
  struct : kern::Node<4194366> { Int index = 0; } n4194366;
  struct : kern::Node<5242894> { Int index = 0; } n5242894;
  struct : kern::Node<5242897> { Int index = 0; } n5242897;
  struct : kern::Node<5242898> { Int index = 0; } n5242898;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n5242899;
  matrix_ofResult n5242900;
  matrix_ofResult n5242901;
  matrix_ofResult n5242902;
  gaps_ride_poseResult n5242904;
  matrix_ofResult n5242905;
  matrix_ofResult n5242906;
  player_positionResult n5242907;
  struct : kern::Node<5242908> { Int index = 0; } n5242908;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n5242909;
  struct : kern::Node<5242910> { Int index = 0; } n5242910;
  Game_clear_spot_on_roadResult n5242912;
  matrix_ofResult n5242913;
  clear_spotResult n5242914;
  clear_spotResult n5242915;
  matrix_ofResult n5242916;
  struct { Bool result = false; } n5242917;
  matrix_ofResult n5242918;
  struct : kern::Node<5242919> { Int index = 0; } n5242919;
  struct : kern::Node<5242920> { Int index = 0; } n5242920;
  city_stream_cpp::CityStream_street_pointResult n5242921;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n5242922;
  standing_feetResult n5242923;
  struct { Bool result = false; } n5242924;
  struct { Bool result = false; } n5242925;
  struct : kern::Node<5242927> { Int index = 0; } n5242927;
  struct { Bool result = false; } n5242933;
  matrix_ofResult n5242934;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n5242935;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n5242936;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n5242937;
  struct { Bool result = false; } n5242938;
  struct { Bool result = false; } n5242939;
  struct { Bool result = false; } n5242940;
  struct { Bool result = false; } n5242941;
  struct { Bool result = false; } n5242942;
  matrix_ofResult n5242943;
  matrix_ofResult n5242944;
  standing_feetResult n5242945;
  struct { Bool result = false; } n5242946;
  struct { Bool result = false; } n5242947;
  struct { Bool result = false; } n5242948;
  struct { Bool result = false; } n5242949;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n5242950;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n5242951;
  state_ofResult n5242952;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n5242953;
  state_ofResult n5242954;
  state_ofResult n5242955;
  struct { Bool result = false; } n5242956;
  struct { Bool result = false; } n5242957;
  common::Rng_nextResult n5242962;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n5242963;
  struct : kern::Node<5242965> { Int index = 0; } n5242965;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n5242967;
  struct { Bool result = false; Camera camera = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}}; } n5242968;
  matrix_ofResult n5242969;
  state_ofResult n5242970;
  Game_update_camera_collideResult n5242971;
  matrix_ofResult n5242972;
  state_ofResult n5242973;
  Game_update_camera_collideResult n5242974;
  matrix_ofResult n5242975;
  state_ofResult n5242976;
  Game_update_camera_collideResult n5242977;
  matrix_ofResult n5242978;
  Game_update_camera_collideResult n5242979;
  struct { Bool result = false; } n5242980;
  struct { Object<kern::game::SoundBank> result = Object<kern::game::SoundBank>{}; } n5242981;
  struct { Object<kern::game::GameAudio> result = Object<kern::game::GameAudio>{}; } n5242982;
  sound_fx_cpp::SoundFx_initResult n5242983;
  struct { List<Float> out = List<Float>{}; } n5242984;
  struct : kern::Node<5242985> { Int index = 0; } n5242985;
  struct : kern::Node<5242986> { Int index = 0; } n5242986;
  struct : kern::Node<5242987> { Int index = 0; } n5242987;
  struct : kern::Node<6291466> { Int index = 0; } n6291466;
  game_destruction_sync_cpp::digest_ofResult n6291484;
  struct : kern::Node<7340047> { Int index = 0; } n7340047;
  struct : kern::Node<7340048> { Int index = 0; } n7340048;
  struct : kern::Node<7340049> { Int index = 0; } n7340049;
  struct : kern::Node<7340050> { Int index = 0; } n7340050;
  combat_audio_cpp::renderResult n7340051;
  struct : kern::Node<7340052> { Int index = 0; } n7340052;
  sfx_idResult n7340053;
  struct { Bool result = false; } n7340054;
  sfx_idResult n7340055;
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
  matrix_ofResult n8388643;
  struct { Object<kern::Input> result = Object<kern::Input>{}; } n8388644;
  touch_play_cpp::TouchPlay_take_pressedResult n8388645;
  struct { Object<kern::Input> result = Object<kern::Input>{}; } n8388646;
  matrix_ofResult n8388647;
  struct { Int64 result = Int64{0x0}; } n8388648;
  matrix_ofResult n8388649;
  struct : kern::Node<8388650> { Int index = 0; } n8388650;
  struct : kern::Node<8388651> { Int index = 0; } n8388651;
  nostr_cpp::from_hexResult n8388652;
  nostr_cpp::from_hexResult n8388654;
  nostr_cpp::event_jsonResult n8388655;
  struct { Bool result = false; } n8388656;
  combat_rules_cpp::Chaos_awardResult n9437212;
  struct : kern::Node<9437215> { Int index = 0; } n9437215;
  struct { Debris value = Debris{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 6.5}; } n9437216;
  struct : kern::Node<9437217> { Int index = 0; } n9437217;
  struct : kern::Node<10485770> { Int index = 0; } n10485770;
  struct : kern::Node<10485771> { Int index = 0; } n10485771;
  struct : kern::Node<10485772> { Int index = 0; } n10485772;
  state_ofResult n10485773;
  remote_on_segmentResult n10485774;
  combat_sfxResult n10485775;
  struct : kern::Node<10485776> { Int index = 0; } n10485776;
  ctl_screen_rayResult n10485777;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n10485778;
  rocket_laser_dotResult n10485779;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n10485780;
  people_cpp::People_shotResult n10485781;
  round_meets_mateResult n10485782;
  soft_target_nearResult n10485783;
  struct : kern::Node<10485784> { Int index = 0; } n10485784;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n10485785;
  car_near_pointResult n10485786;
  people_cpp::People_shotResult n10485787;
  round_meets_mateResult n10485788;
  soft_target_nearResult n10485789;
  struct : kern::Node<10485790> { Int index = 0; } n10485790;
  struct : kern::Node<10485791> { Int index = 0; } n10485791;
  struct : kern::Node<10485792> { Int index = 0; } n10485792;
  struct : kern::Node<10485793> { Int index = 0; } n10485793;
  struct : kern::Node<10485794> { Int index = 0; } n10485794;
  struct { GapsState_SniperRound value = GapsState_SniperRound{game_ShotEvent{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, false, false, Vector{0.0, 0.0, 1.0}, 0}, 0.0}; } n10485795;
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
  struct { GapsState_MotorVoice value = GapsState_MotorVoice{0.0, -1.0, Int64{0x0}, false}; } n11534365;
  struct : kern::Node<11534366> { Int index = 0; } n11534366;
  matrix_ofResult n11534381;
  matrix_ofResult n11534382;
  gaps_rules_cpp::SmokeField_startResult n11534383;
  gaps_rules_cpp::SmokeField_accelerateResult n11534384;
  game_gaps_cpp::jet_roar_pcmResult n11534385;
  struct { Int result = 0; } n11534386;
  game_gaps_cpp::jet_roar_pcmResult n11534387;
  struct { Int result = 0; } n11534388;
  struct : kern::Node<11534389> { Int index = 0; } n11534389;
  player_positionResult n11534390;
  struct : kern::Node<11534391> { Int index = 0; } n11534391;
  struct : kern::Node<11534392> { Int index = 0; } n11534392;
  struct { Int64 result = Int64{0x0}; } n11534393;
  struct { Int64 result = Int64{0x0}; } n11534394;
  struct : kern::Node<11534395> { Int index = 0; } n11534395;
  struct { GapsState_JetVoice value = GapsState_JetVoice{0.0, 0.0, 0.0, 0.0, 0.0, Int64{0x0}, Int64{0x0}, false}; } n11534396;
  gaps_ride_cameraResult n11534398;
  matrix_ofResult n11534399;
  matrix_ofResult n11534400;
  state_ofResult n11534401;
  struct : kern::Node<11534402> { Int index = 0; } n11534402;
  struct : kern::Node<11534403> { Int index = 0; } n11534403;
  struct : kern::Node<11534404> { Int index = 0; } n11534404;
  struct { Bool result = false; } n11534405;
  struct { Bool result = false; Bool changed = false; } n11534406;
  struct { Bool result = false; } n11534407;
  struct { Bool result = false; } n11534408;
  struct { Bool result = false; Bool changed = false; } n11534409;
  struct { Bool result = false; } n11534410;
  struct { Bool result = false; } n11534411;
  struct { Bool result = false; Bool changed = false; } n11534412;
  struct { Bool result = false; } n11534413;
  struct { Bool result = false; } n11534414;
  struct { Bool result = false; Bool changed = false; } n11534415;
  struct { Bool result = false; } n11534416;
  matrix_ofResult n12582922;
  struct : kern::Node<12582923> { Int index = 0; } n12582923;
  struct : kern::Node<12582929> { Int index = 0; } n12582929;
  guided_target_atResult n12582930;
  struct { Int result = 0; } n13631498;
  struct { Bool result = false; } n13631499;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n13631500;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n13631501;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n13631502;
  struct { Bool result = false; } n13631503;
  struct { Bool result = false; } n13631504;
  struct : kern::Node<13631505> { Int index = 0; } n13631505;
  struct : kern::Node<13631506> { Int index = 0; } n13631506;
  struct : kern::Node<13631507> { Int index = 0; } n13631507;
  struct : kern::Node<13631508> { Int index = 0; } n13631508;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n13631519;
  struct : kern::Node<13631520> { Int index = 0; } n13631520;
  struct : kern::Node<13631521> { Int index = 0; } n13631521;
  struct : kern::Node<13631522> { Int index = 0; } n13631522;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n13631523;
  struct : kern::Node<13631524> { Int index = 0; } n13631524;
  struct { Bool result = false; } n13631525;
  struct { Bool result = false; Bool changed = false; } n13631526;
  struct { Bool result = false; } n13631527;
  struct : kern::Node<14680101> { Int index = 0; } n14680101;
  struct : kern::Node<14680108> { Int index = 0; } n14680108;
  life_cpp::Life_shotResult n14680109;
  life_cpp::Life_shotResult n14680111;
  struct { Bool result = false; } n14680112;
  player_positionResult n14680113;
  struct : kern::Node<14680114> { Int index = 0; } n14680114;
  struct : kern::Node<14680115> { Int index = 0; } n14680115;
  struct : kern::Node<14680116> { Int index = 0; } n14680116;
  struct : kern::Node<14680117> { Int index = 0; } n14680117;
  struct : kern::Node<14680118> { Int index = 0; } n14680118;
  struct : kern::Node<14680119> { Int index = 0; } n14680119;
  engine_math_math::Mat4_look_at_2Result n14680120;
  struct : kern::Node<14680121> { Int index = 0; } n14680121;
  struct { Object<kern::LightPool> result = Object<kern::LightPool>{}; } n15728650;
  perf_cpp::Governor_updateResult n15728651;
  struct : kern::Node<15728652> { Int index = 0; } n15728652;
  look_cpp::Shake_sampleResult n15728653;
  struct { Bool result = false; } n15728662;
  struct { Int result = 0; } n15728663;
  struct { Bool result = false; } n15728664;
  vr_hand_matrixResult n15728665;
  struct { Bool result = false; Bool changed = false; } n15728666;
  struct { Bool result = false; } n15728667;
  struct { Int result = 0; } n15728670;
  struct { Bool result = false; } n15728671;
  matrix_ofResult n16777226;
  struct : kern::Node<16777281> { Int index = 0; } n16777281;
  struct : kern::Node<16777291> { Int index = 0; } n16777291;
  struct : kern::Node<16777292> { Int index = 0; } n16777292;
  struct : kern::Node<16777293> { Int index = 0; } n16777293;
  struct : kern::Node<16777294> { Int index = 0; } n16777294;
  struct : kern::Node<16777295> { Int index = 0; } n16777295;
  struct : kern::Node<16777296> { Int index = 0; } n16777296;
  struct : kern::Node<16777297> { Int index = 0; } n16777297;
  struct : kern::Node<16777298> { Int index = 0; } n16777298;
  struct : kern::Node<16777299> { Int index = 0; } n16777299;
  struct : kern::Node<16777300> { Int index = 0; } n16777300;
  struct : kern::Node<16777301> { Int index = 0; } n16777301;
  struct { Game_VisualBlast value = Game_VisualBlast{Vector{0.0, 0.0, 0.0}, 0.0, 1.0}; } n16777302;
  struct : kern::Node<16777303> { Int index = 0; } n16777303;
  struct : kern::Node<16777304> { Int index = 0; } n16777304;
  struct : kern::Node<16777305> { Int index = 0; } n16777305;
  rocket_laser_dotResult n16777306;
  struct : kern::Node<16777307> { Int index = 0; } n16777307;
  struct { Tracer value = Tracer{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{3.0, 2.4, 1.2}}; } n16777308;
  mp_art_cpp::MpArt_initResult n17825802;
  struct : kern::Node<17825803> { Int index = 0; } n17825803;
  struct : kern::Node<17825804> { Int index = 0; } n17825804;
  struct : kern::Node<17825805> { Int index = 0; } n17825805;
  net_session::Session_world_rulesResult n17825806;
  struct : kern::Node<17825807> { Int index = 0; } n17825807;
  struct : kern::Node<17825808> { Int index = 0; } n17825808;
  struct : kern::Node<17825809> { Int index = 0; } n17825809;
  struct : kern::Node<17825810> { Int index = 0; } n17825810;
  state_ofResult n17825811;
  matrix_ofResult n17825812;
  vr_hand_rayResult n17825813;
  wanted_cpp::starsResult n17825814;
  struct : kern::Node<17825815> { Int index = 0; } n17825815;
  matrix_ofResult n17825816;
  struct : kern::Node<17825817> { Int index = 0; } n17825817;
  vr_hand_matrixResult n17825818;
  state_ofResult n17825819;
  state_ofResult n17825820;
  gaps_ride_poseResult n17825821;
  focus_positionResult n17825829;
  road_graph_cpp::RoadGraph_pathResult n17825830;
  struct : kern::Node<17825831> { Int index = 0; } n17825831;
  mp_rules_cpp::VoiceStream_renderResult n17825832;
  person_hit::hitResult n17825833;
  struct : kern::Node<17825834> { Int index = 0; } n17825834;
  remote_on_segment_zonesResult n17825835;
  remote_on_segment_zonesResult n17825836;
  remote_on_segment_zonesResult n17825837;
  project_screenResult n17825841;
  mp_rules_cpp::safe_mobile_marker_pointResult n17825842;
  struct : kern::Node<18874378> { Int index = 0; } n18874378;
  struct { Bool removed = false; } n18874379;
  struct : kern::Node<18874380> { Int index = 0; } n18874380;
  net_session::Session_take_projResult n18874381;
  struct : kern::Node<18874382> { Int index = 0; } n18874382;
  struct : kern::Node<18874383> { Int index = 0; } n18874383;
  struct : kern::Node<18874384> { Int index = 0; } n18874384;
  struct : kern::Node<18874385> { Int index = 0; } n18874385;
  net_session::Session_take_projResult n18874386;
  struct : kern::Node<18874387> { Int index = 0; } n18874387;
  wire::Reader_get_u32Result n18874388;
  wire::Reader_get_u8Result n18874389;
  wire::Reader_get_u16Result n18874390;
  wire::Reader_get_u8Result n18874391;
  wire::Reader_get_u8Result n18874392;
  wire::Reader_get_f32Result n18874393;
  wire::Reader_get_f32Result n18874394;
  wire::Reader_get_f32Result n18874395;
  wire::Reader_get_i16Result n18874396;
  wire::Reader_get_i16Result n18874397;
  wire::Reader_get_i16Result n18874398;
  struct { Bool added = false; Int index = 0; } n18874399;
  mp_rules_cpp::PoseTimeline_pushResult n18874400;
  struct : kern::Node<18874401> { Int index = 0; } n18874401;
  gaps_ride_poseResult n19922959;
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
  struct : kern::Node<20971561> { Int index = 0; } n20971561;
  struct : kern::Node<20971562> { Int index = 0; } n20971562;
  struct : kern::Node<20971563> { Int index = 0; } n20971563;
  struct : kern::Node<20971564> { Int index = 0; } n20971564;
  struct : kern::Node<22020106> { Int index = 0; } n22020106;
  life::Life_animal_ofResult n22020107;
  net_session::Session_take_tossResult n22020108;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n22020109;
  struct : kern::Node<22020110> { Int index = 0; } n22020110;
  life::Life_animal_ofResult n22020111;
  struct : kern::Node<22020112> { Int index = 0; } n22020112;
  struct : kern::Node<22020113> { Int index = 0; } n22020113;
  Game_toss_update_held_hereResult n22020115;
  Game_toss_update_held_hereResult n22020116;
  Game_toss_update_held_hereResult n22020117;
  struct { TossState_Own value = TossState_Own{0, Int64{0x0}, -1, 0.0, 0.0, 0}; } n22020118;
  net_session::Session_take_tossResult n22020119;
  struct : kern::Node<22020120> { Int index = 0; } n22020120;
  wire::Reader_get_u32Result n22020121;
  wire::Reader_get_u8Result n22020122;
  wire::Reader_get_u8Result n22020123;
  wire::Reader_get_u8Result n22020124;
  wire::Reader_get_u64Result n22020125;
  wire::Reader_get_f32Result n22020126;
  wire::Reader_get_f32Result n22020127;
  wire::Reader_get_f32Result n22020128;
  wire::Reader_get_i16Result n22020129;
  wire::Reader_get_i16Result n22020130;
  wire::Reader_get_i16Result n22020131;
  wire::Reader_get_i16Result n22020132;
  wire::Reader_get_i16Result n22020133;
  wire::Reader_get_i16Result n22020134;
  wire::Reader_get_i16Result n22020135;
  mp_rules_cpp::PoseTimeline_pushResult n22020136;
  struct { mp_PoseTimeline value = mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}; } n22020137;
  mp_rules_cpp::PoseTimeline_pushResult n22020138;
  struct : kern::Node<22020139> { Int index = 0; } n22020139;
  life_cpp::Life_net_carry_cowResult n22020140;
  life_cpp::Life_net_carry_petResult n22020141;
  struct { TossState_Remote value = TossState_Remote{0, 0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, -1, false, mp_PoseTimeline{List<mp_PoseTimeline_Snap>{}, List<Float>{}, 0.0, 90.0, List<Float>{}, false}}; } n22020142;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n22020143;
  player_positionResult n22020144;
  struct : kern::Node<22020145> { Int index = 0; } n22020145;
  struct { Int count = 0; Float v0 = 0.0; Float v1 = 0.0; Float v2 = 0.0; } n22020147;
  struct : kern::Node<22020148> { Int index = 0; } n22020148;
  struct : kern::Node<22020149> { Int index = 0; } n22020149;
  aim_rayResult n22020150;
  struct : kern::Node<22020151> { Int index = 0; } n22020151;
  struct : kern::Node<22020152> { Int index = 0; } n22020152;
  struct : kern::Node<22020153> { Int index = 0; } n22020153;
  struct : kern::Node<22020154> { Int index = 0; } n22020154;
  struct : kern::Node<22020155> { Int index = 0; } n22020155;
  toss_jsonResult n22020156;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n23068682;
  player_positionResult n23068690;
  city_stream_cpp::CityStream_street_pointResult n23068691;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n23068692;
  struct { Bool result = false; } n23068693;
  struct { Bool result = false; } n23068694;
  struct : kern::Node<23068695> { Int index = 0; } n23068695;
  common::Rng_nextResult n23068696;
  struct { Bool result = false; } n23068705;
  focus_positionResult n23068707;
  struct : kern::Node<23068708> { Int index = 0; } n23068708;
  struct : kern::Node<23068709> { Int index = 0; } n23068709;
  struct : kern::Node<23068710> { Int index = 0; } n23068710;
  matrix_ofResult n23068711;
  struct : kern::Node<23068712> { Int index = 0; } n23068712;
  struct : kern::Node<23068713> { Int index = 0; } n23068713;
  matrix_ofResult n23068714;
  focus_positionResult n23068715;
  struct : kern::Node<23068716> { Int index = 0; } n23068716;
  struct : kern::Node<23068717> { Int index = 0; } n23068717;
  struct : kern::Node<23068718> { Int index = 0; } n23068718;
  struct : kern::Node<23068719> { Int index = 0; } n23068719;
  struct { Game_minimap_json_Feat value = Game_minimap_json_Feat{0, false, Int64{0x0}, 0, 0.0}; } n23068720;
  matrix_ofResult n23068721;
  struct : kern::Node<23068722> { Int index = 0; } n23068722;
  town_sync_cpp::packResult n24117258;
  struct { Bool added = false; Int index = 0; } n24117259;
  town_sync_cpp::pack_4Result n24117260;
  town_sync_cpp::pack_3Result n24117261;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n24117262;
  struct : kern::Node<24117263> { Int index = 0; } n24117263;
  struct : kern::Node<24117264> { Int index = 0; } n24117264;
  struct : kern::Node<24117265> { Int index = 0; } n24117265;
  town_sync_cpp::pack_stateResult n24117266;
  struct : kern::Node<24117267> { Int index = 0; } n24117267;
  struct : kern::Node<24117268> { Int index = 0; } n24117268;
  struct : kern::Node<24117269> { Int index = 0; } n24117269;
  matrix_ofResult n25165834;
  state_ofResult n25165835;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n25165836;
  common::Rng_nextResult n25165837;
  people_cpp::People_add_citizenResult n25165838;
  common::Rng_unitResult n25165839;
  common::Rng_unitResult n25165840;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n25165848;
  player_positionResult n25165849;
  road_graph_cpp::RoadGraph_pathResult n25165850;
  matrix_ofResult n25165851;
  Game_step_tow_followResult n25165852;
  Game_step_tow_followResult n25165853;
  struct { Bool result = false; } n25165854;
  struct { Bool result = false; } n25165855;
  Game_step_tow_followResult n25165856;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n25165857;
  struct : kern::Node<25165858> { Int index = 0; } n25165858;
  struct : kern::Node<25165859> { Int index = 0; } n25165859;
  struct : kern::Node<26214410> { Int index = 0; } n26214410;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n26214412;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n26214413;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n26214414;
  struct { List<Int64> out = List<Int64>{}; } n26214415;
  struct { Int result = 0; } n27262994;
  struct { Bool result = false; } n27262995;
  struct : kern::Node<27262996> { Int index = 0; } n27262996;
  struct : kern::Node<27262997> { Int index = 0; } n27262997;
  struct { Bool result = false; } n27262998;
  struct { Bool result = false; Bool changed = false; } n27262999;
  struct { Int result = 0; } n27263000;
  struct { Bool result = false; } n27263001;
  struct : kern::Node<27263002> { Int index = 0; } n27263002;
  struct : kern::Node<27263003> { Int index = 0; } n27263003;
  struct { Bool result = false; } n27263004;
  struct { Bool result = false; } n27263005;
  struct { Bool result = false; } n27263006;
  struct { Int result = 0; } n27263007;
  struct { Bool result = false; } n27263008;
  player_positionResult n27263009;
  struct : kern::Node<27263010> { Int index = 0; } n27263010;
  struct : kern::Node<27263011> { Int index = 0; } n27263011;
  struct { Bool result = false; } n27263012;
  struct { List<Float> positions = List<Float>{}; List<Float> colors = List<Float>{}; } n27263013;
  struct { Bool result = false; } n27263014;
  struct : kern::Node<27263015> { Int index = 0; } n27263015;
  struct : kern::Node<27263016> { Int index = 0; } n27263016;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n27263017;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n27263018;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n27263019;
  struct { engine_game_world_sound_cues_place<List<game_SoundCue>> result = {}; } n27263020;
  struct : kern::Node<27263021> { Int index = 0; } n27263021;
  ui_equipment_jsonResult n27263022;
  struct : kern::Node<28311562> { Int index = 0; } n28311562;
  matrix_ofResult n28311563;
  matrix_ofResult n28311564;
  line_of_sightResult n28311565;
  struct : kern::Node<28311566> { Int index = 0; } n28311566;
  vr_hand_matrixResult n29360143;
  vr_hand_matrixResult n29360144;
  struct { Bool result = false; } n29360145;
  struct { Int result = 0; Int framebuffer = 0; } n29360146;
  struct : kern::Node<29360147> { Int index = 0; } n29360147;
  struct { Int result = 0; } n29360148;
  struct { Bool result = false; } n29360149;
  struct { Bool result = false; Bool changed = false; } n29360150;
  struct { Bool result = false; } n29360151;
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
  aim_rayResult n30408750;
  struct : kern::Node<30408759> { Int index = 0; } n30408759;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n31457292;
  world_marks_cpp::Marks_bulletResult n31457294;
  struct : kern::Node<31457295> { Int index = 0; } n31457295;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n31457297;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n31457298;
  struct : kern::Node<32505866> { Int index = 0; } n32505866;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n32505867;
  car_cpp::Car_rigid_specResult n32505868;
  focus_positionResult n32505869;
  struct : kern::Node<32505870> { Int index = 0; } n32505870;
  matrix_ofResult n32505871;
  struct : kern::Node<32505872> { Int index = 0; } n32505872;
  struct : kern::Node<32505873> { Int index = 0; } n32505873;
  struct : kern::Node<32505874> { Int index = 0; } n32505874;
  struct : kern::Node<32505875> { Int index = 0; } n32505875;
  struct : kern::Node<32505876> { Int index = 0; } n32505876;
  struct : kern::Node<32505877> { Int index = 0; } n32505877;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n32505878;
  audio_initResult n33554442;
};
