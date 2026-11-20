// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct Engine : kern::Blueprint {
  // ---- types ----
  KERN_ENUM(AlphaMode, opaque, mask, blend);
  KERN_ENUM(logic_TriggerKind, event, timer, condition, automatic);
  KERN_ENUM(game_Motion, fixed, kinematic, dynamic);
  KERN_ENUM(Key, unknown, a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z, digit0, digit1, digit2, digit3, digit4, digit5, digit6, digit7, digit8, digit9, left, right, up, down, space, enter, escape, tab, backspace, shift_left, shift_right, control_left, control_right, alt_left, alt_right, count);
  KERN_ENUM(BloomState, off, active, disabled_slow_device, unsupported);
  KERN_ENUM(AoQuality, low, medium, high, ultra);
  KERN_ENUM(audio_Bus, master, music, sfx, ambience, ui, voice, count);
  KERN_ENUM(audio_Rolloff, inverse, linear, exponential);
  KERN_ENUM(audio_Wave, square, saw, sine, triangle, noise, pink, pulse);
  KERN_ENUM(game_Shape, box, sphere, cylinder, cone, capsule, plane, torus, ramp, mesh);
  KERN_ENUM(game_ColliderShape, automatic, box, sphere, capsule, hull);
  KERN_ENUM(game_JointKind, hinge, slider, ball, fixed, distance, wheel, motor, parallel, no_collision);
  KERN_ENUM(game_ControlMode, hovercar, rolling, character, flyer, vehicle, soldier, walker);
  KERN_ENUM(game_CameraMode, chase, first_person, top_down);
  KERN_ENUM(game_WeaponKind, rifle, pistol, shotgun, sniper);
  KERN_ENUM(game_TriggerAction, goal, hazard, checkpoint, message, launch, teleport, damage, event);
  KERN_ENUM(game_MoverMode, sine, ping_pong, loop_);
  KERN_ENUM(game_GameGoal, none, collect_all, reach_goal, survive, score);
  KERN_ENUM(physics_ShapeKind, box, sphere, capsule, hull, mesh);
  KERN_ENUM(game_GameStatus, inactive, playing, won, lost);
  KERN_ENUM(game_SimMode, none, simulate, play, replica);
  KERN_ENUM(net_ClientState, denied, timed_out, error, disconnected, connecting, challenged, connected);
  KERN_ENUM(net_ConnectionError, none, desync, send_queue_full, message_too_large, bad_packet, out_of_memory);
  KERN_ENUM(physics_Surface, none, cloth, tpu, floor_);
  KERN_ENUM(physics_BodyType, fixed, kinematic, dynamic);
  KERN_ENUM(physics_JointType, revolute, distance, prismatic, wheel, weld, motor, spherical, parallel, filter);
  KERN_ENUM(net_ChannelType, reliable_ordered, unreliable_unordered);
  KERN_STRUCT(Vec4) {
    Float x = 0.0;
    Float y = 0.0;
    Float z = 0.0;
    Float w = 0.0;
  };
  KERN_STRUCT(Quat) {
    Float x = 0.0;
    Float y = 0.0;
    Float z = 0.0;
    Float w = 1.0;
  };
  KERN_STRUCT(Mat4) {
    Fixed<Float, 16> m = Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0};
  };
  KERN_STRUCT(Sphere) {
    Vector center = Vector{0.0, 0.0, 0.0};
    Float radius = -1.0;
  };
  KERN_STRUCT(Aabb) {
    Vector the_min = Vector{1e+30, 1e+30, 1e+30};
    Vector the_max = Vector{-1e+30, -1e+30, -1e+30};
  };
  KERN_STRUCT(Material) {
    Vec4 color = Vec4{1.0, 1.0, 1.0, 1.0};
    AlphaMode alpha = AlphaMode::opaque;
    Float alpha_cutoff = 0.5;
    Float roughness = 1.0;
    Float metalness = 0.0;
    Vector emissive = Vector{0.0, 0.0, 0.0};
    Bool unlit = false;
    Bool double_sided = false;
    Bool cast_shadow = true;
    Bool frustum_culled = true;
    Bool viewmodel = false;
    Bool linear_colors = false;
    Bool lines = false;
    Fixed<Int, 8> params = Fixed<Int, 8>{};
  };
  KERN_STRUCT(MeshData) {
    List<Float> positions = List<Float>{};
    List<Float> normals = List<Float>{};
    List<Float> colors = List<Float>{};
    List<Float> uvs = List<Float>{};
    List<Float> extra = List<Float>{};
    Int extra_size = 0;
    List<Int> indices = List<Int>{};
  };
  KERN_STRUCT(physics_Body) {
    Int64 id = Int64{0x0};
  };
  KERN_STRUCT(physics_Shape) {
    Int64 id = Int64{0x0};
  };
  KERN_STRUCT(physics_Joint) {
    Int64 id = Int64{0x0};
  };
  KERN_STRUCT(Rgb) {
    Float r = 1.0;
    Float g = 1.0;
    Float b = 1.0;
  };
  KERN_STRUCT(logic_Link) {
    Int from = 0;
    Text out = "";
    Int to = 0;
    Text in = "";
  };
  KERN_STRUCT(Text_Int_pair) {
    Text first = "";
    Int second = 0;
  };
  KERN_STRUCT(Text_Text_pair) {
    Text first = "";
    Text second = "";
  };
  KERN_STRUCT(logic_Node) {
    Int id = 0;
    Text type = "";
    Float x = 0.0;
    Float y = 0.0;
    List<Text_Int_pair> values = List<Text_Int_pair>{};
    List<Text_Text_pair> params = List<Text_Text_pair>{};
    Text comment = "";
  };
  KERN_STRUCT(logic_State) {
    Int id = 0;
    Text name = "";
    Float x = 0.0;
    Float y = 0.0;
    Bool initial = false;
  };
  KERN_STRUCT(logic_Transition) {
    Int id = 0;
    Int from = 0;
    Int to = 0;
    logic_TriggerKind trigger = logic_TriggerKind::event;
    Text event = "";
    Float seconds = 1.0;
    Text variable = "";
    Text label = "";
  };
  KERN_STRUCT(logic_StateMachine) {
    Int id = 0;
    Text name = "State Machine";
    List<logic_State> states = List<logic_State>{};
    List<logic_Transition> transitions = List<logic_Transition>{};
  };
  KERN_STRUCT(logic_Function) {
    Text name = "";
    List<Int> inputs = List<Int>{};
    List<Int> outputs = List<Int>{};
    List<Int> locals = List<Int>{};
    Bool pure = false;
    Bool external = false;
    Text remote = "";
    Text from = "";
    Text help = "";
  };
  KERN_STRUCT(logic_EnumType) {
    Text name = "";
    List<Text> values = List<Text>{};
    Text help = "";
    Bool project = false;
    Text from = "";
  };
  KERN_STRUCT(logic_StructType) {
    Text name = "";
    List<Int> fields = List<Int>{};
    Text help = "";
    Bool project = false;
    Text from = "";
  };
  KERN_STRUCT(logic_CommentBox) {
    Int id = 0;
    Float x = 0.0;
    Float y = 0.0;
    Float w = 400.0;
    Float h = 200.0;
    Text text = "Comment";
    Int color = 0;
  };
  KERN_STRUCT(logic_Graph) {
    List<logic_Node> nodes = List<logic_Node>{};
    List<logic_Link> links = List<logic_Link>{};
    List<Int> variables = List<Int>{};
    List<logic_StateMachine> machines = List<logic_StateMachine>{};
    List<logic_Function> functions = List<logic_Function>{};
    List<logic_EnumType> enums = List<logic_EnumType>{};
    List<logic_StructType> structs = List<logic_StructType>{};
    List<logic_CommentBox> comments = List<logic_CommentBox>{};
    Int next_id = 1;
    Bool library = false;
    List<Text> uses = List<Text>{};
  };
  KERN_STRUCT(game_BodyComponent) {
    game_Motion motion = game_Motion::dynamic;
    Float mass = 0.0;
    Float linear_damping = 0.0;
    Float angular_damping = 0.05;
    Float gravity_scale = 1.0;
    Bool bullet = false;
    Bool start_awake = true;
    Bool allow_sleep = true;
    Fixed<Bool, 3> lock_linear = Fixed<Bool, 3>{};
    Fixed<Bool, 3> lock_angular = Fixed<Bool, 3>{};
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    Vector angular_velocity = Vector{0.0, 0.0, 0.0};
    Text impact_sound = "builtin:impact";
    Float impact_threshold = 2.5;
    Vector center_of_mass = Vector{0.0, 0.0, 0.0};
    Fixed<Float, 6> inertia = Fixed<Float, 6>{};
  };
  KERN_STRUCT(game_Rng) {
    Int s = 0;
  };
  KERN_STRUCT(net_Address) {
    Int64 id = Int64{0x0};
  };
  KERN_STRUCT(Touch) {
    Int id = -1;
    Vector start = Vector{0.0, 0.0, 0.0};
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector previous = Vector{0.0, 0.0, 0.0};
  };
  KERN_STRUCT(RenderStats) {
    Int draw_calls = 0;
    Int shadow_draw_calls = 0;
    Int shadow_resolution = 0;
    Bool ao = false;
    Bool shadow_reused = false;
    Float triangles = 0.0;
    Int instances = 0;
    Int culled_meshes = 0;
    Int visible_meshes = 0;
    Float cpu_ms = 0.0;
    Float gpu_ms = -1.0;
    BloomState bloom = BloomState::off;
  };
  KERN_STRUCT(Camera) {
    Vector eye = Vector{0.0, -18.0, 8.0};
    Vector target = Vector{0.0, 0.0, 2.0};
    Vector up = Vector{0.0, 0.0, 1.0};
    Float fov = 1.0471975511965976;
    Float near = 0.05;
    Float far = 1800.0;
    Bool orthographic = false;
    Float ortho_height = 20.0;
    Bool has_projection = false;
    Mat4 projection = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
  };
  KERN_STRUCT(ShadowOptions) {
    Int resolution = 512;
    Float extent = 120.0;
    Float bias = 0.0012;
    Int filter = 9;
  };
  KERN_STRUCT(BloomOptions) {
    Bool enabled = true;
    Bool forced = false;
    Bool grade = true;
  };
  KERN_STRUCT(AoOptions) {
    Bool enabled = true;
    AoQuality quality = AoQuality::medium;
    Bool half_resolution = true;
    Float radius = 1.6;
    Float power = 2.2;
    Bool adaptive = true;
  };
  KERN_STRUCT(RendererOptions) {
    Int width = 960;
    Int height = 540;
    Bool shadows = false;
    ShadowOptions shadow = ShadowOptions{512, 120.0, 0.0012, 9};
    BloomOptions bloom = BloomOptions{true, false, true};
    AoOptions ao = AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true};
    Bool grade = true;
  };
  KERN_STRUCT(Frame) {
    Float time = 0.0;
    Float dt = 0.0;
    Int64 index = Int64{0x0};
  };
  KERN_STRUCT(audio_ClipInfo) {
    Text name = "";
    Int frames = 0;
    Int channels = 1;
    Float duration = 0.0;
    Float peak = 0.0;
    Bool loopable = false;
  };
  KERN_STRUCT(Float_row) {
    List<Float> items = List<Float>{};
  };
  KERN_STRUCT(audio_PlayParams) {
    Float volume = 1.0;
    Float pitch = 1.0;
    Float pan = 0.0;
    Bool the_loop = false;
    audio_Bus bus = audio_Bus::sfx;
    Float fade_in = 0.0;
    Float start = 0.0;
    Int priority = 0;
    Bool spatial = false;
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector velocity = Vector{0.0, 0.0, 0.0};
    Float min_distance = 1.0;
    Float max_distance = 50.0;
    audio_Rolloff rolloff = audio_Rolloff::inverse;
    Float spread = 0.0;
    Float doppler = 1.0;
  };
  KERN_STRUCT(audio_Listener) {
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector forward = Vector{1.0, 0.0, 0.0};
    Vector up = Vector{0.0, 0.0, 1.0};
    Vector velocity = Vector{0.0, 0.0, 0.0};
  };
  KERN_STRUCT(audio_BusSettings) {
    Float volume = 1.0;
    Bool muted = false;
    Float lowpass_hz = 0.0;
    Float reverb_send = 0.0;
  };
  KERN_STRUCT(audio_Meter) {
    Fixed<Float, 2> peak = Fixed<Float, 2>{};
    Fixed<Float, 2> rms = Fixed<Float, 2>{};
  };
  KERN_STRUCT(audio_Stats) {
    Int voices = 0;
    Int audible = 0;
    Int virtual_voices = 0;
    Int stolen = 0;
    Int clips = 0;
    Int64 frames_rendered = Int64{0x0};
    Float limiter_reduction_db = 0.0;
    audio_Meter master = audio_Meter{Fixed<Float, 2>{}, Fixed<Float, 2>{}};
    Fixed<audio_Meter, 6> buses = Fixed<audio_Meter, 6>{};
    Fixed<Float, 2> peak_hold = Fixed<Float, 2>{};
  };
  KERN_STRUCT(audio_SynthPatch) {
    audio_Wave wave = audio_Wave::square;
    Float volume = 0.5;
    Float attack = 0.005;
    Float sustain = 0.1;
    Float punch = 0.0;
    Float release = 0.2;
    Float frequency = 440.0;
    Float slide = 0.0;
    Float slide_accel = 0.0;
    Float min_frequency = 20.0;
    Float vibrato_depth = 0.0;
    Float vibrato_rate = 6.0;
    Float arp_ratio = 1.0;
    Float arp_time = 0.0;
    Float duty = 0.5;
    Float duty_sweep = 0.0;
    Float fm_ratio = 0.0;
    Float fm_depth = 0.0;
    Float lowpass = 0.0;
    Float lowpass_sweep = 0.0;
    Float resonance = 0.0;
    Float highpass = 0.0;
    Float tremolo_depth = 0.0;
    Float tremolo_rate = 0.0;
    Float repeat = 0.0;
    Float bitcrush = 0.0;
    Int seed = 1;
  };
  KERN_STRUCT(audio_BuiltinSound) {
    Text name = "";
    Text label = "";
    Text category = "";
    Bool the_loop = false;
  };
  KERN_STRUCT(game_MaterialDesc) {
    Vector color = Vector{0.8, 0.8, 0.8};
    Float roughness = 0.6;
    Float metalness = 0.0;
    Vector emissive = Vector{0.0, 0.0, 0.0};
    Float emissive_strength = 0.0;
    Float opacity = 1.0;
    Bool unlit = false;
    Bool cast_shadow = true;
    Bool double_sided = false;
    Text texture = "";
    Float tiling = 1.0;
  };
  KERN_STRUCT(game_MeshComponent) {
    game_Shape shape = game_Shape::box;
    Vector size = Vector{1.0, 1.0, 1.0};
    Float radius = 0.5;
    Float height = 1.0;
    Int segments = 24;
    Text asset = "";
    Bool collision = true;
    Float friction = 0.6;
    Float restitution = 0.0;
    Float rolling_resistance = 0.0;
    Text surface = "";
    game_MaterialDesc material = game_MaterialDesc{Vector{0.8, 0.8, 0.8}, 0.6, 0.0, Vector{0.0, 0.0, 0.0}, 0.0, 1.0, false, true, false, "", 1.0};
  };
  KERN_STRUCT(game_ColliderComponent) {
    game_ColliderShape shape = game_ColliderShape::automatic;
    Vector center = Vector{0.0, 0.0, 0.0};
    Vector rotation = Vector{0.0, 0.0, 0.0};
    Vector size = Vector{1.0, 1.0, 1.0};
    Float radius = 0.5;
    Float length = 1.0;
    List<Vector> points = List<Vector>{};
    Float density = 1000.0;
    Float friction = 0.6;
    Float restitution = 0.0;
    Float rolling_resistance = 0.0;
    Text surface = "";
    Bool sensor = false;
    Int category = 1;
    Int mask = 4294967295;
    Int group = 0;
    Text impact_sound = "auto";
  };
  KERN_STRUCT(game_CityComponent) {
    Int seed = 20261022;
    Int blocks = 7;
    Float pitch = 80.0;
    Float street = 16.0;
    Float max_height = 84.0;
    Bool trees = true;
    Int traffic = 22;
    Bool night_windows = true;
  };
  KERN_STRUCT(game_CityBox) {
    Vector center = Vector{0.0, 0.0, 0.0};
    Vector size = Vector{0.0, 0.0, 0.0};
  };
  KERN_STRUCT(game_CityGeometry) {
    List<MeshData> buildings = List<MeshData>{};
    List<MeshData> roads = List<MeshData>{};
    MeshData plates = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData trunk = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData crown = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    List<Float> trunk_transforms = List<Float>{};
    List<Float> crown_transforms = List<Float>{};
    List<Float> crown_tints = List<Float>{};
    List<game_CityBox> colliders = List<game_CityBox>{};
    Float half = 0.0;
    Int building_count = 0;
  };
  KERN_STRUCT(game_Transform) {
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector rotation = Vector{0.0, 0.0, 0.0};
    Vector scale = Vector{1.0, 1.0, 1.0};
  };
  KERN_STRUCT(game_WeaponComponent) {
    game_WeaponKind kind = game_WeaponKind::rifle;
    Float damage = 24.0;
    Float fire_rate = 600.0;
    Bool automatic = true;
    Int magazine = 30;
    Int reserve = 120;
    Float reload_time = 2.0;
    Float spread = 1.2;
    Float aim_spread = 0.15;
    Float recoil = 0.8;
    Float range = 300.0;
    Float headshot = 2.5;
    Float impulse = 4.0;
    Int pellets = 1;
    Float aim_fov = 42.0;
    Text fire_sound = "builtin:smg";
    Text reload_sound = "builtin:reload";
  };
  KERN_STRUCT(game_WorldSettings) {
    Text channels = "World";
    Vector gravity = Vector{0.0, 0.0, -9.80665};
    Int substeps = 4;
    Float step_hz = 60.0;
    Float time_of_day = 0.42;
    Float day_length = 0.0;
    Float sun_azimuth = 210.0;
    Float fog_density = 0.0009;
    Float exposure = 1.0;
    Float ao_radius = 1.6;
    Float shadow_distance = 80.0;
    Bool clouds = true;
    Vector ground_color = Vector{0.19, 0.27, 0.15};
    Bool ground = true;
    Float ground_friction = 0.7;
    Float ground_restitution = 0.0;
    Text ground_surface = "";
    Float contact_hertz = 30.0;
    Float contact_damping = 10.0;
    Float contact_speed = 3.0;
    Float kill_z = -50.0;
    Text title = "";
    game_GameGoal goal = game_GameGoal::none;
    Float time_limit = 0.0;
    Float score_target = 0.0;
    Int player = 0;
    Bool observer = false;
    Float master_volume = 1.0;
    Text music = "";
    Float music_volume = 0.6;
    Text ambience = "";
    Float ambience_volume = 0.6;
    Float reverb = 0.25;
    Float doppler = 1.0;
  };
  KERN_STRUCT(game_EditorCamera) {
    Vector position = Vector{-24.0, -30.0, 18.0};
    Float yaw = 51.0;
    Float pitch = -24.0;
  };
  KERN_STRUCT(game_LightComponent) {
    Vector color = Vector{1.0, 0.95, 0.85};
    Float intensity = 20.0;
    Float range = 12.0;
  };
  KERN_STRUCT(game_JointComponent) {
    game_JointKind kind = game_JointKind::hinge;
    Int target = 0;
    Vector anchor = Vector{0.0, 0.0, 0.0};
    Vector axis = Vector{0.0, 0.0, 1.0};
    Bool collide_connected = false;
    Bool enable_limit = false;
    Float lower = -45.0;
    Float upper = 45.0;
    Float reference = 0.0;
    Bool enable_motor = false;
    Float motor_speed = 0.0;
    Float max_motor_force = 100.0;
    Bool enable_spring = false;
    Float spring_hertz = 4.0;
    Float spring_damping = 0.5;
    Float length = 0.0;
    Float break_force = 0.0;
    Float break_torque = 0.0;
    Bool servo = false;
    Float servo_target = 0.0;
    Float servo_stiffness = 50.0;
    Float servo_damping = 2.0;
    Float servo_friction = 0.1;
    Text break_sound = "builtin:break";
  };
  KERN_STRUCT(game_ControllerComponent) {
    game_ControlMode mode = game_ControlMode::hovercar;
    Float max_speed = 36.0;
    Float acceleration = 24.0;
    Float turn_rate = 130.0;
    Float jump_speed = 6.0;
    Bool boost = true;
    Float boost_speed = 60.0;
    Float hover_height = 0.6;
    game_CameraMode camera = game_CameraMode::chase;
    Float camera_distance = 12.0;
    Float camera_height = 4.0;
    Float camera_fov = 62.0;
    Text engine_sound = "auto";
    Text jump_sound = "builtin:jump";
    Text boost_sound = "builtin:boost";
    Float crouch_speed = 2.6;
    Float eye_height = 1.62;
    Float look_sensitivity = 1.0;
    Bool head_bob = true;
    Bool autonomous = true;
    Text gait = "velstand";
    Bool rollers = false;
    Text actions = "training";
    Bool Kern_extras = false;
    Float servo_delay = 0.02;
  };
  KERN_STRUCT(game_PickupComponent) {
    Float points = 1.0;
    Vector burst_color = Vector{0.55, 1.0, 0.92};
    Float respawn = 0.0;
    Text sound = "builtin:pickup";
    Float heal = 0.0;
    Int ammo = 0;
  };
  KERN_STRUCT(game_TriggerComponent) {
    game_TriggerAction action = game_TriggerAction::goal;
    Text message = "";
    Bool once = true;
    Text sound = "auto";
    Float strength = 12.0;
    Int target = 0;
  };
  KERN_STRUCT(game_RotatorComponent) {
    Vector rate = Vector{0.0, 0.0, 60.0};
  };
  KERN_STRUCT(game_MoverComponent) {
    Vector offset = Vector{0.0, 0.0, 1.0};
    Float period = 2.0;
    Float phase = 0.0;
    game_MoverMode mode = game_MoverMode::sine;
  };
  KERN_STRUCT(game_AudioComponent) {
    Text sound = "builtin:hum";
    Float volume = 1.0;
    Float pitch = 1.0;
    Bool the_loop = true;
    Bool autoplay = true;
    Bool spatial = true;
    Float min_distance = 2.0;
    Float max_distance = 40.0;
    audio_Rolloff rolloff = audio_Rolloff::inverse;
    audio_Bus bus = audio_Bus::sfx;
    Float doppler = 1.0;
    Float pitch_by_speed = 0.0;
    Float delay = 0.0;
  };
  KERN_STRUCT(game_HealthComponent) {
    Float max_health = 100.0;
    Float armor = 0.0;
    Float regen_delay = 5.0;
    Float regen_rate = 12.0;
    Bool respawn = true;
    Float respawn_delay = 3.0;
    Float points = 10.0;
    Text hit_sound = "builtin:hit";
    Text death_sound = "builtin:down";
    Float explode_radius = 0.0;
    Float explode_damage = 80.0;
  };
  KERN_STRUCT(game_Actor) {
    Int id = 0;
    Text name = "";
    Int parent = 0;
    Text folder = "";
    Bool hidden = false;
    Bool locked = false;
    game_Transform transform = game_Transform{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}};
    List<Text> tags = List<Text>{};
    List<game_MeshComponent> mesh = List<game_MeshComponent>{};
    List<game_LightComponent> light = List<game_LightComponent>{};
    List<game_BodyComponent> body = List<game_BodyComponent>{};
    List<game_ColliderComponent> colliders = List<game_ColliderComponent>{};
    List<game_JointComponent> joints = List<game_JointComponent>{};
    List<game_ControllerComponent> controller = List<game_ControllerComponent>{};
    List<game_PickupComponent> pickup = List<game_PickupComponent>{};
    List<game_TriggerComponent> trigger = List<game_TriggerComponent>{};
    List<game_RotatorComponent> rotator = List<game_RotatorComponent>{};
    List<game_MoverComponent> mover = List<game_MoverComponent>{};
    List<game_CityComponent> city = List<game_CityComponent>{};
    List<game_AudioComponent> audio = List<game_AudioComponent>{};
    List<game_WeaponComponent> weapon = List<game_WeaponComponent>{};
    List<game_HealthComponent> health = List<game_HealthComponent>{};
    List<logic_Graph> logic = List<logic_Graph>{};
  };
  KERN_STRUCT(game_Level) {
    Text name = "Untitled";
    game_WorldSettings world = game_WorldSettings{"World", Vector{0.0, 0.0, -9.80665}, 4, 60.0, 0.42, 0.0, 210.0, 0.0009, 1.0, 1.6, 80.0, true, Vector{0.19, 0.27, 0.15}, true, 0.7, 0.0, "", 30.0, 10.0, 3.0, -50.0, "", game_GameGoal::none, 0.0, 0.0, 0, false, 1.0, "", 0.6, "", 0.6, 0.25, 1.0};
    List<game_Actor> actors = List<game_Actor>{};
    List<Int> textures = List<Int>{};
    List<Int> sounds = List<Int>{};
    List<Int> motions = List<Int>{};
    logic_Graph logic = logic_Graph{List<logic_Node>{}, List<logic_Link>{}, List<Int>{}, List<logic_StateMachine>{}, List<logic_Function>{}, List<logic_EnumType>{}, List<logic_StructType>{}, List<logic_CommentBox>{}, 1, false, List<Text>{}};
    game_EditorCamera camera = game_EditorCamera{Vector{-24.0, -30.0, 18.0}, 51.0, -24.0};
    Int next_id = 1;
  };
  KERN_STRUCT(game_Issue) {
    Bool error = true;
    Int actor = 0;
    Text text = "";
  };
  KERN_STRUCT(ExtensionAttribute) {
    Text key = "";
    Int size = 0;
    Text glsl = "";
  };
  KERN_STRUCT(game_ShapePlan) {
    Int actor = 0;
    physics_ShapeKind kind = physics_ShapeKind::box;
    Vector center = Vector{0.0, 0.0, 0.0};
    Quat rotation = Quat{0.0, 0.0, 0.0, 1.0};
    Vector size = Vector{1.0, 1.0, 1.0};
    Float radius = 0.5;
    Float length = 1.0;
    List<Vector> points = List<Vector>{};
    List<Int> indices = List<Int>{};
    game_ColliderComponent source = game_ColliderComponent{game_ColliderShape::automatic, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 1000.0, 0.6, 0.0, 0.0, "", false, 1, 4294967295, 0, "auto"};
    Bool automatic = false;
  };
  KERN_STRUCT(game_BodyPlan) {
    Int owner = 0;
    game_Motion motion = game_Motion::fixed;
    Bool implicit = false;
    Vector position = Vector{0.0, 0.0, 0.0};
    Quat rotation = Quat{0.0, 0.0, 0.0, 1.0};
    List<game_ShapePlan> shapes = List<game_ShapePlan>{};
  };
  KERN_STRUCT(game_PlayInput) {
    Float move_x = 0.0;
    Float move_y = 0.0;
    Float up = 0.0;
    Bool jump = false;
    Bool boost = false;
    Bool camera = false;
    Bool handbrake = false;
    Int robot = 0;
    Float look_x = 0.0;
    Float look_y = 0.0;
    Float turn_deg = 0.0;
    Float speed_scale = 1.0;
    Float zoom = 0.0;
    Float view_yaw = 0.0;
    Bool has_view_yaw = false;
    Float view_pitch = 0.0;
    Bool fire = false;
    Bool aim = false;
    Bool reload = false;
    Bool crouch = false;
    Bool interact = false;
    Float look_stick_x = 0.0;
    Float look_stick_y = 0.0;
    Bool gamepad = false;
    Float rewind = 0.0;
  };
  KERN_STRUCT(Int_Bool_pair) {
    Int first = 0;
    Bool second = false;
  };
  KERN_STRUCT(game_Hud) {
    game_GameStatus status = game_GameStatus::inactive;
    Text title = "";
    Text message = "";
    Float message_age = 0.0;
    Float score = 0.0;
    Int pickups = 0;
    Int pickups_total = 0;
    Float elapsed = 0.0;
    Float remaining = -1.0;
    Float speed = 0.0;
    Float boost = -1.0;
    Bool boosting = false;
    Bool interior = false;
    Float best = 0.0;
    Bool soldier = false;
    Float health = -1.0;
    Float max_health = 100.0;
    Float armor = 0.0;
    Int ammo = -1;
    Int magazine = 0;
    Int reserve = 0;
    Float reload = -1.0;
    Float spread = 0.0;
    Float hit_age = 99.0;
    Bool hit_head = false;
    Bool hit_kill = false;
    Float damage_age = 99.0;
    Float damage_yaw = 0.0;
    Float respawn_in = -1.0;
    Bool aiming = false;
    Float fov = 60.0;
  };
  KERN_STRUCT(game_SimStats) {
    Int bodies = 0;
    Int awake = 0;
    Int shapes = 0;
    Int joints = 0;
    Int broken_joints = 0;
    Int contacts = 0;
    Int64 steps = Int64{0x0};
  };
  KERN_STRUCT(game_SoundCue) {
    Text sound = "";
    Vector position = Vector{0.0, 0.0, 0.0};
    Float volume = 1.0;
    Float pitch = 1.0;
    Bool spatial = true;
    audio_Bus bus = audio_Bus::sfx;
    Int actor = 0;
    Bool voice = false;
    Bool silence = false;
  };
  KERN_STRUCT(game_ShotEvent) {
    Vector from = Vector{0.0, 0.0, 0.0};
    Vector to = Vector{0.0, 0.0, 0.0};
    Int shooter = 0;
    Int hit = 0;
    Bool impact = false;
    Bool head = false;
    Vector normal = Vector{0.0, 0.0, 1.0};
    Int surface = 0;
  };
  KERN_STRUCT(game_SoldierState) {
    Float health = 100.0;
    Float max_health = 100.0;
    Float armor = 0.0;
    Int ammo = 0;
    Int magazine = 0;
    Int reserve = 0;
    Float reload = -1.0;
    Float spread = 0.0;
    Float respawn_in = -1.0;
    Int hits = 0;
    Int damages = 0;
    Bool hit_head = false;
    Bool hit_kill = false;
    Bool dead = false;
    Float damage_yaw = 0.0;
  };
  KERN_STRUCT(game_GameWorld_SpawnChange) {
    Int source = 0;
    Int id = 0;
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector rotation = Vector{0.0, 0.0, 0.0};
  };
  KERN_STRUCT(physics_QueryFilter) {
    Int64 mask_bits = Int64{0xffffffffffffffff};
    Int64 category_bits = Int64{0xffffffffffffffff};
  };
  KERN_STRUCT(game_GameWorld_WalkerSensors) {
    Fixed<Float, 64> range = Fixed<Float, 64>{};
    Fixed<Int, 64> zone = Fixed<Int, 64>{};
    Float nearest = -1.0;
    Float nearest_bearing = 0.0;
    Float free_left = 0.0;
    Float free_right = 0.0;
    Vector gravity = Vector{0.0, 0.0, -1.0};
    Bool fallen = false;
    Float speed = 0.0;
    Float tof_time = -1.0;
    Int hits = 0;
    Fixed<Vector, 64> hit = Fixed<Vector, 64>{};
    Int forward_observed = 0;
  };
  KERN_STRUCT(game_GameWorld_WalkerProbe) {
    Vector position = Vector{0.0, 0.0, 0.0};
    Quat rotation = Quat{0.0, 0.0, 0.0, 1.0};
    Vector velocity = Vector{0.0, 0.0, 0.0};
    Vector spin = Vector{0.0, 0.0, 0.0};
    Vector gravity = Vector{0.0, 0.0, 0.0};
    Fixed<Bool, 2> foot_contact = Fixed<Bool, 2>{};
    Fixed<Float, 2> foot_height = Fixed<Float, 2>{};
    Bool self_contact = false;
    Bool body_contact = false;
    Fixed<Float, 14> joint = Fixed<Float, 14>{};
    Fixed<Float, 14> joint_speed = Fixed<Float, 14>{};
    Fixed<Float, 14> action = Fixed<Float, 14>{};
  };
  KERN_STRUCT(game_GameWorld_WalkerJoint) {
    Text name = "";
    Float angle = 0.0;
    Float lower = 0.0;
    Float upper = 0.0;
  };
  KERN_STRUCT(game_GameWorld_DetectorModel) {
    Float sigma_abs = 0.0;
    Float sigma_rel = 0.0;
    Float dropout = 0.0;
    Int64 seed = Int64{0x1};
  };
  KERN_STRUCT(game_GameWorld_ArtStats) {
    Int rigs = 0;
    Int decals = 0;
    Int tracers = 0;
    Int chips = 0;
    Bool view_model = false;
  };
  KERN_STRUCT(BuildingStyle) {
    List<Rgb> walls = List<Rgb>{Rgb{0.75, 0.73, 0.7}};
    List<Rgb> roofs = List<Rgb>{Rgb{0.3, 0.3, 0.32}};
    Rgb trim = Rgb{0.957, 0.937, 0.894};
    Float trim_mix = 0.45;
  };
  KERN_STRUCT(BuildingOptions) {
    Float chunk_size = 320.0;
    Int max_buildings = 2600;
    Bool cornice = true;
    Bool wall_records = true;
    BuildingStyle style = BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45};
  };
  KERN_STRUCT(GeoChunk) {
    Int x = 0;
    Int y = 0;
    Int items = 0;
    Int triangles = 0;
    Int hash = 0;
    Bool welded = false;
    MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  };
  KERN_STRUCT(MeshInput) {
    List<Float> positions = List<Float>{};
    List<Float> normals = List<Float>{};
    List<Float> colors = List<Float>{};
    List<Float> uvs = List<Float>{};
    List<Int> indices = List<Int>{};
    List<Float> joints = List<Float>{};
    List<Float> weights = List<Float>{};
    List<Float> bones = List<Float>{};
    Fixed<Float_row, 2> extension = Fixed<Float_row, 2>{};
    List<Float> transforms = List<Float>{};
    List<Float> tints = List<Float>{};
    Material material = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, Fixed<Int, 8>{}};
    Int texture = 0;
    Bool visible = true;
    Int layers = 1;
    Bool pick_bvh = true;
  };
  KERN_STRUCT(MeshLook) {
    Material material = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, Fixed<Int, 8>{}};
    Int extra_slot = 0;
    Int texture = 0;
    Bool visible = true;
    Int layers = 1;
    Bool pick_bvh = true;
  };
  KERN_STRUCT(WeldStats) {
    Int source_vertices = 0;
    Int unique_vertices = 0;
    Int bytes_before = 0;
    Int bytes_after = 0;
    Bool indexed = false;
  };
  KERN_STRUCT(ParticleSpec) {
    Vector position = Vector{0.0, 0.0, 0.0};
    Vector velocity = Vector{0.0, 0.0, 0.0};
    Vec4 color = Vec4{1.0, 1.0, 1.0, 1.0};
    Float start_ms = 0.0;
    Float life_ms = 350.0;
    Float size = 0.35;
    Float gravity = 0.0;
    Float grow = 0.0;
    Float fade_extra = 0.0;
  };
  KERN_STRUCT(ParticleStats) {
    Int active = 0;
    Int dropped = 0;
    Int uploaded = 0;
    Int expired = 0;
  };
  KERN_STRUCT(PointLights) {
    Int count = 0;
    Fixed<Float, 64> pos_range = Fixed<Float, 64>{};
    Fixed<Float, 64> color_intensity = Fixed<Float, 64>{};
  };
  KERN_STRUCT(SkyState) {
    Vector sun = Vector{-0.45, -0.52, 0.72};
    Vector moon = Vector{0.4, 0.5, 0.76};
    Float night = 0.0;
    Bool clouds = true;
    Vector blast_direction = Vector{0.0, 0.0, -1.0};
    Float blast_age = -1.0;
    Vector wind = Vector{0.012, 0.004, 0.0};
  };
  KERN_STRUCT(SkyAtmosphere) {
    Vector fog_color = Vector{0.474, 0.584, 0.716};
    Vector sun_color = Vector{2.8, 2.47, 2.11};
    Vector sun_direction = Vector{-0.45, -0.52, 0.72};
    Float sun_scatter = 0.0;
    Float horizon = 0.0;
  };
  KERN_STRUCT(BloomPass_Occlusion) {
    Int texture = 0;
    Float scale_x = 1.0;
    Float scale_y = 1.0;
    Mat4 projection = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
  };
  KERN_STRUCT(Lighting) {
    Vector sun_direction = Vector{-0.45, -0.52, 0.72};
    Vector sun_color = Vector{2.8, 2.47, 2.11};
    Vector sky_color = Vector{0.469, 0.609, 0.85};
    Vector ground_color = Vector{0.125, 0.094, 0.054};
    Vector fog_color = Vector{0.474, 0.584, 0.716};
    Float fog_density = 0.00085;
    Float exposure = 1.0;
    Float env_intensity = 0.9;
    Vector env_zenith = Vector{0.05, 0.2, 0.62};
    Vector env_horizon = Vector{0.52, 0.66, 0.86};
    Vector env_ground = Vector{0.105, 0.127, 0.084};
    Float fog_height_falloff = 0.0;
    Float fog_sun_scatter = 0.0;
    Float fog_horizon = 0.0;
  };
  KERN_STRUCT(Viewport) {
    Int x = 0;
    Int y = 0;
    Int width = 0;
    Int height = 0;
  };
  KERN_STRUCT(RenderParams) {
    Vec4 clear_color = Vec4{0.045, 0.07, 0.12, 1.0};
    Bool has_viewport = false;
    Viewport viewport = Viewport{0, 0, 0, 0};
    Bool page_hidden = false;
    Int layers = 4294967295;
    Bool sky = true;
    Bool reuse_shadow = false;
    Int framebuffer = 0;
  };
  KERN_STRUCT(Ray) {
    Vector origin = Vector{0.0, 0.0, 0.0};
    Vector direction = Vector{0.0, 0.0, -1.0};
    Float near = 0.05;
    Float far = 1800.0;
  };
  KERN_STRUCT(PickQuery) {
    Float max_distance = 1800.0;
    Int ignore_id = 0;
    Bool include_invisible = false;
    Int layers = 4294967295;
  };
  KERN_STRUCT(PickHit) {
    Int mesh = 0;
    Int instance = 0;
    Int triangle = 0;
    Float distance = 0.0;
    Vector point = Vector{0.0, 0.0, 0.0};
    Vector normal = Vector{0.0, 0.0, 0.0};
    Vector barycentric = Vector{0.0, 0.0, 0.0};
  };
  KERN_STRUCT(PickStats) {
    Int triangles_examined = 0;
    Int bounds_rejected = 0;
  };
  KERN_STRUCT(ShaderSource) {
    Text vertex = "";
    Text fragment = "";
  };
  KERN_STRUCT(net_Message) {
    Int id = 0;
    Int type = 0;
    List<Int> data = List<Int>{};
  };
  KERN_STRUCT(net_EndpointStats) {
    Float rtt_ms = 0.0;
    Float packet_loss = 0.0;
    Float sent_kbps = 0.0;
    Float received_kbps = 0.0;
    Float acked_kbps = 0.0;
    Int64 packets_sent = Int64{0x0};
    Int64 packets_received = Int64{0x0};
    Int64 packets_acked = Int64{0x0};
    Int64 packets_stale = Int64{0x0};
    Int64 packets_invalid = Int64{0x0};
    Int64 packets_too_large_to_send = Int64{0x0};
    Int64 packets_too_large_to_receive = Int64{0x0};
    Int64 fragments_sent = Int64{0x0};
    Int64 fragments_received = Int64{0x0};
    Int64 fragments_invalid = Int64{0x0};
  };
  KERN_STRUCT(net_EndpointConfig) {
    Int max_packet_size = 16384;
    Int fragment_above = 1024;
    Int fragment_size = 1024;
    Int max_fragments = 16;
    Int ack_buffer_size = 256;
    Int sent_packets_buffer_size = 256;
    Int received_packets_buffer_size = 256;
    Int fragment_reassembly_buffer_size = 64;
    Float rtt_smoothing = 0.1;
    Float packet_loss_smoothing = 0.1;
    Float bandwidth_smoothing = 0.1;
    Int packet_header_size = 28;
  };
  KERN_STRUCT(net_Simulator_Packet) {
    net_Address from = net_Address{Int64{0x0}};
    net_Address to = net_Address{Int64{0x0}};
    Float deliver = 0.0;
    List<Int> data = List<Int>{};
  };
  KERN_STRUCT(physics_Pose) {
    Vector position = Vector{0.0, 0.0, 0.0};
    Quat rotation = Quat{0.0, 0.0, 0.0, 1.0};
  };
  KERN_STRUCT(physics_Version) {
    Int major = 0;
    Int minor = 0;
    Int revision = 0;
  };
  KERN_STRUCT(physics_WorldSettings) {
    Vector gravity = Vector{0.0, 0.0, -9.80665};
    Bool enable_sleep = true;
    Bool enable_continuous = true;
    Float restitution_threshold = 1.0;
    Int restitution_iterations = 2;
    Bool restitution_propagation = false;
    Float hit_event_threshold = 1.0;
    Float contact_hertz = 30.0;
    Float contact_damping_ratio = 10.0;
    Float contact_speed = 3.0;
    Float maximum_linear_speed = 400.0;
  };
  KERN_STRUCT(physics_BodySettings) {
    physics_BodyType type = physics_BodyType::dynamic;
    Vector position = Vector{0.0, 0.0, 0.0};
    Quat rotation = Quat{0.0, 0.0, 0.0, 1.0};
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    Vector angular_velocity = Vector{0.0, 0.0, 0.0};
    Float linear_damping = 0.0;
    Float angular_damping = 0.0;
    Float gravity_scale = 1.0;
    Bool enable_sleep = true;
    Bool awake = true;
    Bool enabled = true;
    Bool bullet = false;
    Int64 user = Int64{0x0};
  };
  KERN_STRUCT(physics_ShapeMaterial) {
    Float density = 1.0;
    Float friction = 0.6;
    Float restitution = 0.0;
    Float rolling_resistance = 0.0;
    Bool sensor = false;
    Int64 category_bits = Int64{0x1};
    Int64 mask_bits = Int64{0xffffffffffffffff};
    Int group_index = 0;
    Bool contact_events = true;
    Bool sensor_events = true;
    Bool hit_events = false;
    Int64 user = Int64{0x0};
    physics_Surface surface = physics_Surface::none;
  };
  KERN_STRUCT(physics_ShapeSettings) {
    physics_ShapeKind kind = physics_ShapeKind::box;
    Vector center = Vector{0.0, 0.0, 0.0};
    Quat rotation = Quat{0.0, 0.0, 0.0, 1.0};
    Vector size = Vector{1.0, 1.0, 1.0};
    Float radius = 0.5;
    Float length = 1.0;
    List<Vector> points = List<Vector>{};
    List<Int> indices = List<Int>{};
    physics_ShapeMaterial material = physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none};
  };
  KERN_STRUCT(physics_JointSettings) {
    physics_JointType type = physics_JointType::revolute;
    physics_Body body_a = physics_Body{Int64{0x0}};
    physics_Body body_b = physics_Body{Int64{0x0}};
    physics_Pose frame_a = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    physics_Pose frame_b = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool collide_connected = false;
    Bool enable_spring = false;
    Float hertz = 0.0;
    Float damping_ratio = 0.0;
    Bool enable_limit = false;
    Float lower = 0.0;
    Float upper = 0.0;
    Bool enable_motor = false;
    Float motor_speed = 0.0;
    Float max_motor_torque = 0.0;
    Float max_motor_force = 0.0;
    Vector motor_velocity = Vector{0.0, 0.0, 0.0};
    Float length = 1.0;
    Float linear_hertz = 0.0;
    Float angular_hertz = 0.0;
    Float linear_damping_ratio = 1.0;
    Float angular_damping_ratio = 1.0;
    Vector linear_velocity = Vector{0.0, 0.0, 0.0};
    Vector angular_velocity = Vector{0.0, 0.0, 0.0};
    Float max_velocity_force = 0.0;
    Float max_velocity_torque = 0.0;
    Float max_spring_force = 0.0;
    Float max_spring_torque = 0.0;
    Bool enable_steering = false;
    Bool enable_steering_limit = false;
    Float steering_hertz = 0.0;
    Float steering_damping_ratio = 0.0;
    Float target_steering = 0.0;
    Float max_steering_torque = 0.0;
    Float steering_lower = 0.0;
    Float steering_upper = 0.0;
    Float max_torque = 0.0;
    Float constraint_hertz = 0.0;
    Float constraint_damping_ratio = 0.0;
    Int64 user = Int64{0x0};
  };
  KERN_STRUCT(physics_MotionLocks) {
    Fixed<Bool, 3> linear = Fixed<Bool, 3>{};
    Fixed<Bool, 3> angular = Fixed<Bool, 3>{};
  };
  KERN_STRUCT(physics_ContactPoint) {
    Vector point = Vector{0.0, 0.0, 0.0};
    Vector normal = Vector{0.0, 0.0, 0.0};
    Float separation = 0.0;
    Float impulse = 0.0;
  };
  KERN_STRUCT(physics_Body_Float_pair) {
    physics_Body first = physics_Body{Int64{0x0}};
    Float second = 0.0;
  };
  KERN_STRUCT(physics_RayHit) {
    Bool hit = false;
    physics_Shape shape = physics_Shape{Int64{0x0}};
    physics_Body body = physics_Body{Int64{0x0}};
    Vector point = Vector{0.0, 0.0, 0.0};
    Vector normal = Vector{0.0, 0.0, 0.0};
    Float fraction = 0.0;
  };
  KERN_STRUCT(physics_Capsule) {
    Vector center1 = Vector{0.0, 0.0, 0.0};
    Vector center2 = Vector{0.0, 0.0, 0.0};
    Float radius = 0.3;
  };
  KERN_STRUCT(physics_MoverPlane) {
    Vector normal = Vector{0.0, 0.0, 0.0};
    Float offset = 0.0;
    Vector point = Vector{0.0, 0.0, 0.0};
    physics_Shape shape = physics_Shape{Int64{0x0}};
    physics_Body body = physics_Body{Int64{0x0}};
  };
  KERN_STRUCT(physics_TouchEvent) {
    physics_Shape shape_a = physics_Shape{Int64{0x0}};
    physics_Shape shape_b = physics_Shape{Int64{0x0}};
    physics_Body body_a = physics_Body{Int64{0x0}};
    physics_Body body_b = physics_Body{Int64{0x0}};
  };
  KERN_STRUCT(physics_HitEvent) {
    physics_Shape shape_a = physics_Shape{Int64{0x0}};
    physics_Shape shape_b = physics_Shape{Int64{0x0}};
    physics_Body body_a = physics_Body{Int64{0x0}};
    physics_Body body_b = physics_Body{Int64{0x0}};
    Vector point = Vector{0.0, 0.0, 0.0};
    Vector normal = Vector{0.0, 0.0, 0.0};
    Float approach_speed = 0.0;
  };
  KERN_STRUCT(net_ChannelConfig) {
    net_ChannelType type = net_ChannelType::reliable_ordered;
    Int send_queue_size = 1024;
    Int receive_queue_size = 1024;
    Int max_messages_per_packet = 256;
    Int packet_budget = 0;
    Int max_message_size = 65536;
    Int block_threshold = 512;
    Int block_fragment_size = 1024;
    Float resend_time = 0.1;
  };
  KERN_STRUCT(net_ConnectionConfig) {
    List<net_ChannelConfig> channels = List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}};
    Int max_packet_size = 8192;
    Int max_message_types = 256;
    net_EndpointConfig endpoint = net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28};
  };
  KERN_STRUCT(net_ClientServerConfig) {
    Int64 protocol_id = Int64{0x4b45524e30303031};
    Int max_clients = 16;
    Float timeout = 6.0;
    Float request_interval = 0.1;
    Float connect_timeout = 6.0;
    Float keepalive_interval = 0.1;
    net_ConnectionConfig connection = net_ConnectionConfig{List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}, 8192, 256, net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}};
  };

  // ---- variables ----
  Key In1 = Key::unknown;
  Bool R0 = false;
  Bool R1 = false;
  Bool R2 = false;
  Vector R3 = Vector{0.0, 0.0, 0.0};
  Vector R4 = Vector{0.0, 0.0, 0.0};
  Bool R5 = false;
  Bool R6 = false;
  Float R7 = 0.0;
  List<Touch> R8 = List<Touch>{};
  Int R9 = 0;
  Bool R10 = false;
  Int R18 = 0;
  Int R19_0 = 0;
  Int R20 = 0;
  Text R23 = "";
  Bool R24 = false;
  Float R25 = 0.0;
  Int R27 = 0;
  Int R28 = 0;
  Float R29 = 0.0;
  RenderStats R30 = RenderStats{0, 0, 0, false, false, 0.0, 0, 0, 0, 0.0, -1.0, BloomState::off};
  Vec4 R31 = Vec4{0.0, 0.0, 0.0, 0.0};
  Vec4 In2 = Vec4{0.0, 0.0, 0.0, 0.0};
  Bool R33 = false;
  Vector R35 = Vector{0.0, 0.0, 0.0};
  Vector R37 = Vector{0.0, 0.0, 0.0};
  Vector R39 = Vector{0.0, 0.0, 0.0};
  Float R41 = 0.0;
  Bool R43 = false;
  Bool R45 = false;
  Bool R47 = false;
  Vector R49 = Vector{0.0, 0.0, 0.0};
  Float R51 = 0.0;
  Int R53 = 0;
  Bool R55 = false;
  Camera R57 = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Camera In3 = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Int R59 = 0;
  Int R61 = 0;
  Bool R63 = false;
  Float R65 = 0.0;
  Bool R67 = false;
  RendererOptions R69_0 = RendererOptions{960, 540, false, ShadowOptions{512, 120.0, 0.0012, 9}, BloomOptions{true, false, true}, AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true}, true};
  List<Int> R70_0 = List<Int>{};
  Bool R71_0 = false;
  Frame In4 = Frame{0.0, 0.0, Int64{0x0}};
  audio_Bus In5 = audio_Bus::master;
  Text R75 = "";
  Bool R76_0 = false;
  audio_Bus R76_1 = audio_Bus::master;
  audio_Rolloff In6 = audio_Rolloff::inverse;
  Text R77 = "";
  Bool R78_0 = false;
  audio_Rolloff R78_1 = audio_Rolloff::inverse;
  Bool R79_0 = false;
  Int R80 = 0;
  Bool R81 = false;
  Int R82_0 = 0;
  Int R83_0 = 0;
  Bool R84_0 = false;
  List<audio_ClipInfo> R85 = List<audio_ClipInfo>{};
  List<Float_row> R86 = List<Float_row>{};
  Int R87 = 0;
  audio_PlayParams In7 = audio_PlayParams{1.0, 1.0, 0.0, false, audio_Bus::sfx, 0.0, 0.0, 0, false, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 1.0, 50.0, audio_Rolloff::inverse, 0.0, 1.0};
  Int64 R88_0 = Int64{0x0};
  Bool R91 = false;
  Float R98 = 0.0;
  audio_Listener In8 = audio_Listener{Vector{0.0, 0.0, 0.0}, Vector{1.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}};
  audio_Listener R100 = audio_Listener{Vector{0.0, 0.0, 0.0}, Vector{1.0, 0.0, 0.0}, Vector{0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}};
  audio_BusSettings R101_0 = audio_BusSettings{1.0, false, 0.0, 0.0};
  audio_BusSettings R102 = audio_BusSettings{1.0, false, 0.0, 0.0};
  List<Float> R104_0 = List<Float>{};
  audio_Stats R105 = audio_Stats{0, 0, 0, 0, 0, Int64{0x0}, 0.0, audio_Meter{Fixed<Float, 2>{}, Fixed<Float, 2>{}}, Fixed<audio_Meter, 6>{}, Fixed<Float, 2>{}};
  audio_SynthPatch In9 = audio_SynthPatch{audio_Wave::square, 0.5, 0.005, 0.1, 0.0, 0.2, 440.0, 0.0, 0.0, 20.0, 0.0, 6.0, 1.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1};
  List<Float> R106 = List<Float>{};
  List<Float> R107_0 = List<Float>{};
  List<Float> R108_0 = List<Float>{};
  List<Float> R109_0 = List<Float>{};
  List<audio_BuiltinSound> R110 = List<audio_BuiltinSound>{};
  List<audio_BuiltinSound> R111 = List<audio_BuiltinSound>{};
  Bool R112_0 = false;
  List<Float> R112_1 = List<Float>{};
  Int R112_2 = 0;
  Bool R113_0 = false;
  Bool R114_0 = false;
  Bool R115 = false;
  Text R116 = "";
  List<Float> R117_0 = List<Float>{};
  Int R117_1 = 0;
  Text R118 = "";
  Float R120 = 0.0;
  Float R121 = 0.0;
  Int R122_0 = 0;
  Bool R123_0 = false;
  List<Int> R123_1 = List<Int>{};
  Int R126 = 0;
  Bool R128 = false;
  Bool R130 = false;
  List<Int> R131 = List<Int>{};
  List<Int> R132_0 = List<Int>{};
  game_MeshComponent In10 = game_MeshComponent{game_Shape::box, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, 24, "", true, 0.6, 0.0, 0.0, "", game_MaterialDesc{Vector{0.8, 0.8, 0.8}, 0.6, 0.0, Vector{0.0, 0.0, 0.0}, 0.0, 1.0, false, true, false, "", 1.0}};
  MeshData R134 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  Aabb R135 = Aabb{Vector{1e+30, 1e+30, 1e+30}, Vector{-1e+30, -1e+30, -1e+30}};
  game_ColliderComponent R136 = game_ColliderComponent{game_ColliderShape::automatic, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, 1000.0, 0.6, 0.0, 0.0, "", false, 1, 4294967295, 0, "auto"};
  List<Vector> R137 = List<Vector>{};
  game_CityComponent In11 = game_CityComponent{20261022, 7, 80.0, 16.0, 84.0, true, 22, true};
  game_CityGeometry In12 = game_CityGeometry{List<MeshData>{}, List<MeshData>{}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}, List<Float>{}, List<Float>{}, List<Float>{}, List<game_CityBox>{}, 0.0, 0};
  Bool R138_0 = false;
  game_CityGeometry R138_1 = game_CityGeometry{List<MeshData>{}, List<MeshData>{}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}, List<Float>{}, List<Float>{}, List<Float>{}, List<game_CityBox>{}, 0.0, 0};
  game_Rng In13 = game_Rng{0};
  Vector R139_0 = Vector{0.0, 0.0, 0.0};
  game_Rng R139_1 = game_Rng{0};
  Quat R140 = Quat{0.0, 0.0, 0.0, 1.0};
  Quat In14 = Quat{0.0, 0.0, 0.0, 1.0};
  Vector R141 = Vector{0.0, 0.0, 0.0};
  game_Transform In15 = game_Transform{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}};
  Mat4 R142 = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
  Mat4 In16 = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
  game_Transform R143 = game_Transform{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}};
  game_Shape In17 = game_Shape::box;
  Text R144 = "";
  Bool R145_0 = false;
  game_Shape R145_1 = game_Shape::box;
  game_Motion In18 = game_Motion::fixed;
  Text R146 = "";
  game_ColliderShape In19 = game_ColliderShape::automatic;
  Text R147 = "";
  game_JointKind In20 = game_JointKind::hinge;
  Text R148 = "";
  game_ControlMode In21 = game_ControlMode::hovercar;
  Text R149 = "";
  game_CameraMode In22 = game_CameraMode::chase;
  Text R150 = "";
  game_WeaponKind In23 = game_WeaponKind::rifle;
  Text R151 = "";
  game_WeaponComponent R152 = game_WeaponComponent{game_WeaponKind::rifle, 24.0, 600.0, true, 30, 120, 2.0, 1.2, 0.15, 0.8, 300.0, 2.5, 4.0, 1, 42.0, "builtin:smg", "builtin:reload"};
  game_TriggerAction In24 = game_TriggerAction::goal;
  Text R153 = "";
  game_MoverMode In25 = game_MoverMode::sine;
  Text R154 = "";
  game_GameGoal In26 = game_GameGoal::none;
  Text R155 = "";
  Bool R156 = false;
  game_Level In27 = game_Level{"Untitled", game_WorldSettings{"World", Vector{0.0, 0.0, -9.80665}, 4, 60.0, 0.42, 0.0, 210.0, 0.0009, 1.0, 1.6, 80.0, true, Vector{0.19, 0.27, 0.15}, true, 0.7, 0.0, "", 30.0, 10.0, 3.0, -50.0, "", game_GameGoal::none, 0.0, 0.0, 0, false, 1.0, "", 0.6, "", 0.6, 0.25, 1.0}, List<game_Actor>{}, List<Int>{}, List<Int>{}, List<Int>{}, logic_Graph{List<logic_Node>{}, List<logic_Link>{}, List<Int>{}, List<logic_StateMachine>{}, List<logic_Function>{}, List<logic_EnumType>{}, List<logic_StructType>{}, List<logic_CommentBox>{}, 1, false, List<Text>{}}, game_EditorCamera{Vector{-24.0, -30.0, 18.0}, 51.0, -24.0}, 1};
  Int R157 = 0;
  game_WorldSettings In28 = game_WorldSettings{"World", Vector{0.0, 0.0, -9.80665}, 4, 60.0, 0.42, 0.0, 210.0, 0.0009, 1.0, 1.6, 80.0, true, Vector{0.19, 0.27, 0.15}, true, 0.7, 0.0, "", 30.0, 10.0, 3.0, -50.0, "", game_GameGoal::none, 0.0, 0.0, 0, false, 1.0, "", 0.6, "", 0.6, 0.25, 1.0};
  List<Text> R158 = List<Text>{};
  Int R159 = 0;
  List<game_Actor> R160 = List<game_Actor>{};
  game_Actor In29 = game_Actor{0, "", 0, "", false, false, game_Transform{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}}, List<Text>{}, List<game_MeshComponent>{}, List<game_LightComponent>{}, List<game_BodyComponent>{}, List<game_ColliderComponent>{}, List<game_JointComponent>{}, List<game_ControllerComponent>{}, List<game_PickupComponent>{}, List<game_TriggerComponent>{}, List<game_RotatorComponent>{}, List<game_MoverComponent>{}, List<game_CityComponent>{}, List<game_AudioComponent>{}, List<game_WeaponComponent>{}, List<game_HealthComponent>{}, List<logic_Graph>{}};
  game_Actor R161_0 = game_Actor{0, "", 0, "", false, false, game_Transform{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}}, List<Text>{}, List<game_MeshComponent>{}, List<game_LightComponent>{}, List<game_BodyComponent>{}, List<game_ColliderComponent>{}, List<game_JointComponent>{}, List<game_ControllerComponent>{}, List<game_PickupComponent>{}, List<game_TriggerComponent>{}, List<game_RotatorComponent>{}, List<game_MoverComponent>{}, List<game_CityComponent>{}, List<game_AudioComponent>{}, List<game_WeaponComponent>{}, List<game_HealthComponent>{}, List<logic_Graph>{}};
  game_Level R161_1 = game_Level{"Untitled", game_WorldSettings{"World", Vector{0.0, 0.0, -9.80665}, 4, 60.0, 0.42, 0.0, 210.0, 0.0009, 1.0, 1.6, 80.0, true, Vector{0.19, 0.27, 0.15}, true, 0.7, 0.0, "", 30.0, 10.0, 3.0, -50.0, "", game_GameGoal::none, 0.0, 0.0, 0, false, 1.0, "", 0.6, "", 0.6, 0.25, 1.0}, List<game_Actor>{}, List<Int>{}, List<Int>{}, List<Int>{}, logic_Graph{List<logic_Node>{}, List<logic_Link>{}, List<Int>{}, List<logic_StateMachine>{}, List<logic_Function>{}, List<logic_EnumType>{}, List<logic_StructType>{}, List<logic_CommentBox>{}, 1, false, List<Text>{}}, game_EditorCamera{Vector{-24.0, -30.0, 18.0}, 51.0, -24.0}, 1};
  List<Int> R162 = List<Int>{};
  Mat4 R163 = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
  Int R164 = 0;
  Bool R165 = false;
  Int R166 = 0;
  Int R167 = 0;
  Int R168 = 0;
  Int R169_0 = 0;
  game_Level R169_1 = game_Level{"Untitled", game_WorldSettings{"World", Vector{0.0, 0.0, -9.80665}, 4, 60.0, 0.42, 0.0, 210.0, 0.0009, 1.0, 1.6, 80.0, true, Vector{0.19, 0.27, 0.15}, true, 0.7, 0.0, "", 30.0, 10.0, 3.0, -50.0, "", game_GameGoal::none, 0.0, 0.0, 0, false, 1.0, "", 0.6, "", 0.6, 0.25, 1.0}, List<game_Actor>{}, List<Int>{}, List<Int>{}, List<Int>{}, logic_Graph{List<logic_Node>{}, List<logic_Link>{}, List<Int>{}, List<logic_StateMachine>{}, List<logic_Function>{}, List<logic_EnumType>{}, List<logic_StructType>{}, List<logic_CommentBox>{}, 1, false, List<Text>{}}, game_EditorCamera{Vector{-24.0, -30.0, 18.0}, 51.0, -24.0}, 1};
  Int R170 = 0;
  Text R171 = "";
  Int R172 = 0;
  Int R173 = 0;
  Bool R174 = false;
  List<game_Issue> R175 = List<game_Issue>{};
  Text R176 = "";
  Bool R177_0 = false;
  Vector R177_1 = Vector{0.0, 0.0, 0.0};
  Vector R178 = Vector{0.0, 0.0, 0.0};
  Bool R179 = false;
  List<Text> R180 = List<Text>{};
  Bool R181 = false;
  Text R182 = "";
  List<ExtensionAttribute> R183 = List<ExtensionAttribute>{};
  Text R184 = "";
  Text R185 = "";
  Text R186 = "";
  Text R187 = "";
  List<game_BodyPlan> R190 = List<game_BodyPlan>{};
  game_GameStatus In30 = game_GameStatus::inactive;
  Text R191 = "";
  Int R195 = 0;
  Bool R196_0 = false;
  Mat4 R196_1 = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
  Aabb In31 = Aabb{Vector{1e+30, 1e+30, 1e+30}, Vector{-1e+30, -1e+30, -1e+30}};
  Bool R197_0 = false;
  Aabb R197_1 = Aabb{Vector{1e+30, 1e+30, 1e+30}, Vector{-1e+30, -1e+30, -1e+30}};
  List<Int> R198_0 = List<Int>{};
  List<Text> R199 = List<Text>{};
  Bool R200_0 = false;
  Float R201_0 = 0.0;
  Float R202 = 0.0;
  game_SimMode In32 = game_SimMode::none;
  Bool R204_0 = false;
  Bool R206 = false;
  Int R207 = 0;
  game_SimMode R208 = game_SimMode::none;
  Bool R210 = false;
  Bool R211_0 = false;
  game_PlayInput In33 = game_PlayInput{0.0, 0.0, 0.0, false, false, false, false, 0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, false, 0.0, false, false, false, false, false, 0.0, 0.0, false, 0.0};
  Bool R214_0 = false;
  Bool R215_0 = false;
  Camera R215_1 = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Int R216 = 0;
  Float R217 = 0.0;
  Bool R218 = false;
  Float R219 = 0.0;
  Float R220 = 0.0;
  Int R221_0 = 0;
  Bool R222_0 = false;
  Int R223 = 0;
  List<Int> R226 = List<Int>{};
  List<Int_Bool_pair> In34 = List<Int_Bool_pair>{};
  List<Int_Bool_pair> R227_0 = List<Int_Bool_pair>{};
  List<Int> R228 = List<Int>{};
  List<Int> R230_0 = List<Int>{};
  game_Hud R231_0 = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
  game_Hud R231_3 = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
  game_Hud In35 = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
  Bool R232_0 = false;
  game_Hud R235 = game_Hud{game_GameStatus::inactive, "", "", 0.0, 0.0, 0, 0, 0.0, -1.0, 0.0, -1.0, false, false, 0.0, false, -1.0, 100.0, 0.0, -1, 0, 0, -1.0, 0.0, 99.0, false, false, 99.0, 0.0, -1.0, false, 60.0};
  Float R236 = 0.0;
  game_SimStats R237 = game_SimStats{0, 0, 0, 0, 0, 0, Int64{0x0}};
  game_Level R238 = game_Level{"Untitled", game_WorldSettings{"World", Vector{0.0, 0.0, -9.80665}, 4, 60.0, 0.42, 0.0, 210.0, 0.0009, 1.0, 1.6, 80.0, true, Vector{0.19, 0.27, 0.15}, true, 0.7, 0.0, "", 30.0, 10.0, 3.0, -50.0, "", game_GameGoal::none, 0.0, 0.0, 0, false, 1.0, "", 0.6, "", 0.6, 0.25, 1.0}, List<game_Actor>{}, List<Int>{}, List<Int>{}, List<Int>{}, logic_Graph{List<logic_Node>{}, List<logic_Link>{}, List<Int>{}, List<logic_StateMachine>{}, List<logic_Function>{}, List<logic_EnumType>{}, List<logic_StructType>{}, List<logic_CommentBox>{}, 1, false, List<Text>{}}, game_EditorCamera{Vector{-24.0, -30.0, 18.0}, 51.0, -24.0}, 1};
  Int R239_0 = 0;
  physics_Body R240 = physics_Body{Int64{0x0}};
  Bool R241_0 = false;
  Bool R244 = false;
  List<Float> R245_0 = List<Float>{};
  List<Float> R245_1 = List<Float>{};
  List<Text> R246_0 = List<Text>{};
  Int R246_1 = 0;
  Text R246_2 = "";
  List<Text> R246_3 = List<Text>{};
  List<game_SoundCue> R247_0 = List<game_SoundCue>{};
  Int R247_1 = 0;
  game_SoundCue R247_2 = game_SoundCue{"", Vector{0.0, 0.0, 0.0}, 1.0, 1.0, true, audio_Bus::sfx, 0, false, false};
  List<game_SoundCue> R247_3 = List<game_SoundCue>{};
  game_SoundCue In36 = game_SoundCue{"", Vector{0.0, 0.0, 0.0}, 1.0, 1.0, true, audio_Bus::sfx, 0, false, false};
  List<game_ShotEvent> R248_0 = List<game_ShotEvent>{};
  Int R248_1 = 0;
  game_ShotEvent R248_2 = game_ShotEvent{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, false, false, Vector{0.0, 0.0, 1.0}, 0};
  List<game_ShotEvent> R248_3 = List<game_ShotEvent>{};
  game_ShotEvent In37 = game_ShotEvent{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, false, false, Vector{0.0, 0.0, 1.0}, 0};
  Float R250 = 0.0;
  game_SoldierState In38 = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0};
  Bool R252_0 = false;
  game_SoldierState R252_1 = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0};
  Bool R253_0 = false;
  List<Mat4> In39 = List<Mat4>{};
  Bool R257 = false;
  game_PlayInput R258 = game_PlayInput{0.0, 0.0, 0.0, false, false, false, false, 0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, false, 0.0, false, false, false, false, false, 0.0, 0.0, false, 0.0};
  Int R259_0 = 0;
  List<game_GameWorld_SpawnChange> In40 = List<game_GameWorld_SpawnChange>{};
  List<Int> R261_0 = List<Int>{};
  List<game_GameWorld_SpawnChange> R261_1 = List<game_GameWorld_SpawnChange>{};
  List<Int> R262_0 = List<Int>{};
  List<game_GameWorld_SpawnChange> R262_1 = List<game_GameWorld_SpawnChange>{};
  Bool R263_0 = false;
  Bool R264_0 = false;
  Bool R265_0 = false;
  Bool R266_0 = false;
  Int R267_0 = 0;
  Bool R268_0 = false;
  Vector R269 = Vector{0.0, 0.0, 0.0};
  Bool R270_0 = false;
  Bool R271_0 = false;
  Bool R272_0 = false;
  Bool R273_0 = false;
  Int R274 = 0;
  List<Int> R275_0 = List<Int>{};
  Bool R276 = false;
  physics_QueryFilter In41 = physics_QueryFilter{Int64{0xffffffffffffffff}, Int64{0xffffffffffffffff}};
  Bool R277_0 = false;
  Int R277_1 = 0;
  Vector R277_2 = Vector{0.0, 0.0, 0.0};
  Vector R277_3 = Vector{0.0, 0.0, 0.0};
  game_GameWorld_WalkerSensors In42 = game_GameWorld_WalkerSensors{Fixed<Float, 64>{}, Fixed<Int, 64>{}, -1.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, -1.0}, false, 0.0, -1.0, 0, Fixed<Vector, 64>{}, 0};
  Bool R289_0 = false;
  game_GameWorld_WalkerSensors R289_1 = game_GameWorld_WalkerSensors{Fixed<Float, 64>{}, Fixed<Int, 64>{}, -1.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, -1.0}, false, 0.0, -1.0, 0, Fixed<Vector, 64>{}, 0};
  Bool R290_0 = false;
  Camera R290_1 = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Bool R291_0 = false;
  Bool R292_0 = false;
  Bool R293_0 = false;
  Bool R294_0 = false;
  Text R295_0 = "";
  Bool R296_0 = false;
  Bool R297 = false;
  Bool R298_0 = false;
  Bool R299_0 = false;
  Bool R300_0 = false;
  Float R300_1 = 0.0;
  Float R300_2 = 0.0;
  Float R300_3 = 0.0;
  Text R301 = "";
  Bool R302_0 = false;
  Bool R303_0 = false;
  Bool R304_0 = false;
  Bool R305_0 = false;
  Bool R306 = false;
  game_GameWorld_WalkerProbe In43 = game_GameWorld_WalkerProbe{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Fixed<Bool, 2>{}, Fixed<Float, 2>{}, false, false, Fixed<Float, 14>{}, Fixed<Float, 14>{}, Fixed<Float, 14>{}};
  Bool R307_0 = false;
  game_GameWorld_WalkerProbe R307_1 = game_GameWorld_WalkerProbe{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Fixed<Bool, 2>{}, Fixed<Float, 2>{}, false, false, Fixed<Float, 14>{}, Fixed<Float, 14>{}, Fixed<Float, 14>{}};
  Bool R308_0 = false;
  Text R309 = "";
  List<game_GameWorld_WalkerJoint> R310 = List<game_GameWorld_WalkerJoint>{};
  Bool R311_0 = false;
  Bool R312_0 = false;
  game_GameWorld_DetectorModel In44 = game_GameWorld_DetectorModel{0.0, 0.0, 0.0, Int64{0x1}};
  Bool R313_0 = false;
  Bool R314 = false;
  Bool R315 = false;
  Bool R316_0 = false;
  Bool R316_1 = false;
  Float R316_2 = 0.0;
  Float R316_3 = 0.0;
  Text R317 = "";
  List<Int> R318 = List<Int>{};
  game_GameWorld_ArtStats R319 = game_GameWorld_ArtStats{0, 0, 0, 0, false};
  Int R320_0 = 0;
  Int R321_0 = 0;
  Bool R325_0 = false;
  Bool R326 = false;
  Bool R327 = false;
  List<Text> R328 = List<Text>{};
  List<Text> R329 = List<Text>{};
  Bool R330_0 = false;
  Float R332 = 0.0;
  Int R334 = 0;
  Bool R338 = false;
  Bool R341 = false;
  Int R342 = 0;
  Int R343 = 0;
  game_ControllerComponent In45 = game_ControllerComponent{game_ControlMode::hovercar, 36.0, 24.0, 130.0, 6.0, true, 60.0, 0.6, game_CameraMode::chase, 12.0, 4.0, 62.0, "auto", "builtin:jump", "builtin:boost", 2.6, 1.62, 1.0, true, true, "velstand", false, "training", false, 0.02};
  Text R344 = "";
  BuildingOptions In46 = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
  Bool R345_0 = false;
  Bool R346_0 = false;
  Bool R347 = false;
  List<GeoChunk> R348_0 = List<GeoChunk>{};
  Int R348_1 = 0;
  GeoChunk R348_2 = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}};
  List<GeoChunk> R348_3 = List<GeoChunk>{};
  GeoChunk In47 = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}};
  Int R349 = 0;
  Int R350 = 0;
  Int R351 = 0;
  Int R352 = 0;
  Bool R355 = false;
  List<Float> R357 = List<Float>{};
  Int R358 = 0;
  List<Float> R359_0 = List<Float>{};
  Float R360 = 0.0;
  List<Int> R361_0 = List<Int>{};
  Float R362 = 0.0;
  MeshData In48 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  MeshInput R363 = MeshInput{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Int>{}, List<Float>{}, List<Float>{}, List<Float>{}, Fixed<Float_row, 2>{}, List<Float>{}, List<Float>{}, Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, Fixed<Int, 8>{}}, 0, true, 1, true};
  MeshLook In49 = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, Fixed<Int, 8>{}}, 0, 0, true, 1, true};
  Bool R364_0 = false;
  WeldStats R365_0 = WeldStats{0, 0, 0, 0, false};
  MeshData R365_1 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  Int R366 = 0;
  MeshData R367 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  MeshData R368 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  MeshData R369 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  MeshData R370 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  MeshData R371 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  MeshData R372 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  MeshData R373 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  MeshData R374 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  MeshData R375_0 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  Bool R376_0 = false;
  Bool R377_0 = false;
  Bool R378 = false;
  Bool R379 = false;
  List<GeoChunk> R380_0 = List<GeoChunk>{};
  Int R380_1 = 0;
  GeoChunk R380_2 = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}};
  List<GeoChunk> R380_3 = List<GeoChunk>{};
  Int R381 = 0;
  Int R382 = 0;
  List<Float> R384 = List<Float>{};
  Int R385 = 0;
  Rgb In50 = Rgb{1.0, 1.0, 1.0};
  Bool R386_0 = false;
  MeshData R386_1 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  Float R386_2 = 0.0;
  Float R386_3 = 0.0;
  Bool R387 = false;
  Int R388 = 0;
  Int R389 = 0;
  Int R390 = 0;
  Int R391 = 0;
  Vector R392 = Vector{0.0, 0.0, 0.0};
  Sphere R393 = Sphere{Vector{0.0, 0.0, 0.0}, -1.0};
  Int R394 = 0;
  Sphere In51 = Sphere{Vector{0.0, 0.0, 0.0}, -1.0};
  Sphere R396 = Sphere{Vector{0.0, 0.0, 0.0}, -1.0};
  Bool R397_0 = false;
  ParticleSpec In52 = ParticleSpec{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vec4{1.0, 1.0, 1.0, 1.0}, 0.0, 350.0, 0.35, 0.0, 0.0, 0.0};
  Int R398_0 = 0;
  ParticleStats R399_0 = ParticleStats{0, 0, 0, 0};
  Int R401 = 0;
  Int R402 = 0;
  ParticleStats R403 = ParticleStats{0, 0, 0, 0};
  Bool R404_0 = false;
  PointLights R405_0 = PointLights{0, Fixed<Float, 64>{}, Fixed<Float, 64>{}};
  ShadowOptions In53 = ShadowOptions{512, 120.0, 0.0012, 9};
  Bool R407_0 = false;
  Bool R409 = false;
  Int64 R410_0 = Int64{0x0};
  Int R411_0 = 0;
  Mat4 R412 = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
  Int R413 = 0;
  ShadowOptions R414 = ShadowOptions{512, 120.0, 0.0012, 9};
  Float R416 = 0.0;
  Bool R417_0 = false;
  SkyState In54 = SkyState{Vector{-0.45, -0.52, 0.72}, Vector{0.4, 0.5, 0.76}, 0.0, true, Vector{0.0, 0.0, -1.0}, -1.0, Vector{0.012, 0.004, 0.0}};
  SkyAtmosphere In55 = SkyAtmosphere{Vector{0.474, 0.584, 0.716}, Vector{2.8, 2.47, 2.11}, Vector{-0.45, -0.52, 0.72}, 0.0, 0.0};
  BloomOptions In56 = BloomOptions{true, false, true};
  Bool R421_0 = false;
  List<BloomPass_Occlusion> In57 = List<BloomPass_Occlusion>{};
  BloomState R424 = BloomState::off;
  Int R425 = 0;
  AoOptions In58 = AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true};
  AoOptions R429 = AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true};
  Bool R430_0 = false;
  Int R431 = 0;
  Int R432 = 0;
  Int R433 = 0;
  AoQuality R435 = AoQuality::low;
  Bool R436 = false;
  Bool R437_0 = false;
  Int R439 = 0;
  Bool R440 = false;
  Int R441 = 0;
  RendererOptions In59 = RendererOptions{960, 540, false, ShadowOptions{512, 120.0, 0.0012, 9}, BloomOptions{true, false, true}, AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true}, true};
  Bool R453_0 = false;
  Bool R455_0 = false;
  Int R456 = 0;
  Int R457 = 0;
  Int R458_0 = 0;
  Bool R460 = false;
  MeshInput In60 = MeshInput{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Int>{}, List<Float>{}, List<Float>{}, List<Float>{}, Fixed<Float_row, 2>{}, List<Float>{}, List<Float>{}, Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, Fixed<Int, 8>{}}, 0, true, 1, true};
  Bool R461_0 = false;
  Bool R462_0 = false;
  Bool R462_1 = false;
  Bool R463_0 = false;
  Bool R464_0 = false;
  Bool R465_0 = false;
  Material In61 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, Fixed<Int, 8>{}};
  Bool R466_0 = false;
  Bool R467_0 = false;
  Int R468 = 0;
  List<Int> R469 = List<Int>{};
  Int R470 = 0;
  Int R471_0 = 0;
  Int R472_0 = 0;
  Int R473_0 = 0;
  Int R473_1 = 0;
  Bool R474_0 = false;
  Camera R475 = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}};
  Lighting R477 = Lighting{Vector{-0.45, -0.52, 0.72}, Vector{2.8, 2.47, 2.11}, Vector{0.469, 0.609, 0.85}, Vector{0.125, 0.094, 0.054}, Vector{0.474, 0.584, 0.716}, 0.00085, 1.0, 0.9, Vector{0.05, 0.2, 0.62}, Vector{0.52, 0.66, 0.86}, Vector{0.105, 0.127, 0.084}, 0.0, 0.0, 0.0};
  Lighting In62 = Lighting{Vector{-0.45, -0.52, 0.72}, Vector{2.8, 2.47, 2.11}, Vector{0.469, 0.609, 0.85}, Vector{0.125, 0.094, 0.054}, Vector{0.474, 0.584, 0.716}, 0.00085, 1.0, 0.9, Vector{0.05, 0.2, 0.62}, Vector{0.52, 0.66, 0.86}, Vector{0.105, 0.127, 0.084}, 0.0, 0.0, 0.0};
  SkyState R479 = SkyState{Vector{-0.45, -0.52, 0.72}, Vector{0.4, 0.5, 0.76}, 0.0, true, Vector{0.0, 0.0, -1.0}, -1.0, Vector{0.012, 0.004, 0.0}};
  PointLights In63 = PointLights{0, Fixed<Float, 64>{}, Fixed<Float, 64>{}};
  Bool R481_0 = false;
  PointLights R482 = PointLights{0, Fixed<Float, 64>{}, Fixed<Float, 64>{}};
  List<ShadowOptions> In64 = List<ShadowOptions>{};
  Bool R483_0 = false;
  Int R488 = 0;
  Int R489 = 0;
  Int64 R491 = Int64{0x0};
  RenderParams In65 = RenderParams{Vec4{0.045, 0.07, 0.12, 1.0}, false, Viewport{0, 0, 0, 0}, false, 4294967295, true, false, 0};
  RenderStats In66 = RenderStats{0, 0, 0, false, false, 0.0, 0, 0, 0, 0.0, -1.0, BloomState::off};
  Bool R492_0 = false;
  RenderStats R492_1 = RenderStats{0, 0, 0, false, false, 0.0, 0, 0, 0, 0.0, -1.0, BloomState::off};
  RenderStats R493 = RenderStats{0, 0, 0, false, false, 0.0, 0, 0, 0, 0.0, -1.0, BloomState::off};
  Ray In67 = Ray{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, -1.0}, 0.05, 1800.0};
  Bool R494_0 = false;
  Ray R494_1 = Ray{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, -1.0}, 0.05, 1800.0};
  PickQuery In68 = PickQuery{1800.0, 0, false, 4294967295};
  PickHit In69 = PickHit{0, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}};
  Bool R495_0 = false;
  PickHit R495_1 = PickHit{0, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}};
  PickStats R495_2 = PickStats{0, 0};
  Bool R496_0 = false;
  PickHit R496_1 = PickHit{0, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}};
  PickStats R496_2 = PickStats{0, 0};
  Bool R498_0 = false;
  Bool R499 = false;
  List<Int> R501 = List<Int>{};
  Text R502 = "";
  List<ExtensionAttribute> R503 = List<ExtensionAttribute>{};
  Text R504 = "";
  Text R505 = "";
  Text R506 = "";
  Text R507 = "";
  Text R508 = "";
  Text R509 = "";
  ShaderSource R513 = ShaderSource{"", ""};
  net_ClientState In70 = net_ClientState::denied;
  Text R514 = "";
  net_Address In71 = net_Address{Int64{0x0}};
  net_ClientState R517 = net_ClientState::denied;
  Bool R518 = false;
  Int R519 = 0;
  Int R520 = 0;
  Bool R524_0 = false;
  List<net_Message> R525_0 = List<net_Message>{};
  Int R526_0 = 0;
  Int R527 = 0;
  net_Address R528 = net_Address{Int64{0x0}};
  Bool R531 = false;
  Int R532 = 0;
  Bool R533 = false;
  Int R534 = 0;
  net_Address R535 = net_Address{Int64{0x0}};
  Bool R540_0 = false;
  List<net_Message> R541_0 = List<net_Message>{};
  Int R542_0 = 0;
  List<Int_Bool_pair> R543_0 = List<Int_Bool_pair>{};
  net_ConnectionError In72 = net_ConnectionError::none;
  Text R544 = "";
  Bool R546 = false;
  Bool R547_0 = false;
  List<net_Message> R548_0 = List<net_Message>{};
  net_ConnectionError R552 = net_ConnectionError::none;
  net_EndpointStats R553 = net_EndpointStats{0.0, 0.0, 0.0, 0.0, 0.0, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}};
  Int R554_0 = 0;
  Int R555 = 0;
  Int R556 = 0;
  Int R558 = 0;
  Int R559_0 = 0;
  List<Int> R561 = List<Int>{};
  Float R564 = 0.0;
  net_EndpointStats R565 = net_EndpointStats{0.0, 0.0, 0.0, 0.0, 0.0, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}, Int64{0x0}};
  net_EndpointConfig R566 = net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28};
  Int R571 = 0;
  Int R572 = 0;
  Int R573 = 0;
  Int R574 = 0;
  Bool R575 = false;
  Bool R576 = false;
  Int R577_0 = 0;
  Bool R578_0 = false;
  List<Int> R579_0 = List<Int>{};
  Int R580 = 0;
  Int R581 = 0;
  Int R582 = 0;
  Int R583 = 0;
  Bool R584_0 = false;
  Bool R585_0 = false;
  Bool R586_0 = false;
  Bool R587_0 = false;
  Bool R588_0 = false;
  Int R590 = 0;
  Int R591 = 0;
  Int R592 = 0;
  Bool R593 = false;
  Bool R594_0 = false;
  Int R594_1 = 0;
  Bool R595_0 = false;
  Int R595_1 = 0;
  Bool R596_0 = false;
  List<Int> R596_1 = List<Int>{};
  Bool R597_0 = false;
  Bool R598_0 = false;
  Int R599 = 0;
  Int R600 = 0;
  Int R601 = 0;
  Int R602 = 0;
  Bool R603_0 = false;
  Bool R604_0 = false;
  Bool R605_0 = false;
  Bool R606_0 = false;
  Bool R607_0 = false;
  Int R608 = 0;
  Int R609 = 0;
  Int R610 = 0;
  net_Address R611 = net_Address{Int64{0x0}};
  Bool R612_0 = false;
  Int R613_0 = 0;
  net_Address R613_1 = net_Address{Int64{0x0}};
  List<Int> R613_2 = List<Int>{};
  List<net_Simulator_Packet> In73 = List<net_Simulator_Packet>{};
  List<net_Simulator_Packet> R616_0 = List<net_Simulator_Packet>{};
  Int R618 = 0;
  Int64 R619 = Int64{0x0};
  Int64 R620 = Int64{0x0};
  Int R621_0 = 0;
  Int R622_0 = 0;
  Float R624 = 0.0;
  Int64 R625 = Int64{0x0};
  net_Address R626 = net_Address{Int64{0x0}};
  Bool R628_0 = false;
  Int R629_0 = 0;
  net_Address R629_1 = net_Address{Int64{0x0}};
  List<Int> R629_2 = List<Int>{};
  Bool R632_0 = false;
  net_Address R632_1 = net_Address{Int64{0x0}};
  List<Int> R632_2 = List<Int>{};
  Int R633_0 = 0;
  physics_Surface In74 = physics_Surface::none;
  Text R634 = "";
  Bool R635_0 = false;
  physics_Surface R635_1 = physics_Surface::none;
  Float R636 = 0.0;
  physics_Pose R637 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
  physics_Version R638 = physics_Version{0, 0, 0};
  physics_WorldSettings In75 = physics_WorldSettings{Vector{0.0, 0.0, -9.80665}, true, true, 1.0, 2, false, 1.0, 30.0, 10.0, 3.0, 400.0};
  Bool R639_0 = false;
  Bool R641 = false;
  physics_BodySettings In76 = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
  physics_Body R642_0 = physics_Body{Int64{0x0}};
  physics_Body In77 = physics_Body{Int64{0x0}};
  Bool R644 = false;
  physics_ShapeSettings In78 = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
  physics_Shape R645_0 = physics_Shape{Int64{0x0}};
  physics_Shape In79 = physics_Shape{Int64{0x0}};
  Int R647 = 0;
  physics_JointSettings In80 = physics_JointSettings{physics_JointType::revolute, physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}}, false, false, 0.0, 0.0, false, 0.0, 0.0, false, 0.0, 0.0, 0.0, Vector{0.0, 0.0, 0.0}, 1.0, 0.0, 0.0, 1.0, 1.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, 0.0, false, false, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, Int64{0x0}};
  physics_Joint R648_0 = physics_Joint{Int64{0x0}};
  physics_Joint In81 = physics_Joint{Int64{0x0}};
  Bool R650 = false;
  physics_Pose R652 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
  physics_Pose In82 = physics_Pose{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}};
  Vector R655 = Vector{0.0, 0.0, 0.0};
  Vector R656 = Vector{0.0, 0.0, 0.0};
  Bool R666 = false;
  Float R667 = 0.0;
  physics_BodyType R668 = physics_BodyType::fixed;
  physics_BodyType In83 = physics_BodyType::fixed;
  List<Int64> R670_0 = List<Int64>{};
  Vector R672 = Vector{0.0, 0.0, 0.0};
  Vector R673 = Vector{0.0, 0.0, 0.0};
  Vector R674 = Vector{0.0, 0.0, 0.0};
  Int64 R675 = Int64{0x0};
  Int64 R676 = Int64{0x0};
  physics_Body R677 = physics_Body{Int64{0x0}};
  physics_MotionLocks In84 = physics_MotionLocks{Fixed<Bool, 3>{}, Fixed<Bool, 3>{}};
  Vector R682 = Vector{0.0, 0.0, 0.0};
  Float R685 = 0.0;
  Float R686 = 0.0;
  Float R687 = 0.0;
  Float R689 = 0.0;
  Vector R690 = Vector{0.0, 0.0, 0.0};
  List<physics_ContactPoint> In85 = List<physics_ContactPoint>{};
  List<physics_ContactPoint> R691_0 = List<physics_ContactPoint>{};
  List<physics_Body_Float_pair> In86 = List<physics_Body_Float_pair>{};
  List<physics_Body_Float_pair> R692_0 = List<physics_Body_Float_pair>{};
  physics_RayHit R693 = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
  List<physics_Body> In87 = List<physics_Body>{};
  physics_RayHit R694 = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
  physics_RayHit R695 = physics_RayHit{false, physics_Shape{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0};
  physics_Capsule In88 = physics_Capsule{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.3};
  Float R696 = 0.0;
  List<physics_MoverPlane> In89 = List<physics_MoverPlane>{};
  List<physics_MoverPlane> R697_0 = List<physics_MoverPlane>{};
  List<physics_TouchEvent> R700 = List<physics_TouchEvent>{};
  List<physics_TouchEvent> R701 = List<physics_TouchEvent>{};
  List<physics_TouchEvent> R702 = List<physics_TouchEvent>{};
  List<physics_TouchEvent> R703 = List<physics_TouchEvent>{};
  List<physics_HitEvent> R704 = List<physics_HitEvent>{};
  List<physics_Joint> R705 = List<physics_Joint>{};
  Int R706 = 0;
  Int R707 = 0;
  Int64 R708 = Int64{0x0};
  Vector R709 = Vector{0.0, 0.0, 0.0};
  Bool R710_0 = false;
  Vector R712 = Vector{0.0, 0.0, 0.0};
  Bool R715 = false;
  physics_Body R716 = physics_Body{Int64{0x0}};
  Vector R717 = Vector{0.0, 0.0, 0.0};
  Float R718 = 0.0;
  Bool R720_0 = false;
  Vector R721 = Vector{0.0, 0.0, 0.0};
  Vector R722 = Vector{0.0, 0.0, 0.0};
  Bool R725 = false;
  List<physics_MoverPlane> R726 = List<physics_MoverPlane>{};
  physics_Capsule R727 = physics_Capsule{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.3};
  Bool R728_0 = false;
  MeshData R728_1 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  Bool R730 = false;
  Int R731 = 0;
  Bool R732_0 = false;
  Ray R732_1 = Ray{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, -1.0}, 0.05, 1800.0};
  Bool R733_0 = false;
  PickHit R733_1 = PickHit{0, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}};
  PickStats R733_2 = PickStats{0, 0};
  Int R734_0 = 0;
  Int R736_0 = 0;
  Int R738_0 = 0;
  Int R740_0 = 0;
  Int R742_0 = 0;
  Int R744_0 = 0;
  Int R746_0 = 0;
  Int R748_0 = 0;
  Int R750_0 = 0;
  Int R752_0 = 0;
  Int R754_0 = 0;
  Int R756_0 = 0;
  Int R758_0 = 0;
  Int R760_0 = 0;
  Int R762_0 = 0;
  Int R764_0 = 0;
  Int R766_0 = 0;
  Int R768_0 = 0;
  Int R770_0 = 0;
  Int R772_0 = 0;
  Int R774_0 = 0;
  Int R776_0 = 0;
  Int R778_0 = 0;
  Int R780_0 = 0;
  Int R782_0 = 0;
  Int R784_0 = 0;
  Int R786_0 = 0;
  Int R788_0 = 0;
  Int R790_0 = 0;
  Int R792_0 = 0;
  Int R794_0 = 0;
  Int R796_0 = 0;
  net_ClientServerConfig In90 = net_ClientServerConfig{Int64{0x4b45524e30303031}, 16, 6.0, 0.1, 6.0, 0.1, net_ConnectionConfig{List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}, 8192, 256, net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}}};
  Int R798_0 = 0;
  net_ConnectionConfig In91 = net_ConnectionConfig{List<net_ChannelConfig>{net_ChannelConfig{net_ChannelType::reliable_ordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}, net_ChannelConfig{net_ChannelType::unreliable_unordered, 1024, 1024, 256, 0, 65536, 512, 1024, 0.1}}, 8192, 256, net_EndpointConfig{16384, 1024, 1024, 16, 256, 256, 256, 64, 0.1, 0.1, 0.1, 28}};
  Int R800_0 = 0;
  Int R802_0 = 0;
  Int R804_0 = 0;
  Int R806_0 = 0;
  Int R808_0 = 0;
  Int R810_0 = 0;
  Int R812_0 = 0;
  Int R814_0 = 0;
  Int R816_0 = 0;
  Int H348 = 0;
  List<GeoChunk> P348_0 = List<GeoChunk>{};
  Int P348_1 = 0;
  GeoChunk P348_2 = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}};
  List<GeoChunk> P348_3 = List<GeoChunk>{};
  List<GeoChunk> P348_4 = List<GeoChunk>{};
  List<GeoChunk> In92 = List<GeoChunk>{};
  Int H380 = 0;
  List<GeoChunk> P380_0 = List<GeoChunk>{};
  Int P380_1 = 0;
  GeoChunk P380_2 = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}};
  List<GeoChunk> P380_3 = List<GeoChunk>{};
  List<GeoChunk> P380_4 = List<GeoChunk>{};

  // ---- functions ----
  void Run1() {
    R0 = engine_input_down(0, In1);
    R1 = engine_input_pressed(0, In1);
    R2 = engine_input_released(0, In1);
    R3 = engine_input_pointer(0);
    R4 = engine_input_pointer_delta(0);
    R5 = engine_input_button(0, 0);
    R6 = engine_input_button_pressed(0, 0);
    R7 = engine_input_wheel(0);
    R8 = engine_input_touches(0);
    R9 = engine_input_touch_count(0);
    R10 = engine_input_focused(0);
    engine_input_key(0, In1, false);
    engine_input_pointer_move(0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0});
    engine_input_pointer_buttons(0, 0);
    engine_input_add_wheel(0, 0.0);
    engine_input_touch(0, 0, 0, Vector{0.0, 0.0, 0.0});
    engine_input_set_focus(0, false);
    engine_input_end_frame(0);
    R18 = engine_engine_renderer(0);
    engine_engine_input(n11, 0);
    R19_0 = n11.result;
    R20 = engine_engine_input_2(0);
    engine_engine_set_status(0, "");
    engine_engine_request_pointer_lock(0, false);
    R23 = engine_engine_arg(0, "", "");
    R24 = engine_engine_has_arg(0, "");
    R25 = engine_engine_arg_number(0, "", 0.0);
    engine_engine_set_args(0, "", 0);
    R27 = engine_engine_width(0);
    R28 = engine_engine_height(0);
    R29 = engine_engine_aspect(0);
    R30 = engine_engine_stats(0);
    R31 = engine_engine_clear_color(0);
    engine_engine_set_clear_color(0, In2);
    R33 = engine_engine_xr(0);
    engine_engine_set_xr(0, false);
    R35 = engine_engine_xr_eye(0);
    engine_engine_set_xr_eye(0, Vector{0.0, 0.0, 0.0});
    R37 = engine_engine_xr_forward(0);
    engine_engine_set_xr_forward(0, Vector{0.0, 0.0, 0.0});
    R39 = engine_engine_xr_room_forward(0);
  }

  void Run2() {
    engine_engine_set_xr_room_forward(0, Vector{0.0, 0.0, 0.0});
    R41 = engine_engine_xr_head_yaw(0);
    engine_engine_set_xr_head_yaw(0, 0.0);
    R43 = engine_engine_xr_follow_head(0);
    engine_engine_set_xr_follow_head(0, false);
    R45 = engine_engine_xr_hold_room(0);
    engine_engine_set_xr_hold_room(0, false);
    R47 = engine_engine_xr_full_room(0);
    engine_engine_set_xr_full_room(0, false);
    R49 = engine_engine_xr_room_up(0);
    engine_engine_set_xr_room_up(0, Vector{0.0, 0.0, 0.0});
    R51 = engine_engine_xr_vignette(0);
    engine_engine_set_xr_vignette(0, 0.0);
    R53 = engine_engine_xr_vignette_mesh(0);
    engine_engine_set_xr_vignette_mesh(0, 0);
    R55 = engine_engine_xr_scope(0);
    engine_engine_set_xr_scope(0, false);
    R57 = engine_engine_xr_scope_camera(0);
    engine_engine_set_xr_scope_camera(0, In3);
    R59 = engine_engine_xr_scope_fbo(0);
    engine_engine_set_xr_scope_fbo(0, 0);
    R61 = engine_engine_xr_scope_size(0);
    engine_engine_set_xr_scope_size(0, 0);
    R63 = engine_engine_stereo(0);
    engine_engine_set_stereo(0, false);
    R65 = engine_engine_stereo_ipd(0);
    engine_engine_set_stereo_ipd(0, 0.0);
    R67 = engine_engine_stereo_cross(0);
    engine_engine_set_stereo_cross(0, false);
    engine_app_configure(n12, 0);
    R69_0 = n12.result;
    engine_app_extensions(n13, 0);
    R70_0 = std::move(n13.result);
    engine_app_start(n14, 0, 0);
    R71_0 = n14.result;
    engine_app_update(0, 0, In4);
    engine_app_resized(0, 0);
    engine_app_stop(0, 0);
    engine_mixer_init(n17, 0, 0, 0);
    R79_0 = n17.result;
    R80 = engine_mixer_sample_rate(0);
    R81 = engine_mixer_ready(0);
    engine_mixer_create_clip(n18, 0, List<Float>{}, 0, 0, 0, "");
    R82_0 = n18.result;
    engine_mixer_adopt_clip(n19, 0, List<Float>{}, 0, "", false);
    R83_0 = n19.result;
  }

  void Run3() {
    engine_mixer_destroy_clip(n20, 0, 0);
    R84_0 = n20.result;
    R85 = engine_mixer_clip_info(0, 0);
    R86 = engine_mixer_clip_samples(0, 0);
    R87 = engine_mixer_clip_count(0);
    engine_mixer_play(n21, 0, 0, In7);
    R88_0 = n21.result;
    engine_mixer_stop(0, 0, 0.0);
    engine_mixer_stop_all(0, In5, 0.0);
    R91 = engine_mixer_playing(0, 0);
    engine_mixer_set_volume(0, 0, 0.0, 0.0);
    engine_mixer_set_pitch(0, 0, 0.0);
    engine_mixer_set_pan(0, 0, 0.0);
    engine_mixer_set_position(0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0});
    engine_mixer_set_paused(0, 0, false);
    engine_mixer_set_all_paused(0, false);
    R98 = engine_mixer_position_of(0, 0);
    engine_mixer_set_listener(0, In8);
    R100 = engine_mixer_listener(0);
    engine_mixer_bus(n22, 0, In5);
    R101_0 = n22.result;
    R102 = engine_mixer_bus_2(0, In5);
    engine_mixer_set_reverb(0, 0.0, 0.0, 0.0);
    engine_mixer_render(n23, 0, 0);
    R104_0 = std::move(n23.out);
    R105 = engine_mixer_stats(0);
    engine_builtin_job_start(n28, 0, "", 0);
    R113_0 = n28.result;
    engine_builtin_job_step(n29, 0, 0.0);
    R114_0 = n29.result;
    R115 = engine_builtin_job_done(0);
    R116 = engine_builtin_job_name(0);
    engine_builtin_job_take(n30, 0, 0);
    R117_0 = std::move(n30.result);
    R117_1 = n30.channels;
    R182 = engine_night_windows_name(0);
    R183 = engine_night_windows_attributes(0);
    R184 = engine_night_windows_vertex_declarations(0);
    R185 = engine_night_windows_vertex(0);
    R186 = engine_night_windows_fragment_declarations(0);
    R187 = engine_night_windows_fragment_surface(0);
    engine_night_windows_bind(0, 0);
    engine_night_windows_frame_uniforms(0, 0, 0);
    engine_game_world_set_layers(0, 0);
    engine_game_world_sync(0, In27);
    engine_game_world_clear(0);
    R195 = engine_game_world_actor_for_mesh(0, 0);
    engine_game_world_actor_matrix(n40, 0, 0, In16);
    R196_0 = n40.result;
    R196_1 = n40.out;
  }

  void Run4() {
    engine_game_world_actor_bounds(n41, 0, 0, In31);
    R197_0 = n41.result;
    R197_1 = n41.out;
    engine_game_world_actor_meshes(n42, 0, 0, List<Int>{});
    R198_0 = std::move(n42.out);
    R199 = engine_game_world_pending_textures(0);
    engine_game_world_set_texture_pixels(n43, 0, "", 0, 0, List<Int>{});
    R200_0 = n43.result;
    engine_game_world_apply_lighting(n44, 0, Vector{0.0, 0.0, 0.0});
    R201_0 = n44.result;
    R202 = engine_game_world_time_of_day(0);
    engine_game_world_present(0, 0.0, Vector{0.0, 0.0, 0.0});
    engine_game_world_start(n45, 0, In27, In32);
    R204_0 = n45.result;
    engine_game_world_stop(0);
    R206 = engine_game_world_running(0);
    R207 = engine_game_world_session(0);
    R208 = engine_game_world_mode(0);
    engine_game_world_set_paused(0, false);
    R210 = engine_game_world_paused(0);
    engine_game_world_set_mode(n46, 0, In32);
    R211_0 = n46.result;
    engine_game_world_step(0, 0.0, In33);
    engine_game_world_step_once(0, In33);
    engine_game_world_restart(n47, 0);
    R214_0 = n47.result;
    engine_game_world_update_camera(n48, 0, In3, 0.0, In33);
    R215_0 = n48.result;
    R215_1 = n48.camera;
    R216 = engine_game_world_player(0);
    R217 = engine_game_world_camera_yaw(0);
    R218 = engine_game_world_interior_view(0);
    R219 = engine_game_world_camera_pitch(0);
    R220 = engine_game_world_sim_time(0);
    engine_game_world_add_pawn(n49, 0, 0, 0);
    R221_0 = n49.result;
    engine_game_world_remove_pawn(n50, 0, 0);
    R222_0 = n50.result;
    R223 = engine_game_world_pawn_source(0, 0);
    engine_game_world_set_pawn_input(0, 0, In33);
    engine_game_world_set_local_pawn(0, 0);
    R226 = engine_game_world_pawns(0);
    engine_game_world_take_alive_changes(n51, 0, In34);
    R227_0 = std::move(n51.out);
    R228 = engine_game_world_dead_actors(0);
    engine_game_world_set_alive(0, 0, false);
    engine_game_world_networked_actors(n52, 0, List<Int>{});
    R230_0 = std::move(n52.out);
    engine_game_world_hud_mut(n53, 0);
    R231_0 = n53.result;
    n53.result = In35;
    R231_3 = n53.result;
    engine_game_world_start_replica(n54, 0, In27);
    R232_0 = n54.result;
    engine_game_world_set_replica_pose(0, 0, Vector{0.0, 0.0, 0.0}, In14);
    engine_game_world_step_replica(0, 0.0, 0.0, In33);
    R235 = engine_game_world_hud(0);
    R236 = engine_game_world_aim_blend(0);
  }

  void Run5() {
    R237 = engine_game_world_stats(0);
    R238 = engine_game_world_level(0);
    engine_game_world_physics(n55, 0);
    R239_0 = n55.result;
    R240 = engine_game_world_actor_body(0, 0);
    engine_game_world_begin_grab(n56, 0, 0, Vector{0.0, 0.0, 0.0});
    R241_0 = n56.result;
    engine_game_world_move_grab(0, Vector{0.0, 0.0, 0.0});
    engine_game_world_end_grab(0);
    R244 = engine_game_world_grabbing(0);
    engine_game_world_debug_lines(n57, 0, false, false, false, List<Float>{}, List<Float>{});
    R245_0 = std::move(n57.positions);
    R245_1 = n57.colors;
    engine_game_world_events(n58, 0);
    R246_0 = n58.result;
    list_add(n58.result, "");
    n58.result[0] = "";
    R246_1 = list_length<Text>(n58.result);
    R246_2 = n58.result[9];
    R246_3 = n58.result;
    engine_game_world_sound_cues(n59, 0);
    R247_0 = n59.result;
    list_add(n59.result, In36);
    n59.result[0] = In36;
    R247_1 = list_length<game_SoundCue>(n59.result);
    R247_2 = n59.result[9];
    R247_3 = n59.result;
    engine_game_world_shots(n60, 0);
    R248_0 = n60.result;
    list_add(n60.result, In37);
    n60.result[0] = In37;
    R248_1 = list_length<game_ShotEvent>(n60.result);
    R248_2 = n60.result[9];
    R248_3 = n60.result;
    engine_game_world_damage(0, 0, 0.0, 0, false, Vector{0.0, 0.0, 0.0});
    R250 = engine_game_world_health_of(0, 0);
    engine_game_world_set_health(0, 0, 0.0, 0.0, false);
    engine_game_world_soldier_state(n61, 0, 0, In38);
    R252_0 = n61.result;
    R252_1 = n61.out;
    engine_game_world_respawn_now(n62, 0, 0);
    R253_0 = n62.result;
    engine_game_world_apply_soldier_state(0, In38);
    engine_game_world_set_view(0, In3);
    engine_game_world_set_view_hand(0, In39);
    R257 = engine_game_world_actor_alive(0, 0);
    R258 = engine_game_world_last_input(0);
    engine_game_world_logic_runtime(n63, 0);
    R259_0 = n63.result;
    engine_game_world_key_event(0, "", false);
    engine_game_world_take_replicated_changes(n64, 0, List<Int>{}, In40);
    R261_0 = std::move(n64.props);
    R261_1 = n64.spawns;
    engine_game_world_replicated_state(n65, 0, List<Int>{}, In40);
    R262_0 = std::move(n65.props);
    R262_1 = n65.spawns;
    engine_game_world_set_property(n66, 0, 0, "", 0);
    R263_0 = n66.result;
    engine_game_world_set_property_number(n67, 0, 0, "", 0.0);
    R264_0 = n67.result;
    engine_game_world_set_property_flag(n68, 0, 0, "", false);
    R265_0 = n68.result;
    engine_game_world_set_property_text(n69, 0, 0, "", "");
    R266_0 = n69.result;
    engine_game_world_spawn_copy(n70, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0);
    R267_0 = n70.result;
    engine_game_world_teleport(n71, 0, 0, Vector{0.0, 0.0, 0.0}, List<Vector>{});
    R268_0 = n71.result;
    R269 = engine_game_world_actor_velocity(0, 0);
    engine_game_world_set_actor_velocity(n72, 0, 0, Vector{0.0, 0.0, 0.0});
    R270_0 = n72.result;
    engine_game_world_push_actor(n73, 0, 0, Vector{0.0, 0.0, 0.0}, false);
    R271_0 = n73.result;
    engine_game_world_set_simulate_physics(n74, 0, 0, false);
    R272_0 = n74.result;
    engine_game_world_set_joint_motor(n75, 0, 0, 0, false, 0.0, 0.0);
    R273_0 = n75.result;
    R274 = engine_game_world_find_actor(0, "");
    engine_game_world_actors_with_tag(n76, 0, "", List<Int>{});
    R275_0 = std::move(n76.out);
    R276 = engine_game_world_has_tag(0, 0, "");
  }

  void Run6() {
    engine_game_world_line_trace(n77, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, In41);
    R277_0 = n77.result;
    R277_1 = n77.actor;
    R277_2 = n77.point;
    R277_3 = n77.normal;
    engine_game_world_add_score(0, 0.0);
    engine_game_world_end_game(0, false, "");
    engine_game_world_show_message(0, "");
    engine_game_world_play_sound(0, "", Vector{0.0, 0.0, 0.0}, 0.0, 0.0, false, 0);
    engine_game_world_particle_burst(0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0});
    engine_game_world_shake_camera(0, 0.0);
    engine_game_world_aim_view_model(0, Vector{0.0, 0.0, 0.0}, 0.0, false);
    engine_game_world_destroy_actor(0, 0);
    engine_game_world_heal(0, 0, 0.0);
    engine_game_world_give_ammo(0, 0, 0);
    engine_game_world_call_logic_event(0, "", 0);
    engine_game_world_walker_sensors(n78, 0, 0, In42);
    R289_0 = n78.result;
    R289_1 = n78.out;
    engine_game_world_walker_camera(n79, 0, 0, In3);
    R290_0 = n79.result;
    R290_1 = n79.out;
    engine_game_world_walker_command(n80, 0, 0, 0.0, 0.0, 0.0, 0.0);
    R291_0 = n80.result;
    engine_game_world_walker_look(n81, 0, 0, 0.0, 0.0, 0.0, 0.0);
    R292_0 = n81.result;
    engine_game_world_walker_set_autonomous(n82, 0, 0, false);
    R293_0 = n82.result;
    engine_game_world_walker_quack(n83, 0, 0);
    R294_0 = n83.result;
    engine_game_world_walker_skill(n84, 0, 0, "");
    R295_0 = std::move(n84.result);
    engine_game_world_walker_mouth(n85, 0, 0, 0.0);
    R296_0 = n85.result;
    R297 = engine_game_world_walker_holding(0, 0);
    engine_game_world_walker_pet(n86, 0, 0, 0.0);
    R298_0 = n86.result;
    engine_game_world_walker_hear(n87, 0, 0, false);
    R299_0 = n87.result;
    engine_game_world_walker_servo_load(n88, 0, 0, 0.0, 0.0, 0.0);
    R300_0 = n88.result;
    R300_1 = n88.strain;
    R300_2 = n88.drive;
    R300_3 = n88.speed;
    R301 = engine_game_world_walker_driving(0, 0);
    engine_game_world_set_walker_external(n89, 0, 0, false);
    R302_0 = n89.result;
    engine_game_world_set_walker_pose_edit(n90, 0, 0, false);
    R303_0 = n90.result;
    engine_game_world_set_walker_pose(n91, 0, 0, List<Float>{});
    R304_0 = n91.result;
    engine_game_world_set_walker_action(n92, 0, 0, List<Float>{});
    R305_0 = n92.result;
    R306 = engine_game_world_walker_tick_due(0, 0);
    engine_game_world_walker_probe(n93, 0, 0, In43);
    R307_0 = n93.result;
    R307_1 = n93.out;
    engine_game_world_walker_push(n94, 0, 0, Vector{0.0, 0.0, 0.0});
    R308_0 = n94.result;
    R309 = engine_game_world_walker_said(0, 0);
    R310 = engine_game_world_walker_joints(0, 0);
    engine_game_world_set_walker_play_external(n95, 0, 0, false);
    R311_0 = n95.result;
    engine_game_world_set_walker_play_action(n96, 0, 0, List<Float>{});
    R312_0 = n96.result;
    engine_game_world_set_walker_detector(n97, 0, 0, In44);
    R313_0 = n97.result;
    R314 = engine_game_world_walker_play_due(0, 0);
    R315 = engine_game_world_walker_playing(0, 0);
    engine_game_world_walker_sees(n98, 0, 0, 0, false, 0.0, 0.0);
    R316_0 = n98.result;
    R316_1 = n98.visible;
    R316_2 = n98.bearing;
    R316_3 = n98.distance;
  }

  void Run7() {
    R317 = engine_game_world_walker_state(0, 0);
    R318 = engine_game_world_walkers(0);
    R319 = engine_game_world_art_stats(0);
    engine_sound_bank_mixer(n99, 0);
    R320_0 = n99.result;
    engine_sound_bank_clip(n100, 0, "");
    R321_0 = n100.result;
    engine_sound_bank_request(0, "");
    engine_sound_bank_request_level(0, In27);
    engine_sound_bank_sync_assets(0, In27);
    engine_sound_bank_prepare(n101, 0, 0.0);
    R325_0 = n101.result;
    R326 = engine_sound_bank_ready(0, "");
    R327 = engine_sound_bank_failed(0, "");
    R328 = engine_sound_bank_pending_imports(0);
    R329 = engine_sound_bank_source(0, "");
    engine_sound_bank_set_pcm(n102, 0, "", List<Float>{}, 0, 0, 0);
    R330_0 = n102.result;
    engine_sound_bank_set_failed(0, "", "");
    R332 = engine_sound_bank_duration(0, "");
    engine_sound_bank_clear(0);
    R334 = engine_sound_bank_queued(0);
    engine_game_audio_begin(0, 0);
    engine_game_audio_update(0, 0, In3, 0.0);
    engine_game_audio_end(0, 0.0);
    R338 = engine_game_audio_active(0);
    engine_game_audio_apply_settings(0, In28);
    engine_game_audio_set_muted(0, false);
    R341 = engine_game_audio_muted(0);
    R342 = engine_game_audio_cues_played(0);
    R343 = engine_game_audio_cues_dropped(0);
    engine_building_builder_begin(n103, 0, List<Float>{}, 0, In46);
    R345_0 = n103.result;
    engine_building_builder_step(n104, 0, 0.0);
    R346_0 = n104.result;
    R347 = engine_building_builder_done(0);
    engine_building_builder_chunks(n105, 0);
    R348_0 = n105.result;
    list_add(n105.result, In47);
    n105.result[0] = In47;
    R348_1 = list_length<GeoChunk>(n105.result);
    R348_2 = n105.result[9];
    R348_3 = n105.result;
    R349 = engine_building_builder_processed(0);
    R350 = engine_building_builder_built(0);
    R351 = engine_building_builder_skipped(0);
    R352 = engine_building_builder_triangles(0);
    engine_footprint_stream_begin(0, 0, 0.0, 0.0);
    engine_footprint_stream_begin_2(0, 0, 0.0, 0.0, 0.0, 0.0);
    R355 = engine_footprint_stream_empty(0);
    engine_footprint_stream_ring(0, List<Vector>{}, 0);
    R357 = engine_footprint_stream_data(0);
  }

  void Run8() {
    R358 = engine_footprint_stream_size(0);
    engine_earcut_triangulate(n107, 0, List<Float>{}, 0, List<Int>{}, 0, List<Int>{});
    R361_0 = std::move(n107.out);
    engine_road_builder_begin(n111, 0, List<Float>{}, 0, 0, 0);
    R376_0 = n111.result;
    engine_road_builder_step(n112, 0, 0.0);
    R377_0 = n112.result;
    R378 = engine_road_builder_done(0);
    R379 = engine_road_builder_failed(0);
    engine_road_builder_chunks(n113, 0);
    R380_0 = n113.result;
    list_add(n113.result, In47);
    n113.result[0] = In47;
    R380_1 = list_length<GeoChunk>(n113.result);
    R380_2 = n113.result[9];
    R380_3 = n113.result;
    R381 = engine_road_builder_roads(0);
    R382 = engine_road_builder_triangles(0);
    engine_road_stream_road(0, List<Vector>{}, 0, 0.0, false, 0, 0);
    R384 = engine_road_stream_data(0);
    R385 = engine_road_stream_size(0);
    R387 = engine_mesh_skinned(0);
    R388 = engine_mesh_cull_mode(0);
    R389 = engine_mesh_vertex_count(0);
    R390 = engine_mesh_element_count(0);
    R391 = engine_mesh_triangle_count(0);
    R392 = engine_mesh_posed_vertex(0, 0);
    engine_particles_init(n115, 0, 0, 0, 0);
    R397_0 = n115.result;
    engine_particles_emit(n116, 0, In52);
    R398_0 = n116.result;
    engine_particles_tick(n117, 0, 0.0, Vector{0.0, 0.0, 0.0});
    R399_0 = n117.result;
    engine_particles_clear(0);
    R401 = engine_particles_mesh_id(0);
    R402 = engine_particles_active(0);
    R403 = engine_particles_stats(0);
    engine_light_pool_request(n118, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0);
    R404_0 = n118.result;
    engine_light_pool_resolve(n119, 0, Vector{0.0, 0.0, 0.0});
    R405_0 = std::move(n119.result);
    engine_light_pool_clear(0);
    engine_sun_shadow_map_init(n120, 0, In53);
    R407_0 = n120.result;
    engine_sun_shadow_map_destroy(0, false);
    R409 = engine_sun_shadow_map_valid(0);
    engine_sun_shadow_map_focus_key(n121, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0);
    R410_0 = n121.result;
    engine_sun_shadow_map_draw(n122, 0, List<Int>{}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0, 0.0);
    R411_0 = n122.result;
    R412 = engine_sun_shadow_map_matrix(0);
    R413 = engine_sun_shadow_map_depth_texture(0);
    R414 = engine_sun_shadow_map_options(0);
    engine_sun_shadow_map_set_filter(0, 0);
    R416 = engine_sun_shadow_map_extent(0);
    engine_sky_pass_init(n123, 0);
    R417_0 = n123.result;
    engine_sky_pass_destroy(0, false);
  }

  void Run9() {
    engine_sky_pass_draw(0, In3, 0.0, In54, 0.0, 0.0, In55);
    engine_bloom_pass_configure(0, In56);
    engine_bloom_pass_begin(n124, 0, 0, 0, 0.0, false);
    R421_0 = n124.result;
    engine_bloom_pass_end(0, 0.0, In57);
    engine_bloom_pass_destroy(0, false);
    R424 = engine_bloom_pass_state(0);
    R425 = engine_bloom_pass_depth_texture(0);
    engine_bloom_pass_bind_scene(0);
    engine_ambient_occlusion_pass_configure(0, In58);
    engine_ambient_occlusion_pass_set_radius(0, 0.0);
    R429 = engine_ambient_occlusion_pass_options(0);
    engine_ambient_occlusion_pass_compute(n125, 0, 0, 0, 0, In16, 0.0);
    R430_0 = n125.result;
    R431 = engine_ambient_occlusion_pass_texture(0);
    R432 = engine_ambient_occlusion_pass_width(0);
    R433 = engine_ambient_occlusion_pass_height(0);
    engine_ambient_occlusion_pass_destroy(0, false);
    R435 = engine_ambient_occlusion_pass_quality(0);
    R436 = engine_ambient_occlusion_pass_half_resolution(0);
    engine_program_build(n126, 0, "", "", "");
    R437_0 = n126.result;
    engine_program_destroy(0, false);
    R439 = engine_program_handle(0);
    R440 = engine_program_valid(0);
    R441 = engine_program_location(0, "");
    engine_program_use(0);
    engine_uniform_cache_reset(0);
    engine_uniform_cache_set_int(0, 0, 0);
    engine_uniform_cache_set_float(0, 0, 0.0);
    engine_uniform_cache_set_vec2(0, 0, 0.0, 0.0);
    engine_uniform_cache_set_vec3(0, 0, Vector{0.0, 0.0, 0.0});
    engine_uniform_cache_set_vec4(0, 0, In2);
    engine_uniform_cache_set_vec4_array(0, 0, List<Float>{}, 0);
    engine_uniform_cache_set_vec3_array(0, 0, List<Float>{}, 0);
    engine_uniform_cache_set_mat4(0, 0, List<Float>{});
    engine_renderer_init(n127, 0, In59, List<Int>{});
    R453_0 = n127.result;
    engine_renderer_shutdown(0);
    engine_renderer_resize(n128, 0, 0, 0);
    R455_0 = n128.result;
    R456 = engine_renderer_width(0);
    R457 = engine_renderer_height(0);
    engine_renderer_allocate_id(n129, 0);
    R458_0 = n129.result;
    engine_renderer_release_id(0, 0);
  }

  void Run10() {
    R460 = engine_renderer_id_allocated(0, 0);
    engine_renderer_upsert(n130, 0, 0, In60);
    R461_0 = n130.result;
    engine_renderer_update_instances(n131, 0, 0, List<Float>{}, List<Float>{});
    R462_0 = n131.result;
    R462_1 = n131.changed;
    engine_renderer_update_bones(n132, 0, 0, List<Float>{});
    R463_0 = n132.result;
    engine_renderer_set_visible(n133, 0, 0, false);
    R464_0 = n133.result;
    engine_renderer_remove(n134, 0, 0);
    R465_0 = n134.result;
    engine_renderer_set_material(n135, 0, 0, In61);
    R466_0 = n135.result;
    engine_renderer_set_layers(n136, 0, 0, 0);
    R467_0 = n136.result;
    R468 = engine_renderer_mesh(0, 0);
    R469 = engine_renderer_meshes(0);
    R470 = engine_renderer_mesh_count(0);
    engine_renderer_create_texture(n137, 0, 0);
    R471_0 = n137.result;
    engine_renderer_register_external_texture(n138, 0, 0);
    R472_0 = n138.result;
    engine_renderer_create_render_target(n139, 0, 0, 0);
    R473_0 = n139.result;
    R473_1 = n139.framebuffer;
    engine_renderer_delete_texture(n140, 0, 0);
    R474_0 = n140.result;
    R475 = engine_renderer_camera(0);
    engine_renderer_set_camera(0, In3);
    R477 = engine_renderer_lighting(0);
    engine_renderer_set_lighting(0, In62);
    R479 = engine_renderer_sky(0);
    engine_renderer_set_sky(0, In54);
    engine_renderer_set_point_lights(n141, 0, In63);
    R481_0 = n141.result;
    R482 = engine_renderer_point_lights(0);
    engine_renderer_set_shadows(n142, 0, In64);
    R483_0 = n142.result;
    engine_renderer_set_shadow_distance(0, 0.0);
    engine_renderer_set_shadow_filter(0, 0);
    engine_renderer_set_ambient_occlusion(0, In58);
    engine_renderer_set_ambient_occlusion_radius(0, 0.0);
    R488 = engine_renderer_ambient_occlusion(0);
    R489 = engine_renderer_shadow(0);
    engine_renderer_invalidate(0, false);
    R491 = engine_renderer_scene_revision(0);
    engine_renderer_render(n143, 0, In65, In66);
    R492_0 = n143.result;
    R492_1 = n143.stats;
    R493 = engine_renderer_last_stats(0);
    engine_renderer_screen_ray(n144, 0, 0.0, 0.0, In67);
    R494_0 = n144.result;
    R494_1 = n144.ray;
    engine_renderer_pick(n145, 0, 0.0, 0.0, In68, In69);
    R495_0 = n145.result;
    R495_1 = n145.hit;
    R495_2 = n145.stats;
    engine_renderer_pick_ray(n146, 0, In67, In68, In69);
    R496_0 = n146.result;
    R496_1 = n146.hit;
    R496_2 = n146.stats;
    engine_renderer_context_lost(0);
    engine_renderer_context_restored(n147, 0);
    R498_0 = n147.result;
    R499 = engine_renderer_lost(0);
  }

  void Run11() {
    engine_renderer_set_headless(0);
    R501 = engine_renderer_extensions(0);
    R502 = engine_shader_extension_name(0);
    R503 = engine_shader_extension_attributes(0);
    R504 = engine_shader_extension_vertex_declarations(0);
    R505 = engine_shader_extension_vertex(0);
    R506 = engine_shader_extension_fragment_declarations(0);
    R507 = engine_shader_extension_fragment_discard(0);
    R508 = engine_shader_extension_fragment_input(0);
    R509 = engine_shader_extension_fragment_surface(0);
    engine_shader_extension_bind(0, 0);
    engine_shader_extension_frame_uniforms(0, 0, 0);
    engine_shader_extension_material_uniforms(0, 0, In61, List<Int>{});
    engine_client_connect(0, In71, 0.0);
    engine_client_disconnect(0);
    R517 = engine_client_state(0);
    R518 = engine_client_connected(0);
    R519 = engine_client_client_index(0);
    R520 = engine_client_max_clients(0);
    engine_client_receive_packets(0);
    engine_client_advance_time(0, 0.0);
    engine_client_send_packets(0);
    engine_client_send(n148, 0, 0, 0, List<Int>{});
    R524_0 = n148.result;
    engine_client_receive(n149, 0, 0);
    R525_0 = std::move(n149.result);
    engine_client_connection(n150, 0);
    R526_0 = n150.result;
    R527 = engine_client_connection_2(0);
    R528 = engine_client_server(0);
    engine_server_start(0, 0);
    engine_server_stop(0);
    R531 = engine_server_running(0);
    R532 = engine_server_max_clients(0);
    R533 = engine_server_client_connected(0, 0);
    R534 = engine_server_connected_clients(0);
    R535 = engine_server_client_address(0, 0);
    engine_server_disconnect_client(0, 0);
    engine_server_receive_packets(0);
    engine_server_advance_time(0, 0.0);
    engine_server_send_packets(0);
    engine_server_send(n151, 0, 0, 0, 0, List<Int>{});
    R540_0 = n151.result;
    engine_server_receive(n152, 0, 0, 0);
    R541_0 = std::move(n152.result);
  }

  void Run12() {
    engine_server_connection(n153, 0, 0);
    R542_0 = n153.result;
    engine_server_take_events(n154, 0);
    R543_0 = std::move(n154.result);
    engine_connection_reset(0);
    R546 = engine_connection_can_send(0, 0);
    engine_connection_send(n155, 0, 0, 0, List<Int>{});
    R547_0 = n155.result;
    engine_connection_receive(n156, 0, 0);
    R548_0 = std::move(n156.result);
    engine_connection_send_packet(0);
    engine_connection_receive_packet(0, List<Int>{}, 0);
    engine_connection_advance_time(0, 0.0);
    R552 = engine_connection_error(0);
    R553 = engine_connection_stats(0);
    engine_connection_endpoint(n157, 0);
    R554_0 = n157.result;
    R555 = engine_connection_channel_count(0);
    R556 = engine_connection_pending(0, 0);
    engine_endpoint_reset(0);
    R558 = engine_endpoint_next_sequence(0);
    engine_endpoint_send_packet(n158, 0, List<Int>{}, 0);
    R559_0 = n158.result;
    engine_endpoint_receive_packet(0, List<Int>{}, 0);
    R561 = engine_endpoint_acks(0);
    engine_endpoint_clear_acks(0);
    engine_endpoint_update(0, 0.0);
    R564 = engine_endpoint_time(0);
    R565 = engine_endpoint_stats(0);
    R566 = engine_endpoint_config(0);
    engine_bit_writer_write_bits(0, 0, 0);
    engine_bit_writer_write_align(0);
    engine_bit_writer_write_bytes(0, List<Int>{}, 0);
    engine_bit_writer_flush(0);
    R571 = engine_bit_writer_align_bits(0);
    R572 = engine_bit_writer_bits_written(0);
    R573 = engine_bit_writer_bits_available(0);
    R574 = engine_bit_writer_bytes_written(0);
    R575 = engine_bit_writer_overflow(0);
    R576 = engine_bit_reader_would_overflow(0, 0);
    engine_bit_reader_read_bits(n159, 0, 0);
    R577_0 = n159.result;
    engine_bit_reader_read_align(n160, 0);
    R578_0 = n160.result;
    engine_bit_reader_read_bytes(n161, 0, 0);
    R579_0 = std::move(n161.data);
    R580 = engine_bit_reader_align_bits(0);
    R581 = engine_bit_reader_bits_read(0);
    R582 = engine_bit_reader_bits_remaining(0);
  }

  void Run13() {
    R583 = engine_bit_reader_bytes_read(0);
    engine_write_stream_serialize_integer(n162, 0, 0, 0, 0);
    R584_0 = n162.result;
    engine_write_stream_serialize_bits(n163, 0, 0, 0);
    R585_0 = n163.result;
    engine_write_stream_serialize_bytes(n164, 0, List<Int>{}, 0);
    R586_0 = n164.result;
    engine_write_stream_serialize_align(n165, 0);
    R587_0 = n165.result;
    engine_write_stream_serialize_check(n166, 0, 0);
    R588_0 = n166.result;
    engine_write_stream_flush(0);
    R590 = engine_write_stream_bits_processed(0);
    R591 = engine_write_stream_bytes_processed(0);
    R592 = engine_write_stream_align_bits(0);
    R593 = engine_write_stream_overflow(0);
    engine_read_stream_serialize_integer(n167, 0, 0, 0, 0);
    R594_0 = n167.result;
    R594_1 = n167.value;
    engine_read_stream_serialize_bits(n168, 0, 0, 0);
    R595_0 = n168.result;
    R595_1 = n168.value;
    engine_read_stream_serialize_bytes(n169, 0, 0);
    R596_0 = n169.result;
    R596_1 = n169.data;
    engine_read_stream_serialize_align(n170, 0);
    R597_0 = n170.result;
    engine_read_stream_serialize_check(n171, 0, 0);
    R598_0 = n171.result;
    R599 = engine_read_stream_bits_processed(0);
    R600 = engine_read_stream_bytes_processed(0);
    R601 = engine_read_stream_bits_remaining(0);
    R602 = engine_read_stream_align_bits(0);
    engine_measure_stream_serialize_integer(n172, 0, 0, 0, 0);
    R603_0 = n172.result;
    engine_measure_stream_serialize_bits(n173, 0, 0, 0);
    R604_0 = n173.result;
    engine_measure_stream_serialize_bytes(n174, 0, List<Int>{}, 0);
    R605_0 = n174.result;
    engine_measure_stream_serialize_align(n175, 0);
    R606_0 = n175.result;
    engine_measure_stream_serialize_check(n176, 0, 0);
    R607_0 = n176.result;
    R608 = engine_measure_stream_bits_processed(0);
    R609 = engine_measure_stream_bytes_processed(0);
    R610 = engine_measure_stream_align_bits(0);
    R611 = engine_transport_address(0);
    engine_transport_send(n177, 0, In71, List<Int>{}, 0);
    R612_0 = n177.result;
    engine_transport_receive(n178, 0, In71, 0);
    R613_0 = n178.result;
    R613_1 = n178.from;
    R613_2 = n178.data;
    engine_transport_advance_time(0, 0.0);
    engine_simulator_send(0, In71, In71, List<Int>{}, 0, 0.0);
    engine_simulator_take_due(n179, 0, 0.0, In73);
    R616_0 = std::move(n179.out);
    engine_simulator_clear(0);
    R618 = engine_simulator_in_flight(0);
    R619 = engine_simulator_dropped(0);
    R620 = engine_simulator_duplicated(0);
    engine_loopback_network_create(n180, 0);
    R621_0 = n180.result;
    engine_loopback_network_simulator(n181, 0);
    R622_0 = n181.result;
  }

  void Run14() {
    engine_loopback_network_advance_time(0, 0.0);
    R624 = engine_loopback_network_time(0);
    R625 = engine_loopback_network_delivered(0);
    R626 = engine_host_transport_address(0);
    engine_host_transport_set_address(0, In71);
    engine_host_transport_send(n182, 0, In71, List<Int>{}, 0);
    R628_0 = n182.result;
    engine_host_transport_receive(n183, 0, In71, 0);
    R629_0 = n183.result;
    R629_1 = n183.from;
    R629_2 = n183.data;
    engine_host_transport_advance_time(0, 0.0);
    engine_host_transport_push_incoming(0, In71, List<Int>{}, 0);
    engine_host_transport_take_outgoing(n184, 0, In71, List<Int>{});
    R632_0 = n184.result;
    R632_1 = n184.to;
    R632_2 = n184.data;
    engine_host_transport_simulator(n185, 0);
    R633_0 = n185.result;
    engine_world_create(n187, 0, In75);
    R639_0 = n187.result;
    engine_world_destroy(0);
    R641 = engine_world_valid(0);
    engine_world_create_body(n188, 0, In76);
    R642_0 = n188.result;
    engine_world_destroy_body(0, In77);
    R644 = engine_world_body_valid(0, In77);
    engine_world_create_shape(n189, 0, In77, In78);
    R645_0 = n189.result;
    engine_world_destroy_shape(0, In79);
    R647 = engine_world_mesh_count(0);
    engine_world_create_joint(n190, 0, In80);
    R648_0 = n190.result;
    engine_world_destroy_joint(0, In81, false);
    R650 = engine_world_joint_valid(0, In81);
    engine_world_step(0, 0.0, 0);
    R652 = engine_world_pose(0, In77);
    engine_world_set_pose(0, In77, In82);
    engine_world_set_target_pose(0, In77, In82, 0.0);
    R655 = engine_world_linear_velocity(0, In77);
    R656 = engine_world_angular_velocity(0, In77);
    engine_world_set_linear_velocity(0, In77, Vector{0.0, 0.0, 0.0});
    engine_world_set_angular_velocity(0, In77, Vector{0.0, 0.0, 0.0});
    engine_world_apply_force(0, In77, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0});
    engine_world_apply_force_to_center(0, In77, Vector{0.0, 0.0, 0.0});
    engine_world_apply_torque(0, In77, Vector{0.0, 0.0, 0.0});
    engine_world_apply_linear_impulse(0, In77, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0});
    engine_world_apply_linear_impulse_to_center(0, In77, Vector{0.0, 0.0, 0.0});
    engine_world_apply_angular_impulse(0, In77, Vector{0.0, 0.0, 0.0});
    engine_world_set_awake(0, In77, false);
    R666 = engine_world_awake(0, In77);
    R667 = engine_world_mass(0, In77);
  }

  void Run15() {
    R668 = engine_world_body_type(0, In77);
    engine_world_set_body_type(0, In77, In83);
    engine_world_body_masks(n191, 0, In77, List<Int64>{});
    R670_0 = std::move(n191.out);
    engine_world_set_body_masks(0, In77, List<Int64>{});
    R672 = engine_world_local_point(0, In77, Vector{0.0, 0.0, 0.0});
    R673 = engine_world_world_point(0, In77, Vector{0.0, 0.0, 0.0});
    R674 = engine_world_world_vector(0, In77, Vector{0.0, 0.0, 0.0});
    R675 = engine_world_body_user(0, In77);
    R676 = engine_world_shape_user(0, In79);
    R677 = engine_world_shape_body(0, In79);
    engine_world_set_motion_locks(0, In77, In84);
    engine_world_set_mass(0, In77, 0.0);
    engine_world_set_mass_data(0, In77, 0.0, Vector{0.0, 0.0, 0.0}, List<Float>{});
    engine_world_set_joint_thresholds(0, In81, 0.0, 0.0);
    R682 = engine_world_joint_force(0, In81);
    engine_world_set_joint_motor(0, In81, false, 0.0, 0.0);
    engine_world_set_wheel_steering(0, In81, 0.0);
    R685 = engine_world_wheel_steering(0, In81);
    R686 = engine_world_joint_angle(0, In81);
    R687 = engine_world_joint_speed(0, In81);
    engine_world_set_joint_spring(0, In81, 0.0, 0.0, 0.0);
    R689 = engine_world_hinge_lever(0, In81, Vector{0.0, 0.0, 0.0});
    R690 = engine_world_joint_torque(0, In81);
    engine_world_contact_points(n192, 0, In77, In85);
    R691_0 = std::move(n192.out);
    engine_world_touching(n193, 0, In77, In86);
    R692_0 = std::move(n193.out);
    R693 = engine_world_cast_ray(0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, In41, In77);
    R694 = engine_world_cast_ray_ignoring(0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, In87, In41);
    R695 = engine_world_cast_sphere(0, Vector{0.0, 0.0, 0.0}, 0.0, Vector{0.0, 0.0, 0.0}, In41, In77);
    R696 = engine_world_cast_mover(0, Vector{0.0, 0.0, 0.0}, In88, Vector{0.0, 0.0, 0.0}, In41, In77);
    engine_world_collide_mover(n194, 0, Vector{0.0, 0.0, 0.0}, In88, In41, In77, In89);
    R697_0 = std::move(n194.out);
    engine_world_explode(0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, Int64{0x0});
    engine_world_push_body(0, In77, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0});
    R700 = engine_world_contact_begin(0);
    R701 = engine_world_contact_end(0);
    R702 = engine_world_sensor_begin(0);
    R703 = engine_world_sensor_end(0);
    R704 = engine_world_hits(0);
    R705 = engine_world_joint_events(0);
    R706 = engine_world_body_count(0);
    R707 = engine_world_awake_body_count(0);
  }

  void Run16() {
    R708 = engine_world_step_count(0);
    R709 = engine_world_gravity(0);
    engine_mouse_grab_begin(n195, 0, 0, In77, Vector{0.0, 0.0, 0.0}, 0.0, false);
    R710_0 = n195.result;
    engine_mouse_grab_set_target(0, Vector{0.0, 0.0, 0.0});
    R712 = engine_mouse_grab_target(0);
    engine_mouse_grab_update(0, 0.0);
    engine_mouse_grab_end(0);
    R715 = engine_mouse_grab_active(0);
    R716 = engine_mouse_grab_body(0);
    R717 = engine_mouse_grab_anchor(0);
    R718 = engine_mouse_grab_max_force(0);
    engine_character_mover_reset(0, Vector{0.0, 0.0, 0.0});
    engine_character_mover_step(n196, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, In77);
    R720_0 = n196.result;
    R721 = engine_character_mover_position(0);
    R722 = engine_character_mover_velocity(0);
    engine_character_mover_set_velocity(0, Vector{0.0, 0.0, 0.0});
    engine_character_mover_jump(0, 0.0);
    R725 = engine_character_mover_on_ground(0);
    R726 = engine_character_mover_planes(0);
    R727 = engine_character_mover_capsule(0);
    engine_bvh_build(0, List<Float>{}, 0, List<Int>{}, 0, 0);
    R730 = engine_bvh_empty(0);
    R731 = engine_bvh_triangle_count(0);
    engine_ambient_occlusion_pass_make(n200);
    R734_0 = n200.result;
    engine_ambient_occlusion_pass_release(0);
    engine_bloom_pass_make(n201);
    R736_0 = n201.result;
    engine_bloom_pass_release(0);
    engine_building_builder_make(n202);
    R738_0 = n202.result;
    engine_building_builder_release(0);
    engine_bvh_make(n203);
    R740_0 = n203.result;
    engine_bvh_release(0);
    engine_earcut_make(n204);
    R742_0 = n204.result;
    engine_earcut_release(0);
    engine_engine_release(0);
    engine_footprint_stream_make(n206);
    R746_0 = n206.result;
    engine_footprint_stream_release(0);
    engine_height_sampler_make(n207);
    R748_0 = n207.result;
    engine_height_sampler_release(0);
    engine_input_make(n208);
    R750_0 = n208.result;
    engine_input_release(0);
  }

  void Run17() {
    engine_light_pool_release(0);
    engine_mesh_make(n210);
    R754_0 = n210.result;
    engine_mesh_release(0);
    engine_particles_make(n211);
    R756_0 = n211.result;
    engine_particles_release(0);
    engine_program_make(n212);
    R758_0 = n212.result;
    engine_program_release(0);
    engine_renderer_make(n213);
    R760_0 = n213.result;
    engine_renderer_release(0);
    engine_road_builder_make(n214);
    R762_0 = n214.result;
    engine_road_builder_release(0);
    engine_road_options_make(n215);
    R764_0 = n215.result;
    engine_road_options_release(0);
    engine_road_stream_make(n216);
    R766_0 = n216.result;
    engine_road_stream_release(0);
    engine_sky_pass_make(n217);
    R768_0 = n217.result;
    engine_sky_pass_release(0);
    engine_sun_shadow_map_make(n218);
    R770_0 = n218.result;
    engine_sun_shadow_map_release(0);
    engine_texture_desc_make(n219);
    R772_0 = n219.result;
    engine_texture_desc_release(0);
    engine_uniform_cache_make(n220);
    R774_0 = n220.result;
    engine_uniform_cache_release(0);
    engine_builtin_job_make(n221);
    R776_0 = n221.result;
    engine_builtin_job_release(0);
    engine_mixer_make(n222);
    R778_0 = n222.result;
    engine_mixer_release(0);
    engine_game_audio_release(0);
    engine_game_world_release(0);
    engine_property_change_make(n225);
    R784_0 = n225.result;
    engine_property_change_release(0);
    engine_motion_asset_make(n226);
    R786_0 = n226.result;
    engine_motion_asset_release(0);
    engine_night_windows_make(n227);
    R788_0 = n227.result;
    engine_night_windows_release(0);
    engine_soldier_art_delete_make(n228);
    R790_0 = n228.result;
    engine_soldier_art_delete_release(0);
    engine_sound_asset_make(n229);
    R792_0 = n229.result;
    engine_sound_asset_release(0);
    engine_sound_bank_release(0);
  }

  void Run18() {
    engine_texture_asset_make(n231);
    R796_0 = n231.result;
    engine_texture_asset_release(0);
    engine_client_release(0);
    engine_connection_release(0);
    engine_host_transport_release(0);
    engine_loopback_network_release(0);
    engine_measure_stream_make(n236);
    R806_0 = n236.result;
    engine_measure_stream_release(0);
    engine_server_release(0);
    engine_simulator_release(0);
    engine_character_mover_make(n239);
    R812_0 = n239.result;
    engine_character_mover_release(0);
    engine_mouse_grab_make(n240);
    R814_0 = n240.result;
    engine_mouse_grab_release(0);
    engine_world_make(n241);
    R816_0 = n241.result;
    engine_world_release(0);
    engine_building_builder_make(n242);
    H348 = n242.result;
    engine_building_builder_chunks(n243, H348);
    P348_0 = n243.result;
    list_add(n243.result, In47);
    n243.result[0] = In47;
    P348_1 = list_length<GeoChunk>(n243.result);
    P348_2 = n243.result[9];
    P348_3 = n243.result;
    engine_building_builder_release(H348);
    P348_4 = n243.result;
    n243.result = In92;
    engine_road_builder_make(n244);
    H380 = n244.result;
    engine_road_builder_chunks(n245, H380);
    P380_0 = n245.result;
    list_add(n245.result, In47);
    n245.result[0] = In47;
    P380_1 = list_length<GeoChunk>(n245.result);
    P380_2 = n245.result[9];
    P380_3 = n245.result;
    engine_road_builder_release(H380);
    P380_4 = n245.result;
    n245.result = In92;
  }

  void Free1() {
    R75 = engine_audio_bus_name(In5);
    engine_audio_parse_bus(n15, "", In5);
    R76_0 = n15.result;
    R76_1 = n15.out;
    R77 = engine_audio_rolloff_name(In6);
    engine_audio_parse_rolloff(n16, "", In6);
    R78_0 = n16.result;
    R78_1 = n16.out;
    R106 = engine_audio_synthesize(In9, 0);
    engine_audio_mix_into(n24, List<Float>{}, List<Float>{}, 0.0, 0, 0.0);
    R107_0 = std::move(n24.dst);
    engine_audio_make_seamless(n25, List<Float>{}, 0, 0.0);
    R108_0 = std::move(n25.samples);
    engine_audio_normalize(n26, List<Float>{}, 0.0);
    R109_0 = std::move(n26.samples);
    R110 = engine_audio_builtin_sounds();
    R111 = engine_audio_find_builtin("");
    engine_audio_render_builtin(n27, "", 0, List<Float>{}, 0);
    R112_0 = n27.result;
    R112_1 = n27.out;
    R112_2 = n27.channels;
    R118 = engine_last_error();
    engine_clear_error();
    R120 = engine_now_ms();
    R121 = engine_wall_ms();
    engine_jobs_submit(n31, 0, List<Int>{});
    R122_0 = n31.result;
    engine_jobs_take(n32, 0, List<Int>{});
    R123_0 = n32.result;
    R123_1 = n32.out;
    engine_jobs_cancel(0);
    engine_jobs_pump(0.0);
    R126 = engine_jobs_pending();
    engine_jobs_set_external(false);
    R128 = engine_jobs_external();
    engine_jobs_set_threads(false);
    R130 = engine_jobs_threads();
    R131 = engine_jobs_take_queued();
    engine_jobs_run(n33, 0, List<Int>{}, List<Int>{});
    R132_0 = std::move(n33.output);
    engine_jobs_done(0, List<Int>{});
    R134 = engine_game_shape_mesh(In10);
    R135 = engine_game_shape_bounds(In10);
    R136 = engine_game_fit_collider(In10);
    R137 = engine_game_shape_hull_points(In10);
    engine_game_build_city(n34, In11, In12);
    R138_0 = n34.result;
    R138_1 = n34.out;
    engine_game_city_street_point(n35, In11, In13);
    R139_0 = n35.result;
    R139_1 = n35.rng;
    R140 = engine_game_euler_to_quat(Vector{0.0, 0.0, 0.0});
    R141 = engine_game_quat_to_euler(In14);
    R142 = engine_game_transform_matrix(In15);
    R143 = engine_game_matrix_transform(In16);
    R144 = engine_game_shape_name(In17);
    engine_game_parse_shape(n36, "", In17);
    R145_0 = n36.result;
    R145_1 = n36.out;
    R146 = engine_game_motion_name(In18);
  }

  void Free2() {
    R147 = engine_game_collider_shape_name(In19);
    R148 = engine_game_joint_kind_name(In20);
    R149 = engine_game_control_mode_name(In21);
    R150 = engine_game_camera_mode_name(In22);
    R151 = engine_game_weapon_kind_name(In23);
    R152 = engine_game_weapon_preset(In23);
    R153 = engine_game_trigger_action_name(In24);
    R154 = engine_game_mover_mode_name(In25);
    R155 = engine_game_game_goal_name(In26);
    R156 = engine_game_motion_name_ok("");
    R157 = engine_game_default_player(In27);
    R158 = engine_game_channel_names(In28);
    R159 = engine_game_channel_bits(In28, "");
    R160 = engine_level_find_2(In27, 0);
    engine_level_add(n37, In27, In29);
    R161_0 = std::move(n37.result);
    R161_1 = n37.self;
    R162 = engine_level_children(In27, 0);
    R163 = engine_level_world_matrix(In27, 0);
    R164 = engine_level_body_owner(In27, 0);
    R165 = engine_level_is_ancestor(In27, 0, 0);
    R166 = engine_level_texture(In27, "");
    R167 = engine_level_sound(In27, "");
    R168 = engine_level_motion(In27, "");
    engine_level_motion_2(n38, In27, "");
    R169_0 = n38.result;
    R169_1 = n38.self;
    R170 = engine_game_to_json(In27);
    R171 = engine_game_to_text(In27, 0);
    R172 = engine_game_actor_to_json(In29);
    R173 = engine_game_world_to_json(In28);
    R174 = engine_game_sound_reference_ok(In27, "");
    R175 = engine_game_validate(In27);
    R176 = engine_game_hex_color(Vector{0.0, 0.0, 0.0});
    engine_game_parse_hex_color(n39, "", Vector{0.0, 0.0, 0.0});
    R177_0 = n39.result;
    R177_1 = n39.out;
    R178 = engine_game_srgb_to_linear(Vector{0.0, 0.0, 0.0});
    R179 = engine_game_logic_node_shapes_registered();
    R180 = engine_game_logic_node_types();
    R181 = engine_game_logic_nodes_registered();
    R190 = engine_game_plan_bodies(In27);
    R191 = engine_game_game_status_name(In30);
    R344 = engine_game_engine_sound_for(In45);
    engine_clean_ring(n106, List<Float>{});
    R359_0 = std::move(n106.xy);
    R360 = engine_ring_area(List<Float>{}, 0);
  }

  void Free3() {
    R362 = engine_earcut_deviation(List<Float>{}, 0, List<Int>{}, 0, List<Int>{}, 0);
    R363 = engine_mesh_data_as_input(In48, 0);
    engine_upsert(n108, 0, 0, In48, In49, List<Float>{}, List<Float>{});
    R364_0 = n108.result;
    engine_weld(n109, In48, 0);
    R365_0 = n109.result;
    R365_1 = n109.mesh;
    R366 = engine_content_hash(In48);
    R367 = engine_primitives_box(Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0});
    R368 = engine_primitives_box_on_ground(Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0});
    R369 = engine_primitives_sphere(0.0, 0, 0, Vector{0.0, 0.0, 0.0});
    R370 = engine_primitives_capsule(0.0, 0.0, 0, 0, Vector{0.0, 0.0, 0.0});
    R371 = engine_primitives_torus(0.0, 0.0, 0, 0, Vector{0.0, 0.0, 0.0});
    R372 = engine_primitives_cylinder(0.0, 0.0, 0, Vector{0.0, 0.0, 0.0});
    R373 = engine_primitives_cone(0.0, 0.0, 0, Vector{0.0, 0.0, 0.0});
    R374 = engine_primitives_plane(0.0, 0.0, Vector{0.0, 0.0, 0.0}, 0.0);
    engine_primitives_append(n110, In48, In48, In16);
    R375_0 = std::move(n110.dst);
    engine_build_terrain_tile(n114, 0.0, 0.0, 0.0, 0, In50, 0, In48);
    R386_0 = n114.result;
    R386_1 = n114.out;
    R386_2 = n114.min_height;
    R386_3 = n114.max_height;
    R393 = engine_bounding_sphere(List<Float>{}, 0);
    R394 = engine_mirror_state(List<Float>{}, 0);
    engine_apply_cull_mode(0);
    R396 = engine_instanced_bounds(In51, List<Float>{}, 0);
    engine_uniform_cache_set_mat4_array(0, List<Float>{}, 0);
    R513 = engine_build_standard_shader(List<Int>{});
    R514 = engine_net_client_state_name(In70);
    R544 = engine_net_connection_error_name(In72);
    R634 = engine_physics_surface_name(In74);
    engine_physics_parse_surface(n186, "", In74);
    R635_0 = n186.result;
    R635_1 = n186.out;
    R636 = engine_physics_surface_friction(In74, In74);
    R637 = engine_physics_frame_from_axis(Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, false);
    R638 = engine_physics_version();
    engine_physics_hull_mesh(n197, List<Vector>{}, In48, Vector{0.0, 0.0, 0.0});
    R728_0 = n197.result;
    R728_1 = n197.out;
    engine_screen_ray(n198, In3, 0.0, 0.0, 0.0, 0.0, In67);
    R732_0 = n198.result;
    R732_1 = n198.out;
    engine_pick_meshes(n199, List<Int>{}, In67, In68, In69);
    R733_0 = n199.result;
    R733_1 = n199.hit;
    R733_2 = n199.stats;
    engine_engine_make(n205, 0);
    R744_0 = n205.result;
    engine_light_pool_make(n209, 0, 0);
    R752_0 = n209.result;
    engine_game_audio_make(n223, 0, 0);
    R780_0 = n223.result;
    engine_game_world_make(n224, 0, 0);
    R782_0 = n224.result;
    engine_sound_bank_make(n230, 0);
    R794_0 = n230.result;
    engine_client_make(n232, 0, In90, Int64{0x0});
    R798_0 = n232.result;
    engine_connection_make(n233, In91, 0.0);
    R800_0 = n233.result;
    engine_host_transport_make(n234, In71);
    R802_0 = n234.result;
    engine_loopback_network_make(n235, 0);
    R804_0 = n235.result;
  }

  void Free4() {
    engine_server_make(n237, 0, In90, Int64{0x0});
    R808_0 = n237.result;
    engine_simulator_make(n238, 0);
    R810_0 = n238.result;
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n11;
  struct { RendererOptions result = RendererOptions{960, 540, false, ShadowOptions{512, 120.0, 0.0012, 9}, BloomOptions{true, false, true}, AoOptions{true, AoQuality::medium, true, 1.6, 2.2, true}, true}; } n12;
  struct { List<Int> result = List<Int>{}; } n13;
  struct { Bool result = false; } n14;
  struct { Bool result = false; audio_Bus out = audio_Bus::master; } n15;
  struct { Bool result = false; audio_Rolloff out = audio_Rolloff::inverse; } n16;
  struct { Bool result = false; } n17;
  struct { Int result = 0; } n18;
  struct { Int result = 0; } n19;
  struct { Bool result = false; } n20;
  struct { Int64 result = Int64{0x0}; } n21;
  struct { audio_BusSettings result = audio_BusSettings{1.0, false, 0.0, 0.0}; } n22;
  struct { List<Float> out = List<Float>{}; } n23;
  struct { List<Float> dst = List<Float>{}; } n24;
  struct { List<Float> samples = List<Float>{}; } n25;
  struct { List<Float> samples = List<Float>{}; } n26;
  struct { Bool result = false; List<Float> out = List<Float>{}; Int channels = 0; } n27;
  struct { Bool result = false; } n28;
  struct { Bool result = false; } n29;
  struct { List<Float> result = List<Float>{}; Int channels = 0; } n30;
  struct { Int result = 0; } n31;
  struct { Bool result = false; List<Int> out = List<Int>{}; } n32;
  struct { List<Int> output = List<Int>{}; } n33;
  struct { Bool result = false; game_CityGeometry out = game_CityGeometry{List<MeshData>{}, List<MeshData>{}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}, List<Float>{}, List<Float>{}, List<Float>{}, List<game_CityBox>{}, 0.0, 0}; } n34;
  struct { Vector result = Vector{0.0, 0.0, 0.0}; game_Rng rng = game_Rng{0}; } n35;
  struct { Bool result = false; game_Shape out = game_Shape::box; } n36;
  struct { game_Actor result = game_Actor{0, "", 0, "", false, false, game_Transform{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{1.0, 1.0, 1.0}}, List<Text>{}, List<game_MeshComponent>{}, List<game_LightComponent>{}, List<game_BodyComponent>{}, List<game_ColliderComponent>{}, List<game_JointComponent>{}, List<game_ControllerComponent>{}, List<game_PickupComponent>{}, List<game_TriggerComponent>{}, List<game_RotatorComponent>{}, List<game_MoverComponent>{}, List<game_CityComponent>{}, List<game_AudioComponent>{}, List<game_WeaponComponent>{}, List<game_HealthComponent>{}, List<logic_Graph>{}}; game_Level self = game_Level{"Untitled", game_WorldSettings{"World", Vector{0.0, 0.0, -9.80665}, 4, 60.0, 0.42, 0.0, 210.0, 0.0009, 1.0, 1.6, 80.0, true, Vector{0.19, 0.27, 0.15}, true, 0.7, 0.0, "", 30.0, 10.0, 3.0, -50.0, "", game_GameGoal::none, 0.0, 0.0, 0, false, 1.0, "", 0.6, "", 0.6, 0.25, 1.0}, List<game_Actor>{}, List<Int>{}, List<Int>{}, List<Int>{}, logic_Graph{List<logic_Node>{}, List<logic_Link>{}, List<Int>{}, List<logic_StateMachine>{}, List<logic_Function>{}, List<logic_EnumType>{}, List<logic_StructType>{}, List<logic_CommentBox>{}, 1, false, List<Text>{}}, game_EditorCamera{Vector{-24.0, -30.0, 18.0}, 51.0, -24.0}, 1}; } n37;
  struct { Int result = 0; game_Level self = game_Level{"Untitled", game_WorldSettings{"World", Vector{0.0, 0.0, -9.80665}, 4, 60.0, 0.42, 0.0, 210.0, 0.0009, 1.0, 1.6, 80.0, true, Vector{0.19, 0.27, 0.15}, true, 0.7, 0.0, "", 30.0, 10.0, 3.0, -50.0, "", game_GameGoal::none, 0.0, 0.0, 0, false, 1.0, "", 0.6, "", 0.6, 0.25, 1.0}, List<game_Actor>{}, List<Int>{}, List<Int>{}, List<Int>{}, logic_Graph{List<logic_Node>{}, List<logic_Link>{}, List<Int>{}, List<logic_StateMachine>{}, List<logic_Function>{}, List<logic_EnumType>{}, List<logic_StructType>{}, List<logic_CommentBox>{}, 1, false, List<Text>{}}, game_EditorCamera{Vector{-24.0, -30.0, 18.0}, 51.0, -24.0}, 1}; } n38;
  struct { Bool result = false; Vector out = Vector{0.0, 0.0, 0.0}; } n39;
  struct { Bool result = false; Mat4 out = Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; } n40;
  struct { Bool result = false; Aabb out = Aabb{Vector{1e+30, 1e+30, 1e+30}, Vector{-1e+30, -1e+30, -1e+30}}; } n41;
  struct { List<Int> out = List<Int>{}; } n42;
  struct { Bool result = false; } n43;
  struct { Float result = 0.0; } n44;
  struct { Bool result = false; } n45;
  struct { Bool result = false; } n46;
  struct { Bool result = false; } n47;
  struct { Bool result = false; Camera camera = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}}; } n48;
  struct { Int result = 0; } n49;
  struct { Bool result = false; } n50;
  struct { List<Int_Bool_pair> out = List<Int_Bool_pair>{}; } n51;
  struct { List<Int> out = List<Int>{}; } n52;
  struct { engine_game_world_hud_mut_place<game_Hud> result = {}; } n53;
  struct { Bool result = false; } n54;
  struct { Int result = 0; } n55;
  struct { Bool result = false; } n56;
  struct { List<Float> positions = List<Float>{}; List<Float> colors = List<Float>{}; } n57;
  struct { engine_game_world_events_place<List<Text>> result = {}; } n58;
  struct { engine_game_world_sound_cues_place<List<game_SoundCue>> result = {}; } n59;
  struct { engine_game_world_shots_place<List<game_ShotEvent>> result = {}; } n60;
  struct { Bool result = false; game_SoldierState out = game_SoldierState{100.0, 100.0, 0.0, 0, 0, 0, -1.0, 0.0, -1.0, 0, 0, false, false, false, 0.0}; } n61;
  struct { Bool result = false; } n62;
  struct { Int result = 0; } n63;
  struct { List<Int> props = List<Int>{}; List<game_GameWorld_SpawnChange> spawns = List<game_GameWorld_SpawnChange>{}; } n64;
  struct { List<Int> props = List<Int>{}; List<game_GameWorld_SpawnChange> spawns = List<game_GameWorld_SpawnChange>{}; } n65;
  struct { Bool result = false; } n66;
  struct { Bool result = false; } n67;
  struct { Bool result = false; } n68;
  struct { Bool result = false; } n69;
  struct { Int result = 0; } n70;
  struct { Bool result = false; } n71;
  struct { Bool result = false; } n72;
  struct { Bool result = false; } n73;
  struct { Bool result = false; } n74;
  struct { Bool result = false; } n75;
  struct { List<Int> out = List<Int>{}; } n76;
  struct { Bool result = false; Int actor = 0; Vector point = Vector{0.0, 0.0, 0.0}; Vector normal = Vector{0.0, 0.0, 0.0}; } n77;
  struct { Bool result = false; game_GameWorld_WalkerSensors out = game_GameWorld_WalkerSensors{Fixed<Float, 64>{}, Fixed<Int, 64>{}, -1.0, 0.0, 0.0, 0.0, Vector{0.0, 0.0, -1.0}, false, 0.0, -1.0, 0, Fixed<Vector, 64>{}, 0}; } n78;
  struct { Bool result = false; Camera out = Camera{Vector{0.0, -18.0, 8.0}, Vector{0.0, 0.0, 2.0}, Vector{0.0, 0.0, 1.0}, 1.0471975511965976, 0.05, 1800.0, false, 20.0, false, Mat4{Fixed<Float, 16>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}}; } n79;
  struct { Bool result = false; } n80;
  struct { Bool result = false; } n81;
  struct { Bool result = false; } n82;
  struct { Bool result = false; } n83;
  struct { Text result = ""; } n84;
  struct { Bool result = false; } n85;
  struct { Bool result = false; } n86;
  struct { Bool result = false; } n87;
  struct { Bool result = false; Float strain = 0.0; Float drive = 0.0; Float speed = 0.0; } n88;
  struct { Bool result = false; } n89;
  struct { Bool result = false; } n90;
  struct { Bool result = false; } n91;
  struct { Bool result = false; } n92;
  struct { Bool result = false; game_GameWorld_WalkerProbe out = game_GameWorld_WalkerProbe{Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Fixed<Bool, 2>{}, Fixed<Float, 2>{}, false, false, Fixed<Float, 14>{}, Fixed<Float, 14>{}, Fixed<Float, 14>{}}; } n93;
  struct { Bool result = false; } n94;
  struct { Bool result = false; } n95;
  struct { Bool result = false; } n96;
  struct { Bool result = false; } n97;
  struct { Bool result = false; Bool visible = false; Float bearing = 0.0; Float distance = 0.0; } n98;
  struct { Int result = 0; } n99;
  struct { Int result = 0; } n100;
  struct { Bool result = false; } n101;
  struct { Bool result = false; } n102;
  struct { Bool result = false; } n103;
  struct { Bool result = false; } n104;
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n105;
  struct { List<Float> xy = List<Float>{}; } n106;
  struct { List<Int> out = List<Int>{}; } n107;
  struct { Bool result = false; } n108;
  struct { WeldStats result = WeldStats{0, 0, 0, 0, false}; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n109;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n110;
  struct { Bool result = false; } n111;
  struct { Bool result = false; } n112;
  struct { engine_road_builder_chunks_place<List<GeoChunk>> result = {}; } n113;
  struct { Bool result = false; MeshData out = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; Float min_height = 0.0; Float max_height = 0.0; } n114;
  struct { Bool result = false; } n115;
  struct { Int result = 0; } n116;
  struct { ParticleStats result = ParticleStats{0, 0, 0, 0}; } n117;
  struct { Bool result = false; } n118;
  struct { PointLights result = PointLights{0, Fixed<Float, 64>{}, Fixed<Float, 64>{}}; } n119;
  struct { Bool result = false; } n120;
  struct { Int64 result = Int64{0x0}; } n121;
  struct { Int result = 0; } n122;
  struct { Bool result = false; } n123;
  struct { Bool result = false; } n124;
  struct { Bool result = false; } n125;
  struct { Bool result = false; } n126;
  struct { Bool result = false; } n127;
  struct { Bool result = false; } n128;
  struct { Int result = 0; } n129;
  struct { Bool result = false; } n130;
  struct { Bool result = false; Bool changed = false; } n131;
  struct { Bool result = false; } n132;
  struct { Bool result = false; } n133;
  struct { Bool result = false; } n134;
  struct { Bool result = false; } n135;
  struct { Bool result = false; } n136;
  struct { Int result = 0; } n137;
  struct { Int result = 0; } n138;
  struct { Int result = 0; Int framebuffer = 0; } n139;
  struct { Bool result = false; } n140;
  struct { Bool result = false; } n141;
  struct { Bool result = false; } n142;
  struct { Bool result = false; RenderStats stats = RenderStats{0, 0, 0, false, false, 0.0, 0, 0, 0, 0.0, -1.0, BloomState::off}; } n143;
  struct { Bool result = false; Ray ray = Ray{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, -1.0}, 0.05, 1800.0}; } n144;
  struct { Bool result = false; PickHit hit = PickHit{0, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}; PickStats stats = PickStats{0, 0}; } n145;
  struct { Bool result = false; PickHit hit = PickHit{0, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}; PickStats stats = PickStats{0, 0}; } n146;
  struct { Bool result = false; } n147;
  struct { Bool result = false; } n148;
  struct { List<net_Message> result = List<net_Message>{}; } n149;
  struct { Int result = 0; } n150;
  struct { Bool result = false; } n151;
  struct { List<net_Message> result = List<net_Message>{}; } n152;
  struct { Int result = 0; } n153;
  struct { List<Int_Bool_pair> result = List<Int_Bool_pair>{}; } n154;
  struct { Bool result = false; } n155;
  struct { List<net_Message> result = List<net_Message>{}; } n156;
  struct { Int result = 0; } n157;
  struct { Int result = 0; } n158;
  struct { Int result = 0; } n159;
  struct { Bool result = false; } n160;
  struct { List<Int> data = List<Int>{}; } n161;
  struct { Bool result = false; } n162;
  struct { Bool result = false; } n163;
  struct { Bool result = false; } n164;
  struct { Bool result = false; } n165;
  struct { Bool result = false; } n166;
  struct { Bool result = false; Int value = 0; } n167;
  struct { Bool result = false; Int value = 0; } n168;
  struct { Bool result = false; List<Int> data = List<Int>{}; } n169;
  struct { Bool result = false; } n170;
  struct { Bool result = false; } n171;
  struct { Bool result = false; } n172;
  struct { Bool result = false; } n173;
  struct { Bool result = false; } n174;
  struct { Bool result = false; } n175;
  struct { Bool result = false; } n176;
  struct { Bool result = false; } n177;
  struct { Int result = 0; net_Address from = net_Address{Int64{0x0}}; List<Int> data = List<Int>{}; } n178;
  struct { List<net_Simulator_Packet> out = List<net_Simulator_Packet>{}; } n179;
  struct { Int result = 0; } n180;
  struct { Int result = 0; } n181;
  struct { Bool result = false; } n182;
  struct { Int result = 0; net_Address from = net_Address{Int64{0x0}}; List<Int> data = List<Int>{}; } n183;
  struct { Bool result = false; net_Address to = net_Address{Int64{0x0}}; List<Int> data = List<Int>{}; } n184;
  struct { Int result = 0; } n185;
  struct { Bool result = false; physics_Surface out = physics_Surface::none; } n186;
  struct { Bool result = false; } n187;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n188;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n189;
  struct { physics_Joint result = physics_Joint{Int64{0x0}}; } n190;
  struct { List<Int64> out = List<Int64>{}; } n191;
  struct { List<physics_ContactPoint> out = List<physics_ContactPoint>{}; } n192;
  struct { List<physics_Body_Float_pair> out = List<physics_Body_Float_pair>{}; } n193;
  struct { List<physics_MoverPlane> out = List<physics_MoverPlane>{}; } n194;
  struct { Bool result = false; } n195;
  struct { Bool result = false; } n196;
  struct { Bool result = false; MeshData out = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n197;
  struct { Bool result = false; Ray out = Ray{Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, -1.0}, 0.05, 1800.0}; } n198;
  struct { Bool result = false; PickHit hit = PickHit{0, 0, 0, 0.0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}}; PickStats stats = PickStats{0, 0}; } n199;
  struct { Int result = 0; } n200;
  struct { Int result = 0; } n201;
  struct { Int result = 0; } n202;
  struct { Int result = 0; } n203;
  struct { Int result = 0; } n204;
  struct { Int result = 0; } n205;
  struct { Int result = 0; } n206;
  struct { Int result = 0; } n207;
  struct { Int result = 0; } n208;
  struct { Int result = 0; } n209;
  struct { Int result = 0; } n210;
  struct { Int result = 0; } n211;
  struct { Int result = 0; } n212;
  struct { Int result = 0; } n213;
  struct { Int result = 0; } n214;
  struct { Int result = 0; } n215;
  struct { Int result = 0; } n216;
  struct { Int result = 0; } n217;
  struct { Int result = 0; } n218;
  struct { Int result = 0; } n219;
  struct { Int result = 0; } n220;
  struct { Int result = 0; } n221;
  struct { Int result = 0; } n222;
  struct { Int result = 0; } n223;
  struct { Int result = 0; } n224;
  struct { Int result = 0; } n225;
  struct { Int result = 0; } n226;
  struct { Int result = 0; } n227;
  struct { Int result = 0; } n228;
  struct { Int result = 0; } n229;
  struct { Int result = 0; } n230;
  struct { Int result = 0; } n231;
  struct { Int result = 0; } n232;
  struct { Int result = 0; } n233;
  struct { Int result = 0; } n234;
  struct { Int result = 0; } n235;
  struct { Int result = 0; } n236;
  struct { Int result = 0; } n237;
  struct { Int result = 0; } n238;
  struct { Int result = 0; } n239;
  struct { Int result = 0; } n240;
  struct { Int result = 0; } n241;
  struct { Int result = 0; } n242;
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n243;
  struct { Int result = 0; } n244;
  struct { engine_road_builder_chunks_place<List<GeoChunk>> result = {}; } n245;
};
// How the engine reaches the Blueprint Engine by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
namespace kern::logic::detail {
template <> struct Held<::Engine::Vec4> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::Engine::Vec4 from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::Engine::Vec4& s) { return Value::structure({to_value(s.x), to_value(s.y), to_value(s.z), to_value(s.w)}); }
};
template <> struct Held<::Engine::Quat> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::Engine::Quat from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::Engine::Quat& s) { return Value::structure({to_value(s.x), to_value(s.y), to_value(s.z), to_value(s.w)}); }
};
template <> struct Held<::Engine::Mat4> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::List<kern::f64>>(*v.field(0)); }
  static ::Engine::Mat4 from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0))};
  }
  static Value to(const ::Engine::Mat4& s) { return Value::structure({to_value(s.m)}); }
};
template <> struct Held<::Engine::Sphere> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::f64>(*v.field(1)); }
  static ::Engine::Sphere from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::f64>(*v.field(1))};
  }
  static Value to(const ::Engine::Sphere& s) { return Value::structure({to_value(s.center), to_value(s.radius)}); }
};
template <> struct Held<::Engine::Aabb> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)); }
  static ::Engine::Aabb from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1))};
  }
  static Value to(const ::Engine::Aabb& s) { return Value::structure({to_value(s.the_min), to_value(s.the_max)}); }
};
template <> struct Held<::Engine::Material> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 14 && is_value<::Engine::Vec4>(*v.field(0)) && is_value<::Engine::AlphaMode>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::Vec3>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<bool>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<bool>(*v.field(9)) && is_value<bool>(*v.field(10)) && is_value<bool>(*v.field(11)) && is_value<bool>(*v.field(12)) && is_value<kern::List<kern::logic::Whole>>(*v.field(13)); }
  static ::Engine::Material from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::Vec4>(*v.field(0)), from_value<::Engine::AlphaMode>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::Vec3>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<bool>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<bool>(*v.field(9)), from_value<bool>(*v.field(10)), from_value<bool>(*v.field(11)), from_value<bool>(*v.field(12)), from_value<kern::List<kern::logic::Whole>>(*v.field(13))};
  }
  static Value to(const ::Engine::Material& s) { return Value::structure({to_value(s.color), to_value(s.alpha), to_value(s.alpha_cutoff), to_value(s.roughness), to_value(s.metalness), to_value(s.emissive), to_value(s.unlit), to_value(s.double_sided), to_value(s.cast_shadow), to_value(s.frustum_culled), to_value(s.viewmodel), to_value(s.linear_colors), to_value(s.lines), to_value(s.params)}); }
};
template <> struct Held<::Engine::MeshData> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::List<kern::f64>>(*v.field(0)) && is_value<kern::List<kern::f64>>(*v.field(1)) && is_value<kern::List<kern::f64>>(*v.field(2)) && is_value<kern::List<kern::f64>>(*v.field(3)) && is_value<kern::List<kern::f64>>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::List<kern::logic::Whole>>(*v.field(6)); }
  static ::Engine::MeshData from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0)), from_value<kern::List<kern::f64>>(*v.field(1)), from_value<kern::List<kern::f64>>(*v.field(2)), from_value<kern::List<kern::f64>>(*v.field(3)), from_value<kern::List<kern::f64>>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::List<kern::logic::Whole>>(*v.field(6))};
  }
  static Value to(const ::Engine::MeshData& s) { return Value::structure({to_value(s.positions), to_value(s.normals), to_value(s.colors), to_value(s.uvs), to_value(s.extra), to_value(s.extra_size), to_value(s.indices)}); }
};
template <> struct Held<::Engine::physics_Body> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::logic::Int64>(*v.field(0)); }
  static ::Engine::physics_Body from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0))};
  }
  static Value to(const ::Engine::physics_Body& s) { return Value::structure({to_value(s.id)}); }
};
template <> struct Held<::Engine::physics_Shape> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::logic::Int64>(*v.field(0)); }
  static ::Engine::physics_Shape from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0))};
  }
  static Value to(const ::Engine::physics_Shape& s) { return Value::structure({to_value(s.id)}); }
};
template <> struct Held<::Engine::physics_Joint> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::logic::Int64>(*v.field(0)); }
  static ::Engine::physics_Joint from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0))};
  }
  static Value to(const ::Engine::physics_Joint& s) { return Value::structure({to_value(s.id)}); }
};
template <> struct Held<::Engine::Rgb> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)); }
  static ::Engine::Rgb from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2))};
  }
  static Value to(const ::Engine::Rgb& s) { return Value::structure({to_value(s.r), to_value(s.g), to_value(s.b)}); }
};
template <> struct Held<::Engine::logic_Link> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::logic::Text>(*v.field(3)); }
  static ::Engine::logic_Link from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::logic::Text>(*v.field(3))};
  }
  static Value to(const ::Engine::logic_Link& s) { return Value::structure({to_value(s.from), to_value(s.out), to_value(s.to), to_value(s.in)}); }
};
template <> struct Held<::Engine::Text_Int_pair> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::i64>(*v.field(1)); }
  static ::Engine::Text_Int_pair from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::i64>(*v.field(1))};
  }
  static Value to(const ::Engine::Text_Int_pair& s) { return Value::structure({to_value(s.first), to_value(s.second)}); }
};
template <> struct Held<::Engine::Text_Text_pair> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)); }
  static ::Engine::Text_Text_pair from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1))};
  }
  static Value to(const ::Engine::Text_Text_pair& s) { return Value::structure({to_value(s.first), to_value(s.second)}); }
};
template <> struct Held<::Engine::logic_Node> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::i64>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::List<::Engine::Text_Int_pair>>(*v.field(4)) && is_value<kern::List<::Engine::Text_Text_pair>>(*v.field(5)) && is_value<kern::logic::Text>(*v.field(6)); }
  static ::Engine::logic_Node from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::List<::Engine::Text_Int_pair>>(*v.field(4)), from_value<kern::List<::Engine::Text_Text_pair>>(*v.field(5)), from_value<kern::logic::Text>(*v.field(6))};
  }
  static Value to(const ::Engine::logic_Node& s) { return Value::structure({to_value(s.id), to_value(s.type), to_value(s.x), to_value(s.y), to_value(s.values), to_value(s.params), to_value(s.comment)}); }
};
template <> struct Held<::Engine::logic_State> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::i64>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<bool>(*v.field(4)); }
  static ::Engine::logic_State from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<bool>(*v.field(4))};
  }
  static Value to(const ::Engine::logic_State& s) { return Value::structure({to_value(s.id), to_value(s.name), to_value(s.x), to_value(s.y), to_value(s.initial)}); }
};
template <> struct Held<::Engine::logic_Transition> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 8 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<::Engine::logic_TriggerKind>(*v.field(3)) && is_value<kern::logic::Text>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::logic::Text>(*v.field(6)) && is_value<kern::logic::Text>(*v.field(7)); }
  static ::Engine::logic_Transition from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<::Engine::logic_TriggerKind>(*v.field(3)), from_value<kern::logic::Text>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::logic::Text>(*v.field(6)), from_value<kern::logic::Text>(*v.field(7))};
  }
  static Value to(const ::Engine::logic_Transition& s) { return Value::structure({to_value(s.id), to_value(s.from), to_value(s.to), to_value(s.trigger), to_value(s.event), to_value(s.seconds), to_value(s.variable), to_value(s.label)}); }
};
template <> struct Held<::Engine::logic_StateMachine> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)) && is_value<kern::List<::Engine::logic_State>>(*v.field(2)) && is_value<kern::List<::Engine::logic_Transition>>(*v.field(3)); }
  static ::Engine::logic_StateMachine from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1)), from_value<kern::List<::Engine::logic_State>>(*v.field(2)), from_value<kern::List<::Engine::logic_Transition>>(*v.field(3))};
  }
  static Value to(const ::Engine::logic_StateMachine& s) { return Value::structure({to_value(s.id), to_value(s.name), to_value(s.states), to_value(s.transitions)}); }
};
template <> struct Held<::Engine::logic_Function> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 9 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::List<kern::logic::Whole>>(*v.field(1)) && is_value<kern::List<kern::logic::Whole>>(*v.field(2)) && is_value<kern::List<kern::logic::Whole>>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<kern::logic::Text>(*v.field(6)) && is_value<kern::logic::Text>(*v.field(7)) && is_value<kern::logic::Text>(*v.field(8)); }
  static ::Engine::logic_Function from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::List<kern::logic::Whole>>(*v.field(1)), from_value<kern::List<kern::logic::Whole>>(*v.field(2)), from_value<kern::List<kern::logic::Whole>>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<kern::logic::Text>(*v.field(6)), from_value<kern::logic::Text>(*v.field(7)), from_value<kern::logic::Text>(*v.field(8))};
  }
  static Value to(const ::Engine::logic_Function& s) { return Value::structure({to_value(s.name), to_value(s.inputs), to_value(s.outputs), to_value(s.locals), to_value(s.pure), to_value(s.external), to_value(s.remote), to_value(s.from), to_value(s.help)}); }
};
template <> struct Held<::Engine::logic_EnumType> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::List<kern::logic::Text>>(*v.field(1)) && is_value<kern::logic::Text>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<kern::logic::Text>(*v.field(4)); }
  static ::Engine::logic_EnumType from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::List<kern::logic::Text>>(*v.field(1)), from_value<kern::logic::Text>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<kern::logic::Text>(*v.field(4))};
  }
  static Value to(const ::Engine::logic_EnumType& s) { return Value::structure({to_value(s.name), to_value(s.values), to_value(s.help), to_value(s.project), to_value(s.from)}); }
};
template <> struct Held<::Engine::logic_StructType> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::List<kern::logic::Whole>>(*v.field(1)) && is_value<kern::logic::Text>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<kern::logic::Text>(*v.field(4)); }
  static ::Engine::logic_StructType from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::List<kern::logic::Whole>>(*v.field(1)), from_value<kern::logic::Text>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<kern::logic::Text>(*v.field(4))};
  }
  static Value to(const ::Engine::logic_StructType& s) { return Value::structure({to_value(s.name), to_value(s.fields), to_value(s.help), to_value(s.project), to_value(s.from)}); }
};
template <> struct Held<::Engine::logic_CommentBox> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::i64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::logic::Text>(*v.field(5)) && is_value<kern::i64>(*v.field(6)); }
  static ::Engine::logic_CommentBox from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::logic::Text>(*v.field(5)), from_value<kern::i64>(*v.field(6))};
  }
  static Value to(const ::Engine::logic_CommentBox& s) { return Value::structure({to_value(s.id), to_value(s.x), to_value(s.y), to_value(s.w), to_value(s.h), to_value(s.text), to_value(s.color)}); }
};
template <> struct Held<::Engine::logic_Graph> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 11 && is_value<kern::List<::Engine::logic_Node>>(*v.field(0)) && is_value<kern::List<::Engine::logic_Link>>(*v.field(1)) && is_value<kern::List<kern::logic::Whole>>(*v.field(2)) && is_value<kern::List<::Engine::logic_StateMachine>>(*v.field(3)) && is_value<kern::List<::Engine::logic_Function>>(*v.field(4)) && is_value<kern::List<::Engine::logic_EnumType>>(*v.field(5)) && is_value<kern::List<::Engine::logic_StructType>>(*v.field(6)) && is_value<kern::List<::Engine::logic_CommentBox>>(*v.field(7)) && is_value<kern::i64>(*v.field(8)) && is_value<bool>(*v.field(9)) && is_value<kern::List<kern::logic::Text>>(*v.field(10)); }
  static ::Engine::logic_Graph from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<::Engine::logic_Node>>(*v.field(0)), from_value<kern::List<::Engine::logic_Link>>(*v.field(1)), from_value<kern::List<kern::logic::Whole>>(*v.field(2)), from_value<kern::List<::Engine::logic_StateMachine>>(*v.field(3)), from_value<kern::List<::Engine::logic_Function>>(*v.field(4)), from_value<kern::List<::Engine::logic_EnumType>>(*v.field(5)), from_value<kern::List<::Engine::logic_StructType>>(*v.field(6)), from_value<kern::List<::Engine::logic_CommentBox>>(*v.field(7)), from_value<kern::i64>(*v.field(8)), from_value<bool>(*v.field(9)), from_value<kern::List<kern::logic::Text>>(*v.field(10))};
  }
  static Value to(const ::Engine::logic_Graph& s) { return Value::structure({to_value(s.nodes), to_value(s.links), to_value(s.variables), to_value(s.machines), to_value(s.functions), to_value(s.enums), to_value(s.structs), to_value(s.comments), to_value(s.next_id), to_value(s.library), to_value(s.uses)}); }
};
template <> struct Held<::Engine::game_BodyComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 16 && is_value<::Engine::game_Motion>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<bool>(*v.field(7)) && is_value<kern::List<bool>>(*v.field(8)) && is_value<kern::List<bool>>(*v.field(9)) && is_value<kern::Vec3>(*v.field(10)) && is_value<kern::Vec3>(*v.field(11)) && is_value<kern::logic::Text>(*v.field(12)) && is_value<kern::f64>(*v.field(13)) && is_value<kern::Vec3>(*v.field(14)) && is_value<kern::List<kern::f64>>(*v.field(15)); }
  static ::Engine::game_BodyComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::game_Motion>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<bool>(*v.field(7)), from_value<kern::List<bool>>(*v.field(8)), from_value<kern::List<bool>>(*v.field(9)), from_value<kern::Vec3>(*v.field(10)), from_value<kern::Vec3>(*v.field(11)), from_value<kern::logic::Text>(*v.field(12)), from_value<kern::f64>(*v.field(13)), from_value<kern::Vec3>(*v.field(14)), from_value<kern::List<kern::f64>>(*v.field(15))};
  }
  static Value to(const ::Engine::game_BodyComponent& s) { return Value::structure({to_value(s.motion), to_value(s.mass), to_value(s.linear_damping), to_value(s.angular_damping), to_value(s.gravity_scale), to_value(s.bullet), to_value(s.start_awake), to_value(s.allow_sleep), to_value(s.lock_linear), to_value(s.lock_angular), to_value(s.linear_velocity), to_value(s.angular_velocity), to_value(s.impact_sound), to_value(s.impact_threshold), to_value(s.center_of_mass), to_value(s.inertia)}); }
};
template <> struct Held<::Engine::game_Rng> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::i64>(*v.field(0)); }
  static ::Engine::game_Rng from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0))};
  }
  static Value to(const ::Engine::game_Rng& s) { return Value::structure({to_value(s.s)}); }
};
template <> struct Held<::Engine::net_Address> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::logic::Int64>(*v.field(0)); }
  static ::Engine::net_Address from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0))};
  }
  static Value to(const ::Engine::net_Address& s) { return Value::structure({to_value(s.id)}); }
};
template <> struct Held<::Engine::Touch> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)); }
  static ::Engine::Touch from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3))};
  }
  static Value to(const ::Engine::Touch& s) { return Value::structure({to_value(s.id), to_value(s.start), to_value(s.position), to_value(s.previous)}); }
};
template <> struct Held<::Engine::RenderStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 12 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::i64>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<kern::i64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<::Engine::BloomState>(*v.field(11)); }
  static ::Engine::RenderStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::i64>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<kern::i64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<::Engine::BloomState>(*v.field(11))};
  }
  static Value to(const ::Engine::RenderStats& s) { return Value::structure({to_value(s.draw_calls), to_value(s.shadow_draw_calls), to_value(s.shadow_resolution), to_value(s.ao), to_value(s.shadow_reused), to_value(s.triangles), to_value(s.instances), to_value(s.culled_meshes), to_value(s.visible_meshes), to_value(s.cpu_ms), to_value(s.gpu_ms), to_value(s.bloom)}); }
};
template <> struct Held<::Engine::Camera> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 10 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<::Engine::Mat4>(*v.field(9)); }
  static ::Engine::Camera from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<::Engine::Mat4>(*v.field(9))};
  }
  static Value to(const ::Engine::Camera& s) { return Value::structure({to_value(s.eye), to_value(s.target), to_value(s.up), to_value(s.fov), to_value(s.near), to_value(s.far), to_value(s.orthographic), to_value(s.ortho_height), to_value(s.has_projection), to_value(s.projection)}); }
};
template <> struct Held<::Engine::ShadowOptions> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)); }
  static ::Engine::ShadowOptions from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::i64>(*v.field(3))};
  }
  static Value to(const ::Engine::ShadowOptions& s) { return Value::structure({to_value(s.resolution), to_value(s.extent), to_value(s.bias), to_value(s.filter)}); }
};
template <> struct Held<::Engine::BloomOptions> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<bool>(*v.field(0)) && is_value<bool>(*v.field(1)) && is_value<bool>(*v.field(2)); }
  static ::Engine::BloomOptions from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<bool>(*v.field(0)), from_value<bool>(*v.field(1)), from_value<bool>(*v.field(2))};
  }
  static Value to(const ::Engine::BloomOptions& s) { return Value::structure({to_value(s.enabled), to_value(s.forced), to_value(s.grade)}); }
};
template <> struct Held<::Engine::AoOptions> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 6 && is_value<bool>(*v.field(0)) && is_value<::Engine::AoQuality>(*v.field(1)) && is_value<bool>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<bool>(*v.field(5)); }
  static ::Engine::AoOptions from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<bool>(*v.field(0)), from_value<::Engine::AoQuality>(*v.field(1)), from_value<bool>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<bool>(*v.field(5))};
  }
  static Value to(const ::Engine::AoOptions& s) { return Value::structure({to_value(s.enabled), to_value(s.quality), to_value(s.half_resolution), to_value(s.radius), to_value(s.power), to_value(s.adaptive)}); }
};
template <> struct Held<::Engine::RendererOptions> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<bool>(*v.field(2)) && is_value<::Engine::ShadowOptions>(*v.field(3)) && is_value<::Engine::BloomOptions>(*v.field(4)) && is_value<::Engine::AoOptions>(*v.field(5)) && is_value<bool>(*v.field(6)); }
  static ::Engine::RendererOptions from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<bool>(*v.field(2)), from_value<::Engine::ShadowOptions>(*v.field(3)), from_value<::Engine::BloomOptions>(*v.field(4)), from_value<::Engine::AoOptions>(*v.field(5)), from_value<bool>(*v.field(6))};
  }
  static Value to(const ::Engine::RendererOptions& s) { return Value::structure({to_value(s.width), to_value(s.height), to_value(s.shadows), to_value(s.shadow), to_value(s.bloom), to_value(s.ao), to_value(s.grade)}); }
};
template <> struct Held<::Engine::Frame> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::logic::Int64>(*v.field(2)); }
  static ::Engine::Frame from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::logic::Int64>(*v.field(2))};
  }
  static Value to(const ::Engine::Frame& s) { return Value::structure({to_value(s.time), to_value(s.dt), to_value(s.index)}); }
};
template <> struct Held<::Engine::audio_ClipInfo> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 6 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<bool>(*v.field(5)); }
  static ::Engine::audio_ClipInfo from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<bool>(*v.field(5))};
  }
  static Value to(const ::Engine::audio_ClipInfo& s) { return Value::structure({to_value(s.name), to_value(s.frames), to_value(s.channels), to_value(s.duration), to_value(s.peak), to_value(s.loopable)}); }
};
template <> struct Held<::Engine::Float_row> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::List<kern::f64>>(*v.field(0)); }
  static ::Engine::Float_row from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0))};
  }
  static Value to(const ::Engine::Float_row& s) { return Value::structure({to_value(s.items)}); }
};
template <> struct Held<::Engine::audio_PlayParams> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 16 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<::Engine::audio_Bus>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<kern::Vec3>(*v.field(9)) && is_value<kern::Vec3>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)) && is_value<::Engine::audio_Rolloff>(*v.field(13)) && is_value<kern::f64>(*v.field(14)) && is_value<kern::f64>(*v.field(15)); }
  static ::Engine::audio_PlayParams from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<::Engine::audio_Bus>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<kern::Vec3>(*v.field(9)), from_value<kern::Vec3>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12)), from_value<::Engine::audio_Rolloff>(*v.field(13)), from_value<kern::f64>(*v.field(14)), from_value<kern::f64>(*v.field(15))};
  }
  static Value to(const ::Engine::audio_PlayParams& s) { return Value::structure({to_value(s.volume), to_value(s.pitch), to_value(s.pan), to_value(s.the_loop), to_value(s.bus), to_value(s.fade_in), to_value(s.start), to_value(s.priority), to_value(s.spatial), to_value(s.position), to_value(s.velocity), to_value(s.min_distance), to_value(s.max_distance), to_value(s.rolloff), to_value(s.spread), to_value(s.doppler)}); }
};
template <> struct Held<::Engine::audio_Listener> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)); }
  static ::Engine::audio_Listener from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3))};
  }
  static Value to(const ::Engine::audio_Listener& s) { return Value::structure({to_value(s.position), to_value(s.forward), to_value(s.up), to_value(s.velocity)}); }
};
template <> struct Held<::Engine::audio_BusSettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::f64>(*v.field(0)) && is_value<bool>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::Engine::audio_BusSettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<bool>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::Engine::audio_BusSettings& s) { return Value::structure({to_value(s.volume), to_value(s.muted), to_value(s.lowpass_hz), to_value(s.reverb_send)}); }
};
template <> struct Held<::Engine::audio_Meter> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::List<kern::f64>>(*v.field(0)) && is_value<kern::List<kern::f64>>(*v.field(1)); }
  static ::Engine::audio_Meter from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0)), from_value<kern::List<kern::f64>>(*v.field(1))};
  }
  static Value to(const ::Engine::audio_Meter& s) { return Value::structure({to_value(s.peak), to_value(s.rms)}); }
};
template <> struct Held<::Engine::audio_Stats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 10 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::logic::Int64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<::Engine::audio_Meter>(*v.field(7)) && is_value<kern::List<::Engine::audio_Meter>>(*v.field(8)) && is_value<kern::List<kern::f64>>(*v.field(9)); }
  static ::Engine::audio_Stats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::logic::Int64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<::Engine::audio_Meter>(*v.field(7)), from_value<kern::List<::Engine::audio_Meter>>(*v.field(8)), from_value<kern::List<kern::f64>>(*v.field(9))};
  }
  static Value to(const ::Engine::audio_Stats& s) { return Value::structure({to_value(s.voices), to_value(s.audible), to_value(s.virtual_voices), to_value(s.stolen), to_value(s.clips), to_value(s.frames_rendered), to_value(s.limiter_reduction_db), to_value(s.master), to_value(s.buses), to_value(s.peak_hold)}); }
};
template <> struct Held<::Engine::audio_SynthPatch> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 27 && is_value<::Engine::audio_Wave>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)) && is_value<kern::f64>(*v.field(13)) && is_value<kern::f64>(*v.field(14)) && is_value<kern::f64>(*v.field(15)) && is_value<kern::f64>(*v.field(16)) && is_value<kern::f64>(*v.field(17)) && is_value<kern::f64>(*v.field(18)) && is_value<kern::f64>(*v.field(19)) && is_value<kern::f64>(*v.field(20)) && is_value<kern::f64>(*v.field(21)) && is_value<kern::f64>(*v.field(22)) && is_value<kern::f64>(*v.field(23)) && is_value<kern::f64>(*v.field(24)) && is_value<kern::f64>(*v.field(25)) && is_value<kern::i64>(*v.field(26)); }
  static ::Engine::audio_SynthPatch from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::audio_Wave>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12)), from_value<kern::f64>(*v.field(13)), from_value<kern::f64>(*v.field(14)), from_value<kern::f64>(*v.field(15)), from_value<kern::f64>(*v.field(16)), from_value<kern::f64>(*v.field(17)), from_value<kern::f64>(*v.field(18)), from_value<kern::f64>(*v.field(19)), from_value<kern::f64>(*v.field(20)), from_value<kern::f64>(*v.field(21)), from_value<kern::f64>(*v.field(22)), from_value<kern::f64>(*v.field(23)), from_value<kern::f64>(*v.field(24)), from_value<kern::f64>(*v.field(25)), from_value<kern::i64>(*v.field(26))};
  }
  static Value to(const ::Engine::audio_SynthPatch& s) { return Value::structure({to_value(s.wave), to_value(s.volume), to_value(s.attack), to_value(s.sustain), to_value(s.punch), to_value(s.release), to_value(s.frequency), to_value(s.slide), to_value(s.slide_accel), to_value(s.min_frequency), to_value(s.vibrato_depth), to_value(s.vibrato_rate), to_value(s.arp_ratio), to_value(s.arp_time), to_value(s.duty), to_value(s.duty_sweep), to_value(s.fm_ratio), to_value(s.fm_depth), to_value(s.lowpass), to_value(s.lowpass_sweep), to_value(s.resonance), to_value(s.highpass), to_value(s.tremolo_depth), to_value(s.tremolo_rate), to_value(s.repeat), to_value(s.bitcrush), to_value(s.seed)}); }
};
template <> struct Held<::Engine::audio_BuiltinSound> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)) && is_value<kern::logic::Text>(*v.field(2)) && is_value<bool>(*v.field(3)); }
  static ::Engine::audio_BuiltinSound from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1)), from_value<kern::logic::Text>(*v.field(2)), from_value<bool>(*v.field(3))};
  }
  static Value to(const ::Engine::audio_BuiltinSound& s) { return Value::structure({to_value(s.name), to_value(s.label), to_value(s.category), to_value(s.the_loop)}); }
};
template <> struct Held<::Engine::game_MaterialDesc> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 11 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<bool>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<kern::logic::Text>(*v.field(9)) && is_value<kern::f64>(*v.field(10)); }
  static ::Engine::game_MaterialDesc from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<bool>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<kern::logic::Text>(*v.field(9)), from_value<kern::f64>(*v.field(10))};
  }
  static Value to(const ::Engine::game_MaterialDesc& s) { return Value::structure({to_value(s.color), to_value(s.roughness), to_value(s.metalness), to_value(s.emissive), to_value(s.emissive_strength), to_value(s.opacity), to_value(s.unlit), to_value(s.cast_shadow), to_value(s.double_sided), to_value(s.texture), to_value(s.tiling)}); }
};
template <> struct Held<::Engine::game_MeshComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 12 && is_value<::Engine::game_Shape>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::logic::Text>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::logic::Text>(*v.field(10)) && is_value<::Engine::game_MaterialDesc>(*v.field(11)); }
  static ::Engine::game_MeshComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::game_Shape>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::logic::Text>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::logic::Text>(*v.field(10)), from_value<::Engine::game_MaterialDesc>(*v.field(11))};
  }
  static Value to(const ::Engine::game_MeshComponent& s) { return Value::structure({to_value(s.shape), to_value(s.size), to_value(s.radius), to_value(s.height), to_value(s.segments), to_value(s.asset), to_value(s.collision), to_value(s.friction), to_value(s.restitution), to_value(s.rolling_resistance), to_value(s.surface), to_value(s.material)}); }
};
template <> struct Held<::Engine::game_ColliderComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 17 && is_value<::Engine::game_ColliderShape>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::List<kern::Vec3>>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::logic::Text>(*v.field(11)) && is_value<bool>(*v.field(12)) && is_value<kern::i64>(*v.field(13)) && is_value<kern::i64>(*v.field(14)) && is_value<kern::i64>(*v.field(15)) && is_value<kern::logic::Text>(*v.field(16)); }
  static ::Engine::game_ColliderComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::game_ColliderShape>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::List<kern::Vec3>>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::logic::Text>(*v.field(11)), from_value<bool>(*v.field(12)), from_value<kern::i64>(*v.field(13)), from_value<kern::i64>(*v.field(14)), from_value<kern::i64>(*v.field(15)), from_value<kern::logic::Text>(*v.field(16))};
  }
  static Value to(const ::Engine::game_ColliderComponent& s) { return Value::structure({to_value(s.shape), to_value(s.center), to_value(s.rotation), to_value(s.size), to_value(s.radius), to_value(s.length), to_value(s.points), to_value(s.density), to_value(s.friction), to_value(s.restitution), to_value(s.rolling_resistance), to_value(s.surface), to_value(s.sensor), to_value(s.category), to_value(s.mask), to_value(s.group), to_value(s.impact_sound)}); }
};
template <> struct Held<::Engine::game_CityComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 8 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<kern::i64>(*v.field(6)) && is_value<bool>(*v.field(7)); }
  static ::Engine::game_CityComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<kern::i64>(*v.field(6)), from_value<bool>(*v.field(7))};
  }
  static Value to(const ::Engine::game_CityComponent& s) { return Value::structure({to_value(s.seed), to_value(s.blocks), to_value(s.pitch), to_value(s.street), to_value(s.max_height), to_value(s.trees), to_value(s.traffic), to_value(s.night_windows)}); }
};
template <> struct Held<::Engine::game_CityBox> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)); }
  static ::Engine::game_CityBox from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1))};
  }
  static Value to(const ::Engine::game_CityBox& s) { return Value::structure({to_value(s.center), to_value(s.size)}); }
};
template <> struct Held<::Engine::game_CityGeometry> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 11 && is_value<kern::List<::Engine::MeshData>>(*v.field(0)) && is_value<kern::List<::Engine::MeshData>>(*v.field(1)) && is_value<::Engine::MeshData>(*v.field(2)) && is_value<::Engine::MeshData>(*v.field(3)) && is_value<::Engine::MeshData>(*v.field(4)) && is_value<kern::List<kern::f64>>(*v.field(5)) && is_value<kern::List<kern::f64>>(*v.field(6)) && is_value<kern::List<kern::f64>>(*v.field(7)) && is_value<kern::List<::Engine::game_CityBox>>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::i64>(*v.field(10)); }
  static ::Engine::game_CityGeometry from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<::Engine::MeshData>>(*v.field(0)), from_value<kern::List<::Engine::MeshData>>(*v.field(1)), from_value<::Engine::MeshData>(*v.field(2)), from_value<::Engine::MeshData>(*v.field(3)), from_value<::Engine::MeshData>(*v.field(4)), from_value<kern::List<kern::f64>>(*v.field(5)), from_value<kern::List<kern::f64>>(*v.field(6)), from_value<kern::List<kern::f64>>(*v.field(7)), from_value<kern::List<::Engine::game_CityBox>>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::i64>(*v.field(10))};
  }
  static Value to(const ::Engine::game_CityGeometry& s) { return Value::structure({to_value(s.buildings), to_value(s.roads), to_value(s.plates), to_value(s.trunk), to_value(s.crown), to_value(s.trunk_transforms), to_value(s.crown_transforms), to_value(s.crown_tints), to_value(s.colliders), to_value(s.half), to_value(s.building_count)}); }
};
template <> struct Held<::Engine::game_Transform> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)); }
  static ::Engine::game_Transform from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2))};
  }
  static Value to(const ::Engine::game_Transform& s) { return Value::structure({to_value(s.position), to_value(s.rotation), to_value(s.scale)}); }
};
template <> struct Held<::Engine::game_WeaponComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 17 && is_value<::Engine::game_WeaponKind>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)) && is_value<kern::i64>(*v.field(13)) && is_value<kern::f64>(*v.field(14)) && is_value<kern::logic::Text>(*v.field(15)) && is_value<kern::logic::Text>(*v.field(16)); }
  static ::Engine::game_WeaponComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::game_WeaponKind>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12)), from_value<kern::i64>(*v.field(13)), from_value<kern::f64>(*v.field(14)), from_value<kern::logic::Text>(*v.field(15)), from_value<kern::logic::Text>(*v.field(16))};
  }
  static Value to(const ::Engine::game_WeaponComponent& s) { return Value::structure({to_value(s.kind), to_value(s.damage), to_value(s.fire_rate), to_value(s.automatic), to_value(s.magazine), to_value(s.reserve), to_value(s.reload_time), to_value(s.spread), to_value(s.aim_spread), to_value(s.recoil), to_value(s.range), to_value(s.headshot), to_value(s.impulse), to_value(s.pellets), to_value(s.aim_fov), to_value(s.fire_sound), to_value(s.reload_sound)}); }
};
template <> struct Held<::Engine::game_WorldSettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 34 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<bool>(*v.field(11)) && is_value<kern::Vec3>(*v.field(12)) && is_value<bool>(*v.field(13)) && is_value<kern::f64>(*v.field(14)) && is_value<kern::f64>(*v.field(15)) && is_value<kern::logic::Text>(*v.field(16)) && is_value<kern::f64>(*v.field(17)) && is_value<kern::f64>(*v.field(18)) && is_value<kern::f64>(*v.field(19)) && is_value<kern::f64>(*v.field(20)) && is_value<kern::logic::Text>(*v.field(21)) && is_value<::Engine::game_GameGoal>(*v.field(22)) && is_value<kern::f64>(*v.field(23)) && is_value<kern::f64>(*v.field(24)) && is_value<kern::i64>(*v.field(25)) && is_value<bool>(*v.field(26)) && is_value<kern::f64>(*v.field(27)) && is_value<kern::logic::Text>(*v.field(28)) && is_value<kern::f64>(*v.field(29)) && is_value<kern::logic::Text>(*v.field(30)) && is_value<kern::f64>(*v.field(31)) && is_value<kern::f64>(*v.field(32)) && is_value<kern::f64>(*v.field(33)); }
  static ::Engine::game_WorldSettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<bool>(*v.field(11)), from_value<kern::Vec3>(*v.field(12)), from_value<bool>(*v.field(13)), from_value<kern::f64>(*v.field(14)), from_value<kern::f64>(*v.field(15)), from_value<kern::logic::Text>(*v.field(16)), from_value<kern::f64>(*v.field(17)), from_value<kern::f64>(*v.field(18)), from_value<kern::f64>(*v.field(19)), from_value<kern::f64>(*v.field(20)), from_value<kern::logic::Text>(*v.field(21)), from_value<::Engine::game_GameGoal>(*v.field(22)), from_value<kern::f64>(*v.field(23)), from_value<kern::f64>(*v.field(24)), from_value<kern::i64>(*v.field(25)), from_value<bool>(*v.field(26)), from_value<kern::f64>(*v.field(27)), from_value<kern::logic::Text>(*v.field(28)), from_value<kern::f64>(*v.field(29)), from_value<kern::logic::Text>(*v.field(30)), from_value<kern::f64>(*v.field(31)), from_value<kern::f64>(*v.field(32)), from_value<kern::f64>(*v.field(33))};
  }
  static Value to(const ::Engine::game_WorldSettings& s) { return Value::structure({to_value(s.channels), to_value(s.gravity), to_value(s.substeps), to_value(s.step_hz), to_value(s.time_of_day), to_value(s.day_length), to_value(s.sun_azimuth), to_value(s.fog_density), to_value(s.exposure), to_value(s.ao_radius), to_value(s.shadow_distance), to_value(s.clouds), to_value(s.ground_color), to_value(s.ground), to_value(s.ground_friction), to_value(s.ground_restitution), to_value(s.ground_surface), to_value(s.contact_hertz), to_value(s.contact_damping), to_value(s.contact_speed), to_value(s.kill_z), to_value(s.title), to_value(s.goal), to_value(s.time_limit), to_value(s.score_target), to_value(s.player), to_value(s.observer), to_value(s.master_volume), to_value(s.music), to_value(s.music_volume), to_value(s.ambience), to_value(s.ambience_volume), to_value(s.reverb), to_value(s.doppler)}); }
};
template <> struct Held<::Engine::game_EditorCamera> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)); }
  static ::Engine::game_EditorCamera from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2))};
  }
  static Value to(const ::Engine::game_EditorCamera& s) { return Value::structure({to_value(s.position), to_value(s.yaw), to_value(s.pitch)}); }
};
template <> struct Held<::Engine::game_LightComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)); }
  static ::Engine::game_LightComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2))};
  }
  static Value to(const ::Engine::game_LightComponent& s) { return Value::structure({to_value(s.color), to_value(s.intensity), to_value(s.range)}); }
};
template <> struct Held<::Engine::game_JointComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 24 && is_value<::Engine::game_JointKind>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<bool>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<bool>(*v.field(12)) && is_value<kern::f64>(*v.field(13)) && is_value<kern::f64>(*v.field(14)) && is_value<kern::f64>(*v.field(15)) && is_value<kern::f64>(*v.field(16)) && is_value<kern::f64>(*v.field(17)) && is_value<bool>(*v.field(18)) && is_value<kern::f64>(*v.field(19)) && is_value<kern::f64>(*v.field(20)) && is_value<kern::f64>(*v.field(21)) && is_value<kern::f64>(*v.field(22)) && is_value<kern::logic::Text>(*v.field(23)); }
  static ::Engine::game_JointComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::game_JointKind>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<bool>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<bool>(*v.field(12)), from_value<kern::f64>(*v.field(13)), from_value<kern::f64>(*v.field(14)), from_value<kern::f64>(*v.field(15)), from_value<kern::f64>(*v.field(16)), from_value<kern::f64>(*v.field(17)), from_value<bool>(*v.field(18)), from_value<kern::f64>(*v.field(19)), from_value<kern::f64>(*v.field(20)), from_value<kern::f64>(*v.field(21)), from_value<kern::f64>(*v.field(22)), from_value<kern::logic::Text>(*v.field(23))};
  }
  static Value to(const ::Engine::game_JointComponent& s) { return Value::structure({to_value(s.kind), to_value(s.target), to_value(s.anchor), to_value(s.axis), to_value(s.collide_connected), to_value(s.enable_limit), to_value(s.lower), to_value(s.upper), to_value(s.reference), to_value(s.enable_motor), to_value(s.motor_speed), to_value(s.max_motor_force), to_value(s.enable_spring), to_value(s.spring_hertz), to_value(s.spring_damping), to_value(s.length), to_value(s.break_force), to_value(s.break_torque), to_value(s.servo), to_value(s.servo_target), to_value(s.servo_stiffness), to_value(s.servo_damping), to_value(s.servo_friction), to_value(s.break_sound)}); }
};
template <> struct Held<::Engine::game_ControllerComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 25 && is_value<::Engine::game_ControlMode>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<::Engine::game_CameraMode>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::logic::Text>(*v.field(12)) && is_value<kern::logic::Text>(*v.field(13)) && is_value<kern::logic::Text>(*v.field(14)) && is_value<kern::f64>(*v.field(15)) && is_value<kern::f64>(*v.field(16)) && is_value<kern::f64>(*v.field(17)) && is_value<bool>(*v.field(18)) && is_value<bool>(*v.field(19)) && is_value<kern::logic::Text>(*v.field(20)) && is_value<bool>(*v.field(21)) && is_value<kern::logic::Text>(*v.field(22)) && is_value<bool>(*v.field(23)) && is_value<kern::f64>(*v.field(24)); }
  static ::Engine::game_ControllerComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::game_ControlMode>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<::Engine::game_CameraMode>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::logic::Text>(*v.field(12)), from_value<kern::logic::Text>(*v.field(13)), from_value<kern::logic::Text>(*v.field(14)), from_value<kern::f64>(*v.field(15)), from_value<kern::f64>(*v.field(16)), from_value<kern::f64>(*v.field(17)), from_value<bool>(*v.field(18)), from_value<bool>(*v.field(19)), from_value<kern::logic::Text>(*v.field(20)), from_value<bool>(*v.field(21)), from_value<kern::logic::Text>(*v.field(22)), from_value<bool>(*v.field(23)), from_value<kern::f64>(*v.field(24))};
  }
  static Value to(const ::Engine::game_ControllerComponent& s) { return Value::structure({to_value(s.mode), to_value(s.max_speed), to_value(s.acceleration), to_value(s.turn_rate), to_value(s.jump_speed), to_value(s.boost), to_value(s.boost_speed), to_value(s.hover_height), to_value(s.camera), to_value(s.camera_distance), to_value(s.camera_height), to_value(s.camera_fov), to_value(s.engine_sound), to_value(s.jump_sound), to_value(s.boost_sound), to_value(s.crouch_speed), to_value(s.eye_height), to_value(s.look_sensitivity), to_value(s.head_bob), to_value(s.autonomous), to_value(s.gait), to_value(s.rollers), to_value(s.actions), to_value(s.Kern_extras), to_value(s.servo_delay)}); }
};
template <> struct Held<::Engine::game_PickupComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 6 && is_value<kern::f64>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::logic::Text>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)); }
  static ::Engine::game_PickupComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::logic::Text>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::i64>(*v.field(5))};
  }
  static Value to(const ::Engine::game_PickupComponent& s) { return Value::structure({to_value(s.points), to_value(s.burst_color), to_value(s.respawn), to_value(s.sound), to_value(s.heal), to_value(s.ammo)}); }
};
template <> struct Held<::Engine::game_TriggerComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 6 && is_value<::Engine::game_TriggerAction>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)) && is_value<bool>(*v.field(2)) && is_value<kern::logic::Text>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)); }
  static ::Engine::game_TriggerComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::game_TriggerAction>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1)), from_value<bool>(*v.field(2)), from_value<kern::logic::Text>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::i64>(*v.field(5))};
  }
  static Value to(const ::Engine::game_TriggerComponent& s) { return Value::structure({to_value(s.action), to_value(s.message), to_value(s.once), to_value(s.sound), to_value(s.strength), to_value(s.target)}); }
};
template <> struct Held<::Engine::game_RotatorComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 1 && is_value<kern::Vec3>(*v.field(0)); }
  static ::Engine::game_RotatorComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0))};
  }
  static Value to(const ::Engine::game_RotatorComponent& s) { return Value::structure({to_value(s.rate)}); }
};
template <> struct Held<::Engine::game_MoverComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<::Engine::game_MoverMode>(*v.field(3)); }
  static ::Engine::game_MoverComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<::Engine::game_MoverMode>(*v.field(3))};
  }
  static Value to(const ::Engine::game_MoverComponent& s) { return Value::structure({to_value(s.offset), to_value(s.period), to_value(s.phase), to_value(s.mode)}); }
};
template <> struct Held<::Engine::game_AudioComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 13 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<::Engine::audio_Rolloff>(*v.field(8)) && is_value<::Engine::audio_Bus>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)); }
  static ::Engine::game_AudioComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<::Engine::audio_Rolloff>(*v.field(8)), from_value<::Engine::audio_Bus>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12))};
  }
  static Value to(const ::Engine::game_AudioComponent& s) { return Value::structure({to_value(s.sound), to_value(s.volume), to_value(s.pitch), to_value(s.the_loop), to_value(s.autoplay), to_value(s.spatial), to_value(s.min_distance), to_value(s.max_distance), to_value(s.rolloff), to_value(s.bus), to_value(s.doppler), to_value(s.pitch_by_speed), to_value(s.delay)}); }
};
template <> struct Held<::Engine::game_HealthComponent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 11 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::logic::Text>(*v.field(7)) && is_value<kern::logic::Text>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)); }
  static ::Engine::game_HealthComponent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::logic::Text>(*v.field(7)), from_value<kern::logic::Text>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10))};
  }
  static Value to(const ::Engine::game_HealthComponent& s) { return Value::structure({to_value(s.max_health), to_value(s.armor), to_value(s.regen_delay), to_value(s.regen_rate), to_value(s.respawn), to_value(s.respawn_delay), to_value(s.points), to_value(s.hit_sound), to_value(s.death_sound), to_value(s.explode_radius), to_value(s.explode_damage)}); }
};
template <> struct Held<::Engine::game_Actor> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 23 && is_value<kern::i64>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::logic::Text>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<::Engine::game_Transform>(*v.field(6)) && is_value<kern::List<kern::logic::Text>>(*v.field(7)) && is_value<kern::List<::Engine::game_MeshComponent>>(*v.field(8)) && is_value<kern::List<::Engine::game_LightComponent>>(*v.field(9)) && is_value<kern::List<::Engine::game_BodyComponent>>(*v.field(10)) && is_value<kern::List<::Engine::game_ColliderComponent>>(*v.field(11)) && is_value<kern::List<::Engine::game_JointComponent>>(*v.field(12)) && is_value<kern::List<::Engine::game_ControllerComponent>>(*v.field(13)) && is_value<kern::List<::Engine::game_PickupComponent>>(*v.field(14)) && is_value<kern::List<::Engine::game_TriggerComponent>>(*v.field(15)) && is_value<kern::List<::Engine::game_RotatorComponent>>(*v.field(16)) && is_value<kern::List<::Engine::game_MoverComponent>>(*v.field(17)) && is_value<kern::List<::Engine::game_CityComponent>>(*v.field(18)) && is_value<kern::List<::Engine::game_AudioComponent>>(*v.field(19)) && is_value<kern::List<::Engine::game_WeaponComponent>>(*v.field(20)) && is_value<kern::List<::Engine::game_HealthComponent>>(*v.field(21)) && is_value<kern::List<::Engine::logic_Graph>>(*v.field(22)); }
  static ::Engine::game_Actor from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::logic::Text>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<::Engine::game_Transform>(*v.field(6)), from_value<kern::List<kern::logic::Text>>(*v.field(7)), from_value<kern::List<::Engine::game_MeshComponent>>(*v.field(8)), from_value<kern::List<::Engine::game_LightComponent>>(*v.field(9)), from_value<kern::List<::Engine::game_BodyComponent>>(*v.field(10)), from_value<kern::List<::Engine::game_ColliderComponent>>(*v.field(11)), from_value<kern::List<::Engine::game_JointComponent>>(*v.field(12)), from_value<kern::List<::Engine::game_ControllerComponent>>(*v.field(13)), from_value<kern::List<::Engine::game_PickupComponent>>(*v.field(14)), from_value<kern::List<::Engine::game_TriggerComponent>>(*v.field(15)), from_value<kern::List<::Engine::game_RotatorComponent>>(*v.field(16)), from_value<kern::List<::Engine::game_MoverComponent>>(*v.field(17)), from_value<kern::List<::Engine::game_CityComponent>>(*v.field(18)), from_value<kern::List<::Engine::game_AudioComponent>>(*v.field(19)), from_value<kern::List<::Engine::game_WeaponComponent>>(*v.field(20)), from_value<kern::List<::Engine::game_HealthComponent>>(*v.field(21)), from_value<kern::List<::Engine::logic_Graph>>(*v.field(22))};
  }
  static Value to(const ::Engine::game_Actor& s) { return Value::structure({to_value(s.id), to_value(s.name), to_value(s.parent), to_value(s.folder), to_value(s.hidden), to_value(s.locked), to_value(s.transform), to_value(s.tags), to_value(s.mesh), to_value(s.light), to_value(s.body), to_value(s.colliders), to_value(s.joints), to_value(s.controller), to_value(s.pickup), to_value(s.trigger), to_value(s.rotator), to_value(s.mover), to_value(s.city), to_value(s.audio), to_value(s.weapon), to_value(s.health), to_value(s.logic)}); }
};
template <> struct Held<::Engine::game_Level> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 9 && is_value<kern::logic::Text>(*v.field(0)) && is_value<::Engine::game_WorldSettings>(*v.field(1)) && is_value<kern::List<::Engine::game_Actor>>(*v.field(2)) && is_value<kern::List<kern::logic::Whole>>(*v.field(3)) && is_value<kern::List<kern::logic::Whole>>(*v.field(4)) && is_value<kern::List<kern::logic::Whole>>(*v.field(5)) && is_value<::Engine::logic_Graph>(*v.field(6)) && is_value<::Engine::game_EditorCamera>(*v.field(7)) && is_value<kern::i64>(*v.field(8)); }
  static ::Engine::game_Level from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<::Engine::game_WorldSettings>(*v.field(1)), from_value<kern::List<::Engine::game_Actor>>(*v.field(2)), from_value<kern::List<kern::logic::Whole>>(*v.field(3)), from_value<kern::List<kern::logic::Whole>>(*v.field(4)), from_value<kern::List<kern::logic::Whole>>(*v.field(5)), from_value<::Engine::logic_Graph>(*v.field(6)), from_value<::Engine::game_EditorCamera>(*v.field(7)), from_value<kern::i64>(*v.field(8))};
  }
  static Value to(const ::Engine::game_Level& s) { return Value::structure({to_value(s.name), to_value(s.world), to_value(s.actors), to_value(s.textures), to_value(s.sounds), to_value(s.motions), to_value(s.logic), to_value(s.camera), to_value(s.next_id)}); }
};
template <> struct Held<::Engine::game_Issue> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<bool>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::logic::Text>(*v.field(2)); }
  static ::Engine::game_Issue from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<bool>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::logic::Text>(*v.field(2))};
  }
  static Value to(const ::Engine::game_Issue& s) { return Value::structure({to_value(s.error), to_value(s.actor), to_value(s.text)}); }
};
template <> struct Held<::Engine::ExtensionAttribute> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::logic::Text>(*v.field(2)); }
  static ::Engine::ExtensionAttribute from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::logic::Text>(*v.field(2))};
  }
  static Value to(const ::Engine::ExtensionAttribute& s) { return Value::structure({to_value(s.key), to_value(s.size), to_value(s.glsl)}); }
};
template <> struct Held<::Engine::game_ShapePlan> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 11 && is_value<kern::i64>(*v.field(0)) && is_value<::Engine::physics_ShapeKind>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<::Engine::Quat>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::List<kern::Vec3>>(*v.field(7)) && is_value<kern::List<kern::logic::Whole>>(*v.field(8)) && is_value<::Engine::game_ColliderComponent>(*v.field(9)) && is_value<bool>(*v.field(10)); }
  static ::Engine::game_ShapePlan from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<::Engine::physics_ShapeKind>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<::Engine::Quat>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::List<kern::Vec3>>(*v.field(7)), from_value<kern::List<kern::logic::Whole>>(*v.field(8)), from_value<::Engine::game_ColliderComponent>(*v.field(9)), from_value<bool>(*v.field(10))};
  }
  static Value to(const ::Engine::game_ShapePlan& s) { return Value::structure({to_value(s.actor), to_value(s.kind), to_value(s.center), to_value(s.rotation), to_value(s.size), to_value(s.radius), to_value(s.length), to_value(s.points), to_value(s.indices), to_value(s.source), to_value(s.automatic)}); }
};
template <> struct Held<::Engine::game_BodyPlan> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 6 && is_value<kern::i64>(*v.field(0)) && is_value<::Engine::game_Motion>(*v.field(1)) && is_value<bool>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<::Engine::Quat>(*v.field(4)) && is_value<kern::List<::Engine::game_ShapePlan>>(*v.field(5)); }
  static ::Engine::game_BodyPlan from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<::Engine::game_Motion>(*v.field(1)), from_value<bool>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<::Engine::Quat>(*v.field(4)), from_value<kern::List<::Engine::game_ShapePlan>>(*v.field(5))};
  }
  static Value to(const ::Engine::game_BodyPlan& s) { return Value::structure({to_value(s.owner), to_value(s.motion), to_value(s.implicit), to_value(s.position), to_value(s.rotation), to_value(s.shapes)}); }
};
template <> struct Held<::Engine::game_PlayInput> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 25 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)) && is_value<kern::f64>(*v.field(13)) && is_value<bool>(*v.field(14)) && is_value<kern::f64>(*v.field(15)) && is_value<bool>(*v.field(16)) && is_value<bool>(*v.field(17)) && is_value<bool>(*v.field(18)) && is_value<bool>(*v.field(19)) && is_value<bool>(*v.field(20)) && is_value<kern::f64>(*v.field(21)) && is_value<kern::f64>(*v.field(22)) && is_value<bool>(*v.field(23)) && is_value<kern::f64>(*v.field(24)); }
  static ::Engine::game_PlayInput from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12)), from_value<kern::f64>(*v.field(13)), from_value<bool>(*v.field(14)), from_value<kern::f64>(*v.field(15)), from_value<bool>(*v.field(16)), from_value<bool>(*v.field(17)), from_value<bool>(*v.field(18)), from_value<bool>(*v.field(19)), from_value<bool>(*v.field(20)), from_value<kern::f64>(*v.field(21)), from_value<kern::f64>(*v.field(22)), from_value<bool>(*v.field(23)), from_value<kern::f64>(*v.field(24))};
  }
  static Value to(const ::Engine::game_PlayInput& s) { return Value::structure({to_value(s.move_x), to_value(s.move_y), to_value(s.up), to_value(s.jump), to_value(s.boost), to_value(s.camera), to_value(s.handbrake), to_value(s.robot), to_value(s.look_x), to_value(s.look_y), to_value(s.turn_deg), to_value(s.speed_scale), to_value(s.zoom), to_value(s.view_yaw), to_value(s.has_view_yaw), to_value(s.view_pitch), to_value(s.fire), to_value(s.aim), to_value(s.reload), to_value(s.crouch), to_value(s.interact), to_value(s.look_stick_x), to_value(s.look_stick_y), to_value(s.gamepad), to_value(s.rewind)}); }
};
template <> struct Held<::Engine::Int_Bool_pair> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::i64>(*v.field(0)) && is_value<bool>(*v.field(1)); }
  static ::Engine::Int_Bool_pair from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<bool>(*v.field(1))};
  }
  static Value to(const ::Engine::Int_Bool_pair& s) { return Value::structure({to_value(s.first), to_value(s.second)}); }
};
template <> struct Held<::Engine::game_Hud> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 31 && is_value<::Engine::game_GameStatus>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)) && is_value<kern::logic::Text>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::i64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<bool>(*v.field(11)) && is_value<bool>(*v.field(12)) && is_value<kern::f64>(*v.field(13)) && is_value<bool>(*v.field(14)) && is_value<kern::f64>(*v.field(15)) && is_value<kern::f64>(*v.field(16)) && is_value<kern::f64>(*v.field(17)) && is_value<kern::i64>(*v.field(18)) && is_value<kern::i64>(*v.field(19)) && is_value<kern::i64>(*v.field(20)) && is_value<kern::f64>(*v.field(21)) && is_value<kern::f64>(*v.field(22)) && is_value<kern::f64>(*v.field(23)) && is_value<bool>(*v.field(24)) && is_value<bool>(*v.field(25)) && is_value<kern::f64>(*v.field(26)) && is_value<kern::f64>(*v.field(27)) && is_value<kern::f64>(*v.field(28)) && is_value<bool>(*v.field(29)) && is_value<kern::f64>(*v.field(30)); }
  static ::Engine::game_Hud from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::game_GameStatus>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1)), from_value<kern::logic::Text>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::i64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<bool>(*v.field(11)), from_value<bool>(*v.field(12)), from_value<kern::f64>(*v.field(13)), from_value<bool>(*v.field(14)), from_value<kern::f64>(*v.field(15)), from_value<kern::f64>(*v.field(16)), from_value<kern::f64>(*v.field(17)), from_value<kern::i64>(*v.field(18)), from_value<kern::i64>(*v.field(19)), from_value<kern::i64>(*v.field(20)), from_value<kern::f64>(*v.field(21)), from_value<kern::f64>(*v.field(22)), from_value<kern::f64>(*v.field(23)), from_value<bool>(*v.field(24)), from_value<bool>(*v.field(25)), from_value<kern::f64>(*v.field(26)), from_value<kern::f64>(*v.field(27)), from_value<kern::f64>(*v.field(28)), from_value<bool>(*v.field(29)), from_value<kern::f64>(*v.field(30))};
  }
  static Value to(const ::Engine::game_Hud& s) { return Value::structure({to_value(s.status), to_value(s.title), to_value(s.message), to_value(s.message_age), to_value(s.score), to_value(s.pickups), to_value(s.pickups_total), to_value(s.elapsed), to_value(s.remaining), to_value(s.speed), to_value(s.boost), to_value(s.boosting), to_value(s.interior), to_value(s.best), to_value(s.soldier), to_value(s.health), to_value(s.max_health), to_value(s.armor), to_value(s.ammo), to_value(s.magazine), to_value(s.reserve), to_value(s.reload), to_value(s.spread), to_value(s.hit_age), to_value(s.hit_head), to_value(s.hit_kill), to_value(s.damage_age), to_value(s.damage_yaw), to_value(s.respawn_in), to_value(s.aiming), to_value(s.fov)}); }
};
template <> struct Held<::Engine::game_SimStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::logic::Int64>(*v.field(6)); }
  static ::Engine::game_SimStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::logic::Int64>(*v.field(6))};
  }
  static Value to(const ::Engine::game_SimStats& s) { return Value::structure({to_value(s.bodies), to_value(s.awake), to_value(s.shapes), to_value(s.joints), to_value(s.broken_joints), to_value(s.contacts), to_value(s.steps)}); }
};
template <> struct Held<::Engine::game_SoundCue> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 9 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<::Engine::audio_Bus>(*v.field(5)) && is_value<kern::i64>(*v.field(6)) && is_value<bool>(*v.field(7)) && is_value<bool>(*v.field(8)); }
  static ::Engine::game_SoundCue from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<::Engine::audio_Bus>(*v.field(5)), from_value<kern::i64>(*v.field(6)), from_value<bool>(*v.field(7)), from_value<bool>(*v.field(8))};
  }
  static Value to(const ::Engine::game_SoundCue& s) { return Value::structure({to_value(s.sound), to_value(s.position), to_value(s.volume), to_value(s.pitch), to_value(s.spatial), to_value(s.bus), to_value(s.actor), to_value(s.voice), to_value(s.silence)}); }
};
template <> struct Held<::Engine::game_ShotEvent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 8 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<kern::Vec3>(*v.field(6)) && is_value<kern::i64>(*v.field(7)); }
  static ::Engine::game_ShotEvent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<kern::Vec3>(*v.field(6)), from_value<kern::i64>(*v.field(7))};
  }
  static Value to(const ::Engine::game_ShotEvent& s) { return Value::structure({to_value(s.from), to_value(s.to), to_value(s.shooter), to_value(s.hit), to_value(s.impact), to_value(s.head), to_value(s.normal), to_value(s.surface)}); }
};
template <> struct Held<::Engine::game_SoldierState> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 15 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::i64>(*v.field(9)) && is_value<kern::i64>(*v.field(10)) && is_value<bool>(*v.field(11)) && is_value<bool>(*v.field(12)) && is_value<bool>(*v.field(13)) && is_value<kern::f64>(*v.field(14)); }
  static ::Engine::game_SoldierState from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::i64>(*v.field(9)), from_value<kern::i64>(*v.field(10)), from_value<bool>(*v.field(11)), from_value<bool>(*v.field(12)), from_value<bool>(*v.field(13)), from_value<kern::f64>(*v.field(14))};
  }
  static Value to(const ::Engine::game_SoldierState& s) { return Value::structure({to_value(s.health), to_value(s.max_health), to_value(s.armor), to_value(s.ammo), to_value(s.magazine), to_value(s.reserve), to_value(s.reload), to_value(s.spread), to_value(s.respawn_in), to_value(s.hits), to_value(s.damages), to_value(s.hit_head), to_value(s.hit_kill), to_value(s.dead), to_value(s.damage_yaw)}); }
};
template <> struct Held<::Engine::game_GameWorld_SpawnChange> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)); }
  static ::Engine::game_GameWorld_SpawnChange from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3))};
  }
  static Value to(const ::Engine::game_GameWorld_SpawnChange& s) { return Value::structure({to_value(s.source), to_value(s.id), to_value(s.position), to_value(s.rotation)}); }
};
template <> struct Held<::Engine::physics_QueryFilter> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::logic::Int64>(*v.field(0)) && is_value<kern::logic::Int64>(*v.field(1)); }
  static ::Engine::physics_QueryFilter from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0)), from_value<kern::logic::Int64>(*v.field(1))};
  }
  static Value to(const ::Engine::physics_QueryFilter& s) { return Value::structure({to_value(s.mask_bits), to_value(s.category_bits)}); }
};
template <> struct Held<::Engine::game_GameWorld_WalkerSensors> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 13 && is_value<kern::List<kern::f64>>(*v.field(0)) && is_value<kern::List<kern::logic::Whole>>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::Vec3>(*v.field(6)) && is_value<bool>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::i64>(*v.field(10)) && is_value<kern::List<kern::Vec3>>(*v.field(11)) && is_value<kern::i64>(*v.field(12)); }
  static ::Engine::game_GameWorld_WalkerSensors from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0)), from_value<kern::List<kern::logic::Whole>>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::Vec3>(*v.field(6)), from_value<bool>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::i64>(*v.field(10)), from_value<kern::List<kern::Vec3>>(*v.field(11)), from_value<kern::i64>(*v.field(12))};
  }
  static Value to(const ::Engine::game_GameWorld_WalkerSensors& s) { return Value::structure({to_value(s.range), to_value(s.zone), to_value(s.nearest), to_value(s.nearest_bearing), to_value(s.free_left), to_value(s.free_right), to_value(s.gravity), to_value(s.fallen), to_value(s.speed), to_value(s.tof_time), to_value(s.hits), to_value(s.hit), to_value(s.forward_observed)}); }
};
template <> struct Held<::Engine::game_GameWorld_WalkerProbe> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 12 && is_value<kern::Vec3>(*v.field(0)) && is_value<::Engine::Quat>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::List<bool>>(*v.field(5)) && is_value<kern::List<kern::f64>>(*v.field(6)) && is_value<bool>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<kern::List<kern::f64>>(*v.field(9)) && is_value<kern::List<kern::f64>>(*v.field(10)) && is_value<kern::List<kern::f64>>(*v.field(11)); }
  static ::Engine::game_GameWorld_WalkerProbe from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<::Engine::Quat>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::List<bool>>(*v.field(5)), from_value<kern::List<kern::f64>>(*v.field(6)), from_value<bool>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<kern::List<kern::f64>>(*v.field(9)), from_value<kern::List<kern::f64>>(*v.field(10)), from_value<kern::List<kern::f64>>(*v.field(11))};
  }
  static Value to(const ::Engine::game_GameWorld_WalkerProbe& s) { return Value::structure({to_value(s.position), to_value(s.rotation), to_value(s.velocity), to_value(s.spin), to_value(s.gravity), to_value(s.foot_contact), to_value(s.foot_height), to_value(s.self_contact), to_value(s.body_contact), to_value(s.joint), to_value(s.joint_speed), to_value(s.action)}); }
};
template <> struct Held<::Engine::game_GameWorld_WalkerJoint> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::Engine::game_GameWorld_WalkerJoint from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::Engine::game_GameWorld_WalkerJoint& s) { return Value::structure({to_value(s.name), to_value(s.angle), to_value(s.lower), to_value(s.upper)}); }
};
template <> struct Held<::Engine::game_GameWorld_DetectorModel> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::logic::Int64>(*v.field(3)); }
  static ::Engine::game_GameWorld_DetectorModel from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::logic::Int64>(*v.field(3))};
  }
  static Value to(const ::Engine::game_GameWorld_DetectorModel& s) { return Value::structure({to_value(s.sigma_abs), to_value(s.sigma_rel), to_value(s.dropout), to_value(s.seed)}); }
};
template <> struct Held<::Engine::game_GameWorld_ArtStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<bool>(*v.field(4)); }
  static ::Engine::game_GameWorld_ArtStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<bool>(*v.field(4))};
  }
  static Value to(const ::Engine::game_GameWorld_ArtStats& s) { return Value::structure({to_value(s.rigs), to_value(s.decals), to_value(s.tracers), to_value(s.chips), to_value(s.view_model)}); }
};
template <> struct Held<::Engine::BuildingStyle> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::List<::Engine::Rgb>>(*v.field(0)) && is_value<kern::List<::Engine::Rgb>>(*v.field(1)) && is_value<::Engine::Rgb>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::Engine::BuildingStyle from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<::Engine::Rgb>>(*v.field(0)), from_value<kern::List<::Engine::Rgb>>(*v.field(1)), from_value<::Engine::Rgb>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::Engine::BuildingStyle& s) { return Value::structure({to_value(s.walls), to_value(s.roofs), to_value(s.trim), to_value(s.trim_mix)}); }
};
template <> struct Held<::Engine::BuildingOptions> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::f64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<bool>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<::Engine::BuildingStyle>(*v.field(4)); }
  static ::Engine::BuildingOptions from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<bool>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<::Engine::BuildingStyle>(*v.field(4))};
  }
  static Value to(const ::Engine::BuildingOptions& s) { return Value::structure({to_value(s.chunk_size), to_value(s.max_buildings), to_value(s.cornice), to_value(s.wall_records), to_value(s.style)}); }
};
template <> struct Held<::Engine::GeoChunk> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<::Engine::MeshData>(*v.field(6)); }
  static ::Engine::GeoChunk from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<::Engine::MeshData>(*v.field(6))};
  }
  static Value to(const ::Engine::GeoChunk& s) { return Value::structure({to_value(s.x), to_value(s.y), to_value(s.items), to_value(s.triangles), to_value(s.hash), to_value(s.welded), to_value(s.mesh)}); }
};
template <> struct Held<::Engine::MeshInput> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 16 && is_value<kern::List<kern::f64>>(*v.field(0)) && is_value<kern::List<kern::f64>>(*v.field(1)) && is_value<kern::List<kern::f64>>(*v.field(2)) && is_value<kern::List<kern::f64>>(*v.field(3)) && is_value<kern::List<kern::logic::Whole>>(*v.field(4)) && is_value<kern::List<kern::f64>>(*v.field(5)) && is_value<kern::List<kern::f64>>(*v.field(6)) && is_value<kern::List<kern::f64>>(*v.field(7)) && is_value<kern::List<::Engine::Float_row>>(*v.field(8)) && is_value<kern::List<kern::f64>>(*v.field(9)) && is_value<kern::List<kern::f64>>(*v.field(10)) && is_value<::Engine::Material>(*v.field(11)) && is_value<kern::i64>(*v.field(12)) && is_value<bool>(*v.field(13)) && is_value<kern::i64>(*v.field(14)) && is_value<bool>(*v.field(15)); }
  static ::Engine::MeshInput from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0)), from_value<kern::List<kern::f64>>(*v.field(1)), from_value<kern::List<kern::f64>>(*v.field(2)), from_value<kern::List<kern::f64>>(*v.field(3)), from_value<kern::List<kern::logic::Whole>>(*v.field(4)), from_value<kern::List<kern::f64>>(*v.field(5)), from_value<kern::List<kern::f64>>(*v.field(6)), from_value<kern::List<kern::f64>>(*v.field(7)), from_value<kern::List<::Engine::Float_row>>(*v.field(8)), from_value<kern::List<kern::f64>>(*v.field(9)), from_value<kern::List<kern::f64>>(*v.field(10)), from_value<::Engine::Material>(*v.field(11)), from_value<kern::i64>(*v.field(12)), from_value<bool>(*v.field(13)), from_value<kern::i64>(*v.field(14)), from_value<bool>(*v.field(15))};
  }
  static Value to(const ::Engine::MeshInput& s) { return Value::structure({to_value(s.positions), to_value(s.normals), to_value(s.colors), to_value(s.uvs), to_value(s.indices), to_value(s.joints), to_value(s.weights), to_value(s.bones), to_value(s.extension), to_value(s.transforms), to_value(s.tints), to_value(s.material), to_value(s.texture), to_value(s.visible), to_value(s.layers), to_value(s.pick_bvh)}); }
};
template <> struct Held<::Engine::MeshLook> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 6 && is_value<::Engine::Material>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<bool>(*v.field(5)); }
  static ::Engine::MeshLook from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::Material>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<bool>(*v.field(5))};
  }
  static Value to(const ::Engine::MeshLook& s) { return Value::structure({to_value(s.material), to_value(s.extra_slot), to_value(s.texture), to_value(s.visible), to_value(s.layers), to_value(s.pick_bvh)}); }
};
template <> struct Held<::Engine::WeldStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<bool>(*v.field(4)); }
  static ::Engine::WeldStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<bool>(*v.field(4))};
  }
  static Value to(const ::Engine::WeldStats& s) { return Value::structure({to_value(s.source_vertices), to_value(s.unique_vertices), to_value(s.bytes_before), to_value(s.bytes_after), to_value(s.indexed)}); }
};
template <> struct Held<::Engine::ParticleSpec> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 9 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<::Engine::Vec4>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)); }
  static ::Engine::ParticleSpec from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<::Engine::Vec4>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8))};
  }
  static Value to(const ::Engine::ParticleSpec& s) { return Value::structure({to_value(s.position), to_value(s.velocity), to_value(s.color), to_value(s.start_ms), to_value(s.life_ms), to_value(s.size), to_value(s.gravity), to_value(s.grow), to_value(s.fade_extra)}); }
};
template <> struct Held<::Engine::ParticleStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)); }
  static ::Engine::ParticleStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3))};
  }
  static Value to(const ::Engine::ParticleStats& s) { return Value::structure({to_value(s.active), to_value(s.dropped), to_value(s.uploaded), to_value(s.expired)}); }
};
template <> struct Held<::Engine::PointLights> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::i64>(*v.field(0)) && is_value<kern::List<kern::f64>>(*v.field(1)) && is_value<kern::List<kern::f64>>(*v.field(2)); }
  static ::Engine::PointLights from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::List<kern::f64>>(*v.field(1)), from_value<kern::List<kern::f64>>(*v.field(2))};
  }
  static Value to(const ::Engine::PointLights& s) { return Value::structure({to_value(s.count), to_value(s.pos_range), to_value(s.color_intensity)}); }
};
template <> struct Held<::Engine::SkyState> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::Vec3>(*v.field(6)); }
  static ::Engine::SkyState from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::Vec3>(*v.field(6))};
  }
  static Value to(const ::Engine::SkyState& s) { return Value::structure({to_value(s.sun), to_value(s.moon), to_value(s.night), to_value(s.clouds), to_value(s.blast_direction), to_value(s.blast_age), to_value(s.wind)}); }
};
template <> struct Held<::Engine::SkyAtmosphere> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)); }
  static ::Engine::SkyAtmosphere from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4))};
  }
  static Value to(const ::Engine::SkyAtmosphere& s) { return Value::structure({to_value(s.fog_color), to_value(s.sun_color), to_value(s.sun_direction), to_value(s.sun_scatter), to_value(s.horizon)}); }
};
template <> struct Held<::Engine::BloomPass_Occlusion> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<::Engine::Mat4>(*v.field(3)); }
  static ::Engine::BloomPass_Occlusion from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<::Engine::Mat4>(*v.field(3))};
  }
  static Value to(const ::Engine::BloomPass_Occlusion& s) { return Value::structure({to_value(s.texture), to_value(s.scale_x), to_value(s.scale_y), to_value(s.projection)}); }
};
template <> struct Held<::Engine::Lighting> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 14 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::Vec3>(*v.field(8)) && is_value<kern::Vec3>(*v.field(9)) && is_value<kern::Vec3>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<kern::f64>(*v.field(12)) && is_value<kern::f64>(*v.field(13)); }
  static ::Engine::Lighting from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::Vec3>(*v.field(8)), from_value<kern::Vec3>(*v.field(9)), from_value<kern::Vec3>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<kern::f64>(*v.field(12)), from_value<kern::f64>(*v.field(13))};
  }
  static Value to(const ::Engine::Lighting& s) { return Value::structure({to_value(s.sun_direction), to_value(s.sun_color), to_value(s.sky_color), to_value(s.ground_color), to_value(s.fog_color), to_value(s.fog_density), to_value(s.exposure), to_value(s.env_intensity), to_value(s.env_zenith), to_value(s.env_horizon), to_value(s.env_ground), to_value(s.fog_height_falloff), to_value(s.fog_sun_scatter), to_value(s.fog_horizon)}); }
};
template <> struct Held<::Engine::Viewport> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)); }
  static ::Engine::Viewport from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3))};
  }
  static Value to(const ::Engine::Viewport& s) { return Value::structure({to_value(s.x), to_value(s.y), to_value(s.width), to_value(s.height)}); }
};
template <> struct Held<::Engine::RenderParams> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 8 && is_value<::Engine::Vec4>(*v.field(0)) && is_value<bool>(*v.field(1)) && is_value<::Engine::Viewport>(*v.field(2)) && is_value<bool>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<kern::i64>(*v.field(7)); }
  static ::Engine::RenderParams from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::Vec4>(*v.field(0)), from_value<bool>(*v.field(1)), from_value<::Engine::Viewport>(*v.field(2)), from_value<bool>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<kern::i64>(*v.field(7))};
  }
  static Value to(const ::Engine::RenderParams& s) { return Value::structure({to_value(s.clear_color), to_value(s.has_viewport), to_value(s.viewport), to_value(s.page_hidden), to_value(s.layers), to_value(s.sky), to_value(s.reuse_shadow), to_value(s.framebuffer)}); }
};
template <> struct Held<::Engine::Ray> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::Engine::Ray from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::Engine::Ray& s) { return Value::structure({to_value(s.origin), to_value(s.direction), to_value(s.near), to_value(s.far)}); }
};
template <> struct Held<::Engine::PickQuery> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::f64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<bool>(*v.field(2)) && is_value<kern::i64>(*v.field(3)); }
  static ::Engine::PickQuery from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<bool>(*v.field(2)), from_value<kern::i64>(*v.field(3))};
  }
  static Value to(const ::Engine::PickQuery& s) { return Value::structure({to_value(s.max_distance), to_value(s.ignore_id), to_value(s.include_invisible), to_value(s.layers)}); }
};
template <> struct Held<::Engine::PickHit> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::Vec3>(*v.field(5)) && is_value<kern::Vec3>(*v.field(6)); }
  static ::Engine::PickHit from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::Vec3>(*v.field(5)), from_value<kern::Vec3>(*v.field(6))};
  }
  static Value to(const ::Engine::PickHit& s) { return Value::structure({to_value(s.mesh), to_value(s.instance), to_value(s.triangle), to_value(s.distance), to_value(s.point), to_value(s.normal), to_value(s.barycentric)}); }
};
template <> struct Held<::Engine::PickStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)); }
  static ::Engine::PickStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1))};
  }
  static Value to(const ::Engine::PickStats& s) { return Value::structure({to_value(s.triangles_examined), to_value(s.bounds_rejected)}); }
};
template <> struct Held<::Engine::ShaderSource> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::logic::Text>(*v.field(0)) && is_value<kern::logic::Text>(*v.field(1)); }
  static ::Engine::ShaderSource from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Text>(*v.field(0)), from_value<kern::logic::Text>(*v.field(1))};
  }
  static Value to(const ::Engine::ShaderSource& s) { return Value::structure({to_value(s.vertex), to_value(s.fragment)}); }
};
template <> struct Held<::Engine::net_Message> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::List<kern::logic::Whole>>(*v.field(2)); }
  static ::Engine::net_Message from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::List<kern::logic::Whole>>(*v.field(2))};
  }
  static Value to(const ::Engine::net_Message& s) { return Value::structure({to_value(s.id), to_value(s.type), to_value(s.data)}); }
};
template <> struct Held<::Engine::net_EndpointStats> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 15 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::logic::Int64>(*v.field(5)) && is_value<kern::logic::Int64>(*v.field(6)) && is_value<kern::logic::Int64>(*v.field(7)) && is_value<kern::logic::Int64>(*v.field(8)) && is_value<kern::logic::Int64>(*v.field(9)) && is_value<kern::logic::Int64>(*v.field(10)) && is_value<kern::logic::Int64>(*v.field(11)) && is_value<kern::logic::Int64>(*v.field(12)) && is_value<kern::logic::Int64>(*v.field(13)) && is_value<kern::logic::Int64>(*v.field(14)); }
  static ::Engine::net_EndpointStats from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::logic::Int64>(*v.field(5)), from_value<kern::logic::Int64>(*v.field(6)), from_value<kern::logic::Int64>(*v.field(7)), from_value<kern::logic::Int64>(*v.field(8)), from_value<kern::logic::Int64>(*v.field(9)), from_value<kern::logic::Int64>(*v.field(10)), from_value<kern::logic::Int64>(*v.field(11)), from_value<kern::logic::Int64>(*v.field(12)), from_value<kern::logic::Int64>(*v.field(13)), from_value<kern::logic::Int64>(*v.field(14))};
  }
  static Value to(const ::Engine::net_EndpointStats& s) { return Value::structure({to_value(s.rtt_ms), to_value(s.packet_loss), to_value(s.sent_kbps), to_value(s.received_kbps), to_value(s.acked_kbps), to_value(s.packets_sent), to_value(s.packets_received), to_value(s.packets_acked), to_value(s.packets_stale), to_value(s.packets_invalid), to_value(s.packets_too_large_to_send), to_value(s.packets_too_large_to_receive), to_value(s.fragments_sent), to_value(s.fragments_received), to_value(s.fragments_invalid)}); }
};
template <> struct Held<::Engine::net_EndpointConfig> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 12 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::i64>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::i64>(*v.field(11)); }
  static ::Engine::net_EndpointConfig from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::i64>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::i64>(*v.field(11))};
  }
  static Value to(const ::Engine::net_EndpointConfig& s) { return Value::structure({to_value(s.max_packet_size), to_value(s.fragment_above), to_value(s.fragment_size), to_value(s.max_fragments), to_value(s.ack_buffer_size), to_value(s.sent_packets_buffer_size), to_value(s.received_packets_buffer_size), to_value(s.fragment_reassembly_buffer_size), to_value(s.rtt_smoothing), to_value(s.packet_loss_smoothing), to_value(s.bandwidth_smoothing), to_value(s.packet_header_size)}); }
};
template <> struct Held<::Engine::net_Simulator_Packet> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<::Engine::net_Address>(*v.field(0)) && is_value<::Engine::net_Address>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::List<kern::logic::Whole>>(*v.field(3)); }
  static ::Engine::net_Simulator_Packet from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::net_Address>(*v.field(0)), from_value<::Engine::net_Address>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::List<kern::logic::Whole>>(*v.field(3))};
  }
  static Value to(const ::Engine::net_Simulator_Packet& s) { return Value::structure({to_value(s.from), to_value(s.to), to_value(s.deliver), to_value(s.data)}); }
};
template <> struct Held<::Engine::physics_Pose> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::Vec3>(*v.field(0)) && is_value<::Engine::Quat>(*v.field(1)); }
  static ::Engine::physics_Pose from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<::Engine::Quat>(*v.field(1))};
  }
  static Value to(const ::Engine::physics_Pose& s) { return Value::structure({to_value(s.position), to_value(s.rotation)}); }
};
template <> struct Held<::Engine::physics_Version> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)); }
  static ::Engine::physics_Version from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2))};
  }
  static Value to(const ::Engine::physics_Version& s) { return Value::structure({to_value(s.major), to_value(s.minor), to_value(s.revision)}); }
};
template <> struct Held<::Engine::physics_WorldSettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 11 && is_value<kern::Vec3>(*v.field(0)) && is_value<bool>(*v.field(1)) && is_value<bool>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<kern::f64>(*v.field(9)) && is_value<kern::f64>(*v.field(10)); }
  static ::Engine::physics_WorldSettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<bool>(*v.field(1)), from_value<bool>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<kern::f64>(*v.field(9)), from_value<kern::f64>(*v.field(10))};
  }
  static Value to(const ::Engine::physics_WorldSettings& s) { return Value::structure({to_value(s.gravity), to_value(s.enable_sleep), to_value(s.enable_continuous), to_value(s.restitution_threshold), to_value(s.restitution_iterations), to_value(s.restitution_propagation), to_value(s.hit_event_threshold), to_value(s.contact_hertz), to_value(s.contact_damping_ratio), to_value(s.contact_speed), to_value(s.maximum_linear_speed)}); }
};
template <> struct Held<::Engine::physics_BodySettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 13 && is_value<::Engine::physics_BodyType>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<::Engine::Quat>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::f64>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<bool>(*v.field(9)) && is_value<bool>(*v.field(10)) && is_value<bool>(*v.field(11)) && is_value<kern::logic::Int64>(*v.field(12)); }
  static ::Engine::physics_BodySettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::physics_BodyType>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<::Engine::Quat>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::f64>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<bool>(*v.field(9)), from_value<bool>(*v.field(10)), from_value<bool>(*v.field(11)), from_value<kern::logic::Int64>(*v.field(12))};
  }
  static Value to(const ::Engine::physics_BodySettings& s) { return Value::structure({to_value(s.type), to_value(s.position), to_value(s.rotation), to_value(s.linear_velocity), to_value(s.angular_velocity), to_value(s.linear_damping), to_value(s.angular_damping), to_value(s.gravity_scale), to_value(s.enable_sleep), to_value(s.awake), to_value(s.enabled), to_value(s.bullet), to_value(s.user)}); }
};
template <> struct Held<::Engine::physics_ShapeMaterial> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 13 && is_value<kern::f64>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<bool>(*v.field(4)) && is_value<kern::logic::Int64>(*v.field(5)) && is_value<kern::logic::Int64>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<bool>(*v.field(8)) && is_value<bool>(*v.field(9)) && is_value<bool>(*v.field(10)) && is_value<kern::logic::Int64>(*v.field(11)) && is_value<::Engine::physics_Surface>(*v.field(12)); }
  static ::Engine::physics_ShapeMaterial from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::f64>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<bool>(*v.field(4)), from_value<kern::logic::Int64>(*v.field(5)), from_value<kern::logic::Int64>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<bool>(*v.field(8)), from_value<bool>(*v.field(9)), from_value<bool>(*v.field(10)), from_value<kern::logic::Int64>(*v.field(11)), from_value<::Engine::physics_Surface>(*v.field(12))};
  }
  static Value to(const ::Engine::physics_ShapeMaterial& s) { return Value::structure({to_value(s.density), to_value(s.friction), to_value(s.restitution), to_value(s.rolling_resistance), to_value(s.sensor), to_value(s.category_bits), to_value(s.mask_bits), to_value(s.group_index), to_value(s.contact_events), to_value(s.sensor_events), to_value(s.hit_events), to_value(s.user), to_value(s.surface)}); }
};
template <> struct Held<::Engine::physics_ShapeSettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 9 && is_value<::Engine::physics_ShapeKind>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<::Engine::Quat>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<kern::List<kern::Vec3>>(*v.field(6)) && is_value<kern::List<kern::logic::Whole>>(*v.field(7)) && is_value<::Engine::physics_ShapeMaterial>(*v.field(8)); }
  static ::Engine::physics_ShapeSettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::physics_ShapeKind>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<::Engine::Quat>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<kern::List<kern::Vec3>>(*v.field(6)), from_value<kern::List<kern::logic::Whole>>(*v.field(7)), from_value<::Engine::physics_ShapeMaterial>(*v.field(8))};
  }
  static Value to(const ::Engine::physics_ShapeSettings& s) { return Value::structure({to_value(s.kind), to_value(s.center), to_value(s.rotation), to_value(s.size), to_value(s.radius), to_value(s.length), to_value(s.points), to_value(s.indices), to_value(s.material)}); }
};
template <> struct Held<::Engine::physics_JointSettings> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 40 && is_value<::Engine::physics_JointType>(*v.field(0)) && is_value<::Engine::physics_Body>(*v.field(1)) && is_value<::Engine::physics_Body>(*v.field(2)) && is_value<::Engine::physics_Pose>(*v.field(3)) && is_value<::Engine::physics_Pose>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<bool>(*v.field(6)) && is_value<kern::f64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)) && is_value<bool>(*v.field(9)) && is_value<kern::f64>(*v.field(10)) && is_value<kern::f64>(*v.field(11)) && is_value<bool>(*v.field(12)) && is_value<kern::f64>(*v.field(13)) && is_value<kern::f64>(*v.field(14)) && is_value<kern::f64>(*v.field(15)) && is_value<kern::Vec3>(*v.field(16)) && is_value<kern::f64>(*v.field(17)) && is_value<kern::f64>(*v.field(18)) && is_value<kern::f64>(*v.field(19)) && is_value<kern::f64>(*v.field(20)) && is_value<kern::f64>(*v.field(21)) && is_value<kern::Vec3>(*v.field(22)) && is_value<kern::Vec3>(*v.field(23)) && is_value<kern::f64>(*v.field(24)) && is_value<kern::f64>(*v.field(25)) && is_value<kern::f64>(*v.field(26)) && is_value<kern::f64>(*v.field(27)) && is_value<bool>(*v.field(28)) && is_value<bool>(*v.field(29)) && is_value<kern::f64>(*v.field(30)) && is_value<kern::f64>(*v.field(31)) && is_value<kern::f64>(*v.field(32)) && is_value<kern::f64>(*v.field(33)) && is_value<kern::f64>(*v.field(34)) && is_value<kern::f64>(*v.field(35)) && is_value<kern::f64>(*v.field(36)) && is_value<kern::f64>(*v.field(37)) && is_value<kern::f64>(*v.field(38)) && is_value<kern::logic::Int64>(*v.field(39)); }
  static ::Engine::physics_JointSettings from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::physics_JointType>(*v.field(0)), from_value<::Engine::physics_Body>(*v.field(1)), from_value<::Engine::physics_Body>(*v.field(2)), from_value<::Engine::physics_Pose>(*v.field(3)), from_value<::Engine::physics_Pose>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<bool>(*v.field(6)), from_value<kern::f64>(*v.field(7)), from_value<kern::f64>(*v.field(8)), from_value<bool>(*v.field(9)), from_value<kern::f64>(*v.field(10)), from_value<kern::f64>(*v.field(11)), from_value<bool>(*v.field(12)), from_value<kern::f64>(*v.field(13)), from_value<kern::f64>(*v.field(14)), from_value<kern::f64>(*v.field(15)), from_value<kern::Vec3>(*v.field(16)), from_value<kern::f64>(*v.field(17)), from_value<kern::f64>(*v.field(18)), from_value<kern::f64>(*v.field(19)), from_value<kern::f64>(*v.field(20)), from_value<kern::f64>(*v.field(21)), from_value<kern::Vec3>(*v.field(22)), from_value<kern::Vec3>(*v.field(23)), from_value<kern::f64>(*v.field(24)), from_value<kern::f64>(*v.field(25)), from_value<kern::f64>(*v.field(26)), from_value<kern::f64>(*v.field(27)), from_value<bool>(*v.field(28)), from_value<bool>(*v.field(29)), from_value<kern::f64>(*v.field(30)), from_value<kern::f64>(*v.field(31)), from_value<kern::f64>(*v.field(32)), from_value<kern::f64>(*v.field(33)), from_value<kern::f64>(*v.field(34)), from_value<kern::f64>(*v.field(35)), from_value<kern::f64>(*v.field(36)), from_value<kern::f64>(*v.field(37)), from_value<kern::f64>(*v.field(38)), from_value<kern::logic::Int64>(*v.field(39))};
  }
  static Value to(const ::Engine::physics_JointSettings& s) { return Value::structure({to_value(s.type), to_value(s.body_a), to_value(s.body_b), to_value(s.frame_a), to_value(s.frame_b), to_value(s.collide_connected), to_value(s.enable_spring), to_value(s.hertz), to_value(s.damping_ratio), to_value(s.enable_limit), to_value(s.lower), to_value(s.upper), to_value(s.enable_motor), to_value(s.motor_speed), to_value(s.max_motor_torque), to_value(s.max_motor_force), to_value(s.motor_velocity), to_value(s.length), to_value(s.linear_hertz), to_value(s.angular_hertz), to_value(s.linear_damping_ratio), to_value(s.angular_damping_ratio), to_value(s.linear_velocity), to_value(s.angular_velocity), to_value(s.max_velocity_force), to_value(s.max_velocity_torque), to_value(s.max_spring_force), to_value(s.max_spring_torque), to_value(s.enable_steering), to_value(s.enable_steering_limit), to_value(s.steering_hertz), to_value(s.steering_damping_ratio), to_value(s.target_steering), to_value(s.max_steering_torque), to_value(s.steering_lower), to_value(s.steering_upper), to_value(s.max_torque), to_value(s.constraint_hertz), to_value(s.constraint_damping_ratio), to_value(s.user)}); }
};
template <> struct Held<::Engine::physics_MotionLocks> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<kern::List<bool>>(*v.field(0)) && is_value<kern::List<bool>>(*v.field(1)); }
  static ::Engine::physics_MotionLocks from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<bool>>(*v.field(0)), from_value<kern::List<bool>>(*v.field(1))};
  }
  static Value to(const ::Engine::physics_MotionLocks& s) { return Value::structure({to_value(s.linear), to_value(s.angular)}); }
};
template <> struct Held<::Engine::physics_ContactPoint> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)); }
  static ::Engine::physics_ContactPoint from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3))};
  }
  static Value to(const ::Engine::physics_ContactPoint& s) { return Value::structure({to_value(s.point), to_value(s.normal), to_value(s.separation), to_value(s.impulse)}); }
};
template <> struct Held<::Engine::physics_Body_Float_pair> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 2 && is_value<::Engine::physics_Body>(*v.field(0)) && is_value<kern::f64>(*v.field(1)); }
  static ::Engine::physics_Body_Float_pair from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::physics_Body>(*v.field(0)), from_value<kern::f64>(*v.field(1))};
  }
  static Value to(const ::Engine::physics_Body_Float_pair& s) { return Value::structure({to_value(s.first), to_value(s.second)}); }
};
template <> struct Held<::Engine::physics_RayHit> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 6 && is_value<bool>(*v.field(0)) && is_value<::Engine::physics_Shape>(*v.field(1)) && is_value<::Engine::physics_Body>(*v.field(2)) && is_value<kern::Vec3>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::f64>(*v.field(5)); }
  static ::Engine::physics_RayHit from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<bool>(*v.field(0)), from_value<::Engine::physics_Shape>(*v.field(1)), from_value<::Engine::physics_Body>(*v.field(2)), from_value<kern::Vec3>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::f64>(*v.field(5))};
  }
  static Value to(const ::Engine::physics_RayHit& s) { return Value::structure({to_value(s.hit), to_value(s.shape), to_value(s.body), to_value(s.point), to_value(s.normal), to_value(s.fraction)}); }
};
template <> struct Held<::Engine::physics_Capsule> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 3 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::Vec3>(*v.field(1)) && is_value<kern::f64>(*v.field(2)); }
  static ::Engine::physics_Capsule from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::Vec3>(*v.field(1)), from_value<kern::f64>(*v.field(2))};
  }
  static Value to(const ::Engine::physics_Capsule& s) { return Value::structure({to_value(s.center1), to_value(s.center2), to_value(s.radius)}); }
};
template <> struct Held<::Engine::physics_MoverPlane> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 5 && is_value<kern::Vec3>(*v.field(0)) && is_value<kern::f64>(*v.field(1)) && is_value<kern::Vec3>(*v.field(2)) && is_value<::Engine::physics_Shape>(*v.field(3)) && is_value<::Engine::physics_Body>(*v.field(4)); }
  static ::Engine::physics_MoverPlane from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::Vec3>(*v.field(0)), from_value<kern::f64>(*v.field(1)), from_value<kern::Vec3>(*v.field(2)), from_value<::Engine::physics_Shape>(*v.field(3)), from_value<::Engine::physics_Body>(*v.field(4))};
  }
  static Value to(const ::Engine::physics_MoverPlane& s) { return Value::structure({to_value(s.normal), to_value(s.offset), to_value(s.point), to_value(s.shape), to_value(s.body)}); }
};
template <> struct Held<::Engine::physics_TouchEvent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<::Engine::physics_Shape>(*v.field(0)) && is_value<::Engine::physics_Shape>(*v.field(1)) && is_value<::Engine::physics_Body>(*v.field(2)) && is_value<::Engine::physics_Body>(*v.field(3)); }
  static ::Engine::physics_TouchEvent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::physics_Shape>(*v.field(0)), from_value<::Engine::physics_Shape>(*v.field(1)), from_value<::Engine::physics_Body>(*v.field(2)), from_value<::Engine::physics_Body>(*v.field(3))};
  }
  static Value to(const ::Engine::physics_TouchEvent& s) { return Value::structure({to_value(s.shape_a), to_value(s.shape_b), to_value(s.body_a), to_value(s.body_b)}); }
};
template <> struct Held<::Engine::physics_HitEvent> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<::Engine::physics_Shape>(*v.field(0)) && is_value<::Engine::physics_Shape>(*v.field(1)) && is_value<::Engine::physics_Body>(*v.field(2)) && is_value<::Engine::physics_Body>(*v.field(3)) && is_value<kern::Vec3>(*v.field(4)) && is_value<kern::Vec3>(*v.field(5)) && is_value<kern::f64>(*v.field(6)); }
  static ::Engine::physics_HitEvent from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::physics_Shape>(*v.field(0)), from_value<::Engine::physics_Shape>(*v.field(1)), from_value<::Engine::physics_Body>(*v.field(2)), from_value<::Engine::physics_Body>(*v.field(3)), from_value<kern::Vec3>(*v.field(4)), from_value<kern::Vec3>(*v.field(5)), from_value<kern::f64>(*v.field(6))};
  }
  static Value to(const ::Engine::physics_HitEvent& s) { return Value::structure({to_value(s.shape_a), to_value(s.shape_b), to_value(s.body_a), to_value(s.body_b), to_value(s.point), to_value(s.normal), to_value(s.approach_speed)}); }
};
template <> struct Held<::Engine::net_ChannelConfig> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 9 && is_value<::Engine::net_ChannelType>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::i64>(*v.field(6)) && is_value<kern::i64>(*v.field(7)) && is_value<kern::f64>(*v.field(8)); }
  static ::Engine::net_ChannelConfig from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<::Engine::net_ChannelType>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::i64>(*v.field(6)), from_value<kern::i64>(*v.field(7)), from_value<kern::f64>(*v.field(8))};
  }
  static Value to(const ::Engine::net_ChannelConfig& s) { return Value::structure({to_value(s.type), to_value(s.send_queue_size), to_value(s.receive_queue_size), to_value(s.max_messages_per_packet), to_value(s.packet_budget), to_value(s.max_message_size), to_value(s.block_threshold), to_value(s.block_fragment_size), to_value(s.resend_time)}); }
};
template <> struct Held<::Engine::net_ConnectionConfig> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 4 && is_value<kern::List<::Engine::net_ChannelConfig>>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<::Engine::net_EndpointConfig>(*v.field(3)); }
  static ::Engine::net_ConnectionConfig from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<::Engine::net_ChannelConfig>>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<::Engine::net_EndpointConfig>(*v.field(3))};
  }
  static Value to(const ::Engine::net_ConnectionConfig& s) { return Value::structure({to_value(s.channels), to_value(s.max_packet_size), to_value(s.max_message_types), to_value(s.endpoint)}); }
};
template <> struct Held<::Engine::net_ClientServerConfig> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::logic::Int64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::f64>(*v.field(2)) && is_value<kern::f64>(*v.field(3)) && is_value<kern::f64>(*v.field(4)) && is_value<kern::f64>(*v.field(5)) && is_value<::Engine::net_ConnectionConfig>(*v.field(6)); }
  static ::Engine::net_ClientServerConfig from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::logic::Int64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::f64>(*v.field(2)), from_value<kern::f64>(*v.field(3)), from_value<kern::f64>(*v.field(4)), from_value<kern::f64>(*v.field(5)), from_value<::Engine::net_ConnectionConfig>(*v.field(6))};
  }
  static Value to(const ::Engine::net_ClientServerConfig& s) { return Value::structure({to_value(s.protocol_id), to_value(s.max_clients), to_value(s.timeout), to_value(s.request_interval), to_value(s.connect_timeout), to_value(s.keepalive_interval), to_value(s.connection)}); }
};
}  // namespace kern::logic::detail
namespace {
struct Engine_Native final : kern::logic::NativeBlueprint {
  Engine self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, const std::vector<kern::logic::Value>& args, std::vector<kern::logic::Value>& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "Run1") {
      kern::logic::native_call(true);
      self.Run1();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run2") {
      kern::logic::native_call(true);
      self.Run2();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run3") {
      kern::logic::native_call(true);
      self.Run3();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run4") {
      kern::logic::native_call(true);
      self.Run4();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run5") {
      kern::logic::native_call(true);
      self.Run5();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run6") {
      kern::logic::native_call(true);
      self.Run6();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run7") {
      kern::logic::native_call(true);
      self.Run7();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run8") {
      kern::logic::native_call(true);
      self.Run8();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run9") {
      kern::logic::native_call(true);
      self.Run9();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run10") {
      kern::logic::native_call(true);
      self.Run10();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run11") {
      kern::logic::native_call(true);
      self.Run11();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run12") {
      kern::logic::native_call(true);
      self.Run12();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run13") {
      kern::logic::native_call(true);
      self.Run13();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run14") {
      kern::logic::native_call(true);
      self.Run14();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run15") {
      kern::logic::native_call(true);
      self.Run15();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run16") {
      kern::logic::native_call(true);
      self.Run16();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run17") {
      kern::logic::native_call(true);
      self.Run17();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Run18") {
      kern::logic::native_call(true);
      self.Run18();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Free1") {
      kern::logic::native_call(true);
      self.Free1();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Free2") {
      kern::logic::native_call(true);
      self.Free2();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Free3") {
      kern::logic::native_call(true);
      self.Free3();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Free4") {
      kern::logic::native_call(true);
      self.Free4();
      kern::logic::native_call(false);
      return true;
    }
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    if (variable == "In1") { out = kern::logic::to_value(self.In1); return true; }
    if (variable == "R0") { out = kern::logic::to_value(self.R0); return true; }
    if (variable == "R1") { out = kern::logic::to_value(self.R1); return true; }
    if (variable == "R2") { out = kern::logic::to_value(self.R2); return true; }
    if (variable == "R3") { out = kern::logic::to_value(self.R3); return true; }
    if (variable == "R4") { out = kern::logic::to_value(self.R4); return true; }
    if (variable == "R5") { out = kern::logic::to_value(self.R5); return true; }
    if (variable == "R6") { out = kern::logic::to_value(self.R6); return true; }
    if (variable == "R7") { out = kern::logic::to_value(self.R7); return true; }
    if (variable == "R8") { out = kern::logic::to_value(self.R8); return true; }
    if (variable == "R9") { out = kern::logic::to_value(self.R9); return true; }
    if (variable == "R10") { out = kern::logic::to_value(self.R10); return true; }
    if (variable == "R18") { out = kern::logic::to_value(self.R18); return true; }
    if (variable == "R19_0") { out = kern::logic::to_value(self.R19_0); return true; }
    if (variable == "R20") { out = kern::logic::to_value(self.R20); return true; }
    if (variable == "R23") { out = kern::logic::to_value(self.R23); return true; }
    if (variable == "R24") { out = kern::logic::to_value(self.R24); return true; }
    if (variable == "R25") { out = kern::logic::to_value(self.R25); return true; }
    if (variable == "R27") { out = kern::logic::to_value(self.R27); return true; }
    if (variable == "R28") { out = kern::logic::to_value(self.R28); return true; }
    if (variable == "R29") { out = kern::logic::to_value(self.R29); return true; }
    if (variable == "R30") { out = kern::logic::to_value(self.R30); return true; }
    if (variable == "R31") { out = kern::logic::to_value(self.R31); return true; }
    if (variable == "In2") { out = kern::logic::to_value(self.In2); return true; }
    if (variable == "R33") { out = kern::logic::to_value(self.R33); return true; }
    if (variable == "R35") { out = kern::logic::to_value(self.R35); return true; }
    if (variable == "R37") { out = kern::logic::to_value(self.R37); return true; }
    if (variable == "R39") { out = kern::logic::to_value(self.R39); return true; }
    if (variable == "R41") { out = kern::logic::to_value(self.R41); return true; }
    if (variable == "R43") { out = kern::logic::to_value(self.R43); return true; }
    if (variable == "R45") { out = kern::logic::to_value(self.R45); return true; }
    if (variable == "R47") { out = kern::logic::to_value(self.R47); return true; }
    if (variable == "R49") { out = kern::logic::to_value(self.R49); return true; }
    if (variable == "R51") { out = kern::logic::to_value(self.R51); return true; }
    if (variable == "R53") { out = kern::logic::to_value(self.R53); return true; }
    if (variable == "R55") { out = kern::logic::to_value(self.R55); return true; }
    if (variable == "R57") { out = kern::logic::to_value(self.R57); return true; }
    if (variable == "In3") { out = kern::logic::to_value(self.In3); return true; }
    if (variable == "R59") { out = kern::logic::to_value(self.R59); return true; }
    if (variable == "R61") { out = kern::logic::to_value(self.R61); return true; }
    if (variable == "R63") { out = kern::logic::to_value(self.R63); return true; }
    if (variable == "R65") { out = kern::logic::to_value(self.R65); return true; }
    if (variable == "R67") { out = kern::logic::to_value(self.R67); return true; }
    if (variable == "R69_0") { out = kern::logic::to_value(self.R69_0); return true; }
    if (variable == "R70_0") { out = kern::logic::to_value(self.R70_0); return true; }
    if (variable == "R71_0") { out = kern::logic::to_value(self.R71_0); return true; }
    if (variable == "In4") { out = kern::logic::to_value(self.In4); return true; }
    if (variable == "In5") { out = kern::logic::to_value(self.In5); return true; }
    if (variable == "R75") { out = kern::logic::to_value(self.R75); return true; }
    if (variable == "R76_0") { out = kern::logic::to_value(self.R76_0); return true; }
    if (variable == "R76_1") { out = kern::logic::to_value(self.R76_1); return true; }
    if (variable == "In6") { out = kern::logic::to_value(self.In6); return true; }
    if (variable == "R77") { out = kern::logic::to_value(self.R77); return true; }
    if (variable == "R78_0") { out = kern::logic::to_value(self.R78_0); return true; }
    if (variable == "R78_1") { out = kern::logic::to_value(self.R78_1); return true; }
    if (variable == "R79_0") { out = kern::logic::to_value(self.R79_0); return true; }
    if (variable == "R80") { out = kern::logic::to_value(self.R80); return true; }
    if (variable == "R81") { out = kern::logic::to_value(self.R81); return true; }
    if (variable == "R82_0") { out = kern::logic::to_value(self.R82_0); return true; }
    if (variable == "R83_0") { out = kern::logic::to_value(self.R83_0); return true; }
    if (variable == "R84_0") { out = kern::logic::to_value(self.R84_0); return true; }
    if (variable == "R85") { out = kern::logic::to_value(self.R85); return true; }
    if (variable == "R86") { out = kern::logic::to_value(self.R86); return true; }
    if (variable == "R87") { out = kern::logic::to_value(self.R87); return true; }
    if (variable == "In7") { out = kern::logic::to_value(self.In7); return true; }
    if (variable == "R88_0") { out = kern::logic::to_value(self.R88_0); return true; }
    if (variable == "R91") { out = kern::logic::to_value(self.R91); return true; }
    if (variable == "R98") { out = kern::logic::to_value(self.R98); return true; }
    if (variable == "In8") { out = kern::logic::to_value(self.In8); return true; }
    if (variable == "R100") { out = kern::logic::to_value(self.R100); return true; }
    if (variable == "R101_0") { out = kern::logic::to_value(self.R101_0); return true; }
    if (variable == "R102") { out = kern::logic::to_value(self.R102); return true; }
    if (variable == "R104_0") { out = kern::logic::to_value(self.R104_0); return true; }
    if (variable == "R105") { out = kern::logic::to_value(self.R105); return true; }
    if (variable == "In9") { out = kern::logic::to_value(self.In9); return true; }
    if (variable == "R106") { out = kern::logic::to_value(self.R106); return true; }
    if (variable == "R107_0") { out = kern::logic::to_value(self.R107_0); return true; }
    if (variable == "R108_0") { out = kern::logic::to_value(self.R108_0); return true; }
    if (variable == "R109_0") { out = kern::logic::to_value(self.R109_0); return true; }
    if (variable == "R110") { out = kern::logic::to_value(self.R110); return true; }
    if (variable == "R111") { out = kern::logic::to_value(self.R111); return true; }
    if (variable == "R112_0") { out = kern::logic::to_value(self.R112_0); return true; }
    if (variable == "R112_1") { out = kern::logic::to_value(self.R112_1); return true; }
    if (variable == "R112_2") { out = kern::logic::to_value(self.R112_2); return true; }
    if (variable == "R113_0") { out = kern::logic::to_value(self.R113_0); return true; }
    if (variable == "R114_0") { out = kern::logic::to_value(self.R114_0); return true; }
    if (variable == "R115") { out = kern::logic::to_value(self.R115); return true; }
    if (variable == "R116") { out = kern::logic::to_value(self.R116); return true; }
    if (variable == "R117_0") { out = kern::logic::to_value(self.R117_0); return true; }
    if (variable == "R117_1") { out = kern::logic::to_value(self.R117_1); return true; }
    if (variable == "R118") { out = kern::logic::to_value(self.R118); return true; }
    if (variable == "R120") { out = kern::logic::to_value(self.R120); return true; }
    if (variable == "R121") { out = kern::logic::to_value(self.R121); return true; }
    if (variable == "R122_0") { out = kern::logic::to_value(self.R122_0); return true; }
    if (variable == "R123_0") { out = kern::logic::to_value(self.R123_0); return true; }
    if (variable == "R123_1") { out = kern::logic::to_value(self.R123_1); return true; }
    if (variable == "R126") { out = kern::logic::to_value(self.R126); return true; }
    if (variable == "R128") { out = kern::logic::to_value(self.R128); return true; }
    if (variable == "R130") { out = kern::logic::to_value(self.R130); return true; }
    if (variable == "R131") { out = kern::logic::to_value(self.R131); return true; }
    if (variable == "R132_0") { out = kern::logic::to_value(self.R132_0); return true; }
    if (variable == "In10") { out = kern::logic::to_value(self.In10); return true; }
    if (variable == "R134") { out = kern::logic::to_value(self.R134); return true; }
    if (variable == "R135") { out = kern::logic::to_value(self.R135); return true; }
    if (variable == "R136") { out = kern::logic::to_value(self.R136); return true; }
    if (variable == "R137") { out = kern::logic::to_value(self.R137); return true; }
    if (variable == "In11") { out = kern::logic::to_value(self.In11); return true; }
    if (variable == "In12") { out = kern::logic::to_value(self.In12); return true; }
    if (variable == "R138_0") { out = kern::logic::to_value(self.R138_0); return true; }
    if (variable == "R138_1") { out = kern::logic::to_value(self.R138_1); return true; }
    if (variable == "In13") { out = kern::logic::to_value(self.In13); return true; }
    if (variable == "R139_0") { out = kern::logic::to_value(self.R139_0); return true; }
    if (variable == "R139_1") { out = kern::logic::to_value(self.R139_1); return true; }
    if (variable == "R140") { out = kern::logic::to_value(self.R140); return true; }
    if (variable == "In14") { out = kern::logic::to_value(self.In14); return true; }
    if (variable == "R141") { out = kern::logic::to_value(self.R141); return true; }
    if (variable == "In15") { out = kern::logic::to_value(self.In15); return true; }
    if (variable == "R142") { out = kern::logic::to_value(self.R142); return true; }
    if (variable == "In16") { out = kern::logic::to_value(self.In16); return true; }
    if (variable == "R143") { out = kern::logic::to_value(self.R143); return true; }
    if (variable == "In17") { out = kern::logic::to_value(self.In17); return true; }
    if (variable == "R144") { out = kern::logic::to_value(self.R144); return true; }
    if (variable == "R145_0") { out = kern::logic::to_value(self.R145_0); return true; }
    if (variable == "R145_1") { out = kern::logic::to_value(self.R145_1); return true; }
    if (variable == "In18") { out = kern::logic::to_value(self.In18); return true; }
    if (variable == "R146") { out = kern::logic::to_value(self.R146); return true; }
    if (variable == "In19") { out = kern::logic::to_value(self.In19); return true; }
    if (variable == "R147") { out = kern::logic::to_value(self.R147); return true; }
    if (variable == "In20") { out = kern::logic::to_value(self.In20); return true; }
    if (variable == "R148") { out = kern::logic::to_value(self.R148); return true; }
    if (variable == "In21") { out = kern::logic::to_value(self.In21); return true; }
    if (variable == "R149") { out = kern::logic::to_value(self.R149); return true; }
    if (variable == "In22") { out = kern::logic::to_value(self.In22); return true; }
    if (variable == "R150") { out = kern::logic::to_value(self.R150); return true; }
    if (variable == "In23") { out = kern::logic::to_value(self.In23); return true; }
    if (variable == "R151") { out = kern::logic::to_value(self.R151); return true; }
    if (variable == "R152") { out = kern::logic::to_value(self.R152); return true; }
    if (variable == "In24") { out = kern::logic::to_value(self.In24); return true; }
    if (variable == "R153") { out = kern::logic::to_value(self.R153); return true; }
    if (variable == "In25") { out = kern::logic::to_value(self.In25); return true; }
    if (variable == "R154") { out = kern::logic::to_value(self.R154); return true; }
    if (variable == "In26") { out = kern::logic::to_value(self.In26); return true; }
    if (variable == "R155") { out = kern::logic::to_value(self.R155); return true; }
    if (variable == "R156") { out = kern::logic::to_value(self.R156); return true; }
    if (variable == "In27") { out = kern::logic::to_value(self.In27); return true; }
    if (variable == "R157") { out = kern::logic::to_value(self.R157); return true; }
    if (variable == "In28") { out = kern::logic::to_value(self.In28); return true; }
    if (variable == "R158") { out = kern::logic::to_value(self.R158); return true; }
    if (variable == "R159") { out = kern::logic::to_value(self.R159); return true; }
    if (variable == "R160") { out = kern::logic::to_value(self.R160); return true; }
    if (variable == "In29") { out = kern::logic::to_value(self.In29); return true; }
    if (variable == "R161_0") { out = kern::logic::to_value(self.R161_0); return true; }
    if (variable == "R161_1") { out = kern::logic::to_value(self.R161_1); return true; }
    if (variable == "R162") { out = kern::logic::to_value(self.R162); return true; }
    if (variable == "R163") { out = kern::logic::to_value(self.R163); return true; }
    if (variable == "R164") { out = kern::logic::to_value(self.R164); return true; }
    if (variable == "R165") { out = kern::logic::to_value(self.R165); return true; }
    if (variable == "R166") { out = kern::logic::to_value(self.R166); return true; }
    if (variable == "R167") { out = kern::logic::to_value(self.R167); return true; }
    if (variable == "R168") { out = kern::logic::to_value(self.R168); return true; }
    if (variable == "R169_0") { out = kern::logic::to_value(self.R169_0); return true; }
    if (variable == "R169_1") { out = kern::logic::to_value(self.R169_1); return true; }
    if (variable == "R170") { out = kern::logic::to_value(self.R170); return true; }
    if (variable == "R171") { out = kern::logic::to_value(self.R171); return true; }
    if (variable == "R172") { out = kern::logic::to_value(self.R172); return true; }
    if (variable == "R173") { out = kern::logic::to_value(self.R173); return true; }
    if (variable == "R174") { out = kern::logic::to_value(self.R174); return true; }
    if (variable == "R175") { out = kern::logic::to_value(self.R175); return true; }
    if (variable == "R176") { out = kern::logic::to_value(self.R176); return true; }
    if (variable == "R177_0") { out = kern::logic::to_value(self.R177_0); return true; }
    if (variable == "R177_1") { out = kern::logic::to_value(self.R177_1); return true; }
    if (variable == "R178") { out = kern::logic::to_value(self.R178); return true; }
    if (variable == "R179") { out = kern::logic::to_value(self.R179); return true; }
    if (variable == "R180") { out = kern::logic::to_value(self.R180); return true; }
    if (variable == "R181") { out = kern::logic::to_value(self.R181); return true; }
    if (variable == "R182") { out = kern::logic::to_value(self.R182); return true; }
    if (variable == "R183") { out = kern::logic::to_value(self.R183); return true; }
    if (variable == "R184") { out = kern::logic::to_value(self.R184); return true; }
    if (variable == "R185") { out = kern::logic::to_value(self.R185); return true; }
    if (variable == "R186") { out = kern::logic::to_value(self.R186); return true; }
    if (variable == "R187") { out = kern::logic::to_value(self.R187); return true; }
    if (variable == "R190") { out = kern::logic::to_value(self.R190); return true; }
    if (variable == "In30") { out = kern::logic::to_value(self.In30); return true; }
    if (variable == "R191") { out = kern::logic::to_value(self.R191); return true; }
    if (variable == "R195") { out = kern::logic::to_value(self.R195); return true; }
    if (variable == "R196_0") { out = kern::logic::to_value(self.R196_0); return true; }
    if (variable == "R196_1") { out = kern::logic::to_value(self.R196_1); return true; }
    if (variable == "In31") { out = kern::logic::to_value(self.In31); return true; }
    if (variable == "R197_0") { out = kern::logic::to_value(self.R197_0); return true; }
    if (variable == "R197_1") { out = kern::logic::to_value(self.R197_1); return true; }
    if (variable == "R198_0") { out = kern::logic::to_value(self.R198_0); return true; }
    if (variable == "R199") { out = kern::logic::to_value(self.R199); return true; }
    if (variable == "R200_0") { out = kern::logic::to_value(self.R200_0); return true; }
    if (variable == "R201_0") { out = kern::logic::to_value(self.R201_0); return true; }
    if (variable == "R202") { out = kern::logic::to_value(self.R202); return true; }
    if (variable == "In32") { out = kern::logic::to_value(self.In32); return true; }
    if (variable == "R204_0") { out = kern::logic::to_value(self.R204_0); return true; }
    if (variable == "R206") { out = kern::logic::to_value(self.R206); return true; }
    if (variable == "R207") { out = kern::logic::to_value(self.R207); return true; }
    if (variable == "R208") { out = kern::logic::to_value(self.R208); return true; }
    if (variable == "R210") { out = kern::logic::to_value(self.R210); return true; }
    if (variable == "R211_0") { out = kern::logic::to_value(self.R211_0); return true; }
    if (variable == "In33") { out = kern::logic::to_value(self.In33); return true; }
    if (variable == "R214_0") { out = kern::logic::to_value(self.R214_0); return true; }
    if (variable == "R215_0") { out = kern::logic::to_value(self.R215_0); return true; }
    if (variable == "R215_1") { out = kern::logic::to_value(self.R215_1); return true; }
    if (variable == "R216") { out = kern::logic::to_value(self.R216); return true; }
    if (variable == "R217") { out = kern::logic::to_value(self.R217); return true; }
    if (variable == "R218") { out = kern::logic::to_value(self.R218); return true; }
    if (variable == "R219") { out = kern::logic::to_value(self.R219); return true; }
    if (variable == "R220") { out = kern::logic::to_value(self.R220); return true; }
    if (variable == "R221_0") { out = kern::logic::to_value(self.R221_0); return true; }
    if (variable == "R222_0") { out = kern::logic::to_value(self.R222_0); return true; }
    if (variable == "R223") { out = kern::logic::to_value(self.R223); return true; }
    if (variable == "R226") { out = kern::logic::to_value(self.R226); return true; }
    if (variable == "In34") { out = kern::logic::to_value(self.In34); return true; }
    if (variable == "R227_0") { out = kern::logic::to_value(self.R227_0); return true; }
    if (variable == "R228") { out = kern::logic::to_value(self.R228); return true; }
    if (variable == "R230_0") { out = kern::logic::to_value(self.R230_0); return true; }
    if (variable == "R231_0") { out = kern::logic::to_value(self.R231_0); return true; }
    if (variable == "R231_3") { out = kern::logic::to_value(self.R231_3); return true; }
    if (variable == "In35") { out = kern::logic::to_value(self.In35); return true; }
    if (variable == "R232_0") { out = kern::logic::to_value(self.R232_0); return true; }
    if (variable == "R235") { out = kern::logic::to_value(self.R235); return true; }
    if (variable == "R236") { out = kern::logic::to_value(self.R236); return true; }
    if (variable == "R237") { out = kern::logic::to_value(self.R237); return true; }
    if (variable == "R238") { out = kern::logic::to_value(self.R238); return true; }
    if (variable == "R239_0") { out = kern::logic::to_value(self.R239_0); return true; }
    if (variable == "R240") { out = kern::logic::to_value(self.R240); return true; }
    if (variable == "R241_0") { out = kern::logic::to_value(self.R241_0); return true; }
    if (variable == "R244") { out = kern::logic::to_value(self.R244); return true; }
    if (variable == "R245_0") { out = kern::logic::to_value(self.R245_0); return true; }
    if (variable == "R245_1") { out = kern::logic::to_value(self.R245_1); return true; }
    if (variable == "R246_0") { out = kern::logic::to_value(self.R246_0); return true; }
    if (variable == "R246_1") { out = kern::logic::to_value(self.R246_1); return true; }
    if (variable == "R246_2") { out = kern::logic::to_value(self.R246_2); return true; }
    if (variable == "R246_3") { out = kern::logic::to_value(self.R246_3); return true; }
    if (variable == "R247_0") { out = kern::logic::to_value(self.R247_0); return true; }
    if (variable == "R247_1") { out = kern::logic::to_value(self.R247_1); return true; }
    if (variable == "R247_2") { out = kern::logic::to_value(self.R247_2); return true; }
    if (variable == "R247_3") { out = kern::logic::to_value(self.R247_3); return true; }
    if (variable == "In36") { out = kern::logic::to_value(self.In36); return true; }
    if (variable == "R248_0") { out = kern::logic::to_value(self.R248_0); return true; }
    if (variable == "R248_1") { out = kern::logic::to_value(self.R248_1); return true; }
    if (variable == "R248_2") { out = kern::logic::to_value(self.R248_2); return true; }
    if (variable == "R248_3") { out = kern::logic::to_value(self.R248_3); return true; }
    if (variable == "In37") { out = kern::logic::to_value(self.In37); return true; }
    if (variable == "R250") { out = kern::logic::to_value(self.R250); return true; }
    if (variable == "In38") { out = kern::logic::to_value(self.In38); return true; }
    if (variable == "R252_0") { out = kern::logic::to_value(self.R252_0); return true; }
    if (variable == "R252_1") { out = kern::logic::to_value(self.R252_1); return true; }
    if (variable == "R253_0") { out = kern::logic::to_value(self.R253_0); return true; }
    if (variable == "In39") { out = kern::logic::to_value(self.In39); return true; }
    if (variable == "R257") { out = kern::logic::to_value(self.R257); return true; }
    if (variable == "R258") { out = kern::logic::to_value(self.R258); return true; }
    if (variable == "R259_0") { out = kern::logic::to_value(self.R259_0); return true; }
    if (variable == "In40") { out = kern::logic::to_value(self.In40); return true; }
    if (variable == "R261_0") { out = kern::logic::to_value(self.R261_0); return true; }
    if (variable == "R261_1") { out = kern::logic::to_value(self.R261_1); return true; }
    if (variable == "R262_0") { out = kern::logic::to_value(self.R262_0); return true; }
    if (variable == "R262_1") { out = kern::logic::to_value(self.R262_1); return true; }
    if (variable == "R263_0") { out = kern::logic::to_value(self.R263_0); return true; }
    if (variable == "R264_0") { out = kern::logic::to_value(self.R264_0); return true; }
    if (variable == "R265_0") { out = kern::logic::to_value(self.R265_0); return true; }
    if (variable == "R266_0") { out = kern::logic::to_value(self.R266_0); return true; }
    if (variable == "R267_0") { out = kern::logic::to_value(self.R267_0); return true; }
    if (variable == "R268_0") { out = kern::logic::to_value(self.R268_0); return true; }
    if (variable == "R269") { out = kern::logic::to_value(self.R269); return true; }
    if (variable == "R270_0") { out = kern::logic::to_value(self.R270_0); return true; }
    if (variable == "R271_0") { out = kern::logic::to_value(self.R271_0); return true; }
    if (variable == "R272_0") { out = kern::logic::to_value(self.R272_0); return true; }
    if (variable == "R273_0") { out = kern::logic::to_value(self.R273_0); return true; }
    if (variable == "R274") { out = kern::logic::to_value(self.R274); return true; }
    if (variable == "R275_0") { out = kern::logic::to_value(self.R275_0); return true; }
    if (variable == "R276") { out = kern::logic::to_value(self.R276); return true; }
    if (variable == "In41") { out = kern::logic::to_value(self.In41); return true; }
    if (variable == "R277_0") { out = kern::logic::to_value(self.R277_0); return true; }
    if (variable == "R277_1") { out = kern::logic::to_value(self.R277_1); return true; }
    if (variable == "R277_2") { out = kern::logic::to_value(self.R277_2); return true; }
    if (variable == "R277_3") { out = kern::logic::to_value(self.R277_3); return true; }
    if (variable == "In42") { out = kern::logic::to_value(self.In42); return true; }
    if (variable == "R289_0") { out = kern::logic::to_value(self.R289_0); return true; }
    if (variable == "R289_1") { out = kern::logic::to_value(self.R289_1); return true; }
    if (variable == "R290_0") { out = kern::logic::to_value(self.R290_0); return true; }
    if (variable == "R290_1") { out = kern::logic::to_value(self.R290_1); return true; }
    if (variable == "R291_0") { out = kern::logic::to_value(self.R291_0); return true; }
    if (variable == "R292_0") { out = kern::logic::to_value(self.R292_0); return true; }
    if (variable == "R293_0") { out = kern::logic::to_value(self.R293_0); return true; }
    if (variable == "R294_0") { out = kern::logic::to_value(self.R294_0); return true; }
    if (variable == "R295_0") { out = kern::logic::to_value(self.R295_0); return true; }
    if (variable == "R296_0") { out = kern::logic::to_value(self.R296_0); return true; }
    if (variable == "R297") { out = kern::logic::to_value(self.R297); return true; }
    if (variable == "R298_0") { out = kern::logic::to_value(self.R298_0); return true; }
    if (variable == "R299_0") { out = kern::logic::to_value(self.R299_0); return true; }
    if (variable == "R300_0") { out = kern::logic::to_value(self.R300_0); return true; }
    if (variable == "R300_1") { out = kern::logic::to_value(self.R300_1); return true; }
    if (variable == "R300_2") { out = kern::logic::to_value(self.R300_2); return true; }
    if (variable == "R300_3") { out = kern::logic::to_value(self.R300_3); return true; }
    if (variable == "R301") { out = kern::logic::to_value(self.R301); return true; }
    if (variable == "R302_0") { out = kern::logic::to_value(self.R302_0); return true; }
    if (variable == "R303_0") { out = kern::logic::to_value(self.R303_0); return true; }
    if (variable == "R304_0") { out = kern::logic::to_value(self.R304_0); return true; }
    if (variable == "R305_0") { out = kern::logic::to_value(self.R305_0); return true; }
    if (variable == "R306") { out = kern::logic::to_value(self.R306); return true; }
    if (variable == "In43") { out = kern::logic::to_value(self.In43); return true; }
    if (variable == "R307_0") { out = kern::logic::to_value(self.R307_0); return true; }
    if (variable == "R307_1") { out = kern::logic::to_value(self.R307_1); return true; }
    if (variable == "R308_0") { out = kern::logic::to_value(self.R308_0); return true; }
    if (variable == "R309") { out = kern::logic::to_value(self.R309); return true; }
    if (variable == "R310") { out = kern::logic::to_value(self.R310); return true; }
    if (variable == "R311_0") { out = kern::logic::to_value(self.R311_0); return true; }
    if (variable == "R312_0") { out = kern::logic::to_value(self.R312_0); return true; }
    if (variable == "In44") { out = kern::logic::to_value(self.In44); return true; }
    if (variable == "R313_0") { out = kern::logic::to_value(self.R313_0); return true; }
    if (variable == "R314") { out = kern::logic::to_value(self.R314); return true; }
    if (variable == "R315") { out = kern::logic::to_value(self.R315); return true; }
    if (variable == "R316_0") { out = kern::logic::to_value(self.R316_0); return true; }
    if (variable == "R316_1") { out = kern::logic::to_value(self.R316_1); return true; }
    if (variable == "R316_2") { out = kern::logic::to_value(self.R316_2); return true; }
    if (variable == "R316_3") { out = kern::logic::to_value(self.R316_3); return true; }
    if (variable == "R317") { out = kern::logic::to_value(self.R317); return true; }
    if (variable == "R318") { out = kern::logic::to_value(self.R318); return true; }
    if (variable == "R319") { out = kern::logic::to_value(self.R319); return true; }
    if (variable == "R320_0") { out = kern::logic::to_value(self.R320_0); return true; }
    if (variable == "R321_0") { out = kern::logic::to_value(self.R321_0); return true; }
    if (variable == "R325_0") { out = kern::logic::to_value(self.R325_0); return true; }
    if (variable == "R326") { out = kern::logic::to_value(self.R326); return true; }
    if (variable == "R327") { out = kern::logic::to_value(self.R327); return true; }
    if (variable == "R328") { out = kern::logic::to_value(self.R328); return true; }
    if (variable == "R329") { out = kern::logic::to_value(self.R329); return true; }
    if (variable == "R330_0") { out = kern::logic::to_value(self.R330_0); return true; }
    if (variable == "R332") { out = kern::logic::to_value(self.R332); return true; }
    if (variable == "R334") { out = kern::logic::to_value(self.R334); return true; }
    if (variable == "R338") { out = kern::logic::to_value(self.R338); return true; }
    if (variable == "R341") { out = kern::logic::to_value(self.R341); return true; }
    if (variable == "R342") { out = kern::logic::to_value(self.R342); return true; }
    if (variable == "R343") { out = kern::logic::to_value(self.R343); return true; }
    if (variable == "In45") { out = kern::logic::to_value(self.In45); return true; }
    if (variable == "R344") { out = kern::logic::to_value(self.R344); return true; }
    if (variable == "In46") { out = kern::logic::to_value(self.In46); return true; }
    if (variable == "R345_0") { out = kern::logic::to_value(self.R345_0); return true; }
    if (variable == "R346_0") { out = kern::logic::to_value(self.R346_0); return true; }
    if (variable == "R347") { out = kern::logic::to_value(self.R347); return true; }
    if (variable == "R348_0") { out = kern::logic::to_value(self.R348_0); return true; }
    if (variable == "R348_1") { out = kern::logic::to_value(self.R348_1); return true; }
    if (variable == "R348_2") { out = kern::logic::to_value(self.R348_2); return true; }
    if (variable == "R348_3") { out = kern::logic::to_value(self.R348_3); return true; }
    if (variable == "In47") { out = kern::logic::to_value(self.In47); return true; }
    if (variable == "R349") { out = kern::logic::to_value(self.R349); return true; }
    if (variable == "R350") { out = kern::logic::to_value(self.R350); return true; }
    if (variable == "R351") { out = kern::logic::to_value(self.R351); return true; }
    if (variable == "R352") { out = kern::logic::to_value(self.R352); return true; }
    if (variable == "R355") { out = kern::logic::to_value(self.R355); return true; }
    if (variable == "R357") { out = kern::logic::to_value(self.R357); return true; }
    if (variable == "R358") { out = kern::logic::to_value(self.R358); return true; }
    if (variable == "R359_0") { out = kern::logic::to_value(self.R359_0); return true; }
    if (variable == "R360") { out = kern::logic::to_value(self.R360); return true; }
    if (variable == "R361_0") { out = kern::logic::to_value(self.R361_0); return true; }
    if (variable == "R362") { out = kern::logic::to_value(self.R362); return true; }
    if (variable == "In48") { out = kern::logic::to_value(self.In48); return true; }
    if (variable == "R363") { out = kern::logic::to_value(self.R363); return true; }
    if (variable == "In49") { out = kern::logic::to_value(self.In49); return true; }
    if (variable == "R364_0") { out = kern::logic::to_value(self.R364_0); return true; }
    if (variable == "R365_0") { out = kern::logic::to_value(self.R365_0); return true; }
    if (variable == "R365_1") { out = kern::logic::to_value(self.R365_1); return true; }
    if (variable == "R366") { out = kern::logic::to_value(self.R366); return true; }
    if (variable == "R367") { out = kern::logic::to_value(self.R367); return true; }
    if (variable == "R368") { out = kern::logic::to_value(self.R368); return true; }
    if (variable == "R369") { out = kern::logic::to_value(self.R369); return true; }
    if (variable == "R370") { out = kern::logic::to_value(self.R370); return true; }
    if (variable == "R371") { out = kern::logic::to_value(self.R371); return true; }
    if (variable == "R372") { out = kern::logic::to_value(self.R372); return true; }
    if (variable == "R373") { out = kern::logic::to_value(self.R373); return true; }
    if (variable == "R374") { out = kern::logic::to_value(self.R374); return true; }
    if (variable == "R375_0") { out = kern::logic::to_value(self.R375_0); return true; }
    if (variable == "R376_0") { out = kern::logic::to_value(self.R376_0); return true; }
    if (variable == "R377_0") { out = kern::logic::to_value(self.R377_0); return true; }
    if (variable == "R378") { out = kern::logic::to_value(self.R378); return true; }
    if (variable == "R379") { out = kern::logic::to_value(self.R379); return true; }
    if (variable == "R380_0") { out = kern::logic::to_value(self.R380_0); return true; }
    if (variable == "R380_1") { out = kern::logic::to_value(self.R380_1); return true; }
    if (variable == "R380_2") { out = kern::logic::to_value(self.R380_2); return true; }
    if (variable == "R380_3") { out = kern::logic::to_value(self.R380_3); return true; }
    if (variable == "R381") { out = kern::logic::to_value(self.R381); return true; }
    if (variable == "R382") { out = kern::logic::to_value(self.R382); return true; }
    if (variable == "R384") { out = kern::logic::to_value(self.R384); return true; }
    if (variable == "R385") { out = kern::logic::to_value(self.R385); return true; }
    if (variable == "In50") { out = kern::logic::to_value(self.In50); return true; }
    if (variable == "R386_0") { out = kern::logic::to_value(self.R386_0); return true; }
    if (variable == "R386_1") { out = kern::logic::to_value(self.R386_1); return true; }
    if (variable == "R386_2") { out = kern::logic::to_value(self.R386_2); return true; }
    if (variable == "R386_3") { out = kern::logic::to_value(self.R386_3); return true; }
    if (variable == "R387") { out = kern::logic::to_value(self.R387); return true; }
    if (variable == "R388") { out = kern::logic::to_value(self.R388); return true; }
    if (variable == "R389") { out = kern::logic::to_value(self.R389); return true; }
    if (variable == "R390") { out = kern::logic::to_value(self.R390); return true; }
    if (variable == "R391") { out = kern::logic::to_value(self.R391); return true; }
    if (variable == "R392") { out = kern::logic::to_value(self.R392); return true; }
    if (variable == "R393") { out = kern::logic::to_value(self.R393); return true; }
    if (variable == "R394") { out = kern::logic::to_value(self.R394); return true; }
    if (variable == "In51") { out = kern::logic::to_value(self.In51); return true; }
    if (variable == "R396") { out = kern::logic::to_value(self.R396); return true; }
    if (variable == "R397_0") { out = kern::logic::to_value(self.R397_0); return true; }
    if (variable == "In52") { out = kern::logic::to_value(self.In52); return true; }
    if (variable == "R398_0") { out = kern::logic::to_value(self.R398_0); return true; }
    if (variable == "R399_0") { out = kern::logic::to_value(self.R399_0); return true; }
    if (variable == "R401") { out = kern::logic::to_value(self.R401); return true; }
    if (variable == "R402") { out = kern::logic::to_value(self.R402); return true; }
    if (variable == "R403") { out = kern::logic::to_value(self.R403); return true; }
    if (variable == "R404_0") { out = kern::logic::to_value(self.R404_0); return true; }
    if (variable == "R405_0") { out = kern::logic::to_value(self.R405_0); return true; }
    if (variable == "In53") { out = kern::logic::to_value(self.In53); return true; }
    if (variable == "R407_0") { out = kern::logic::to_value(self.R407_0); return true; }
    if (variable == "R409") { out = kern::logic::to_value(self.R409); return true; }
    if (variable == "R410_0") { out = kern::logic::to_value(self.R410_0); return true; }
    if (variable == "R411_0") { out = kern::logic::to_value(self.R411_0); return true; }
    if (variable == "R412") { out = kern::logic::to_value(self.R412); return true; }
    if (variable == "R413") { out = kern::logic::to_value(self.R413); return true; }
    if (variable == "R414") { out = kern::logic::to_value(self.R414); return true; }
    if (variable == "R416") { out = kern::logic::to_value(self.R416); return true; }
    if (variable == "R417_0") { out = kern::logic::to_value(self.R417_0); return true; }
    if (variable == "In54") { out = kern::logic::to_value(self.In54); return true; }
    if (variable == "In55") { out = kern::logic::to_value(self.In55); return true; }
    if (variable == "In56") { out = kern::logic::to_value(self.In56); return true; }
    if (variable == "R421_0") { out = kern::logic::to_value(self.R421_0); return true; }
    if (variable == "In57") { out = kern::logic::to_value(self.In57); return true; }
    if (variable == "R424") { out = kern::logic::to_value(self.R424); return true; }
    if (variable == "R425") { out = kern::logic::to_value(self.R425); return true; }
    if (variable == "In58") { out = kern::logic::to_value(self.In58); return true; }
    if (variable == "R429") { out = kern::logic::to_value(self.R429); return true; }
    if (variable == "R430_0") { out = kern::logic::to_value(self.R430_0); return true; }
    if (variable == "R431") { out = kern::logic::to_value(self.R431); return true; }
    if (variable == "R432") { out = kern::logic::to_value(self.R432); return true; }
    if (variable == "R433") { out = kern::logic::to_value(self.R433); return true; }
    if (variable == "R435") { out = kern::logic::to_value(self.R435); return true; }
    if (variable == "R436") { out = kern::logic::to_value(self.R436); return true; }
    if (variable == "R437_0") { out = kern::logic::to_value(self.R437_0); return true; }
    if (variable == "R439") { out = kern::logic::to_value(self.R439); return true; }
    if (variable == "R440") { out = kern::logic::to_value(self.R440); return true; }
    if (variable == "R441") { out = kern::logic::to_value(self.R441); return true; }
    if (variable == "In59") { out = kern::logic::to_value(self.In59); return true; }
    if (variable == "R453_0") { out = kern::logic::to_value(self.R453_0); return true; }
    if (variable == "R455_0") { out = kern::logic::to_value(self.R455_0); return true; }
    if (variable == "R456") { out = kern::logic::to_value(self.R456); return true; }
    if (variable == "R457") { out = kern::logic::to_value(self.R457); return true; }
    if (variable == "R458_0") { out = kern::logic::to_value(self.R458_0); return true; }
    if (variable == "R460") { out = kern::logic::to_value(self.R460); return true; }
    if (variable == "In60") { out = kern::logic::to_value(self.In60); return true; }
    if (variable == "R461_0") { out = kern::logic::to_value(self.R461_0); return true; }
    if (variable == "R462_0") { out = kern::logic::to_value(self.R462_0); return true; }
    if (variable == "R462_1") { out = kern::logic::to_value(self.R462_1); return true; }
    if (variable == "R463_0") { out = kern::logic::to_value(self.R463_0); return true; }
    if (variable == "R464_0") { out = kern::logic::to_value(self.R464_0); return true; }
    if (variable == "R465_0") { out = kern::logic::to_value(self.R465_0); return true; }
    if (variable == "In61") { out = kern::logic::to_value(self.In61); return true; }
    if (variable == "R466_0") { out = kern::logic::to_value(self.R466_0); return true; }
    if (variable == "R467_0") { out = kern::logic::to_value(self.R467_0); return true; }
    if (variable == "R468") { out = kern::logic::to_value(self.R468); return true; }
    if (variable == "R469") { out = kern::logic::to_value(self.R469); return true; }
    if (variable == "R470") { out = kern::logic::to_value(self.R470); return true; }
    if (variable == "R471_0") { out = kern::logic::to_value(self.R471_0); return true; }
    if (variable == "R472_0") { out = kern::logic::to_value(self.R472_0); return true; }
    if (variable == "R473_0") { out = kern::logic::to_value(self.R473_0); return true; }
    if (variable == "R473_1") { out = kern::logic::to_value(self.R473_1); return true; }
    if (variable == "R474_0") { out = kern::logic::to_value(self.R474_0); return true; }
    if (variable == "R475") { out = kern::logic::to_value(self.R475); return true; }
    if (variable == "R477") { out = kern::logic::to_value(self.R477); return true; }
    if (variable == "In62") { out = kern::logic::to_value(self.In62); return true; }
    if (variable == "R479") { out = kern::logic::to_value(self.R479); return true; }
    if (variable == "In63") { out = kern::logic::to_value(self.In63); return true; }
    if (variable == "R481_0") { out = kern::logic::to_value(self.R481_0); return true; }
    if (variable == "R482") { out = kern::logic::to_value(self.R482); return true; }
    if (variable == "In64") { out = kern::logic::to_value(self.In64); return true; }
    if (variable == "R483_0") { out = kern::logic::to_value(self.R483_0); return true; }
    if (variable == "R488") { out = kern::logic::to_value(self.R488); return true; }
    if (variable == "R489") { out = kern::logic::to_value(self.R489); return true; }
    if (variable == "R491") { out = kern::logic::to_value(self.R491); return true; }
    if (variable == "In65") { out = kern::logic::to_value(self.In65); return true; }
    if (variable == "In66") { out = kern::logic::to_value(self.In66); return true; }
    if (variable == "R492_0") { out = kern::logic::to_value(self.R492_0); return true; }
    if (variable == "R492_1") { out = kern::logic::to_value(self.R492_1); return true; }
    if (variable == "R493") { out = kern::logic::to_value(self.R493); return true; }
    if (variable == "In67") { out = kern::logic::to_value(self.In67); return true; }
    if (variable == "R494_0") { out = kern::logic::to_value(self.R494_0); return true; }
    if (variable == "R494_1") { out = kern::logic::to_value(self.R494_1); return true; }
    if (variable == "In68") { out = kern::logic::to_value(self.In68); return true; }
    if (variable == "In69") { out = kern::logic::to_value(self.In69); return true; }
    if (variable == "R495_0") { out = kern::logic::to_value(self.R495_0); return true; }
    if (variable == "R495_1") { out = kern::logic::to_value(self.R495_1); return true; }
    if (variable == "R495_2") { out = kern::logic::to_value(self.R495_2); return true; }
    if (variable == "R496_0") { out = kern::logic::to_value(self.R496_0); return true; }
    if (variable == "R496_1") { out = kern::logic::to_value(self.R496_1); return true; }
    if (variable == "R496_2") { out = kern::logic::to_value(self.R496_2); return true; }
    if (variable == "R498_0") { out = kern::logic::to_value(self.R498_0); return true; }
    if (variable == "R499") { out = kern::logic::to_value(self.R499); return true; }
    if (variable == "R501") { out = kern::logic::to_value(self.R501); return true; }
    if (variable == "R502") { out = kern::logic::to_value(self.R502); return true; }
    if (variable == "R503") { out = kern::logic::to_value(self.R503); return true; }
    if (variable == "R504") { out = kern::logic::to_value(self.R504); return true; }
    if (variable == "R505") { out = kern::logic::to_value(self.R505); return true; }
    if (variable == "R506") { out = kern::logic::to_value(self.R506); return true; }
    if (variable == "R507") { out = kern::logic::to_value(self.R507); return true; }
    if (variable == "R508") { out = kern::logic::to_value(self.R508); return true; }
    if (variable == "R509") { out = kern::logic::to_value(self.R509); return true; }
    if (variable == "R513") { out = kern::logic::to_value(self.R513); return true; }
    if (variable == "In70") { out = kern::logic::to_value(self.In70); return true; }
    if (variable == "R514") { out = kern::logic::to_value(self.R514); return true; }
    if (variable == "In71") { out = kern::logic::to_value(self.In71); return true; }
    if (variable == "R517") { out = kern::logic::to_value(self.R517); return true; }
    if (variable == "R518") { out = kern::logic::to_value(self.R518); return true; }
    if (variable == "R519") { out = kern::logic::to_value(self.R519); return true; }
    if (variable == "R520") { out = kern::logic::to_value(self.R520); return true; }
    if (variable == "R524_0") { out = kern::logic::to_value(self.R524_0); return true; }
    if (variable == "R525_0") { out = kern::logic::to_value(self.R525_0); return true; }
    if (variable == "R526_0") { out = kern::logic::to_value(self.R526_0); return true; }
    if (variable == "R527") { out = kern::logic::to_value(self.R527); return true; }
    if (variable == "R528") { out = kern::logic::to_value(self.R528); return true; }
    if (variable == "R531") { out = kern::logic::to_value(self.R531); return true; }
    if (variable == "R532") { out = kern::logic::to_value(self.R532); return true; }
    if (variable == "R533") { out = kern::logic::to_value(self.R533); return true; }
    if (variable == "R534") { out = kern::logic::to_value(self.R534); return true; }
    if (variable == "R535") { out = kern::logic::to_value(self.R535); return true; }
    if (variable == "R540_0") { out = kern::logic::to_value(self.R540_0); return true; }
    if (variable == "R541_0") { out = kern::logic::to_value(self.R541_0); return true; }
    if (variable == "R542_0") { out = kern::logic::to_value(self.R542_0); return true; }
    if (variable == "R543_0") { out = kern::logic::to_value(self.R543_0); return true; }
    if (variable == "In72") { out = kern::logic::to_value(self.In72); return true; }
    if (variable == "R544") { out = kern::logic::to_value(self.R544); return true; }
    if (variable == "R546") { out = kern::logic::to_value(self.R546); return true; }
    if (variable == "R547_0") { out = kern::logic::to_value(self.R547_0); return true; }
    if (variable == "R548_0") { out = kern::logic::to_value(self.R548_0); return true; }
    if (variable == "R552") { out = kern::logic::to_value(self.R552); return true; }
    if (variable == "R553") { out = kern::logic::to_value(self.R553); return true; }
    if (variable == "R554_0") { out = kern::logic::to_value(self.R554_0); return true; }
    if (variable == "R555") { out = kern::logic::to_value(self.R555); return true; }
    if (variable == "R556") { out = kern::logic::to_value(self.R556); return true; }
    if (variable == "R558") { out = kern::logic::to_value(self.R558); return true; }
    if (variable == "R559_0") { out = kern::logic::to_value(self.R559_0); return true; }
    if (variable == "R561") { out = kern::logic::to_value(self.R561); return true; }
    if (variable == "R564") { out = kern::logic::to_value(self.R564); return true; }
    if (variable == "R565") { out = kern::logic::to_value(self.R565); return true; }
    if (variable == "R566") { out = kern::logic::to_value(self.R566); return true; }
    if (variable == "R571") { out = kern::logic::to_value(self.R571); return true; }
    if (variable == "R572") { out = kern::logic::to_value(self.R572); return true; }
    if (variable == "R573") { out = kern::logic::to_value(self.R573); return true; }
    if (variable == "R574") { out = kern::logic::to_value(self.R574); return true; }
    if (variable == "R575") { out = kern::logic::to_value(self.R575); return true; }
    if (variable == "R576") { out = kern::logic::to_value(self.R576); return true; }
    if (variable == "R577_0") { out = kern::logic::to_value(self.R577_0); return true; }
    if (variable == "R578_0") { out = kern::logic::to_value(self.R578_0); return true; }
    if (variable == "R579_0") { out = kern::logic::to_value(self.R579_0); return true; }
    if (variable == "R580") { out = kern::logic::to_value(self.R580); return true; }
    if (variable == "R581") { out = kern::logic::to_value(self.R581); return true; }
    if (variable == "R582") { out = kern::logic::to_value(self.R582); return true; }
    if (variable == "R583") { out = kern::logic::to_value(self.R583); return true; }
    if (variable == "R584_0") { out = kern::logic::to_value(self.R584_0); return true; }
    if (variable == "R585_0") { out = kern::logic::to_value(self.R585_0); return true; }
    if (variable == "R586_0") { out = kern::logic::to_value(self.R586_0); return true; }
    if (variable == "R587_0") { out = kern::logic::to_value(self.R587_0); return true; }
    if (variable == "R588_0") { out = kern::logic::to_value(self.R588_0); return true; }
    if (variable == "R590") { out = kern::logic::to_value(self.R590); return true; }
    if (variable == "R591") { out = kern::logic::to_value(self.R591); return true; }
    if (variable == "R592") { out = kern::logic::to_value(self.R592); return true; }
    if (variable == "R593") { out = kern::logic::to_value(self.R593); return true; }
    if (variable == "R594_0") { out = kern::logic::to_value(self.R594_0); return true; }
    if (variable == "R594_1") { out = kern::logic::to_value(self.R594_1); return true; }
    if (variable == "R595_0") { out = kern::logic::to_value(self.R595_0); return true; }
    if (variable == "R595_1") { out = kern::logic::to_value(self.R595_1); return true; }
    if (variable == "R596_0") { out = kern::logic::to_value(self.R596_0); return true; }
    if (variable == "R596_1") { out = kern::logic::to_value(self.R596_1); return true; }
    if (variable == "R597_0") { out = kern::logic::to_value(self.R597_0); return true; }
    if (variable == "R598_0") { out = kern::logic::to_value(self.R598_0); return true; }
    if (variable == "R599") { out = kern::logic::to_value(self.R599); return true; }
    if (variable == "R600") { out = kern::logic::to_value(self.R600); return true; }
    if (variable == "R601") { out = kern::logic::to_value(self.R601); return true; }
    if (variable == "R602") { out = kern::logic::to_value(self.R602); return true; }
    if (variable == "R603_0") { out = kern::logic::to_value(self.R603_0); return true; }
    if (variable == "R604_0") { out = kern::logic::to_value(self.R604_0); return true; }
    if (variable == "R605_0") { out = kern::logic::to_value(self.R605_0); return true; }
    if (variable == "R606_0") { out = kern::logic::to_value(self.R606_0); return true; }
    if (variable == "R607_0") { out = kern::logic::to_value(self.R607_0); return true; }
    if (variable == "R608") { out = kern::logic::to_value(self.R608); return true; }
    if (variable == "R609") { out = kern::logic::to_value(self.R609); return true; }
    if (variable == "R610") { out = kern::logic::to_value(self.R610); return true; }
    if (variable == "R611") { out = kern::logic::to_value(self.R611); return true; }
    if (variable == "R612_0") { out = kern::logic::to_value(self.R612_0); return true; }
    if (variable == "R613_0") { out = kern::logic::to_value(self.R613_0); return true; }
    if (variable == "R613_1") { out = kern::logic::to_value(self.R613_1); return true; }
    if (variable == "R613_2") { out = kern::logic::to_value(self.R613_2); return true; }
    if (variable == "In73") { out = kern::logic::to_value(self.In73); return true; }
    if (variable == "R616_0") { out = kern::logic::to_value(self.R616_0); return true; }
    if (variable == "R618") { out = kern::logic::to_value(self.R618); return true; }
    if (variable == "R619") { out = kern::logic::to_value(self.R619); return true; }
    if (variable == "R620") { out = kern::logic::to_value(self.R620); return true; }
    if (variable == "R621_0") { out = kern::logic::to_value(self.R621_0); return true; }
    if (variable == "R622_0") { out = kern::logic::to_value(self.R622_0); return true; }
    if (variable == "R624") { out = kern::logic::to_value(self.R624); return true; }
    if (variable == "R625") { out = kern::logic::to_value(self.R625); return true; }
    if (variable == "R626") { out = kern::logic::to_value(self.R626); return true; }
    if (variable == "R628_0") { out = kern::logic::to_value(self.R628_0); return true; }
    if (variable == "R629_0") { out = kern::logic::to_value(self.R629_0); return true; }
    if (variable == "R629_1") { out = kern::logic::to_value(self.R629_1); return true; }
    if (variable == "R629_2") { out = kern::logic::to_value(self.R629_2); return true; }
    if (variable == "R632_0") { out = kern::logic::to_value(self.R632_0); return true; }
    if (variable == "R632_1") { out = kern::logic::to_value(self.R632_1); return true; }
    if (variable == "R632_2") { out = kern::logic::to_value(self.R632_2); return true; }
    if (variable == "R633_0") { out = kern::logic::to_value(self.R633_0); return true; }
    if (variable == "In74") { out = kern::logic::to_value(self.In74); return true; }
    if (variable == "R634") { out = kern::logic::to_value(self.R634); return true; }
    if (variable == "R635_0") { out = kern::logic::to_value(self.R635_0); return true; }
    if (variable == "R635_1") { out = kern::logic::to_value(self.R635_1); return true; }
    if (variable == "R636") { out = kern::logic::to_value(self.R636); return true; }
    if (variable == "R637") { out = kern::logic::to_value(self.R637); return true; }
    if (variable == "R638") { out = kern::logic::to_value(self.R638); return true; }
    if (variable == "In75") { out = kern::logic::to_value(self.In75); return true; }
    if (variable == "R639_0") { out = kern::logic::to_value(self.R639_0); return true; }
    if (variable == "R641") { out = kern::logic::to_value(self.R641); return true; }
    if (variable == "In76") { out = kern::logic::to_value(self.In76); return true; }
    if (variable == "R642_0") { out = kern::logic::to_value(self.R642_0); return true; }
    if (variable == "In77") { out = kern::logic::to_value(self.In77); return true; }
    if (variable == "R644") { out = kern::logic::to_value(self.R644); return true; }
    if (variable == "In78") { out = kern::logic::to_value(self.In78); return true; }
    if (variable == "R645_0") { out = kern::logic::to_value(self.R645_0); return true; }
    if (variable == "In79") { out = kern::logic::to_value(self.In79); return true; }
    if (variable == "R647") { out = kern::logic::to_value(self.R647); return true; }
    if (variable == "In80") { out = kern::logic::to_value(self.In80); return true; }
    if (variable == "R648_0") { out = kern::logic::to_value(self.R648_0); return true; }
    if (variable == "In81") { out = kern::logic::to_value(self.In81); return true; }
    if (variable == "R650") { out = kern::logic::to_value(self.R650); return true; }
    if (variable == "R652") { out = kern::logic::to_value(self.R652); return true; }
    if (variable == "In82") { out = kern::logic::to_value(self.In82); return true; }
    if (variable == "R655") { out = kern::logic::to_value(self.R655); return true; }
    if (variable == "R656") { out = kern::logic::to_value(self.R656); return true; }
    if (variable == "R666") { out = kern::logic::to_value(self.R666); return true; }
    if (variable == "R667") { out = kern::logic::to_value(self.R667); return true; }
    if (variable == "R668") { out = kern::logic::to_value(self.R668); return true; }
    if (variable == "In83") { out = kern::logic::to_value(self.In83); return true; }
    if (variable == "R670_0") { out = kern::logic::to_value(self.R670_0); return true; }
    if (variable == "R672") { out = kern::logic::to_value(self.R672); return true; }
    if (variable == "R673") { out = kern::logic::to_value(self.R673); return true; }
    if (variable == "R674") { out = kern::logic::to_value(self.R674); return true; }
    if (variable == "R675") { out = kern::logic::to_value(self.R675); return true; }
    if (variable == "R676") { out = kern::logic::to_value(self.R676); return true; }
    if (variable == "R677") { out = kern::logic::to_value(self.R677); return true; }
    if (variable == "In84") { out = kern::logic::to_value(self.In84); return true; }
    if (variable == "R682") { out = kern::logic::to_value(self.R682); return true; }
    if (variable == "R685") { out = kern::logic::to_value(self.R685); return true; }
    if (variable == "R686") { out = kern::logic::to_value(self.R686); return true; }
    if (variable == "R687") { out = kern::logic::to_value(self.R687); return true; }
    if (variable == "R689") { out = kern::logic::to_value(self.R689); return true; }
    if (variable == "R690") { out = kern::logic::to_value(self.R690); return true; }
    if (variable == "In85") { out = kern::logic::to_value(self.In85); return true; }
    if (variable == "R691_0") { out = kern::logic::to_value(self.R691_0); return true; }
    if (variable == "In86") { out = kern::logic::to_value(self.In86); return true; }
    if (variable == "R692_0") { out = kern::logic::to_value(self.R692_0); return true; }
    if (variable == "R693") { out = kern::logic::to_value(self.R693); return true; }
    if (variable == "In87") { out = kern::logic::to_value(self.In87); return true; }
    if (variable == "R694") { out = kern::logic::to_value(self.R694); return true; }
    if (variable == "R695") { out = kern::logic::to_value(self.R695); return true; }
    if (variable == "In88") { out = kern::logic::to_value(self.In88); return true; }
    if (variable == "R696") { out = kern::logic::to_value(self.R696); return true; }
    if (variable == "In89") { out = kern::logic::to_value(self.In89); return true; }
    if (variable == "R697_0") { out = kern::logic::to_value(self.R697_0); return true; }
    if (variable == "R700") { out = kern::logic::to_value(self.R700); return true; }
    if (variable == "R701") { out = kern::logic::to_value(self.R701); return true; }
    if (variable == "R702") { out = kern::logic::to_value(self.R702); return true; }
    if (variable == "R703") { out = kern::logic::to_value(self.R703); return true; }
    if (variable == "R704") { out = kern::logic::to_value(self.R704); return true; }
    if (variable == "R705") { out = kern::logic::to_value(self.R705); return true; }
    if (variable == "R706") { out = kern::logic::to_value(self.R706); return true; }
    if (variable == "R707") { out = kern::logic::to_value(self.R707); return true; }
    if (variable == "R708") { out = kern::logic::to_value(self.R708); return true; }
    if (variable == "R709") { out = kern::logic::to_value(self.R709); return true; }
    if (variable == "R710_0") { out = kern::logic::to_value(self.R710_0); return true; }
    if (variable == "R712") { out = kern::logic::to_value(self.R712); return true; }
    if (variable == "R715") { out = kern::logic::to_value(self.R715); return true; }
    if (variable == "R716") { out = kern::logic::to_value(self.R716); return true; }
    if (variable == "R717") { out = kern::logic::to_value(self.R717); return true; }
    if (variable == "R718") { out = kern::logic::to_value(self.R718); return true; }
    if (variable == "R720_0") { out = kern::logic::to_value(self.R720_0); return true; }
    if (variable == "R721") { out = kern::logic::to_value(self.R721); return true; }
    if (variable == "R722") { out = kern::logic::to_value(self.R722); return true; }
    if (variable == "R725") { out = kern::logic::to_value(self.R725); return true; }
    if (variable == "R726") { out = kern::logic::to_value(self.R726); return true; }
    if (variable == "R727") { out = kern::logic::to_value(self.R727); return true; }
    if (variable == "R728_0") { out = kern::logic::to_value(self.R728_0); return true; }
    if (variable == "R728_1") { out = kern::logic::to_value(self.R728_1); return true; }
    if (variable == "R730") { out = kern::logic::to_value(self.R730); return true; }
    if (variable == "R731") { out = kern::logic::to_value(self.R731); return true; }
    if (variable == "R732_0") { out = kern::logic::to_value(self.R732_0); return true; }
    if (variable == "R732_1") { out = kern::logic::to_value(self.R732_1); return true; }
    if (variable == "R733_0") { out = kern::logic::to_value(self.R733_0); return true; }
    if (variable == "R733_1") { out = kern::logic::to_value(self.R733_1); return true; }
    if (variable == "R733_2") { out = kern::logic::to_value(self.R733_2); return true; }
    if (variable == "R734_0") { out = kern::logic::to_value(self.R734_0); return true; }
    if (variable == "R736_0") { out = kern::logic::to_value(self.R736_0); return true; }
    if (variable == "R738_0") { out = kern::logic::to_value(self.R738_0); return true; }
    if (variable == "R740_0") { out = kern::logic::to_value(self.R740_0); return true; }
    if (variable == "R742_0") { out = kern::logic::to_value(self.R742_0); return true; }
    if (variable == "R744_0") { out = kern::logic::to_value(self.R744_0); return true; }
    if (variable == "R746_0") { out = kern::logic::to_value(self.R746_0); return true; }
    if (variable == "R748_0") { out = kern::logic::to_value(self.R748_0); return true; }
    if (variable == "R750_0") { out = kern::logic::to_value(self.R750_0); return true; }
    if (variable == "R752_0") { out = kern::logic::to_value(self.R752_0); return true; }
    if (variable == "R754_0") { out = kern::logic::to_value(self.R754_0); return true; }
    if (variable == "R756_0") { out = kern::logic::to_value(self.R756_0); return true; }
    if (variable == "R758_0") { out = kern::logic::to_value(self.R758_0); return true; }
    if (variable == "R760_0") { out = kern::logic::to_value(self.R760_0); return true; }
    if (variable == "R762_0") { out = kern::logic::to_value(self.R762_0); return true; }
    if (variable == "R764_0") { out = kern::logic::to_value(self.R764_0); return true; }
    if (variable == "R766_0") { out = kern::logic::to_value(self.R766_0); return true; }
    if (variable == "R768_0") { out = kern::logic::to_value(self.R768_0); return true; }
    if (variable == "R770_0") { out = kern::logic::to_value(self.R770_0); return true; }
    if (variable == "R772_0") { out = kern::logic::to_value(self.R772_0); return true; }
    if (variable == "R774_0") { out = kern::logic::to_value(self.R774_0); return true; }
    if (variable == "R776_0") { out = kern::logic::to_value(self.R776_0); return true; }
    if (variable == "R778_0") { out = kern::logic::to_value(self.R778_0); return true; }
    if (variable == "R780_0") { out = kern::logic::to_value(self.R780_0); return true; }
    if (variable == "R782_0") { out = kern::logic::to_value(self.R782_0); return true; }
    if (variable == "R784_0") { out = kern::logic::to_value(self.R784_0); return true; }
    if (variable == "R786_0") { out = kern::logic::to_value(self.R786_0); return true; }
    if (variable == "R788_0") { out = kern::logic::to_value(self.R788_0); return true; }
    if (variable == "R790_0") { out = kern::logic::to_value(self.R790_0); return true; }
    if (variable == "R792_0") { out = kern::logic::to_value(self.R792_0); return true; }
    if (variable == "R794_0") { out = kern::logic::to_value(self.R794_0); return true; }
    if (variable == "R796_0") { out = kern::logic::to_value(self.R796_0); return true; }
    if (variable == "In90") { out = kern::logic::to_value(self.In90); return true; }
    if (variable == "R798_0") { out = kern::logic::to_value(self.R798_0); return true; }
    if (variable == "In91") { out = kern::logic::to_value(self.In91); return true; }
    if (variable == "R800_0") { out = kern::logic::to_value(self.R800_0); return true; }
    if (variable == "R802_0") { out = kern::logic::to_value(self.R802_0); return true; }
    if (variable == "R804_0") { out = kern::logic::to_value(self.R804_0); return true; }
    if (variable == "R806_0") { out = kern::logic::to_value(self.R806_0); return true; }
    if (variable == "R808_0") { out = kern::logic::to_value(self.R808_0); return true; }
    if (variable == "R810_0") { out = kern::logic::to_value(self.R810_0); return true; }
    if (variable == "R812_0") { out = kern::logic::to_value(self.R812_0); return true; }
    if (variable == "R814_0") { out = kern::logic::to_value(self.R814_0); return true; }
    if (variable == "R816_0") { out = kern::logic::to_value(self.R816_0); return true; }
    if (variable == "H348") { out = kern::logic::to_value(self.H348); return true; }
    if (variable == "P348_0") { out = kern::logic::to_value(self.P348_0); return true; }
    if (variable == "P348_1") { out = kern::logic::to_value(self.P348_1); return true; }
    if (variable == "P348_2") { out = kern::logic::to_value(self.P348_2); return true; }
    if (variable == "P348_3") { out = kern::logic::to_value(self.P348_3); return true; }
    if (variable == "P348_4") { out = kern::logic::to_value(self.P348_4); return true; }
    if (variable == "In92") { out = kern::logic::to_value(self.In92); return true; }
    if (variable == "H380") { out = kern::logic::to_value(self.H380); return true; }
    if (variable == "P380_0") { out = kern::logic::to_value(self.P380_0); return true; }
    if (variable == "P380_1") { out = kern::logic::to_value(self.P380_1); return true; }
    if (variable == "P380_2") { out = kern::logic::to_value(self.P380_2); return true; }
    if (variable == "P380_3") { out = kern::logic::to_value(self.P380_3); return true; }
    if (variable == "P380_4") { out = kern::logic::to_value(self.P380_4); return true; }
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    if (variable == "In1") { kern::logic::native_set(self.In1, value); return true; }
    if (variable == "R0") { kern::logic::native_set(self.R0, value); return true; }
    if (variable == "R1") { kern::logic::native_set(self.R1, value); return true; }
    if (variable == "R2") { kern::logic::native_set(self.R2, value); return true; }
    if (variable == "R3") { kern::logic::native_set(self.R3, value); return true; }
    if (variable == "R4") { kern::logic::native_set(self.R4, value); return true; }
    if (variable == "R5") { kern::logic::native_set(self.R5, value); return true; }
    if (variable == "R6") { kern::logic::native_set(self.R6, value); return true; }
    if (variable == "R7") { kern::logic::native_set(self.R7, value); return true; }
    if (variable == "R8") { kern::logic::native_set(self.R8, value); return true; }
    if (variable == "R9") { kern::logic::native_set(self.R9, value); return true; }
    if (variable == "R10") { kern::logic::native_set(self.R10, value); return true; }
    if (variable == "R18") { kern::logic::native_set(self.R18, value); return true; }
    if (variable == "R19_0") { kern::logic::native_set(self.R19_0, value); return true; }
    if (variable == "R20") { kern::logic::native_set(self.R20, value); return true; }
    if (variable == "R23") { kern::logic::native_set(self.R23, value); return true; }
    if (variable == "R24") { kern::logic::native_set(self.R24, value); return true; }
    if (variable == "R25") { kern::logic::native_set(self.R25, value); return true; }
    if (variable == "R27") { kern::logic::native_set(self.R27, value); return true; }
    if (variable == "R28") { kern::logic::native_set(self.R28, value); return true; }
    if (variable == "R29") { kern::logic::native_set(self.R29, value); return true; }
    if (variable == "R30") { kern::logic::native_set(self.R30, value); return true; }
    if (variable == "R31") { kern::logic::native_set(self.R31, value); return true; }
    if (variable == "In2") { kern::logic::native_set(self.In2, value); return true; }
    if (variable == "R33") { kern::logic::native_set(self.R33, value); return true; }
    if (variable == "R35") { kern::logic::native_set(self.R35, value); return true; }
    if (variable == "R37") { kern::logic::native_set(self.R37, value); return true; }
    if (variable == "R39") { kern::logic::native_set(self.R39, value); return true; }
    if (variable == "R41") { kern::logic::native_set(self.R41, value); return true; }
    if (variable == "R43") { kern::logic::native_set(self.R43, value); return true; }
    if (variable == "R45") { kern::logic::native_set(self.R45, value); return true; }
    if (variable == "R47") { kern::logic::native_set(self.R47, value); return true; }
    if (variable == "R49") { kern::logic::native_set(self.R49, value); return true; }
    if (variable == "R51") { kern::logic::native_set(self.R51, value); return true; }
    if (variable == "R53") { kern::logic::native_set(self.R53, value); return true; }
    if (variable == "R55") { kern::logic::native_set(self.R55, value); return true; }
    if (variable == "R57") { kern::logic::native_set(self.R57, value); return true; }
    if (variable == "In3") { kern::logic::native_set(self.In3, value); return true; }
    if (variable == "R59") { kern::logic::native_set(self.R59, value); return true; }
    if (variable == "R61") { kern::logic::native_set(self.R61, value); return true; }
    if (variable == "R63") { kern::logic::native_set(self.R63, value); return true; }
    if (variable == "R65") { kern::logic::native_set(self.R65, value); return true; }
    if (variable == "R67") { kern::logic::native_set(self.R67, value); return true; }
    if (variable == "R69_0") { kern::logic::native_set(self.R69_0, value); return true; }
    if (variable == "R70_0") { kern::logic::native_set(self.R70_0, value); return true; }
    if (variable == "R71_0") { kern::logic::native_set(self.R71_0, value); return true; }
    if (variable == "In4") { kern::logic::native_set(self.In4, value); return true; }
    if (variable == "In5") { kern::logic::native_set(self.In5, value); return true; }
    if (variable == "R75") { kern::logic::native_set(self.R75, value); return true; }
    if (variable == "R76_0") { kern::logic::native_set(self.R76_0, value); return true; }
    if (variable == "R76_1") { kern::logic::native_set(self.R76_1, value); return true; }
    if (variable == "In6") { kern::logic::native_set(self.In6, value); return true; }
    if (variable == "R77") { kern::logic::native_set(self.R77, value); return true; }
    if (variable == "R78_0") { kern::logic::native_set(self.R78_0, value); return true; }
    if (variable == "R78_1") { kern::logic::native_set(self.R78_1, value); return true; }
    if (variable == "R79_0") { kern::logic::native_set(self.R79_0, value); return true; }
    if (variable == "R80") { kern::logic::native_set(self.R80, value); return true; }
    if (variable == "R81") { kern::logic::native_set(self.R81, value); return true; }
    if (variable == "R82_0") { kern::logic::native_set(self.R82_0, value); return true; }
    if (variable == "R83_0") { kern::logic::native_set(self.R83_0, value); return true; }
    if (variable == "R84_0") { kern::logic::native_set(self.R84_0, value); return true; }
    if (variable == "R85") { kern::logic::native_set(self.R85, value); return true; }
    if (variable == "R86") { kern::logic::native_set(self.R86, value); return true; }
    if (variable == "R87") { kern::logic::native_set(self.R87, value); return true; }
    if (variable == "In7") { kern::logic::native_set(self.In7, value); return true; }
    if (variable == "R88_0") { kern::logic::native_set(self.R88_0, value); return true; }
    if (variable == "R91") { kern::logic::native_set(self.R91, value); return true; }
    if (variable == "R98") { kern::logic::native_set(self.R98, value); return true; }
    if (variable == "In8") { kern::logic::native_set(self.In8, value); return true; }
    if (variable == "R100") { kern::logic::native_set(self.R100, value); return true; }
    if (variable == "R101_0") { kern::logic::native_set(self.R101_0, value); return true; }
    if (variable == "R102") { kern::logic::native_set(self.R102, value); return true; }
    if (variable == "R104_0") { kern::logic::native_set(self.R104_0, value); return true; }
    if (variable == "R105") { kern::logic::native_set(self.R105, value); return true; }
    if (variable == "In9") { kern::logic::native_set(self.In9, value); return true; }
    if (variable == "R106") { kern::logic::native_set(self.R106, value); return true; }
    if (variable == "R107_0") { kern::logic::native_set(self.R107_0, value); return true; }
    if (variable == "R108_0") { kern::logic::native_set(self.R108_0, value); return true; }
    if (variable == "R109_0") { kern::logic::native_set(self.R109_0, value); return true; }
    if (variable == "R110") { kern::logic::native_set(self.R110, value); return true; }
    if (variable == "R111") { kern::logic::native_set(self.R111, value); return true; }
    if (variable == "R112_0") { kern::logic::native_set(self.R112_0, value); return true; }
    if (variable == "R112_1") { kern::logic::native_set(self.R112_1, value); return true; }
    if (variable == "R112_2") { kern::logic::native_set(self.R112_2, value); return true; }
    if (variable == "R113_0") { kern::logic::native_set(self.R113_0, value); return true; }
    if (variable == "R114_0") { kern::logic::native_set(self.R114_0, value); return true; }
    if (variable == "R115") { kern::logic::native_set(self.R115, value); return true; }
    if (variable == "R116") { kern::logic::native_set(self.R116, value); return true; }
    if (variable == "R117_0") { kern::logic::native_set(self.R117_0, value); return true; }
    if (variable == "R117_1") { kern::logic::native_set(self.R117_1, value); return true; }
    if (variable == "R118") { kern::logic::native_set(self.R118, value); return true; }
    if (variable == "R120") { kern::logic::native_set(self.R120, value); return true; }
    if (variable == "R121") { kern::logic::native_set(self.R121, value); return true; }
    if (variable == "R122_0") { kern::logic::native_set(self.R122_0, value); return true; }
    if (variable == "R123_0") { kern::logic::native_set(self.R123_0, value); return true; }
    if (variable == "R123_1") { kern::logic::native_set(self.R123_1, value); return true; }
    if (variable == "R126") { kern::logic::native_set(self.R126, value); return true; }
    if (variable == "R128") { kern::logic::native_set(self.R128, value); return true; }
    if (variable == "R130") { kern::logic::native_set(self.R130, value); return true; }
    if (variable == "R131") { kern::logic::native_set(self.R131, value); return true; }
    if (variable == "R132_0") { kern::logic::native_set(self.R132_0, value); return true; }
    if (variable == "In10") { kern::logic::native_set(self.In10, value); return true; }
    if (variable == "R134") { kern::logic::native_set(self.R134, value); return true; }
    if (variable == "R135") { kern::logic::native_set(self.R135, value); return true; }
    if (variable == "R136") { kern::logic::native_set(self.R136, value); return true; }
    if (variable == "R137") { kern::logic::native_set(self.R137, value); return true; }
    if (variable == "In11") { kern::logic::native_set(self.In11, value); return true; }
    if (variable == "In12") { kern::logic::native_set(self.In12, value); return true; }
    if (variable == "R138_0") { kern::logic::native_set(self.R138_0, value); return true; }
    if (variable == "R138_1") { kern::logic::native_set(self.R138_1, value); return true; }
    if (variable == "In13") { kern::logic::native_set(self.In13, value); return true; }
    if (variable == "R139_0") { kern::logic::native_set(self.R139_0, value); return true; }
    if (variable == "R139_1") { kern::logic::native_set(self.R139_1, value); return true; }
    if (variable == "R140") { kern::logic::native_set(self.R140, value); return true; }
    if (variable == "In14") { kern::logic::native_set(self.In14, value); return true; }
    if (variable == "R141") { kern::logic::native_set(self.R141, value); return true; }
    if (variable == "In15") { kern::logic::native_set(self.In15, value); return true; }
    if (variable == "R142") { kern::logic::native_set(self.R142, value); return true; }
    if (variable == "In16") { kern::logic::native_set(self.In16, value); return true; }
    if (variable == "R143") { kern::logic::native_set(self.R143, value); return true; }
    if (variable == "In17") { kern::logic::native_set(self.In17, value); return true; }
    if (variable == "R144") { kern::logic::native_set(self.R144, value); return true; }
    if (variable == "R145_0") { kern::logic::native_set(self.R145_0, value); return true; }
    if (variable == "R145_1") { kern::logic::native_set(self.R145_1, value); return true; }
    if (variable == "In18") { kern::logic::native_set(self.In18, value); return true; }
    if (variable == "R146") { kern::logic::native_set(self.R146, value); return true; }
    if (variable == "In19") { kern::logic::native_set(self.In19, value); return true; }
    if (variable == "R147") { kern::logic::native_set(self.R147, value); return true; }
    if (variable == "In20") { kern::logic::native_set(self.In20, value); return true; }
    if (variable == "R148") { kern::logic::native_set(self.R148, value); return true; }
    if (variable == "In21") { kern::logic::native_set(self.In21, value); return true; }
    if (variable == "R149") { kern::logic::native_set(self.R149, value); return true; }
    if (variable == "In22") { kern::logic::native_set(self.In22, value); return true; }
    if (variable == "R150") { kern::logic::native_set(self.R150, value); return true; }
    if (variable == "In23") { kern::logic::native_set(self.In23, value); return true; }
    if (variable == "R151") { kern::logic::native_set(self.R151, value); return true; }
    if (variable == "R152") { kern::logic::native_set(self.R152, value); return true; }
    if (variable == "In24") { kern::logic::native_set(self.In24, value); return true; }
    if (variable == "R153") { kern::logic::native_set(self.R153, value); return true; }
    if (variable == "In25") { kern::logic::native_set(self.In25, value); return true; }
    if (variable == "R154") { kern::logic::native_set(self.R154, value); return true; }
    if (variable == "In26") { kern::logic::native_set(self.In26, value); return true; }
    if (variable == "R155") { kern::logic::native_set(self.R155, value); return true; }
    if (variable == "R156") { kern::logic::native_set(self.R156, value); return true; }
    if (variable == "In27") { kern::logic::native_set(self.In27, value); return true; }
    if (variable == "R157") { kern::logic::native_set(self.R157, value); return true; }
    if (variable == "In28") { kern::logic::native_set(self.In28, value); return true; }
    if (variable == "R158") { kern::logic::native_set(self.R158, value); return true; }
    if (variable == "R159") { kern::logic::native_set(self.R159, value); return true; }
    if (variable == "R160") { kern::logic::native_set(self.R160, value); return true; }
    if (variable == "In29") { kern::logic::native_set(self.In29, value); return true; }
    if (variable == "R161_0") { kern::logic::native_set(self.R161_0, value); return true; }
    if (variable == "R161_1") { kern::logic::native_set(self.R161_1, value); return true; }
    if (variable == "R162") { kern::logic::native_set(self.R162, value); return true; }
    if (variable == "R163") { kern::logic::native_set(self.R163, value); return true; }
    if (variable == "R164") { kern::logic::native_set(self.R164, value); return true; }
    if (variable == "R165") { kern::logic::native_set(self.R165, value); return true; }
    if (variable == "R166") { kern::logic::native_set(self.R166, value); return true; }
    if (variable == "R167") { kern::logic::native_set(self.R167, value); return true; }
    if (variable == "R168") { kern::logic::native_set(self.R168, value); return true; }
    if (variable == "R169_0") { kern::logic::native_set(self.R169_0, value); return true; }
    if (variable == "R169_1") { kern::logic::native_set(self.R169_1, value); return true; }
    if (variable == "R170") { kern::logic::native_set(self.R170, value); return true; }
    if (variable == "R171") { kern::logic::native_set(self.R171, value); return true; }
    if (variable == "R172") { kern::logic::native_set(self.R172, value); return true; }
    if (variable == "R173") { kern::logic::native_set(self.R173, value); return true; }
    if (variable == "R174") { kern::logic::native_set(self.R174, value); return true; }
    if (variable == "R175") { kern::logic::native_set(self.R175, value); return true; }
    if (variable == "R176") { kern::logic::native_set(self.R176, value); return true; }
    if (variable == "R177_0") { kern::logic::native_set(self.R177_0, value); return true; }
    if (variable == "R177_1") { kern::logic::native_set(self.R177_1, value); return true; }
    if (variable == "R178") { kern::logic::native_set(self.R178, value); return true; }
    if (variable == "R179") { kern::logic::native_set(self.R179, value); return true; }
    if (variable == "R180") { kern::logic::native_set(self.R180, value); return true; }
    if (variable == "R181") { kern::logic::native_set(self.R181, value); return true; }
    if (variable == "R182") { kern::logic::native_set(self.R182, value); return true; }
    if (variable == "R183") { kern::logic::native_set(self.R183, value); return true; }
    if (variable == "R184") { kern::logic::native_set(self.R184, value); return true; }
    if (variable == "R185") { kern::logic::native_set(self.R185, value); return true; }
    if (variable == "R186") { kern::logic::native_set(self.R186, value); return true; }
    if (variable == "R187") { kern::logic::native_set(self.R187, value); return true; }
    if (variable == "R190") { kern::logic::native_set(self.R190, value); return true; }
    if (variable == "In30") { kern::logic::native_set(self.In30, value); return true; }
    if (variable == "R191") { kern::logic::native_set(self.R191, value); return true; }
    if (variable == "R195") { kern::logic::native_set(self.R195, value); return true; }
    if (variable == "R196_0") { kern::logic::native_set(self.R196_0, value); return true; }
    if (variable == "R196_1") { kern::logic::native_set(self.R196_1, value); return true; }
    if (variable == "In31") { kern::logic::native_set(self.In31, value); return true; }
    if (variable == "R197_0") { kern::logic::native_set(self.R197_0, value); return true; }
    if (variable == "R197_1") { kern::logic::native_set(self.R197_1, value); return true; }
    if (variable == "R198_0") { kern::logic::native_set(self.R198_0, value); return true; }
    if (variable == "R199") { kern::logic::native_set(self.R199, value); return true; }
    if (variable == "R200_0") { kern::logic::native_set(self.R200_0, value); return true; }
    if (variable == "R201_0") { kern::logic::native_set(self.R201_0, value); return true; }
    if (variable == "R202") { kern::logic::native_set(self.R202, value); return true; }
    if (variable == "In32") { kern::logic::native_set(self.In32, value); return true; }
    if (variable == "R204_0") { kern::logic::native_set(self.R204_0, value); return true; }
    if (variable == "R206") { kern::logic::native_set(self.R206, value); return true; }
    if (variable == "R207") { kern::logic::native_set(self.R207, value); return true; }
    if (variable == "R208") { kern::logic::native_set(self.R208, value); return true; }
    if (variable == "R210") { kern::logic::native_set(self.R210, value); return true; }
    if (variable == "R211_0") { kern::logic::native_set(self.R211_0, value); return true; }
    if (variable == "In33") { kern::logic::native_set(self.In33, value); return true; }
    if (variable == "R214_0") { kern::logic::native_set(self.R214_0, value); return true; }
    if (variable == "R215_0") { kern::logic::native_set(self.R215_0, value); return true; }
    if (variable == "R215_1") { kern::logic::native_set(self.R215_1, value); return true; }
    if (variable == "R216") { kern::logic::native_set(self.R216, value); return true; }
    if (variable == "R217") { kern::logic::native_set(self.R217, value); return true; }
    if (variable == "R218") { kern::logic::native_set(self.R218, value); return true; }
    if (variable == "R219") { kern::logic::native_set(self.R219, value); return true; }
    if (variable == "R220") { kern::logic::native_set(self.R220, value); return true; }
    if (variable == "R221_0") { kern::logic::native_set(self.R221_0, value); return true; }
    if (variable == "R222_0") { kern::logic::native_set(self.R222_0, value); return true; }
    if (variable == "R223") { kern::logic::native_set(self.R223, value); return true; }
    if (variable == "R226") { kern::logic::native_set(self.R226, value); return true; }
    if (variable == "In34") { kern::logic::native_set(self.In34, value); return true; }
    if (variable == "R227_0") { kern::logic::native_set(self.R227_0, value); return true; }
    if (variable == "R228") { kern::logic::native_set(self.R228, value); return true; }
    if (variable == "R230_0") { kern::logic::native_set(self.R230_0, value); return true; }
    if (variable == "R231_0") { kern::logic::native_set(self.R231_0, value); return true; }
    if (variable == "R231_3") { kern::logic::native_set(self.R231_3, value); return true; }
    if (variable == "In35") { kern::logic::native_set(self.In35, value); return true; }
    if (variable == "R232_0") { kern::logic::native_set(self.R232_0, value); return true; }
    if (variable == "R235") { kern::logic::native_set(self.R235, value); return true; }
    if (variable == "R236") { kern::logic::native_set(self.R236, value); return true; }
    if (variable == "R237") { kern::logic::native_set(self.R237, value); return true; }
    if (variable == "R238") { kern::logic::native_set(self.R238, value); return true; }
    if (variable == "R239_0") { kern::logic::native_set(self.R239_0, value); return true; }
    if (variable == "R240") { kern::logic::native_set(self.R240, value); return true; }
    if (variable == "R241_0") { kern::logic::native_set(self.R241_0, value); return true; }
    if (variable == "R244") { kern::logic::native_set(self.R244, value); return true; }
    if (variable == "R245_0") { kern::logic::native_set(self.R245_0, value); return true; }
    if (variable == "R245_1") { kern::logic::native_set(self.R245_1, value); return true; }
    if (variable == "R246_0") { kern::logic::native_set(self.R246_0, value); return true; }
    if (variable == "R246_1") { kern::logic::native_set(self.R246_1, value); return true; }
    if (variable == "R246_2") { kern::logic::native_set(self.R246_2, value); return true; }
    if (variable == "R246_3") { kern::logic::native_set(self.R246_3, value); return true; }
    if (variable == "R247_0") { kern::logic::native_set(self.R247_0, value); return true; }
    if (variable == "R247_1") { kern::logic::native_set(self.R247_1, value); return true; }
    if (variable == "R247_2") { kern::logic::native_set(self.R247_2, value); return true; }
    if (variable == "R247_3") { kern::logic::native_set(self.R247_3, value); return true; }
    if (variable == "In36") { kern::logic::native_set(self.In36, value); return true; }
    if (variable == "R248_0") { kern::logic::native_set(self.R248_0, value); return true; }
    if (variable == "R248_1") { kern::logic::native_set(self.R248_1, value); return true; }
    if (variable == "R248_2") { kern::logic::native_set(self.R248_2, value); return true; }
    if (variable == "R248_3") { kern::logic::native_set(self.R248_3, value); return true; }
    if (variable == "In37") { kern::logic::native_set(self.In37, value); return true; }
    if (variable == "R250") { kern::logic::native_set(self.R250, value); return true; }
    if (variable == "In38") { kern::logic::native_set(self.In38, value); return true; }
    if (variable == "R252_0") { kern::logic::native_set(self.R252_0, value); return true; }
    if (variable == "R252_1") { kern::logic::native_set(self.R252_1, value); return true; }
    if (variable == "R253_0") { kern::logic::native_set(self.R253_0, value); return true; }
    if (variable == "In39") { kern::logic::native_set(self.In39, value); return true; }
    if (variable == "R257") { kern::logic::native_set(self.R257, value); return true; }
    if (variable == "R258") { kern::logic::native_set(self.R258, value); return true; }
    if (variable == "R259_0") { kern::logic::native_set(self.R259_0, value); return true; }
    if (variable == "In40") { kern::logic::native_set(self.In40, value); return true; }
    if (variable == "R261_0") { kern::logic::native_set(self.R261_0, value); return true; }
    if (variable == "R261_1") { kern::logic::native_set(self.R261_1, value); return true; }
    if (variable == "R262_0") { kern::logic::native_set(self.R262_0, value); return true; }
    if (variable == "R262_1") { kern::logic::native_set(self.R262_1, value); return true; }
    if (variable == "R263_0") { kern::logic::native_set(self.R263_0, value); return true; }
    if (variable == "R264_0") { kern::logic::native_set(self.R264_0, value); return true; }
    if (variable == "R265_0") { kern::logic::native_set(self.R265_0, value); return true; }
    if (variable == "R266_0") { kern::logic::native_set(self.R266_0, value); return true; }
    if (variable == "R267_0") { kern::logic::native_set(self.R267_0, value); return true; }
    if (variable == "R268_0") { kern::logic::native_set(self.R268_0, value); return true; }
    if (variable == "R269") { kern::logic::native_set(self.R269, value); return true; }
    if (variable == "R270_0") { kern::logic::native_set(self.R270_0, value); return true; }
    if (variable == "R271_0") { kern::logic::native_set(self.R271_0, value); return true; }
    if (variable == "R272_0") { kern::logic::native_set(self.R272_0, value); return true; }
    if (variable == "R273_0") { kern::logic::native_set(self.R273_0, value); return true; }
    if (variable == "R274") { kern::logic::native_set(self.R274, value); return true; }
    if (variable == "R275_0") { kern::logic::native_set(self.R275_0, value); return true; }
    if (variable == "R276") { kern::logic::native_set(self.R276, value); return true; }
    if (variable == "In41") { kern::logic::native_set(self.In41, value); return true; }
    if (variable == "R277_0") { kern::logic::native_set(self.R277_0, value); return true; }
    if (variable == "R277_1") { kern::logic::native_set(self.R277_1, value); return true; }
    if (variable == "R277_2") { kern::logic::native_set(self.R277_2, value); return true; }
    if (variable == "R277_3") { kern::logic::native_set(self.R277_3, value); return true; }
    if (variable == "In42") { kern::logic::native_set(self.In42, value); return true; }
    if (variable == "R289_0") { kern::logic::native_set(self.R289_0, value); return true; }
    if (variable == "R289_1") { kern::logic::native_set(self.R289_1, value); return true; }
    if (variable == "R290_0") { kern::logic::native_set(self.R290_0, value); return true; }
    if (variable == "R290_1") { kern::logic::native_set(self.R290_1, value); return true; }
    if (variable == "R291_0") { kern::logic::native_set(self.R291_0, value); return true; }
    if (variable == "R292_0") { kern::logic::native_set(self.R292_0, value); return true; }
    if (variable == "R293_0") { kern::logic::native_set(self.R293_0, value); return true; }
    if (variable == "R294_0") { kern::logic::native_set(self.R294_0, value); return true; }
    if (variable == "R295_0") { kern::logic::native_set(self.R295_0, value); return true; }
    if (variable == "R296_0") { kern::logic::native_set(self.R296_0, value); return true; }
    if (variable == "R297") { kern::logic::native_set(self.R297, value); return true; }
    if (variable == "R298_0") { kern::logic::native_set(self.R298_0, value); return true; }
    if (variable == "R299_0") { kern::logic::native_set(self.R299_0, value); return true; }
    if (variable == "R300_0") { kern::logic::native_set(self.R300_0, value); return true; }
    if (variable == "R300_1") { kern::logic::native_set(self.R300_1, value); return true; }
    if (variable == "R300_2") { kern::logic::native_set(self.R300_2, value); return true; }
    if (variable == "R300_3") { kern::logic::native_set(self.R300_3, value); return true; }
    if (variable == "R301") { kern::logic::native_set(self.R301, value); return true; }
    if (variable == "R302_0") { kern::logic::native_set(self.R302_0, value); return true; }
    if (variable == "R303_0") { kern::logic::native_set(self.R303_0, value); return true; }
    if (variable == "R304_0") { kern::logic::native_set(self.R304_0, value); return true; }
    if (variable == "R305_0") { kern::logic::native_set(self.R305_0, value); return true; }
    if (variable == "R306") { kern::logic::native_set(self.R306, value); return true; }
    if (variable == "In43") { kern::logic::native_set(self.In43, value); return true; }
    if (variable == "R307_0") { kern::logic::native_set(self.R307_0, value); return true; }
    if (variable == "R307_1") { kern::logic::native_set(self.R307_1, value); return true; }
    if (variable == "R308_0") { kern::logic::native_set(self.R308_0, value); return true; }
    if (variable == "R309") { kern::logic::native_set(self.R309, value); return true; }
    if (variable == "R310") { kern::logic::native_set(self.R310, value); return true; }
    if (variable == "R311_0") { kern::logic::native_set(self.R311_0, value); return true; }
    if (variable == "R312_0") { kern::logic::native_set(self.R312_0, value); return true; }
    if (variable == "In44") { kern::logic::native_set(self.In44, value); return true; }
    if (variable == "R313_0") { kern::logic::native_set(self.R313_0, value); return true; }
    if (variable == "R314") { kern::logic::native_set(self.R314, value); return true; }
    if (variable == "R315") { kern::logic::native_set(self.R315, value); return true; }
    if (variable == "R316_0") { kern::logic::native_set(self.R316_0, value); return true; }
    if (variable == "R316_1") { kern::logic::native_set(self.R316_1, value); return true; }
    if (variable == "R316_2") { kern::logic::native_set(self.R316_2, value); return true; }
    if (variable == "R316_3") { kern::logic::native_set(self.R316_3, value); return true; }
    if (variable == "R317") { kern::logic::native_set(self.R317, value); return true; }
    if (variable == "R318") { kern::logic::native_set(self.R318, value); return true; }
    if (variable == "R319") { kern::logic::native_set(self.R319, value); return true; }
    if (variable == "R320_0") { kern::logic::native_set(self.R320_0, value); return true; }
    if (variable == "R321_0") { kern::logic::native_set(self.R321_0, value); return true; }
    if (variable == "R325_0") { kern::logic::native_set(self.R325_0, value); return true; }
    if (variable == "R326") { kern::logic::native_set(self.R326, value); return true; }
    if (variable == "R327") { kern::logic::native_set(self.R327, value); return true; }
    if (variable == "R328") { kern::logic::native_set(self.R328, value); return true; }
    if (variable == "R329") { kern::logic::native_set(self.R329, value); return true; }
    if (variable == "R330_0") { kern::logic::native_set(self.R330_0, value); return true; }
    if (variable == "R332") { kern::logic::native_set(self.R332, value); return true; }
    if (variable == "R334") { kern::logic::native_set(self.R334, value); return true; }
    if (variable == "R338") { kern::logic::native_set(self.R338, value); return true; }
    if (variable == "R341") { kern::logic::native_set(self.R341, value); return true; }
    if (variable == "R342") { kern::logic::native_set(self.R342, value); return true; }
    if (variable == "R343") { kern::logic::native_set(self.R343, value); return true; }
    if (variable == "In45") { kern::logic::native_set(self.In45, value); return true; }
    if (variable == "R344") { kern::logic::native_set(self.R344, value); return true; }
    if (variable == "In46") { kern::logic::native_set(self.In46, value); return true; }
    if (variable == "R345_0") { kern::logic::native_set(self.R345_0, value); return true; }
    if (variable == "R346_0") { kern::logic::native_set(self.R346_0, value); return true; }
    if (variable == "R347") { kern::logic::native_set(self.R347, value); return true; }
    if (variable == "R348_0") { kern::logic::native_set(self.R348_0, value); return true; }
    if (variable == "R348_1") { kern::logic::native_set(self.R348_1, value); return true; }
    if (variable == "R348_2") { kern::logic::native_set(self.R348_2, value); return true; }
    if (variable == "R348_3") { kern::logic::native_set(self.R348_3, value); return true; }
    if (variable == "In47") { kern::logic::native_set(self.In47, value); return true; }
    if (variable == "R349") { kern::logic::native_set(self.R349, value); return true; }
    if (variable == "R350") { kern::logic::native_set(self.R350, value); return true; }
    if (variable == "R351") { kern::logic::native_set(self.R351, value); return true; }
    if (variable == "R352") { kern::logic::native_set(self.R352, value); return true; }
    if (variable == "R355") { kern::logic::native_set(self.R355, value); return true; }
    if (variable == "R357") { kern::logic::native_set(self.R357, value); return true; }
    if (variable == "R358") { kern::logic::native_set(self.R358, value); return true; }
    if (variable == "R359_0") { kern::logic::native_set(self.R359_0, value); return true; }
    if (variable == "R360") { kern::logic::native_set(self.R360, value); return true; }
    if (variable == "R361_0") { kern::logic::native_set(self.R361_0, value); return true; }
    if (variable == "R362") { kern::logic::native_set(self.R362, value); return true; }
    if (variable == "In48") { kern::logic::native_set(self.In48, value); return true; }
    if (variable == "R363") { kern::logic::native_set(self.R363, value); return true; }
    if (variable == "In49") { kern::logic::native_set(self.In49, value); return true; }
    if (variable == "R364_0") { kern::logic::native_set(self.R364_0, value); return true; }
    if (variable == "R365_0") { kern::logic::native_set(self.R365_0, value); return true; }
    if (variable == "R365_1") { kern::logic::native_set(self.R365_1, value); return true; }
    if (variable == "R366") { kern::logic::native_set(self.R366, value); return true; }
    if (variable == "R367") { kern::logic::native_set(self.R367, value); return true; }
    if (variable == "R368") { kern::logic::native_set(self.R368, value); return true; }
    if (variable == "R369") { kern::logic::native_set(self.R369, value); return true; }
    if (variable == "R370") { kern::logic::native_set(self.R370, value); return true; }
    if (variable == "R371") { kern::logic::native_set(self.R371, value); return true; }
    if (variable == "R372") { kern::logic::native_set(self.R372, value); return true; }
    if (variable == "R373") { kern::logic::native_set(self.R373, value); return true; }
    if (variable == "R374") { kern::logic::native_set(self.R374, value); return true; }
    if (variable == "R375_0") { kern::logic::native_set(self.R375_0, value); return true; }
    if (variable == "R376_0") { kern::logic::native_set(self.R376_0, value); return true; }
    if (variable == "R377_0") { kern::logic::native_set(self.R377_0, value); return true; }
    if (variable == "R378") { kern::logic::native_set(self.R378, value); return true; }
    if (variable == "R379") { kern::logic::native_set(self.R379, value); return true; }
    if (variable == "R380_0") { kern::logic::native_set(self.R380_0, value); return true; }
    if (variable == "R380_1") { kern::logic::native_set(self.R380_1, value); return true; }
    if (variable == "R380_2") { kern::logic::native_set(self.R380_2, value); return true; }
    if (variable == "R380_3") { kern::logic::native_set(self.R380_3, value); return true; }
    if (variable == "R381") { kern::logic::native_set(self.R381, value); return true; }
    if (variable == "R382") { kern::logic::native_set(self.R382, value); return true; }
    if (variable == "R384") { kern::logic::native_set(self.R384, value); return true; }
    if (variable == "R385") { kern::logic::native_set(self.R385, value); return true; }
    if (variable == "In50") { kern::logic::native_set(self.In50, value); return true; }
    if (variable == "R386_0") { kern::logic::native_set(self.R386_0, value); return true; }
    if (variable == "R386_1") { kern::logic::native_set(self.R386_1, value); return true; }
    if (variable == "R386_2") { kern::logic::native_set(self.R386_2, value); return true; }
    if (variable == "R386_3") { kern::logic::native_set(self.R386_3, value); return true; }
    if (variable == "R387") { kern::logic::native_set(self.R387, value); return true; }
    if (variable == "R388") { kern::logic::native_set(self.R388, value); return true; }
    if (variable == "R389") { kern::logic::native_set(self.R389, value); return true; }
    if (variable == "R390") { kern::logic::native_set(self.R390, value); return true; }
    if (variable == "R391") { kern::logic::native_set(self.R391, value); return true; }
    if (variable == "R392") { kern::logic::native_set(self.R392, value); return true; }
    if (variable == "R393") { kern::logic::native_set(self.R393, value); return true; }
    if (variable == "R394") { kern::logic::native_set(self.R394, value); return true; }
    if (variable == "In51") { kern::logic::native_set(self.In51, value); return true; }
    if (variable == "R396") { kern::logic::native_set(self.R396, value); return true; }
    if (variable == "R397_0") { kern::logic::native_set(self.R397_0, value); return true; }
    if (variable == "In52") { kern::logic::native_set(self.In52, value); return true; }
    if (variable == "R398_0") { kern::logic::native_set(self.R398_0, value); return true; }
    if (variable == "R399_0") { kern::logic::native_set(self.R399_0, value); return true; }
    if (variable == "R401") { kern::logic::native_set(self.R401, value); return true; }
    if (variable == "R402") { kern::logic::native_set(self.R402, value); return true; }
    if (variable == "R403") { kern::logic::native_set(self.R403, value); return true; }
    if (variable == "R404_0") { kern::logic::native_set(self.R404_0, value); return true; }
    if (variable == "R405_0") { kern::logic::native_set(self.R405_0, value); return true; }
    if (variable == "In53") { kern::logic::native_set(self.In53, value); return true; }
    if (variable == "R407_0") { kern::logic::native_set(self.R407_0, value); return true; }
    if (variable == "R409") { kern::logic::native_set(self.R409, value); return true; }
    if (variable == "R410_0") { kern::logic::native_set(self.R410_0, value); return true; }
    if (variable == "R411_0") { kern::logic::native_set(self.R411_0, value); return true; }
    if (variable == "R412") { kern::logic::native_set(self.R412, value); return true; }
    if (variable == "R413") { kern::logic::native_set(self.R413, value); return true; }
    if (variable == "R414") { kern::logic::native_set(self.R414, value); return true; }
    if (variable == "R416") { kern::logic::native_set(self.R416, value); return true; }
    if (variable == "R417_0") { kern::logic::native_set(self.R417_0, value); return true; }
    if (variable == "In54") { kern::logic::native_set(self.In54, value); return true; }
    if (variable == "In55") { kern::logic::native_set(self.In55, value); return true; }
    if (variable == "In56") { kern::logic::native_set(self.In56, value); return true; }
    if (variable == "R421_0") { kern::logic::native_set(self.R421_0, value); return true; }
    if (variable == "In57") { kern::logic::native_set(self.In57, value); return true; }
    if (variable == "R424") { kern::logic::native_set(self.R424, value); return true; }
    if (variable == "R425") { kern::logic::native_set(self.R425, value); return true; }
    if (variable == "In58") { kern::logic::native_set(self.In58, value); return true; }
    if (variable == "R429") { kern::logic::native_set(self.R429, value); return true; }
    if (variable == "R430_0") { kern::logic::native_set(self.R430_0, value); return true; }
    if (variable == "R431") { kern::logic::native_set(self.R431, value); return true; }
    if (variable == "R432") { kern::logic::native_set(self.R432, value); return true; }
    if (variable == "R433") { kern::logic::native_set(self.R433, value); return true; }
    if (variable == "R435") { kern::logic::native_set(self.R435, value); return true; }
    if (variable == "R436") { kern::logic::native_set(self.R436, value); return true; }
    if (variable == "R437_0") { kern::logic::native_set(self.R437_0, value); return true; }
    if (variable == "R439") { kern::logic::native_set(self.R439, value); return true; }
    if (variable == "R440") { kern::logic::native_set(self.R440, value); return true; }
    if (variable == "R441") { kern::logic::native_set(self.R441, value); return true; }
    if (variable == "In59") { kern::logic::native_set(self.In59, value); return true; }
    if (variable == "R453_0") { kern::logic::native_set(self.R453_0, value); return true; }
    if (variable == "R455_0") { kern::logic::native_set(self.R455_0, value); return true; }
    if (variable == "R456") { kern::logic::native_set(self.R456, value); return true; }
    if (variable == "R457") { kern::logic::native_set(self.R457, value); return true; }
    if (variable == "R458_0") { kern::logic::native_set(self.R458_0, value); return true; }
    if (variable == "R460") { kern::logic::native_set(self.R460, value); return true; }
    if (variable == "In60") { kern::logic::native_set(self.In60, value); return true; }
    if (variable == "R461_0") { kern::logic::native_set(self.R461_0, value); return true; }
    if (variable == "R462_0") { kern::logic::native_set(self.R462_0, value); return true; }
    if (variable == "R462_1") { kern::logic::native_set(self.R462_1, value); return true; }
    if (variable == "R463_0") { kern::logic::native_set(self.R463_0, value); return true; }
    if (variable == "R464_0") { kern::logic::native_set(self.R464_0, value); return true; }
    if (variable == "R465_0") { kern::logic::native_set(self.R465_0, value); return true; }
    if (variable == "In61") { kern::logic::native_set(self.In61, value); return true; }
    if (variable == "R466_0") { kern::logic::native_set(self.R466_0, value); return true; }
    if (variable == "R467_0") { kern::logic::native_set(self.R467_0, value); return true; }
    if (variable == "R468") { kern::logic::native_set(self.R468, value); return true; }
    if (variable == "R469") { kern::logic::native_set(self.R469, value); return true; }
    if (variable == "R470") { kern::logic::native_set(self.R470, value); return true; }
    if (variable == "R471_0") { kern::logic::native_set(self.R471_0, value); return true; }
    if (variable == "R472_0") { kern::logic::native_set(self.R472_0, value); return true; }
    if (variable == "R473_0") { kern::logic::native_set(self.R473_0, value); return true; }
    if (variable == "R473_1") { kern::logic::native_set(self.R473_1, value); return true; }
    if (variable == "R474_0") { kern::logic::native_set(self.R474_0, value); return true; }
    if (variable == "R475") { kern::logic::native_set(self.R475, value); return true; }
    if (variable == "R477") { kern::logic::native_set(self.R477, value); return true; }
    if (variable == "In62") { kern::logic::native_set(self.In62, value); return true; }
    if (variable == "R479") { kern::logic::native_set(self.R479, value); return true; }
    if (variable == "In63") { kern::logic::native_set(self.In63, value); return true; }
    if (variable == "R481_0") { kern::logic::native_set(self.R481_0, value); return true; }
    if (variable == "R482") { kern::logic::native_set(self.R482, value); return true; }
    if (variable == "In64") { kern::logic::native_set(self.In64, value); return true; }
    if (variable == "R483_0") { kern::logic::native_set(self.R483_0, value); return true; }
    if (variable == "R488") { kern::logic::native_set(self.R488, value); return true; }
    if (variable == "R489") { kern::logic::native_set(self.R489, value); return true; }
    if (variable == "R491") { kern::logic::native_set(self.R491, value); return true; }
    if (variable == "In65") { kern::logic::native_set(self.In65, value); return true; }
    if (variable == "In66") { kern::logic::native_set(self.In66, value); return true; }
    if (variable == "R492_0") { kern::logic::native_set(self.R492_0, value); return true; }
    if (variable == "R492_1") { kern::logic::native_set(self.R492_1, value); return true; }
    if (variable == "R493") { kern::logic::native_set(self.R493, value); return true; }
    if (variable == "In67") { kern::logic::native_set(self.In67, value); return true; }
    if (variable == "R494_0") { kern::logic::native_set(self.R494_0, value); return true; }
    if (variable == "R494_1") { kern::logic::native_set(self.R494_1, value); return true; }
    if (variable == "In68") { kern::logic::native_set(self.In68, value); return true; }
    if (variable == "In69") { kern::logic::native_set(self.In69, value); return true; }
    if (variable == "R495_0") { kern::logic::native_set(self.R495_0, value); return true; }
    if (variable == "R495_1") { kern::logic::native_set(self.R495_1, value); return true; }
    if (variable == "R495_2") { kern::logic::native_set(self.R495_2, value); return true; }
    if (variable == "R496_0") { kern::logic::native_set(self.R496_0, value); return true; }
    if (variable == "R496_1") { kern::logic::native_set(self.R496_1, value); return true; }
    if (variable == "R496_2") { kern::logic::native_set(self.R496_2, value); return true; }
    if (variable == "R498_0") { kern::logic::native_set(self.R498_0, value); return true; }
    if (variable == "R499") { kern::logic::native_set(self.R499, value); return true; }
    if (variable == "R501") { kern::logic::native_set(self.R501, value); return true; }
    if (variable == "R502") { kern::logic::native_set(self.R502, value); return true; }
    if (variable == "R503") { kern::logic::native_set(self.R503, value); return true; }
    if (variable == "R504") { kern::logic::native_set(self.R504, value); return true; }
    if (variable == "R505") { kern::logic::native_set(self.R505, value); return true; }
    if (variable == "R506") { kern::logic::native_set(self.R506, value); return true; }
    if (variable == "R507") { kern::logic::native_set(self.R507, value); return true; }
    if (variable == "R508") { kern::logic::native_set(self.R508, value); return true; }
    if (variable == "R509") { kern::logic::native_set(self.R509, value); return true; }
    if (variable == "R513") { kern::logic::native_set(self.R513, value); return true; }
    if (variable == "In70") { kern::logic::native_set(self.In70, value); return true; }
    if (variable == "R514") { kern::logic::native_set(self.R514, value); return true; }
    if (variable == "In71") { kern::logic::native_set(self.In71, value); return true; }
    if (variable == "R517") { kern::logic::native_set(self.R517, value); return true; }
    if (variable == "R518") { kern::logic::native_set(self.R518, value); return true; }
    if (variable == "R519") { kern::logic::native_set(self.R519, value); return true; }
    if (variable == "R520") { kern::logic::native_set(self.R520, value); return true; }
    if (variable == "R524_0") { kern::logic::native_set(self.R524_0, value); return true; }
    if (variable == "R525_0") { kern::logic::native_set(self.R525_0, value); return true; }
    if (variable == "R526_0") { kern::logic::native_set(self.R526_0, value); return true; }
    if (variable == "R527") { kern::logic::native_set(self.R527, value); return true; }
    if (variable == "R528") { kern::logic::native_set(self.R528, value); return true; }
    if (variable == "R531") { kern::logic::native_set(self.R531, value); return true; }
    if (variable == "R532") { kern::logic::native_set(self.R532, value); return true; }
    if (variable == "R533") { kern::logic::native_set(self.R533, value); return true; }
    if (variable == "R534") { kern::logic::native_set(self.R534, value); return true; }
    if (variable == "R535") { kern::logic::native_set(self.R535, value); return true; }
    if (variable == "R540_0") { kern::logic::native_set(self.R540_0, value); return true; }
    if (variable == "R541_0") { kern::logic::native_set(self.R541_0, value); return true; }
    if (variable == "R542_0") { kern::logic::native_set(self.R542_0, value); return true; }
    if (variable == "R543_0") { kern::logic::native_set(self.R543_0, value); return true; }
    if (variable == "In72") { kern::logic::native_set(self.In72, value); return true; }
    if (variable == "R544") { kern::logic::native_set(self.R544, value); return true; }
    if (variable == "R546") { kern::logic::native_set(self.R546, value); return true; }
    if (variable == "R547_0") { kern::logic::native_set(self.R547_0, value); return true; }
    if (variable == "R548_0") { kern::logic::native_set(self.R548_0, value); return true; }
    if (variable == "R552") { kern::logic::native_set(self.R552, value); return true; }
    if (variable == "R553") { kern::logic::native_set(self.R553, value); return true; }
    if (variable == "R554_0") { kern::logic::native_set(self.R554_0, value); return true; }
    if (variable == "R555") { kern::logic::native_set(self.R555, value); return true; }
    if (variable == "R556") { kern::logic::native_set(self.R556, value); return true; }
    if (variable == "R558") { kern::logic::native_set(self.R558, value); return true; }
    if (variable == "R559_0") { kern::logic::native_set(self.R559_0, value); return true; }
    if (variable == "R561") { kern::logic::native_set(self.R561, value); return true; }
    if (variable == "R564") { kern::logic::native_set(self.R564, value); return true; }
    if (variable == "R565") { kern::logic::native_set(self.R565, value); return true; }
    if (variable == "R566") { kern::logic::native_set(self.R566, value); return true; }
    if (variable == "R571") { kern::logic::native_set(self.R571, value); return true; }
    if (variable == "R572") { kern::logic::native_set(self.R572, value); return true; }
    if (variable == "R573") { kern::logic::native_set(self.R573, value); return true; }
    if (variable == "R574") { kern::logic::native_set(self.R574, value); return true; }
    if (variable == "R575") { kern::logic::native_set(self.R575, value); return true; }
    if (variable == "R576") { kern::logic::native_set(self.R576, value); return true; }
    if (variable == "R577_0") { kern::logic::native_set(self.R577_0, value); return true; }
    if (variable == "R578_0") { kern::logic::native_set(self.R578_0, value); return true; }
    if (variable == "R579_0") { kern::logic::native_set(self.R579_0, value); return true; }
    if (variable == "R580") { kern::logic::native_set(self.R580, value); return true; }
    if (variable == "R581") { kern::logic::native_set(self.R581, value); return true; }
    if (variable == "R582") { kern::logic::native_set(self.R582, value); return true; }
    if (variable == "R583") { kern::logic::native_set(self.R583, value); return true; }
    if (variable == "R584_0") { kern::logic::native_set(self.R584_0, value); return true; }
    if (variable == "R585_0") { kern::logic::native_set(self.R585_0, value); return true; }
    if (variable == "R586_0") { kern::logic::native_set(self.R586_0, value); return true; }
    if (variable == "R587_0") { kern::logic::native_set(self.R587_0, value); return true; }
    if (variable == "R588_0") { kern::logic::native_set(self.R588_0, value); return true; }
    if (variable == "R590") { kern::logic::native_set(self.R590, value); return true; }
    if (variable == "R591") { kern::logic::native_set(self.R591, value); return true; }
    if (variable == "R592") { kern::logic::native_set(self.R592, value); return true; }
    if (variable == "R593") { kern::logic::native_set(self.R593, value); return true; }
    if (variable == "R594_0") { kern::logic::native_set(self.R594_0, value); return true; }
    if (variable == "R594_1") { kern::logic::native_set(self.R594_1, value); return true; }
    if (variable == "R595_0") { kern::logic::native_set(self.R595_0, value); return true; }
    if (variable == "R595_1") { kern::logic::native_set(self.R595_1, value); return true; }
    if (variable == "R596_0") { kern::logic::native_set(self.R596_0, value); return true; }
    if (variable == "R596_1") { kern::logic::native_set(self.R596_1, value); return true; }
    if (variable == "R597_0") { kern::logic::native_set(self.R597_0, value); return true; }
    if (variable == "R598_0") { kern::logic::native_set(self.R598_0, value); return true; }
    if (variable == "R599") { kern::logic::native_set(self.R599, value); return true; }
    if (variable == "R600") { kern::logic::native_set(self.R600, value); return true; }
    if (variable == "R601") { kern::logic::native_set(self.R601, value); return true; }
    if (variable == "R602") { kern::logic::native_set(self.R602, value); return true; }
    if (variable == "R603_0") { kern::logic::native_set(self.R603_0, value); return true; }
    if (variable == "R604_0") { kern::logic::native_set(self.R604_0, value); return true; }
    if (variable == "R605_0") { kern::logic::native_set(self.R605_0, value); return true; }
    if (variable == "R606_0") { kern::logic::native_set(self.R606_0, value); return true; }
    if (variable == "R607_0") { kern::logic::native_set(self.R607_0, value); return true; }
    if (variable == "R608") { kern::logic::native_set(self.R608, value); return true; }
    if (variable == "R609") { kern::logic::native_set(self.R609, value); return true; }
    if (variable == "R610") { kern::logic::native_set(self.R610, value); return true; }
    if (variable == "R611") { kern::logic::native_set(self.R611, value); return true; }
    if (variable == "R612_0") { kern::logic::native_set(self.R612_0, value); return true; }
    if (variable == "R613_0") { kern::logic::native_set(self.R613_0, value); return true; }
    if (variable == "R613_1") { kern::logic::native_set(self.R613_1, value); return true; }
    if (variable == "R613_2") { kern::logic::native_set(self.R613_2, value); return true; }
    if (variable == "In73") { kern::logic::native_set(self.In73, value); return true; }
    if (variable == "R616_0") { kern::logic::native_set(self.R616_0, value); return true; }
    if (variable == "R618") { kern::logic::native_set(self.R618, value); return true; }
    if (variable == "R619") { kern::logic::native_set(self.R619, value); return true; }
    if (variable == "R620") { kern::logic::native_set(self.R620, value); return true; }
    if (variable == "R621_0") { kern::logic::native_set(self.R621_0, value); return true; }
    if (variable == "R622_0") { kern::logic::native_set(self.R622_0, value); return true; }
    if (variable == "R624") { kern::logic::native_set(self.R624, value); return true; }
    if (variable == "R625") { kern::logic::native_set(self.R625, value); return true; }
    if (variable == "R626") { kern::logic::native_set(self.R626, value); return true; }
    if (variable == "R628_0") { kern::logic::native_set(self.R628_0, value); return true; }
    if (variable == "R629_0") { kern::logic::native_set(self.R629_0, value); return true; }
    if (variable == "R629_1") { kern::logic::native_set(self.R629_1, value); return true; }
    if (variable == "R629_2") { kern::logic::native_set(self.R629_2, value); return true; }
    if (variable == "R632_0") { kern::logic::native_set(self.R632_0, value); return true; }
    if (variable == "R632_1") { kern::logic::native_set(self.R632_1, value); return true; }
    if (variable == "R632_2") { kern::logic::native_set(self.R632_2, value); return true; }
    if (variable == "R633_0") { kern::logic::native_set(self.R633_0, value); return true; }
    if (variable == "In74") { kern::logic::native_set(self.In74, value); return true; }
    if (variable == "R634") { kern::logic::native_set(self.R634, value); return true; }
    if (variable == "R635_0") { kern::logic::native_set(self.R635_0, value); return true; }
    if (variable == "R635_1") { kern::logic::native_set(self.R635_1, value); return true; }
    if (variable == "R636") { kern::logic::native_set(self.R636, value); return true; }
    if (variable == "R637") { kern::logic::native_set(self.R637, value); return true; }
    if (variable == "R638") { kern::logic::native_set(self.R638, value); return true; }
    if (variable == "In75") { kern::logic::native_set(self.In75, value); return true; }
    if (variable == "R639_0") { kern::logic::native_set(self.R639_0, value); return true; }
    if (variable == "R641") { kern::logic::native_set(self.R641, value); return true; }
    if (variable == "In76") { kern::logic::native_set(self.In76, value); return true; }
    if (variable == "R642_0") { kern::logic::native_set(self.R642_0, value); return true; }
    if (variable == "In77") { kern::logic::native_set(self.In77, value); return true; }
    if (variable == "R644") { kern::logic::native_set(self.R644, value); return true; }
    if (variable == "In78") { kern::logic::native_set(self.In78, value); return true; }
    if (variable == "R645_0") { kern::logic::native_set(self.R645_0, value); return true; }
    if (variable == "In79") { kern::logic::native_set(self.In79, value); return true; }
    if (variable == "R647") { kern::logic::native_set(self.R647, value); return true; }
    if (variable == "In80") { kern::logic::native_set(self.In80, value); return true; }
    if (variable == "R648_0") { kern::logic::native_set(self.R648_0, value); return true; }
    if (variable == "In81") { kern::logic::native_set(self.In81, value); return true; }
    if (variable == "R650") { kern::logic::native_set(self.R650, value); return true; }
    if (variable == "R652") { kern::logic::native_set(self.R652, value); return true; }
    if (variable == "In82") { kern::logic::native_set(self.In82, value); return true; }
    if (variable == "R655") { kern::logic::native_set(self.R655, value); return true; }
    if (variable == "R656") { kern::logic::native_set(self.R656, value); return true; }
    if (variable == "R666") { kern::logic::native_set(self.R666, value); return true; }
    if (variable == "R667") { kern::logic::native_set(self.R667, value); return true; }
    if (variable == "R668") { kern::logic::native_set(self.R668, value); return true; }
    if (variable == "In83") { kern::logic::native_set(self.In83, value); return true; }
    if (variable == "R670_0") { kern::logic::native_set(self.R670_0, value); return true; }
    if (variable == "R672") { kern::logic::native_set(self.R672, value); return true; }
    if (variable == "R673") { kern::logic::native_set(self.R673, value); return true; }
    if (variable == "R674") { kern::logic::native_set(self.R674, value); return true; }
    if (variable == "R675") { kern::logic::native_set(self.R675, value); return true; }
    if (variable == "R676") { kern::logic::native_set(self.R676, value); return true; }
    if (variable == "R677") { kern::logic::native_set(self.R677, value); return true; }
    if (variable == "In84") { kern::logic::native_set(self.In84, value); return true; }
    if (variable == "R682") { kern::logic::native_set(self.R682, value); return true; }
    if (variable == "R685") { kern::logic::native_set(self.R685, value); return true; }
    if (variable == "R686") { kern::logic::native_set(self.R686, value); return true; }
    if (variable == "R687") { kern::logic::native_set(self.R687, value); return true; }
    if (variable == "R689") { kern::logic::native_set(self.R689, value); return true; }
    if (variable == "R690") { kern::logic::native_set(self.R690, value); return true; }
    if (variable == "In85") { kern::logic::native_set(self.In85, value); return true; }
    if (variable == "R691_0") { kern::logic::native_set(self.R691_0, value); return true; }
    if (variable == "In86") { kern::logic::native_set(self.In86, value); return true; }
    if (variable == "R692_0") { kern::logic::native_set(self.R692_0, value); return true; }
    if (variable == "R693") { kern::logic::native_set(self.R693, value); return true; }
    if (variable == "In87") { kern::logic::native_set(self.In87, value); return true; }
    if (variable == "R694") { kern::logic::native_set(self.R694, value); return true; }
    if (variable == "R695") { kern::logic::native_set(self.R695, value); return true; }
    if (variable == "In88") { kern::logic::native_set(self.In88, value); return true; }
    if (variable == "R696") { kern::logic::native_set(self.R696, value); return true; }
    if (variable == "In89") { kern::logic::native_set(self.In89, value); return true; }
    if (variable == "R697_0") { kern::logic::native_set(self.R697_0, value); return true; }
    if (variable == "R700") { kern::logic::native_set(self.R700, value); return true; }
    if (variable == "R701") { kern::logic::native_set(self.R701, value); return true; }
    if (variable == "R702") { kern::logic::native_set(self.R702, value); return true; }
    if (variable == "R703") { kern::logic::native_set(self.R703, value); return true; }
    if (variable == "R704") { kern::logic::native_set(self.R704, value); return true; }
    if (variable == "R705") { kern::logic::native_set(self.R705, value); return true; }
    if (variable == "R706") { kern::logic::native_set(self.R706, value); return true; }
    if (variable == "R707") { kern::logic::native_set(self.R707, value); return true; }
    if (variable == "R708") { kern::logic::native_set(self.R708, value); return true; }
    if (variable == "R709") { kern::logic::native_set(self.R709, value); return true; }
    if (variable == "R710_0") { kern::logic::native_set(self.R710_0, value); return true; }
    if (variable == "R712") { kern::logic::native_set(self.R712, value); return true; }
    if (variable == "R715") { kern::logic::native_set(self.R715, value); return true; }
    if (variable == "R716") { kern::logic::native_set(self.R716, value); return true; }
    if (variable == "R717") { kern::logic::native_set(self.R717, value); return true; }
    if (variable == "R718") { kern::logic::native_set(self.R718, value); return true; }
    if (variable == "R720_0") { kern::logic::native_set(self.R720_0, value); return true; }
    if (variable == "R721") { kern::logic::native_set(self.R721, value); return true; }
    if (variable == "R722") { kern::logic::native_set(self.R722, value); return true; }
    if (variable == "R725") { kern::logic::native_set(self.R725, value); return true; }
    if (variable == "R726") { kern::logic::native_set(self.R726, value); return true; }
    if (variable == "R727") { kern::logic::native_set(self.R727, value); return true; }
    if (variable == "R728_0") { kern::logic::native_set(self.R728_0, value); return true; }
    if (variable == "R728_1") { kern::logic::native_set(self.R728_1, value); return true; }
    if (variable == "R730") { kern::logic::native_set(self.R730, value); return true; }
    if (variable == "R731") { kern::logic::native_set(self.R731, value); return true; }
    if (variable == "R732_0") { kern::logic::native_set(self.R732_0, value); return true; }
    if (variable == "R732_1") { kern::logic::native_set(self.R732_1, value); return true; }
    if (variable == "R733_0") { kern::logic::native_set(self.R733_0, value); return true; }
    if (variable == "R733_1") { kern::logic::native_set(self.R733_1, value); return true; }
    if (variable == "R733_2") { kern::logic::native_set(self.R733_2, value); return true; }
    if (variable == "R734_0") { kern::logic::native_set(self.R734_0, value); return true; }
    if (variable == "R736_0") { kern::logic::native_set(self.R736_0, value); return true; }
    if (variable == "R738_0") { kern::logic::native_set(self.R738_0, value); return true; }
    if (variable == "R740_0") { kern::logic::native_set(self.R740_0, value); return true; }
    if (variable == "R742_0") { kern::logic::native_set(self.R742_0, value); return true; }
    if (variable == "R744_0") { kern::logic::native_set(self.R744_0, value); return true; }
    if (variable == "R746_0") { kern::logic::native_set(self.R746_0, value); return true; }
    if (variable == "R748_0") { kern::logic::native_set(self.R748_0, value); return true; }
    if (variable == "R750_0") { kern::logic::native_set(self.R750_0, value); return true; }
    if (variable == "R752_0") { kern::logic::native_set(self.R752_0, value); return true; }
    if (variable == "R754_0") { kern::logic::native_set(self.R754_0, value); return true; }
    if (variable == "R756_0") { kern::logic::native_set(self.R756_0, value); return true; }
    if (variable == "R758_0") { kern::logic::native_set(self.R758_0, value); return true; }
    if (variable == "R760_0") { kern::logic::native_set(self.R760_0, value); return true; }
    if (variable == "R762_0") { kern::logic::native_set(self.R762_0, value); return true; }
    if (variable == "R764_0") { kern::logic::native_set(self.R764_0, value); return true; }
    if (variable == "R766_0") { kern::logic::native_set(self.R766_0, value); return true; }
    if (variable == "R768_0") { kern::logic::native_set(self.R768_0, value); return true; }
    if (variable == "R770_0") { kern::logic::native_set(self.R770_0, value); return true; }
    if (variable == "R772_0") { kern::logic::native_set(self.R772_0, value); return true; }
    if (variable == "R774_0") { kern::logic::native_set(self.R774_0, value); return true; }
    if (variable == "R776_0") { kern::logic::native_set(self.R776_0, value); return true; }
    if (variable == "R778_0") { kern::logic::native_set(self.R778_0, value); return true; }
    if (variable == "R780_0") { kern::logic::native_set(self.R780_0, value); return true; }
    if (variable == "R782_0") { kern::logic::native_set(self.R782_0, value); return true; }
    if (variable == "R784_0") { kern::logic::native_set(self.R784_0, value); return true; }
    if (variable == "R786_0") { kern::logic::native_set(self.R786_0, value); return true; }
    if (variable == "R788_0") { kern::logic::native_set(self.R788_0, value); return true; }
    if (variable == "R790_0") { kern::logic::native_set(self.R790_0, value); return true; }
    if (variable == "R792_0") { kern::logic::native_set(self.R792_0, value); return true; }
    if (variable == "R794_0") { kern::logic::native_set(self.R794_0, value); return true; }
    if (variable == "R796_0") { kern::logic::native_set(self.R796_0, value); return true; }
    if (variable == "In90") { kern::logic::native_set(self.In90, value); return true; }
    if (variable == "R798_0") { kern::logic::native_set(self.R798_0, value); return true; }
    if (variable == "In91") { kern::logic::native_set(self.In91, value); return true; }
    if (variable == "R800_0") { kern::logic::native_set(self.R800_0, value); return true; }
    if (variable == "R802_0") { kern::logic::native_set(self.R802_0, value); return true; }
    if (variable == "R804_0") { kern::logic::native_set(self.R804_0, value); return true; }
    if (variable == "R806_0") { kern::logic::native_set(self.R806_0, value); return true; }
    if (variable == "R808_0") { kern::logic::native_set(self.R808_0, value); return true; }
    if (variable == "R810_0") { kern::logic::native_set(self.R810_0, value); return true; }
    if (variable == "R812_0") { kern::logic::native_set(self.R812_0, value); return true; }
    if (variable == "R814_0") { kern::logic::native_set(self.R814_0, value); return true; }
    if (variable == "R816_0") { kern::logic::native_set(self.R816_0, value); return true; }
    if (variable == "H348") { kern::logic::native_set(self.H348, value); return true; }
    if (variable == "P348_0") { kern::logic::native_set(self.P348_0, value); return true; }
    if (variable == "P348_1") { kern::logic::native_set(self.P348_1, value); return true; }
    if (variable == "P348_2") { kern::logic::native_set(self.P348_2, value); return true; }
    if (variable == "P348_3") { kern::logic::native_set(self.P348_3, value); return true; }
    if (variable == "P348_4") { kern::logic::native_set(self.P348_4, value); return true; }
    if (variable == "In92") { kern::logic::native_set(self.In92, value); return true; }
    if (variable == "H380") { kern::logic::native_set(self.H380, value); return true; }
    if (variable == "P380_0") { kern::logic::native_set(self.P380_0, value); return true; }
    if (variable == "P380_1") { kern::logic::native_set(self.P380_1, value); return true; }
    if (variable == "P380_2") { kern::logic::native_set(self.P380_2, value); return true; }
    if (variable == "P380_3") { kern::logic::native_set(self.P380_3, value); return true; }
    if (variable == "P380_4") { kern::logic::native_set(self.P380_4, value); return true; }
    return false;
  }
};
[[maybe_unused]] const bool Engine_registered =
    (kern::logic::register_native_blueprint(0x18d0913e16609fd9ull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new Engine_Native()); }), true);
}  // namespace
