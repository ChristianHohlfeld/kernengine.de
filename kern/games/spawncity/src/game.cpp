// SpawnCity on Kern: world, modes, fixed-step hook, cameras (see game.h).
#include "gaps_rules.h"
#include "jet_seats.h"
#include "lamps.h"
#include "spawn_guard.h"
#include "game.h"
#include "world_traffic.h"
#include "game_world.h"  // WorldLife (M3 world state)
#include "game_life.h"   // GameLife (M3 life)

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "kern/core/base.h"
#include "kern/core/json.h"
#include "kern/logic/logic.h"
#include "rules.h"

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
  // The level file is the project (level/spawncity.klevel, rules.h): the world's settings - the
  // 480 s day, the fog of stylized_world_style.mjs, no ground plane (the terrain's tiles are the
  // ground), a kill plane far below (real terrain lies under the origin in places), the city's
  // ambience - and the actors that carry the rules as Blueprints. What follows adds what the
  // device decides and what is still built in code.
  Level l;
  {
    kern::json::Value doc;
    std::string error;
    if (!kern::json::parse(rules::level_text(), doc, &error) || !from_json(doc, l, &error)) {
      kern::log(kern::LogLevel::error, "SpawnCity: the level file is broken: %s", error.c_str());
      l = Level{};
    }
  }
  l.world.step_hz = kStepHz;
  l.world.substeps = substeps_;
  l.world.shadow_distance = mobile_ ? 70 : 110;
  l.world.ground_color = {0.36f, 0.40f, 0.31f};

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
  s.weapon->spread = s.weapon->aim_spread = s.weapon->recoil = 0;  // the original's rounds: no cone (select_foot_weapon)
  HealthComponent h;
  h.max_health = kPlayerHealth;
  h.regen_delay = 0;  // player_vitals_runtime.mjs: no regeneration (only a RESET or a respawn heals)
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
  // world_procedural_population.mjs CAR_COUNT 42 (phone 28), BUS_COUNT 8 (phone 5)
  const u32 n = mobile_ ? 28 : 42, nb = mobile_ ? 5 : 8;
  for (u32 i = 0; i < n + nb; ++i) {
    char name[32];
    if (i < n) std::snprintf(name, sizeof name, "Car %u", i + 1);
    else std::snprintf(name, sizeof name, "Bus %u", i - n + 1);
    const CarSpec spec = i >= n ? CarSpec::bus() : CarSpec::car();
    l.add(vehicle_actor(name, Car::rigid_spec(spec), {static_cast<f32>(i % 20) * 10, static_cast<f32>(i / 20) * 12 - 40, -400}, 0));
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
  node.id = l.logic.next_id++;
  node.type = kNodeType;
  l.logic.nodes.push_back(node);
  return l;
}

