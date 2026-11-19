struct Game : virtual kern::Blueprint, virtual city_stream, virtual combat_audio_cpp, virtual common, virtual engine_game_runtime, virtual engine_math_math, virtual engine_physics_physics, virtual equipment, virtual equipment_cpp, virtual game, virtual game_controls, virtual game_equipment_cpp, virtual game_ui, virtual jet, virtual settings {
  // ---- variables ----
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
  Bool rig_init_ = false;
  /// game_controls.h
  ControlsState ctl_ = ControlsState{controls_Devices{true, false, 0, true, false, 0}, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0, false, false, false, controls_ViewButton{false, false, 0.0, -99.0}, controls_VoiceChord{false, false, false}, controls_ReleaseLatch{false}, false, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, controls_LookAxis{0.0}, false, false, controls_RateStick{1500.0, 0.05, Vector{0.0, 0.0, 0.0}}, controls_RateStick{730.0, 0.025, Vector{0.0, 0.0, 0.0}}, controls_WorldLook{0.0, 0.0, false, 0.0, false}, controls_CarHeadSpring{0.0, 0.0}, 1.0, false, false, false, controls_GrenadeCook{3, 0.0, false, 0.0, 0.7, 0}, false, 0.0, false, false, false, false, 0.0, 0.0, -1, false, -1.0, -1.0, -1.0, 0, false, List<Vector>{}, 0, "", 0.0, 0.0, 0, 0, false, false, false, false, false, Vector{0.0, 0.0, 0.0}, 0.0, false, false, 0};
  Fixed<Int, 16> sfx_cursor_ = List<Int>{};
  Bool combat_audio_ready_ = false;
  Bool player_dead_ = false;
  /// vs_combat_presentation.mjs: down until the player asks (R / Enter / Space, pad A / Y, the RESPAWN button) - not before 3 s (RESPAWN_COOLDOWN_MS); back 3..5 m from where he went down
  Float died_at_ = -1.0;
  /// rules / feedback (the wanted level is the "Wanted" Blueprint's: wanted.h)
  Text toast_ = "";
  Float toast_age_ = 99.0;
  Rng rng_ = Rng{20261029, 0.0, false};
  /// audio
  Int mixer_ = 0;
  Int bank_ = 0;
  /// menus, settings, start, equipment, presentation (game_ui.h)
  UiState ui_ = UiState{Settings{List<Float>{}, 1}, SettingsTable{List<Option>{}, List<OptSpec>{}}, 0, false, false, 0, false, 0.0, 0.0, 0, 0, 0, Vector{1000000000.0, 1000000000.0, 0.0}, 0.0, -1, -1, GpsLive{GpsLive_View::eye, 0, 0, false, false, false, false, 0.0, -1000000000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}}, 0, "", Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, 0, 0, 0, List<Vector>{}, false, 0.0, false, Jetpack{1.0, false, false, 0, 0.0, -1.0, -99.0}, false, false, 0.0, List<Int64>{}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, 0, 0, 0, 0, 0, 0, FlightLog{List<Flight>{}, List<FlightLog_Event>{}, Flight{0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, "", List<Vector>{}}, false, false, Vector{0.0, 0.0, 0.0}, 0.0, -1000000000.0, 1, 0}, "", MusicSynth{0.0, 0, -2, -2, 0.0, 0.0, 1.0, Int64{0x0}, 0, 1, 0, 1, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, MusicSynth_Voice{0.0, 0.0, 0.0, 0.9995, 0.0, 0.0}, List<MusicSynth_Voice>{}, 0.0, 0.0, List<Float>{}, 0}, 0, true, "", "", 99.0, Vector{0.0, 0.0, 0.0}, false, 0};
  /// (kept by sfx_id)
  List<Text> sfx_id_ids = List<Text>{};
  KERN_CONST Float kRespawnCooldown = 3.0;
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

  struct matrix_ofResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  matrix_ofResult matrix_of(Int a = 0) {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool actor_matrix = false;
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    engine_game_world_actor_matrix(n1048586, world_, a, m);
    actor_matrix = n1048586.result;
    m = n1048586.out;
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
    for (auto it1048591 = flow_for(n1048591, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(city_.lanes_))))); it1048591.next();) {
      skip = false;
      if (logic_or(logic_not(city_.lanes_[n1048591.index].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(city_.lanes_[n1048591.index].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(city_.lanes_[n1048591.index].points))), 0.5))) {
          a = city_.lanes_[n1048591.index].points[k];
          b = city_.lanes_[n1048591.index].points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))];
          ab = vector_sub(b, a);
          l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(ab).x, vector_break(ab).y);
          t = kern::formula([](Float l2, Float a) -> Float { return ((l2 > 1e-06) ? a : 0.0); }, l2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(c).x, vector_break(a).x, vector_break(ab).x, vector_break(c).y, vector_break(a).y, vector_break(ab).y, l2), 0.0, 1.0));
          q = vector_add(a, vector_scale(ab, t));
          if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(q).x, vector_break(c).y, vector_break(q).y), kern::formula([](Float reach, Float a) -> Float { return (reach + (a * 0.5)); }, reach, city_.lanes_[n1048591.index].width))) {
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
    engine_game_world_set_mode(n1048595, world_, game_SimMode::simulate);
    set_mode = n1048595.result;
  }

  void eject() {
    BodyState s = BodyState{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool teleport = false;
    Bool set_actor_velocity = false;
    n1048600 = state_of(jet_actor_);
    s = n1048600.Result;
    jet_.pilot = false;
    engine_game_world_set_alive(world_, chute_actor_, true);
    engine_game_world_teleport(n1048601, world_, chute_actor_, vector_add(s.position, engine_math_math::Quat_rotate(s.rotation, Vector{0.0, 0.0, 2.8}).Result), List<Vector>{});
    teleport = n1048601.result;
    engine_game_world_set_actor_velocity(n1048602, world_, chute_actor_, vector_add(s.velocity, engine_math_math::Quat_rotate(s.rotation, vector_make(0.0, 0.0, kEjectUp)).Result));
    set_actor_velocity = n1048602.result;
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
    n1048605 = common::Rng_next(rng_);
    start_2 = n1048605.Result;
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
    engine_game_world_physics(n1048606, world_);
    physics = n1048606.result;
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

  struct details_command_startsResult { Bool Result = false; };
  KERN_PURE details_command_startsResult details_command_starts(const Text& cmd, Text p = "") {
    return {math_compare<"==">(kern::to<Float>(text_rfind(cmd, p, 0)), 0.0)};
  }

  struct sfx_idResult { Text Result = ""; };
  /// "sc:<kind><variant>" for every sound of the bank (built once).
  sfx_idResult sfx_id(combat_audio_Kind k = combat_audio_Kind::shot, Int v = 0) {
    if (math_compare<"==">(kern::to<Float>(list_length<Text>(sfx_id_ids)), 0.0)) {
      for (auto it2097164 = flow_for(n2097164, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<combat_audio_Kind>(combat_audio_Kind::count))))); it2097164.next();) {
        for (auto it2097165 = flow_for(n2097165, 0, 3); it2097165.next();) {
          list_add(sfx_id_ids, text_append(text_append("sc:", combat_audio_cpp::kind_name(enum_from_index<combat_audio_Kind>(n2097164.index).value).Result, ""), text_format<"%u">(-1, n2097165.index), ""));
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
    for (auto it2097166 = flow_for(n2097166, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<combat_audio_Kind>(combat_audio_Kind::count))))); it2097166.next();) {
      k = enum_from_index<combat_audio_Kind>(n2097166.index).value;
      for (auto it2097167 = flow_for(n2097167, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_get<Int>(kVariants, n2097166.index))))); it2097167.next();) {
        n2097168 = combat_audio_cpp::render(k, n2097167.index, kSampleRate);
        pcm = n2097168.Result;
        n2097169 = sfx_id(k, n2097167.index);
        id = n2097169.Result;
        engine_sound_bank_request(bank_, id);
        engine_sound_bank_set_pcm(n2097170, bank_, id, pcm, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(pcm)))), 1, kSampleRate);
        set_pcm = n2097170.result;
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
    n2097171 = sfx_id(k, v);
    return {n2097171.Result};
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
    n3145738 = game_equipment_cpp::render_loop(list_make<audio_SynthPatch, "2">(roar, rumble), rate, 1.5, 0.8);
    engine_mixer_adopt_clip(n3145739, m, n3145738.Result, 1, "spawncity:jetpack", true);
    adopt_clip = n3145739.result;
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
    for (auto it3145740 = flow_for(n3145740, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<audio_SynthPatch>(items))))); it3145740.next();) {
      synthesize = engine_audio_synthesize(items[n3145740.index], rate);
      engine_audio_mix_into(n3145741, w, synthesize, 0.0, rate, 1.0);
      w = n3145741.dst;
    }
    engine_audio_normalize(n3145742, w, 0.9);
    w = n3145742.samples;
    engine_mixer_adopt_clip(n3145743, m, w, 1, "spawncity:jetpack-ignition", false);
    adopt_clip_2 = n3145743.result;
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
    for (auto it3145744 = flow_for(n3145744, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<audio_SynthPatch>(items_2))))); it3145744.next();) {
      synthesize_2 = engine_audio_synthesize(items_2[n3145744.index], rate);
      engine_audio_mix_into(n3145745, w_2, synthesize_2, 0.0, rate, 1.0);
      w_2 = n3145745.dst;
    }
    engine_audio_normalize(n3145746, w_2, 0.8);
    w_2 = n3145746.samples;
    engine_mixer_adopt_clip(n3145747, m, w_2, 1, "spawncity:impact", false);
    adopt_clip_3 = n3145747.result;
    ui_.thump_clip = adopt_clip_3;
    hum = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
    hum.wave = audio_Wave::pink;
    hum.lowpass = 420.0;
    hum.highpass = 40.0;
    hum.volume = 0.7;
    hum.tremolo_depth = 0.15;
    hum.tremolo_rate = 0.13;
    n3145748 = game_equipment_cpp::render_loop(list_make<audio_SynthPatch, "1">(hum), rate, 6.0, 0.6);
    engine_mixer_adopt_clip(n3145749, m, n3145748.Result, 1, "spawncity:city", true);
    adopt_clip_4 = n3145749.result;
    ui_.city_clip = adopt_clip_4;
    made = List<Float>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float rate) -> Float { return (rate * 6.0); }, kern::to<Float>(rate))), 0.0);
    birds = made;
    r = struct_make<Rng>(4711, 0.0, false);
    for (auto it3145750 = flow_for(n3145750, 0, 13); it3145750.next();) {
      chirp = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
      chirp.wave = audio_Wave::sine;
      n3145751 = common::Rng_unit(r);
      chirp.frequency = kern::formula([](Float the_result) -> Float { return (2600.0 + (1800.0 * the_result)); }, n3145751.Result);
      n3145752 = common::Rng_unit(r);
      chirp.slide = kern::formula([](Float the_result) -> Float { return ((the_result < 0.5) ? 3.0 : (-2.0)); }, n3145752.Result);
      chirp.vibrato_depth = 2.0;
      chirp.vibrato_rate = 28.0;
      chirp.attack = 0.005;
      n3145753 = common::Rng_unit(r);
      chirp.sustain = kern::formula([](Float the_result) -> Float { return (0.03 + (0.05 * the_result)); }, n3145753.Result);
      chirp.release = 0.04;
      n3145754 = common::Rng_unit(r);
      chirp.repeat = kern::formula([](Float the_result) -> Float { return ((the_result < 0.5) ? 0.09 : 0.0); }, n3145754.Result);
      n3145755 = common::Rng_unit(r);
      chirp.volume = kern::formula([](Float the_result) -> Float { return (0.25 + (0.25 * the_result)); }, n3145755.Result);
      synthesize_3 = engine_audio_synthesize(chirp, rate);
      n3145756 = common::Rng_unit(r);
      engine_audio_mix_into(n3145757, birds, synthesize_3, kern::formula([](Float the_result) -> Float { return (5.6 * the_result); }, n3145756.Result), rate, 1.0);
      birds = n3145757.dst;
    }
    list_resize(birds, kern::to<Int>(kern::formula([](Float rate) -> Float { return (rate * 6.0); }, kern::to<Float>(rate))), 0.0);
    engine_audio_make_seamless(n3145758, birds, rate, 0.2);
    birds = n3145758.samples;
    engine_audio_normalize(n3145759, birds, 0.5);
    birds = n3145759.samples;
    engine_mixer_adopt_clip(n3145760, m, birds, 1, "spawncity:birds", true);
    adopt_clip_5 = n3145760.result;
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
    n3145761 = game_equipment_cpp::render_loop(list_make<audio_SynthPatch, "2">(cricket, pulse), rate, 4.0, 0.35);
    engine_mixer_adopt_clip(n3145762, m, n3145761.Result, 1, "spawncity:crickets", true);
    adopt_clip_6 = n3145762.result;
    ui_.crickets_clip = adopt_clip_6;
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

  void ui_render_music(List<Float>& out, Int frames = 0) {
    equipment_cpp::MusicSynth_render_add(ui_.music, out, frames);
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

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool result = false; Mat4 out = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n1048586;
  struct : kern::Node<1048591> { Int index = 0; } n1048591;
  struct { Bool result = false; } n1048595;
  state_ofResult n1048600;
  struct { Bool result = false; } n1048601;
  struct { Bool result = false; } n1048602;
  common::Rng_nextResult n1048605;
  struct { Int result = 0; } n1048606;
  struct : kern::Node<2097164> { Int index = 0; } n2097164;
  struct : kern::Node<2097165> { Int index = 0; } n2097165;
  struct : kern::Node<2097166> { Int index = 0; } n2097166;
  struct : kern::Node<2097167> { Int index = 0; } n2097167;
  combat_audio_cpp::renderResult n2097168;
  sfx_idResult n2097169;
  struct { Bool result = false; } n2097170;
  sfx_idResult n2097171;
  game_equipment_cpp::render_loopResult n3145738;
  struct { Int result = 0; } n3145739;
  struct : kern::Node<3145740> { Int index = 0; } n3145740;
  struct { List<Float> dst = List<Float>{}; } n3145741;
  struct { List<Float> samples = List<Float>{}; } n3145742;
  struct { Int result = 0; } n3145743;
  struct : kern::Node<3145744> { Int index = 0; } n3145744;
  struct { List<Float> dst = List<Float>{}; } n3145745;
  struct { List<Float> samples = List<Float>{}; } n3145746;
  struct { Int result = 0; } n3145747;
  game_equipment_cpp::render_loopResult n3145748;
  struct { Int result = 0; } n3145749;
  struct : kern::Node<3145750> { Int index = 0; } n3145750;
  common::Rng_unitResult n3145751;
  common::Rng_unitResult n3145752;
  common::Rng_unitResult n3145753;
  common::Rng_unitResult n3145754;
  common::Rng_unitResult n3145755;
  common::Rng_unitResult n3145756;
  struct { List<Float> dst = List<Float>{}; } n3145757;
  struct { List<Float> samples = List<Float>{}; } n3145758;
  struct { List<Float> samples = List<Float>{}; } n3145759;
  struct { Int result = 0; } n3145760;
  game_equipment_cpp::render_loopResult n3145761;
  struct { Int result = 0; } n3145762;
};
