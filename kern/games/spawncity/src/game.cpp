// SpawnCity on Kern: world, modes, fixed-step hook, cameras (see game.h).
#include "game.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "kern/core/base.h"
#include "kern/logic/logic.h"

namespace sc {

using kern::game::ActorId;
using kern::game::GameWorld;
using kern::game::PlayInput;

namespace {

constexpr f32 kStepHz = 240;                 // fixed physics rate: the firmware runs 4 ticks per step (~1 kHz)
constexpr f32 kEnterCar = 5.2f, kEnterJet = 9;  // player_car_mode.mjs ENTER_RADIUS_M, jet_mode.mjs ENTER_M
constexpr f32 kDroneAhead = 2.9f;            // player_vehicle_runtime_v2.mjs: the drone starts 2.9 m ahead
constexpr f32 kMouseRad = 0.0026f;           // player_walk_mode_v4.mjs: raw mouse 0.0026 rad per px
constexpr f32 kPlayerHealth = 100 * 1.18f;   // player_vitals_runtime.mjs: +18 % tougher
constexpr f32 kChuteOpen = 1.1f, kChuteSink = 6.5f, kChuteSteer = 4, kEjectUp = 28;  // jet_mode.mjs

const char* kNodeType = "spawncity.fixed_step";

Vec3 car_color(u32 i) {
  static const Vec3 c[] = {{0.62f, 0.05f, 0.04f}, {0.08f, 0.16f, 0.42f}, {0.85f, 0.85f, 0.86f}, {0.06f, 0.06f, 0.07f}, {0.45f, 0.47f, 0.5f},
                           {0.12f, 0.32f, 0.18f}, {0.9f, 0.62f, 0.08f}, {0.3f, 0.5f, 0.75f}, {0.55f, 0.36f, 0.22f}, {0.7f, 0.7f, 0.66f}};
  return c[i % (sizeof c / sizeof c[0])];
}

f32 deadzone(f32 v, f32 dz = 0.14f) {  // xbox_gamepad.mjs deadzone .14
  const f32 a = std::fabs(v);
  return a <= dz ? 0 : sign0(v) * (a - dz) / (1 - dz);
}

kern::game::Actor vehicle_actor(const char* name, const RigidSpec& spec, Vec3 pos, f32 yaw_deg) {
  using namespace kern::game;
  Actor a;
  a.name = name;
  a.transform.position = pos;
  a.transform.rotation = {0, 0, yaw_deg};
  BodyComponent b;
  b.motion = Motion::dynamic;
  b.mass = spec.mass.mass;
  b.center_of_mass = spec.mass.center;
  for (int i = 0; i < 6; ++i) b.inertia[i] = spec.mass.inertia[i];
  b.linear_damping = spec.linear_damping;
  b.angular_damping = spec.angular_damping;
  b.bullet = spec.bullet;
  b.allow_sleep = false;
  a.body = b;
  for (const ColliderSpec& c : spec.colliders) {
    ColliderComponent cc;
    switch (c.kind) {
      case kern::physics::ShapeKind::box: cc.shape = ColliderShape::box; break;
      case kern::physics::ShapeKind::sphere: cc.shape = ColliderShape::sphere; break;
      case kern::physics::ShapeKind::capsule: cc.shape = ColliderShape::capsule; break;
      default: cc.shape = ColliderShape::hull; break;
    }
    cc.center = c.center;
    cc.rotation = quat_to_euler(c.rotation);
    cc.size = c.size;
    cc.radius = c.radius;
    cc.length = c.length;
    cc.points = c.points;
    cc.friction = c.friction;
    cc.restitution = c.restitution;
    cc.density = 100;
    a.colliders.push_back(cc);
  }
  return a;
}

void register_node() {
  static bool done = false;
  if (done) return;
  done = true;
  kern::logic::NodeDef d;
  d.type = kNodeType;
  d.label = "SpawnCity Fixed Step";
  d.category = "SpawnCity";
  d.help = "Runs the game's vehicle force models in every fixed physics step (Box3D clears forces after each step).";
  d.kind = kern::logic::NodeKind::event;
  d.event = "logic.tick";
  d.accepts = [](const kern::logic::Node&, const kern::logic::Event&) { return true; };
  d.run = [](kern::logic::Context&) {
    game().fixed_step(1.0f / kStepHz);
    return kern::logic::kStop;
  };
  kern::logic::library().add(std::move(d));
}

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

const char* mode_name(Mode m) {
  switch (m) {
    case Mode::foot: return "foot";
    case Mode::drone: return "drone";
    case Mode::car: return "car";
    case Mode::jet: return "jet";
    case Mode::parachute: return "parachute";
  }
  return "?";
}

Game& game() {
  static Game g;
  return g;
}

Game::Game() = default;
Game::~Game() = default;

// ---------------------------------------------------------------------------------------------
// level

kern::game::Level Game::build_level() const {
  using namespace kern::game;
  Level l;
  l.name = "SpawnCity";
  l.world.title = "SpawnCity";
  l.world.step_hz = kStepHz;
  l.world.substeps = 4;
  l.world.time_of_day = 0.40f;
  l.world.day_length = 480;          // SpawnCity: a day-night cycle of 480 s
  l.world.fog_density = 0.00085f;     // stylized_world_style.mjs
  l.world.shadow_distance = mobile_ ? 70 : 110;
  l.world.ao_radius = 1.6f;
  l.world.ground = true;
  l.world.ground_color = {0.36f, 0.40f, 0.31f};
  l.world.ground_friction = 0.8f;
  l.world.kill_z = -60;
  l.world.ambience = "builtin:city";
  l.world.ambience_volume = 0.35f;
  l.world.reverb = 0.2f;

  // The player: Kern's soldier with SpawnCity's on-foot values (walk 5.5, sprint 8.1, jump 3.6 m/s,
  // eye 1.68 m, 90 degrees horizontal field of view), starting with the pistol.
  Actor s;
  s.name = "Player";
  s.transform.position = {0, 0, 0.725f};
  MeshComponent m;
  m.shape = Shape::capsule;
  m.radius = 0.35f;
  m.height = 1.45f;
  m.material.color = {0.32f, 0.42f, 0.3f};
  s.mesh = m;
  BodyComponent b;
  b.motion = Motion::dynamic;
  b.mass = 80;
  b.allow_sleep = false;
  b.lock_angular[0] = b.lock_angular[1] = true;  // stands upright while not possessed (flying the drone)
  s.body = b;
  ControllerComponent c = soldier_controller_;
  s.controller = c;
  s.weapon = weapon_preset(WeaponKind::pistol);
  HealthComponent h;
  h.max_health = kPlayerHealth;
  h.respawn = true;
  s.health = h;
  const ActorId sid = l.add(s).id;
  Actor head;
  head.name = "Head";
  head.parent = sid;
  head.transform.position = {0, 0, 0.83f};
  MeshComponent hm;
  hm.shape = Shape::sphere;
  hm.radius = 0.17f;
  head.mesh = hm;
  head.tags.push_back("head");
  l.add(head);

  // Vehicles: bodies only (the game draws them).
  l.add(vehicle_actor("Drone", Drone::rigid_spec(), {0, 0, -200}, 0));
  l.add(vehicle_actor("Jet", Jet::rigid_spec(), {0, 0, -300}, 0));
  const u32 n = mobile_ ? 6 : 10;
  for (u32 i = 0; i <= n; ++i) {
    char name[32];
    std::snprintf(name, sizeof name, i == n ? "Bus" : "Car %u", i + 1);
    const CarSpec spec = i == n ? CarSpec::bus() : CarSpec::car();
    l.add(vehicle_actor(name, Car::rigid_spec(spec), {static_cast<f32>(i) * 10, 0, -400}, 0));
  }
  // Parachute (pilot + canopy): an upright capsule body carried by the canopy's drag.
  {
    RigidSpec rs;
    ColliderSpec cap;
    cap.kind = kern::physics::ShapeKind::capsule;
    cap.radius = 0.3f;
    cap.length = 1.1f;
    rs.colliders.push_back(cap);
    rs.mass.mass = 90;
    box_inertia(90, {0.6f, 0.6f, 1.7f}, rs.mass.inertia);
    Actor a = vehicle_actor("Parachute", rs, {0, 0, -500}, 0);
    a.body->lock_angular[0] = a.body->lock_angular[1] = a.body->lock_angular[2] = true;
    l.add(a);
  }

  // The fixed-step hook: one node of the game's own type in the level graph.
  kern::logic::Node node;
  node.id = 1;
  node.type = kNodeType;
  l.logic.nodes.push_back(node);
  l.logic.next_id = 2;
  return l;
}

bool Game::start(kern::Engine& e) {
  engine_ = &e;
  mobile_ = e.arg_number("mobile", 0) > 0;
  lat_ = e.arg_number("lat", kKonstanzLat);
  lon_ = e.arg_number("lon", kKonstanzLon);
  fpv_tilt_deg_ = static_cast<f32>(kern::clamp(e.arg_number("fpvtilt", -15), -15.0, 50.0));
  sensitivity_ = static_cast<f32>(kern::clamp(e.arg_number("mouse", 100), 20.0, 300.0)) / 100;  // mouse_settings.mjs 20..300 %
  if (const char* s = e.arg("start")) start_mode_ = s;

  register_node();
  const bool coarse = e.arg_number("coarse", mobile_ ? 1 : 0) > 0;
  arbiter_.set_platform(!coarse, coarse, coarse ? 5 : 0);
  soldier_controller_ = kern::game::ControllerComponent{};
  soldier_controller_.mode = kern::game::ControlMode::soldier;
  soldier_controller_.camera = kern::game::CameraMode::first_person;
  soldier_controller_.max_speed = 5.5f;
  soldier_controller_.boost_speed = 8.1f;
  soldier_controller_.jump_speed = 3.6f;
  soldier_controller_.eye_height = 1.68f;
  soldier_controller_.camera_fov = 59;  // 90 degrees across a 16:9 view (camera_pitch_contract.mjs)

  world_ = std::make_unique<GameWorld>(e.renderer, &windows_);
  if (!world_->start(build_level(), kern::game::SimMode::play)) return kern::fail("level start: %s", kern::last_error());
  soldier_ = world_->find_actor("Player");
  drone_actor_ = world_->find_actor("Drone");
  jet_actor_ = world_->find_actor("Jet");
  chute_actor_ = world_->find_actor("Parachute");
  const u32 n = mobile_ ? 6 : 10;
  for (u32 i = 0; i <= n; ++i) {
    char name[32];
    std::snprintf(name, sizeof name, i == n ? "Bus" : "Car %u", i + 1);
    TrafficCar c;
    c.actor = world_->find_actor(name);
    c.car = std::make_unique<Car>(i == n ? CarSpec::bus() : CarSpec::car());
    c.color = i == n ? Vec3{0.92f, 0.78f, 0.1f} : car_color(i);
    c.target_speed = i == n ? 8 : 9 + static_cast<f32>(i % 4);
    cars_.push_back(std::move(c));
  }
  for (ActorId a : {drone_actor_, jet_actor_, chute_actor_}) world_->set_alive(a, false);
  for (TrafficCar& c : cars_) world_->set_alive(c.actor, false);
  world_->set_paused(true);  // until the ground under the player is built

  city_.start(e.renderer, world_->physics(), lon_, lat_);
  if (e.arg_number("offline", 0) > 0) city_.go_offline();
  if (!art_.init(e.renderer)) return false;
  foot_touch_.set_controller(&soldier_controller_, true, true);
  e.set_status("SpawnCity: Karte wird geladen ...");
  cam_.eye = {-30, -40, 60};
  cam_.target = {0, 0, 0};
  e.renderer.camera = cam_;
  return true;
}

void Game::stop() {
  world_.reset();
}

kern::Mat4 Game::matrix_of(ActorId a) const {
  kern::Mat4 m;
  world_->actor_matrix(a, m);
  return m;
}

Vec3 Game::player_position() const {
  if (mode_ == Mode::car && car_index_ >= 0) return matrix_of(cars_[car_index_].actor).translation();
  if (mode_ == Mode::jet) return matrix_of(jet_actor_).translation();
  if (mode_ == Mode::parachute) return matrix_of(chute_actor_).translation();
  return matrix_of(soldier_).translation();
}

Vec3 Game::focus_position() const {
  if (mode_ == Mode::drone && drone_out_) return matrix_of(drone_actor_).translation();
  return player_position();
}

bool Game::clear_spot(Vec3 center, Vec2 half, f32 search, Spot& out) const {
  // A place where rays from above reach the ground (no roof, no wall) across the footprint.
  const kern::physics::World& w = world_->physics();
  for (f32 r = 0; r <= search; r += 6) {
    const int steps = r == 0 ? 1 : static_cast<int>(2 * kPi * r / 8);
    for (int i = 0; i < steps; ++i) {
      const f32 a = 2 * kPi * static_cast<f32>(i) / static_cast<f32>(steps);
      const Vec3 c{center.x + r * std::cos(a), center.y + r * std::sin(a), 0};
      bool ok = true;
      for (int ix = -2; ix <= 2 && ok; ++ix)
        for (int iy = -2; iy <= 2 && ok; ++iy) {
          const Vec3 o{c.x + half.x * static_cast<f32>(ix) * 0.5f, c.y + half.y * static_cast<f32>(iy) * 0.5f, 80};
          const RayResult h = cast(w, o, {0, 0, -1}, 90, {});
          ok = h.hit && h.point.z < 0.3f;
        }
      if (ok) { out = {c, 0}; return true; }
    }
  }
  return false;
}

void Game::place_world() {
  // Player at the start (a free spot near the requested place), the jet on the nearest square big
  // enough for it (SpawnCity: the VTOL stands next to the player at the start).
  Spot p{};
  if (!clear_spot({0, 0, 0}, {2, 2}, 200, p)) p = {{0, 0, 0}, 0};
  spawn_ = p;
  world_->teleport(soldier_, p.p + Vec3{0, 0, 0.73f});
  Spot j{};
  if (clear_spot(p.p + Vec3{18, 6, 0}, {16, 20}, 260, j)) {
    world_->set_alive(jet_actor_, true);
    const Vec3 rot{0, 0, std::atan2(p.p.y - j.p.y, p.p.x - j.p.x) / kDeg - 90};  // nose (+Y) towards the player
    world_->teleport(jet_actor_, j.p + Vec3{0, 0, 2.1f}, &rot);
    jet_.reset();
    jet_.pilot = false;
    jet_placed_ = true;
  }
  world_placed_ = true;
  world_->set_paused(false);
  engine_->set_status("");
  toast_ = city_.offline() ? "OFFLINE: prozedurale Ersatzstadt (keine echten Kartendaten)" : "Konstanz - echte OSM-Daten";
  toast_age_ = 0;
  if (start_mode_ == "drone") toggle_drone();
  else if (start_mode_ == "jet") command("mode jet");
  else if (start_mode_ == "car") command("mode car");
}

// ---------------------------------------------------------------------------------------------
// modes

void Game::hide_soldier() {
  world_->set_alive(soldier_, false);
  world_->set_mode(kern::game::SimMode::simulate);
}

void Game::show_soldier(Vec3 feet, f32 yaw_deg) {
  if (!world_->actor_alive(soldier_)) world_->set_alive(soldier_, true);
  world_->set_mode(kern::game::SimMode::play);
  const Vec3 rot{0, 0, yaw_deg};
  world_->teleport(soldier_, feet + Vec3{0, 0, 0.73f}, &rot);
}

void Game::toggle_drone() {
  if (mode_ == Mode::drone) {
    // Stow the drone (killDrone): back to the pilot.
    world_->set_alive(drone_actor_, false);
    drone_out_ = false;
    world_->set_mode(kern::game::SimMode::play);
    mode_ = Mode::foot;
    return;
  }
  if (mode_ != Mode::foot) return;
  const Vec3 feet = matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f};
  const f32 yaw = world_->camera_yaw() * kDeg;
  Vec3 at = feet + Vec3{std::cos(yaw), std::sin(yaw), 0} * kDroneAhead;
  const RayResult g = cast(world_->physics(), at + Vec3{0, 0, 3}, {0, 0, -1}, 10, body(soldier_));
  at.z = (g.hit ? g.point.z : feet.z) + 0.0225f;
  world_->set_alive(drone_actor_, true);
  // The nose (-X) looks where the pilot looks.
  const Vec3 rot{0, 0, world_->camera_yaw() + 180};
  world_->teleport(drone_actor_, at, &rot);
  world_->set_actor_velocity(drone_actor_, {});
  drone_.reset();
  sticks_ = DroneSticks{};
  sticks_.clearance = 1.2f;  // control_semantics.mjs defaultHoverAgl
  landing_ = false;
  drone_out_ = true;
  // The pilot stays where he is (not possessed: the soldier stands while the drone flies).
  world_->set_mode(kern::game::SimMode::simulate);
  mode_ = Mode::drone;
  rig_init_ = false;
}

void Game::switch_mode(Mode m, i32 vehicle) {
  if (m == mode_ && (m != Mode::car || vehicle == car_index_)) return;
  if (mode_ == Mode::drone) toggle_drone();  // stow first
  switch (m) {
    case Mode::foot: break;
    case Mode::drone: toggle_drone(); return;
    case Mode::car: {
      TrafficCar& c = cars_[vehicle];
      if (c.driver && c.placed) {
        // Carjacking: the driver is thrown out; a witness reports it (wanted_system_logic.mjs).
        wanted_.report(wanted::Crime::carjacking, true);
        toast_ = "Auto geklaut! Der Fahrer ist raus.";
        toast_age_ = 0;
      }
      c.driver = false;
      car_index_ = vehicle;
      hide_soldier();
      car_cockpit_ = false;
      break;
    }
    case Mode::jet:
      hide_soldier();
      jet_.pilot = true;
      jet_.engine_on = true;
      break;
    case Mode::parachute: break;
  }
  mode_ = m;
  rig_init_ = false;
  head_yaw_ = head_pitch_ = 0;
}

void Game::try_enter() {
  const Vec3 me = matrix_of(soldier_).translation();
  i32 best = -1;
  f32 best_d = kEnterCar;
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    if (!cars_[i].placed) continue;
    const Vec3 p = matrix_of(cars_[i].actor).translation();
    const f32 d = std::hypot(p.x - me.x, p.y - me.y) - (cars_[i].car->spec().half.x - 1.78f);
    if (d < best_d) { best_d = d; best = static_cast<i32>(i); }
  }
  if (jet_placed_) {
    const Vec3 j = matrix_of(jet_actor_).translation();
    if (std::hypot(j.x - me.x, j.y - me.y) < kEnterJet && (best < 0 || std::hypot(j.x - me.x, j.y - me.y) < best_d)) { switch_mode(Mode::jet); return; }
  }
  if (best >= 0) switch_mode(Mode::car, best);
}