bool Game::start(kern::Engine& e) {
  engine_ = &e;
  mobile_ = e.arg_number("mobile", 0) > 0;
  // Box3D's solver runs 240 times a second: its manual's "60 Hz with 4 sub-steps" (and the
  // original's world_rigid_body_runtime.mjs: FIXED_DT 1/60, SOLVER_SUBSTEPS 4) - here as one
  // sub-step per 240 Hz step (the game's own step, so collisions are found 240 times a second
  // too). Four sub-steps on top of 240 Hz were 960 solver passes a second: four times the work
  // for nothing, the largest cost of a street full of cars.
  substeps_ = static_cast<i32>(kern::clamp(e.arg_number("substeps", 1), 1.0, 8.0));
  city_.building_budget = mobile_ ? 2600 : 4000;  // world_city_buildings.mjs MAX_FOOTPRINTS 2600 (a desktop draws more of the city)
  lat_ = e.arg_number("lat", kKonstanzLat);
  lon_ = e.arg_number("lon", kKonstanzLon);
  fpv_tilt_deg_ = static_cast<f32>(kern::clamp(e.arg_number("fpvtilt", -15), -15.0, 50.0));
  sensitivity_ = static_cast<f32>(kern::clamp(e.arg_number("mouse", 100), 20.0, 300.0)) / 100;  // mouse_settings.mjs 20..300 %
  if (const char* s = e.arg("start")) start_mode_ = s;

  register_node();
  const bool coarse = e.arg_number("coarse", mobile_ ? 1 : 0) > 0;
  soldier_controller_ = kern::game::ControllerComponent{};
  soldier_controller_.mode = kern::game::ControlMode::soldier;
  soldier_controller_.camera = kern::game::CameraMode::first_person;
  soldier_controller_.max_speed = 5.5f;
  soldier_controller_.boost_speed = 8.1f;
  soldier_controller_.jump_speed = 3.6f;
  soldier_controller_.eye_height = 1.68f;
  soldier_controller_.camera_fov = 59;  // 90 degrees across a 16:9 view (camera_pitch_contract.mjs)

  life_ = std::make_unique<WorldLife>();  // M3 world state (game_world.h)
  world_ = std::make_unique<GameWorld>(e.renderer);  // the night of the facades: game_look.cpp (windows_.night)
  if (!world_->start(build_level(), kern::game::SimMode::play)) return kern::fail("level start: %s", kern::last_error());
  // The rules that are Blueprints of the level's actors: bound to the C++ that still calls them.
  {
    kern::logic::Runtime* logic = world_->logic_runtime();
    kern::logic::Instance* wanted = logic ? logic->find(world_->find_actor("Wanted")) : nullptr;
    if (!wanted || wanted->disabled()) return kern::fail("the level's Blueprint \"Wanted\" is missing or has errors");
    wanted::bind(wanted);
    kern::logic::Instance* combat_detail = logic ? logic->find(world_->find_actor("CombatDetail")) : nullptr;
    if (!combat_detail || combat_detail->disabled()) return kern::fail("the level's Blueprint \"CombatDetail\" is missing or has errors");
    detail::bind(combat_detail);
    kern::logic::Instance* guided_rules = logic ? logic->find(world_->find_actor("Guided")) : nullptr;
    if (!guided_rules || guided_rules->disabled()) return kern::fail("the level's Blueprint \"Guided\" is missing or has errors");
    guided::bind(guided_rules);
    kern::logic::Instance* spawn_guard = logic ? logic->find(world_->find_actor("SpawnGuard")) : nullptr;
    if (!spawn_guard || spawn_guard->disabled()) return kern::fail("the level's Blueprint \"SpawnGuard\" is missing or has errors");
    spawn::bind(spawn_guard);
    kern::logic::Instance* jet_seats = logic ? logic->find(world_->find_actor("JetSeats")) : nullptr;
    if (!jet_seats || jet_seats->disabled()) return kern::fail("the level's Blueprint \"JetSeats\" is missing or has errors");
    seats::bind(jet_seats);
    kern::logic::Instance* lamp_rules = logic ? logic->find(world_->find_actor("Lamps")) : nullptr;
    if (!lamp_rules || lamp_rules->disabled()) return kern::fail("the level's Blueprint \"Lamps\" is missing or has errors");
    lamps::bind(lamp_rules);
    kern::logic::Instance* sirens = logic ? logic->find(world_->find_actor("Sirens")) : nullptr;
    if (!sirens || sirens->disabled()) return kern::fail("the level's Blueprint \"Sirens\" is missing or has errors");
    gaps::bind_sirens(sirens);
    kern::logic::Instance* control_rules = logic ? logic->find(world_->find_actor("Controls")) : nullptr;
    if (!control_rules || control_rules->disabled()) return kern::fail("the level's Blueprint \"Controls\" is missing or has errors");
    controls::bind(control_rules);
    kern::logic::Instance* settings = logic ? logic->find(world_->find_actor("Settings")) : nullptr;
    if (!settings || settings->disabled() || !bind_settings(settings)) return kern::fail("the level's Blueprint \"Settings\" is missing, has errors or not one row per setting");
    ui_.settings = Settings();  // every setting at the table's default (the page replays the player's own with "set")
    kern::logic::Instance* jetpack = logic ? logic->find(world_->find_actor("Jetpack")) : nullptr;
    if (!jetpack || jetpack->disabled()) return kern::fail("the level's Blueprint \"Jetpack\" is missing or has errors");
    Jetpack::bind(jetpack);
    kern::logic::Instance* traffic = logic ? logic->find(world_->find_actor("Traffic")) : nullptr;
    if (!traffic || traffic->disabled()) return kern::fail("the level's Blueprint \"Traffic\" is missing or has errors");
    TrafficRules::bind(traffic);
    kern::logic::Instance* multiplayer = logic ? logic->find(world_->find_actor("Multiplayer")) : nullptr;
    if (!multiplayer || multiplayer->disabled()) return kern::fail("the level's Blueprint \"Multiplayer\" is missing or has errors");
    mp::bind(multiplayer);
    // the combat and event rules (combat_rules.h)
    const struct { const char* actor; bool (*bind)(kern::logic::Instance*); } combat_rules[] = {
        {"Military", &combat::bind_military}, {"Zombies", &combat::bind_zombies}, {"Sandbox", &combat::bind_sandbox}, {"Fun", &combat::bind_fun}, {"Race", &combat::bind_race}};
    for (const auto& r : combat_rules) {
      kern::logic::Instance* running = logic ? logic->find(world_->find_actor(r.actor)) : nullptr;
      if (!running || running->disabled() || !r.bind(running)) return kern::fail("the level's Blueprint \"%s\" is missing, has errors or a table the game cannot read", r.actor);
    }
  }
  ctl_.dev.platform(!coarse, coarse, coarse ? 5 : 0);  // the page reports the real device (game_controls.cpp)
  soldier_ = world_->find_actor("Player");
  drone_actor_ = world_->find_actor("Drone");
  jet_actor_ = world_->find_actor("Jet");
  chute_actor_ = world_->find_actor("Parachute");
  const u32 n = mobile_ ? 28 : 42, nb = mobile_ ? 5 : 8;  // CAR_COUNT / BUS_COUNT (world_procedural_population.mjs)
  for (u32 i = 0; i < n + nb; ++i) {
    char name[32];
    if (i < n) std::snprintf(name, sizeof name, "Car %u", i + 1);
    else std::snprintf(name, sizeof name, "Bus %u", i - n + 1);
    const bool bus = i >= n;
    TrafficCar c;
    c.actor = world_->find_actor(name);
    c.car = std::make_unique<Car>(bus ? CarSpec::bus() : CarSpec::car());
    c.color = bus ? Vec3{0.92f, 0.78f, 0.1f} : car_color(i);
    c.target_speed = bus ? 8 : 9 + static_cast<f32>(i % 4);
    cars_.push_back(std::move(c));
  }
  for (u32 i = 0; i < 3; ++i) {
    char name[32];
    std::snprintf(name, sizeof name, "Police %u", i + 1);
    TrafficCar c;
    c.actor = world_->find_actor(name);
    c.car = std::make_unique<Car>(CarSpec::car());
    c.color = {0.067f, 0.078f, 0.098f};  // buildCruiser: the black body 0x111419, white doors (people_art light_bar)
    c.police = true;
    c.driver = false;
    cars_.push_back(std::move(c));
  }
  for (ActorId a : {drone_actor_, jet_actor_, chute_actor_}) world_->set_alive(a, false);
  for (TrafficCar& c : cars_) world_->set_alive(c.actor, false);
  world_->set_paused(true);  // until the ground under the player is built

  // Elevation from the Terrarium tiles unless offline or ?dem=0 (terrain.h).
  // without map data the original plays its training level; ?offline=1 (the tests) keeps the
  // bigger stand-in town, ?world=training / ?world=town choose
  {
    const std::string wsel = e.arg("world", "");
    city_.training_offline = wsel == "training" || (wsel != "town" && e.arg_number("offline", 0) <= 0);
  }
  city_.start(e.renderer, world_->physics(), lon_, lat_, e.arg_number("dem", 1) > 0 && e.arg_number("offline", 0) <= 0);
  if (e.arg_number("offline", 0) > 0) city_.go_offline();
  if (!art_.init(e.renderer)) return false;
  if (!people_art_.init(e.renderer)) return false;
  if (!combat_art_.init(e.renderer)) return false;  // combat and events (game_military.cpp)
  destruction_.init(e.renderer);
  gaps_init(e);  // sc-m3-gaps (game_gaps.cpp)
  people_.set_budget(mobile_ ? 68 : 100);  // PERSON_SLOTS (world_procedural_population.mjs)
  if (mobile_) people_.set_ranges(140, 160);  // world_procedural_population.mjs PEOPLE_SHOW_M / HIDE_M (phone)
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
  // riding in a mate's jet (the walker is put away where he got in): where the jet is - the city
  // streams, the traffic and the people live around it, not around the place he boarded
  if (gaps_riding()) {
    Vec3 p;
    Quat q;
    if (!gaps_ride_car() && gaps_ride_pose(p, q)) return p;
  }
  return matrix_of(soldier_).translation();
}

Vec3 Game::focus_position() const {
  if (mode_ == Mode::drone && drone_out_) return matrix_of(drone_actor_).translation();
  return player_position();
}

bool Game::clear_spot(Vec3 center, Vec2 half, f32 search, Spot& out, bool off_road) const {
  // A place where rays from above reach the ground (no roof, no wall) across the footprint;
  // `off_road`: and no street under it (the jet stands beside the streets, never on one where it
  // would block the traffic and the people).
  const std::vector<Lane>& lanes = city_.lanes();
  auto on_road = [&](Vec2 c) {
    if (!off_road) return false;
    const f32 reach = std::hypot(half.x, half.y) + 2;
    for (const Lane& L : lanes) {
      if (!L.alive || L.points.size() < 2) continue;
      for (std::size_t k = 0; k + 1 < L.points.size(); ++k) {
        const Vec2 a = L.points[k], b = L.points[k + 1], ab = b - a;
        const f32 l2 = ab.x * ab.x + ab.y * ab.y;
        const f32 t = l2 > 1e-6f ? clampf(((c.x - a.x) * ab.x + (c.y - a.y) * ab.y) / l2, 0, 1) : 0;
        const Vec2 q = a + ab * t;
        if (std::hypot(c.x - q.x, c.y - q.y) < reach + L.width * 0.5f) return true;
      }
    }
    return false;
  };
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
      if (ok && !on_road({c.x, c.y})) { out = {c, 0}; return true; }
    }
  }
  return false;
}

