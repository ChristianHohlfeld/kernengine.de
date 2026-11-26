struct Game : virtual kern::Blueprint, virtual blueprint_call, virtual car, virtual car_cpp, virtual city_stream, virtual city_stream_cpp, virtual combat_art_cpp, virtual combat_audio, virtual combat_audio_cpp, virtual combat_rules_cpp, virtual common, virtual destruction, virtual engine_app_app, virtual engine_geo_mesh_data, virtual engine_math_math, virtual engine_physics_physics, virtual equipment, virtual equipment_cpp, virtual game_barricades_cpp, virtual game_combat, virtual game_controls, virtual game_destruction_sync, virtual game_destruction_sync_cpp, virtual game_equipment_cpp, virtual game_gaps, virtual game_gaps_cpp, virtual game_lamps_cpp, virtual game_life, virtual game_life_cpp, virtual game_look, virtual game_look_cpp, virtual game_net, virtual game_projsync, virtual game_projsync_cpp, virtual game_swarm_cpp, virtual game_toss, virtual game_toss_cpp, virtual game_townsync, virtual game_ui, virtual game_vehicle_damage_cpp, virtual game_weapons_cpp, virtual game_world, virtual gaps_rules, virtual gaps_rules_cpp, virtual gps_live, virtual gps_live_cpp, virtual guided, virtual interiors, virtual interiors_cpp, virtual lamps, virtual lamps_cpp, virtual life, virtual life_art_cpp, virtual life_cpp, virtual look, virtual look_cpp, virtual mp_art, virtual mp_art_cpp, virtual mp_rules_cpp, virtual net_session, virtual net_session_cpp, virtual nostr_cpp, virtual people, virtual people_art_cpp, virtual people_cpp, virtual perf, virtual perf_cpp, virtual person_hit, virtual ragdoll, virtual ragdoll_cpp, virtual settings, virtual settings_cpp, virtual terrain, virtual terrain_cpp, virtual town_sync_cpp, virtual vehicle_art_cpp, virtual wire, virtual world_marks_cpp {
  // ---- variables ----
  PadState pad = PadState{false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, false};
  List<DestroySync> dsync_ = List<DestroySync>{};
  TossState toss_ = TossState{List<TossState_Own>{}, List<TossState_Remote>{}, 0.0, 0, 0};
  List<ProjSync> projsync_ = List<ProjSync>{};
  U32 proj_serial_ = 0;
  List<TownSync> townsync_ = List<TownSync>{};
  Bool puppet_world_ = false;
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
  Bool laser_pinned_ = false;
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
  List<TrafficCar> cars_ = List<TrafficCar>{};
  Mode mode_ = Mode::foot;
  /// the car the player drives
  I32 car_index_ = -1;
  Bool world_placed_ = false;
  /// 0 fpv, 1 follow, 2 third (the page restores the player's last choice; FPV first)
  I32 drone_cam_ = 0;
  Bool car_cockpit_ = false;
  Bool jet_cockpit_ = false;
  Camera cam_ = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Float head_yaw_ = 0.0;
  Float head_pitch_ = 0.0;
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  Bool touch_mode_ = false;
  Float frame_w_ = 960.0;
  Float frame_h_ = 540.0;
  /// the town
  People people_ = People{List<People_Person>{}, List<CrimeReport>{}, List<People_Death>{}, List<People_Fall>{}, List<People_RemoteHit>{}, List<People_Hurt>{}, false, false, 60, 1, 0, 0, 170.0, 190.0};
  PeopleArt people_art_ = PeopleArt{Object<kern::Renderer>{}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}, PeopleArt_Part{0, List<Float>{}, List<Float>{}, false}};
  CombatArt combat_art_ = CombatArt{Object<kern::Renderer>{}, List<CombatArt_Part>{}};
  /// buildings broken by weapons (destruction.h)
  Destruction destruction_ = Destruction{Object<kern::Renderer>{}, Map<Int, Float>{}, List<Int_Float_pair>{}, List<Destruction_Piece>{}, List<Destruction_Torn>{}, Destruction_Stats{0, 0, 0, 0}};
  CombatState combat_ = CombatState{combat_MilitaryClock{false, -1.0, -1.0, 0.0}, 0, 0, 0, 0.0, List<Heli>{}, List<Tank>{}, List<Shell>{}, List<SoldierAi>{}, List<Float>{}, true, List<FlakGun>{}, List<FlakRound>{}, List<Puff>{}, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0, 0, List<FighterJet>{}, List<Bomb>{}, List<Blast>{}, List<Flare>{}, 50.0, -1000000000.0, List<Tracer>{}, List<Debris>{}, List<U8>{}, List<Bool>{}, List<Bool>{}, false, 0, 0, 0, combat_ZombieRounds{false, 0, 0, 0, 0.0, -1.0}, List<ZombieAi>{}, true, false, -99.0, List<EnemyAi>{}, List<EnemyRocket>{}, combat_Chaos{0, 1, -1.0, 0.0}, -99.0, -1.0, 0, combat_Fun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, -1.0, combat_Race{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}, false, Vector{0.0, 0.0, 0.0}, "", "", -99.0, -99.0, false, 0.0};
  /// GUIDED missiles in flight (game_guided.cpp)
  List<guided_Flight> guided_ = List<guided_Flight>{};
  Fixed<U32, 16> sfx_cursor_ = List<U32>{};
  Bool combat_audio_ready_ = false;
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
  Float next_attack_ = 0.0;
  /// multiplayer
  net_Session net_ = net_Session{net_Session_Role::none, Object<kern::net::HostTransport>{}, Object<kern::net::Server>{}, Object<kern::net::Client>{}, net_Address{Int64{0x0}}, List<net_Remote>{}, List<net_Hit>{}, List<net_Hit>{}, List<net_Kill>{}, List<net_Fx>{}, List<net_Swarm>{}, List<net_SwarmHit>{}, List<U8_row>{}, List<net_PingEvent>{}, List<net_VoiceFrame>{}, List<U8>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, false, false, 0.0, 0, 0.0, List<U8>{}, Vector{0.0, 0.0, 0.0}, 255, net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<I8>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_Scoreboard{List<mp_Score>{}}, false, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_SwarmParts>{}, 0, 0, false, List<Int_Float_pair>{}, mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}, mp_Ballot{0, 0, false, 0, List<U8>{}, List<U8>{}, 0.0, mp_Ballot_Status::pending}, 0, "Aenderungen nur einstimmig", 0, 0, 0, seats_Table{List<seats_Row>{}, 0}, 0.0};
  /// pings, feed, voice, big map (game_net.h)
  List<MpLocal> mp_ = List<MpLocal>{};
  List<Game_Lamp> lamps_ = List<Game_Lamp>{};
  /// where the ground changed this frame (city_stream.h take_ground_changes)
  List<Vec4> ground_changes_ = List<Vec4>{};
  Float night_ = 0.0;
  Bool show_lamps_ = true;
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
  /// audio
  Object<kern::audio::Mixer> mixer_ = Object<kern::audio::Mixer>{};
  Object<kern::game::SoundBank> bank_ = Object<kern::game::SoundBank>{};
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
  /// building_interiors.mjs PLANKS
  KERN_CONST I32 kPlanks = 6;
  /// zombie_nights.mjs PLANK_MS
  KERN_CONST Float kPlankS = 3.4;
  /// DOOR_W
  KERN_CONST Float kDoorW = 1.8;
  /// vertex locations 14 and 15
  KERN_CONST U32 kMaxExtensionAttributes = 2;
  /// building records per message (a full state is split)
  KERN_CONST Int kPerMessage = 400;
  KERN_CONST U8 kState = 2;
  /// The constant kVariants of the Blueprint combat_audio_cpp
  KERN_CONST Fixed<U32, 11> kVariants = List<U32>{3, 4, 4, 4, 2, 4, 2, 3, 4, 3, 2};
  /// The constant kSampleRate of the Blueprint combat_audio_cpp
  KERN_CONST U32 kSampleRate = 44100;
  KERN_CONST Fixed<MusicSynth_Station, 5> kStations = List<MusicSynth_Station>{MusicSynth_Station{"KERN FM", "Synthwave", 100.0, 11}, MusicSynth_Station{"BODENSEE BEATS", "House", 124.0, 23}, MusicSynth_Station{"LOFI LAGUNE", "Lo-Fi", 78.0, 37}, MusicSynth_Station{"8-BIT RADIO", "Chiptune", 144.0, 41}, MusicSynth_Station{"NACHTSCHICHT", "Techno", 132.0, 53}};
  KERN_CONST Float kG = 9.80665;
  /// GRENADE_RESTITUTION
  KERN_CONST Float kRestitution = 0.58;
  /// GRENADE_SURFACE_DAMPING
  KERN_CONST Float kSurfaceDamping = 0.88;
  /// GRENADE_MAX_BOUNCES
  KERN_CONST I32 kMaxBounces = 7;
  /// gameplay_final_runtime_v2.mjs
  KERN_CONST Float kGrenadeR = 0.05265;
  /// SpawnCity's numbers. Damage of the hitscan guns is SpawnCity's multiplayer damage (VS_WEAPON_DAMAGE smg 13, glock 26, sniper 95) times the zone (head 3, torso 1, legs .65).
  KERN_CONST Fixed<weapons_Spec, 14> kTable = List<weapons_Spec>{weapons_Spec{"MP", 55.0, 180.0, 13.0, 0.0, 240.0, 1}, weapons_Spec{"Glock", 110.0, 240.0, 26.0, 0.0, 0.0, 2}, weapons_Spec{"Sniper .50", 1150.0, 1500.0, 95.0, 0.0, 850.0, 3}, weapons_Spec{"Granatwerfer 40 mm", 760.0, 300.0, 125.0, 8.5, 40.0, 5}, weapons_Spec{"Raketenwerfer", 900.0, 420.0, 160.0, 9.5, 40.0, 6}, weapons_Spec{"Faeuste", 300.0, 1.8, 24.0, 0.0, 0.0, 7}, weapons_Spec{"Messer", 380.0, 1.6, 45.0, 0.0, 0.0, 8}, weapons_Spec{"Handgranate", 4000.0, 40.0, 125.0, 8.5, 0.0, 0}, weapons_Spec{"Drohnen-MG", 92.0, 650.0, 13.0, 0.0, 0.0, 0}, weapons_Spec{"Drohnen-Raketen", 950.0, 220.0, 100.0, 8.0, 31.0, 0}, weapons_Spec{"GUIDED", 5000.0, 600.0, 34.0, 4.0, 34.0, 0}, weapons_Spec{"Bordkanone", 55.0, 1800.0, 30.0, 0.0, 0.0, 0}, weapons_Spec{"Luft-Rakete", 2400.0, 1300.0, 120.0, 8.0, 260.0, 0}, weapons_Spec{"Bombe", 450.0, 0.0, 170.0, 17.0, 0.0, 0}};
  KERN_CONST Float kPi = 3.141592653589793;
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
  KERN_CONST Float kDeg = 0.017453292519943295;
  KERN_CONST Fixed<Text, 4> kVmNames = List<Text>{"BOOMSTICK 40 mm", "ROCKET LAUNCHER", "FISTS", "KNIFE"};
  /// world.* switches that make the world (friendly fire is the rule voted on: game_rules_sync)
  KERN_CONST Fixed<Opt, 13> kWorldRules = List<Opt>{Opt::world_people, Opt::world_traffic, Opt::world_dogs, Opt::world_cats, Opt::world_birds, Opt::world_zombies, Opt::world_police, Opt::world_military, Opt::world_flak, Opt::world_jetpack, Opt::world_terrain, Opt::world_guided, Opt::world_always_day};
  /// mp_fun_hud.mjs: a kill line stays 5.2 s
  KERN_CONST Float kFeedMs = 5200.0;
  /// The number of each value of net_PlayerFlag, in their order
  KERN_CONST List<Int> net_PlayerFlag_numbers = List<Int>{1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
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
  KERN_CONST U8 kDoorsMax = 64;
  /// the blasts kept (scorches 40, soot 48, breaches 64 - world_marks.h)
  KERN_CONST Int kMarks = 64;
  KERN_CONST U8 kPlanksMax = 6;
  KERN_CONST Vector kTowYellow = Vector{0.9490196078431372, 0.7176470588235294, 0.0196078431372549};
  /// s of Y held = RESET
  KERN_CONST Float kResetHold = 1.0;
  /// Magazines and spare rounds on foot (gameplay_final_runtime_v2.mjs WEAPONS).
  KERN_CONST Fixed<Loadout, 7> kFoot = List<Loadout>{Loadout{weapons_Id::smg, 30, 150}, Loadout{weapons_Id::glock, 17, 85}, Loadout{weapons_Id::sniper, 5, 25}, Loadout{weapons_Id::grenade_launcher, 6, 18}, Loadout{weapons_Id::rocket, 1, 6}, Loadout{weapons_Id::fists, -1, -1}, Loadout{weapons_Id::knife, -1, -1}};
  /// hand_grenade.mjs FUSE_MS 4000
  KERN_CONST Float kGrenadeFuse = 4.0;
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
      engine_renderer_allocate_id(n3145750, gaps_[0].renderer);
      allocate_id = n3145750.result;
      gaps_[0].plank_part.id = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
      look.material = m;
      look.extra_slot = kMaxExtensionAttributes;
      look.visible = false;
      engine_upsert(n3145751, gaps_[0].renderer, gaps_[0].plank_part.id, box, look, List<Float>{}, List<Float>{});
      upsert = n3145751.result;
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
      n3145753 = gaps_barricade_doors(doors);
      both = n3145753.Result;
    }
    if (both) {
      for (auto it3145754 = flow_for(n3145754, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_DoorView>(doors))))); it3145754.next();) {
        yaw = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(doors[n3145754.index].d).y, vector_break(doors[n3145754.index].d).x);
        for (auto it3145755 = flow_for(n3145755, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(doors[n3145754.index].planks)))); it3145755.next();) {
          along = kern::formula([](Float index) -> Float { return ((kern::math::mod(index, 2.0) != 0.0) ? 0.06 : (-0.06)); }, kern::to<Float>(n3145755.index));
          tilt = kern::formula([](Float index) -> Float { return (((kern::math::mod(index, 2.0) != 0.0) ? 1.0 : (-1.0)) * (0.08 + (0.05 * kern::math::mod(index, 3.0)))); }, kern::to<Float>(n3145755.index));
          at = vector_make(kern::formula([](Float x, Float x_2, Float along, Float x_3) -> Float { return ((x + (x_2 * along)) + (x_3 * 0.22)); }, vector_break(doors[n3145754.index].c).x, vector_break(doors[n3145754.index].d).x, along, vector_break(doors[n3145754.index].n).x), kern::formula([](Float y, Float y_2, Float along, Float y_3) -> Float { return ((y + (y_2 * along)) + (y_3 * 0.22)); }, vector_break(doors[n3145754.index].c).y, vector_break(doors[n3145754.index].d).y, along, vector_break(doors[n3145754.index].n).y), kern::formula([](Float a, Float index) -> Float { return ((a + 0.35) + (index * 0.36)); }, doors[n3145754.index].the_floor, kern::to<Float>(n3145755.index)));
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
      engine_renderer_set_visible(n3145756, gaps_[0].renderer, gaps_[0].plank_part.id, false);
      set_visible = n3145756.result;
      return;
    }
    engine_renderer_update_instances(n3145757, gaps_[0].renderer, gaps_[0].plank_part.id, gaps_[0].plank_part.transforms, gaps_[0].plank_part.tints);
    update_instances = n3145757.result;
    engine_renderer_set_visible(n3145758, gaps_[0].renderer, gaps_[0].plank_part.id, true);
    set_visible_2 = n3145758.result;
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
    n4194314 = project_screen(nx, ny, depth, p);
    on = n4194314.Result;
    x = kern::formula([](Float nx, Float frame_w_) -> Float { return (kern::math::div((nx + 1.0), 2.0) * frame_w_); }, nx, frame_w_);
    y = kern::formula([](Float ny, Float frame_h_) -> Float { return (kern::math::div((1.0 - ny), 2.0) * frame_h_); }, ny, frame_h_);
    return {logic_and(on, math_compare<">">(depth, 0.0))};
  }

  struct ctl_command_startsResult { Bool Result = false; };
  KERN_PURE ctl_command_startsResult ctl_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
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
      engine_mixer_render(n5242897, mixer_, frames);
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(n5242897.out))));
      for (auto it5242898 = flow_for(n5242898, 0, last); it5242898.next();) {
        audio_out_[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(at), kern::to<Float>(n5242898.index)))] = list_get<Float>(n5242897.out, n5242898.index);
      }
      if (web_audio_) {
        fill = 0.0;
        first = 0;
        last_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(audio_out_))));
        for (auto it5242899 = flow_for(n5242899, first, last_2); it5242899.next();) {
          audio_out_[n5242899.index] = fill;
        }
      } else {
        ui_render_music(audio_out_, frames);
      }
    } else {
      fill_2 = 0.0;
      first_2 = 0;
      last_3 = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(audio_out_))));
      for (auto it5242900 = flow_for(n5242900, first_2, last_3); it5242900.next();) {
        audio_out_[n5242900.index] = fill_2;
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

  struct sfx_idResult { Text Result = ""; };
  /// "sc:<kind><variant>" for every sound of the bank (built once).
  sfx_idResult sfx_id(combat_audio_Kind k = combat_audio_Kind::shot, U32 v = 0) {
    if (math_compare<"==">(kern::to<Float>(list_length<Text>(sfx_id_ids)), 0.0)) {
      for (auto it7340042 = flow_for(n7340042, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<combat_audio_Kind>(combat_audio_Kind::count))))); it7340042.next();) {
        for (auto it7340043 = flow_for(n7340043, 0, 3); it7340043.next();) {
          list_add(sfx_id_ids, text_append(text_append("sc:", combat_audio_cpp::kind_name(enum_from_index<combat_audio_Kind>(n7340042.index).value).Result, ""), text_format<"%u">(-1, n7340043.index), ""));
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
    for (auto it7340044 = flow_for(n7340044, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<combat_audio_Kind>(combat_audio_Kind::count))))); it7340044.next();) {
      k = enum_from_index<combat_audio_Kind>(n7340044.index).value;
      for (auto it7340045 = flow_for(n7340045, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(int64_to_int(int64_of_int(list_get<U32>(kVariants, n7340044.index))))))); it7340045.next();) {
        pcm = List<Float>{};
        n7340046 = combat_audio_cpp::render(k, n7340045.index, kSampleRate);
        list = n7340046.Result;
        for (auto it7340047 = flow_for(n7340047, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(list))))); it7340047.next();) {
          x = list[n7340047.index];
          list_add(pcm, x);
        }
        n7340048 = sfx_id(k, n7340045.index);
        id = n7340048.Result;
        engine_sound_bank_request(bank_, id);
        engine_sound_bank_set_pcm(n7340049, bank_, id, pcm, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(pcm)))), 1, int64_low(int64_of_int(kSampleRate)));
        set_pcm = n7340049.result;
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
    n7340050 = sfx_id(k, v);
    return {n7340050.Result};
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
      for (auto it8388643 = flow_for(n8388643, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Text>(part))))); it8388643.next();) {
        sp = text_find(rest, " ", 0);
        if (math_compare<"==">(kern::to<Float>(sp), -1.0)) {
          out = "usage: nostr event <created_at> <secret> <aux> <tags> <content>";
          return {true};
        }
        part[n8388643.index] = text_sub(rest, 0, sp);
        rest = text_sub(rest, kern::to<Int>(kern::formula([](Float sp) -> Float { return (sp + 1.0); }, kern::to<Float>(sp))), -1);
      }
      sk = List<U8>{};
      aux = List<U8>{};
      n8388644 = nostr_cpp::from_hex(sk, part[1]);
      either = logic_not(n8388644.Result);
      if (logic_not(either)) {
        n8388646 = nostr_cpp::from_hex(aux, part[2]);
        either = logic_not(n8388646.Result);
      }
      if (logic_or(logic_or(either, math_compare<"==">(kern::to<Float>(text_length(part[3])), 0.0)), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(part[3], 0)))))), 91.0))) {
        out = "bad key or tags";
        return {true};
      }
      n8388647 = nostr_cpp::event_json(sk, aux, out, text_parse_int(part[0], 0, 10).value, 30078, part[3], rest);
      if (logic_not(n8388647.Result)) {
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
      engine_game_world_push_actor(n8388648, world_, soldier_, Vector{0.0, 0.0, 270.0}, true);
      push_actor = n8388648.result;
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
    n9437195 = combat_rules_cpp::Chaos_award(combat_.chaos, pts, now);
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
    for (auto it9437198 = flow_for(n9437198, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Debris>(combat_.debris))))); it9437198.next();) {
      combat_.debris[n9437198.index].v = vector_make(vector_break(combat_.debris[n9437198.index].v).x, vector_break(combat_.debris[n9437198.index].v).y, kern::formula([](Float z, Float kG, Float dt) -> Float { return (z - (kG * dt)); }, vector_break(combat_.debris[n9437198.index].v).z, kG, dt));
      combat_.debris[n9437198.index].p = vector_add(combat_.debris[n9437198.index].p, vector_scale(combat_.debris[n9437198.index].v, dt));
      a = engine_math_math::Quat_axis_angle(logic_select<Vector>(math_compare<">">(vector_length(combat_.debris[n9437198.index].spin), 9.999999717180685e-10), vector_normalize(combat_.debris[n9437198.index].spin), Vector{0.0, 0.0, 1.0}), kern::formula([](Float a, Float dt) -> Float { return (a * dt); }, vector_length(combat_.debris[n9437198.index].spin), dt)).Result;
      combat_.debris[n9437198.index].q = common::qnormalize(engine_math_math::Quat_mul(combat_.debris[n9437198.index].q, a).Result).Result;
      the_floor = kern::formula([](Float the_result, Float z) -> Float { return ((the_result + 0.06) + (z * 0.5)); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(combat_.debris[n9437198.index].p).x, vector_break(combat_.debris[n9437198.index].p).y).Result, vector_break(combat_.debris[n9437198.index].size).z);
      if (math_compare<"<">(vector_break(combat_.debris[n9437198.index].p).z, the_floor)) {
        combat_.debris[n9437198.index].p = vector_make(vector_break(combat_.debris[n9437198.index].p).x, vector_break(combat_.debris[n9437198.index].p).y, the_floor);
        if (math_compare<"<">(vector_break(combat_.debris[n9437198.index].v).z, 0.0)) {
          combat_.debris[n9437198.index].v = vector_make(vector_break(combat_.debris[n9437198.index].v).x, vector_break(combat_.debris[n9437198.index].v).y, kern::formula([](Float z) -> Float { return ((-z) * 0.3); }, vector_break(combat_.debris[n9437198.index].v).z));
        }
        combat_.debris[n9437198.index].v = vector_make(kern::formula([](Float x) -> Float { return (x * 0.76); }, vector_break(combat_.debris[n9437198.index].v).x), vector_break(combat_.debris[n9437198.index].v).y, vector_break(combat_.debris[n9437198.index].v).z);
        combat_.debris[n9437198.index].v = vector_make(vector_break(combat_.debris[n9437198.index].v).x, kern::formula([](Float y) -> Float { return (y * 0.76); }, vector_break(combat_.debris[n9437198.index].v).y), vector_break(combat_.debris[n9437198.index].v).z);
        combat_.debris[n9437198.index].spin = vector_scale(combat_.debris[n9437198.index].spin, 0.6);
      }
    }
    at = 0;
    kept = at;
    end = list_length<Debris>(combat_.debris);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(update_debris_match(combat_.debris[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n9437199, combat_.debris[at]);
          combat_.debris[kept] = n9437199.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(combat_.debris, kept, Debris{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 6.5});
    if (math_compare<">">(kern::formula([](Float a) -> Float { return ((63.0 < 0.0) != (a < 0.0) ? (a < 0.0) : (63.0 < a)); }, kern::to<Float>(list_length<Debris>(combat_.debris))), 0.5)) {
      first = 0;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 63.0); }, kern::to<Float>(list_length<Debris>(combat_.debris))))))));
      for (auto it9437200 = flow_for(n9437200, first, last); it9437200.next();) {
        list_remove_at(combat_.debris, first);
      }
    }
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

  struct foot_remote_detonateResult { Bool Result = false; };
  foot_remote_detonateResult foot_remote_detonate(Float now = 0.0) {
    Bool any = false;
    any = false;
    for (auto it10485776 = flow_for(n10485776, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it10485776.next();) {
      if (logic_and(logic_and(projectiles_[n10485776.index].alive, projectiles_[n10485776.index].by_player), logic_or(enum_equal<weapons_Id>(projectiles_[n10485776.index].id, weapons_Id::grenade_launcher), enum_equal<weapons_Id>(projectiles_[n10485776.index].id, weapons_Id::rocket)))) {
        projectiles_[n10485776.index].fuse = 0.0;
        projectiles_[n10485776.index].born = kern::formula([](Float now) -> Float { return (now - 1.0); }, now);
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
      for (auto it10485786 = flow_for(n10485786, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_ShotEvent>(shots))))); it10485786.next();) {
        if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(shots[n10485786.index].shooter), kern::to<Float>(soldier_)), enum_equal<Mode>(mode_, Mode::foot)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper))) {
          list_add(arrived, shots[n10485786.index]);
        }
      }
      return;
    }
    for (auto it10485787 = flow_for(n10485787, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_ShotEvent>(shots))))); it10485787.next();) {
      if (logic_and(logic_and(math_compare<"==">(kern::to<Float>(shots[n10485787.index].shooter), kern::to<Float>(soldier_)), enum_equal<Mode>(mode_, Mode::foot)), enum_equal<weapons_Id>(foot_weapon_, weapons_Id::sniper))) {
        list_add(gaps_[0].sniper_rounds, struct_make<GapsState_SniperRound>(shots[n10485787.index], kern::formula([](Float now, Float a) -> Float { return (now + kern::math::div(a, 850.0)); }, now, vector_length(vector_sub(shots[n10485787.index].to, shots[n10485787.index].from)))));
      }
    }
    for (auto it10485788 = flow_for(n10485788, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_SniperRound>(gaps_[0].sniper_rounds))))); it10485788.next();) {
      if (math_compare<">=">(now, gaps_[0].sniper_rounds[n10485788.index].due)) {
        list_add(arrived, gaps_[0].sniper_rounds[n10485788.index].s);
      }
    }
    at = 0;
    kept = at;
    end = list_length<GapsState_SniperRound>(gaps_[0].sniper_rounds);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(gaps_sniper_rounds_match(gaps_[0].sniper_rounds[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n10485789, gaps_[0].sniper_rounds[at]);
          gaps_[0].sniper_rounds[kept] = n10485789.value;
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
    for (auto it11534367 = flow_for(n11534367, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<gaps_Puff>(gaps_[0].smoke.puffs_))))); it11534367.next();) {
      skip = false;
      if (logic_not(gaps_[0].smoke.puffs_[n11534367.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (gaps_[0].smoke.puffs_[n11534367.index].ember) {
          m = game_gaps_cpp::billboard(gaps_[0].smoke.puffs_[n11534367.index].pos, right, up, vector_scale(f, -1.0), kern::formula([](Float the_result) -> Float { return (the_result * 0.5); }, gaps_rules_cpp::SmokeField_size(gaps_[0].smoke.puffs_[n11534367.index], now).Result), gaps_[0].smoke.puffs_[n11534367.index].rotation).Result;
          place = list_length<Float>(gaps_[0].ember.transforms);
          list_insert_list(gaps_[0].ember.transforms, place, m.m);
          place_2 = list_length<Float>(gaps_[0].ember.tints);
          item = vector_break(gaps_[0].smoke.puffs_[n11534367.index].color).x;
          item_2 = vector_break(gaps_[0].smoke.puffs_[n11534367.index].color).y;
          item_3 = vector_break(gaps_[0].smoke.puffs_[n11534367.index].color).z;
          item_4 = gaps_[0].smoke.puffs_[n11534367.index].opacity;
          list_insert_list(gaps_[0].ember.tints, place_2, list_make<Float, "4">(item, item_2, item_3, item_4));
        } else {
          m_2 = game_gaps_cpp::billboard(gaps_[0].smoke.puffs_[n11534367.index].pos, right, up, vector_scale(f, -1.0), kern::formula([](Float the_result) -> Float { return (the_result * 0.5); }, gaps_rules_cpp::SmokeField_size(gaps_[0].smoke.puffs_[n11534367.index], now).Result), gaps_[0].smoke.puffs_[n11534367.index].rotation).Result;
          place_3 = list_length<Float>(gaps_[0].disc.transforms);
          list_insert_list(gaps_[0].disc.transforms, place_3, m_2.m);
          place_4 = list_length<Float>(gaps_[0].disc.tints);
          item_5 = vector_break(gaps_[0].smoke.puffs_[n11534367.index].color).x;
          item_6 = vector_break(gaps_[0].smoke.puffs_[n11534367.index].color).y;
          item_7 = vector_break(gaps_[0].smoke.puffs_[n11534367.index].color).z;
          item_8 = gaps_[0].smoke.puffs_[n11534367.index].opacity;
          list_insert_list(gaps_[0].disc.tints, place_4, list_make<Float, "4">(item_5, item_6, item_7, item_8));
        }
      }
    }
    list_clear(gaps_[0].trail_part.transforms);
    list_clear(gaps_[0].trail_part.tints);
    for (auto it11534368 = flow_for(n11534368, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_TrailPuff>(gaps_[0].trail))))); it11534368.next();) {
      skip_2 = false;
      if (math_compare<"<">(gaps_[0].trail[n11534368.index].born, 0.0)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        m_3 = game_gaps_cpp::billboard(gaps_[0].trail[n11534368.index].p, right, up, vector_scale(f, -1.0), 0.8, 0.0).Result;
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
    for (auto it11534369 = flow_for(n11534369, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GapsState_RocketPuff>(gaps_[0].rocket_smoke))))); it11534369.next();) {
      skip_3 = false;
      if (math_compare<"<">(gaps_[0].rocket_smoke[n11534369.index].born, 0.0)) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float a, Float a_2) -> Float { return kern::math::div((now - a), a_2); }, now, gaps_[0].rocket_smoke[n11534369.index].born, gaps_[0].rocket_smoke[n11534369.index].life), 0.0, 1.0));
        m_4 = game_gaps_cpp::billboard(gaps_[0].rocket_smoke[n11534369.index].p, right, up, vector_scale(f, -1.0), kern::formula([](Float t) -> Float { return ((0.18 + (1.1 * kern::math::sqrt(t))) * 0.5); }, t), gaps_[0].rocket_smoke[n11534369.index].rot).Result;
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
        engine_renderer_set_visible(n11534370, gaps_[0].renderer, gaps_[0].disc.id, false);
        set_visible = n11534370.result;
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        engine_renderer_update_instances(n11534371, gaps_[0].renderer, gaps_[0].disc.id, gaps_[0].disc.transforms, gaps_[0].disc.tints);
        update_instances = n11534371.result;
        engine_renderer_set_visible(n11534372, gaps_[0].renderer, gaps_[0].disc.id, true);
        set_visible_2 = n11534372.result;
      }
    }
    skip_4 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].ember.id), 0.0))) {
      skip_4 = true;
    }
    if (logic_not(skip_4)) {
      if (math_compare<"==">(kern::to<Float>(list_length<Float>(gaps_[0].ember.transforms)), 0.0)) {
        engine_renderer_set_visible(n11534373, gaps_[0].renderer, gaps_[0].ember.id, false);
        set_visible_3 = n11534373.result;
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        engine_renderer_update_instances(n11534374, gaps_[0].renderer, gaps_[0].ember.id, gaps_[0].ember.transforms, gaps_[0].ember.tints);
        update_instances_2 = n11534374.result;
        engine_renderer_set_visible(n11534375, gaps_[0].renderer, gaps_[0].ember.id, true);
        set_visible_4 = n11534375.result;
      }
    }
    skip_4 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].trail_part.id), 0.0))) {
      skip_4 = true;
    }
    if (logic_not(skip_4)) {
      if (math_compare<"==">(kern::to<Float>(list_length<Float>(gaps_[0].trail_part.transforms)), 0.0)) {
        engine_renderer_set_visible(n11534376, gaps_[0].renderer, gaps_[0].trail_part.id, false);
        set_visible_5 = n11534376.result;
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        engine_renderer_update_instances(n11534377, gaps_[0].renderer, gaps_[0].trail_part.id, gaps_[0].trail_part.transforms, gaps_[0].trail_part.tints);
        update_instances_3 = n11534377.result;
        engine_renderer_set_visible(n11534378, gaps_[0].renderer, gaps_[0].trail_part.id, true);
        set_visible_6 = n11534378.result;
      }
    }
    skip_4 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(gaps_[0].rocket_part.id), 0.0))) {
      skip_4 = true;
    }
    if (logic_not(skip_4)) {
      if (math_compare<"==">(kern::to<Float>(list_length<Float>(gaps_[0].rocket_part.transforms)), 0.0)) {
        engine_renderer_set_visible(n11534379, gaps_[0].renderer, gaps_[0].rocket_part.id, false);
        set_visible_7 = n11534379.result;
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        engine_renderer_update_instances(n11534380, gaps_[0].renderer, gaps_[0].rocket_part.id, gaps_[0].rocket_part.transforms, gaps_[0].rocket_part.tints);
        update_instances_4 = n11534380.result;
        engine_renderer_set_visible(n11534381, gaps_[0].renderer, gaps_[0].rocket_part.id, true);
        set_visible_8 = n11534381.result;
      }
    }
  }

  struct gaps_command_startsResult { Bool Result = false; };
  KERN_PURE gaps_command_startsResult gaps_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  void draw_guided() {
    for (auto it12582922 = flow_for(n12582922, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<guided_Flight>(guided_))))); it12582922.next();) {
      people_art_cpp::PeopleArt_projectile(people_art_, guided_[n12582922.index].m.p, guided_[n12582922.index].m.v, 0.06, 0.9, guided_[n12582922.index].m.motor);
    }
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
        value_3 = skins[kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::mod(index, 5.0); }, kern::to<Float>(n14680074.index)))];
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
    for (auto it14680078 = flow_for(n14680078, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_))))); it14680078.next();) {
      dead += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(glife_[0].life.cows_[n14680078.index].dead)));
    }
    for (auto it14680079 = flow_for(n14680079, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it14680079.next();) {
      air += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(glife_[0].life.flocks_[n14680079.index].air)));
    }
    pigeons = 0;
    for (auto it14680080 = flow_for(n14680080, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_))))); it14680080.next();) {
      pigeons += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Pigeon>(glife_[0].life.flocks_[n14680080.index].birds))));
    }
    for (auto it14680081 = flow_for(n14680081, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Bird>(glife_[0].life.birds_))))); it14680081.next();) {
      birds += kern::to<Int>(kern::formula([](Float now, Float a) -> Float { return kern::math::wrap32(((now >= a) ? 1.0 : 0.0)); }, now, glife_[0].life.birds_[n14680081.index].dead_until));
    }
    joggers = 0;
    if (life::Life_has_chain(glife_[0].life).Result) {
      for (auto it14680082 = flow_for(n14680082, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Runner>(glife_[0].life.chain_.runners))))); it14680082.next();) {
        joggers += kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(((a != 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(glife_[0].life.chain_.runners[n14680082.index].alive)));
      }
    }
    b = "";
    b = text_format<",\"life\":{\"cows\":%u,\"cowsDead\":%u,\"cowHits\":%u,\"flocks\":%u,\"flocksAir\":%u,\"pigeons\":%u,\"birds\":%u,\"birdHits\":%u,\"trees\":%u,\"joggers\":%u,\"joggerStreak\":%u,\"ragdolls\":%u,\"ragdollSpawns\":%u,\"falls\":%u,\"getups\":%u,\"shouts\":%u,\"honks\":%u">(639, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Cow>(glife_[0].life.cows_)))), dead, life::Life_cow_hits(glife_[0].life).Result, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Flock>(glife_[0].life.flocks_)))), air, pigeons, birds, life::Life_bird_hits(glife_[0].life).Result, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Life_Tree>(glife_[0].life.trees_)))), joggers, life::Life_jogger_streak(glife_[0].life).Result, ragdoll_cpp::Ragdolls_active(glife_[0].life.ragdolls_).Result, ragdoll::Ragdolls_spawned(glife_[0].life.ragdolls_).Result, glife_[0].falls, glife_[0].getups, life::Life_shouts(glife_[0].life).Result, life::Life_honks(glife_[0].life).Result);
    s = text_append(s, b, "");
    if (math_compare<"<">(now, glife_[0].banner_until)) {
      s = text_append(s, text_append(text_append(",\"banner\":\"", game_life_cpp::json_escape(glife_[0].banner).Result, ""), "\"", ""), "");
    }
    s = text_append(s, ",\"say\":[", "");
    for (auto it14680083 = flow_for(n14680083, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<LifeOut_Say>(glife_[0].speech))))); it14680083.next();) {
      b = text_format<"%s[\"%s\",%.2f,%.2f,%.2f,%.2f]">(639, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n14680083.index), 0.0), ",", ""), game_life_cpp::json_escape(glife_[0].speech[n14680083.index].text).Result, glife_[0].speech[n14680083.index].pitch, glife_[0].speech[n14680083.index].rate, glife_[0].speech[n14680083.index].volume, glife_[0].speech[n14680083.index].delay);
      s = text_append(s, b, "");
    }
    list_clear(glife_[0].speech);
    s = text_append(s, "],\"bubbles\":[", "");
    first = true;
    aspect = kern::formula([](Float frame_h_, Float frame_w_) -> Float { return ((frame_h_ > 0.0) ? kern::math::div(frame_w_, frame_h_) : kern::math::div(16.0, 9.0)); }, frame_h_, frame_w_);
    a = engine_math_math::projection_matrix(cam_, aspect).Result;
    n14680084 = engine_math_math::Mat4_look_at_2(cam_.eye, cam_.target, cam_.up);
    a_2 = n14680084.Result;
    vp = engine_math_math::Mat4_mul(a, a_2).Result;
    for (auto it14680085 = flow_for(n14680085, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Voice>(glife_[0].life.voices_))))); it14680085.next();) {
      skip = false;
      c = engine_math_math::Mat4_transform(vp, engine_math_math::Vec4_new(vector_break(glife_[0].life.voices_[n14680085.index].at).x, vector_break(glife_[0].life.voices_[n14680085.index].at).y, vector_break(glife_[0].life.voices_[n14680085.index].at).z, 1.0).Result).Result;
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
          b = text_format<"%s[%.4f,%.4f,\"%s\",\"%s\",%s]">(639, logic_select<Text>(first, "", ","), kern::formula([](Float x) -> Float { return kern::math::div((x + 1.0), 2.0); }, x), kern::formula([](Float y) -> Float { return kern::math::div((1.0 - y), 2.0); }, y), game_life_cpp::json_escape(glife_[0].life.voices_[n14680085.index].text).Result, glife_[0].life.voices_[n14680085.index].mood, logic_select<Text>(math_compare<"<">(kern::formula([](Float a, Float now) -> Float { return (a - now); }, glife_[0].life.voices_[n14680085.index].until, now), 0.35), "true", "false"));
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

  void launch_jets(Float cx = 0.0, Float cy = 0.0, Bool strike = false, Float at = 0.0, Float alt = 0.0, Float offset = 0.0, Bool remote = false) {
    Float z = 0.0;
    FighterJet j = FighterJet{0.0, 0.0, 0.0, 0.0, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false};
    z = kern::formula([](Float the_result, Float alt) -> Float { return (the_result + alt); }, city_stream_cpp::CityStream_ground_height(city_, cx, cy).Result, alt);
    for (auto it16777233 = flow_for(n16777233, 0, 1); it16777233.next();) {
      j = FighterJet{0.0, 0.0, 0.0, 0.0, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false};
      j.x = kern::formula([](Float cx, Float offset, Float index) -> Float { return ((cx + offset) - (index * 26.0)); }, cx, offset, kern::to<Float>(n16777233.index));
      j.y0 = kern::formula([](Float cy, Float combat_kJetSpan, Float index) -> Float { return ((cy - combat_kJetSpan) - (index * 42.0)); }, cy, combat_kJetSpan, kern::to<Float>(n16777233.index));
      j.z = kern::formula([](Float z, Float index) -> Float { return (z + (index * 6.0)); }, z, kern::to<Float>(n16777233.index));
      j.t0 = at;
      j.strike = logic_and(strike, math_compare<"==">(kern::to<Float>(n16777233.index), 0.0));
      j.target = vector_make(cx, cy, 0.0);
      j.remote = remote;
      list_add(combat_.jets, j);
    }
  }

  struct draw_military_matchResult { Bool Result = false; };
  /// updateBlasts: hot core (sphere r .45, x .5 .. 5, opacity .9 (1-t)) and flat ring (inner .6 outer 1, x .7 .. 7.7, opacity .78 (1-t)^1.6), e = 1 - (1-t)^3 over 540 ms, additive orange / gold
  KERN_PURE draw_military_matchResult draw_military_match(const Game_VisualBlast& b, Float now = 0.0) {
    return {logic_or(math_compare<">=">(kern::formula([](Float now, Float born) -> Float { return (now - born); }, now, b.born), 0.54), math_compare<"<">(now, b.born))};
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
    for (auto it17825815 = flow_for(n17825815, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825815.next();) {
      sample_rate = engine_mixer_sample_rate(mixer_);
      n17825816 = mp_rules_cpp::VoiceStream_render(net_.remotes_[n17825815.index].voice, out, frames, sample_rate);
    }
  }

  struct remote_on_segment_zonesResult { Bool Result = false; };
  remote_on_segment_zonesResult remote_on_segment_zones(Float& t_hit, mp_Zone& z, Vector from = Vector{0.0, 0.0, 0.0}, Vector seg = Vector{0.0, 0.0, 0.0}, Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float speed = 0.0) {
    Float t = 0.0;
    BodyPart part = BodyPart::none;
    t = t_hit;
    n17825817 = person_hit::hit(t, feet, blueprint_call::decimal(yaw).Result, blueprint_call::decimal(speed).Result, from, seg, 1.16, 0.0);
    part = n17825817.Result;
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
    for (auto it17825818 = flow_for(n17825818, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<net_Remote>(net_.remotes_))))); it17825818.next();) {
      skip = false;
      if (math_compare<">">(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return (kern::math::band(a, kern::math::bor(a_2, a_3)) != 0.0); }, kern::to<Float>(net_.remotes_[n17825818.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kDead))), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kPassenger)))), 0.5)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_and(gaps_riding().Result, math_compare<"==">(kern::to<Float>(net_.remotes_[n17825818.index].state.slot), kern::to<Float>(gaps_[0].ride_owner)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (enum_equal<net_PlayerMode>(net_.remotes_[n17825818.index].state.mode, net_PlayerMode::foot)) {
            t = 2.0;
            z = mp_Zone::torso;
            feet = logic_select<Vector>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(net_.remotes_[n17825818.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated)))), 0.5), vector_sub(net_.remotes_[n17825818.index].person_shown, vector_make(0.0, 0.0, kSeatDrop)), net_.remotes_[n17825818.index].person_shown);
            n17825819 = remote_on_segment_zones(t, z, from, seg, feet, net_.remotes_[n17825818.index].state.avatar_yaw, net_.remotes_[n17825818.index].state.speed);
            if (logic_and(n17825819.Result, math_compare<"<">(t, best_t))) {
              best_t = t;
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825818.index)));
              best_zone = z;
            }
            skip = true;
          }
          if (logic_not(skip)) {
            if (enum_equal<net_PlayerMode>(net_.remotes_[n17825818.index].state.mode, net_PlayerMode::drone)) {
              t_2 = 2.0;
              z_2 = mp_Zone::torso;
              n17825820 = remote_on_segment_zones(t_2, z_2, from, seg, net_.remotes_[n17825818.index].person_shown, net_.remotes_[n17825818.index].state.avatar_yaw, 0.0);
              if (logic_and(n17825820.Result, math_compare<"<">(t_2, best_t))) {
                best_t = t_2;
                best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825818.index)));
                best_zone = z_2;
              }
            }
            if (enum_equal<net_PlayerMode>(net_.remotes_[n17825818.index].state.mode, net_PlayerMode::parachute)) {
              t_3 = 2.0;
              z_3 = mp_Zone::torso;
              n17825821 = remote_on_segment_zones(t_3, z_3, from, seg, vector_sub(net_.remotes_[n17825818.index].shown, Vector{0.0, 0.0, 1.75}), net_.remotes_[n17825818.index].state.yaw, 0.0);
              if (logic_and(n17825821.Result, math_compare<"<">(t_3, best_t))) {
                best_t = t_3;
                best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825818.index)));
                best_zone = z_3;
              }
            }
            person = enum_equal<net_PlayerMode>(net_.remotes_[n17825818.index].state.mode, net_PlayerMode::foot);
            c = logic_select<Vector>(person, vector_add(net_.remotes_[n17825818.index].person_shown, vector_make(0.0, 0.0, kern::formula([](Float a, Float a_2, Float kSeatDrop) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? (0.73 - kSeatDrop) : 0.73); }, kern::to<Float>(net_.remotes_[n17825818.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kSeated))), kSeatDrop))), net_.remotes_[n17825818.index].shown);
            radius = 0.522;
            switch (flow_switch_enum<net_PlayerMode>(net_.remotes_[n17825818.index].state.mode)) {
              case net_PlayerMode::foot: {
                c = vector_add(c, Vector{0.0, 0.0, 0.2});
              } break;
              case net_PlayerMode::drone: {
                radius = 0.3;
              } break;
              case net_PlayerMode::car: {
                radius = kern::formula([](Float a, Float a_2) -> Float { return ((kern::math::band(a, a_2) != 0.0) ? 3.2 : 2.0); }, kern::to<Float>(net_.remotes_[n17825818.index].state.flags), kern::to<Float>(list_get<Int>(net_PlayerFlag_numbers, enum_index<net_PlayerFlag>(net_PlayerFlag::kBus))));
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
              best = kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n17825818.index)));
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
    n17825824 = project_screen(nx, ny, depth, p);
    on = n17825824.Result;
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
    n17825825 = mp_rules_cpp::safe_mobile_marker_point(vector_make(kern::formula([](Float w, Float x) -> Float { return (kern::math::div(w, 2.0) + x); }, w, x), kern::formula([](Float h, Float y) -> Float { return (kern::math::div(h, 2.0) + y); }, h, y), 0.0), w, h, touch_mode_);
    q = n17825825.Result;
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
    for (auto it20971555 = flow_for(n20971555, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Game_RemoteSwarm>(swarms_))))); it20971555.next();) {
      for (auto it20971556 = flow_for(n20971556, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_RemoteUnit>(swarms_[n20971555.index].units))))); it20971556.next();) {
        if (enum_equal<net_SwarmKind>(swarms_[n20971555.index].units[n20971556.index].kind, net_SwarmKind::heli)) {
          a_2 = engine_math_math::Mat4_translate(swarms_[n20971555.index].units[n20971556.index].p).Result;
          a_3 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(swarms_[n20971555.index].units[n20971556.index].yaw).Result, Vector{1.0, 1.0, 1.0}).Result;
          a = engine_math_math::Mat4_mul(a_2, a_3).Result;
          combat_art_cpp::CombatArt_heli(combat_art_, a, swarms_[n20971555.index].units[n20971556.index].rotor, math_compare<"!=">(kern::to<Float>(swarms_[n20971555.index].units[n20971556.index].flags), 0.0));
        } else if (enum_equal<net_SwarmKind>(swarms_[n20971555.index].units[n20971556.index].kind, net_SwarmKind::tank)) {
          a_5 = engine_math_math::Mat4_translate(vector_make(vector_break(swarms_[n20971555.index].units[n20971556.index].p).x, vector_break(swarms_[n20971555.index].units[n20971556.index].p).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(swarms_[n20971555.index].units[n20971556.index].p).x, vector_break(swarms_[n20971555.index].units[n20971556.index].p).y).Result)).Result;
          a_6 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(swarms_[n20971555.index].units[n20971556.index].yaw).Result, Vector{1.0, 1.0, 1.0}).Result;
          a_4 = engine_math_math::Mat4_mul(a_5, a_6).Result;
          combat_art_cpp::CombatArt_tank(combat_art_, a_4, swarms_[n20971555.index].units[n20971556.index].aux, math_compare<"!=">(kern::to<Float>(swarms_[n20971555.index].units[n20971556.index].flags), 0.0));
        } else if (logic_and(enum_equal<net_SwarmKind>(swarms_[n20971555.index].units[n20971556.index].kind, net_SwarmKind::enemy), math_compare<">=">(kern::to<Float>(swarms_[n20971555.index].units[n20971556.index].person), 0.0))) {
          type = enum_from_index<combat_EnemyType>(people_.people_[swarms_[n20971555.index].units[n20971556.index].person].role).value;
          a_8 = engine_math_math::Mat4_translate(vector_add(people_.people_[swarms_[n20971555.index].units[n20971556.index].person].pos, Vector{0.0, 0.0, 1.4})).Result;
          a_9 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(people_.people_[swarms_[n20971555.index].units[n20971556.index].person].yaw).Result, Vector{0.27, 0.45, 0.06}).Result;
          a_7 = engine_math_math::Mat4_mul(a_8, a_9).Result;
          combat_art_cpp::CombatArt_put(combat_art_, a_7, CombatArt_Shape::glow_box, vector_scale(struct_break<combat_EnemySpec>(enemy_spec(type).Result).stripe, 1.6));
          if (enum_equal<combat_EnemyType>(type, combat_EnemyType::heavy)) {
            a_11 = engine_math_math::Mat4_translate(vector_add(people_.people_[swarms_[n20971555.index].units[n20971556.index].person].pos, vector_make(kern::formula([](Float a) -> Float { return (kern::math::cos(a) * 0.1); }, people_.people_[swarms_[n20971555.index].units[n20971556.index].person].yaw), kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.1); }, people_.people_[swarms_[n20971555.index].units[n20971556.index].person].yaw), 1.62))).Result;
            a_13 = common::yaw_quat(people_.people_[swarms_[n20971555.index].units[n20971556.index].person].yaw).Result;
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
    for (auto it20971557 = flow_for(n20971557, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Game_RemoteSwarm>(swarms_))))); it20971557.next();) {
      for (auto it20971558 = flow_for(n20971558, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_RemoteUnit>(swarms_[n20971557.index].units))))); it20971558.next();) {
        skip = false;
        if (logic_not(enum_equal<net_SwarmKind>(swarms_[n20971557.index].units[n20971558.index].kind, net_SwarmKind::cruiser))) {
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
          rest = car_rest_pose(swarm_cruiser_[0].s_, vector_break(swarms_[n20971557.index].units[n20971558.index].p).x, vector_break(swarms_[n20971557.index].units[n20971558.index].p).y, swarms_[n20971557.index].units[n20971558.index].yaw).Result;
          car::Car_show(swarm_cruiser_[0], swarms_[n20971557.index].units[n20971558.index].aux);
          a_2 = engine_math_math::Mat4_trs(rest.position, rest.rotation, Vector{1.0, 1.0, 1.0}).Result;
          vehicle_art_cpp::VehicleArt_car(art_, a_2, swarm_cruiser_[0], logic_select<Vector>(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(swarms_[n20971557.index].units[n20971558.index].flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserWreck)))), 0.5), kWreckBlack, kCruiserWhite), false, false);
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
    for (auto it20971559 = flow_for(n20971559, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Game_RemoteSwarm>(swarms_))))); it20971559.next();) {
      for (auto it20971560 = flow_for(n20971560, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_RemoteUnit>(swarms_[n20971559.index].units))))); it20971560.next();) {
        if (enum_equal<net_SwarmKind>(swarms_[n20971559.index].units[n20971560.index].kind, net_SwarmKind::cruiser)) {
          a = car::CarSpec_car().Result;
          rest = car_rest_pose(a, vector_break(swarms_[n20971559.index].units[n20971560.index].p).x, vector_break(swarms_[n20971559.index].units[n20971560.index].p).y, swarms_[n20971559.index].units[n20971560.index].yaw).Result;
          a_2 = engine_math_math::Mat4_trs(rest.position, rest.rotation, Vector{1.0, 1.0, 1.0}).Result;
          people_art_cpp::PeopleArt_light_bar(people_art_, a_2, math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(swarms_[n20971559.index].units[n20971560.index].flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims)))), 0.5), now);
          if (logic_and(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(swarms_[n20971559.index].units[n20971560.index].flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kCruiserWreck)))), 0.5), math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, swarms_[n20971559.index].units[n20971560.index].wreck_at), 40.0))) {
            people_art_cpp::PeopleArt_fire(people_art_, vector_add(rest.position, Vector{0.0, 0.0, 0.4}), 0.35, now);
          }
        } else if (enum_equal<net_SwarmKind>(swarms_[n20971559.index].units[n20971560.index].kind, net_SwarmKind::police_drone)) {
          v = vector_scale(vector_sub(swarms_[n20971559.index].units[n20971560.index].tp, swarms_[n20971559.index].units[n20971560.index].p), 10.0);
          people_art_cpp::PeopleArt_police_drone(people_art_, swarms_[n20971559.index].units[n20971560.index].p, swarms_[n20971559.index].units[n20971560.index].yaw, kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float y) -> Float { return ((-y) * 0.04); }, vector_break(v).y), -0.3, 0.3)), kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x) -> Float { return (x * 0.04); }, vector_break(v).x), -0.3, 0.3)), logic_not(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return (kern::math::band(a, a_2) != 0.0); }, kern::to<Float>(swarms_[n20971559.index].units[n20971560.index].flags), kern::to<Float>(list_get<Int>(net_SwarmFlag_numbers, enum_index<net_SwarmFlag>(net_SwarmFlag::kOfficerAims)))), 0.5)), now, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::mod(a, 5.0)); }, kern::to<Float>(swarms_[n20971559.index].units[n20971560.index].id))));
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
    for (auto it22020143 = flow_for(n22020143, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it22020143.next();) {
      skip = false;
      if (logic_not(projectiles_[n22020143.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (projectiles_[n22020143.index].ghost) {
          ghosts += 1;
          gp = projectiles_[n22020143.index].pos;
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
    engine_game_world_respawn_now(n23068687, world_, soldier_);
    respawn_now = n23068687.result;
    return {respawn_now};
  }

  void update_lamps() {
    gaps_lamps_place();
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
    n25165834 = state_of(cars_[ci].actor);
    s = n25165834.Result;
    yaw = common::heading_of(engine_math_math::Quat_rotate(s.rotation, Vector{1.0, 0.0, 0.0}).Result).Result;
    side = vector_make(kern::formula([](Float yaw) -> Float { return (-kern::math::sin(yaw)); }, yaw), kern::formula([](Float yaw) -> Float { return kern::math::cos(yaw); }, yaw), 0.0);
    at = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x + (x_2 * 1.8)); }, vector_break(s.position).x, vector_break(side).x), kern::formula([](Float y, Float y_2) -> Float { return (y + (y_2 * 1.8)); }, vector_break(s.position).y, vector_break(side).y), 0.0);
    engine_game_world_physics(n25165835, world_);
    physics = n25165835.result;
    if (struct_break<the_RayResult_>(common::cast(physics, s.position, vector_make(vector_break(side).x, vector_break(side).y, 0.0), 2.3, body(cars_[ci].actor).Result, Int64{0xffffffffffffffff}).Result).hit) {
      at = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x - (x_2 * 1.8)); }, vector_break(s.position).x, vector_break(side).x), kern::formula([](Float y, Float y_2) -> Float { return (y - (y_2 * 1.8)); }, vector_break(s.position).y, vector_break(side).y), 0.0);
    }
    at = vector_make(vector_break(at).x, vector_break(at).y, city_stream_cpp::CityStream_ground_height(city_, vector_break(at).x, vector_break(at).y).Result);
    arg = at;
    arg_2 = kern::formula([](Float yaw, Float kPi) -> Float { return (yaw + kern::math::div(kPi, 2.0)); }, yaw, kPi);
    n25165836 = common::Rng_next(rng_);
    n25165837 = people_cpp::People_add_citizen(people_, arg, arg_2, kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::bor(the_result, 1.0); }, kern::to<Float>(n25165836.Result))));
    p = n25165837.Result;
    if (logic_and(logic_and(math_compare<">=">(kern::to<Float>(cars_[ci].lane), 0.0), math_compare<"<">(kern::to<Float>(cars_[ci].lane), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(list_length<Lane>(city_.lanes_)))))))), city_.lanes_[cars_[ci].lane].alive)) {
      people_.people_[p].lane = cars_[ci].lane;
      people_.people_[p].seg = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return (a - ((a_2 > 0.0) ? 1.0 : 0.0)); }, kern::to<Float>(cars_[ci].index), kern::to<Float>(cars_[ci].dir))))), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (kern::math::wrap32s(a) - 2.0); }, kern::to<Float>(list_length<Vector>(city_.lanes_[cars_[ci].lane].points)))))))));
      n25165838 = common::Rng_unit(rng_);
      people_.people_[p].dir = kern::to<Int>(kern::formula([](Float the_result) -> Float { return ((the_result < 0.5) ? 1.0 : (-1.0)); }, n25165838.Result));
      people_.people_[p].side = 1.0;
      people_.people_[p].off = kern::formula([](Float a) -> Float { return ((a * 0.5) + 1.7); }, city_.lanes_[cars_[ci].lane].width);
      people_.people_[p].along = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(at).x, vector_break(city_.lanes_[cars_[ci].lane].points[people_.people_[p].seg]).x, vector_break(at).y, vector_break(city_.lanes_[cars_[ci].lane].points[people_.people_[p].seg]).y);
    }
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    n25165839 = common::Rng_unit(rng_);
    if (math_compare<"<">(n25165839.Result, 0.5)) {
      people_cpp::People_frighten(people_, s.position, 4.0, now, 1.2);
    } else {
      people_.people_[p].state = People_State::wait;
      people_.people_[p].wait_until = kern::formula([](Float now) -> Float { return (now + 2.6); }, now);
      people_.people_[p].yaw = kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(s.position).y, vector_break(at).y, vector_break(s.position).x, vector_break(at).x);
      list_add(life_[0].delayed_fright, struct_make<WorldLife_Fright>(p, kern::formula([](Float now) -> Float { return (now + 2.6); }, now), s.position));
    }
    life_[0].ejected = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(life_[0].ejected)));
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
    engine_game_world_physics(n25165840, world_);
    physics = n25165840.result;
    w = physics;
    for (auto it25165841 = flow_for(n25165841, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))))); it25165841.next();) {
      skip = false;
      either = logic_not(engine_physics_physics::Body_valid(life_[0].trucks[n25165841.index].body).Result);
      if (logic_not(either)) {
        body_valid_2 = engine_world_body_valid(w, life_[0].trucks[n25165841.index].body);
        either = logic_not(body_valid_2);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        pose = engine_world_pose(w, life_[0].trucks[n25165841.index].body);
        p = pose;
        m = engine_math_math::Mat4_trs(p.position, p.rotation, Vector{1.0, 1.0, 1.0}).Result;
        vehicle_art_cpp::VehicleArt_car(art_, m, life_[0].trucks[n25165841.index].car[0], kTowYellow, false, math_compare<"<">(life_[0].trucks[n25165841.index].input.pedal, -0.05));
      }
    }
  }

  struct car_towedResult { Bool Result = false; };
  car_towedResult car_towed(I32 car = 0) {
    if (logic_not(math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0))) {
      return {false};
    }
    for (auto it25165842 = flow_for(n25165842, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))))); it25165842.next();) {
      if (math_compare<"==">(kern::to<Float>(life_[0].trucks[n25165842.index].job), kern::to<Float>(car))) {
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
    engine_renderer_allocate_id(n27262993, r_);
    allocate_id = n27262993.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    look.material = mat;
    look.visible = false;
    engine_upsert(n27262994, r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n27262994.result;
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
      engine_renderer_update_instances(n27262998, r_, id, m.m, List<Float>{});
      update_instances = n27262998.result;
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
      for (auto it27262995 = flow_for(n27262995, 0, 11); it27262995.next();) {
        a0 = kern::formula([](Float index) -> Float { return ((-0.5) + kern::math::div(index, 12.0)); }, kern::to<Float>(n27262995.index));
        a1 = kern::formula([](Float index) -> Float { return ((-0.5) + kern::math::div((index + 1.0), 12.0)); }, kern::to<Float>(n27262995.index));
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
    for (auto it27262996 = flow_for(n27262996, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U32>(items))))); it27262996.next();) {
      id = items[n27262996.index];
      if (math_compare<"!=">(kern::to<Float>(id), 0.0)) {
        engine_renderer_set_visible(n27262997, r, id, show);
        set_visible = n27262997.result;
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
        engine_renderer_allocate_id(n27262999, r);
        allocate_id = n27262999.result;
        ui_.trail_mesh = allocate_id;
      }
      if (math_compare<"<">(kern::to<Float>(list_length<Vector>(ui_.trail)), 2.0)) {
        engine_renderer_set_visible(n27263000, r, ui_.trail_mesh, false);
        set_visible_2 = n27263000.result;
      } else {
        line = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
        blue_2 = Vector{0.26, 0.52, 0.96};
        for (auto it27263001 = flow_for(n27263001, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(ui_.trail))))); it27263001.next();) {
          item_5 = ui_.trail[kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n27263001.index)))];
          item_6 = ui_.trail[n27263001.index];
          items_2 = list_make<Vector, "2">(item_5, item_6);
          for (auto it27263002 = flow_for(n27263002, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(items_2))))); it27263002.next();) {
            engine_geo_mesh_data::MeshData_push(line, vector_make(vector_break(items_2[n27263002.index]).x, vector_break(items_2[n27263002.index]).y, kern::formula([](Float the_result) -> Float { return (the_result + 0.12); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(items_2[n27263002.index]).x, vector_break(items_2[n27263002.index]).y).Result)), Vector{0.0, 0.0, 1.0}, blue_2, Vector{0.0, 0.0, 0.0});
          }
        }
        look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
        look.material.lines = true;
        look.material.unlit = true;
        look.material.cast_shadow = false;
        look.material.frustum_culled = false;
        look.visible = true;
        engine_upsert(n27263003, r, ui_.trail_mesh, line, look, List<Float>{}, List<Float>{});
        upsert = n27263003.result;
        if (logic_not(upsert)) {
          engine_renderer_release_id(r, ui_.trail_mesh);
          ui_.trail_mesh = 0;
        }
      }
    }
    if (logic_and(math_compare<"!=">(kern::to<Float>(ui_.trail_mesh), 0.0), logic_not(gps_live::GpsLive_active(ui_.gps).Result))) {
      engine_renderer_set_visible(n27263004, r, ui_.trail_mesh, false);
      set_visible_3 = n27263004.result;
    } else if (logic_and(logic_and(math_compare<"!=">(kern::to<Float>(ui_.trail_mesh), 0.0), math_compare<">=">(kern::to<Float>(list_length<Vector>(ui_.trail)), 2.0)), logic_not(ui_.trail_dirty))) {
      engine_renderer_set_visible(n27263005, r, ui_.trail_mesh, true);
      set_visible_4 = n27263005.result;
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
    for (auto it27263006 = flow_for(n27263006, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceUnit>(units_))))); it27263006.next();) {
      if (logic_and(math_compare<">=">(kern::to<Float>(units_[n27263006.index].car), 0.0), math_compare<"!=">(kern::to<Float>(units_[n27263006.index].car), kern::to<Float>(car_index_)))) {
        engine_game_world_set_alive(world_, cars_[units_[n27263006.index].car].actor, false);
        cars_[units_[n27263006.index].car].placed = false;
      }
      if (math_compare<">=">(kern::to<Float>(units_[n27263006.index].car), 0.0)) {
        cars_[units_[n27263006.index].car].unit = -1;
      }
    }
    list_clear(units_);
    for (auto it27263007 = flow_for(n27263007, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<PoliceDrone>(pdrones_))))); it27263007.next();) {
      pdrones_[n27263007.index].active = false;
    }
    drone_wave_live_ = false;
    next_drone_wave_ = -1.0;
    star_since_ = -1.0;
  }

  void ui_mix_cues() {
    Float fx = 0.0;
    fx = kern::formula([](Float the_result) -> Float { return kern::math::max(0.01, kern::math::div(the_result, 72.0)); }, settings::Settings_get(ui_.settings, Opt::audio_fx).Result);
    engine_game_world_sound_cues(n27263011, world_);
    for (auto it27263012 = flow_for(n27263012, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<game_SoundCue>(n27263011.result))))); it27263012.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(n27263011.result[n27263012.index].actor), kern::to<Float>(soldier_)), logic_or(logic_or(text_equal(n27263011.result[n27263012.index].sound, "sc:shot0"), text_equal(n27263011.result[n27263012.index].sound, "sc:pistol0")), text_equal(n27263011.result[n27263012.index].sound, "builtin:sniper")))) {
        osfx("foot.shot", n27263011.result[n27263012.index].position, kern::to<Float>(enum_index<weapons_Id>(foot_weapon_)), 0.0, 0.0);
      }
      if (logic_or(text_equal(n27263011.result[n27263012.index].sound, "builtin:step"), text_equal(n27263011.result[n27263012.index].sound, "builtin:land"))) {
        n27263011.result[n27263012.index].volume = kern::formula([](Float a, Float the_result, Float fx) -> Float { return (a * kern::math::div(kern::math::div(the_result, 30.0), fx)); }, n27263011.result[n27263012.index].volume, settings::Settings_get(ui_.settings, Opt::audio_steps).Result, fx);
      } else if (logic_or(logic_or(logic_or(logic_or(text_equal(n27263011.result[n27263012.index].sound, "builtin:pistol"), text_equal(n27263011.result[n27263012.index].sound, "builtin:smg")), text_equal(n27263011.result[n27263012.index].sound, "builtin:sniper")), text_equal(n27263011.result[n27263012.index].sound, "builtin:rifle")), text_equal(n27263011.result[n27263012.index].sound, "builtin:shotgun"))) {
        n27263011.result[n27263012.index].volume = kern::formula([](Float a, Float the_result, Float fx) -> Float { return (a * kern::math::div(kern::math::div(the_result, 82.0), fx)); }, n27263011.result[n27263012.index].volume, settings::Settings_get(ui_.settings, Opt::audio_shots).Result, fx);
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
    n27263013 = ui_equipment_json();
    return {text_append(s, n27263013.Result, "")};
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

  void draw_projectiles() {
    for (auto it30408732 = flow_for(n30408732, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it30408732.next();) {
      switch (flow_switch_enum<weapons_Id>(projectiles_[n30408732.index].id)) {
        case weapons_Id::smg: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::glock: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::sniper: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::grenade_launcher: {
        } break;
        case weapons_Id::rocket: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::fists: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::knife: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::hand_grenade: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.05, 0.14, false);
        } break;
        case weapons_Id::drone_gun: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::drone_rockets: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::drone_guided: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::jet_cannon: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.06, 0.7, true);
        } break;
        case weapons_Id::jet_missile: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.12, 2.8, true);
        } break;
        case weapons_Id::jet_bomb: {
          people_art_cpp::PeopleArt_projectile(people_art_, projectiles_[n30408732.index].pos, projectiles_[n30408732.index].vel, 0.25, 1.9, false);
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

  void world_bullet(Vector at = Vector{0.0, 0.0, 0.0}) {
    if (math_compare<">">(kern::to<Float>(list_length<WorldLife>(life_)), 0.0)) {
      n31457292 = world_marks_cpp::Marks_bullet(life_[0].marks, city_, rng_, at);
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
    for (auto it31457293 = flow_for(n31457293, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldLife_TowTruck>(life_[0].trucks))))); it31457293.next();) {
      skip = false;
      either = logic_not(engine_physics_physics::Body_valid(life_[0].trucks[n31457293.index].body).Result);
      if (logic_not(either)) {
        engine_game_world_physics(n31457295, world_);
        physics_2 = n31457295.result;
        body_valid = engine_world_body_valid(physics_2, life_[0].trucks[n31457293.index].body);
        either = logic_not(body_valid);
      }
      if (either) {
        skip = true;
      }
      if (logic_not(skip)) {
        engine_game_world_physics(n31457296, world_);
        physics_3 = n31457296.result;
        pose = engine_world_pose(physics_3, life_[0].trucks[n31457293.index].body);
        d = vector_length(vector_sub(struct_break<physics_Pose>(pose).position, at));
        if (math_compare<"<">(d, radius)) {
          life_[0].trucks[n31457293.index].hp = kern::formula([](Float a, Float damage, Float d, Float radius) -> Float { return (a - ((damage * 1.3) * kern::math::pow((1.0 - kern::math::div(d, radius)), 1.2))); }, life_[0].trucks[n31457293.index].hp, damage, d, radius);
        }
      }
    }
    town_mark(at, radius, kern::formula([](Float damage) -> Float { return kern::math::min(1.4, kern::math::div(damage, 110.0)); }, damage), settings::Settings_on(ui_.settings, Opt::world_terrain).Result);
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
    engine_game_world_physics(n32505869, world_);
    physics = n32505869.result;
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
  struct { Int result = 0; } n3145750;
  struct { Bool result = false; } n3145751;
  gaps_barricade_doorsResult n3145753;
  struct : kern::Node<3145754> { Int index = 0; } n3145754;
  struct : kern::Node<3145755> { Int index = 0; } n3145755;
  struct { Bool result = false; } n3145756;
  struct { Bool result = false; Bool changed = false; } n3145757;
  struct { Bool result = false; } n3145758;
  project_screenResult n4194314;
  struct : kern::Node<5242894> { Int index = 0; } n5242894;
  struct { List<Float> out = List<Float>{}; } n5242897;
  struct : kern::Node<5242898> { Int index = 0; } n5242898;
  struct : kern::Node<5242899> { Int index = 0; } n5242899;
  struct : kern::Node<5242900> { Int index = 0; } n5242900;
  struct : kern::Node<6291466> { Int index = 0; } n6291466;
  game_destruction_sync_cpp::digest_ofResult n6291484;
  struct : kern::Node<7340042> { Int index = 0; } n7340042;
  struct : kern::Node<7340043> { Int index = 0; } n7340043;
  struct : kern::Node<7340044> { Int index = 0; } n7340044;
  struct : kern::Node<7340045> { Int index = 0; } n7340045;
  combat_audio_cpp::renderResult n7340046;
  struct : kern::Node<7340047> { Int index = 0; } n7340047;
  sfx_idResult n7340048;
  struct { Bool result = false; } n7340049;
  sfx_idResult n7340050;
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
  struct : kern::Node<8388643> { Int index = 0; } n8388643;
  nostr_cpp::from_hexResult n8388644;
  nostr_cpp::from_hexResult n8388646;
  nostr_cpp::event_jsonResult n8388647;
  struct { Bool result = false; } n8388648;
  combat_rules_cpp::Chaos_awardResult n9437195;
  struct : kern::Node<9437198> { Int index = 0; } n9437198;
  struct { Debris value = Debris{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 6.5}; } n9437199;
  struct : kern::Node<9437200> { Int index = 0; } n9437200;
  struct : kern::Node<10485770> { Int index = 0; } n10485770;
  struct : kern::Node<10485771> { Int index = 0; } n10485771;
  struct : kern::Node<10485772> { Int index = 0; } n10485772;
  state_ofResult n10485773;
  remote_on_segmentResult n10485774;
  combat_sfxResult n10485775;
  struct : kern::Node<10485776> { Int index = 0; } n10485776;
  struct : kern::Node<10485786> { Int index = 0; } n10485786;
  struct : kern::Node<10485787> { Int index = 0; } n10485787;
  struct : kern::Node<10485788> { Int index = 0; } n10485788;
  struct { GapsState_SniperRound value = GapsState_SniperRound{game_ShotEvent{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, false, false, Vector{0.0, 0.0, 1.0}, 0}, 0.0}; } n10485789;
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
  struct : kern::Node<11534367> { Int index = 0; } n11534367;
  struct : kern::Node<11534368> { Int index = 0; } n11534368;
  struct : kern::Node<11534369> { Int index = 0; } n11534369;
  struct { Bool result = false; } n11534370;
  struct { Bool result = false; Bool changed = false; } n11534371;
  struct { Bool result = false; } n11534372;
  struct { Bool result = false; } n11534373;
  struct { Bool result = false; Bool changed = false; } n11534374;
  struct { Bool result = false; } n11534375;
  struct { Bool result = false; } n11534376;
  struct { Bool result = false; Bool changed = false; } n11534377;
  struct { Bool result = false; } n11534378;
  struct { Bool result = false; } n11534379;
  struct { Bool result = false; Bool changed = false; } n11534380;
  struct { Bool result = false; } n11534381;
  struct : kern::Node<12582922> { Int index = 0; } n12582922;
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
  struct : kern::Node<14680074> { Int index = 0; } n14680074;
  life_cpp::Life_shotResult n14680075;
  life_cpp::Life_shotResult n14680077;
  struct : kern::Node<14680078> { Int index = 0; } n14680078;
  struct : kern::Node<14680079> { Int index = 0; } n14680079;
  struct : kern::Node<14680080> { Int index = 0; } n14680080;
  struct : kern::Node<14680081> { Int index = 0; } n14680081;
  struct : kern::Node<14680082> { Int index = 0; } n14680082;
  struct : kern::Node<14680083> { Int index = 0; } n14680083;
  engine_math_math::Mat4_look_at_2Result n14680084;
  struct : kern::Node<14680085> { Int index = 0; } n14680085;
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
  struct : kern::Node<16777233> { Int index = 0; } n16777233;
  mp_art_cpp::MpArt_initResult n17825802;
  struct : kern::Node<17825803> { Int index = 0; } n17825803;
  struct : kern::Node<17825804> { Int index = 0; } n17825804;
  struct : kern::Node<17825805> { Int index = 0; } n17825805;
  net_session::Session_world_rulesResult n17825806;
  struct : kern::Node<17825807> { Int index = 0; } n17825807;
  struct : kern::Node<17825808> { Int index = 0; } n17825808;
  struct : kern::Node<17825809> { Int index = 0; } n17825809;
  struct : kern::Node<17825810> { Int index = 0; } n17825810;
  struct : kern::Node<17825815> { Int index = 0; } n17825815;
  mp_rules_cpp::VoiceStream_renderResult n17825816;
  person_hit::hitResult n17825817;
  struct : kern::Node<17825818> { Int index = 0; } n17825818;
  remote_on_segment_zonesResult n17825819;
  remote_on_segment_zonesResult n17825820;
  remote_on_segment_zonesResult n17825821;
  project_screenResult n17825824;
  mp_rules_cpp::safe_mobile_marker_pointResult n17825825;
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
  struct : kern::Node<20971555> { Int index = 0; } n20971555;
  struct : kern::Node<20971556> { Int index = 0; } n20971556;
  struct : kern::Node<20971557> { Int index = 0; } n20971557;
  struct : kern::Node<20971558> { Int index = 0; } n20971558;
  struct : kern::Node<20971559> { Int index = 0; } n20971559;
  struct : kern::Node<20971560> { Int index = 0; } n20971560;
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
  struct : kern::Node<22020143> { Int index = 0; } n22020143;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n23068682;
  struct { Bool result = false; } n23068687;
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
  state_ofResult n25165834;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n25165835;
  common::Rng_nextResult n25165836;
  people_cpp::People_add_citizenResult n25165837;
  common::Rng_unitResult n25165838;
  common::Rng_unitResult n25165839;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n25165840;
  struct : kern::Node<25165841> { Int index = 0; } n25165841;
  struct : kern::Node<25165842> { Int index = 0; } n25165842;
  struct : kern::Node<26214410> { Int index = 0; } n26214410;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n26214412;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n26214413;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n26214414;
  struct { List<Int64> out = List<Int64>{}; } n26214415;
  struct { Int result = 0; } n27262993;
  struct { Bool result = false; } n27262994;
  struct : kern::Node<27262995> { Int index = 0; } n27262995;
  struct : kern::Node<27262996> { Int index = 0; } n27262996;
  struct { Bool result = false; } n27262997;
  struct { Bool result = false; Bool changed = false; } n27262998;
  struct { Int result = 0; } n27262999;
  struct { Bool result = false; } n27263000;
  struct : kern::Node<27263001> { Int index = 0; } n27263001;
  struct : kern::Node<27263002> { Int index = 0; } n27263002;
  struct { Bool result = false; } n27263003;
  struct { Bool result = false; } n27263004;
  struct { Bool result = false; } n27263005;
  struct : kern::Node<27263006> { Int index = 0; } n27263006;
  struct : kern::Node<27263007> { Int index = 0; } n27263007;
  struct { engine_game_world_sound_cues_place<List<game_SoundCue>> result = {}; } n27263011;
  struct : kern::Node<27263012> { Int index = 0; } n27263012;
  ui_equipment_jsonResult n27263013;
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
  struct : kern::Node<30408732> { Int index = 0; } n30408732;
  world_marks_cpp::Marks_bulletResult n31457292;
  struct : kern::Node<31457293> { Int index = 0; } n31457293;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n31457295;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n31457296;
  struct : kern::Node<32505866> { Int index = 0; } n32505866;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n32505867;
  car_cpp::Car_rigid_specResult n32505868;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n32505869;
};