void Game::exit_vehicle() {
  if (mode_ == Mode::car && car_index_ >= 0) {
    TrafficCar& c = cars_[car_index_];
    const BodyState s = state_of(c.actor);
    const Vec3 left = s.rotation.rotate({0, 1, 0});
    const f32 side = c.car->spec().half.y + 0.9f;
    Vec3 out = s.position + left * side;
    if (cast(world_->physics(), s.position, left, side + 0.5f, body(c.actor)).hit) out = s.position - left * side;
    out.z = city_.ground_height(out.x, out.y);
    show_soldier(out, heading_of(s.rotation.rotate({1, 0, 0})) / kDeg);
    c.input = CarInput{};
    c.input.handbrake = true;
    car_index_ = -1;
    mode_ = Mode::foot;
  } else if (mode_ == Mode::jet) {
    const BodyState s = state_of(jet_actor_);
    if (jet_.telemetry().contacts >= 2 && kern::length(s.velocity) < 8) {
      const Vec3 right = s.rotation.rotate({1, 0, 0});
      Vec3 out = s.position - right * 7.5f;
      out.z = city_.ground_height(out.x, out.y);
      jet_.pilot = false;
      jet_.engine_on = false;
      show_soldier(out, heading_of(s.rotation.rotate({0, 1, 0})) / kDeg);
      mode_ = Mode::foot;
    } else {
      eject();
    }
  } else if (mode_ == Mode::drone) {
    toggle_drone();
  }
}