void Game::spawn_guard() {
  // world_spawn_guard.mjs: once the buildings around the start are solid, a player who stands
  // inside one goes out to the nearest street (the start was chosen before the houses were there).
  if (spawn_checked_ || mode_ != Mode::foot || city_.stats().t_collision < 0) return;
  spawn_checked_ = true;
  // the jet was placed before the streets were there: on a street now, it moves beside it
  if (jet_placed_ && mode_ != Mode::jet && !jet_.pilot) {
    const Vec3 jp = matrix_of(jet_actor_).translation();
    Spot here{}, there{};
    if (!clear_spot(jp, {16, 20}, 0, here, true) && clear_spot(jp, {16, 20}, 260, there, true)) {
      const Vec3 me0 = matrix_of(soldier_).translation();
      const Vec3 rot{0, 0, std::atan2(me0.y - there.p.y, me0.x - there.p.x) / kDeg - 90};
      world_->teleport(jet_actor_, there.p + Vec3{0, 0, 2.1f}, &rot);
    }
  }
  const Vec3 me = matrix_of(soldier_).translation();
  if (!city_.building_at({me.x, me.y})) return;
  Vec3 p;
  f32 heading = 0;
  for (f32 r : {40.0f, 80.0f, 160.0f})
    for (int i = 0; i < 6; ++i)
      if (city_.street_point(rng_, me, r, p, heading) && !city_.building_at({p.x, p.y})) {
        put_down(p, heading / kDeg);
        return;
      }
}

