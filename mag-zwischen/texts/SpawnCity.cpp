struct SpawnCity : virtual kern::Blueprint, virtual blueprint_call, virtual city_stream, virtual city_stream_cpp, virtual common, virtual controls, virtual controls_cpp, virtual drone, virtual drone_art_cpp, virtual drone_cpp, virtual engine_app_app, virtual engine_game_level, virtual engine_game_runtime, virtual engine_physics_physics, virtual settings, virtual settings_cpp, virtual sound_fx_cpp, virtual spiel_cpp, virtual terrain_cpp, virtual tiles_cpp, virtual touch_play, virtual touch_play_cpp, kern::Implements<kern::App> {
  // ---- variables ----
  PadState pad = PadState{false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, false};
  Object<kern::Engine> engine_ = Object<kern::Engine>{};
  Object<kern::game::GameWorld> world_ = Object<kern::game::GameWorld>{};
  u32 drone_actor_ = 0;
  CityStream city_ = CityStream{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 2600, 1, List<Vector>{}, 1, false, 0, 0, List<Vec4>{}, CityStream_NearCollision{physics_Body{0}, physics_Body{0}, Vector{1e+09, 1e+09, 0.0}, Vector{1e+09, 1e+09, 0.0}, 1000000, List<u64_u32_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0}, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, 0.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{1e+09, 1e+09, 0.0}, Vector{1e+09, 1e+09, 0.0}, -1000000000.0, Map<u32, CityStream_Damage>{}, List<u32>{}, false, Object<kern::Renderer>{}, Object<kern::physics::World>{}, GeoProjector{0.0, 0.0, 1.0}, TileKey{14, 0, 0}, Map<u64, List<CityStream_Tile>>{}, List<TileKey>{}, List<Lane>{}, false, 0, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<u64, Terrain_Image>{}, List<TileKey>{}, Vector{1e+09, 1e+09, 0.0}, Object<kern::Renderer>{}, Object<kern::physics::World>{}, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0f, false, Terrain_Grid{0.0f, 0.0f, 10.0f, 0, List<f32>{}, 0.0f, 0.0f}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<u64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<u64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<u64, Terrain_RenderTile>{}, Map<u64, Terrain_CollisionTile>{}, Map<u64, u32>{}, Map<u64, bool>{}, Set<u64>{}, Set<u64>{}, Set<u64>{}, List<u64>{}, List<u64>{}, List<u64>{}, Vector{1e+09, 1e+09, 0.0}, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, false, false}}, List<Vec4>{}, 0, -1};
  Drone drone_ = Drone{DroneParams{0.225f, 0.127f, 2300.0f, 0.08f, 1.1e-05f, 0.105f, 0.012f, 45.0f, 45.0f, 4.0f, 1.5f, 0.055f, 180.0f, 1.225f, 0.35f, 0.18f, Vector{0.0, 0.0, 0.0}, 0.18f, 0.035f, 0.005f, 0.2f, 0.03f, 0.02f, 0.03f, 0.1f, 0.015f, -0.015f, -0.07f, 0.028f}, fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0f, false, false, false, false, false, false, false, 0.0f, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, false}, fc_RC{List<u16>{}, false, false, false, 0}, fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false}, 0, 0, false}, List<Vector>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 1.0f, 16.8f, 0.0f, 0.0f, 0.0f, 0.0f, Vector{0.0, 0.0, 0.0}, List<u16>{1000, 1000, 1000, 1000}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0.0, 1, 0.01f, 0.01f, Vector{0.0, 0.0, 0.0}, 1, true, Rng{1168318247, 0.0f, false}, Rng{1905503009, 0.0f, false}, Rng{1168232869, 0.0f, false}, List<f32>{0.08f, -0.05f, 0.04f}, List<f32>{0.001f, -0.0015f, 0.002f}, 0, false, DroneTelemetry{false, true, false, false, false, 0, List<u16>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, List<f32>{}}};
  DroneSticks sticks_ = DroneSticks{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.2f, false, true, false};
  DroneArt art_ = DroneArt{Object<kern::Renderer>{}, DroneArt_Part{0, List<f32>{}, List<f32>{}, false}, DroneArt_Part{0, List<f32>{}, List<f32>{}, false}, DroneArt_Part{0, List<f32>{}, List<f32>{}, false}, DroneArt_Part{0, List<f32>{}, List<f32>{}, false}, DroneArt_Part{0, List<f32>{}, List<f32>{}, false}};
  SettingsTable settings_table_ = SettingsTable{List<Option>{}, List<OptSpec>{}};
  Settings settings_ = Settings{List<f32>{}, 1};
  /// the version of the settings applied
  u32 applied_ = 0;
  TouchPlay touch_ = TouchPlay{960.0f, 540.0f, 0.0f, 0.0f, 0.0f, 0.0f, TouchLayout{TouchMode::off, true, "", false, false, false, 0, -1.0f, false, false, false, true, false, false, "FPV", false, false, false, false, false, false}, List<TouchBtn>{}, List<TouchPlay_Finger>{}, List<Text>{}, List<Text>{}, List<TouchFire>{}, -1, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f, 1.0f, 0.0f, false, List<TouchPlay_DomRect>{}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
  /// a finger was last (the touch controls show)
  bool touch_mode_ = false;
  /// the mouse as the right stick (desktop_controls.mjs: 1500 px/s, 50 ms)
  controls_RateStick mouse_ = controls_RateStick{0.0f, 0.0f, Vector{0.0, 0.0, 0.0}};
  /// mouse movement since the last frame (CSS px)
  Vector mouse_px_ = Vector{0.0, 0.0, 0.0};
  /// the pointer lock
  bool locked_ = false;
  /// the pad's buttons of the frame before
  u32 pad_was_ = 0;
  f64 lon_ = 0.0;
  f64 lat_ = 0.0;
  bool mobile_ = false;
  bool placed_ = false;
  bool paused_ = false;
  bool landing_ = false;
  bool height_held_ = false;
  f32 landing_still_ = 0.0f;
  /// the time the fixed steps have not taken yet (s)
  f64 accumulator_ = 0.0;
  u64 steps_ = 0;
  /// 0 FPV, 1 follow, 2 third (simulator.mjs camFpv / camFollow / camThird)
  i32 cam_mode_ = 0;
  Camera cam_ = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471976f, 0.05f, 1800.0f, false, 20.0f, false, Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}};
  Vector cam_anchor_ = Vector{0.0, 0.0, 0.0};
  Vector cam_vel_ = Vector{0.0, 0.0, 0.0};
  Vector cam_heading_ = Vector{1.0, 0.0, 0.0};
  bool rig_init_ = false;
  Vector focus_prev_ = Vector{0.0, 0.0, 0.0};
  bool focus_prev_valid_ = false;
  f32 fps_ = 60.0f;
  f32 time_ = 0.0f;
  Text toast_ = "";
  f32 toast_age_ = 99.0f;
  u32 resets_ = 0;
  u32 moves_ = 0;
  List<Vec4> ground_changes_ = List<Vec4>{};
  /// sound: Kern's mixer (?audio=kern) or the original's on the page (audio web 1: the page asks for "audio frame")
  Object<kern::audio::Mixer> mixer_ = Object<kern::audio::Mixer>{};
  Object<kern::game::SoundBank> bank_ = Object<kern::game::SoundBank>{};
  Object<kern::game::GameAudio> game_audio_ = Object<kern::game::GameAudio>{};
  SoundFx fx_ = SoundFx{Object<kern::audio::Mixer>{}, SoundFx_Loop{0, 0, 0.0f}, SoundFx_Loop{0, 0, 0.0f}, SoundFx_Loop{0, 0, 0.0f}, SoundFx_Loop{0, 0, 0.0f}, SoundFx_Loop{0, 0, 0.0f}, SoundFx_Loop{0, 0, 0.0f}, SoundFx_Loop{0, 0, 0.0f}};
  List<f32> audio_out_ = List<f32>{};
  bool web_audio_ = false;
  /// (the actor of a world whose Blueprint the program calls: Controls)
  Actor controls_g_rules = Actor{};
  /// (the actor of a world whose Blueprint the program calls: Settings)
  Actor settings_g_rules = Actor{};
  KERN_CONST u32 kCount = 60;
  /// Default start: Konstanz, Marktstaette (old town by the harbour).
  KERN_CONST f64 kKonstanzLat = 47.66;
  /// Default start: Konstanz, Marktstaette (old town by the harbour).
  KERN_CONST f64 kKonstanzLon = 9.1766;
  KERN_CONST f32 kPi = 3.1415927f;
  /// fixed physics rate: the firmware runs 4 ticks per step (~1 kHz); the level's world.stepHz (tests/test_entry.cpp holds it)
  KERN_CONST f64 kStepHz = 240.0;
  /// at most 0.1 s of simulation per frame (GameWorld::step's guard against the spiral of death)
  KERN_CONST u32 kMaxSteps = 24;
  KERN_CONST f32 kDeg = 0.017453292f;
  /// thumb_layout.mjs STICK
  KERN_CONST f32 kStick = 168.0f;
  /// control_semantics.mjs normalizedPointer
  KERN_CONST f32 kRadiusK = 0.42f;
  /// foot_touch_controller.mjs edge turn
  KERN_CONST f32 kEdgeStart = 0.92f;
  /// foot_touch_controller.mjs edge turn
  KERN_CONST f32 kEdgeRampPx = 42.0f;

  // ---- functions ----
  struct Controls_DeviceInputResult { bool desktop = false; bool pad = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("DeviceInput") Controls_DeviceInputResult Controls_DeviceInput(controls_Devices_Event event = controls_Devices_Event::touch_down, bool fine = false, bool coarse = false, bool desktop = false, bool pad_2 = false);

  struct Controls_Smoothstep01Result { f64 value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("Smoothstep01") Controls_Smoothstep01Result Controls_Smoothstep01(f64 t = 0.0);

  struct Controls_WalkPadAxisResult { f64 value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("WalkPadAxis") Controls_WalkPadAxisResult Controls_WalkPadAxis(f64 v = 0.0);

  struct Controls_ShapeFirstPersonMoveResult { f64 x = 0.0; f64 y = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("ShapeFirstPersonMove") Controls_ShapeFirstPersonMoveResult Controls_ShapeFirstPersonMove(f64 x = 0.0, f64 y = 0.0, f64 fineness = 0.0, bool invertH = false);

  struct Controls_FpsDynamicCurveResult { f64 value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("FpsDynamicCurve") Controls_FpsDynamicCurveResult Controls_FpsDynamicCurve(f64 n = 0.0);

  struct Controls_LookCurveStrengthResult { f64 strength = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookCurveStrength") Controls_LookCurveStrengthResult Controls_LookCurveStrength(f64 fineness = 0.0);

  struct Controls_ShapeFpsStickResult { f64 x = 0.0; f64 y = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("ShapeFpsStick") Controls_ShapeFpsStickResult Controls_ShapeFpsStick(f64 x = 0.0, f64 y = 0.0, f64 inner = 0.0, f64 curve = 0.0);

  struct Controls_LookAxisStepResult { f64 velocity = 0.0; f64 angle = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookAxisStep") Controls_LookAxisStepResult Controls_LookAxisStep(f64 velocity = 0.0, f64 target = 0.0, f64 dt = 0.0);

  struct Controls_TouchLookDeltaResult { f64 yaw = 0.0; f64 pitch = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("TouchLookDelta") Controls_TouchLookDeltaResult Controls_TouchLookDelta(f64 dx = 0.0, f64 dy = 0.0, f64 speed = 0.0);

  struct Controls_FpsAimAssistResult { bool active = false; f64 slowdown = 0.0; f64 yaw = 0.0; f64 pitch = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("FpsAimAssist") Controls_FpsAimAssistResult Controls_FpsAimAssist(f64 yawErr = 0.0, f64 pitchErr = 0.0, f64 dist = 0.0, f64 input = 0.0, f64 inYaw = 0.0, f64 inPitch = 0.0, f64 s = 0.0);

  struct Controls_AimEllipseResult { f64 value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("AimEllipse") Controls_AimEllipseResult Controls_AimEllipse(f64 yawErr = 0.0, f64 pitchErr = 0.0);

  struct Controls_GamepadAxisResult { f64 value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GamepadAxis") Controls_GamepadAxisResult Controls_GamepadAxis(f64 v = 0.0, f64 dz = 0.0);

  struct Controls_HardCutResult { f64 value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("HardCut") Controls_HardCutResult Controls_HardCut(f64 v = 0.0, f64 dz = 0.0);

  struct Controls_HeightAxisResult { f64 value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("HeightAxis") Controls_HeightAxisResult Controls_HeightAxis(f64 rt = 0.0, f64 lt = 0.0);

  struct Controls_PhoneAxisResult { f64 value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("PhoneAxis") Controls_PhoneAxisResult Controls_PhoneAxis(f64 v = 0.0, f64 fineness = 0.0);

  struct Controls_GameStickMagnitudeResult { f64 value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GameStickMagnitude") Controls_GameStickMagnitudeResult Controls_GameStickMagnitude(f64 a = 0.0);

  struct Controls_InverseGameStickMagnitudeResult { f64 value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("InverseGameStickMagnitude") Controls_InverseGameStickMagnitudeResult Controls_InverseGameStickMagnitude(f64 v = 0.0);

  struct Controls_GameStickLeftResult { f64 roll = 0.0; f64 pitch = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GameStickLeft") Controls_GameStickLeftResult Controls_GameStickLeft(f64 x = 0.0, f64 y = 0.0, f64 fineness = 0.0, bool invertH = false, f64 maxKmh = 0.0);

  struct Controls_GameStickRightResult { f64 yaw = 0.0; f64 bodyPitch = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GameStickRight") Controls_GameStickRightResult Controls_GameStickRight(f64 x = 0.0, f64 y = 0.0, f64 fineness = 0.0, bool invertH = false, bool invertV = false);

  struct Controls_ClearancePadAxisResult { f64 value = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("ClearancePadAxis") Controls_ClearancePadAxisResult Controls_ClearancePadAxis(f64 centreY = 0.0, f64 y = 0.0, f64 height = 0.0);

  struct Controls_RateStickStepResult { f64 rate = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("RateStickStep") Controls_RateStickStepResult Controls_RateStickStep(f64 rate = 0.0, f64 px = 0.0, f64 dt = 0.0, f64 fullPxS = 0.0, f64 tau = 0.0);

  struct Controls_LookAddResult { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookAdd") Controls_LookAddResult Controls_LookAdd(controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}, f64 dx = 0.0, f64 dy = 0.0, f64 degPerPxX = 0.0, f64 degPerPxY = 0.0);

  struct Controls_LookSetResult { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookSet") Controls_LookSetResult Controls_LookSet(f64 yaw = 0.0, f64 pitch = 0.0);

  struct Controls_LookReleaseResult { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookRelease") Controls_LookReleaseResult Controls_LookRelease(controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false});

  struct Controls_LookStepResult { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("LookStep") Controls_LookStepResult Controls_LookStep(controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}, f64 dt = 0.0, bool keep = false);

  struct Controls_CarHeadStepResult { f64 yaw = 0.0; f64 pitch = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("CarHeadStep") Controls_CarHeadStepResult Controls_CarHeadStep(f64 yaw = 0.0, f64 pitch = 0.0, f64 x = 0.0, f64 y = 0.0, f64 dt = 0.0);

  struct Controls_ViewButtonStepResult { i64 out = 0; bool was = false; bool heldMap = false; f64 downAt = 0.0; f64 mapAt = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("ViewButtonStep") Controls_ViewButtonStepResult Controls_ViewButtonStep(bool was = false, bool heldMap = false, f64 downAt = 0.0, f64 mapAt = 0.0, bool down = false, f64 now = 0.0);

  struct Controls_VoiceChordStepResult { bool mic = false; bool mute = false; bool suppressWeapon = false; bool r3 = false; bool y = false; bool latched = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("VoiceChordStep") Controls_VoiceChordStepResult Controls_VoiceChordStep(bool r3 = false, bool y = false, bool latched = false, bool r3Now = false, bool yNow = false, bool allowed = false);

  struct Controls_ReleaseLatchStepResult { bool neutral = false; bool armed = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("ReleaseLatchStep") Controls_ReleaseLatchStepResult Controls_ReleaseLatchStep(bool armed = false, List<f64> values = List<f64>{});

  struct Controls_GrenadePullResult { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; bool pulled = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadePull") Controls_GrenadePullResult Controls_GrenadePull(controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}, f64 now = 0.0, i64 source = 0);

  struct Controls_GrenadeDragResult { f64 power = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeDrag") Controls_GrenadeDragResult Controls_GrenadeDrag(f64 upPx = 0.0);

  struct Controls_GrenadeReleaseResult { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; f64 left = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeRelease") Controls_GrenadeReleaseResult Controls_GrenadeRelease(controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}, f64 now = 0.0);

  struct Controls_GrenadeStepResult { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; bool cookedOff = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeStep") Controls_GrenadeStepResult Controls_GrenadeStep(controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}, f64 now = 0.0);

  struct Controls_GrenadeLeftResult { f64 seconds = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeLeft") Controls_GrenadeLeftResult Controls_GrenadeLeft(controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}, f64 now = 0.0);

  struct Controls_GrenadeSpeedResult { f64 mps = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeSpeed") Controls_GrenadeSpeedResult Controls_GrenadeSpeed(f64 power = 0.0);

  struct Controls_DevicePlatformResult { bool desktop = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("DevicePlatform") Controls_DevicePlatformResult Controls_DevicePlatform(bool fine = false, i64 touchPoints = 0);

  struct Controls_HintLineResult { Text line = ""; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("HintLine") Controls_HintLineResult Controls_HintLine(Text mode = "", Text weapon = "", bool dead = false);

  void Devices_input(controls_Devices& self, controls_Devices_Event e = controls_Devices_Event::touch_down) {
    bool now_desktop = false;
    bool now_pad = false;
    now_desktop = false;
    now_pad = false;
    func_call_on<"Controls_DeviceInput">(controls_g_rules, kern::out(n10.desktop, n10.pad), e, self.fine, self.coarse, self.desktop, self.pad);
    now_desktop = n10.desktop;
    now_pad = n10.pad;
    controls::Devices_set(self, now_desktop);
    self.pad = now_pad;
  }

  void Devices_touch_down(controls_Devices& self) {
    Devices_input(self, controls_Devices_Event::touch_down);
  }

  void Devices_mouse_down(controls_Devices& self) {
    Devices_input(self, controls_Devices_Event::mouse_down);
  }

  /// The mouse moved by itself (not locked, not over the UI).
  void Devices_mouse_move(controls_Devices& self) {
    Devices_input(self, controls_Devices_Event::mouse_move);
  }

  /// A real key (any device, phones too).
  void Devices_key(controls_Devices& self) {
    Devices_input(self, controls_Devices_Event::key);
  }

  void Devices_pad_button(controls_Devices& self) {
    Devices_input(self, controls_Devices_Event::pad_button);
  }

  struct bindResult { bool Result = false; };
  bindResult bind(Object<kern::game::GameWorld> world_2 = Object<kern::game::GameWorld>{}) {
    Object<kern::game::GameWorld> world_2_ = Object<kern::game::GameWorld>{};
    Actor actor_named = Actor{};
    world_2_ = world_2;
    actor_named = engine_game_world_actor_named(world_2_, "Controls");
    controls_g_rules = actor_named;
    return {func_has_blueprint(controls_g_rules)};
  }

  struct smoothstep01Result { f32 Result = 0.0f; };
  smoothstep01Result smoothstep01(f32 t = 0.0f) {
    func_call_on<"Controls_Smoothstep01">(controls_g_rules, kern::out(n11.value), blueprint_call::decimal(t).Result);
    return {num_cast<f32, f64>(n11.value)};
  }

  struct walk_pad_axisResult { f32 Result = 0.0f; };
  walk_pad_axisResult walk_pad_axis(f32 v = 0.0f) {
    func_call_on<"Controls_WalkPadAxis">(controls_g_rules, kern::out(n12.value), blueprint_call::decimal(v).Result);
    return {num_cast<f32, f64>(n12.value)};
  }

  struct shape_first_person_moveResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  shape_first_person_moveResult shape_first_person_move(f32 x = 0.0f, f32 y = 0.0f, f32 move_fineness = 0.0f, bool invert_h = false) {
    f64 rx = 0.0;
    f64 ry = 0.0;
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_ShapeFirstPersonMove">(controls_g_rules, kern::out(n13.x, n13.y), blueprint_call::decimal(x).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(move_fineness).Result, invert_h);
    rx = n13.x;
    ry = n13.y;
    return {vector_make(num_cast<f32, f64>(rx), num_cast<f32, f64>(ry), 0.0f)};
  }

  struct fps_dynamic_curveResult { f32 Result = 0.0f; };
  fps_dynamic_curveResult fps_dynamic_curve(f32 n = 0.0f) {
    func_call_on<"Controls_FpsDynamicCurve">(controls_g_rules, kern::out(n14.value), blueprint_call::decimal(n).Result);
    return {num_cast<f32, f64>(n14.value)};
  }

  struct look_curve_strengthResult { f32 Result = 0.0f; };
  look_curve_strengthResult look_curve_strength(f32 look_fineness = 0.0f) {
    func_call_on<"Controls_LookCurveStrength">(controls_g_rules, kern::out(n15.strength), blueprint_call::decimal(look_fineness).Result);
    return {num_cast<f32, f64>(n15.strength)};
  }

  struct shape_fps_stickResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  shape_fps_stickResult shape_fps_stick(f32 x = 0.0f, f32 y = 0.0f, f32 inner = 0.0f, f32 curve_strength = 0.0f) {
    f64 rx = 0.0;
    f64 ry = 0.0;
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_ShapeFpsStick">(controls_g_rules, kern::out(n16.x, n16.y), blueprint_call::decimal(x).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(inner).Result, blueprint_call::decimal(curve_strength).Result);
    rx = n16.x;
    ry = n16.y;
    return {vector_make(num_cast<f32, f64>(rx), num_cast<f32, f64>(ry), 0.0f)};
  }

  struct LookAxis_stepResult { f32 Result = 0.0f; };
  LookAxis_stepResult LookAxis_step(controls_LookAxis& self, f32 target = 0.0f, f32 dt = 0.0f) {
    f64 rx = 0.0;
    f64 ry = 0.0;
    Vector r = Vector{0.0, 0.0, 0.0};
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_LookAxisStep">(controls_g_rules, kern::out(n17.velocity, n17.angle), blueprint_call::decimal(self.velocity).Result, blueprint_call::decimal(target).Result, blueprint_call::decimal(dt).Result);
    rx = n17.velocity;
    ry = n17.angle;
    r = vector_make(num_cast<f32, f64>(rx), num_cast<f32, f64>(ry), 0.0f);
    self.velocity = vector_break(r).x;
    return {vector_break(r).y};
  }

  void touch_look_delta(f32& yaw, f32& pitch, f32 dx = 0.0f, f32 dy = 0.0f, f32 speed_px_s = 0.0f) {
    f64 rx = 0.0;
    f64 ry = 0.0;
    Vector r = Vector{0.0, 0.0, 0.0};
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_TouchLookDelta">(controls_g_rules, kern::out(n18.yaw, n18.pitch), blueprint_call::decimal(dx).Result, blueprint_call::decimal(dy).Result, blueprint_call::decimal(speed_px_s).Result);
    rx = n18.yaw;
    ry = n18.pitch;
    r = vector_make(num_cast<f32, f64>(rx), num_cast<f32, f64>(ry), 0.0f);
    yaw = vector_break(r).x;
    pitch = vector_break(r).y;
  }

  struct fps_aim_assistResult { controls_AimAssist Result = controls_AimAssist{false, 1.0f, 0.0f, 0.0f}; };
  fps_aim_assistResult fps_aim_assist(f32 yaw = 0.0f, f32 pitch = 0.0f, f32 dist = 0.0f, f32 input = 0.0f, f32 in_yaw = 0.0f, f32 in_pitch = 0.0f, f32 s = 0.0f) {
    controls_AimAssist a = controls_AimAssist{false, 1.0f, 0.0f, 0.0f};
    f64 slowdown = 0.0;
    f64 turn_yaw = 0.0;
    f64 turn_pitch = 0.0;
    a = controls_AimAssist{false, 1.0f, 0.0f, 0.0f};
    slowdown = 0.0;
    turn_yaw = 0.0;
    turn_pitch = 0.0;
    func_call_on<"Controls_FpsAimAssist">(controls_g_rules, kern::out(n19.active, n19.slowdown, n19.yaw, n19.pitch), blueprint_call::decimal(yaw).Result, blueprint_call::decimal(pitch).Result, blueprint_call::decimal(dist).Result, blueprint_call::decimal(input).Result, blueprint_call::decimal(in_yaw).Result, blueprint_call::decimal(in_pitch).Result, blueprint_call::decimal(s).Result);
    a.active = n19.active;
    slowdown = n19.slowdown;
    turn_yaw = n19.yaw;
    turn_pitch = n19.pitch;
    a.slowdown = num_cast<f32, f64>(slowdown);
    a.yaw = num_cast<f32, f64>(turn_yaw);
    a.pitch = num_cast<f32, f64>(turn_pitch);
    return {a};
  }

  struct aim_ellipseResult { f32 Result = 0.0f; };
  aim_ellipseResult aim_ellipse(f32 yaw_err = 0.0f, f32 pitch_err = 0.0f) {
    func_call_on<"Controls_AimEllipse">(controls_g_rules, kern::out(n20.value), blueprint_call::decimal(yaw_err).Result, blueprint_call::decimal(pitch_err).Result);
    return {num_cast<f32, f64>(n20.value)};
  }

  struct gamepad_axisResult { f32 Result = 0.0f; };
  gamepad_axisResult gamepad_axis(f32 v = 0.0f, f32 dz = 0.0f) {
    func_call_on<"Controls_GamepadAxis">(controls_g_rules, kern::out(n21.value), blueprint_call::decimal(v).Result, blueprint_call::decimal(dz).Result);
    return {num_cast<f32, f64>(n21.value)};
  }

  struct hard_cutResult { f32 Result = 0.0f; };
  hard_cutResult hard_cut(f32 v = 0.0f, f32 dz = 0.0f) {
    func_call_on<"Controls_HardCut">(controls_g_rules, kern::out(n22.value), blueprint_call::decimal(v).Result, blueprint_call::decimal(dz).Result);
    return {num_cast<f32, f64>(n22.value)};
  }

  struct height_axisResult { f32 Result = 0.0f; };
  height_axisResult height_axis(f32 rt = 0.0f, f32 lt = 0.0f) {
    func_call_on<"Controls_HeightAxis">(controls_g_rules, kern::out(n23.value), blueprint_call::decimal(rt).Result, blueprint_call::decimal(lt).Result);
    return {num_cast<f32, f64>(n23.value)};
  }

  struct phone_axisResult { f32 Result = 0.0f; };
  phone_axisResult phone_axis(f32 v = 0.0f, f32 fineness = 0.0f) {
    func_call_on<"Controls_PhoneAxis">(controls_g_rules, kern::out(n24.value), blueprint_call::decimal(v).Result, blueprint_call::decimal(fineness).Result);
    return {num_cast<f32, f64>(n24.value)};
  }

  struct game_state_stick_magnitudeResult { f32 Result = 0.0f; };
  game_state_stick_magnitudeResult game_state_stick_magnitude(f32 a = 0.0f) {
    func_call_on<"Controls_GameStickMagnitude">(controls_g_rules, kern::out(n25.value), blueprint_call::decimal(a).Result);
    return {num_cast<f32, f64>(n25.value)};
  }

  struct inverse_game_state_stick_magnitudeResult { f32 Result = 0.0f; };
  inverse_game_state_stick_magnitudeResult inverse_game_state_stick_magnitude(f32 v = 0.0f) {
    func_call_on<"Controls_InverseGameStickMagnitude">(controls_g_rules, kern::out(n26.value), blueprint_call::decimal(v).Result);
    return {num_cast<f32, f64>(n26.value)};
  }

  void apply_game_stick_left(controls_DroneAxes& out, f32 x = 0.0f, f32 y = 0.0f, f32 fineness = 0.0f, bool invert_h = false, f32 max_kmh = 0.0f) {
    f64 rx = 0.0;
    f64 ry = 0.0;
    Vector r = Vector{0.0, 0.0, 0.0};
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_GameStickLeft">(controls_g_rules, kern::out(n27.roll, n27.pitch), blueprint_call::decimal(x).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(fineness).Result, invert_h, blueprint_call::decimal(max_kmh).Result);
    rx = n27.roll;
    ry = n27.pitch;
    r = vector_make(num_cast<f32, f64>(rx), num_cast<f32, f64>(ry), 0.0f);
    out.roll = vector_break(r).x;
    out.pitch = vector_break(r).y;
  }

  void apply_game_stick_right(controls_DroneAxes& out, f32 x = 0.0f, f32 y = 0.0f, f32 fineness = 0.0f, bool invert_h = false, bool invert_v = false) {
    f64 rx = 0.0;
    f64 ry = 0.0;
    Vector r = Vector{0.0, 0.0, 0.0};
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_GameStickRight">(controls_g_rules, kern::out(n28.yaw, n28.bodyPitch), blueprint_call::decimal(x).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(fineness).Result, invert_h, invert_v);
    rx = n28.yaw;
    ry = n28.bodyPitch;
    r = vector_make(num_cast<f32, f64>(rx), num_cast<f32, f64>(ry), 0.0f);
    out.yaw = vector_break(r).x;
    out.body_pitch = vector_break(r).y;
  }

  struct clearance_pad_axisResult { f32 Result = 0.0f; };
  clearance_pad_axisResult clearance_pad_axis(f32 centre_y = 0.0f, f32 y = 0.0f, f32 height = 0.0f) {
    func_call_on<"Controls_ClearancePadAxis">(controls_g_rules, kern::out(n29.value), blueprint_call::decimal(centre_y).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(height).Result);
    return {num_cast<f32, f64>(n29.value)};
  }

  void RateStick_step(controls_RateStick& self, Vector px = Vector{0.0, 0.0, 0.0}, f32 dt = 0.0f) {
    func_call_on<"Controls_RateStickStep">(controls_g_rules, kern::out(n30.rate), blueprint_call::decimal(vector_break(self.rate).x).Result, blueprint_call::decimal(vector_break(px).x).Result, blueprint_call::decimal(dt).Result, blueprint_call::decimal(self.full_px_s).Result, blueprint_call::decimal(self.tau).Result);
    self.rate = vector_make(num_cast<f32, f64>(n30.rate), vector_break(self.rate).y, vector_break(self.rate).z);
    func_call_on<"Controls_RateStickStep">(controls_g_rules, kern::out(n31.rate), blueprint_call::decimal(vector_break(self.rate).y).Result, blueprint_call::decimal(vector_break(px).y).Result, blueprint_call::decimal(dt).Result, blueprint_call::decimal(self.full_px_s).Result, blueprint_call::decimal(self.tau).Result);
    self.rate = vector_make(vector_break(self.rate).x, num_cast<f32, f64>(n31.rate), vector_break(self.rate).z);
  }

  void WorldLook_add(controls_WorldLook& self, f32 dx = 0.0f, f32 dy = 0.0f, f32 kx = 0.0f, f32 ky = 0.0f) {
    controls_Look l = controls_Look{0.0, 0.0, false, 0.0, false};
    l = controls_Look{0.0, 0.0, false, 0.0, false};
    func_call_on<"Controls_LookAdd">(controls_g_rules, kern::out(n32.look), controls_cpp::look_of(self).Result, blueprint_call::decimal(dx).Result, blueprint_call::decimal(dy).Result, blueprint_call::decimal(kx).Result, blueprint_call::decimal(ky).Result);
    l = n32.look;
    controls_cpp::take(l, self);
  }

  void WorldLook_set(controls_WorldLook& self, f32 yaw = 0.0f, f32 pitch = 0.0f) {
    controls_Look l = controls_Look{0.0, 0.0, false, 0.0, false};
    l = controls_Look{0.0, 0.0, false, 0.0, false};
    func_call_on<"Controls_LookSet">(controls_g_rules, kern::out(n33.look), blueprint_call::decimal(yaw).Result, blueprint_call::decimal(pitch).Result);
    l = n33.look;
    controls_cpp::take(l, self);
  }

  void WorldLook_release(controls_WorldLook& self) {
    controls_Look l = controls_Look{0.0, 0.0, false, 0.0, false};
    l = controls_Look{0.0, 0.0, false, 0.0, false};
    func_call_on<"Controls_LookRelease">(controls_g_rules, kern::out(n34.look), controls_cpp::look_of(self).Result);
    l = n34.look;
    controls_cpp::take(l, self);
  }

  void WorldLook_step(controls_WorldLook& self, f32 dt = 0.0f, bool keep = false) {
    controls_Look l = controls_Look{0.0, 0.0, false, 0.0, false};
    l = controls_Look{0.0, 0.0, false, 0.0, false};
    func_call_on<"Controls_LookStep">(controls_g_rules, kern::out(n35.look), controls_cpp::look_of(self).Result, blueprint_call::decimal(dt).Result, keep);
    l = n35.look;
    controls_cpp::take(l, self);
  }

  void CarHeadSpring_step(controls_CarHeadSpring& self, f32 x = 0.0f, f32 y = 0.0f, f32 dt = 0.0f) {
    f64 rx = 0.0;
    f64 ry = 0.0;
    Vector r = Vector{0.0, 0.0, 0.0};
    rx = 0.0;
    ry = 0.0;
    func_call_on<"Controls_CarHeadStep">(controls_g_rules, kern::out(n36.yaw, n36.pitch), blueprint_call::decimal(self.yaw).Result, blueprint_call::decimal(self.pitch).Result, blueprint_call::decimal(x).Result, blueprint_call::decimal(y).Result, blueprint_call::decimal(dt).Result);
    rx = n36.yaw;
    ry = n36.pitch;
    r = vector_make(num_cast<f32, f64>(rx), num_cast<f32, f64>(ry), 0.0f);
    self.yaw = vector_break(r).x;
    self.pitch = vector_break(r).y;
  }

  struct ViewButton_stepResult { i32 Result = 0; };
  ViewButton_stepResult ViewButton_step(controls_ViewButton& self, bool down = false, f32 now = 0.0f) {
    i64 out = 0;
    bool now_was = false;
    bool now_held_map = false;
    f64 now_down_at = 0.0;
    f64 now_map_at = 0.0;
    out = 0;
    now_was = false;
    now_held_map = false;
    now_down_at = 0.0;
    now_map_at = 0.0;
    func_call_on<"Controls_ViewButtonStep">(controls_g_rules, kern::out(n37.out, n37.was, n37.heldMap, n37.downAt, n37.mapAt), self.was, self.held_map, blueprint_call::decimal(self.down_at).Result, blueprint_call::decimal(self.map_at).Result, down, blueprint_call::decimal(now).Result);
    out = n37.out;
    now_was = n37.was;
    now_held_map = n37.heldMap;
    now_down_at = n37.downAt;
    now_map_at = n37.mapAt;
    self.was = now_was;
    self.held_map = now_held_map;
    self.down_at = num_cast<f32, f64>(now_down_at);
    self.map_at = num_cast<f32, f64>(now_map_at);
    return {num_cast<i32, i64>(out)};
  }

  struct VoiceChord_stepResult { controls_VoiceChord_Out Result = controls_VoiceChord_Out{false, false, false}; };
  VoiceChord_stepResult VoiceChord_step(controls_VoiceChord& self, bool r3_now = false, bool y_now = false, bool allowed = false) {
    controls_VoiceChord_Out o = controls_VoiceChord_Out{false, false, false};
    o = controls_VoiceChord_Out{false, false, false};
    func_call_on<"Controls_VoiceChordStep">(controls_g_rules, kern::out(n38.mic, n38.mute, n38.suppressWeapon, n38.r3, n38.y, n38.latched), self.r3, self.y, self.latched, r3_now, y_now, allowed);
    o.mic = n38.mic;
    o.mute = n38.mute;
    o.suppress_weapon = n38.suppressWeapon;
    self.r3 = n38.r3;
    self.y = n38.y;
    self.latched = n38.latched;
    return {o};
  }

  struct ReleaseLatch_stepResult { bool Result = false; };
  ReleaseLatch_stepResult ReleaseLatch_step(controls_ReleaseLatch& self, const Fixed<f32, 9>& values) {
    List<f64> list = List<f64>{};
    f32 v = 0.0f;
    bool neutral = false;
    list = List<f64>{};
    for (auto it39 = flow_for(n39, 0, math_sub<i64>(list_length<f32>(values), 1)); it39.next();) {
      v = values[n39.index];
      list_add(list, num_cast<f64, f32>(v));
    }
    neutral = false;
    func_call_on<"Controls_ReleaseLatchStep">(controls_g_rules, kern::out(n40.neutral, n40.armed), self.armed, list);
    neutral = n40.neutral;
    self.armed = n40.armed;
    return {neutral};
  }

  struct GrenadeCook_pullResult { bool Result = false; };
  GrenadeCook_pullResult GrenadeCook_pull(controls_GrenadeCook& self, f32 now = 0.0f, i32 from = 0) {
    controls_Cook c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    bool pulled = false;
    c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    pulled = false;
    func_call_on<"Controls_GrenadePull">(controls_g_rules, kern::out(n41.cook, n41.pulled), controls_cpp::cook_of(self).Result, blueprint_call::decimal(now).Result, num_cast<i64, i32>(from));
    c = n41.cook;
    pulled = n41.pulled;
    controls_cpp::take_2(c, self);
    return {pulled};
  }

  void GrenadeCook_drag(controls_GrenadeCook& self, f32 up_px = 0.0f) {
    func_call_on<"Controls_GrenadeDrag">(controls_g_rules, kern::out(n42.power), blueprint_call::decimal(up_px).Result);
    self.power = num_cast<f32, f64>(n42.power);
  }

  struct GrenadeCook_releaseResult { f32 Result = 0.0f; };
  GrenadeCook_releaseResult GrenadeCook_release(controls_GrenadeCook& self, f32 now = 0.0f) {
    controls_Cook c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    f64 left = 0.0;
    c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    left = 0.0;
    func_call_on<"Controls_GrenadeRelease">(controls_g_rules, kern::out(n43.cook, n43.left), controls_cpp::cook_of(self).Result, blueprint_call::decimal(now).Result);
    c = n43.cook;
    left = n43.left;
    controls_cpp::take_2(c, self);
    return {num_cast<f32, f64>(left)};
  }

  struct GrenadeCook_stepResult { bool Result = false; };
  GrenadeCook_stepResult GrenadeCook_step(controls_GrenadeCook& self, f32 now = 0.0f) {
    controls_Cook c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    bool cooked_off = false;
    c = controls_Cook{0, 0.0, false, 0.0, 0.0, 0};
    cooked_off = false;
    func_call_on<"Controls_GrenadeStep">(controls_g_rules, kern::out(n44.cook, n44.cookedOff), controls_cpp::cook_of(self).Result, blueprint_call::decimal(now).Result);
    c = n44.cook;
    cooked_off = n44.cookedOff;
    controls_cpp::take_2(c, self);
    return {cooked_off};
  }

  struct GrenadeCook_leftResult { f32 Result = 0.0f; };
  GrenadeCook_leftResult GrenadeCook_left(const controls_GrenadeCook& self, f32 now = 0.0f) {
    func_call_on<"Controls_GrenadeLeft">(controls_g_rules, kern::out(n45.seconds), controls_cpp::cook_of(self).Result, blueprint_call::decimal(now).Result);
    return {num_cast<f32, f64>(n45.seconds)};
  }

  struct GrenadeCook_speedResult { f32 Result = 0.0f; };
  GrenadeCook_speedResult GrenadeCook_speed(const controls_GrenadeCook& self) {
    func_call_on<"Controls_GrenadeSpeed">(controls_g_rules, kern::out(n46.mps), blueprint_call::decimal(self.power).Result);
    return {num_cast<f32, f64>(n46.mps)};
  }

  struct grenade_fuseResult { f32 Result = 0.0f; };
  KERN_PURE grenade_fuseResult grenade_fuse() {
    return {num_cast<f32, f64>(func_get_on<"GrenadeFuse", f64>(controls_g_rules))};
  }

  void Devices_platform(controls_Devices& self, bool fine_pointer = false, bool coarse_pointer = false, i32 max_touch_points = 0) {
    self.fine = fine_pointer;
    self.coarse = coarse_pointer;
    self.touch_points = max_touch_points;
    func_call_on<"Controls_DevicePlatform">(controls_g_rules, kern::out(n47.desktop), self.fine, num_cast<i64, i32>(self.touch_points));
    controls::Devices_set(self, n47.desktop);
  }

  struct hint_lineResult { Text Result = ""; };
  hint_lineResult hint_line(Text mode = "", Text weapon = "", bool dead = false) {
    Text line = "";
    line = "";
    func_call_on<"Controls_HintLine">(controls_g_rules, kern::out(n48.line), mode, weapon, dead);
    line = n48.line;
    return {line};
  }

  struct sc_tile_requestsResult { Text Result = ""; };
  /// ---- map tiles: the page fetches what the game asks for ----
  KERN_EXPORT_AS("sc_tile_requests", "-> text") sc_tile_requestsResult sc_tile_requests() {
    n1048586 = city_stream_cpp::CityStream_take_requests(city_);
    return {n1048586.Result};
  }

  KERN_EXPORT_AS("sc_tile_data", "i32 i32 i32 bytes at len") void sc_tile_data(i32 z = 0, i32 x = 0, i32 y = 0, List<u8> bytes = List<u8>{}, u32 bytes_at = 0, u32 length = 0) {
    i64 bytes_at_i = 0;
    TileKey a = TileKey{14, 0, 0};
    List<u8> part = List<u8>{};
    bytes_at_i = num_cast<i64, u32>(bytes_at);
    a = struct_make<TileKey>(z, x, y);
    part = List<u8>{};
    list_resize(part, num_cast<i64, u32>(length), 0);
    list_copy_bytes<"u8", "u8">(part, 0, bytes, bytes_at_i, num_cast<i64, u32>(length));
    city_stream_cpp::CityStream_provide(city_, a, part);
  }

  KERN_EXPORT_AS("sc_tile_failed", "i32 i32 i32") void sc_tile_failed(i32 z = 0, i32 x = 0, i32 y = 0) {
    TileKey a = TileKey{14, 0, 0};
    a = struct_make<TileKey>(z, x, y);
    city_stream_cpp::CityStream_fail(city_, a);
  }

  KERN_EXPORT_AS("sc_offline", "") void sc_offline() {
    city_stream_cpp::CityStream_go_offline(city_);
  }

  struct sc_dem_requestsResult { Text Result = ""; };
  /// ---- elevation tiles (Terrarium PNGs): the page decodes the image, the game the heights ----
  KERN_EXPORT_AS("sc_dem_requests", "-> text") sc_dem_requestsResult sc_dem_requests() {
    n1048587 = terrain_cpp::Terrain_take_requests(city_.terrain_);
    return {n1048587.Result};
  }

  /// RGBA pixels of a requested elevation tile (`length` = width x height x 4 bytes, copied).
  KERN_EXPORT_AS("sc_dem_data", "i32 i32 i32 bytes at len u32 u32") void sc_dem_data(i32 z = 0, i32 x = 0, i32 y = 0, List<u8> rgba = List<u8>{}, u32 rgba_at = 0, u32 length = 0, u32 width = 0, u32 height = 0) {
    i64 rgba_at_i = 0;
    TileKey a = TileKey{14, 0, 0};
    List<u8> part = List<u8>{};
    rgba_at_i = num_cast<i64, u32>(rgba_at);
    a = struct_make<TileKey>(z, x, y);
    part = List<u8>{};
    list_resize(part, num_cast<i64, u32>(length), 0);
    list_copy_bytes<"u8", "u8">(part, 0, rgba, rgba_at_i, num_cast<i64, u32>(length));
    terrain_cpp::Terrain_provide(city_.terrain_, a, part, width, height);
  }

  KERN_EXPORT_AS("sc_dem_failed", "i32 i32 i32") void sc_dem_failed(i32 z = 0, i32 x = 0, i32 y = 0) {
    TileKey a = TileKey{14, 0, 0};
    a = struct_make<TileKey>(z, x, y);
    terrain_cpp::Terrain_fail(city_.terrain_, a);
  }

  struct sc_image_requestsResult { Text Result = ""; };
  /// ---- aerial imagery for the ground colour (world_ground.mjs): the page decodes, the game averages ----
  KERN_EXPORT_AS("sc_image_requests", "-> text") sc_image_requestsResult sc_image_requests() {
    n1048588 = terrain_cpp::Terrain_take_image_requests(city_.terrain_);
    return {n1048588.Result};
  }

  /// RGBA pixels of a requested aerial tile (`length` = width x height x 4 bytes, copied).
  KERN_EXPORT_AS("sc_image_data", "i32 i32 i32 bytes at len u32 u32") void sc_image_data(i32 z = 0, i32 x = 0, i32 y = 0, List<u8> rgba = List<u8>{}, u32 rgba_at = 0, u32 length = 0, u32 width = 0, u32 height = 0) {
    i64 rgba_at_i = 0;
    TileKey a = TileKey{14, 0, 0};
    List<u8> part = List<u8>{};
    rgba_at_i = num_cast<i64, u32>(rgba_at);
    a = struct_make<TileKey>(z, x, y);
    part = List<u8>{};
    list_resize(part, num_cast<i64, u32>(length), 0);
    list_copy_bytes<"u8", "u8">(part, 0, rgba, rgba_at_i, num_cast<i64, u32>(length));
    terrain_cpp::Terrain_provide_image(city_.terrain_, a, part, width, height);
  }

  KERN_EXPORT_AS("sc_image_failed", "i32 i32 i32") void sc_image_failed(i32 z = 0, i32 x = 0, i32 y = 0) {
    TileKey a = TileKey{14, 0, 0};
    a = struct_make<TileKey>(z, x, y);
    terrain_cpp::Terrain_fail_image(city_.terrain_, a);
  }

  /// ---- input the App host does not forward ----
  KERN_EXPORT_AS("sc_gamepad", "i32 f32 f32 f32 f32 f32 f32 u32 i32") void sc_gamepad(i32 connected = 0, f32 lx = 0.0f, f32 ly = 0.0f, f32 rx = 0.0f, f32 ry = 0.0f, f32 lt = 0.0f, f32 rt = 0.0f, u32 buttons = 0, i32 active = 0) {
    pad.pressed = kern::formula<u32, u32>([](auto buttons, auto buttons_2) { return (buttons & (~buttons_2)); }, buttons, pad.buttons);
    pad.connected = math_compare<"!=", i32>(connected, 0);
    pad.lx = lx;
    pad.ly = ly;
    pad.rx = rx;
    pad.ry = ry;
    pad.lt = lt;
    pad.rt = rt;
    pad.buttons = buttons;
    pad.active = math_compare<"!=", i32>(active, 0);
  }

  /// Mouse movement under the pointer lock (movementX/Y of mousemove, CSS px, clamped +-420 per event by the page: desktop_controls.mjs MAX_EVENT_PX) and the lock state.
  KERN_EXPORT_AS("sc_mouse", "f32 f32") void sc_mouse(f32 dx = 0.0f, f32 dy = 0.0f) {
    mouse_move(dx, dy);
  }

  KERN_EXPORT_AS("sc_pointer_lock", "i32") void sc_pointer_lock(i32 locked = 0) {
    pointer_locked(math_compare<"!=", i32>(locked, 0));
  }

  /// Viewport in CSS px, safe-area insets and the device pixel ratio.
  KERN_EXPORT_AS("sc_touch_frame", "f32 f32 f32 f32 f32 f32 f32") void sc_touch_frame(f32 w = 0.0f, f32 h = 0.0f, f32 sl = 0.0f, f32 st = 0.0f, f32 sr = 0.0f, f32 sb = 0.0f, f32 dpr = 0.0f) {
    set_touch_frame(w, h, sl, st, sr, sb, dpr);
  }

  struct sc_touchResult { i32 Result = 0; };
  /// A touch / pen finger (type 0 down, 1 move, 2 up, 3 cancel; CSS px; `t` the time in ms, not needed). Returns 1 when used.
  KERN_EXPORT_AS("sc_touch", "u32 i32 f32 f32 f64 -> i32") sc_touchResult sc_touch(u32 type = 0, i32 id = 0, f32 x = 0.0f, f32 y = 0.0f, f64 t = 0.0) {
    n1048589 = touch(type, id, x, y);
    return {kern::formula<bool, i32, i32>([](auto the_result, auto a1, auto a0) { return (the_result ? a1 : a0); }, n1048589.Result, 1, 0)};
  }

  struct sc_touch_viewResult { Text Result = ""; };
  KERN_EXPORT_AS("sc_touch_view", "-> text") sc_touch_viewResult sc_touch_view() {
    n1048590 = touch_view_json();
    return {n1048590.Result};
  }

  struct sc_hudResult { Text Result = ""; };
  /// ---- presentation ----
  KERN_EXPORT_AS("sc_hud", "-> text") sc_hudResult sc_hud() {
    n1048591 = hud_json();
    return {n1048591.Result};
  }

  struct sc_commandResult { Text Result = ""; };
  KERN_EXPORT_AS("sc_command", "text len -> text") sc_commandResult sc_command(Text text = "", u32 length = 0) {
    Text a = "";
    a = text_sub(text, 0, num_cast<i64, u32>(length));
    n1048592 = command(a);
    return {n1048592.Result};
  }

  struct sc_audio_initResult { i32 Result = 0; };
  /// ---- audio: Kern's mixer renders, the page plays (KernAudio) ----
  KERN_EXPORT_AS("sc_audio_init", "u32 -> i32") sc_audio_initResult sc_audio_init(u32 rate = 0) {
    n1048593 = audio_init(rate);
    return {kern::formula<bool, i32, i32>([](auto the_result, auto a1, auto a0) { return (the_result ? a1 : a0); }, n1048593.Result, 1, 0)};
  }

  struct sc_audio_renderResult { List<f32> Result = List<f32>{}; };
  KERN_EXPORT_AS("sc_audio_render", "u32 -> floats") sc_audio_renderResult sc_audio_render(u32 frames = 0) {
    n1048594 = audio_render(frames);
    return {n1048594.Result};
  }

  struct Settings_SnapResult { f64 value = 0.0; };
  /// (of the Blueprint of the actor Settings, called on it)
  KERN_EXTERN_AS("Snap") Settings_SnapResult Settings_Snap(Option option = Option{"", "", "", "", 0.0, 0.0, 0.0, 0.0, false, false}, f64 value = 0.0);

  struct Settings_WordResult { bool known = false; f64 value = 0.0; };
  /// (of the Blueprint of the actor Settings, called on it)
  KERN_EXTERN_AS("Word") Settings_WordResult Settings_Word(Text text = "");

  struct Settings_InSectionResult { bool in = false; };
  /// (of the Blueprint of the actor Settings, called on it)
  KERN_EXTERN_AS("InSection") Settings_InSectionResult Settings_InSection(Option option = Option{"", "", "", "", 0.0, 0.0, 0.0, 0.0, false, false}, Text section = "");

  struct bind_settingsResult { bool Result = false; };
  bind_settingsResult bind_settings(SettingsTable& table, Object<kern::game::GameWorld> world_2 = Object<kern::game::GameWorld>{}) {
    Object<kern::game::GameWorld> world_2_ = Object<kern::game::GameWorld>{};
    bool runs = false;
    Actor actor_named = Actor{};
    world_2_ = world_2;
    actor_named = engine_game_world_actor_named(world_2_, "Settings");
    settings_g_rules = actor_named;
    runs = func_has_blueprint(settings_g_rules);
    list_clear(table.specs);
    table.rows = func_get_on<"Options", List<Option>>(settings_g_rules);
    if (logic_or(logic_not(runs), math_compare<"!=", u64>(num_cast<u64, i64>(list_length<Option>(table.rows)), num_cast<u64, u32>(kCount)))) {
      list_clear(table.rows);
      return {false};
    }
    for (auto it2097162 = flow_for(n2097162, 0, math_sub<i64>(list_length<Option>(table.rows), 1)); it2097162.next();) {
      list_add(table.specs, struct_make<OptSpec>(table.rows[n2097162.index].key, table.rows[n2097162.index].section, table.rows[n2097162.index].label, table.rows[n2097162.index].help, num_cast<f32, f64>(table.rows[n2097162.index].the_min), num_cast<f32, f64>(table.rows[n2097162.index].the_max), num_cast<f32, f64>(table.rows[n2097162.index].step), num_cast<f32, f64>(table.rows[n2097162.index].def), table.rows[n2097162.index].toggle, table.rows[n2097162.index].live));
    }
    return {true};
  }

  void Settings_set(Settings& self, const SettingsTable& table, Opt o = Opt::mouse_sensitivity, f32 value = 0.0f) {
    u32 i = 0;
    f32 v = 0.0f;
    i = num_cast<u32, i64>(enum_index<Opt>(o));
    if (logic_or(math_compare<">=", u64>(num_cast<u64, u32>(i), num_cast<u64, i64>(list_length<Option>(table.rows))), logic_not(kern::formula<f32>([](auto value) { return isfinite(value); }, value)))) {
      return;
    }
    func_call_on<"Settings_Snap">(settings_g_rules, kern::out(n2097163.value), table.rows[num_cast<i64, u32>(i)], blueprint_call::decimal(value).Result);
    v = num_cast<f32, f64>(n2097163.value);
    if (math_compare<"!=", f32>(v, self.v_[num_cast<i64, u32>(i)])) {
      self.v_[num_cast<i64, u32>(i)] = v;
      self.version_ = kern::formula<u32>([](auto version_) { return (version_ + 1); }, self.version_);
    }
  }

  struct Settings_set_2Result { bool Result = false; };
  Settings_set_2Result Settings_set_2(Settings& self, const SettingsTable& table, Text key = "", Text value = "") {
    Opt o = Opt::mouse_sensitivity;
    bool known = false;
    f64 word = 0.0;
    Text text = "";
    i64 end = 0;
    f64 d = 0.0;
    o = Opt::mouse_sensitivity;
    n2097164 = settings_cpp::find_opt(table, o, key);
    if (logic_not(n2097164.Result)) {
      return {false};
    }
    known = false;
    word = 0.0;
    func_call_on<"Settings_Word">(settings_g_rules, kern::out(n2097165.known, n2097165.value), value);
    known = n2097165.known;
    word = n2097165.value;
    if (known) {
      Settings_set(self, table, o, num_cast<f32, f64>(word));
      return {true};
    }
    text = value;
    end = 0;
    end = text_parse_float(text, 0).end;
    d = text_parse_float(text, 0).value;
    if (math_compare<"==", i64>(end, 0)) {
      return {false};
    }
    Settings_set(self, table, o, num_cast<f32, f64>(d));
    return {true};
  }

  void Settings_reset_section(Settings& self, const SettingsTable& table, Text section = "") {
    u32 i = 0;
    i = 0;
    while (loop(math_compare<"<", u64>(num_cast<u64, u32>(i), num_cast<u64, i64>(list_length<Option>(table.rows))))) {
      func_call_on<"Settings_InSection">(settings_g_rules, kern::out(n2097166.in), table.rows[num_cast<i64, u32>(i)], section);
      if (n2097166.in) {
        Settings_set(self, table, enum_from_index<Opt>(num_cast<i64, u32>(i)).value, table.specs[num_cast<i64, u32>(i)].def);
      }
      i += 1;
    }
  }

  void pointer_locked(bool locked = false) {
    locked_ = locked;
  }

  struct placedResult { bool Result = false; };
  /// ---- what a test reads
  KERN_PURE placedResult placed() {
    return {placed_};
  }

  struct worldResult { Object<kern::game::GameWorld> Result = Object<kern::game::GameWorld>{}; };
  KERN_PURE worldResult world() {
    return {world_};
  }

  struct stepsResult { u64 Result = 0; };
  /// The fixed steps run so far (1/240 s each).
  KERN_PURE stepsResult steps() {
    return {steps_};
  }

  struct startResult { bool Result = false; };
  KERN_OVERRIDE startResult start(Object<kern::Engine> e = Object<kern::Engine>{}) {
    Object<kern::Engine> e_ = Object<kern::Engine>{};
    f64 arg_number = 0.0;
    f64 arg_number_2 = 0.0;
    f64 arg_number_3 = 0.0;
    Object<kern::game::GameWorld> old = Object<kern::game::GameWorld>{};
    Object<kern::game::GameWorld> game_world = Object<kern::game::GameWorld>{};
    bool start_level = false;
    Text last_error = "";
    bool fail = false;
    Text last_error_2 = "";
    bool fail_2 = false;
    bool fail_3 = false;
    Actor spawn = Actor{};
    Text last_error_3 = "";
    bool fail_4 = false;
    Text wsel = "";
    Text arg = "";
    f64 arg_number_4 = 0.0;
    f64 arg_number_5 = 0.0;
    bool both = false;
    f64 arg_number_6 = 0.0;
    f64 arg_number_7 = 0.0;
    bool both_2 = false;
    bool either = false;
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    f64 arg_number_8 = 0.0;
    f64 arg_number_9 = 0.0;
    f64 arg_number_10 = 0.0;
    bool both_3 = false;
    f64 arg_2 = 0.0;
    f64 arg_3 = 0.0;
    f64 arg_number_11 = 0.0;
    Object<kern::Renderer> renderer_2 = Object<kern::Renderer>{};
    Object<kern::Renderer> renderer_3 = Object<kern::Renderer>{};
    e_ = e;
    engine_ = e_;
    web_audio_ = false;
    locked_ = web_audio_;
    touch_mode_ = locked_;
    paused_ = touch_mode_;
    placed_ = paused_;
    steps_ = 0;
    accumulator_ = 0.0;
    applied_ = 0;
    moves_ = 0;
    resets_ = moves_;
    cam_mode_ = 0;
    mouse_px_ = Vector{0.0, 0.0, 0.0};
    pad = struct_make<PadState>(false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, false);
    pad_was_ = 0;
    fps_ = 60.0f;
    toast_ = "";
    toast_age_ = 99.0f;
    arg_number = engine_engine_arg_number(e_, "mobile", 0.0);
    mobile_ = math_compare<">", f64>(arg_number, 0.0);
    arg_number_2 = engine_engine_arg_number(e_, "lat", kKonstanzLat);
    lat_ = arg_number_2;
    arg_number_3 = engine_engine_arg_number(e_, "lon", kKonstanzLon);
    lon_ = arg_number_3;
    city_.building_budget = kern::formula<bool, i32, i32>([](auto mobile_, auto a2600, auto a4000) { return num_cast<u32, i32>((mobile_ ? a2600 : a4000)); }, mobile_, 2600, 4000);
    old = world_;
    engine_game_world_make_2(n4194314, e_);
    game_world = n4194314.result;
    world_ = game_world;
    if (object_valid<kern::game::GameWorld>(old)) {
      engine_game_world_release(old);
    }
    engine_game_world_start_level(n4194315, world_, "spawncity.klevel", game_SimMode::simulate);
    start_level = n4194315.result;
    if (logic_not(start_level)) {
      last_error = engine_last_error();
      engine_fail(n4194316, text_format<"level start: %s">(-1, last_error));
      fail = n4194316.result;
      return {fail};
    }
    n4194317 = bind(world_);
    if (logic_not(n4194317.Result)) {
      last_error_2 = engine_last_error();
      engine_fail(n4194318, text_format<"the level's Blueprint \"Controls\" is missing or has errors: %s">(-1, last_error_2));
      fail_2 = n4194318.result;
      return {fail_2};
    }
    n4194319 = bind_settings(settings_table_, world_);
    if (logic_not(n4194319.Result)) {
      engine_fail(n4194320, text_format<"the level's Blueprint \"Settings\" is missing, has errors or not one row per setting">(-1));
      fail_3 = n4194320.result;
      return {fail_3};
    }
    n4194321 = settings_cpp::Settings_new(settings_table_);
    settings_ = n4194321.Result;
    engine_game_world_spawn(n4194322, world_, "Drone", struct_make<game_Transform>(Vector{0.0, 0.0, -200.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}), 0);
    spawn = n4194322.result;
    drone_actor_ = num_cast<u32, i64>(actor_id(spawn));
    if (logic_not(num_cast<bool, u32>(drone_actor_))) {
      last_error_3 = engine_last_error();
      engine_fail(n4194323, text_format<"SpawnCity: the level's template \"Drone\": %s (tools/level_templates.sh puts it in)">(-1, last_error_3));
      fail_4 = n4194323.result;
      return {fail_4};
    }
    engine_game_world_set_alive(world_, drone_actor_, false);
    engine_game_world_set_paused(world_, true);
    arg = engine_engine_arg(e_, "world", "");
    wsel = arg;
    either = text_equal(wsel, "training");
    if (logic_not(either)) {
      both_2 = logic_not(text_equal(wsel, "town"));
      if (both_2) {
        arg_number_7 = engine_engine_arg_number(e_, "offline", 0.0);
        both_2 = math_compare<"<=", f64>(arg_number_7, 0.0);
      }
      either = both_2;
    }
    city_.training_offline = either;
    renderer = engine_engine_renderer(e_);
    engine_game_world_physics(n4194324, world_);
    physics = n4194324.result;
    arg_2 = lon_;
    arg_3 = lat_;
    arg_number_8 = engine_engine_arg_number(e_, "dem", 1.0);
    both_3 = math_compare<">", f64>(arg_number_8, 0.0);
    if (both_3) {
      arg_number_10 = engine_engine_arg_number(e_, "offline", 0.0);
      both_3 = math_compare<"<=", f64>(arg_number_10, 0.0);
    }
    city_stream_cpp::CityStream_start(city_, renderer, physics, arg_2, arg_3, both_3);
    arg_number_11 = engine_engine_arg_number(e_, "offline", 0.0);
    if (math_compare<">", f64>(arg_number_11, 0.0)) {
      city_stream_cpp::CityStream_go_offline(city_);
    }
    renderer_2 = engine_engine_renderer(e_);
    n4194325 = drone_art_cpp::DroneArt_init(art_, renderer_2);
    if (logic_not(n4194325.Result)) {
      return {false};
    }
    mouse_ = struct_make<controls_RateStick>(1500.0f, 0.05f, Vector{0.0, 0.0, 0.0});
    engine_engine_set_status(e_, "SpawnCity: Karte wird geladen ...");
    cam_.eye = Vector{-30.0, -40.0, 60.0};
    cam_.target = Vector{0.0, 0.0, 0.0};
    renderer_3 = engine_engine_renderer(e_);
    engine_renderer_set_camera(renderer_3, cam_);
    return {true};
  }

  KERN_OVERRIDE void stop(Object<kern::Engine> arg1 = Object<kern::Engine>{}) {
    Object<kern::game::GameAudio> old = Object<kern::game::GameAudio>{};
    Object<kern::game::SoundBank> old_2 = Object<kern::game::SoundBank>{};
    Object<kern::game::GameWorld> old_3 = Object<kern::game::GameWorld>{};
    if (object_valid<kern::game::GameAudio>(game_audio_)) {
      engine_game_audio_end(game_audio_, 0.25f);
    }
    old = game_audio_;
    game_audio_ = Object<kern::game::GameAudio>{};
    if (object_valid<kern::game::GameAudio>(old)) {
      engine_game_audio_release(old);
    }
    old_2 = bank_;
    bank_ = Object<kern::game::SoundBank>{};
    if (object_valid<kern::game::SoundBank>(old_2)) {
      engine_sound_bank_release(old_2);
    }
    city_stream_cpp::CityStream_clear(city_);
    old_3 = world_;
    world_ = Object<kern::game::GameWorld>{};
    if (object_valid<kern::game::GameWorld>(old_3)) {
      engine_game_world_release(old_3);
    }
    placed_ = false;
  }

  struct drone_matrixResult { Mat4 Result = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}; };
  drone_matrixResult drone_matrix() {
    Mat4 m = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    bool actor_matrix = false;
    m = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    if (object_valid<kern::game::GameWorld>(world_)) {
      engine_game_world_actor_matrix(n4194326, world_, drone_actor_, m);
      actor_matrix = n4194326.result;
      m = n4194326.out;
    }
    return {m};
  }

  struct drone_positionResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  drone_positionResult drone_position() {
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    physics_Body actor_body = physics_Body{0};
    if (logic_or(logic_not(object_valid<kern::game::GameWorld>(world_)), logic_not(num_cast<bool, u32>(drone_actor_)))) {
      return {Vector{0.0, 0.0, 0.0}};
    }
    engine_game_world_physics(n4194327, world_);
    physics = n4194327.result;
    actor_body = engine_game_world_actor_body(world_, drone_actor_);
    return {struct_break<BodyState>(common::read_body(physics, actor_body).Result).position};
  }

  struct placeResult { bool Result = false; };
  /// The drone on the ground at the start: the first spot round the origin that is no building, on what is there.
  placeResult place() {
    Vector at = Vector{0.0, 0.0, 0.0};
    bool found = false;
    i32 ring = 0;
    i32 n = 0;
    i32 k = 0;
    f32 a = 0.0f;
    f32 r = 0.0f;
    Vector p = Vector{0.0, 0.0, 0.0};
    f32 ground = 0.0f;
    the_RayResult_ g = the_RayResult_{false, 0.0f, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    physics_Body actor_body = physics_Body{0};
    Vector rot = Vector{0.0, 0.0, 0.0};
    bool teleport = false;
    bool set_actor_velocity = false;
    at = Vector{0.0, 0.0, 0.0};
    found = false;
    ring = 0;
    while (loop(logic_and(math_compare<"<", i32>(ring, 40), logic_not(found)))) {
      n = kern::formula<i32, i32>([](auto ring, auto a1) { return ((ring == 0) ? a1 : (ring * 8)); }, ring, 1);
      k = 0;
      while (loop(logic_and(math_compare<"<", i32>(k, n), logic_not(found)))) {
        a = kern::formula<i32, f32, i32>([](auto k, auto kPi, auto n) { return (((num_cast<f32, i32>(k) * 2.0f) * kPi) / num_cast<f32, i32>(n)); }, k, kPi, n);
        r = kern::formula<i32>([](auto ring) { return (num_cast<f32, i32>(ring) * 5.0f); }, ring);
        p = vector_make(kern::formula<f32, f32>([](auto r, auto a) { return (r * cos(a)); }, r, a), kern::formula<f32, f32>([](auto r, auto a) { return (r * sin(a)); }, r, a), 0.0f);
        if (logic_not(city_stream_cpp::CityStream_building_at(city_, p).Result)) {
          at = vector_make(vector_break(p).x, vector_break(p).y, 0.0f);
          found = true;
        }
        k += 1;
      }
      ring += 1;
    }
    city_stream_cpp::CityStream_collide(city_, at, true);
    ground = city_stream_cpp::CityStream_ground_height(city_, vector_break(at).x, vector_break(at).y).Result;
    engine_game_world_physics(n4194328, world_);
    physics = n4194328.result;
    actor_body = engine_game_world_actor_body(world_, drone_actor_);
    g = common::cast(physics, vector_make(vector_break(at).x, vector_break(at).y, kern::formula<f32>([](auto ground) { return (ground + 60.0f); }, ground)), Vector{0.0, 0.0, -1.0}, 80.0f, actor_body, 18446744073709551615u).Result;
    at = vector_make(vector_break(at).x, vector_break(at).y, kern::formula<bool, f32, f32>([](auto hit, auto z, auto ground) { return ((hit ? z : ground) + 0.0225f); }, g.hit, vector_break(g.point).z, ground));
    engine_game_world_set_alive(world_, drone_actor_, true);
    rot = Vector{0.0, 0.0, 180.0};
    engine_game_world_teleport(n4194329, world_, drone_actor_, at, list_make<Vector, "1">(rot));
    teleport = n4194329.result;
    engine_game_world_set_actor_velocity(n4194330, world_, drone_actor_, Vector{0.0, 0.0, 0.0});
    set_actor_velocity = n4194330.result;
    drone_cpp::Drone_reset(drone_);
    sticks_ = struct_make<DroneSticks>(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.2f, false, true, false);
    sticks_.clearance = settings::Settings_get(settings_, Opt::drone_hover_agl).Result;
    landing_ = false;
    height_held_ = false;
    accumulator_ = 0.0;
    rig_init_ = false;
    focus_prev_valid_ = false;
    placed_ = true;
    engine_game_world_set_paused(world_, false);
    engine_engine_set_status(engine_, "");
    toast_ = logic_select<Text>(city_stream::CityStream_offline(city_).Result, "", "Willkommen in SpawnCity");
    toast_age_ = kern::formula<bool, i32, i32>([](auto the_result, auto a99, auto a0) { return num_cast<f32, i32>((the_result ? a99 : a0)); }, city_stream::CityStream_offline(city_).Result, 99, 0);
    return {true};
  }

  /// RESET (game_reset.mjs): the drone back at the start, on the ground, disarmed.
  void reset() {
    if (logic_not(placed_)) {
      return;
    }
    n4194331 = place();
    resets_ = kern::formula<u32>([](auto resets_) { return (resets_ + 1); }, resets_);
    toast_ = "Zurueck zum Start";
    toast_age_ = 0.0f;
  }

  /// The world moves to another place: the city streams there and the drone starts anew.
  void move_world(f64 lon = 0.0, f64 lat = 0.0) {
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    f64 arg_number = 0.0;
    f64 arg_number_2 = 0.0;
    f64 arg_number_3 = 0.0;
    bool both = false;
    f64 arg = 0.0;
    f64 arg_2 = 0.0;
    f64 arg_number_4 = 0.0;
    lon_ = lon;
    lat_ = lat;
    city_stream_cpp::CityStream_clear(city_);
    renderer = engine_engine_renderer(engine_);
    engine_game_world_physics(n4194332, world_);
    physics = n4194332.result;
    arg = lon_;
    arg_2 = lat_;
    arg_number = engine_engine_arg_number(engine_, "dem", 1.0);
    both = math_compare<">", f64>(arg_number, 0.0);
    if (both) {
      arg_number_3 = engine_engine_arg_number(engine_, "offline", 0.0);
      both = math_compare<"<=", f64>(arg_number_3, 0.0);
    }
    city_stream_cpp::CityStream_start(city_, renderer, physics, arg, arg_2, both);
    arg_number_4 = engine_engine_arg_number(engine_, "offline", 0.0);
    if (math_compare<">", f64>(arg_number_4, 0.0)) {
      city_stream_cpp::CityStream_go_offline(city_);
    }
    engine_game_world_set_alive(world_, drone_actor_, false);
    engine_game_world_set_paused(world_, true);
    placed_ = false;
    moves_ = kern::formula<u32>([](auto moves_) { return (moves_ + 1); }, moves_);
    toast_ = "Neuer Startort - die Stadt wird geladen";
    toast_age_ = 0.0f;
  }

  void apply_settings() {
    bool has_arg = false;
    bool has_arg_2 = false;
    bool both = false;
    f64 arg_number = 0.0;
    both = logic_and(math_compare<"==", u32>(applied_, 0), object_valid<kern::Engine>(engine_));
    if (both) {
      has_arg_2 = engine_engine_has_arg(engine_, "fpvtilt");
      both = has_arg_2;
    }
    if (both) {
      arg_number = engine_engine_arg_number(engine_, "fpvtilt", -15.0);
      Settings_set(settings_, settings_table_, Opt::cam_fpv_tilt, num_cast<f32, f64>(arg_number));
    }
    applied_ = settings::Settings_version(settings_).Result;
  }

  /// The sticks from the keyboard, the mouse, the gamepad and the touch controls (CONTROLS.md, the drone).
  void read_input(Object<kern::Engine> e = Object<kern::Engine>{}, f32 dt = 0.0f) {
    Object<kern::Engine> e_ = Object<kern::Engine>{};
    Object<kern::Input> in = Object<kern::Input>{};
    Object<kern::Input> input = Object<kern::Input>{};
    bool button_pressed = false;
    bool button_pressed_2 = false;
    bool button_pressed_3 = false;
    bool button_pressed_4 = false;
    bool either = false;
    bool both = false;
    TouchLayout layout = TouchLayout{TouchMode::off, true, "", false, false, false, 0, -1.0f, false, false, false, true, false, false, "FPV", false, false, false, false, false, false};
    Vector left = Vector{0.0, 0.0, 0.0};
    Vector right = Vector{0.0, 0.0, 0.0};
    controls_DroneAxes a = controls_DroneAxes{0.0f, 0.0f, 0.0f, 0.0f};
    f32 axis = 0.0f;
    bool down = false;
    bool down_2 = false;
    bool down_3 = false;
    bool down_4 = false;
    bool either_2 = false;
    bool pressed = false;
    Text a_2 = "";
    Text a_3 = "";
    bool either_3 = false;
    bool arm_key = false;
    bool pressed_2 = false;
    Text a_4 = "";
    Text a_5 = "";
    bool either_4 = false;
    f32 agl = 0.0f;
    Text a_6 = "";
    bool have_agl = false;
    f32 rate = 0.0f;
    e_ = e;
    engine_engine_input(n4194334, e_);
    input = n4194334.result;
    in = input;
    if (touch_mode_) {
      if (locked_) {
        engine_engine_request_pointer_lock(e_, false);
      }
    } else {
      both = logic_not(locked_);
      if (both) {
        button_pressed_2 = engine_input_button_pressed(in, 0);
        either = button_pressed_2;
        if (logic_not(either)) {
          button_pressed_4 = engine_input_button_pressed(in, 1);
          either = button_pressed_4;
        }
        both = either;
      }
      if (both) {
        engine_engine_request_pointer_lock(e_, true);
      }
    }
    layout = TouchLayout{TouchMode::off, true, "", false, false, false, 0, -1.0f, false, false, false, true, false, false, "FPV", false, false, false, false, false, false};
    if (touch_mode_) {
      layout.mode = TouchMode::drone;
    }
    layout.armed = drone_.tele_.armed;
    layout.camera = logic_select<Text>(math_compare<"==", i32>(cam_mode_, 0), "FPV", logic_select<Text>(math_compare<"==", i32>(cam_mode_, 1), "FOLLOW", "THIRD"));
    touch_play_cpp::TouchPlay_set_layout(touch_, layout);
    left = vector_make(kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return (the_result - the_result_2); }, spiel_cpp::key(in, Key::d).Result, spiel_cpp::key(in, Key::a).Result), kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return (the_result - the_result_2); }, spiel_cpp::key(in, Key::s).Result, spiel_cpp::key(in, Key::w).Result), 0.0f);
    if (touch_mode_) {
      left = vector_add(left, touch_play::TouchPlay_move(touch_).Result);
    }
    if (pad.connected) {
      n4194335 = gamepad_axis(pad.lx, 0.14f);
      n4194336 = gamepad_axis(kern::formula<f32>([](auto ly) { return (-ly); }, pad.ly), 0.14f);
      left = vector_add(left, vector_make(n4194335.Result, n4194336.Result, 0.0f));
    }
    right = vector_make(kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return (the_result - the_result_2); }, spiel_cpp::key(in, Key::right).Result, spiel_cpp::key(in, Key::left).Result), kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return (the_result - the_result_2); }, spiel_cpp::key(in, Key::down).Result, spiel_cpp::key(in, Key::up).Result), 0.0f);
    RateStick_step(mouse_, mouse_px_, dt);
    mouse_px_ = Vector{0.0, 0.0, 0.0};
    right = vector_add(right, mouse_.rate);
    if (touch_mode_) {
      right = vector_add(right, touch_play::TouchPlay_look(touch_).Result);
    }
    if (pad.connected) {
      n4194337 = gamepad_axis(pad.rx, 0.14f);
      n4194338 = gamepad_axis(kern::formula<f32>([](auto ry) { return (-ry); }, pad.ry), 0.14f);
      right = vector_add(right, vector_make(n4194337.Result, n4194338.Result, 0.0f));
    }
    right = vector_make(common::clampf(vector_break(right).x, -1.0f, 1.0f).Result, common::clampf(vector_break(right).y, -1.0f, 1.0f).Result, 0.0f);
    a = controls_DroneAxes{0.0f, 0.0f, 0.0f, 0.0f};
    apply_game_stick_left(a, vector_break(left).x, vector_break(left).y, settings::Settings_get(settings_, Opt::drone_left_fineness).Result, settings::Settings_on(settings_, Opt::drone_invert_roll).Result, settings::Settings_get(settings_, Opt::drone_max_speed_kmh).Result);
    apply_game_stick_right(a, vector_break(right).x, vector_break(right).y, settings::Settings_get(settings_, Opt::drone_right_fineness).Result, settings::Settings_on(settings_, Opt::drone_invert_yaw).Result, settings::Settings_on(settings_, Opt::drone_invert_pitch).Result);
    sticks_.roll = a.roll;
    sticks_.pitch = a.pitch;
    sticks_.yaw = a.yaw;
    sticks_.body_pitch = a.body_pitch;
    down_3 = engine_input_down(in, Key::shift_left);
    either_2 = down_3;
    if (logic_not(either_2)) {
      down_4 = engine_input_down(in, Key::shift_right);
      either_2 = down_4;
    }
    axis = kern::formula<f32, bool, f32, f32>([](auto the_result, auto either_2, auto a0f, auto a0f_2) { return (the_result - (either_2 ? a0f : a0f_2)); }, spiel_cpp::key(in, Key::space).Result, either_2, 1.0f, 0.0f);
    if (pad.connected) {
      n4194339 = height_axis(pad.rt, pad.lt);
      axis += n4194339.Result;
    }
    if (touch_mode_) {
      axis += touch_play::TouchPlay_clearance(touch_).Result;
    }
    axis = common::clampf(axis, -1.0f, 1.0f).Result;
    pressed = engine_input_pressed(in, Key::c);
    either_3 = logic_or(pressed, logic_and(pad.connected, spiel_cpp::pad_hit(pad, num_cast<i32, i64>(enum_index<PadButtonIndex>(PadButtonIndex::kPadY))).Result));
    if (logic_not(either_3)) {
      a_3 = "cam";
      n4194340 = touch_play_cpp::TouchPlay_take_pressed(touch_, a_3);
      either_3 = n4194340.Result;
    }
    if (either_3) {
      cam_mode_ = kern::formula<i32>([](auto cam_mode_) { return ((cam_mode_ + 1) % 3); }, cam_mode_);
      rig_init_ = false;
    }
    pressed_2 = engine_input_pressed(in, Key::r);
    either_4 = logic_or(pressed_2, logic_and(pad.connected, spiel_cpp::pad_hit(pad, num_cast<i32, i64>(enum_index<PadButtonIndex>(PadButtonIndex::kPadA))).Result));
    if (logic_not(either_4)) {
      a_5 = "arm";
      n4194341 = touch_play_cpp::TouchPlay_take_pressed(touch_, a_5);
      either_4 = n4194341.Result;
    }
    arm_key = either_4;
    agl = kern::formula<f32, f32>([](auto agl, auto a0f) { return ((agl >= 0.0f) ? agl : a0f); }, drone_.tele_.agl, 0.0f);
    if (arm_key) {
      if (logic_and(logic_not(drone_.tele_.armed), logic_not(sticks_.arm))) {
        sticks_.arm = true;
        landing_ = false;
      } else if (logic_and(logic_and(drone_.tele_.armed, math_compare<">", f32>(agl, 0.7f)), logic_not(landing_))) {
        landing_ = true;
      } else if (landing_) {
        landing_ = false;
      } else {
        sticks_.arm = false;
        landing_ = false;
      }
    }
    a_6 = "stop";
    n4194342 = touch_play_cpp::TouchPlay_take_pressed(touch_, a_6);
    sticks_.kill = logic_or(n4194342.Result, logic_and(pad.connected, spiel_cpp::pad_hit(pad, num_cast<i32, i64>(enum_index<PadButtonIndex>(PadButtonIndex::kPadB))).Result));
    if (sticks_.kill) {
      sticks_.arm = false;
      landing_ = false;
    }
    if (logic_and(logic_and(logic_and(logic_and(logic_and(math_compare<">", f32>(axis, 0.3f), logic_not(drone_.tele_.armed)), logic_not(drone_.tele_.calibrating)), logic_not(drone_.tele_.fault)), logic_not(landing_)), logic_not(sticks_.kill))) {
      sticks_.arm = true;
    }
    if (logic_and(drone_.tele_.fault, logic_not(drone_.tele_.armed))) {
      sticks_.arm = false;
    }
    if (landing_) {
      axis = kern::formula<f32, f32>([](auto agl, auto a0f) { return (-((agl > 6.0f) ? a0f : fmax(0.35f, (agl / 6.0f)))); }, agl, 1.0f);
      if (logic_and(math_compare<"<=", f32>(agl, 0.7f), math_compare<"<", f32>(kern::formula<f32>([](auto vz) { return abs(vz); }, drone_.tele_.vz), 0.35f))) {
        landing_still_ = kern::formula<f32, f32>([](auto landing_still_, auto dt) { return (landing_still_ + dt); }, landing_still_, dt);
      } else {
        landing_still_ = 0.0f;
      }
      if (math_compare<">", f32>(landing_still_, 0.25f)) {
        sticks_.arm = false;
        landing_ = false;
        landing_still_ = 0.0f;
      }
    }
    have_agl = math_compare<">=", f32>(drone_.tele_.agl, 0.0f);
    if (math_compare<">", f32>(kern::formula<f32>([](auto axis) { return abs(axis); }, axis), 0.0001f)) {
      height_held_ = true;
      rate = drone_cpp::clearance_rate(axis).Result;
      sticks_.clearance = kern::formula<bool, f32, f32>([](auto have_agl, auto the_result, auto the_result_2) { return (have_agl ? the_result : the_result_2); }, have_agl, common::clampf(kern::formula<f32, f32>([](auto agl, auto the_result) { return (agl + the_result); }, agl, drone_cpp::clearance_lead(rate).Result), 0.5f, 120.0f).Result, common::clampf(kern::formula<f32, f32, f32>([](auto clearance, auto rate, auto the_result) { return (clearance + (rate * the_result)); }, sticks_.clearance, rate, common::clampf(dt, 0.0f, 0.05f).Result), 0.5f, 120.0f).Result);
    } else if (height_held_) {
      height_held_ = false;
      if (have_agl) {
        sticks_.clearance = common::clampf(kern::formula<f32, f32>([](auto agl, auto the_result) { return (agl + the_result); }, agl, drone_cpp::clearance_lead(drone_.tele_.vz).Result), 0.5f, 120.0f).Result;
      }
    }
  }

  /// The drone's force model in every fixed step of the world (Box3D clears forces after each step): the game takes the steps itself - one GameWorld::step_once, then the flight controller and the motors - as many as the frame's time.
  void fly(f32 dt = 0.0f) {
    f64 h = 0.0;
    u32 n = 0;
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    physics_Body actor_body = physics_Body{0};
    h = kern::formula<f64>([](auto kStepHz) { return (1.0 / kStepHz); }, kStepHz);
    accumulator_ = kern::formula<f64, f64>([](auto accumulator_, auto dt) { return (accumulator_ + (dt < 0.0 ? 0.0 : (0.25 < dt ? 0.25 : dt))); }, accumulator_, num_cast<f64, f32>(dt));
    n = 0;
    while (loop(logic_and(math_compare<">=", f64>(accumulator_, h), math_compare<"<", u32>(n, kMaxSteps)))) {
      n4194343 = drone_position();
      city_stream_cpp::CityStream_collide(city_, n4194343.Result, false);
      engine_game_world_step_once(world_, struct_make<game_PlayInput>(0.0f, 0.0f, 0.0f, false, false, false, false, 0, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, false, 0.0f, false, false, false, false, false, 0.0f, 0.0f, false, 0.0f));
      engine_game_world_physics(n4194344, world_);
      physics = n4194344.result;
      actor_body = engine_game_world_actor_body(world_, drone_actor_);
      drone_cpp::Drone_step(drone_, sticks_, physics, actor_body, num_cast<f32, f64>(h));
      accumulator_ = kern::formula<f64, f64>([](auto accumulator_, auto h) { return (accumulator_ - h); }, accumulator_, h);
      steps_ = kern::formula<u64>([](auto steps_) { return (steps_ + 1); }, steps_);
      n += 1;
    }
    if (math_compare<">=", f64>(accumulator_, h)) {
      accumulator_ = kern::formula<f64, f64>([](auto accumulator_, auto h) { return fmod(accumulator_, h); }, accumulator_, h);
    }
  }

  void update_camera(Object<kern::Engine> arg1 = Object<kern::Engine>{}, f32 dt = 0.0f) {
    Camera c = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471976f, 0.05f, 1800.0f, false, 20.0f, false, Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}};
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    physics_Body actor_body = physics_Body{0};
    Vector pos = Vector{0.0, 0.0, 0.0};
    Vector translation = Vector{0.0, 0.0, 0.0};
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Vector rotate = Vector{0.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 0.0};
    Vector rotate_2 = Vector{0.0, 0.0, 0.0};
    f32 fov = 0.0f;
    f32 tilt = 0.0f;
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector u = Vector{0.0, 0.0, 0.0};
    Vector heading = Vector{0.0, 0.0, 0.0};
    f32 hl = 0.0f;
    bool third = false;
    f32 pos_rate = 0.0f;
    f32 vel_rate = 0.0f;
    f32 head_rate = 0.0f;
    f32 max_lag = 0.0f;
    Vector predicted = Vector{0.0, 0.0, 0.0};
    Vector lag = Vector{0.0, 0.0, 0.0};
    f32 l = 0.0f;
    f32 length = 0.0f;
    f32 ha = 0.0f;
    f32 hb = 0.0f;
    f32 hd_angle = 0.0f;
    f32 back = 0.0f;
    f32 upm = 0.0f;
    f32 ahead = 0.0f;
    f32 look_up = 0.0f;
    Vector hd = Vector{0.0, 0.0, 0.0};
    Vector want = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    physics_RayHit hit = physics_RayHit{false, physics_Shape{0}, physics_Body{0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f};
    Object<kern::physics::World> physics_2 = Object<kern::physics::World>{};
    physics_Body actor_body_2 = physics_Body{0};
    physics_RayHit cast_sphere = physics_RayHit{false, physics_Shape{0}, physics_Body{0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f};
    c = cam_;
    c.near = 0.05f;
    c.far = 2400.0f;
    if (logic_not(placed_)) {
      c.eye = vector_make(0.0f, 0.0f, kern::formula<f32>([](auto the_result) { return (the_result + 1.68f); }, city_stream_cpp::CityStream_ground_height(city_, 0.0f, 0.0f).Result));
      c.target = vector_add(c.eye, Vector{10.0, 0.0, 0.0});
      c.up = Vector{0.0, 0.0, 1.0};
      c.fov = kern::formula<f32>([](auto kDeg) { return (50.0f * kDeg); }, kDeg);
      cam_ = c;
      return;
    }
    engine_game_world_physics(n4194346, world_);
    physics = n4194346.result;
    actor_body = engine_game_world_actor_body(world_, drone_actor_);
    s = common::read_body(physics, actor_body).Result;
    n4194347 = drone_matrix();
    translation = engine_mat4_translation(n4194347.Result);
    pos = translation;
    rotate = engine_quat_rotate(s.rotation, Vector{-1.0, 0.0, 0.0});
    fwd = rotate;
    rotate_2 = engine_quat_rotate(s.rotation, Vector{0.0, 0.0, 1.0});
    up = rotate_2;
    fov = settings::Settings_get(settings_, Opt::cam_fov).Result;
    if (math_compare<"==", i32>(cam_mode_, 0)) {
      tilt = kern::formula<f32, f32>([](auto the_result, auto kDeg) { return (the_result * kDeg); }, settings::Settings_get(settings_, Opt::cam_fpv_tilt).Result, kDeg);
      f = vector_add(vector_scale(fwd, kern::formula<f32>([](auto tilt) { return cos(tilt); }, tilt)), vector_scale(up, kern::formula<f32>([](auto tilt) { return sin(tilt); }, tilt)));
      u = vector_sub(vector_scale(up, kern::formula<f32>([](auto tilt) { return cos(tilt); }, tilt)), vector_scale(fwd, kern::formula<f32>([](auto tilt) { return sin(tilt); }, tilt)));
      c.eye = vector_add(vector_add(pos, vector_scale(fwd, 0.102f)), vector_scale(up, 0.073f));
      c.target = vector_add(c.eye, vector_scale(f, 4.0f));
      c.up = u;
      c.fov = kern::formula<f32, f32>([](auto fov, auto kDeg) { return (fov * kDeg); }, fov, kDeg);
      c.near = 0.02f;
      cam_ = c;
      return;
    }
    heading = vector_make(vector_break(fwd).x, vector_break(fwd).y, 0.0f);
    hl = kern::formula<f32, f32>([](auto x, auto y) { return sqrt(((x * x) + (y * y))); }, vector_break(heading).x, vector_break(heading).y);
    heading = logic_select<Vector>(math_compare<">", f32>(hl, 0.2f), vector_scale(heading, kern::formula<f32>([](auto hl) { return (1.0f / hl); }, hl)), cam_heading_);
    third = math_compare<"==", i32>(cam_mode_, 2);
    pos_rate = kern::formula<bool, f32, f32>([](auto third, auto a6f, auto a8f) { return (third ? a6f : a8f); }, third, 3.6f, 4.8f);
    vel_rate = kern::formula<bool, f32, f32>([](auto third, auto a2f, auto a0f) { return (third ? a2f : a0f); }, third, 5.2f, 7.0f);
    head_rate = kern::formula<bool, f32, f32>([](auto third, auto a8f, auto a5f) { return (third ? a8f : a5f); }, third, 3.8f, 7.5f);
    max_lag = kern::formula<bool, f32, f32>([](auto third, auto a36f, auto a18f) { return (third ? a36f : a18f); }, third, 0.36f, 0.18f);
    if (logic_not(rig_init_)) {
      cam_anchor_ = pos;
      cam_vel_ = s.velocity;
      cam_heading_ = heading;
      rig_init_ = true;
    }
    cam_vel_ = vector_add(cam_vel_, vector_scale(vector_sub(s.velocity, cam_vel_), common::damp_alpha(vel_rate, dt).Result));
    predicted = vector_add(cam_anchor_, vector_scale(cam_vel_, dt));
    cam_anchor_ = vector_add(predicted, vector_scale(vector_sub(pos, predicted), common::damp_alpha(pos_rate, dt).Result));
    lag = vector_sub(pos, cam_anchor_);
    length = engine_length_2(lag);
    l = length;
    if (math_compare<">", f32>(l, max_lag)) {
      cam_anchor_ = vector_sub(pos, vector_scale(lag, kern::formula<f32, f32>([](auto max_lag, auto l) { return (max_lag / l); }, max_lag, l)));
    }
    ha = kern::formula<f32, f32>([](auto y, auto x) { return atan2(y, x); }, vector_break(cam_heading_).y, vector_break(cam_heading_).x);
    hb = kern::formula<f32, f32>([](auto y, auto x) { return atan2(y, x); }, vector_break(heading).y, vector_break(heading).x);
    hd_angle = kern::formula<f32, f32, f32>([](auto ha, auto the_result, auto the_result_2) { return (ha + (the_result * the_result_2)); }, ha, common::wrap_pi(kern::formula<f32, f32>([](auto hb, auto ha) { return (hb - ha); }, hb, ha)).Result, common::damp_alpha(head_rate, dt).Result);
    cam_heading_ = vector_make(kern::formula<f32>([](auto hd_angle) { return cos(hd_angle); }, hd_angle), kern::formula<f32>([](auto hd_angle) { return sin(hd_angle); }, hd_angle), 0.0f);
    back = kern::formula<bool, f32, f32>([](auto third, auto a3583128f, auto a65f) { return (third ? a3583128f : a65f); }, third, 1.3583128f, 1.65f);
    upm = kern::formula<bool, f32, f32>([](auto third, auto a6338793f, auto a78f) { return (third ? a6338793f : a78f); }, third, 0.6338793f, 0.78f);
    ahead = kern::formula<bool, f32, f32>([](auto third, auto a55f, auto a38f) { return (third ? a55f : a38f); }, third, 0.55f, 0.38f);
    look_up = kern::formula<bool, f32, f32>([](auto third, auto a18f, auto a1f) { return (third ? a18f : a1f); }, third, 0.18f, 0.1f);
    hd = vector_make(vector_break(cam_heading_).x, vector_break(cam_heading_).y, 0.0f);
    want = vector_add(vector_sub(cam_anchor_, vector_scale(hd, back)), vector_make(0.0f, 0.0f, upm));
    d = vector_sub(want, cam_anchor_);
    engine_game_world_physics(n4194348, world_);
    physics_2 = n4194348.result;
    actor_body_2 = engine_game_world_actor_body(world_, drone_actor_);
    cast_sphere = engine_world_cast_sphere(physics_2, cam_anchor_, 0.17f, d, engine_physics_physics::QueryFilter_new(18446744073709551615u, 18446744073709551615u).Result, actor_body_2);
    hit = cast_sphere;
    c.eye = logic_select<Vector>(hit.hit, vector_add(cam_anchor_, vector_scale(d, kern::formula<f32>([](auto fraction) { return fmax(0.0f, (fraction - 0.02f)); }, hit.fraction))), want);
    c.target = vector_add(vector_add(cam_anchor_, vector_scale(hd, ahead)), vector_make(0.0f, 0.0f, look_up));
    c.up = Vector{0.0, 0.0, 1.0};
    c.fov = kern::formula<bool, f32, f32, f32>([](auto third, auto the_result, auto the_result_2, auto kDeg) { return ((third ? the_result : the_result_2) * kDeg); }, third, common::clampf(kern::formula<f32>([](auto fov) { return ((62.0f * fov) / 105.0f); }, fov), 35.0f, 100.0f).Result, common::clampf(kern::formula<f32>([](auto fov) { return ((52.0f * fov) / 105.0f); }, fov), 30.0f, 90.0f).Result, kDeg);
    c.near = 0.06f;
    cam_ = c;
  }

  void update_sound(f32 dt = 0.0f) {
    bool ready = false;
    bool prepare = false;
    SoundState s = SoundState{EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, 0.0f, 1.0f, 1.0f};
    f32 w = 0.0f;
    f32 o = 0.0f;
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    f32 length = 0.0f;
    ready = engine_mixer_ready(mixer_);
    if (logic_or(logic_not(ready), logic_not(object_valid<kern::game::GameWorld>(world_)))) {
      return;
    }
    engine_sound_bank_prepare(n4194352, bank_, 3.0);
    prepare = n4194352.result;
    engine_game_audio_update(game_audio_, world_, cam_, dt);
    engine_mixer_bus(n4194353, mixer_, audio_Bus::master);
    n4194353.result.muted = logic_or(logic_not(settings::Settings_on(settings_, Opt::audio_on).Result), paused_);
    engine_mixer_bus(n4194354, mixer_, audio_Bus::sfx);
    n4194354.result.volume = kern::formula<f32>([](auto the_result) { return (the_result / 72.0f); }, settings::Settings_get(settings_, Opt::audio_fx).Result);
    engine_mixer_bus(n4194355, mixer_, audio_Bus::ambience);
    n4194355.result.volume = kern::formula<f32>([](auto the_result) { return (the_result / 46.0f); }, settings::Settings_get(settings_, Opt::audio_ambient).Result);
    s = SoundState{EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, 0.0f, 1.0f, 1.0f};
    s.drone_mix = kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return ((the_result / 50.0f) / fmax(0.01f, (the_result_2 / 72.0f))); }, settings::Settings_get(settings_, Opt::audio_drone).Result, settings::Settings_get(settings_, Opt::audio_fx).Result);
    if (placed_) {
      w = 0.0f;
      for (auto it4194356 = flow_for(n4194356, 0, math_sub<i64>(list_length<f32>(drone_.tele_.omega), 1)); it4194356.next();) {
        o = drone_.tele_.omega[n4194356.index];
        w += o;
      }
      w = kern::formula<f32>([](auto w) { return (w / 4.0f); }, w);
      s.drone.active = math_compare<">", f32>(w, 5.0f);
      n4194357 = drone_position();
      s.drone.position = n4194357.Result;
      the_actor_velocity = engine_game_world_actor_velocity(world_, drone_actor_);
      s.drone.velocity = the_actor_velocity;
      s.drone.rate = common::clampf(kern::formula<f32>([](auto w) { return (w / 3600.0f); }, w), 0.0f, 1.2f).Result;
      s.drone.load = common::clampf(kern::formula<f32>([](auto current_a) { return (current_a / 120.0f); }, drone_.tele_.current_a), 0.0f, 1.0f).Result;
      length = engine_length_2(s.drone.velocity);
      s.wind = length;
    }
    sound_fx_cpp::SoundFx_update(fx_, s, dt);
  }

  KERN_OVERRIDE void update(Object<kern::Engine> e = Object<kern::Engine>{}, Frame f = Frame{0.0, 0.0f, 0}) {
    Object<kern::Engine> e_ = Object<kern::Engine>{};
    f32 dt = 0.0f;
    Text buf = "";
    Vector focus = Vector{0.0, 0.0, 0.0};
    Vector v = Vector{0.0, 0.0, 0.0};
    f32 l = 0.0f;
    f32 length = 0.0f;
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    f32 apply_lighting = 0.0f;
    Fixed<Vector, 4> motors = Fixed<Vector, 4>{};
    Fixed<f32, 4> angles = Fixed<f32, 4>{};
    Mat4 a = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    e_ = e;
    dt = f.dt;
    time_ = num_cast<f32, f64>(f.time);
    if (math_compare<">", f32>(dt, 0.0f)) {
      fps_ = kern::formula<f32, f32>([](auto fps_, auto dt) { return ((fps_ * 0.95f) + ((1.0f / dt) * 0.05f)); }, fps_, dt);
    }
    toast_age_ = kern::formula<f32, f32>([](auto toast_age_, auto dt) { return (toast_age_ + dt); }, toast_age_, dt);
    apply_settings();
    if (logic_not(placed_)) {
      city_stream_cpp::CityStream_update(city_, Vector{0.0, 0.0, 0.0}, 24.0);
      list_clear(ground_changes_);
      city_stream::CityStream_take_ground_changes(city_, ground_changes_);
      if (city_stream_cpp::CityStream_loaded_at(city_, Vector{0.0, 0.0, 0.0}).Result) {
        n4194333 = place();
      } else {
        buf = "";
        buf = text_format<"SpawnCity: %s ... %u Gebaeude">(127, logic_select<Text>(city_stream::CityStream_offline(city_).Result, "Ersatzstadt wird gebaut", "die Stadt wird geladen"), num_cast<i64, u32>(city_.stats_.buildings));
        engine_engine_set_status(e_, buf);
      }
    }
    if (logic_and(placed_, logic_not(paused_))) {
      read_input(e_, dt);
      fly(dt);
      n4194345 = drone_position();
      focus = n4194345.Result;
      city_.anchor = focus;
      if (logic_and(math_compare<">", f32>(dt, 0.0f), focus_prev_valid_)) {
        v = vector_scale(vector_sub(focus, focus_prev_), kern::formula<f32>([](auto dt) { return (1.0f / dt); }, dt));
        v = vector_make(vector_break(v).x, vector_break(v).y, 0.0f);
        length = engine_length_2(v);
        l = length;
        city_.terrain_.lead = logic_select<Vector>(math_compare<">", f32>(l, 15.0f), vector_scale(v, kern::formula<f32>([](auto l) { return (fmin((l * 12.0f), 2500.0f) / l); }, l)), Vector{0.0, 0.0, 0.0});
        city_.view_lead = logic_select<Vector>(math_compare<">", f32>(l, 6.0f), vector_scale(v, kern::formula<f32>([](auto l) { return (fmin((l * 4.0f), 420.0f) / l); }, l)), Vector{0.0, 0.0, 0.0});
      }
      focus_prev_ = focus;
      focus_prev_valid_ = true;
      city_stream_cpp::CityStream_update(city_, focus, kern::formula<bool, i32, i32>([](auto mobile_, auto a3, auto a4) { return num_cast<f64, i32>((mobile_ ? a3 : a4)); }, mobile_, 3, 4));
      list_clear(ground_changes_);
      city_stream::CityStream_take_ground_changes(city_, ground_changes_);
    }
    pad_was_ = pad.buttons;
    update_camera(e_, dt);
    renderer = engine_engine_renderer(e_);
    engine_renderer_set_camera(renderer, cam_);
    engine_game_world_set_view(world_, cam_);
    engine_game_world_apply_lighting(n4194349, world_, cam_.eye);
    apply_lighting = n4194349.result;
    engine_game_world_present(world_, kern::formula<f64>([](auto time) { return (time * 1000.0); }, f.time), cam_.eye);
    if (placed_) {
      motors = List<Vector>{};
      angles = List<f32>{};
      for (auto it4194350 = flow_for(n4194350, 0, 3); it4194350.next();) {
        motors[num_cast<i64, i32>(num_cast<i32, i64>(n4194350.index))] = drone::Drone_motor_position(drone_, num_cast<i32, i64>(n4194350.index)).Result;
        angles[num_cast<i64, i32>(num_cast<i32, i64>(n4194350.index))] = drone::Drone_rotor_angle(drone_, num_cast<i32, i64>(n4194350.index)).Result;
      }
      n4194351 = drone_matrix();
      a = n4194351.Result;
      drone_art_cpp::DroneArt_draw(art_, a, motors, angles, drone_.p_.prop_d, drone_.tele_.armed, math_compare<"==", i32>(cam_mode_, 0));
    } else {
      drone_art_cpp::DroneArt_hide(art_);
    }
    update_sound(dt);
  }

  void mouse_move(f32 dx = 0.0f, f32 dy = 0.0f) {
    if (logic_not(locked_)) {
      return;
    }
    mouse_px_ = vector_make(kern::formula<f32, f32>([](auto x, auto dx) { return (x + dx); }, vector_break(mouse_px_).x, dx), vector_break(mouse_px_).y, vector_break(mouse_px_).z);
    mouse_px_ = vector_make(vector_break(mouse_px_).x, kern::formula<f32, f32>([](auto y, auto dy) { return (y + dy); }, vector_break(mouse_px_).y, dy), vector_break(mouse_px_).z);
  }

  void set_touch_frame(f32 w = 0.0f, f32 h = 0.0f, f32 sl = 0.0f, f32 st = 0.0f, f32 sr = 0.0f, f32 sb = 0.0f, f32 arg7 = 0.0f) {
    touch_play_cpp::TouchPlay_set_frame(touch_, w, h, sl, st, sr, sb);
  }

  struct touchResult { bool Result = false; };
  touchResult touch(u32 type = 0, i32 id = 0, f32 x = 0.0f, f32 y = 0.0f) {
    if (math_compare<"==", u32>(type, 0)) {
      touch_mode_ = true;
    }
    n4194358 = TouchPlay_pointer(touch_, type, id, x, y, 0.0);
    return {n4194358.Result};
  }

  struct touch_view_jsonResult { Text Result = ""; };
  touch_view_jsonResult touch_view_json() {
    Text raw = "";
    Text out = "";
    i8 c = 0;
    if (logic_not(touch_mode_)) {
      return {"null"};
    }
    n4194359 = touch_play_cpp::TouchPlay_view_json(touch_);
    raw = n4194359.Result;
    out = "";
    for (auto it4194360 = flow_for(n4194360, 0, math_sub<i64>(text_length(raw), 1)); it4194360.next();) {
      c = num_cast<i8, i64>(text_code_at(raw, n4194360.index));
      if (math_compare<"==", i32>(num_cast<i32, i8>(c), 10)) {
        out = text_append(out, "\\n", "");
      } else if (math_compare<"<", i32>(num_cast<i32, u8>(num_cast<u8, i8>(c)), 32)) {
        out = text_append(out, text_of_code(32), "");
      } else {
        out = text_append(out, text_of_code(num_cast<i64, i8>(c)), "");
      }
    }
    return {out};
  }

  struct audio_initResult { bool Result = false; };
  audio_initResult audio_init(u32 rate = 0) {
    bool ready = false;
    bool init = false;
    Object<kern::game::SoundBank> old = Object<kern::game::SoundBank>{};
    Object<kern::game::SoundBank> sound_bank = Object<kern::game::SoundBank>{};
    Object<kern::game::GameAudio> old_2 = Object<kern::game::GameAudio>{};
    Object<kern::game::GameAudio> game_audio = Object<kern::game::GameAudio>{};
    ready = engine_mixer_ready(mixer_);
    if (ready) {
      return {true};
    }
    engine_mixer_init(n4194361, mixer_, rate, 64);
    init = n4194361.result;
    if (logic_not(init)) {
      return {false};
    }
    old = bank_;
    engine_sound_bank_make(n4194362, mixer_);
    sound_bank = n4194362.result;
    bank_ = sound_bank;
    if (object_valid<kern::game::SoundBank>(old)) {
      engine_sound_bank_release(old);
    }
    old_2 = game_audio_;
    engine_game_audio_make(n4194363, mixer_, bank_);
    game_audio = n4194363.result;
    game_audio_ = game_audio;
    if (object_valid<kern::game::GameAudio>(old_2)) {
      engine_game_audio_release(old_2);
    }
    if (object_valid<kern::game::GameWorld>(world_)) {
      engine_game_audio_begin(game_audio_, world_);
    }
    n4194364 = sound_fx_cpp::SoundFx_init(fx_, mixer_);
    return {n4194364.Result};
  }

  struct audio_renderResult { List<f32> Result = List<f32>{}; };
  audio_renderResult audio_render(u32 frames = 0) {
    bool ready = false;
    i64 at = 0;
    i64 last = 0;
    list_resize(audio_out_, num_cast<i64, u64>(kern::formula<u32>([](auto frames) { return (num_cast<u64, u32>(frames) * 2); }, frames)), 0.0f);
    ready = engine_mixer_ready(mixer_);
    if (logic_and(ready, logic_not(web_audio_))) {
      at = 0;
      engine_mixer_render(n4194365, mixer_, frames);
      last = math_sub<i64>(list_length<f32>(n4194365.out), 1);
      for (auto it4194366 = flow_for(n4194366, 0, last); it4194366.next();) {
        audio_out_[math_add<i64>(at, n4194366.index)] = list_get<f32>(n4194365.out, n4194366.index);
      }
    } else {
      for (auto it4194367 = flow_for(n4194367, 0, math_sub<i64>(list_length<f32>(audio_out_), 1)); it4194367.next();) {
        audio_out_[n4194367.index] = 0.0f;
      }
    }
    return {audio_out_};
  }

  struct audio_frameResult { Text Result = ""; };
  /// What the original's sound modules read (web/audio.mjs, every frame): the listener, the drone's motors, the settings in the original's names (audio_settings.mjs normalizeAudioSettings). The other parts of the original's state (on foot, the car, jets, police, missiles) are empty: the small game has none of them.
  audio_frameResult audio_frame() {
    Text s = "";
    Text b = "";
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Vector normalize = Vector{0.0, 0.0, 0.0};
    Vector p = Vector{0.0, 0.0, 0.0};
    s = "{\"e\":[]";
    b = "";
    normalize = engine_normalize(vector_sub(cam_.target, cam_.eye), Vector{0.0, 1.0, 0.0});
    fwd = normalize;
    b = text_format<",\"paused\":%d,\"mode\":\"drone\",\"fwd\":[%.3f,%.3f,%.3f],\"L\":[%.2f,%.2f,%.2f]">(511, num_cast<i64, i32>(kern::formula<bool, i32, i32>([](auto paused_, auto a1, auto a0) { return (paused_ ? a1 : a0); }, paused_, 1, 0)), num_cast<f64, f32>(vector_break(fwd).x), num_cast<f64, f32>(vector_break(fwd).y), num_cast<f64, f32>(vector_break(fwd).z), num_cast<f64, f32>(vector_break(cam_.eye).x), num_cast<f64, f32>(vector_break(cam_.eye).y), num_cast<f64, f32>(vector_break(cam_.eye).z));
    s = text_append(s, b, "");
    b = text_format<",\"set\":{\"soundEnabled\":%d,\"musicEnabled\":%d,\"musicVolume\":%.0f,\"droneVolume\":%.0f,\"shotsVolume\":%.0f,\"fxVolume\":%.0f,\"footstepsVolume\":%.0f,\"vehicleVolume\":%.0f,\"ambientVolume\":%.0f}">(511, num_cast<i64, i32>(kern::formula<bool, i32, i32>([](auto the_result, auto a1, auto a0) { return (the_result ? a1 : a0); }, settings::Settings_on(settings_, Opt::audio_on).Result, 1, 0)), num_cast<i64, i32>(kern::formula<bool, i32, i32>([](auto the_result, auto a1, auto a0) { return (the_result ? a1 : a0); }, settings::Settings_on(settings_, Opt::music_on).Result, 1, 0)), num_cast<f64, f32>(settings::Settings_get(settings_, Opt::music_volume).Result), num_cast<f64, f32>(settings::Settings_get(settings_, Opt::audio_drone).Result), num_cast<f64, f32>(settings::Settings_get(settings_, Opt::audio_shots).Result), num_cast<f64, f32>(settings::Settings_get(settings_, Opt::audio_fx).Result), num_cast<f64, f32>(settings::Settings_get(settings_, Opt::audio_steps).Result), num_cast<f64, f32>(settings::Settings_get(settings_, Opt::audio_vehicle).Result), num_cast<f64, f32>(settings::Settings_get(settings_, Opt::audio_ambient).Result));
    s = text_append(s, b, "");
    n4194368 = drone_position();
    p = n4194368.Result;
    b = text_format<",\"walk\":{\"dead\":0,\"air\":0,\"p\":[%.2f,%.2f,%.2f]}">(511, num_cast<f64, f32>(vector_break(p).x), num_cast<f64, f32>(vector_break(p).y), num_cast<f64, f32>(vector_break(p).z));
    s = text_append(s, b, "");
    if (placed_) {
      b = text_format<",\"drone\":{\"st\":%u,\"arm\":%d,\"d\":%.4f,\"w\":[%.2f,%.2f,%.2f,%.2f],\"mt\":[%.5f,%.5f,%.5f,%.5f],\"pt\":[%.5f,%.5f,%.5f,%.5f],\"p\":[%.2f,%.2f,%.2f]}">(511, num_cast<i64, u32>(drone_.tele_.state), num_cast<i64, i32>(kern::formula<bool, i32, i32>([](auto arm, auto a1, auto a0) { return (arm ? a1 : a0); }, sticks_.arm, 1, 0)), num_cast<f64, f32>(drone_.p_.prop_d), num_cast<f64, f32>(drone_.tele_.omega[0]), num_cast<f64, f32>(drone_.tele_.omega[1]), num_cast<f64, f32>(drone_.tele_.omega[2]), num_cast<f64, f32>(drone_.tele_.omega[3]), num_cast<f64, f32>(drone_.tele_.motor_torque[0]), num_cast<f64, f32>(drone_.tele_.motor_torque[1]), num_cast<f64, f32>(drone_.tele_.motor_torque[2]), num_cast<f64, f32>(drone_.tele_.motor_torque[3]), num_cast<f64, f32>(drone_.tele_.prop_torque[0]), num_cast<f64, f32>(drone_.tele_.prop_torque[1]), num_cast<f64, f32>(drone_.tele_.prop_torque[2]), num_cast<f64, f32>(drone_.tele_.prop_torque[3]), num_cast<f64, f32>(vector_break(p).x), num_cast<f64, f32>(vector_break(p).y), num_cast<f64, f32>(vector_break(p).z));
      s = text_append(s, b, "");
    }
    s = text_append(s, ",\"jp\":[0,0],\"rj\":[],\"pc\":[],\"pd\":[],\"fj\":[],\"gm\":[],\"rk\":[],\"rp\":[],\"gps\":0}", "");
    return {s};
  }

  struct hud_jsonResult { Text Result = ""; };
  hud_jsonResult hud_json() {
    Text b = "";
    Text s = "";
    Vector pos = Vector{0.0, 0.0, 0.0};
    Vector vel = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    Vector the_actor_velocity_2 = Vector{0.0, 0.0, 0.0};
    Vector chosen = Vector{0.0, 0.0, 0.0};
    f32 agl = 0.0f;
    f32 length = 0.0f;
    f64 lon = 0.0;
    f64 lat = 0.0;
    Text hint = "";
    Text a = "";
    Text arg = "";
    Text arg_2 = "";
    Text arg_3 = "";
    Text arg_4 = "";
    b = "";
    s = "{";
    b = text_format<"\"mode\":\"drone\",\"placed\":%s,\"offline\":%s,\"realData\":%s">(639, logic_select<Text>(placed_, "true", "false"), logic_select<Text>(city_stream::CityStream_offline(city_).Result, "true", "false"), logic_select<Text>(city_stream::CityStream_has_real_data(city_).Result, "true", "false"));
    s = text_append(s, b, "");
    b = text_format<",\"tiles\":{\"ready\":%u,\"loading\":%u,\"failed\":%u,\"buildings\":%u,\"colliders\":%u">(639, num_cast<i64, u32>(city_.stats_.tiles_ready), num_cast<i64, u32>(city_.stats_.tiles_loading), num_cast<i64, u32>(city_.stats_.tiles_failed), num_cast<i64, u32>(city_.stats_.buildings), num_cast<i64, u32>(city_.stats_.colliders));
    s = text_append(s, b, "");
    b = text_format<",\"t\":{\"request\":%.0f,\"data\":%.0f,\"decoded\":%.0f,\"terrain\":%.0f,\"built\":%.0f,\"collision\":%.0f,\"work\":%.0f,\"worst\":%.1f,\"worstStep\":\"%s\",\"worstStepMs\":%.1f,\"rebuilds\":%u,\"relays\":%u,\"uploads\":%u,\"uploadKVerts\":%u}}">(639, city_.stats_.t_request, city_.stats_.t_data, city_.stats_.t_decoded, city_.stats_.t_terrain, city_.stats_.t_built, city_.stats_.t_collision, city_.stats_.work_ms, city_.stats_.worst_ms, city_.stats_.worst_step, city_.stats_.worst_step_ms, num_cast<i64, u32>(city_.stats_.rebuilds), num_cast<i64, u32>(city_.stats_.relays), num_cast<i64, u32>(city_.stats_.uploads), num_cast<i64, u32>(city_.stats_.upload_vertices));
    s = text_append(s, b, "");
    b = text_format<",\"fps\":%.0f,\"touch\":%s,\"steps\":%llu">(639, num_cast<f64, f32>(fps_), logic_select<Text>(touch_mode_, "true", "false"), num_cast<i64, u64>(steps_));
    s = text_append(s, b, "");
    n4194369 = drone_position();
    pos = n4194369.Result;
    if (placed_) {
      the_actor_velocity_2 = engine_game_world_actor_velocity(world_, drone_actor_);
      chosen = the_actor_velocity_2;
    } else {
      chosen = Vector{0.0, 0.0, 0.0};
    }
    vel = chosen;
    agl = kern::formula<f32, f32, f32>([](auto agl, auto z, auto the_result) { return ((agl >= 0.0f) ? agl : (z - the_result)); }, drone_.tele_.agl, vector_break(pos).z, city_stream_cpp::CityStream_ground_height(city_, vector_break(pos).x, vector_break(pos).y).Result);
    b = text_format<",\"drone\":{\"armed\":%s,\"calibrating\":%s,\"fault\":%s,\"game\":%s,\"landing\":%s,\"battery\":%.2f,\"volts\":%.2f,\"amps\":%.1f,\"clearance\":%.1f,\"cam\":%d,\"pulses\":[%u,%u,%u,%u]}">(639, logic_select<Text>(drone_.tele_.armed, "true", "false"), logic_select<Text>(drone_.tele_.calibrating, "true", "false"), logic_select<Text>(drone_.tele_.fault, "true", "false"), logic_select<Text>(sticks_.game_mode, "true", "false"), logic_select<Text>(landing_, "true", "false"), num_cast<f64, f32>(drone_.tele_.battery_soc), num_cast<f64, f32>(drone_.tele_.battery_v), num_cast<f64, f32>(drone_.tele_.current_a), num_cast<f64, f32>(sticks_.clearance), num_cast<i64, i32>(cam_mode_), num_cast<i64, i32>(num_cast<i32, u16>(drone_.tele_.pulses[0])), num_cast<i64, i32>(num_cast<i32, u16>(drone_.tele_.pulses[1])), num_cast<i64, i32>(num_cast<i32, u16>(drone_.tele_.pulses[2])), num_cast<i64, i32>(num_cast<i32, u16>(drone_.tele_.pulses[3])));
    s = text_append(s, b, "");
    length = engine_length_2(vel);
    b = text_format<",\"speed\":%.2f,\"vz\":%.2f,\"agl\":%.1f,\"alt\":%.1f,\"x\":%.2f,\"y\":%.2f">(639, num_cast<f64, f32>(length), num_cast<f64, f32>(vector_break(vel).z), num_cast<f64, f32>(agl), num_cast<f64, f32>(vector_break(pos).z), num_cast<f64, f32>(vector_break(pos).x), num_cast<f64, f32>(vector_break(pos).y));
    s = text_append(s, b, "");
    lon = lon_;
    lat = lat_;
    tiles_cpp::unproject(city_.proj_, lon, lat, num_cast<f64, f32>(vector_break(pos).x), num_cast<f64, f32>(vector_break(pos).y));
    b = text_format<",\"lon\":%.6f,\"lat\":%.6f">(639, lon, lat);
    s = text_append(s, b, "");
    hint = logic_select<Text>(logic_and(placed_, drone_.tele_.calibrating), "Flugregler kalibriert - stillhalten", "");
    s = text_append(s, text_append(text_append(",\"hint\":\"", spiel_cpp::json_escape(hint).Result, ""), "\"", ""), "");
    if (math_compare<"<", f32>(toast_age_, 4.0f)) {
      s = text_append(s, text_append(text_append(",\"toast\":\"", spiel_cpp::json_escape(toast_).Result, ""), "\"", ""), "");
    }
    b = text_format<",\"ui\":{\"placed\":%s,\"paused\":%s,\"settings\":%u,\"resets\":%u,\"moves\":%u,\"origin\":[%.6f,%.6f],\"northUp\":%s,\"audioOn\":%s,\"musicOn\":%s}">(639, logic_select<Text>(placed_, "true", "false"), logic_select<Text>(paused_, "true", "false"), num_cast<i64, u32>(settings::Settings_version(settings_).Result), num_cast<i64, u32>(resets_), num_cast<i64, u32>(moves_), lat_, lon_, logic_select<Text>(settings::Settings_on(settings_, Opt::view_minimap_north).Result, "true", "false"), logic_select<Text>(settings::Settings_on(settings_, Opt::audio_on).Result, "true", "false"), logic_select<Text>(settings::Settings_on(settings_, Opt::music_on).Result, "true", "false"));
    s = text_append(s, b, "");
    arg = logic_select<Text>(touch_mode_, "touch", "desktop");
    arg_2 = logic_select<Text>(pad.connected, "true", "false");
    arg_3 = logic_select<Text>(locked_, "true", "false");
    arg_4 = logic_select<Text>(touch_play::TouchPlay_any(touch_).Result, "true", "false");
    n4194370 = hint_line("drone", "", false);
    a = n4194370.Result;
    b = text_format<",\"ctl\":{\"device\":\"%s\",\"pad\":%s,\"locked\":%s,\"fingers\":%s,\"hint\":\"%s\",\"sticks\":[%.3f,%.3f,%.3f,%.3f],\"view\":{\"eye\":[%.2f,%.2f,%.2f]}}">(639, arg, arg_2, arg_3, arg_4, spiel_cpp::json_escape(a).Result, num_cast<f64, f32>(sticks_.roll), num_cast<f64, f32>(sticks_.pitch), num_cast<f64, f32>(sticks_.yaw), num_cast<f64, f32>(sticks_.body_pitch), num_cast<f64, f32>(vector_break(cam_.eye).x), num_cast<f64, f32>(vector_break(cam_.eye).y), num_cast<f64, f32>(vector_break(cam_.eye).z));
    s = text_append(s, b, "");
    for (auto it4194371 = flow_for(n4194371, 0, math_sub<i64>(text_length(s), 1)); it4194371.next();) {
      if (math_compare<"<", i32>(num_cast<i32, u8>(num_cast<u8, i8>(num_cast<i8, i64>(text_code_at(s, n4194371.index)))), 32)) {
        text_set_code(s, n4194371.index, 32);
      }
    }
    return {text_append(s, "}", "")};
  }

  struct settings_commandResult { Text Result = ""; };
  /// The settings (settings.h: the Blueprint "Settings" of the level holds the table): the page's menu and its replay.
  settings_commandResult settings_command(const Text& cmd) {
    Text rest = "";
    u32 ok = 0;
    u32 bad = 0;
    u64 at = 0;
    u64 end = 0;
    Text pair = "";
    u64 sp = 0;
    bool both = false;
    Opt o = Opt::mouse_sensitivity;
    Text b = "";
    if (logic_or(text_equal(cmd, "settings"), text_equal(cmd, "settings schema"))) {
      n4194372 = settings_cpp::Settings_schema_json(settings_, settings_table_);
      return {n4194372.Result};
    }
    if (text_equal(cmd, "settings values")) {
      return {settings_cpp::Settings_values_json(settings_, settings_table_).Result};
    }
    if (math_compare<"==", u64>(num_cast<u64, i64>(text_rfind(cmd, "settings reset", 0)), 0)) {
      Settings_reset_section(settings_, settings_table_, logic_select<Text>(math_compare<">", u64>(num_cast<u64, i64>(text_length(cmd)), 15), text_sub(cmd, 15, -1), ""));
      return {settings_cpp::Settings_values_json(settings_, settings_table_).Result};
    }
    if (math_compare<"==", u64>(num_cast<u64, i64>(text_rfind(cmd, "set ", 0)), 0)) {
      rest = text_sub(cmd, 4, -1);
      ok = 0;
      bad = 0;
      at = 0;
      while (loop(math_compare<"<", u64>(at, num_cast<u64, i64>(text_length(rest))))) {
        end = num_cast<u64, i64>(text_find(rest, ";", num_cast<i64, u64>(at)));
        if (math_compare<"==", u64>(end, 18446744073709551615u)) {
          end = num_cast<u64, i64>(text_length(rest));
        }
        pair = text_sub(rest, num_cast<i64, u64>(at), num_cast<i64, u64>(kern::formula<u64, u64>([](auto end, auto at) { return (end - at); }, end, at)));
        sp = num_cast<u64, i64>(text_find(pair, " ", 0));
        both = math_compare<"!=", u64>(sp, 18446744073709551615u);
        if (both) {
          n4194374 = Settings_set_2(settings_, settings_table_, text_sub(pair, 0, num_cast<i64, u64>(sp)), text_sub(pair, num_cast<i64, u64>(kern::formula<u64>([](auto sp) { return (sp + 1); }, sp)), -1));
          both = n4194374.Result;
        }
        if (both) {
          ok += 1;
        } else if (logic_not(math_compare<"==", i64>(text_length(pair), 0))) {
          bad += 1;
        }
        at = kern::formula<u64>([](auto end) { return (end + 1); }, end);
      }
      return {logic_select<Text>(num_cast<bool, u32>(bad), logic_select<Text>(num_cast<bool, u32>(ok), "partly", "unknown setting"), "ok")};
    }
    if (math_compare<"==", u64>(num_cast<u64, i64>(text_rfind(cmd, "get ", 0)), 0)) {
      o = Opt::mouse_sensitivity;
      n4194375 = settings_cpp::find_opt(settings_table_, o, text_sub(cmd, 4, -1));
      if (logic_not(n4194375.Result)) {
        return {"unknown setting"};
      }
      b = "";
      b = text_format<"%g">(31, num_cast<f64, f32>(settings::Settings_get(settings_, o).Result));
      return {b};
    }
    return {""};
  }

  struct command_startsResult { bool Result = false; };
  KERN_PURE command_startsResult command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==", u64>(num_cast<u64, i64>(text_rfind(cmd, p, 0)), 0)};
  }

  struct commandResult { Text Result = ""; };
  commandResult command(const Text& cmd) {
    f64 lat = 0.0;
    f64 lon = 0.0;
    Text b = "";
    Text a = "";
    Text a_2 = "";
    List<TouchPlay_DomRect> rects = List<TouchPlay_DomRect>{};
    u64 i = 0;
    u64 j = 0;
    Text id = "";
    TouchPlay_DomRect d = TouchPlay_DomRect{"", 0.0f, 0.0f, 0.0f, 0.0f};
    f64 x = 0.0;
    f64 y = 0.0;
    f64 w = 0.0;
    f64 h = 0.0;
    Text text = "";
    Text b_2 = "";
    Vector p = Vector{0.0, 0.0, 0.0};
    Text b_3 = "";
    if (logic_not(object_valid<kern::game::GameWorld>(world_))) {
      return {"no world"};
    }
    if (logic_or(logic_or(command_starts(cmd, "settings").Result, command_starts(cmd, "set ").Result), command_starts(cmd, "get ").Result)) {
      n4194376 = settings_command(cmd);
      return {n4194376.Result};
    }
    if (command_starts(cmd, "pause ").Result) {
      paused_ = logic_and(math_compare<">", u64>(num_cast<u64, i64>(text_length(cmd)), 6), math_compare<"==", i32>(num_cast<i32, i8>(num_cast<i8, i64>(text_code_at(cmd, 6))), 49));
      if (logic_and(object_valid<kern::Engine>(engine_), paused_)) {
        engine_engine_request_pointer_lock(engine_, false);
      }
      return {logic_select<Text>(paused_, "paused", "running")};
    }
    if (text_equal(cmd, "reset")) {
      reset();
      return {"ok"};
    }
    if (command_starts(cmd, "start ").Result) {
      lat = 0.0;
      lon = 0.0;
      text_scan<"%lf %lf">(n4194377, cmd, 6, lat, lon);
      lat = n4194377.v0;
      lon = n4194377.v1;
      if (logic_or(logic_or(logic_or(logic_or(math_compare<"<", i32>(num_cast<i32, i64>(n4194377.count), 2), logic_not(kern::formula<f64>([](auto lat) { return isfinite(lat); }, lat))), logic_not(kern::formula<f64>([](auto lon) { return isfinite(lon); }, lon))), math_compare<">", f64>(kern::formula<f64>([](auto lat) { return abs(lat); }, lat), 85.0)), math_compare<">", f64>(kern::formula<f64>([](auto lon) { return abs(lon); }, lon), 180.0))) {
        return {"usage: start <lat> <lon> [name]"};
      }
      move_world(lon, lat);
      return {"moving"};
    }
    if (text_equal(cmd, "origin")) {
      b = "";
      b = text_format<"{\"lat\":%.6f,\"lon\":%.6f}">(95, lat_, lon_);
      return {b};
    }
    if (logic_or(text_equal(cmd, "cam"), text_equal(cmd, "cam next"))) {
      cam_mode_ = kern::formula<i32>([](auto cam_mode_) { return ((cam_mode_ + 1) % 3); }, cam_mode_);
      rig_init_ = false;
      return {logic_select<Text>(math_compare<"==", i32>(cam_mode_, 0), "fpv", logic_select<Text>(math_compare<"==", i32>(cam_mode_, 1), "follow", "third"))};
    }
    if (logic_or(logic_or(text_equal(cmd, "cam fpv"), text_equal(cmd, "cam follow")), text_equal(cmd, "cam third"))) {
      cam_mode_ = kern::formula<bool, i32, bool, i32, i32>([](auto a, auto a0, auto a_2, auto a1, auto a2) { return (a ? a0 : (a_2 ? a1 : a2)); }, text_equal(cmd, "cam fpv"), 0, text_equal(cmd, "cam follow"), 1, 2);
      rig_init_ = false;
      return {text_sub(cmd, 4, -1)};
    }
    if (logic_or(text_equal(cmd, "mode drone"), text_equal(cmd, "mode"))) {
      return {"drone"};
    }
    if (command_starts(cmd, "press ").Result) {
      a = text_sub(cmd, 6, -1);
      touch_play::TouchPlay_inject(touch_, a);
      return {"ok"};
    }
    if (command_starts(cmd, "device ").Result) {
      a_2 = text_sub(cmd, 7, -1);
      if (text_equal(a_2, "touch")) {
        touch_mode_ = true;
      } else if (logic_or(logic_or(text_equal(a_2, "mouse"), text_equal(a_2, "mousemove")), text_equal(a_2, "key"))) {
        touch_mode_ = false;
      }
      return {logic_select<Text>(touch_mode_, "touch", "desktop")};
    }
    if (command_starts(cmd, "touch rects").Result) {
      rects = List<TouchPlay_DomRect>{};
      i = 11;
      while (loop(math_compare<"<", u64>(i, num_cast<u64, i64>(text_length(cmd))))) {
        j = num_cast<u64, i64>(text_find(cmd, ";", num_cast<i64, u64>(i)));
        if (math_compare<"==", u64>(j, 18446744073709551615u)) {
          j = num_cast<u64, i64>(text_length(cmd));
        }
        id = "";
        d = TouchPlay_DomRect{"", 0.0f, 0.0f, 0.0f, 0.0f};
        x = 0.0;
        y = 0.0;
        w = 0.0;
        h = 0.0;
        text = text_sub(cmd, num_cast<i64, u64>(i), num_cast<i64, u64>(kern::formula<u64, u64>([](auto j, auto i) { return (j - i); }, j, i)));
        text_scan<" %31s %lf %lf %lf %lf">(n4194378, text, 0, id, x, y, w, h);
        id = n4194378.v0;
        x = n4194378.v1;
        y = n4194378.v2;
        w = n4194378.v3;
        h = n4194378.v4;
        if (math_compare<"==", i32>(num_cast<i32, i64>(n4194378.count), 5)) {
          d.id = id;
          d.x = num_cast<f32, f64>(x);
          d.y = num_cast<f32, f64>(y);
          d.w = num_cast<f32, f64>(w);
          d.h = num_cast<f32, f64>(h);
          list_add(rects, d);
        }
        i = kern::formula<u64>([](auto j) { return (j + 1); }, j);
      }
      touch_play_cpp::TouchPlay_set_dom_rects(touch_, rects);
      return {"ok"};
    }
    if (text_equal(cmd, "touch release")) {
      touch_play_cpp::TouchPlay_release_all(touch_);
      return {"ok"};
    }
    if (text_equal(cmd, "camera")) {
      b_2 = "";
      b_2 = text_format<"{\"eye\":[%.3f,%.3f,%.3f],\"target\":[%.3f,%.3f,%.3f],\"fov\":%.3f}">(199, num_cast<f64, f32>(vector_break(cam_.eye).x), num_cast<f64, f32>(vector_break(cam_.eye).y), num_cast<f64, f32>(vector_break(cam_.eye).z), num_cast<f64, f32>(vector_break(cam_.target).x), num_cast<f64, f32>(vector_break(cam_.target).y), num_cast<f64, f32>(vector_break(cam_.target).z), num_cast<f64, f32>(cam_.fov));
      return {b_2};
    }
    if (text_equal(cmd, "audio frame")) {
      n4194379 = audio_frame();
      return {n4194379.Result};
    }
    if (command_starts(cmd, "audio web ").Result) {
      web_audio_ = logic_not(text_equal(text_sub(cmd, 10, -1), "0"));
      return {logic_select<Text>(web_audio_, "web", "kern")};
    }
    if (text_equal(cmd, "drone")) {
      n4194380 = drone_position();
      p = n4194380.Result;
      b_3 = "";
      b_3 = text_format<"{\"x\":%.3f,\"y\":%.3f,\"z\":%.3f,\"armed\":%s,\"steps\":%llu}">(159, num_cast<f64, f32>(vector_break(p).x), num_cast<f64, f32>(vector_break(p).y), num_cast<f64, f32>(vector_break(p).z), logic_select<Text>(drone_.tele_.armed, "true", "false"), num_cast<i64, u64>(steps_));
      return {b_3};
    }
    return {"unknown command"};
  }

  struct pointer_matchResult { bool Result = false; };
  KERN_PURE pointer_matchResult pointer_match(const TouchPlay_Finger& f, i32 id = 0) {
    return {math_compare<"==", i32>(f.id, id)};
  }

  struct pointer_match_2Result { bool Result = false; };
  /// The newest finger takes the stick over; deflection counts from where it lands.
  KERN_PURE pointer_match_2Result pointer_match_2(const TouchPlay_Finger& g) {
    return {math_compare<"==", i32>(num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(g.kind)), num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::move)))};
  }

  struct pointer_match_3Result { bool Result = false; };
  KERN_PURE pointer_match_3Result pointer_match_3(const TouchPlay_Finger& g) {
    return {math_compare<"==", i32>(num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(g.kind)), num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::look)))};
  }

  struct pointer_match_4Result { bool Result = false; };
  KERN_PURE pointer_match_4Result pointer_match_4(const TouchPlay_Finger& g) {
    return {math_compare<"==", i32>(num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(g.kind)), num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::fire)))};
  }

  struct pointer_match_5Result { bool Result = false; };
  KERN_PURE pointer_match_5Result pointer_match_5(const TouchFire& fi, i32 id = 0) {
    return {logic_and(math_compare<"==", i32>(fi.id, id), logic_not(fi.lifted))};
  }

  struct TouchPlay_pointerResult { bool Result = false; };
  TouchPlay_pointerResult TouchPlay_pointer(TouchPlay& self, u32 type = 0, i32 id = 0, f32 x = 0.0f, f32 y = 0.0f, f64 t_ms = 0.0) {
    i64 it = 0;
    i64 at = 0;
    i64 end = 0;
    bool go = false;
    f32 t = 0.0f;
    TouchPlay_Finger f = TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0, 0.0, Vector{0.0, 0.0, 0.0}};
    f32 r = 0.0f;
    f32 Lcx = 0.0f;
    f32 Lcy = 0.0f;
    f32 Rcx = 0.0f;
    f32 Rcy = 0.0f;
    i32 at_2 = 0;
    i64 at_3 = 0;
    i64 kept = 0;
    i64 end_2 = 0;
    i64 at_4 = 0;
    i64 kept_2 = 0;
    i64 end_3 = 0;
    TouchFire fire = TouchFire{0, 0.0f, 0.0f, 0.0f, -1.0f, true, 0, -1.0f, false};
    i64 count = 0;
    i64 at_5 = 0;
    i64 end_4 = 0;
    i64 count_2 = 0;
    i64 at_6 = 0;
    i64 end_5 = 0;
    bool both = false;
    u64 other = 0;
    f32 dx = 0.0f;
    f32 dy = 0.0f;
    f64 dt_ms = 0.0;
    f32 radius = 0.0f;
    i32 selected = 0;
    f32 vx = 0.0f;
    f32 vy = 0.0f;
    f32 m = 0.0f;
    f32 yaw = 0.0f;
    f32 pitch = 0.0f;
    f32 cap = 0.0f;
    f32 ox = 0.0f;
    f32 oy = 0.0f;
    f32 dist = 0.0f;
    f32 pull = 0.0f;
    f32 edge = 0.0f;
    f32 m_2 = 0.0f;
    f32 vx_2 = 0.0f;
    f32 vy_2 = 0.0f;
    f32 m_3 = 0.0f;
    u64 other_2 = 0;
    f32 d = 0.0f;
    i32 selected_2 = 0;
    i64 at_7 = 0;
    i64 kept_3 = 0;
    i64 end_6 = 0;
    bool still = false;
    bool inside = false;
    if (enum_equal<TouchMode>(self.lay_.mode, TouchMode::off)) {
      return {false};
    }
    at = 0;
    end = list_length<TouchPlay_Finger>(self.fingers_);
    go = math_compare<"<", i64>(at, end);
    while (loop(go)) {
      if (pointer_match(self.fingers_[at], id).Result) {
        go = false;
      } else {
        at += 1;
        go = math_compare<"<", i64>(at, end);
      }
    }
    it = at;
    t = kern::formula<f64>([](auto t_ms) { return num_cast<f32, f64>((t_ms / 1000.0)); }, t_ms);
    if (math_compare<"==", u32>(type, 0)) {
      if (logic_not(math_compare<"==", i64>(it, list_length<TouchPlay_Finger>(self.fingers_)))) {
        list_remove_at(self.fingers_, it);
      }
      f = TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0, 0.0, Vector{0.0, 0.0, 0.0}};
      f.id = id;
      f.last_x = x;
      f.x = f.last_x;
      f.x0 = f.x;
      f.last_y = y;
      f.y = f.last_y;
      f.y0 = f.y;
      f.last_t = t_ms;
      f.t0 = f.last_t;
      r = kern::formula<f32>([](auto kStick) { return (kStick / 2.0f); }, kStick);
      Lcx = kern::formula<f32, f32>([](auto sl_, auto r) { return (sl_ + r); }, self.sl_, r);
      Lcy = kern::formula<f32, f32, f32>([](auto h_, auto sb_, auto r) { return ((h_ - sb_) - r); }, self.h_, self.sb_, r);
      Rcx = kern::formula<f32, f32, f32, f32>([](auto w_, auto sr_, auto the_result, auto r) { return (((w_ - sr_) - the_result) - r); }, self.w_, self.sr_, touch_play::TouchPlay_right_inset().Result, r);
      Rcy = kern::formula<f32, f32, f32>([](auto h_, auto sb_, auto r) { return ((h_ - sb_) - r); }, self.h_, self.sb_, r);
      at_2 = touch_play_cpp::TouchPlay_button_at(self, x, y).Result;
      if (math_compare<">=", i32>(at_2, 0)) {
        f.kind = TouchPlay_Finger_Kind::press;
        f.button = self.buttons_[num_cast<i64, i32>(at_2)].id;
        self.buttons_[num_cast<i64, i32>(at_2)].pressed = true;
        if (self.buttons_[num_cast<i64, i32>(at_2)].on_down) {
          list_add(self.pressed_, self.buttons_[num_cast<i64, i32>(at_2)].id);
        }
        if (text_equal(self.buttons_[num_cast<i64, i32>(at_2)].id, "clearance")) {
          f.kind = TouchPlay_Finger_Kind::clearance;
          n5242890 = clearance_pad_axis(kern::formula<f32, f32>([](auto pad_y_, auto pad_h_) { return (pad_y_ + (pad_h_ / 2.0f)); }, self.pad_y_, self.pad_h_), y, self.pad_h_);
          self.clearance_ = n5242890.Result;
        }
        if (text_equal(self.buttons_[num_cast<i64, i32>(at_2)].id, "nade")) {
          self.nade_up_ = 0.0f;
        }
      } else if (self.lay_.aiming) {
        f.kind = TouchPlay_Finger_Kind::strike;
      } else if (logic_and(logic_and(self.lay_.alive, logic_not(touch_play::TouchPlay_no_sticks(self).Result)), touch_play_cpp::TouchPlay_in_circle(Lcx, Lcy, r, x, y).Result)) {
        at_3 = 0;
        kept = at_3;
        end_2 = list_length<TouchPlay_Finger>(self.fingers_);
        while (loop(math_compare<"<", i64>(at_3, end_2))) {
          if (logic_not(pointer_match_2(self.fingers_[at_3]).Result)) {
            if (math_compare<"<", i64>(kept, at_3)) {
              var_take(n5242891, self.fingers_[at_3]);
              self.fingers_[kept] = n5242891.value;
            }
            kept += 1;
          }
          at_3 += 1;
        }
        list_resize(self.fingers_, kept, TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0, 0.0, Vector{0.0, 0.0, 0.0}});
        f.kind = TouchPlay_Finger_Kind::move;
        f.base = Vector{0.0, 0.0, 0.0};
        self.move_id_ = id;
        self.move_ = Vector{0.0, 0.0, 0.0};
      } else if (logic_and(logic_and(self.lay_.alive, logic_not(touch_play::TouchPlay_no_sticks(self).Result)), touch_play_cpp::TouchPlay_in_circle(Rcx, Rcy, r, x, y).Result)) {
        at_4 = 0;
        kept_2 = at_4;
        end_3 = list_length<TouchPlay_Finger>(self.fingers_);
        while (loop(math_compare<"<", i64>(at_4, end_3))) {
          if (logic_not(pointer_match_3(self.fingers_[at_4]).Result)) {
            if (math_compare<"<", i64>(kept_2, at_4)) {
              var_take(n5242892, self.fingers_[at_4]);
              self.fingers_[kept_2] = n5242892.value;
            }
            kept_2 += 1;
          }
          at_4 += 1;
        }
        list_resize(self.fingers_, kept_2, TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0, 0.0, Vector{0.0, 0.0, 0.0}});
        f.kind = TouchPlay_Finger_Kind::look;
        self.look_id_ = id;
        self.look_edge_ = Vector{0.0, 0.0, 0.0};
        self.look_ = self.look_edge_;
      } else if (logic_and(self.lay_.alive, logic_or(logic_or(enum_equal<TouchMode>(self.lay_.mode, TouchMode::foot), enum_equal<TouchMode>(self.lay_.mode, TouchMode::drone)), enum_equal<TouchMode>(self.lay_.mode, TouchMode::jet)))) {
        f.kind = TouchPlay_Finger_Kind::fire;
        fire = TouchFire{0, 0.0f, 0.0f, 0.0f, -1.0f, true, 0, -1.0f, false};
        fire.id = id;
        fire.x = x;
        fire.y = y;
        fire.down_t = t;
        fire.hand = kern::formula<f32, f32, i32, i32>([](auto x, auto w_, auto a1, auto a0) { return ((x < (w_ / 2.0f)) ? a1 : a0); }, x, self.w_, 1, 0);
        list_add(self.fires_, fire);
        both = enum_equal<TouchMode>(self.lay_.mode, TouchMode::jet);
        if (both) {
          count_2 = 0;
          at_6 = 0;
          end_5 = list_length<TouchPlay_Finger>(self.fingers_);
          while (loop(math_compare<"<", i64>(at_6, end_5))) {
            if (pointer_match_4(self.fingers_[at_6]).Result) {
              count_2 += 1;
            }
            at_6 += 1;
          }
          both = math_compare<">", i64>(count_2, 0);
        }
        if (both) {
          list_remove_at(self.fires_, math_sub<i64>(list_length<TouchFire>(self.fires_), 1));
          f.kind = TouchPlay_Finger_Kind::none;
        }
      } else if (enum_equal<TouchMode>(self.lay_.mode, TouchMode::car)) {
        f.kind = TouchPlay_Finger_Kind::pinch;
        other = num_cast<u64, i64>(list_length<TouchPlay_Finger>(self.fingers_));
        for (auto it5242893 = flow_for(n5242893, 0, math_sub<i64>(list_length<TouchPlay_Finger>(self.fingers_), 1)); it5242893.next();) {
          if (math_compare<"==", i32>(num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(self.fingers_[n5242893.index].kind)), num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::pinch)))) {
            other = num_cast<u64, i64>(n5242893.index);
          }
        }
        if (math_compare<"<", u64>(other, num_cast<u64, i64>(list_length<TouchPlay_Finger>(self.fingers_)))) {
          self.pinch_d_ = kern::formula<f32, f32, f32, f32>([](auto a, auto x, auto a_2, auto y) { return hypot((a - x), (a_2 - y)); }, self.fingers_[num_cast<i64, u64>(other)].x, x, self.fingers_[num_cast<i64, u64>(other)].y, y);
        }
      } else {
        f.kind = TouchPlay_Finger_Kind::none;
      }
      list_add(self.fingers_, f);
      return {logic_not(logic_or(logic_or(math_compare<"==", i32>(num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(f.kind)), num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::pinch))), math_compare<"==", i32>(num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(f.kind)), num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::none)))), logic_and(math_compare<"==", i32>(num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(f.kind)), num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::fire))), enum_equal<TouchMode>(self.lay_.mode, TouchMode::drone))))};
    }
    if (math_compare<"==", i64>(it, list_length<TouchPlay_Finger>(self.fingers_))) {
      return {false};
    }
    if (math_compare<"==", u32>(type, 1)) {
      dx = kern::formula<f32, f32>([](auto x, auto a) { return (x - a); }, x, self.fingers_[it].last_x);
      dy = kern::formula<f32, f32>([](auto y, auto a) { return (y - a); }, y, self.fingers_[it].last_y);
      dt_ms = kern::formula<f64, f64>([](auto t_ms, auto a) { return max(1.0, (t_ms - a)); }, t_ms, self.fingers_[it].last_t);
      self.fingers_[it].x = x;
      self.fingers_[it].y = y;
      radius = kern::formula<f32, f32>([](auto kStick, auto kRadiusK) { return max(1.0f, (kStick * kRadiusK)); }, kStick, kRadiusK);
      selected = num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(self.fingers_[it].kind));
      if (math_compare<"==", i32>(selected, 0)) {
        vx = kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto a, auto radius) { return (x + ((x_2 - a) / radius)); }, vector_break(self.fingers_[it].base).x, x, self.fingers_[it].x0, radius);
        vy = kern::formula<f32, f32, f32, f32>([](auto y, auto y_2, auto a, auto radius) { return (y + ((y_2 - a) / radius)); }, vector_break(self.fingers_[it].base).y, y, self.fingers_[it].y0, radius);
        m = kern::formula<f32, f32>([](auto vx, auto vy) { return hypot(vx, vy); }, vx, vy);
        if (math_compare<">", f32>(m, 1.0f)) {
          vx = kern::formula<f32, f32>([](auto vx, auto m) { return (vx / m); }, vx, m);
          vy = kern::formula<f32, f32>([](auto vy, auto m) { return (vy / m); }, vy, m);
        }
        self.move_ = vector_make(vx, vy, 0.0f);
      } else if (math_compare<"==", i32>(selected, 1)) {
        if (enum_equal<TouchMode>(self.lay_.mode, TouchMode::foot)) {
          if (logic_or(math_compare<"!=", f32>(dx, 0.0f), math_compare<"!=", f32>(dy, 0.0f))) {
            yaw = 0.0f;
            pitch = 0.0f;
            touch_look_delta(yaw, pitch, dx, dy, kern::formula<f32, f32, f64>([](auto dx, auto dy, auto dt_ms) { return num_cast<f32, f64>(((num_cast<f64, f32>(hypot(dx, dy)) / dt_ms) * 1000.0)); }, dx, dy, dt_ms));
            self.drag_ = vector_make(kern::formula<f32, f32>([](auto x, auto yaw) { return (x + yaw); }, vector_break(self.drag_).x, yaw), vector_break(self.drag_).y, vector_break(self.drag_).z);
            self.drag_ = vector_make(vector_break(self.drag_).x, kern::formula<f32, f32>([](auto y, auto pitch) { return (y + pitch); }, vector_break(self.drag_).y, pitch), vector_break(self.drag_).z);
          }
          cap = kern::formula<f32, f32, f32>([](auto radius, auto kEdgeStart, auto kEdgeRampPx) { return ((radius * kEdgeStart) + kEdgeRampPx); }, radius, kEdgeStart, kEdgeRampPx);
          ox = kern::formula<f32, f32>([](auto x, auto a) { return (x - a); }, x, self.fingers_[it].x0);
          oy = kern::formula<f32, f32>([](auto y, auto a) { return (y - a); }, y, self.fingers_[it].y0);
          dist = kern::formula<f32, f32>([](auto ox, auto oy) { return hypot(ox, oy); }, ox, oy);
          if (math_compare<">", f32>(dist, cap)) {
            pull = kern::formula<f32, f32>([](auto dist, auto cap) { return ((dist - cap) / dist); }, dist, cap);
            self.fingers_[it].x0 = kern::formula<f32, f32, f32>([](auto a, auto ox, auto pull) { return (a + (ox * pull)); }, self.fingers_[it].x0, ox, pull);
            self.fingers_[it].y0 = kern::formula<f32, f32, f32>([](auto a, auto oy, auto pull) { return (a + (oy * pull)); }, self.fingers_[it].y0, oy, pull);
            ox = kern::formula<f32, f32>([](auto ox, auto pull) { return (ox * (1.0f - pull)); }, ox, pull);
            oy = kern::formula<f32, f32>([](auto oy, auto pull) { return (oy * (1.0f - pull)); }, oy, pull);
            dist = cap;
          }
          edge = common::clampf(kern::formula<f32, f32, f32, f32>([](auto dist, auto kEdgeStart, auto radius, auto kEdgeRampPx) { return ((dist - (kEdgeStart * radius)) / kEdgeRampPx); }, dist, kEdgeStart, radius, kEdgeRampPx), 0.0f, 1.0f).Result;
          m_2 = kern::formula<f32, f32>([](auto dist, auto radius) { return (dist / radius); }, dist, radius);
          self.look_ = vector_make(kern::formula<f32, f32, f32, f32>([](auto m_2, auto dist, auto ox, auto a0f) { return (min(1.0f, m_2) * ((dist > 0.0f) ? (ox / dist) : a0f)); }, m_2, dist, ox, 0.0f), kern::formula<f32, f32, f32, f32>([](auto m_2, auto dist, auto oy, auto a0f) { return (min(1.0f, m_2) * ((dist > 0.0f) ? (oy / dist) : a0f)); }, m_2, dist, oy, 0.0f), 0.0f);
          self.look_edge_ = logic_select<Vector>(logic_and(math_compare<">", f32>(edge, 0.0f), math_compare<">", f32>(m_2, 0.0f)), vector_make(kern::formula<f32, f32, f32, f32>([](auto ox, auto radius, auto m_2, auto edge) { return (((ox / radius) / m_2) * edge); }, ox, radius, m_2, edge), kern::formula<f32, f32, f32, f32>([](auto oy, auto radius, auto m_2, auto edge) { return (((oy / radius) / m_2) * edge); }, oy, radius, m_2, edge), 0.0f), Vector{0.0, 0.0, 0.0});
        } else {
          vx_2 = kern::formula<f32, f32, f32>([](auto x, auto a, auto radius) { return ((x - a) / radius); }, x, self.fingers_[it].x0, radius);
          vy_2 = kern::formula<f32, f32, f32>([](auto y, auto a, auto radius) { return ((y - a) / radius); }, y, self.fingers_[it].y0, radius);
          m_3 = kern::formula<f32, f32>([](auto vx_2, auto vy_2) { return hypot(vx_2, vy_2); }, vx_2, vy_2);
          if (math_compare<">", f32>(m_3, 1.0f)) {
            vx_2 = kern::formula<f32, f32>([](auto vx_2, auto m_3) { return (vx_2 / m_3); }, vx_2, m_3);
            vy_2 = kern::formula<f32, f32>([](auto vy_2, auto m_3) { return (vy_2 / m_3); }, vy_2, m_3);
          }
          self.look_ = vector_make(vx_2, vy_2, 0.0f);
        }
      } else if (math_compare<"==", i32>(selected, 2)) {
        for (auto it5242894 = flow_for(n5242894, 0, math_sub<i64>(list_length<TouchFire>(self.fires_), 1)); it5242894.next();) {
          if (math_compare<"==", i32>(self.fires_[n5242894.index].id, id)) {
            self.fires_[n5242894.index].x = x;
            self.fires_[n5242894.index].y = y;
          }
        }
      } else if (math_compare<"==", i32>(selected, 4)) {
        n5242895 = clearance_pad_axis(kern::formula<f32, f32>([](auto pad_y_, auto pad_h_) { return (pad_y_ + (pad_h_ / 2.0f)); }, self.pad_y_, self.pad_h_), y, self.pad_h_);
        self.clearance_ = n5242895.Result;
      } else if (math_compare<"==", i32>(selected, 3)) {
        if (text_equal(self.fingers_[it].button, "nade")) {
          self.nade_up_ = kern::formula<f32, f32>([](auto a, auto y) { return (a - y); }, self.fingers_[it].y0, y);
        }
      } else if (math_compare<"==", i32>(selected, 5)) {
        other_2 = num_cast<u64, i64>(list_length<TouchPlay_Finger>(self.fingers_));
        for (auto it5242896 = flow_for(n5242896, 0, math_sub<i64>(list_length<TouchPlay_Finger>(self.fingers_), 1)); it5242896.next();) {
          if (logic_and(math_compare<"==", i32>(num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(self.fingers_[n5242896.index].kind)), num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::pinch))), math_compare<"!=", i32>(self.fingers_[n5242896.index].id, id))) {
            other_2 = num_cast<u64, i64>(n5242896.index);
          }
        }
        if (math_compare<"<", u64>(other_2, num_cast<u64, i64>(list_length<TouchPlay_Finger>(self.fingers_)))) {
          d = kern::formula<f32, f32, f32, f32>([](auto a, auto x, auto a_2, auto y) { return hypot((a - x), (a_2 - y)); }, self.fingers_[num_cast<i64, u64>(other_2)].x, x, self.fingers_[num_cast<i64, u64>(other_2)].y, y);
          if (logic_and(math_compare<">", f32>(self.pinch_d_, 10.0f), math_compare<">", f32>(d, 10.0f))) {
            self.zoom_ = kern::formula<f32, f32, f32>([](auto zoom_, auto pinch_d_, auto d) { return (zoom_ * (pinch_d_ / d)); }, self.zoom_, self.pinch_d_, d);
          }
          self.pinch_d_ = d;
        }
      } else {
      }
      self.fingers_[it].last_x = x;
      self.fingers_[it].last_y = y;
      self.fingers_[it].last_t = t_ms;
      return {true};
    }
    selected_2 = num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(self.fingers_[it].kind));
    if (math_compare<"==", i32>(selected_2, 0)) {
      self.move_ = Vector{0.0, 0.0, 0.0};
      self.move_id_ = -1;
    } else if (math_compare<"==", i32>(selected_2, 1)) {
      self.look_edge_ = Vector{0.0, 0.0, 0.0};
      self.look_ = self.look_edge_;
      self.look_id_ = -1;
    } else if (math_compare<"==", i32>(selected_2, 2)) {
      for (auto it5242897 = flow_for(n5242897, 0, math_sub<i64>(list_length<TouchFire>(self.fires_), 1)); it5242897.next();) {
        if (logic_and(math_compare<"==", i32>(self.fires_[n5242897.index].id, id), self.fires_[n5242897.index].fresh)) {
          self.fires_[n5242897.index].lifted = true;
        }
      }
      at_7 = 0;
      kept_3 = at_7;
      end_6 = list_length<TouchFire>(self.fires_);
      while (loop(math_compare<"<", i64>(at_7, end_6))) {
        if (logic_not(pointer_match_5(self.fires_[at_7], id).Result)) {
          if (math_compare<"<", i64>(kept_3, at_7)) {
            var_take(n5242898, self.fires_[at_7]);
            self.fires_[kept_3] = n5242898.value;
          }
          kept_3 += 1;
        }
        at_7 += 1;
      }
      list_resize(self.fires_, kept_3, TouchFire{0, 0.0f, 0.0f, 0.0f, -1.0f, true, 0, -1.0f, false});
    } else if (math_compare<"==", i32>(selected_2, 4)) {
      self.clearance_ = 0.0f;
    } else if (math_compare<"==", i32>(selected_2, 6)) {
      if (math_compare<"==", u32>(type, 2)) {
        self.strike_tap_ = true;
        self.strike_x_ = x;
        self.strike_y_ = y;
      }
    } else if (math_compare<"==", i32>(selected_2, 3)) {
      still = false;
      for (auto it5242899 = flow_for(n5242899, 0, math_sub<i64>(list_length<TouchPlay_Finger>(self.fingers_), 1)); it5242899.next();) {
        if (logic_and(logic_and(math_compare<"!=", i32>(self.fingers_[n5242899.index].id, id), math_compare<"==", i32>(num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(self.fingers_[n5242899.index].kind)), num_cast<i32, i64>(enum_index<TouchPlay_Finger_Kind>(TouchPlay_Finger_Kind::press)))), text_equal(self.fingers_[n5242899.index].button, self.fingers_[it].button))) {
          still = true;
        }
      }
      for (auto it5242900 = flow_for(n5242900, 0, math_sub<i64>(list_length<TouchBtn>(self.buttons_), 1)); it5242900.next();) {
        if (text_equal(self.buttons_[n5242900.index].id, self.fingers_[it].button)) {
          if (logic_not(still)) {
            self.buttons_[n5242900.index].pressed = false;
          }
          inside = logic_and(logic_and(logic_and(math_compare<">=", f32>(x, kern::formula<f32>([](auto a) { return (a - 4.0f); }, self.buttons_[n5242900.index].x)), math_compare<"<=", f32>(x, kern::formula<f32, f32>([](auto a, auto a_2) { return ((a + a_2) + 4.0f); }, self.buttons_[n5242900.index].x, self.buttons_[n5242900.index].w))), math_compare<">=", f32>(y, kern::formula<f32>([](auto a) { return (a - 4.0f); }, self.buttons_[n5242900.index].y))), math_compare<"<=", f32>(y, kern::formula<f32, f32>([](auto a, auto a_2) { return ((a + a_2) + 4.0f); }, self.buttons_[n5242900.index].y, self.buttons_[n5242900.index].h)));
          if (logic_and(logic_and(logic_not(self.buttons_[n5242900.index].on_down), math_compare<"==", u32>(type, 2)), inside)) {
            list_add(self.pressed_, self.buttons_[n5242900.index].id);
          }
        }
      }
      list_add(self.released_, self.fingers_[it].button);
    } else {
    }
    list_remove_at(self.fingers_, it);
    return {true};
  }

  // ---- what nodes give and keep ----
  struct { bool desktop = false; bool pad = false; } n10;
  struct { f64 value = 0.0; } n11;
  struct { f64 value = 0.0; } n12;
  struct { f64 x = 0.0; f64 y = 0.0; } n13;
  struct { f64 value = 0.0; } n14;
  struct { f64 strength = 0.0; } n15;
  struct { f64 x = 0.0; f64 y = 0.0; } n16;
  struct { f64 velocity = 0.0; f64 angle = 0.0; } n17;
  struct { f64 yaw = 0.0; f64 pitch = 0.0; } n18;
  struct { bool active = false; f64 slowdown = 0.0; f64 yaw = 0.0; f64 pitch = 0.0; } n19;
  struct { f64 value = 0.0; } n20;
  struct { f64 value = 0.0; } n21;
  struct { f64 value = 0.0; } n22;
  struct { f64 value = 0.0; } n23;
  struct { f64 value = 0.0; } n24;
  struct { f64 value = 0.0; } n25;
  struct { f64 value = 0.0; } n26;
  struct { f64 roll = 0.0; f64 pitch = 0.0; } n27;
  struct { f64 yaw = 0.0; f64 bodyPitch = 0.0; } n28;
  struct { f64 value = 0.0; } n29;
  struct { f64 rate = 0.0; } n30;
  struct { f64 rate = 0.0; } n31;
  struct { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; } n32;
  struct { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; } n33;
  struct { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; } n34;
  struct { controls_Look look = controls_Look{0.0, 0.0, false, 0.0, false}; } n35;
  struct { f64 yaw = 0.0; f64 pitch = 0.0; } n36;
  struct { i64 out = 0; bool was = false; bool heldMap = false; f64 downAt = 0.0; f64 mapAt = 0.0; } n37;
  struct { bool mic = false; bool mute = false; bool suppressWeapon = false; bool r3 = false; bool y = false; bool latched = false; } n38;
  struct : kern::Node<39> { i64 index = 0; } n39;
  struct { bool neutral = false; bool armed = false; } n40;
  struct { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; bool pulled = false; } n41;
  struct { f64 power = 0.0; } n42;
  struct { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; f64 left = 0.0; } n43;
  struct { controls_Cook cook = controls_Cook{0, 0.0, false, 0.0, 0.0, 0}; bool cookedOff = false; } n44;
  struct { f64 seconds = 0.0; } n45;
  struct { f64 mps = 0.0; } n46;
  struct { bool desktop = false; } n47;
  struct { Text line = ""; } n48;
  city_stream_cpp::CityStream_take_requestsResult n1048586;
  terrain_cpp::Terrain_take_requestsResult n1048587;
  terrain_cpp::Terrain_take_image_requestsResult n1048588;
  touchResult n1048589;
  touch_view_jsonResult n1048590;
  hud_jsonResult n1048591;
  commandResult n1048592;
  audio_initResult n1048593;
  audio_renderResult n1048594;
  struct : kern::Node<2097162> { i64 index = 0; } n2097162;
  struct { f64 value = 0.0; } n2097163;
  settings_cpp::find_optResult n2097164;
  struct { bool known = false; f64 value = 0.0; } n2097165;
  struct { bool in = false; } n2097166;
  struct { Object<kern::game::GameWorld> result = Object<kern::game::GameWorld>{}; } n4194314;
  struct { bool result = false; } n4194315;
  struct { bool result = false; } n4194316;
  bindResult n4194317;
  struct { bool result = false; } n4194318;
  bind_settingsResult n4194319;
  struct { bool result = false; } n4194320;
  settings_cpp::Settings_newResult n4194321;
  struct { Actor result = Actor{}; } n4194322;
  struct { bool result = false; } n4194323;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194324;
  drone_art_cpp::DroneArt_initResult n4194325;
  struct { bool result = false; Mat4 out = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}; } n4194326;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194327;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194328;
  struct { bool result = false; } n4194329;
  struct { bool result = false; } n4194330;
  placeResult n4194331;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194332;
  placeResult n4194333;
  struct { Object<kern::Input> result = Object<kern::Input>{}; } n4194334;
  gamepad_axisResult n4194335;
  gamepad_axisResult n4194336;
  gamepad_axisResult n4194337;
  gamepad_axisResult n4194338;
  height_axisResult n4194339;
  touch_play_cpp::TouchPlay_take_pressedResult n4194340;
  touch_play_cpp::TouchPlay_take_pressedResult n4194341;
  touch_play_cpp::TouchPlay_take_pressedResult n4194342;
  drone_positionResult n4194343;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194344;
  drone_positionResult n4194345;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194346;
  drone_matrixResult n4194347;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194348;
  struct { f32 result = 0.0f; } n4194349;
  struct : kern::Node<4194350> { i64 index = 0; } n4194350;
  drone_matrixResult n4194351;
  struct { bool result = false; } n4194352;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n4194353;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n4194354;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n4194355;
  struct : kern::Node<4194356> { i64 index = 0; } n4194356;
  drone_positionResult n4194357;
  TouchPlay_pointerResult n4194358;
  touch_play_cpp::TouchPlay_view_jsonResult n4194359;
  struct : kern::Node<4194360> { i64 index = 0; } n4194360;
  struct { bool result = false; } n4194361;
  struct { Object<kern::game::SoundBank> result = Object<kern::game::SoundBank>{}; } n4194362;
  struct { Object<kern::game::GameAudio> result = Object<kern::game::GameAudio>{}; } n4194363;
  sound_fx_cpp::SoundFx_initResult n4194364;
  struct { List<f32> out = List<f32>{}; } n4194365;
  struct : kern::Node<4194366> { i64 index = 0; } n4194366;
  struct : kern::Node<4194367> { i64 index = 0; } n4194367;
  drone_positionResult n4194368;
  drone_positionResult n4194369;
  hint_lineResult n4194370;
  struct : kern::Node<4194371> { i64 index = 0; } n4194371;
  settings_cpp::Settings_schema_jsonResult n4194372;
  Settings_set_2Result n4194374;
  settings_cpp::find_optResult n4194375;
  settings_commandResult n4194376;
  struct { i64 count = 0; f64 v0 = 0.0; f64 v1 = 0.0; } n4194377;
  struct { i64 count = 0; Text v0 = ""; f64 v1 = 0.0; f64 v2 = 0.0; f64 v3 = 0.0; f64 v4 = 0.0; } n4194378;
  audio_frameResult n4194379;
  drone_positionResult n4194380;
  clearance_pad_axisResult n5242890;
  struct { TouchPlay_Finger value = TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0, 0.0, Vector{0.0, 0.0, 0.0}}; } n5242891;
  struct { TouchPlay_Finger value = TouchPlay_Finger{0, TouchPlay_Finger_Kind::none, "", 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0, 0.0, Vector{0.0, 0.0, 0.0}}; } n5242892;
  struct : kern::Node<5242893> { i64 index = 0; } n5242893;
  struct : kern::Node<5242894> { i64 index = 0; } n5242894;
  clearance_pad_axisResult n5242895;
  struct : kern::Node<5242896> { i64 index = 0; } n5242896;
  struct : kern::Node<5242897> { i64 index = 0; } n5242897;
  struct { TouchFire value = TouchFire{0, 0.0f, 0.0f, 0.0f, -1.0f, true, 0, -1.0f, false}; } n5242898;
  struct : kern::Node<5242899> { i64 index = 0; } n5242899;
  struct : kern::Node<5242900> { i64 index = 0; } n5242900;
};