void Game::eject() {
  // Ejection seat (jet_mode.mjs): +28 m/s upwards, the canopy opens after 1.1 s.
  const BodyState s = state_of(jet_actor_);
  jet_.pilot = false;
  world_->set_alive(chute_actor_, true);
  world_->teleport(chute_actor_, s.position + s.rotation.rotate({0, 0, 3.0f}));
  world_->set_actor_velocity(chute_actor_, s.velocity + Vec3{0, 0, kEjectUp});
  chute_time_ = 0;
  chute_open_ = false;
  mode_ = Mode::parachute;
  rig_init_ = false;
  toast_ = "Schleudersitz!";
  toast_age_ = 0;
}

// ---------------------------------------------------------------------------------------------
// fixed step (Kern Logic node, after every Box3D step)

void Game::fixed_step(f32 h) {
  if (!world_ || !world_placed_) return;
  ++steps_;
  kern::physics::World& w = world_->physics();
  if (drone_out_) {
    const kern::physics::Body b = body(drone_actor_);
    if (b.valid()) drone_.step(w, b, h, sticks_);
  }
  if (jet_placed_) {
    const kern::physics::Body b = body(jet_actor_);
    JetInput in = mode_ == Mode::jet ? jet_in_ : JetInput{};
    if (mode_ != Mode::jet && jet_.telemetry().contacts > 0) in.brake = 1;  // parked: brakes on
    if (b.valid()) jet_.step(w, b, h, in);
  }
  drive_traffic(h);
  if (mode_ == Mode::parachute) step_parachute(h);
}

void Game::drive_traffic(f32 h) {
  kern::physics::World& w = world_->physics();
  const std::vector<Lane>& lanes = city_.lanes();
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    TrafficCar& c = cars_[i];
    if (!c.placed) continue;
    const kern::physics::Body b = body(c.actor);
    if (!b.valid()) continue;
    if (static_cast<i32>(i) == car_index_ && mode_ == Mode::car) {
      c.car->step(w, b, h, c.input);
      continue;
    }
    if (!c.driver || c.lane < 0 || c.lane >= static_cast<i32>(lanes.size()) || !lanes[c.lane].alive) {
      CarInput parked;
      parked.handbrake = true;
      parked.pedal = c.driver ? -0.3f * sign0(c.car->forward_speed()) : 0;
      c.car->step(w, b, h, parked);
      continue;
    }
    // AI driver: follow the lane on its right-hand side (SpawnCity aiInput), keep 1.6 s + 6 m.
    const Lane& L = lanes[c.lane];
    const BodyState s = read_body(w, b);
    const Vec3 fwd = s.rotation.rotate({1, 0, 0});
    const i32 n = static_cast<i32>(L.points.size());
    c.index = std::clamp(c.index, 0, n - 1);
    const Vec2 p = L.points[c.index];
    const i32 prev = std::clamp(c.index - c.dir, 0, n - 1);
    Vec2 d = p - L.points[prev];
    const f32 dl = std::sqrt(d.x * d.x + d.y * d.y);
    d = dl > 1e-3f ? d * (1.0f / dl) : Vec2{fwd.x, fwd.y};
    const f32 off = std::fmin(L.width * 0.25f, 1.8f);
    const Vec3 target{p.x + d.y * off, p.y - d.x * off, 0};
    if (std::hypot(target.x - s.position.x, target.y - s.position.y) < 7) {
      c.index += c.dir;
      if (c.index < 0 || c.index >= n) {
        // End of the street: continue on a street that starts here, else turn round.
        const Vec2 end = L.points[c.index < 0 ? 0 : n - 1];
        i32 next = -1, next_dir = 1;
        const u32 start = rng_.next();
        for (std::size_t k = 0; k < lanes.size() && next < 0; ++k) {
          const std::size_t j = (start + k) % lanes.size();
          if (static_cast<i32>(j) == c.lane || !lanes[j].alive || lanes[j].points.size() < 2) continue;
          const Vec2 a = lanes[j].points.front(), z = lanes[j].points.back();
          if (std::hypot(a.x - end.x, a.y - end.y) < 4) { next = static_cast<i32>(j); next_dir = 1; }
          else if (std::hypot(z.x - end.x, z.y - end.y) < 4) { next = static_cast<i32>(j); next_dir = -1; }
        }
        if (next >= 0) {
          c.lane = next;
          c.dir = next_dir;
          c.index = next_dir > 0 ? 1 : static_cast<i32>(lanes[next].points.size()) - 2;
        } else {
          c.dir = -c.dir;
          c.index = std::clamp(c.index + 2 * c.dir, 0, n - 1);
        }
      }
    }
    CarInput in = ai_drive(s.position, fwd, c.car->forward_speed(), target, c.target_speed);
    const f32 v = std::fmax(0.0f, c.car->forward_speed());
    const f32 gap = 1.6f * v + 6;
    const Vec3 nose = s.position + fwd * (c.car->spec().half.x + 0.2f) + Vec3{0, 0, 0.1f};
    const RayResult ahead = cast(w, nose, fwd, gap, b);
    if (ahead.hit) in.pedal = std::fmin(in.pedal, ahead.distance < 6 ? -1.0f : -0.3f);
    // Stuck against something: back off a little, then try again.
    c.stuck = (in.pedal > 0.2f && v < 0.3f) ? c.stuck + h : 0;
    if (c.stuck > 3) { in.pedal = -0.6f; in.steer = -in.steer; if (c.stuck > 5) c.stuck = 0; }
    c.input = in;
    c.car->step(w, b, h, in);
  }
}

