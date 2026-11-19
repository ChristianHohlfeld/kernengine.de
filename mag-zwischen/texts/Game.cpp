struct Game : virtual kern::Blueprint, virtual car, virtual city_stream, virtual city_stream_cpp, virtual combat_art_cpp, virtual combat_audio_cpp, virtual combat_detail, virtual combat_rules_cpp, virtual common, virtual controls_cpp, virtual drone_cpp, virtual engine_geo_mesh_data, virtual engine_math_math, virtual engine_physics_physics, virtual equipment, virtual equipment_cpp, virtual game, virtual game_barricades_cpp, virtual game_combat, virtual game_controls, virtual game_controls_cpp, virtual game_cpp, virtual game_destruction_sync, virtual game_destruction_sync_cpp, virtual game_equipment_cpp, virtual game_gaps, virtual game_gaps_cpp, virtual game_guided_cpp, virtual game_lamps_cpp, virtual game_life, virtual game_life_cpp, virtual game_look, virtual game_look_cpp, virtual game_military_cpp, virtual game_net, virtual game_net_cpp, virtual game_projsync, virtual game_swarm_cpp, virtual game_toss, virtual game_townsync, virtual game_traffic_cpp, virtual game_ui, virtual game_vehicle_damage_cpp, virtual game_weapons_cpp, virtual game_world, virtual gaps_rules, virtual gaps_rules_cpp, virtual gps_live, virtual gps_live_cpp, virtual guided_cpp, virtual interiors, virtual interiors_cpp, virtual jet, virtual jet_seats_cpp, virtual lamps, virtual lamps_cpp, virtual life, virtual life_art_cpp, virtual life_cpp, virtual look, virtual look_cpp, virtual mp_art, virtual mp_art_cpp, virtual mp_rules, virtual mp_rules_cpp, virtual net_session, virtual net_session_cpp, virtual nostr_cpp, virtual people, virtual people_art_cpp, virtual people_cpp, virtual perf, virtual person_hit, virtual ragdoll, virtual ragdoll_cpp, virtual road_graph_cpp, virtual settings, virtual settings_cpp, virtual spawn_guard_cpp, virtual terrain, virtual terrain_cpp, virtual tiles_cpp, virtual touch_play, virtual touch_play_cpp, virtual vehicle_art_cpp, virtual wanted_cpp, virtual weapons, virtual world_marks_cpp, virtual world_traffic_cpp {
  // ---- variables ----
  PadState pad = PadState{false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, false};
  List<DestroySync> dsync_ = List<DestroySync>{};
  Int toss_test_ = -1;
  TossState toss_ = TossState{List<TossState_Own>{}, List<TossState_Remote>{}, 0.0, 0, 0};
  List<ProjSync> projsync_ = List<ProjSync>{};
  Int proj_serial_ = 0;
  List<TownSync> townsync_ = List<TownSync>{};
  Bool puppet_world_ = false;
  /// client: the host's index each of our cars plays (-1 none)
  List<Int> car_net_ = List<Int>{};
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
  Bool vr_menu_valid_ = false;
  Int vr_menu_hover_ = -1;
  Int vr_menu_page_ = 0;
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
  Int vr_scope_tex_ = 0;
  Int vr_scope_mesh_ = 0;
  /// the headset HUD panel (the page's texture, "vr hud <id>")
  Int vr_hud_tex_ = 0;
  /// the headset HUD panel (the page's texture, "vr hud <id>")
  Int vr_hud_mesh_ = 0;
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
  /// every mode's touch controls (touch_play.h)
  TouchPlay pad_touch_ = TouchPlay{960.0, 540.0, 0.0, 0.0, 0.0, 0.0, TouchLayout{TouchMode::off, true, "", false, false, false, 0, -1.0, false, false, false, true, false, false, false, false, false, false, false, false}, List<TouchBtn>{}, List<TouchPlay_Finger>{}, List<Text>{}, List<Text>{}, List<TouchFire>{}, -1, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, 0.0, false, List<TouchPlay_DomRect>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  Bool touch_mode_ = false;
  Float frame_w_ = 960.0;
  Float frame_h_ = 540.0;
  game_PlayInput play_in_ = game_PlayInput{0.0, 0.0, 0.0, false, false, false, false, 0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, false, 0.0, false, false, false, false, false, 0.0, 0.0, false, 0.0};
  /// the town
  People people_ = People{List<People_Person>{}, List<CrimeReport>{}, List<People_Death>{}, List<People_Fall>{}, List<People_RemoteHit>{}, List<People_Hurt>{}, false, false, 60, 1, 0, 0, 170.0, 190.0};
  PeopleArt people_art_ = PeopleArt{0, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}};
  CombatArt combat_art_ = CombatArt{0, List<CombatArt_Part>{}};
  CombatState combat_ = CombatState{combat_MilitaryClock{false, -1.0, -1.0, 0.0}, 0, 0, 0, 0.0, List<Heli>{}, List<Tank>{}, List<Shell>{}, List<SoldierAi>{}, List<Float>{}, true, List<FlakGun>{}, List<FlakRound>{}, List<Puff>{}, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0, 0, List<FighterJet>{}, List<Bomb>{}, List<Blast>{}, List<Flare>{}, 50.0, -1000000000.0, List<Tracer>{}, List<Debris>{}, List<Int>{}, List<Bool>{}, List<Bool>{}, false, 0, 0, 0, combat_ZombieRounds{false, 0, 0, 0, 0.0, -1.0}, List<ZombieAi>{}, true, false, -99.0, List<EnemyAi>{}, List<EnemyRocket>{}, combat_Chaos{0, 1, -1.0, 0.0}, -99.0, -1.0, 0, combat_Fun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, -1.0, combat_Race{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}, false, Vector{0.0, 0.0, 0.0}, "", "", -99.0, -99.0, false, 0.0};
  /// GUIDED missiles in flight (game_guided.cpp)
  List<guided_Flight> guided_ = List<guided_Flight>{};
  Float guided_last_ = -99.0;
  Text guided_lock_ = "";
  /// the first press marks, the second launches (game_guided.cpp)
  guided_Target guided_mark_ = guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"};
  Vector guided_mark_at_ = Vector{0.0, 0.0, 0.0};
  Float guided_mark_time_ = -99.0;
  Bool guided_marked_ = false;
  Fixed<detail_CriticalClock, 5> pd_crit_ = List<detail_CriticalClock>{};
  detail_CriticalClock drone_crit_ = detail_CriticalClock{-1.0, -1.0};
  /// the console's ballot ("ballot ...", game_details.cpp)
  mp_Ballot ballot_ = mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::invalid};
  Int bullet_pushes_ = 0;
  Int crit_blasts_ = 0;
  Fixed<Int, 16> sfx_cursor_ = List<Int>{};
  Bool combat_audio_ready_ = false;
  RoadGraph roads_ = RoadGraph{Int64{0xffffffffffffffff}, List<RoadGraph_Node>{}, Map<Int64, Int>{}};
  List<PoliceUnit> units_ = List<PoliceUnit>{};
  Fixed<PoliceDrone, 5> pdrones_ = List<PoliceDrone>{};
  Vector last_known_ = Vector{0.0, 0.0, 0.0};
  Float last_contact_ = -99.0;
  Float next_drone_wave_ = -1.0;
  Float emp_ready_at_ = 0.0;
  Int drone_serial_ = 0;
  Bool drone_wave_live_ = false;
  /// weapons
  List<Projectile> projectiles_ = List<Projectile>{};
  weapons_Id foot_weapon_ = weapons_Id::glock;
  Fixed<Int, 16> ammo_mag_ = List<Int>{};
  Fixed<Int, 16> ammo_reserve_ = List<Int>{};
  Int grenades_ = 4;
  Int drone_weapon_ = 0;
  /// jet_mode.mjs camOff (the chase camera's eased offset) // tests: distance of the car "mode car" took // jet_mode.mjs rocketSide: the pods fire in turn
  Vector jet_cam_off_ = Vector{0.0, 0.0, 0.0};
  Float next_attack_ = 0.0;
  Float next_grenade_ = 0.0;
  Float next_jet_missile_ = 0.0;
  Bool trigger_was_ = false;
  /// multiplayer
  net_Session net_ = net_Session{net_Session_Role::none, 0, 0, 0, net_Address{Int64{0x0}}, List<net_Remote>{}, List<net_Hit>{}, List<net_Hit>{}, List<net_Kill>{}, List<net_Fx>{}, List<net_Swarm>{}, List<net_SwarmHit>{}, List<Int_row>{}, List<net_PingEvent>{}, List<net_VoiceFrame>{}, List<Int>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, false, false, 0.0, 0, 0.0, List<Int>{}, Vector{0.0, 0.0, 0.0}, 255, net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_Scoreboard{List<mp_Score>{}}, false, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_SwarmParts>{}, 0, 0, false, List<Int_Float_pair>{}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, 0, "Aenderungen nur einstimmig", 0, 0, 0, seats_Table{List<seats_Row>{}, 0}, 0.0};
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
  Int64 steps_ = Int64{0x0};
  Rng rng_ = Rng{20261029, 0.0, false};
  /// world_spawn_guard (Game::spawn_guard)
  Bool spawn_checked_ = false;
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
  Int combat_kContracts = 0;
  /// (of the program: combat::kCourseScale)
  Float combat_kCourseScale = 0.0;
  /// (of the program: combat::kEndStars)
  Int combat_kEndStars = 0;
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
  Int combat_kGates = 0;
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
  Int combat_kMaxSoldiers = 0;
  /// (of the program: combat::kMilHit)
  Float combat_kMilHit = 0.0;
  /// (of the program: combat::kMomentumTarget)
  Int combat_kMomentumTarget = 0;
  /// (of the program: combat::kOverdriveS)
  Float combat_kOverdriveS = 0.0;
  /// (of the program: combat::kPunchDmg)
  Float combat_kPunchDmg = 0.0;
  /// (of the program: combat::kRaceLaps)
  Int combat_kRaceLaps = 0;
  /// (of the program: combat::kReinforceS)
  Float combat_kReinforceS = 0.0;
  /// (of the program: combat::kShellDmg)
  Float combat_kShellDmg = 0.0;
  /// (of the program: combat::kSoldierBursts)
  Int combat_kSoldierBursts = 0;
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
  Int combat_kSquad = 0;
  /// (of the program: combat::kStabDmg)
  Float combat_kStabDmg = 0.0;
  /// (of the program: combat::kStick)
  Int combat_kStick = 0;
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
  Int combat_kZombieCap = 0;
  /// (of the program: combat::kZombieHit)
  Float combat_kZombieHit = 0.0;
  /// (of the program: combat::kZombieHitPts)
  Int combat_kZombieHitPts = 0;
  /// (of the program: combat::kZombieKillPts)
  Int combat_kZombieKillPts = 0;
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
  /// (kept by game_controls)
  AimTarget game_controls_g_aim = AimTarget{0, -1, -99.0};
  /// (kept by game_controls)
  Text game_controls_g_last_exit = "";
  /// (kept by game_controls)
  ViewTrack game_controls_g_view = ViewTrack{0, -1.0, false, -1.0, -1.0, -1.0, -1.0, 0, false, combat_Fun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, false, 0, List<Text>{}, true};
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
  List<Int> mp_rules_g_palette = List<Int>{};
  /// (kept by sfx_id)
  List<Text> sfx_id_ids = List<Text>{};
  KERN_CONST combat_EnemySpec kNoEnemy = combat_EnemySpec{"", 1.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0, 1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}};
  KERN_CONST combat_Contract kNoContract = combat_Contract{"", "", "", 1, 0};
  KERN_CONST combat_Gate kNoGate = combat_Gate{"", Vector{0.0, 0.0, 0.0}, 1.0, 1.0, false};
  KERN_CONST Float kRespawnCooldown = 3.0;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kPoleH = 6.2;
  /// The number of each value of net_PlayerFlag, in their order
  KERN_CONST List<Int> net_PlayerFlag_numbers = List<Int>{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
  /// building_interiors.mjs PLANKS
  KERN_CONST Int kPlanks = 6;
  /// nearDoor
  KERN_CONST Float kRepairM = 3.2;
  /// DOOR_W
  KERN_CONST Float kDoorW = 1.8;
  /// vertex locations 14 and 15
  KERN_CONST Int kMaxExtensionAttributes = 2;
  KERN_CONST Float kDeg = 0.017453292519943295;
  /// desktop_controls.mjs LOOK_DEG_PER_PX (pitch x .85)
  KERN_CONST Float kLookDegPx = 0.12;
  KERN_CONST Float kRad = 57.29577951308238;
  /// player_car_mode.mjs ENTER_RADIUS_M, jet_mode.mjs ENTER_M
  KERN_CONST Float kEnterJetM = 9.0;
  /// player_car_mode.mjs ENTER_RADIUS_M, jet_mode.mjs ENTER_M
  KERN_CONST Float kEnterCarM = 5.2;
  /// SpawnCity's numbers. Damage of the hitscan guns is SpawnCity's multiplayer damage (VS_WEAPON_DAMAGE smg 13, glock 26, sniper 95) times the zone (head 3, torso 1, legs .65).
  KERN_CONST Fixed<weapons_Spec, 14> kTable = List<weapons_Spec>{weapons_Spec{"MP", 55.0, 180.0, 13.0, 0.0, 240.0, 1}, weapons_Spec{"Glock", 110.0, 240.0, 26.0, 0.0, 0.0, 2}, weapons_Spec{"Sniper .50", 1150.0, 1500.0, 95.0, 0.0, 850.0, 3}, weapons_Spec{"Granatwerfer 40 mm", 760.0, 300.0, 125.0, 8.5, 40.0, 5}, weapons_Spec{"Raketenwerfer", 900.0, 420.0, 160.0, 9.5, 40.0, 6}, weapons_Spec{"Faeuste", 300.0, 1.8, 24.0, 0.0, 0.0, 7}, weapons_Spec{"Messer", 380.0, 1.6, 45.0, 0.0, 0.0, 8}, weapons_Spec{"Handgranate", 4000.0, 40.0, 125.0, 8.5, 0.0, 0}, weapons_Spec{"Drohnen-MG", 92.0, 650.0, 13.0, 0.0, 0.0, 0}, weapons_Spec{"Drohnen-Raketen", 950.0, 220.0, 100.0, 8.0, 31.0, 0}, weapons_Spec{"GUIDED", 5000.0, 600.0, 34.0, 4.0, 34.0, 0}, weapons_Spec{"Bordkanone", 55.0, 1800.0, 30.0, 0.0, 0.0, 0}, weapons_Spec{"Luft-Rakete", 2400.0, 1300.0, 120.0, 8.0, 260.0, 0}, weapons_Spec{"Bombe", 450.0, 0.0, 170.0, 17.0, 0.0, 0}};
  KERN_CONST Float kPi = 3.141592653589793;
  /// m: the safe margin a player is put down over the ground
  KERN_CONST Float kSpawnLift = 0.35;
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
  KERN_CONST Fixed<Text, 8> kGateNames = List<Text>{"START / FINISH", "GATE 2", "GATE 3", "HIGH GATE", "GATE 5", "GATE 6", "LOW GATE", "GATE 8"};
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
  /// SPAWN_GRACE_MS: right after the world is shown, anything may appear
  KERN_CONST Float kSpawnGraceS = 6.0;
  /// world_procedural_population.mjs
  KERN_CONST Float kVehicleEntryM = 600.0;
  /// jet_mode.mjs HP_MAX
  KERN_CONST Float kJetHpMax = 400.0;
  /// startWorldCriticalDamage delayMs 12000 (jet)
  KERN_CONST Float kJetBurnS = 12.0;
  KERN_CONST Float kGuidedMarkSeconds = 8.0;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kArm = 1.42;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kHeadZ = 6.0;
  KERN_CONST Fixed<perf_Tier, 5> kTier = List<perf_Tier>{perf_Tier{"minimal", 16, 35.0, 1, 0, 70.0, 90.0, 2, 0.6}, perf_Tier{"niedrig", 30, 50.0, 4, 0, 110.0, 140.0, 3, 0.75}, perf_Tier{"mittel", 44, 70.0, 4, 1, 140.0, 180.0, 4, 1.0}, perf_Tier{"hoch", 64, 90.0, 9, 2, 170.0, 220.0, 6, 1.0}, perf_Tier{"maximal", 80, 110.0, 9, 3, 170.0, 220.0, 10, 1.0}};
  KERN_CONST Fixed<Text, 4> kVmNames = List<Text>{"BOOMSTICK 40 mm", "ROCKET LAUNCHER", "FISTS", "KNIFE"};
  KERN_CONST Vector kSoldierShirt = Vector{0.33, 0.38, 0.24};
  KERN_CONST Vector kSoldierPants = Vector{0.29, 0.32, 0.22};
  KERN_CONST Fixed<Vector, 4> kSoldierSkins = List<Vector>{Vector{0.78, 0.58, 0.44}, Vector{0.66, 0.44, 0.32}, Vector{0.48, 0.29, 0.2}, Vector{0.82, 0.64, 0.49}};
  /// world.* switches that make the world (friendly fire is the rule voted on: game_rules_sync)
  KERN_CONST Fixed<Opt, 13> kWorldRules = List<Opt>{Opt::world_people, Opt::world_traffic, Opt::world_dogs, Opt::world_cats, Opt::world_birds, Opt::world_zombies, Opt::world_police, Opt::world_military, Opt::world_flak, Opt::world_jetpack, Opt::world_terrain, Opt::world_guided, Opt::world_always_day};
  /// a seated passenger: hips 0.5 m over the road instead of 0.92 (game_seats.cpp)
  KERN_CONST Float kSeatDrop = 0.42;
  KERN_CONST Float kArriveM = 25.0;
  /// world_map_full.mjs: a new route at most every 10 s
  KERN_CONST Float kNavRerouteMs = 10000.0;
  KERN_CONST Float kKillMarkerMs = 260.0;
  KERN_CONST Float kHitMarkerMs = 110.0;
  KERN_CONST Float kHurtMs = 140.0;
  /// VS_POSE_STALE_HOLD_MS
  KERN_CONST Float kStaleHoldMs = 3000.0;
  /// no shooter (an accident, the police, a fall)
  KERN_CONST Int kNobody = 254;
  KERN_CONST Fixed<Text, 5> kModes = List<Text>{"foot", "drone", "car", "jet", "parachute"};
  /// jet_mode.mjs ENTER_M, passengerJetPose 3500 ms, coordinator timeout
  KERN_CONST Float kBoardM = 9.0;
  /// The number of each value of net_SwarmFlag, in their order
  KERN_CONST List<Int> net_SwarmFlag_numbers = List<Int>{128, 1, 2, 4, 3};
  KERN_CONST Float kFallenMs = 1500.0;
  KERN_CONST Int kPerson = 0;
  KERN_CONST Int kBird = 3;
  KERN_CONST Int kCow = 1;
  KERN_CONST Int kPet = 2;
  KERN_CONST Fixed<Float, 5> kPdFormation = List<Float>{0.0, 0.08, -0.08, 0.15, -0.15};
  /// wanted_police_drones.mjs
  KERN_CONST Float kPdHp = 100.0;
  /// the blasts kept (scorches 40, soot 48, breaches 64 - world_marks.h)
  KERN_CONST Int kMarks = 64;
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
  KERN_PURE contractResult contract(Int index = 0) {
    return {logic_select<combat_Contract>(math_compare<">">(kern::to<Float>(combat_kContracts), 0.0), combat_rules_g_contracts[kern::to<Int>(kern::formula([](Float index, Float combat_kContracts) -> Float { return kern::math::mod((kern::math::mod(index, combat_kContracts) + combat_kContracts), combat_kContracts); }, kern::to<Float>(index), kern::to<Float>(combat_kContracts)))], kNoContract)};
  }

  struct bind_raceResult { Bool Result = false; };
  /// ---------------------------------------------------------------- "Race"
  bind_raceResult bind_race() {
    Bool runs = false;
    Int n = 0;
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

  struct hit_materialResult { Float Result = 0.0; };
  /// world_action_feedback.mjs materialKind(hit): a person / player -> flesh; a car, bus, vehicle or lamp (or a metal object, metalness > .32: the police drones) -> metal; a tree -> wood; else concrete. Codes for audio.mjs: 1 concrete, 2 metal, 3 flesh, 4 wood.
  KERN_PURE hit_materialResult hit_material(Vector at = Vector{0.0, 0.0, 0.0}, Int surface = 0, Bool flesh = false) {
    List<Int> list = List<Int>{};
    Int i = 0;
    Float s = 0.0;
    Float h = 0.0;
    Float cx = 0.0;
    Float cz = 0.0;
    if (flesh) {
      return {3.0};
    }
    if (math_compare<"==">(kern::to<Float>(surface), 2.0)) {
      return {2.0};
    }
    if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
      list = lamps_cpp::Field_near(gaps_[0].lamps, vector_break(at).x, vector_break(at).y, 2.5).Result;
      for (auto it2097162 = flow_for(n2097162, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(list))))); it2097162.next();) {
        i = list[n2097162.index];
        if (logic_and(logic_and(math_compare<"<">(kern::to<Float>(gaps_[0].lamps.lamps_[i].state), 2.0), logic_or(math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(at).x, vector_break(gaps_[0].lamps.lamps_[i].foot).x, vector_break(at).y, vector_break(gaps_[0].lamps.lamps_[i].foot).y), kern::formula([](Float the_result) -> Float { return (the_result + 0.25); }, lamps_cpp::pole_radius().Result)), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(at).x, vector_break(gaps_[0].lamps.lamps_[i].head).x, vector_break(at).y, vector_break(gaps_[0].lamps.lamps_[i].head).y), 0.6))), math_compare<"<">(vector_break(at).z, kern::formula([](Float z, Float kPoleH) -> Float { return ((z + kPoleH) + 0.4); }, vector_break(gaps_[0].lamps.lamps_[i].foot).z, kPoleH)))) {
          return {2.0};
        }
      }
    }
    if (math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)) {
      for (auto it2097163 = flow_for(n2097163, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Tree>(glife_[0].life.trees_))))); it2097163.next();) {
        s = glife_[0].life.trees_[n2097163.index].scale;
        h = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(at).x, vector_break(glife_[0].life.trees_[n2097163.index].foot).x, vector_break(at).y, vector_break(glife_[0].life.trees_[n2097163.index].foot).y);
        if (logic_and(logic_and(math_compare<"<">(h, kern::formula([](Float s) -> Float { return (0.3 * s); }, s)), math_compare<">">(vector_break(at).z, kern::formula([](Float z) -> Float { return (z - 0.2); }, vector_break(glife_[0].life.trees_[n2097163.index].foot).z))), math_compare<"<">(vector_break(at).z, kern::formula([](Float z, Float s) -> Float { return (z + (2.4 * s)); }, vector_break(glife_[0].life.trees_[n2097163.index].foot).z, s)))) {
          return {4.0};
        }
        cx = kern::formula([](Float h, Float s) -> Float { return kern::math::div(h, ((1.0 * s) + 0.15)); }, h, s);
        cz = kern::formula([](Float z, Float z_2, Float s) -> Float { return kern::math::div(((z - z_2) - (2.75 * s)), ((1.2 * s) + 0.15)); }, vector_break(at).z, vector_break(glife_[0].life.trees_[n2097163.index].foot).z, s);
        if (math_compare<"<">(kern::formula([](Float cx, Float cz) -> Float { return ((cx * cx) + (cz * cz)); }, cx, cz), 1.0)) {
          return {4.0};
        }
      }
    }
    return {1.0};
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
    engine_game_world_soldier_state(n2097164, world_, soldier_, ss);
    soldier_state = n2097164.result;
    ss = n2097164.out;
    have = soldier_state;
    n2097165 = matrix_of(soldier_);
    a = n2097165.Result;
    feet = engine_math_math::Mat4_translation(a).Result;
    air = logic_and(enum_equal<Mode>(mode_, Mode::foot), math_compare<">">(kern::formula([](Float z, Float the_result) -> Float { return (z - the_result); }, vector_break(feet).z, city_stream_cpp::CityStream_ground_height(city_, vector_break(feet).x, vector_break(feet).y).Result), 1.15));
    b = text_format<",\"walk\":{\"dead\":%d,\"air\":%d,\"p\":">(511, kern::to<Int>(kern::formula([](Float have, Float dead) -> Float { return (((have != 0.0) && (dead != 0.0)) ? 1.0 : 0.0); }, kern::to<Float>(have), kern::to<Float>(ss.dead))), kern::to<Int>(kern::formula([](Float air) -> Float { return ((air != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(air))));
    s = text_append(s, b, "");
    web_audio_state_v3(b, s, logic_select<Vector>(enum_equal<Mode>(mode_, Mode::foot), cam_.eye, feet));
    s = text_append(s, "}", "");
    if (drone_out_) {
      b = text_format<",\"drone\":{\"st\":%u,\"arm\":%d,\"d\":%.4f,\"w\":[%.2f,%.2f,%.2f,%.2f],\"mt\":[%.5f,%.5f,%.5f,%.5f],\"pt\":[%.5f,%.5f,%.5f,%.5f],\"p\":">(511, drone_.tele_.state, kern::to<Int>(kern::formula([](Float arm) -> Float { return ((arm != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(sticks_.arm))), drone_.p_.prop_d, drone_.tele_.omega[0], drone_.tele_.omega[1], drone_.tele_.omega[2], drone_.tele_.omega[3], drone_.tele_.motor_torque[0], drone_.tele_.motor_torque[1], drone_.tele_.motor_torque[2], drone_.tele_.motor_torque[3], drone_.tele_.prop_torque[0], drone_.tele_.prop_torque[1], drone_.tele_.prop_torque[2], drone_.tele_.prop_torque[3]);
      s = text_append(s, b, "");
      n2097166 = matrix_of(drone_actor_);
      a_2 = n2097166.Result;
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
    for (auto it2097167 = flow_for(n2097167, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it2097167.next();) {
      skip = false;
      if (logic_not(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n2097167.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetpack)))), 0.5))) {
        skip = true;
      }
      if (logic_not(skip)) {
        b = text_format<"%s[%zu,%.2f,%.2f,%.2f]">(511, logic_select<Text>(first, "", ","), n2097167.index, vector_break(net_.remotes_[n2097167.index].shown).x, vector_break(net_.remotes_[n2097167.index].shown).y, kern::formula([](Float z) -> Float { return (z - 0.6); }, vector_break(net_.remotes_[n2097167.index].shown).z));
        s = text_append(s, b, "");
        first = false;
      }
    }
    s = text_append(s, "],\"rp\":[", "");
    first_2 = true;
    for (auto it2097168 = flow_for(n2097168, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it2097168.next();) {
      skip_2 = false;
      if (logic_not(enum_equal<net_PlayerMode>(net_.remotes_[n2097168.index].state.mode, net_PlayerMode::foot))) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        b = text_format<"%s[%u,%.2f,%.2f,%.2f,%d]">(511, logic_select<Text>(first_2, "", ","), net_.remotes_[n2097168.index].state.slot, vector_break(net_.remotes_[n2097168.index].person_shown).x, vector_break(net_.remotes_[n2097168.index].person_shown).y, vector_break(net_.remotes_[n2097168.index].person_shown).z, kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(net_.remotes_[n2097168.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetpack))))));
        s = text_append(s, b, "");
        first_2 = false;
      }
    }
    s = text_append(s, "],\"pc\":[", "");
    first_3 = true;
    for (auto it2097169 = flow_for(n2097169, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it2097169.next();) {
      skip_3 = false;
      if (logic_or(logic_or(logic_or(math_compare<"<">(kern::to<Float>(units_[n2097169.index].car), 0.0), enum_equal<PoliceUnit_State>(units_[n2097169.index].state, PoliceUnit_State::abandoned)), enum_equal<PoliceUnit_State>(units_[n2097169.index].state, PoliceUnit_State::wreck)), enum_equal<PoliceUnit_State>(units_[n2097169.index].state, PoliceUnit_State::leave))) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(car_index_), kern::to<Float>(units_[n2097169.index].car)))) {
          skip_3 = true;
        }
        if (logic_not(skip_3)) {
          if (logic_not(first_3)) {
            s = text_append(s, ",", "");
          }
          n2097170 = matrix_of(cars_[units_[n2097169.index].car].actor);
          a_3 = n2097170.Result;
          web_audio_state_v3(b, s, vector_add(engine_math_math::Mat4_translation(a_3).Result, Vector{0.0, 0.0, 0.9}));
          first_3 = false;
        }
      }
    }
    s = text_append(s, "],\"pd\":[", "");
    first_3 = true;
    for (auto it2097171 = flow_for(n2097171, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it2097171.next();) {
      skip_4 = false;
      if (logic_or(logic_or(logic_not(pdrones_[n2097171.index].active), pdrones_[n2097171.index].retreating), pdrones_[n2097171.index].emp)) {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        if (logic_not(first_3)) {
          s = text_append(s, ",", "");
        }
        web_audio_state_v3(b, s, pdrones_[n2097171.index].pos);
        first_3 = false;
      }
    }
    s = text_append(s, "],\"fj\":[", "");
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    first_4 = true;
    for (auto it2097172 = flow_for(n2097172, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<FighterJet>(combat_.jets))))); it2097172.next();) {
      b = text_format<"%s[%.2f,%.2f,%.2f,%.3f]">(511, logic_select<Text>(first_4, "", ","), combat_.jets[n2097172.index].x, combat_.jets[n2097172.index].y0, combat_.jets[n2097172.index].z, kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, combat_.jets[n2097172.index].t0));
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
    for (auto it2097173 = flow_for(n2097173, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))); it2097173.next();) {
      skip_5 = false;
      if (logic_not(guided_[n2097173.index].m.motor)) {
        skip_5 = true;
      }
      if (logic_not(skip_5)) {
        b = text_format<"%s[%.3f,%.2f,%.2f,%.2f]">(511, logic_select<Text>(first_5, "", ","), guided_[n2097173.index].m.born, vector_break(guided_[n2097173.index].m.p).x, vector_break(guided_[n2097173.index].m.p).y, vector_break(guided_[n2097173.index].m.p).z);
        s = text_append(s, b, "");
        first_5 = false;
      }
    }
    s = text_append(s, "],\"rk\":[", "");
    first_6 = true;
    for (auto it2097174 = flow_for(n2097174, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it2097174.next();) {
      skip_6 = false;
      if (logic_or(logic_not(projectiles_[n2097174.index].alive), projectiles_[n2097174.index].guided)) {
        skip_6 = true;
      }
      if (logic_not(skip_6)) {
        kind = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((a != 0.0) ? 1.0 : ((a_2 != 0.0) ? 2.0 : ((a_3 != 0.0) ? 3.0 : 0.0))); }, kern::to<Float>(enum_equal<weapons_Id>(projectiles_[n2097174.index].id, weapons_Id::rocket)), kern::to<Float>(enum_equal<weapons_Id>(projectiles_[n2097174.index].id, weapons_Id::drone_rockets)), kern::to<Float>(enum_equal<weapons_Id>(projectiles_[n2097174.index].id, weapons_Id::jet_missile))));
        if (logic_not(math_compare<"!=">(kern::to<Float>(kind), 0.0))) {
          skip_6 = true;
        }
        if (logic_not(skip_6)) {
          b = text_format<"%s[%.3f,%.2f,%.2f,%.2f,%d]">(511, logic_select<Text>(first_6, "", ","), kern::formula([](Float a, Float kind) -> Float { return (a + (0.0001 * kind)); }, projectiles_[n2097174.index].born, kern::to<Float>(kind)), vector_break(projectiles_[n2097174.index].pos).x, vector_break(projectiles_[n2097174.index].pos).y, vector_break(projectiles_[n2097174.index].pos).z, kind);
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
    map_add(n3145738, gaps_[0].planks, key_2);
    key_3 = key;
    map_add(n3145739, gaps_[0].planks, key_3);
    if (math_compare<"!=">(kern::to<Float>(list_length<Int>(gaps_[0].planks[n3145739.index])), kern::to<Float>(list_length<Interiors_Door>(glife_[0].interiors.plan_.doors)))) {
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
    if (math_compare<">">(kern::to<Float>(map_length<Int64, List<Int>>(gaps_[0].planks)), 400.0)) {
      it = 0;
      stop_2 = false;
      while (loop(logic_and(logic_not(stop_2), logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(gaps_[0].planks))))))) {
        if (logic_not(int64_equal(map_key_at<Int64, List<Int>>(gaps_[0].planks, it), key))) {
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
      list_add(out, struct_make<GapsState_DoorView>(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n3145741.index))), glife_[0].interiors.plan_.doors[n3145741.index].c, glife_[0].interiors.plan_.doors[n3145741.index].d, glife_[0].interiors.plan_.doors[n3145741.index].n, list_get<Int>(gaps_[0].planks[n3145742.index], n3145741.index), kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((a != 0.0) ? a_2 : a_3); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<Float>(glife_[0].interiors.plan_.levels)), 0.0)), glife_[0].interiors.plan_.base, glife_[0].interiors.plan_.levels[0]), key));
    }
    return {true};
  }

  struct gaps_repair_offerResult { Bool Result = false; };
  gaps_repair_offerResult gaps_repair_offer() {
    Int it = 0;
    Vector me = Vector{0.0, 0.0, 0.0};
    Int n = 0;
    if (logic_or(logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(combat_.zombies.active)), logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0))), logic_not(interiors::Interiors_active(glife_[0].interiors).Result))) {
      return {false};
    }
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<Int>>(gaps_[0].planks, game_barricades_cpp::building_key(glife_[0].interiors.plan_).Result).index), kern::to<Float>(map_length<Int64, List<Int>>(gaps_[0].planks))));
    n3145769 = player_position();
    me = n3145769.Result;
    for (auto it3145770 = flow_for(n3145770, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(glife_[0].interiors.plan_.doors))))); it3145770.next();) {
      n = kern::to<Int>(kern::formula([](Float a, Float index, Float a_2, Float kPlanks, Float a_3) -> Float { return (((a != 0.0) || (index >= a_2)) ? kPlanks : a_3); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(gaps_[0].planks)))), kern::to<Float>(n3145770.index), kern::to<Float>(list_length<Int>(gaps_[0].planks[it])), kern::to<Float>(kPlanks), kern::to<Float>(gaps_[0].planks[it][n3145770.index])));
      if (logic_and(math_compare<"<">(kern::to<Float>(n), kern::to<Float>(kPlanks)), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(me).x, vector_break(glife_[0].interiors.plan_.doors[n3145770.index].c).x, vector_break(me).y, vector_break(glife_[0].interiors.plan_.doors[n3145770.index].c).y), kRepairM))) {
        return {true};
      }
    }
    return {false};
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
      engine_renderer_allocate_id(n3145771, gaps_[0].renderer);
      allocate_id = n3145771.result;
      gaps_[0].plank_part.id = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
      look.material = m;
      look.extra_slot = kMaxExtensionAttributes;
      look.visible = false;
      engine_upsert(n3145772, gaps_[0].renderer, gaps_[0].plank_part.id, box, look, List<Float>{}, List<Float>{});
      upsert = n3145772.result;
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
      n3145774 = gaps_barricade_doors(doors);
      both = n3145774.Result;
    }
    if (both) {
      for (auto it3145775 = flow_for(n3145775, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_DoorView>(doors))))); it3145775.next();) {
        yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(doors[n3145775.index].d).y, vector_break(doors[n3145775.index].d).x);
        for (auto it3145776 = flow_for(n3145776, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(doors[n3145775.index].planks)))); it3145776.next();) {
          along = kern::formula([](Float index) -> Float { return ((kern::math::mod(index, 2.0) != 0.0) ? 0.06 : (-0.06)); }, kern::to<Float>(n3145776.index));
          tilt = kern::formula([](Float index) -> Float { return (((kern::math::mod(index, 2.0) != 0.0) ? 1.0 : (-1.0)) * (0.08 + (0.05 * kern::math::mod(index, 3.0)))); }, kern::to<Float>(n3145776.index));
          at = vector_make(kern::formula([](Float x, Float x_2, Float along, Float x_3) -> Float { return ((x + (x_2 * along)) + (x_3 * 0.22)); }, vector_break(doors[n3145775.index].c).x, vector_break(doors[n3145775.index].d).x, along, vector_break(doors[n3145775.index].n).x), kern::formula([](Float y, Float y_2, Float along, Float y_3) -> Float { return ((y + (y_2 * along)) + (y_3 * 0.22)); }, vector_break(doors[n3145775.index].c).y, vector_break(doors[n3145775.index].d).y, along, vector_break(doors[n3145775.index].n).y), kern::formula([](Float a, Float index) -> Float { return ((a + 0.35) + (index * 0.36)); }, doors[n3145775.index].the_floor, kern::to<Float>(n3145776.index)));
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
      engine_renderer_set_visible(n3145777, gaps_[0].renderer, gaps_[0].plank_part.id, false);
      set_visible = n3145777.result;
      return;
    }
    engine_renderer_update_instances(n3145778, gaps_[0].renderer, gaps_[0].plank_part.id, gaps_[0].plank_part.transforms, gaps_[0].plank_part.tints);
    update_instances = n3145778.result;
    engine_renderer_set_visible(n3145779, gaps_[0].renderer, gaps_[0].plank_part.id, true);
    set_visible_2 = n3145779.result;
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
        n4194318 = vr_hand_ray(from, dir);
        both_2 = n4194318.Result;
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

  void Game_ctl_assist_consider(Float& best, AimTarget& game_controls_g_aim_2, Vector eye = Vector{0.0, 0.0, 0.0}, Float fyaw = 0.0, Float fpitch = 0.0, Int kind = 0, Int i = 0) {
    Vector q = Vector{0.0, 0.0, 0.0};
    Float y = 0.0;
    Float pt = 0.0;
    Float dd = 0.0;
    Float e = 0.0;
    Float score = 0.0;
    Int physics = 0;
    q = Vector{0.0, 0.0, 0.0};
    n4194324 = Game_ctl_assist_point(q, kind, i);
    if (logic_not(n4194324.Result)) {
      return;
    }
    y = 0.0;
    pt = 0.0;
    dd = 0.0;
    ctl_assist_errors(y, pt, dd, eye, fyaw, fpitch, q);
    n4194325 = controls_cpp::aim_ellipse(y, pt);
    e = n4194325.Result;
    if (logic_or(logic_or(math_compare<">=">(e, 1.0), math_compare<">">(dd, 90.0)), math_compare<"<">(dd, 0.5))) {
      return;
    }
    score = kern::formula([](Float e, Float dd) -> Float { return (e + (kern::math::div(dd, 90.0) * 0.12)); }, e, dd);
    if (math_compare<">=">(score, best)) {
      return;
    }
    engine_game_world_physics(n4194326, world_);
    physics = n4194326.result;
    if (struct_break<the_RayResult_>(common::cast(physics, eye, vector_scale(vector_sub(q, eye), kern::formula([](Float dd) -> Float { return kern::math::div(1.0, dd); }, dd)), kern::formula([](Float dd) -> Float { return (dd - 0.4); }, dd), body(soldier_).Result, Int64{0x1}).Result).hit) {
      return;
    }
    best = score;
    game_controls_g_aim_2.kind = kind;
    game_controls_g_aim_2.index = i;
  }

  struct ctl_assistResult { controls_AimAssist Result = controls_AimAssist{false, 1.0, 0.0, 0.0}; };
  ctl_assistResult ctl_assist(Float in_yaw = 0.0, Float in_pitch = 0.0, Float input = 0.0) {
    Float s = 0.0;
    Vector eye = Vector{0.0, 0.0, 0.0};
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Float fyaw = 0.0;
    Float fpitch = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Float ye = 0.0;
    Float pe = 0.0;
    Float d = 0.0;
    Float now = 0.0;
    Bool either = false;
    Float best = 0.0;
    controls_AimAssist a = controls_AimAssist{false, 1.0, 0.0, 0.0};
    s = kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 100.0); }, settings::Settings_get(ui_.settings, Opt::foot_aim_assist).Result);
    if (logic_or(logic_or(math_compare<"<=">(s, 0.0), math_compare<"<=">(input, 0.02)), player_dead_)) {
      return {struct_make<controls_AimAssist>(false, 1.0, 0.0, 0.0)};
    }
    eye = cam_.eye;
    fwd = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{1.0, 0.0, 0.0});
    fyaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(fwd).y, vector_break(fwd).x);
    fpitch = kern::formula([](Float z) -> Float { return kern::math::asin((z < (-1.0) ? (-1.0) : (1.0 < z ? 1.0 : z))); }, vector_break(fwd).z);
    p = Vector{0.0, 0.0, 0.0};
    ye = 0.0;
    pe = 0.0;
    d = 0.0;
    now = ctl_.clock;
    either = math_compare<">=">(kern::formula([](Float now, Float scan_at) -> Float { return (now - scan_at); }, now, game_controls_g_aim.scan_at), 0.115);
    if (logic_not(either)) {
      n4194323 = Game_ctl_assist_point(p, game_controls_g_aim.kind, game_controls_g_aim.index);
      either = logic_not(n4194323.Result);
    }
    if (either) {
      game_controls_g_aim = struct_make<AimTarget>(0, -1, -99.0);
      game_controls_g_aim.scan_at = now;
      best = 1000000000.0;
      for (auto it4194327 = flow_for(n4194327, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<People_Person>(people_.people_))))))))); it4194327.next();) {
        Game_ctl_assist_consider(best, game_controls_g_aim, eye, fyaw, fpitch, 1, n4194327.index);
      }
      for (auto it4194328 = flow_for(n4194328, 0, 4); it4194328.next();) {
        Game_ctl_assist_consider(best, game_controls_g_aim, eye, fyaw, fpitch, 2, n4194328.index);
      }
      for (auto it4194329 = flow_for(n4194329, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))))))); it4194329.next();) {
        Game_ctl_assist_consider(best, game_controls_g_aim, eye, fyaw, fpitch, 3, n4194329.index);
      }
      n4194330 = Game_ctl_assist_point(p, game_controls_g_aim.kind, game_controls_g_aim.index);
      if (logic_not(n4194330.Result)) {
        return {struct_make<controls_AimAssist>(false, 1.0, 0.0, 0.0)};
      }
    }
    ctl_assist_errors(ye, pe, d, eye, fyaw, fpitch, p);
    n4194331 = controls_cpp::fps_aim_assist(ye, pe, d, input, in_yaw, in_pitch, s);
    a = n4194331.Result;
    ctl_.assist_on = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float assist_on, Float active, Float slowdown) -> Float { return kern::math::bor(assist_on, (active && ((1.0 - slowdown) > 0.0015))); }, kern::to<Float>(ctl_.assist_on), kern::to<Float>(a.active), a.slowdown)))), 0.0);
    return {a};
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
    n4194332 = project_screen(nx, ny, depth, p);
    on = n4194332.Result;
    x = kern::formula([](Float nx, Float frame_w_) -> Float { return (kern::math::div((nx + 1.0), 2.0) * frame_w_); }, nx, frame_w_);
    y = kern::formula([](Float ny, Float frame_h_) -> Float { return (kern::math::div((1.0 - ny), 2.0) * frame_h_); }, ny, frame_h_);
    return {logic_and(on, math_compare<">">(depth, 0.0))};
  }

  void ctl_view_json_toast(ViewTrack& game_controls_g_view_2, const Text& body_2) {
    game_controls_g_view_2.toast_serial = kern::to<Int>(kern::formula([](Float toast_serial) -> Float { return kern::math::wrap32(toast_serial + 1.0); }, kern::to<Float>(game_controls_g_view_2.toast_serial)));
    list_add(game_controls_g_view_2.toasts, text_append(text_append(text_append(text_append("{\"id\":", text_format<"%u">(-1, game_controls_g_view_2.toast_serial), ""), ",", ""), body_2, ""), "}", ""));
    if (math_compare<">">(kern::to<Float>(list_length<Text>(game_controls_g_view_2.toasts)), 5.0)) {
      list_remove_at(game_controls_g_view_2.toasts, 0);
    }
  }

  struct ctl_view_jsonResult { Text Result = ""; };
  ctl_view_jsonResult ctl_view_json() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Text s = "";
    Text b = "";
    Float hp = 0.0;
    game_Hud hud = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
    Float dhp = 0.0;
    Float jhp = 0.0;
    Float chp = 0.0;
    BodyState st = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float v = 0.0;
    Float fx = 0.0;
    Float fy = 0.0;
    Bool fpm = false;
    Bool landed = false;
    Float rocket_left = 0.0;
    Int stars = 0;
    Int active = 0;
    Int in_range = 0;
    Float inbound = 0.0;
    Vector me = Vector{0.0, 0.0, 0.0};
    Float emp_range = 0.0;
    Bool skip = false;
    Bool searching = false;
    Float escape_left = 0.0;
    Float chosen = 0.0;
    Float next_wave = 0.0;
    combat_Contract k = combat_Contract{"", "", "", 0, 0};
    Bool died = false;
    Bool drone_lost = false;
    combat_Contract pk = combat_Contract{"", "", "", 0, 0};
    Text a = "";
    Text a_2 = "";
    Text a_3 = "";
    Text a_4 = "";
    Text a_5 = "";
    Text a_6 = "";
    Text a_7 = "";
    Float lx = 0.0;
    Float ly = 0.0;
    Float ld = 0.0;
    Bool lock = false;
    Text label = "";
    guided_Flight g = guided_Flight{guided_Missile{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, guided_Dodge{0.0, false}, Vector{0.0, 0.0, 0.0}, true}, guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"}, Vector{0.0, 0.0, 0.0}};
    Vector dir = Vector{0.0, 0.0, 0.0};
    the_RayResult_ r = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics = 0;
    Bool actor = false;
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool both = false;
    Float reach = 0.0;
    Int at = 0;
    Int count = 0;
    Bool stop_2 = false;
    Bool skip_2 = false;
    Vector to = Vector{0.0, 0.0, 0.0};
    Float along = 0.0;
    Bool first = false;
    Bool skip_3 = false;
    Float x = 0.0;
    Float y = 0.0;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    s = ",\"view\":{";
    b = "";
    b = text_format<"\"eye\":[%.2f,%.2f,%.2f]">(639, vector_break(cam_.eye).x, vector_break(cam_.eye).y, vector_break(cam_.eye).z);
    s = text_append(s, b, "");
    hud = engine_game_world_hud(world_);
    hp = struct_break<game_Hud>(hud).health;
    dhp = drone_hp_;
    jhp = jet_hp_;
    chp = kern::formula([](Float car_index_, Float a) -> Float { return ((car_index_ >= 0.0) ? a : (-1.0)); }, kern::to<Float>(car_index_), cars_[car_index_].hp);
    if (logic_or(logic_or(logic_or(logic_and(logic_and(math_compare<">=">(game_controls_g_view.last_hp, 0.0), math_compare<">=">(hp, 0.0)), math_compare<"<">(hp, kern::formula([](Float last_hp) -> Float { return (last_hp - 0.01); }, game_controls_g_view.last_hp))), logic_and(enum_equal<Mode>(mode_, Mode::drone), math_compare<"<">(dhp, kern::formula([](Float last_drone_hp) -> Float { return (last_drone_hp - 0.01); }, game_controls_g_view.last_drone_hp)))), logic_and(enum_equal<Mode>(mode_, Mode::jet), math_compare<"<">(jhp, kern::formula([](Float last_jet_hp) -> Float { return (last_jet_hp - 0.01); }, game_controls_g_view.last_jet_hp)))), logic_and(logic_and(logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(chp, 0.0)), math_compare<">=">(game_controls_g_view.last_car_hp, 0.0)), math_compare<"<">(chp, kern::formula([](Float last_car_hp) -> Float { return (last_car_hp - 0.01); }, game_controls_g_view.last_car_hp))))) {
      game_controls_g_view.dmg = kern::to<Int>(kern::formula([](Float dmg) -> Float { return kern::math::wrap32(dmg + 1.0); }, kern::to<Float>(game_controls_g_view.dmg)));
    }
    game_controls_g_view.last_hp = hp;
    game_controls_g_view.last_drone_hp = dhp;
    game_controls_g_view.last_jet_hp = jhp;
    game_controls_g_view.last_car_hp = chp;
    b = text_format<",\"dmg\":%u,\"droneHits\":%u,\"assist\":%s">(639, game_controls_g_view.dmg, ctl_.drone_hits, logic_select<Text>(ctl_.assist_on, "true", "false"));
    s = text_append(s, b, "");
    if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0))) {
      b = text_format<",\"carKmh\":%.0f">(639, kern::formula([](Float the_result) -> Float { return (kern::math::abs(the_result) * 3.6); }, car::Car_forward_speed(cars_[car_index_].car[0]).Result));
      s = text_append(s, b, "");
    }
    b = text_format<",\"cockpit\":%s">(639, logic_select<Text>(car_cockpit_, "true", "false"));
    s = text_append(s, b, "");
    b = text_format<",\"xboxAim\":%s">(639, logic_select<Text>(logic_and(logic_and(logic_and(ctl_.aim_scheme, enum_equal<Mode>(mode_, Mode::drone)), pad.connected), game::PadState_down(pad, enum_index<PadButtonIndex>(PadButtonIndex::kPadLB)).Result), "true", "false"));
    s = text_append(s, b, "");
    if (logic_and(enum_equal<Mode>(mode_, Mode::jet), jet_placed_)) {
      n4194333 = state_of(jet_actor_);
      st = n4194333.Result;
      v = vector_length(st.velocity);
      fx = 0.0;
      fy = 0.0;
      fpm = false;
      if (math_compare<">">(v, 15.0)) {
        n4194334 = Game_ctl_view_json_screen(fx, fy, vector_add(st.position, vector_scale(st.velocity, 4.0)));
        fpm = n4194334.Result;
      }
      landed = logic_or(jet::Jet_landed(jet_).Result, logic_and(math_compare<">=">(kern::to<Float>(jet_.tele_.contacts), 2.0), math_compare<"<">(v, 4.0)));
      rocket_left = kern::formula([](Float next_jet_missile_, Float now) -> Float { return (kern::math::max(0.0, (next_jet_missile_ - now)) * 1000.0); }, next_jet_missile_, now);
      b = text_format<",\"jet\":{\"kmh\":%.0f,\"mach\":%.3f,\"agl\":%.1f,\"nz\":%.2f,\"alpha\":%.2f,\"flight\":%s,\"nozzleCos\":%.3f,\"lever\":%.3f,\"thrust\":%.3f,\"landed\":%s,\"hp\":%.0f,\"burning\":%s,\"rocketLeft\":%.0f,\"fpm\":%s,\"fx\":%.1f,\"fy\":%.1f}">(639, kern::formula([](Float v) -> Float { return (v * 3.6); }, v), jet_.tele_.mach, jet_.tele_.agl, jet_.tele_.nz, kern::formula([](Float v, Float alpha, Float kDeg) -> Float { return ((v > 20.0) ? kern::math::div(alpha, kDeg) : 0.0); }, v, jet_.tele_.alpha, kDeg), logic_select<Text>(enum_equal<Jet_Mode>(jet::Jet_mode(jet_).Result, Jet_Mode::flight), "true", "false"), kern::formula([](Float the_result) -> Float { return kern::math::cos(the_result); }, jet::Jet_nozzle(jet_).Result), jet::Jet_lever(jet_).Result, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float the_result, Float thrust_max) -> Float { return kern::math::div(the_result, thrust_max); }, jet::Jet_thrust(jet_).Result, jet_.s_.thrust_max), 0.0, 1.0)), logic_select<Text>(landed, "true", "false"), kern::formula([](Float jet_hp_) -> Float { return kern::math::round((kern::math::div(jet_hp_, 400.0) * 100.0)); }, jet_hp_), logic_select<Text>(logic_and(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0), gaps_[0].jet_burning), "true", "false"), rocket_left, logic_select<Text>(fpm, "true", "false"), fx, fy);
      s = text_append(s, b, "");
    }
    n4194335 = wanted_cpp::stars();
    stars = n4194335.Result;
    if (logic_and(math_compare<">">(kern::to<Float>(game_controls_g_view.last_stars), 0.0), math_compare<"==">(kern::to<Float>(stars), 0.0))) {
      game_controls_g_view.banner_until = kern::formula([](Float now) -> Float { return (now + 2.1); }, now);
      game_controls_g_view.busted = player_dead_;
    }
    game_controls_g_view.last_stars = stars;
    active = 0;
    in_range = 0;
    inbound = 1000000000.0;
    n4194336 = player_position();
    me = n4194336.Result;
    emp_range = wanted_cpp::emp_range().Result;
    for (auto it4194337 = flow_for(n4194337, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it4194337.next();) {
      skip = false;
      if (logic_not(pdrones_[n4194337.index].active)) {
        skip = true;
      }
      if (logic_not(skip)) {
        active += 1;
        if (logic_and(logic_not(pdrones_[n4194337.index].emp), math_compare<"<=">(vector_length(vector_sub(pdrones_[n4194337.index].pos, me)), emp_range))) {
          in_range += 1;
        }
        if (math_compare<"==">(kern::to<Float>(pdrones_[n4194337.index].shots), 0.0)) {
          inbound = kern::formula([](Float inbound, Float a, Float now) -> Float { return kern::math::min(inbound, (a - now)); }, inbound, pdrones_[n4194337.index].next_shot, now);
        }
      }
    }
    searching = logic_and(math_compare<">">(kern::to<Float>(stars), 0.0), math_compare<">">(wanted_cpp::hidden_ms().Result, 0.0));
    if (math_compare<">">(kern::to<Float>(stars), 0.0)) {
      n4194339 = wanted_cpp::escape_ms(stars);
      chosen = kern::formula([](Float the_result, Float the_result_2) -> Float { return kern::math::max(0.0, (the_result - the_result_2)); }, n4194339.Result, wanted_cpp::hidden_ms().Result);
    } else {
      chosen = 0.0;
    }
    escape_left = kern::formula([](Float chosen) -> Float { return chosen; }, chosen);
    next_wave = kern::formula([](Float active, Float next_drone_wave_, Float now) -> Float { return (((active == 0.0) && (next_drone_wave_ > now)) ? ((next_drone_wave_ - now) * 1000.0) : (-1.0)); }, kern::to<Float>(active), next_drone_wave_, now);
    b = text_format<",\"wanted\":{\"stars\":%d,\"active\":%d,\"searching\":%s,\"escapeMs\":%.0f,\"inboundMs\":%.0f,\"nextWaveMs\":%.0f,\"banner\":%s,\"busted\":%s,\"empMs\":%.0f,\"inRange\":%d,\"miss\":%s}">(639, stars, active, logic_select<Text>(searching, "true", "false"), escape_left, kern::formula([](Float inbound) -> Float { return ((inbound < 100000000.0) ? (kern::math::max(0.0, inbound) * 1000.0) : 0.0); }, inbound), next_wave, logic_select<Text>(math_compare<"<">(now, game_controls_g_view.banner_until), "true", "false"), logic_select<Text>(game_controls_g_view.busted, "true", "false"), kern::formula([](Float emp_ready_at_, Float now) -> Float { return (kern::math::max(0.0, (emp_ready_at_ - now)) * 1000.0); }, emp_ready_at_, now), in_range, logic_select<Text>(math_compare<"<">(now, ctl_.emp_miss_until), "true", "false"));
    s = text_append(s, b, "");
    k = contract(combat_.fun.contract_index).Result;
    if (game_controls_g_view.fun_valid) {
      died = logic_and(player_dead_, logic_not(game_controls_g_view.was_dead));
      drone_lost = logic_and(logic_and(logic_not(drone_out_), game_controls_g_view.drone_was_alive), math_compare<"<=">(drone_hp_, 0.0));
      if (logic_and(combat_.fun.contract_complete, logic_not(game_controls_g_view.fun.contract_complete))) {
        pk = contract(game_controls_g_view.fun.contract_index).Result;
        a = text_append(text_append(text_append("\"type\":\"contractComplete\",\"name\":\"", game_controls_cpp::jesc(pk.name).Result, ""), "\",\"reward\":", ""), text_format<"%d">(-1, pk.reward), "");
        ctl_view_json_toast(game_controls_g_view, a);
      }
      if (logic_and(died, math_compare<">">(kern::to<Float>(game_controls_g_view.fun.at_risk), 0.0))) {
        a_2 = text_append(text_append(text_append("\"type\":\"insurance\",\"insured\":", text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float score, Float score_2) -> Float { return kern::math::max(0.0, (score - score_2)); }, kern::to<Float>(combat_.fun.score), kern::to<Float>(game_controls_g_view.fun.score)))), ""), ",\"lost\":", ""), text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float at_risk, Float at_risk_2, Float score, Float score_2) -> Float { return kern::math::max(0.0, ((at_risk - at_risk_2) - kern::math::max(0.0, (score - score_2)))); }, kern::to<Float>(game_controls_g_view.fun.at_risk), kern::to<Float>(combat_.fun.at_risk), kern::to<Float>(combat_.fun.score), kern::to<Float>(game_controls_g_view.fun.score)))), "");
        ctl_view_json_toast(game_controls_g_view, a_2);
      } else if (logic_and(drone_lost, math_compare<">">(kern::to<Float>(game_controls_g_view.fun.at_risk), kern::to<Float>(combat_.fun.at_risk)))) {
        a_3 = text_append(text_append(text_append(text_append(text_append("\"type\":\"setback\",\"label\":\"", game_controls_cpp::jesc(combat_.fun.last_label).Result, ""), "\",\"lost\":", ""), text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float at_risk, Float at_risk_2) -> Float { return (at_risk - at_risk_2); }, kern::to<Float>(game_controls_g_view.fun.at_risk), kern::to<Float>(combat_.fun.at_risk)))), ""), ",\"mercy\":", ""), text_format<"%d">(-1, combat_.fun.mercy), "");
        ctl_view_json_toast(game_controls_g_view, a_3);
      } else if (logic_and(logic_and(math_compare<">">(kern::to<Float>(combat_.fun.score), kern::to<Float>(game_controls_g_view.fun.score)), math_compare<"==">(kern::to<Float>(combat_.fun.at_risk), 0.0)), math_compare<">">(kern::to<Float>(game_controls_g_view.fun.at_risk), 0.0))) {
        a_4 = text_append(text_append(text_append(text_append(text_append("\"type\":\"bank\",\"points\":", text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float score, Float score_2) -> Float { return (score - score_2); }, kern::to<Float>(combat_.fun.score), kern::to<Float>(game_controls_g_view.fun.score)))), ""), ",\"peakStars\":", ""), text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float max_stars, Float stars) -> Float { return kern::math::max(max_stars, stars); }, kern::to<Float>(game_controls_g_view.fun.max_stars), kern::to<Float>(game_controls_g_view.fun.stars)))), ""), ",\"score\":", ""), text_format<"%d">(-1, combat_.fun.score), "");
        ctl_view_json_toast(game_controls_g_view, a_4);
      }
      if (logic_and(math_compare<">">(combat_.fun.overdrive_until, kern::formula([](Float overdrive_until) -> Float { return (overdrive_until + 0.01); }, game_controls_g_view.fun.overdrive_until)), math_compare<">">(combat_.fun.overdrive_until, now))) {
        a_5 = text_append(text_append(text_append("\"type\":\"overdrive\",\"durationMs\":", text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float combat_kOverdriveS) -> Float { return kern::math::trunc((combat_kOverdriveS * 1000.0)); }, combat_kOverdriveS))), ""), ",\"bonus\":", ""), text_format<"%d">(-1, kern::to<Int>(kern::formula([](Float at_risk, Float at_risk_2) -> Float { return kern::math::max(0.0, (at_risk - at_risk_2)); }, kern::to<Float>(combat_.fun.at_risk), kern::to<Float>(game_controls_g_view.fun.at_risk)))), "");
        ctl_view_json_toast(game_controls_g_view, a_5);
      } else if (logic_and(logic_and(math_compare<"!=">(kern::to<Float>(combat_.fun.last_award), kern::to<Float>(game_controls_g_view.fun.last_award)), math_compare<">=">(kern::to<Float>(combat_.fun.combo), 2.0)), math_compare<">">(kern::to<Float>(combat_.fun.at_risk), kern::to<Float>(game_controls_g_view.fun.at_risk)))) {
        n4194340 = combat_rules_cpp::Fun_multiplier(combat_.fun, now);
        b = text_format<"\"type\":\"award\",\"points\":%d,\"label\":\"%s\",\"combo\":%d,\"multiplier\":%.2f">(639, kern::to<Int>(kern::formula([](Float at_risk, Float at_risk_2) -> Float { return (at_risk - at_risk_2); }, kern::to<Float>(combat_.fun.at_risk), kern::to<Float>(game_controls_g_view.fun.at_risk))), game_controls_cpp::jesc(combat_.fun.last_label).Result, combat_.fun.combo, n4194340.Result);
        a_6 = b;
        ctl_view_json_toast(game_controls_g_view, a_6);
      }
      if (logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(game_controls_g_view.fun.combo), 2.0), math_compare<"==">(kern::to<Float>(combat_.fun.combo), 0.0)), math_compare<"==">(kern::to<Float>(combat_.fun.score), kern::to<Float>(game_controls_g_view.fun.score))), logic_not(died))) {
        a_7 = text_append("\"type\":\"comboBreak\",\"combo\":", text_format<"%d">(-1, game_controls_g_view.fun.combo), "");
        ctl_view_json_toast(game_controls_g_view, a_7);
      }
    }
    game_controls_g_view.fun = combat_.fun;
    game_controls_g_view.fun_valid = true;
    game_controls_g_view.was_dead = player_dead_;
    game_controls_g_view.drone_was_alive = drone_out_;
    n4194341 = combat_rules_cpp::Fun_police_damage_scale(combat_.fun);
    n4194342 = combat_rules_cpp::Fun_multiplier(combat_.fun, now);
    n4194343 = wanted_cpp::stars();
    b = text_format<",\"fun\":{\"reward\":%d,\"complete\":%s,\"overdrive\":%s,\"momentum\":%d,\"mercy\":%d,\"damageScale\":%.2f,\"risk\":%d,\"score\":%d,\"high\":%d,\"mult\":%.2f,\"name\":\"%s\",\"hint\":\"%s\",\"progress\":%d,\"target\":%d,\"stars\":%d,\"toasts\":[">(639, k.reward, logic_select<Text>(combat_.fun.contract_complete, "true", "false"), logic_select<Text>(math_compare<"<">(now, combat_.fun.overdrive_until), "true", "false"), combat_.fun.momentum, combat_.fun.mercy, n4194341.Result, combat_.fun.at_risk, combat_.fun.score, combat_.fun.high_score, n4194342.Result, game_controls_cpp::jesc(k.name).Result, game_controls_cpp::jesc(k.hint).Result, combat_.fun.contract_progress, k.target, n4194343.Result);
    s = text_append(s, b, "");
    for (auto it4194344 = flow_for(n4194344, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Text>(game_controls_g_view.toasts))))); it4194344.next();) {
      s = text_append(s, text_append(logic_select<Text>(math_compare<"!=">(kern::to<Float>(n4194344.index), 0.0), ",", ""), game_controls_g_view.toasts[n4194344.index], ""), "");
    }
    s = text_append(s, "]}", "");
    if (enum_equal<Mode>(mode_, Mode::drone)) {
      lx = 0.0;
      ly = 0.0;
      ld = 0.0;
      lock = false;
      label = "";
      if (logic_and(logic_not(math_compare<"==">(kern::to<Float>(list_length<guided_Flight>(guided_)), 0.0)), guided_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))].m.tracking)) {
        g = guided_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))];
        n4194345 = Game_ctl_view_json_screen(lx, ly, g.m.goal);
        lock = n4194345.Result;
        ld = vector_length(vector_sub(g.m.goal, cam_.eye));
        label = g.t.label;
      }
      dir = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{1.0, 0.0, 0.0});
      engine_game_world_physics(n4194346, world_);
      physics = n4194346.result;
      r = common::cast(physics, cam_.eye, dir, 600.0, body(drone_actor_).Result, int64_or(int64_or(Int64{0x2}, Int64{0x4}), Int64{0x1})).Result;
      actor = false;
      for (auto it4194347 = flow_for(n4194347, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it4194347.next();) {
        both = logic_and(r.hit, cars_[n4194347.index].placed);
        if (both) {
          n4194349 = matrix_of(cars_[n4194347.index].actor);
          a_9 = n4194349.Result;
          both = math_compare<"<">(vector_length(vector_sub(engine_math_math::Mat4_translation(a_9).Result, r.point)), kern::formula([](Float x) -> Float { return (x + 1.5); }, vector_break(cars_[n4194347.index].car[0].s_.half).x));
        }
        if (both) {
          actor = true;
        }
      }
      reach = kern::formula([](Float hit, Float distance) -> Float { return ((hit != 0.0) ? distance : 600.0); }, kern::to<Float>(r.hit), r.distance);
      count = list_length<People_Person>(people_.people_);
      at = 0;
      stop_2 = false;
      while (loop(logic_and(logic_not(stop_2), math_compare<"<">(kern::to<Float>(at), kern::to<Float>(count))))) {
        skip_2 = false;
        if (logic_or(enum_equal<People_State>(people_.people_[at].state, People_State::gone), enum_equal<People_State>(people_.people_[at].state, People_State::down))) {
          skip_2 = true;
        }
        if (logic_not(logic_or(stop_2, skip_2))) {
          to = vector_sub(vector_add(people_.people_[at].pos, Vector{0.0, 0.0, 1.0}), cam_.eye);
          along = vector_dot(to, dir);
          if (logic_and(logic_and(math_compare<">">(along, 0.0), math_compare<"<">(along, reach)), math_compare<"<">(vector_length(vector_sub(to, vector_scale(dir, along))), 1.2))) {
            actor = true;
            stop_2 = true;
          }
        }
        if (logic_not(stop_2)) {
          at += 1;
        }
      }
      b = text_format<",\"guidedView\":{\"lock\":%s,\"x\":%.1f,\"y\":%.1f,\"d\":%.0f,\"label\":\"%s\",\"denied\":%s,\"actor\":%s,\"cdMs\":%.0f}">(639, logic_select<Text>(lock, "true", "false"), lx, ly, ld, game_controls_cpp::jesc(label).Result, logic_select<Text>(math_compare<"<">(now, ctl_.guided_denied_until), "true", "false"), logic_select<Text>(actor, "true", "false"), kern::formula([](Float guided_last_, Float the_result, Float now) -> Float { return (kern::math::max(0.0, ((guided_last_ + the_result) - now)) * 1000.0); }, guided_last_, guided_cpp::cooldown().Result, now));
      s = text_append(s, b, "");
    }
    b = text_format<",\"strikeK\":%.3f">(639, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float last_strike, Float combat_kStrikeCooldownS) -> Float { return kern::math::div((now - last_strike), combat_kStrikeCooldownS); }, now, combat_.last_strike, combat_kStrikeCooldownS), 0.0, 1.0)));
    s = text_append(s, b, "");
    s = text_append(s, ",\"impacts\":[", "");
    first = true;
    for (auto it4194350 = flow_for(n4194350, kern::to<Int>(kern::formula([](Float impact_serial) -> Float { return ((impact_serial > 8.0) ? kern::math::wrap32(impact_serial - 8.0) : 0.0); }, kern::to<Float>(ctl_.impact_serial))), kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(ctl_.impact_serial)))); it4194350.next();) {
      skip_3 = false;
      x = 0.0;
      y = 0.0;
      n4194351 = Game_ctl_view_json_screen(x, y, ctl_.impacts[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::mod(index, 8.0); }, kern::to<Float>(n4194350.index)))]);
      if (logic_not(n4194351.Result)) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        b = text_format<"%s[%u,%.0f,%.0f]">(639, logic_select<Text>(first, "", ","), kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index + 1.0); }, kern::to<Float>(n4194350.index))), x, y);
        s = text_append(s, b, "");
        first = false;
      }
    }
    s = text_append(s, "]", "");
    return {text_append(s, "}", "")};
  }

  void car_input(Int e = 0, Float dt = 0.0) {
    Int e_ = 0;
    Int in = 0;
    Int input = 0;
    CarInput ci = CarInput{0.0, 0.0, false, 32.0, 9.0};
    Float steer = 0.0;
    Float pedal = 0.0;
    Float look_x = 0.0;
    Float look_y = 0.0;
    Bool brake = false;
    Bool down = false;
    Text a = "";
    Text a_2 = "";
    Bool either = false;
    Bool xr = false;
    Bool xr_2 = false;
    Bool xr_3 = false;
    Bool both = false;
    e_ = e;
    engine_engine_input(n4194365, e_);
    input = n4194365.result;
    in = input;
    if (math_compare<"<">(kern::to<Float>(car_index_), 0.0)) {
      return;
    }
    ci = CarInput{0.0, 0.0, false, 32.0, 9.0};
    ci.max_speed = 31.944444444444443;
    ci.max_reverse = 8.88888888888889;
    steer = kern::formula([](Float the_result, Float the_result_2) -> Float { return (the_result - the_result_2); }, game_controls_cpp::key(in, Key::d).Result, game_controls_cpp::key(in, Key::a).Result);
    pedal = kern::formula([](Float the_result, Float the_result_2) -> Float { return (the_result - the_result_2); }, game_controls_cpp::key(in, Key::w).Result, game_controls_cpp::key(in, Key::s).Result);
    look_x = 0.0;
    look_y = 0.0;
    if (touch_mode_) {
      steer += vector_break(touch_play::TouchPlay_move(pad_touch_).Result).x;
      pedal += kern::formula([](Float y) -> Float { return (-y); }, vector_break(touch_play::TouchPlay_move(pad_touch_).Result).y);
      look_x += vector_break(touch_play::TouchPlay_look(pad_touch_).Result).x;
      look_y += kern::formula([](Float y) -> Float { return (-y); }, vector_break(touch_play::TouchPlay_look(pad_touch_).Result).y);
    }
    down = engine_input_down(in, Key::space);
    either = down;
    if (logic_not(either)) {
      a_2 = "hb";
      either = touch_play_cpp::TouchPlay_held(pad_touch_, a_2).Result;
    }
    brake = either;
    both = math_compare<"!=">(kern::to<Float>(engine_), 0.0);
    if (both) {
      xr_3 = engine_engine_xr(engine_);
      both = xr_3;
    }
    xr = both;
    if (pad.connected) {
      n4194366 = controls_cpp::gamepad_axis(pad.lx, 0.08);
      steer += n4194366.Result;
      pedal += kern::formula([](Float rt, Float lt) -> Float { return (rt - lt); }, pad.rt, pad.lt);
      if (logic_not(xr)) {
        n4194367 = controls_cpp::gamepad_axis(pad.rx, 0.08);
        look_x += n4194367.Result;
        n4194368 = controls_cpp::gamepad_axis(pad.ry, 0.08);
        look_y += n4194368.Result;
      }
      brake = logic_or(brake, game::PadState_down(pad, enum_index<PadButtonIndex>(PadButtonIndex::kPadB)).Result);
    }
    ci.steer = kern::formula([](Float steer) -> Float { return (-(steer < (-1.0) ? (-1.0) : (1.0 < steer ? 1.0 : steer))); }, steer);
    ci.pedal = kern::formula([](Float pedal) -> Float { return (pedal < (-1.0) ? (-1.0) : (1.0 < pedal ? 1.0 : pedal)); }, pedal);
    ci.handbrake = brake;
    cars_[car_index_].input = ci;
    controls_cpp::CarHeadSpring_step(ctl_.car_head, kern::formula([](Float look_x) -> Float { return (look_x < (-1.0) ? (-1.0) : (1.0 < look_x ? 1.0 : look_x)); }, look_x), kern::formula([](Float look_y) -> Float { return (look_y < (-1.0) ? (-1.0) : (1.0 < look_y ? 1.0 : look_y)); }, look_y), dt);
    if (logic_or(math_compare<">">(kern::formula([](Float x) -> Float { return (x != 0.0); }, vector_break(ctl_.md).x), 0.5), math_compare<">">(kern::formula([](Float y) -> Float { return (y != 0.0); }, vector_break(ctl_.md).y), 0.5))) {
      controls_cpp::WorldLook_add(ctl_.look, vector_break(ctl_.md).x, vector_break(ctl_.md).y, kLookDegPx, kern::formula([](Float kLookDegPx) -> Float { return (kLookDegPx * 0.85); }, kLookDegPx));
    }
    head_yaw_ = kern::formula([](Float xr, Float yaw, Float yaw_deg, Float kRad) -> Float { return ((xr != 0.0) ? 0.0 : (-(yaw + kern::math::div(yaw_deg, kRad)))); }, kern::to<Float>(xr), ctl_.car_head.yaw, ctl_.look.yaw_deg, kRad);
    head_pitch_ = kern::formula([](Float xr, Float pitch, Float pitch_deg, Float kRad) -> Float { return ((xr != 0.0) ? 0.0 : (pitch + kern::math::div(pitch_deg, kRad))); }, kern::to<Float>(xr), ctl_.car_head.pitch, ctl_.look.pitch_deg, kRad);
    if (math_compare<">">(kern::formula([](Float wheel) -> Float { return (wheel != 0.0); }, ctl_.wheel), 0.5)) {
      ctl_.car_zoom = kern::formula([](Float car_zoom, Float wheel) -> Float { return (car_zoom * ((wheel > 0.0) ? 1.1 : kern::math::div(1.0, 1.1))); }, ctl_.car_zoom, ctl_.wheel);
    }
    if (touch_mode_) {
      n4194369 = touch_play_cpp::TouchPlay_take_zoom(pad_touch_);
      ctl_.car_zoom = kern::formula([](Float car_zoom, Float the_result) -> Float { return (car_zoom * the_result); }, ctl_.car_zoom, n4194369.Result);
    }
    ctl_.car_zoom = kern::formula([](Float car_zoom) -> Float { return (car_zoom < 0.55 ? 0.55 : (2.6 < car_zoom ? 2.6 : car_zoom)); }, ctl_.car_zoom);
  }

  void parachute_input(Int e = 0, Float dt = 0.0) {
    Int e_ = 0;
    Int in = 0;
    Int input = 0;
    Float roll = 0.0;
    Float pitch = 0.0;
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    the_RayResult_ g = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics = 0;
    Float camera_yaw = 0.0;
    e_ = e;
    engine_engine_input(n4194378, e_);
    input = n4194378.result;
    in = input;
    roll = kern::formula([](Float the_result, Float the_result_2) -> Float { return (the_result - the_result_2); }, game_controls_cpp::key(in, Key::right).Result, game_controls_cpp::key(in, Key::left).Result);
    pitch = kern::formula([](Float the_result, Float the_result_2) -> Float { return (the_result - the_result_2); }, game_controls_cpp::key(in, Key::down).Result, game_controls_cpp::key(in, Key::up).Result);
    if (touch_mode_) {
      roll += vector_break(touch_play::TouchPlay_look(pad_touch_).Result).x;
      pitch += vector_break(touch_play::TouchPlay_look(pad_touch_).Result).y;
    }
    if (logic_and(ctl_.locked, logic_not(touch_mode_))) {
      controls_cpp::RateStick_step(ctl_.jet_mouse, ctl_.md, dt);
      roll += vector_break(ctl_.jet_mouse.rate).x;
      pitch = kern::formula([](Float pitch, Float y) -> Float { return (pitch - y); }, pitch, vector_break(ctl_.jet_mouse.rate).y);
    }
    if (pad.connected) {
      n4194379 = controls_cpp::hard_cut(pad.rx, 0.12);
      roll += n4194379.Result;
      n4194380 = controls_cpp::hard_cut(kern::formula([](Float ry) -> Float { return (-ry); }, pad.ry), 0.12);
      pitch += n4194380.Result;
    }
    chute_steer_ = logic_select<Vector>(chute_open_, vector_make(kern::formula([](Float roll) -> Float { return (roll < (-1.0) ? (-1.0) : (1.0 < roll ? 1.0 : roll)); }, roll), kern::formula([](Float pitch) -> Float { return (-(pitch < (-1.0) ? (-1.0) : (1.0 < pitch ? 1.0 : pitch))); }, pitch), 0.0), Vector{0.0, 0.0, 0.0});
    head_yaw_ = 0.0;
    n4194381 = state_of(chute_actor_);
    s = n4194381.Result;
    engine_game_world_physics(n4194382, world_);
    physics = n4194382.result;
    g = common::cast(physics, s.position, Vector{0.0, 0.0, -1.0}, 2.0, body(chute_actor_).Result, Int64{0xffffffffffffffff}).Result;
    if (logic_and(logic_and(g.hit, math_compare<"<">(g.distance, 1.0)), math_compare<">">(chute_time_, 0.5))) {
      engine_game_world_set_alive(world_, chute_actor_, false);
      camera_yaw = engine_game_world_camera_yaw(world_);
      show_soldier(g.point, camera_yaw);
      mode_ = Mode::foot;
    }
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

  struct touchResult { Bool Result = false; };
  touchResult touch(Int type = 0, Int id = 0, Float x = 0.0, Float y = 0.0, Float t_ms = 0.0) {
    if (math_compare<"==">(kern::to<Float>(type), 0.0)) {
      controls_cpp::Devices_touch_down(ctl_.dev);
    }
    touch_mode_ = logic_not(ctl_.dev.desktop);
    n4194383 = touch_play_cpp::TouchPlay_pointer(pad_touch_, type, id, x, y, t_ms);
    return {n4194383.Result};
  }

  void update_touch_layout() {
    TouchLayout l = TouchLayout{TouchMode::off, true, "", false, false, false, 0, -1.0, false, false, false, true, false, false, false, false, false, false, false, false};
    Vector me = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool both = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float chosen = 0.0;
    Float sim_time = 0.0;
    Float sim_time_2 = 0.0;
    l = TouchLayout{TouchMode::off, true, "", false, false, false, 0, -1.0, false, false, false, true, false, false, false, false, false, false, false, false};
    if (logic_and(touch_mode_, world_placed_)) {
      switch (flow_switch_enum<Mode>(mode_)) {
        case Mode::foot: {
          l.mode = TouchMode::foot;
        } break;
        case Mode::drone: {
          l.mode = TouchMode::drone;
        } break;
        case Mode::car: {
          l.mode = TouchMode::car;
        } break;
        case Mode::jet: {
          l.mode = TouchMode::jet;
        } break;
        case Mode::parachute: {
          l.mode = TouchMode::chute;
        } break;
      }
    }
    l.alive = logic_not(player_dead_);
    l.pad_drone = ctl_.pad_drone;
    l.aiming = ctl_.aiming;
    if (enum_equal<Mode>(mode_, Mode::foot)) {
      n4194384 = matrix_of(soldier_);
      a = n4194384.Result;
      me = engine_math_math::Mat4_translation(a).Result;
      both = jet_placed_;
      if (both) {
        n4194386 = matrix_of(jet_actor_);
        a_3 = n4194386.Result;
        n4194387 = matrix_of(jet_actor_);
        a_4 = n4194387.Result;
        both = math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(engine_math_math::Mat4_translation(a_3).Result).x, vector_break(me).x, vector_break(engine_math_math::Mat4_translation(a_4).Result).y, vector_break(me).y), kEnterJetM);
      }
      l.near_jet = both;
      for (auto it4194388 = flow_for(n4194388, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it4194388.next();) {
        if (logic_and(cars_[n4194388.index].placed, logic_not(cars_[n4194388.index].wreck))) {
          n4194389 = matrix_of(cars_[n4194388.index].actor);
          a_5 = n4194389.Result;
          p = engine_math_math::Mat4_translation(a_5).Result;
          l.near_car = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float near_car, Float x, Float x_2, Float y, Float y_2, Float kEnterCarM) -> Float { return kern::math::bor(near_car, (kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))) < kEnterCarM)); }, kern::to<Float>(l.near_car), vector_break(p).x, vector_break(me).x, vector_break(p).y, vector_break(me).y, kEnterCarM)))), 0.0);
        }
      }
      n4194390 = gaps_boardable_2();
      l.near_car = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float near_car, Float the_result, Float a, Float a_2) -> Float { return kern::math::bor(near_car, ((the_result >= 0.0) || (a && a_2))); }, kern::to<Float>(l.near_car), kern::to<Float>(n4194390.Result), kern::to<Float>(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), kern::to<Float>(gaps_[0].riding))))), 0.0);
      l.weapon = struct_break<weapons_Spec>(list_get<weapons_Spec>(kTable, enum_index<weapons_Id>(foot_weapon_))).name;
      l.sniper = enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper);
      l.scope_on = ctl_.scope_on;
      l.grenade = true;
      l.grenades = ctl_.nade.carry;
      if (ctl_.nade.cooking) {
        n4194392 = controls_cpp::GrenadeCook_left(ctl_.nade, ctl_.clock);
        chosen = n4194392.Result;
      } else {
        chosen = -1.0;
      }
      l.grenade_left = kern::formula([](Float chosen) -> Float { return chosen; }, chosen);
      sim_time = engine_game_world_sim_time(world_);
      l.drone_cooldown = math_compare<"<">(sim_time, combat_.drone_ready_at);
    } else if (enum_equal<Mode>(mode_, Mode::drone)) {
      l.weapon = logic_select<Text>(math_compare<"==">(kern::to<Float>(drone_weapon_), 0.0), "MG", logic_select<Text>(math_compare<"==">(kern::to<Float>(drone_weapon_), 1.0), "RAKETEN", "GUIDED"));
      l.armed = drone_.tele_.armed;
    }
    ctl_.near_car = l.near_car;
    ctl_.near_jet = l.near_jet;
    n4194393 = wanted_cpp::stars();
    l.emp = logic_and(math_compare<">">(kern::to<Float>(n4194393.Result), 0.0), logic_or(enum_equal<Mode>(mode_, Mode::foot), enum_equal<Mode>(mode_, Mode::drone)));
    sim_time_2 = engine_game_world_sim_time(world_);
    l.strike_ready = math_compare<">=">(kern::formula([](Float sim_time_2, Float last_strike) -> Float { return (sim_time_2 - last_strike); }, sim_time_2, combat_.last_strike), combat_kStrikeCooldownS);
    l.cockpit = car_cockpit_;
    l.hover = enum_equal<Jet_Mode>(jet::Jet_mode(jet_).Result, Jet_Mode::hover);
    l.rocket = ctl_.rocket;
    l.ground = math_compare<">=">(kern::to<Float>(jet_.tele_.contacts), 2.0);
    touch_play_cpp::TouchPlay_set_layout(pad_touch_, l);
  }

  struct touch_view_jsonResult { Text Result = ""; };
  touch_view_jsonResult touch_view_json() {
    Text raw = "";
    Text out = "";
    Int c = 0;
    if (logic_not(touch_mode_)) {
      return {"null"};
    }
    n4194394 = touch_play_cpp::TouchPlay_view_json(pad_touch_);
    raw = n4194394.Result;
    out = "";
    out = text_reserve(out, kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 16.0); }, kern::to<Float>(text_length(raw)))));
    for (auto it4194395 = flow_for(n4194395, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(raw))))); it4194395.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(raw, n4194395.index))));
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

  struct ctl_hud_jsonResult { Text Result = ""; };
  ctl_hud_jsonResult ctl_hud_json() {
    Text m = "";
    Text hint = "";
    Text s = "";
    Text b = "";
    Float chosen = 0.0;
    Bool fade = false;
    if (ui_.paused) {
      ctl_.was_paused = true;
    }
    m = logic_select<Text>(enum_equal<Mode>(mode_, Mode::parachute), "jet", game_cpp::mode_name(mode_).Result);
    n4194396 = controls_cpp::hint_line(m, game_controls_cpp::hint_weapon(mode_, foot_weapon_, drone_weapon_).Result, player_dead_);
    hint = n4194396.Result;
    if (logic_not(text_equal(hint, ctl_.hint))) {
      ctl_.hint = hint;
      ctl_.hint_at = ctl_.clock;
    }
    s = ",\"ctl\":{\"device\":\"";
    s = text_append(s, logic_select<Text>(ctl_.dev.desktop, "desktop", "touch"), "");
    b = "";
    if (ctl_.nade.cooking) {
      n4194398 = controls_cpp::GrenadeCook_left(ctl_.nade, ctl_.clock);
      chosen = n4194398.Result;
    } else {
      chosen = -1.0;
    }
    b = text_format<"\",\"pad\":%s,\"locked\":%s,\"aiming\":%s,\"dead\":%s,\"rocket\":%s,\"scope\":%s,\"zoom\":%.3f,\"look\":[%.2f,%.2f],\"nade\":{\"carry\":%d,\"left\":%.2f},">(255, logic_select<Text>(ctl_.dev.pad, "true", "false"), logic_select<Text>(ctl_.locked, "true", "false"), logic_select<Text>(ctl_.aiming, "true", "false"), logic_select<Text>(player_dead_, "true", "false"), logic_select<Text>(ctl_.rocket, "true", "false"), logic_select<Text>(ctl_.scope_on, "true", "false"), ctl_.car_zoom, ctl_.look.yaw_deg, ctl_.look.pitch_deg, ctl_.nade.carry, kern::formula([](Float chosen) -> Float { return chosen; }, chosen));
    s = text_append(s, b, "");
    b = text_format<"\"lmb\":%s,\"rmb\":%s,\"trigger\":%s,\"fingers\":%s,">(255, logic_select<Text>(ctl_.lmb, "true", "false"), logic_select<Text>(ctl_.rmb, "true", "false"), logic_select<Text>(trigger_was_, "true", "false"), logic_select<Text>(touch_play::TouchPlay_any(pad_touch_).Result, "true", "false"));
    s = text_append(s, b, "");
    fade = logic_and(math_compare<">">(kern::formula([](Float clock, Float hint_at) -> Float { return (clock - hint_at); }, ctl_.clock, ctl_.hint_at), 12.0), logic_not(player_dead_));
    s = text_append(s, text_append(text_append("\"lastExit\":\"", game_controls_g_last_exit, ""), "\",", ""), "");
    s = text_append(s, text_append(text_append("\"hint\":\"", ctl_.hint, ""), "\",\"hintFaded\":", ""), "");
    s = text_append(s, logic_select<Text>(fade, "true", "false"), "");
    s = text_append(s, logic_select<Text>(ctl_.near_car, ",\"nearCar\":true", ",\"nearCar\":false"), "");
    s = text_append(s, logic_select<Text>(logic_and(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0), gaps_[0].riding), ",\"riding\":true", ",\"riding\":false"), "");
    n4194399 = gaps_boardable_2();
    s = text_append(s, logic_select<Text>(math_compare<">=">(kern::to<Float>(n4194399.Result), 0.0), ",\"board\":true", ",\"board\":false"), "");
    s = text_append(s, logic_select<Text>(ctl_.near_jet, ",\"nearJet\":true", ",\"nearJet\":false"), "");
    s = text_append(s, logic_select<Text>(ctl_.pad_drone, ",\"padDrone\":true", ",\"padDrone\":false"), "");
    b = text_format<",\"sticks\":[%.3f,%.3f,%.3f,%.3f],\"head\":[%.3f,%.3f]">(255, sticks_.roll, sticks_.pitch, sticks_.yaw, sticks_.body_pitch, head_yaw_, head_pitch_);
    s = text_append(s, b, "");
    n4194400 = ctl_view_json();
    s = text_append(s, n4194400.Result, "");
    return {text_append(s, "}", "")};
  }

  struct ctl_command_startsResult { Bool Result = false; };
  KERN_PURE ctl_command_startsResult ctl_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  void key_down_event() {
    controls_cpp::Devices_key(ctl_.dev);
  }

  struct matrix_ofResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  matrix_ofResult matrix_of(Int a = 0) {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool actor_matrix = false;
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    engine_game_world_actor_matrix(n5242896, world_, a, m);
    actor_matrix = n5242896.result;
    m = n5242896.out;
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
      n5242897 = matrix_of(cars_[car_index_].actor);
      a = n5242897.Result;
      return {engine_math_math::Mat4_translation(a).Result};
    }
    if (enum_equal<Mode>(mode_, Mode::jet)) {
      n5242898 = matrix_of(jet_actor_);
      a_2 = n5242898.Result;
      return {engine_math_math::Mat4_translation(a_2).Result};
    }
    if (enum_equal<Mode>(mode_, Mode::parachute)) {
      n5242899 = matrix_of(chute_actor_);
      a_3 = n5242899.Result;
      return {engine_math_math::Mat4_translation(a_3).Result};
    }
    if (gaps_riding().Result) {
      p = Vector{0.0, 0.0, 0.0};
      q = Quat{0.0, 0.0, 0.0, 1.0};
      both = logic_not(gaps_ride_car().Result);
      if (both) {
        n5242901 = gaps_ride_pose(p, q);
        both = n5242901.Result;
      }
      if (both) {
        return {p};
      }
    }
    n5242902 = matrix_of(soldier_);
    a_4 = n5242902.Result;
    return {engine_math_math::Mat4_translation(a_4).Result};
  }

  struct focus_positionResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  focus_positionResult focus_position() {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_and(enum_equal<Mode>(mode_, Mode::drone), drone_out_)) {
      n5242903 = matrix_of(drone_actor_);
      a = n5242903.Result;
      return {engine_math_math::Mat4_translation(a).Result};
    }
    n5242904 = player_position();
    return {n5242904.Result};
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
    for (auto it5242905 = flow_for(n5242905, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it5242905.next();) {
      skip = false;
      if (logic_or(logic_not(city_.lanes_[n5242905.index].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(city_.lanes_[n5242905.index].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(city_.lanes_[n5242905.index].points))), 0.5))) {
          a = city_.lanes_[n5242905.index].points[k];
          b = city_.lanes_[n5242905.index].points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))];
          ab = vector_sub(b, a);
          l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(ab).x, vector_break(ab).y);
          t = kern::formula([](Float l2, Float a) -> Float { return ((l2 > 1e-06) ? a : 0.0); }, l2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(c).x, vector_break(a).x, vector_break(ab).x, vector_break(c).y, vector_break(a).y, vector_break(ab).y, l2), 0.0, 1.0));
          q = vector_add(a, vector_scale(ab, t));
          if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(q).x, vector_break(c).y, vector_break(q).y), kern::formula([](Float reach, Float a) -> Float { return (reach + (a * 0.5)); }, reach, city_.lanes_[n5242905.index].width))) {
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
    engine_game_world_physics(n5242906, world_);
    physics = n5242906.result;
    w = physics;
    r = 0.0;
    while (loop(math_compare<"<=">(r, search))) {
      steps = kern::to<Int>(kern::formula([](Float r, Float kPi) -> Float { return ((r == 0.0) ? 1.0 : kern::math::trunc(kern::math::div(((2.0 * kPi) * r), 8.0))); }, r, kPi));
      for (auto it5242907 = flow_for(n5242907, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(steps)))); it5242907.next();) {
        a = kern::formula([](Float kPi, Float index, Float steps) -> Float { return kern::math::div(((2.0 * kPi) * index), steps); }, kPi, kern::to<Float>(n5242907.index), kern::to<Float>(steps));
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
    engine_game_world_physics(n5242918, world_);
    physics = n5242918.result;
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
    n5242919 = standing_feet(feet);
    engine_game_world_teleport(n5242920, world_, soldier_, vector_add(n5242919.Result, vector_make(0.0, 0.0, kern::formula([](Float kSpawnLift) -> Float { return (0.73 + kSpawnLift); }, kSpawnLift))), list_make<Vector, "1">(rot));
    teleport = n5242920.result;
    engine_game_world_set_actor_velocity(n5242921, world_, soldier_, Vector{0.0, 0.0, 0.0});
    set_actor_velocity = n5242921.result;
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
      n5242908 = matrix_of(jet_actor_);
      a = n5242908.Result;
      jp = engine_math_math::Mat4_translation(a).Result;
      here = struct_make<Game_Spot>(Vector{0.0, 0.0, 0.0}, 0.0);
      there = struct_make<Game_Spot>(Vector{0.0, 0.0, 0.0}, 0.0);
      n5242909 = clear_spot(here, jp, Vector{16.0, 20.0, 0.0}, 0.0, true);
      both = logic_not(n5242909.Result);
      if (both) {
        n5242911 = clear_spot(there, jp, Vector{16.0, 20.0, 0.0}, 260.0, true);
        both = n5242911.Result;
      }
      if (both) {
        n5242912 = matrix_of(soldier_);
        a_2 = n5242912.Result;
        me0 = engine_math_math::Mat4_translation(a_2).Result;
        rot = vector_make(0.0, 0.0, kern::formula([](Float y, Float y_2, Float x, Float x_2, Float kDeg) -> Float { return (kern::math::div(kern::math::atan2((y - y_2), (x - x_2)), kDeg) - 90.0); }, vector_break(me0).y, vector_break(there.p).y, vector_break(me0).x, vector_break(there.p).x, kDeg));
        engine_game_world_teleport(n5242913, world_, jet_actor_, vector_add(there.p, Vector{0.0, 0.0, 2.1}), list_make<Vector, "1">(rot));
        teleport = n5242913.result;
      }
    }
    n5242914 = matrix_of(soldier_);
    a_3 = n5242914.Result;
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
    for (auto it5242915 = flow_for(n5242915, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it5242915.next();) {
      r = items[n5242915.index];
      for (auto it5242916 = flow_for(n5242916, 0, 5); it5242916.next();) {
        n5242917 = city_stream_cpp::CityStream_street_point(city_, rng_, p, heading, me, r);
        if (logic_and(n5242917.Result, logic_not(city_stream_cpp::CityStream_building_at(city_, vector_make(vector_break(p).x, vector_break(p).y, 0.0)).Result))) {
          put_down(p, kern::formula([](Float heading, Float kDeg) -> Float { return kern::math::div(heading, kDeg); }, heading, kDeg));
          return;
        }
      }
    }
  }

  struct Game_place_world_takenResult { Bool Result = false; };
  Game_place_world_takenResult Game_place_world_taken(Vector at = Vector{0.0, 0.0, 0.0}) {
    for (auto it5242923 = flow_for(n5242923, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it5242923.next();) {
      if (logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n5242923.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetOut)))), 0.5), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(net_.remotes_[n5242923.index].state.jet_pos).x, vector_break(at).x, vector_break(net_.remotes_[n5242923.index].state.jet_pos).y, vector_break(at).y), 24.0))) {
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
      engine_game_world_set_mode(n5242930, world_, game_SimMode::play);
      set_mode = n5242930.result;
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
    n5242931 = matrix_of(soldier_);
    a = n5242931.Result;
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
      engine_game_world_physics(n5242932, world_);
      physics = n5242932.result;
      both = logic_not(struct_break<the_RayResult_>(common::cast(physics, vector_add(c, Vector{0.0, 0.0, 0.3}), Vector{0.0, 0.0, 1.0}, 25.0, body(soldier_).Result, Int64{0xffffffffffffffff}).Result).hit);
      if (both) {
        engine_game_world_physics(n5242934, world_);
        physics_3 = n5242934.result;
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
    engine_game_world_physics(n5242935, world_);
    physics_4 = n5242935.result;
    g = common::cast(physics_4, vector_add(at, Vector{0.0, 0.0, 3.0}), Vector{0.0, 0.0, -1.0}, 10.0, body(soldier_).Result, Int64{0xffffffffffffffff}).Result;
    at = vector_make(vector_break(at).x, vector_break(at).y, kern::formula([](Float hit, Float z, Float z_2) -> Float { return (((hit != 0.0) ? z : z_2) + 0.0225); }, kern::to<Float>(g.hit), vector_break(g.point).z, vector_break(feet).z));
    engine_game_world_set_alive(world_, drone_actor_, true);
    camera_yaw_2 = engine_game_world_camera_yaw(world_);
    rot = vector_make(0.0, 0.0, kern::formula([](Float camera_yaw_2) -> Float { return (camera_yaw_2 + 180.0); }, camera_yaw_2));
    engine_game_world_teleport(n5242936, world_, drone_actor_, at, list_make<Vector, "1">(rot));
    teleport = n5242936.result;
    engine_game_world_set_actor_velocity(n5242937, world_, drone_actor_, Vector{0.0, 0.0, 0.0});
    set_actor_velocity = n5242937.result;
    drone_cpp::Drone_reset(drone_);
    sticks_ = struct_make<DroneSticks>(0.0, 0.0, 0.0, 0.0, 0.0, 1.2, false, true, false);
    sticks_.clearance = 1.2;
    landing_ = false;
    drone_out_ = true;
    engine_game_world_set_mode(n5242938, world_, game_SimMode::simulate);
    set_mode_2 = n5242938.result;
    mode_ = Mode::drone;
    rig_init_ = false;
  }

  void hide_soldier() {
    Bool set_mode = false;
    engine_game_world_set_alive(world_, soldier_, false);
    engine_game_world_set_mode(n5242939, world_, game_SimMode::simulate);
    set_mode = n5242939.result;
  }

  void show_soldier(Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw_deg = 0.0) {
    Bool actor_alive = false;
    Bool set_mode = false;
    Vector rot = Vector{0.0, 0.0, 0.0};
    actor_alive = engine_game_world_actor_alive(world_, soldier_);
    if (logic_not(actor_alive)) {
      engine_game_world_set_alive(world_, soldier_, true);
    }
    engine_game_world_set_mode(n5242940, world_, game_SimMode::play);
    set_mode = n5242940.result;
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
    n5242941 = matrix_of(soldier_);
    a = n5242941.Result;
    at = engine_math_math::Mat4_translation(a).Result;
    if (logic_and(math_compare<">">(now, spawn_guard_until_), logic_or(math_compare<">=">(spawn_guard_ready_at_, 0.0), terrain_cpp::Terrain_ground_ready(city_.terrain_, vector_break(at).x, vector_break(at).y).Result))) {
      spawn_guard_until_ = -1.0;
      return;
    }
    n5242942 = matrix_of(soldier_);
    a_2 = n5242942.Result;
    c = engine_math_math::Mat4_translation(a_2).Result;
    feet = vector_make(vector_break(c).x, vector_break(c).y, kern::formula([](Float z) -> Float { return (z - 0.73); }, vector_break(c).z));
    ready = terrain_cpp::Terrain_ground_ready(city_.terrain_, vector_break(feet).x, vector_break(feet).y).Result;
    n5242943 = standing_feet(feet);
    stand = n5242943.Result;
    if (logic_not(ready)) {
      hold = vector_add(stand, vector_make(0.0, 0.0, kSpawnLift));
      if (math_compare<">">(kern::formula([](Float z, Float z_2) -> Float { return kern::math::abs((z - z_2)); }, vector_break(feet).z, vector_break(hold).z), 0.05)) {
        rot = vector_make(0.0, 0.0, vector_break(spawn_guard_).z);
        engine_game_world_teleport(n5242944, world_, soldier_, vector_add(hold, Vector{0.0, 0.0, 0.73}), list_make<Vector, "1">(rot));
        teleport = n5242944.result;
      }
      engine_game_world_set_actor_velocity(n5242945, world_, soldier_, Vector{0.0, 0.0, 0.0});
      set_actor_velocity = n5242945.result;
      spawn_guard_ready_at_ = -1.0;
      return;
    }
    if (math_compare<"<">(spawn_guard_ready_at_, 0.0)) {
      spawn_guard_ready_at_ = now;
    }
    if (math_compare<"<">(vector_break(feet).z, kern::formula([](Float z) -> Float { return (z - 0.25); }, vector_break(stand).z))) {
      engine_game_world_teleport(n5242946, world_, soldier_, vector_add(stand, Vector{0.0, 0.0, 0.83}), List<Vector>{});
      teleport_2 = n5242946.result;
      engine_game_world_set_actor_velocity(n5242947, world_, soldier_, Vector{0.0, 0.0, 0.0});
      set_actor_velocity_2 = n5242947.result;
    }
    if (math_compare<">">(kern::formula([](Float now, Float spawn_guard_ready_at_) -> Float { return (now - spawn_guard_ready_at_); }, now, spawn_guard_ready_at_), 2.0)) {
      spawn_guard_until_ = -1.0;
    }
  }

  void switch_mode(Mode m = Mode::foot, Int vehicle = 0) {
    Int physics = 0;
    Bool body_valid = false;
    Int physics_2 = 0;
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
        engine_game_world_physics(n5242948, world_);
        physics = n5242948.result;
        body_valid = engine_world_body_valid(physics, body(cars_[vehicle].actor).Result);
        if (body_valid) {
          engine_game_world_physics(n5242949, world_);
          physics_2 = n5242949.result;
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
    n5242953 = state_of(jet_actor_);
    s = n5242953.Result;
    jet_.pilot = false;
    engine_game_world_set_alive(world_, chute_actor_, true);
    engine_game_world_teleport(n5242954, world_, chute_actor_, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 2.8}).Result), List<Vector>{});
    teleport = n5242954.result;
    engine_game_world_set_actor_velocity(n5242955, world_, chute_actor_, vector_add(s.velocity, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kEjectUp)).Result));
    set_actor_velocity = n5242955.result;
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
      n5242950 = state_of(cars_[car_index_].actor);
      s = n5242950.Result;
      left = engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 1.0, 0.0}).Result;
      side = kern::formula([](Float y) -> Float { return (y + 0.9); }, vector_break(cars_[car_index_].car[0].s_.half).y);
      out = vector_add(s.position, vector_scale(left, side));
      engine_game_world_physics(n5242951, world_);
      physics = n5242951.result;
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
      n5242952 = state_of(jet_actor_);
      s_2 = n5242952.Result;
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
    n5242959 = common::Rng_next(rng_);
    start_2 = n5242959.Result;
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
    engine_game_world_physics(n5242960, world_);
    physics = n5242960.result;
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
    for (auto it5242962 = flow_for(n5242962, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(mates))))); it5242962.next();) {
      d = kern::formula([](Float d, Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::min(d, kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2)))); }, d, vector_break(p).x, vector_break(mates[n5242962.index]).x, vector_break(p).y, vector_break(mates[n5242962.index]).y);
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
    engine_game_world_physics(n5242964, world_);
    physics = n5242964.result;
    cast_sphere = engine_world_cast_sphere(physics, anchor, 0.17, d, engine_physics_physics::QueryFilter_new(Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}).Result, ignore);
    h = cast_sphere;
    return {logic_select<Vector>(h.hit, vector_add(anchor, vector_scale(d, kern::formula([](Float fraction) -> Float { return kern::math::max(0.0, (fraction - 0.02)); }, h.fraction))), want)};
  }

  void update_camera(Int e = 0, Float dt = 0.0) {
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
        engine_game_world_update_camera(n5242965, world_, c, dt, play_in_);
        update_camera_2 = n5242965.result;
        c = n5242965.camera;
        cam_valid_ = update_camera_2;
      } break;
      case Mode::drone: {
        n5242966 = matrix_of(drone_actor_);
        m = n5242966.Result;
        pos = engine_math_math::Mat4_translation(m).Result;
        n5242967 = state_of(drone_actor_);
        q = struct_break<BodyState>(n5242967.Result).rotation;
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
          n5242968 = Game_update_camera_collide(cam_anchor_, want, body(drone_actor_).Result);
          c.eye = n5242968.Result;
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
          n5242969 = matrix_of(cars_[car_index_].actor);
          m_2 = n5242969.Result;
          n5242970 = state_of(cars_[car_index_].actor);
          s = n5242970.Result;
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
            n5242971 = Game_update_camera_collide(tgt, want_2, body(cars_[car_index_].actor).Result);
            c.eye = n5242971.Result;
            c.target = tgt;
            c.up = Vector{0.0, 0.0, 1.0};
            c.fov = kern::formula([](Float the_result, Float kDeg) -> Float { return (kern::math::min(80.0, (70.0 + (kern::math::abs(the_result) * 0.25))) * kDeg); }, car::Car_forward_speed(cars_[car_index_].car[0]).Result, kDeg);
          }
        }
      } break;
      case Mode::jet: {
        n5242972 = matrix_of(jet_actor_);
        m_3 = n5242972.Result;
        n5242973 = state_of(jet_actor_);
        s_2 = n5242973.Result;
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
          n5242974 = Game_update_camera_collide(vector_add(pos_3, vector_scale(up_2, 2.0)), vector_add(pos_3, jet_cam_off_), body(jet_actor_).Result);
          c.eye = n5242974.Result;
          c.target = vector_add(pos_3, vector_scale(nose, 30.0));
          c.up = logic_select<Vector>(math_compare<">">(vector_length(vector_add(up_2, vector_scale(vector_sub(Vector{0.0, 0.0, 1.0}, up_2), 0.35))), 9.999999717180685e-10), vector_normalize(vector_add(up_2, vector_scale(vector_sub(Vector{0.0, 0.0, 1.0}, up_2), 0.35))), Vector{0.0, 0.0, 1.0});
          c.near = 1.0;
        }
        c.fov = kern::formula([](Float a, Float kDeg) -> Float { return (a * kDeg); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a) -> Float { return (66.0 + kern::math::div(a, 30.0)); }, vector_length(s_2.velocity)), 66.0, 90.0), kDeg);
        c.far = 6000.0;
      } break;
      case Mode::parachute: {
        n5242975 = matrix_of(chute_actor_);
        a_5 = n5242975.Result;
        pos_4 = engine_math_math::Mat4_translation(a_5).Result;
        n5242976 = Game_update_camera_collide(pos_4, vector_add(pos_4, Vector{0.0, -14.0, 7.0}), body(chute_actor_).Result);
        c.eye = n5242976.Result;
        c.target = vector_add(pos_4, Vector{0.0, 0.0, 2.0});
        c.up = Vector{0.0, 0.0, 1.0};
        c.fov = kern::formula([](Float kDeg) -> Float { return (70.0 * kDeg); }, kDeg);
      } break;
    }
    ctl_camera(c);
    gaps_camera(c);
    cam_ = c;
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
      engine_mixer_render(n5242983, mixer_, frames);
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(n5242983.out))));
      for (auto it5242984 = flow_for(n5242984, 0, last); it5242984.next();) {
        audio_out_[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(at), kern::to<Float>(n5242984.index)))] = list_get<Float>(n5242983.out, n5242984.index);
      }
      if (web_audio_) {
        fill = 0.0;
        first = 0;
        last_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(audio_out_))));
        for (auto it5242985 = flow_for(n5242985, first, last_2); it5242985.next();) {
          audio_out_[n5242985.index] = fill;
        }
      } else {
        ui_render_music(audio_out_, frames);
      }
    } else {
      fill_2 = 0.0;
      first_2 = 0;
      last_3 = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(audio_out_))));
      for (auto it5242986 = flow_for(n5242986, first_2, last_3); it5242986.next();) {
        audio_out_[n5242986.index] = fill_2;
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
    n6291466 = game_destruction_sync_cpp::digest_of(c, dm);
    b = text_format<"{\"epoch\":%u,\"craters\":%zu,\"damaged\":%zu,\"digest\":%u,\"sent\":%u,\"got\":%u,\"asks\":%u,\"pending\":%zu,\"mismatches\":%u}">(255, dsync_[0].epoch, list_length<Terrain_CraterRec>(c), list_length<CityStream_DamageRec>(dm), n6291466.Result, dsync_[0].sent_states, dsync_[0].got_states, dsync_[0].asks, list_length<DestroySync_Mine>(dsync_[0].mine), dsync_[0].mismatches);
    return {b};
  }

  void details_hud(Text& s) {
    Int smoking = 0;
    Text b = "";
    smoking = 0;
    for (auto it7340042 = flow_for(n7340042, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<detail_CriticalClock>(pd_crit_))))); it7340042.next();) {
      smoking += kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(combat_detail::CriticalClock_armed(pd_crit_[n7340042.index]).Result)));
    }
    b = "";
    n7340043 = mp_rules_cpp::ballot_name(ballot_.status);
    b = text_format<",\"details\":{\"droneCritical\":%s,\"pdSmoking\":%d,\"critBlasts\":%u,\"bulletPushes\":%u,\"ballot\":\"%s\"}">(199, logic_select<Text>(combat_detail::CriticalClock_armed(drone_crit_).Result, "true", "false"), smoking, crit_blasts_, bullet_pushes_, n7340043.Result);
    s = text_append(s, b, "");
  }

  struct details_command_startsResult { Bool Result = false; };
  KERN_PURE details_command_startsResult details_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct sfx_idResult { Text Result = ""; };
  /// "sc:<kind><variant>" for every sound of the bank (built once).
  sfx_idResult sfx_id(combat_audio_Kind k = combat_audio_Kind::shot, Int v = 0) {
    if (math_compare<"==">(kern::to<Float>(list_length<Text>(sfx_id_ids)), 0.0)) {
      for (auto it7340045 = flow_for(n7340045, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<combat_audio_Kind>(combat_audio_Kind::count))))); it7340045.next();) {
        for (auto it7340046 = flow_for(n7340046, 0, 3); it7340046.next();) {
          list_add(sfx_id_ids, text_append(text_append("sc:", combat_audio_cpp::kind_name(enum_from_index<combat_audio_Kind>(n7340045.index).value).Result, ""), text_format<"%u">(-1, n7340046.index), ""));
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
    for (auto it7340047 = flow_for(n7340047, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<combat_audio_Kind>(combat_audio_Kind::count))))); it7340047.next();) {
      k = enum_from_index<combat_audio_Kind>(n7340047.index).value;
      for (auto it7340048 = flow_for(n7340048, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_get<Int>(kVariants, n7340047.index))))); it7340048.next();) {
        n7340049 = combat_audio_cpp::render(k, n7340048.index, kSampleRate);
        pcm = n7340049.Result;
        n7340050 = sfx_id(k, n7340048.index);
        id = n7340050.Result;
        engine_sound_bank_request(bank_, id);
        engine_sound_bank_set_pcm(n7340051, bank_, id, pcm, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(pcm)))), 1, kSampleRate);
        set_pcm = n7340051.result;
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
    n7340052 = sfx_id(k, v);
    return {n7340052.Result};
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

  void ui_jetpack(Float dt = 0.0) {
    Bool on = false;
    Bool actor_alive = false;
    Bool actor_alive_2 = false;
    Bool both = false;
    game_Hud hud = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
    game_Hud hud_2 = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
    Bool both_2 = false;
    Bool jump = false;
    Bool pressed = false;
    Vector pos = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    the_RayResult_ below = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics = 0;
    Float height = 0.0;
    Vector v = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    Bool airborne = false;
    Float now = 0.0;
    Float sim_time = 0.0;
    Bool both_3 = false;
    Bool set_actor_velocity = false;
    Bool ready = false;
    audio_PlayParams p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
    Int64 play = Int64{0x0};
    Float acc = 0.0;
    Bool push_actor = false;
    Float yaw = 0.0;
    Float camera_yaw = 0.0;
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector r = Vector{0.0, 0.0, 0.0};
    Vector want = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity_2 = Vector{0.0, 0.0, 0.0};
    Vector dv = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Float cap = 0.0;
    Bool set_actor_velocity_2 = false;
    both = logic_and(settings::Settings_on(ui_.settings, Opt::world_jetpack).Result, enum_equal<Mode>(mode_, Mode::foot));
    if (both) {
      actor_alive_2 = engine_game_world_actor_alive(world_, soldier_);
      both = actor_alive_2;
    }
    both_2 = both;
    if (both_2) {
      hud_2 = engine_game_world_hud(world_);
      both_2 = math_compare<">">(struct_break<game_Hud>(hud_2).health, 0.0);
    }
    on = both_2;
    jump = play_in_.jump;
    pressed = logic_and(jump, logic_not(ui_.jump_was));
    ui_.jump_was = jump;
    if (logic_not(on)) {
      ui_.jetpack.thrusting = false;
      return;
    }
    n8388643 = matrix_of(soldier_);
    a = n8388643.Result;
    pos = engine_math_math::Mat4_translation(a).Result;
    engine_game_world_physics(n8388644, world_);
    physics = n8388644.result;
    below = common::cast(physics, pos, Vector{0.0, 0.0, -1.0}, 80.0, body(soldier_).Result, Int64{0xffffffffffffffff}).Result;
    height = kern::formula([](Float hit, Float distance) -> Float { return (((hit != 0.0) ? distance : 80.0) - 0.73); }, kern::to<Float>(below.hit), below.distance);
    the_actor_velocity = engine_game_world_actor_velocity(world_, soldier_);
    v = the_actor_velocity;
    airborne = math_compare<">">(height, 0.15);
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    both_3 = logic_and(logic_or(pressed, ui_.jet_pending), airborne);
    if (both_3) {
      n8388646 = equipment_cpp::Jetpack_ignite(ui_.jetpack, vector_break(v).z);
      both_3 = n8388646.Result;
    }
    if (both_3) {
      ui_.jet_pending = false;
      if (math_compare<"<">(vector_break(v).z, 0.0)) {
        v = vector_make(vector_break(v).x, vector_break(v).y, kern::formula([](Float z, Float the_result) -> Float { return (z * the_result); }, vector_break(v).z, equipment_cpp::Jetpack_ignition_damp().Result));
        engine_game_world_set_actor_velocity(n8388647, world_, soldier_, v);
        set_actor_velocity = n8388647.result;
      }
      ready = engine_mixer_ready(mixer_);
      if (logic_and(ready, math_compare<"!=">(kern::to<Float>(ui_.whoomp_clip), 0.0))) {
        p = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
        p.volume = 0.8;
        p.bus = audio_Bus::sfx;
        engine_mixer_play(n8388648, mixer_, ui_.whoomp_clip, p);
        play = n8388648.result;
      }
      engine_game_world_shake_camera(world_, 0.12);
      osfx("jetpack.ignition", pos, 0.0, 0.0, 0.0);
    }
    n8388649 = equipment_cpp::Jetpack_update(ui_.jetpack, dt, now, airborne, vector_break(v).z, height, jump);
    acc = n8388649.Result;
    ui_.jet_acc = acc;
    if (math_compare<">">(acc, 0.0)) {
      engine_game_world_push_actor(n8388650, world_, soldier_, vector_make(0.0, 0.0, kern::formula([](Float acc, Float dt) -> Float { return ((acc * dt) * 75.0); }, acc, dt)), true);
      push_actor = n8388650.result;
      camera_yaw = engine_game_world_camera_yaw(world_);
      yaw = kern::formula([](Float camera_yaw, Float kDeg) -> Float { return (camera_yaw * kDeg); }, camera_yaw, kDeg);
      f = vector_make(kern::formula([](Float yaw) -> Float { return kern::math::cos(yaw); }, yaw), kern::formula([](Float yaw) -> Float { return kern::math::sin(yaw); }, yaw), 0.0);
      r = vector_make(kern::formula([](Float yaw) -> Float { return kern::math::sin(yaw); }, yaw), kern::formula([](Float yaw) -> Float { return (-kern::math::cos(yaw)); }, yaw), 0.0);
      want = vector_scale(vector_add(vector_scale(f, play_in_.move_y), vector_scale(r, play_in_.move_x)), 8.1);
      the_actor_velocity_2 = engine_game_world_actor_velocity(world_, soldier_);
      v = the_actor_velocity_2;
      dv = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(want).x, vector_break(v).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(want).y, vector_break(v).y), 0.0);
      l = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(dv).x, vector_break(dv).y);
      cap = kern::formula([](Float the_result, Float dt) -> Float { return (the_result * dt); }, equipment_cpp::Jetpack_air_accel().Result, dt);
      if (math_compare<">">(l, cap)) {
        dv = vector_scale(dv, kern::formula([](Float cap, Float l) -> Float { return kern::math::div(cap, l); }, cap, l));
      }
      if (math_compare<">">(l, 0.001)) {
        engine_game_world_set_actor_velocity(n8388651, world_, soldier_, vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x + x_2); }, vector_break(v).x, vector_break(dv).x), kern::formula([](Float y, Float y_2) -> Float { return (y + y_2); }, vector_break(v).y, vector_break(dv).y), vector_break(v).z));
        set_actor_velocity_2 = n8388651.result;
      }
    }
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
      n8388652 = matrix_of(drone_actor_);
      a = n8388652.Result;
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
      engine_engine_input(n8388653, engine_);
      input = n8388653.result;
      pressed = engine_input_pressed(input, Key::n);
      either = logic_or(pressed, game::PadState_hit(pad, enum_index<PadButtonIndex>(PadButtonIndex::kPadRight)).Result);
      if (logic_not(either)) {
        a_3 = "radio";
        n8388654 = touch_play_cpp::TouchPlay_take_pressed(pad_touch_, a_3);
        either = n8388654.Result;
      }
      next = either;
      engine_engine_input(n8388655, engine_);
      input_2 = n8388655.result;
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
          n8388656 = matrix_of(cars_[car_index_].actor);
          a_4 = n8388656.Result;
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
          engine_mixer_play(n8388657, mixer_, ui_.thump_clip, p);
          play = n8388657.result;
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
    n8388658 = matrix_of(soldier_);
    a_2 = n8388658.Result;
    game_equipment_cpp::drive_loop(a, m, ui_.jet_clip, kern::formula([](Float burn) -> Float { return ((burn != 0.0) ? 0.75 : 0.0); }, kern::to<Float>(burn)), audio_Bus::sfx, true, vector_sub(engine_math_math::Mat4_translation(a_2).Result, Vector{0.0, 0.0, 0.6}));
    ui_.jet_voice = int64_to_int(a);
    for (auto it8388659 = flow_for(n8388659, 0, 7); it8388659.next();) {
      on = logic_and(logic_and(math_compare<">">(kern::formula([](Float index, Float a) -> Float { return ((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a)); }, kern::to<Float>(n8388659.index), kern::to<Float>(list_length<net_Remote>(net_.remotes_))), 0.5), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n8388659.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetpack)))), 0.5)), logic_not(ui_.paused));
      game_equipment_cpp::drive_loop(ui_.remote_jet_voice[n8388659.index], m, ui_.jet_clip, kern::formula([](Float on) -> Float { return ((on != 0.0) ? 0.7 : 0.0); }, kern::to<Float>(on)), audio_Bus::sfx, true, logic_select<Vector>(on, vector_sub(net_.remotes_[n8388659.index].shown, Vector{0.0, 0.0, 0.6}), Vector{0.0, 0.0, 0.0}));
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
      for (auto it8388660 = flow_for(n8388660, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Text>(part))))); it8388660.next();) {
        sp = text_find(rest, " ", 0);
        if (math_compare<"==">(kern::to<Float>(sp), -1.0)) {
          out = "usage: nostr event <created_at> <secret> <aux> <tags> <content>";
          return {true};
        }
        part[n8388660.index] = text_sub(rest, 0, sp);
        rest = text_sub(rest, kern::to<Int>(kern::formula([](Float sp) -> Float { return (sp + 1.0); }, kern::to<Float>(sp))), -1);
      }
      sk = List<Int>{};
      aux = List<Int>{};
      n8388661 = nostr_cpp::from_hex(sk, part[1]);
      either = logic_not(n8388661.Result);
      if (logic_not(either)) {
        n8388663 = nostr_cpp::from_hex(aux, part[2]);
        either = logic_not(n8388663.Result);
      }
      if (logic_or(logic_or(either, math_compare<"==">(kern::to<Float>(text_length(part[3])), 0.0)), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(part[3], 0)))))), 91.0))) {
        out = "bad key or tags";
        return {true};
      }
      n8388664 = nostr_cpp::event_json(sk, aux, out, text_parse_int(part[0], 0, 10).value, 30078, part[3], rest);
      if (logic_not(n8388664.Result)) {
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
      engine_game_world_push_actor(n8388665, world_, soldier_, Vector{0.0, 0.0, 270.0}, true);
      push_actor = n8388665.result;
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

  void update_race() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Int passed = 0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Text a_2 = "";
    if (logic_or(logic_or(logic_or(logic_not(combat_.race_on), logic_not(enum_equal<Mode>(mode_, Mode::drone))), logic_not(drone_out_)), logic_not(drone_.tele_.armed))) {
      return;
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    n9437242 = matrix_of(drone_actor_);
    a = n9437242.Result;
    n9437243 = combat_rules_cpp::Race_update(combat_.race, vector_sub(engine_math_math::Mat4_translation(a).Result, combat_.race_origin), now);
    passed = n9437243.Result;
    if (math_compare<"<">(kern::to<Float>(passed), 0.0)) {
      return;
    }
    if (combat_.race.finished) {
      a_2 = text_append(text_append(text_append("ZIEL \302\267 ", combat_rules_cpp::race_time(combat_.race.finish_time).Result, ""), " \302\267 Bestrunde ", ""), combat_rules_cpp::race_time(combat_.race.best_lap).Result, "");
      combat_banner(a_2);
    } else if (math_compare<"==">(kern::to<Float>(passed), 0.0)) {
      combat_.pop = text_append(text_append(text_append("RUNDE ", text_format<"%d">(-1, combat_.race.lap), ""), "/", ""), text_format<"%d">(-1, combat_kRaceLaps), "");
      combat_.pop_at = now;
    } else {
      combat_.pop = list_get<Text>(kGateNames, passed);
      combat_.pop_at = now;
    }
    engine_game_world_play_sound(world_, "builtin:checkpoint", cam_.eye, 0.6, 1.0, false, 0);
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
    for (auto it9437244 = flow_for(n9437244, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Debris>(combat_.debris))))); it9437244.next();) {
      combat_.debris[n9437244.index].v = vector_make(vector_break(combat_.debris[n9437244.index].v).x, vector_break(combat_.debris[n9437244.index].v).y, kern::formula([](Float z, Float kG, Float dt) -> Float { return (z - (kG * dt)); }, vector_break(combat_.debris[n9437244.index].v).z, kG, dt));
      combat_.debris[n9437244.index].p = vector_add(combat_.debris[n9437244.index].p, vector_scale(combat_.debris[n9437244.index].v, dt));
      a = engine_math_math::Quat_axis_angle(logic_select<Vector>(math_compare<">">(vector_length(combat_.debris[n9437244.index].spin), 9.999999717180685e-10), vector_normalize(combat_.debris[n9437244.index].spin), Vector{0.0, 0.0, 1.0}), kern::formula([](Float a, Float dt) -> Float { return (a * dt); }, vector_length(combat_.debris[n9437244.index].spin), dt)).Result;
      combat_.debris[n9437244.index].q = common::qnormalize(engine_math_math::Quat_mul(combat_.debris[n9437244.index].q, a).Result).Result;
      the_floor = kern::formula([](Float the_result, Float z) -> Float { return ((the_result + 0.06) + (z * 0.5)); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(combat_.debris[n9437244.index].p).x, vector_break(combat_.debris[n9437244.index].p).y).Result, vector_break(combat_.debris[n9437244.index].size).z);
      if (math_compare<"<">(vector_break(combat_.debris[n9437244.index].p).z, the_floor)) {
        combat_.debris[n9437244.index].p = vector_make(vector_break(combat_.debris[n9437244.index].p).x, vector_break(combat_.debris[n9437244.index].p).y, the_floor);
        if (math_compare<"<">(vector_break(combat_.debris[n9437244.index].v).z, 0.0)) {
          combat_.debris[n9437244.index].v = vector_make(vector_break(combat_.debris[n9437244.index].v).x, vector_break(combat_.debris[n9437244.index].v).y, kern::formula([](Float z) -> Float { return ((-z) * 0.3); }, vector_break(combat_.debris[n9437244.index].v).z));
        }
        combat_.debris[n9437244.index].v = vector_make(kern::formula([](Float x) -> Float { return (x * 0.76); }, vector_break(combat_.debris[n9437244.index].v).x), vector_break(combat_.debris[n9437244.index].v).y, vector_break(combat_.debris[n9437244.index].v).z);
        combat_.debris[n9437244.index].v = vector_make(vector_break(combat_.debris[n9437244.index].v).x, kern::formula([](Float y) -> Float { return (y * 0.76); }, vector_break(combat_.debris[n9437244.index].v).y), vector_break(combat_.debris[n9437244.index].v).z);
        combat_.debris[n9437244.index].spin = vector_scale(combat_.debris[n9437244.index].spin, 0.6);
      }
    }
    at = 0;
    kept = at;
    end = list_length<Debris>(combat_.debris);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(update_debris_match(combat_.debris[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n9437245, combat_.debris[at]);
          combat_.debris[kept] = n9437245.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(combat_.debris, kept, Debris{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 6.5});
    if (math_compare<">">(kern::formula([](Float a) -> Float { return ((63.0 < 0.0) != (a < 0.0) ? (a < 0.0) : (63.0 < a)); }, kern::to<Float>(list_length<Debris>(combat_.debris))), 0.5)) {
      first = 0;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 63.0); }, kern::to<Float>(list_length<Debris>(combat_.debris))))))));
      for (auto it9437246 = flow_for(n9437246, first, last); it9437246.next();) {
        list_remove_at(combat_.debris, first);
      }
    }
  }

  void draw_events() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Float fade = 0.0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip = false;
    combat_EnemySpec s = combat_EnemySpec{"", 0.0, 0.0, 0.0, 0.0, 0, 0.0, 0.0, 0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Quat a_11 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_12 = Quat{0.0, 0.0, 0.0, 1.0};
    Float t = 0.0;
    Float s_2 = 0.0;
    Mat4 a_13 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_14 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_15 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float k = 0.0;
    Bool skip_2 = false;
    Bool actor_alive = false;
    Bool actor_alive_2 = false;
    Bool either = false;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector pos = Vector{0.0, 0.0, 0.0};
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Vector side = Vector{0.0, 0.0, 0.0};
    Vector h = Vector{0.0, 0.0, 0.0};
    Float item = 0.0;
    Float item_2 = 0.0;
    Fixed<Float, 2> items = Fixed<Float, 2>{};
    Float sd = 0.0;
    Mat4 a_16 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_17 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_18 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector pool = Vector{0.0, 0.0, 0.0};
    Vector g = Vector{0.0, 0.0, 0.0};
    Mat4 a_19 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_20 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_21 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip_3 = false;
    Float age = 0.0;
    Vector at = Vector{0.0, 0.0, 0.0};
    Mat4 a_22 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float t_2 = 0.0;
    Float s_3 = 0.0;
    Mat4 a_23 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_24 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_25 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float rise = 0.0;
    Float s_4 = 0.0;
    Mat4 a_26 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_27 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_28 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    combat_Gate g_2 = combat_Gate{"", Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false};
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector r = Vector{0.0, 0.0, 0.0};
    Vector col = Vector{0.0, 0.0, 0.0};
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    for (auto it9437247 = flow_for(n9437247, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Debris>(combat_.debris))))); it9437247.next();) {
      fade = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a, Float a_2, Float now) -> Float { return kern::math::div(((a + a_2) - now), 1.4); }, combat_.debris[n9437247.index].born, combat_.debris[n9437247.index].life, now), 0.0, 1.0));
      a = engine_math_math::Mat4_trs(combat_.debris[n9437247.index].p, combat_.debris[n9437247.index].q, vector_scale(combat_.debris[n9437247.index].size, kern::formula([](Float fade) -> Float { return (0.3 + (0.7 * fade)); }, fade))).Result;
      combat_art_cpp::CombatArt_put(combat_art_, a, CombatArt_Shape::box, combat_.debris[n9437247.index].color);
    }
    for (auto it9437248 = flow_for(n9437248, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<EnemyRocket>(combat_.rockets))))); it9437248.next();) {
      a_3 = engine_math_math::Mat4_translate(combat_.rockets[n9437248.index].p).Result;
      a_4 = engine_math_math::Mat4_scale(Vector{0.44, 0.44, 0.44}).Result;
      a_2 = engine_math_math::Mat4_mul(a_3, a_4).Result;
      combat_art_cpp::CombatArt_put(combat_art_, a_2, CombatArt_Shape::glow_sphere, Vector{3.0, 1.6, 0.5});
    }
    for (auto it9437249 = flow_for(n9437249, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<EnemyAi>(combat_.enemies))))); it9437249.next();) {
      skip = false;
      if (math_compare<"<">(kern::to<Float>(combat_.enemies[n9437249.index].person), 0.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        s = enemy_spec(combat_.enemies[n9437249.index].type).Result;
        a_6 = engine_math_math::Mat4_translate(vector_add(people_.people_[combat_.enemies[n9437249.index].person].pos, Vector{0.0, 0.0, 1.4})).Result;
        a_7 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(people_.people_[combat_.enemies[n9437249.index].person].yaw).Result, Vector{0.27, 0.45, 0.06}).Result;
        a_5 = engine_math_math::Mat4_mul(a_6, a_7).Result;
        combat_art_cpp::CombatArt_put(combat_art_, a_5, CombatArt_Shape::glow_box, vector_scale(s.stripe, 1.6));
        if (enum_equal<combat_EnemyType>(combat_.enemies[n9437249.index].type, combat_EnemyType::heavy)) {
          a_9 = engine_math_math::Mat4_translate(vector_add(people_.people_[combat_.enemies[n9437249.index].person].pos, vector_make(kern::formula([](Float a) -> Float { return (kern::math::cos(a) * 0.1); }, people_.people_[combat_.enemies[n9437249.index].person].yaw), kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.1); }, people_.people_[combat_.enemies[n9437249.index].person].yaw), 1.62))).Result;
          a_11 = common::yaw_quat(people_.people_[combat_.enemies[n9437249.index].person].yaw).Result;
          a_12 = engine_math_math::Quat_axis_angle(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
          a_10 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_mul(a_11, a_12).Result, Vector{0.26, 0.26, 1.1}).Result;
          a_8 = engine_math_math::Mat4_mul(a_9, a_10).Result;
          combat_art_cpp::CombatArt_put(combat_art_, a_8, CombatArt_Shape::cylinder, Vector{0.1, 0.1, 0.13});
        }
      }
    }
    for (auto it9437250 = flow_for(n9437250, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Tank>(combat_.tanks))))); it9437250.next();) {
      if (logic_and(combat_.tanks[n9437250.index].wreck, math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, combat_.tanks[n9437250.index].wreck_at), 40.0))) {
        for (auto it9437251 = flow_for(n9437251, 0, 2); it9437251.next();) {
          t = kern::formula([](Float now, Float index) -> Float { return ((now * (3.1 + index)) + (index * 2.1)); }, now, kern::to<Float>(n9437251.index));
          s_2 = kern::formula([](Float t) -> Float { return (1.4 * (0.55 + (0.25 * kern::math::sin(t)))); }, t);
          a_14 = engine_math_math::Mat4_translate(vector_add(combat_.tanks[n9437250.index].p, vector_make(kern::formula([](Float t) -> Float { return (0.3 * kern::math::sin(t)); }, t), kern::formula([](Float t) -> Float { return (0.3 * kern::math::cos(t)); }, t), kern::formula([](Float s_2, Float index) -> Float { return ((2.0 + (s_2 * 0.6)) + (index * 0.5)); }, s_2, kern::to<Float>(n9437251.index))))).Result;
          a_15 = engine_math_math::Mat4_scale(vector_make(s_2, s_2, kern::formula([](Float s_2) -> Float { return (s_2 * 1.4); }, s_2))).Result;
          a_13 = engine_math_math::Mat4_mul(a_14, a_15).Result;
          combat_art_cpp::CombatArt_put(combat_art_, a_13, CombatArt_Shape::glow_sphere, Vector{2.8, 1.1, 0.25});
        }
      }
    }
    if (math_compare<">">(night_, 0.25)) {
      k = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float night_) -> Float { return kern::math::div((night_ - 0.25), 0.5); }, night_), 0.0, 1.0));
      for (auto it9437252 = flow_for(n9437252, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it9437252.next();) {
        skip_2 = false;
        either = logic_or(logic_not(cars_[n9437252.index].placed), cars_[n9437252.index].wreck);
        if (logic_not(either)) {
          actor_alive_2 = engine_game_world_actor_alive(world_, cars_[n9437252.index].actor);
          either = logic_not(actor_alive_2);
        }
        if (either) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          if (logic_and(logic_not(cars_[n9437252.index].driver), logic_not(logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n9437252.index))))), kern::to<Float>(car_index_)))))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            n9437253 = matrix_of(cars_[n9437252.index].actor);
            m = n9437253.Result;
            pos = engine_math_math::Mat4_translation(m).Result;
            if (math_compare<">">(vector_length(vector_sub(pos, cam_.eye)), 160.0)) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              fwd = logic_select<Vector>(math_compare<">">(vector_length(vector_make(m.m[0], m.m[1], 0.0)), 9.999999717180685e-10), vector_normalize(vector_make(m.m[0], m.m[1], 0.0)), Vector{1.0, 0.0, 0.0});
              side = vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(fwd).y), vector_break(fwd).x, 0.0);
              h = cars_[n9437252.index].car[0].s_.half;
              item = -1.0;
              item_2 = 1.0;
              items = list_make<Float, "2">(item, item_2);
              for (auto it9437254 = flow_for(n9437254, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it9437254.next();) {
                sd = items[n9437254.index];
                a_17 = engine_math_math::Mat4_translate(vector_add(vector_add(vector_add(pos, vector_scale(fwd, kern::formula([](Float x) -> Float { return (x + 0.02); }, vector_break(h).x))), vector_scale(side, kern::formula([](Float sd, Float y) -> Float { return ((sd * y) * 0.7); }, sd, vector_break(h).y))), Vector{0.0, 0.0, 0.05})).Result;
                a_18 = engine_math_math::Mat4_scale(Vector{0.16, 0.16, 0.12}).Result;
                a_16 = engine_math_math::Mat4_mul(a_17, a_18).Result;
                combat_art_cpp::CombatArt_put(combat_art_, a_16, CombatArt_Shape::glow_sphere, vector_scale(Vector{3.2, 3.0, 2.6}, k));
              }
              pool = vector_add(pos, vector_scale(fwd, kern::formula([](Float x) -> Float { return (x + 5.5); }, vector_break(h).x)));
              g = vector_make(vector_break(pool).x, vector_break(pool).y, kern::formula([](Float the_result) -> Float { return (the_result + 0.06); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(pool).x, vector_break(pool).y).Result));
              a_20 = engine_math_math::Mat4_translate(g).Result;
              a_21 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(fwd).y, vector_break(fwd).x)).Result, Vector{9.0, 4.2, 0.05}).Result;
              a_19 = engine_math_math::Mat4_mul(a_20, a_21).Result;
              combat_art_cpp::CombatArt_put_2(combat_art_, a_19, CombatArt_Shape::fade_sphere, vector_scale(Vector{1.0, 0.93, 0.78}, 1.6), kern::formula([](Float k) -> Float { return (0.2 * k); }, k));
            }
          }
        }
      }
    }
    for (auto it9437255 = flow_for(n9437255, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it9437255.next();) {
      skip_3 = false;
      if (logic_or(logic_not(cars_[n9437255.index].placed), logic_not(cars_[n9437255.index].wreck))) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        age = kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, cars_[n9437255.index].wreck_at);
        n9437256 = matrix_of(cars_[n9437255.index].actor);
        a_22 = n9437256.Result;
        at = engine_math_math::Mat4_translation(a_22).Result;
        for (auto it9437257 = flow_for(n9437257, 0, 2); it9437257.next();) {
          t_2 = kern::formula([](Float now, Float index) -> Float { return ((now * (2.7 + index)) + (index * 2.3)); }, now, kern::to<Float>(n9437257.index));
          if (math_compare<"<">(age, 9.0)) {
            s_3 = kern::formula([](Float t_2, Float a) -> Float { return ((1.1 * (0.55 + (0.25 * kern::math::sin(t_2)))) * a); }, t_2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float age) -> Float { return (1.4 - kern::math::div(age, 9.0)); }, age), 0.3, 1.0));
            a_24 = engine_math_math::Mat4_translate(vector_add(at, vector_make(kern::formula([](Float t_2) -> Float { return (0.35 * kern::math::sin(t_2)); }, t_2), kern::formula([](Float t_2) -> Float { return (0.35 * kern::math::cos(t_2)); }, t_2), kern::formula([](Float s_3, Float index) -> Float { return ((1.1 + (s_3 * 0.5)) + (index * 0.4)); }, s_3, kern::to<Float>(n9437257.index))))).Result;
            a_25 = engine_math_math::Mat4_scale(vector_make(s_3, s_3, kern::formula([](Float s_3) -> Float { return (s_3 * 1.5); }, s_3))).Result;
            a_23 = engine_math_math::Mat4_mul(a_24, a_25).Result;
            combat_art_cpp::CombatArt_put(combat_art_, a_23, CombatArt_Shape::glow_sphere, Vector{2.8, 1.05, 0.22});
          }
          rise = kern::formula([](Float now, Float index) -> Float { return kern::math::mod(((now * 0.9) + (index * 1.3)), 4.0); }, now, kern::to<Float>(n9437257.index));
          s_4 = kern::formula([](Float rise) -> Float { return (0.7 + (rise * 0.45)); }, rise);
          a_27 = engine_math_math::Mat4_translate(vector_add(at, vector_make(kern::formula([](Float t_2) -> Float { return (0.4 * kern::math::sin((t_2 * 0.3))); }, t_2), kern::formula([](Float t_2) -> Float { return (0.4 * kern::math::cos((t_2 * 0.3))); }, t_2), kern::formula([](Float rise) -> Float { return (1.6 + (rise * 1.6)); }, rise)))).Result;
          a_28 = engine_math_math::Mat4_scale(vector_make(s_4, s_4, s_4)).Result;
          a_26 = engine_math_math::Mat4_mul(a_27, a_28).Result;
          combat_art_cpp::CombatArt_put_2(combat_art_, a_26, CombatArt_Shape::fade_sphere, Vector{0.05, 0.05, 0.055}, kern::formula([](Float rise) -> Float { return (0.55 * (1.0 - kern::math::div(rise, 4.0))); }, rise));
        }
      }
    }
    if (combat_.race_on) {
      for (auto it9437258 = flow_for(n9437258, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(combat_kGates)))); it9437258.next();) {
        g_2 = gate(n9437258.index).Result;
        n = Vector{0.0, 0.0, 0.0};
        r = Vector{0.0, 0.0, 0.0};
        combat_rules_cpp::gate_axes(n, r, n9437258.index);
        col = logic_select<Vector>(g_2.start, Vector{1.0, 0.78, 0.34}, Vector{0.95, 0.96, 0.97});
        if (logic_and(logic_and(logic_and(combat_.race.started, logic_not(combat_.race.finished)), math_compare<"<">(kern::to<Float>(n9437258.index), kern::to<Float>(combat_.race.next))), math_compare<"!=">(kern::to<Float>(combat_.race.next), 0.0))) {
          col = Vector{0.35, 0.72, 1.0};
        }
        if (logic_and(logic_not(combat_.race.finished), math_compare<"==">(kern::to<Float>(n9437258.index), kern::to<Float>(combat_.race.next)))) {
          col = vector_scale(Vector{0.43, 0.96, 0.66}, 2.2);
        }
        combat_art_cpp::CombatArt_gate(combat_art_, vector_add(combat_.race_origin, g_2.center), kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(r).y, vector_break(r).x), g_2.width, g_2.height, col);
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

  void blast_police_drones(Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float damage = 0.0, Bool by_player = false) {
    Float now = 0.0;
    Float sim_time = 0.0;
    Bool skip = false;
    Float dd = 0.0;
    Float amount = 0.0;
    Float hits = 0.0;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    for (auto it10485791 = flow_for(n10485791, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it10485791.next();) {
      skip = false;
      if (logic_not(pdrones_[n10485791.index].active)) {
        skip = true;
      }
      if (logic_not(skip)) {
        dd = vector_length(vector_sub(pdrones_[n10485791.index].pos, at));
        if (math_compare<">=">(dd, radius)) {
          skip = true;
        }
        if (logic_not(skip)) {
          n10485792 = line_of_sight(at, pdrones_[n10485791.index].pos);
          amount = kern::formula([](Float damage, Float the_result) -> Float { return (damage * the_result); }, damage, weapons::blast_factor(dd, radius, logic_not(n10485792.Result)).Result);
          hits = kern::formula([](Float amount) -> Float { return kern::math::min(3.0, kern::math::max(0.0, kern::math::round(kern::math::div(amount, 34.0)))); }, amount);
          if (math_compare<"<=">(hits, 0.0)) {
            skip = true;
          }
          if (logic_not(skip)) {
            pdrones_[n10485791.index].hp = kern::formula([](Float a, Float hits) -> Float { return (a - (34.0 * hits)); }, pdrones_[n10485791.index].hp, hits);
            if (math_compare<"<=">(pdrones_[n10485791.index].hp, 0.0)) {
              pdrones_[n10485791.index].active = false;
              pdrones_[n10485791.index].destroyed_until = kern::formula([](Float now) -> Float { return (now + 9.0); }, now);
              if (by_player) {
                wanted_cpp::report(wanted_Crime::police_drone, true);
              }
            }
          }
        }
      }
    }
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
      for (auto it10485794 = flow_for(n10485794, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_ShotEvent>(shots))))); it10485794.next();) {
        if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(shots[n10485794.index].shooter), kern::to<Float>(soldier_)), enum_equal<Mode>(mode_, Mode::foot)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper))) {
          list_add(arrived, shots[n10485794.index]);
        }
      }
      return;
    }
    for (auto it10485795 = flow_for(n10485795, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_ShotEvent>(shots))))); it10485795.next();) {
      if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(shots[n10485795.index].shooter), kern::to<Float>(soldier_)), enum_equal<Mode>(mode_, Mode::foot)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper))) {
        list_add(gaps_[0].sniper_rounds, struct_make<GapsState_SniperRound>(shots[n10485795.index], kern::formula([](Float now, Float a) -> Float { return (now + kern::math::div(a, 850.0)); }, now, vector_length(vector_sub(shots[n10485795.index].to, shots[n10485795.index].from)))));
      }
    }
    for (auto it10485796 = flow_for(n10485796, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_SniperRound>(gaps_[0].sniper_rounds))))); it10485796.next();) {
      if (math_compare<">=">(now, gaps_[0].sniper_rounds[n10485796.index].due)) {
        list_add(arrived, gaps_[0].sniper_rounds[n10485796.index].s);
      }
    }
    at = 0;
    kept = at;
    end = list_length<GapsState_SniperRound>(gaps_[0].sniper_rounds);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(gaps_sniper_rounds_match(gaps_[0].sniper_rounds[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n10485797, gaps_[0].sniper_rounds[at]);
          gaps_[0].sniper_rounds[kept] = n10485797.value;
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

  struct gaps_spawn_viewersResult { Bool Result = false; };
  gaps_spawn_viewersResult gaps_spawn_viewers(List<spawn_Viewer>& vs) {
    Float sim_time = 0.0;
    Float sim_time_2 = 0.0;
    Bool both = false;
    spawn_Viewer me = spawn_Viewer{Vector{0.0, 0.0, 0.0}, Vector{1.0, 0.0, 0.0}, 1.5707963267948966, 1500.0, false};
    Float aspect = 0.0;
    Float foot_fov = 0.0;
    spawn_Viewer v = spawn_Viewer{Vector{0.0, 0.0, 0.0}, Vector{1.0, 0.0, 0.0}, 1.5707963267948966, 1500.0, false};
    net_PlayerState s = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255};
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(math_compare<"!=">(kern::to<Float>(world_), 0.0)))) {
      return {false};
    }
    both = math_compare<">=">(gaps_[0].world_shown, 0.0);
    if (both) {
      sim_time_2 = engine_game_world_sim_time(world_);
      both = math_compare<"<">(kern::formula([](Float sim_time_2, Float a) -> Float { return (sim_time_2 - a); }, sim_time_2, gaps_[0].world_shown), kSpawnGraceS);
    }
    if (both) {
      return {false};
    }
    me = spawn_Viewer{Vector{0.0, 0.0, 0.0}, Vector{1.0, 0.0, 0.0}, 1.5707963267948966, 1500.0, false};
    me.eye = cam_.eye;
    me.forward = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{1.0, 0.0, 0.0});
    aspect = kern::formula([](Float frame_w_, Float frame_h_) -> Float { return kern::math::div(frame_w_, kern::math::max(1.0, frame_h_)); }, frame_w_, frame_h_);
    foot_fov = kern::formula([](Float kDeg, Float aspect) -> Float { return (2.0 * kern::math::atan(kern::math::div(kern::math::tan((45.0 * kDeg)), kern::math::max(0.2, aspect)))); }, kDeg, aspect);
    n11534367 = spawn_guard_cpp::cone_half_angle(kern::formula([](Float a, Float fov, Float foot_fov) -> Float { return ((a != 0.0) ? kern::math::max(fov, foot_fov) : fov); }, kern::to<Float>(enum_equal<Mode>(mode_, Mode::foot)), cam_.fov, foot_fov), aspect);
    me.half_angle = n11534367.Result;
    me.far = kern::formula([](Float far, Float the_result) -> Float { return kern::math::max(200.0, ((far > 0.0) ? far : the_result)); }, cam_.far, spawn_guard_cpp::default_far().Result);
    me.local = true;
    list_add(vs, me);
    for (auto it11534368 = flow_for(n11534368, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it11534368.next();) {
      v = spawn_Viewer{Vector{0.0, 0.0, 0.0}, Vector{1.0, 0.0, 0.0}, 1.5707963267948966, 1500.0, false};
      v.half_angle = me.half_angle;
      v.far = me.far;
      s = net_.remotes_[n11534368.index].state;
      switch (flow_switch_enum<net_PlayerMode>(s.mode)) {
        case net_PlayerMode::foot: {
          v.eye = vector_add(net_.remotes_[n11534368.index].person_shown, Vector{0.0, 0.0, 1.55});
          v.forward = vector_make(kern::formula([](Float avatar_yaw) -> Float { return kern::math::cos(avatar_yaw); }, s.avatar_yaw), kern::formula([](Float avatar_yaw) -> Float { return kern::math::sin(avatar_yaw); }, s.avatar_yaw), 0.0);
        } break;
        case net_PlayerMode::drone: {
          v.eye = net_.remotes_[n11534368.index].shown;
          v.forward = engine_math_math::Quat_rotate(net_.remotes_[n11534368.index].shown_rotation, Vector{-1.0, 0.0, 0.0}).Result;
        } break;
        case net_PlayerMode::car: {
          v.eye = vector_add(net_.remotes_[n11534368.index].shown, Vector{0.0, 0.0, 1.2});
          v.forward = engine_math_math::Quat_rotate(net_.remotes_[n11534368.index].shown_rotation, Vector{1.0, 0.0, 0.0}).Result;
        } break;
        case net_PlayerMode::jet: {
          v.eye = net_.remotes_[n11534368.index].shown;
          v.forward = engine_math_math::Quat_rotate(net_.remotes_[n11534368.index].shown_rotation, Vector{0.0, 1.0, 0.0}).Result;
        } break;
        case net_PlayerMode::parachute: {
          v.eye = vector_add(net_.remotes_[n11534368.index].person_shown, Vector{0.0, 0.0, 1.55});
          v.forward = vector_make(kern::formula([](Float avatar_yaw) -> Float { return kern::math::cos(avatar_yaw); }, s.avatar_yaw), kern::formula([](Float avatar_yaw) -> Float { return kern::math::sin(avatar_yaw); }, s.avatar_yaw), 0.0);
        } break;
      }
      v.forward = logic_select<Vector>(math_compare<">">(vector_length(v.forward), 9.999999717180685e-10), vector_normalize(v.forward), Vector{1.0, 0.0, 0.0});
      list_add(vs, v);
    }
    return {true};
  }

  struct gaps_can_spawnResult { Bool Result = false; };
  gaps_can_spawnResult gaps_can_spawn(Vector p = Vector{0.0, 0.0, 0.0}, Float height = 0.0, Float far_entry = 0.0) {
    List<spawn_Viewer> vs = List<spawn_Viewer>{};
    Bool ok = false;
    Int put = 0;
    vs = List<spawn_Viewer>{};
    n11534369 = gaps_spawn_viewers(vs);
    if (logic_not(n11534369.Result)) {
      return {true};
    }
    n11534370 = spawn_guard_cpp::can_spawn(vs, p, far_entry, height);
    ok = n11534370.Result;
    put = kern::to<Int>(kern::formula([](Float ok, Float a, Float a_2) -> Float { return kern::math::wrap32(((ok != 0.0) ? a : a_2) + 1.0); }, kern::to<Float>(ok), kern::to<Float>(gaps_[0].spawn_allowed), kern::to<Float>(gaps_[0].spawn_denied)));
    if (ok) {
      gaps_[0].spawn_allowed = put;
    } else {
      gaps_[0].spawn_denied = put;
    }
    return {ok};
  }

  struct gaps_vehicle_spawn_okResult { Bool Result = false; };
  gaps_vehicle_spawn_okResult gaps_vehicle_spawn_ok(Int car = 0, Vector pos = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0) {
    Vector h = Vector{0.0, 0.0, 0.0};
    spawn_Footprint me = spawn_Footprint{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}};
    Bool skip = false;
    Bool actor_alive = false;
    Bool actor_alive_2 = false;
    Bool either = false;
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector oh = Vector{0.0, 0.0, 0.0};
    spawn_Footprint a = spawn_Footprint{Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}};
    h = cars_[car].car[0].s_.half;
    me = struct_make<spawn_Footprint>(vector_make(vector_break(pos).x, vector_break(pos).y, 0.0), yaw, vector_make(vector_break(h).x, vector_break(h).y, 0.0));
    for (auto it11534371 = flow_for(n11534371, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it11534371.next();) {
      skip = false;
      either = logic_or(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n11534371.index))))), kern::to<Float>(car)), logic_not(cars_[n11534371.index].placed));
      if (logic_not(either)) {
        actor_alive_2 = engine_game_world_actor_alive(world_, cars_[n11534371.index].actor);
        either = logic_not(actor_alive_2);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        n11534372 = state_of(cars_[n11534371.index].actor);
        s = n11534372.Result;
        f = engine_math_math::Quat_rotate(s.rotation, Vector{1.0, 0.0, 0.0}).Result;
        oh = cars_[n11534371.index].car[0].s_.half;
        a = struct_make<spawn_Footprint>(vector_make(vector_break(s.position).x, vector_break(s.position).y, 0.0), kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(f).y, vector_break(f).x), vector_make(vector_break(oh).x, vector_break(oh).y, 0.0));
        n11534373 = spawn_guard_cpp::footprints_overlap(me, a, 0.35);
        if (n11534373.Result) {
          gaps_[0].overlap_denied = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(gaps_[0].overlap_denied)));
          return {false};
        }
      }
    }
    n11534374 = gaps_can_spawn(pos, 2.2, kVehicleEntryM);
    return {n11534374.Result};
  }

  struct gaps_drone_dueResult { Bool Result = false; };
  gaps_drone_dueResult gaps_drone_due(Int slot = 0, Int stars = 0) {
    Float now = 0.0;
    Float sim_time = 0.0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), math_compare<">=">(kern::to<Float>(slot), 5.0))) {
      return {true};
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    if (math_compare<"<">(gaps_[0].pd_due[slot], 0.0)) {
      n11534375 = wanted_cpp::arrival_delay_ms(stars);
      gaps_[0].pd_due[slot] = kern::formula([](Float now, Float the_result) -> Float { return (now + kern::math::div(the_result, 1000.0)); }, now, n11534375.Result);
    }
    return {math_compare<">=">(now, gaps_[0].pd_due[slot])};
  }

  void gaps_drone_revealed(Int slot = 0, Int stars = 0) {
    Float now = 0.0;
    Float sim_time = 0.0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), math_compare<">=">(kern::to<Float>(slot), 5.0))) {
      return;
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    n11534376 = gaps_rules_cpp::post_arrival_grace_ms(stars);
    pdrones_[slot].next_shot = kern::formula([](Float now, Float the_result, Float slot) -> Float { return kern::math::max((now + kern::math::div(the_result, 1000.0)), ((now + 0.26) + (slot * 0.09))); }, now, n11534376.Result, kern::to<Float>(slot));
    gaps_[0].pd_due[slot] = -1.0;
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
      n11534377 = matrix_of(jet_actor_);
      a = n11534377.Result;
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
      n11534378 = matrix_of(jet_actor_);
      a_3 = n11534378.Result;
      n11534379 = gaps_rules_cpp::SmokeField_start(gaps_[0].smoke, a_2, engine_math_math::Mat4_translation(a_3).Result, 2.2, kJetBurnS, now);
      toast_ = "JET BRENNT";
      toast_age_ = 0.0;
    }
    if (math_compare<"<=">(jet_hp_, 0.0)) {
      gaps_[0].jet_burning = true;
      a_4 = "jet";
      n11534380 = gaps_rules_cpp::SmokeField_accelerate(gaps_[0].smoke, a_4, now, now);
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
      n11534405 = gaps_ride_camera(c);
      both = n11534405.Result;
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
      n11534406 = matrix_of(cars_[car_index_].actor);
      a_2 = n11534406.Result;
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
    n11534407 = matrix_of(drone_actor_);
    raw = n11534407.Result;
    if (logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(enum_equal<Mode>(mode_, Mode::drone))), math_compare<"==">(kern::to<Float>(drone_cam_), 0.0))) {
      if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
        gaps_rules::VisualPose_invalidate(gaps_[0].drone_visual);
      }
      return {raw};
    }
    p = Vector{0.0, 0.0, 0.0};
    q = Quat{0.0, 0.0, 0.0, 1.0};
    n11534408 = state_of(drone_actor_);
    gaps_rules_cpp::VisualPose_update(gaps_[0].drone_visual, p, q, engine_math_math::Mat4_translation(raw).Result, struct_break<BodyState>(n11534408.Result).rotation, cam_anchor_, kern::to<Int>(kern::formula([](Float drone_cam_) -> Float { return ((drone_cam_ == 2.0) ? 2.0 : 1.0); }, kern::to<Float>(drone_cam_))), gaps_[0].frame_dt);
    return {engine_math_math::Mat4_trs(p, q, Vector{1.0, 1.0, 1.0}).Result};
  }

  struct gaps_hud_matchResult { Bool Result = false; };
  KERN_PURE gaps_hud_matchResult gaps_hud_match(const GapsState_RocketPuff& r) {
    return {math_compare<">=">(r.born, 0.0)};
  }

  void gaps_hud(Text& s) {
    Float now = 0.0;
    Float sim_time = 0.0;
    Text b = "";
    Int inbound = 0;
    Float next = 0.0;
    Bool first = false;
    Bool skip = false;
    Float x = 0.0;
    Float y = 0.0;
    Float depth = 0.0;
    Int placed = 0;
    Int far = 0;
    Int count = 0;
    Int at = 0;
    Int end = 0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(math_compare<"!=">(kern::to<Float>(world_), 0.0)))) {
      return;
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    b = "";
    inbound = 0;
    next = 1000000000.0;
    n11534409 = wanted_cpp::stars();
    if (math_compare<">">(kern::to<Float>(n11534409.Result), 0.0)) {
      for (auto it11534410 = flow_for(n11534410, 0, 4); it11534410.next();) {
        if (logic_and(math_compare<">">(gaps_[0].pd_due[n11534410.index], now), logic_not(pdrones_[n11534410.index].active))) {
          inbound = kern::to<Int>(kern::formula([](Float inbound) -> Float { return kern::math::wrap32(inbound + 1.0); }, kern::to<Float>(inbound)));
          next = kern::formula([](Float next, Float a, Float now) -> Float { return kern::math::min(next, (a - now)); }, next, gaps_[0].pd_due[n11534410.index], now);
        }
      }
    }
    n11534411 = gaps_repair_offer();
    b = text_format<",\"gaps\":{\"inbound\":%u,\"inboundS\":%.1f,\"sirenCar\":%.4f,\"sirenDrone\":%.4f,\"smoke\":%zu,\"puffs\":%u,\"organic\":%u,\"repair\":%s,\"spawnAllowed\":%u,\"spawnDenied\":%u,\"overlapDenied\":%u,\"camLifted\":%s,\"jetBurning\":%s,\"lamps\":%zu,\"lampsBroken\":%zu,\"lampsFallen\":%zu,\"bursts\":[">(399, inbound, kern::formula([](Float inbound, Float next) -> Float { return ((inbound != 0.0) ? next : 0.0); }, kern::to<Float>(inbound), next), gaps_[0].car_level, gaps_[0].drone_level, gaps_rules::SmokeField_effects(gaps_[0].smoke).Result, gaps_rules_cpp::SmokeField_live_puffs(gaps_[0].smoke).Result, gaps_[0].organic_plays, logic_select<Text>(n11534411.Result, "true", "false"), gaps_[0].spawn_allowed, gaps_[0].spawn_denied, gaps_[0].overlap_denied, logic_select<Text>(gaps_[0].camera_lifted, "true", "false"), logic_select<Text>(gaps_[0].jet_burning, "true", "false"), list_length<lamps_Lamp>(gaps_[0].lamps.lamps_), lamps::Field_broken(gaps_[0].lamps).Result, list_length<physics_Body>(gaps_[0].fallen));
    s = text_append(s, b, "");
    first = true;
    for (auto it11534412 = flow_for(n11534412, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_Burst>(gaps_[0].bursts))))); it11534412.next();) {
      skip = false;
      x = 0.0;
      y = 0.0;
      depth = 0.0;
      n11534413 = project_screen(x, y, depth, gaps_[0].bursts[n11534412.index].p);
      if (logic_not(n11534413.Result)) {
        skip = true;
      }
      if (logic_not(skip)) {
        b = text_format<"%s[%.4f,%.4f,%u]">(399, logic_select<Text>(first, "", ","), x, y, gaps_[0].bursts[n11534412.index].serial);
        s = text_append(s, b, "");
        first = false;
      }
    }
    placed = 0;
    far = 0;
    for (auto it11534414 = flow_for(n11534414, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it11534414.next();) {
      placed = kern::to<Int>(kern::formula([](Float placed, Float a) -> Float { return kern::math::wrap32(placed + kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0))); }, kern::to<Float>(placed), kern::to<Float>(cars_[n11534414.index].placed)));
      far = kern::to<Int>(kern::formula([](Float far, Float a) -> Float { return kern::math::wrap32(far + kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0))); }, kern::to<Float>(far), kern::to<Float>(cars_[n11534414.index].far)));
    }
    count = 0;
    at = 0;
    end = list_length<GapsState_RocketPuff>(gaps_[0].rocket_smoke);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (gaps_hud_match(gaps_[0].rocket_smoke[at]).Result) {
        count += 1;
      }
      at += 1;
    }
    b = text_format<"],\"footShots\":%u,\"rocketSmoke\":%u,\"carsPlaced\":%u,\"farCars\":%u}">(399, gaps_[0].foot_shots, kern::to<Int>(kern::formula([](Float count) -> Float { return kern::math::wrap32(count); }, kern::to<Float>(count))), placed, far);
    s = text_append(s, b, "");
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

  void Game_guided_press_consider(Float& best_score, guided_Target& best, Vector& best_at, const guided_Target& t, Vector eye = Vector{0.0, 0.0, 0.0}, Vector fwd = Vector{0.0, 0.0, 0.0}, Float height = 0.0, Vector at = Vector{0.0, 0.0, 0.0}) {
    Float dist = 0.0;
    Float s = 0.0;
    dist = vector_length(vector_sub(at, eye));
    if (logic_or(math_compare<">">(dist, guided_cpp::range().Result), math_compare<"<">(dist, 3.0))) {
      return;
    }
    n12582929 = guided_cpp::pick_score(eye, fwd, cam_.fov, height, at);
    s = n12582929.Result;
    if (logic_and(math_compare<">=">(s, 0.0), math_compare<"<">(s, best_score))) {
      best_score = s;
      best = t;
      best_at = at;
    }
  }

  struct guided_pressResult { Bool Result = false; };
  guided_pressResult guided_press() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Text b = "";
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Vector eye = Vector{0.0, 0.0, 0.0};
    Vector mark_now = Vector{0.0, 0.0, 0.0};
    Bool marked = false;
    Bool either = false;
    Bool either_2 = false;
    Bool both = false;
    Float height = 0.0;
    guided_Target best = guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"};
    Float best_score = 0.0;
    Vector best_at = Vector{0.0, 0.0, 0.0};
    guided_Target a = guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"};
    Bool skip = false;
    Bool bus = false;
    guided_Target a_2 = guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    guided_Target a_4 = guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"};
    guided_Target a_5 = guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"};
    the_RayResult_ r = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics = 0;
    guided_Target best_2 = guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"};
    Vector best_at_2 = Vector{0.0, 0.0, 0.0};
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool pedestrian = false;
    guided_Flight f = guided_Flight{guided_Missile{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, guided_Dodge{0.0, false}, Vector{0.0, 0.0, 0.0}, true}, guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"}, Vector{0.0, 0.0, 0.0}};
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    if (logic_or(logic_not(drone_out_), player_dead_)) {
      return {false};
    }
    if (math_compare<"<">(now, kern::formula([](Float guided_last_, Float the_result) -> Float { return (guided_last_ + the_result); }, guided_last_, guided_cpp::cooldown().Result))) {
      b = "";
      b = text_format<"LENKRAKETE LAEDT - %.1f s">(63, kern::formula([](Float guided_last_, Float the_result, Float now) -> Float { return ((guided_last_ + the_result) - now); }, guided_last_, guided_cpp::cooldown().Result, now));
      toast_ = b;
      toast_age_ = 0.0;
      engine_game_world_play_sound(world_, "builtin:click", cam_.eye, 0.4, 0.7, false, 0);
      osfx("guided_missile.denied", cam_.eye, 0.0, 0.0, 0.0);
      return {false};
    }
    fwd = Vector{0.0, 0.0, 0.0};
    n12582924 = aim_ray(fwd);
    eye = n12582924.Result;
    mark_now = Vector{0.0, 0.0, 0.0};
    both = logic_and(guided_marked_, math_compare<"<">(kern::formula([](Float now, Float guided_mark_time_) -> Float { return (now - guided_mark_time_); }, now, guided_mark_time_), kGuidedMarkSeconds));
    if (both) {
      either_2 = enum_equal<guided_TargetKind>(guided_mark_.kind, guided_TargetKind::point);
      if (logic_not(either_2)) {
        n12582928 = guided_target_at(guided_mark_, mark_now);
        either_2 = n12582928.Result;
      }
      both = either_2;
    }
    marked = both;
    if (logic_not(marked)) {
      height = kern::formula([](Float frame_h_) -> Float { return kern::math::max(1.0, frame_h_); }, frame_h_);
      best = guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"};
      best_score = guided_cpp::pick_px().Result;
      best_at = Vector{0.0, 0.0, 0.0};
      for (auto it12582930 = flow_for(n12582930, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<People_Person>(people_.people_))))); it12582930.next();) {
        if (game_guided_cpp::person_up(people_.people_[n12582930.index]).Result) {
          a = struct_make<guided_Target>(guided_TargetKind::person, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n12582930.index))), 0, game_guided_cpp::person_label(people_.people_[n12582930.index]).Result);
          Game_guided_press_consider(best_score, best, best_at, a, eye, fwd, height, vector_add(people_.people_[n12582930.index].pos, Vector{0.0, 0.0, 1.1}));
        }
      }
      for (auto it12582931 = flow_for(n12582931, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it12582931.next();) {
        skip = false;
        if (logic_or(logic_not(cars_[n12582931.index].placed), cars_[n12582931.index].wreck)) {
          skip = true;
        }
        if (logic_not(skip)) {
          bus = math_compare<">">(cars_[n12582931.index].car[0].s_.radius, 0.4);
          a_2 = struct_make<guided_Target>(guided_TargetKind::car, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n12582931.index))), 0, logic_select<Text>(bus, "BUS", "AUTO"));
          n12582932 = matrix_of(cars_[n12582931.index].actor);
          a_3 = n12582932.Result;
          Game_guided_press_consider(best_score, best, best_at, a_2, eye, fwd, height, vector_add(engine_math_math::Mat4_translation(a_3).Result, Vector{0.0, 0.0, 0.8}));
        }
      }
      for (auto it12582933 = flow_for(n12582933, 0, 4); it12582933.next();) {
        if (pdrones_[n12582933.index].active) {
          a_4 = struct_make<guided_Target>(guided_TargetKind::police_drone, n12582933.index, 0, "POLIZEIDROHNE");
          Game_guided_press_consider(best_score, best, best_at, a_4, eye, fwd, height, pdrones_[n12582933.index].pos);
        }
      }
      for (auto it12582934 = flow_for(n12582934, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it12582934.next();) {
        a_5 = struct_make<guided_Target>(guided_TargetKind::player, -1, net_.remotes_[n12582934.index].state.slot, logic_select<Text>(enum_equal<net_PlayerMode>(net_.remotes_[n12582934.index].state.mode, net_PlayerMode::drone), "DROHNE", "SPIELER"));
        Game_guided_press_consider(best_score, best, best_at, a_5, eye, fwd, height, vector_add(net_.remotes_[n12582934.index].shown, vector_make(0.0, 0.0, kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.1 : 0.0); }, kern::to<Float>(enum_equal<net_PlayerMode>(net_.remotes_[n12582934.index].state.mode, net_PlayerMode::foot))))));
      }
      if (enum_equal<guided_TargetKind>(best.kind, guided_TargetKind::point)) {
        engine_game_world_physics(n12582935, world_);
        physics = n12582935.result;
        r = common::cast(physics, eye, fwd, guided_cpp::range().Result, body(drone_actor_).Result, Int64{0xffffffffffffffff}).Result;
        best_at = logic_select<Vector>(r.hit, r.point, vector_add(eye, vector_scale(fwd, guided_cpp::range().Result)));
        best.label = "ZIELPUNKT";
      }
      gaps_guided_sounds();
      engine_game_world_play_sound(world_, "sc:locktone", eye, 1.0, 1.0, false, 0);
      osfx("guided_missile.lockTone", eye, 0.0, 0.0, 0.0);
      guided_mark_ = best;
      guided_mark_at_ = best_at;
      guided_mark_time_ = now;
      guided_marked_ = true;
      toast_ = text_append(text_append("MARKIERT: ", best.label, ""), " - NOCHMAL TIPPEN = FEUER", "");
      toast_age_ = 0.0;
      return {false};
    }
    best_2 = guided_mark_;
    best_at_2 = logic_select<Vector>(enum_equal<guided_TargetKind>(best_2.kind, guided_TargetKind::point), guided_mark_at_, mark_now);
    guided_marked_ = false;
    n12582936 = state_of(drone_actor_);
    s = n12582936.Result;
    pedestrian = enum_equal<guided_TargetKind>(best_2.kind, guided_TargetKind::person);
    if (enum_equal<guided_TargetKind>(best_2.kind, guided_TargetKind::player)) {
      for (auto it12582937 = flow_for(n12582937, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it12582937.next();) {
        if (math_compare<"==">(kern::to<Float>(net_.remotes_[n12582937.index].state.slot), kern::to<Float>(best_2.slot))) {
          pedestrian = enum_equal<net_PlayerMode>(net_.remotes_[n12582937.index].state.mode, net_PlayerMode::foot);
        }
      }
    }
    f = guided_Flight{guided_Missile{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, guided_Dodge{0.0, false}, Vector{0.0, 0.0, 0.0}, true}, guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"}, Vector{0.0, 0.0, 0.0}};
    n12582938 = guided_cpp::launch(vector_sub(s.position, Vector{0.0, 0.0, 0.35}), best_at_2, now, pedestrian);
    f.m = n12582938.Result;
    f.m.v = vector_add(f.m.v, s.velocity);
    f.t = best_2;
    f.prev = f.m.p;
    list_add(guided_, f);
    guided_last_ = now;
    guided_lock_ = best_2.label;
    if (logic_and(logic_not(enum_equal<guided_TargetKind>(best_2.kind, guided_TargetKind::point)), pedestrian)) {
      toast_ = text_append(text_append("GUIDED LOCK - ", best_2.label, ""), " - SPRINT ZUM AUSWEICHEN", "");
      toast_age_ = 0.0;
    } else {
      toast_ = text_append("LENKRAKETE AUF ", best_2.label, "");
      toast_age_ = 0.0;
    }
    people_cpp::People_frighten(people_, s.position, 18.0, now, 1.0);
    return {true};
  }

  void draw_guided() {
    for (auto it12582941 = flow_for(n12582941, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))); it12582941.next();) {
      people_art_cpp::PeopleArt_projectile(people_art_, guided_[n12582941.index].m.p, guided_[n12582941.index].m.v, 0.06, 0.9, guided_[n12582941.index].m.motor);
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
      n12582943 = guided_target_at(guided_mark_, at);
      both = logic_not(n12582943.Result);
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

  void guided_hud(Text& s) {
    Float now = 0.0;
    Float sim_time = 0.0;
    Text b = "";
    Float dist = 0.0;
    Text lock = "";
    guided_Flight f = guided_Flight{guided_Missile{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, false, guided_Dodge{0.0, false}, Vector{0.0, 0.0, 0.0}, true}, guided_Target{guided_TargetKind::point, -1, 0, "ZIELPUNKT"}, Vector{0.0, 0.0, 0.0}};
    Bool marked = false;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    b = "";
    dist = -1.0;
    lock = "";
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<guided_Flight>(guided_)), 0.0))) {
      f = guided_[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))];
      dist = vector_length(vector_sub(f.m.goal, f.m.p));
      lock = logic_select<Text>(f.m.tracking, f.t.label, "VERLOREN");
    }
    marked = logic_and(guided_marked_, math_compare<"<">(kern::formula([](Float now, Float guided_mark_time_) -> Float { return (now - guided_mark_time_); }, now, guided_mark_time_), kGuidedMarkSeconds));
    b = text_format<",\"guided\":{\"cd\":%.1f,\"flying\":%zu,\"lock\":\"%s\",\"dist\":%.0f,\"last\":\"%s\",\"marked\":\"%s\"}">(159, kern::formula([](Float guided_last_, Float the_result, Float now) -> Float { return kern::math::max(0.0, ((guided_last_ + the_result) - now)); }, guided_last_, guided_cpp::cooldown().Result, now), list_length<guided_Flight>(guided_), lock, dist, guided_lock_, logic_select<Text>(marked, guided_mark_.label, ""));
    s = text_append(s, b, "");
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
    Int w = 0;
    Int physics = 0;
    Bool body_valid = false;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0))) {
      return;
    }
    engine_game_world_physics(n13631509, world_);
    physics = n13631509.result;
    w = physics;
    for (auto it13631510 = flow_for(n13631510, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<physics_Body>(gaps_[0].fallen))))); it13631510.next();) {
      body_valid = engine_world_body_valid(w, gaps_[0].fallen[n13631510.index]);
      if (body_valid) {
        engine_world_destroy_body(w, gaps_[0].fallen[n13631510.index]);
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
      engine_renderer_set_visible(n13631515, gaps_[0].renderer, p.id, false);
      set_visible = n13631515.result;
      return;
    }
    engine_renderer_update_instances(n13631516, gaps_[0].renderer, p.id, p.transforms, p.tints);
    update_instances = n13631516.result;
    engine_renderer_set_visible(n13631517, gaps_[0].renderer, p.id, true);
    set_visible_2 = n13631517.result;
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
      for (auto it13631511 = flow_for(n13631511, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(gaps_[0].lamps.lamps_))))); it13631511.next();) {
        skip = false;
        if (math_compare<"==">(kern::to<Float>(gaps_[0].lamps.lamps_[n13631511.index].state), 2.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_or(math_compare<">">(kern::formula([](Float x, Float x_2) -> Float { return kern::math::abs((x - x_2)); }, vector_break(gaps_[0].lamps.lamps_[n13631511.index].foot).x, vector_break(cam_.eye).x), kern::formula([](Float lamp_m, Float the_result) -> Float { return (lamp_m * kern::math::min(the_result, 3.0)); }, look_.lamp_m, view_zoom().Result)), math_compare<">">(kern::formula([](Float y, Float y_2) -> Float { return kern::math::abs((y - y_2)); }, vector_break(gaps_[0].lamps.lamps_[n13631511.index].foot).y, vector_break(cam_.eye).y), kern::formula([](Float lamp_m, Float the_result) -> Float { return (lamp_m * kern::math::min(the_result, 3.0)); }, look_.lamp_m, view_zoom().Result)))) {
            skip = true;
          }
          if (logic_not(skip)) {
            m = game_lamps_cpp::yaw_at(gaps_[0].lamps.lamps_[n13631511.index].foot, gaps_[0].lamps.lamps_[n13631511.index].yaw).Result;
            gaps_lamps_draw_put(gaps_[0].lamp_pole, m, engine_math_math::Vec4_new(1.0, 1.0, 1.0, 1.0).Result);
            if (logic_and(math_compare<"==">(kern::to<Float>(gaps_[0].lamps.lamps_[n13631511.index].state), 0.0), math_compare<">">(glow, 0.01))) {
              a_2 = engine_math_math::Mat4_translate(vector_make(kArm, 0.0, kern::formula([](Float kHeadZ) -> Float { return (kHeadZ - 0.075); }, kHeadZ))).Result;
              a = engine_math_math::Mat4_mul(m, a_2).Result;
              gaps_lamps_draw_put(gaps_[0].lamp_glow, a, engine_math_math::Vec4_new(glow, glow, glow, 1.0).Result);
              c = vector_make(vector_break(gaps_[0].lamps.lamps_[n13631511.index].head).x, vector_break(gaps_[0].lamps.lamps_[n13631511.index].head).y, kern::formula([](Float the_result) -> Float { return (the_result + 0.14); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(gaps_[0].lamps.lamps_[n13631511.index].head).x, vector_break(gaps_[0].lamps.lamps_[n13631511.index].head).y).Result));
              for (auto it13631512 = flow_for(n13631512, 0, 3); it13631512.next();) {
                rr = kern::formula([](Float index) -> Float { return (13.0 * (1.0 - (index * 0.22))); }, kern::to<Float>(n13631512.index));
                a_4 = engine_math_math::Mat4_translate(vector_add(c, vector_make(0.0, 0.0, kern::formula([](Float index) -> Float { return (index * 0.004); }, kern::to<Float>(n13631512.index))))).Result;
                a_5 = engine_math_math::Mat4_scale(vector_make(rr, rr, 1.0)).Result;
                a_3 = engine_math_math::Mat4_mul(a_4, a_5).Result;
                gaps_lamps_draw_put(gaps_[0].lamp_pool, a_3, engine_math_math::Vec4_new(1.0, 1.0, 1.0, kern::formula([](Float glow) -> Float { return ((glow * 0.55) * 0.22); }, glow)).Result);
              }
            }
          }
        }
      }
      engine_game_world_physics(n13631513, world_);
      physics = n13631513.result;
      w = physics;
      for (auto it13631514 = flow_for(n13631514, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<physics_Body>(gaps_[0].fallen))))); it13631514.next();) {
        skip_2 = false;
        body_valid = engine_world_body_valid(w, gaps_[0].fallen[n13631514.index]);
        if (logic_not(body_valid)) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          s = common::read_body(w, gaps_[0].fallen[n13631514.index]).Result;
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

  void life_draw() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Float sim_time_2 = 0.0;
    Float chosen = 0.0;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0)), logic_not(glife_[0].art_ready))) {
      return;
    }
    if (math_compare<"!=">(kern::to<Float>(world_), 0.0)) {
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
    n14680076 = life_cpp::Life_shot(glife_[0].life, to, glife_[0].out, from, 6.0, sim_time, true);
    return {n14680076.Result};
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
      engine_game_world_set_actor_velocity(n14680077, world_, soldier_, vector_add(the_actor_velocity, dv));
      set_actor_velocity = n14680077.result;
      if (math_compare<">">(f.a, 0.0)) {
        n14680078 = player_position();
        engine_game_world_damage(world_, soldier_, kern::formula([](Float a) -> Float { return kern::math::min(100.0, a); }, f.a), 0, false, vector_sub(n14680078.Result, dv));
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
    for (auto it14680079 = flow_for(n14680079, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_))))); it14680079.next();) {
      dead = kern::to<Int>(kern::formula([](Float dead, Float a) -> Float { return kern::math::wrap32(dead + kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0))); }, kern::to<Float>(dead), kern::to<Float>(glife_[0].life.cows_[n14680079.index].dead)));
    }
    for (auto it14680080 = flow_for(n14680080, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it14680080.next();) {
      air = kern::to<Int>(kern::formula([](Float air, Float a) -> Float { return kern::math::wrap32(air + kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0))); }, kern::to<Float>(air), kern::to<Float>(glife_[0].life.flocks_[n14680080.index].air)));
    }
    pigeons = 0;
    for (auto it14680081 = flow_for(n14680081, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it14680081.next();) {
      pigeons = kern::to<Int>(kern::formula([](Float pigeons, Float a) -> Float { return kern::math::wrap32(pigeons + kern::math::wrap32(a)); }, kern::to<Float>(pigeons), kern::to<Float>(list_length<Life_Pigeon>(glife_[0].life.flocks_[n14680081.index].birds))));
    }
    for (auto it14680082 = flow_for(n14680082, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Bird>(glife_[0].life.birds_))))); it14680082.next();) {
      birds = kern::to<Int>(kern::formula([](Float birds, Float now, Float a) -> Float { return kern::math::wrap32(birds + kern::math::wrap32(((now >= a) ? 1.0 : 0.0))); }, kern::to<Float>(birds), now, glife_[0].life.birds_[n14680082.index].dead_until));
    }
    joggers = 0;
    if (life::Life_has_chain(glife_[0].life).Result) {
      for (auto it14680083 = flow_for(n14680083, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Runner>(glife_[0].life.chain_.runners))))); it14680083.next();) {
        joggers = kern::to<Int>(kern::formula([](Float joggers, Float a) -> Float { return kern::math::wrap32(joggers + kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0))); }, kern::to<Float>(joggers), kern::to<Float>(glife_[0].life.chain_.runners[n14680083.index].alive)));
      }
    }
    b = "";
    b = text_format<",\"life\":{\"cows\":%u,\"cowsDead\":%u,\"cowHits\":%u,\"flocks\":%u,\"flocksAir\":%u,\"pigeons\":%u,\"birds\":%u,\"birdHits\":%u,\"trees\":%u,\"joggers\":%u,\"joggerStreak\":%u,\"ragdolls\":%u,\"ragdollSpawns\":%u,\"falls\":%u,\"getups\":%u,\"shouts\":%u,\"honks\":%u">(639, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_)))), dead, life::Life_cow_hits(glife_[0].life).Result, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_)))), air, pigeons, birds, life::Life_bird_hits(glife_[0].life).Result, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Tree>(glife_[0].life.trees_)))), joggers, life::Life_jogger_streak(glife_[0].life).Result, ragdoll_cpp::Ragdolls_active(glife_[0].life.ragdolls_).Result, ragdoll::Ragdolls_spawned(glife_[0].life.ragdolls_).Result, glife_[0].falls, glife_[0].getups, life::Life_shouts(glife_[0].life).Result, life::Life_honks(glife_[0].life).Result);
    s = text_append(s, b, "");
    if (math_compare<"<">(now, glife_[0].banner_until)) {
      s = text_append(s, text_append(text_append(",\"banner\":\"", game_life_cpp::json_escape(glife_[0].banner).Result, ""), "\"", ""), "");
    }
    s = text_append(s, ",\"say\":[", "");
    for (auto it14680084 = flow_for(n14680084, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<LifeOut_Say>(glife_[0].speech))))); it14680084.next();) {
      b = text_format<"%s[\"%s\",%.2f,%.2f,%.2f,%.2f]">(639, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n14680084.index), 0.0), ",", ""), game_life_cpp::json_escape(glife_[0].speech[n14680084.index].text).Result, glife_[0].speech[n14680084.index].pitch, glife_[0].speech[n14680084.index].rate, glife_[0].speech[n14680084.index].volume, glife_[0].speech[n14680084.index].delay);
      s = text_append(s, b, "");
    }
    list_clear(glife_[0].speech);
    s = text_append(s, "],\"bubbles\":[", "");
    first = true;
    aspect = kern::formula([](Float frame_h_, Float frame_w_) -> Float { return ((frame_h_ > 0.0) ? kern::math::div(frame_w_, frame_h_) : kern::math::div(16.0, 9.0)); }, frame_h_, frame_w_);
    a = engine_math_math::projection_matrix(cam_, aspect).Result;
    n14680085 = engine_math_math::Mat4_look_at_2(cam_.eye, cam_.target, cam_.up);
    a_2 = n14680085.Result;
    vp = engine_math_math::Mat4_mul(a, a_2).Result;
    for (auto it14680086 = flow_for(n14680086, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Voice>(glife_[0].life.voices_))))); it14680086.next();) {
      skip = false;
      c = engine_math_math::Mat4_transform(vp, engine_math_math::Vec4_new(vector_break(glife_[0].life.voices_[n14680086.index].at).x, vector_break(glife_[0].life.voices_[n14680086.index].at).y, vector_break(glife_[0].life.voices_[n14680086.index].at).z, 1.0).Result).Result;
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
          b = text_format<"%s[%.4f,%.4f,\"%s\",\"%s\",%s]">(639, logic_select<Text>(first, "", ","), kern::formula([](Float x) -> Float { return kern::math::div((x + 1.0), 2.0); }, x), kern::formula([](Float y) -> Float { return kern::math::div((1.0 - y), 2.0); }, y), game_life_cpp::json_escape(glife_[0].life.voices_[n14680086.index].text).Result, glife_[0].life.voices_[n14680086.index].mood, logic_select<Text>(math_compare<"<">(kern::formula([](Float a, Float now) -> Float { return (a - now); }, glife_[0].life.voices_[n14680086.index].until, now), 0.35), "true", "false"));
          s = text_append(s, b, "");
          first = false;
        }
      }
    }
    s = text_append(s, "]", "");
    interiors_cpp::Interiors_hud(glife_[0].interiors, s);
    s = text_append(s, "}", "");
  }

  void life_draw_ragdolls() {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<GameLife>(glife_)), 0.0))) {
      return;
    }
    list_clear(life_draw_ragdolls_bones);
    ragdoll_cpp::Ragdolls_draw(glife_[0].life.ragdolls_, life_draw_ragdolls_bones);
    people_art_cpp::PeopleArt_ragdolls(people_art_, life_draw_ragdolls_bones, glife_[0].life.ragdolls_);
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

  struct look_people_budgetResult { Int Result = 0; };
  KERN_PURE look_people_budgetResult look_people_budget() {
    return {kern::to<Int>(kern::formula([](Float a, Float mobile_) -> Float { return ((a != 0.0) ? ((mobile_ != 0.0) ? 68.0 : 100.0) : 0.0); }, kern::to<Float>(settings::Settings_on(ui_.settings, Opt::world_people).Result), kern::to<Float>(mobile_)))};
  }

  void look_apply_tier() {
    Int t = 0;
    perf_Tier q = perf_Tier{"", 0, 0.0, 0, 0, 0.0, 0.0, 0, 0.0};
    Int old = 0;
    Int LightPool_2 = 0;
    Int r = 0;
    Int renderer = 0;
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
      LightPool_2 = n15728650.result;
      look_.lights = LightPool_2;
      if (math_compare<"!=">(kern::to<Float>(old), 0.0)) {
        engine_light_pool_release(old);
      }
    }
    if (math_compare<"!=">(kern::to<Float>(engine_), 0.0)) {
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
    n15728651 = look_cpp::Shake_sample(look_.shake, angles, offset, dt, time_, kern::formula([](Float a) -> Float { return ((a != 0.0) ? 0.85 : 1.0); }, kern::to<Float>(enum_equal<Mode>(mode_, Mode::drone))));
    if (logic_and(n15728651.Result, logic_not(enum_equal<Mode>(mode_, Mode::foot)))) {
      look_cpp::shake_camera(cam_, angles, offset);
    }
  }

  void look_blast(Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0) {
    Float r = 0.0;
    r = kern::formula([](Float radius) -> Float { return kern::math::max(6.0, radius); }, radius);
    if (math_compare<">=">(kern::to<Float>(list_length<LookState_Blast>(look_.blasts)), 4.0)) {
      list_remove_at(look_.blasts, 0);
    }
    list_add(look_.blasts, struct_make<LookState_Blast>(vector_add(at, Vector{0.0, 0.0, 1.5}), time_, 0.42, kern::formula([](Float r) -> Float { return (220.0 + (r * 18.0)); }, r), kern::formula([](Float r) -> Float { return (r * 4.0); }, r)));
    if (logic_not(enum_equal<Mode>(mode_, Mode::foot))) {
      look_cpp::Shake_add(look_.shake, look_cpp::blast_trauma(vector_length(vector_sub(cam_.eye, at)), radius).Result, 1.0);
    }
  }

  void Game_look_lighting_req(Vector p = Vector{0.0, 0.0, 0.0}, Int hex = 0, Float three_intensity = 0.0, Float range = 0.0, Float d_ref = 0.0) {
    Float k = 0.0;
    Bool request = false;
    k = look_cpp::Kern_intensity(three_intensity, range, d_ref).Result;
    if (math_compare<">">(k, 0.0)) {
      engine_light_pool_request(n15728653, look_.lights, p, look_cpp::hex_linear(hex).Result, k, range);
      request = n15728653.result;
    }
  }

  void look_vr_hud() {
    Int r = 0;
    Int renderer = 0;
    Bool on = false;
    Bool xr = false;
    Bool xr_2 = false;
    Bool both = false;
    Bool set_visible = false;
    MeshData q = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
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
        engine_renderer_set_visible(n15728665, r, vr_hud_mesh_, false);
        set_visible = n15728665.result;
      }
      return;
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(vr_hud_mesh_), 0.0))) {
      q = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
      engine_geo_mesh_data::MeshData_push(q, Vector{-0.5, 0.0, 0.5}, Vector{0.0, -1.0, 0.0}, Vector{1.0, 1.0, 1.0}, Vector{0.0, 0.0, 0.0});
      engine_geo_mesh_data::MeshData_push(q, Vector{0.5, 0.0, 0.5}, Vector{0.0, -1.0, 0.0}, Vector{1.0, 1.0, 1.0}, Vector{1.0, 0.0, 0.0});
      engine_geo_mesh_data::MeshData_push(q, Vector{0.5, 0.0, -0.5}, Vector{0.0, -1.0, 0.0}, Vector{1.0, 1.0, 1.0}, Vector{1.0, 1.0, 0.0});
      engine_geo_mesh_data::MeshData_push(q, Vector{-0.5, 0.0, -0.5}, Vector{0.0, -1.0, 0.0}, Vector{1.0, 1.0, 1.0}, Vector{0.0, 1.0, 0.0});
      q.indices = list_make<Int, "6">(0, 2, 1, 0, 3, 2);
      engine_renderer_allocate_id(n15728666, r);
      allocate_id = n15728666.result;
      vr_hud_mesh_ = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
      look.material.unlit = true;
      look.material.double_sided = true;
      look.material.cast_shadow = false;
      look.material.frustum_culled = false;
      look.material.alpha = AlphaMode::blend;
      look.extra_slot = kMaxExtensionAttributes;
      look.texture = vr_hud_tex_;
      engine_upsert(n15728667, r, vr_hud_mesh_, q, look, List<Float>{}, List<Float>{});
      upsert = n15728667.result;
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
    n15728668 = vr_hand_matrix(lh, 0);
    if (n15728668.Result) {
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
    engine_renderer_update_instances(n15728669, r, vr_hud_mesh_, m, List<Float>{});
    update_instances = n15728669.result;
    engine_renderer_set_visible(n15728670, r, vr_hud_mesh_, true);
    set_visible_2 = n15728670.result;
  }

  void look_view_models_make(const MeshData& d, Int& id, Int r = 0, Int n15728671_index = 0, Bool g = false) {
    Int r_ = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    r_ = r;
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(d.positions)), 0.0)) {
      return;
    }
    engine_renderer_allocate_id(n15728673, r_);
    allocate_id = n15728673.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = game_look_cpp::vm_material(g, math_compare<"==">(kern::to<Float>(n15728671_index), 1.0)).Result;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    engine_upsert(n15728674, r_, id, d, look, List<Float>{}, List<Float>{});
    upsert = n15728674.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, id);
      id = 0;
    }
  }

  struct look_hud_jsonResult { Text Result = ""; };
  look_hud_jsonResult look_hud_json() {
    perf_Tier q = perf_Tier{"", 0, 0.0, 0, 0, 0.0, 0.0, 0, 0.0};
    Float hours = 0.0;
    Int hh = 0;
    Int mm = 0;
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

  struct spawn_unseenResult { Bool Result = false; };
  spawn_unseenResult spawn_unseen(Vector p = Vector{0.0, 0.0, 0.0}, Float height = 0.0) {
    Vector top = Vector{0.0, 0.0, 0.0};
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Vector to = Vector{0.0, 0.0, 0.0};
    Float d = 0.0;
    Int physics = 0;
    if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
      n16777228 = gaps_can_spawn(p, height, 0.0);
      return {n16777228.Result};
    }
    top = vector_add(p, vector_make(0.0, 0.0, height));
    fwd = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{1.0, 0.0, 0.0});
    to = vector_sub(top, cam_.eye);
    d = vector_length(to);
    if (math_compare<"<">(d, 1.0)) {
      return {false};
    }
    if (math_compare<"<">(vector_dot(fwd, vector_scale(to, kern::formula([](Float d) -> Float { return kern::math::div(1.0, d); }, d))), kern::formula([](Float fov) -> Float { return kern::math::cos(kern::math::min(1.4, (((fov * 0.5) * 1.9) + 0.15))); }, cam_.fov))) {
      return {true};
    }
    engine_game_world_physics(n16777229, world_);
    physics = n16777229.result;
    return {struct_break<the_RayResult_>(common::cast(physics, cam_.eye, vector_scale(to, kern::formula([](Float d) -> Float { return kern::math::div(1.0, d); }, d)), kern::formula([](Float d) -> Float { return (d - 1.0); }, d), struct_make<physics_Body>(Int64{0x0}), Int64{0x1}).Result).hit};
  }

  void spawn_heli(Vector target = Vector{0.0, 0.0, 0.0}) {
    Bool skip = false;
    Float a = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Heli h = Heli{Heli_State::inbound, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, false, 0};
    Float sim_time = 0.0;
    for (auto it16777230 = flow_for(n16777230, 0, 11); it16777230.next();) {
      skip = false;
      n16777231 = common::Rng_range(rng_, 0.0, kern::formula([](Float kPi) -> Float { return (2.0 * kPi); }, kPi));
      a = n16777231.Result;
      p = vector_make(kern::formula([](Float x, Float a) -> Float { return (x + (kern::math::cos(a) * 650.0)); }, vector_break(target).x, a), kern::formula([](Float y, Float a) -> Float { return (y + (kern::math::sin(a) * 650.0)); }, vector_break(target).y, a), 0.0);
      p = vector_make(vector_break(p).x, vector_break(p).y, kern::formula([](Float the_result) -> Float { return (the_result + 95.0); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(p).x, vector_break(p).y).Result));
      n16777232 = spawn_unseen(p, 6.0);
      if (logic_not(n16777232.Result)) {
        skip = true;
      }
      if (logic_not(skip)) {
        h = Heli{Heli_State::inbound, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, false, 0};
        h.hp = combat_kHeliHp;
        h.p = p;
        h.orbit = a;
        h.yaw = kern::formula([](Float y, Float y_2, Float x, Float x_2, Float kPi) -> Float { return (kern::math::atan2((y - y_2), (x - x_2)) - kern::math::div(kPi, 2.0)); }, vector_break(target).y, vector_break(p).y, vector_break(target).x, vector_break(p).x, kPi);
        sim_time = engine_game_world_sim_time(world_);
        h.next_burst = kern::formula([](Float sim_time) -> Float { return (sim_time + 5.0); }, sim_time);
        list_add(combat_.helis, h);
        osfx("military_response.sound", h.p, 1.0, 0.2, 0.3);
        combat_.pending_heli = kern::to<Int>(kern::formula([](Float pending_heli) -> Float { return (pending_heli - 1.0); }, kern::to<Float>(combat_.pending_heli)));
        return;
      }
    }
  }

  struct spawn_tankResult { Bool Result = false; };
  spawn_tankResult spawn_tank(Vector target = Vector{0.0, 0.0, 0.0}) {
    Int k = 0;
    Bool skip = false;
    Bool either = false;
    Vector q = Vector{0.0, 0.0, 0.0};
    Vector p = Vector{0.0, 0.0, 0.0};
    Float d = 0.0;
    Bool either_2 = false;
    Tank t = Tank{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, -99.0, -1.0, -99.0, -1.0, 0.0, 0.0, -1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, List<Vector>{}, false, false, false, false, 0, physics_Body{Int64{0x0}}};
    Float now = 0.0;
    Float sim_time = 0.0;
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    Int physics = 0;
    physics_Body create_body = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    Int physics_2 = 0;
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    if (logic_not(int64_equal(roads_.built_from, road_graph_cpp::lane_signature(city_.lanes_).Result))) {
      road_graph_cpp::RoadGraph_rebuild(roads_, city_.lanes_);
    }
    k = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(k), 40.0), logic_not(math_compare<"==">(kern::to<Float>(list_length<Lane>(city_.lanes_)), 0.0))))) {
      skip = false;
      n16777233 = common::Rng_next(rng_);
      either = logic_not(city_.lanes_[kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n16777233.Result), kern::to<Float>(list_length<Lane>(city_.lanes_))))].alive);
      if (logic_not(either)) {
        n16777235 = common::Rng_next(rng_);
        either = math_compare<"==">(kern::to<Float>(list_length<Vector>(city_.lanes_[kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n16777235.Result), kern::to<Float>(list_length<Lane>(city_.lanes_))))].points)), 0.0);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        n16777236 = common::Rng_next(rng_);
        n16777237 = common::Rng_next(rng_);
        n16777238 = common::Rng_next(rng_);
        q = city_.lanes_[kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n16777236.Result), kern::to<Float>(list_length<Lane>(city_.lanes_))))].points[kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n16777237.Result), kern::to<Float>(list_length<Vector>(city_.lanes_[kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n16777238.Result), kern::to<Float>(list_length<Lane>(city_.lanes_))))].points))))];
        p = vector_make(vector_break(q).x, vector_break(q).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(q).x, vector_break(q).y).Result);
        d = game_military_cpp::dist2d(p, target).Result;
        either_2 = logic_or(math_compare<"<">(d, 170.0), math_compare<">">(d, 300.0));
        if (logic_not(either_2)) {
          n16777240 = spawn_unseen(p, 2.0);
          either_2 = logic_not(n16777240.Result);
        }
        if (either_2) {
          skip = true;
        }
        if (logic_not(skip)) {
          t = Tank{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, -99.0, -1.0, -99.0, -1.0, 0.0, 0.0, -1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, List<Vector>{}, false, false, false, false, 0, physics_Body{Int64{0x0}}};
          t.hp = combat_kTankHp;
          t.p = p;
          sim_time = engine_game_world_sim_time(world_);
          now = sim_time;
          t.next_shell = kern::formula([](Float now) -> Float { return (now + 6.0); }, now);
          t.progress_p = vector_make(vector_break(p).x, vector_break(p).y, 0.0);
          t.progress_t = now;
          n16777241 = road_graph_cpp::RoadGraph_path(roads_, t.path, vector_make(vector_break(p).x, vector_break(p).y, 0.0), vector_make(vector_break(target).x, vector_break(target).y, 0.0));
          if (math_compare<">">(kern::to<Float>(list_length<Vector>(t.path)), 1.0)) {
            t.yaw = kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(t.path[1]).y, vector_break(p).y, vector_break(t.path[1]).x, vector_break(p).x);
          }
          bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
          bs.type = physics_BodyType::kinematic;
          bs.position = vector_add(p, Vector{0.0, 0.0, 0.85});
          bs.rotation = common::yaw_quat(t.yaw).Result;
          engine_game_world_physics(n16777242, world_);
          physics = n16777242.result;
          engine_world_create_body(n16777243, physics, bs);
          create_body = n16777243.result;
          t.body = create_body;
          if (engine_physics_physics::Body_valid(t.body).Result) {
            ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
            ss.kind = physics_ShapeKind::box;
            ss.size = Vector{6.9, 3.1, 1.6};
            ss.material.friction = 0.8;
            ss.material.category_bits = Int64{0x2};
            engine_game_world_physics(n16777244, world_);
            physics_2 = n16777244.result;
            engine_world_create_shape(n16777245, physics_2, t.body, ss);
            create_shape = n16777245.result;
          }
          list_add(combat_.tanks, t);
          return {true};
        }
      }
      k += 1;
    }
    return {false};
  }

  struct spawn_squadResult { Int Result = 0; };
  spawn_squadResult spawn_squad(Vector target = Vector{0.0, 0.0, 0.0}, Int n = 0) {
    Int alive = 0;
    Int k = 0;
    Bool skip = false;
    Bool either = false;
    Vector q = Vector{0.0, 0.0, 0.0};
    Vector base = Vector{0.0, 0.0, 0.0};
    Float d = 0.0;
    Bool either_2 = false;
    Int placed = 0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Int seed = 0;
    SoldierAi s = SoldierAi{-1, 20.0, 0.0, -99.0, -99.0, -99.0, 0, false, List<Vector>{}, 0};
    Float sim_time = 0.0;
    alive = 0;
    for (auto it16777246 = flow_for(n16777246, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<SoldierAi>(combat_.soldiers))))); it16777246.next();) {
      alive += kern::to<Int>(kern::formula([](Float a) -> Float { return ((a >= 0.0) ? 1.0 : 0.0); }, kern::to<Float>(combat_.soldiers[n16777246.index].person)));
    }
    k = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(k), 30.0), logic_not(math_compare<"==">(kern::to<Float>(list_length<Lane>(city_.lanes_)), 0.0))))) {
      skip = false;
      n16777247 = common::Rng_next(rng_);
      either = logic_not(city_.lanes_[kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n16777247.Result), kern::to<Float>(list_length<Lane>(city_.lanes_))))].alive);
      if (logic_not(either)) {
        n16777249 = common::Rng_next(rng_);
        either = math_compare<"==">(kern::to<Float>(list_length<Vector>(city_.lanes_[kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n16777249.Result), kern::to<Float>(list_length<Lane>(city_.lanes_))))].points)), 0.0);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        n16777250 = common::Rng_next(rng_);
        n16777251 = common::Rng_next(rng_);
        n16777252 = common::Rng_next(rng_);
        q = city_.lanes_[kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n16777250.Result), kern::to<Float>(list_length<Lane>(city_.lanes_))))].points[kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n16777251.Result), kern::to<Float>(list_length<Vector>(city_.lanes_[kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n16777252.Result), kern::to<Float>(list_length<Lane>(city_.lanes_))))].points))))];
        base = vector_make(vector_break(q).x, vector_break(q).y, 0.0);
        d = game_military_cpp::dist2d(base, target).Result;
        either_2 = logic_or(math_compare<"<">(d, 90.0), math_compare<">">(d, 170.0));
        if (logic_not(either_2)) {
          n16777254 = spawn_unseen(base, 1.0);
          either_2 = logic_not(n16777254.Result);
        }
        if (either_2) {
          skip = true;
        }
        if (logic_not(skip)) {
          placed = 0;
          while (loop(logic_and(math_compare<"<">(kern::to<Float>(placed), kern::to<Float>(n)), math_compare<"<">(kern::to<Float>(alive), kern::to<Float>(combat_kMaxSoldiers))))) {
            p = vector_add(base, vector_make(kern::formula([](Float placed) -> Float { return (kern::math::mod(placed, 2.0) * 1.6); }, kern::to<Float>(placed)), kern::formula([](Float placed) -> Float { return (kern::math::trunc(kern::math::div(placed, 2.0)) * 1.6); }, kern::to<Float>(placed)), 0.0));
            n16777255 = common::Rng_next(rng_);
            seed = n16777255.Result;
            s = SoldierAi{-1, 20.0, 0.0, -99.0, -99.0, -99.0, 0, false, List<Vector>{}, 0};
            n16777256 = people_cpp::People_add_scripted(people_, People_Kind::soldier, p, 0.0, combat_kSoldierHp, seed, kSoldierShirt, kSoldierPants, list_get<Vector>(kSoldierSkins, kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::mod(seed, 4.0); }, kern::to<Float>(seed)))), kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::band(kern::math::mod(seed, 4.0), 255.0); }, kern::to<Float>(seed))));
            s.person = n16777256.Result;
            n16777257 = common::Rng_unit(rng_);
            s.hold = kern::formula([](Float the_result) -> Float { return (20.0 + (the_result * 12.0)); }, n16777257.Result);
            sim_time = engine_game_world_sim_time(world_);
            n16777258 = common::Rng_unit(rng_);
            s.next_shot = kern::formula([](Float sim_time, Float the_result) -> Float { return ((sim_time + 3.0) + (the_result * 2.0)); }, sim_time, n16777258.Result);
            list_add(combat_.soldiers, s);
            placed += 1;
            alive += 1;
          }
          return {placed};
        }
      }
      k += 1;
    }
    return {0};
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
    for (auto it16777311 = flow_for(n16777311, 0, 1); it16777311.next();) {
      j = FighterJet{0.0, 0.0, 0.0, 0.0, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false};
      j.x = kern::formula([](Float cx, Float offset, Float index) -> Float { return ((cx + offset) - (index * 26.0)); }, cx, offset, kern::to<Float>(n16777311.index));
      j.y0 = kern::formula([](Float cy, Float combat_kJetSpan, Float index) -> Float { return ((cy - combat_kJetSpan) - (index * 42.0)); }, cy, combat_kJetSpan, kern::to<Float>(n16777311.index));
      j.z = kern::formula([](Float z, Float index) -> Float { return (z + (index * 6.0)); }, z, kern::to<Float>(n16777311.index));
      j.t0 = at;
      j.strike = logic_and(strike, math_compare<"==">(kern::to<Float>(n16777311.index), 0.0));
      j.target = vector_make(cx, cy, 0.0);
      j.remote = remote;
      list_add(combat_.jets, j);
    }
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
    for (auto it17825804 = flow_for(n17825804, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Opt>(list))))); it17825804.next();) {
      w = list[n17825804.index];
      key = struct_break<OptSpec>(settings_cpp::opt_spec(ui_.settings_table, w).Result).key;
      o = text_append(o, logic_select<Text>(first, "\"", ",\""), "");
      for (auto it17825805 = flow_for(n17825805, 6, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(text_length(key))))); it17825805.next();) {
        o = text_append(o, text_of_code(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(key, n17825805.index))))), "");
      }
      o = text_append(o, logic_select<Text>(settings::Settings_on(ui_.settings, w).Result, "\":true", "\":false"), "");
      first = false;
    }
    return {text_append(o, "}", "")};
  }

  struct my_stateResult { net_PlayerState Result = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}; };
  my_stateResult my_state() {
    net_PlayerState s = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255};
    Int a = 0;
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
    Int f_2 = 0;
    BodyState d_2 = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    BodyState j = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector rp = Vector{0.0, 0.0, 0.0};
    Quat rq = Quat{0.0, 0.0, 0.0, 1.0};
    s = net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255};
    s.mode = enum_from_index<net_PlayerMode>(enum_index<Mode>(mode_)).value;
    a = kern::to<Int>(kern::formula([](Float a, Float car_index_, Float a_2, Float a_3, Float jet_actor_, Float a_4, Float drone_actor_, Float a_5, Float chute_actor_, Float soldier_) -> Float { return (((a != 0.0) && (car_index_ >= 0.0)) ? a_2 : ((a_3 != 0.0) ? jet_actor_ : ((a_4 != 0.0) ? drone_actor_ : ((a_5 != 0.0) ? chute_actor_ : soldier_)))); }, kern::to<Float>(enum_equal<Mode>(mode_, Mode::car)), kern::to<Float>(car_index_), kern::to<Float>(cars_[car_index_].actor), kern::to<Float>(enum_equal<Mode>(mode_, Mode::jet)), kern::to<Float>(jet_actor_), kern::to<Float>(enum_equal<Mode>(mode_, Mode::drone)), kern::to<Float>(drone_actor_), kern::to<Float>(enum_equal<Mode>(mode_, Mode::parachute)), kern::to<Float>(chute_actor_), kern::to<Float>(soldier_)));
    n17825806 = state_of(a);
    b = n17825806.Result;
    s.position = b.position;
    s.velocity = b.velocity;
    s.rotation = b.rotation;
    n17825807 = matrix_of(soldier_);
    a_2 = n17825807.Result;
    s.avatar = vector_sub(engine_math_math::Mat4_translation(a_2).Result, Vector{0.0, 0.0, 0.73});
    camera_yaw = engine_game_world_camera_yaw(world_);
    s.avatar_yaw = kern::formula([](Float camera_yaw, Float kDeg) -> Float { return (camera_yaw * kDeg); }, camera_yaw, kDeg);
    camera_yaw_2 = engine_game_world_camera_yaw(world_);
    s.yaw = kern::formula([](Float camera_yaw_2, Float kDeg) -> Float { return (camera_yaw_2 * kDeg); }, camera_yaw_2, kDeg);
    camera_pitch = engine_game_world_camera_pitch(world_);
    s.pitch = kern::formula([](Float camera_pitch, Float kDeg) -> Float { return (camera_pitch * kDeg); }, camera_pitch, kDeg);
    both = math_compare<"!=">(kern::to<Float>(engine_), 0.0);
    if (both) {
      xr_2 = engine_engine_xr(engine_);
      both = xr_2;
    }
    if (both) {
      from = Vector{0.0, 0.0, 0.0};
      dir = Vector{0.0, 0.0, 0.0};
      both_2 = enum_equal<Mode>(mode_, Mode::foot);
      if (both_2) {
        n17825809 = vr_hand_ray(from, dir);
        both_2 = n17825809.Result;
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
    n17825810 = wanted_cpp::stars();
    s.stars = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::band(the_result, 255.0); }, kern::to<Float>(n17825810.Result)));
    s.weapon = enum_index<weapons_Id>(foot_weapon_);
    s.hands = 0;
    for (auto it17825811 = flow_for(n17825811, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(s.hand))))); it17825811.next();) {
      s.hand[n17825811.index] = 0;
    }
    if (logic_and(enum_equal<Mode>(mode_, Mode::foot), logic_not(player_dead_))) {
      sim_time = engine_game_world_sim_time(world_);
      tnow = sim_time;
      n17825812 = matrix_of(soldier_);
      a_3 = n17825812.Result;
      feet = vector_sub(engine_math_math::Mat4_translation(a_3).Result, Vector{0.0, 0.0, 0.73});
      both_3 = math_compare<"!=">(kern::to<Float>(engine_), 0.0);
      if (both_3) {
        xr_4 = engine_engine_xr(engine_);
        both_3 = xr_4;
      }
      if (logic_and(both_3, vr_)) {
        hm = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
        any = false;
        for (auto it17825813 = flow_for(n17825813, 0, 1); it17825813.next();) {
          n17825814 = vr_hand_matrix(hm, n17825813.index);
          if (n17825814.Result) {
            d = vector_sub(engine_math_math::Mat4_translation(hm).Result, feet);
            s.hand[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index * 3.0) + 0.0); }, kern::to<Float>(n17825813.index)))] = kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(a), 255.0), 128.0) - 128.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x) -> Float { return (x * 100.0); }, vector_break(d).x), -127.0, 127.0)));
            s.hand[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index * 3.0) + 1.0); }, kern::to<Float>(n17825813.index)))] = kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(a), 255.0), 128.0) - 128.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float y) -> Float { return (y * 100.0); }, vector_break(d).y), -127.0, 127.0)));
            s.hand[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index * 3.0) + 2.0); }, kern::to<Float>(n17825813.index)))] = kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::bxor(kern::math::band(kern::math::round(a), 255.0), 128.0) - 128.0); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float z) -> Float { return ((z * 50.0) - 127.0); }, vector_break(d).z), -127.0, 127.0)));
            any = true;
          }
        }
        if (any) {
          s.hands = kern::to<Int>(kern::formula([](Float hands) -> Float { return kern::math::band(kern::math::bor(hands, 1.0), 255.0); }, kern::to<Float>(s.hands)));
        }
      } else if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
        if (math_compare<"<">(kern::formula([](Float tnow, Float a) -> Float { return (tnow - a); }, tnow, gaps_[0].fist_last[0]), 0.22)) {
          s.hands = kern::to<Int>(kern::formula([](Float hands) -> Float { return kern::math::band(kern::math::bor(hands, 2.0), 255.0); }, kern::to<Float>(s.hands)));
        }
        if (math_compare<"<">(kern::formula([](Float tnow, Float a) -> Float { return (tnow - a); }, tnow, gaps_[0].fist_last[1]), 0.22)) {
          s.hands = kern::to<Int>(kern::formula([](Float hands) -> Float { return kern::math::band(kern::math::bor(hands, 4.0), 255.0); }, kern::to<Float>(s.hands)));
        }
        if (logic_and(enum_equal<weapons_Id>(foot_weapon_, weapons_Id::knife), math_compare<"<">(kern::formula([](Float tnow, Float vm_fire_at) -> Float { return (tnow - vm_fire_at); }, tnow, look_.vm_fire_at), 0.3))) {
          s.hands = kern::to<Int>(kern::formula([](Float hands) -> Float { return kern::math::band(kern::math::bor(hands, 8.0), 255.0); }, kern::to<Float>(s.hands)));
        }
      }
    }
    s.car = 255;
    if (logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<">=">(kern::to<Float>(car_index_), 0.0))) {
      if (net_session::Session_hosting(net_).Result) {
        s.car = kern::to<Int>(kern::formula([](Float car_index_) -> Float { return kern::math::band(kern::math::min(car_index_, 254.0), 255.0); }, kern::to<Float>(car_index_)));
      } else if (logic_and(logic_and(puppet_world_, math_compare<">">(kern::formula([](Float car_index_, Float a) -> Float { return ((car_index_ < 0.0) != (a < 0.0) ? (a < 0.0) : (car_index_ < a)); }, kern::to<Float>(car_index_), kern::to<Float>(list_length<Int>(car_net_))), 0.5)), math_compare<">=">(kern::to<Float>(car_net_[car_index_]), 0.0))) {
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
      n17825815 = state_of(drone_actor_);
      d_2 = n17825815.Result;
      s.drone_pos = d_2.position;
      s.drone_rot = d_2.rotation;
    }
    if (logic_and(jet_placed_, logic_not(enum_equal<Mode>(mode_, Mode::jet)))) {
      f_2 = kern::to<Int>(kern::formula([](Float f_2, Float a) -> Float { return kern::math::band(kern::math::bor(f_2, a), 65535.0); }, kern::to<Float>(f_2), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kJetOut)))));
      n17825816 = state_of(jet_actor_);
      j = n17825816.Result;
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
      n17825817 = gaps_ride_pose(rp, rq);
      if (n17825817.Result) {
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
    for (auto it17825818 = flow_for(n17825818, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825818.next();) {
      sample_rate = engine_mixer_sample_rate(mixer_);
      n17825819 = mp_rules_cpp::VoiceStream_render(net_.remotes_[n17825818.index].voice, out, frames, sample_rate);
    }
  }

  struct remote_on_segment_zonesResult { Bool Result = false; };
  remote_on_segment_zonesResult remote_on_segment_zones(Float& t_hit, mp_Zone& z, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float speed = 0.0) {
    BodyPart part = BodyPart::none;
    n17825820 = person_hit::hit(t_hit, feet, yaw, speed, from, seg, 1.16, 1.0);
    part = n17825820.Result;
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
    for (auto it17825821 = flow_for(n17825821, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825821.next();) {
      skip = false;
      if (math_compare<">">(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return (kern::math::band(a, kern::math::bor(a_2, a_3)) != 0.0); }, kern::to<Float>(net_.remotes_[n17825821.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead))), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_and(gaps_riding().Result, math_compare<"==">(kern::to<Float>(net_.remotes_[n17825821.index].state.slot), kern::to<Float>(gaps_[0].ride_owner)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (enum_equal<net_PlayerMode>(net_.remotes_[n17825821.index].state.mode, net_PlayerMode::foot)) {
            t = 2.0;
            z = mp_Zone::torso;
            feet = logic_select<Vector>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825821.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated)))), 0.5), vector_sub(net_.remotes_[n17825821.index].person_shown, vector_make(0.0, 0.0, kSeatDrop)), net_.remotes_[n17825821.index].person_shown);
            n17825822 = remote_on_segment_zones(t, z, from, seg, feet, net_.remotes_[n17825821.index].state.avatar_yaw, net_.remotes_[n17825821.index].state.speed);
            if (logic_and(n17825822.Result, math_compare<"<">(t, best_t))) {
              best_t = t;
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825821.index)));
              best_zone = z;
            }
            skip = true;
          }
          if (logic_not(skip)) {
            if (enum_equal<net_PlayerMode>(net_.remotes_[n17825821.index].state.mode, net_PlayerMode::drone)) {
              t_2 = 2.0;
              z_2 = mp_Zone::torso;
              n17825823 = remote_on_segment_zones(t_2, z_2, from, seg, net_.remotes_[n17825821.index].person_shown, net_.remotes_[n17825821.index].state.avatar_yaw, 0.0);
              if (logic_and(n17825823.Result, math_compare<"<">(t_2, best_t))) {
                best_t = t_2;
                best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825821.index)));
                best_zone = z_2;
              }
            }
            if (enum_equal<net_PlayerMode>(net_.remotes_[n17825821.index].state.mode, net_PlayerMode::parachute)) {
              t_3 = 2.0;
              z_3 = mp_Zone::torso;
              n17825824 = remote_on_segment_zones(t_3, z_3, from, seg, vector_sub(net_.remotes_[n17825821.index].shown, Vector{0.0, 0.0, 1.75}), net_.remotes_[n17825821.index].state.yaw, 0.0);
              if (logic_and(n17825824.Result, math_compare<"<">(t_3, best_t))) {
                best_t = t_3;
                best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825821.index)));
                best_zone = z_3;
              }
            }
            person = enum_equal<net_PlayerMode>(net_.remotes_[n17825821.index].state.mode, net_PlayerMode::foot);
            c = logic_select<Vector>(person, vector_add(net_.remotes_[n17825821.index].person_shown, vector_make(0.0, 0.0, kern::formula([](Float a, Float a_2, Float kSeatDrop) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? (0.73 - kSeatDrop) : 0.73); }, kern::to<Float>(net_.remotes_[n17825821.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated))), kSeatDrop))), net_.remotes_[n17825821.index].shown);
            radius = 0.522;
            switch (flow_switch_enum<net_PlayerMode>(net_.remotes_[n17825821.index].state.mode)) {
              case net_PlayerMode::foot: {
                c = vector_add(c, Vector{0.0, 0.0, 0.2});
              } break;
              case net_PlayerMode::drone: {
                radius = 0.3;
              } break;
              case net_PlayerMode::car: {
                radius = kern::formula([](Float a, Float a_2) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? 3.2 : 2.0); }, kern::to<Float>(net_.remotes_[n17825821.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kBus))));
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
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825821.index)));
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

  struct draw_remote_people_hpResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE draw_remote_people_hpResult draw_remote_people_hp(const People_Person& p, const net_Remote& net_remotes_n17825832_index, Int side = 0) {
    return {vector_add(p.pos, vector_make(kern::formula([](Float a) -> Float { return kern::math::div(a, 100.0); }, kern::to<Float>(net_remotes_n17825832_index.state.hand[kern::to<Int>(kern::formula([](Float side) -> Float { return (side * 3.0); }, kern::to<Float>(side)))])), kern::formula([](Float a) -> Float { return kern::math::div(a, 100.0); }, kern::to<Float>(net_remotes_n17825832_index.state.hand[kern::to<Int>(kern::formula([](Float side) -> Float { return ((side * 3.0) + 1.0); }, kern::to<Float>(side)))])), kern::formula([](Float a) -> Float { return kern::math::div((a + 127.0), 50.0); }, kern::to<Float>(net_remotes_n17825832_index.state.hand[kern::to<Int>(kern::formula([](Float side) -> Float { return ((side * 3.0) + 2.0); }, kern::to<Float>(side)))]))))};
  }

  void draw_remote_people() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float camera_yaw = 0.0;
    Vector chosen = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    Vector at = Vector{0.0, 0.0, 0.0};
    Float d = 0.0;
    Bool skip_2 = false;
    People_Person p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
    weapons_Id w = weapons_Id::smg;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    if (logic_and(enum_equal<Mode>(mode_, Mode::parachute), world_placed_)) {
      n17825825 = matrix_of(chute_actor_);
      a = n17825825.Result;
      camera_yaw = engine_game_world_camera_yaw(world_);
      if (net_session::Session_active(net_).Result) {
        n17825827 = net_session_cpp::Session_color_of(net_, net_session::Session_self_slot(net_).Result);
        chosen = mp_rules::rgb(n17825827.Result).Result;
      } else {
        chosen = Vector{0.3, 0.36, 0.28};
      }
      Game_draw_remote_people_pilot(now, vector_sub(engine_math_math::Mat4_translation(a).Result, Vector{0.0, 0.0, 0.85}), kern::formula([](Float camera_yaw, Float kDeg) -> Float { return (camera_yaw * kDeg); }, camera_yaw, kDeg), chosen);
    }
    if (logic_not(net_session::Session_active(net_).Result)) {
      return;
    }
    for (auto it17825828 = flow_for(n17825828, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825828.next();) {
      if (logic_and(logic_and(enum_equal<net_PlayerMode>(net_.remotes_[n17825828.index].state.mode, net_PlayerMode::parachute), logic_not(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825828.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead)))), 0.5))), logic_not(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825828.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)))) {
        n17825829 = net_session_cpp::Session_color_of(net_, net_.remotes_[n17825828.index].state.slot);
        Game_draw_remote_people_pilot(now, vector_sub(net_.remotes_[n17825828.index].shown, Vector{0.0, 0.0, 0.85}), net_.remotes_[n17825828.index].state.yaw, mp_rules::rgb(n17825829.Result).Result);
      }
    }
    for (auto it17825830 = flow_for(n17825830, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825830.next();) {
      skip = false;
      if (math_compare<">">(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return (kern::math::band(a, kern::math::bor(a_2, a_3)) != 0.0); }, kern::to<Float>(net_.remotes_[n17825830.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead))), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        at = logic_select<Vector>(logic_or(enum_equal<net_PlayerMode>(net_.remotes_[n17825830.index].state.mode, net_PlayerMode::foot), enum_equal<net_PlayerMode>(net_.remotes_[n17825830.index].state.mode, net_PlayerMode::drone)), net_.remotes_[n17825830.index].person_shown, net_.remotes_[n17825830.index].shown);
        d = vector_length(vector_sub(at, cam_.eye));
        if (math_compare<">">(d, 40.0)) {
          n17825831 = net_session_cpp::Session_color_of(net_, net_.remotes_[n17825830.index].state.slot);
          people_art_cpp::PeopleArt_beacon(people_art_, at, mp_rules::rgb(n17825831.Result).Result, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float d) -> Float { return kern::math::div((d - 40.0), 200.0); }, d), 0.0, 1.0)));
        }
      }
    }
    for (auto it17825832 = flow_for(n17825832, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825832.next();) {
      skip_2 = false;
      p = People_Person{People_Kind::citizen, People_State::walk, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 100.0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, -1, 0, 1, 0.0, 1.0, 4.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, -1, false, 0.0, -1, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, -99.0, 0.0, -99.0, false, 0, 0.0, -99.0, false, -1, Vector{0.0, 0.0, 0.0}, 0.0};
      p.kind = People_Kind::citizen;
      n17825833 = net_session_cpp::Session_color_of(net_, net_.remotes_[n17825832.index].state.slot);
      p.shirt = mp_rules::rgb(n17825833.Result).Result;
      p.pants = Vector{0.12, 0.13, 0.16};
      p.skin = Vector{0.86, 0.66, 0.5};
      p.phase = net_.remotes_[n17825832.index].phase;
      p.yaw = net_.remotes_[n17825832.index].state.avatar_yaw;
      if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825832.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825832.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead)))), 0.5)) {
          p.state = People_State::down;
          p.fall_dir = vector_make(kern::formula([](Float a) -> Float { return kern::math::cos(a); }, net_.remotes_[n17825832.index].state.avatar_yaw), kern::formula([](Float a) -> Float { return kern::math::sin(a); }, net_.remotes_[n17825832.index].state.avatar_yaw), 0.0);
        }
        if (enum_equal<net_PlayerMode>(net_.remotes_[n17825832.index].state.mode, net_PlayerMode::foot)) {
          p.pos = net_.remotes_[n17825832.index].person_shown;
          p.v = net_.remotes_[n17825832.index].state.speed;
          if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825832.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated)))), 0.5)) {
            p.pos = vector_make(vector_break(p.pos).x, vector_break(p.pos).y, kern::formula([](Float z, Float kSeatDrop) -> Float { return (z - kSeatDrop); }, vector_break(p.pos).z, kSeatDrop));
            p.v = 0.0;
          }
          w = enum_from_index<weapons_Id>(net_.remotes_[n17825832.index].state.weapon).value;
          p.held = net_.remotes_[n17825832.index].state.weapon;
          p.aiming = logic_and(logic_not(enum_equal<weapons_Id>(w, weapons_Id::fists)), logic_not(enum_equal<weapons_Id>(w, weapons_Id::knife)));
          p.aim_pitch = net_.remotes_[n17825832.index].state.pitch;
          p.yaw = net_.remotes_[n17825832.index].state.yaw;
          p.hands = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(a, 1.0); }, kern::to<Float>(net_.remotes_[n17825832.index].state.hands))))), 0.0);
          if (p.hands) {
            p.hand_l = draw_remote_people_hp(p, net_.remotes_[n17825832.index], 0).Result;
            p.hand_r = draw_remote_people_hp(p, net_.remotes_[n17825832.index], 1).Result;
            p.aiming = false;
          }
          p.punch = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(kern::math::band(kern::math::shr(a, 1.0), 7.0), 255.0); }, kern::to<Float>(net_.remotes_[n17825832.index].state.hands)));
          if (math_compare<"!=">(kern::to<Float>(p.punch), 0.0)) {
            p.aiming = false;
          }
          people_art_cpp::PeopleArt_person(people_art_, p, now);
        } else if (enum_equal<net_PlayerMode>(net_.remotes_[n17825832.index].state.mode, net_PlayerMode::drone)) {
          p.pos = net_.remotes_[n17825832.index].person_shown;
          p.v = 0.0;
          p.aiming = true;
          people_art_cpp::PeopleArt_person(people_art_, p, now);
        }
      }
    }
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
    n17825834 = focus_position();
    p3 = n17825834.Result;
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
    n17825835 = road_graph_cpp::RoadGraph_path(roads_, path, me, mp_[0].dest);
    if (logic_and(logic_and(n17825835.Result, math_compare<">=">(kern::to<Float>(list_length<Vector>(path)), 2.0)), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(path[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(path))))]).x, vector_break(mp_[0].dest).x, vector_break(path[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(path))))]).y, vector_break(mp_[0].dest).y), 60.0))) {
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
    n17825836 = project_screen(nx, ny, depth, p);
    on = n17825836.Result;
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
    n17825837 = mp_rules_cpp::safe_mobile_marker_point(vector_make(kern::formula([](Float w, Float x) -> Float { return (kern::math::div(w, 2.0) + x); }, w, x), kern::formula([](Float h, Float y) -> Float { return (kern::math::div(h, 2.0) + y); }, h, y), 0.0), w, h, touch_mode_);
    q = n17825837.Result;
    sx = vector_break(q).x;
    sy = vector_break(q).y;
    angle = kern::formula([](Float y, Float x, Float kDeg) -> Float { return (kern::math::div(kern::math::atan2(y, x), kDeg) - 90.0); }, y, x, kDeg);
  }

  struct mp_hud_jsonResult { Text Result = ""; };
  mp_hud_jsonResult mp_hud_json() {
    Float now = 0.0;
    Text s = "";
    Text b = "";
    Float w = 0.0;
    Float h = 0.0;
    Bool first = false;
    Float d = 0.0;
    Float sc = 0.0;
    Float nx = 0.0;
    Float ny = 0.0;
    Float depth = 0.0;
    Bool on = false;
    mp_Ping a = mp_Ping{0, Vector{0.0, 0.0, 0.0}, 0, 0.0};
    Vector p3 = Vector{0.0, 0.0, 0.0};
    Vector me = Vector{0.0, 0.0, 0.0};
    Float left = 0.0;
    Float chosen = 0.0;
    List<mp_Crumb> crumbs = List<mp_Crumb>{};
    Vector ahead = Vector{0.0, 0.0, 0.0};
    Float bearing = 0.0;
    Vector look = Vector{0.0, 0.0, 0.0};
    Float rel = 0.0;
    Float cx = 0.0;
    Float cy = 0.0;
    Float ca = 0.0;
    Bool on_2 = false;
    mp_Turn t = mp_Turn{mp_Turn_Dir::none, 0.0, 0.0};
    Text turn = "";
    Int self = 0;
    Text a_2 = "";
    Vector look_2 = Vector{0.0, 0.0, 0.0};
    Float ang = 0.0;
    Vector mine = Vector{0.0, 0.0, 0.0};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector chosen_2 = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    Vector anchor = Vector{0.0, 0.0, 0.0};
    Float x = 0.0;
    Float y = 0.0;
    Float ang_2 = 0.0;
    Bool on_3 = false;
    Float d_2 = 0.0;
    Vector his = Vector{0.0, 0.0, 0.0};
    Float dz = 0.0;
    Vector look_3 = Vector{0.0, 0.0, 0.0};
    Float rel_2 = 0.0;
    List<mp_Score> list = List<mp_Score>{};
    Int it = 0;
    net_Kill k = net_Kill{0, 254, 0, 0};
    Bool accident = false;
    Text chosen_3 = "";
    game_Hud hud = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
    mp_local_ready();
    now = kern::formula([](Float time_) -> Float { return (time_ * 1000.0); }, time_);
    s = "";
    b = "";
    w = frame_w_;
    h = frame_h_;
    s = text_append(s, ",\"mp\":{", "");
    b = text_format<"\"frames\":%u,\"mapOpen\":%s">(511, mp_[0].frames, logic_select<Text>(mp_[0].map_open, "true", "false"));
    s = text_append(s, b, "");
    b = text_format<",\"voice\":{\"mic\":%s,\"muted\":%s,\"all\":%s,\"tx\":%u}">(511, logic_select<Text>(mp_[0].mic, "true", "false"), logic_select<Text>(mp_[0].muted, "true", "false"), logic_select<Text>(mp_[0].everywhere, "true", "false"), mp_[0].seq);
    s = text_append(s, b, "");
    s = text_append(s, ",\"pings\":[", "");
    first = true;
    for (auto it17825838 = flow_for(n17825838, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Ping>(mp_[0].pings.pings_))))); it17825838.next();) {
      d = vector_length(vector_sub(cam_.eye, mp_[0].pings.pings_[n17825838.index].p));
      sc = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float d) -> Float { return kern::math::div(d, 22.0); }, d), 1.0, 40.0));
      nx = 0.0;
      ny = 0.0;
      depth = 0.0;
      n17825839 = project_screen(nx, ny, depth, vector_add(mp_[0].pings.pings_[n17825838.index].p, vector_make(0.0, 0.0, kern::formula([](Float sc) -> Float { return (3.4 * sc); }, sc))));
      on = n17825839.Result;
      a = mp_[0].pings.pings_[n17825838.index];
      n17825840 = mp_rules_cpp::PingBoard_fade(a, now);
      b = text_format<"%s{\"on\":%s,\"x\":%.0f,\"y\":%.0f,\"d\":%.0f,\"color\":\"%s\",\"fade\":%.2f,\"mine\":%s}">(511, logic_select<Text>(first, "", ","), logic_select<Text>(on, "true", "false"), kern::formula([](Float nx, Float w) -> Float { return (kern::math::div((nx + 1.0), 2.0) * w); }, nx, w), kern::formula([](Float ny, Float h) -> Float { return (kern::math::div((1.0 - ny), 2.0) * h); }, ny, h), d, game_net_cpp::hex(mp_[0].pings.pings_[n17825838.index].color).Result, n17825840.Result, logic_select<Text>(math_compare<"==">(kern::to<Float>(mp_[0].pings.pings_[n17825838.index].owner), kern::to<Float>(net_session::Session_self_slot(net_).Result)), "true", "false"));
      s = text_append(s, b, "");
      first = false;
    }
    s = text_append(s, "]", "");
    if (mp_[0].has_dest) {
      n17825841 = focus_position();
      p3 = n17825841.Result;
      me = vector_make(vector_break(p3).x, vector_break(p3).y, 0.0);
      if (math_compare<">=">(kern::to<Float>(list_length<Vector>(mp_[0].route)), 2.0)) {
        n17825843 = mp_rules_cpp::route_left(mp_[0].route, me);
        chosen = n17825843.Result;
      } else {
        chosen = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(mp_[0].dest).x, vector_break(me).x, vector_break(mp_[0].dest).y, vector_break(me).y);
      }
      left = kern::formula([](Float chosen) -> Float { return chosen; }, chosen);
      n17825844 = mp_rules_cpp::sample_breadcrumbs(mp_[0].route, me, 3, 18.0, 14.0);
      crumbs = n17825844.Result;
      ahead = logic_select<Vector>(math_compare<"==">(kern::to<Float>(list_length<mp_Crumb>(crumbs)), 0.0), mp_[0].dest, crumbs[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Crumb>(crumbs))))].p);
      bearing = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::atan2((x - x_2), (y - y_2)); }, vector_break(ahead).x, vector_break(me).x, vector_break(ahead).y, vector_break(me).y);
      look = vector_sub(cam_.target, cam_.eye);
      rel = common::wrap_pi(kern::formula([](Float bearing, Float x, Float y) -> Float { return (bearing - kern::math::atan2(x, y)); }, bearing, vector_break(look).x, vector_break(look).y)).Result;
      cx = 0.0;
      cy = 0.0;
      ca = 0.0;
      on_2 = false;
      Game_mp_hud_json_marker(cx, cy, on_2, ca, w, h, vector_make(vector_break(ahead).x, vector_break(ahead).y, kern::formula([](Float z) -> Float { return (z + 1.2); }, vector_break(p3).z)));
      n17825845 = mp_rules_cpp::upcoming_turn(mp_[0].route, me, 250.0, 12.0, 38.0);
      t = n17825845.Result;
      turn = "";
      if (logic_not(enum_equal<mp_Turn_Dir>(t.dir, mp_Turn_Dir::none))) {
        turn = text_append(text_append(logic_select<Text>(enum_equal<mp_Turn_Dir>(t.dir, mp_Turn_Dir::left), "links", logic_select<Text>(enum_equal<mp_Turn_Dir>(t.dir, mp_Turn_Dir::right), "rechts", "wenden")), " in ", ""), mp_rules_cpp::format_distance(t.distance).Result, "");
      }
      n17825846 = mp_rules_cpp::bearing_arrow(rel);
      b = text_format<",\"nav\":{\"name\":\"%s\",\"dist\":\"%s\",\"arrow\":\"%s\",\"turn\":\"%s\",\"x\":%.0f,\"y\":%.0f,\"on\":%s,\"angle\":%.0f,\"streets\":%s}">(511, game_net_cpp::esc(mp_[0].dest_name).Result, mp_rules_cpp::format_distance(left).Result, n17825846.Result, turn, cx, cy, logic_select<Text>(on_2, "true", "false"), ca, logic_select<Text>(mp_[0].route_streets, "true", "false"));
      s = text_append(s, b, "");
    }
    if (logic_not(math_compare<"==">(kern::to<Float>(text_length(mp_[0].fetch_json)), 0.0))) {
      s = text_append(s, text_append(",\"navFetch\":", mp_[0].fetch_json, ""), "");
    }
    if (net_session::Session_active(net_).Result) {
      self = net_session::Session_self_slot(net_).Result;
      n17825847 = net_session_cpp::Session_color_of(net_, self);
      a_2 = net_session_cpp::Session_status(net_).Result;
      b = text_format<",\"self\":%u,\"index\":%u,\"color\":\"%s\",\"host\":%s,\"connected\":%s,\"ff\":%s,\"vote\":\"%s\",\"votePending\":%s,\"status\":\"%s\",\"poses\":%u">(511, self, net_session_cpp::Session_index_of(net_, self).Result, game_net_cpp::hex(n17825847.Result).Result, logic_select<Text>(net_session::Session_hosting(net_).Result, "true", "false"), logic_select<Text>(net_session_cpp::Session_connected(net_).Result, "true", "false"), logic_select<Text>(net_session::Session_friendly_fire(net_).Result, "true", "false"), game_net_cpp::esc(net_.vote_text_).Result, logic_select<Text>(net_session::Session_vote_pending(net_).Result, "true", "false"), game_net_cpp::esc(a_2).Result, net_session::Session_sent_poses(net_).Result);
      s = text_append(s, b, "");
      b = text_format<",\"hit\":%.0f,\"hitKill\":%s,\"hitHead\":%s,\"hitShow\":%s">(511, kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, mp_[0].hit_at), logic_select<Text>(mp_[0].hit_kill, "true", "false"), logic_select<Text>(mp_[0].hit_head, "true", "false"), logic_select<Text>(math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, mp_[0].hit_at), kern::formula([](Float a, Float kKillMarkerMs, Float kHitMarkerMs) -> Float { return ((a != 0.0) ? kKillMarkerMs : kHitMarkerMs); }, kern::to<Float>(mp_[0].hit_kill), kKillMarkerMs, kHitMarkerMs)), "true", "false"));
      s = text_append(s, b, "");
      look_2 = vector_sub(cam_.target, cam_.eye);
      ang = kern::formula([](Float y, Float x, Float y_2, Float y_3, Float x_2, Float x_3, Float kDeg) -> Float { return kern::math::div((kern::math::atan2(y, x) - kern::math::atan2((y_2 - y_3), (x_2 - x_3))), kDeg); }, vector_break(look_2).y, vector_break(look_2).x, vector_break(mp_[0].hurt_from).y, vector_break(cam_.eye).y, vector_break(mp_[0].hurt_from).x, vector_break(cam_.eye).x, kDeg);
      b = text_format<",\"hurt\":%s,\"hurtAngle\":%.0f">(511, logic_select<Text>(math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, mp_[0].hurt_at), kHurtMs), "true", "false"), ang);
      s = text_append(s, b, "");
      b = text_format<",\"shield\":%s">(511, logic_select<Text>(net_session_cpp::Session_protected_now(net_, self, now).Result, "true", "false"));
      s = text_append(s, b, "");
      s = text_append(s, ",\"players\":[", "");
      first = true;
      if (enum_equal<Mode>(mode_, Mode::foot)) {
        n17825849 = matrix_of(soldier_);
        a_4 = n17825849.Result;
        chosen_2 = vector_sub(engine_math_math::Mat4_translation(a_4).Result, Vector{0.0, 0.0, 0.73});
      } else {
        n17825850 = focus_position();
        chosen_2 = n17825850.Result;
      }
      mine = chosen_2;
      for (auto it17825851 = flow_for(n17825851, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825851.next();) {
        skip = false;
        if (math_compare<">">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, net_.remotes_[n17825851.index].received_ms), kStaleHoldMs)) {
          skip = true;
        }
        if (logic_not(skip)) {
          anchor = logic_select<Vector>(enum_equal<net_PlayerMode>(net_.remotes_[n17825851.index].state.mode, net_PlayerMode::foot), vector_add(net_.remotes_[n17825851.index].person_shown, Vector{0.0, 0.0, 2.05}), vector_add(net_.remotes_[n17825851.index].shown, Vector{0.0, 0.0, 0.45}));
          x = 0.0;
          y = 0.0;
          ang_2 = 0.0;
          on_3 = false;
          Game_mp_hud_json_marker(x, y, on_3, ang_2, w, h, anchor);
          d_2 = vector_length(vector_sub(anchor, cam_.eye));
          his = logic_select<Vector>(enum_equal<net_PlayerMode>(net_.remotes_[n17825851.index].state.mode, net_PlayerMode::foot), net_.remotes_[n17825851.index].person_shown, net_.remotes_[n17825851.index].shown);
          dz = kern::formula([](Float z, Float z_2) -> Float { return (z - z_2); }, vector_break(his).z, vector_break(mine).z);
          look_3 = vector_sub(cam_.target, cam_.eye);
          rel_2 = kern::formula([](Float the_result, Float kDeg) -> Float { return kern::math::div(the_result, kDeg); }, common::wrap_pi(kern::formula([](Float x, Float x_2, Float y, Float y_2, Float x_3, Float y_3) -> Float { return (kern::math::atan2((x - x_2), (y - y_2)) - kern::math::atan2(x_3, y_3)); }, vector_break(his).x, vector_break(cam_.eye).x, vector_break(his).y, vector_break(cam_.eye).y, vector_break(look_3).x, vector_break(look_3).y)).Result, kDeg);
          n17825852 = net_session_cpp::Session_color_of(net_, net_.remotes_[n17825851.index].state.slot);
          b = text_format<"%s{\"slot\":%u,\"index\":%u,\"color\":\"%s\",\"hp\":%.0f,\"dead\":%s,\"mode\":\"%s\",\"x\":%.0f,\"y\":%.0f,\"on\":%s,\"angle\":%.0f,\"d\":%.1f,\"dz\":%.1f,\"rel\":%.0f,\"talk\":%s,\"protected\":%s,\"wx\":%.1f,\"wy\":%.1f,\"stars\":%u}">(511, logic_select<Text>(first, "", ","), net_.remotes_[n17825851.index].state.slot, net_session_cpp::Session_index_of(net_, net_.remotes_[n17825851.index].state.slot).Result, game_net_cpp::hex(n17825852.Result).Result, net_.remotes_[n17825851.index].shown_hp, logic_select<Text>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825851.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead)))), 0.5), "true", "false"), game_cpp::mode_name(enum_from_index<Mode>(enum_index<net_PlayerMode>(net_.remotes_[n17825851.index].state.mode)).value).Result, x, y, logic_select<Text>(on_3, "true", "false"), ang_2, d_2, dz, rel_2, logic_select<Text>(math_compare<"<">(now, net_.remotes_[n17825851.index].speaking_until), "true", "false"), logic_select<Text>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825851.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kProtected)))), 0.5), "true", "false"), vector_break(net_.remotes_[n17825851.index].shown).x, vector_break(net_.remotes_[n17825851.index].shown).y, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::min(a, 5.0); }, kern::to<Float>(net_.remotes_[n17825851.index].state.stars))));
          s = text_append(s, b, "");
          first = false;
        }
      }
      s = text_append(s, "],\"scores\":[", "");
      first = true;
      list = mp_rules_cpp::Scoreboard_ranked(net_.scores_).Result;
      for (auto it17825853 = flow_for(n17825853, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Score>(list))))); it17825853.next();) {
        n17825854 = net_session_cpp::Session_color_of(net_, list[n17825853.index].slot);
        b = text_format<"%s{\"index\":%u,\"color\":\"%s\",\"self\":%s,\"k\":%u,\"d\":%u}">(511, logic_select<Text>(first, "", ","), net_session_cpp::Session_index_of(net_, list[n17825853.index].slot).Result, game_net_cpp::hex(n17825854.Result).Result, logic_select<Text>(math_compare<"==">(kern::to<Float>(list[n17825853.index].slot), kern::to<Float>(self)), "true", "false"), list[n17825853.index].kills, list[n17825853.index].deaths);
        s = text_append(s, b, "");
        first = false;
      }
      s = text_append(s, "],\"feed\":[", "");
      first = true;
      it = list_length<MpLocal_Feed>(mp_[0].feed);
      while (loop(logic_not(math_compare<"==">(kern::to<Float>(it), 0.0)))) {
        k = mp_[0].feed[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(it)))].k;
        accident = logic_or(math_compare<"==">(kern::to<Float>(k.by), kern::to<Float>(kNobody)), math_compare<"==">(kern::to<Float>(k.by), kern::to<Float>(k.victim)));
        if (accident) {
          chosen_3 = "#ffffff";
        } else {
          n17825856 = net_session_cpp::Session_color_of(net_, k.by);
          chosen_3 = game_net_cpp::hex(n17825856.Result).Result;
        }
        n17825857 = net_session_cpp::Session_color_of(net_, k.victim);
        n17825858 = mp_rules_cpp::weapon_label(enum_from_index<mp_Weapon>(k.weapon).value);
        b = text_format<"%s{\"by\":%u,\"byColor\":\"%s\",\"bySelf\":%s,\"victim\":%u,\"victimColor\":\"%s\",\"victimSelf\":%s,\"weapon\":\"%s\",\"head\":%s,\"accident\":%s,\"age\":%.0f}">(511, logic_select<Text>(first, "", ","), kern::to<Int>(kern::formula([](Float accident, Float a) -> Float { return ((accident != 0.0) ? 0.0 : a); }, kern::to<Float>(accident), kern::to<Float>(net_session_cpp::Session_index_of(net_, k.by).Result))), chosen_3, logic_select<Text>(math_compare<"==">(kern::to<Float>(k.by), kern::to<Float>(self)), "true", "false"), net_session_cpp::Session_index_of(net_, k.victim).Result, game_net_cpp::hex(n17825857.Result).Result, logic_select<Text>(math_compare<"==">(kern::to<Float>(k.victim), kern::to<Float>(self)), "true", "false"), n17825858.Result, logic_select<Text>(math_compare<"==">(kern::to<Float>(k.zone), kern::to<Float>(enum_index<mp_Zone>(mp_Zone::head))), "true", "false"), logic_select<Text>(accident, "true", "false"), kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, mp_[0].feed[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(it)))].at));
        s = text_append(s, b, "");
        first = false;
        it = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(it)));
      }
      s = text_append(s, "]", "");
      if (player_dead_) {
        hud = engine_game_world_hud(world_);
        b = text_format<",\"respawnIn\":%.1f">(511, kern::formula([](Float respawn_in) -> Float { return kern::math::max(0.0, respawn_in); }, struct_break<game_Hud>(hud).respawn_in));
        s = text_append(s, b, "");
      }
    }
    return {text_append(s, "}", "")};
  }

  struct bigmap_jsonResult { Text Result = ""; };
  bigmap_jsonResult bigmap_json(Float cx = 0.0, Float cy = 0.0, Float range = 0.0) {
    Text o = "";
    Text b = "";
    Vector me = Vector{0.0, 0.0, 0.0};
    Vector look = Vector{0.0, 0.0, 0.0};
    Float lon = 0.0;
    Float lat = 0.0;
    Int mi = 0;
    Bool first = false;
    Bool skip = false;
    Bool near = false;
    Int at = 0;
    Int count = 0;
    Bool stop_2 = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    Vector np = Vector{0.0, 0.0, 0.0};
    Float heading = 0.0;
    Vector j = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    mp_local_ready();
    o = "";
    b = "";
    n17825859 = focus_position();
    me = n17825859.Result;
    look = vector_sub(cam_.target, cam_.eye);
    lon = 0.0;
    lat = 0.0;
    tiles_cpp::unproject(city_.proj_, lon, lat, vector_break(me).x, vector_break(me).y);
    mi = enum_index<Mode>(mode_);
    b = text_format<"{\"me\":[%.1f,%.1f,%.3f],\"lon\":%.6f,\"lat\":%.6f,\"lon0\":%.8f,\"lat0\":%.8f,\"mode\":\"%s\",\"agl\":%.0f,\"real\":%s,\"lanes\":[">(255, vector_break(me).x, vector_break(me).y, kern::formula([](Float x, Float y) -> Float { return kern::math::atan2(x, y); }, vector_break(look).x, vector_break(look).y), lon, lat, city_.proj_.lon0, city_.proj_.lat0, logic_select<Text>(math_compare<"<">(kern::to<Float>(mi), 5.0), list_get<Text>(kModes, mi), "foot"), kern::formula([](Float z, Float the_result) -> Float { return (z - the_result); }, vector_break(me).z, city_stream_cpp::CityStream_ground_height(city_, vector_break(me).x, vector_break(me).y).Result), logic_select<Text>(city_stream::CityStream_has_real_data(city_).Result, "true", "false"));
    o = text_append(o, b, "");
    first = true;
    for (auto it17825860 = flow_for(n17825860, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it17825860.next();) {
      skip = false;
      if (logic_not(city_.lanes_[n17825860.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        near = false;
        count = list_length<Vector>(city_.lanes_[n17825860.index].points);
        at = 0;
        stop_2 = false;
        while (loop(logic_and(logic_not(stop_2), math_compare<"<">(kern::to<Float>(at), kern::to<Float>(count))))) {
          if (logic_and(math_compare<"<">(kern::formula([](Float x, Float cx) -> Float { return kern::math::abs((x - cx)); }, vector_break(city_.lanes_[n17825860.index].points[at]).x, cx), range), math_compare<"<">(kern::formula([](Float y, Float cy) -> Float { return kern::math::abs((y - cy)); }, vector_break(city_.lanes_[n17825860.index].points[at]).y, cy), range))) {
            near = true;
            stop_2 = true;
          }
          if (logic_not(stop_2)) {
            at += 1;
          }
        }
        if (logic_not(near)) {
          skip = true;
        }
        if (logic_not(skip)) {
          o = text_append(o, logic_select<Text>(first, "[", ",["), "");
          first = false;
          for (auto it17825861 = flow_for(n17825861, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(city_.lanes_[n17825860.index].points))))); it17825861.next();) {
            b = text_format<"%s%.0f,%.0f">(255, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n17825861.index), 0.0), ",", ""), vector_break(city_.lanes_[n17825860.index].points[n17825861.index]).x, vector_break(city_.lanes_[n17825860.index].points[n17825861.index]).y);
            o = text_append(o, b, "");
          }
          o = text_append(o, "]", "");
        }
      }
    }
    o = text_append(o, "],\"players\":[", "");
    first = true;
    if (net_session::Session_active(net_).Result) {
      for (auto it17825862 = flow_for(n17825862, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825862.next();) {
        p = logic_select<Vector>(logic_or(enum_equal<net_PlayerMode>(net_.remotes_[n17825862.index].state.mode, net_PlayerMode::foot), enum_equal<net_PlayerMode>(net_.remotes_[n17825862.index].state.mode, net_PlayerMode::drone)), net_.remotes_[n17825862.index].person_shown, net_.remotes_[n17825862.index].shown);
        np = p;
        heading = net_.remotes_[n17825862.index].state.avatar_yaw;
        n17825863 = mp_rules_cpp::NavTimeline_sample(net_.remotes_[n17825862.index].nav, np, heading, kern::formula([](Float time_) -> Float { return (time_ * 1000.0); }, time_));
        n17825864 = net_session_cpp::Session_color_of(net_, net_.remotes_[n17825862.index].state.slot);
        b = text_format<"%s{\"index\":%u,\"color\":\"%s\",\"x\":%.1f,\"y\":%.1f,\"mode\":\"%s\",\"dead\":%s}">(255, logic_select<Text>(first, "", ","), net_session_cpp::Session_index_of(net_, net_.remotes_[n17825862.index].state.slot).Result, game_net_cpp::hex(n17825864.Result).Result, vector_break(np).x, vector_break(np).y, game_cpp::mode_name(enum_from_index<Mode>(enum_index<net_PlayerMode>(net_.remotes_[n17825862.index].state.mode)).value).Result, logic_select<Text>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825862.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead)))), 0.5), "true", "false"));
        o = text_append(o, b, "");
        first = false;
      }
    }
    o = text_append(o, "],\"pings\":[", "");
    first = true;
    for (auto it17825865 = flow_for(n17825865, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<mp_Ping>(mp_[0].pings.pings_))))); it17825865.next();) {
      b = text_format<"%s{\"x\":%.1f,\"y\":%.1f,\"color\":\"%s\"}">(255, logic_select<Text>(first, "", ","), vector_break(mp_[0].pings.pings_[n17825865.index].p).x, vector_break(mp_[0].pings.pings_[n17825865.index].p).y, game_net_cpp::hex(mp_[0].pings.pings_[n17825865.index].color).Result);
      o = text_append(o, b, "");
      first = false;
    }
    o = text_append(o, "]", "");
    if (mp_[0].has_dest) {
      b = text_format<",\"dest\":{\"x\":%.1f,\"y\":%.1f,\"name\":\"%s\"},\"route\":[">(255, vector_break(mp_[0].dest).x, vector_break(mp_[0].dest).y, game_net_cpp::esc(mp_[0].dest_name).Result);
      o = text_append(o, b, "");
      for (auto it17825866 = flow_for(n17825866, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(mp_[0].route))))); it17825866.next();) {
        b = text_format<"%s%.0f,%.0f">(255, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n17825866.index), 0.0), ",", ""), vector_break(mp_[0].route[n17825866.index]).x, vector_break(mp_[0].route[n17825866.index]).y);
        o = text_append(o, b, "");
      }
      o = text_append(o, "]", "");
    }
    if (logic_and(jet_placed_, logic_not(enum_equal<Mode>(mode_, Mode::jet)))) {
      n17825867 = matrix_of(jet_actor_);
      a = n17825867.Result;
      j = engine_math_math::Mat4_translation(a).Result;
      b = text_format<",\"jet\":[%.0f,%.0f]">(255, vector_break(j).x, vector_break(j).y);
      o = text_append(o, b, "");
    }
    return {text_append(o, "}", "")};
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

  struct gaps_boardableResult { Int Result = 0; };
  /// nearestRemoteBoardable: a mate's landed jet or his car standing (nearly) still, he at the wheel (his pilot seat in the table), within reach - -1 for none. The hint, the touch button and the press all ask this one.
  gaps_boardableResult gaps_boardable(Float& dist) {
    Vector me = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int best = 0;
    Float bd = 0.0;
    Bool skip = false;
    Bool jet = false;
    Bool car = false;
    Float d = 0.0;
    if (logic_or(logic_or(logic_or(logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)), logic_not(net_session::Session_active(net_).Result)), logic_not(enum_equal<Mode>(mode_, Mode::foot))), player_dead_), gaps_[0].riding)) {
      return {-1};
    }
    n19922954 = matrix_of(soldier_);
    a = n19922954.Result;
    me = engine_math_math::Mat4_translation(a).Result;
    best = -1;
    bd = kBoardM;
    for (auto it19922955 = flow_for(n19922955, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it19922955.next();) {
      skip = false;
      if (math_compare<">">(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return (kern::math::band(a, kern::math::bor(a_2, a_3)) != 0.0); }, kern::to<Float>(net_.remotes_[n19922955.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead))), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        jet = logic_and(logic_and(enum_equal<net_PlayerMode>(net_.remotes_[n19922955.index].state.mode, net_PlayerMode::jet), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n19922955.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kGearDown)))), 0.5)), math_compare<"<=">(net_.remotes_[n19922955.index].state.speed, 4.0));
        car = logic_and(enum_equal<net_PlayerMode>(net_.remotes_[n19922955.index].state.mode, net_PlayerMode::car), math_compare<"<=">(net_.remotes_[n19922955.index].state.speed, 3.0));
        if (logic_and(logic_not(jet), logic_not(car))) {
          skip = true;
        }
        if (logic_not(skip)) {
          n19922956 = jet_seats_cpp::Table_get(net_.seats_, net_.remotes_[n19922955.index].state.slot);
          if (math_compare<"!=">(kern::to<Float>(struct_break<seats_Row>(n19922956.Result).pilot), kern::to<Float>(net_.remotes_[n19922955.index].state.slot))) {
            skip = true;
          }
          if (logic_not(skip)) {
            d = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(net_.remotes_[n19922955.index].shown).x, vector_break(me).x, vector_break(net_.remotes_[n19922955.index].shown).y, vector_break(me).y);
            if (logic_and(math_compare<"<">(d, kern::formula([](Float car, Float kBoardM) -> Float { return ((car != 0.0) ? 5.5 : kBoardM); }, kern::to<Float>(car), kBoardM)), math_compare<"<">(d, bd))) {
              bd = d;
              best = net_.remotes_[n19922955.index].state.slot;
            }
          }
        }
      }
    }
    dist = bd;
    return {best};
  }

  struct gaps_boardable_2Result { Int Result = 0; };
  gaps_boardable_2Result gaps_boardable_2() {
    Float dist = 0.0;
    dist = 0.0;
    n19922957 = gaps_boardable(dist);
    return {n19922957.Result};
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
    n19922958 = gaps_ride_pose(p, q);
    if (logic_not(n19922958.Result)) {
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
    swarm_serial_ = kern::to<Int>(kern::formula([](Float swarm_serial_) -> Float { return kern::math::band((swarm_serial_ + 1.0), 65535.0); }, kern::to<Float>(swarm_serial_)));
    if (math_compare<"==">(kern::to<Float>(swarm_serial_), 0.0)) {
      swarm_serial_ = kern::to<Int>(kern::formula([](Float swarm_serial_) -> Float { return kern::math::band((swarm_serial_ + 1.0), 65535.0); }, kern::to<Float>(swarm_serial_)));
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
            u.flags = kern::to<Int>(kern::formula([](Float flags, Float a) -> Float { return kern::math::band(kern::math::bor(flags, a), 255.0); }, kern::to<Float>(u.flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kUnitDead)))));
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
    for (auto it20971558 = flow_for(n20971558, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Game_RemoteSwarm>(swarms_))))); it20971558.next();) {
      for (auto it20971559 = flow_for(n20971559, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_RemoteUnit>(swarms_[n20971558.index].units))))); it20971559.next();) {
        if (enum_equal<net_SwarmKind>(swarms_[n20971558.index].units[n20971559.index].kind, net_SwarmKind::heli)) {
          a_2 = engine_math_math::Mat4_translate(swarms_[n20971558.index].units[n20971559.index].p).Result;
          a_3 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(swarms_[n20971558.index].units[n20971559.index].yaw).Result, Vector{1.0, 1.0, 1.0}).Result;
          a = engine_math_math::Mat4_mul(a_2, a_3).Result;
          combat_art_cpp::CombatArt_heli(combat_art_, a, swarms_[n20971558.index].units[n20971559.index].rotor, math_compare<"!=">(kern::to<Float>(swarms_[n20971558.index].units[n20971559.index].flags), 0.0));
        } else if (enum_equal<net_SwarmKind>(swarms_[n20971558.index].units[n20971559.index].kind, net_SwarmKind::tank)) {
          a_5 = engine_math_math::Mat4_translate(vector_make(vector_break(swarms_[n20971558.index].units[n20971559.index].p).x, vector_break(swarms_[n20971558.index].units[n20971559.index].p).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(swarms_[n20971558.index].units[n20971559.index].p).x, vector_break(swarms_[n20971558.index].units[n20971559.index].p).y).Result)).Result;
          a_6 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(swarms_[n20971558.index].units[n20971559.index].yaw).Result, Vector{1.0, 1.0, 1.0}).Result;
          a_4 = engine_math_math::Mat4_mul(a_5, a_6).Result;
          combat_art_cpp::CombatArt_tank(combat_art_, a_4, swarms_[n20971558.index].units[n20971559.index].aux, math_compare<"!=">(kern::to<Float>(swarms_[n20971558.index].units[n20971559.index].flags), 0.0));
        } else if (logic_and(enum_equal<net_SwarmKind>(swarms_[n20971558.index].units[n20971559.index].kind, net_SwarmKind::enemy), math_compare<">=">(kern::to<Float>(swarms_[n20971558.index].units[n20971559.index].person), 0.0))) {
          type = enum_from_index<combat_EnemyType>(people_.people_[swarms_[n20971558.index].units[n20971559.index].person].role).value;
          a_8 = engine_math_math::Mat4_translate(vector_add(people_.people_[swarms_[n20971558.index].units[n20971559.index].person].pos, Vector{0.0, 0.0, 1.4})).Result;
          a_9 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(people_.people_[swarms_[n20971558.index].units[n20971559.index].person].yaw).Result, Vector{0.27, 0.45, 0.06}).Result;
          a_7 = engine_math_math::Mat4_mul(a_8, a_9).Result;
          combat_art_cpp::CombatArt_put(combat_art_, a_7, CombatArt_Shape::glow_box, vector_scale(struct_break<combat_EnemySpec>(enemy_spec(type).Result).stripe, 1.6));
          if (enum_equal<combat_EnemyType>(type, combat_EnemyType::heavy)) {
            a_11 = engine_math_math::Mat4_translate(vector_add(people_.people_[swarms_[n20971558.index].units[n20971559.index].person].pos, vector_make(kern::formula([](Float a) -> Float { return (kern::math::cos(a) * 0.1); }, people_.people_[swarms_[n20971558.index].units[n20971559.index].person].yaw), kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.1); }, people_.people_[swarms_[n20971558.index].units[n20971559.index].person].yaw), 1.62))).Result;
            a_13 = common::yaw_quat(people_.people_[swarms_[n20971558.index].units[n20971559.index].person].yaw).Result;
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
    for (auto it20971562 = flow_for(n20971562, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Game_RemoteSwarm>(swarms_))))); it20971562.next();) {
      for (auto it20971563 = flow_for(n20971563, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_RemoteUnit>(swarms_[n20971562.index].units))))); it20971563.next();) {
        if (enum_equal<net_SwarmKind>(swarms_[n20971562.index].units[n20971563.index].kind, net_SwarmKind::cruiser)) {
          a = car::CarSpec_car().Result;
          rest = car_rest_pose(a, vector_break(swarms_[n20971562.index].units[n20971563.index].p).x, vector_break(swarms_[n20971562.index].units[n20971563.index].p).y, swarms_[n20971562.index].units[n20971563.index].yaw).Result;
          a_2 = engine_math_math::Mat4_trs(rest.position, rest.rotation, Vector{1.0, 1.0, 1.0}).Result;
          people_art_cpp::PeopleArt_light_bar(people_art_, a_2, math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(swarms_[n20971562.index].units[n20971563.index].flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims)))), 0.5), now);
          if (logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(swarms_[n20971562.index].units[n20971563.index].flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserWreck)))), 0.5), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, swarms_[n20971562.index].units[n20971563.index].wreck_at), 40.0))) {
            people_art_cpp::PeopleArt_fire(people_art_, vector_add(rest.position, Vector{0.0, 0.0, 0.4}), 0.35, now);
          }
        } else if (enum_equal<net_SwarmKind>(swarms_[n20971562.index].units[n20971563.index].kind, net_SwarmKind::police_drone)) {
          v = vector_scale(vector_sub(swarms_[n20971562.index].units[n20971563.index].tp, swarms_[n20971562.index].units[n20971563.index].p), 10.0);
          people_art_cpp::PeopleArt_police_drone(people_art_, swarms_[n20971562.index].units[n20971563.index].p, swarms_[n20971562.index].units[n20971563.index].yaw, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float y) -> Float { return ((-y) * 0.04); }, vector_break(v).y), -0.3, 0.3)), kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x) -> Float { return (x * 0.04); }, vector_break(v).x), -0.3, 0.3)), logic_not(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(swarms_[n20971562.index].units[n20971563.index].flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims)))), 0.5)), now, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mod(a, 5.0)); }, kern::to<Float>(swarms_[n20971562.index].units[n20971563.index].id))));
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
    for (auto it22020120 = flow_for(n22020120, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it22020120.next();) {
      skip = false;
      if (logic_not(projectiles_[n22020120.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (projectiles_[n22020120.index].ghost) {
          ghosts = kern::to<Int>(kern::formula([](Float ghosts) -> Float { return kern::math::wrap32(ghosts + 1.0); }, kern::to<Float>(ghosts)));
          gp = projectiles_[n22020120.index].pos;
        } else {
          rounds = kern::to<Int>(kern::formula([](Float rounds) -> Float { return kern::math::wrap32(rounds + 1.0); }, kern::to<Float>(rounds)));
        }
      }
    }
    b = text_format<"{\"own\":%zu,\"remote\":%zu,\"sent\":%u,\"got\":%u,\"rounds\":%u,\"ghosts\":%u,\"ghost\":[%.2f,%.2f,%.2f]}">(255, list_length<TossState_Own>(toss_.own), list_length<TossState_Remote>(toss_.remote), toss_.sent, toss_.got, rounds, ghosts, vector_break(gp).x, vector_break(gp).y, vector_break(gp).z);
    return {b};
  }

  struct toss_commandResult { Text Result = ""; };
  /// tests: "toss person" - the nearest citizen goes down as ours (as a hand would take him); "toss fling X Y Z" - his doll flies; "toss where I" - the chest of the host's citizen I here
  toss_commandResult toss_command(const Text& cmd) {
    Int w = 0;
    Int physics = 0;
    Float now = 0.0;
    Float sim_time = 0.0;
    Vector me = Vector{0.0, 0.0, 0.0};
    Int best = 0;
    Float bd = 0.0;
    Bool skip = false;
    Float d = 0.0;
    Vector v = Vector{0.0, 0.0, 0.0};
    Bool either = false;
    physics_Body bb = physics_Body{Int64{0x0}};
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    Int id = 0;
    physics_Body b = physics_Body{Int64{0x0}};
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both_2 = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Text o = "";
    Int id_2 = 0;
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
    engine_game_world_physics(n22020108, world_);
    physics = n22020108.result;
    w = physics;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    if (text_equal(cmd, "toss person")) {
      n22020109 = player_position();
      me = n22020109.Result;
      best = -1;
      bd = 60.0;
      for (auto it22020110 = flow_for(n22020110, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<People_Person>(people_.people_))))); it22020110.next();) {
        skip = false;
        if (logic_or(logic_or(enum_equal<People_State>(people_.people_[n22020110.index].state, People_State::down), enum_equal<People_State>(people_.people_[n22020110.index].state, People_State::gone)), math_compare<"<">(now, people_.people_[n22020110.index].knocked_until))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_and(logic_not(net_session::Session_hosting(net_).Result), math_compare<"<">(kern::to<Float>(people_.people_[n22020110.index].net), 0.0))) {
            skip = true;
          }
          if (logic_not(skip)) {
            d = vector_length(vector_sub(people_.people_[n22020110.index].pos, me));
            if (math_compare<"<">(d, bd)) {
              bd = d;
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n22020110.index)));
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
        text_scan<"%f %f %f">(n22020112, cmd, 11, vector_break(v).x, vector_break(v).y, vector_break(v).z);
        v = vector_make(n22020112.v0, vector_break(v).y, vector_break(v).z);
        v = vector_make(vector_break(v).x, n22020112.v1, vector_break(v).z);
        v = vector_make(vector_break(v).x, vector_break(v).y, n22020112.v2);
        either = math_compare<"!=">(kern::to<Float>(n22020112.count), 3.0);
      }
      if (either) {
        return {"usage"};
      }
      for (auto it22020113 = flow_for(n22020113, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<Ragdolls_Bone>(Ragdolls_Bone::kBones))))); it22020113.next();) {
        bb = ragdoll_cpp::Ragdolls_bone_body(glife_[0].life.ragdolls_, int64_add(int64_of_int(toss_test_), Int64{0x1}), enum_from_index<Ragdolls_Bone>(n22020113.index).value).Result;
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
      id = kern::math::wrap32s(int64_low(text_parse_int(cmd, 11, 10).value));
      for (auto it22020114 = flow_for(n22020114, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<People_Person>(people_.people_))))); it22020114.next();) {
        if (logic_select<Bool>(net_session::Session_hosting(net_).Result, math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n22020114.index))))), kern::to<Float>(id)), math_compare<"==">(kern::to<Float>(people_.people_[n22020114.index].net), kern::to<Float>(id)))) {
          b = ragdoll_cpp::Ragdolls_bone_body(glife_[0].life.ragdolls_, int64_add(int64_of_int(n22020114.index), Int64{0x1}), Ragdolls_Bone::chest).Result;
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
      id_2 = kern::math::wrap32s(int64_low(text_parse_int(cmd, 10, 10).value));
      dir = Vector{0.0, 0.0, 0.0};
      n22020115 = aim_ray(dir);
      eye = n22020115.Result;
      d_2 = logic_select<Vector>(math_compare<">">(vector_length(vector_add(dir, Vector{0.0, 0.0, 0.35})), 9.999999717180685e-10), vector_normalize(vector_add(dir, Vector{0.0, 0.0, 0.35})), dir);
      fire_projectile(enum_from_index<weapons_Id>(id_2).value, vector_add(eye, vector_scale(d_2, 1.2)), vector_scale(d_2, kern::formula([](Float id_2) -> Float { return ((id_2 == 7.0) ? 14.0 : ((id_2 == 4.0) ? 22.0 : 40.0)); }, kern::to<Float>(id_2))), true, body(soldier_).Result);
      return {"fired"};
    }
    if (text_equal(cmd, "toss animals")) {
      o_2 = text_append(text_append("{\"epoch\":", text_format<"%f">(-1, life::Life_epoch(glife_[0].life).Result), ""), ",\"cows\":[", "");
      t = "";
      first = true;
      for (auto it22020116 = flow_for(n22020116, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_))))); it22020116.next();) {
        t = text_format<"%s[%u,%.1f,%.1f]">(95, logic_select<Text>(first, "", ","), int64_low(int64_and(glife_[0].life.cows_[n22020116.index].key, Int64{0xffff})), vector_break(glife_[0].life.cows_[n22020116.index].pos).x, vector_break(glife_[0].life.cows_[n22020116.index].pos).y);
        o_2 = text_append(o_2, t, "");
        first = false;
      }
      o_2 = text_append(o_2, "],\"pets\":[", "");
      first = true;
      for (auto it22020117 = flow_for(n22020117, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(glife_[0].life.pets_))))); it22020117.next();) {
        skip_2 = false;
        if (logic_not(glife_[0].life.pets_[n22020117.index].shown)) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          t = text_format<"%s[%zu,%.1f,%.1f]">(95, logic_select<Text>(first, "", ","), n22020117.index, vector_break(glife_[0].life.pets_[n22020117.index].pos).x, vector_break(glife_[0].life.pets_[n22020117.index].pos).y);
          o_2 = text_append(o_2, t, "");
          first = false;
        }
      }
      o_2 = text_append(o_2, "],\"birds\":[", "");
      first = true;
      for (auto it22020118 = flow_for(n22020118, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it22020118.next();) {
        for (auto it22020119 = flow_for(n22020119, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pigeon>(glife_[0].life.flocks_[n22020118.index].birds))))); it22020119.next();) {
          t = text_format<"%s[%.1f,%.1f]">(95, logic_select<Text>(first, "", ","), vector_break(glife_[0].life.flocks_[n22020118.index].birds[n22020119.index].pos).x, vector_break(glife_[0].life.flocks_[n22020118.index].birds[n22020119.index].pos).y);
          o_2 = text_append(o_2, t, "");
          first = false;
        }
      }
      return {text_append(o_2, "]}", "")};
    }
    n22020121 = toss_json();
    return {n22020121.Result};
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

  struct people_viewResult { PeopleView Result = PeopleView{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, true, true, false, List<spawn_Viewer>{}, 0.0, 0.0, List<VehicleView>{}, List<Vector>{}}; };
  people_viewResult people_view() {
    PeopleView v = PeopleView{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, true, true, false, List<spawn_Viewer>{}, 0.0, 0.0, List<VehicleView>{}, List<Vector>{}};
    Int physics = 0;
    Bool skip = false;
    Bool actor_alive = false;
    Bool actor_alive_2 = false;
    Bool either = false;
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    VehicleView vv = VehicleView{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, false, false};
    BodyState s_2 = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    BodyState s_3 = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool flying = false;
    Quat a = Quat{0.0, 0.0, 0.0, 1.0};
    v = PeopleView{0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, true, true, false, List<spawn_Viewer>{}, 0.0, 0.0, List<VehicleView>{}, List<Vector>{}};
    engine_game_world_physics(n23068683, world_);
    physics = n23068683.result;
    v.physics = physics;
    n23068684 = focus_position();
    v.focus = n23068684.Result;
    if (net_session::Session_hosting(net_).Result) {
      v.foci = mate_foci().Result;
    }
    n23068685 = player_position();
    v.player = n23068685.Result;
    if (enum_equal<Mode>(mode_, Mode::foot)) {
      v.player = vector_make(vector_break(v.player).x, vector_break(v.player).y, kern::formula([](Float z) -> Float { return (z - 0.73); }, vector_break(v.player).z));
    }
    v.player_on_foot = enum_equal<Mode>(mode_, Mode::foot);
    v.player_alive = logic_not(player_dead_);
    n23068686 = gaps_spawn_viewers(v.spawn_viewers);
    v.spawn_watched = n23068686.Result;
    v.spawn_far_entry = kern::formula([](Float mobile_) -> Float { return ((mobile_ != 0.0) ? 125.0 : 150.0); }, kern::to<Float>(mobile_));
    v.spawn_height = 1.9;
    for (auto it23068687 = flow_for(n23068687, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it23068687.next();) {
      skip = false;
      either = logic_not(cars_[n23068687.index].placed);
      if (logic_not(either)) {
        actor_alive_2 = engine_game_world_actor_alive(world_, cars_[n23068687.index].actor);
        either = logic_not(actor_alive_2);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        n23068688 = state_of(cars_[n23068687.index].actor);
        s = n23068688.Result;
        vv = VehicleView{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, false, false};
        vv.position = s.position;
        vv.velocity = s.velocity;
        vv.rotation = s.rotation;
        vv.half = vector_make(vector_break(cars_[n23068687.index].car[0].s_.half).x, vector_break(cars_[n23068687.index].car[0].s_.half).y, 0.0);
        vv.player = logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n23068687.index))))), kern::to<Float>(car_index_)));
        list_add(v.vehicles, vv);
      }
    }
    if (drone_out_) {
      n23068689 = state_of(drone_actor_);
      s_2 = n23068689.Result;
      list_add(v.vehicles, struct_make<VehicleView>(vector_sub(s_2.position, vector_make(0.0, 0.0, city_stream_cpp::CityStream_ground_height(city_, vector_break(s_2.position).x, vector_break(s_2.position).y).Result)), s_2.velocity, s_2.rotation, Vector{0.3, 0.3, 0.0}, true, true));
    }
    if (jet_placed_) {
      n23068690 = state_of(jet_actor_);
      s_3 = n23068690.Result;
      flying = math_compare<"==">(kern::to<Float>(jet_.tele_.contacts), 0.0);
      a = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
      list_add(v.vehicles, struct_make<VehicleView>(s_3.position, s_3.velocity, engine_math_math::Quat_mul(s_3.rotation, a).Result, Vector{7.3, 5.0, 0.0}, enum_equal<Mode>(mode_, Mode::jet), flying));
    }
    return {v};
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
    for (auto it23068713 = flow_for(n23068713, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit_Cop>(u.cops))))); it23068713.next();) {
      skip = false;
      if (logic_or(logic_not(u.cops[n23068713.index].inside), logic_not(math_compare<"!=">(kern::to<Float>(u.cops[n23068713.index].seed), 0.0)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        at = vector_add(vector_add(pos, vector_scale(fwd, 0.25)), vector_scale(left, kern::formula([](Float a) -> Float { return (1.35 * a); }, kern::to<Float>(u.cops[n23068713.index].seat))));
        at = vector_make(vector_break(at).x, vector_break(at).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(at).x, vector_break(at).y).Result);
        n23068714 = line_of_sight(vector_add(pos, Vector{0.0, 0.0, 1.0}), vector_add(at, Vector{0.0, 0.0, 1.0}));
        if (logic_not(n23068714.Result)) {
          at = vector_sub(pos, vector_scale(fwd, 2.6));
        }
        at = vector_make(vector_break(at).x, vector_break(at).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(at).x, vector_break(at).y).Result);
        n23068715 = people_cpp::People_add_officer(people_, at, kern::formula([](Float yaw, Float a, Float kPi) -> Float { return (yaw + kern::math::div((a * kPi), 2.0)); }, yaw, kern::to<Float>(u.cops[n23068713.index].seat), kPi), unit, u.cops[n23068713.index].seed);
        u.cops[n23068713.index].person = n23068715.Result;
        u.cops[n23068713.index].inside = false;
        n23068716 = common::Rng_unit(rng_);
        u.cops[n23068713.index].next_shot = kern::formula([](Float now, Float the_result) -> Float { return ((now + 0.7) + (the_result * 0.5)); }, now, n23068716.Result);
        u.cops[n23068713.index].los = false;
        u.cops[n23068713.index].los_at = -1.0;
      }
    }
    u.state = PoliceUnit_State::engage;
  }

  void spawn_police_drones() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Int stars = 0;
    Int desired = 0;
    Int active = 0;
    Vector anchor = Vector{0.0, 0.0, 0.0};
    Vector chosen = Vector{0.0, 0.0, 0.0};
    Float forward = 0.0;
    Int i = 0;
    Bool skip = false;
    Float base = 0.0;
    Bool placed = false;
    Int k = 0;
    Bool skip_2 = false;
    Float a = 0.0;
    Float r = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    the_RayResult_ roof = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics = 0;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    n23068719 = wanted_cpp::stars();
    stars = n23068719.Result;
    n23068720 = wanted_cpp::police_count(stars);
    desired = n23068720.Result;
    active = 0;
    for (auto it23068721 = flow_for(n23068721, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it23068721.next();) {
      active += kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(pdrones_[n23068721.index].active)));
    }
    if (math_compare<">">(kern::to<Float>(active), 0.0)) {
      drone_wave_live_ = true;
      if (math_compare<">=">(kern::to<Float>(active), kern::to<Float>(desired))) {
        return;
      }
    } else if (drone_wave_live_) {
      drone_wave_live_ = false;
      n23068722 = wanted_cpp::wave_break_ms(stars);
      next_drone_wave_ = kern::formula([](Float now, Float the_result) -> Float { return (now + kern::math::div(the_result, 1000.0)); }, now, n23068722.Result);
    }
    if (logic_and(math_compare<">=">(next_drone_wave_, 0.0), math_compare<"<">(now, next_drone_wave_))) {
      return;
    }
    n23068723 = wanted_cpp::escape_ms(stars);
    if (math_compare<">">(last_contact_, kern::formula([](Float now, Float the_result) -> Float { return (now - kern::math::div(the_result, 1000.0)); }, now, n23068723.Result))) {
      chosen = last_known_;
    } else {
      n23068725 = player_position();
      chosen = n23068725.Result;
    }
    anchor = chosen;
    forward = kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(cam_.target).y, vector_break(cam_.eye).y, vector_break(cam_.target).x, vector_break(cam_.eye).x);
    i = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(i), 5.0), math_compare<"<">(kern::to<Float>(active), kern::to<Float>(desired))))) {
      skip = false;
      if (logic_or(pdrones_[i].active, math_compare<"<">(now, pdrones_[i].destroyed_until))) {
        skip = true;
      }
      if (logic_not(skip)) {
        n23068726 = gaps_drone_due(i, stars);
        if (logic_not(n23068726.Result)) {
          active += 1;
          skip = true;
        }
        if (logic_not(skip)) {
          base = kern::formula([](Float forward, Float kPi, Float i) -> Float { return ((forward + kPi) + ((i - 2.0) * 0.34)); }, forward, kPi, kern::to<Float>(i));
          placed = false;
          k = 0;
          while (loop(logic_and(math_compare<"<">(kern::to<Float>(k), 12.0), logic_not(placed)))) {
            skip_2 = false;
            a = kern::formula([](Float base, Float k) -> Float { return (base + ((((kern::math::mod(k, 2.0) != 0.0) ? 1.0 : (-1.0)) * kern::math::ceil(kern::math::div(k, 2.0))) * 0.38)); }, base, kern::to<Float>(k));
            n23068727 = wanted_cpp::spawn_radius(kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32s(i); }, kern::to<Float>(i))));
            r = kern::formula([](Float the_result, Float k) -> Float { return (the_result + (kern::math::shr(k, 2.0) * 6.0)); }, n23068727.Result, kern::to<Float>(k));
            p = vector_make(kern::formula([](Float x, Float a, Float r) -> Float { return (x + (kern::math::cos(a) * r)); }, vector_break(anchor).x, a, r), kern::formula([](Float y, Float a, Float r) -> Float { return (y + (kern::math::sin(a) * r)); }, vector_break(anchor).y, a, r), 0.0);
            engine_game_world_physics(n23068728, world_);
            physics = n23068728.result;
            roof = common::cast(physics, vector_make(vector_break(p).x, vector_break(p).y, 80.0), Vector{0.0, 0.0, -1.0}, 90.0, struct_make<physics_Body>(Int64{0x0}), Int64{0x1}).Result;
            p = vector_make(vector_break(p).x, vector_break(p).y, kern::formula([](Float z, Float a, Float hit, Float z_2) -> Float { return kern::math::max(((z + 0.18) + a), (((hit != 0.0) ? z_2 : 0.0) + 2.5)); }, vector_break(anchor).z, list_get<Float>(kPdFormation, i), kern::to<Float>(roof.hit), vector_break(roof.point).z));
            n23068729 = line_of_sight(p, vector_add(p, Vector{0.0, 0.0, 0.5}));
            if (logic_not(n23068729.Result)) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              pdrones_[i] = struct_make<PoliceDrone>(false, false, false, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 100.0, 0.0, 0.0, 0.0, 0.0, -1.0, -1.0, 0.0, 0.0, -1.0, 0, 0);
              pdrones_[i].active = true;
              pdrones_[i].pos = p;
              pdrones_[i].yaw = kern::formula([](Float a, Float kPi) -> Float { return (a + kPi); }, a, kPi);
              pdrones_[i].vel = vector_make(kern::formula([](Float a) -> Float { return ((-kern::math::sin(a)) * 1.2); }, a), kern::formula([](Float a) -> Float { return (kern::math::cos(a) * 1.2); }, a), 0.0);
              pdrones_[i].hp = kPdHp;
              n23068730 = wanted_cpp::engage_delay_ms(stars);
              pdrones_[i].next_shot = kern::formula([](Float now, Float the_result) -> Float { return (now + kern::math::div(the_result, 1000.0)); }, now, n23068730.Result);
              drone_serial_ = kern::to<Int>(kern::formula([](Float drone_serial_) -> Float { return kern::math::wrap32(drone_serial_ + 1.0); }, kern::to<Float>(drone_serial_)));
              pdrones_[i].serial = drone_serial_;
              gaps_drone_revealed(i, stars);
              placed = true;
              active += 1;
            }
            k += 1;
          }
        }
      }
      i = kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32(i + 1.0); }, kern::to<Float>(i)));
    }
  }

  void emp_pulse() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Bool either = false;
    Vector me = Vector{0.0, 0.0, 0.0};
    Int targets = 0;
    Float emp_range = 0.0;
    Int hit = 0;
    Bool skip = false;
    Float dist = 0.0;
    Float impulse = 0.0;
    Vector away = Vector{0.0, 0.0, 0.0};
    Float al = 0.0;
    Float a = 0.0;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    either = math_compare<"<">(now, emp_ready_at_);
    if (logic_not(either)) {
      n23068732 = wanted_cpp::stars();
      either = math_compare<"<=">(kern::to<Float>(n23068732.Result), 0.0);
    }
    if (either) {
      toast_ = logic_select<Text>(math_compare<"<">(now, emp_ready_at_), "EMP laedt ...", "EMP: keine Ziele");
      toast_age_ = 0.0;
      return;
    }
    n23068733 = player_position();
    me = n23068733.Result;
    targets = 0;
    emp_range = wanted_cpp::emp_range().Result;
    for (auto it23068734 = flow_for(n23068734, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it23068734.next();) {
      targets += kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float emp_range) -> Float { return ((((a != 0.0) && (!(a_2 != 0.0))) && (a_3 <= emp_range)) ? 1.0 : 0.0); }, kern::to<Float>(pdrones_[n23068734.index].active), kern::to<Float>(pdrones_[n23068734.index].emp), vector_length(vector_sub(pdrones_[n23068734.index].pos, me)), emp_range));
    }
    if (math_compare<"==">(kern::to<Float>(targets), 0.0)) {
      ctl_.emp_miss_until = kern::formula([](Float now) -> Float { return (now + 1.1); }, now);
      return;
    }
    emp_ready_at_ = kern::formula([](Float now, Float the_result) -> Float { return (now + kern::math::div(the_result, 1000.0)); }, now, wanted_cpp::emp_cooldown_ms().Result);
    hit = 0;
    for (auto it23068735 = flow_for(n23068735, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it23068735.next();) {
      skip = false;
      if (logic_or(logic_not(pdrones_[n23068735.index].active), pdrones_[n23068735.index].emp)) {
        skip = true;
      }
      if (logic_not(skip)) {
        dist = vector_length(vector_sub(pdrones_[n23068735.index].pos, me));
        n23068736 = wanted_cpp::emp_impulse(dist);
        impulse = n23068736.Result;
        if (math_compare<"<=">(impulse, 0.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          pdrones_[n23068735.index].emp = true;
          pdrones_[n23068735.index].sees = false;
          away = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(pdrones_[n23068735.index].pos).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(pdrones_[n23068735.index].pos).y, vector_break(me).y), 0.0);
          al = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(away).x, vector_break(away).y);
          if (math_compare<"<">(al, 0.01)) {
            a = kern::formula([](Float index, Float kPi) -> Float { return ((kern::math::div(index, 5.0) * 2.0) * kPi); }, kern::to<Float>(n23068735.index), kPi);
            away = vector_make(kern::formula([](Float a) -> Float { return kern::math::cos(a); }, a), kern::formula([](Float a) -> Float { return kern::math::sin(a); }, a), 0.0);
          } else {
            away = vector_scale(away, kern::formula([](Float al) -> Float { return kern::math::div(1.0, al); }, al));
          }
          pdrones_[n23068735.index].vel = vector_add(pdrones_[n23068735.index].vel, vector_scale(vector_add(vector_scale(away, impulse), vector_make(0.0, 0.0, kern::formula([](Float impulse) -> Float { return (-kern::math::max(16.0, (impulse * 0.16))); }, impulse))), 0.05555555555555555));
          pdrones_[n23068735.index].hp = kern::formula([](Float a, Float kPdHp) -> Float { return kern::math::min(a, kern::math::ceil((kPdHp * 0.34))); }, pdrones_[n23068735.index].hp, kPdHp);
          hit = kern::to<Int>(kern::formula([](Float hit) -> Float { return kern::math::wrap32(hit + 1.0); }, kern::to<Float>(hit)));
        }
      }
    }
    engine_game_world_particle_burst(world_, vector_add(me, Vector{0.0, 0.0, 1.2}), Vector{0.4, 0.8, 2.4});
    engine_game_world_play_sound(world_, "builtin:hazard", me, 0.9, 1.4, true, 0);
    osfx("wanted_police_drones.emp", me, 0.0, 0.0, 0.0);
    toast_ = logic_select<Text>(math_compare<"!=">(kern::to<Float>(hit), 0.0), text_append(text_append("EMP! ", text_format<"%u">(-1, hit), ""), " Drohne(n) getroffen", ""), "EMP: keine Drohne in Reichweite");
    toast_age_ = 0.0;
  }

  struct request_respawnResult { Bool Result = false; };
  request_respawnResult request_respawn() {
    Bool respawn_now = false;
    if (logic_or(logic_not(player_dead_), math_compare<">">(respawn_wait().Result, 0.0))) {
      return {false};
    }
    engine_game_world_respawn_now(n23068750, world_, soldier_);
    respawn_now = n23068750.result;
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
    n23068752 = focus_position();
    me = n23068752.Result;
    range = 180.0;
    o = "";
    b = "";
    look = vector_sub(cam_.target, cam_.eye);
    b = text_format<"{\"me\":[%.1f,%.1f,%.3f],\"lanes\":[">(95, vector_break(me).x, vector_break(me).y, kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(look).y, vector_break(look).x));
    o = text_append(o, b, "");
    first = true;
    for (auto it23068753 = flow_for(n23068753, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it23068753.next();) {
      skip = false;
      if (logic_not(city_.lanes_[n23068753.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        near = false;
        k = 0;
        while (loop(logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(city_.lanes_[n23068753.index].points))), 0.5), logic_not(near)))) {
          a = city_.lanes_[n23068753.index].points[k];
          d = vector_sub(city_.lanes_[n23068753.index].points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))], a);
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
          for (auto it23068754 = flow_for(n23068754, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(city_.lanes_[n23068753.index].points))))); it23068754.next();) {
            b = text_format<"%s%.0f,%.0f">(95, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n23068754.index), 0.0), ",", ""), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(city_.lanes_[n23068753.index].points[n23068754.index]).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(city_.lanes_[n23068753.index].points[n23068754.index]).y, vector_break(me).y));
            o = text_append(o, b, "");
          }
          o = text_append(o, "]", "");
        }
      }
    }
    o = text_append(o, "],\"police\":[", "");
    first = true;
    for (auto it23068755 = flow_for(n23068755, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it23068755.next();) {
      skip_2 = false;
      if (math_compare<"<">(kern::to<Float>(units_[n23068755.index].car), 0.0)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        n23068756 = matrix_of(cars_[units_[n23068755.index].car].actor);
        a_2 = n23068756.Result;
        p = engine_math_math::Mat4_translation(a_2).Result;
        b = text_format<"%s[%.0f,%.0f]">(95, logic_select<Text>(first, "", ","), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(p).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(p).y, vector_break(me).y));
        o = text_append(o, b, "");
        first = false;
      }
    }
    for (auto it23068757 = flow_for(n23068757, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it23068757.next();) {
      skip_3 = false;
      if (logic_not(pdrones_[n23068757.index].active)) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        b = text_format<"%s[%.0f,%.0f]">(95, logic_select<Text>(first, "", ","), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(pdrones_[n23068757.index].pos).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(pdrones_[n23068757.index].pos).y, vector_break(me).y));
        o = text_append(o, b, "");
        first = false;
      }
    }
    o = text_append(o, "],\"players\":[", "");
    first = true;
    for (auto it23068758 = flow_for(n23068758, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it23068758.next();) {
      b = text_format<"%s[%.0f,%.0f]">(95, logic_select<Text>(first, "", ","), kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(net_.remotes_[n23068758.index].shown).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(net_.remotes_[n23068758.index].shown).y, vector_break(me).y));
      o = text_append(o, b, "");
      first = false;
    }
    o = text_append(o, "]", "");
    if (logic_and(jet_placed_, logic_not(enum_equal<Mode>(mode_, Mode::jet)))) {
      n23068759 = matrix_of(jet_actor_);
      a_3 = n23068759.Result;
      j = engine_math_math::Mat4_translation(a_3).Result;
      b = text_format<",\"jet\":[%.0f,%.0f]">(95, kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(j).x, vector_break(me).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(j).y, vector_break(me).y));
      o = text_append(o, b, "");
    }
    return {text_append(o, "}", "")};
  }

  void draw_town() {
    Float now = 0.0;
    Float sim_time = 0.0;
    Bool skip = false;
    Bool actor_alive = false;
    Bool actor_alive_2 = false;
    Bool either = false;
    Bool on = false;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector at = Vector{0.0, 0.0, 0.0};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    people_art_cpp::PeopleArt_begin(people_art_);
    if (world_placed_) {
      if (show_lamps_) {
        update_lamps();
      }
      gaps_lamps_draw();
      sim_time = engine_game_world_sim_time(world_);
      now = sim_time;
      for (auto it23068760 = flow_for(n23068760, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<People_Person>(people_.people_))))); it23068760.next();) {
        if (logic_and(logic_and(logic_not(enum_equal<People_State>(people_.people_[n23068760.index].state, People_State::gone)), math_compare<"<">(vector_length(vector_sub(people_.people_[n23068760.index].pos, cam_.eye)), kern::formula([](Float people_m, Float the_result) -> Float { return (people_m * kern::math::min(the_result, 4.0)); }, look_.people_m, view_zoom().Result))), logic_not(life_hides_person(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n23068760.index)))).Result))) {
          people_art_cpp::PeopleArt_person(people_art_, people_.people_[n23068760.index], now);
        }
      }
      life_draw_ragdolls();
      draw_projectiles();
      draw_guided();
      for (auto it23068761 = flow_for(n23068761, 0, 4); it23068761.next();) {
        if (pdrones_[n23068761.index].active) {
          people_art_cpp::PeopleArt_police_drone(people_art_, pdrones_[n23068761.index].pos, pdrones_[n23068761.index].yaw, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float y) -> Float { return ((-y) * 0.04); }, vector_break(pdrones_[n23068761.index].vel).y), -0.3, 0.3)), kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x) -> Float { return (x * 0.04); }, vector_break(pdrones_[n23068761.index].vel).x), -0.3, 0.3)), logic_not(pdrones_[n23068761.index].emp), now, n23068761.index);
        }
      }
      for (auto it23068762 = flow_for(n23068762, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it23068762.next();) {
        skip = false;
        either = logic_not(cars_[n23068762.index].placed);
        if (logic_not(either)) {
          actor_alive_2 = engine_game_world_actor_alive(world_, cars_[n23068762.index].actor);
          either = logic_not(actor_alive_2);
        }
        if (either) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (cars_[n23068762.index].police) {
            on = logic_and(logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(cars_[n23068762.index].unit), 0.0), math_compare<"<">(kern::to<Float>(cars_[n23068762.index].unit), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<PoliceUnit>(units_)))))))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n23068762.index].unit].state, PoliceUnit_State::abandoned))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n23068762.index].unit].state, PoliceUnit_State::wreck))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n23068762.index].unit].state, PoliceUnit_State::leave)));
            if (logic_not(logic_and(logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(car_index_), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n23068762.index))))))), car_cockpit_))) {
              n23068763 = matrix_of(cars_[n23068762.index].actor);
              a = n23068763.Result;
              people_art_cpp::PeopleArt_light_bar(people_art_, a, on, now);
            }
          }
          if (logic_or(math_compare<">=">(cars_[n23068762.index].burn_until, 0.0), cars_[n23068762.index].wreck)) {
            n23068764 = matrix_of(cars_[n23068762.index].actor);
            a_2 = n23068764.Result;
            at = engine_math_math::Mat4_translation(a_2).Result;
            if (math_compare<">=">(cars_[n23068762.index].burn_until, 0.0)) {
            } else if (math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, cars_[n23068762.index].wreck_at), 40.0)) {
              people_art_cpp::PeopleArt_fire(people_art_, vector_add(at, Vector{0.0, 0.0, 0.4}), 0.35, now);
            }
          }
        }
      }
    }
    draw_remote_people();
    if (world_placed_) {
      swarm_draw_town();
    }
    life_draw_people();
    people_art_cpp::PeopleArt_end(people_art_);
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

  void town_mark(Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float strength = 0.0, Bool craters = false) {
    Int physics = 0;
    Float sim_time = 0.0;
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0))) {
      return;
    }
    engine_game_world_physics(n24117258, world_);
    physics = n24117258.result;
    sim_time = engine_game_world_sim_time(world_);
    world_marks_cpp::Marks_blast(life_[0].marks, city_, rng_, at, radius, strength, physics, sim_time, craters);
    townsync_ready();
    list_add(townsync_[0].marks, struct_make<town_MarkRec>(at, radius, strength));
    if (math_compare<">">(kern::formula([](Float kMarks, Float a) -> Float { return ((kMarks < 0.0) != (a < 0.0) ? (a < 0.0) : (kMarks < a)); }, kern::to<Float>(kMarks), kern::to<Float>(list_length<town_MarkRec>(townsync_[0].marks))), 0.5)) {
      list_remove_at(townsync_[0].marks, 0);
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
    map_add(n24117259, gaps_[0].planks, key);
    key_2 = r.key;
    map_add(n24117260, gaps_[0].planks, key_2);
    if (math_compare<"!=">(kern::to<Float>(list_length<Int>(gaps_[0].planks[n24117260.index])), kern::to<Float>(r.doors))) {
      key_3 = r.key;
      map_add(n24117261, gaps_[0].planks, key_3);
      t = gaps_[0].planks[n24117261.index];
      count = r.doors;
      fill = kPlanksMax;
      list_clear(t);
      list_resize(t, count, fill);
      put = t;
      gaps_[0].planks[n24117261.index] = put;
    }
    key_4 = r.key;
    map_add(n24117262, gaps_[0].planks, key_4);
    t_2 = gaps_[0].planks[n24117262.index];
    t_2[r.door] = r.planks;
    put_2 = t_2;
    gaps_[0].planks[n24117262.index] = put_2;
  }

  struct townsync_jsonResult { Text Result = ""; };
  townsync_jsonResult townsync_json() {
    Int open_doors = 0;
    Int n = 0;
    Text b = "";
    townsync_ready();
    open_doors = 0;
    if (math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0)) {
      for (auto it24117266 = flow_for(n24117266, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<Int>>(gaps_[0].planks))))); it24117266.next();) {
        for (auto it24117267 = flow_for(n24117267, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(gaps_[0].planks[n24117266.index]))))); it24117267.next();) {
          n = gaps_[0].planks[n24117266.index][n24117267.index];
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

  struct traffic_gapResult { Float Result = 0.0; };
  traffic_gapResult traffic_gap(Int& limiter, Int car = 0, Vector pos = Vector{0.0, 0.0, 0.0}, Vector fwd = Vector{0.0, 0.0, 0.0}) {
    Int count = 0;
    TrafficRules_Vehicle fill = TrafficRules_Vehicle{Vector{0.0, 0.0, 0.0}, false, false};
    Bool skip = false;
    Bool actor_alive = false;
    Bool actor_alive_2 = false;
    Bool either = false;
    Vector p = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip_2 = false;
    Int physics = 0;
    Int physics_2 = 0;
    Bool body_valid = false;
    Bool either_2 = false;
    Vector p_2 = Vector{0.0, 0.0, 0.0};
    Int physics_3 = 0;
    physics_Pose pose = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Float fl = 0.0;
    Vector f2 = Vector{0.0, 0.0, 0.0};
    Int who = 0;
    Float v = 0.0;
    TrafficRules a_2 = TrafficRules{Int64{0xffffffffffffffff}, -1.0, TrafficRules_Laws{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}, List<TrafficRules_Junction>{}, Map<Int64, List<Int>>{}, Map<Int, TrafficRules_Blocked>{}};
    List<TrafficRules_Vehicle> a_3 = List<TrafficRules_Vehicle>{};
    limiter = -1;
    if (logic_or(logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0)), math_compare<"<">(kern::to<Float>(car), 0.0))) {
      return {1000000000.0};
    }
    if (logic_not(int64_equal(life_[0].vehicles_step, steps_))) {
      life_[0].vehicles_step = steps_;
      count = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, kern::to<Float>(list_length<TrafficCar>(cars_)), kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))));
      fill = struct_make<TrafficRules_Vehicle>(Vector{0.0, 0.0, 0.0}, false, false);
      list_clear(life_[0].vehicles);
      list_resize(life_[0].vehicles, count, fill);
      for (auto it25165835 = flow_for(n25165835, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it25165835.next();) {
        skip = false;
        either = logic_not(cars_[n25165835.index].placed);
        if (logic_not(either)) {
          actor_alive_2 = engine_game_world_actor_alive(world_, cars_[n25165835.index].actor);
          either = logic_not(actor_alive_2);
        }
        if (either) {
          skip = true;
        }
        if (logic_not(skip)) {
          n25165836 = matrix_of(cars_[n25165835.index].actor);
          a = n25165836.Result;
          p = engine_math_math::Mat4_translation(a).Result;
          life_[0].vehicles[n25165835.index] = struct_make<TrafficRules_Vehicle>(vector_make(vector_break(p).x, vector_break(p).y, 0.0), math_compare<">">(cars_[n25165835.index].car[0].s_.radius, 0.4), true);
        }
      }
      for (auto it25165837 = flow_for(n25165837, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))))); it25165837.next();) {
        skip_2 = false;
        either_2 = logic_not(engine_physics_physics::Body_valid(life_[0].trucks[n25165837.index].body).Result);
        if (logic_not(either_2)) {
          engine_game_world_physics(n25165839, world_);
          physics_2 = n25165839.result;
          body_valid = engine_world_body_valid(physics_2, life_[0].trucks[n25165837.index].body);
          either_2 = logic_not(body_valid);
        }
        if (either_2) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          engine_game_world_physics(n25165840, world_);
          physics_3 = n25165840.result;
          pose = engine_world_pose(physics_3, life_[0].trucks[n25165837.index].body);
          p_2 = struct_break<physics_Pose>(pose).position;
          life_[0].vehicles[kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return (a + index); }, kern::to<Float>(list_length<TrafficCar>(cars_)), kern::to<Float>(n25165837.index)))] = struct_make<TrafficRules_Vehicle>(vector_make(vector_break(p_2).x, vector_break(p_2).y, 0.0), false, true);
        }
      }
    }
    fl = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(fwd).x, vector_break(fwd).y);
    f2 = logic_select<Vector>(math_compare<">">(fl, 0.0001), vector_make(kern::formula([](Float x, Float fl) -> Float { return kern::math::div(x, fl); }, vector_break(fwd).x, fl), kern::formula([](Float y, Float fl) -> Float { return kern::math::div(y, fl); }, vector_break(fwd).y, fl), 0.0), Vector{1.0, 0.0, 0.0});
    who = -1;
    a_2 = life_[0].rules;
    a_3 = life_[0].vehicles;
    n25165841 = world_traffic_cpp::TrafficRules_gap_speed(a_2, a_3, who, car, vector_make(vector_break(pos).x, vector_break(pos).y, 0.0), f2, math_compare<">">(cars_[car].car[0].s_.radius, 0.4));
    v = n25165841.Result;
    limiter = kern::to<Int>(kern::formula([](Float who, Float a) -> Float { return (((who >= 0.0) && (who < kern::math::wrap32s(a))) ? who : (-1.0)); }, kern::to<Float>(who), kern::to<Float>(list_length<TrafficCar>(cars_))));
    return {v};
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
    n25165842 = state_of(cars_[ci].actor);
    s = n25165842.Result;
    yaw = common::heading_of(engine_math_math::Quat_rotate(s.rotation, Vector{1.0, 0.0, 0.0}).Result).Result;
    side = vector_make(kern::formula([](Float yaw) -> Float { return (-kern::math::sin(yaw)); }, yaw), kern::formula([](Float yaw) -> Float { return kern::math::cos(yaw); }, yaw), 0.0);
    at = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x + (x_2 * 1.8)); }, vector_break(s.position).x, vector_break(side).x), kern::formula([](Float y, Float y_2) -> Float { return (y + (y_2 * 1.8)); }, vector_break(s.position).y, vector_break(side).y), 0.0);
    engine_game_world_physics(n25165843, world_);
    physics = n25165843.result;
    if (struct_break<the_RayResult_>(common::cast(physics, s.position, vector_make(vector_break(side).x, vector_break(side).y, 0.0), 2.3, body(cars_[ci].actor).Result, Int64{0xffffffffffffffff}).Result).hit) {
      at = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - (x_2 * 1.8)); }, vector_break(s.position).x, vector_break(side).x), kern::formula([](Float y, Float y_2) -> Float { return (y - (y_2 * 1.8)); }, vector_break(s.position).y, vector_break(side).y), 0.0);
    }
    at = vector_make(vector_break(at).x, vector_break(at).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(at).x, vector_break(at).y).Result);
    n25165844 = common::Rng_next(rng_);
    n25165845 = people_cpp::People_add_citizen(people_, at, kern::formula([](Float yaw, Float kPi) -> Float { return (yaw + kern::math::div(kPi, 2.0)); }, yaw, kPi), kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::bor(the_result, 1.0); }, kern::to<Float>(n25165844.Result))));
    p = n25165845.Result;
    if (logic_and(logic_and(math_compare<">=">(kern::to<Float>(cars_[ci].lane), 0.0), math_compare<"<">(kern::to<Float>(cars_[ci].lane), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Lane>(city_.lanes_)))))))), city_.lanes_[cars_[ci].lane].alive)) {
      people_.people_[p].lane = cars_[ci].lane;
      people_.people_[p].seg = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return (a - ((a_2 > 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(cars_[ci].index), kern::to<Float>(cars_[ci].dir))))), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::wrap32s(a) - 2.0); }, kern::to<Float>(list_length<Vector>(city_.lanes_[cars_[ci].lane].points)))))))));
      n25165846 = common::Rng_unit(rng_);
      people_.people_[p].dir = kern::to<Int>(kern::formula([](Float the_result) -> Float { return ((the_result < 0.5) ? 1.0 : (-1.0)); }, n25165846.Result));
      people_.people_[p].side = 1.0;
      people_.people_[p].off = kern::formula([](Float a) -> Float { return ((a * 0.5) + 1.7); }, city_.lanes_[cars_[ci].lane].width);
      people_.people_[p].along = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(at).x, vector_break(city_.lanes_[cars_[ci].lane].points[people_.people_[p].seg]).x, vector_break(at).y, vector_break(city_.lanes_[cars_[ci].lane].points[people_.people_[p].seg]).y);
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    n25165847 = common::Rng_unit(rng_);
    if (math_compare<"<">(n25165847.Result, 0.5)) {
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
    for (auto it25165853 = flow_for(n25165853, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))))); it25165853.next();) {
      if (math_compare<"==">(kern::to<Float>(life_[0].trucks[n25165853.index].job), kern::to<Float>(car))) {
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
    engine_renderer_allocate_id(n27262994, r_);
    allocate_id = n27262994.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = mat;
    look.visible = false;
    engine_upsert(n27262995, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n27262995.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, id);
      id = 0;
    }
  }

  void ui_draw_gps_put(const Mat4& m, Int r = 0, Int id = 0) {
    Int r_ = 0;
    Bool update_instances = false;
    r_ = r;
    if (math_compare<"!=">(kern::to<Float>(id), 0.0)) {
      engine_renderer_update_instances(n27262999, r_, id, m.m, List<Float>{});
      update_instances = n27262999.result;
    }
  }

  /// The position puck and the track line on the ground (gps_live_mode.mjs ensureScene / pushTrailPoint).
  void ui_draw_gps(Float dt = 0.0) {
    Int r = 0;
    Int renderer = 0;
    Vector blue = Vector{0.0, 0.0, 0.0};
    MeshData a = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData a_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData a_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData wedge = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Vector c = Vector{0.0, 0.0, 0.0};
    Float a0 = 0.0;
    Float a1 = 0.0;
    Bool show = false;
    Int item = 0;
    Int item_2 = 0;
    Int item_3 = 0;
    Int item_4 = 0;
    Fixed<Int, 4> items = Fixed<Int, 4>{};
    Int id = 0;
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
    MeshData line = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Vector blue_2 = Vector{0.0, 0.0, 0.0};
    Vector item_5 = Vector{0.0, 0.0, 0.0};
    Vector item_6 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 2> items_2 = Fixed<Vector, 2>{};
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
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
      wedge = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
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
    items = list_make<Int, "4">(item, item_2, item_3, item_4);
    for (auto it27262997 = flow_for(n27262997, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(items))))); it27262997.next();) {
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
        line = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
        blue_2 = Vector{0.26, 0.52, 0.96};
        for (auto it27263002 = flow_for(n27263002, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(ui_.trail))))); it27263002.next();) {
          item_5 = ui_.trail[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n27263002.index)))];
          item_6 = ui_.trail[n27263002.index];
          items_2 = list_make<Vector, "2">(item_5, item_6);
          for (auto it27263003 = flow_for(n27263003, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(items_2))))); it27263003.next();) {
            engine_geo_mesh_data::MeshData_push(line, vector_make(vector_break(items_2[n27263003.index]).x, vector_break(items_2[n27263003.index]).y, kern::formula([](Float the_result) -> Float { return (the_result + 0.12); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(items_2[n27263003.index]).x, vector_break(items_2[n27263003.index]).y).Result)), Vector{0.0, 0.0, 1.0}, blue_2, Vector{0.0, 0.0, 0.0});
          }
        }
        look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
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
      engine_renderer_allocate_id(n27263007, r_);
      allocate_id = n27263007.result;
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
    engine_upsert(n27263008, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n27263008.result;
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

  void ui_before_input(Int e = 0) {
    ui_.head_yaw_before = head_yaw_;
    ui_.head_pitch_before = head_pitch_;
  }

  void ui_after_input(Float dt = 0.0) {
    switch (flow_switch_enum<Mode>(mode_)) {
      case Mode::foot: {
      } break;
      case Mode::drone: {
        if (logic_not(ui_.drone_seen)) {
          sticks_.clearance = settings::Settings_get(ui_.settings, Opt::drone_hover_agl).Result;
          ui_.drone_seen = true;
        }
      } break;
      case Mode::car: {
        if (math_compare<">=">(kern::to<Float>(car_index_), 0.0)) {
          cars_[car_index_].input.max_speed = kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 3.6); }, settings::Settings_get(ui_.settings, Opt::car_max_kmh).Result);
          cars_[car_index_].input.max_reverse = kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 3.6); }, settings::Settings_get(ui_.settings, Opt::car_reverse_kmh).Result);
          cars_[car_index_].input.steer = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a, Float the_result) -> Float { return kern::math::div((a * the_result), 100.0); }, cars_[car_index_].input.steer, settings::Settings_get(ui_.settings, Opt::car_steering).Result), -1.0, 1.0));
          cars_[car_index_].input.pedal = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a, Float the_result) -> Float { return kern::math::div((a * the_result), 100.0); }, cars_[car_index_].input.pedal, settings::Settings_get(ui_.settings, Opt::car_throttle).Result), -1.0, 1.0));
        }
      } break;
      case Mode::jet: {
      } break;
      case Mode::parachute: {
      } break;
    }
    if (logic_not(enum_equal<Mode>(mode_, Mode::drone))) {
      ui_.drone_seen = false;
    }
    ui_jetpack(dt);
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
    n27263018 = ui_equipment_json();
    return {text_append(s, n27263018.Result, "")};
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
    n29360139 = vr_hand_matrix(m, 1);
    if (logic_not(n29360139.Result)) {
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
      n29360141 = vr_hand_matrix(m, 1);
      both_2 = n29360141.Result;
    }
    on = both_2;
    engine_engine_set_xr_scope(engine_, false);
    if (logic_not(on)) {
      if (math_compare<"!=">(kern::to<Float>(vr_scope_mesh_), 0.0)) {
        engine_renderer_set_visible(n29360142, r, vr_scope_mesh_, false);
        set_visible = n29360142.result;
      }
      return;
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(vr_scope_tex_), 0.0))) {
      fbo = 0;
      engine_renderer_create_render_target(n29360143, r, 512, fbo);
      create_render_target = n29360143.result;
      fbo = n29360143.framebuffer;
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
      for (auto it29360144 = flow_for(n29360144, 0, kSeg); it29360144.next();) {
        a = kern::formula([](Float index, Float kSeg, Float kPi) -> Float { return ((kern::math::div(index, kSeg) * 2.0) * kPi); }, kern::to<Float>(n29360144.index), kern::to<Float>(kSeg), kPi);
        engine_geo_mesh_data::MeshData_push(d, vector_make(kern::formula([](Float a) -> Float { return (kern::math::cos(a) * 0.5); }, a), 0.0, kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.5); }, a)), n, Vector{1.0, 1.0, 1.0}, vector_make(kern::formula([](Float a) -> Float { return (0.5 + (kern::math::cos(a) * 0.5)); }, a), kern::formula([](Float a) -> Float { return (0.5 - (kern::math::sin(a) * 0.5)); }, a), 0.0));
        if (math_compare<">">(kern::to<Float>(n29360144.index), 0.0)) {
          place = list_length<Int>(d.indices);
          item = 0;
          item_2 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n29360144.index)));
          item_3 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32((index + 1.0)); }, kern::to<Float>(n29360144.index)));
          list_insert_list(d.indices, place, list_make<Int, "3">(item, item_2, item_3));
        }
      }
      engine_renderer_allocate_id(n29360145, r);
      allocate_id = n29360145.result;
      vr_scope_mesh_ = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
      look.material.unlit = true;
      look.material.double_sided = true;
      look.material.cast_shadow = false;
      look.material.frustum_culled = false;
      look.extra_slot = kMaxExtensionAttributes;
      look.texture = vr_scope_tex_;
      engine_upsert(n29360146, r, vr_scope_mesh_, d, look, List<Float>{}, List<Float>{});
      upsert = n29360146.result;
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
    engine_renderer_update_instances(n29360147, r, vr_scope_mesh_, lm, List<Float>{});
    update_instances = n29360147.result;
    engine_renderer_set_visible(n29360148, r, vr_scope_mesh_, true);
    set_visible_2 = n29360148.result;
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
      n29360150 = vr_hand_matrix(m, 1);
      both = n29360150.Result;
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
    n30408741 = aim_ray(dir);
    eye = n30408741.Result;
    the_actor_velocity = engine_game_world_actor_velocity(world_, soldier_);
    v = vector_add(vector_add(vector_scale(dir, kGrenadeThrow), Vector{0.0, 0.0, 3.5}), the_actor_velocity);
    fire_projectile(weapons_Id::hand_grenade, vector_add(eye, vector_scale(dir, 0.5)), v, true, body(soldier_).Result);
  }

  void draw_projectiles() {
    for (auto it30408750 = flow_for(n30408750, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it30408750.next();) {
      switch (flow_switch_enum<weapons_Id>(projectiles_[n30408750.index].id)) {
        case weapons_Id::smg: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::glock: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::sniper: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::grenade_launcher: {
        } break;
        case weapons_Id::rocket: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::fists: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::knife: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::hand_grenade: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.05, 0.14, false);
        } break;
        case weapons_Id::drone_gun: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::drone_rockets: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::drone_guided: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::jet_cannon: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::jet_missile: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.12, 2.8, true);
        } break;
        case weapons_Id::jet_bomb: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408750.index].pos, projectiles_[n30408750.index].vel, 0.25, 1.9, false);
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
    mp_rules_g_palette = func_get_on<"Palette", List<Int>>(actor_find("Multiplayer"));
    return {runs};
  }

  struct paletteResult { List<Int> Result = List<Int>{}; };
  KERN_PURE paletteResult palette() {
    return {mp_rules_g_palette};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { combat_GateRow gate = combat_GateRow{"", Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false}; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct { Int result = 0; } n1048586;
  struct : kern::Node<2097162> { Int index = 0; } n2097162;
  struct : kern::Node<2097163> { Int index = 0; } n2097163;
  struct { Bool result = false; game_SoldierState out = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0}; } n2097164;
  matrix_ofResult n2097165;
  matrix_ofResult n2097166;
  struct : kern::Node<2097167> { Int index = 0; } n2097167;
  struct : kern::Node<2097168> { Int index = 0; } n2097168;
  struct : kern::Node<2097169> { Int index = 0; } n2097169;
  matrix_ofResult n2097170;
  struct : kern::Node<2097171> { Int index = 0; } n2097171;
  struct : kern::Node<2097172> { Int index = 0; } n2097172;
  struct : kern::Node<2097173> { Int index = 0; } n2097173;
  struct : kern::Node<2097174> { Int index = 0; } n2097174;
  struct { Bool added = false; Int index = 0; } n3145738;
  struct { Bool added = false; Int index = 0; } n3145739;
  struct { Bool added = false; Int index = 0; } n3145740;
  struct : kern::Node<3145741> { Int index = 0; } n3145741;
  struct { Bool added = false; Int index = 0; } n3145742;
  player_positionResult n3145769;
  struct : kern::Node<3145770> { Int index = 0; } n3145770;
  struct { Int result = 0; } n3145771;
  struct { Bool result = false; } n3145772;
  gaps_barricade_doorsResult n3145774;
  struct : kern::Node<3145775> { Int index = 0; } n3145775;
  struct : kern::Node<3145776> { Int index = 0; } n3145776;
  struct { Bool result = false; } n3145777;
  struct { Bool result = false; Bool changed = false; } n3145778;
  struct { Bool result = false; } n3145779;
  vr_hand_rayResult n4194318;
  Game_ctl_assist_pointResult n4194323;
  Game_ctl_assist_pointResult n4194324;
  controls_cpp::aim_ellipseResult n4194325;
  struct { Int result = 0; } n4194326;
  struct : kern::Node<4194327> { Int index = 0; } n4194327;
  struct : kern::Node<4194328> { Int index = 0; } n4194328;
  struct : kern::Node<4194329> { Int index = 0; } n4194329;
  Game_ctl_assist_pointResult n4194330;
  controls_cpp::fps_aim_assistResult n4194331;
  project_screenResult n4194332;
  state_ofResult n4194333;
  Game_ctl_view_json_screenResult n4194334;
  wanted_cpp::starsResult n4194335;
  player_positionResult n4194336;
  struct : kern::Node<4194337> { Int index = 0; } n4194337;
  wanted_cpp::escape_msResult n4194339;
  combat_rules_cpp::Fun_multiplierResult n4194340;
  combat_rules_cpp::Fun_police_damage_scaleResult n4194341;
  combat_rules_cpp::Fun_multiplierResult n4194342;
  wanted_cpp::starsResult n4194343;
  struct : kern::Node<4194344> { Int index = 0; } n4194344;
  Game_ctl_view_json_screenResult n4194345;
  struct { Int result = 0; } n4194346;
  struct : kern::Node<4194347> { Int index = 0; } n4194347;
  matrix_ofResult n4194349;
  struct : kern::Node<4194350> { Int index = 0; } n4194350;
  Game_ctl_view_json_screenResult n4194351;
  struct { Int result = 0; } n4194365;
  controls_cpp::gamepad_axisResult n4194366;
  controls_cpp::gamepad_axisResult n4194367;
  controls_cpp::gamepad_axisResult n4194368;
  touch_play_cpp::TouchPlay_take_zoomResult n4194369;
  struct { Int result = 0; } n4194378;
  controls_cpp::hard_cutResult n4194379;
  controls_cpp::hard_cutResult n4194380;
  state_ofResult n4194381;
  struct { Int result = 0; } n4194382;
  touch_play_cpp::TouchPlay_pointerResult n4194383;
  matrix_ofResult n4194384;
  matrix_ofResult n4194386;
  matrix_ofResult n4194387;
  struct : kern::Node<4194388> { Int index = 0; } n4194388;
  matrix_ofResult n4194389;
  gaps_boardable_2Result n4194390;
  controls_cpp::GrenadeCook_leftResult n4194392;
  wanted_cpp::starsResult n4194393;
  touch_play_cpp::TouchPlay_view_jsonResult n4194394;
  struct : kern::Node<4194395> { Int index = 0; } n4194395;
  controls_cpp::hint_lineResult n4194396;
  controls_cpp::GrenadeCook_leftResult n4194398;
  gaps_boardable_2Result n4194399;
  ctl_view_jsonResult n4194400;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n5242896;
  matrix_ofResult n5242897;
  matrix_ofResult n5242898;
  matrix_ofResult n5242899;
  gaps_ride_poseResult n5242901;
  matrix_ofResult n5242902;
  matrix_ofResult n5242903;
  player_positionResult n5242904;
  struct : kern::Node<5242905> { Int index = 0; } n5242905;
  struct { Int result = 0; } n5242906;
  struct : kern::Node<5242907> { Int index = 0; } n5242907;
  matrix_ofResult n5242908;
  clear_spotResult n5242909;
  clear_spotResult n5242911;
  matrix_ofResult n5242912;
  struct { Bool result = false; } n5242913;
  matrix_ofResult n5242914;
  struct : kern::Node<5242915> { Int index = 0; } n5242915;
  struct : kern::Node<5242916> { Int index = 0; } n5242916;
  city_stream_cpp::CityStream_street_pointResult n5242917;
  struct { Int result = 0; } n5242918;
  standing_feetResult n5242919;
  struct { Bool result = false; } n5242920;
  struct { Bool result = false; } n5242921;
  struct : kern::Node<5242923> { Int index = 0; } n5242923;
  struct { Bool result = false; } n5242930;
  matrix_ofResult n5242931;
  struct { Int result = 0; } n5242932;
  struct { Int result = 0; } n5242934;
  struct { Int result = 0; } n5242935;
  struct { Bool result = false; } n5242936;
  struct { Bool result = false; } n5242937;
  struct { Bool result = false; } n5242938;
  struct { Bool result = false; } n5242939;
  struct { Bool result = false; } n5242940;
  matrix_ofResult n5242941;
  matrix_ofResult n5242942;
  standing_feetResult n5242943;
  struct { Bool result = false; } n5242944;
  struct { Bool result = false; } n5242945;
  struct { Bool result = false; } n5242946;
  struct { Bool result = false; } n5242947;
  struct { Int result = 0; } n5242948;
  struct { Int result = 0; } n5242949;
  state_ofResult n5242950;
  struct { Int result = 0; } n5242951;
  state_ofResult n5242952;
  state_ofResult n5242953;
  struct { Bool result = false; } n5242954;
  struct { Bool result = false; } n5242955;
  common::Rng_nextResult n5242959;
  struct { Int result = 0; } n5242960;
  struct : kern::Node<5242962> { Int index = 0; } n5242962;
  struct { Int result = 0; } n5242964;
  struct { Bool result = false; Camera camera = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}}; } n5242965;
  matrix_ofResult n5242966;
  state_ofResult n5242967;
  Game_update_camera_collideResult n5242968;
  matrix_ofResult n5242969;
  state_ofResult n5242970;
  Game_update_camera_collideResult n5242971;
  matrix_ofResult n5242972;
  state_ofResult n5242973;
  Game_update_camera_collideResult n5242974;
  matrix_ofResult n5242975;
  Game_update_camera_collideResult n5242976;
  struct { List<Float> out = List<Float>{}; } n5242983;
  struct : kern::Node<5242984> { Int index = 0; } n5242984;
  struct : kern::Node<5242985> { Int index = 0; } n5242985;
  struct : kern::Node<5242986> { Int index = 0; } n5242986;
  game_destruction_sync_cpp::digest_ofResult n6291466;
  struct : kern::Node<7340042> { Int index = 0; } n7340042;
  mp_rules_cpp::ballot_nameResult n7340043;
  struct : kern::Node<7340045> { Int index = 0; } n7340045;
  struct : kern::Node<7340046> { Int index = 0; } n7340046;
  struct : kern::Node<7340047> { Int index = 0; } n7340047;
  struct : kern::Node<7340048> { Int index = 0; } n7340048;
  combat_audio_cpp::renderResult n7340049;
  sfx_idResult n7340050;
  struct { Bool result = false; } n7340051;
  sfx_idResult n7340052;
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
  struct { Int result = 0; } n8388644;
  equipment_cpp::Jetpack_igniteResult n8388646;
  struct { Bool result = false; } n8388647;
  struct { Int64 result = Int64{0x0}; } n8388648;
  equipment_cpp::Jetpack_updateResult n8388649;
  struct { Bool result = false; } n8388650;
  struct { Bool result = false; } n8388651;
  matrix_ofResult n8388652;
  struct { Int result = 0; } n8388653;
  touch_play_cpp::TouchPlay_take_pressedResult n8388654;
  struct { Int result = 0; } n8388655;
  matrix_ofResult n8388656;
  struct { Int64 result = Int64{0x0}; } n8388657;
  matrix_ofResult n8388658;
  struct : kern::Node<8388659> { Int index = 0; } n8388659;
  struct : kern::Node<8388660> { Int index = 0; } n8388660;
  nostr_cpp::from_hexResult n8388661;
  nostr_cpp::from_hexResult n8388663;
  nostr_cpp::event_jsonResult n8388664;
  struct { Bool result = false; } n8388665;
  matrix_ofResult n9437242;
  combat_rules_cpp::Race_updateResult n9437243;
  struct : kern::Node<9437244> { Int index = 0; } n9437244;
  struct { Debris value = Debris{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 6.5}; } n9437245;
  struct : kern::Node<9437246> { Int index = 0; } n9437246;
  struct : kern::Node<9437247> { Int index = 0; } n9437247;
  struct : kern::Node<9437248> { Int index = 0; } n9437248;
  struct : kern::Node<9437249> { Int index = 0; } n9437249;
  struct : kern::Node<9437250> { Int index = 0; } n9437250;
  struct : kern::Node<9437251> { Int index = 0; } n9437251;
  struct : kern::Node<9437252> { Int index = 0; } n9437252;
  matrix_ofResult n9437253;
  struct : kern::Node<9437254> { Int index = 0; } n9437254;
  struct : kern::Node<9437255> { Int index = 0; } n9437255;
  matrix_ofResult n9437256;
  struct : kern::Node<9437257> { Int index = 0; } n9437257;
  struct : kern::Node<9437258> { Int index = 0; } n9437258;
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
  struct : kern::Node<10485791> { Int index = 0; } n10485791;
  line_of_sightResult n10485792;
  struct : kern::Node<10485794> { Int index = 0; } n10485794;
  struct : kern::Node<10485795> { Int index = 0; } n10485795;
  struct : kern::Node<10485796> { Int index = 0; } n10485796;
  struct { GapsState_SniperRound value = GapsState_SniperRound{game_ShotEvent{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, false, false, Vector{0.0, 0.0, 1.0}, 0}, 0.0}; } n10485797;
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
  spawn_guard_cpp::cone_half_angleResult n11534367;
  struct : kern::Node<11534368> { Int index = 0; } n11534368;
  gaps_spawn_viewersResult n11534369;
  spawn_guard_cpp::can_spawnResult n11534370;
  struct : kern::Node<11534371> { Int index = 0; } n11534371;
  state_ofResult n11534372;
  spawn_guard_cpp::footprints_overlapResult n11534373;
  gaps_can_spawnResult n11534374;
  wanted_cpp::arrival_delay_msResult n11534375;
  gaps_rules_cpp::post_arrival_grace_msResult n11534376;
  matrix_ofResult n11534377;
  matrix_ofResult n11534378;
  gaps_rules_cpp::SmokeField_startResult n11534379;
  gaps_rules_cpp::SmokeField_accelerateResult n11534380;
  gaps_ride_cameraResult n11534405;
  matrix_ofResult n11534406;
  matrix_ofResult n11534407;
  state_ofResult n11534408;
  wanted_cpp::starsResult n11534409;
  struct : kern::Node<11534410> { Int index = 0; } n11534410;
  gaps_repair_offerResult n11534411;
  struct : kern::Node<11534412> { Int index = 0; } n11534412;
  project_screenResult n11534413;
  struct : kern::Node<11534414> { Int index = 0; } n11534414;
  matrix_ofResult n12582922;
  struct : kern::Node<12582923> { Int index = 0; } n12582923;
  aim_rayResult n12582924;
  guided_target_atResult n12582928;
  guided_cpp::pick_scoreResult n12582929;
  struct : kern::Node<12582930> { Int index = 0; } n12582930;
  struct : kern::Node<12582931> { Int index = 0; } n12582931;
  matrix_ofResult n12582932;
  struct : kern::Node<12582933> { Int index = 0; } n12582933;
  struct : kern::Node<12582934> { Int index = 0; } n12582934;
  struct { Int result = 0; } n12582935;
  state_ofResult n12582936;
  struct : kern::Node<12582937> { Int index = 0; } n12582937;
  guided_cpp::launchResult n12582938;
  struct : kern::Node<12582941> { Int index = 0; } n12582941;
  guided_target_atResult n12582943;
  struct { Int result = 0; } n13631498;
  struct { Bool result = false; } n13631499;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n13631500;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n13631501;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n13631502;
  struct { Bool result = false; } n13631503;
  struct { Bool result = false; } n13631504;
  struct : kern::Node<13631505> { Int index = 0; } n13631505;
  struct : kern::Node<13631506> { Int index = 0; } n13631506;
  struct : kern::Node<13631507> { Int index = 0; } n13631507;
  struct : kern::Node<13631508> { Int index = 0; } n13631508;
  struct { Int result = 0; } n13631509;
  struct : kern::Node<13631510> { Int index = 0; } n13631510;
  struct : kern::Node<13631511> { Int index = 0; } n13631511;
  struct : kern::Node<13631512> { Int index = 0; } n13631512;
  struct { Int result = 0; } n13631513;
  struct : kern::Node<13631514> { Int index = 0; } n13631514;
  struct { Bool result = false; } n13631515;
  struct { Bool result = false; Bool changed = false; } n13631516;
  struct { Bool result = false; } n13631517;
  struct : kern::Node<14680074> { Int index = 0; } n14680074;
  life_cpp::Life_shotResult n14680075;
  life_cpp::Life_shotResult n14680076;
  struct { Bool result = false; } n14680077;
  player_positionResult n14680078;
  struct : kern::Node<14680079> { Int index = 0; } n14680079;
  struct : kern::Node<14680080> { Int index = 0; } n14680080;
  struct : kern::Node<14680081> { Int index = 0; } n14680081;
  struct : kern::Node<14680082> { Int index = 0; } n14680082;
  struct : kern::Node<14680083> { Int index = 0; } n14680083;
  struct : kern::Node<14680084> { Int index = 0; } n14680084;
  engine_math_math::Mat4_look_at_2Result n14680085;
  struct : kern::Node<14680086> { Int index = 0; } n14680086;
  struct { Int result = 0; } n15728650;
  look_cpp::Shake_sampleResult n15728651;
  struct { Bool result = false; } n15728653;
  struct { Bool result = false; } n15728665;
  struct { Int result = 0; } n15728666;
  struct { Bool result = false; } n15728667;
  vr_hand_matrixResult n15728668;
  struct { Bool result = false; Bool changed = false; } n15728669;
  struct { Bool result = false; } n15728670;
  struct { Int result = 0; } n15728673;
  struct { Bool result = false; } n15728674;
  matrix_ofResult n16777226;
  gaps_can_spawnResult n16777228;
  struct { Int result = 0; } n16777229;
  struct : kern::Node<16777230> { Int index = 0; } n16777230;
  common::Rng_rangeResult n16777231;
  spawn_unseenResult n16777232;
  common::Rng_nextResult n16777233;
  common::Rng_nextResult n16777235;
  common::Rng_nextResult n16777236;
  common::Rng_nextResult n16777237;
  common::Rng_nextResult n16777238;
  spawn_unseenResult n16777240;
  road_graph_cpp::RoadGraph_pathResult n16777241;
  struct { Int result = 0; } n16777242;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n16777243;
  struct { Int result = 0; } n16777244;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n16777245;
  struct : kern::Node<16777246> { Int index = 0; } n16777246;
  common::Rng_nextResult n16777247;
  common::Rng_nextResult n16777249;
  common::Rng_nextResult n16777250;
  common::Rng_nextResult n16777251;
  common::Rng_nextResult n16777252;
  spawn_unseenResult n16777254;
  common::Rng_nextResult n16777255;
  people_cpp::People_add_scriptedResult n16777256;
  common::Rng_unitResult n16777257;
  common::Rng_unitResult n16777258;
  struct : kern::Node<16777311> { Int index = 0; } n16777311;
  mp_art_cpp::MpArt_initResult n17825802;
  struct : kern::Node<17825803> { Int index = 0; } n17825803;
  struct : kern::Node<17825804> { Int index = 0; } n17825804;
  struct : kern::Node<17825805> { Int index = 0; } n17825805;
  state_ofResult n17825806;
  matrix_ofResult n17825807;
  vr_hand_rayResult n17825809;
  wanted_cpp::starsResult n17825810;
  struct : kern::Node<17825811> { Int index = 0; } n17825811;
  matrix_ofResult n17825812;
  struct : kern::Node<17825813> { Int index = 0; } n17825813;
  vr_hand_matrixResult n17825814;
  state_ofResult n17825815;
  state_ofResult n17825816;
  gaps_ride_poseResult n17825817;
  struct : kern::Node<17825818> { Int index = 0; } n17825818;
  mp_rules_cpp::VoiceStream_renderResult n17825819;
  person_hit::hitResult n17825820;
  struct : kern::Node<17825821> { Int index = 0; } n17825821;
  remote_on_segment_zonesResult n17825822;
  remote_on_segment_zonesResult n17825823;
  remote_on_segment_zonesResult n17825824;
  matrix_ofResult n17825825;
  net_session_cpp::Session_color_ofResult n17825827;
  struct : kern::Node<17825828> { Int index = 0; } n17825828;
  net_session_cpp::Session_color_ofResult n17825829;
  struct : kern::Node<17825830> { Int index = 0; } n17825830;
  net_session_cpp::Session_color_ofResult n17825831;
  struct : kern::Node<17825832> { Int index = 0; } n17825832;
  net_session_cpp::Session_color_ofResult n17825833;
  focus_positionResult n17825834;
  road_graph_cpp::RoadGraph_pathResult n17825835;
  project_screenResult n17825836;
  mp_rules_cpp::safe_mobile_marker_pointResult n17825837;
  struct : kern::Node<17825838> { Int index = 0; } n17825838;
  project_screenResult n17825839;
  mp_rules_cpp::PingBoard_fadeResult n17825840;
  focus_positionResult n17825841;
  mp_rules_cpp::route_leftResult n17825843;
  mp_rules_cpp::sample_breadcrumbsResult n17825844;
  mp_rules_cpp::upcoming_turnResult n17825845;
  mp_rules_cpp::bearing_arrowResult n17825846;
  net_session_cpp::Session_color_ofResult n17825847;
  matrix_ofResult n17825849;
  focus_positionResult n17825850;
  struct : kern::Node<17825851> { Int index = 0; } n17825851;
  net_session_cpp::Session_color_ofResult n17825852;
  struct : kern::Node<17825853> { Int index = 0; } n17825853;
  net_session_cpp::Session_color_ofResult n17825854;
  net_session_cpp::Session_color_ofResult n17825856;
  net_session_cpp::Session_color_ofResult n17825857;
  mp_rules_cpp::weapon_labelResult n17825858;
  focus_positionResult n17825859;
  struct : kern::Node<17825860> { Int index = 0; } n17825860;
  struct : kern::Node<17825861> { Int index = 0; } n17825861;
  struct : kern::Node<17825862> { Int index = 0; } n17825862;
  mp_rules_cpp::NavTimeline_sampleResult n17825863;
  net_session_cpp::Session_color_ofResult n17825864;
  struct : kern::Node<17825865> { Int index = 0; } n17825865;
  struct : kern::Node<17825866> { Int index = 0; } n17825866;
  matrix_ofResult n17825867;
  struct : kern::Node<18874378> { Int index = 0; } n18874378;
  struct { Bool removed = false; } n18874379;
  matrix_ofResult n19922954;
  struct : kern::Node<19922955> { Int index = 0; } n19922955;
  jet_seats_cpp::Table_getResult n19922956;
  gaps_boardableResult n19922957;
  gaps_ride_poseResult n19922958;
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
  struct : kern::Node<20971558> { Int index = 0; } n20971558;
  struct : kern::Node<20971559> { Int index = 0; } n20971559;
  struct : kern::Node<20971562> { Int index = 0; } n20971562;
  struct : kern::Node<20971563> { Int index = 0; } n20971563;
  struct : kern::Node<22020106> { Int index = 0; } n22020106;
  life::Life_animal_ofResult n22020107;
  struct { Int result = 0; } n22020108;
  player_positionResult n22020109;
  struct : kern::Node<22020110> { Int index = 0; } n22020110;
  struct { Int count = 0; Float v0 = 0.0; Float v1 = 0.0; Float v2 = 0.0; } n22020112;
  struct : kern::Node<22020113> { Int index = 0; } n22020113;
  struct : kern::Node<22020114> { Int index = 0; } n22020114;
  aim_rayResult n22020115;
  struct : kern::Node<22020116> { Int index = 0; } n22020116;
  struct : kern::Node<22020117> { Int index = 0; } n22020117;
  struct : kern::Node<22020118> { Int index = 0; } n22020118;
  struct : kern::Node<22020119> { Int index = 0; } n22020119;
  struct : kern::Node<22020120> { Int index = 0; } n22020120;
  toss_jsonResult n22020121;
  struct { Int result = 0; } n23068682;
  struct { Int result = 0; } n23068683;
  focus_positionResult n23068684;
  player_positionResult n23068685;
  gaps_spawn_viewersResult n23068686;
  struct : kern::Node<23068687> { Int index = 0; } n23068687;
  state_ofResult n23068688;
  state_ofResult n23068689;
  state_ofResult n23068690;
  struct : kern::Node<23068713> { Int index = 0; } n23068713;
  line_of_sightResult n23068714;
  people_cpp::People_add_officerResult n23068715;
  common::Rng_unitResult n23068716;
  wanted_cpp::starsResult n23068719;
  wanted_cpp::police_countResult n23068720;
  struct : kern::Node<23068721> { Int index = 0; } n23068721;
  wanted_cpp::wave_break_msResult n23068722;
  wanted_cpp::escape_msResult n23068723;
  player_positionResult n23068725;
  gaps_drone_dueResult n23068726;
  wanted_cpp::spawn_radiusResult n23068727;
  struct { Int result = 0; } n23068728;
  line_of_sightResult n23068729;
  wanted_cpp::engage_delay_msResult n23068730;
  wanted_cpp::starsResult n23068732;
  player_positionResult n23068733;
  struct : kern::Node<23068734> { Int index = 0; } n23068734;
  struct : kern::Node<23068735> { Int index = 0; } n23068735;
  wanted_cpp::emp_impulseResult n23068736;
  struct { Bool result = false; } n23068750;
  focus_positionResult n23068752;
  struct : kern::Node<23068753> { Int index = 0; } n23068753;
  struct : kern::Node<23068754> { Int index = 0; } n23068754;
  struct : kern::Node<23068755> { Int index = 0; } n23068755;
  matrix_ofResult n23068756;
  struct : kern::Node<23068757> { Int index = 0; } n23068757;
  struct : kern::Node<23068758> { Int index = 0; } n23068758;
  matrix_ofResult n23068759;
  struct : kern::Node<23068760> { Int index = 0; } n23068760;
  struct : kern::Node<23068761> { Int index = 0; } n23068761;
  struct : kern::Node<23068762> { Int index = 0; } n23068762;
  matrix_ofResult n23068763;
  matrix_ofResult n23068764;
  struct { Int result = 0; } n24117258;
  struct { Bool added = false; Int index = 0; } n24117259;
  struct { Bool added = false; Int index = 0; } n24117260;
  struct { Bool added = false; Int index = 0; } n24117261;
  struct { Bool added = false; Int index = 0; } n24117262;
  struct : kern::Node<24117266> { Int index = 0; } n24117266;
  struct : kern::Node<24117267> { Int index = 0; } n24117267;
  matrix_ofResult n25165834;
  struct : kern::Node<25165835> { Int index = 0; } n25165835;
  matrix_ofResult n25165836;
  struct : kern::Node<25165837> { Int index = 0; } n25165837;
  struct { Int result = 0; } n25165839;
  struct { Int result = 0; } n25165840;
  world_traffic_cpp::TrafficRules_gap_speedResult n25165841;
  state_ofResult n25165842;
  struct { Int result = 0; } n25165843;
  common::Rng_nextResult n25165844;
  people_cpp::People_add_citizenResult n25165845;
  common::Rng_unitResult n25165846;
  common::Rng_unitResult n25165847;
  struct { Int result = 0; } n25165851;
  struct : kern::Node<25165852> { Int index = 0; } n25165852;
  struct : kern::Node<25165853> { Int index = 0; } n25165853;
  struct : kern::Node<26214410> { Int index = 0; } n26214410;
  struct { Int result = 0; } n26214412;
  struct { Int result = 0; } n26214413;
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
  ui_equipment_jsonResult n27263018;
  vr_hand_matrixResult n29360139;
  vr_hand_matrixResult n29360141;
  struct { Bool result = false; } n29360142;
  struct { Int result = 0; Int framebuffer = 0; } n29360143;
  struct : kern::Node<29360144> { Int index = 0; } n29360144;
  struct { Int result = 0; } n29360145;
  struct { Bool result = false; } n29360146;
  struct { Bool result = false; Bool changed = false; } n29360147;
  struct { Bool result = false; } n29360148;
  vr_hand_matrixResult n29360150;
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
  aim_rayResult n30408741;
  struct : kern::Node<30408750> { Int index = 0; } n30408750;
  world_marks_cpp::Marks_bulletResult n31457293;
  struct : kern::Node<32505866> { Int index = 0; } n32505866;
};
