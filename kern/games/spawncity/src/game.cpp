// SpawnCity on Kern: world, modes, fixed-step hook, cameras (see game.h).
#include "game.h"
#include "game_world.h"  // WorldLife (M3 world state)
#include "game_life.h"   // GameLife (M3 life)

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
constexpr f32 kPlayerHealth = 100 * 1.18f;   // player_vitals_runtime.mjs: +18 % tougher
constexpr f32 kChuteOpen = 1.1f, kChuteSink = 6.5f, kChuteSteer = 4, kEjectUp = 28;  // jet_mode.mjs

const char* kNodeType = "spawncity.fixed_step";

Vec3 car_color(u32 i) {
  static const Vec3 c[] = {{0.62f, 0.05f, 0.04f}, {0.08f, 0.16f, 0.42f}, {0.85f, 0.85f, 0.86f}, {0.06f, 0.06f, 0.07f}, {0.45f, 0.47f, 0.5f},
                           {0.12f, 0.32f, 0.18f}, {0.9f, 0.62f, 0.08f}, {0.3f, 0.5f, 0.75f}, {0.55f, 0.36f, 0.22f}, {0.7f, 0.7f, 0.66f}};
  return c[i % (sizeof c / sizeof c[0])];
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
  l.world.ground = false;  // the terrain's ground tiles (terrain.h) are the ground
  l.world.ground_color = {0.36f, 0.40f, 0.31f};
  l.world.ground_friction = 0.8f;
  l.world.kill_z = -3000;  // real terrain lies below the origin in places (heights are relative to it)
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
  // Police cruisers (police_ground_units.mjs: at most three units).
  for (u32 i = 0; i < 3; ++i) {
    char name[32];
    std::snprintf(name, sizeof name, "Police %u", i + 1);
    l.add(vehicle_actor(name, Car::rigid_spec(CarSpec::car()), {static_cast<f32>(i) * 10, 20, -400}, 0));
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
  ctl_.dev.platform(!coarse, coarse, coarse ? 5 : 0);  // the page reports the real device (game_controls.cpp)
  soldier_controller_ = kern::game::ControllerComponent{};
  soldier_controller_.mode = kern::game::ControlMode::soldier;
  soldier_controller_.camera = kern::game::CameraMode::first_person;
  soldier_controller_.max_speed = 5.5f;
  soldier_controller_.boost_speed = 8.1f;
  soldier_controller_.jump_speed = 3.6f;
  soldier_controller_.eye_height = 1.68f;
  soldier_controller_.camera_fov = 59;  // 90 degrees across a 16:9 view (camera_pitch_contract.mjs)

  life_ = std::make_unique<WorldLife>();  // M3 world state (game_world.h)
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
  for (u32 i = 0; i < 3; ++i) {
    char name[32];
    std::snprintf(name, sizeof name, "Police %u", i + 1);
    TrafficCar c;
    c.actor = world_->find_actor(name);
    c.car = std::make_unique<Car>(CarSpec::car());
    c.color = {0.93f, 0.94f, 0.96f};
    c.police = true;
    c.driver = false;
    cars_.push_back(std::move(c));
  }
  for (ActorId a : {drone_actor_, jet_actor_, chute_actor_}) world_->set_alive(a, false);
  for (TrafficCar& c : cars_) world_->set_alive(c.actor, false);
  world_->set_paused(true);  // until the ground under the player is built

  // Elevation from the Terrarium tiles unless offline or ?dem=0 (terrain.h).
  city_.start(e.renderer, world_->physics(), lon_, lat_, e.arg_number("dem", 1) > 0 && e.arg_number("offline", 0) <= 0);
  if (e.arg_number("offline", 0) > 0) city_.go_offline();
  if (!art_.init(e.renderer)) return false;
  if (!people_art_.init(e.renderer)) return false;
  if (!combat_art_.init(e.renderer)) return false;  // combat and events (game_military.cpp)
  gaps_init(e);  // sc-m3-gaps (game_gaps.cpp)
  people_.set_budget(mobile_ ? 44 : 80);
  init_weapons();
  e.set_status("SpawnCity: Karte wird geladen ...");
  cam_.eye = {-30, -40, 60};
  cam_.target = {0, 0, 0};
  e.renderer.camera = cam_;
  return true;
}

void Game::stop() {
  glife_.reset();  // M3 life: its bodies live in the world
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
      Vec3 c{center.x + r * std::cos(a), center.y + r * std::sin(a), 0};
      c.z = city_.ground_height(c.x, c.y);
      bool ok = true;
      for (int ix = -2; ix <= 2 && ok; ++ix)
        for (int iy = -2; iy <= 2 && ok; ++iy) {
          const Vec3 o{c.x + half.x * static_cast<f32>(ix) * 0.5f, c.y + half.y * static_cast<f32>(iy) * 0.5f, c.z + 80};
          const RayResult h = cast(w, o, {0, 0, -1}, 90, {});
          ok = h.hit && h.point.z < city_.ground_height(o.x, o.y) + 0.3f && !city_.terrain().water_at(o.x, o.y);
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
  // Offline the HUD keeps its own banner; with map data a short greeting.
  toast_ = city_.offline() ? "" : "Willkommen in SpawnCity";
  toast_age_ = city_.offline() ? 99 : 0;
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
  if (gaps_riding()) return;  // riding in a mate's jet (game_seats.cpp)
  if (mode_ == Mode::drone) {
    // Stow the drone (killDrone): back to the pilot.
    world_->set_alive(drone_actor_, false);
    drone_out_ = false;
    world_->set_mode(kern::game::SimMode::play);
    mode_ = Mode::foot;
    return;
  }
  if (mode_ != Mode::foot) return;
  // player_vitals_logic.mjs: a shot-down drone is replaced after 8 s
  if (world_->sim_time() < combat_.drone_ready_at) { toast_ = "Neue Drohne in " + std::to_string(static_cast<int>(std::ceil(combat_.drone_ready_at - world_->sim_time()))) + " s"; toast_age_ = 0; return; }
  const Vec3 feet = matrix_of(soldier_).translation() - Vec3{0, 0, 0.73f};
  const f32 yaw = world_->camera_yaw() * kDeg;
  // 2.9 m ahead of the pilot; if something hangs over that spot (a wing, a balcony) or stands
  // there, the next free side (left, right, behind) - the drone needs open sky to take off.
  Vec3 at = feet + Vec3{std::cos(yaw), std::sin(yaw), 0} * kDroneAhead;
  for (f32 turn : {0.0f, 0.5f * kPi, -0.5f * kPi, kPi}) {
    const Vec3 c = feet + Vec3{std::cos(yaw + turn), std::sin(yaw + turn), 0} * kDroneAhead;
    const bool open = !cast(world_->physics(), c + Vec3{0, 0, 0.3f}, {0, 0, 1}, 25, body(soldier_)).hit &&
                      !cast(world_->physics(), feet + Vec3{0, 0, 0.3f}, kern::normalize(c - feet), kDroneAhead + 0.3f, body(soldier_)).hit;
    if (open) { at = c; break; }
  }
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
        eject_driver(vehicle);  // M3: the driver gets out and runs (game_traffic.cpp)
        toast_ = "Auto geklaut! Der Fahrer ist raus.";
        toast_age_ = 0;
      }
      c.driver = false;
      car_index_ = vehicle;
      // a parked car may be asleep in Box3D: the throttle must reach it at once
      if (world_->physics().body_valid(body(c.actor))) world_->physics().set_awake(body(c.actor), true);
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
  if (gaps_seat_press()) return;  // a mate's jet as passenger, or out of it (game_seats.cpp)
  if (gaps_repair_barricade()) return;  // zombie night: F boards a door up again (game_barricades.cpp)
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
  step_world(h);  // M3 world: buoyancy, water drag (game_world.cpp)
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
    if ((static_cast<i32>(i) == car_index_ && mode_ == Mode::car) || (c.police && c.unit >= 0 && !c.wreck)) {
      c.car->step(w, b, h, c.input);
      continue;
    }
    if (c.wreck || !c.driver || c.lane < 0 || c.lane >= static_cast<i32>(lanes.size()) || !lanes[c.lane].alive) {
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
    // Approaching the end of the street (a junction): slow down to turn (SpawnCity aiInput).
    f32 want_speed = c.target_speed;
    {
      const Vec2 end = c.dir > 0 ? L.points.back() : L.points.front();
      const f32 to_end = std::hypot(end.x - s.position.x, end.y - s.position.y);
      if (to_end < 18) want_speed = std::fmin(want_speed, 4.5f + to_end * 0.3f);
    }
    want_speed = std::fmin(want_speed, traffic_rules(static_cast<i32>(i), s.position, fwd, c.car->forward_speed()));  // M3 lights / right of way
    CarInput in = ai_drive(s.position, fwd, c.car->forward_speed(), target, want_speed);
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
    // jet_mode.mjs stepPara: v.x -> rollIn * 4, v.y -> -pitchIn * 4 (world axes), at 0.8 /s
    // (chute_steer_ = {rollIn, -pitchIn}, game_controls.cpp)
    const Vec2 want = chute_steer_ * kChuteSteer;
    f.x = m * 0.8f * (want.x - s.velocity.x);
    f.y = m * 0.8f * (want.y - s.velocity.y);
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
    if (c.police) {
      // An abandoned cruiser far away is put back into the pool.
      if (c.placed && c.unit < 0 && std::hypot(matrix_of(c.actor).translation().x - me.x, matrix_of(c.actor).translation().y - me.y) > 420) {
        world_->set_alive(c.actor, false);
        c.placed = false;
        c.wreck = false;
        c.hp = 300;
      }
      continue;
    }
    bool replace = !c.placed;
    if (c.placed) {
      const Vec3 p = matrix_of(c.actor).translation();
      const f32 d = std::hypot(p.x - me.x, p.y - me.y);
      // Far away (and so out of sight), fallen through, or its street unloaded: bring it back.
      replace = d > 420 || p.z < city_.ground_height(p.x, p.y) - 5 || (c.driver && (c.lane < 0 || c.lane >= static_cast<i32>(lanes.size()) || !lanes[c.lane].alive) && d > 120);
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
      // a spot along the segment (a long street segment's middle can be far away while the street
      // itself runs right past the player: the synthetic test town, long OSM ways)
      f32 along = seg * 0.5f;
      if (seg > 16) {
        const f32 t0 = clampf(((me.x - a.x) * (bpt.x - a.x) + (me.y - a.y) * (bpt.y - a.y)) / seg, 0, seg);  // nearest to the player
        along = clampf(t0 + (rng_.unit() < 0.5f ? -1.0f : 1.0f) * rng_.range(45, 240), 4, seg - 4);
      }
      const Vec2 mid = a + (bpt - a) * (along / seg);
      const f32 d = std::hypot(mid.x - me.x, mid.y - me.y);
      if (d < 45 || d > 280) continue;
      const i32 dir = rng_.unit() < 0.5f ? 1 : -1;
      const Vec2 dv = (dir > 0 ? bpt - a : a - bpt) * (1.0f / seg);
      const f32 off = std::fmin(L.width * 0.25f, 1.8f);
      const Vec3 pos{mid.x + dv.y * off, mid.y - dv.x * off, city_.ground_height(mid.x, mid.y) + Car::ground_offset(c.car->spec()) + 0.05f};
      // Nothing in the way (another car, a building corner).
      if (cast(world_->physics(), pos + Vec3{0, 0, 3}, {0, 0, -1}, 3.2f, {}).hit) continue;
      if (!gaps_vehicle_spawn_ok(static_cast<i32>(i), pos, std::atan2(dv.y, dv.x))) continue;  // no overlap, unseen (game_gaps.cpp)
      if (!world_->actor_alive(c.actor)) world_->set_alive(c.actor, true);
      const Vec3 rot{0, 0, std::atan2(dv.y, dv.x) / kDeg};
      world_->teleport(c.actor, pos, &rot);
      world_->set_actor_velocity(c.actor, {});
      c.car->reset();
      c.placed = true;
      c.driver = true;
      c.hp = 300;
      c.wreck = false;
      c.hit_by_player = false;
      c.burn_until = -1;
      c.lane = li;
      c.dir = dir;
      c.index = dir > 0 ? k + 1 : k;
      c.stuck = 0;
      break;
    }
  }
}

// input: read_input, foot / drone / car / jet / parachute input, touch: game_controls.cpp

// ---------------------------------------------------------------------------------------------
// frame

void Game::update(kern::Engine& e, const kern::Frame& f) {
  const f32 dt = f.dt;
  time_ = f.time;
  if (dt > 0) fps_ = fps_ * 0.95f + (1.0f / dt) * 0.05f;
  toast_age_ += dt;
  look_begin(e, f);  // frame times, automatic quality, day clock (game_look.cpp)
  update_net(dt);
  ui_update(dt);  // settings, WORLD options, menus (game_ui.cpp)
  if (!world_placed_) {
    city_.update({0, 0, 0}, 24);  // loading behind the title: nothing to keep smooth (the title draws no 3D)
    if (city_.ready_at({0, 0, 0})) place_world();
    else {
      const StreamStats& s = city_.stats();
      char buf[128];
      std::snprintf(buf, sizeof buf, "SpawnCity: %s ... %u Gebaeude", city_.offline() ? "Ersatzstadt wird gebaut" : "Konstanz wird geladen", s.buildings);
      e.set_status(buf);
    }
  }
  if (world_placed_ && !ui_.paused) {  // a menu is open: nothing moves (game_ui.cpp)
    mp_input(e, dt);  // multiplayer keys (voice, ping, big map, vote): game_net.cpp
    read_input(e, dt);
    update_touch_layout();
    look_lap(perf::Slot::logic);
    world_->step(dt, play_in_);
    look_lap(perf::Slot::physics);
    static f32 traffic_clock = 0;
    traffic_clock += dt;
    if (traffic_clock > 0.5f && ui_.settings.on(Opt::world_traffic)) { traffic_clock = 0; manage_traffic(); }
    city_.anchor = player_position();  // ground collision stays under the pilot too
    city_.update(focus_position(), mobile_ ? 3 : 4);
    look_lap(perf::Slot::stream);
    update_town(dt);
    update_projectiles(dt);
    update_world(dt);  // M3 world (game_world.cpp)
    life_update(dt);   // M3 life: animals, street life, ragdolls, interiors (game_life.cpp)
    update_guided(dt);   // GUIDED missiles (game_guided.cpp)
    update_details(dt);  // critical damage, ballots (game_details.cpp)
    gaps_update(dt);  // dispatch, critical smoke, organic hits, impact bursts (game_gaps.cpp)
    update_combat(dt);  // military, flak, jets, fights, zombies, enemies, race (game_military.cpp, game_events.cpp)
    // Soldier died and respawned: GameWorld handles it (respawn at the spawn point).
  }
  look_lap(perf::Slot::logic);
  update_camera(e, dt);
  ui_camera(dt);  // LIVE GPS views (game_ui.cpp)
  look_camera(dt);  // FPV stabilizer, camera shake, speed (game_look.cpp)
  e.renderer.camera = cam_;
  world_->set_view(cam_);
  night_ = world_->apply_lighting(cam_.eye);
  look_lighting(dt);  // own day clock, point lights, quality (game_look.cpp)
  world_->present(f.time * 1000.0, cam_.eye);
  draw_vehicles();
  draw_town();
  draw_world();  // M3 world (game_world.cpp)
  life_draw();   // M3 life (game_life.cpp)
  draw_combat();
  gaps_draw();  // critical smoke (game_gaps.cpp)
  look_draw(e, dt);  // speed streaks, weapon models, frame times (game_look.cpp)
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
        c.eye = pos + fwd * 0.102f + up * (0.028f + 0.045f);  // + fpv_view_height.mjs FPV_VIEW_EXTRA_UP_M
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
        // zoom (pinch / wheel / + -, player_car_mode.mjs): distance 5.4 z, height 2.15 (.55 + .45 z)
        const f32 cz = ctl_.car_zoom;
        const Vec3 want = cam_anchor_ - look_dir * (5.4f * zoom * cz) + Vec3{0, 0, 2.15f * zoom * (0.55f + 0.45f * cz)};
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
  ctl_camera(c);  // the shared head look in the drone, jet and parachute (game_controls.cpp)
  gaps_camera(c);  // camera_collision_guard.mjs, riding in a mate's jet (game_gaps.cpp)
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
      art_.drone(gaps_drone_matrix(), motors,  // visual_pose_stabilization.mjs (game_gaps.cpp)
                 angles, drone_.params().prop_d, drone_.telemetry().armed, mode_ == Mode::drone && drone_cam_ == 0);
    }
    if (mode_ == Mode::parachute) art_.parachute(matrix_of(chute_actor_).translation() - Vec3{0, 0, 0.85f}, world_->camera_yaw() * kDeg, chute_open_);
    draw_remote_vehicles();
    draw_world_vehicles();  // M3: tow trucks (game_traffic.cpp)
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
  ui_audio_init();  // jetpack, soundscape, music (game_equipment.cpp)
  combat_audio_init();  // combat_audio_bank.mjs sounds (game_details.cpp)
  return true;
}