void Game::place_world() {
  spawn_checked_ = false;
  // Player at the start (a free spot near the requested place), the jet on the nearest square big
  // enough for it (SpawnCity: the VTOL stands next to the player at the start).
  Spot p{};
  if (!clear_spot({0, 0, 0}, {2, 2}, 200, p)) p = {{0, 0, 0}, 0};
  spawn_ = p;
  put_down(p.p, p.yaw / kDeg);  // on the ground as it will be (the spawn guard: the start)
  Spot j{};
  // in a session every player has his own jet: each on its own square (the others' jets stand
  // where they stand - never two on one spot), looked for round the start by the player's place
  const u32 idx = net_.active() ? net_.index_of(net_.self_slot()) : 1;
  auto taken = [&](Vec3 at) {
    for (const net::Remote& r : net_.remotes())
      if ((r.state.flags & net::kJetOut) && std::hypot(r.state.jet_pos.x - at.x, r.state.jet_pos.y - at.y) < 24) return true;
    return false;
  };
  bool found = false;
  for (u32 k = 0; k < 8 && !found; ++k) {
    const f32 a = static_cast<f32>((idx > 0 ? idx - 1 : 0) + k) * 0.785398f;
    const Vec3 c = idx <= 1 && k == 0 ? p.p + Vec3{18, 6, 0} : p.p + Vec3{std::cos(a) * 45, std::sin(a) * 45, 0};
    found = (clear_spot(c, {16, 20}, 260, j, true) || clear_spot(c, {16, 20}, 260, j)) && !taken(j.p);
  }
  if (found) {
    world_->set_alive(jet_actor_, true);
    const Vec3 rot{0, 0, std::atan2(p.p.y - j.p.y, p.p.x - j.p.x) / kDeg - 90};  // nose (+Y) towards the player
    world_->teleport(jet_actor_, j.p + Vec3{0, 0, 2.1f}, &rot);
    jet_.reset();
    jet_flown_ = false;
    jet_v_valid_ = false;
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
  // a safe margin over the ground as it is now, and the spawn guard (spawn_guard_step) until the
  // ground there is final
  (void)rot;
  put_down(feet, yaw_deg);
}

void Game::put_down(Vec3 feet, f32 yaw_deg) {
  const Vec3 rot{0, 0, yaw_deg};
  world_->teleport(soldier_, standing_feet(feet) + Vec3{0, 0, 0.73f + kSpawnLift}, &rot);
  world_->set_actor_velocity(soldier_, {});
  spawn_guard_ = {feet.x, feet.y, yaw_deg};
  spawn_guard_until_ = world_->sim_time() + 20;
  spawn_guard_ready_at_ = -1;
}

void Game::spawn_guard_step() {
  // A player put down (start, RESET, respawn, out of a vehicle) stands on the ground as it is
  // once it is final there: while the elevation or the collision under him is still coming (the
  // first seconds of a start, a reload), he is held over it - his feet a margin above the ground
  // he will get - and set down on it when it is there; for 2 s after that a ground that moved
  // under him (the elevation arriving late) lifts him back onto it. Then the guard is done.
  if (spawn_guard_until_ < 0 || mode_ != Mode::foot || player_dead_) { spawn_guard_until_ = -1; return; }
  const f32 now = world_->sim_time();
  // (no end while the ground under him is not there yet: he is never dropped onto nothing)
  {
    const Vec3 at = matrix_of(soldier_).translation();
    if (now > spawn_guard_until_ && (spawn_guard_ready_at_ >= 0 || city_.terrain().ground_ready(at.x, at.y))) { spawn_guard_until_ = -1; return; }
  }
  const Vec3 c = matrix_of(soldier_).translation();
  // he moved off on his own (walked, a vehicle): the guard follows him while it lasts
  const Vec3 feet{c.x, c.y, c.z - 0.73f};
  const bool ready = city_.terrain().ground_ready(feet.x, feet.y);
  const Vec3 stand = standing_feet(feet);
  if (!ready) {
    // held: just over where the ground will be, no fall speed
    const Vec3 hold = stand + Vec3{0, 0, kSpawnLift};
    if (std::fabs(feet.z - hold.z) > 0.05f) { const Vec3 rot{0, 0, spawn_guard_.z}; world_->teleport(soldier_, hold + Vec3{0, 0, 0.73f}, &rot); }
    world_->set_actor_velocity(soldier_, {});
    spawn_guard_ready_at_ = -1;
    return;
  }
  if (spawn_guard_ready_at_ < 0) spawn_guard_ready_at_ = now;
  if (feet.z < stand.z - 0.25f) {
    // the ground came up over his feet: back onto it
    world_->teleport(soldier_, stand + Vec3{0, 0, 0.73f + 0.1f});
    world_->set_actor_velocity(soldier_, {});
  }
  if (now - spawn_guard_ready_at_ > 2.0f) spawn_guard_until_ = -1;
}

Vec3 Game::standing_feet(Vec3 feet) const {
  // Where a player put down at `feet` stands, as the ground is now: the terrain's height there (a
  // crater may have sunk it since the spot was taken - RESET, a respawn), or the first floor a ray
  // from 1 m over the spot meets (a deck, a roof the spot is on - not a bridge above it); never
  // below the ground.
  const f32 ground = city_.ground_height(feet.x, feet.y);
  const RayResult r = cast(world_->physics(), {feet.x, feet.y, std::fmax(feet.z, ground) + 1.0f}, {0, 0, -1}, 1.0f + std::fabs(feet.z - ground) + 1.0f, body(soldier_), kCatWorld);
  f32 z = ground;
  if (r.hit && r.normal.z > 0.6f && r.point.z > ground - 0.05f) z = r.point.z;
  return {feet.x, feet.y, z + 0.02f};
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
        wanted::report(wanted::Crime::carjacking, true);
        eject_driver(vehicle);  // M3: the driver gets out and runs (game_traffic.cpp)
        toast_ = "Auto geklaut! Der Fahrer ist raus.";
        toast_age_ = 0;
      }
      c.driver = false;
      car_index_ = vehicle;
      // a parked car may be asleep in Box3D: the throttle must reach it at once
      if (world_->physics().body_valid(body(c.actor))) world_->physics().set_awake(body(c.actor), true);
      hide_soldier();
      car_cockpit_ = vr_;  // VR: the driver's seat (a chase camera swinging round the car sickens)
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
  // Ejection seat (jet_mode.mjs exit): 2.8 m and +28 m/s along the jet's own up, the canopy opens after 1.1 s.
  const BodyState s = state_of(jet_actor_);
  jet_.pilot = false;
  world_->set_alive(chute_actor_, true);
  world_->teleport(chute_actor_, s.position + s.rotation.rotate({0, 0, 2.8f}));
  world_->set_actor_velocity(chute_actor_, s.velocity + s.rotation.rotate({0, 0, kEjectUp}));
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
  spawn_guard_step();  // a player put down stands on the final ground (game.cpp)
  kern::physics::World& w = world_->physics();
  if (drone_out_) {
    const kern::physics::Body b = body(drone_actor_);
    if (b.valid()) drone_.step(w, b, h, sticks_);
  }
  if (jet_placed_) {
    const kern::physics::Body b = body(jet_actor_);
    JetInput in = mode_ == Mode::jet ? jet_in_ : JetInput{};
    if (mode_ != Mode::jet && jet_.telemetry().contacts > 0) in.brake = 1;  // parked: brakes on
    if (b.valid()) {
      // jet_mode.mjs onPhysicsImpact: what the step took off its speed beyond the flight forces
      // (a wall, a roof, the ground nose first) - over 5 m/s it hurts, (dv - 5)^1.7 * 4; a hard
      // landing (gear sink over 4 m/s) (gi - 4) * 80; a parked jet that never flew is spared
      const Vec3 v_now = w.linear_velocity(b);
      if (mode_ == Mode::jet) jet_flown_ = true;
      if (jet_flown_ && jet_v_valid_) {
        const f32 dv = kern::length(v_now - jet_v_ctrl_);
        if (dv > 5) gaps_jet_damage(std::pow(dv - 5, 1.7f) * 4);
        if (mode_ == Mode::jet && dv > 2) look_.shake.trauma = std::fmin(1.0f, look_.shake.trauma + dv / 15);
      }
      jet_.step(w, b, h, in);
      if (jet_flown_ && jet_.gear_impact() > 4) gaps_jet_damage((jet_.gear_impact() - 4) * 80);
      jet_v_ctrl_ = w.linear_velocity(b);
      jet_v_valid_ = true;
    }
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
    // (a car nobody drives here - not placed, removed, moved by a mate or by the host, without
    // physics far away - has no wheels on joints: car.h detach; Car::step puts them on)
    if (!c.placed || !world_->actor_alive(c.actor)) { c.car->detach(); continue; }
    const kern::physics::Body b = body(c.actor);
    if (!b.valid() || !w.body_valid(b)) { c.car->detach(); continue; }
    c.car->hit_user = static_cast<u64>(c.actor);
    if (i < car_lent_.size() && car_lent_[i]) { c.car->detach(); continue; }  // MP host: a mate drives it (game_worldsync.cpp)
    if (puppet_car_step(c, i, h)) { c.car->detach(); continue; }             // MP client: the host's car (game_worldsync.cpp)
    if (c.far) { c.car->detach(); traffic_far_step(c, i, h); continue; }  // no physics far away (game_traffic_far.cpp)
    if (life_) {
      // hanging on a tow truck's boom (game_traffic.cpp puts it there every step): not driven
      bool towed = false;
      for (const WorldLife::TowTruck& t : life_->trucks) towed = towed || (t.towing && t.job == static_cast<i32>(i));
      if (towed) { c.car->detach(); continue; }
    }
    car_body_dynamic(c);  // (it was the host's puppet a moment ago: driven here now)
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
    // AI traffic at 60 Hz (world_rigid_body_runtime.mjs FIXED_DT 1/60): its tyres are solved every 4th
    // 240 Hz step (staggered over the cars), the forces held in between
    if ((steps_ + i) % 4 != 0) { c.car->reapply(w, b); continue; }
    // AI driver: follow the lane on its right-hand side (SpawnCity aiInput) at the speed the
    // lights, the people and the vehicle ahead leave him.
    const Lane& L = lanes[c.lane];
    const BodyState s = read_body(w, b);
    const Vec3 fwd = s.rotation.rotate({1, 0, 0});
    const i32 n = static_cast<i32>(L.points.size());
    c.index = std::clamp(c.index, 0, n - 1);
    const f32 off = std::fmin(L.width * 0.25f, 1.8f);
    // his target (world_procedural_population.mjs): the point a lookahead on along his lane from
    // where he is - max(7 m, 1.3 s at his speed) -, on its right-hand side
    const TrafficRules::Pursuit pu = TrafficRules::pursuit(L, c.index, c.dir, {s.position.x, s.position.y}, std::fmax(7.0f, c.target_speed * 1.3f), off);
    const Vec3 target{pu.target.x, pu.target.y, 0};
    // the lane point ahead is reached: the next one; within 2.5 m of the lane's end on to a street
    // that starts there, else round (traffic_advance)
    if (c.index + c.dir < 0 || c.index + c.dir >= n ? pu.end < 2.5f : pu.passed) traffic_advance(c);
    // Approaching the end of the street (a junction): slow down to turn (SpawnCity aiInput).
    f32 want_speed = c.target_speed;
    if (pu.end < 18) want_speed = std::fmin(want_speed, 4.5f + pu.end * 0.3f);
    {
      // the bend ahead (driveSpeed): the lane's direction at the target against his own
      f32 turn = std::atan2(pu.tangent.y, pu.tangent.x) - std::atan2(fwd.y, fwd.x);
      while (turn > kPi) turn -= 2 * kPi;
      while (turn < -kPi) turn += 2 * kPi;
      turn = std::fabs(turn);
      if (turn > 0.12f) {
        const f32 look = std::fmax(6.0f, std::hypot(target.x - s.position.x, target.y - s.position.y));
        const f32 R = look / (2 * std::sin(std::fmin(turn, 1.5f) / 2) + 1e-3f);
        want_speed = std::fmin(want_speed, std::fmax(3.2f, std::sqrt(3.5f * R)));
      }
    }
    want_speed = std::fmin(want_speed, traffic_rules(static_cast<i32>(i), s.position, fwd, c.car->forward_speed()));  // M3 lights / right of way
    // the vehicle ahead (world_procedural_population.mjs driveSpeed): a speed to aim for - the
    // driver brakes down to a stop behind it and never backs off (what stands for good is the tow
    // service's: game_traffic.cpp)
    {
      i32 limiter = -1;
      const f32 free_way = traffic_gap(static_cast<i32>(i), s.position, fwd, &limiter);
      c.ahead = free_way < 0.6f ? limiter : -1;
      want_speed = std::fmin(want_speed, free_way);
    }
    c.want = want_speed;
    const CarInput in = ai_drive(s.position, fwd, c.car->forward_speed(), target, want_speed);
    c.input = in;
    c.car->step(w, b, h * 4, in);
    {
      const f32 lift = s.position.z - city_.ground_height(s.position.x, s.position.y) - Car::ground_offset(c.car->spec());
      c.peak_up = std::fmax(c.peak_up, s.velocity.z);
      c.peak_lift = std::fmax(c.peak_lift, lift);
    }
  }
}

