struct Game : virtual kern::Blueprint, virtual city_stream, virtual combat_rules_cpp, virtual common, virtual engine_gfx_passes, virtual engine_math_math, virtual engine_physics_physics, virtual game, virtual game_combat, virtual game_controls, virtual game_gaps, virtual game_life, virtual game_look, virtual game_look_cpp, virtual game_toss, virtual game_ui, virtual jet, virtual life, virtual net_session, virtual people, virtual perf, virtual settings {
  // ---- variables ----
  TossState toss_ = TossState{List<TossState_Own>{}, List<TossState_Remote>{}, 0.0, 0, 0};
  List<GapsState> gaps_ = List<GapsState>{};
  Int engine_ = 0;
  Fixed<Game_VrHand, 2> vr_hands_ = List<Game_VrHand>{};
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
  Camera cam_ = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Bool rig_init_ = false;
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  /// the town
  People people_ = People{List<People_Person>{}, List<CrimeReport>{}, List<People_Death>{}, List<People_Fall>{}, List<People_RemoteHit>{}, List<People_Hurt>{}, false, false, 60, 1, 0, 0, 170.0, 190.0};
  CombatState combat_ = CombatState{combat_MilitaryClock{false, -1.0, -1.0, 0.0}, 0, 0, 0, 0.0, List<Heli>{}, List<Tank>{}, List<Shell>{}, List<SoldierAi>{}, List<Float>{}, true, List<FlakGun>{}, List<FlakRound>{}, List<Puff>{}, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0, 0, List<FighterJet>{}, List<Bomb>{}, List<Blast>{}, List<Flare>{}, 50.0, -1000000000.0, List<Tracer>{}, List<Debris>{}, List<Int>{}, List<Bool>{}, List<Bool>{}, false, 0, 0, 0, combat_ZombieRounds{false, 0, 0, 0, 0.0, -1.0}, List<ZombieAi>{}, true, false, -99.0, List<EnemyAi>{}, List<EnemyRocket>{}, combat_Chaos{0, 1, -1.0, 0.0}, -99.0, -1.0, 0, combat_Fun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, -1.0, combat_Race{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}, false, Vector{0.0, 0.0, 0.0}, "", "", -99.0, -99.0, false, 0.0};
  /// weapons
  List<Projectile> projectiles_ = List<Projectile>{};
  /// multiplayer
  net_Session net_ = net_Session{net_Session_Role::none, 0, 0, 0, net_Address{Int64{0x0}}, List<net_Remote>{}, List<net_Hit>{}, List<net_Hit>{}, List<net_Kill>{}, List<net_Fx>{}, List<net_Swarm>{}, List<net_SwarmHit>{}, List<Int_row>{}, List<net_PingEvent>{}, List<net_VoiceFrame>{}, List<Int>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, false, false, false, 0.0, 0, 0.0, List<Int>{}, Vector{0.0, 0.0, 0.0}, 255, net_PlayerState{0, net_PlayerMode::foot, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 0.0, 0.0, 100.0, 0.0, 0, 0, -1.0, List<Int>{0, 0, 0, 0, 0, 0}, 0, 255}, mp_Scoreboard{List<mp_Score>{}}, false, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_Blob>{}, List<net_Session_SwarmParts>{}, 0, 0, false, List<Int_Float_pair>{}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, mp_Ballot{0, 0, false, 0, List<Int>{}, List<Int>{}, 0.0, mp_Ballot_Status::pending}, 0, "Aenderungen nur einstimmig", 0, 0, 0, seats_Table{List<seats_Row>{}, 0}, 0.0};
  Float night_ = 0.0;
  Bool player_dead_ = false;
  /// vs_combat_presentation.mjs: down until the player asks (R / Enter / Space, pad A / Y, the RESPAWN button) - not before 3 s (RESPAWN_COOLDOWN_MS); back 3..5 m from where he went down
  Float died_at_ = -1.0;
  /// rules / feedback (the wanted level is the "Wanted" Blueprint's: wanted.h)
  Text toast_ = "";
  Float toast_age_ = 99.0;
  Rng rng_ = Rng{20261029, 0.0, false};
  Int bank_ = 0;
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
  KERN_CONST combat_EnemySpec kNoEnemy = combat_EnemySpec{"", 1.0, 0.0, 0.0, 1.0, 1, 0.0, 0.0, 0, 1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}};
  KERN_CONST combat_Contract kNoContract = combat_Contract{"", "", "", 1, 0};
  KERN_CONST combat_Gate kNoGate = combat_Gate{"", Vector{0.0, 0.0, 0.0}, 1.0, 1.0, false};
  KERN_CONST Float kRespawnCooldown = 3.0;
  /// jet_mode.mjs
  KERN_CONST Float kEjectUp = 28.0;
  /// jet_mode.mjs
  KERN_CONST Float kChuteOpen = 1.1;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSink = 6.5;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSteer = 4.0;
  KERN_CONST Fixed<Text, 8> kGateNames = List<Text>{"START / FINISH", "GATE 2", "GATE 3", "HIGH GATE", "GATE 5", "GATE 6", "LOW GATE", "GATE 8"};
  /// vertex locations 14 and 15
  KERN_CONST Int kMaxExtensionAttributes = 2;
  KERN_CONST Float kDeg = 0.017453292519943295;
  KERN_CONST Fixed<perf_Tier, 5> kTier = List<perf_Tier>{perf_Tier{"minimal", 16, 35.0, 1, 0, 70.0, 90.0, 2, 0.6}, perf_Tier{"niedrig", 30, 50.0, 4, 0, 110.0, 140.0, 3, 0.75}, perf_Tier{"mittel", 44, 70.0, 4, 1, 140.0, 180.0, 4, 1.0}, perf_Tier{"hoch", 64, 90.0, 9, 2, 170.0, 220.0, 6, 1.0}, perf_Tier{"maximal", 80, 110.0, 9, 3, 170.0, 220.0, 10, 1.0}};
  KERN_CONST Fixed<Text, 4> kVmNames = List<Text>{"BOOMSTICK 40 mm", "ROCKET LAUNCHER", "FISTS", "KNIFE"};
  KERN_CONST Int kPerson = 0;
  KERN_CONST Int kBird = 3;
  KERN_CONST Int kCow = 1;
  KERN_CONST Int kPet = 2;

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
    n2097176 = state_of(jet_actor_);
    s = n2097176.Result;
    jet_.pilot = false;
    engine_game_world_set_alive(world_, chute_actor_, true);
    engine_game_world_teleport(n2097177, world_, chute_actor_, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 2.8}).Result), List<Vector>{});
    teleport = n2097177.result;
    engine_game_world_set_actor_velocity(n2097178, world_, chute_actor_, vector_add(s.velocity, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kEjectUp)).Result));
    set_actor_velocity = n2097178.result;
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
    n2097181 = common::Rng_next(rng_);
    start_2 = n2097181.Result;
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
    engine_game_world_physics(n2097182, world_);
    physics = n2097182.result;
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

  void combat_hud(Text& s) {
    Float now = 0.0;
    Float sim_time = 0.0;
    Int soldiers = 0;
    Int fighters = 0;
    Int flak_alive = 0;
    combat_Contract k = combat_Contract{"", "", "", 0, 0};
    Text b = "";
    Float cur = 0.0;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    soldiers = 0;
    fighters = 0;
    for (auto it3145749 = flow_for(n3145749, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<SoldierAi>(combat_.soldiers))))); it3145749.next();) {
      soldiers += kern::to<Int>(math_compare<">=">(kern::to<Float>(combat_.soldiers[n3145749.index].person), 0.0));
    }
    for (auto it3145750 = flow_for(n3145750, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<People_Person>(people_.people_))))); it3145750.next();) {
      fighters += kern::to<Int>(enum_equal<People_State>(people_.people_[n3145750.index].state, People_State::fight));
    }
    flak_alive = 0;
    for (auto it3145751 = flow_for(n3145751, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<FlakGun>(combat_.flak))))); it3145751.next();) {
      flak_alive += kern::to<Int>(logic_not(combat_.flak[n3145751.index].dead));
    }
    k = contract(combat_.fun.contract_index).Result;
    b = "";
    n3145752 = combat_rules_cpp::Fun_multiplier(combat_.fun, now);
    b = text_format<",\"combat\":{\"mil\":\"%s\",\"helis\":%zu,\"tanks\":%zu,\"soldiers\":%d,\"flak\":%u,\"flakDestroyed\":%u,\"flakHits\":%u,\"flakRounds\":%zu,\"jets\":%zu,\"bombs\":%zu,\"strike\":%.1f,\"fighters\":%d,\"gangsterShots\":%u,\"enemies\":%zu,\"zombies\":{\"night\":%s,\"round\":%d,\"alive\":%zu,\"points\":%d},\"chaos\":{\"score\":%d,\"combo\":%d,\"flow\":%.2f},\"fun\":{\"score\":%d,\"risk\":%d,\"high\":%d,\"combo\":%d,\"mult\":%.2f,\"contract\":\"%s\",\"hint\":\"%s\",\"progress\":%d,\"target\":%d,\"done\":%s},\"scope\":%s,\"debris\":%zu,\"drone\":%.1f">(767, logic_select<Text>(combat_.military.active, "on", logic_select<Text>(math_compare<">=">(combat_.military.alert_at, 0.0), "alert", "off")), list_length<Heli>(combat_.helis), list_length<Tank>(combat_.tanks), soldiers, flak_alive, combat_.flak_destroyed, combat_.flak_hits, list_length<FlakRound>(combat_.flak_rounds), list_length<FighterJet>(combat_.jets), list_length<Bomb>(combat_.bombs), kern::formula([](Float combat_kStrikeCooldownS, Float now, Float last_strike) -> Float { return kern::math::max(0.0, (combat_kStrikeCooldownS - (now - last_strike))); }, combat_kStrikeCooldownS, now, combat_.last_strike), fighters, combat_.gangster_shots, list_length<EnemyAi>(combat_.enemies), logic_select<Text>(combat_.zombies.active, "true", "false"), combat_.zombies.the_round, list_length<ZombieAi>(combat_.zombie_ai), combat_.zombies.points, combat_.chaos.score, combat_.chaos.combo, combat_.chaos.flow, combat_.fun.score, combat_.fun.at_risk, combat_.fun.high_score, combat_.fun.combo, n3145752.Result, k.name, k.hint, combat_.fun.contract_progress, k.target, logic_select<Text>(combat_.fun.contract_complete, "true", "false"), logic_select<Text>(combat_.scope, "true", "false"), list_length<Debris>(combat_.debris), kern::formula([](Float drone_ready_at, Float now) -> Float { return kern::math::max(0.0, (drone_ready_at - now)); }, combat_.drone_ready_at, now));
    s = text_append(s, b, "");
    if (combat_.race_on) {
      cur = kern::formula([](Float started, Float finished, Float last_lap, Float now, Float lap_start) -> Float { return ((started != 0.0) ? ((finished != 0.0) ? last_lap : (now - lap_start)) : 0.0); }, kern::to<Float>(combat_.race.started), kern::to<Float>(combat_.race.finished), combat_.race.last_lap, now, combat_.race.lap_start);
      b = text_format<",\"race\":{\"lap\":%d,\"laps\":%d,\"next\":\"%s\",\"time\":\"%s\",\"best\":\"%s\",\"finished\":%s}">(767, combat_.race.lap, combat_kRaceLaps, logic_select<Text>(combat_.race.finished, "FINISHED", list_get<Text>(kGateNames, combat_.race.next)), combat_rules_cpp::race_time(cur).Result, combat_rules_cpp::race_time(combat_.race.best_lap).Result, logic_select<Text>(combat_.race.finished, "true", "false"));
      s = text_append(s, b, "");
    }
    if (math_compare<"<">(kern::formula([](Float now, Float banner_at) -> Float { return (now - banner_at); }, now, combat_.banner_at), 3.8)) {
      s = text_append(s, text_append(text_append(",\"banner\":\"", combat_.banner, ""), "\"", ""), "");
    }
    if (math_compare<"<">(kern::formula([](Float now, Float pop_at) -> Float { return (now - pop_at); }, now, combat_.pop_at), 1.5)) {
      s = text_append(s, text_append(text_append(",\"pop\":\"", combat_.pop, ""), "\"", ""), "");
    }
    s = text_append(s, "}", "");
  }

  struct combat_command_startsResult { Bool Result = false; };
  KERN_PURE combat_command_startsResult combat_command_starts(const Text& cmd, Text p = "") {
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
    engine_renderer_allocate_id(n4194315, renderer);
    allocate_id = n4194315.result;
    p.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    renderer_2 = engine_engine_renderer(e);
    engine_upsert(n4194316, renderer_2, p.id, disc, look, parked.m, List<Float>{});
    upsert = n4194316.result;
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
    for (auto it4194317 = flow_for(n4194317, 0, 1); it4194317.next();) {
      for (auto it4194318 = flow_for(n4194318, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float rate) -> Float { return (rate * 0.05); }, kern::to<Float>(rate)))))))); it4194318.next();) {
        n = kern::to<Int>(kern::formula([](Float index, Float rate, Float index_2) -> Float { return (kern::math::trunc(((index * 0.09) * rate)) + index_2); }, kern::to<Float>(n4194317.index), kern::to<Float>(rate), kern::to<Float>(n4194318.index)));
        if (math_compare<">">(kern::formula([](Float n, Float a) -> Float { return ((n < 0.0) != (a < 0.0) ? (a < 0.0) : (n < a)); }, kern::to<Float>(n), kern::to<Float>(list_length<Float>(lock))), 0.5)) {
          lock[n] = kern::formula([](Float index, Float rate) -> Float { return ((kern::math::mod(kern::math::div((index * 1760.0), rate), 1.0) < 0.5) ? 0.05 : (-0.05)); }, kern::to<Float>(n4194318.index), kern::to<Float>(rate));
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
    engine_sound_bank_set_pcm(n4194319, bank_, "sc:locktone", lock, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(lock)))), 1, rate);
    set_pcm = n4194319.result;
    engine_sound_bank_request(bank_, "sc:guidedbang");
    engine_sound_bank_set_pcm(n4194320, bank_, "sc:guidedbang", bang, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(bang)))), 1, rate);
    set_pcm_2 = n4194320.result;
    gaps_[0].guided_sounds = true;
  }

  struct gaps_command_startsResult { Bool Result = false; };
  KERN_PURE gaps_command_startsResult gaps_command_starts(const Text& cmd, Text p = "") {
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
      engine_light_pool_make(n5242890, look_.pool_size, 96);
      LightPool_2 = n5242890.result;
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

  void look_view_models_make(const MeshData& d, Int& id, Int r = 0, Int n5242896_index = 0, Bool g = false) {
    Int r_ = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    r_ = r;
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(d.positions)), 0.0)) {
      return;
    }
    engine_renderer_allocate_id(n5242898, r_);
    allocate_id = n5242898.result;
    id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = game_look_cpp::vm_material(g, math_compare<"==">(kern::to<Float>(n5242896_index), 1.0)).Result;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    engine_upsert(n5242899, r_, id, d, look, List<Float>{}, List<Float>{});
    upsert = n5242899.result;
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
    for (auto it6291466 = flow_for(n6291466, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TossState_Own>(toss_.own))))); it6291466.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(toss_.own[n6291466.index].kind), kern::to<Float>(kind)), int64_equal(toss_.own[n6291466.index].id, net_id))) {
        toss_.own[n6291466.index].still = 0.0;
        toss_.own[n6291466.index].done_sent = 0;
        toss_.own[n6291466.index].local = local;
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
    n6291467 = life::Life_animal_of(glife_[0].life, id, b);
    a = n6291467.Result;
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
    for (auto it6291470 = flow_for(n6291470, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Game_VrHand>(vr_hands_))))); it6291470.next();) {
      if (logic_and(math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kPerson)), math_compare<"==">(kern::to<Float>(vr_hands_[n6291470.index].grab_person), kern::to<Float>(o.local)))) {
        return {true};
      }
      if (logic_and(math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kBird)), math_compare<"==">(kern::to<Float>(vr_hands_[n6291470.index].bird), kern::to<Float>(o.local)))) {
        return {true};
      }
      if (logic_and(logic_or(math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kCow)), math_compare<"==">(kern::to<Float>(o.kind), kern::to<Float>(kPet))), engine_physics_physics::Body_valid(vr_hands_[n6291470.index].grab).Result)) {
        id = Int64{0x0};
        n6291471 = life::Life_animal_of(glife_[0].life, id, vr_hands_[n6291470.index].grab);
        a = n6291471.Result;
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
    for (auto it6291477 = flow_for(n6291477, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Projectile>(projectiles_))))); it6291477.next();) {
      skip = false;
      if (logic_not(projectiles_[n6291477.index].alive)) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (projectiles_[n6291477.index].ghost) {
          ghosts += 1;
          gp = projectiles_[n6291477.index].pos;
        } else {
          rounds += 1;
        }
      }
    }
    b = text_format<"{\"own\":%zu,\"remote\":%zu,\"sent\":%u,\"got\":%u,\"rounds\":%u,\"ghosts\":%u,\"ghost\":[%.2f,%.2f,%.2f]}">(255, list_length<TossState_Own>(toss_.own), list_length<TossState_Remote>(toss_.remote), toss_.sent, toss_.got, rounds, ghosts, vector_break(gp).x, vector_break(gp).y, vector_break(gp).z);
    return {b};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { combat_GateRow gate = combat_GateRow{"", Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false}; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct { Int result = 0; } n1048586;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n2097162;
  struct : kern::Node<2097167> { Int index = 0; } n2097167;
  struct { Bool result = false; } n2097171;
  state_ofResult n2097176;
  struct { Bool result = false; } n2097177;
  struct { Bool result = false; } n2097178;
  common::Rng_nextResult n2097181;
  struct { Int result = 0; } n2097182;
  struct : kern::Node<3145749> { Int index = 0; } n3145749;
  struct : kern::Node<3145750> { Int index = 0; } n3145750;
  struct : kern::Node<3145751> { Int index = 0; } n3145751;
  combat_rules_cpp::Fun_multiplierResult n3145752;
  struct { Int result = 0; } n4194315;
  struct { Bool result = false; } n4194316;
  struct : kern::Node<4194317> { Int index = 0; } n4194317;
  struct : kern::Node<4194318> { Int index = 0; } n4194318;
  struct { Bool result = false; } n4194319;
  struct { Bool result = false; } n4194320;
  struct { Int result = 0; } n5242890;
  struct { Int result = 0; } n5242898;
  struct { Bool result = false; } n5242899;
  struct : kern::Node<6291466> { Int index = 0; } n6291466;
  life::Life_animal_ofResult n6291467;
  struct : kern::Node<6291470> { Int index = 0; } n6291470;
  life::Life_animal_ofResult n6291471;
  struct : kern::Node<6291477> { Int index = 0; } n6291477;
};