const f32* Game::audio_render(u32 frames) {
  audio_out_.resize(static_cast<std::size_t>(frames) * 2);
  if (mixer_.ready()) {
    mixer_.render(audio_out_.data(), frames);
    ui_render_music(audio_out_.data(), frames);  // title music and car radio (game_equipment.cpp)
  }
  else std::fill(audio_out_.begin(), audio_out_.end(), 0.0f);
  mix_voice(audio_out_.data(), frames);  // proximity voice of the other players (game_net.cpp)
  return audio_out_.data();
}

void Game::update_sound(f32 dt) {
  if (!mixer_.ready() || !world_) return;
  bank_->prepare(3);
  ui_mix_cues();  // settings: shots / steps before GameAudio plays the cues (game_ui.cpp)
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
  gaps_sirens(dt);  // police_sirens.mjs: horn and drone whoop (game_gaps.cpp)
  gaps_jet_audio(dt);  // fighter_jets.mjs: roar with retarded time and Doppler (game_gaps.cpp)
  s.wind = kern::length(world_->actor_velocity(mode_ == Mode::jet ? jet_actor_ : mode_ == Mode::parachute ? chute_actor_ : soldier_));
  ui_mix_audio(s);  // settings: audio sliders (game_ui.cpp)
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
  hud_world(s);  // terrain, water, animals, ... (game_world.cpp)
  life_hud(s);   // animals, street life, ragdolls, interiors (game_life.cpp)
  add(",\"fps\":%.0f,\"touch\":%s,\"stars\":%d,\"heat\":%.1f", fps_, touch_mode_ ? "true" : "false", wanted_.stars(), wanted_.heat());
  {
    u32 units = 0;
    for (const PoliceUnit& u : units_) units += u.car >= 0 ? 1 : 0;
    add(",\"people\":%u,\"officers\":%u,\"police\":%u", people_.count(People::Kind::citizen), people_.count(People::Kind::officer), units);
    if (mode_ == Mode::car && car_index_ >= 0) add(",\"carHp\":%.0f", cars_[car_index_].hp);
    const f32 now = world_->sim_time();
    add(",\"weapon\":\"%s\",\"grenades\":%d,\"emp\":%.1f,\"projectiles\":%u", mode_ == Mode::drone ? (drone_weapon_ == 0 ? "MG" : drone_weapon_ == 1 ? "Raketen" : "GUIDED")
        : mode_ == Mode::jet ? "Bordkanone" : weapons::spec(foot_weapon_).name, grenades_, std::fmax(0.0f, emp_ready_at_ - now), static_cast<u32>(projectiles_.size()));
    u32 drones = 0;
    for (const PoliceDrone& d : pdrones_) drones += d.active ? 1 : 0;
    add(",\"policeDrones\":%u,\"explosions\":%u", drones, explosions_);
    if (net_.active()) {
      add(",\"net\":{\"host\":%s,\"connected\":%s,\"slot\":%u,\"players\":%zu,\"status\":\"%s\"}", net_.hosting() ? "true" : "false", net_.connected() ? "true" : "false",
          net_.self_slot(), net_.remotes().size() + 1, json_escape(net_.status()).c_str());
    }
    s += mp_hud_json();  // multiplayer HUD data (game_net.cpp)
    if (mode_ == Mode::drone) add(",\"droneHp\":%.0f", drone_hp_);
    if (mode_ == Mode::jet) add(",\"jetHp\":%.0f", jet_hp_);
  }
  const kern::game::Hud& h = world_->hud();
  // Launchers and melee are the game's: their rounds (melee: none) instead of Kern's magazine.
  i32 ammo = h.ammo, mag = h.magazine, reserve = h.reserve;
  // gameplay_final_runtime_v2.mjs: the weapons on foot have no magazines and no ammo counter (-1 hides it)
  ammo = mag = reserve = -1;
  (void)h.ammo;
  add(",\"health\":%.0f,\"maxHealth\":%.0f,\"ammo\":%d,\"mag\":%d,\"reserve\":%d,\"reload\":%.2f,\"spread\":%.2f,\"aiming\":%s,\"respawn\":%.1f", h.health,
      h.max_health, ammo, mag, reserve, h.reload, h.spread, h.aiming ? "true" : "false", h.respawn_in);
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
      {
        // tests: why a car does not drive (throttle, wheels on the ground, height over the ground, how far it was jacked from)
        const Car& cc = *cars_[car_index_].car;
        const Vec3 cp = matrix_of(cars_[car_index_].actor).translation();
        i32 contacts = 0;
        for (int w = 0; w < 4; ++w) contacts += cc.wheel(w).contact ? 1 : 0;
        add(",\"car\":{\"gear\":\"%s\",\"bus\":%s,\"throttle\":%.2f,\"vf\":%.2f,\"contacts\":%d,\"agl\":%.2f,\"jacked\":%.0f,\"awake\":%s}", cc.gear() < 0 ? "R" : "D",
            cc.spec().radius > 0.4f ? "true" : "false", cc.throttle(), cc.forward_speed(), contacts, cp.z - city_.ground_height(cp.x, cp.y), jacked_at_,
            world_->physics().body_valid(body(cars_[car_index_].actor)) && world_->physics().awake(body(cars_[car_index_].actor)) ? "true" : "false");
      }
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
  add(",\"speed\":%.2f,\"vz\":%.2f,\"agl\":%.1f,\"alt\":%.1f,\"t\":%.3f", kern::length(vel), vel.z, agl, pos.z, world_->sim_time());
  f64 lon, lat;
  unproject(city_.projector(), pos.x, pos.y, lon, lat);
  add(",\"lon\":%.6f,\"lat\":%.6f", lon, lat);
  // Context hint.
  hint_.clear();
  if (mode_ == Mode::foot && world_placed_) {
    const Vec3 me = matrix_of(soldier_).translation();
    // player_car_mode.mjs / jet_mode.mjs renderButtons: the key of the device in use (touch: the round buttons)
    const char* k = ctl_.dev.pad ? "\u24CD  " : "F · ";
    if (!touch_mode_) {
      if (jet_placed_ && std::hypot(matrix_of(jet_actor_).translation().x - me.x, matrix_of(jet_actor_).translation().y - me.y) < kEnterJet) hint_ = std::string(k) + "\u2708 JET EINSTEIGEN";
      for (const TrafficCar& c : cars_) {
        if (!c.placed) continue;
        const Vec3 p = matrix_of(c.actor).translation();
        if (std::hypot(p.x - me.x, p.y - me.y) < kEnterCar) { hint_ = std::string(k) + "\U0001F697 EINSTEIGEN"; break; }
      }
    }
  }
  if (mode_ == Mode::drone && drone_.telemetry().calibrating) hint_ = "Flugregler kalibriert - stillhalten";
  combat_hud(s);  // "combat":{...} (game_events.cpp)
  guided_hud(s);   // "guided":{...} (game_guided.cpp)
  details_hud(s);  // "details":{...} (game_details.cpp)
  gaps_hud(s);  // "gaps":{...} (game_gaps.cpp)
  s += ",\"hint\":\"" + json_escape(hint_) + "\"";
  if (toast_age_ < 4) s += ",\"toast\":\"" + json_escape(toast_) + "\"";
  s += ui_hud_json();  // menus, settings, equipment (game_ui.cpp)
  s += look_hud_json();  // frame times, quality, time of day (game_look.cpp)
  s += ctl_hud_json();   // "ctl":{...} device, hint line, aiming (game_controls.cpp)
  // JSON allows no raw control characters (a line break in a hint or name broke JSON.parse on the
  // page); outside strings none are ever written, so every one becomes a space.
  for (char& c : s)
    if (static_cast<unsigned char>(c) < 0x20) c = ' ';
  return s + "}";
}