void Game::step_parachute(f32 h) {
  kern::physics::World& w = world_->physics();
  const kern::physics::Body b = body(chute_actor_);
  if (!b.valid()) return;
  const BodyState s = read_body(w, b);
  const f32 m = w.mass(b);
  chute_time_ += h;
  chute_open_ = chute_time_ >= kChuteOpen;
  Vec3 f;
  const f32 speed = kern::length(s.velocity);
  if (!chute_open_) {
    // Free fall: air drag with a terminal speed of about 55 m/s (player_walk_mode_v4.mjs).
    f = s.velocity * (-(m * kG / (55 * 55)) * speed);
  } else {
    // Canopy: vertical drag for a 6.5 m/s sink, glide steered at up to 4 m/s.
    f.z = -m * kG / (kChuteSink * kChuteSink) * s.velocity.z * std::fabs(s.velocity.z);
    const f32 yaw = world_->camera_yaw() * kDeg;
    const Vec2 fw{std::cos(yaw), std::sin(yaw)}, rt{std::sin(yaw), -std::cos(yaw)};
    const Vec2 want = (fw * chute_steer_.y + rt * chute_steer_.x) * kChuteSteer;
    f.x = m * 1.5f * (want.x - s.velocity.x);
    f.y = m * 1.5f * (want.y - s.velocity.y);
  }
  w.apply_force_to_center(b, f);
}

// ---------------------------------------------------------------------------------------------
// traffic placement (every half second)

void Game::manage_traffic() {
  const std::vector<Lane>& lanes = city_.lanes();
  const Vec3 me = focus_position();
  i32 alive_lanes = 0;
  for (const Lane& l : lanes) alive_lanes += l.alive ? 1 : 0;
  if (alive_lanes == 0) return;
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    TrafficCar& c = cars_[i];
    if (static_cast<i32>(i) == car_index_) continue;
    bool replace = !c.placed;
    if (c.placed) {
      const Vec3 p = matrix_of(c.actor).translation();
      const f32 d = std::hypot(p.x - me.x, p.y - me.y);
      // Far away (and so out of sight), fallen through, or its street unloaded: bring it back.
      replace = d > 420 || p.z < -5 || (c.driver && (c.lane < 0 || c.lane >= static_cast<i32>(lanes.size()) || !lanes[c.lane].alive) && d > 120);
      // An abandoned car stays where it was left (SpawnCity: 180 s).
    }
    if (!replace) continue;
    for (int attempt = 0; attempt < 24; ++attempt) {
      const i32 li = static_cast<i32>(rng_.next() % lanes.size());
      const Lane& L = lanes[li];
      if (!L.alive || L.points.size() < 2) continue;
      const i32 k = static_cast<i32>(rng_.next() % (L.points.size() - 1));
      const Vec2 a = L.points[k], bpt = L.points[k + 1];
      const f32 seg = std::hypot(bpt.x - a.x, bpt.y - a.y);
      if (seg < 8) continue;
      const Vec2 mid = (a + bpt) * 0.5f;
      const f32 d = std::hypot(mid.x - me.x, mid.y - me.y);
      if (d < 45 || d > 280) continue;
      const i32 dir = rng_.unit() < 0.5f ? 1 : -1;
      const Vec2 dv = (dir > 0 ? bpt - a : a - bpt) * (1.0f / seg);
      const f32 off = std::fmin(L.width * 0.25f, 1.8f);
      const Vec3 pos{mid.x + dv.y * off, mid.y - dv.x * off, city_.ground_height(mid.x, mid.y) + Car::ground_offset(c.car->spec()) + 0.05f};
      // Nothing in the way (another car, a building corner).
      if (cast(world_->physics(), pos + Vec3{0, 0, 3}, {0, 0, -1}, 3.2f, {}).hit) continue;
      if (!world_->actor_alive(c.actor)) world_->set_alive(c.actor, true);
      const Vec3 rot{0, 0, std::atan2(dv.y, dv.x) / kDeg};
      world_->teleport(c.actor, pos, &rot);
      world_->set_actor_velocity(c.actor, {});
      c.car->reset();
      c.placed = true;
      c.driver = true;
      c.lane = li;
      c.dir = dir;
      c.index = dir > 0 ? k + 1 : k;
      c.stuck = 0;
      break;
    }
  }
}

// ---------------------------------------------------------------------------------------------
// input

void Game::read_input(kern::Engine& e, f32 dt) {
  kern::Input& in = e.input;
  using kern::Key;
  // Device arbitration (touch / mouse + keyboard / pad) as SpawnCity decides it.
  for (u32 k = 1; k < static_cast<u32>(Key::count); ++k)
    if (in.pressed(static_cast<Key>(k))) { arbiter_.key_down(); break; }
  if (pad.active) arbiter_.gamepad_used();
  touch_mode_ = arbiter_.touch();
  if (!touch_mode_ && in.button_pressed(0)) e.request_pointer_lock(true);

  const bool enter = in.pressed(Key::f) || in.pressed(Key::e) || pad.hit(kPadX) || pad_touch_.take_pressed("enter");
  const bool drone_key = in.pressed(Key::v) || pad.hit(kPadDown) || pad_touch_.take_pressed("mode") || pad_touch_.take_pressed("drone");
  const bool cam_key = in.pressed(Key::c) || pad.hit(kPadView) || pad_touch_.take_pressed("cam");
  if (enter) {
    if (mode_ == Mode::foot) try_enter();
    else if (mode_ == Mode::car || mode_ == Mode::jet) exit_vehicle();
  }
  if (drone_key) {
    if (mode_ == Mode::foot || mode_ == Mode::drone) toggle_drone();
    else if (mode_ == Mode::jet) jet_.toggle_mode();  // V in the jet: hover / flight (jet_mode.mjs)
  }
  if (cam_key) {
    if (mode_ == Mode::drone) { drone_cam_ = (drone_cam_ + 1) % 3; rig_init_ = false; }
    else if (mode_ == Mode::car) car_cockpit_ = !car_cockpit_;
    else if (mode_ == Mode::jet) jet_cockpit_ = !jet_cockpit_;
  }
  if (pad_touch_.take_pressed("vtol") && mode_ == Mode::jet) jet_.toggle_mode();

  play_in_ = PlayInput{};
  switch (mode_) {
    case Mode::foot: play_in_ = foot_input(e, dt); break;
    case Mode::drone: drone_input(e, dt); break;
    case Mode::car: car_input(e, dt); break;
    case Mode::jet: jet_input(e, dt); break;
    case Mode::parachute: parachute_input(e, dt); break;
  }
}

PlayInput Game::foot_input(kern::Engine& e, f32 dt) {
  using kern::Key;
  kern::Input& in = e.input;
  PlayInput p;
  p.move_x = (in.down(Key::d) ? 1.0f : 0.0f) - (in.down(Key::a) ? 1.0f : 0.0f);
  p.move_y = (in.down(Key::w) ? 1.0f : 0.0f) - (in.down(Key::s) ? 1.0f : 0.0f);
  p.jump = in.down(Key::space);
  p.boost = in.down(Key::shift_left) || in.down(Key::shift_right);
  p.crouch = in.down(Key::control_left);
  p.reload = in.pressed(Key::r);
  p.fire = in.button(0);
  p.aim = in.button(2);
  const Vec2 md = in.pointer_delta() * (1.0f / dpr_);
  p.look_x = md.x * kMouseRad / kDeg * sensitivity_;
  p.look_y = md.y * kMouseRad / kDeg * sensitivity_;
  if (pad.connected) {
    p.move_x = clampf(p.move_x + deadzone(pad.lx), -1, 1);
    p.move_y = clampf(p.move_y + deadzone(pad.ly), -1, 1);
    p.look_stick_x = deadzone(pad.rx, 0.05f);
    p.look_stick_y = -deadzone(pad.ry, 0.05f);
    p.jump |= pad.down(kPadA);
    p.boost |= pad.down(kPadL3);
    p.crouch |= pad.down(kPadB);
    p.reload |= pad.hit(kPadRB);
    p.fire |= pad.rt > 0.5f;
    p.aim |= pad.lt > 0.5f;
    p.gamepad = pad.active || p.gamepad;
  }
  if (touch_mode_) foot_touch_.update(dt, p, world_->hud().fov);
  return p;
}

