struct Game : virtual kern::Blueprint, virtual city_stream_cpp, virtual combat_art_cpp, virtual combat_rules_cpp, virtual common, virtual drone, virtual engine_game_runtime, virtual engine_math_math, virtual engine_physics_physics, virtual game, virtual game_combat, virtual game_controls, virtual jet, virtual people, virtual terrain_cpp {
  // ---- variables ----
  Int world_ = 0;
  CityStream city_ = CityStream{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 2600, 1, List<Vector>{}, 1, false, 0, 0, List<Vec4>{}, CityStream_NearCollision{physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, List<Int64_Int_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0}, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, 0.0, 0.0, 0.0, "", 0.0, 0, 0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, Map<Int, CityStream_Damage>{}, List<Int>{}, false, 0, 0, GeoProjector{0.0, 0.0, 1.0}, TileKey{14, 0, 0}, Map<Int64, List<CityStream_Tile>>{}, List<TileKey>{}, List<Lane>{}, false, 0, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<Int64, Terrain_Image>{}, List<TileKey>{}, Vector{1000000000.0, 1000000000.0, 0.0}, 0, 0, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0, false, Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<Int64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<Int64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<Int64, Terrain_RenderTile>{}, Map<Int64, Terrain_CollisionTile>{}, Map<Int64, Int>{}, Map<Int64, Bool>{}, Set<Int64>{}, Set<Int64>{}, Set<Int64>{}, List<Int64>{}, List<Int64>{}, List<Int64>{}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false}}, List<Vec4>{}, Int64{0x0}, -1};
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
  Bool chute_open_ = false;
  Float chute_time_ = 0.0;
  Vector chute_steer_ = Vector{0.0, 0.0, 0.0};
  Camera cam_ = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Vector cam_anchor_ = Vector{0.0, 0.0, 0.0};
  Vector cam_vel_ = Vector{0.0, 0.0, 0.0};
  Vector cam_heading_ = Vector{1.0, 0.0, 0.0};
  Bool rig_init_ = false;
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  /// the town
  People people_ = People{List<People_Person>{}, List<CrimeReport>{}, List<People_Death>{}, List<People_Fall>{}, List<People_RemoteHit>{}, List<People_Hurt>{}, false, false, 60, 1, 0, 0, 170.0, 190.0};
  CombatArt combat_art_ = CombatArt{0, List<CombatArt_Part>{}};
  CombatState combat_ = CombatState{combat_MilitaryClock{false, -1.0, -1.0, 0.0}, 0, 0, 0, 0.0, List<Heli>{}, List<Tank>{}, List<Shell>{}, List<SoldierAi>{}, List<Float>{}, true, List<FlakGun>{}, List<FlakRound>{}, List<Puff>{}, -99.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0, 0, List<FighterJet>{}, List<Bomb>{}, List<Blast>{}, List<Flare>{}, 50.0, -1000000000.0, List<Tracer>{}, List<Debris>{}, List<Int>{}, List<Bool>{}, List<Bool>{}, false, 0, 0, 0, combat_ZombieRounds{false, 0, 0, 0, 0.0, -1.0}, List<ZombieAi>{}, true, false, -99.0, List<EnemyAi>{}, List<EnemyRocket>{}, combat_Chaos{0, 1, -1.0, 0.0}, -99.0, -1.0, 0, combat_Fun{0, 0, 0, 0, 0, 0, 0, 0, -1000000000.0, -1000000000.0, 0, 0, 0, 0, 0, 0.0, false, "READY"}, -1.0, combat_Race{false, false, 0, 0, 0.0, 0.0, -1.0, -1.0, -1.0, -99.0, false, Vector{0.0, 0.0, 0.0}}, false, Vector{0.0, 0.0, 0.0}, "", "", -99.0, -99.0, false, 0.0};
  Float night_ = 0.0;
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
  Rng rng_ = Rng{20261029, 0.0, false};
  /// world_spawn_guard (Game::spawn_guard)
  Bool spawn_checked_ = false;
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
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST Float kDeg = 0.017453292519943295;
  /// m: the safe margin a player is put down over the ground
  KERN_CONST Float kSpawnLift = 0.35;
  /// jet_mode.mjs
  KERN_CONST Float kEjectUp = 28.0;
  /// jet_mode.mjs
  KERN_CONST Float kChuteOpen = 1.1;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSink = 6.5;
  /// jet_mode.mjs
  KERN_CONST Float kChuteSteer = 4.0;
  KERN_CONST Fixed<Text, 8> kGateNames = List<Text>{"START / FINISH", "GATE 2", "GATE 3", "HIGH GATE", "GATE 5", "GATE 6", "LOW GATE", "GATE 8"};
  KERN_CONST Float kG = 9.80665;

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
    engine_game_world_physics(n2097168, world_);
    physics = n2097168.result;
    w = physics;
    r = 0.0;
    while (loop(math_compare<"<=">(r, search))) {
      steps = kern::to<Int>(kern::formula([](Float r, Float kPi) -> Float { return ((r == 0.0) ? 1.0 : kern::math::trunc(kern::math::div(((2.0 * kPi) * r), 8.0))); }, r, kPi));
      for (auto it2097169 = flow_for(n2097169, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(steps)))); it2097169.next();) {
        a = kern::formula([](Float kPi, Float index, Float steps) -> Float { return kern::math::div(((2.0 * kPi) * index), steps); }, kPi, kern::to<Float>(n2097169.index), kern::to<Float>(steps));
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
    engine_game_world_physics(n2097180, world_);
    physics = n2097180.result;
    r = common::cast(physics, vector_make(vector_break(feet).x, vector_break(feet).y, kern::formula([](Float z, Float ground) -> Float { return (kern::math::max(z, ground) + 1.0); }, vector_break(feet).z, ground)), Vector{0.0, 0.0, -1.0}, kern::formula([](Float z, Float ground) -> Float { return ((1.0 + kern::math::abs((z - ground))) + 1.0); }, vector_break(feet).z, ground), body(soldier_).Result, Int64{0x1}).Result;
    z = ground;
    if (logic_and(logic_and(r.hit, math_compare<">">(vector_break(r.normal).z, 0.6)), math_compare<">">(vector_break(r.point).z, kern::formula([](Float ground) -> Float { return (ground - 0.05); }, ground)))) {
      z = vector_break(r.point).z;
    }
    return {vector_make(vector_break(feet).x, vector_break(feet).y, kern::formula([](Float z) -> Float { return (z + 0.02); }, z))};
  }

  void put_down(Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw_deg = 0.0) {
    Vector rot = Vector{0.0, 0.0, 0.0};
    Int arg = 0;
    Int arg_2 = 0;
    Bool teleport = false;
    Bool set_actor_velocity = false;
    Float sim_time = 0.0;
    rot = vector_make(0.0, 0.0, yaw_deg);
    arg = world_;
    arg_2 = soldier_;
    n2097181 = standing_feet(feet);
    engine_game_world_teleport(n2097182, arg, arg_2, vector_add(n2097181.Result, vector_make(0.0, 0.0, kern::formula([](Float kSpawnLift) -> Float { return (0.73 + kSpawnLift); }, kSpawnLift))), list_make<Vector, "1">(rot));
    teleport = n2097182.result;
    engine_game_world_set_actor_velocity(n2097183, world_, soldier_, Vector{0.0, 0.0, 0.0});
    set_actor_velocity = n2097183.result;
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
      n2097170 = matrix_of(jet_actor_);
      a = n2097170.Result;
      jp = engine_math_math::Mat4_translation(a).Result;
      here = struct_make<Game_Spot>(Vector{0.0, 0.0, 0.0}, 0.0);
      there = struct_make<Game_Spot>(Vector{0.0, 0.0, 0.0}, 0.0);
      n2097171 = clear_spot(here, jp, Vector{16.0, 20.0, 0.0}, 0.0, true);
      both = logic_not(n2097171.Result);
      if (both) {
        n2097173 = clear_spot(there, jp, Vector{16.0, 20.0, 0.0}, 260.0, true);
        both = n2097173.Result;
      }
      if (both) {
        n2097174 = matrix_of(soldier_);
        a_2 = n2097174.Result;
        me0 = engine_math_math::Mat4_translation(a_2).Result;
        rot = vector_make(0.0, 0.0, kern::formula([](Float y, Float y_2, Float x, Float x_2, Float kDeg) -> Float { return (kern::math::div(kern::math::atan2((y - y_2), (x - x_2)), kDeg) - 90.0); }, vector_break(me0).y, vector_break(there.p).y, vector_break(me0).x, vector_break(there.p).x, kDeg));
        engine_game_world_teleport(n2097175, world_, jet_actor_, vector_add(there.p, Vector{0.0, 0.0, 2.1}), list_make<Vector, "1">(rot));
        teleport = n2097175.result;
      }
    }
    n2097176 = matrix_of(soldier_);
    a_3 = n2097176.Result;
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
    for (auto it2097177 = flow_for(n2097177, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it2097177.next();) {
      r = items[n2097177.index];
      for (auto it2097178 = flow_for(n2097178, 0, 5); it2097178.next();) {
        n2097179 = city_stream_cpp::CityStream_street_point(city_, rng_, p, heading, me, r);
        if (logic_and(n2097179.Result, logic_not(city_stream_cpp::CityStream_building_at(city_, vector_make(vector_break(p).x, vector_break(p).y, 0.0)).Result))) {
          put_down(p, kern::formula([](Float heading, Float kDeg) -> Float { return kern::math::div(heading, kDeg); }, heading, kDeg));
          return;
        }
      }
    }
  }

  void hide_soldier() {
    Bool set_mode = false;
    engine_game_world_set_alive(world_, soldier_, false);
    engine_game_world_set_mode(n2097185, world_, game_SimMode::simulate);
    set_mode = n2097185.result;
  }

  void show_soldier(Vector feet = Vector{0.0, 0.0, 0.0}, Float yaw_deg = 0.0) {
    Bool actor_alive = false;
    Bool set_mode = false;
    Vector rot = Vector{0.0, 0.0, 0.0};
    actor_alive = engine_game_world_actor_alive(world_, soldier_);
    if (logic_not(actor_alive)) {
      engine_game_world_set_alive(world_, soldier_, true);
    }
    engine_game_world_set_mode(n2097186, world_, game_SimMode::play);
    set_mode = n2097186.result;
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
    n2097187 = matrix_of(soldier_);
    a = n2097187.Result;
    at = engine_math_math::Mat4_translation(a).Result;
    if (logic_and(math_compare<">">(now, spawn_guard_until_), logic_or(math_compare<">=">(spawn_guard_ready_at_, 0.0), terrain_cpp::Terrain_ground_ready(city_.terrain_, vector_break(at).x, vector_break(at).y).Result))) {
      spawn_guard_until_ = -1.0;
      return;
    }
    n2097188 = matrix_of(soldier_);
    a_2 = n2097188.Result;
    c = engine_math_math::Mat4_translation(a_2).Result;
    feet = vector_make(vector_break(c).x, vector_break(c).y, kern::formula([](Float z) -> Float { return (z - 0.73); }, vector_break(c).z));
    ready = terrain_cpp::Terrain_ground_ready(city_.terrain_, vector_break(feet).x, vector_break(feet).y).Result;
    n2097189 = standing_feet(feet);
    stand = n2097189.Result;
    if (logic_not(ready)) {
      hold = vector_add(stand, vector_make(0.0, 0.0, kSpawnLift));
      if (math_compare<">">(kern::formula([](Float z, Float z_2) -> Float { return kern::math::abs((z - z_2)); }, vector_break(feet).z, vector_break(hold).z), 0.05)) {
        rot = vector_make(0.0, 0.0, vector_break(spawn_guard_).z);
        engine_game_world_teleport(n2097190, world_, soldier_, vector_add(hold, Vector{0.0, 0.0, 0.73}), list_make<Vector, "1">(rot));
        teleport = n2097190.result;
      }
      engine_game_world_set_actor_velocity(n2097191, world_, soldier_, Vector{0.0, 0.0, 0.0});
      set_actor_velocity = n2097191.result;
      spawn_guard_ready_at_ = -1.0;
      return;
    }
    if (math_compare<"<">(spawn_guard_ready_at_, 0.0)) {
      spawn_guard_ready_at_ = now;
    }
    if (math_compare<"<">(vector_break(feet).z, kern::formula([](Float z) -> Float { return (z - 0.25); }, vector_break(stand).z))) {
      engine_game_world_teleport(n2097192, world_, soldier_, vector_add(stand, Vector{0.0, 0.0, 0.83}), List<Vector>{});
      teleport_2 = n2097192.result;
      engine_game_world_set_actor_velocity(n2097193, world_, soldier_, Vector{0.0, 0.0, 0.0});
      set_actor_velocity_2 = n2097193.result;
    }
    if (math_compare<">">(kern::formula([](Float now, Float spawn_guard_ready_at_) -> Float { return (now - spawn_guard_ready_at_); }, now, spawn_guard_ready_at_), 2.0)) {
      spawn_guard_until_ = -1.0;
    }
  }

  void eject() {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    n2097197 = state_of(jet_actor_);
    s = n2097197.Result;
    jet_.pilot = false;
    engine_game_world_set_alive(world_, chute_actor_, true);
    engine_game_world_teleport(n2097198, world_, chute_actor_, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 2.8}).Result), List<Vector>{});
    teleport = n2097198.result;
    engine_game_world_set_actor_velocity(n2097199, world_, chute_actor_, vector_add(s.velocity, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kEjectUp)).Result));
    set_actor_velocity = n2097199.result;
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
    n2097203 = common::Rng_next(rng_);
    start_2 = n2097203.Result;
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
    engine_game_world_physics(n2097204, world_);
    physics = n2097204.result;
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
    engine_game_world_physics(n2097205, world_);
    physics = n2097205.result;
    cast_sphere = engine_world_cast_sphere(physics, anchor, 0.17, d, engine_physics_physics::QueryFilter_new(Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}).Result, ignore);
    h = cast_sphere;
    return {logic_select<Vector>(h.hit, vector_add(anchor, vector_scale(d, kern::formula([](Float fraction) -> Float { return kern::math::max(0.0, (fraction - 0.02)); }, h.fraction))), want)};
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
    n3145746 = matrix_of(drone_actor_);
    a = n3145746.Result;
    n3145747 = combat_rules_cpp::Race_update(combat_.race, vector_sub(engine_math_math::Mat4_translation(a).Result, combat_.race_origin), now);
    passed = n3145747.Result;
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
    for (auto it3145748 = flow_for(n3145748, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Debris>(combat_.debris))))); it3145748.next();) {
      combat_.debris[n3145748.index].v = vector_make(vector_break(combat_.debris[n3145748.index].v).x, vector_break(combat_.debris[n3145748.index].v).y, kern::formula([](Float z, Float kG, Float dt) -> Float { return (z - (kG * dt)); }, vector_break(combat_.debris[n3145748.index].v).z, kG, dt));
      combat_.debris[n3145748.index].p = vector_add(combat_.debris[n3145748.index].p, vector_scale(combat_.debris[n3145748.index].v, dt));
      a = engine_math_math::Quat_axis_angle(logic_select<Vector>(math_compare<">">(vector_length(combat_.debris[n3145748.index].spin), 9.999999717180685e-10), vector_normalize(combat_.debris[n3145748.index].spin), Vector{0.0, 0.0, 1.0}), kern::formula([](Float a, Float dt) -> Float { return (a * dt); }, vector_length(combat_.debris[n3145748.index].spin), dt)).Result;
      combat_.debris[n3145748.index].q = common::qnormalize(engine_math_math::Quat_mul(combat_.debris[n3145748.index].q, a).Result).Result;
      the_floor = kern::formula([](Float the_result, Float z) -> Float { return ((the_result + 0.06) + (z * 0.5)); }, city_stream_cpp::CityStream_ground_height(city_, vector_break(combat_.debris[n3145748.index].p).x, vector_break(combat_.debris[n3145748.index].p).y).Result, vector_break(combat_.debris[n3145748.index].size).z);
      if (math_compare<"<">(vector_break(combat_.debris[n3145748.index].p).z, the_floor)) {
        combat_.debris[n3145748.index].p = vector_make(vector_break(combat_.debris[n3145748.index].p).x, vector_break(combat_.debris[n3145748.index].p).y, the_floor);
        if (math_compare<"<">(vector_break(combat_.debris[n3145748.index].v).z, 0.0)) {
          combat_.debris[n3145748.index].v = vector_make(vector_break(combat_.debris[n3145748.index].v).x, vector_break(combat_.debris[n3145748.index].v).y, kern::formula([](Float z) -> Float { return ((-z) * 0.3); }, vector_break(combat_.debris[n3145748.index].v).z));
        }
        combat_.debris[n3145748.index].v = vector_make(kern::formula([](Float x) -> Float { return (x * 0.76); }, vector_break(combat_.debris[n3145748.index].v).x), vector_break(combat_.debris[n3145748.index].v).y, vector_break(combat_.debris[n3145748.index].v).z);
        combat_.debris[n3145748.index].v = vector_make(vector_break(combat_.debris[n3145748.index].v).x, kern::formula([](Float y) -> Float { return (y * 0.76); }, vector_break(combat_.debris[n3145748.index].v).y), vector_break(combat_.debris[n3145748.index].v).z);
        combat_.debris[n3145748.index].spin = vector_scale(combat_.debris[n3145748.index].spin, 0.6);
      }
    }
    at = 0;
    kept = at;
    end = list_length<Debris>(combat_.debris);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      if (logic_not(update_debris_match(combat_.debris[at], now).Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n3145749, combat_.debris[at]);
          combat_.debris[kept] = n3145749.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(combat_.debris, kept, Debris{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 6.5});
    if (math_compare<">">(kern::formula([](Float a) -> Float { return ((63.0 < 0.0) != (a < 0.0) ? (a < 0.0) : (63.0 < a)); }, kern::to<Float>(list_length<Debris>(combat_.debris))), 0.5)) {
      first = 0;
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 63.0); }, kern::to<Float>(list_length<Debris>(combat_.debris))))))));
      for (auto it3145750 = flow_for(n3145750, first, last); it3145750.next();) {
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
    for (auto it3145754 = flow_for(n3145754, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Debris>(combat_.debris))))); it3145754.next();) {
      fade = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a, Float a_2, Float now) -> Float { return kern::math::div(((a + a_2) - now), 1.4); }, combat_.debris[n3145754.index].born, combat_.debris[n3145754.index].life, now), 0.0, 1.0));
      a = engine_math_math::Mat4_trs(combat_.debris[n3145754.index].p, combat_.debris[n3145754.index].q, vector_scale(combat_.debris[n3145754.index].size, kern::formula([](Float fade) -> Float { return (0.3 + (0.7 * fade)); }, fade))).Result;
      combat_art_cpp::CombatArt_put(combat_art_, a, CombatArt_Shape::box, combat_.debris[n3145754.index].color);
    }
    for (auto it3145755 = flow_for(n3145755, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<EnemyRocket>(combat_.rockets))))); it3145755.next();) {
      a_3 = engine_math_math::Mat4_translate(combat_.rockets[n3145755.index].p).Result;
      a_4 = engine_math_math::Mat4_scale(Vector{0.44, 0.44, 0.44}).Result;
      a_2 = engine_math_math::Mat4_mul(a_3, a_4).Result;
      combat_art_cpp::CombatArt_put(combat_art_, a_2, CombatArt_Shape::glow_sphere, Vector{3.0, 1.6, 0.5});
    }
    for (auto it3145756 = flow_for(n3145756, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<EnemyAi>(combat_.enemies))))); it3145756.next();) {
      skip = false;
      if (math_compare<"<">(kern::to<Float>(combat_.enemies[n3145756.index].person), 0.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        s = enemy_spec(combat_.enemies[n3145756.index].type).Result;
        a_6 = engine_math_math::Mat4_translate(vector_add(people_.people_[combat_.enemies[n3145756.index].person].pos, Vector{0.0, 0.0, 1.4})).Result;
        a_7 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, common::yaw_quat(people_.people_[combat_.enemies[n3145756.index].person].yaw).Result, Vector{0.27, 0.45, 0.06}).Result;
        a_5 = engine_math_math::Mat4_mul(a_6, a_7).Result;
        combat_art_cpp::CombatArt_put(combat_art_, a_5, CombatArt_Shape::glow_box, vector_scale(s.stripe, 1.6));
        if (enum_equal<combat_EnemyType>(combat_.enemies[n3145756.index].type, combat_EnemyType::heavy)) {
          a_9 = engine_math_math::Mat4_translate(vector_add(people_.people_[combat_.enemies[n3145756.index].person].pos, vector_make(kern::formula([](Float a) -> Float { return (kern::math::cos(a) * 0.1); }, people_.people_[combat_.enemies[n3145756.index].person].yaw), kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.1); }, people_.people_[combat_.enemies[n3145756.index].person].yaw), 1.62))).Result;
          a_11 = common::yaw_quat(people_.people_[combat_.enemies[n3145756.index].person].yaw).Result;
          a_12 = engine_math_math::Quat_axis_angle(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
          a_10 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_mul(a_11, a_12).Result, Vector{0.26, 0.26, 1.1}).Result;
          a_8 = engine_math_math::Mat4_mul(a_9, a_10).Result;
          combat_art_cpp::CombatArt_put(combat_art_, a_8, CombatArt_Shape::cylinder, Vector{0.1, 0.1, 0.13});
        }
      }
    }
    for (auto it3145757 = flow_for(n3145757, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Tank>(combat_.tanks))))); it3145757.next();) {
      if (logic_and(combat_.tanks[n3145757.index].wreck, math_compare<"<">(kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, combat_.tanks[n3145757.index].wreck_at), 40.0))) {
        for (auto it3145758 = flow_for(n3145758, 0, 2); it3145758.next();) {
          t = kern::formula([](Float now, Float index) -> Float { return ((now * (3.1 + index)) + (index * 2.1)); }, now, kern::to<Float>(n3145758.index));
          s_2 = kern::formula([](Float t) -> Float { return (1.4 * (0.55 + (0.25 * kern::math::sin(t)))); }, t);
          a_14 = engine_math_math::Mat4_translate(vector_add(combat_.tanks[n3145757.index].p, vector_make(kern::formula([](Float t) -> Float { return (0.3 * kern::math::sin(t)); }, t), kern::formula([](Float t) -> Float { return (0.3 * kern::math::cos(t)); }, t), kern::formula([](Float s_2, Float index) -> Float { return ((2.0 + (s_2 * 0.6)) + (index * 0.5)); }, s_2, kern::to<Float>(n3145758.index))))).Result;
          a_15 = engine_math_math::Mat4_scale(vector_make(s_2, s_2, kern::formula([](Float s_2) -> Float { return (s_2 * 1.4); }, s_2))).Result;
          a_13 = engine_math_math::Mat4_mul(a_14, a_15).Result;
          combat_art_cpp::CombatArt_put(combat_art_, a_13, CombatArt_Shape::glow_sphere, Vector{2.8, 1.1, 0.25});
        }
      }
    }
    if (math_compare<">">(night_, 0.25)) {
      k = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float night_) -> Float { return kern::math::div((night_ - 0.25), 0.5); }, night_), 0.0, 1.0));
      for (auto it3145759 = flow_for(n3145759, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it3145759.next();) {
        skip_2 = false;
        either = logic_or(logic_not(cars_[n3145759.index].placed), cars_[n3145759.index].wreck);
        if (logic_not(either)) {
          actor_alive_2 = engine_game_world_actor_alive(world_, cars_[n3145759.index].actor);
          either = logic_not(actor_alive_2);
        }
        if (either) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          if (logic_and(logic_not(cars_[n3145759.index].driver), logic_not(logic_and(enum_equal<Mode>(mode_, Mode::car), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n3145759.index))))), kern::to<Float>(car_index_)))))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            n3145760 = matrix_of(cars_[n3145759.index].actor);
            m = n3145760.Result;
            pos = engine_math_math::Mat4_translation(m).Result;
            if (math_compare<">">(vector_length(vector_sub(pos, cam_.eye)), 160.0)) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              fwd = logic_select<Vector>(math_compare<">">(vector_length(vector_make(m.m[0], m.m[1], 0.0)), 9.999999717180685e-10), vector_normalize(vector_make(m.m[0], m.m[1], 0.0)), Vector{1.0, 0.0, 0.0});
              side = vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(fwd).y), vector_break(fwd).x, 0.0);
              h = cars_[n3145759.index].car[0].s_.half;
              item = -1.0;
              item_2 = 1.0;
              items = list_make<Float, "2">(item, item_2);
              for (auto it3145761 = flow_for(n3145761, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it3145761.next();) {
                sd = items[n3145761.index];
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
    for (auto it3145762 = flow_for(n3145762, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TrafficCar>(cars_))))); it3145762.next();) {
      skip_3 = false;
      if (logic_or(logic_not(cars_[n3145762.index].placed), logic_not(cars_[n3145762.index].wreck))) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        age = kern::formula([](Float now, Float a) -> Float { return (now - a); }, now, cars_[n3145762.index].wreck_at);
        n3145763 = matrix_of(cars_[n3145762.index].actor);
        a_22 = n3145763.Result;
        at = engine_math_math::Mat4_translation(a_22).Result;
        for (auto it3145764 = flow_for(n3145764, 0, 2); it3145764.next();) {
          t_2 = kern::formula([](Float now, Float index) -> Float { return ((now * (2.7 + index)) + (index * 2.3)); }, now, kern::to<Float>(n3145764.index));
          if (math_compare<"<">(age, 9.0)) {
            s_3 = kern::formula([](Float t_2, Float a) -> Float { return ((1.1 * (0.55 + (0.25 * kern::math::sin(t_2)))) * a); }, t_2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float age) -> Float { return (1.4 - kern::math::div(age, 9.0)); }, age), 0.3, 1.0));
            a_24 = engine_math_math::Mat4_translate(vector_add(at, vector_make(kern::formula([](Float t_2) -> Float { return (0.35 * kern::math::sin(t_2)); }, t_2), kern::formula([](Float t_2) -> Float { return (0.35 * kern::math::cos(t_2)); }, t_2), kern::formula([](Float s_3, Float index) -> Float { return ((1.1 + (s_3 * 0.5)) + (index * 0.4)); }, s_3, kern::to<Float>(n3145764.index))))).Result;
            a_25 = engine_math_math::Mat4_scale(vector_make(s_3, s_3, kern::formula([](Float s_3) -> Float { return (s_3 * 1.5); }, s_3))).Result;
            a_23 = engine_math_math::Mat4_mul(a_24, a_25).Result;
            combat_art_cpp::CombatArt_put(combat_art_, a_23, CombatArt_Shape::glow_sphere, Vector{2.8, 1.05, 0.22});
          }
          rise = kern::formula([](Float now, Float index) -> Float { return kern::math::mod(((now * 0.9) + (index * 1.3)), 4.0); }, now, kern::to<Float>(n3145764.index));
          s_4 = kern::formula([](Float rise) -> Float { return (0.7 + (rise * 0.45)); }, rise);
          a_27 = engine_math_math::Mat4_translate(vector_add(at, vector_make(kern::formula([](Float t_2) -> Float { return (0.4 * kern::math::sin((t_2 * 0.3))); }, t_2), kern::formula([](Float t_2) -> Float { return (0.4 * kern::math::cos((t_2 * 0.3))); }, t_2), kern::formula([](Float rise) -> Float { return (1.6 + (rise * 1.6)); }, rise)))).Result;
          a_28 = engine_math_math::Mat4_scale(vector_make(s_4, s_4, s_4)).Result;
          a_26 = engine_math_math::Mat4_mul(a_27, a_28).Result;
          combat_art_cpp::CombatArt_put_2(combat_art_, a_26, CombatArt_Shape::fade_sphere, Vector{0.05, 0.05, 0.055}, kern::formula([](Float rise) -> Float { return (0.55 * (1.0 - kern::math::div(rise, 4.0))); }, rise));
        }
      }
    }
    if (combat_.race_on) {
      for (auto it3145765 = flow_for(n3145765, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(combat_kGates)))); it3145765.next();) {
        g_2 = gate(n3145765.index).Result;
        n = Vector{0.0, 0.0, 0.0};
        r = Vector{0.0, 0.0, 0.0};
        combat_rules_cpp::gate_axes(n, r, n3145765.index);
        col = logic_select<Vector>(g_2.start, Vector{1.0, 0.78, 0.34}, Vector{0.95, 0.96, 0.97});
        if (logic_and(logic_and(logic_and(combat_.race.started, logic_not(combat_.race.finished)), math_compare<"<">(kern::to<Float>(n3145765.index), kern::to<Float>(combat_.race.next))), math_compare<"!=">(kern::to<Float>(combat_.race.next), 0.0))) {
          col = Vector{0.35, 0.72, 1.0};
        }
        if (logic_and(logic_not(combat_.race.finished), math_compare<"==">(kern::to<Float>(n3145765.index), kern::to<Float>(combat_.race.next)))) {
          col = vector_scale(Vector{0.43, 0.96, 0.66}, 2.2);
        }
        combat_art_cpp::CombatArt_gate(combat_art_, vector_add(combat_.race_origin, g_2.center), kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(r).y, vector_break(r).x), g_2.width, g_2.height, col);
      }
    }
  }

  void combat_hud(Text& s) {
    Float now = 0.0;
    Float sim_time = 0.0;
    Int soldiers = 0;
    Int fighters = 0;
    Int flak_alive = 0;
    combat_Contract k = combat_Contract{"", "", "", 0, 0};
    Text b = "";
    Text arg = "";
    Int arg_2 = 0;
    Int arg_3 = 0;
    Int arg_4 = 0;
    Int arg_5 = 0;
    Int arg_6 = 0;
    Int arg_7 = 0;
    Int arg_8 = 0;
    Int arg_9 = 0;
    Int arg_10 = 0;
    Float arg_11 = 0.0;
    Int arg_12 = 0;
    Int arg_13 = 0;
    Int arg_14 = 0;
    Text arg_15 = "";
    Int arg_16 = 0;
    Int arg_17 = 0;
    Int arg_18 = 0;
    Int arg_19 = 0;
    Int arg_20 = 0;
    Float arg_21 = 0.0;
    Int arg_22 = 0;
    Int arg_23 = 0;
    Int arg_24 = 0;
    Int arg_25 = 0;
    Float cur = 0.0;
    sim_time = engine_game_world_sim_time(world_);
    now = sim_time;
    soldiers = 0;
    fighters = 0;
    for (auto it3145766 = flow_for(n3145766, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<SoldierAi>(combat_.soldiers))))); it3145766.next();) {
      soldiers += kern::to<Int>(math_compare<">=">(kern::to<Float>(combat_.soldiers[n3145766.index].person), 0.0));
    }
    for (auto it3145767 = flow_for(n3145767, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<People_Person>(people_.people_))))); it3145767.next();) {
      fighters += kern::to<Int>(enum_equal<People_State>(people_.people_[n3145767.index].state, People_State::fight));
    }
    flak_alive = 0;
    for (auto it3145768 = flow_for(n3145768, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<FlakGun>(combat_.flak))))); it3145768.next();) {
      flak_alive = kern::to<Int>(kern::formula([](Float flak_alive, Float a) -> Float { return kern::math::wrap32(flak_alive + (!a)); }, kern::to<Float>(flak_alive), kern::to<Float>(combat_.flak[n3145768.index].dead)));
    }
    k = contract(combat_.fun.contract_index).Result;
    b = "";
    arg = logic_select<Text>(combat_.military.active, "on", logic_select<Text>(math_compare<">=">(combat_.military.alert_at, 0.0), "alert", "off"));
    arg_2 = list_length<Heli>(combat_.helis);
    arg_3 = list_length<Tank>(combat_.tanks);
    arg_4 = soldiers;
    arg_5 = flak_alive;
    arg_6 = combat_.flak_destroyed;
    arg_7 = combat_.flak_hits;
    arg_8 = list_length<FlakRound>(combat_.flak_rounds);
    arg_9 = list_length<FighterJet>(combat_.jets);
    arg_10 = list_length<Bomb>(combat_.bombs);
    arg_11 = kern::formula([](Float combat_kStrikeCooldownS, Float now, Float last_strike) -> Float { return kern::math::max(0.0, (combat_kStrikeCooldownS - (now - last_strike))); }, combat_kStrikeCooldownS, now, combat_.last_strike);
    arg_12 = fighters;
    arg_13 = combat_.gangster_shots;
    arg_14 = list_length<EnemyAi>(combat_.enemies);
    arg_15 = logic_select<Text>(combat_.zombies.active, "true", "false");
    arg_16 = combat_.zombies.the_round;
    arg_17 = list_length<ZombieAi>(combat_.zombie_ai);
    arg_18 = combat_.zombies.points;
    arg_19 = combat_.chaos.score;
    arg_20 = combat_.chaos.combo;
    arg_21 = combat_.chaos.flow;
    arg_22 = combat_.fun.score;
    arg_23 = combat_.fun.at_risk;
    arg_24 = combat_.fun.high_score;
    arg_25 = combat_.fun.combo;
    n3145769 = combat_rules_cpp::Fun_multiplier(combat_.fun, now);
    b = text_format<",\"combat\":{\"mil\":\"%s\",\"helis\":%zu,\"tanks\":%zu,\"soldiers\":%d,\"flak\":%u,\"flakDestroyed\":%u,\"flakHits\":%u,\"flakRounds\":%zu,\"jets\":%zu,\"bombs\":%zu,\"strike\":%.1f,\"fighters\":%d,\"gangsterShots\":%u,\"enemies\":%zu,\"zombies\":{\"night\":%s,\"round\":%d,\"alive\":%zu,\"points\":%d},\"chaos\":{\"score\":%d,\"combo\":%d,\"flow\":%.2f},\"fun\":{\"score\":%d,\"risk\":%d,\"high\":%d,\"combo\":%d,\"mult\":%.2f,\"contract\":\"%s\",\"hint\":\"%s\",\"progress\":%d,\"target\":%d,\"done\":%s},\"scope\":%s,\"debris\":%zu,\"drone\":%.1f">(767, arg, arg_2, arg_3, arg_4, arg_5, arg_6, arg_7, arg_8, arg_9, arg_10, arg_11, arg_12, arg_13, arg_14, arg_15, arg_16, arg_17, arg_18, arg_19, arg_20, arg_21, arg_22, arg_23, arg_24, arg_25, n3145769.Result, k.name, k.hint, combat_.fun.contract_progress, k.target, logic_select<Text>(combat_.fun.contract_complete, "true", "false"), logic_select<Text>(combat_.scope, "true", "false"), list_length<Debris>(combat_.debris), kern::formula([](Float drone_ready_at, Float now) -> Float { return kern::math::max(0.0, (drone_ready_at - now)); }, combat_.drone_ready_at, now));
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

  struct update_flak_matchResult { Bool Result = false; };
  KERN_PURE update_flak_matchResult update_flak_match(const FlakGun& g, Int64 cx = Int64{0x0}, Int64 dx = Int64{0x0}, Int64 cy = Int64{0x0}, Int64 dy = Int64{0x0}) {
    return {logic_and(int64_equal(g.cx, int64_add(cx, dx)), int64_equal(g.cy, int64_add(cy, dy)))};
  }

  void launch_jets(Float cx = 0.0, Float cy = 0.0, Bool strike = false, Float at = 0.0, Float alt = 0.0, Float offset = 0.0, Bool remote = false) {
    Float z = 0.0;
    FighterJet j = FighterJet{0.0, 0.0, 0.0, 0.0, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false};
    z = kern::formula([](Float the_result, Float alt) -> Float { return (the_result + alt); }, city_stream_cpp::CityStream_ground_height(city_, cx, cy).Result, alt);
    for (auto it4194334 = flow_for(n4194334, 0, 1); it4194334.next();) {
      j = FighterJet{0.0, 0.0, 0.0, 0.0, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false};
      j.x = kern::formula([](Float cx, Float offset, Float index) -> Float { return ((cx + offset) - (index * 26.0)); }, cx, offset, kern::to<Float>(n4194334.index));
      j.y0 = kern::formula([](Float cy, Float combat_kJetSpan, Float index) -> Float { return ((cy - combat_kJetSpan) - (index * 42.0)); }, cy, combat_kJetSpan, kern::to<Float>(n4194334.index));
      j.z = kern::formula([](Float z, Float index) -> Float { return (z + (index * 6.0)); }, z, kern::to<Float>(n4194334.index));
      j.t0 = at;
      j.strike = logic_and(strike, math_compare<"==">(kern::to<Float>(n4194334.index), 0.0));
      j.target = vector_make(cx, cy, 0.0);
      j.remote = remote;
      list_add(combat_.jets, j);
    }
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
  struct { Int result = 0; } n2097168;
  struct : kern::Node<2097169> { Int index = 0; } n2097169;
  matrix_ofResult n2097170;
  clear_spotResult n2097171;
  clear_spotResult n2097173;
  matrix_ofResult n2097174;
  struct { Bool result = false; } n2097175;
  matrix_ofResult n2097176;
  struct : kern::Node<2097177> { Int index = 0; } n2097177;
  struct : kern::Node<2097178> { Int index = 0; } n2097178;
  city_stream_cpp::CityStream_street_pointResult n2097179;
  struct { Int result = 0; } n2097180;
  standing_feetResult n2097181;
  struct { Bool result = false; } n2097182;
  struct { Bool result = false; } n2097183;
  struct { Bool result = false; } n2097185;
  struct { Bool result = false; } n2097186;
  matrix_ofResult n2097187;
  matrix_ofResult n2097188;
  standing_feetResult n2097189;
  struct { Bool result = false; } n2097190;
  struct { Bool result = false; } n2097191;
  struct { Bool result = false; } n2097192;
  struct { Bool result = false; } n2097193;
  state_ofResult n2097197;
  struct { Bool result = false; } n2097198;
  struct { Bool result = false; } n2097199;
  common::Rng_nextResult n2097203;
  struct { Int result = 0; } n2097204;
  struct { Int result = 0; } n2097205;
  matrix_ofResult n3145746;
  combat_rules_cpp::Race_updateResult n3145747;
  struct : kern::Node<3145748> { Int index = 0; } n3145748;
  struct { Debris value = Debris{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, 0.0, 6.5}; } n3145749;
  struct : kern::Node<3145750> { Int index = 0; } n3145750;
  struct : kern::Node<3145754> { Int index = 0; } n3145754;
  struct : kern::Node<3145755> { Int index = 0; } n3145755;
  struct : kern::Node<3145756> { Int index = 0; } n3145756;
  struct : kern::Node<3145757> { Int index = 0; } n3145757;
  struct : kern::Node<3145758> { Int index = 0; } n3145758;
  struct : kern::Node<3145759> { Int index = 0; } n3145759;
  matrix_ofResult n3145760;
  struct : kern::Node<3145761> { Int index = 0; } n3145761;
  struct : kern::Node<3145762> { Int index = 0; } n3145762;
  matrix_ofResult n3145763;
  struct : kern::Node<3145764> { Int index = 0; } n3145764;
  struct : kern::Node<3145765> { Int index = 0; } n3145765;
  struct : kern::Node<3145766> { Int index = 0; } n3145766;
  struct : kern::Node<3145767> { Int index = 0; } n3145767;
  struct : kern::Node<3145768> { Int index = 0; } n3145768;
  combat_rules_cpp::Fun_multiplierResult n3145769;
  matrix_ofResult n4194314;
  struct : kern::Node<4194334> { Int index = 0; } n4194334;
};
