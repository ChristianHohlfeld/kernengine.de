struct Game : virtual kern::Blueprint, virtual car, virtual common, virtual engine_geo_mesh_data, virtual engine_gfx_passes, virtual engine_math_math, virtual engine_physics_physics, virtual equipment, virtual game_barricades_cpp, virtual game_combat, virtual game_controls, virtual game_destruction_sync, virtual game_gaps, virtual game_gaps_cpp, virtual game_guided_cpp, virtual game_lamps_cpp, virtual game_life, virtual game_look, virtual game_look_cpp, virtual game_net, virtual game_projsync, virtual game_projsync_cpp, virtual game_swarm_cpp, virtual game_toss, virtual game_townsync, virtual game_traffic_cpp, virtual game_ui, virtual game_vehicle_damage_cpp, virtual game_weapons_cpp, virtual game_world, virtual game_worldsync_cpp, virtual gaps_rules, virtual guided, virtual interiors, virtual jet, virtual lamps, virtual life, virtual net_session, virtual people, virtual people_cpp, virtual perf, virtual person_hit, virtual settings, virtual terrain, virtual wire {
  // ---- variables ----
  PadState pad = PadState{false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, false};
  List<DestroySync> dsync_ = List<DestroySync>{};
  TossState toss_ = TossState{List<TossState_Own>{}, List<TossState_Remote>{}, 0.0, 0, 0};
  List<ProjSync> projsync_ = List<ProjSync>{};
  Int proj_serial_ = 0;
  List<TownSync> townsync_ = List<TownSync>{};
  Bool puppet_world_ = false;
  /// client: the host's index each of our cars plays (-1 none)
  List<Int> car_net_ = List<Int>{};
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
  Bool world_placed_ = false;
  Bool chute_open_ = false;
  Float chute_time_ = 0.0;
  Vector chute_steer_ = Vector{0.0, 0.0, 0.0};
  /// 0 fpv, 1 follow, 2 third (the page restores the player's last choice; FPV first)
  Int drone_cam_ = 0;
  Bool car_cockpit_ = false;
  Bool jet_cockpit_ = false;
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
  /// jet: jet_mode.mjs HP_MAX 400
  Float star_since_ = -1.0;
  Bool player_dead_ = false;
  /// vs_combat_presentation.mjs: down until the player asks (R / Enter / Space, pad A / Y, the RESPAWN button) - not before 3 s (RESPAWN_COOLDOWN_MS); back 3..5 m from where he went down
  Float died_at_ = -1.0;
  /// rules / feedback (the wanted level is the "Wanted" Blueprint's: wanted.h)
  Text toast_ = "";
  Float toast_age_ = 99.0;
  Float time_ = 0.0;
  Rng rng_ = Rng{20261029, 0.0, false};
  Int bank_ = 0;
  /// the original's sound on the page (game_audio_web.cpp): events and state for web/audio.mjs
  Bool web_audio_ = false;
  Text osfx_ = "";
  Bool mobile_ = false;
  /// menus, settings, start, equipment, presentation (game_ui.h)
  UiState ui_ = UiState{Settings{List<Float>{}, 1}, SettingsTable{List<Option>{}, List<OptSpec>{}}, 0, false, false, 0, false, 0.0, 0.0, 0, 0, 0, Vector{1000000000.0, 1000000000.0, 0.0}, 0.0, -1, -1, GpsLive{GpsLive_View::eye, 0, 0, false, false, false, false, 0.0, -1000000000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}}, 0, "", Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, 0, 0, 0, List<Vector>{}, false, 0.0, false, Jetpack{1.0, false, false, 0, 0.0, -1.0, -99.0}, false, false, 0.0, List<Int64>{}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, 0, 0, 0, 0, 0, 0, FlightLog{List<Flight>{}, List<FlightLog_Event>{}, Flight{0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", List<Vector>{}}, false, false, Vector{0.0, 0.0, 0.0}, 0.0, -1000000000.0, 1, 0}, "", MusicSynth{0.0, 0, -2, -2, 0.0, 0.0, 1.0, Int64{0x0}, 0, 1, 0, 1, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, List<MusicSynth_Voice>{}, 0.0, 0.0, List<Float>{}, 0}, 0, true, "", "", 99.0, Vector{0.0, 0.0, 0.0}, false, 0};
  /// performance and the look (game_look.h)
  LookState look_ = LookState{perf_Meter{0.0, 0.0, List<Float>{}, List<Float>{}, 0.0, 0.0, -1.0}, perf_Governor{4, 4, true, 0.016666666666666666, 0.0, 0.0, 0.0, 0.0, List<Float>{}, List<Float>{}, 0}, false, false, false, -1, 0, 170.0, 220.0, look_Shake{0.0, 17.3}, look_Stabilizer{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0}, look_SpeedLines{List<look_SpeedLines_Streak>{}, false, false, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, Rng{24301, 0.0, false}}, Vector{0.0, 0.0, 0.0}, false, -1, 0.4, 0.0, 0, 16, List<LookState_Blast>{}, List<LookState_Glint>{}, -1.0, -99.0, List<Vector>{}, 0, 0, List<Float>{}, List<Float>{}, List<Int>{}, List<Int>{}, List<Int>{}, List<Int>{}, false, -1, -99.0, 0.0, 0.0, 0.0, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, -30.0}, -99.0, 30.0, -99.0, false, -1, 0, List<Int>{}};
  /// M3 life (last: destroyed before world_)
  List<GameLife> glife_ = List<GameLife>{};
  /// (of the program: combat::kStrikeCooldownS)
  Float combat_kStrikeCooldownS = 0.0;
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
  KERN_CONST Fixed<MusicSynth_Station, 5> kStations = List<MusicSynth_Station>{MusicSynth_Station{"KERN FM", "Synthwave", 100.0, 11}, MusicSynth_Station{"BODENSEE BEATS", "House", 124.0, 23}, MusicSynth_Station{"LOFI LAGUNE", "Lo-Fi", 78.0, 37}, MusicSynth_Station{"8-BIT RADIO", "Chiptune", 144.0, 41}, MusicSynth_Station{"NACHTSCHICHT", "Techno", 132.0, 53}};
  /// ROCKET_LASER_RANGE_M
  KERN_CONST Float kLaserRange = 420.0;
  /// SpawnCity's numbers. Damage of the hitscan guns is SpawnCity's multiplayer damage (VS_WEAPON_DAMAGE smg 13, glock 26, sniper 95) times the zone (head 3, torso 1, legs .65).
  KERN_CONST Fixed<weapons_Spec, 14> kTable = List<weapons_Spec>{weapons_Spec{"MP", 55.0, 180.0, 13.0, 0.0, 240.0, 1}, weapons_Spec{"Glock", 110.0, 240.0, 26.0, 0.0, 0.0, 2}, weapons_Spec{"Sniper .50", 1150.0, 1500.0, 95.0, 0.0, 850.0, 3}, weapons_Spec{"Granatwerfer 40 mm", 760.0, 300.0, 125.0, 8.5, 40.0, 5}, weapons_Spec{"Raketenwerfer", 900.0, 420.0, 160.0, 9.5, 40.0, 6}, weapons_Spec{"Faeuste", 300.0, 1.8, 24.0, 0.0, 0.0, 7}, weapons_Spec{"Messer", 380.0, 1.6, 45.0, 0.0, 0.0, 8}, weapons_Spec{"Handgranate", 4000.0, 40.0, 125.0, 8.5, 0.0, 0}, weapons_Spec{"Drohnen-MG", 92.0, 650.0, 13.0, 0.0, 0.0, 0}, weapons_Spec{"Drohnen-Raketen", 950.0, 220.0, 100.0, 8.0, 31.0, 0}, weapons_Spec{"GUIDED", 5000.0, 600.0, 34.0, 4.0, 34.0, 0}, weapons_Spec{"Bordkanone", 55.0, 1800.0, 30.0, 0.0, 0.0, 0}, weapons_Spec{"Luft-Rakete", 2400.0, 1300.0, 120.0, 8.0, 260.0, 0}, weapons_Spec{"Bombe", 450.0, 0.0, 170.0, 17.0, 0.0, 0}};
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kPoleH = 6.2;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kArm = 1.42;
  /// The lamp as it is built and laid out (the model and the field, not a rule).
  KERN_CONST Float kHeadZ = 6.0;
  KERN_CONST Float kDeg = 0.017453292519943295;
  KERN_CONST Fixed<perf_Tier, 5> kTier = List<perf_Tier>{perf_Tier{"minimal", 16, 35.0, 1, 0, 70.0, 90.0, 2, 0.6}, perf_Tier{"niedrig", 30, 50.0, 4, 0, 110.0, 140.0, 3, 0.75}, perf_Tier{"mittel", 44, 70.0, 4, 1, 140.0, 180.0, 4, 1.0}, perf_Tier{"hoch", 64, 90.0, 9, 2, 170.0, 220.0, 6, 1.0}, perf_Tier{"maximal", 80, 110.0, 9, 3, 170.0, 220.0, 10, 1.0}};
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST Fixed<Text, 4> kVmNames = List<Text>{"BOOMSTICK 40 mm", "ROCKET LAUNCHER", "FISTS", "KNIFE"};
  /// world.* switches that make the world (friendly fire is the rule voted on: game_rules_sync)
  KERN_CONST Fixed<Opt, 13> kWorldRules = List<Opt>{Opt::world_people, Opt::world_traffic, Opt::world_dogs, Opt::world_cats, Opt::world_birds, Opt::world_zombies, Opt::world_police, Opt::world_military, Opt::world_flak, Opt::world_jetpack, Opt::world_terrain, Opt::world_guided, Opt::world_always_day};
  /// The number of each value of net_PlayerFlag, in their order
  KERN_CONST List<Int> net_PlayerFlag_numbers = List<Int>{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
  /// a seated passenger: hips 0.5 m over the road instead of 0.92 (game_seats.cpp)
  KERN_CONST Float kSeatDrop = 0.42;
  /// The number of each value of net_SwarmFlag, in their order
  KERN_CONST List<Int> net_SwarmFlag_numbers = List<Int>{128, 1, 2, 4, 3};
  KERN_CONST Float kFallenMs = 1500.0;
  KERN_CONST Int kPerson = 0;
  KERN_CONST Int kBird = 3;
  KERN_CONST Int kCow = 1;
  KERN_CONST Int kPet = 2;
  KERN_CONST Int kPlanksMax = 6;
  /// s of Y held = RESET
  KERN_CONST Float kResetHold = 1.0;
  /// Magazines and spare rounds on foot (gameplay_final_runtime_v2.mjs WEAPONS).
  KERN_CONST Fixed<Loadout, 7> kFoot = List<Loadout>{Loadout{weapons_Id::smg, 30, 150}, Loadout{weapons_Id::glock, 17, 85}, Loadout{weapons_Id::sniper, 5, 25}, Loadout{weapons_Id::grenade_launcher, 6, 18}, Loadout{weapons_Id::rocket, 1, 6}, Loadout{weapons_Id::fists, -1, -1}, Loadout{weapons_Id::knife, -1, -1}};
  /// hand_grenade.mjs FUSE_MS 4000
  KERN_CONST Float kGrenadeFuse = 4.0;
  /// hand_grenade.mjs FUSE_MS 4000
  KERN_CONST Float kGrenadeThrow = 14.0;

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
    map_add(n2097162, gaps_[0].planks, key_2);
    key_3 = key;
    map_add(n2097163, gaps_[0].planks, key_3);
    if (math_compare<"!=">(kern::to<Float>(list_length<Int>(gaps_[0].planks[n2097163.index])), kern::to<Float>(list_length<Interiors_Door>(glife_[0].interiors.plan_.doors)))) {
      key_4 = key;
      map_add(n2097164, gaps_[0].planks, key_4);
      t = gaps_[0].planks[n2097164.index];
      count = list_length<Interiors_Door>(glife_[0].interiors.plan_.doors);
      fill = kern::to<Int>(kern::formula([](Float kPlanks) -> Float { return kern::math::band(kPlanks, 255.0); }, kern::to<Float>(kPlanks)));
      list_clear(t);
      list_resize(t, count, fill);
      put = t;
      gaps_[0].planks[n2097164.index] = put;
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
    for (auto it2097165 = flow_for(n2097165, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Interiors_Door>(glife_[0].interiors.plan_.doors))))); it2097165.next();) {
      key_5 = key;
      map_add(n2097166, gaps_[0].planks, key_5);
      list_add(out, struct_make<GapsState_DoorView>(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n2097165.index))), glife_[0].interiors.plan_.doors[n2097165.index].c, glife_[0].interiors.plan_.doors[n2097165.index].d, glife_[0].interiors.plan_.doors[n2097165.index].n, list_get<Int>(gaps_[0].planks[n2097166.index], n2097165.index), kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((a != 0.0) ? a_2 : a_3); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<Float>(glife_[0].interiors.plan_.levels)), 0.0)), glife_[0].interiors.plan_.base, glife_[0].interiors.plan_.levels[0]), key));
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
      engine_renderer_allocate_id(n2097170, gaps_[0].renderer);
      allocate_id = n2097170.result;
      gaps_[0].plank_part.id = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
      look.material = m;
      look.extra_slot = kMaxExtensionAttributes;
      look.visible = false;
      engine_upsert(n2097171, gaps_[0].renderer, gaps_[0].plank_part.id, box, look, List<Float>{}, List<Float>{});
      upsert = n2097171.result;
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
      n2097173 = gaps_barricade_doors(doors);
      both = n2097173.Result;
    }
    if (both) {
      for (auto it2097174 = flow_for(n2097174, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_DoorView>(doors))))); it2097174.next();) {
        yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(doors[n2097174.index].d).y, vector_break(doors[n2097174.index].d).x);
        for (auto it2097175 = flow_for(n2097175, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(doors[n2097174.index].planks)))); it2097175.next();) {
          along = kern::formula([](Float index) -> Float { return ((kern::math::mod(index, 2.0) != 0.0) ? 0.06 : (-0.06)); }, kern::to<Float>(n2097175.index));
          tilt = kern::formula([](Float index) -> Float { return (((kern::math::mod(index, 2.0) != 0.0) ? 1.0 : (-1.0)) * (0.08 + (0.05 * kern::math::mod(index, 3.0)))); }, kern::to<Float>(n2097175.index));
          at = vector_make(kern::formula([](Float x, Float x_2, Float along, Float x_3) -> Float { return ((x + (x_2 * along)) + (x_3 * 0.22)); }, vector_break(doors[n2097174.index].c).x, vector_break(doors[n2097174.index].d).x, along, vector_break(doors[n2097174.index].n).x), kern::formula([](Float y, Float y_2, Float along, Float y_3) -> Float { return ((y + (y_2 * along)) + (y_3 * 0.22)); }, vector_break(doors[n2097174.index].c).y, vector_break(doors[n2097174.index].d).y, along, vector_break(doors[n2097174.index].n).y), kern::formula([](Float a, Float index) -> Float { return ((a + 0.35) + (index * 0.36)); }, doors[n2097174.index].the_floor, kern::to<Float>(n2097175.index)));
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
      engine_renderer_set_visible(n2097176, gaps_[0].renderer, gaps_[0].plank_part.id, false);
      set_visible = n2097176.result;
      return;
    }
    engine_renderer_update_instances(n2097177, gaps_[0].renderer, gaps_[0].plank_part.id, gaps_[0].plank_part.transforms, gaps_[0].plank_part.tints);
    update_instances = n2097177.result;
    engine_renderer_set_visible(n2097178, gaps_[0].renderer, gaps_[0].plank_part.id, true);
    set_visible_2 = n2097178.result;
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
        n3145740 = vr_hand_ray(from, dir);
        both_2 = n3145740.Result;
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
    n3145747 = project_screen(nx, ny, depth, p);
    on = n3145747.Result;
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

  struct ctl_command_startsResult { Bool Result = false; };
  KERN_PURE ctl_command_startsResult ctl_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct matrix_ofResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  matrix_ofResult matrix_of(Int a = 0) {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool actor_matrix = false;
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    engine_game_world_actor_matrix(n4194314, world_, a, m);
    actor_matrix = n4194314.result;
    m = n4194314.out;
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
    for (auto it4194319 = flow_for(n4194319, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it4194319.next();) {
      skip = false;
      if (logic_or(logic_not(city_.lanes_[n4194319.index].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(city_.lanes_[n4194319.index].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(city_.lanes_[n4194319.index].points))), 0.5))) {
          a = city_.lanes_[n4194319.index].points[k];
          b = city_.lanes_[n4194319.index].points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))];
          ab = vector_sub(b, a);
          l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(ab).x, vector_break(ab).y);
          t = kern::formula([](Float l2, Float a) -> Float { return ((l2 > 1e-06) ? a : 0.0); }, l2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(c).x, vector_break(a).x, vector_break(ab).x, vector_break(c).y, vector_break(a).y, vector_break(ab).y, l2), 0.0, 1.0));
          q = vector_add(a, vector_scale(ab, t));
          if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(q).x, vector_break(c).y, vector_break(q).y), kern::formula([](Float reach, Float a) -> Float { return (reach + (a * 0.5)); }, reach, city_.lanes_[n4194319.index].width))) {
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
    engine_game_world_set_mode(n4194323, world_, game_SimMode::simulate);
    set_mode = n4194323.result;
  }

  void eject() {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    n4194336 = state_of(jet_actor_);
    s = n4194336.Result;
    jet_.pilot = false;
    engine_game_world_set_alive(world_, chute_actor_, true);
    engine_game_world_teleport(n4194337, world_, chute_actor_, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 2.8}).Result), List<Vector>{});
    teleport = n4194337.result;
    engine_game_world_set_actor_velocity(n4194338, world_, chute_actor_, vector_add(s.velocity, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kEjectUp)).Result));
    set_actor_velocity = n4194338.result;
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
    n4194341 = common::Rng_next(rng_);
    start_2 = n4194341.Result;
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
    engine_game_world_physics(n4194342, world_);
    physics = n4194342.result;
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

  void dsync_ready() {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<DestroySync>(dsync_)), 0.0))) {
      dsync_ = list_make<DestroySync, "1">(DestroySync{1, 0.0, 0.0, List<Terrain_CraterRec>{}, List<DestroySync_Mine>{}, List<CityStream_DamageRec>{}, List<Int_Float_pair>{}, List<Int>{}, 0, -99.0, false, false, 0, 0, 0});
    }
  }

  struct details_command_startsResult { Bool Result = false; };
  KERN_PURE details_command_startsResult details_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
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

  struct combat_command_startsResult { Bool Result = false; };
  KERN_PURE combat_command_startsResult combat_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct car_near_pointResult { Bool Result = false; };
  car_near_pointResult car_near_point(Vector point = Vector{0.0, 0.0, 0.0}) {
    Bool skip = false;
    BodyState b = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Vector local = Vector{0.0, 0.0, 0.0};
    Vector h = Vector{0.0, 0.0, 0.0};
    for (auto it9437196 = flow_for(n9437196, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it9437196.next();) {
      skip = false;
      if (logic_or(logic_not(cars_[n9437196.index].placed), cars_[n9437196.index].wreck)) {
        skip = true;
      }
      if (logic_not(skip)) {
        n9437197 = state_of(cars_[n9437196.index].actor);
        b = n9437197.Result;
        if (math_compare<">">(vector_length(vector_sub(b.position, point)), 6.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          local = common::to_local(b.rotation, vector_sub(point, b.position)).Result;
          h = cars_[n9437196.index].car[0].s_.half;
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
    n9437198 = remote_on_segment(t, z, from, to);
    if (logic_or(math_compare<"<">(kern::to<Float>(n9437198.Result), 0.0), math_compare<">">(t, 1.0))) {
      return {false};
    }
    at = vector_add(from, vector_scale(vector_sub(to, from), t));
    return {true};
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
    for (auto it9437199 = flow_for(n9437199, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it9437199.next();) {
      flying = logic_or(flying, logic_and(logic_and(logic_and(projectiles_[n9437199.index].alive, projectiles_[n9437199.index].by_player), enum_equal<weapons_Id>(projectiles_[n9437199.index].id, weapons_Id::rocket)), projectiles_[n9437199.index].guided));
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
      n9437200 = ctl_screen_ray(from, ad, laser_fx_, laser_fy_);
    } else {
      ad = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{1.0, 0.0, 0.0});
      from = cam_.eye;
    }
    engine_game_world_physics(n9437201, world_);
    physics = n9437201.result;
    r = common::cast(physics, from, ad, kLaserRange, body(soldier_).Result, Int64{0xffffffffffffffff}).Result;
    laser_valid_ = r.hit;
    laser_dot_ = logic_select<Vector>(r.hit, r.point, vector_add(from, vector_scale(ad, kLaserRange)));
    return {laser_dot_};
  }

  struct foot_remote_detonateResult { Bool Result = false; };
  foot_remote_detonateResult foot_remote_detonate(Float now = 0.0) {
    Bool any = false;
    any = false;
    for (auto it9437203 = flow_for(n9437203, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it9437203.next();) {
      if (logic_and(logic_and(projectiles_[n9437203.index].alive, projectiles_[n9437203.index].by_player), logic_or(enum_equal<weapons_Id>(projectiles_[n9437203.index].id, weapons_Id::grenade_launcher), enum_equal<weapons_Id>(projectiles_[n9437203.index].id, weapons_Id::rocket)))) {
        projectiles_[n9437203.index].fuse = 0.0;
        projectiles_[n9437203.index].born = kern::formula([](Float now) -> Float { return (now - 1.0); }, now);
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
      for (auto it9437216 = flow_for(n9437216, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_ShotEvent>(shots))))); it9437216.next();) {
        if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(shots[n9437216.index].shooter), kern::to<Float>(soldier_)), enum_equal<Mode>(mode_, Mode::foot)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper))) {
          list_add(arrived, shots[n9437216.index]);
        }
      }
      return;
    }
    for (auto it9437217 = flow_for(n9437217, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_ShotEvent>(shots))))); it9437217.next();) {
      if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(shots[n9437217.index].shooter), kern::to<Float>(soldier_)), enum_equal<Mode>(mode_, Mode::foot)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper))) {
        list_add(gaps_[0].sniper_rounds, struct_make<GapsState_SniperRound>(shots[n9437217.index], kern::formula([](Float now, Float a) -> Float { return (now + kern::math::div(a, 850.0)); }, now, vector_length(vector_sub(shots[n9437217.index].to, shots[n9437217.index].from)))));
      }
    }
    for (auto it9437218 = flow_for(n9437218, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_SniperRound>(gaps_[0].sniper_rounds))))); it9437218.next();) {
      if (math_compare<">=">(now, gaps_[0].sniper_rounds[n9437218.index].due)) {
        list_add(arrived, gaps_[0].sniper_rounds[n9437218.index].s);
      }
    }
    at = 0;
    kept = at;
    end = list_length<GapsState_SniperRound>(gaps_[0].sniper_rounds);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(gaps_sniper_rounds_match(gaps_[0].sniper_rounds[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n9437219, gaps_[0].sniper_rounds[at]);
          gaps_[0].sniper_rounds[kept] = n9437219.value;
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
    engine_renderer_allocate_id(n10485771, renderer);
    allocate_id = n10485771.result;
    p.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    renderer_2 = engine_engine_renderer(e);
    engine_upsert(n10485772, renderer_2, p.id, disc, look, parked.m, List<Float>{});
    upsert = n10485772.result;
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
    gaps_ = list_make<GapsState, "1">(GapsState{gaps_SirenVoice{false, 0.0, 1.0, 0.0}, gaps_SirenVoice{false, 0.0, 1.0, 0.0}, 0, 0, 0, 0, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, 0.0, 0.0, 0.0, List<Float>{-1.0, -1.0, -1.0, -1.0, -1.0}, gaps_SmokeField{List<gaps_SmokeEffect>{}, List<gaps_Puff>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, 0, List<Float>{}, false, -1.0, -99.0, -99.0, 0, List<GapsState_Burst>{}, 0, -1.0, 0, 0, 0, false, gaps_VisualPose{false, 0, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, 0.016666666666666666, 0, 0, List<GapsState_JetVoice>{}, 0, List<GapsState_TrailPuff>{}, 0, List<GapsState_MotorVoice>{}, 0, false, GapsState_Part{0, List<Float>{}, List<Float>{}}, 0.0, 0, false, Map<Int64, List<Int>>{}, Map<Int64, Float>{}, Map<Int, GapsState_ZombieDoor>{}, 0, GapsState_Part{0, List<Float>{}, List<Float>{}}, false, false, false, -10.0, 0, 0, -1.0, 0.0, lamps_Field{List<lamps_Lamp>{}, Set<Int64>{}, Map<Int64, Int>{}, Map<Int64, List<Int>>{}}, 4294967295, false, false, -1.0, 0, List<physics_Body>{}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, GapsState_Part{0, List<Float>{}, List<Float>{}}, List<Float>{-99.0, -99.0}, -99.0, -1.0, false, 0.0, 0.0, List<GapsState_RocketPuff>{}, 0, 0, List<GapsState_SniperRound>{}, GapsState_Part{0, List<Float>{}, List<Float>{}}});
    gaps_[0].smoke = gaps_rules::SmokeField_of(kern::to<Int>(kern::formula([](Float mobile_) -> Float { return kern::math::wrap32(((mobile_ != 0.0) ? 48.0 : 78.0)); }, kern::to<Float>(mobile_)))).Result;
    renderer = engine_engine_renderer(e);
    gaps_[0].renderer = renderer;
    n10485770 = game_gaps_cpp::smoke_disc();
    disc = n10485770.Result;
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
    engine_renderer_allocate_id(n10485773, renderer_3);
    allocate_id = n10485773.result;
    gaps_[0].trail_part.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    renderer_4 = engine_engine_renderer(e);
    engine_upsert(n10485774, renderer_4, gaps_[0].trail_part.id, disc, look, parked.m, List<Float>{});
    upsert = n10485774.result;
    if (logic_not(upsert)) {
      renderer_5 = engine_engine_renderer(e);
      engine_renderer_release_id(renderer_5, gaps_[0].trail_part.id);
      gaps_[0].trail_part.id = 0;
    }
    m.color = engine_math_math::Vec4_new(0.847, 0.847, 0.847, 1.0).Result;
    look.material = m;
    renderer_6 = engine_engine_renderer(e);
    engine_renderer_allocate_id(n10485775, renderer_6);
    allocate_id_2 = n10485775.result;
    gaps_[0].rocket_part.id = allocate_id_2;
    renderer_7 = engine_engine_renderer(e);
    engine_upsert(n10485776, renderer_7, gaps_[0].rocket_part.id, disc, look, parked.m, List<Float>{});
    upsert_2 = n10485776.result;
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
    for (auto it10485777 = flow_for(n10485777, 0, 1); it10485777.next();) {
      for (auto it10485778 = flow_for(n10485778, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float rate) -> Float { return (rate * 0.05); }, kern::to<Float>(rate)))))))); it10485778.next();) {
        n = kern::to<Int>(kern::formula([](Float index, Float rate, Float index_2) -> Float { return (kern::math::trunc(((index * 0.09) * rate)) + index_2); }, kern::to<Float>(n10485777.index), kern::to<Float>(rate), kern::to<Float>(n10485778.index)));
        if (math_compare<">">(kern::formula([](Float n, Float a) -> Float { return ((n < 0.0) != (a < 0.0) ? (a < 0.0) : (n < a)); }, kern::to<Float>(n), kern::to<Float>(list_length<Float>(lock))), 0.5)) {
          lock[n] = kern::formula([](Float index, Float rate) -> Float { return ((kern::math::mod(kern::math::div((index * 1760.0), rate), 1.0) < 0.5) ? 0.05 : (-0.05)); }, kern::to<Float>(n10485778.index), kern::to<Float>(rate));
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
    engine_sound_bank_set_pcm(n10485779, bank_, "sc:locktone", lock, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(lock)))), 1, rate);
    set_pcm = n10485779.result;
    engine_sound_bank_request(bank_, "sc:guidedbang");
    engine_sound_bank_set_pcm(n10485780, bank_, "sc:guidedbang", bang, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(bang)))), 1, rate);
    set_pcm_2 = n10485780.result;
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
        n11534346 = matrix_of(cars_[t.index].actor);
        a = n11534346.Result;
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
        for (auto it11534347 = flow_for(n11534347, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it11534347.next();) {
          if (math_compare<"==">(kern::to<Float>(net_.remotes_[n11534347.index].state.slot), kern::to<Float>(t.slot))) {
            at = vector_add(net_.remotes_[n11534347.index].shown, vector_make(0.0, 0.0, kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.1 : 0.0); }, kern::to<Float>(enum_equal<net_PlayerMode>(net_.remotes_[n11534347.index].state.mode, net_PlayerMode::foot)))));
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
    engine_renderer_allocate_id(n12582922, r_);
    allocate_id = n12582922.result;
    p.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    engine_upsert(n12582923, r_, p.id, mesh, look, parked.m, List<Float>{});
    upsert = n12582923.result;
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
    engine_primitives_append(n12582924, pole, cylinder, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    pole = n12582924.dst;
    box = engine_primitives_box(Vector{1.5, 0.09, 0.09}, steel);
    engine_primitives_append(n12582925, pole, box, engine_math_math::Mat4_translate(vector_make(0.72, 0.0, kern::formula([](Float kPoleH) -> Float { return (kPoleH - 0.1); }, kPoleH))).Result);
    pole = n12582925.dst;
    box_2 = engine_primitives_box(Vector{0.6, 0.26, 0.14}, head);
    engine_primitives_append(n12582926, pole, box_2, engine_math_math::Mat4_translate(vector_make(kArm, 0.0, kHeadZ)).Result);
    pole = n12582926.dst;
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
    engine_sound_bank_set_pcm(n12582927, bank_, "sc:lampglass", glass, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(glass)))), 1, rate);
    set_pcm = n12582927.result;
    engine_sound_bank_request(bank_, "sc:lampclang");
    engine_sound_bank_set_pcm(n12582928, bank_, "sc:lampclang", clang, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(clang)))), 1, rate);
    set_pcm_2 = n12582928.result;
    gaps_[0].lamp_sounds = true;
  }

  void gaps_lamps_mirror() {
    list_clear(lamps_);
    for (auto it12582929 = flow_for(n12582929, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<lamps_Lamp>(gaps_[0].lamps.lamps_))))); it12582929.next();) {
      list_add(lamps_, struct_make<Game_Lamp>(gaps_[0].lamps.lamps_[n12582929.index].foot, gaps_[0].lamps.lamps_[n12582929.index].yaw, gaps_[0].lamps.lamps_[n12582929.index].lane, gaps_[0].lamps.lamps_[n12582929.index].state));
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
    engine_game_world_physics(n12582932, world_);
    physics = n12582932.result;
    w = physics;
    for (auto it12582933 = flow_for(n12582933, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<physics_Body>(gaps_[0].fallen))))); it12582933.next();) {
      body_valid = engine_world_body_valid(w, gaps_[0].fallen[n12582933.index]);
      if (body_valid) {
        engine_world_destroy_body(w, gaps_[0].fallen[n12582933.index]);
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

  struct life_command_startsResult { Bool Result = false; };
  KERN_PURE life_command_startsResult life_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
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
      engine_light_pool_make(n14680074, look_.pool_size, 96);
      LightPool_2 = n14680074.result;
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
        engine_renderer_set_visible(n14680077, r, vr_hud_mesh_, false);
        set_visible = n14680077.result;
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
      engine_renderer_allocate_id(n14680078, r);
      allocate_id = n14680078.result;
      vr_hud_mesh_ = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
      look.material.unlit = true;
      look.material.double_sided = true;
      look.material.cast_shadow = false;
      look.material.frustum_culled = false;
      look.material.alpha = AlphaMode::blend;
      look.extra_slot = kMaxExtensionAttributes;
      look.texture = vr_hud_tex_;
      engine_upsert(n14680079, r, vr_hud_mesh_, q, look, List<Float>{}, List<Float>{});
      upsert = n14680079.result;
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
    n14680080 = vr_hand_matrix(lh, 0);
    if (n14680080.Result) {
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
    engine_renderer_update_instances(n14680081, r, vr_hud_mesh_, m, List<Float>{});
    update_instances = n14680081.result;
    engine_renderer_set_visible(n14680082, r, vr_hud_mesh_, true);
    set_visible_2 = n14680082.result;
  }

  void look_view_models_make(const MeshData& d, Int& id, Int r = 0, Int n14680083_index = 0, Bool g = false) {
    Int r_ = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    r_ = r;
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(d.positions)), 0.0)) {
      return;
    }
    engine_renderer_allocate_id(n14680085, r_);
    allocate_id = n14680085.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = game_look_cpp::vm_material(g, math_compare<"==">(kern::to<Float>(n14680083_index), 1.0)).Result;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    engine_upsert(n14680086, r_, id, d, look, List<Float>{}, List<Float>{});
    upsert = n14680086.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, id);
      id = 0;
    }
  }

  void look_view_models_put(List<Float>& t, const Mat4& m) {
    Int place = 0;
    place = list_length<Float>(t);
    list_insert_list(t, place, m.m);
  }

  struct look_view_models_easeResult { Float Result = 0.0; };
  KERN_PURE look_view_models_easeResult look_view_models_ease(Float t = 0.0) {
    return {kern::formula([](Float t) -> Float { return ((t * t) * (3.0 - (2.0 * t))); }, t)};
  }

  void look_view_models(Float dt = 0.0) {
    Int r = 0;
    Int renderer = 0;
    Bool arms = false;
    MeshData body_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData glow = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Int selected = 0;
    Bool which = false;
    Bool which_2 = false;
    Mat4 vr_probe = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool vr_hand = false;
    Int want = 0;
    game_Hud hud = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
    game_Hud hud_2 = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
    Bool both = false;
    Float now = 0.0;
    Float sim_time = 0.0;
    Int wid = 0;
    Int item = 0;
    Int item_2 = 0;
    Fixed<Int, 2> items = Fixed<Int, 2>{};
    Int id = 0;
    Bool set_visible = false;
    Int item_3 = 0;
    Int item_4 = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    Fixed<Int, 4> items_2 = Fixed<Int, 4>{};
    Int id_2 = 0;
    Bool set_visible_2 = false;
    List<Int> list = List<Int>{};
    List<Int> meshes = List<Int>{};
    Bool skip = false;
    Int m = 0;
    Bool either = false;
    Bool mine = false;
    Bool either_2 = false;
    Bool either_3 = false;
    Int arg = 0;
    Bool set_visible_3 = false;
    Vector v = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    Float sp = 0.0;
    Float bx = 0.0;
    Float bz = 0.0;
    Float raise = 0.0;
    Float drop = 0.0;
    Float age = 0.0;
    Mat4 cam = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    List<Float> t = List<Float>{};
    Int selected_2 = 0;
    Float kick = 0.0;
    Bool rkt = false;
    Mat4 g = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector rr = Vector{0.0, 0.0, 0.0};
    Vector uu = Vector{0.0, 0.0, 0.0};
    the_RayResult_ h = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Int physics = 0;
    Vector want_2 = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Vector grip = Vector{0.0, 0.0, 0.0};
    Vector axis = Vector{0.0, 0.0, 0.0};
    Vector to = Vector{0.0, 0.0, 0.0};
    Float ang = 0.0;
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 hand = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_13 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_14 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_15 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_16 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float jab = 0.0;
    Int side = 0;
    Float j = 0.0;
    Mat4 hand_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_17 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_18 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_19 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 ctl = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_20 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_21 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_22 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_23 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 chosen = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float u = 0.0;
    Float px = 0.0;
    Float py = 0.0;
    Float pz = 0.0;
    Float rx = 0.0;
    Float ry = 0.0;
    Float rz = 0.0;
    Float k = 0.0;
    Float k_2 = 0.0;
    Float k_3 = 0.0;
    Mat4 ctl_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_24 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_25 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_26 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_27 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_28 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_29 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int item_7 = 0;
    Int item_8 = 0;
    Fixed<Int, 2> items_3 = Fixed<Int, 2>{};
    Bool skip_2 = false;
    Int id_3 = 0;
    Bool update_instances = false;
    Bool set_visible_4 = false;
    Mat4 lh = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 fm = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_30 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_31 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_32 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool update_instances_2 = false;
    Bool set_visible_5 = false;
    renderer = engine_engine_renderer(engine_);
    r = renderer;
    if (logic_not(look_.vm_built)) {
      look_.vm_built = true;
      for (auto it14680083 = flow_for(n14680083, 0, 1); it14680083.next();) {
        arms = math_compare<"==">(kern::to<Float>(n14680083.index), 0.0);
        for (auto it14680084 = flow_for(n14680084, 0, 3); it14680084.next();) {
          body_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
          glow = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
          selected = n14680084.index;
          if (math_compare<"==">(kern::to<Float>(selected), 0.0)) {
            game_look_cpp::boomstick(body_2, glow, arms);
          } else if (math_compare<"==">(kern::to<Float>(selected), 1.0)) {
            game_look_cpp::rocket_launcher(body_2, glow, arms);
          } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
            game_look_cpp::fist(body_2, arms);
          } else {
            game_look_cpp::knife(body_2, glow, arms);
          }
          which = math_compare<"!=">(kern::to<Float>(n14680083.index), 0.0);
          if (which) {
            look_view_models_make(body_2, look_.vm_body_vr[n14680084.index], r, n14680083.index, false);
          } else {
            look_view_models_make(body_2, look_.vm_body[n14680084.index], r, n14680083.index, false);
          }
          which_2 = math_compare<"!=">(kern::to<Float>(n14680083.index), 0.0);
          if (which_2) {
            look_view_models_make(glow, look_.vm_glow_vr[n14680084.index], r, n14680083.index, true);
          } else {
            look_view_models_make(glow, look_.vm_glow[n14680084.index], r, n14680083.index, true);
          }
        }
      }
    }
    vr_probe = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    n14680087 = vr_hand_matrix(vr_probe, 1);
    vr_hand = n14680087.Result;
    both = logic_and(logic_and(world_placed_, enum_equal<Mode>(mode_, Mode::foot)), logic_not(player_dead_));
    if (both) {
      hud_2 = engine_game_world_hud(world_);
      both = math_compare<">">(struct_break<game_Hud>(hud_2).health, 0.0);
    }
    want = kern::to<Int>(kern::formula([](Float both, Float a) -> Float { return ((both != 0.0) ? a : (-1.0)); }, kern::to<Float>(both), kern::to<Float>(game_look_cpp::vm_index(foot_weapon_).Result)));
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    wid = enum_index<weapons_Id>(foot_weapon_);
    if (math_compare<"!=">(kern::to<Float>(wid), kern::to<Float>(look_.vm_prev_weapon))) {
      look_.vm_prev_weapon = wid;
      look_.vm_switch_at = now;
    } else if (logic_and(math_compare<">">(next_attack_, kern::formula([](Float vm_prev_next_attack) -> Float { return (vm_prev_next_attack + 0.0001); }, look_.vm_prev_next_attack)), math_compare<">">(kern::formula([](Float now, Float vm_switch_at) -> Float { return (now - vm_switch_at); }, now, look_.vm_switch_at), 0.05))) {
      look_.vm_fire_at = now;
      look_.vm_punch = kern::to<Int>(kern::formula([](Float vm_punch) -> Float { return kern::math::bxor(vm_punch, 1.0); }, kern::to<Float>(look_.vm_punch)));
    }
    look_.vm_prev_next_attack = next_attack_;
    for (auto it14680088 = flow_for(n14680088, 0, 3); it14680088.next();) {
      item = kern::to<Int>(kern::formula([](Float vr_hand, Float a, Float a_2) -> Float { return ((vr_hand != 0.0) ? a : a_2); }, kern::to<Float>(vr_hand), kern::to<Float>(look_.vm_body[n14680088.index]), kern::to<Float>(look_.vm_body_vr[n14680088.index])));
      item_2 = kern::to<Int>(kern::formula([](Float vr_hand, Float a, Float a_2) -> Float { return ((vr_hand != 0.0) ? a : a_2); }, kern::to<Float>(vr_hand), kern::to<Float>(look_.vm_glow[n14680088.index]), kern::to<Float>(look_.vm_glow_vr[n14680088.index])));
      items = list_make<Int, "2">(item, item_2);
      for (auto it14680089 = flow_for(n14680089, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(items))))); it14680089.next();) {
        id = items[n14680089.index];
        if (math_compare<"!=">(kern::to<Float>(id), 0.0)) {
          engine_renderer_set_visible(n14680090, r, id, false);
          set_visible = n14680090.result;
        }
      }
      if (math_compare<"!=">(kern::to<Float>(n14680088.index), kern::to<Float>(want))) {
        item_3 = look_.vm_body[n14680088.index];
        item_4 = look_.vm_glow[n14680088.index];
        item_5 = look_.vm_body_vr[n14680088.index];
        item_6 = look_.vm_glow_vr[n14680088.index];
        items_2 = list_make<Int, "4">(item_3, item_4, item_5, item_6);
        for (auto it14680091 = flow_for(n14680091, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(items_2))))); it14680091.next();) {
          id_2 = items_2[n14680091.index];
          if (math_compare<"!=">(kern::to<Float>(id_2), 0.0)) {
            engine_renderer_set_visible(n14680092, r, id_2, false);
            set_visible_2 = n14680092.result;
          }
        }
      }
    }
    look_.vm_shown = want;
    if (math_compare<"<">(kern::to<Float>(want), 0.0)) {
      return;
    }
    meshes = engine_renderer_meshes(r);
    list = meshes;
    for (auto it14680093 = flow_for(n14680093, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(list))))); it14680093.next();) {
      m = list[n14680093.index];
      skip = false;
      engine_mesh_visible(n14680094, m);
      either = logic_not(n14680094.result);
      if (logic_not(either)) {
        engine_mesh_material(n14680096, m);
        either = logic_not(n14680096.result.viewmodel);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        engine_mesh_id(n14680097, m);
        mine = math_compare<"==">(kern::to<Float>(n14680097.result), kern::to<Float>(look_.streak_mesh));
        for (auto it14680098 = flow_for(n14680098, 0, 3); it14680098.next();) {
          either_2 = mine;
          if (logic_not(either_2)) {
            engine_mesh_id(n14680100, m);
            either_2 = math_compare<"==">(kern::to<Float>(n14680100.result), kern::to<Float>(look_.vm_body[n14680098.index]));
          }
          either_3 = either_2;
          if (logic_not(either_3)) {
            engine_mesh_id(n14680102, m);
            either_3 = math_compare<"==">(kern::to<Float>(n14680102.result), kern::to<Float>(look_.vm_glow[n14680098.index]));
          }
          mine = either_3;
        }
        if (logic_not(mine)) {
          arg = r;
          engine_mesh_id(n14680103, m);
          engine_renderer_set_visible(n14680104, arg, n14680103.result, false);
          set_visible_3 = n14680104.result;
        }
      }
    }
    the_actor_velocity = engine_game_world_actor_velocity(world_, soldier_);
    v = the_actor_velocity;
    sp = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(v).x, vector_break(v).y);
    look_.vm_bob = kern::formula([](Float vm_bob, Float dt, Float sp) -> Float { return (vm_bob + ((dt * sp) * 1.6)); }, look_.vm_bob, dt, sp);
    look_.vm_bob_w = kern::formula([](Float vm_bob_w, Float sp, Float the_result) -> Float { return (vm_bob_w + ((kern::math::min(1.0, kern::math::div(sp, 5.5)) - vm_bob_w) * the_result)); }, look_.vm_bob_w, sp, common::damp_alpha(8.0, dt).Result);
    bx = kern::formula([](Float vm_bob, Float vm_bob_w) -> Float { return ((kern::math::sin(vm_bob) * 0.014) * vm_bob_w); }, look_.vm_bob, look_.vm_bob_w);
    bz = kern::formula([](Float vm_bob, Float vm_bob_w) -> Float { return (((-kern::math::cos((2.0 * vm_bob))) * 0.017) * vm_bob_w); }, look_.vm_bob, look_.vm_bob_w);
    raise = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float vm_switch_at) -> Float { return kern::math::div((now - vm_switch_at), 0.25); }, now, look_.vm_switch_at), 0.0, 1.0));
    drop = kern::formula([](Float raise) -> Float { return (((1.0 - raise) * (1.0 - raise)) * 0.25); }, raise);
    age = kern::formula([](Float now, Float vm_fire_at) -> Float { return (now - vm_fire_at); }, now, look_.vm_fire_at);
    cam = game_look_cpp::camera_frame(cam_).Result;
    t = List<Float>{};
    selected_2 = want;
    if (logic_or(math_compare<"==">(kern::to<Float>(selected_2), 0.0), math_compare<"==">(kern::to<Float>(selected_2), 1.0))) {
      kick = kern::formula([](Float age, Float kPi) -> Float { return (((age >= 0.0) && (age < 0.22)) ? kern::math::sin((kern::math::div(age, 0.22) * kPi)) : 0.0); }, age, kPi);
      rkt = math_compare<"==">(kern::to<Float>(want), 1.0);
      a = engine_math_math::Mat4_translate(vector_make(kern::formula([](Float rkt, Float bx) -> Float { return (((rkt != 0.0) ? 0.285 : 0.245) + (bx * 0.45)); }, kern::to<Float>(rkt), bx), kern::formula([](Float rkt, Float bz, Float drop) -> Float { return ((((rkt != 0.0) ? (-0.245) : (-0.2)) + (bz * 0.2)) - drop); }, kern::to<Float>(rkt), bz, drop), kern::formula([](Float rkt) -> Float { return ((rkt != 0.0) ? (-0.43) : (-0.47)); }, kern::to<Float>(rkt)))).Result;
      a_2 = game_look_cpp::euler(-0.03, -0.06, -0.018).Result;
      g = engine_math_math::Mat4_mul(a, a_2).Result;
      f = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{1.0, 0.0, 0.0});
      rr = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(f, cam_.up)), 9.999999717180685e-10), vector_normalize(vector_cross(f, cam_.up)), Vector{0.0, -1.0, 0.0});
      uu = vector_cross(rr, f);
      if (logic_or(math_compare<">">(kern::formula([](Float now, Float vm_rest_at) -> Float { return (now - vm_rest_at); }, now, look_.vm_rest_at), 0.16), math_compare<"<">(now, look_.vm_rest_at))) {
        look_.vm_rest_at = now;
        engine_game_world_physics(n14680105, world_);
        physics = n14680105.result;
        h = common::cast(physics, cam_.eye, f, 180.0, body(soldier_).Result, Int64{0xffffffffffffffff}).Result;
        look_.vm_rest_depth = kern::formula([](Float hit, Float a) -> Float { return ((hit != 0.0) ? (a < 1.2 ? 1.2 : (180.0 < a ? 180.0 : a)) : 30.0); }, kern::to<Float>(h.hit), vector_length(vector_sub(h.point, cam_.eye)));
      }
      want_2 = vector_make(0.0, 0.0, kern::formula([](Float vm_rest_depth) -> Float { return (-vm_rest_depth); }, look_.vm_rest_depth));
      if (math_compare<"<">(now, look_.vm_aim_until)) {
        d = vector_sub(look_.vm_aim_point, cam_.eye);
        want_2 = vector_make(vector_dot(d, rr), vector_dot(d, uu), kern::formula([](Float a) -> Float { return (-a); }, vector_dot(d, f)));
        if (look_.vm_aim_snap) {
          look_.vm_aim_smooth = want_2;
        }
      }
      look_.vm_aim_snap = false;
      look_.vm_aim_smooth = vector_add(look_.vm_aim_smooth, vector_scale(vector_sub(want_2, look_.vm_aim_smooth), common::damp_alpha(22.0, dt).Result));
      grip = engine_math_math::Mat4_transform_point(g, Vector{0.0, 0.0, 0.0}).Result;
      axis = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(engine_math_math::Mat4_transform_point(g, Vector{0.0, 0.0, -1.0}).Result, grip)), 9.999999717180685e-10), vector_normalize(vector_sub(engine_math_math::Mat4_transform_point(g, Vector{0.0, 0.0, -1.0}).Result, grip)), Vector{0.0, 0.0, -1.0});
      to = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(look_.vm_aim_smooth, grip)), 9.999999717180685e-10), vector_normalize(vector_sub(look_.vm_aim_smooth, grip)), Vector{0.0, 0.0, -1.0});
      ang = kern::formula([](Float a) -> Float { return kern::math::acos((a < (-1.0) ? (-1.0) : (1.0 < a ? 1.0 : a))); }, vector_dot(axis, to));
      if (math_compare<">">(ang, 0.0001)) {
        a_5 = engine_math_math::Mat4_translate(grip).Result;
        a_6 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(logic_select<Vector>(math_compare<">">(vector_length(vector_cross(axis, to)), 9.999999717180685e-10), vector_normalize(vector_cross(axis, to)), Vector{1.0, 0.0, 0.0}), ang).Result, Vector{1.0, 1.0, 1.0}).Result;
        a_4 = engine_math_math::Mat4_mul(a_5, a_6).Result;
        a_7 = engine_math_math::Mat4_translate(vector_scale(grip, -1.0)).Result;
        a_3 = engine_math_math::Mat4_mul(a_4, a_7).Result;
        g = engine_math_math::Mat4_mul(a_3, g).Result;
      }
      hand = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
      n14680106 = vr_hand_matrix(hand, 1);
      if (n14680106.Result) {
        a_10 = engine_math_math::Mat4_translate(vector_make(0.0, 0.0, kern::formula([](Float kick) -> Float { return (kick * 0.08); }, kick))).Result;
        a_9 = engine_math_math::Mat4_mul(hand, a_10).Result;
        a_11 = game_look_cpp::euler(kern::formula([](Float kick) -> Float { return (kick * 0.12); }, kick), 0.0, 0.0).Result;
        a_8 = engine_math_math::Mat4_mul(a_9, a_11).Result;
        look_view_models_put(t, a_8);
      } else {
        a_14 = engine_math_math::Mat4_mul(cam, g).Result;
        a_15 = engine_math_math::Mat4_translate(vector_make(0.0, 0.0, kern::formula([](Float kick) -> Float { return (kick * 0.08); }, kick))).Result;
        a_13 = engine_math_math::Mat4_mul(a_14, a_15).Result;
        a_16 = game_look_cpp::euler(kern::formula([](Float kick) -> Float { return (kick * 0.12); }, kick), 0.0, 0.0).Result;
        a_12 = engine_math_math::Mat4_mul(a_13, a_16).Result;
        look_view_models_put(t, a_12);
      }
    } else if (math_compare<"==">(kern::to<Float>(selected_2), 2.0)) {
      jab = kern::formula([](Float age, Float kPi) -> Float { return (((age >= 0.0) && (age < 0.3)) ? kern::math::sin((kern::math::div(age, 0.3) * kPi)) : 0.0); }, age, kPi);
      side = -1;
      while (loop(math_compare<"<=">(kern::to<Float>(side), 1.0))) {
        j = kern::formula([](Float side, Float vm_punch, Float jab) -> Float { return (((side > 0.0) == (vm_punch == 1.0)) ? jab : 0.0); }, kern::to<Float>(side), kern::to<Float>(look_.vm_punch), jab);
        a_18 = engine_math_math::Mat4_translate(vector_make(kern::formula([](Float side, Float bx) -> Float { return ((side * 0.17) + (bx * 0.4)); }, kern::to<Float>(side), bx), kern::formula([](Float bz, Float drop, Float j) -> Float { return ((((-0.22) + (bz * 0.2)) - drop) + (j * 0.05)); }, bz, drop, j), kern::formula([](Float j) -> Float { return ((-0.42) - (j * 0.24)); }, j))).Result;
        a_17 = engine_math_math::Mat4_mul(cam, a_18).Result;
        a_19 = game_look_cpp::euler(0.1, kern::formula([](Float side, Float j) -> Float { return (((-side) * 0.12) * j); }, kern::to<Float>(side), j), 0.0).Result;
        hand_2 = engine_math_math::Mat4_mul(a_17, a_19).Result;
        ctl = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
        n14680107 = vr_hand_matrix(ctl, kern::to<Int>(kern::formula([](Float side) -> Float { return ((side > 0.0) ? 1.0 : 0.0); }, kern::to<Float>(side))));
        if (n14680107.Result) {
          a_20 = game_look_cpp::euler(0.25, 0.0, 0.0).Result;
          hand_2 = engine_math_math::Mat4_mul(ctl, a_20).Result;
        }
        if (math_compare<"<">(kern::to<Float>(side), 0.0)) {
          a_23 = engine_math_math::Mat4_scale(Vector{-1.0, 1.0, 1.0}).Result;
          chosen = engine_math_math::Mat4_mul(hand_2, a_23).Result;
        } else {
          chosen = hand_2;
        }
        a_21 = chosen;
        look_view_models_put(t, a_21);
        side += 2;
      }
    } else {
      u = kern::formula([](Float age) -> Float { return (((age >= 0.0) && (age < 0.38)) ? kern::math::div(age, 0.38) : 1.0); }, age);
      px = 0.0;
      py = 0.0;
      pz = 0.0;
      rx = 0.0;
      ry = 0.0;
      rz = 0.0;
      if (math_compare<"<">(u, 1.0)) {
        if (math_compare<"<">(u, 0.18)) {
          k = look_view_models_ease(kern::formula([](Float u) -> Float { return kern::math::div(u, 0.18); }, u)).Result;
          px = kern::formula([](Float k) -> Float { return (0.06 * k); }, k);
          py = kern::formula([](Float k) -> Float { return (0.09 * k); }, k);
          pz = kern::formula([](Float k) -> Float { return (0.05 * k); }, k);
          rz = kern::formula([](Float k) -> Float { return ((-0.5) * k); }, k);
          ry = kern::formula([](Float k) -> Float { return ((-0.25) * k); }, k);
        } else if (math_compare<"<">(u, 0.53)) {
          k_2 = look_view_models_ease(kern::formula([](Float u) -> Float { return kern::math::div((u - 0.18), 0.35); }, u)).Result;
          px = kern::formula([](Float k_2) -> Float { return (0.06 - (0.42 * k_2)); }, k_2);
          py = kern::formula([](Float k_2) -> Float { return (0.09 - (0.17 * k_2)); }, k_2);
          pz = kern::formula([](Float k_2) -> Float { return (0.05 - (0.2 * k_2)); }, k_2);
          rz = kern::formula([](Float k_2) -> Float { return ((-0.5) + (1.4 * k_2)); }, k_2);
          ry = kern::formula([](Float k_2) -> Float { return ((-0.25) + (1.2 * k_2)); }, k_2);
          rx = kern::formula([](Float k_2) -> Float { return ((-0.35) * k_2); }, k_2);
        } else {
          k_3 = look_view_models_ease(kern::formula([](Float u) -> Float { return kern::math::div((u - 0.53), 0.47); }, u)).Result;
          px = kern::formula([](Float k_3) -> Float { return ((-0.36) * (1.0 - k_3)); }, k_3);
          py = kern::formula([](Float k_3) -> Float { return ((-0.08) * (1.0 - k_3)); }, k_3);
          pz = kern::formula([](Float k_3) -> Float { return ((-0.15) * (1.0 - k_3)); }, k_3);
          rz = kern::formula([](Float k_3) -> Float { return (0.9 * (1.0 - k_3)); }, k_3);
          ry = kern::formula([](Float k_3) -> Float { return (0.95 * (1.0 - k_3)); }, k_3);
          rx = kern::formula([](Float k_3) -> Float { return ((-0.35) * (1.0 - k_3)); }, k_3);
        }
      }
      ctl_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
      n14680108 = vr_hand_matrix(ctl_2, 1);
      if (n14680108.Result) {
        a_25 = game_look_cpp::euler(0.25, 0.0, 0.0).Result;
        a_24 = engine_math_math::Mat4_mul(ctl_2, a_25).Result;
        look_view_models_put(t, a_24);
      } else {
        a_28 = engine_math_math::Mat4_translate(vector_make(kern::formula([](Float px, Float bx) -> Float { return ((0.2 + px) + (bx * 0.4)); }, px, bx), kern::formula([](Float py, Float bz, Float drop) -> Float { return ((((-0.21) + py) + (bz * 0.2)) - drop); }, py, bz, drop), kern::formula([](Float pz) -> Float { return ((-0.4) + pz); }, pz))).Result;
        a_27 = engine_math_math::Mat4_mul(cam, a_28).Result;
        a_29 = game_look_cpp::euler(rx, ry, rz).Result;
        a_26 = engine_math_math::Mat4_mul(a_27, a_29).Result;
        look_view_models_put(t, a_26);
      }
    }
    item_7 = kern::to<Int>(kern::formula([](Float vr_hand, Float a, Float a_2) -> Float { return ((vr_hand != 0.0) ? a : a_2); }, kern::to<Float>(vr_hand), kern::to<Float>(look_.vm_body_vr[want]), kern::to<Float>(look_.vm_body[want])));
    item_8 = kern::to<Int>(kern::formula([](Float vr_hand, Float a, Float a_2) -> Float { return ((vr_hand != 0.0) ? a : a_2); }, kern::to<Float>(vr_hand), kern::to<Float>(look_.vm_glow_vr[want]), kern::to<Float>(look_.vm_glow[want])));
    items_3 = list_make<Int, "2">(item_7, item_8);
    for (auto it14680109 = flow_for(n14680109, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(items_3))))); it14680109.next();) {
      id_3 = items_3[n14680109.index];
      skip_2 = false;
      if (logic_not(math_compare<"!=">(kern::to<Float>(id_3), 0.0))) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        engine_renderer_update_instances(n14680110, r, id_3, t, List<Float>{});
        update_instances = n14680110.result;
        engine_renderer_set_visible(n14680111, r, id_3, true);
        set_visible_4 = n14680111.result;
      }
    }
    if (logic_and(logic_and(vr_hand, math_compare<"==">(kern::to<Float>(want), 3.0)), math_compare<"!=">(kern::to<Float>(look_.vm_body_vr[2]), 0.0))) {
      lh = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
      n14680112 = vr_hand_matrix(lh, 0);
      if (n14680112.Result) {
        a_31 = game_look_cpp::euler(0.25, 0.0, 0.0).Result;
        a_30 = engine_math_math::Mat4_mul(lh, a_31).Result;
        a_32 = engine_math_math::Mat4_scale(Vector{-1.0, 1.0, 1.0}).Result;
        fm = engine_math_math::Mat4_mul(a_30, a_32).Result;
        engine_renderer_update_instances(n14680113, r, look_.vm_body_vr[2], fm.m, List<Float>{});
        update_instances_2 = n14680113.result;
        engine_renderer_set_visible(n14680114, r, look_.vm_body_vr[2], true);
        set_visible_5 = n14680114.result;
      }
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
    n15728650 = matrix_of(a);
    a_2 = n15728650.Result;
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
    for (auto it16777226 = flow_for(n16777226, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Opt>(list))))); it16777226.next();) {
      w = list[n16777226.index];
      if (enum_equal<Opt>(w, o)) {
        return {true};
      }
    }
    return {false};
  }

  struct remote_on_segment_zonesResult { Bool Result = false; };
  remote_on_segment_zonesResult remote_on_segment_zones(Float& t_hit, mp_Zone& z, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float speed = 0.0) {
    BodyPart part = BodyPart::none;
    n16777232 = person_hit::hit(t_hit, feet, yaw, speed, from, seg, 1.16, 1.0);
    part = n16777232.Result;
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
    for (auto it16777233 = flow_for(n16777233, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it16777233.next();) {
      skip = false;
      if (math_compare<">">(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return (kern::math::band(a, kern::math::bor(a_2, a_3)) != 0.0); }, kern::to<Float>(net_.remotes_[n16777233.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead))), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_and(gaps_riding().Result, math_compare<"==">(kern::to<Float>(net_.remotes_[n16777233.index].state.slot), kern::to<Float>(gaps_[0].ride_owner)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (enum_equal<net_PlayerMode>(net_.remotes_[n16777233.index].state.mode, net_PlayerMode::foot)) {
            t = 2.0;
            z = mp_Zone::torso;
            feet = logic_select<Vector>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n16777233.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated)))), 0.5), vector_sub(net_.remotes_[n16777233.index].person_shown, vector_make(0.0, 0.0, kSeatDrop)), net_.remotes_[n16777233.index].person_shown);
            n16777234 = remote_on_segment_zones(t, z, from, seg, feet, net_.remotes_[n16777233.index].state.avatar_yaw, net_.remotes_[n16777233.index].state.speed);
            if (logic_and(n16777234.Result, math_compare<"<">(t, best_t))) {
              best_t = t;
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n16777233.index)));
              best_zone = z;
            }
            skip = true;
          }
          if (logic_not(skip)) {
            if (enum_equal<net_PlayerMode>(net_.remotes_[n16777233.index].state.mode, net_PlayerMode::drone)) {
              t_2 = 2.0;
              z_2 = mp_Zone::torso;
              n16777235 = remote_on_segment_zones(t_2, z_2, from, seg, net_.remotes_[n16777233.index].person_shown, net_.remotes_[n16777233.index].state.avatar_yaw, 0.0);
              if (logic_and(n16777235.Result, math_compare<"<">(t_2, best_t))) {
                best_t = t_2;
                best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n16777233.index)));
                best_zone = z_2;
              }
            }
            if (enum_equal<net_PlayerMode>(net_.remotes_[n16777233.index].state.mode, net_PlayerMode::parachute)) {
              t_3 = 2.0;
              z_3 = mp_Zone::torso;
              n16777236 = remote_on_segment_zones(t_3, z_3, from, seg, vector_sub(net_.remotes_[n16777233.index].shown, Vector{0.0, 0.0, 1.75}), net_.remotes_[n16777233.index].state.yaw, 0.0);
              if (logic_and(n16777236.Result, math_compare<"<">(t_3, best_t))) {
                best_t = t_3;
                best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n16777233.index)));
                best_zone = z_3;
              }
            }
            person = enum_equal<net_PlayerMode>(net_.remotes_[n16777233.index].state.mode, net_PlayerMode::foot);
            c = logic_select<Vector>(person, vector_add(net_.remotes_[n16777233.index].person_shown, vector_make(0.0, 0.0, kern::formula([](Float a, Float a_2, Float kSeatDrop) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? (0.73 - kSeatDrop) : 0.73); }, kern::to<Float>(net_.remotes_[n16777233.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated))), kSeatDrop))), net_.remotes_[n16777233.index].shown);
            radius = 0.522;
            switch (flow_switch_enum<net_PlayerMode>(net_.remotes_[n16777233.index].state.mode)) {
              case net_PlayerMode::foot: {
                c = vector_add(c, Vector{0.0, 0.0, 0.2});
              } break;
              case net_PlayerMode::drone: {
                radius = 0.3;
              } break;
              case net_PlayerMode::car: {
                radius = kern::formula([](Float a, Float a_2) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? 3.2 : 2.0); }, kern::to<Float>(net_.remotes_[n16777233.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kBus))));
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
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n16777233.index)));
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
    for (auto it17825802 = flow_for(n17825802, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it17825802.next();) {
      if (logic_and(logic_and(logic_and(projectiles_[n17825802.index].ghost, projectiles_[n17825802.index].alive), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::shr(a, 16.0); }, kern::to<Float>(projectiles_[n17825802.index].net))))), kern::to<Float>(owner))), math_compare<"<">(vector_length(vector_sub(projectiles_[n17825802.index].pos, at)), 8.0))) {
        projectiles_[n17825802.index].alive = false;
        key = projectiles_[n17825802.index].net;
        map_remove(n17825803, projsync_[0].ghosts, key);
      }
    }
  }

  struct projsync_update_matchResult { Bool Result = false; };
  KERN_PURE projsync_update_matchResult projsync_update_match(const Int_Int_pair& projsync_0_sent_n17825808_index, const Game_projsync_update_Rec& r) {
    return {math_compare<"==">(kern::to<Float>(r.id), kern::to<Float>(projsync_0_sent_n17825808_index.first))};
  }

  struct gaps_ridingResult { Bool Result = false; };
  KERN_PURE gaps_ridingResult gaps_riding() {
    return {logic_and(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0), gaps_[0].riding)};
  }

  struct gaps_ride_carResult { Bool Result = false; };
  KERN_PURE gaps_ride_carResult gaps_ride_car() {
    return {logic_and(logic_and(math_compare<">">(kern::to<Float>(list_length<GapsState>(gaps_)), 0.0), gaps_[0].riding), gaps_[0].ride_car)};
  }

  void swarm_clear() {
    for (auto it19922954 = flow_for(n19922954, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Game_RemoteSwarm>(swarms_))))); it19922954.next();) {
      for (auto it19922955 = flow_for(n19922955, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_RemoteUnit>(swarms_[n19922954.index].units))))); it19922955.next();) {
        if (math_compare<">=">(kern::to<Float>(swarms_[n19922954.index].units[n19922955.index].person), 0.0)) {
          people_.people_[swarms_[n19922954.index].units[n19922955.index].person].state = People_State::gone;
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
  KERN_PURE swarm_collect_matchResult swarm_collect_match(const Game_SwarmSent& swarm_sent_n19922972_index, const Game_SwarmSent& s) {
    return {logic_and(math_compare<"==">(kern::to<Float>(s.unit.id), kern::to<Float>(swarm_sent_n19922972_index.unit.id)), enum_equal<net_SwarmKind>(s.unit.kind, swarm_sent_n19922972_index.unit.kind))};
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
    for (auto it19922956 = flow_for(n19922956, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<ZombieAi>(combat_.zombie_ai))))); it19922956.next();) {
      skip = false;
      if (math_compare<"<">(kern::to<Float>(combat_.zombie_ai[n19922956.index].person), 0.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.zombie_ai[n19922956.index].id), 0.0))) {
          n19922957 = Game_swarm_collect_next_id();
          combat_.zombie_ai[n19922956.index].id = n19922957.Result;
        }
        list_add(out, struct_make<net_SwarmUnit>(combat_.zombie_ai[n19922956.index].id, net_SwarmKind::zombie, kern::to<Int>(kern::formula([](Float st, Float a) -> Float { return kern::math::band((((st - a) < 0.6) ? 1.0 : 0.0), 255.0); }, st, combat_.zombie_ai[n19922956.index].swing)), people_.people_[combat_.zombie_ai[n19922956.index].person].pos, common::wrap_pi(people_.people_[combat_.zombie_ai[n19922956.index].person].yaw).Result, people_.people_[combat_.zombie_ai[n19922956.index].person].v));
      }
    }
    for (auto it19922958 = flow_for(n19922958, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Heli>(combat_.helis))))); it19922958.next();) {
      skip_2 = false;
      if (combat_.helis[n19922958.index].dead) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.helis[n19922958.index].id), 0.0))) {
          n19922959 = Game_swarm_collect_next_id();
          combat_.helis[n19922958.index].id = n19922959.Result;
        }
        list_add(out, struct_make<net_SwarmUnit>(combat_.helis[n19922958.index].id, net_SwarmKind::heli, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(((a != 0.0) ? 1.0 : 0.0), 255.0); }, kern::to<Float>(enum_equal<Heli_State>(combat_.helis[n19922958.index].state, Heli_State::down)))), combat_.helis[n19922958.index].p, common::wrap_pi(combat_.helis[n19922958.index].yaw).Result, 0.0));
      }
    }
    for (auto it19922960 = flow_for(n19922960, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Tank>(combat_.tanks))))); it19922960.next();) {
      skip_3 = false;
      if (combat_.tanks[n19922960.index].dead) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.tanks[n19922960.index].id), 0.0))) {
          n19922961 = Game_swarm_collect_next_id();
          combat_.tanks[n19922960.index].id = n19922961.Result;
        }
        list_add(out, struct_make<net_SwarmUnit>(combat_.tanks[n19922960.index].id, net_SwarmKind::tank, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(((a != 0.0) ? 1.0 : 0.0), 255.0); }, kern::to<Float>(combat_.tanks[n19922960.index].wreck))), combat_.tanks[n19922960.index].p, common::wrap_pi(combat_.tanks[n19922960.index].yaw).Result, common::wrap_pi(combat_.tanks[n19922960.index].turret).Result));
      }
    }
    for (auto it19922962 = flow_for(n19922962, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<SoldierAi>(combat_.soldiers))))); it19922962.next();) {
      skip_4 = false;
      if (math_compare<"<">(kern::to<Float>(combat_.soldiers[n19922962.index].person), 0.0)) {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.soldiers[n19922962.index].id), 0.0))) {
          n19922963 = Game_swarm_collect_next_id();
          combat_.soldiers[n19922962.index].id = n19922963.Result;
        }
        if (logic_or(enum_equal<People_State>(people_.people_[combat_.soldiers[n19922962.index].person].state, People_State::down), enum_equal<People_State>(people_.people_[combat_.soldiers[n19922962.index].person].state, People_State::gone))) {
          skip_4 = true;
        }
        if (logic_not(skip_4)) {
          anim = kern::to<Int>(kern::formula([](Float a, Float st, Float a_2, Float a_3) -> Float { return kern::math::band((((a != 0.0) || ((st - a_2) < 0.5)) ? 1.0 : ((a_3 > 3.0) ? 2.0 : ((a_3 > 0.0) ? 3.0 : 0.0))), 255.0); }, kern::to<Float>(combat_.soldiers[n19922962.index].los), st, combat_.soldiers[n19922962.index].last_shot, people_.people_[combat_.soldiers[n19922962.index].person].v));
          list_add(out, struct_make<net_SwarmUnit>(combat_.soldiers[n19922962.index].id, net_SwarmKind::soldier, anim, people_.people_[combat_.soldiers[n19922962.index].person].pos, common::wrap_pi(people_.people_[combat_.soldiers[n19922962.index].person].yaw).Result, people_.people_[combat_.soldiers[n19922962.index].person].v));
        }
      }
    }
    for (auto it19922964 = flow_for(n19922964, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it19922964.next();) {
      skip_5 = false;
      if (math_compare<"<">(kern::to<Float>(units_[n19922964.index].car), 0.0)) {
        skip_5 = true;
      }
      if (logic_not(skip_5)) {
        for (auto it19922965 = flow_for(n19922965, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit_Cop>(units_[n19922964.index].cops))))); it19922965.next();) {
          skip_6 = false;
          if (logic_or(math_compare<"<">(kern::to<Float>(units_[n19922964.index].cops[n19922965.index].person), 0.0), units_[n19922964.index].cops[n19922965.index].inside)) {
            skip_6 = true;
          }
          if (logic_not(skip_6)) {
            p_at = units_[n19922964.index].cops[n19922965.index].person;
            if (logic_or(logic_or(math_compare<"!=">(kern::to<Float>(people_.people_[p_at].seed), kern::to<Float>(units_[n19922964.index].cops[n19922965.index].seed)), enum_equal<People_State>(people_.people_[p_at].state, People_State::down)), enum_equal<People_State>(people_.people_[p_at].state, People_State::gone))) {
              skip_6 = true;
            }
            if (logic_not(skip_6)) {
              if (logic_not(math_compare<"!=">(kern::to<Float>(units_[n19922964.index].cops[n19922965.index].id), 0.0))) {
                n19922966 = Game_swarm_collect_next_id();
                units_[n19922964.index].cops[n19922965.index].id = n19922966.Result;
              }
              flags = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::band(kern::math::bor(kern::math::wrap32(((a != 0.0) ? a_2 : 0.0)), kern::math::wrap32(kern::math::shl(kern::math::mod(a_3, 5.0), a_2))), 255.0); }, kern::to<Float>(people_.people_[p_at].aiming), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims))), kern::to<Float>(units_[n19922964.index].cops[n19922965.index].seed)));
              list_add(out, struct_make<net_SwarmUnit>(units_[n19922964.index].cops[n19922965.index].id, net_SwarmKind::officer, flags, people_.people_[p_at].pos, common::wrap_pi(people_.people_[p_at].yaw).Result, people_.people_[p_at].v));
              list_add(sent, struct_make<Game_SwarmSent>(out[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_SwarmUnit>(out))))], units_[n19922964.index].cops[n19922965.index].person, people_.people_[p_at].seed));
            }
          }
        }
      }
    }
    for (auto it19922967 = flow_for(n19922967, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it19922967.next();) {
      skip_7 = false;
      either = logic_or(logic_not(cars_[n19922967.index].police), logic_not(cars_[n19922967.index].placed));
      if (logic_not(either)) {
        actor_alive_2 = engine_game_world_actor_alive(world_, cars_[n19922967.index].actor);
        either = logic_not(actor_alive_2);
      }
      if (logic_or(either, logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(car_index_), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n19922967.index))))))))) {
        skip_7 = true;
      }
      if (logic_not(skip_7)) {
        n19922968 = state_of(cars_[n19922967.index].actor);
        s = n19922968.Result;
        f = engine_math_math::Quat_rotate(s.rotation, Vector{1.0, 0.0, 0.0}).Result;
        lights = logic_and(logic_and(logic_and(logic_and(math_compare<">=">(kern::to<Float>(cars_[n19922967.index].unit), 0.0), math_compare<"<">(kern::to<Float>(cars_[n19922967.index].unit), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<PoliceUnit>(units_)))))))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n19922967.index].unit].state, PoliceUnit_State::abandoned))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n19922967.index].unit].state, PoliceUnit_State::wreck))), logic_not(enum_equal<PoliceUnit_State>(units_[cars_[n19922967.index].unit].state, PoliceUnit_State::leave)));
        flags_2 = kern::to<Int>(kern::formula([](Float lights, Float a, Float a_2, Float a_3, Float a_4, Float a_5) -> Float { return kern::math::band(kern::math::bor(kern::math::bor(((lights != 0.0) ? a : 0.0), ((a_2 != 0.0) ? a_3 : 0.0)), ((a_4 >= 0.0) ? a_5 : 0.0)), 255.0); }, kern::to<Float>(lights), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims))), kern::to<Float>(cars_[n19922967.index].wreck), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserWreck))), cars_[n19922967.index].burn_until, kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserBurns)))));
        list_add(out, struct_make<net_SwarmUnit>(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::band((index + 1.0), 65535.0); }, kern::to<Float>(n19922967.index))), net_SwarmKind::cruiser, flags_2, s.position, kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(f).y, vector_break(f).x), car::Car_steer_angle(cars_[n19922967.index].car[0]).Result));
      }
    }
    for (auto it19922969 = flow_for(n19922969, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it19922969.next();) {
      if (pdrones_[n19922969.index].active) {
        list_add(out, struct_make<net_SwarmUnit>(game_swarm_cpp::drone_id(pdrones_[n19922969.index]).Result, net_SwarmKind::police_drone, kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::band(((a != 0.0) ? a_2 : 0.0), 255.0); }, kern::to<Float>(pdrones_[n19922969.index].emp), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims))))), pdrones_[n19922969.index].pos, common::wrap_pi(pdrones_[n19922969.index].yaw).Result, 0.0));
      }
    }
    for (auto it19922970 = flow_for(n19922970, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<EnemyAi>(combat_.enemies))))); it19922970.next();) {
      skip_8 = false;
      if (math_compare<"<">(kern::to<Float>(combat_.enemies[n19922970.index].person), 0.0)) {
        skip_8 = true;
      }
      if (logic_not(skip_8)) {
        p_at_2 = combat_.enemies[n19922970.index].person;
        if (logic_or(logic_or(logic_not(enum_equal<People_Kind>(people_.people_[p_at_2].kind, People_Kind::enemy)), enum_equal<People_State>(people_.people_[p_at_2].state, People_State::down)), enum_equal<People_State>(people_.people_[p_at_2].state, People_State::gone))) {
          skip_8 = true;
        }
        if (logic_not(skip_8)) {
          if (logic_not(math_compare<"!=">(kern::to<Float>(combat_.enemies[n19922970.index].id), 0.0))) {
            n19922971 = Game_swarm_collect_next_id();
            combat_.enemies[n19922970.index].id = n19922971.Result;
          }
          flags_3 = kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::band(kern::math::bor(kern::math::band(a, a_2), ((a_3 != 0.0) ? a_4 : 0.0)), 255.0); }, kern::to<Float>(enum_index<combat_EnemyType>(combat_.enemies[n19922970.index].type)), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kEnemyTypeMask))), kern::to<Float>(people_.people_[p_at_2].aiming), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserBurns)))));
          list_add(out, struct_make<net_SwarmUnit>(combat_.enemies[n19922970.index].id, net_SwarmKind::enemy, flags_3, people_.people_[p_at_2].pos, common::wrap_pi(people_.people_[p_at_2].yaw).Result, people_.people_[p_at_2].v));
          list_add(sent, struct_make<Game_SwarmSent>(out[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_SwarmUnit>(out))))], combat_.enemies[n19922970.index].person, people_.people_[p_at_2].seed));
        }
      }
    }
    for (auto it19922972 = flow_for(n19922972, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_SwarmSent>(swarm_sent_))))); it19922972.next();) {
      skip_9 = false;
      at = 0;
      end = list_length<Game_SwarmSent>(sent);
      go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      while (loop(go)) {
        if (swarm_collect_match(swarm_sent_[n19922972.index], sent[at]).Result) {
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
        if (logic_or(math_compare<"<">(kern::to<Float>(swarm_sent_[n19922972.index].person), 0.0), math_compare<">=">(kern::to<Float>(swarm_sent_[n19922972.index].person), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<People_Person>(people_.people_))))))))) {
          skip_9 = true;
        }
        if (logic_not(skip_9)) {
          if (logic_or(math_compare<"!=">(kern::to<Float>(people_.people_[swarm_sent_[n19922972.index].person].seed), kern::to<Float>(swarm_sent_[n19922972.index].seed)), logic_not(enum_equal<People_State>(people_.people_[swarm_sent_[n19922972.index].person].state, People_State::down)))) {
            skip_9 = true;
          }
          if (logic_not(skip_9)) {
            u = swarm_sent_[n19922972.index].unit;
            u.flags = kern::to<Int>(kern::formula([](Float flags, Float a) -> Float { return kern::math::band(kern::math::bor(flags, a), 255.0); }, kern::to<Float>(u.flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kUnitDead)))));
            u.p = people_.people_[swarm_sent_[n19922972.index].person].pos;
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
          var_take(n19922973, swarm_fallen_[at_2]);
          swarm_fallen_[kept] = n19922973.value;
        }
        kept += 1;
      }
      at_2 += 1;
    }
    list_resize(swarm_fallen_, kept, Game_SwarmFallen{net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, 0.0});
    for (auto it19922974 = flow_for(n19922974, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_SwarmFallen>(swarm_fallen_))))); it19922974.next();) {
      list_add(out, swarm_fallen_[n19922974.index].unit);
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
    for (auto it20971530 = flow_for(n20971530, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TossState_Own>(toss_.own))))); it20971530.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(toss_.own[n20971530.index].kind), kern::to<Float>(kind)), int64_equal(toss_.own[n20971530.index].id, net_id))) {
        toss_.own[n20971530.index].still = 0.0;
        toss_.own[n20971530.index].done_sent = 0;
        toss_.own[n20971530.index].local = local;
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
    n20971531 = life::Life_animal_of(glife_[0].life, id, b);
    a = n20971531.Result;
    if (math_compare<"==">(kern::to<Float>(a), 1.0)) {
      toss_own(kCow, -1, id);
    } else if (math_compare<"==">(kern::to<Float>(a), 2.0)) {
      toss_own(kPet, kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(id)))), id);
    }
  }

  struct toss_update_body_okResult { Bool Result = false; };
  KERN_PURE toss_update_body_okResult toss_update_body_ok(Int w = 0, physics_Body b = physics_Body{Int64{0x0}}) {
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
    Int a = 0;
    for (auto it20971534 = flow_for(n20971534, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_VrHand>(vr_hands_))))); it20971534.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kPerson)), math_compare<"==">(kern::to<Float>(vr_hands_[n20971534.index].grab_person), kern::to<Float>(o.local)))) {
        return {true};
      }
      if (logic_and(math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kBird)), math_compare<"==">(kern::to<Float>(vr_hands_[n20971534.index].bird), kern::to<Float>(o.local)))) {
        return {true};
      }
      if (logic_and(logic_or(math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kCow)), math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kPet))), engine_physics_physics::Body_valid(vr_hands_[n20971534.index].grab).Result)) {
        id = Int64{0x0};
        n20971535 = life::Life_animal_of(glife_[0].life, id, vr_hands_[n20971534.index].grab);
        a = n20971535.Result;
        if (logic_or(logic_and(logic_and(math_compare<"==">(kern::to<Float>(a), 1.0), math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kCow))), int64_equal(id, o.id)), logic_and(logic_and(math_compare<"==">(kern::to<Float>(a), 2.0), math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kPet))), int64_equal(id, o.id)))) {
          return {true};
        }
      }
    }
    return {false};
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
    for (auto it20971541 = flow_for(n20971541, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it20971541.next();) {
      skip = false;
      if (logic_not(projectiles_[n20971541.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (projectiles_[n20971541.index].ghost) {
          ghosts = kern::to<Int>(kern::formula([](Float ghosts) -> Float { return kern::math::wrap32(ghosts + 1.0); }, kern::to<Float>(ghosts)));
          gp = projectiles_[n20971541.index].pos;
        } else {
          rounds = kern::to<Int>(kern::formula([](Float rounds) -> Float { return kern::math::wrap32(rounds + 1.0); }, kern::to<Float>(rounds)));
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
    engine_game_world_physics(n22020106, world_);
    physics = n22020106.result;
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
    engine_game_world_respawn_now(n22020115, world_, soldier_);
    respawn_now = n22020115.result;
    return {respawn_now};
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
    key = r.key;
    map_add(n23068682, gaps_[0].planks, key);
    st_row = n23068682.index;
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
      for (auto it23068685 = flow_for(n23068685, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<Int>>(gaps_[0].planks))))); it23068685.next();) {
        for (auto it23068686 = flow_for(n23068686, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(gaps_[0].planks[n23068685.index]))))); it23068686.next();) {
          n = gaps_[0].planks[n23068685.index][n23068686.index];
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
    n24117258 = matrix_of(c.actor);
    a = n24117258.Result;
    p = engine_math_math::Mat4_translation(a).Result;
    return {game_traffic_cpp::lane_distance(city_.lanes_[c.lane], vector_make(vector_break(p).x, vector_break(p).y, 0.0)).Result};
  }

  struct car_towedResult { Bool Result = false; };
  KERN_PURE car_towedResult car_towed(Int car = 0) {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0))) {
      return {false};
    }
    for (auto it24117281 = flow_for(n24117281, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))))); it24117281.next();) {
      if (math_compare<"==">(kern::to<Float>(life_[0].trucks[n24117281.index].job), kern::to<Float>(car))) {
        return {true};
      }
    }
    return {false};
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
      engine_game_world_physics(n25165836, world_);
      physics_2 = n25165836.result;
      body_valid = engine_world_body_valid(physics_2, b);
      both = body_valid;
    }
    if (both) {
      engine_game_world_physics(n25165837, world_);
      physics_3 = n25165837.result;
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
    engine_renderer_allocate_id(n26214410, r_);
    allocate_id = n26214410.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = mat;
    look.visible = false;
    engine_upsert(n26214411, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n26214411.result;
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
      engine_renderer_allocate_id(n26214415, r_);
      allocate_id = n26214415.result;
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
    engine_upsert(n26214416, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n26214416.result;
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
    for (auto it26214417 = flow_for(n26214417, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it26214417.next();) {
      if (logic_and(math_compare<">=">(kern::to<Float>(units_[n26214417.index].car), 0.0), math_compare<"!=">(kern::to<Float>(units_[n26214417.index].car), kern::to<Float>(car_index_)))) {
        engine_game_world_set_alive(world_, cars_[units_[n26214417.index].car].actor, false);
        cars_[units_[n26214417.index].car].placed = false;
      }
      if (math_compare<">=">(kern::to<Float>(units_[n26214417.index].car), 0.0)) {
        cars_[units_[n26214417.index].car].unit = -1;
      }
    }
    list_clear(units_);
    for (auto it26214418 = flow_for(n26214418, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it26214418.next();) {
      pdrones_[n26214418.index].active = false;
    }
    drone_wave_live_ = false;
    next_drone_wave_ = -1.0;
    star_since_ = -1.0;
  }

  void ui_mix_cues() {
    Float fx = 0.0;
    fx = kern::formula([](Float the_result) -> Float { return kern::math::max(0.01, kern::math::div(the_result, 72.0)); }, settings::Settings_get(ui_.settings, Opt::audio_fx).Result);
    engine_game_world_sound_cues(n26214420, world_);
    for (auto it26214421 = flow_for(n26214421, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_SoundCue>(n26214420.result))))); it26214421.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(n26214420.result[n26214421.index].actor), kern::to<Float>(soldier_)), logic_or(logic_or(text_equal(n26214420.result[n26214421.index].sound, "sc:shot0"), text_equal(n26214420.result[n26214421.index].sound, "sc:pistol0")), text_equal(n26214420.result[n26214421.index].sound, "builtin:sniper")))) {
        osfx("foot.shot", n26214420.result[n26214421.index].position, kern::to<Float>(enum_index<weapons_Id>(foot_weapon_)), 0.0, 0.0);
      }
      if (logic_or(text_equal(n26214420.result[n26214421.index].sound, "builtin:step"), text_equal(n26214420.result[n26214421.index].sound, "builtin:land"))) {
        n26214420.result[n26214421.index].volume = kern::formula([](Float a, Float the_result, Float fx) -> Float { return (a * kern::math::div(kern::math::div(the_result, 30.0), fx)); }, n26214420.result[n26214421.index].volume, settings::Settings_get(ui_.settings, Opt::audio_steps).Result, fx);
      } else if (logic_or(logic_or(logic_or(logic_or(text_equal(n26214420.result[n26214421.index].sound, "builtin:pistol"), text_equal(n26214420.result[n26214421.index].sound, "builtin:smg")), text_equal(n26214420.result[n26214421.index].sound, "builtin:sniper")), text_equal(n26214420.result[n26214421.index].sound, "builtin:rifle")), text_equal(n26214420.result[n26214421.index].sound, "builtin:shotgun"))) {
        n26214420.result[n26214421.index].volume = kern::formula([](Float a, Float the_result, Float fx) -> Float { return (a * kern::math::div(kern::math::div(the_result, 82.0), fx)); }, n26214420.result[n26214421.index].volume, settings::Settings_get(ui_.settings, Opt::audio_shots).Result, fx);
      }
    }
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
    n28311565 = vr_hand_matrix(m, 1);
    if (logic_not(n28311565.Result)) {
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
      n28311566 = vr_hand_matrix(m, 1);
      both_2 = n28311566.Result;
    }
    on = both_2;
    engine_engine_set_xr_scope(engine_, false);
    if (logic_not(on)) {
      if (math_compare<"!=">(kern::to<Float>(vr_scope_mesh_), 0.0)) {
        engine_renderer_set_visible(n28311567, r, vr_scope_mesh_, false);
        set_visible = n28311567.result;
      }
      return;
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(vr_scope_tex_), 0.0))) {
      fbo = 0;
      engine_renderer_create_render_target(n28311568, r, 512, fbo);
      create_render_target = n28311568.result;
      fbo = n28311568.framebuffer;
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
      for (auto it28311569 = flow_for(n28311569, 0, kSeg); it28311569.next();) {
        a = kern::formula([](Float index, Float kSeg, Float kPi) -> Float { return ((kern::math::div(index, kSeg) * 2.0) * kPi); }, kern::to<Float>(n28311569.index), kern::to<Float>(kSeg), kPi);
        engine_geo_mesh_data::MeshData_push(d, vector_make(kern::formula([](Float a) -> Float { return (kern::math::cos(a) * 0.5); }, a), 0.0, kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.5); }, a)), n, Vector{1.0, 1.0, 1.0}, vector_make(kern::formula([](Float a) -> Float { return (0.5 + (kern::math::cos(a) * 0.5)); }, a), kern::formula([](Float a) -> Float { return (0.5 - (kern::math::sin(a) * 0.5)); }, a), 0.0));
        if (math_compare<">">(kern::to<Float>(n28311569.index), 0.0)) {
          place = list_length<Int>(d.indices);
          item = 0;
          item_2 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n28311569.index)));
          item_3 = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32((index + 1.0)); }, kern::to<Float>(n28311569.index)));
          list_insert_list(d.indices, place, list_make<Int, "3">(item, item_2, item_3));
        }
      }
      engine_renderer_allocate_id(n28311570, r);
      allocate_id = n28311570.result;
      vr_scope_mesh_ = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
      look.material.unlit = true;
      look.material.double_sided = true;
      look.material.cast_shadow = false;
      look.material.frustum_culled = false;
      look.extra_slot = kMaxExtensionAttributes;
      look.texture = vr_scope_tex_;
      engine_upsert(n28311571, r, vr_scope_mesh_, d, look, List<Float>{}, List<Float>{});
      upsert = n28311571.result;
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
    engine_renderer_update_instances(n28311572, r, vr_scope_mesh_, lm, List<Float>{});
    update_instances = n28311572.result;
    engine_renderer_set_visible(n28311573, r, vr_scope_mesh_, true);
    set_visible_2 = n28311573.result;
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
      n28311574 = vr_hand_matrix(m, 1);
      both = n28311574.Result;
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
    for (auto it29360138 = flow_for(n29360138, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Loadout>(list))))); it29360138.next();) {
      ammo_mag_[enum_index<weapons_Id>(list[n29360138.index].id)] = list[n29360138.index].magazine;
      ammo_reserve_[enum_index<weapons_Id>(list[n29360138.index].id)] = list[n29360138.index].reserve;
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
    engine_game_world_soldier_state(n29360139, world_, soldier_, st);
    soldier_state = n29360139.result;
    st = n29360139.out;
    if (logic_and(logic_and(soldier_state, game_weapons_cpp::hitscan_on_foot(foot_weapon_).Result), math_compare<">=">(kern::to<Float>(st.ammo), 0.0))) {
      ammo_mag_[enum_index<weapons_Id>(foot_weapon_)] = st.ammo;
      ammo_reserve_[enum_index<weapons_Id>(foot_weapon_)] = st.reserve;
    }
    foot_weapon_ = id;
    s = list_get<weapons_Spec>(kTable, enum_index<weapons_Id>(id));
    kind = logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::smg), "rifle", logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::sniper), "sniper", logic_select<Text>(logic_or(enum_equal<weapons_Id>(id, weapons_Id::grenade_launcher), enum_equal<weapons_Id>(id, weapons_Id::rocket)), "shotgun", "pistol")));
    engine_game_world_set_property_text(n29360140, world_, soldier_, "components.weapon.kind", kind);
    set_property_text = n29360140.result;
    if (game_weapons_cpp::hitscan_on_foot(id).Result) {
      engine_game_world_set_property_number(n29360141, world_, soldier_, "components.weapon.damage", s.damage);
      set_property_number = n29360141.result;
      engine_game_world_set_property_number(n29360142, world_, soldier_, "components.weapon.fireRate", kern::formula([](Float interval_ms) -> Float { return kern::math::div(60000.0, interval_ms); }, s.interval_ms));
      set_property_number_2 = n29360142.result;
      engine_game_world_set_property_flag(n29360143, world_, soldier_, "components.weapon.automatic", enum_equal<weapons_Id>(id, weapons_Id::smg));
      set_property_flag = n29360143.result;
      engine_game_world_set_property_number(n29360144, world_, soldier_, "components.weapon.range", s.range_m);
      set_property_number_3 = n29360144.result;
      engine_game_world_set_property_number(n29360145, world_, soldier_, "components.weapon.headshot", 3.0);
      set_property_number_4 = n29360145.result;
      list = kFoot;
      for (auto it29360146 = flow_for(n29360146, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Loadout>(list))))); it29360146.next();) {
        if (enum_equal<weapons_Id>(list[n29360146.index].id, id)) {
          engine_game_world_set_property_number(n29360147, world_, soldier_, "components.weapon.magazine", kern::to<Float>(list[n29360146.index].magazine));
          set_property_number_5 = n29360147.result;
        }
      }
      engine_game_world_set_property_text(n29360148, world_, soldier_, "components.weapon.fireSound", logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::smg), "sc:shot0", logic_select<Text>(enum_equal<weapons_Id>(id, weapons_Id::sniper), "builtin:sniper", "sc:pistol0")));
      set_property_text_2 = n29360148.result;
      engine_game_world_set_property_number(n29360149, world_, soldier_, "components.weapon.pellets", 1.0);
      set_property_number_6 = n29360149.result;
      engine_game_world_set_property_number(n29360150, world_, soldier_, "components.weapon.aimFov", kern::formula([](Float a) -> Float { return ((a != 0.0) ? 8.09 : 42.6); }, kern::to<Float>(enum_equal<weapons_Id>(id, weapons_Id::sniper))));
      set_property_number_7 = n29360150.result;
      engine_game_world_set_property_number(n29360151, world_, soldier_, "components.weapon.spread", 0.0);
      set_property_number_8 = n29360151.result;
      engine_game_world_set_property_number(n29360152, world_, soldier_, "components.weapon.aimSpread", 0.0);
      set_property_number_9 = n29360152.result;
      engine_game_world_set_property_number(n29360153, world_, soldier_, "components.weapon.recoil", 0.0);
      set_property_number_10 = n29360153.result;
      engine_game_world_soldier_state(n29360154, world_, soldier_, st);
      soldier_state_2 = n29360154.result;
      st = n29360154.out;
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
    for (auto it29360155 = flow_for(n29360155, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it29360155.next();) {
      if (enum_equal<weapons_Id>(struct_break<Loadout>(list_get<Loadout>(kFoot, n29360155.index)).id, foot_weapon_)) {
        cur = n29360155.index;
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
      n29360156 = ctl_screen_ray(from, dir, ctl_.aim_x, ctl_.aim_y);
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
    n29360166 = aim_ray(dir);
    eye = n29360166.Result;
    the_actor_velocity = engine_game_world_actor_velocity(world_, soldier_);
    v = vector_add(vector_add(vector_scale(dir, kGrenadeThrow), Vector{0.0, 0.0, 3.5}), the_actor_velocity);
    fire_projectile(weapons_Id::hand_grenade, vector_add(eye, vector_scale(dir, 0.5)), v, true, body(soldier_).Result);
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
    for (auto it31457290 = flow_for(n31457290, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it31457290.next();) {
      skip = false;
      if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n31457290.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        list_add(out, logic_select<Vector>(logic_or(enum_equal<net_PlayerMode>(net_.remotes_[n31457290.index].state.mode, net_PlayerMode::foot), enum_equal<net_PlayerMode>(net_.remotes_[n31457290.index].state.mode, net_PlayerMode::drone)), net_.remotes_[n31457290.index].person_shown, net_.remotes_[n31457290.index].shown));
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

  struct the_Game_worldsync_update_list_Result { Game_worldsync_update_List Result = Game_worldsync_update_List{0, 0, Vector{0.0, 0.0, 0.0}, 0, List<Int>{}, List<Int>{}, 0, 0}; };
  the_Game_worldsync_update_list_Result the_Game_worldsync_update_list_(Int to = 0, Vector o = Vector{0.0, 0.0, 0.0}, Int t_ms = 0, Int type = 0) {
    Game_worldsync_update_List l = Game_worldsync_update_List{0, 0, Vector{0.0, 0.0, 0.0}, 0, List<Int>{}, List<Int>{}, 0, 0};
    l = struct_make<Game_worldsync_update_List>(to, type, o, t_ms, List<Int>{}, List<Int>{}, 0, 0);
    return {l};
  }

  void List_head(Game_worldsync_update_List& self) {
    Int place = 0;
    list_clear(self.w);
    wire::put_u8(self.w, self.type);
    wire::put_u8(self.w, 0);
    wire::put_u8(self.w, self.part);
    wire::put_f32(self.w, vector_break(self.o).x);
    wire::put_f32(self.w, vector_break(self.o).y);
    wire::put_f32(self.w, vector_break(self.o).z);
    wire::put_u32(self.w, self.t_ms);
    place = list_length<Int>(self.w);
    list_insert_list(self.w, place, self.extra);
    self.n = 0;
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool added = false; Int index = 0; } n2097162;
  struct { Bool added = false; Int index = 0; } n2097163;
  struct { Bool added = false; Int index = 0; } n2097164;
  struct : kern::Node<2097165> { Int index = 0; } n2097165;
  struct { Bool added = false; Int index = 0; } n2097166;
  struct { Int result = 0; } n2097170;
  struct { Bool result = false; } n2097171;
  gaps_barricade_doorsResult n2097173;
  struct : kern::Node<2097174> { Int index = 0; } n2097174;
  struct : kern::Node<2097175> { Int index = 0; } n2097175;
  struct { Bool result = false; } n2097176;
  struct { Bool result = false; Bool changed = false; } n2097177;
  struct { Bool result = false; } n2097178;
  vr_hand_rayResult n3145740;
  project_screenResult n3145747;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n4194314;
  struct : kern::Node<4194319> { Int index = 0; } n4194319;
  struct { Bool result = false; } n4194323;
  state_ofResult n4194336;
  struct { Bool result = false; } n4194337;
  struct { Bool result = false; } n4194338;
  common::Rng_nextResult n4194341;
  struct { Int result = 0; } n4194342;
  struct : kern::Node<9437196> { Int index = 0; } n9437196;
  state_ofResult n9437197;
  remote_on_segmentResult n9437198;
  struct : kern::Node<9437199> { Int index = 0; } n9437199;
  ctl_screen_rayResult n9437200;
  struct { Int result = 0; } n9437201;
  struct : kern::Node<9437203> { Int index = 0; } n9437203;
  struct : kern::Node<9437216> { Int index = 0; } n9437216;
  struct : kern::Node<9437217> { Int index = 0; } n9437217;
  struct : kern::Node<9437218> { Int index = 0; } n9437218;
  struct { GapsState_SniperRound value = GapsState_SniperRound{game_ShotEvent{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, false, false, Vector{0.0, 0.0, 1.0}, 0}, 0.0}; } n9437219;
  game_gaps_cpp::smoke_discResult n10485770;
  struct { Int result = 0; } n10485771;
  struct { Bool result = false; } n10485772;
  struct { Int result = 0; } n10485773;
  struct { Bool result = false; } n10485774;
  struct { Int result = 0; } n10485775;
  struct { Bool result = false; } n10485776;
  struct : kern::Node<10485777> { Int index = 0; } n10485777;
  struct : kern::Node<10485778> { Int index = 0; } n10485778;
  struct { Bool result = false; } n10485779;
  struct { Bool result = false; } n10485780;
  matrix_ofResult n11534346;
  struct : kern::Node<11534347> { Int index = 0; } n11534347;
  struct { Int result = 0; } n12582922;
  struct { Bool result = false; } n12582923;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n12582924;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n12582925;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n12582926;
  struct { Bool result = false; } n12582927;
  struct { Bool result = false; } n12582928;
  struct : kern::Node<12582929> { Int index = 0; } n12582929;
  struct { Int result = 0; } n12582932;
  struct : kern::Node<12582933> { Int index = 0; } n12582933;
  struct { Int result = 0; } n14680074;
  struct { Bool result = false; } n14680077;
  struct { Int result = 0; } n14680078;
  struct { Bool result = false; } n14680079;
  vr_hand_matrixResult n14680080;
  struct { Bool result = false; Bool changed = false; } n14680081;
  struct { Bool result = false; } n14680082;
  struct : kern::Node<14680083> { Int index = 0; } n14680083;
  struct : kern::Node<14680084> { Int index = 0; } n14680084;
  struct { Int result = 0; } n14680085;
  struct { Bool result = false; } n14680086;
  vr_hand_matrixResult n14680087;
  struct : kern::Node<14680088> { Int index = 0; } n14680088;
  struct : kern::Node<14680089> { Int index = 0; } n14680089;
  struct { Bool result = false; } n14680090;
  struct : kern::Node<14680091> { Int index = 0; } n14680091;
  struct { Bool result = false; } n14680092;
  struct : kern::Node<14680093> { Int index = 0; } n14680093;
  struct { engine_mesh_visible_place<Bool> result = {}; } n14680094;
  struct { engine_mesh_material_place<Material> result = {}; } n14680096;
  struct { engine_mesh_id_place<Int> result = {}; } n14680097;
  struct : kern::Node<14680098> { Int index = 0; } n14680098;
  struct { engine_mesh_id_place<Int> result = {}; } n14680100;
  struct { engine_mesh_id_place<Int> result = {}; } n14680102;
  struct { engine_mesh_id_place<Int> result = {}; } n14680103;
  struct { Bool result = false; } n14680104;
  struct { Int result = 0; } n14680105;
  vr_hand_matrixResult n14680106;
  vr_hand_matrixResult n14680107;
  vr_hand_matrixResult n14680108;
  struct : kern::Node<14680109> { Int index = 0; } n14680109;
  struct { Bool result = false; Bool changed = false; } n14680110;
  struct { Bool result = false; } n14680111;
  vr_hand_matrixResult n14680112;
  struct { Bool result = false; Bool changed = false; } n14680113;
  struct { Bool result = false; } n14680114;
  matrix_ofResult n15728650;
  struct : kern::Node<16777226> { Int index = 0; } n16777226;
  person_hit::hitResult n16777232;
  struct : kern::Node<16777233> { Int index = 0; } n16777233;
  remote_on_segment_zonesResult n16777234;
  remote_on_segment_zonesResult n16777235;
  remote_on_segment_zonesResult n16777236;
  struct : kern::Node<17825802> { Int index = 0; } n17825802;
  struct { Bool removed = false; } n17825803;
  struct : kern::Node<19922954> { Int index = 0; } n19922954;
  struct : kern::Node<19922955> { Int index = 0; } n19922955;
  struct : kern::Node<19922956> { Int index = 0; } n19922956;
  Game_swarm_collect_next_idResult n19922957;
  struct : kern::Node<19922958> { Int index = 0; } n19922958;
  Game_swarm_collect_next_idResult n19922959;
  struct : kern::Node<19922960> { Int index = 0; } n19922960;
  Game_swarm_collect_next_idResult n19922961;
  struct : kern::Node<19922962> { Int index = 0; } n19922962;
  Game_swarm_collect_next_idResult n19922963;
  struct : kern::Node<19922964> { Int index = 0; } n19922964;
  struct : kern::Node<19922965> { Int index = 0; } n19922965;
  Game_swarm_collect_next_idResult n19922966;
  struct : kern::Node<19922967> { Int index = 0; } n19922967;
  state_ofResult n19922968;
  struct : kern::Node<19922969> { Int index = 0; } n19922969;
  struct : kern::Node<19922970> { Int index = 0; } n19922970;
  Game_swarm_collect_next_idResult n19922971;
  struct : kern::Node<19922972> { Int index = 0; } n19922972;
  struct { Game_SwarmFallen value = Game_SwarmFallen{net_SwarmUnit{0, net_SwarmKind::zombie, 0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, 0.0}; } n19922973;
  struct : kern::Node<19922974> { Int index = 0; } n19922974;
  struct : kern::Node<20971530> { Int index = 0; } n20971530;
  life::Life_animal_ofResult n20971531;
  struct : kern::Node<20971534> { Int index = 0; } n20971534;
  life::Life_animal_ofResult n20971535;
  struct : kern::Node<20971541> { Int index = 0; } n20971541;
  struct { Int result = 0; } n22020106;
  struct { Bool result = false; } n22020115;
  struct { Bool added = false; Int index = 0; } n23068682;
  struct : kern::Node<23068685> { Int index = 0; } n23068685;
  struct : kern::Node<23068686> { Int index = 0; } n23068686;
  matrix_ofResult n24117258;
  struct : kern::Node<24117281> { Int index = 0; } n24117281;
  struct { Int result = 0; } n25165836;
  struct { Int result = 0; } n25165837;
  struct { Int result = 0; } n26214410;
  struct { Bool result = false; } n26214411;
  struct { Int result = 0; } n26214415;
  struct { Bool result = false; } n26214416;
  struct : kern::Node<26214417> { Int index = 0; } n26214417;
  struct : kern::Node<26214418> { Int index = 0; } n26214418;
  struct { engine_game_world_sound_cues_place<List<game_SoundCue>> result = {}; } n26214420;
  struct : kern::Node<26214421> { Int index = 0; } n26214421;
  vr_hand_matrixResult n28311565;
  vr_hand_matrixResult n28311566;
  struct { Bool result = false; } n28311567;
  struct { Int result = 0; Int framebuffer = 0; } n28311568;
  struct : kern::Node<28311569> { Int index = 0; } n28311569;
  struct { Int result = 0; } n28311570;
  struct { Bool result = false; } n28311571;
  struct { Bool result = false; Bool changed = false; } n28311572;
  struct { Bool result = false; } n28311573;
  vr_hand_matrixResult n28311574;
  struct : kern::Node<29360138> { Int index = 0; } n29360138;
  struct { Bool result = false; game_SoldierState out = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0}; } n29360139;
  struct { Bool result = false; } n29360140;
  struct { Bool result = false; } n29360141;
  struct { Bool result = false; } n29360142;
  struct { Bool result = false; } n29360143;
  struct { Bool result = false; } n29360144;
  struct { Bool result = false; } n29360145;
  struct : kern::Node<29360146> { Int index = 0; } n29360146;
  struct { Bool result = false; } n29360147;
  struct { Bool result = false; } n29360148;
  struct { Bool result = false; } n29360149;
  struct { Bool result = false; } n29360150;
  struct { Bool result = false; } n29360151;
  struct { Bool result = false; } n29360152;
  struct { Bool result = false; } n29360153;
  struct { Bool result = false; game_SoldierState out = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0}; } n29360154;
  struct : kern::Node<29360155> { Int index = 0; } n29360155;
  ctl_screen_rayResult n29360156;
  aim_rayResult n29360166;
  struct : kern::Node<31457290> { Int index = 0; } n31457290;
};