void Game::drone_input(kern::Engine& e, f32 dt) {
  using kern::Key;
  kern::Input& in = e.input;
  const DroneTelemetry& t = drone_.telemetry();
  const Vec2 md = in.pointer_delta() * (1.0f / dpr_);
  // Sticks: W/S A/D move (GAME: velocity), Q/E and the mouse turn (rate), Space / Shift height.
  f32 roll = (in.down(Key::d) ? 1.0f : 0.0f) - (in.down(Key::a) ? 1.0f : 0.0f) + deadzone(pad.lx) + pad_touch_.left().x;
  f32 pitch = (in.down(Key::w) ? 1.0f : 0.0f) - (in.down(Key::s) ? 1.0f : 0.0f) + deadzone(pad.ly) + pad_touch_.left().y;
  // Turning right = negative yaw stick on this airframe (the FC's yaw axis).
  const f32 mouse_yaw = clampf(md.x * sensitivity_ / std::fmax(dt, 1e-3f) / 900.0f, -1, 1);
  drone_yaw_rate_ += (mouse_yaw - drone_yaw_rate_) * damp_alpha(20, dt);
  f32 yaw = -((in.down(Key::e) ? 1.0f : 0.0f) - (in.down(Key::q) ? 1.0f : 0.0f)) - drone_yaw_rate_ - deadzone(pad.rx) - pad_touch_.right().x;
  f32 axis = (in.down(Key::space) ? 1.0f : 0.0f) - (in.down(Key::shift_left) || in.down(Key::shift_right) ? 1.0f : 0.0f) + (pad.rt - pad.lt) +
             (touch_mode_ ? pad_touch_.right().y : 0.0f);
  axis = clampf(axis, -1, 1);
  // Body pitch (GAME channel 7): the mouse's vertical / the right stick's vertical, held.
  sticks_.body_pitch = clampf(sticks_.body_pitch - md.y * 0.004f * sensitivity_ - deadzone(pad.ry) * dt * 1.5f, -1, 1);
  if (in.pressed(Key::b)) sticks_.body_pitch = 0;
  sticks_.roll = clampf(roll, -1, 1);
  sticks_.pitch = clampf(pitch, -1, 1);
  sticks_.yaw = clampf(yaw, -1, 1);
  if (in.pressed(Key::g)) sticks_.game_mode = !sticks_.game_mode;  // GAME (height hold) / ANGLE (manual throttle)
  sticks_.kill = in.down(Key::k);

  // ARM (R / A / touch): arm on the ground; in the air it starts a controlled landing (simulator.mjs).
  const bool arm_key = in.pressed(Key::r) || pad.hit(kPadA) || pad_touch_.take_pressed("arm");
  const f32 agl = t.agl >= 0 ? t.agl : 0;
  if (arm_key) {
    if (!t.armed && !sticks_.arm) { sticks_.arm = true; landing_ = false; }
    else if (t.armed && agl > 0.7f) landing_ = true;
    else { sticks_.arm = false; landing_ = false; }
  }
  // Climbing on a disarmed, calibrated drone arms it (the touch ARM habit of SpawnCity's HUD).
  if (axis > 0.3f && !t.armed && !t.calibrating && !t.fault && !landing_) sticks_.arm = true;
  if (t.fault && !t.armed) sticks_.arm = false;  // the switch goes low for the power cycle

  if (sticks_.game_mode) {
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
      sticks_.clearance = have_agl ? clampf(agl + clearance_lead(rate), 0.5f, 120) : clampf(sticks_.clearance + axis * 12 * dt, 0.5f, 120);
    } else if (height_held_) {
      height_held_ = false;
      if (have_agl) sticks_.clearance = clampf(agl + clearance_lead(t.vz), 0.5f, 120);
    }
  } else {
    sticks_.throttle = clampf(sticks_.throttle + axis * dt * 0.8f, 0, 1);
  }
}

void Game::car_input(kern::Engine& e, f32 dt) {
  using kern::Key;
  kern::Input& in = e.input;
  if (car_index_ < 0) return;
  TrafficCar& c = cars_[car_index_];
  CarInput ci;
  // vehicle_control_settings.mjs: 115 km/h, reverse 32 km/h; player_car_mode.mjs: WASD, Space handbrake,
  // pad LS steer, RT gas, LT brake / reverse, B handbrake.
  ci.max_speed = 115 / 3.6f;
  ci.max_reverse = 32 / 3.6f;
  ci.pedal = clampf((in.down(Key::w) ? 1.0f : 0.0f) - (in.down(Key::s) ? 1.0f : 0.0f) + pad.rt - pad.lt + pad_touch_.right().y, -1, 1);
  ci.steer = clampf((in.down(Key::a) ? 1.0f : 0.0f) - (in.down(Key::d) ? 1.0f : 0.0f) - deadzone(pad.lx) - pad_touch_.left().x, -1, 1);
  ci.handbrake = in.down(Key::space) || pad.down(kPadB) || pad_touch_.held("hb");
  c.input = ci;
  // Head look in the cockpit (1.75 rad yaw, 0.7 rad pitch).
  const Vec2 md = in.pointer_delta() * (1.0f / dpr_);
  head_yaw_ = clampf(head_yaw_ - md.x * kMouseRad * sensitivity_ - deadzone(pad.rx) * dt * 2, -1.75f, 1.75f);
  head_pitch_ = clampf(head_pitch_ - md.y * kMouseRad * sensitivity_ + deadzone(pad.ry) * dt * 2, -0.7f, 0.7f);
  if (!in.button(2) && md.x == 0 && pad.rx == 0) head_yaw_ *= std::exp(-dt * 1.5f);
}

void Game::jet_input(kern::Engine& e, f32 dt) {
  using kern::Key;
  kern::Input& in = e.input;
  JetInput j;
  j.lift = clampf((in.down(Key::w) ? 1.0f : 0.0f) - (in.down(Key::s) ? 1.0f : 0.0f) + deadzone(pad.ly) + pad_touch_.left().y, -1, 1);
  j.yaw = clampf((in.down(Key::d) ? 1.0f : 0.0f) - (in.down(Key::a) ? 1.0f : 0.0f) + deadzone(pad.lx) + pad_touch_.left().x, -1, 1);
  // Mouse: rate command d/dt / 730 with a 25 ms lag (jet_mode.mjs:266-267); arrows as keys.
  const Vec2 md = in.pointer_delta() * (1.0f / dpr_);
  const f32 a = damp_alpha(1.0f / 0.025f, dt);
  jet_pitch_rate_ += (clampf(-md.y * sensitivity_ / std::fmax(dt, 1e-3f) / 730, -1, 1) - jet_pitch_rate_) * a;
  jet_roll_rate_ += (clampf(md.x * sensitivity_ / std::fmax(dt, 1e-3f) / 730, -1, 1) - jet_roll_rate_) * a;
  j.pitch = clampf(jet_pitch_rate_ + (in.down(Key::down) ? 1.0f : 0.0f) - (in.down(Key::up) ? 1.0f : 0.0f) - deadzone(pad.ry) + pad_touch_.right().y, -1, 1);
  j.roll = clampf(jet_roll_rate_ + (in.down(Key::right) ? 1.0f : 0.0f) - (in.down(Key::left) ? 1.0f : 0.0f) + deadzone(pad.rx) + pad_touch_.right().x, -1, 1);
  j.brake = jet_.telemetry().contacts > 0 && j.lift <= 0 ? 1.0f : 0.0f;
  if (pad.hit(kPadLB)) jet_.toggle_mode();
  jet_in_ = j;
}

void Game::parachute_input(kern::Engine& e, f32 dt) {
  using kern::Key;
  kern::Input& in = e.input;
  chute_steer_ = {clampf((in.down(Key::d) ? 1.0f : 0.0f) - (in.down(Key::a) ? 1.0f : 0.0f) + deadzone(pad.lx) + pad_touch_.left().x, -1, 1),
                  clampf((in.down(Key::w) ? 1.0f : 0.0f) - (in.down(Key::s) ? 1.0f : 0.0f) + deadzone(pad.ly) + pad_touch_.left().y, -1, 1)};
  // Look around while hanging under the canopy.
  PlayInput p;
  const Vec2 md = in.pointer_delta() * (1.0f / dpr_);
  head_yaw_ -= md.x * kMouseRad * sensitivity_ + deadzone(pad.rx) * dt * 2 + pad_touch_.right().x * dt * 2;
  (void)p;
  // Landed: on foot again where the pilot touched down.
  const BodyState s = state_of(chute_actor_);
  const RayResult g = cast(world_->physics(), s.position, {0, 0, -1}, 2.0f, body(chute_actor_));
  if (g.hit && g.distance < 1.0f && chute_time_ > 0.5f) {
    world_->set_alive(chute_actor_, false);
    show_soldier(g.point, world_->camera_yaw());
    mode_ = Mode::foot;
  }
}