std::string Game::command(const std::string& cmd) {
  if (!world_) return "no world";
  if (std::string out; ctl_command(cmd, out)) return out;  // controls / devices (game_controls.cpp)
  if (std::string out; ui_command(cmd, out)) return out;  // settings, menus, start, equipment (game_ui.cpp)
  if (std::string out; combat_command(cmd, out)) return out;  // combat / event commands (game_events.cpp)
  if (std::string out; details_command(cmd, out)) return out;  // guided / critical / ballot commands (game_details.cpp)
  if (std::string out; gaps_command(cmd, out)) return out;  // sc-m3-gaps test commands (game_gaps.cpp)
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
      world_->teleport(soldier_, {j.x + 4, j.y, city_.ground_height(j.x + 4, j.y) + 0.73f});
      switch_mode(Mode::jet);
      return mode_name(mode_);
    }
    if (m == "car") {
      i32 best = -1;
      f32 bd = 1e9f;
      for (std::size_t i = 0; i < cars_.size(); ++i) {
        // A drivable one: not a bus, not burning or burnt out (the nearest after a fight may be).
        if (!cars_[i].placed || cars_[i].car->spec().radius > 0.4f || cars_[i].wreck || cars_[i].burn_until >= 0 || cars_[i].police) continue;
        const Vec3 p = matrix_of(cars_[i].actor).translation();
        f32 d = std::hypot(p.x - me.x, p.y - me.y);
        // tests: rather one on its four wheels and rolling than one stuck on rubble or nosed into a
        // wall (CI: 0 wheels on the ground 2.4 m up / standing with the throttle on)
        i32 on = 0;
        for (int w = 0; w < 4; ++w) on += cars_[i].car->wheel(w).contact ? 1 : 0;
        const f32 agl = p.z - city_.ground_height(p.x, p.y);
        const f32 score = d + (on < 4 || agl > 1.2f ? 2000.0f : 0.0f) + (std::fabs(cars_[i].car->forward_speed()) < 0.5f ? 400.0f : 0.0f);
        if (score < bd) { bd = score; best = static_cast<i32>(i); }
      }
      if (best < 0) {
        // Tests: say why (placed / bus / wrecked / burning / police / alive in the world)
        u32 placed = 0, bus = 0, wreck = 0, burn = 0, police = 0, alive = 0;
        for (const TrafficCar& c : cars_) {
          placed += c.placed; bus += c.car->spec().radius > 0.4f; wreck += c.wreck; burn += c.burn_until >= 0; police += c.police;
          alive += world_->actor_alive(c.actor);
        }
        char why[160];
        std::snprintf(why, sizeof why, "no car (of %zu: placed %u, bus %u, wreck %u, burning %u, police %u, alive %u, lanes %zu)", cars_.size(), placed, bus, wreck, burn, police, alive,
                      city_.lanes().size());
        return why;
      }
      {
        const Vec3 p = matrix_of(cars_[best].actor).translation();
        jacked_at_ = std::hypot(p.x - me.x, p.y - me.y);  // tests: how far away the car was
      }
      switch_mode(Mode::car, best);
      return mode_name(mode_);
    }
    return "unknown mode";
  }
  if (cmd == "eject" && mode_ == Mode::jet) { eject(); return "ok"; }
  if (cmd == "cam") {
    // Same as C / View: the mode's next camera.
    if (mode_ == Mode::drone) { drone_cam_ = (drone_cam_ + 1) % 3; rig_init_ = false; return drone_cam_ == 0 ? "fpv" : drone_cam_ == 1 ? "follow" : "third"; }
    if (mode_ == Mode::car) { car_cockpit_ = !car_cockpit_; return car_cockpit_ ? "cockpit" : "chase"; }
    if (mode_ == Mode::jet) { jet_cockpit_ = !jet_cockpit_; return jet_cockpit_ ? "cockpit" : "chase"; }
    return "none";
  }
  // Multiplayer, voice, pings, big map and navigation (game_net.cpp).
  if (starts("net ") || starts("mp ") || starts("voice") || starts("ping") || starts("bigmap") || starts("nav ") || starts("qr ")) return net_command(cmd);
  if (cmd == "map") return map_json();
  if (cmd == "minimap") return minimap_json();
  if (starts("lamps ")) { show_lamps_ = cmd[6] == '1'; return show_lamps_ ? "on" : "off"; }
  if (cmd == "attack") {
    // Tests: one attack with the weapon in hand (as a click).
    const std::size_t before = projectiles_.size();
    if (mode_ == Mode::foot) foot_attack(true, true);
    else if (mode_ == Mode::drone) drone_attack(true, true);
    else if (mode_ == Mode::jet) jet_attack(true, true, false);
    std::string r = std::to_string(projectiles_.size() - before) + " " + std::to_string(next_attack_ - world_->sim_time()) + " mag " + std::to_string(ammo_mag_[static_cast<u8>(foot_weapon_)]);
    if (!projectiles_.empty()) {
      const Projectile& p = projectiles_.back();
      char b[160];
      std::snprintf(b, sizeof b, " pos %.1f %.1f %.1f vel %.1f %.1f %.1f", p.pos.x, p.pos.y, p.pos.z, p.vel.x, p.vel.y, p.vel.z);
      r += b;
    }
    return r;
  }
  if (starts("weapon ")) {
    const int n = std::atoi(cmd.c_str() + 7);
    if (n >= 0 && n < 8) select_foot_weapon(static_cast<weapons::Id>(n));
    return weapons::spec(foot_weapon_).name;
  }
  if (starts("wanted ")) {
    // Tests / debugging: raise the wanted level to N stars (0 clears it).
    const int n = std::atoi(cmd.c_str() + 7);
    wanted_.clear();
    for (int k = 0; k < 64 && wanted_.stars() < n; ++k) wanted_.report(wanted::Crime::police_officer, true);
    if (n > 0) star_since_ = world_->sim_time() - wanted::arrival_delay_ms(wanted_.stars()) / 1000;  // units are due at once
    return std::to_string(wanted_.stars());
  }
  {
    std::string out;  // time, quality, perf, look (game_look.cpp)
    if (look_command(cmd, out)) return out;
  }
  if (cmd == "offline") { city_.go_offline(); return "ok"; }
  if (cmd == "camera") {
    // tests: where the view is (the overview while loading, else the mode's camera)
    char b[256];
    std::snprintf(b, sizeof b, "{\"eye\":[%.1f,%.1f,%.1f],\"target\":[%.1f,%.1f,%.1f],\"valid\":%s,\"soldier\":%s,\"placed\":%s,\"mode\":\"%s\"}", cam_.eye.x, cam_.eye.y,
                  cam_.eye.z, cam_.target.x, cam_.target.y, cam_.target.z, cam_valid_ ? "true" : "false", world_->actor_alive(soldier_) ? "true" : "false", world_placed_ ? "true" : "false",
                  mode_name(mode_));
    return b;
  }
  if (cmd == "state") return hud_json();
  {
    std::string out;  // M3 world test commands (game_world.cpp)
    if (world_command(cmd, out)) return out;
    if (life_command(cmd, out)) return out;  // M3 life test commands (game_life.cpp)
  }
  return "unknown command";
}

}  // namespace sc
