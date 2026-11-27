struct SpawnCity : virtual kern::Blueprint, virtual blueprint_call, virtual city_stream_cpp, virtual common, virtual controls, virtual controls_cpp, virtual drone, virtual drone_cpp, virtual settings, virtual settings_cpp, virtual sound_fx_cpp, virtual spiel_cpp, virtual terrain_cpp, virtual touch_play, virtual touch_play_cpp {
  // ---- variables ----
  PadState pad = PadState{false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, false};
  Object<kern::Engine> engine_ = Object<kern::Engine>{};
  Object<kern::game::GameWorld> world_ = Object<kern::game::GameWorld>{};
  u32 drone_actor_ = 0;
  CityStream city_ = CityStream{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 2600, 1, List<Vector>{}, 1, false, 0, 0, List<Vec4>{}, CityStream_NearCollision{physics_Body{0}, physics_Body{0}, Vector{1e+09, 1e+09, 0.0}, Vector{1e+09, 1e+09, 0.0}, 1000000, List<u64_u32_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0}, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, 0.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{1e+09, 1e+09, 0.0}, Vector{1e+09, 1e+09, 0.0}, -1000000000.0, Map<u32, CityStream_Damage>{}, List<u32>{}, false, Object<kern::Renderer>{}, Object<kern::physics::World>{}, GeoProjector{0.0, 0.0, 1.0}, TileKey{14, 0, 0}, Map<u64, List<CityStream_Tile>>{}, List<TileKey>{}, List<Lane>{}, false, 0, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<u64, Terrain_Image>{}, List<TileKey>{}, Vector{1e+09, 1e+09, 0.0}, Object<kern::Renderer>{}, Object<kern::physics::World>{}, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0f, false, Terrain_Grid{0.0f, 0.0f, 10.0f, 0, List<f32>{}, 0.0f, 0.0f}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<u64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<u64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<u64, Terrain_RenderTile>{}, Map<u64, Terrain_CollisionTile>{}, Map<u64, u32>{}, Map<u64, bool>{}, Set<u64>{}, Set<u64>{}, Set<u64>{}, List<u64>{}, List<u64>{}, List<u64>{}, Vector{1e+09, 1e+09, 0.0}, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, false, false}}, List<Vec4>{}, 0, -1};
  Drone drone_ = Drone{DroneParams{0.225f, 0.127f, 2300.0f, 0.08f, 1.1e-05f, 0.105f, 0.012f, 45.0f, 45.0f, 4.0f, 1.5f, 0.055f, 180.0f, 1.225f, 0.35f, 0.18f, Vector{0.0, 0.0, 0.0}, 0.18f, 0.035f, 0.005f, 0.2f, 0.03f, 0.02f, 0.03f, 0.1f, 0.015f, -0.015f, -0.07f, 0.028f}, fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0f, false, false, false, false, false, false, false, 0.0f, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, false}, fc_RC{List<u16>{}, false, false, false, 0}, fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false}, 0, 0, false}, List<Vector>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 1.0f, 16.8f, 0.0f, 0.0f, 0.0f, 0.0f, Vector{0.0, 0.0, 0.0}, List<u16>{1000, 1000, 1000, 1000}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0.0, 1, 0.01f, 0.01f, Vector{0.0, 0.0, 0.0}, 1, true, Rng{1168318247, 0.0f, false}, Rng{1905503009, 0.0f, false}, Rng{1168232869, 0.0f, false}, List<f32>{0.08f, -0.05f, 0.04f}, List<f32>{0.001f, -0.0015f, 0.002f}, 0, false, DroneTelemetry{false, true, false, false, false, 0, List<u16>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, List<f32>{}}};
  DroneSticks sticks_ = DroneSticks{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.2f, false, true, false};
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
  bool placed_ = false;
  bool paused_ = false;
  bool landing_ = false;
  bool height_held_ = false;
  f32 landing_still_ = 0.0f;
  u64 steps_ = 0;
  /// 0 FPV, 1 follow, 2 third (simulator.mjs camFpv / camFollow / camThird)
  i32 cam_mode_ = 0;
  Camera cam_ = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471976f, 0.05f, 1800.0f, false, 20.0f, false, Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}};
  bool rig_init_ = false;
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

  struct Controls_VoiceChordStepResult { bool mic = false; bool mute = false; bool suppressWeapon = false; bool r3 = false; bool y = false; bool latched = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("VoiceChordStep") Controls_VoiceChordStepResult Controls_VoiceChordStep(bool r3 = false, bool y = false, bool latched = false, bool r3Now = false, bool yNow = false, bool allowed = false);

  struct Controls_ReleaseLatchStepResult { bool neutral = false; bool armed = false; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("ReleaseLatchStep") Controls_ReleaseLatchStepResult Controls_ReleaseLatchStep(bool armed = false, List<f64> values = List<f64>{});

  struct Controls_GrenadeDragResult { f64 power = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeDrag") Controls_GrenadeDragResult Controls_GrenadeDrag(f64 upPx = 0.0);

  struct Controls_GrenadeSpeedResult { f64 mps = 0.0; };
  /// (of the Blueprint of the actor Controls, called on it)
  KERN_EXTERN_AS("GrenadeSpeed") Controls_GrenadeSpeedResult Controls_GrenadeSpeed(f64 power = 0.0);

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

  struct VoiceChord_stepResult { controls_VoiceChord_Out Result = controls_VoiceChord_Out{false, false, false}; };
  VoiceChord_stepResult VoiceChord_step(controls_VoiceChord& self, bool r3_now = false, bool y_now = false, bool allowed = false) {
    controls_VoiceChord_Out o = controls_VoiceChord_Out{false, false, false};
    o = controls_VoiceChord_Out{false, false, false};
    func_call_on<"Controls_VoiceChordStep">(controls_g_rules, kern::out(n37.mic, n37.mute, n37.suppressWeapon, n37.r3, n37.y, n37.latched), self.r3, self.y, self.latched, r3_now, y_now, allowed);
    o.mic = n37.mic;
    o.mute = n37.mute;
    o.suppress_weapon = n37.suppressWeapon;
    self.r3 = n37.r3;
    self.y = n37.y;
    self.latched = n37.latched;
    return {o};
  }

  struct ReleaseLatch_stepResult { bool Result = false; };
  ReleaseLatch_stepResult ReleaseLatch_step(controls_ReleaseLatch& self, const Fixed<f32, 9>& values) {
    List<f64> list = List<f64>{};
    f32 v = 0.0f;
    bool neutral = false;
    list = List<f64>{};
    for (auto it38 = flow_for(n38, 0, math_sub<i64>(list_length<f32>(values), 1)); it38.next();) {
      v = values[n38.index];
      list_add(list, num_cast<f64, f32>(v));
    }
    neutral = false;
    func_call_on<"Controls_ReleaseLatchStep">(controls_g_rules, kern::out(n39.neutral, n39.armed), self.armed, list);
    neutral = n39.neutral;
    self.armed = n39.armed;
    return {neutral};
  }

  void GrenadeCook_drag(controls_GrenadeCook& self, f32 up_px = 0.0f) {
    func_call_on<"Controls_GrenadeDrag">(controls_g_rules, kern::out(n40.power), blueprint_call::decimal(up_px).Result);
    self.power = num_cast<f32, f64>(n40.power);
  }

  struct GrenadeCook_speedResult { f32 Result = 0.0f; };
  GrenadeCook_speedResult GrenadeCook_speed(const controls_GrenadeCook& self) {
    func_call_on<"Controls_GrenadeSpeed">(controls_g_rules, kern::out(n41.mps), blueprint_call::decimal(self.power).Result);
    return {num_cast<f32, f64>(n41.mps)};
  }

  struct grenade_fuseResult { f32 Result = 0.0f; };
  KERN_PURE grenade_fuseResult grenade_fuse() {
    return {num_cast<f32, f64>(func_get_on<"GrenadeFuse", f64>(controls_g_rules))};
  }

  struct hint_lineResult { Text Result = ""; };
  hint_lineResult hint_line(Text mode = "", Text weapon = "", bool dead = false) {
    Text line = "";
    line = "";
    func_call_on<"Controls_HintLine">(controls_g_rules, kern::out(n42.line), mode, weapon, dead);
    line = n42.line;
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

  struct sc_audio_initResult { i32 Result = 0; };
  /// ---- audio: Kern's mixer renders, the page plays (KernAudio) ----
  KERN_EXPORT_AS("sc_audio_init", "u32 -> i32") sc_audio_initResult sc_audio_init(u32 rate = 0) {
    n1048589 = audio_init(rate);
    return {kern::formula<bool, i32, i32>([](auto the_result, auto a1, auto a0) { return (the_result ? a1 : a0); }, n1048589.Result, 1, 0)};
  }

  struct sc_audio_renderResult { List<f32> Result = List<f32>{}; };
  KERN_EXPORT_AS("sc_audio_render", "u32 -> floats") sc_audio_renderResult sc_audio_render(u32 frames = 0) {
    n1048590 = audio_render(frames);
    return {n1048590.Result};
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

  void stop(Object<kern::Engine> arg1 = Object<kern::Engine>{}) {
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
      engine_game_world_actor_matrix(n4194325, world_, drone_actor_, m);
      actor_matrix = n4194325.result;
      m = n4194325.out;
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
    engine_game_world_physics(n4194326, world_);
    physics = n4194326.result;
    actor_body = engine_game_world_actor_body(world_, drone_actor_);
    return {struct_break<BodyState>(common::read_body(physics, actor_body).Result).position};
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
    engine_engine_input(n4194328, e_);
    input = n4194328.result;
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
      n4194329 = gamepad_axis(pad.lx, 0.14f);
      n4194330 = gamepad_axis(kern::formula<f32>([](auto ly) { return (-ly); }, pad.ly), 0.14f);
      left = vector_add(left, vector_make(n4194329.Result, n4194330.Result, 0.0f));
    }
    right = vector_make(kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return (the_result - the_result_2); }, spiel_cpp::key(in, Key::right).Result, spiel_cpp::key(in, Key::left).Result), kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return (the_result - the_result_2); }, spiel_cpp::key(in, Key::down).Result, spiel_cpp::key(in, Key::up).Result), 0.0f);
    RateStick_step(mouse_, mouse_px_, dt);
    mouse_px_ = Vector{0.0, 0.0, 0.0};
    right = vector_add(right, mouse_.rate);
    if (touch_mode_) {
      right = vector_add(right, touch_play::TouchPlay_look(touch_).Result);
    }
    if (pad.connected) {
      n4194331 = gamepad_axis(pad.rx, 0.14f);
      n4194332 = gamepad_axis(kern::formula<f32>([](auto ry) { return (-ry); }, pad.ry), 0.14f);
      right = vector_add(right, vector_make(n4194331.Result, n4194332.Result, 0.0f));
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
      n4194333 = height_axis(pad.rt, pad.lt);
      axis += n4194333.Result;
    }
    if (touch_mode_) {
      axis += touch_play::TouchPlay_clearance(touch_).Result;
    }
    axis = common::clampf(axis, -1.0f, 1.0f).Result;
    pressed = engine_input_pressed(in, Key::c);
    either_3 = logic_or(pressed, logic_and(pad.connected, spiel_cpp::pad_hit(pad, num_cast<i32, i64>(enum_index<PadButtonIndex>(PadButtonIndex::kPadY))).Result));
    if (logic_not(either_3)) {
      a_3 = "cam";
      n4194334 = touch_play_cpp::TouchPlay_take_pressed(touch_, a_3);
      either_3 = n4194334.Result;
    }
    if (either_3) {
      cam_mode_ = kern::formula<i32>([](auto cam_mode_) { return ((cam_mode_ + 1) % 3); }, cam_mode_);
      rig_init_ = false;
    }
    pressed_2 = engine_input_pressed(in, Key::r);
    either_4 = logic_or(pressed_2, logic_and(pad.connected, spiel_cpp::pad_hit(pad, num_cast<i32, i64>(enum_index<PadButtonIndex>(PadButtonIndex::kPadA))).Result));
    if (logic_not(either_4)) {
      a_5 = "arm";
      n4194335 = touch_play_cpp::TouchPlay_take_pressed(touch_, a_5);
      either_4 = n4194335.Result;
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
    n4194336 = touch_play_cpp::TouchPlay_take_pressed(touch_, a_6);
    sticks_.kill = logic_or(n4194336.Result, logic_and(pad.connected, spiel_cpp::pad_hit(pad, num_cast<i32, i64>(enum_index<PadButtonIndex>(PadButtonIndex::kPadB))).Result));
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
    engine_mixer_init(n4194337, mixer_, rate, 64);
    init = n4194337.result;
    if (logic_not(init)) {
      return {false};
    }
    old = bank_;
    engine_sound_bank_make(n4194338, mixer_);
    sound_bank = n4194338.result;
    bank_ = sound_bank;
    if (object_valid<kern::game::SoundBank>(old)) {
      engine_sound_bank_release(old);
    }
    old_2 = game_audio_;
    engine_game_audio_make(n4194339, mixer_, bank_);
    game_audio = n4194339.result;
    game_audio_ = game_audio;
    if (object_valid<kern::game::GameAudio>(old_2)) {
      engine_game_audio_release(old_2);
    }
    if (object_valid<kern::game::GameWorld>(world_)) {
      engine_game_audio_begin(game_audio_, world_);
    }
    n4194340 = sound_fx_cpp::SoundFx_init(fx_, mixer_);
    return {n4194340.Result};
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
      engine_mixer_render(n4194341, mixer_, frames);
      last = math_sub<i64>(list_length<f32>(n4194341.out), 1);
      for (auto it4194342 = flow_for(n4194342, 0, last); it4194342.next();) {
        audio_out_[math_add<i64>(at, n4194342.index)] = list_get<f32>(n4194341.out, n4194342.index);
      }
    } else {
      for (auto it4194343 = flow_for(n4194343, 0, math_sub<i64>(list_length<f32>(audio_out_), 1)); it4194343.next();) {
        audio_out_[n4194343.index] = 0.0f;
      }
    }
    return {audio_out_};
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
    engine_sound_bank_prepare(n4194344, bank_, 3.0);
    prepare = n4194344.result;
    engine_game_audio_update(game_audio_, world_, cam_, dt);
    engine_mixer_bus(n4194345, mixer_, audio_Bus::master);
    n4194345.result.muted = logic_or(logic_not(settings::Settings_on(settings_, Opt::audio_on).Result), paused_);
    engine_mixer_bus(n4194346, mixer_, audio_Bus::sfx);
    n4194346.result.volume = kern::formula<f32>([](auto the_result) { return (the_result / 72.0f); }, settings::Settings_get(settings_, Opt::audio_fx).Result);
    engine_mixer_bus(n4194347, mixer_, audio_Bus::ambience);
    n4194347.result.volume = kern::formula<f32>([](auto the_result) { return (the_result / 46.0f); }, settings::Settings_get(settings_, Opt::audio_ambient).Result);
    s = SoundState{EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f}, 0.0f, 1.0f, 1.0f};
    s.drone_mix = kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return ((the_result / 50.0f) / fmax(0.01f, (the_result_2 / 72.0f))); }, settings::Settings_get(settings_, Opt::audio_drone).Result, settings::Settings_get(settings_, Opt::audio_fx).Result);
    if (placed_) {
      w = 0.0f;
      for (auto it4194348 = flow_for(n4194348, 0, math_sub<i64>(list_length<f32>(drone_.tele_.omega), 1)); it4194348.next();) {
        o = drone_.tele_.omega[n4194348.index];
        w += o;
      }
      w = kern::formula<f32>([](auto w) { return (w / 4.0f); }, w);
      s.drone.active = math_compare<">", f32>(w, 5.0f);
      n4194349 = drone_position();
      s.drone.position = n4194349.Result;
      the_actor_velocity = engine_game_world_actor_velocity(world_, drone_actor_);
      s.drone.velocity = the_actor_velocity;
      s.drone.rate = common::clampf(kern::formula<f32>([](auto w) { return (w / 3600.0f); }, w), 0.0f, 1.2f).Result;
      s.drone.load = common::clampf(kern::formula<f32>([](auto current_a) { return (current_a / 120.0f); }, drone_.tele_.current_a), 0.0f, 1.0f).Result;
      length = engine_length_2(s.drone.velocity);
      s.wind = length;
    }
    sound_fx_cpp::SoundFx_update(fx_, s, dt);
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
    n4194350 = drone_position();
    p = n4194350.Result;
    b = text_format<",\"walk\":{\"dead\":0,\"air\":0,\"p\":[%.2f,%.2f,%.2f]}">(511, num_cast<f64, f32>(vector_break(p).x), num_cast<f64, f32>(vector_break(p).y), num_cast<f64, f32>(vector_break(p).z));
    s = text_append(s, b, "");
    if (placed_) {
      b = text_format<",\"drone\":{\"st\":%u,\"arm\":%d,\"d\":%.4f,\"w\":[%.2f,%.2f,%.2f,%.2f],\"mt\":[%.5f,%.5f,%.5f,%.5f],\"pt\":[%.5f,%.5f,%.5f,%.5f],\"p\":[%.2f,%.2f,%.2f]}">(511, num_cast<i64, u32>(drone_.tele_.state), num_cast<i64, i32>(kern::formula<bool, i32, i32>([](auto arm, auto a1, auto a0) { return (arm ? a1 : a0); }, sticks_.arm, 1, 0)), num_cast<f64, f32>(drone_.p_.prop_d), num_cast<f64, f32>(drone_.tele_.omega[0]), num_cast<f64, f32>(drone_.tele_.omega[1]), num_cast<f64, f32>(drone_.tele_.omega[2]), num_cast<f64, f32>(drone_.tele_.omega[3]), num_cast<f64, f32>(drone_.tele_.motor_torque[0]), num_cast<f64, f32>(drone_.tele_.motor_torque[1]), num_cast<f64, f32>(drone_.tele_.motor_torque[2]), num_cast<f64, f32>(drone_.tele_.motor_torque[3]), num_cast<f64, f32>(drone_.tele_.prop_torque[0]), num_cast<f64, f32>(drone_.tele_.prop_torque[1]), num_cast<f64, f32>(drone_.tele_.prop_torque[2]), num_cast<f64, f32>(drone_.tele_.prop_torque[3]), num_cast<f64, f32>(vector_break(p).x), num_cast<f64, f32>(vector_break(p).y), num_cast<f64, f32>(vector_break(p).z));
      s = text_append(s, b, "");
    }
    s = text_append(s, ",\"jp\":[0,0],\"rj\":[],\"pc\":[],\"pd\":[],\"fj\":[],\"gm\":[],\"rk\":[],\"rp\":[],\"gps\":0}", "");
    return {s};
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
      n4194352 = settings_cpp::Settings_schema_json(settings_, settings_table_);
      return {n4194352.Result};
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
          n4194354 = Settings_set_2(settings_, settings_table_, text_sub(pair, 0, num_cast<i64, u64>(sp)), text_sub(pair, num_cast<i64, u64>(kern::formula<u64>([](auto sp) { return (sp + 1); }, sp)), -1));
          both = n4194354.Result;
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
      n4194355 = settings_cpp::find_opt(settings_table_, o, text_sub(cmd, 4, -1));
      if (logic_not(n4194355.Result)) {
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

  struct pointer_matchResult { bool Result = false; };
  KERN_PURE pointer_matchResult pointer_match(const TouchPlay_Finger& f, i32 id = 0) {
    return {math_compare<"==", i32>(f.id, id)};
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
  struct { bool mic = false; bool mute = false; bool suppressWeapon = false; bool r3 = false; bool y = false; bool latched = false; } n37;
  struct : kern::Node<38> { i64 index = 0; } n38;
  struct { bool neutral = false; bool armed = false; } n39;
  struct { f64 power = 0.0; } n40;
  struct { f64 mps = 0.0; } n41;
  struct { Text line = ""; } n42;
  city_stream_cpp::CityStream_take_requestsResult n1048586;
  terrain_cpp::Terrain_take_requestsResult n1048587;
  terrain_cpp::Terrain_take_image_requestsResult n1048588;
  audio_initResult n1048589;
  audio_renderResult n1048590;
  struct : kern::Node<2097162> { i64 index = 0; } n2097162;
  struct { f64 value = 0.0; } n2097163;
  settings_cpp::find_optResult n2097164;
  struct { bool known = false; f64 value = 0.0; } n2097165;
  struct { bool in = false; } n2097166;
  struct { bool result = false; Mat4 out = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}; } n4194325;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194326;
  struct { Object<kern::Input> result = Object<kern::Input>{}; } n4194328;
  gamepad_axisResult n4194329;
  gamepad_axisResult n4194330;
  gamepad_axisResult n4194331;
  gamepad_axisResult n4194332;
  height_axisResult n4194333;
  touch_play_cpp::TouchPlay_take_pressedResult n4194334;
  touch_play_cpp::TouchPlay_take_pressedResult n4194335;
  touch_play_cpp::TouchPlay_take_pressedResult n4194336;
  struct { bool result = false; } n4194337;
  struct { Object<kern::game::SoundBank> result = Object<kern::game::SoundBank>{}; } n4194338;
  struct { Object<kern::game::GameAudio> result = Object<kern::game::GameAudio>{}; } n4194339;
  sound_fx_cpp::SoundFx_initResult n4194340;
  struct { List<f32> out = List<f32>{}; } n4194341;
  struct : kern::Node<4194342> { i64 index = 0; } n4194342;
  struct : kern::Node<4194343> { i64 index = 0; } n4194343;
  struct { bool result = false; } n4194344;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n4194345;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n4194346;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n4194347;
  struct : kern::Node<4194348> { i64 index = 0; } n4194348;
  drone_positionResult n4194349;
  drone_positionResult n4194350;
  settings_cpp::Settings_schema_jsonResult n4194352;
  Settings_set_2Result n4194354;
  settings_cpp::find_optResult n4194355;
};