void Game::traffic_advance(TrafficCar& c) {
  // the next lane point; at the end of the street on to a street that starts here, else turn round
  const std::vector<Lane>& lanes = city_.lanes();
  const i32 n = static_cast<i32>(lanes[c.lane].points.size());
  const Lane& L = lanes[c.lane];
  c.index += c.dir;
  if (c.index >= 0 && c.index < n) return;
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

void Game::step_parachute(f32 h) {
  kern::physics::World& w = world_->physics();
  const kern::physics::Body b = body(chute_actor_);
  if (!b.valid()) return;
  const BodyState s = read_body(w, b);
  const f32 m = w.mass(b);
  chute_time_ += h;
  chute_open_ = chute_time_ >= kChuteOpen;
  // jet_mode.mjs stepPara: gravity, the horizontal speed damped .25 /s throughout; once the canopy is
  // open (1.1 s) v.z eases towards -6.5 at 1.6 /s (against gravity: a ~12.6 m/s sink) and v.x / v.y
  // towards rollIn * 4 / -pitchIn * 4 at 0.8 /s (chute_steer_ = {rollIn, -pitchIn}, game_controls.cpp)
  Vec3 f{-m * 0.25f * s.velocity.x, -m * 0.25f * s.velocity.y, 0};
  if (chute_open_) {
    f.z = m * 1.6f * (-kChuteSink - s.velocity.z);
    const Vec2 want = chute_steer_ * kChuteSteer;
    f.x += m * 0.8f * (want.x - s.velocity.x);
    f.y += m * 0.8f * (want.y - s.velocity.y);
  }
  w.apply_force_to_center(b, f);
}

// ---------------------------------------------------------------------------------------------
// traffic placement (every half second)

void Game::manage_traffic() {
  if (puppet_world_) return;  // MP client in the host's town: his traffic (game_worldsync.cpp)
  const std::vector<Lane>& lanes = city_.lanes();
  const Vec3 me = focus_position();
  // MP host: the traffic lives around every player - placed near one of them, taken away only
  // when it is far from all (game_worldsync.cpp sends it)
  const std::vector<Vec3> mates = net_.hosting() ? mate_foci() : std::vector<Vec3>{};
  auto nearest = [&](Vec3 p) {
    f32 d = std::hypot(p.x - me.x, p.y - me.y);
    for (const Vec3& m : mates) d = std::fmin(d, std::hypot(p.x - m.x, p.y - m.y));
    return d;
  };
  i32 alive_lanes = 0;
  for (const Lane& l : lanes) alive_lanes += l.alive ? 1 : 0;
  if (alive_lanes == 0) return;
  traffic_far_switch();  // physics only near the focus (game_traffic_far.cpp)
  for (std::size_t i = 0; i < cars_.size(); ++i) {
    TrafficCar& c = cars_[i];
    if (static_cast<i32>(i) == car_index_) continue;
    if (i < car_lent_.size() && car_lent_[i]) continue;  // a mate drives it
    if (c.police) {
      // An abandoned cruiser far away is put back into the pool.
      if (c.placed && c.unit < 0 && std::hypot(matrix_of(c.actor).translation().x - me.x, matrix_of(c.actor).translation().y - me.y) > 420) {
        world_->set_alive(c.actor, false);
        c.placed = false;
        c.wreck = false;
        c.hp = car_max_hp(c);
      }
      continue;
    }
    bool replace = !c.placed;
    if (c.placed) {
      const Vec3 p = matrix_of(c.actor).translation();
      const f32 d = nearest(p);
      // Far away (and so out of sight), fallen through, or its street unloaded: bring it back.
      // VEHICLE_RELOCATE_M 330 (world_procedural_population.mjs)
      replace = d > 330 || p.z < city_.ground_height(p.x, p.y) - 5 || (c.driver && (c.lane < 0 || c.lane >= static_cast<i32>(lanes.size()) || !lanes[c.lane].alive) && d > 120);
      // A burnt-out wreck no tow truck came for is cleared after 45 s (the street is free again);
      // it comes back as a whole car somewhere else.
      if (c.wreck && world_->sim_time() - c.wreck_at > 45 && !car_towed(static_cast<i32>(i))) replace = true;
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
        const Vec3 around = mates.empty() || attempt % (mates.size() + 1) == 0 ? me : mates[attempt % (mates.size() + 1) - 1];
        const f32 t0 = clampf(((around.x - a.x) * (bpt.x - a.x) + (around.y - a.y) * (bpt.y - a.y)) / seg, 0, seg);  // nearest to the player
        along = clampf(t0 + (rng_.unit() < 0.5f ? -1.0f : 1.0f) * rng_.range(45, 240), 4, seg - 4);
      }
      const Vec2 mid = a + (bpt - a) * (along / seg);
      const f32 d = nearest({mid.x, mid.y, 0});
      if (d < 45 || d > 240) continue;  // VEHICLE_PLACE_MIN_M / VEHICLE_PLACE_MAX_M (from the nearest player)
      const i32 dir = rng_.unit() < 0.5f ? 1 : -1;
      const Vec2 dv = (dir > 0 ? bpt - a : a - bpt) * (1.0f / seg);
      const f32 off = std::fmin(L.width * 0.25f, 1.8f);
      const Vec3 pos{mid.x + dv.y * off, mid.y - dv.x * off, city_.ground_height(mid.x, mid.y) + Car::ground_offset(c.car->spec()) + 0.05f};
      // Nothing in the way (another car, a building corner).
      if (cast(world_->physics(), pos + Vec3{0, 0, 3}, {0, 0, -1}, 3.2f, {}).hit) continue;
      if (!gaps_vehicle_spawn_ok(static_cast<i32>(i), pos, std::atan2(dv.y, dv.x))) continue;  // no overlap, unseen (game_gaps.cpp)
      if (!world_->actor_alive(c.actor)) { world_->set_alive(c.actor, true); c.far = false; c.solid_masks.clear(); }  // (a new body: solid)
      const f32 yaw = std::atan2(dv.y, dv.x);
      const Vec3 rot{0, 0, yaw / kDeg};
      world_->teleport(c.actor, pos, &rot);
      world_->set_actor_velocity(c.actor, {});
      c.car->reset();
      c.placed = true;
      c.driver = true;
      c.hp = car_max_hp(c);
      c.wreck = false;
      c.hit_by_player = false;
      c.burn_until = -1;
      c.lane = li;
      c.dir = dir;
      c.index = dir > 0 ? k + 1 : k;
      c.want = 0;
      c.ahead = -1;
      c.far_speed = 0;
      // with physics only near a player and on ground that stands (game_traffic_far.cpp); there
      // it rests on its wheels as the ground lies
      if (d < 150 && city_.terrain().ground_ready(pos.x, pos.y)) {
        traffic_solid(c);
        car_body_dynamic(c);  // (it was without physics where it stood before)
        if (const kern::physics::Body cb = body(c.actor); cb.valid()) world_->physics().set_pose(cb, car_rest_pose(c.car->spec(), pos.x, pos.y, yaw));
      } else traffic_make_far(c);
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
    ground_changes_.clear();
    city_.take_ground_changes(ground_changes_);
    if (city_.ready_at({0, 0, 0})) place_world();
    else {
      const StreamStats& s = city_.stats();
      char buf[128];
      std::snprintf(buf, sizeof buf, "SpawnCity: %s ... %u Gebaeude", city_.offline() ? "Ersatzstadt wird gebaut" : "die Stadt wird geladen", s.buildings);
      e.set_status(buf);
    }
  }
  if (world_placed_ && !ui_.paused) {  // a menu is open: nothing moves (game_ui.cpp)
    mp_input(e, dt);  // multiplayer keys (voice, ping, big map, vote): game_net.cpp
    vr_menu_input(dt);  // the headset's wrist menu (game_vr.cpp): while open the pad is the menu's
    if (vr_menu_open_) { pad.rt = pad.lt = 0; pad.pressed = 0; pad.buttons = 0; pad.rx = pad.ry = 0; }  // the menu's: no turn, no weapon step
    read_input(e, dt);
    update_touch_layout();
    look_lap(perf::Slot::logic);
    world_->step(dt, play_in_);
    look_lap(perf::Slot::physics);
    static f32 traffic_clock = 0;
    traffic_clock += dt;
    if (traffic_clock > 0.5f && ui_.settings.on(Opt::world_traffic)) { traffic_clock = 0; manage_traffic(); }
    worldsync_update(dt);  // one world in multiplayer (game_worldsync.cpp)
    dsync_update(dt);  // one destroyed city in multiplayer (game_destruction_sync.cpp)
    townsync_update(dt);  // lamps, planks, marks; a client's hits on the host's town (game_townsync.cpp)
    toss_update(dt);   // thrown people and animals fly on every machine (game_toss.cpp)
    projsync_update(dt);  // every round in flight on every machine (game_projsync.cpp)
    city_.anchor = player_position();  // ground collision stays under the pilot too
    {
      // the elevation ahead of a fast flight (terrain.h lead)
      const Vec3 f = focus_position();
      if (dt > 0 && focus_prev_valid_) {
        Vec3 v = (f - focus_prev_) * (1.0f / dt);
        v.z = 0;
        const f32 l = kern::length(v);
        city_.terrain().lead = l > 15 ? v * (std::fmin(l * 12, 2500.0f) / l) : Vec3{};
        // the city ahead of a fast car: built along the next 4 s of the way (at most 420 m ahead)
        city_.view_lead = l > 6 ? v * (std::fmin(l * 4, 420.0f) / l) : Vec3{};
      }
      focus_prev_ = f;
      focus_prev_valid_ = true;
    }
    city_.update(focus_position(), mobile_ ? 3 : 4);
    ground_changes_.clear();
    city_.take_ground_changes(ground_changes_);
    spawn_guard();
    look_lap(perf::Slot::stream);
    update_town(dt);
    update_projectiles(dt);
    update_world(dt);  // M3 world (game_world.cpp)
    life_update(dt);   // M3 life: animals, street life, ragdolls, interiors (game_life.cpp)
    update_guided(dt);   // GUIDED missiles (game_guided.cpp)
    update_details(dt);  // critical damage, ballots (game_details.cpp)
    gaps_update(dt);  // dispatch, critical smoke, organic hits, impact bursts (game_gaps.cpp)
    update_combat(dt);  // military, flak, jets, fights, zombies, enemies, race (game_military.cpp, game_events.cpp)
    swarm_update(dt);  // the mates' zombies, military, police and sandbox enemies (game_swarm.cpp)
    destruction_.update(dt, city_, rng_);  // broken buildings: torn pieces, debris (destruction.h)
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
    // While the city loads the view stands at the start place at eye height (the original never
    // shows an overview from above: the game view starts on the ground).
    c.eye = {0, 0, city_.ground_height(0, 0) + 1.68f};
    c.target = c.eye + Vec3{10, 0, 0};
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
        // simulator.mjs: third clamp(62 x fov / 105, 35, 100), follow clamp(52 x fov / 105, 30, 90)
        c.fov = (third ? clampf(62 * fpv_fov_deg_ / 105, 35, 100) : clampf(52 * fpv_fov_deg_ / 105, 30, 90)) * kDeg;
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
        c.fov = 80 * kDeg;  // player_car_mode.mjs cockpit: camera.fov = 80 (vertical, as Kern's)
        c.near = bus ? 0.22f : 0.04f;
      } else {
        // Chase (player_car_mode.mjs): 5.4 m back, 2.15 m up, heading smoothing 1 - exp(-6 dt); bus x 1.75.
        const f32 zoom = bus ? 1.75f : 1.0f;
        rig(pos, s.velocity, {fwd.x, fwd.y}, 9, 9, 6, 0.6f);
        const Vec3 hd = kern::normalize(Vec3{cam_heading_.x, cam_heading_.y, 0});
        const Vec3 look_dir = Quat::axis_angle({0, 0, 1}, head_yaw_).rotate(hd);
        // zoom (pinch / wheel / + -, player_car_mode.mjs): distance 5.4 z, height 2.15 (.55 + .45 z)
        const f32 cz = ctl_.car_zoom;
        // player_car_mode.mjs: the target 1.05 m ahead of the car and 1.2 m up, the eye d back and h above
        // the target; fov min(80, 70 + |v| x .25)
        const Vec3 tgt = cam_anchor_ + look_dir * 1.05f + Vec3{0, 0, 1.2f};
        const Vec3 want = tgt - look_dir * (5.4f * zoom * cz) + Vec3{0, 0, 2.15f * zoom * (0.55f + 0.45f * cz)};
        c.eye = collide(tgt, want, body(tc.actor));
        c.target = tgt;
        c.up = {0, 0, 1};
        c.fov = std::fmin(80.0f, 70 + std::fabs(tc.car->forward_speed()) * 0.25f) * kDeg;
      }
      break;
    }
    case Mode::jet: {
      const kern::Mat4 m = matrix_of(jet_actor_);
      const BodyState s = state_of(jet_actor_);
      const Vec3 pos = m.translation();
      const Vec3 nose = s.rotation.rotate({0, 1, 0}), up = s.rotation.rotate({0, 0, 1});
      if (jet_cockpit_) {
        // jet_mode.mjs applyCamera cockpit: cockpitEye (0, 3.5, 1.42), along the nose, near .5
        c.eye = (m * kern::Mat4::translate({0, 3.5f, 1.42f})).translation();
        c.target = c.eye + nose * 10.0f;
        c.up = up;
        c.near = 0.5f;
      } else {
        // jet_mode.mjs applyCamera chase: 22 + min(14, v / 40) m behind and 6.5 m above the airframe (in
        // its own frame), eased with 1 - e^(-6 dt); up 35 % towards the world's; looking 30 m ahead
        const f32 sp = kern::length(s.velocity), dist = 22 + std::fmin(14.0f, sp / 40);
        const Vec3 want = nose * -dist + up * 6.5f;
        if (!rig_init_) { jet_cam_off_ = want; rig_init_ = true; }
        jet_cam_off_ = jet_cam_off_ + (want - jet_cam_off_) * (1 - std::exp(-std::fmin(dt, 0.1f) * 6));
        c.eye = collide(pos + up * 2.0f, pos + jet_cam_off_, body(jet_actor_));
        c.target = pos + nose * 30.0f;
        c.up = kern::normalize(up + (Vec3{0, 0, 1} - up) * 0.35f, {0, 0, 1});
        c.near = 1;
      }
      c.fov = clampf(66 + kern::length(s.velocity) / 30, 66, 90) * kDeg;  // both views
      c.far = 6000;
      break;
    }
    case Mode::parachute: {
      const Vec3 pos = matrix_of(chute_actor_).translation();
      // jet_mode.mjs applyCamera (para): 14 m south and 7 m above the canopy, at 2 m above it, fov 70
      c.eye = collide(pos, pos + Vec3{0, -14, 7}, body(chute_actor_));
      c.target = pos + Vec3{0, 0, 2};
      c.up = {0, 0, 1};
      c.fov = 70 * kDeg;
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
      // a wreck is burnt out: charred, no lights - nobody takes it for a car still to be shot
      art_.car(matrix_of(c.actor), *c.car, c.wreck ? Vec3{0.055f, 0.05f, 0.045f} : c.color, mine && car_cockpit_, !c.wreck && c.input.pedal < -0.05f);
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
    swarm_draw_vehicles();  // the mates' cruisers (game_swarm.cpp)
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
    // the page plays the original's own sound (game_audio_web.cpp): Kern's mix is not heard and
    // its music not made
    if (web_audio_) std::fill(audio_out_.begin(), audio_out_.end(), 0.0f);
    else ui_render_music(audio_out_.data(), frames);  // title music and car radio (game_equipment.cpp)
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
  add(",\"tiles\":{\"ready\":%u,\"loading\":%u,\"failed\":%u,\"buildings\":%u,\"colliders\":%u", st.tiles_ready, st.tiles_loading, st.tiles_failed, st.buildings,
      st.colliders);
  add(",\"t\":{\"request\":%.0f,\"data\":%.0f,\"decoded\":%.0f,\"terrain\":%.0f,\"built\":%.0f,\"collision\":%.0f,\"work\":%.0f,\"worst\":%.1f,\"worstStep\":\"%s\",\"worstStepMs\":%.1f,\"rebuilds\":%u,\"relays\":%u,\"uploads\":%u,\"uploadKVerts\":%u}}", st.t_request, st.t_data,
      st.t_decoded, st.t_terrain, st.t_built, st.t_collision, st.work_ms, st.worst_ms, st.worst_step, st.worst_step_ms, st.rebuilds, st.relays, st.uploads, st.upload_vertices);
  if (vr_) s += vr_menu_json();  // the headset's wrist menu (game_vr.cpp)
  hud_world(s);  // terrain, water, animals, ... (game_world.cpp)
  life_hud(s);   // animals, street life, ragdolls, interiors (game_life.cpp)
  add(",\"fps\":%.0f,\"touch\":%s,\"stars\":%d,\"heat\":%.1f", fps_, touch_mode_ ? "true" : "false", wanted::stars(), wanted::heat());
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
  // player_vitals_runtime.mjs: PLAYER_MAX_HP 100, every hit divided by PLAYER_DAMAGE_TOLERANCE 1.18. Kern's
  // soldier holds 118 (the same thing for every source of damage, falls included): shown as 0..100
  add(",\"health\":%.0f,\"maxHealth\":%.0f,\"ammo\":%d,\"mag\":%d,\"reserve\":%d,\"reload\":%.2f,\"spread\":%.2f,\"aiming\":%s,\"ads\":%.2f,\"respawn\":%.1f",
      h.health >= 0 ? h.health / 1.18f : h.health, h.max_health > 0 ? h.max_health / 1.18f : h.max_health, ammo, mag, reserve, h.reload, h.spread, h.aiming ? "true" : "false", world_->aim_blend(), net_.active() ? h.respawn_in : respawn_wait());
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
      // (a mate's car / jet to ride along in, the ride to leave: the EINSTEIGEN button says it, view.mjs)
    }
  }
  if (mode_ == Mode::drone && drone_.telemetry().calibrating) hint_ = "Flugregler kalibriert - stillhalten";
  combat_hud(s);  // "combat":{...} (game_events.cpp)
  guided_hud(s);   // "guided":{...} (game_guided.cpp)
  details_hud(s);  // "details":{...} (game_details.cpp)
  gaps_hud(s);  // "gaps":{...} (game_gaps.cpp)
  s += ",\"hint\":\"" + json_escape(hint_) + "\"";
  s += world_rules_json();  // the WORLD switches in force (the host's in a session: game_net.cpp)
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
  if (std::string out; web_audio_command(cmd, out)) return out;  // the original's sound on the page (game_audio_web.cpp)
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
  // tests: "tp X Y" puts the walker on the ground there
  if (cmd.rfind("tp ", 0) == 0 && mode_ == Mode::foot) {
    f32 x = 0, y = 0;
    if (std::sscanf(cmd.c_str() + 3, "%f %f", &x, &y) != 2) return "usage: tp X Y";
    world_->teleport(soldier_, standing_feet({x, y, city_.ground_height(x, y)}) + Vec3{0, 0, 0.73f});
    world_->set_actor_velocity(soldier_, {});
    return "ok";
  }
  // simulator.mjs camFpv / camFollow / camThird: the drone's camera by name (any mode: the preference)
  if (cmd == "cam fpv" || cmd == "cam follow" || cmd == "cam third") {
    drone_cam_ = cmd == "cam fpv" ? 0 : cmd == "cam follow" ? 1 : 2;
    rig_init_ = false;
    return cmd.substr(4);
  }
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
  if (starts("vr hud ")) { vr_hud_tex_ = static_cast<u32>(std::atoi(cmd.c_str() + 7)); vr_hud_mesh_ = 0; return "hud"; }
  if (cmd == "vr state") {
    char b[200];
    const Vec3 rf = engine_ ? engine_->xr_room_forward : Vec3{};
    std::snprintf(b, sizeof b, "{\"body\":%.1f,\"head\":%.1f,\"room\":%.1f,\"follow\":%d,\"hand\":%d,\"arms\":%d,\"scope\":%d}", world_->camera_yaw(), engine_ ? engine_->xr_head_yaw : 0,
                  std::atan2(rf.y, rf.x) / kDeg, engine_ && engine_->xr_follow_head ? 1 : 0, vr_hands_[1].valid ? 1 : 0, vr_arms_ ? 1 : 0, engine_ && engine_->xr_scope ? 1 : 0);
    return b;
  }
  if (cmd == "destroy sync") return dsync_json();
  if (cmd == "town sync") return townsync_json();  // tests: the town's state in a session (game_townsync.cpp)
  if (cmd.rfind("toss", 0) == 0) return toss_command(cmd);
  if (cmd == "world list") return worldsync_list();  // the street life here (tests: game_worldsync.cpp)
  if (cmd == "vr 1" || cmd == "vr 0") {
    // the page entered / left VR: the views from the inside (FPV drone, cockpits), a level camera
    vr_ = cmd == "vr 1";
    if (vr_) { drone_cam_ = 0; car_cockpit_ = true; jet_cockpit_ = true; rig_init_ = false; }
    // a headset draws two eyes at 72-90 Hz: start low, never above the middle level (a missed
    // frame there is a stutter you feel); back on the page the platform's own ladder
    if (vr_) look_.gov.reset(1, 2);
    else look_.gov.reset(mobile_ ? perf::kMobileStart : perf::kDesktopStart, mobile_ ? perf::kMobileMax : perf::kTiers - 1);
    look_apply_tier();
    return vr_ ? "vr" : "flat";
  }
  if (starts("weapon ")) {
    const int n = std::atoi(cmd.c_str() + 7);
    if (n >= 0 && n < 8) select_foot_weapon(static_cast<weapons::Id>(n));
    return weapons::spec(foot_weapon_).name;
  }
  if (cmd == "blueprints") {
    // Tests / debugging: the level's Blueprints as they run - name, how they are executed
    // (interpreter, compiled WebAssembly, native C++), functions and nodes.
    std::string s = "[";
    if (kern::logic::Runtime* rt = world_->logic_runtime())
      for (const auto& inst : rt->instances()) {
        char b[256];
        std::snprintf(b, sizeof b, "%s{\"name\":\"%s\",\"tier\":\"%s\",\"functions\":%zu,\"nodes\":%zu,\"disabled\":%s}", s.size() > 1 ? "," : "", inst->name().c_str(), inst->tier(),
                      inst->graph().functions.size(), inst->graph().nodes.size(), inst->disabled() ? "true" : "false");
        s += b;
      }
    return s + "]";
  }
  if (starts("wanted ")) {
    // Tests / debugging: raise the wanted level to N stars (0 clears it).
    const int n = std::atoi(cmd.c_str() + 7);
    wanted::clear();
    for (int k = 0; k < 64 && wanted::stars() < n; ++k) wanted::report(wanted::Crime::police_officer, true);
    if (n > 0) star_since_ = world_->sim_time() - wanted::arrival_delay_ms(wanted::stars()) / 1000;  // units are due at once
    return std::to_string(wanted::stars());
  }
  {
    std::string out;  // time, quality, perf, look (game_look.cpp)
    if (look_command(cmd, out)) return out;
  }
  if (cmd == "offline") { city_.go_offline(); return "ok"; }
  if (cmd == "traffic bodies") {
    // tests: every placed car - where it rides (over its design ride height), its peaks, its wheels
    std::string out = "[";
    for (const TrafficCar& c : cars_) {
      if (!c.placed) continue;
      const Vec3 p = matrix_of(c.actor).translation();
      char b[420];
      std::snprintf(b, sizeof b, "%s{\"bus\":%d,\"far\":%d,\"rig\":%d,\"lift\":%.2f,\"peakUp\":%.2f,\"peakLift\":%.2f,\"x\":%.0f,\"y\":%.0f", out.size() > 1 ? "," : "",
                    c.car->spec().radius > 0.4f ? 1 : 0, c.far ? 1 : 0, c.car->rigged() ? 1 : 0,
                    p.z - city_.ground_height(p.x, p.y) - Car::ground_offset(c.car->spec()), c.peak_up, c.peak_lift, p.x, p.y);
      out += b;
      // how it stands: upright (up.z), its speed, body type and sleep, the ground the physics has
      // under it against the ground's height there (gap > 0: the collision lies lower), and every
      // wheel - its travel and how far its tread is over (+) or in (-) the ground
      const kern::physics::Body cb = body(c.actor);
      kern::physics::World& w = world_->physics();
      if (cb.valid() && w.body_valid(cb)) {
        const BodyState st = read_body(w, cb);
        std::vector<kern::physics::Body> own{cb};
        for (int k = 0; k < 4; ++k) if (c.car->rigged()) own.push_back(c.car->wheel_body(k));
        const kern::physics::RayHit hit = w.cast_ray_ignoring(p + Vec3{0, 0, 4}, {0, 0, -12}, kern::Span<const kern::physics::Body>(own.data(), own.size()), kCatWorld);
        std::snprintf(b, sizeof b, ",\"up\":%.2f,\"v\":%.1f,\"vz\":%.2f,\"dyn\":%d,\"awake\":%d,\"gap\":%.2f,\"driver\":%d,\"wreck\":%d,\"pedal\":%.2f,\"hb\":%d", st.rotation.rotate({0, 0, 1}).z,
                      std::hypot(st.velocity.x, st.velocity.y), st.velocity.z, w.body_type(cb) == kern::physics::BodyType::dynamic ? 1 : 0, w.awake(cb) ? 1 : 0,
                      hit.hit ? city_.ground_height(p.x, p.y) - hit.point.z : 99.0f, c.driver ? 1 : 0, c.wreck ? 1 : 0, c.input.pedal, c.input.handbrake ? 1 : 0);
        out += b;
        // its driver: the speed he aims for, the car he stands behind, how far he is off his lane
        const bool on_lane = c.lane >= 0 && c.lane < static_cast<i32>(city_.lanes().size()) && city_.lanes()[c.lane].alive;
        std::snprintf(b, sizeof b, ",\"want\":%.1f,\"ahead\":%d,\"off\":%.1f,\"farV\":%.1f", c.want > 1e8f ? -1.0f : c.want, c.ahead, on_lane ? traffic_lane_distance(c) : -1.0f, c.far_speed);
        out += b;
        if (on_lane) {
          const Lane& ln = city_.lanes()[c.lane];
          std::snprintf(b, sizeof b, ",\"lane\":[%d,%zu,%d,%d,%.0f,%.0f,%.0f,%.0f]", c.lane, ln.points.size(), c.index, c.dir, ln.points.front().x, ln.points.front().y, ln.points.back().x, ln.points.back().y);
          out += b;
        }
        if (c.car->rigged()) {
          out += ",\"wheels\":[";
          for (int k = 0; k < 4; ++k) {
            const kern::physics::Body wb = c.car->wheel_body(k);
            const Vec3 hub = wb.valid() && w.body_valid(wb) ? w.pose(wb).position : Vec3{};
            std::snprintf(b, sizeof b, "%s[%.2f,%.2f,%d]", k ? "," : "", c.car->wheel(k).travel, hub.z - c.car->spec().radius - city_.ground_height(hub.x, hub.y),
                          wb.valid() && w.body_valid(wb) && w.awake(wb) ? 1 : 0);
            out += b;
          }
          out += "]";
          // a car that stands wrong: what it touches - per part (0 the chassis, 1-4 the wheels)
          // the other body [part, its type (0 fixed, 1 kinematic, 2 dynamic), the car it belongs to
          // or -1, impulse] and the contact points [part, normal x y z, separation, impulse,
          // height over the ground]
          bool odd = p.z - city_.ground_height(p.x, p.y) - Car::ground_offset(c.car->spec()) < -0.08f;
          for (int k = 0; k < 4; ++k) {
            const kern::physics::Body wb = c.car->wheel_body(k);
            if (!wb.valid() || !w.body_valid(wb)) continue;
            const Vec3 hub = w.pose(wb).position;
            odd = odd || hub.z - c.car->spec().radius - city_.ground_height(hub.x, hub.y) < -0.08f || c.car->wheel(k).travel > c.car->spec().susp_upper + 0.05f;
          }
          if (odd) {
            auto owner = [&](kern::physics::Body o) {
              for (std::size_t j = 0; j < cars_.size(); ++j) {
                if (body(cars_[j].actor) == o) return static_cast<i32>(j);
                for (int k = 0; k < 4; ++k) if (cars_[j].car->rigged() && cars_[j].car->wheel_body(k) == o) return static_cast<i32>(j);
              }
              return -1;
            };
            std::string touch = ",\"touch\":[", cps = ",\"cp\":[";
            for (int part = 0; part < 5; ++part) {
              const kern::physics::Body pb = part == 0 ? cb : c.car->wheel_body(part - 1);
              if (!pb.valid() || !w.body_valid(pb)) continue;
              std::vector<std::pair<kern::physics::Body, f32>> ts;
              w.touching(pb, ts);
              for (const auto& [o, imp] : ts) {
                const kern::physics::BodyType bt = w.body_valid(o) ? w.body_type(o) : kern::physics::BodyType::fixed;
                std::snprintf(b, sizeof b, "%s[%d,%d,%d,%.0f]", touch.back() == '[' ? "" : ",", part, bt == kern::physics::BodyType::fixed ? 0 : bt == kern::physics::BodyType::kinematic ? 1 : 2, owner(o), imp);
                touch += b;
              }
              std::vector<kern::physics::ContactPoint> pts;
              w.contact_points(pb, pts);
              for (const kern::physics::ContactPoint& q : pts) {
                if (cps.size() > 1500) break;
                std::snprintf(b, sizeof b, "%s[%d,%.2f,%.2f,%.2f,%.2f,%.0f,%.2f]", cps.back() == '[' ? "" : ",", part, q.normal.x, q.normal.y, q.normal.z, q.separation, q.impulse,
                              q.point.z - city_.ground_height(q.point.x, q.point.y));
                cps += b;
              }
            }
            out += touch + "]" + cps + "]";
          }
        }
      }
      out += "}";
    }
    return out + "]";
  }
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