void Game::set_touch_frame(f32 w, f32 h, f32 sl, f32 st, f32 sr, f32 sb, f32 dpr) {
  frame_w_ = w;
  frame_h_ = h;
  dpr_ = dpr > 0 ? dpr : 1;
  foot_touch_.set_frame(w, h, sl, st, sr, sb);
  pad_touch_.set_frame(w, h, sl, st, sr, sb);
}

bool Game::touch(u32 type, i32 id, f32 x, f32 y, f64 t_ms) {
  if (type == 0) arbiter_.pointer_down(true);
  if (pad_touch_.pointer(type, id, x, y)) return true;
  if (mode_ == Mode::foot) return foot_touch_.pointer(type, id, x, y, t_ms);
  return false;
}

void Game::update_touch_layout() {
  std::vector<PadButton> b;
  switch (mode_) {
    case Mode::foot: {
      b.push_back({"mode", "DROHNE", 0x17694f});
      // The context button appears near a vehicle (mobile_gameplay_ui.mjs).
      const Vec3 me = matrix_of(soldier_).translation();
      bool near = jet_placed_ && std::hypot(matrix_of(jet_actor_).translation().x - me.x, matrix_of(jet_actor_).translation().y - me.y) < kEnterJet;
      for (const TrafficCar& c : cars_)
        if (c.placed) { const Vec3 p = matrix_of(c.actor).translation(); near |= std::hypot(p.x - me.x, p.y - me.y) < kEnterCar; }
      if (near) b.push_back({"enter", "EINSTEIGEN", 0x7a4b12});
      pad_touch_.set_layout(false, b);
      break;
    }
    case Mode::drone:
      b.push_back({"mode", "ZU FUSS", 0x17694f});
      b.push_back({"arm", drone_.telemetry().armed ? "LANDEN" : "ARM", drone_.telemetry().armed ? 0x7a1a12u : 0x1a5a12u});
      b.push_back({"cam", "KAMERA", 0x0d1118});
      pad_touch_.set_layout(true, b, "FLUG", "HOEHE / DREHEN");
      break;
    case Mode::car:
      b.push_back({"enter", "AUSSTEIGEN", 0x7a4b12});
      b.push_back({"hb", "HANDBREMSE", 0x0d1118});
      b.push_back({"cam", "KAMERA", 0x0d1118});
      pad_touch_.set_layout(true, b, "LENKEN", "GAS / BREMSE");
      break;
    case Mode::jet:
      b.push_back({"enter", jet_.telemetry().contacts >= 2 ? "AUSSTEIGEN" : "EJECT", 0x7a1a12});
      b.push_back({"vtol", jet_.mode() == Jet::Mode::hover ? "FLUG" : "SCHWEBEN", 0x17694f});
      b.push_back({"cam", "KAMERA", 0x0d1118});
      pad_touch_.set_layout(true, b, "SCHUB / SEITE", "NICKEN / ROLLEN");
      break;
    case Mode::parachute: pad_touch_.set_layout(true, b, "STEUERN", "UMSEHEN"); break;
  }
}

std::string Game::touch_view_json() {
  if (!touch_mode_) return "null";
  return pad_touch_.view_json(mode_ == Mode::foot ? &foot_touch_.view() : nullptr);
}

// ---------------------------------------------------------------------------------------------
// frame

void Game::update(kern::Engine& e, const kern::Frame& f) {
  const f32 dt = f.dt;
  time_ = f.time;
  if (dt > 0) fps_ = fps_ * 0.95f + (1.0f / dt) * 0.05f;
  toast_age_ += dt;
  if (!world_placed_) {
    city_.update({0, 0, 0}, 8);
    if (city_.ready_at({0, 0, 0})) place_world();
    else {
      const StreamStats& s = city_.stats();
      char buf[128];
      std::snprintf(buf, sizeof buf, "SpawnCity: %s ... %u Gebaeude", city_.offline() ? "Ersatzstadt wird gebaut" : "Konstanz wird geladen", s.buildings);
      e.set_status(buf);
    }
  }
  if (world_placed_) {
    read_input(e, dt);
    update_touch_layout();
    world_->step(dt, play_in_);
    static f32 traffic_clock = 0;
    traffic_clock += dt;
    if (traffic_clock > 0.5f) { traffic_clock = 0; manage_traffic(); }
    city_.update(focus_position(), mobile_ ? 3 : 4);
    wanted_.update(dt, false);
    // Soldier died and respawned: GameWorld handles it (respawn at the spawn point).
  }
  update_camera(e, dt);
  e.renderer.camera = cam_;
  world_->set_view(cam_);
  world_->apply_lighting(cam_.eye);
  world_->present(f.time * 1000.0, cam_.eye);
  draw_vehicles();
  update_sound(dt);
}

