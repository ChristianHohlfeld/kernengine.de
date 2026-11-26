// SpawnCity on Kern, the small game (see spiel.h).
#include "spiel.h"

#include <cmath>
#include <cstdio>
#include <cstdlib>

#include "blueprint_call.h"  // (rules::kLoopBudget: the guard the rules' loops run under)
#include "kern/core/base.h"
#include "tiles.h"

namespace sc {

using kern::Key;
using kern::game::ActorId;

namespace {

constexpr f64 kStepHz = 240;   // fixed physics rate: the firmware runs 4 ticks per step (~1 kHz); the level's world.stepHz
constexpr u32 kMaxSteps = 24;  // at most 0.1 s of simulation per frame (GameWorld::step's guard against the spiral of death)

f32 key(const kern::Input& in, Key k) { return in.down(k) ? 1.0f : 0.0f; }

std::string json_escape(const std::string& s) {
  std::string o;
  for (char c : s) {
    if (c == '"' || c == '\\') o += '\\';
    if (static_cast<unsigned char>(c) < 0x20) continue;
    o += c;
  }
  return o;
}

}  // namespace

bool pad_down(const PadState& p, int b) { return (p.buttons & (1u << b)) != 0; }
bool pad_hit(const PadState& p, int b) { return (p.pressed & (1u << b)) != 0; }

SpawnCity& game() {
  static SpawnCity g;
  return g;
}

kern::Span<kern::ShaderExtension* const> SpawnCity::extensions() {
  extensions_[0] = &facades_;
  extensions_[1] = &water_;
  return {extensions_, 2};
}

// ---------------------------------------------------------------------------------------------
// start and stop

bool SpawnCity::start(kern::Engine& e) {
  engine_ = &e;
  // (a start again begins as the first: the game is one object for the whole process)
  placed_ = paused_ = touch_mode_ = locked_ = web_audio_ = false;
  steps_ = 0;
  accumulator_ = 0;
  applied_ = 0;
  resets_ = moves_ = 0;
  cam_mode_ = 0;
  mouse_px_ = {};
  pad = PadState{};
  pad_was_ = 0;
  fps_ = 60;
  toast_.clear();
  toast_age_ = 99;
  mobile_ = e.arg_number("mobile", 0) > 0;
  lat_ = e.arg_number("lat", kKonstanzLat);
  lon_ = e.arg_number("lon", kKonstanzLon);
  city_.building_budget = mobile_ ? 2600 : 4000;  // world_city_buildings.mjs MAX_FOOTPRINTS 2600 (a desktop draws more of the city)
  // The world of the program: its level is the project's start level (level/spawncity.klevel): the world's settings
  // (the 240 Hz step, the fog, no ground plane - the terrain's tiles are the ground), the rules that are Blueprints
  // (Controls, Settings) and the drone as a template. No player: the drone flies in a simulated world.
  world_ = std::make_unique<kern::game::GameWorld>(e);
  if (!world_->start_level("spawncity.klevel", kern::game::SimMode::simulate)) return kern::fail("level start: %s", kern::last_error());
  if (static_cast<f64>(world_->level().world.step_hz) != kStepHz)
    return kern::fail("SpawnCity: the level steps at %g Hz, the drone's fixed step is %g Hz (level/spawncity.klevel world.stepHz)", world_->level().world.step_hz, kStepHz);
  kern::logic::Runtime* logic = world_->logic_runtime();
  if (!logic || !controls::bind(*logic)) return kern::fail("the level's Blueprint \"Controls\" is missing or has errors");
  if (!bind_settings(*logic, settings_table_)) return kern::fail("the level's Blueprint \"Settings\" is missing, has errors or not one row per setting");
  settings_ = Settings(settings_table_);  // every setting at the table's default (the page replays the player's own with "set")
  logic->budget = rules::kLoopBudget;
  // The drone: a copy of the level's template, parked far below the world until the ground under the start is there.
  drone_actor_ = world_->spawn("Drone", kern::game::Transform{{0, 0, -200}, {0, 0, 0}, {1, 1, 1}}).id;
  if (!drone_actor_) return kern::fail("SpawnCity: the level's template \"Drone\": %s (tools/level_templates.sh puts it in)", kern::last_error());
  world_->set_alive(drone_actor_, false);
  world_->set_paused(true);
  // Without map data the original plays its training level; ?offline=1 (the tests) keeps the bigger stand-in town,
  // ?world=training / ?world=town choose. Elevation from the Terrarium tiles unless offline or ?dem=0.
  const std::string wsel = e.arg("world", "");
  city_.training_offline = wsel == "training" || (wsel != "town" && e.arg_number("offline", 0) <= 0);
  city_.start(e.renderer(), world_->physics(), lon_, lat_, e.arg_number("dem", 1) > 0 && e.arg_number("offline", 0) <= 0);
  if (e.arg_number("offline", 0) > 0) city_.go_offline();
  if (!art_.init(e.renderer())) return false;
  mouse_ = controls::RateStick{1500, 0.05f, {}};
  e.set_status("SpawnCity: Karte wird geladen ...");
  cam_.eye = {-30, -40, 60};
  cam_.target = {0, 0, 0};
  e.renderer().set_camera(cam_);
  return true;
}

void SpawnCity::stop(kern::Engine&) {
  if (game_audio_) game_audio_->end();
  game_audio_.reset();
  bank_.reset();
  city_.clear();
  world_.reset();
  placed_ = false;
}

kern::Mat4 SpawnCity::drone_matrix() const {
  kern::Mat4 m;
  if (world_) world_->actor_matrix(drone_actor_, m);
  return m;
}

Vec3 SpawnCity::drone_position() const {
  if (!world_ || !drone_actor_) return {};
  return read_body(world_->physics(), world_->actor_body(drone_actor_)).position;
}

// The drone on the ground at the start: the first spot round the origin that is no building, on what is there.
bool SpawnCity::place() {
  Vec3 at{0, 0, 0};
  bool found = false;
  for (i32 ring = 0; ring < 40 && !found; ++ring) {
    const i32 n = ring == 0 ? 1 : ring * 8;
    for (i32 k = 0; k < n && !found; ++k) {
      const f32 a = static_cast<f32>(k) * 2 * kPi / static_cast<f32>(n), r = static_cast<f32>(ring) * 5;
      const Vec2 p{r * std::cos(a), r * std::sin(a)};
      if (!city_.building_at(p)) { at = {p.x, p.y, 0}; found = true; }
    }
  }
  const f32 ground = city_.ground_height(at.x, at.y);
  const RayResult g = cast(world_->physics(), {at.x, at.y, ground + 60}, {0, 0, -1}, 80, world_->actor_body(drone_actor_));
  at.z = (g.hit ? g.point.z : ground) + 0.0225f;
  world_->set_alive(drone_actor_, true);
  const Vec3 rot{0, 0, 180};  // the nose (-X) looks along +X
  world_->teleport(drone_actor_, at, &rot);
  world_->set_actor_velocity(drone_actor_, {});
  drone_.reset();
  sticks_ = DroneSticks{};
  sticks_.clearance = settings_.get(Opt::drone_hover_agl);
  landing_ = false;
  height_held_ = false;
  accumulator_ = 0;
  rig_init_ = false;
  focus_prev_valid_ = false;
  placed_ = true;
  world_->set_paused(false);
  engine_->set_status("");
  toast_ = city_.offline() ? "" : "Willkommen in SpawnCity";
  toast_age_ = city_.offline() ? 99 : 0;
  return true;
}

// RESET (game_reset.mjs): the drone back at the start, on the ground, disarmed.
void SpawnCity::reset() {
  if (!placed_) return;
  place();
  ++resets_;
  toast_ = "Zurueck zum Start";
  toast_age_ = 0;
}

// The world moves to another place: the city streams there and the drone starts anew.
void SpawnCity::move_world(f64 lon, f64 lat) {
  lon_ = lon;
  lat_ = lat;
  city_.clear();
  city_.start(engine_->renderer(), world_->physics(), lon_, lat_, engine_->arg_number("dem", 1) > 0 && engine_->arg_number("offline", 0) <= 0);
  if (engine_->arg_number("offline", 0) > 0) city_.go_offline();
  world_->set_alive(drone_actor_, false);
  world_->set_paused(true);
  placed_ = false;
  ++moves_;
  toast_ = "Neuer Startort - die Stadt wird geladen";
  toast_age_ = 0;
}

void SpawnCity::apply_settings() {
  if (applied_ == 0 && engine_ && engine_->has_arg("fpvtilt")) settings_.set(settings_table_, Opt::cam_fpv_tilt, static_cast<f32>(engine_->arg_number("fpvtilt", -15)));
  applied_ = settings_.version();
}

// ---------------------------------------------------------------------------------------------
// per frame

void SpawnCity::update(kern::Engine& e, const kern::Frame& f) {
  const f32 dt = f.dt;
  time_ = static_cast<f32>(f.time);
  if (dt > 0) fps_ = fps_ * 0.95f + (1.0f / dt) * 0.05f;
  toast_age_ += dt;
  apply_settings();
  if (!placed_) {
    city_.update({0, 0, 0}, 24);  // loading behind the title: nothing to keep smooth
    ground_changes_.clear();
    city_.take_ground_changes(ground_changes_);
    if (city_.ready_at({0, 0, 0})) place();
    else {
      char buf[128];
      std::snprintf(buf, sizeof buf, "SpawnCity: %s ... %u Gebaeude", city_.offline() ? "Ersatzstadt wird gebaut" : "die Stadt wird geladen", city_.stats().buildings);
      e.set_status(buf);
    }
  }
  if (placed_ && !paused_) {
    read_input(e, dt);
    fly(dt);
    const Vec3 focus = drone_position();
    city_.anchor = focus;  // ground collision stays under the drone
    // the elevation ahead of a fast flight (terrain.h lead), the city ahead of it (4 s of the way, at most 420 m)
    if (dt > 0 && focus_prev_valid_) {
      Vec3 v = (focus - focus_prev_) * (1.0f / dt);
      v.z = 0;
      const f32 l = kern::length(v);
      city_.terrain().lead = l > 15 ? v * (std::fmin(l * 12, 2500.0f) / l) : Vec3{};
      city_.view_lead = l > 6 ? v * (std::fmin(l * 4, 420.0f) / l) : Vec3{};
    }
    focus_prev_ = focus;
    focus_prev_valid_ = true;
    city_.update(focus, mobile_ ? 3 : 4);
    ground_changes_.clear();
    city_.take_ground_changes(ground_changes_);
  }
  pad_was_ = pad.buttons;
  update_camera(e, dt);
  e.renderer().set_camera(cam_);
  world_->set_view(cam_);
  facades_.set_night(world_->apply_lighting(cam_.eye));
  water_.set_time(time_);
  world_->present(f.time * 1000.0, cam_.eye);
  if (placed_) {
    std::array<Vec3, 4> motors;
    std::array<f32, 4> angles;
    for (int i = 0; i < 4; ++i) { motors[i] = drone_.motor_position(i); angles[i] = drone_.rotor_angle(i); }
    art_.draw(drone_matrix(), motors, angles, drone_.params().prop_d, drone_.telemetry().armed, cam_mode_ == 0);
  } else {
    art_.hide();
  }
  update_sound(dt);
}

// The drone's force model in every fixed step of the world (Box3D clears forces after each step): the game takes the
// steps itself - one GameWorld::step_once, then the flight controller and the motors - as many as the frame's time.
void SpawnCity::fly(f32 dt) {
  const f64 h = 1.0 / kStepHz;
  accumulator_ += kern::clamp(static_cast<f64>(dt), 0.0, 0.25);
  u32 n = 0;
  while (accumulator_ >= h && n < kMaxSteps) {
    world_->step_once({});
    drone_.step(world_->physics(), world_->actor_body(drone_actor_), static_cast<f32>(h), sticks_);
    accumulator_ -= h;
    ++steps_;
    ++n;
  }
  if (accumulator_ >= h) accumulator_ = std::fmod(accumulator_, h);  // behind: drop whole steps, keep the phase
}

// The sticks from the keyboard, the mouse, the gamepad and the touch controls (CONTROLS.md, the drone).
void SpawnCity::read_input(kern::Engine& e, f32 dt) {
  const kern::Input& in = e.input();
  const DroneTelemetry& t = drone_.telemetry();
  // the touch controls of the drone (touch_play.cpp)
  TouchLayout layout;
  if (touch_mode_) layout.mode = TouchMode::drone;
  layout.armed = t.armed;
  touch_.set_layout(layout);
  // left stick: WASD (x = D - A, y = S - W), the touch stick, LS
  Vec2 left{key(in, Key::d) - key(in, Key::a), key(in, Key::s) - key(in, Key::w)};
  if (touch_mode_) left += touch_.move();
  if (pad.connected) left += Vec2{controls::gamepad_axis(pad.lx), controls::gamepad_axis(-pad.ly)};
  // right stick: arrows, the mouse as a rate (1500 px/s, 50 ms), the touch stick, RS
  Vec2 right{key(in, Key::right) - key(in, Key::left), key(in, Key::down) - key(in, Key::up)};
  mouse_.step(mouse_px_, dt);
  mouse_px_ = {};
  right += mouse_.rate;
  if (touch_mode_) right += touch_.look();
  if (pad.connected) right += Vec2{controls::gamepad_axis(pad.rx), controls::gamepad_axis(-pad.ry)};
  right = {clampf(right.x, -1, 1), clampf(right.y, -1, 1)};
  controls::DroneAxes a;
  controls::apply_game_stick_left(a, left.x, left.y, settings_.get(Opt::drone_left_fineness), settings_.on(Opt::drone_invert_roll), settings_.get(Opt::drone_max_speed_kmh));
  controls::apply_game_stick_right(a, right.x, right.y, settings_.get(Opt::drone_right_fineness), settings_.on(Opt::drone_invert_yaw), settings_.on(Opt::drone_invert_pitch));
  sticks_.roll = a.roll;
  sticks_.pitch = a.pitch;
  sticks_.yaw = a.yaw;
  sticks_.body_pitch = a.body_pitch;
  // height axis: Space / Shift, RT - LT, the touch altitude pad
  f32 axis = key(in, Key::space) - ((in.down(Key::shift_left) || in.down(Key::shift_right)) ? 1.0f : 0.0f);
  if (pad.connected) axis += controls::height_axis(pad.rt, pad.lt);
  if (touch_mode_) axis += touch_.clearance();
  axis = clampf(axis, -1, 1);
  // the camera: C, Y, the touch button
  if (in.pressed(Key::c) || (pad.connected && pad_hit(pad, kPadY)) || touch_.take_pressed("cam")) { cam_mode_ = (cam_mode_ + 1) % 3; rig_init_ = false; }
  // ARM (R, A, the ARM button): arm on the ground; in the air a controlled landing (simulator.mjs)
  const bool arm_key = in.pressed(Key::r) || (pad.connected && pad_hit(pad, kPadA)) || touch_.take_pressed("arm");
  const f32 agl = t.agl >= 0 ? t.agl : 0;
  if (arm_key) {
    if (!t.armed && !sticks_.arm) { sticks_.arm = true; landing_ = false; }
    else if (t.armed && agl > 0.7f && !landing_) landing_ = true;
    else if (landing_) landing_ = false;  // a second tap cancels the landing
    else { sticks_.arm = false; landing_ = false; }
  }
  // STOP (touch, B): the hard kill
  sticks_.kill = touch_.take_pressed("stop") || (pad.connected && pad_hit(pad, kPadB));
  if (sticks_.kill) { sticks_.arm = false; landing_ = false; }
  if (axis > 0.3f && !t.armed && !t.calibrating && !t.fault && !landing_ && !sticks_.kill) sticks_.arm = true;
  if (t.fault && !t.armed) sticks_.arm = false;  // the switch goes low for the power cycle
  if (landing_) {
    axis = -(agl > 6 ? 1.0f : std::fmax(0.35f, agl / 6));
    if (agl <= 0.7f && std::fabs(t.vz) < 0.35f) landing_still_ += dt;
    else landing_still_ = 0;
    if (landing_still_ > 0.25f) { sticks_.arm = false; landing_ = false; landing_still_ = 0; }
  }
  const bool have_agl = t.agl >= 0;
  if (std::fabs(axis) > 1e-4f) {
    height_held_ = true;
    const f32 rate = clearance_rate(axis);
    sticks_.clearance = have_agl ? clampf(agl + clearance_lead(rate), 0.5f, 120) : clampf(sticks_.clearance + rate * clampf(dt, 0, 0.05f), 0.5f, 120);
  } else if (height_held_) {
    height_held_ = false;
    if (have_agl) sticks_.clearance = clampf(agl + clearance_lead(t.vz), 0.5f, 120);
  }
}

void SpawnCity::mouse_move(f32 dx, f32 dy) {
  if (!locked_) return;
  mouse_px_.x += dx;
  mouse_px_.y += dy;
}

void SpawnCity::update_camera(kern::Engine&, f32 dt) {
  kern::Camera c = cam_;
  c.near = 0.05f;
  c.far = 2400;
  if (!placed_) {
    // While the city loads the view stands at the start place at eye height.
    c.eye = {0, 0, city_.ground_height(0, 0) + 1.68f};
    c.target = c.eye + Vec3{10, 0, 0};
    c.up = {0, 0, 1};
    c.fov = 50 * kDeg;
    cam_ = c;
    return;
  }
  const BodyState s = read_body(world_->physics(), world_->actor_body(drone_actor_));
  const Vec3 pos = drone_matrix().translation();
  const Vec3 fwd = s.rotation.rotate({-1, 0, 0}), up = s.rotation.rotate({0, 0, 1});
  const f32 fov = settings_.get(Opt::cam_fov);
  if (cam_mode_ == 0) {
    // FPV: rigid on the frame, tilted (camera_settings.mjs -15 .. 50 deg).
    const f32 tilt = settings_.get(Opt::cam_fpv_tilt) * kDeg;
    const Vec3 f = fwd * std::cos(tilt) + up * std::sin(tilt), u = up * std::cos(tilt) - fwd * std::sin(tilt);
    c.eye = pos + fwd * 0.102f + up * (0.028f + 0.045f);  // + fpv_view_height.mjs FPV_VIEW_EXTRA_UP_M
    c.target = c.eye + f * 4.0f;
    c.up = u;
    c.fov = fov * kDeg;
    c.near = 0.02f;
    cam_ = c;
    return;
  }
  // follow / third (camera_stabilization.mjs): a damped inertial anchor and heading, lag capped
  Vec2 heading{fwd.x, fwd.y};
  const f32 hl = std::sqrt(heading.x * heading.x + heading.y * heading.y);
  heading = hl > 0.2f ? heading * (1.0f / hl) : cam_heading_;
  const bool third = cam_mode_ == 2;
  const f32 pos_rate = third ? 3.6f : 4.8f, vel_rate = third ? 5.2f : 7.0f, head_rate = third ? 3.8f : 7.5f, max_lag = third ? 0.36f : 0.18f;
  if (!rig_init_) { cam_anchor_ = pos; cam_vel_ = s.velocity; cam_heading_ = heading; rig_init_ = true; }
  cam_vel_ += (s.velocity - cam_vel_) * damp_alpha(vel_rate, dt);
  const Vec3 predicted = cam_anchor_ + cam_vel_ * dt;
  cam_anchor_ = predicted + (pos - predicted) * damp_alpha(pos_rate, dt);
  const Vec3 lag = pos - cam_anchor_;
  const f32 l = kern::length(lag);
  if (l > max_lag) cam_anchor_ = pos - lag * (max_lag / l);
  const f32 ha = std::atan2(cam_heading_.y, cam_heading_.x), hb = std::atan2(heading.y, heading.x);
  const f32 hd_angle = ha + wrap_pi(hb - ha) * damp_alpha(head_rate, dt);
  cam_heading_ = {std::cos(hd_angle), std::sin(hd_angle)};
  const f32 back = third ? 1.5f * 2.25f / 2.4847f : 1.65f, upm = third ? 1.5f * 1.05f / 2.4847f : 0.78f;
  const f32 ahead = third ? 0.55f : 0.38f, look_up = third ? 0.18f : 0.10f;
  const Vec3 hd{cam_heading_.x, cam_heading_.y, 0};
  const Vec3 want = cam_anchor_ - hd * back + Vec3{0, 0, upm};
  // Box3D sphere sweep from the anchor (camera_collision_guard.mjs)
  const Vec3 d = want - cam_anchor_;
  const kern::physics::RayHit hit = world_->physics().cast_sphere(cam_anchor_, 0.17f, d, ~0ull, world_->actor_body(drone_actor_));
  c.eye = hit.hit ? cam_anchor_ + d * std::fmax(0.0f, hit.fraction - 0.02f) : want;
  c.target = cam_anchor_ + hd * ahead + Vec3{0, 0, look_up};
  c.up = {0, 0, 1};
  // simulator.mjs: third clamp(62 x fov / 105, 35, 100), follow clamp(52 x fov / 105, 30, 90)
  c.fov = (third ? clampf(62 * fov / 105, 35, 100) : clampf(52 * fov / 105, 30, 90)) * kDeg;
  c.near = 0.06f;
  cam_ = c;
}

// ---------------------------------------------------------------------------------------------
// touch

void SpawnCity::set_touch_frame(f32 w, f32 h, f32 sl, f32 st, f32 sr, f32 sb, f32) { touch_.set_frame(w, h, sl, st, sr, sb); }

bool SpawnCity::touch(u32 type, i32 id, f32 x, f32 y) {
  if (type == 0) touch_mode_ = true;
  return touch_.pointer(type, id, x, y, 0);
}

std::string SpawnCity::touch_view_json() {
  if (!touch_mode_) return "null";
  // A label's line break ("▲\nHÖHE\n▼") must reach JSON.parse escaped.
  const std::string raw = touch_.view_json();
  std::string out;
  for (char c : raw) {
    if (c == '\n') out += "\\n";
    else if (static_cast<unsigned char>(c) < 0x20) out += ' ';
    else out += c;
  }
  return out;
}

// ---------------------------------------------------------------------------------------------
// sound

bool SpawnCity::audio_init(u32 rate) {
  if (mixer_.ready()) return true;
  if (!mixer_.init(rate, 64)) return false;
  bank_ = std::make_unique<kern::game::SoundBank>(mixer_);
  game_audio_ = std::make_unique<kern::game::GameAudio>(mixer_, *bank_);
  if (world_) game_audio_->begin(*world_);   // (the sounds of its level: the city's ambience)
  return fx_.init(mixer_);
}

const std::vector<f32>& SpawnCity::audio_render(u32 frames) {
  audio_out_.resize(static_cast<std::size_t>(frames) * 2);
  // the page plays the original's own sound ("audio web 1"): Kern's mix is not heard then
  if (mixer_.ready() && !web_audio_) mixer_.render(audio_out_.data(), frames);
  else for (f32& v : audio_out_) v = 0;
  return audio_out_;
}

void SpawnCity::update_sound(f32 dt) {
  if (!mixer_.ready() || !world_) return;
  bank_->prepare(3);
  game_audio_->update(*world_, cam_, dt);
  // audio_settings.mjs: the sound switch, the effects / ambient / drone sliders (relative to SpawnCity's mix)
  mixer_.bus(kern::audio::Bus::master).muted = !settings_.on(Opt::audio_on) || paused_;
  mixer_.bus(kern::audio::Bus::sfx).volume = settings_.get(Opt::audio_fx) / 72;
  mixer_.bus(kern::audio::Bus::ambience).volume = settings_.get(Opt::audio_ambient) / 46;
  SoundState s;
  s.drone_mix = settings_.get(Opt::audio_drone) / 50 / std::fmax(0.01f, settings_.get(Opt::audio_fx) / 72);
  if (placed_) {
    const DroneTelemetry& t = drone_.telemetry();
    f32 w = 0;
    for (f32 o : t.omega) w += o;
    w /= 4;
    s.drone.active = w > 5;
    s.drone.position = drone_position();
    s.drone.velocity = world_->actor_velocity(drone_actor_);
    s.drone.rate = clampf(w / 3600, 0, 1.2f);
    s.drone.load = clampf(t.current_a / 120, 0, 1);
    s.wind = kern::length(s.drone.velocity);
  }
  fx_.update(s, dt);
}

// What the original's sound modules read (web/audio.mjs, every frame): the listener, the drone's motors, the settings
// in the original's names (audio_settings.mjs normalizeAudioSettings). The other parts of the original's state (on
// foot, the car, jets, police, missiles) are empty: the small game has none of them.
std::string SpawnCity::audio_frame() {
  std::string s = "{\"e\":[]";
  char b[512];
  const Vec3 fwd = kern::normalize(cam_.target - cam_.eye, {0, 1, 0});
  std::snprintf(b, sizeof b, ",\"paused\":%d,\"mode\":\"drone\",\"fwd\":[%.3f,%.3f,%.3f],\"L\":[%.2f,%.2f,%.2f]", paused_ ? 1 : 0, fwd.x, fwd.y, fwd.z, cam_.eye.x, cam_.eye.y, cam_.eye.z);
  s += b;
  std::snprintf(b, sizeof b, ",\"set\":{\"soundEnabled\":%d,\"musicEnabled\":%d,\"musicVolume\":%.0f,\"droneVolume\":%.0f,\"shotsVolume\":%.0f,\"fxVolume\":%.0f,\"footstepsVolume\":%.0f,"
      "\"vehicleVolume\":%.0f,\"ambientVolume\":%.0f}",
      settings_.on(Opt::audio_on) ? 1 : 0, settings_.on(Opt::music_on) ? 1 : 0, settings_.get(Opt::music_volume), settings_.get(Opt::audio_drone), settings_.get(Opt::audio_shots),
      settings_.get(Opt::audio_fx), settings_.get(Opt::audio_steps), settings_.get(Opt::audio_vehicle), settings_.get(Opt::audio_ambient));
  s += b;
  const Vec3 p = drone_position();
  std::snprintf(b, sizeof b, ",\"walk\":{\"dead\":0,\"air\":0,\"p\":[%.2f,%.2f,%.2f]}", p.x, p.y, p.z);
  s += b;
  if (placed_) {
    // motor_sound.mjs update(model, cameraPosition) / syncFcState(fcState, arm)
    const DroneTelemetry& t = drone_.telemetry();
    std::snprintf(b, sizeof b, ",\"drone\":{\"st\":%u,\"arm\":%d,\"d\":%.4f,\"w\":[%.2f,%.2f,%.2f,%.2f],\"mt\":[%.5f,%.5f,%.5f,%.5f],\"pt\":[%.5f,%.5f,%.5f,%.5f],\"p\":[%.2f,%.2f,%.2f]}",
                  t.state, sticks_.arm ? 1 : 0, drone_.params().prop_d, t.omega[0], t.omega[1], t.omega[2], t.omega[3], t.motor_torque[0], t.motor_torque[1], t.motor_torque[2],
                  t.motor_torque[3], t.prop_torque[0], t.prop_torque[1], t.prop_torque[2], t.prop_torque[3], p.x, p.y, p.z);
    s += b;
  }
  s += ",\"jp\":[0,0],\"rj\":[],\"pc\":[],\"pd\":[],\"fj\":[],\"gm\":[],\"rk\":[],\"rp\":[],\"gps\":0}";
  return s;
}

// ---------------------------------------------------------------------------------------------
// HUD and commands

std::string SpawnCity::hud_json() {
  char b[640];
  std::string s = "{";
  std::snprintf(b, sizeof b, "\"mode\":\"drone\",\"placed\":%s,\"offline\":%s,\"realData\":%s", placed_ ? "true" : "false", city_.offline() ? "true" : "false",
                city_.has_real_data() ? "true" : "false");
  s += b;
  const StreamStats& st = city_.stats();
  std::snprintf(b, sizeof b, ",\"tiles\":{\"ready\":%u,\"loading\":%u,\"failed\":%u,\"buildings\":%u,\"colliders\":%u", st.tiles_ready, st.tiles_loading, st.tiles_failed, st.buildings,
                st.colliders);
  s += b;
  std::snprintf(b, sizeof b, ",\"t\":{\"request\":%.0f,\"data\":%.0f,\"decoded\":%.0f,\"terrain\":%.0f,\"built\":%.0f,\"collision\":%.0f,\"work\":%.0f,\"worst\":%.1f,\"worstStep\":\"%s\","
                "\"worstStepMs\":%.1f,\"rebuilds\":%u,\"relays\":%u,\"uploads\":%u,\"uploadKVerts\":%u}}",
                st.t_request, st.t_data, st.t_decoded, st.t_terrain, st.t_built, st.t_collision, st.work_ms, st.worst_ms, st.worst_step, st.worst_step_ms, st.rebuilds, st.relays,
                st.uploads, st.upload_vertices);
  s += b;
  std::snprintf(b, sizeof b, ",\"fps\":%.0f,\"touch\":%s,\"steps\":%llu", fps_, touch_mode_ ? "true" : "false", static_cast<unsigned long long>(steps_));
  s += b;
  const DroneTelemetry& t = drone_.telemetry();
  const Vec3 pos = drone_position();
  const Vec3 vel = placed_ ? world_->actor_velocity(drone_actor_) : Vec3{};
  const f32 agl = t.agl >= 0 ? t.agl : pos.z - city_.ground_height(pos.x, pos.y);
  std::snprintf(b, sizeof b, ",\"drone\":{\"armed\":%s,\"calibrating\":%s,\"fault\":%s,\"game\":%s,\"landing\":%s,\"battery\":%.2f,\"volts\":%.2f,\"amps\":%.1f,\"clearance\":%.1f,"
                "\"cam\":%d,\"pulses\":[%u,%u,%u,%u]}",
                t.armed ? "true" : "false", t.calibrating ? "true" : "false", t.fault ? "true" : "false", sticks_.game_mode ? "true" : "false", landing_ ? "true" : "false",
                t.battery_soc, t.battery_v, t.current_a, sticks_.clearance, cam_mode_, t.pulses[0], t.pulses[1], t.pulses[2], t.pulses[3]);
  s += b;
  std::snprintf(b, sizeof b, ",\"speed\":%.2f,\"vz\":%.2f,\"agl\":%.1f,\"alt\":%.1f,\"x\":%.2f,\"y\":%.2f", kern::length(vel), vel.z, agl, pos.z, pos.x, pos.y);
  s += b;
  f64 lon = lon_, lat = lat_;
  unproject(city_.projector(), pos.x, pos.y, lon, lat);
  std::snprintf(b, sizeof b, ",\"lon\":%.6f,\"lat\":%.6f", lon, lat);
  s += b;
  const std::string hint = placed_ && t.calibrating ? "Flugregler kalibriert - stillhalten" : "";
  s += ",\"hint\":\"" + json_escape(hint) + "\"";
  if (toast_age_ < 4) s += ",\"toast\":\"" + json_escape(toast_) + "\"";
  std::snprintf(b, sizeof b, ",\"ui\":{\"placed\":%s,\"paused\":%s,\"settings\":%u,\"resets\":%u,\"moves\":%u,\"origin\":[%.6f,%.6f],\"northUp\":%s,\"audioOn\":%s,\"musicOn\":%s}",
                placed_ ? "true" : "false", paused_ ? "true" : "false", settings_.version(), resets_, moves_, lat_, lon_, settings_.on(Opt::view_minimap_north) ? "true" : "false",
                settings_.on(Opt::audio_on) ? "true" : "false", settings_.on(Opt::music_on) ? "true" : "false");
  s += b;
  std::snprintf(b, sizeof b, ",\"ctl\":{\"device\":\"%s\",\"pad\":%s,\"locked\":%s,\"hint\":\"%s\",\"sticks\":[%.3f,%.3f,%.3f,%.3f],\"view\":{\"eye\":[%.2f,%.2f,%.2f]}}",
                touch_mode_ ? "touch" : "desktop", pad.connected ? "true" : "false", locked_ ? "true" : "false", json_escape(controls::hint_line("drone", "", false)).c_str(),
                sticks_.roll, sticks_.pitch, sticks_.yaw, sticks_.body_pitch, cam_.eye.x, cam_.eye.y, cam_.eye.z);
  s += b;
  // JSON allows no raw control characters: outside strings none are ever written, so every one becomes a space.
  for (char& c : s)
    if (static_cast<unsigned char>(c) < 0x20) c = ' ';
  return s + "}";
}

// The settings (settings.h: the Blueprint "Settings" of the level holds the table): the page's menu and its replay.
std::string SpawnCity::settings_command(const std::string& cmd) {
  if (cmd == "settings" || cmd == "settings schema") return settings_.schema_json(settings_table_);
  if (cmd == "settings values") return settings_.values_json(settings_table_);
  if (cmd.rfind("settings reset", 0) == 0) {
    settings_.reset_section(settings_table_, cmd.size() > 15 ? cmd.substr(15) : std::string());
    return settings_.values_json(settings_table_);
  }
  if (cmd.rfind("set ", 0) == 0) {
    // "set <key> <value>" (several pairs separated by ';' are fine: the page replays its copy)
    const std::string rest = cmd.substr(4);
    u32 ok = 0, bad = 0;
    std::size_t at = 0;
    while (at < rest.size()) {
      std::size_t end = rest.find(';', at);
      if (end == std::string::npos) end = rest.size();
      const std::string pair = rest.substr(at, end - at);
      const std::size_t sp = pair.find(' ');
      if (sp != std::string::npos && settings_.set(settings_table_, std::string_view(pair).substr(0, sp), std::string_view(pair).substr(sp + 1))) ++ok;
      else if (!pair.empty()) ++bad;
      at = end + 1;
    }
    return bad ? (ok ? "partly" : "unknown setting") : "ok";
  }
  if (cmd.rfind("get ", 0) == 0) {
    Opt o;
    if (!find_opt(settings_table_, cmd.substr(4), o)) return "unknown setting";
    char b[32];
    std::snprintf(b, sizeof b, "%g", static_cast<double>(settings_.get(o)));
    return b;
  }
  return "";
}

std::string SpawnCity::command(const std::string& cmd) {
  if (!world_) return "no world";
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  if (starts("settings") || starts("set ") || starts("get ")) return settings_command(cmd);
  if (starts("pause ")) {
    paused_ = cmd.size() > 6 && cmd[6] == '1';
    if (engine_ && paused_) engine_->request_pointer_lock(false);
    return paused_ ? "paused" : "running";
  }
  if (cmd == "reset") { reset(); return "ok"; }
  if (starts("start ")) {
    // the start place (start_location.mjs): "start <lat> <lon> [name]"
    double lat = 0, lon = 0;
    if (std::sscanf(cmd.c_str() + 6, "%lf %lf", &lat, &lon) < 2 || !std::isfinite(lat) || !std::isfinite(lon) || std::fabs(lat) > 85 || std::fabs(lon) > 180) return "usage: start <lat> <lon> [name]";
    move_world(lon, lat);
    return "moving";
  }
  if (cmd == "origin") {
    char b[96];
    std::snprintf(b, sizeof b, "{\"lat\":%.6f,\"lon\":%.6f}", lat_, lon_);
    return b;
  }
  // simulator.mjs camFpv / camFollow / camThird; "cam" alone the next one
  if (cmd == "cam" || cmd == "cam next") { cam_mode_ = (cam_mode_ + 1) % 3; rig_init_ = false; return cam_mode_ == 0 ? "fpv" : cam_mode_ == 1 ? "follow" : "third"; }
  if (cmd == "cam fpv" || cmd == "cam follow" || cmd == "cam third") {
    cam_mode_ = cmd == "cam fpv" ? 0 : cmd == "cam follow" ? 1 : 2;
    rig_init_ = false;
    return cmd.substr(4);
  }
  if (cmd == "mode drone" || cmd == "mode") return "drone";
  if (starts("press ")) { touch_.inject(cmd.substr(6)); return "ok"; }   // a click on the page's button (ARM, STOP, KAMERA)
  if (starts("device ")) {
    // the page: "device touch|mouse|mousemove|key|pad|platform ..."
    const std::string a = cmd.substr(7);
    if (a == "touch") touch_mode_ = true;
    else if (a == "mouse" || a == "mousemove" || a == "key") touch_mode_ = false;
    return touch_mode_ ? "touch" : "desktop";
  }
  if (starts("touch rects")) {
    // web/view.mjs: "touch rects id x y w h;id x y w h;..." - where the page drew each button
    std::vector<TouchPlay::DomRect> rects;
    std::size_t i = 11;
    while (i < cmd.size()) {
      std::size_t j = cmd.find(';', i);
      if (j == std::string::npos) j = cmd.size();
      char id[32] = {};
      TouchPlay::DomRect d;
      if (std::sscanf(cmd.substr(i, j - i).c_str(), " %31s %f %f %f %f", id, &d.x, &d.y, &d.w, &d.h) == 5) { d.id = id; rects.push_back(d); }
      i = j + 1;
    }
    touch_.set_dom_rects(std::move(rects));
    return "ok";
  }
  if (cmd == "audio frame") return audio_frame();
  if (starts("audio web ")) { web_audio_ = cmd.substr(10) != "0"; return web_audio_ ? "web" : "kern"; }
  // tests: the drone's state
  if (cmd == "drone") {
    const Vec3 p = drone_position();
    char b[160];
    std::snprintf(b, sizeof b, "{\"x\":%.3f,\"y\":%.3f,\"z\":%.3f,\"armed\":%s,\"steps\":%llu}", p.x, p.y, p.z, drone_.telemetry().armed ? "true" : "false",
                  static_cast<unsigned long long>(steps_));
    return b;
  }
  return "unknown command";
}

}  // namespace sc
