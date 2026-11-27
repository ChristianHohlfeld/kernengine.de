struct SpawnCity : virtual kern::Blueprint, virtual common, virtual drone, virtual settings, virtual spiel {
  // ---- variables ----
  PadState pad = PadState{false, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0, 0, false};
  Object<kern::game::GameWorld> world_ = Object<kern::game::GameWorld>{};
  u32 drone_actor_ = 0;
  Drone drone_ = Drone{DroneParams{0.225f, 0.127f, 2300.0f, 0.08f, 1.1e-05f, 0.105f, 0.012f, 45.0f, 45.0f, 4.0f, 1.5f, 0.055f, 180.0f, 1.225f, 0.35f, 0.18f, Vector{0.0, 0.0, 0.0}, 0.18f, 0.035f, 0.005f, 0.2f, 0.03f, 0.02f, 0.03f, 0.1f, 0.015f, -0.015f, -0.07f, 0.028f}, fc_FirmwareRuntime{fc_StateRuntime{fc_Runtime{fc_Filters{List<fc_PT1>{fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}, fc_PT1{100.0f, 0.0f, false}}, List<fc_PT1>{fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}, fc_PT1{30.0f, 0.0f, false}}}, fc_Controller{fc_Attitude{0.0f, 0.0f, 0.0f}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.003f, 0.0007f, 1e-05f, 0.18f, 0.38f, 55.0f}, fc_PT1{55.0f, 0.0f, false}, 0.0f, 0.0f, false}, fc_PID{fc_Gains{0.0015f, 0.0007f, 0.0f, 0.14f, 0.28f, 40.0f}, fc_PT1{40.0f, 0.0f, false}, 0.0f, 0.0f, false}}, fc_ArmState{false, false, 0}, false, 0, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0, fc_FaultCode::kFaultNone, false}, fc_StateController{0.0f, false, false, false, false, false, false, false, 0.0f, fc_V3{0.0f, 0.0f, 0.0f}, fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.39f, fc_StateControllerDebug{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f}}, 0.0f, false}, fc_RC{List<u16>{}, false, false, false, 0}, fc_NavigationState{fc_V3{0.0f, 0.0f, 0.0f}, 0.0f, false, false, false, 0.0f, false}, 0, 0, false}, List<Vector>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 1.0f, 16.8f, 0.0f, 0.0f, 0.0f, 0.0f, Vector{0.0, 0.0, 0.0}, List<u16>{1000, 1000, 1000, 1000}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false, 0.0, 1, 0.01f, 0.01f, Vector{0.0, 0.0, 0.0}, 1, true, Rng{1168318247, 0.0f, false}, Rng{1905503009, 0.0f, false}, Rng{1168232869, 0.0f, false}, List<f32>{0.08f, -0.05f, 0.04f}, List<f32>{0.001f, -0.0015f, 0.002f}, 0, false, DroneTelemetry{false, true, false, false, false, 0, List<u16>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, List<f32>{}}};
  DroneSticks sticks_ = DroneSticks{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.2f, false, true, false};
  Settings settings_ = Settings{List<f32>{}, 1};
  /// mouse movement since the last frame (CSS px)
  Vector mouse_px_ = Vector{0.0, 0.0, 0.0};
  /// the pointer lock
  bool locked_ = false;
  bool placed_ = false;
  bool paused_ = false;
  u64 steps_ = 0;
  Camera cam_ = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471976f, 0.05f, 1800.0f, false, 20.0f, false, Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}};
  /// sound: Kern's mixer (?audio=kern) or the original's on the page (audio web 1: the page asks for "audio frame")
  Object<kern::audio::Mixer> mixer_ = Object<kern::audio::Mixer>{};
  List<f32> audio_out_ = List<f32>{};
  bool web_audio_ = false;

  // ---- functions ----
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

  struct sc_audio_renderResult { List<f32> Result = List<f32>{}; };
  KERN_EXPORT_AS("sc_audio_render", "u32 -> floats") sc_audio_renderResult sc_audio_render(u32 frames = 0) {
    n10 = audio_render(frames);
    return {n10.Result};
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

  struct drone_matrixResult { Mat4 Result = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}; };
  drone_matrixResult drone_matrix() {
    Mat4 m = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    bool actor_matrix = false;
    m = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    if (object_valid<kern::game::GameWorld>(world_)) {
      engine_game_world_actor_matrix(n2097165, world_, drone_actor_, m);
      actor_matrix = n2097165.result;
      m = n2097165.out;
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
    engine_game_world_physics(n2097166, world_);
    physics = n2097166.result;
    actor_body = engine_game_world_actor_body(world_, drone_actor_);
    return {struct_break<BodyState>(common::read_body(physics, actor_body).Result).position};
  }

  void mouse_move(f32 dx = 0.0f, f32 dy = 0.0f) {
    if (logic_not(locked_)) {
      return;
    }
    mouse_px_ = vector_make(kern::formula<f32, f32>([](auto x, auto dx) { return (x + dx); }, vector_break(mouse_px_).x, dx), vector_break(mouse_px_).y, vector_break(mouse_px_).z);
    mouse_px_ = vector_make(vector_break(mouse_px_).x, kern::formula<f32, f32>([](auto y, auto dy) { return (y + dy); }, vector_break(mouse_px_).y, dy), vector_break(mouse_px_).z);
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
      engine_mixer_render(n2097171, mixer_, frames);
      last = math_sub<i64>(list_length<f32>(n2097171.out), 1);
      for (auto it2097172 = flow_for(n2097172, 0, last); it2097172.next();) {
        audio_out_[math_add<i64>(at, n2097172.index)] = list_get<f32>(n2097171.out, n2097172.index);
      }
    } else {
      for (auto it2097173 = flow_for(n2097173, 0, math_sub<i64>(list_length<f32>(audio_out_), 1)); it2097173.next();) {
        audio_out_[n2097173.index] = 0.0f;
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
    n2097180 = drone_position();
    p = n2097180.Result;
    b = text_format<",\"walk\":{\"dead\":0,\"air\":0,\"p\":[%.2f,%.2f,%.2f]}">(511, num_cast<f64, f32>(vector_break(p).x), num_cast<f64, f32>(vector_break(p).y), num_cast<f64, f32>(vector_break(p).z));
    s = text_append(s, b, "");
    if (placed_) {
      b = text_format<",\"drone\":{\"st\":%u,\"arm\":%d,\"d\":%.4f,\"w\":[%.2f,%.2f,%.2f,%.2f],\"mt\":[%.5f,%.5f,%.5f,%.5f],\"pt\":[%.5f,%.5f,%.5f,%.5f],\"p\":[%.2f,%.2f,%.2f]}">(511, num_cast<i64, u32>(drone_.tele_.state), num_cast<i64, i32>(kern::formula<bool, i32, i32>([](auto arm, auto a1, auto a0) { return (arm ? a1 : a0); }, sticks_.arm, 1, 0)), num_cast<f64, f32>(drone_.p_.prop_d), num_cast<f64, f32>(drone_.tele_.omega[0]), num_cast<f64, f32>(drone_.tele_.omega[1]), num_cast<f64, f32>(drone_.tele_.omega[2]), num_cast<f64, f32>(drone_.tele_.omega[3]), num_cast<f64, f32>(drone_.tele_.motor_torque[0]), num_cast<f64, f32>(drone_.tele_.motor_torque[1]), num_cast<f64, f32>(drone_.tele_.motor_torque[2]), num_cast<f64, f32>(drone_.tele_.motor_torque[3]), num_cast<f64, f32>(drone_.tele_.prop_torque[0]), num_cast<f64, f32>(drone_.tele_.prop_torque[1]), num_cast<f64, f32>(drone_.tele_.prop_torque[2]), num_cast<f64, f32>(drone_.tele_.prop_torque[3]), num_cast<f64, f32>(vector_break(p).x), num_cast<f64, f32>(vector_break(p).y), num_cast<f64, f32>(vector_break(p).z));
      s = text_append(s, b, "");
    }
    s = text_append(s, ",\"jp\":[0,0],\"rj\":[],\"pc\":[],\"pd\":[],\"fj\":[],\"gm\":[],\"rk\":[],\"rp\":[],\"gps\":0}", "");
    return {s};
  }

  struct command_startsResult { bool Result = false; };
  KERN_PURE command_startsResult command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==", u64>(num_cast<u64, i64>(text_rfind(cmd, p, 0)), 0)};
  }

  // ---- what nodes give and keep ----
  audio_renderResult n10;
  struct { bool result = false; Mat4 out = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}; } n2097165;
  struct { Object<kern::physics::World> result = Object<kern::physics::World>{}; } n2097166;
  struct { List<f32> out = List<f32>{}; } n2097171;
  struct : kern::Node<2097172> { i64 index = 0; } n2097172;
  struct : kern::Node<2097173> { i64 index = 0; } n2097173;
  drone_positionResult n2097180;
};