void Game::update_camera(kern::Engine& e, f32 dt) {
  kern::Camera c = cam_;
  c.near = 0.05f;
  c.far = 2400;
  const f32 aspect = e.aspect();
  (void)aspect;
  if (!world_placed_) {
    // Overview while the city loads.
    const f32 a = static_cast<f32>(time_) * 0.05f;
    c.eye = {std::cos(a) * 220, std::sin(a) * 220, 140};
    c.target = {0, 0, 0};
    c.up = {0, 0, 1};
    c.fov = 50 * kDeg;
    cam_ = c;
    return;
  }
  auto rig = [&](Vec3 anchor, Vec3 vel, Vec2 heading, f32 pos_rate, f32 vel_rate, f32 head_rate, f32 max_lag) {
    // camera_stabilization.mjs: a damped inertial anchor and heading, lag capped.
    if (!rig_init_) { cam_anchor_ = anchor; cam_vel_ = vel; cam_heading_ = heading; rig_init_ = true; }
    cam_vel_ += (vel - cam_vel_) * damp_alpha(vel_rate, dt);
    const Vec3 predicted = cam_anchor_ + cam_vel_ * dt;
    cam_anchor_ = predicted + (anchor - predicted) * damp_alpha(pos_rate, dt);
    const Vec3 lag = anchor - cam_anchor_;
    const f32 l = kern::length(lag);
    if (l > max_lag) cam_anchor_ = anchor - lag * (max_lag / l);
    const f32 ha = std::atan2(cam_heading_.y, cam_heading_.x), hb = std::atan2(heading.y, heading.x);
    const f32 h = ha + wrap_pi(hb - ha) * damp_alpha(head_rate, dt);
    cam_heading_ = {std::cos(h), std::sin(h)};
  };
  auto collide = [&](Vec3 anchor, Vec3 want, kern::physics::Body ignore) {
    // Box3D sphere sweep from the anchor (camera_collision_guard / world_building_collision_physics).
    const Vec3 d = want - anchor;
    const kern::physics::RayHit h = world_->physics().cast_sphere(anchor, 0.17f, d, ~0ull, ignore);
    return h.hit ? anchor + d * std::fmax(0.0f, h.fraction - 0.02f) : want;
  };
  switch (mode_) {
    case Mode::foot:
      cam_valid_ = world_->update_camera(c, dt, play_in_);
      break;
    case Mode::drone: {
      const kern::Mat4 m = matrix_of(drone_actor_);
      const Vec3 pos = m.translation();
      const Quat q = state_of(drone_actor_).rotation;
      const Vec3 fwd = q.rotate({-1, 0, 0}), up = q.rotate({0, 0, 1});
      const Vec3 vel = world_->actor_velocity(drone_actor_);
      if (drone_cam_ == 0) {
        // FPV: rigid on the frame, tilted (camera_settings.mjs -15 .. 50 deg), FOV 105.
        const f32 t = fpv_tilt_deg_ * kDeg;
        const Vec3 f = fwd * std::cos(t) + up * std::sin(t), u = up * std::cos(t) - fwd * std::sin(t);
        c.eye = pos + fwd * 0.102f + up * 0.028f;
        c.target = c.eye + f * 4.0f;
        c.up = u;
        c.fov = fpv_fov_deg_ * kDeg;
        c.near = 0.02f;
      } else {
        Vec2 h{fwd.x, fwd.y};
        const f32 hl = std::sqrt(h.x * h.x + h.y * h.y);
        h = hl > 0.2f ? h * (1.0f / hl) : cam_heading_;
        const bool third = drone_cam_ == 2;
        rig(pos, vel, h, third ? 3.6f : 4.8f, third ? 5.2f : 7.0f, third ? 3.8f : 7.5f, third ? 0.36f : 0.18f);
        const f32 back = third ? 1.5f * 2.25f / 2.4847f : 1.65f, upm = third ? 1.5f * 1.05f / 2.4847f : 0.78f;
        const f32 ahead = third ? 0.55f : 0.38f, look_up = third ? 0.18f : 0.10f;
        const Vec3 hd{cam_heading_.x, cam_heading_.y, 0};
        const Vec3 want = cam_anchor_ - hd * back + Vec3{0, 0, upm};
        c.eye = collide(cam_anchor_, want, body(drone_actor_));
        c.target = cam_anchor_ + hd * ahead + Vec3{0, 0, look_up};
        c.up = {0, 0, 1};
        c.fov = clampf((third ? 62 : 52) * fpv_fov_deg_ / 105, 30, 100) * kDeg;
        c.near = 0.06f;
      }
      break;
    }
    case Mode::car: {
      if (car_index_ < 0) break;
      TrafficCar& tc = cars_[car_index_];
      const kern::Mat4 m = matrix_of(tc.actor);
      const BodyState s = state_of(tc.actor);
      const Vec3 pos = m.translation();
      const Vec3 fwd = s.rotation.rotate({1, 0, 0});
      const bool bus = tc.car->spec().radius > 0.4f;
      if (car_cockpit_) {
        const kern::Mat4 seat = m * kern::Mat4::translate(bus ? Vec3{3.3f, 0.72f, 2.05f - 1.48f} : Vec3{0.04f, 0.36f, 1.24f - 0.54f});
        c.eye = seat.translation();
        const Vec3 look = s.rotation.rotate({std::cos(head_yaw_) * std::cos(head_pitch_), std::sin(head_yaw_) * std::cos(head_pitch_), std::sin(head_pitch_)});
        c.target = c.eye + look * 5.0f;
        c.up = s.rotation.rotate({0, 0, 1});
        c.fov = 80 * kDeg * 0.75f;
        c.near = bus ? 0.22f : 0.04f;
      } else {
        // Chase (player_car_mode.mjs): 5.4 m back, 2.15 m up, heading smoothing 1 - exp(-6 dt); bus x 1.75.
        const f32 zoom = bus ? 1.75f : 1.0f;
        rig(pos, s.velocity, {fwd.x, fwd.y}, 9, 9, 6, 0.6f);
        const Vec3 hd = kern::normalize(Vec3{cam_heading_.x, cam_heading_.y, 0});
        const Vec3 look_dir = Quat::axis_angle({0, 0, 1}, head_yaw_).rotate(hd);
        const Vec3 want = cam_anchor_ - look_dir * (5.4f * zoom) + Vec3{0, 0, 2.15f * zoom};
        c.eye = collide(cam_anchor_ + Vec3{0, 0, 1.2f}, want, body(tc.actor));
        c.target = cam_anchor_ + look_dir * 2.0f + Vec3{0, 0, 0.9f * zoom};
        c.up = {0, 0, 1};
        c.fov = (58 + std::fmin(14.0f, std::fabs(tc.car->forward_speed()) * 0.25f)) * kDeg;
      }
      break;
    }
    case Mode::jet: {
      const kern::Mat4 m = matrix_of(jet_actor_);
      const BodyState s = state_of(jet_actor_);
      const Vec3 pos = m.translation();
      const Vec3 nose = s.rotation.rotate({0, 1, 0}), up = s.rotation.rotate({0, 0, 1});
      if (jet_cockpit_) {
        c.eye = (m * kern::Mat4::translate({0, 3.5f, 1.42f})).translation();
        c.target = c.eye + nose * 10.0f;
        c.up = up;
        c.fov = 72 * kDeg * 0.8f;
        c.near = 0.1f;
      } else {
        const f32 nl = std::hypot(nose.x, nose.y);
        rig(pos, s.velocity, nl > 0.2f ? Vec2{nose.x / nl, nose.y / nl} : cam_heading_, 6, 8, 4, 2.5f);
        const Vec3 hd{cam_heading_.x, cam_heading_.y, 0};
        const f32 pitch = std::asin(clampf(nose.z, -1, 1)) * 0.6f;
        const Vec3 back = hd * std::cos(pitch) + Vec3{0, 0, std::sin(pitch)};
        const Vec3 want = cam_anchor_ - back * 24.0f + Vec3{0, 0, 6.5f};
        c.eye = collide(cam_anchor_ + Vec3{0, 0, 2}, want, body(jet_actor_));
        c.target = cam_anchor_ + back * 12.0f + Vec3{0, 0, 2.0f};
        c.up = {0, 0, 1};
        c.fov = (60 + std::fmin(12.0f, kern::length(s.velocity) * 0.04f)) * kDeg;
      }
      c.far = 6000;
      break;
    }
    case Mode::parachute: {
      const Vec3 pos = matrix_of(chute_actor_).translation();
      const f32 yaw = world_->camera_yaw() * kDeg + head_yaw_;
      const Vec3 hd{std::cos(yaw), std::sin(yaw), 0};
      c.eye = collide(pos, pos - hd * 9.0f + Vec3{0, 0, 3.5f}, body(chute_actor_));
      c.target = pos + hd * 4.0f;
      c.up = {0, 0, 1};
      c.fov = 60 * kDeg;
      break;
    }
  }
  cam_ = c;
}

void Game::draw_vehicles() {
  art_.begin();
  if (world_placed_) {
    for (std::size_t i = 0; i < cars_.size(); ++i) {
      const TrafficCar& c = cars_[i];
      if (!c.placed || !world_->actor_alive(c.actor)) continue;
      const bool mine = static_cast<i32>(i) == car_index_ && mode_ == Mode::car;
      art_.car(matrix_of(c.actor), *c.car, c.color, mine && car_cockpit_, c.input.pedal < -0.05f);
    }
    if (jet_placed_) {
      art_.jet(matrix_of(jet_actor_), jet_.gear_down(), jet_.nozzle(), jet_.thrust() / jet_.spec().thrust_max, mode_ == Mode::jet && jet_cockpit_);
    }
    if (drone_out_) {
      Vec3 motors[4];
      f32 angles[4];
      for (int i = 0; i < 4; ++i) { motors[i] = drone_.motor_position(i); angles[i] = drone_.rotor_angle(i); }
      art_.drone(matrix_of(drone_actor_), motors, angles, drone_.params().prop_d, drone_.telemetry().armed, mode_ == Mode::drone && drone_cam_ == 0);
    }
    if (mode_ == Mode::parachute) art_.parachute(matrix_of(chute_actor_).translation() - Vec3{0, 0, 0.85f}, world_->camera_yaw() * kDeg, chute_open_);
  }
  art_.end();
}

// ---------------------------------------------------------------------------------------------
// audio

bool Game::audio_init(u32 rate) {
  if (mixer_.ready()) return true;
  if (!mixer_.init(rate, 64)) return false;
  bank_ = std::make_unique<kern::game::SoundBank>(mixer_);
  game_audio_ = std::make_unique<kern::game::GameAudio>(mixer_, *bank_);
  if (world_) bank_->request_level(world_->level());
  fx_.init(mixer_);
  return true;
}

const f32* Game::audio_render(u32 frames) {
  audio_out_.resize(static_cast<std::size_t>(frames) * 2);
  if (mixer_.ready()) mixer_.render(audio_out_.data(), frames);
  else std::fill(audio_out_.begin(), audio_out_.end(), 0.0f);
  return audio_out_.data();
}

