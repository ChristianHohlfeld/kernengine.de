struct SpawnCity : virtual kern::Blueprint, virtual city_stream, virtual city_stream_cpp, virtual common, virtual drone_cpp, virtual engine_game_runtime, virtual engine_math_math, virtual engine_physics_physics, virtual settings, virtual sound_fx_cpp, virtual spiel, virtual terrain_cpp, virtual touch_play_cpp {
  // ---- variables ----
  PadState pad = PadState{false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0, 0, false};
  Object<kern::Engine> engine_ = Object<kern::Engine>{};
  Object<kern::game::GameWorld> world_ = Object<kern::game::GameWorld>{};
  U32 drone_actor_ = 0;
  CityStream city_ = CityStream{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 2600, 1, List<Vector>{}, 1, false, 0, 0, List<Vec4>{}, CityStream_NearCollision{physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, 1000000, List<Int64_Int_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0}, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, 0.0, 0.0, 0.0, "", 0.0, 0, 0, 0, Int64{0x0}, Vector{0.0, 0.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, Map<Int, CityStream_Damage>{}, List<U32>{}, false, Object<kern::Renderer>{}, Object<kern::physics::World>{}, GeoProjector{0.0, 0.0, 1.0}, TileKey{14, 0, 0}, Map<Int64, List<CityStream_Tile>>{}, List<TileKey>{}, List<Lane>{}, false, 0, StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}, Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<Int64, Terrain_Image>{}, List<TileKey>{}, Vector{1000000000.0, 1000000000.0, 0.0}, Object<kern::Renderer>{}, Object<kern::physics::World>{}, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0, false, Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<Int64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<Int64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<Int64, Terrain_RenderTile>{}, Map<Int64, Terrain_CollisionTile>{}, Map<Int64, Int>{}, Map<Int64, Bool>{}, Set<Int64>{}, Set<Int64>{}, Set<Int64>{}, List<Int64>{}, List<Int64>{}, List<Int64>{}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false}}, List<Vec4>{}, Int64{0x0}, -1};
  Drone drone_ = Drone{DroneParams{0.225, 0.127, 2300.0, 0.08, 1.1e-05, 0.105, 0.012, 45.0, 45.0, 4.0, 1.5, 0.055, 180.0, 1.225, 0.35, 0.18, Vector{0.0, 0.0, 0.0}, 0.18, 0.035, 0.005, 0.2, 0.03, 0.02, 0.03, 0.1, 0.015, -0.015, -0.07, 0.028}, fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}, fc_PT1{100.0, 0.0, false}}, List<fc_PT1>{fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}, fc_PT1{30.0, 0.0, false}}}, fc_Controller{fc_Attitude{0.0, 0.0, 0.0}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.003, 0.0007, 1e-05, 0.18, 0.38, 55.0}, fc_PT1{55.0, 0.0, false}, 0.0, 0.0, false}, fc_PID{fc_Gains{0.0015, 0.0007, 0.0, 0.14, 0.28, 40.0}, fc_PT1{40.0, 0.0, false}, 0.0, 0.0, false}}, fc_ArmState{false, false, Int64{0x0}}, false, 0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0, false, false, false, false, false, false, false, 0.0, fc_V3{0.0, 0.0, 0.0}, fc_V3{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.39, fc_StateControllerDebug{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}}, 0.0, false}, fc_RC{List<U16>{}, false, false, false, Int64{0x0}}, fc_NavigationState{fc_V3{0.0, 0.0, 0.0}, 0.0, false, false, false, 0.0, false}, Int64{0x0}, 0, false}, List<Vector>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 1.0, 16.8, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, List<U16>{1000, 1000, 1000, 1000}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0.0, 1, 0.01, 0.01, Vector{0.0, 0.0, 0.0}, 1, true, Rng{1168318247, 0.0, false}, Rng{1905503009, 0.0, false}, Rng{1168232869, 0.0, false}, List<Float>{0.08, -0.05, 0.04}, List<Float>{0.001, -0.0015, 0.002}, 0, false, DroneTelemetry{false, true, false, false, false, 0, List<U16>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, List<Float>{}}};
  DroneSticks sticks_ = DroneSticks{0.0, 0.0, 0.0, 0.0, 0.0, 1.2, false, true, false};
  Settings settings_ = Settings{List<Float>{}, 1};
  TouchPlay touch_ = TouchPlay{960.0, 540.0, 0.0, 0.0, 0.0, 0.0, TouchLayout{TouchMode::off, true, "", false, false, false, 0, -1.0, false, false, false, true, false, false, "FPV", false, false, false, false, false, false}, List<TouchBtn>{}, List<TouchPlay_Finger>{}, List<Text>{}, List<Text>{}, List<TouchFire>{}, -1, -1, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, 0.0, false, List<TouchPlay_DomRect>{}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
  /// a finger was last (the touch controls show)
  Bool touch_mode_ = false;
  /// mouse movement since the last frame (CSS px)
  Vector mouse_px_ = Vector{0.0, 0.0, 0.0};
  /// the pointer lock
  Bool locked_ = false;
  Float lon_ = 0.0;
  Float lat_ = 0.0;
  Bool placed_ = false;
  Bool paused_ = false;
  Bool landing_ = false;
  Bool height_held_ = false;
  /// the time the fixed steps have not taken yet (s)
  Float accumulator_ = 0.0;
  Int64 steps_ = Int64{0x0};
  /// 0 FPV, 1 follow, 2 third (simulator.mjs camFpv / camFollow / camThird)
  I32 cam_mode_ = 0;
  Camera cam_ = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Vector cam_anchor_ = Vector{0.0, 0.0, 0.0};
  Vector cam_vel_ = Vector{0.0, 0.0, 0.0};
  Vector cam_heading_ = Vector{1.0, 0.0, 0.0};
  Bool rig_init_ = false;
  Bool focus_prev_valid_ = false;
  Text toast_ = "";
  Float toast_age_ = 99.0;
  U32 resets_ = 0;
  U32 moves_ = 0;
  /// sound: Kern's mixer (?audio=kern) or the original's on the page (audio web 1: the page asks for "audio frame")
  Object<kern::audio::Mixer> mixer_ = Object<kern::audio::Mixer>{};
  Object<kern::game::SoundBank> bank_ = Object<kern::game::SoundBank>{};
  Object<kern::game::GameAudio> game_audio_ = Object<kern::game::GameAudio>{};
  SoundFx fx_ = SoundFx{Object<kern::audio::Mixer>{}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}, SoundFx_Loop{0, Int64{0x0}, 0.0}};
  List<Float> audio_out_ = List<Float>{};
  Bool web_audio_ = false;
  KERN_CONST Float kPi = 3.141592653589793;
  /// fixed physics rate: the firmware runs 4 ticks per step (~1 kHz); the level's world.stepHz (tests/test_entry.cpp holds it)
  KERN_CONST Float kStepHz = 240.0;
  /// at most 0.1 s of simulation per frame (GameWorld::step's guard against the spiral of death)
  KERN_CONST U32 kMaxSteps = 24;
  KERN_CONST Float kDeg = 0.017453292519943295;

  // ---- functions ----
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

  /// Mouse movement under the pointer lock (movementX/Y of mousemove, CSS px, clamped +-420 per event by the page: desktop_controls.mjs MAX_EVENT_PX) and the lock state.
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

  struct sc_audio_initResult { I32 Result = 0; };
  /// ---- audio: Kern's mixer renders, the page plays (KernAudio) ----
  KERN_EXPORT_AS("sc_audio_init", "u32 -> i32") sc_audio_initResult sc_audio_init(U32 rate = 0) {
    n1048586 = audio_init(rate);
    return {kern::to<Int>(kern::formula([](Float the_result) -> Float { return ((the_result != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(n1048586.Result)))};
  }

  void pointer_locked(Bool locked = false) {
    locked_ = locked;
  }

  struct placedResult { Bool Result = false; };
  /// ---- what a test reads
  KERN_PURE placedResult placed() {
    return {placed_};
  }

  struct worldResult { Object<kern::game::GameWorld> Result = Object<kern::game::GameWorld>{}; };
  KERN_PURE worldResult world() {
    return {world_};
  }

  struct stepsResult { Int64 Result = Int64{0x0}; };
  /// The fixed steps run so far (1/240 s each).
  KERN_PURE stepsResult steps() {
    return {steps_};
  }

  void stop(Object<kern::Engine> arg1 = Object<kern::Engine>{}) {
    Object<kern::game::GameAudio> old = Object<kern::game::GameAudio>{};
    Object<kern::game::SoundBank> old_2 = Object<kern::game::SoundBank>{};
    Object<kern::game::GameWorld> old_3 = Object<kern::game::GameWorld>{};
    if (object_valid<kern::game::GameAudio>(game_audio_)) {
      engine_game_audio_end(game_audio_, 0.25);
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

  struct drone_matrixResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  drone_matrixResult drone_matrix() {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool actor_matrix = false;
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (object_valid<kern::game::GameWorld>(world_)) {
      engine_game_world_actor_matrix(n4194317, world_, drone_actor_, m);
      actor_matrix = n4194317.result;
      m = n4194317.out;
    }
    return {m};
  }

  struct drone_positionResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  drone_positionResult drone_position() {
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    physics_Body actor_body = physics_Body{Int64{0x0}};
    if (logic_or(logic_not(object_valid<kern::game::GameWorld>(world_)), logic_not(math_compare<"!=">(kern::to<Float>(drone_actor_), 0.0)))) {
      return {Vector{0.0, 0.0, 0.0}};
    }
    engine_game_world_physics(n4194318, world_);
    physics = n4194318.result;
    actor_body = engine_game_world_actor_body(world_, drone_actor_);
    return {struct_break<BodyState>(common::read_body(physics, actor_body).Result).position};
  }

  struct placeResult { Bool Result = false; };
  /// The drone on the ground at the start: the first spot round the origin that is no building, on what is there.
  placeResult place() {
    Vector at = Vector{0.0, 0.0, 0.0};
    Bool found = false;
    I32 ring = 0;
    I32 n = 0;
    I32 k = 0;
    Float a = 0.0;
    Float r = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Float ground = 0.0;
    the_RayResult_ g = the_RayResult_{false, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    physics_Body actor_body = physics_Body{Int64{0x0}};
    Vector rot = Vector{0.0, 0.0, 0.0};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    at = Vector{0.0, 0.0, 0.0};
    found = false;
    ring = 0;
    while (loop(logic_and(math_compare<"<">(kern::to<Float>(ring), 40.0), logic_not(found)))) {
      n = kern::to<Int>(kern::formula([](Float ring) -> Float { return ((ring == 0.0) ? 1.0 : (ring * 8.0)); }, kern::to<Float>(ring)));
      k = 0;
      while (loop(logic_and(math_compare<"<">(kern::to<Float>(k), kern::to<Float>(n)), logic_not(found)))) {
        a = kern::formula([](Float k, Float kPi, Float n) -> Float { return kern::math::div(((k * 2.0) * kPi), n); }, kern::to<Float>(k), kPi, kern::to<Float>(n));
        r = kern::formula([](Float ring) -> Float { return (ring * 5.0); }, kern::to<Float>(ring));
        p = vector_make(kern::formula([](Float r, Float a) -> Float { return (r * kern::math::cos(a)); }, r, a), kern::formula([](Float r, Float a) -> Float { return (r * kern::math::sin(a)); }, r, a), 0.0);
        if (logic_not(city_stream_cpp::CityStream_building_at(city_, p).Result)) {
          at = vector_make(vector_break(p).x, vector_break(p).y, 0.0);
          found = true;
        }
        k += 1;
      }
      ring += 1;
    }
    city_stream_cpp::CityStream_collide(city_, at, true);
    ground = city_stream_cpp::CityStream_ground_height(city_, vector_break(at).x, vector_break(at).y).Result;
    engine_game_world_physics(n4194319, world_);
    physics = n4194319.result;
    actor_body = engine_game_world_actor_body(world_, drone_actor_);
    g = common::cast(physics, vector_make(vector_break(at).x, vector_break(at).y, kern::formula([](Float ground) -> Float { return (ground + 60.0); }, ground)), Vector{0.0, 0.0, -1.0}, 80.0, actor_body, Int64{0xffffffffffffffff}).Result;
    at = vector_make(vector_break(at).x, vector_break(at).y, kern::formula([](Float hit, Float z, Float ground) -> Float { return (((hit != 0.0) ? z : ground) + 0.0225); }, kern::to<Float>(g.hit), vector_break(g.point).z, ground));
    engine_game_world_set_alive(world_, drone_actor_, true);
    rot = Vector{0.0, 0.0, 180.0};
    engine_game_world_teleport(n4194320, world_, drone_actor_, at, list_make<Vector, "1">(rot));
    teleport = n4194320.result;
    engine_game_world_set_actor_velocity(n4194321, world_, drone_actor_, Vector{0.0, 0.0, 0.0});
    set_actor_velocity = n4194321.result;
    drone_cpp::Drone_reset(drone_);
    sticks_ = struct_make<DroneSticks>(0.0, 0.0, 0.0, 0.0, 0.0, 1.2, false, true, false);
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
    toast_age_ = kern::formula([](Float a) -> Float { return ((a != 0.0) ? 99.0 : 0.0); }, kern::to<Float>(city_stream::CityStream_offline(city_).Result));
    return {true};
  }

  /// RESET (game_reset.mjs): the drone back at the start, on the ground, disarmed.
  void reset() {
    if (logic_not(placed_)) {
      return;
    }
    n4194322 = place();
    resets_ = kern::to<Int>(kern::formula([](Float resets_) -> Float { return (resets_ + 1.0); }, kern::to<Float>(resets_)));
    toast_ = "Zurueck zum Start";
    toast_age_ = 0.0;
  }

  /// The world moves to another place: the city streams there and the drone starts anew.
  void move_world(Float lon = 0.0, Float lat = 0.0) {
    Object<kern::Renderer> renderer = Object<kern::Renderer>{};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    Float arg_number = 0.0;
    Float arg_number_2 = 0.0;
    Float arg_number_3 = 0.0;
    Bool both = false;
    Float arg = 0.0;
    Float arg_2 = 0.0;
    Float arg_number_4 = 0.0;
    lon_ = lon;
    lat_ = lat;
    city_stream_cpp::CityStream_clear(city_);
    renderer = engine_engine_renderer(engine_);
    engine_game_world_physics(n4194323, world_);
    physics = n4194323.result;
    arg = lon_;
    arg_2 = lat_;
    arg_number = engine_engine_arg_number(engine_, "dem", 1.0);
    both = math_compare<">">(arg_number, 0.0);
    if (both) {
      arg_number_3 = engine_engine_arg_number(engine_, "offline", 0.0);
      both = math_compare<"<=">(arg_number_3, 0.0);
    }
    city_stream_cpp::CityStream_start(city_, renderer, physics, arg, arg_2, both);
    arg_number_4 = engine_engine_arg_number(engine_, "offline", 0.0);
    if (math_compare<">">(arg_number_4, 0.0)) {
      city_stream_cpp::CityStream_go_offline(city_);
    }
    engine_game_world_set_alive(world_, drone_actor_, false);
    engine_game_world_set_paused(world_, true);
    placed_ = false;
    moves_ = kern::to<Int>(kern::formula([](Float moves_) -> Float { return (moves_ + 1.0); }, kern::to<Float>(moves_)));
    toast_ = "Neuer Startort - die Stadt wird geladen";
    toast_age_ = 0.0;
  }

  /// The drone's force model in every fixed step of the world (Box3D clears forces after each step): the game takes the steps itself - one GameWorld::step_once, then the flight controller and the motors - as many as the frame's time.
  void fly(Float dt = 0.0) {
    Float h = 0.0;
    U32 n = 0;
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    physics_Body actor_body = physics_Body{Int64{0x0}};
    h = kern::formula([](Float kStepHz) -> Float { return kern::math::div(1.0, kStepHz); }, kStepHz);
    accumulator_ = kern::formula([](Float accumulator_, Float dt) -> Float { return (accumulator_ + (dt < 0.0 ? 0.0 : (0.25 < dt ? 0.25 : dt))); }, accumulator_, dt);
    n = 0;
    while (loop(logic_and(math_compare<">=">(accumulator_, h), math_compare<"<">(kern::to<Float>(n), kern::to<Float>(kMaxSteps))))) {
      n4194324 = drone_position();
      city_stream_cpp::CityStream_collide(city_, n4194324.Result, false);
      engine_game_world_step_once(world_, struct_make<game_PlayInput>(0.0, 0.0, 0.0, false, false, false, false, 0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, false, 0.0, false, false, false, false, false, 0.0, 0.0, false, 0.0));
      engine_game_world_physics(n4194325, world_);
      physics = n4194325.result;
      actor_body = engine_game_world_actor_body(world_, drone_actor_);
      drone_cpp::Drone_step(drone_, sticks_, physics, actor_body, h);
      accumulator_ = kern::formula([](Float accumulator_, Float h) -> Float { return (accumulator_ - h); }, accumulator_, h);
      steps_ = int64_add(steps_, Int64{0x1});
      n += 1;
    }
    if (math_compare<">=">(accumulator_, h)) {
      accumulator_ = kern::formula([](Float accumulator_, Float h) -> Float { return kern::math::mod(accumulator_, h); }, accumulator_, h);
    }
  }

  void mouse_move(Float dx = 0.0, Float dy = 0.0) {
    if (logic_not(locked_)) {
      return;
    }
    mouse_px_ = vector_make(kern::formula([](Float x, Float dx) -> Float { return (x + dx); }, vector_break(mouse_px_).x, dx), vector_break(mouse_px_).y, vector_break(mouse_px_).z);
    mouse_px_ = vector_make(vector_break(mouse_px_).x, kern::formula([](Float y, Float dy) -> Float { return (y + dy); }, vector_break(mouse_px_).y, dy), vector_break(mouse_px_).z);
  }

  void update_camera(Object<kern::Engine> arg1 = Object<kern::Engine>{}, Float dt = 0.0) {
    Camera c = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Object<kern::physics::World> physics = Object<kern::physics::World>{};
    physics_Body actor_body = physics_Body{Int64{0x0}};
    Vector pos = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 0.0};
    Float fov = 0.0;
    Float tilt = 0.0;
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector u = Vector{0.0, 0.0, 0.0};
    Vector heading = Vector{0.0, 0.0, 0.0};
    Float hl = 0.0;
    Bool third = false;
    Float pos_rate = 0.0;
    Float vel_rate = 0.0;
    Float head_rate = 0.0;
    Float max_lag = 0.0;
    Vector predicted = Vector{0.0, 0.0, 0.0};
    Vector lag = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Float ha = 0.0;
    Float hb = 0.0;
    Float hd_angle = 0.0;
    Float back = 0.0;
    Float upm = 0.0;
    Float ahead = 0.0;
    Float look_up = 0.0;
    Vector hd = Vector{0.0, 0.0, 0.0};
    Vector want = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    physics_RayHit hit = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    Object<kern::physics::World> physics_2 = Object<kern::physics::World>{};
    physics_Body actor_body_2 = physics_Body{Int64{0x0}};
    physics_RayHit cast_sphere = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
    c = cam_;
    c.near = 0.05;
    c.far = 2400.0;
    if (logic_not(placed_)) {
      c.eye = vector_make(0.0, 0.0, kern::formula([](Float the_result) -> Float { return (the_result + 1.68); }, city_stream_cpp::CityStream_ground_height(city_, 0.0, 0.0).Result));
      c.target = vector_add(c.eye, Vector{10.0, 0.0, 0.0});
      c.up = Vector{0.0, 0.0, 1.0};
      c.fov = kern::formula([](Float kDeg) -> Float { return (50.0 * kDeg); }, kDeg);
      cam_ = c;
      return;
    }
    engine_game_world_physics(n4194327, world_);
    physics = n4194327.result;
    actor_body = engine_game_world_actor_body(world_, drone_actor_);
    s = common::read_body(physics, actor_body).Result;
    n4194328 = drone_matrix();
    a = n4194328.Result;
    pos = engine_math_math::Mat4_translation(a).Result;
    fwd = engine_math_math::Quat_rotate(s.rotation, Vector{-1.0, 0.0, 0.0}).Result;
    up = engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 1.0}).Result;
    fov = settings::Settings_get(settings_, Opt::cam_fov).Result;
    if (math_compare<"==">(kern::to<Float>(cam_mode_), 0.0)) {
      tilt = kern::formula([](Float the_result, Float kDeg) -> Float { return (the_result * kDeg); }, settings::Settings_get(settings_, Opt::cam_fpv_tilt).Result, kDeg);
      f = vector_add(vector_scale(fwd, kern::formula([](Float tilt) -> Float { return kern::math::cos(tilt); }, tilt)), vector_scale(up, kern::formula([](Float tilt) -> Float { return kern::math::sin(tilt); }, tilt)));
      u = vector_sub(vector_scale(up, kern::formula([](Float tilt) -> Float { return kern::math::cos(tilt); }, tilt)), vector_scale(fwd, kern::formula([](Float tilt) -> Float { return kern::math::sin(tilt); }, tilt)));
      c.eye = vector_add(vector_add(pos, vector_scale(fwd, 0.102)), vector_scale(up, 0.073));
      c.target = vector_add(c.eye, vector_scale(f, 4.0));
      c.up = u;
      c.fov = kern::formula([](Float fov, Float kDeg) -> Float { return (fov * kDeg); }, fov, kDeg);
      c.near = 0.02;
      cam_ = c;
      return;
    }
    heading = vector_make(vector_break(fwd).x, vector_break(fwd).y, 0.0);
    hl = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt(((x * x) + (y * y))); }, vector_break(heading).x, vector_break(heading).y);
    heading = logic_select<Vector>(math_compare<">">(hl, 0.2), vector_scale(heading, kern::formula([](Float hl) -> Float { return kern::math::div(1.0, hl); }, hl)), cam_heading_);
    third = math_compare<"==">(kern::to<Float>(cam_mode_), 2.0);
    pos_rate = kern::formula([](Float third) -> Float { return ((third != 0.0) ? 3.6 : 4.8); }, kern::to<Float>(third));
    vel_rate = kern::formula([](Float third) -> Float { return ((third != 0.0) ? 5.2 : 7.0); }, kern::to<Float>(third));
    head_rate = kern::formula([](Float third) -> Float { return ((third != 0.0) ? 3.8 : 7.5); }, kern::to<Float>(third));
    max_lag = kern::formula([](Float third) -> Float { return ((third != 0.0) ? 0.36 : 0.18); }, kern::to<Float>(third));
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
    l = vector_length(lag);
    if (math_compare<">">(l, max_lag)) {
      cam_anchor_ = vector_sub(pos, vector_scale(lag, kern::formula([](Float max_lag, Float l) -> Float { return kern::math::div(max_lag, l); }, max_lag, l)));
    }
    ha = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(cam_heading_).y, vector_break(cam_heading_).x);
    hb = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(heading).y, vector_break(heading).x);
    hd_angle = kern::formula([](Float ha, Float the_result, Float the_result_2) -> Float { return (ha + (the_result * the_result_2)); }, ha, common::wrap_pi(kern::formula([](Float hb, Float ha) -> Float { return (hb - ha); }, hb, ha)).Result, common::damp_alpha(head_rate, dt).Result);
    cam_heading_ = vector_make(kern::formula([](Float hd_angle) -> Float { return kern::math::cos(hd_angle); }, hd_angle), kern::formula([](Float hd_angle) -> Float { return kern::math::sin(hd_angle); }, hd_angle), 0.0);
    back = kern::formula([](Float third) -> Float { return ((third != 0.0) ? kern::math::div((1.5 * 2.25), 2.4847) : 1.65); }, kern::to<Float>(third));
    upm = kern::formula([](Float third) -> Float { return ((third != 0.0) ? kern::math::div((1.5 * 1.05), 2.4847) : 0.78); }, kern::to<Float>(third));
    ahead = kern::formula([](Float third) -> Float { return ((third != 0.0) ? 0.55 : 0.38); }, kern::to<Float>(third));
    look_up = kern::formula([](Float third) -> Float { return ((third != 0.0) ? 0.18 : 0.1); }, kern::to<Float>(third));
    hd = vector_make(vector_break(cam_heading_).x, vector_break(cam_heading_).y, 0.0);
    want = vector_add(vector_sub(cam_anchor_, vector_scale(hd, back)), vector_make(0.0, 0.0, upm));
    d = vector_sub(want, cam_anchor_);
    engine_game_world_physics(n4194329, world_);
    physics_2 = n4194329.result;
    actor_body_2 = engine_game_world_actor_body(world_, drone_actor_);
    cast_sphere = engine_world_cast_sphere(physics_2, cam_anchor_, 0.17, d, engine_physics_physics::QueryFilter_new(Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}).Result, actor_body_2);
    hit = cast_sphere;
    c.eye = logic_select<Vector>(hit.hit, vector_add(cam_anchor_, vector_scale(d, kern::formula([](Float fraction) -> Float { return kern::math::max(0.0, (fraction - 0.02)); }, hit.fraction))), want);
    c.target = vector_add(vector_add(cam_anchor_, vector_scale(hd, ahead)), vector_make(0.0, 0.0, look_up));
    c.up = Vector{0.0, 0.0, 1.0};
    c.fov = kern::formula([](Float third, Float a, Float a_2, Float kDeg) -> Float { return (((third != 0.0) ? a : a_2) * kDeg); }, kern::to<Float>(third), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float fov) -> Float { return kern::math::div((62.0 * fov), 105.0); }, fov), 35.0, 100.0), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float fov) -> Float { return kern::math::div((52.0 * fov), 105.0); }, fov), 30.0, 90.0), kDeg);
    c.near = 0.06;
    cam_ = c;
  }

  void set_touch_frame(Float w = 0.0, Float h = 0.0, Float sl = 0.0, Float st = 0.0, Float sr = 0.0, Float sb = 0.0, Float arg7 = 0.0) {
    touch_play_cpp::TouchPlay_set_frame(touch_, w, h, sl, st, sr, sb);
  }

  struct touch_view_jsonResult { Text Result = ""; };
  touch_view_jsonResult touch_view_json() {
    Text raw = "";
    Text out = "";
    Int c = 0;
    if (logic_not(touch_mode_)) {
      return {"null"};
    }
    n4194330 = touch_play_cpp::TouchPlay_view_json(touch_);
    raw = n4194330.Result;
    out = "";
    for (auto it4194331 = flow_for(n4194331, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(text_length(raw))))); it4194331.next();) {
      c = kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(raw, n4194331.index))));
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
    engine_mixer_init(n4194332, mixer_, rate, 64);
    init = n4194332.result;
    if (logic_not(init)) {
      return {false};
    }
    old = bank_;
    engine_sound_bank_make(n4194333, mixer_);
    sound_bank = n4194333.result;
    bank_ = sound_bank;
    if (object_valid<kern::game::SoundBank>(old)) {
      engine_sound_bank_release(old);
    }
    old_2 = game_audio_;
    engine_game_audio_make(n4194334, mixer_, bank_);
    game_audio = n4194334.result;
    game_audio_ = game_audio;
    if (object_valid<kern::game::GameAudio>(old_2)) {
      engine_game_audio_release(old_2);
    }
    if (object_valid<kern::game::GameWorld>(world_)) {
      engine_game_audio_begin(game_audio_, world_);
    }
    n4194335 = sound_fx_cpp::SoundFx_init(fx_, mixer_);
    return {n4194335.Result};
  }

  struct audio_renderResult { List<Float> Result = List<Float>{}; };
  audio_renderResult audio_render(U32 frames = 0) {
    Bool ready = false;
    Int at = 0;
    Int last = 0;
    list_resize(audio_out_, kern::to<Int>(kern::formula([](Float frames) -> Float { return (frames * 2.0); }, kern::to<Float>(frames))), 0.0);
    ready = engine_mixer_ready(mixer_);
    if (logic_and(ready, logic_not(web_audio_))) {
      at = 0;
      engine_mixer_render(n4194336, mixer_, frames);
      last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(n4194336.out))));
      for (auto it4194337 = flow_for(n4194337, 0, last); it4194337.next();) {
        audio_out_[kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(at), kern::to<Float>(n4194337.index)))] = list_get<Float>(n4194336.out, n4194337.index);
      }
    } else {
      for (auto it4194338 = flow_for(n4194338, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(audio_out_))))); it4194338.next();) {
        audio_out_[n4194338.index] = 0.0;
      }
    }
    return {audio_out_};
  }

  void update_sound(Float dt = 0.0) {
    Bool ready = false;
    Bool prepare = false;
    SoundState s = SoundState{EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, 0.0, 1.0, 1.0};
    Float w = 0.0;
    Float o = 0.0;
    Vector the_actor_velocity = Vector{0.0, 0.0, 0.0};
    ready = engine_mixer_ready(mixer_);
    if (logic_or(logic_not(ready), logic_not(object_valid<kern::game::GameWorld>(world_)))) {
      return;
    }
    engine_sound_bank_prepare(n4194339, bank_, 3.0);
    prepare = n4194339.result;
    engine_game_audio_update(game_audio_, world_, cam_, dt);
    engine_mixer_bus(n4194340, mixer_, audio_Bus::master);
    n4194340.result.muted = logic_or(logic_not(settings::Settings_on(settings_, Opt::audio_on).Result), paused_);
    engine_mixer_bus(n4194341, mixer_, audio_Bus::sfx);
    n4194341.result.volume = kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 72.0); }, settings::Settings_get(settings_, Opt::audio_fx).Result);
    engine_mixer_bus(n4194342, mixer_, audio_Bus::ambience);
    n4194342.result.volume = kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 46.0); }, settings::Settings_get(settings_, Opt::audio_ambient).Result);
    s = SoundState{EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, EngineSound{false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0}, 0.0, 1.0, 1.0};
    s.drone_mix = kern::formula([](Float the_result, Float the_result_2) -> Float { return kern::math::div(kern::math::div(the_result, 50.0), kern::math::max(0.01, kern::math::div(the_result_2, 72.0))); }, settings::Settings_get(settings_, Opt::audio_drone).Result, settings::Settings_get(settings_, Opt::audio_fx).Result);
    if (placed_) {
      w = 0.0;
      for (auto it4194343 = flow_for(n4194343, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(drone_.tele_.omega))))); it4194343.next();) {
        o = drone_.tele_.omega[n4194343.index];
        w += o;
      }
      w = kern::formula([](Float w) -> Float { return kern::math::div(w, 4.0); }, w);
      s.drone.active = math_compare<">">(w, 5.0);
      n4194344 = drone_position();
      s.drone.position = n4194344.Result;
      the_actor_velocity = engine_game_world_actor_velocity(world_, drone_actor_);
      s.drone.velocity = the_actor_velocity;
      s.drone.rate = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float w) -> Float { return kern::math::div(w, 3600.0); }, w), 0.0, 1.2));
      s.drone.load = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float current_a) -> Float { return kern::math::div(current_a, 120.0); }, drone_.tele_.current_a), 0.0, 1.0));
      s.wind = vector_length(s.drone.velocity);
    }
    sound_fx_cpp::SoundFx_update(fx_, s, dt);
  }

  struct audio_frameResult { Text Result = ""; };
  /// What the original's sound modules read (web/audio.mjs, every frame): the listener, the drone's motors, the settings in the original's names (audio_settings.mjs normalizeAudioSettings). The other parts of the original's state (on foot, the car, jets, police, missiles) are empty: the small game has none of them.
  audio_frameResult audio_frame() {
    Text s = "";
    Text b = "";
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Vector p = Vector{0.0, 0.0, 0.0};
    s = "{\"e\":[]";
    b = "";
    fwd = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(cam_.target, cam_.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(cam_.target, cam_.eye)), Vector{0.0, 1.0, 0.0});
    b = text_format<",\"paused\":%d,\"mode\":\"drone\",\"fwd\":[%.3f,%.3f,%.3f],\"L\":[%.2f,%.2f,%.2f]">(511, kern::to<Int>(kern::formula([](Float paused_) -> Float { return ((paused_ != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(paused_))), vector_break(fwd).x, vector_break(fwd).y, vector_break(fwd).z, vector_break(cam_.eye).x, vector_break(cam_.eye).y, vector_break(cam_.eye).z);
    s = text_append(s, b, "");
    b = text_format<",\"set\":{\"soundEnabled\":%d,\"musicEnabled\":%d,\"musicVolume\":%.0f,\"droneVolume\":%.0f,\"shotsVolume\":%.0f,\"fxVolume\":%.0f,\"footstepsVolume\":%.0f,\"vehicleVolume\":%.0f,\"ambientVolume\":%.0f}">(511, kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(settings_, Opt::audio_on).Result))), kern::to<Int>(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(settings::Settings_on(settings_, Opt::music_on).Result))), settings::Settings_get(settings_, Opt::music_volume).Result, settings::Settings_get(settings_, Opt::audio_drone).Result, settings::Settings_get(settings_, Opt::audio_shots).Result, settings::Settings_get(settings_, Opt::audio_fx).Result, settings::Settings_get(settings_, Opt::audio_steps).Result, settings::Settings_get(settings_, Opt::audio_vehicle).Result, settings::Settings_get(settings_, Opt::audio_ambient).Result);
    s = text_append(s, b, "");
    n4194345 = drone_position();
    p = n4194345.Result;
    b = text_format<",\"walk\":{\"dead\":0,\"air\":0,\"p\":[%.2f,%.2f,%.2f]}">(511, vector_break(p).x, vector_break(p).y, vector_break(p).z);
    s = text_append(s, b, "");
    if (placed_) {
      b = text_format<",\"drone\":{\"st\":%u,\"arm\":%d,\"d\":%.4f,\"w\":[%.2f,%.2f,%.2f,%.2f],\"mt\":[%.5f,%.5f,%.5f,%.5f],\"pt\":[%.5f,%.5f,%.5f,%.5f],\"p\":[%.2f,%.2f,%.2f]}">(511, drone_.tele_.state, kern::to<Int>(kern::formula([](Float arm) -> Float { return ((arm != 0.0) ? 1.0 : 0.0); }, kern::to<Float>(sticks_.arm))), drone_.p_.prop_d, drone_.tele_.omega[0], drone_.tele_.omega[1], drone_.tele_.omega[2], drone_.tele_.omega[3], drone_.tele_.motor_torque[0], drone_.tele_.motor_torque[1], drone_.tele_.motor_torque[2], drone_.tele_.motor_torque[3], drone_.tele_.prop_torque[0], drone_.tele_.prop_torque[1], drone_.tele_.prop_torque[2], drone_.tele_.prop_torque[3], vector_break(p).x, vector_break(p).y, vector_break(p).z);
      s = text_append(s, b, "");
    }
    s = text_append(s, ",\"jp\":[0,0],\"rj\":[],\"pc\":[],\"pd\":[],\"fj\":[],\"gm\":[],\"rk\":[],\"rp\":[],\"gps\":0}", "");
    return {s};
  }

  struct command_startsResult { Bool Result = false; };
  KERN_PURE command_startsResult command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  // ---- what nodes give and keep ----
  audio_initResult n1048586;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n4194317;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194318;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194319;
  struct { Bool result = false; } n4194320;
  struct { Bool result = false; } n4194321;
  placeResult n4194322;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194323;
  drone_positionResult n4194324;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194325;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194327;
  drone_matrixResult n4194328;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n4194329;
  touch_play_cpp::TouchPlay_view_jsonResult n4194330;
  struct : kern::Node<4194331> { Int index = 0; } n4194331;
  struct { Bool result = false; } n4194332;
  struct { Object<kern::game::SoundBank> result = Object<kern::game::SoundBank>{}; } n4194333;
  struct { Object<kern::game::GameAudio> result = Object<kern::game::GameAudio>{}; } n4194334;
  sound_fx_cpp::SoundFx_initResult n4194335;
  struct { List<Float> out = List<Float>{}; } n4194336;
  struct : kern::Node<4194337> { Int index = 0; } n4194337;
  struct : kern::Node<4194338> { Int index = 0; } n4194338;
  struct { Bool result = false; } n4194339;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n4194340;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n4194341;
  struct { engine_mixer_bus_place<audio_BusSettings> result = {}; } n4194342;
  struct : kern::Node<4194343> { Int index = 0; } n4194343;
  drone_positionResult n4194344;
  drone_positionResult n4194345;
};