void Game::update_sound(f32 dt) {
  if (!mixer_.ready() || !world_) return;
  bank_->prepare(3);
  game_audio_->update(*world_, cam_, dt);
  SoundState s;
  if (drone_out_) {
    const DroneTelemetry& t = drone_.telemetry();
    f32 w = 0;
    for (f32 o : t.omega) w += o;
    w /= 4;
    s.drone.active = w > 5;
    s.drone.position = matrix_of(drone_actor_).translation();
    s.drone.velocity = world_->actor_velocity(drone_actor_);
    s.drone.rate = clampf(w / 3600, 0, 1.2f);
    s.drone.load = clampf(t.current_a / 120, 0, 1);
  }
  if (mode_ == Mode::car && car_index_ >= 0) {
    const TrafficCar& c = cars_[car_index_];
    s.car.active = true;
    s.car.position = matrix_of(c.actor).translation();
    s.car.velocity = world_->actor_velocity(c.actor);
    s.car.rate = clampf(std::fabs(c.car->forward_speed()) / 32, 0, 1);
    s.car.load = c.car->throttle();
  }
  if (jet_placed_ && jet_.thrust() > 100) {
    s.jet.active = true;
    s.jet.position = matrix_of(jet_actor_).translation();
    s.jet.velocity = world_->actor_velocity(jet_actor_);
    s.jet.rate = clampf(jet_.thrust() / jet_.spec().thrust_dry, 0, 1.3f);
    s.jet.load = clampf(jet_.thrust() / jet_.spec().thrust_max, 0, 1);
  }
  s.wind = kern::length(world_->actor_velocity(mode_ == Mode::jet ? jet_actor_ : mode_ == Mode::parachute ? chute_actor_ : soldier_));
  fx_.update(s, dt);
}

// ---------------------------------------------------------------------------------------------
// HUD and commands

std::string Game::hud_json() {
  char b[512];
  std::string s = "{";
  auto add = [&](const char* fmt, auto... v) { std::snprintf(b, sizeof b, fmt, v...); s += b; };
  add("\"mode\":\"%s\",\"placed\":%s,\"offline\":%s,\"realData\":%s", mode_name(mode_), world_placed_ ? "true" : "false", city_.offline() ? "true" : "false",
      city_.has_real_data() ? "true" : "false");
  const StreamStats& st = city_.stats();
  add(",\"tiles\":{\"ready\":%u,\"loading\":%u,\"failed\":%u,\"buildings\":%u,\"colliders\":%u}", st.tiles_ready, st.tiles_loading, st.tiles_failed, st.buildings,
      st.colliders);
  add(",\"fps\":%.0f,\"touch\":%s,\"stars\":%d", fps_, touch_mode_ ? "true" : "false", wanted_.stars());
  const kern::game::Hud& h = world_->hud();
  add(",\"health\":%.0f,\"maxHealth\":%.0f,\"ammo\":%d,\"mag\":%d,\"reserve\":%d,\"reload\":%.2f,\"spread\":%.2f,\"aiming\":%s,\"respawn\":%.1f", h.health,
      h.max_health, h.ammo, h.magazine, h.reserve, h.reload, h.spread, h.aiming ? "true" : "false", h.respawn_in);
  Vec3 pos = focus_position(), vel;
  f32 agl = pos.z - city_.ground_height(pos.x, pos.y);
  switch (mode_) {
    case Mode::foot: vel = world_->actor_velocity(soldier_); agl -= 0.73f; break;
    case Mode::drone: {
      vel = world_->actor_velocity(drone_actor_);
      const DroneTelemetry& t = drone_.telemetry();
      agl = t.agl >= 0 ? t.agl : agl;
      add(",\"drone\":{\"armed\":%s,\"calibrating\":%s,\"fault\":%s,\"game\":%s,\"landing\":%s,\"battery\":%.2f,\"volts\":%.2f,\"amps\":%.1f,\"clearance\":%.1f,"
          "\"cam\":%d,\"pulses\":[%u,%u,%u,%u]}",
          t.armed ? "true" : "false", t.calibrating ? "true" : "false", t.fault ? "true" : "false", sticks_.game_mode ? "true" : "false",
          landing_ ? "true" : "false", t.battery_soc, t.battery_v, t.current_a, sticks_.clearance, drone_cam_, t.pulses[0], t.pulses[1], t.pulses[2],
          t.pulses[3]);
      break;
    }
    case Mode::car:
      if (car_index_ >= 0) {
        vel = world_->actor_velocity(cars_[car_index_].actor);
        add(",\"car\":{\"gear\":\"%s\",\"bus\":%s}", cars_[car_index_].car->gear() < 0 ? "R" : "D", cars_[car_index_].car->spec().radius > 0.4f ? "true" : "false");
      }
      break;
    case Mode::jet: {
      vel = world_->actor_velocity(jet_actor_);
      const JetTelemetry& t = jet_.telemetry();
      agl = t.agl;
      add(",\"jet\":{\"mode\":\"%s\",\"thrust\":%.2f,\"lever\":%.2f,\"mach\":%.2f,\"alpha\":%.1f,\"nz\":%.1f,\"gear\":%s,\"heading\":%.0f,\"pitch\":%.1f,\"roll\":%.1f,"
          "\"fuel\":%.2f}",
          jet_.mode() == Jet::Mode::hover ? "SCHWEBEN" : "FLUG", jet_.thrust() / jet_.spec().thrust_max, jet_.lever(), t.mach, t.alpha / kDeg, t.nz,
          jet_.gear_down() ? "true" : "false", std::fmod(-t.heading / kDeg + 360, 360.0f), t.pitch / kDeg, t.roll / kDeg, jet_.fuel);
      break;
    }
    case Mode::parachute: vel = world_->actor_velocity(chute_actor_); add(",\"chute\":%s", chute_open_ ? "true" : "false"); break;
  }
  add(",\"speed\":%.2f,\"vz\":%.2f,\"agl\":%.1f,\"alt\":%.1f", kern::length(vel), vel.z, agl, pos.z);
  f64 lon, lat;
  unproject(city_.projector(), pos.x, pos.y, lon, lat);
  add(",\"lon\":%.6f,\"lat\":%.6f", lon, lat);
  // Context hint.
  hint_.clear();
  if (mode_ == Mode::foot && world_placed_) {
    const Vec3 me = matrix_of(soldier_).translation();
    if (jet_placed_ && std::hypot(matrix_of(jet_actor_).translation().x - me.x, matrix_of(jet_actor_).translation().y - me.y) < kEnterJet) hint_ = "F: Jet fliegen";
    for (const TrafficCar& c : cars_) {
      if (!c.placed) continue;
      const Vec3 p = matrix_of(c.actor).translation();
      if (std::hypot(p.x - me.x, p.y - me.y) < kEnterCar) { hint_ = c.driver ? "F: Auto klauen" : "F: Einsteigen"; break; }
    }
  }
  if (mode_ == Mode::drone && drone_.telemetry().calibrating) hint_ = "Flugregler kalibriert - stillhalten";
  else if (mode_ == Mode::drone && !drone_.telemetry().armed) hint_ = "R / Leertaste: ARM und steigen";
  s += ",\"hint\":\"" + json_escape(hint_) + "\"";
  if (toast_age_ < 4) s += ",\"toast\":\"" + json_escape(toast_) + "\"";
  return s + "}";
}

std::string Game::command(const std::string& cmd) {
  if (!world_) return "no world";
  auto starts = [&](const char* p) { return cmd.rfind(p, 0) == 0; };
  if (starts("mode ")) {
    const std::string m = cmd.substr(5);
    if (!world_placed_) return "not placed";
    if (m == "foot") {
      if (mode_ == Mode::drone) toggle_drone();
      else if (mode_ == Mode::car || mode_ == Mode::jet) exit_vehicle();
      return mode_name(mode_);
    }
    if (mode_ != Mode::foot) command("mode foot");
    if (mode_ != Mode::foot) return "busy";
    if (m == "drone") { toggle_drone(); return mode_name(mode_); }
    const Vec3 me = matrix_of(soldier_).translation();
    if (m == "jet" && jet_placed_) {
      // Walk up to the jet (debug / tests): stand next to it, then board.
      const Vec3 j = matrix_of(jet_actor_).translation();
      world_->teleport(soldier_, {j.x + 4, j.y, 0.73f});
      switch_mode(Mode::jet);
      return mode_name(mode_);
    }
    if (m == "car") {
      i32 best = -1;
      f32 bd = 1e9f;
      for (std::size_t i = 0; i < cars_.size(); ++i) {
        if (!cars_[i].placed || cars_[i].car->spec().radius > 0.4f) continue;
        const Vec3 p = matrix_of(cars_[i].actor).translation();
        const f32 d = std::hypot(p.x - me.x, p.y - me.y);
        if (d < bd) { bd = d; best = static_cast<i32>(i); }
      }
      if (best < 0) return "no car";
      switch_mode(Mode::car, best);
      return mode_name(mode_);
    }
    return "unknown mode";
  }
  if (cmd == "eject" && mode_ == Mode::jet) { eject(); return "ok"; }
  if (starts("time ")) {
    world_->level();  // time of day is the world's; Kern advances it while playing
    return "ok";
  }
  if (cmd == "offline") { city_.go_offline(); return "ok"; }
  if (cmd == "state") return hud_json();
  return "unknown command";
}

}  // namespace sc
