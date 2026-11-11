// The game cases of the native Blueprint test: two levels whose Blueprints use every gameplay node
// (kern/game/blueprint_nodes.h), each with a scenario that makes its events happen.
//   Arena: overlaps, hits, a pickup, a trigger, damage, death and respawn, input, a second player,
//          the end of the game - and every actor, physics, combat and effect node.
//   Robot: the Micro Duck's room; the level's Blueprint reads the robot's sensors and drives it.
//
// kern_tests plays them in the interpreter (game_blueprints_as_classes: the scenarios do what they
// are meant to) and writes the levels (--emit-game-native). run.sh has tools/blueprint_cpp write
// each level's Blueprints as C++ classes - the tool a game's build uses - and compiles them with
// game_native_test.cpp, which plays every scenario twice, in the interpreter and as those
// classes: the two must agree in everything the dump below shows.
#pragma once

#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "gl_stub.h"
#include "kern/game/demos.h"
#include "kern/game/runtime.h"
#include "kern/game/walker.h"
#include "kern/kern.h"
#include "kern/logic/logic.h"
#include "native_show.h"

namespace game_cases {

using namespace kern;

/// The level's own Blueprint: a Custom Event per group of nodes (the scenario calls them), and a
/// handler for every event of the game.
inline const char* kLevel = R"kn(
struct Level : kern::Blueprint {
  Text Log = "";
  Int Players = 0;
  Int Crates = 0;
  Int Hits = 0;
  Int Moved = 0;
  Float Sum = 0.0;
  Float Impact = 0.0;
  Float Damage = 0.0;
  Vector At = Vector{0.0, 0.0, 0.0};
  Vector Facing = Vector{0.0, 0.0, 0.0};
  Vector HitAt = Vector{0.0, 0.0, 0.0};
  Actor Last = Actor{0};
  Actor Made = Actor{0};
  List<Actor> Tagged = List<Actor>{};
  Bool Ok = false;
  Bool Won = false;

  KERN_EVENT(on1, event_begin)
  void on1() {
    game_message("Welcome");
    Players = actor_player_count();
    Last = actor_player(0);
    Log = text_append(Log, actor_name(actor_find("Door")), ";");
  }

  KERN_EVENT(on2, event_custom<"Values">)
  void on2(Actor actor, Float value, Text text) {
    Crates = actor_find_tag("crate").count;
    Last = actor_find_tag(text).actor;
    Ok = logic_and(actor_has_tag(Last, "crate"), actor_valid(Last));
    At = actor_location(actor);
    Facing = vector_add(actor_forward(actor_player(0)), actor_rotation(actor));
    Sum = math_add(actor_distance(actor, Last), actor_velocity(Last).speed);
    Log = text_append(Log, actor_name(actor_random_tag(n20, "crate")), ";");
    Sum = math_add(Sum, math_add(actor_health(actor).health, game_score().score));
    Moved = game_score().pickups;
    Won = actor_health(actor).dead;
  }

  KERN_EVENT(on3, event_custom<"Moves">)
  void on3(Actor actor, Float value, Text text) {
    actor_set_location(actor, Vector{3.0, 3.0, 2.0});
    actor_set_rotation(actor, vector_make(0.0, 0.0, value));
    actor_move_by(actor, Vector{0.0, 1.0, 0.0});
    actor_set_property(actor, "components.mesh.material.roughness", text);
    actor_set_light(actor_find("Lamp"), Vector{0.2, 0.4, 0.8}, 3.0, 7.0);
    actor_set_color(actor, Vector{0.1, 0.9, 0.3});
    actor_enable(actor_find("Lamp"), math_compare<">">(value, 0.0));
  }

  KERN_EVENT(on4, event_custom<"Physics">)
  void on4(Actor actor, Float value, Text text) {
    actor_set_velocity(actor, Vector{1.0, 0.0, 0.0});
    actor_impulse(actor, vector_make(0.0, 0.0, value));
    actor_force(actor, Vector{0.0, 50.0, 0.0});
    actor_launch(actor_find(text), Vector{0.0, 0.0, 4.0});
    actor_simulate(actor_find("Sleeper"), true);
    actor_joint_motor(actor_find("Flap"), 0, true, 90.0, 500.0);
    physics_trace(n40, Vector{0.0, 0.0, 10.0}, Vector{0.0, 0.0, -1.0}, Actor{0});
    Ok = n40.hit;
    Last = n40.actor;
    At = vector_add(n40.location, n40.normal);
    physics_trace(Vector{0.0, 0.0, 10.0}, Vector{0.0, 0.0, 9.0}, actor);
  }

  KERN_EVENT(on5, event_custom<"Boom">)
  void on5(Actor actor, Float value, Text text) {
    actor_explode(actor_location(actor), 6.0, value, 20.0);
  }

  KERN_EVENT(on6, event_custom<"Combat">)
  void on6(Actor actor, Float value, Text text) {
    combat_damage(actor, value, actor_player(0));
    combat_heal(actor, 5.0);
    combat_ammo(actor_player(0), 12);
  }

  KERN_EVENT(on7, event_custom<"Kill">)
  void on7(Actor actor, Float value, Text text) {
    combat_kill(actor);
  }

  KERN_EVENT(on8, event_custom<"Effects">)
  void on8(Actor actor, Float value, Text text) {
    game_sound<"builtin:confirm">(Vector{1.0, 2.0, 3.0}, 0.5, 1.2, true);
    game_particles(actor_location(actor), Vector{0.2, 0.6, 1.0});
    game_shake(0.3);
    game_add_score(value);
  }

  KERN_EVENT(on9, event_custom<"Spawn">)
  void on9(Actor actor, Float value, Text text) {
    actor_spawn<"Template">(n50, Actor{0}, vector_make(value, 5.0, 0.5), Vector{0.0, 0.0, 45.0});
    Made = n50.spawned;
    Log = text_append(Log, actor_name(n50.spawned), ";");
    actor_spawn<"Nothing">(actor, Vector{0.0, 9.0, 0.5}, Vector{0.0, 0.0, 0.0});
  }

  KERN_EVENT(on10, event_custom<"Remove">)
  void on10(Actor actor, Float value, Text text) {
    actor_destroy(Made);
  }

  KERN_EVENT(on11, event_custom<"Each">)
  void on11(Actor actor, Float value, Text text) {
    for (auto it60 = flow_for_each_tag(n60, text); it60.next();) {
      actor_move_by(n60.actor, vector_make(0.0, 0.0, kern::to<Float>(n60.index)));
      Moved += 1;
    }
    Tagged = actor_all_with_tag(text);
    for (auto it61 = flow_for_each<Actor>(n61, Tagged); it61.next();) {
      Log = text_append(Log, actor_name(n61.item), kern::to<Text>(n61.index));
    }
    Log = text_append(Log, "each", kern::to<Text>(list_length<Actor>(Tagged)));
  }

  KERN_EVENT(on12, event_custom<"Win">)
  void on12(Actor actor, Float value, Text text) {
    game_win(text);
  }

  KERN_EVENT(on13, event_custom<"Lose">)
  void on13(Actor actor, Float value, Text text) {
    game_lose(text);
  }

  KERN_EVENT(on70, game_overlap_begin)
  void on70(Actor actor, Actor other) {
    Log = text_append(Log, "in:", actor_name(other));
  }

  KERN_EVENT(on71, game_overlap_end)
  void on71(Actor actor, Actor other) {
    Log = text_append(Log, "out:", actor_name(actor));
  }

  KERN_EVENT(on72, game_touched_sensor)
  void on72(Actor actor, Actor other) {
    Log = text_append(Log, "touched:", actor_name(other));
  }

  KERN_EVENT(on73, game_hit)
  void on73(Actor actor, Actor other, Float speed, Vector point, Vector normal) {
    Hits += 1;
    Impact = math_add(Impact, speed);
    HitAt = vector_add(point, normal);
  }

  KERN_EVENT(on74, game_pickup)
  void on74(Actor actor, Actor other, Float points) {
    Log = text_append(Log, "pickup:", kern::to<Text>(points));
  }

  KERN_EVENT(on75, game_trigger)
  void on75(Actor actor, Actor other) {
    Log = text_append(Log, "trigger:", actor_name(actor));
  }

  KERN_EVENT(on76, game_damaged)
  void on76(Actor actor, Actor other, Float amount, Bool head) {
    Damage = math_add(Damage, amount);
    Log = text_append(Log, "hurt:", actor_name(actor));
  }

  KERN_EVENT(on77, game_death)
  void on77(Actor actor, Actor other, Bool head) {
    Log = text_append(Log, "death:", actor_name(actor));
  }

  KERN_EVENT(on78, game_respawn)
  void on78(Actor actor) {
    Log = text_append(Log, "respawn:", actor_name(actor));
  }

  KERN_EVENT(on79, game_interact)
  void on79(Actor actor, Actor other, Vector point) {
    Log = text_append(Log, "interact:", actor_name(actor));
  }

  KERN_EVENT(on80, game_pawn_added)
  void on80(Actor actor) {
    Players = actor_player_count();
    Log = text_append(Log, "joined:", actor_name(actor));
  }

  KERN_EVENT(on81, game_over)
  void on81(Bool won, Text message) {
    Log = text_append(Log, "over:", message);
    Won = won;
  }

  KERN_EVENT(on82, game_input<"jump", "pressed">)
  void on82(Actor actor, Bool pressed) {
    Log = text_append(Log, "jump", ";");
  }

  KERN_EVENT(on83, game_key<"F", "both">)
  void on83(Bool pressed, Actor actor) {
    Log = text_append(Log, "key:", kern::to<Text>(pressed));
  }

  kern::Node<20> n20;
  struct { Bool hit = false; Actor actor = Actor{0}; Vector location = Vector{0.0, 0.0, 0.0}; Vector normal = Vector{0.0, 0.0, 0.0}; } n40;
  struct { Actor spawned = Actor{0}; } n50;
  struct : kern::Node<60> { Actor actor = Actor{0}; Int index = 0; } n60;
  struct : kern::Node<61> { Actor item = Actor{0}; Int index = 0; } n61;
};
)kn";

/// A sensor volume: whoever enters scores and opens the door (Self as a value).
inline const char* kZone = R"kn(
struct Zone : kern::Blueprint {
  Int Visits = 0;

  KERN_EVENT(on1, game_overlap_begin)
  void on1(Actor actor, Actor other) {
    Visits += 1;
    game_add_score(5.0);
    flow_call<"OpenDoor">(Actor{0}, actor_self(), 1.0, "");
  }
};
)kn";

/// The door moves itself up over a second (an Actor input left empty is Self; a Tween).
inline const char* kDoor = R"kn(
struct Door : kern::Blueprint {
  Text Opener = "";

  KERN_EVENT(on1, event_custom<"OpenDoor">)
  void on1(Actor actor, Float value, Text text) {
    Opener = actor_name(actor);
    flow_tween<"linear">(n2, "play", 1.0, {
      {"update", [this] {
        actor_set_location(Actor{0}, vector_lerp(Vector{10.0, 0.0, 1.5}, Vector{10.0, 0.0, 4.5}, n2.alpha));
      }},
      {"finished", [this] {
        actor_set_color(actor_self(), Vector{0.2, 0.8, 0.2});
      }},
    });
  }

  struct : kern::Node<2> { Float alpha = 0.0; } n2;
};
)kn";

/// Three crates share this Blueprint: one class, three instances.
inline const char* kCrate = R"kn(
struct Crate : kern::Blueprint {
  Int Bumps = 0;
  Vector Home = Vector{0.0, 0.0, 0.0};

  KERN_EVENT(on1, event_begin)
  void on1() {
    Home = actor_location(actor_self());
  }

  KERN_EVENT(on2, game_hit)
  void on2(Actor actor, Actor other, Float speed, Vector point, Vector normal) {
    Bumps += 1;
  }

  KERN_EVENT(on3, event_custom<"Home">)
  void on3(Actor actor, Float value, Text text) {
    actor_set_location(Actor{0}, Home);
    actor_set_velocity(Actor{0}, Vector{0.0, 0.0, 0.0});
  }
};
)kn";

/// Something with health that looks after itself.
inline const char* kDummy = R"kn(
struct Dummy : kern::Blueprint {
  Float Taken = 0.0;
  Int Deaths = 0;

  KERN_EVENT(on1, game_damaged)
  void on1(Actor actor, Actor other, Float amount, Bool head) {
    Taken += amount;
    if (math_compare<"<">(actor_health(Actor{0}).health, 30.0)) {
      combat_heal(Actor{0}, 10.0);
    }
  }

  KERN_EVENT(on2, game_death)
  void on2(Actor actor, Actor other, Bool head) {
    Deaths += 1;
    game_particles(actor_location(Actor{0}), Vector{1.0, 0.0, 0.0});
  }
};
)kn";

/// The Blueprint of the robot's room: every quarter of a second it reads what the robot senses
/// (values several inputs read: one name each); the scenario drives the robot through events.
inline const char* kRobot = R"kn(
struct Level : kern::Blueprint {
  Text State = "";
  Bool Seen = false;
  Bool Holding = false;
  Bool Blocked = false;
  Bool Fallen = false;
  Float Near = 0.0;
  Float Bearing = 0.0;
  Float Free = 0.0;
  Float Speed = 0.0;
  Float BallBearing = 0.0;
  Float BallDistance = 0.0;
  Int Looks = 0;

  KERN_EVENT(on1, event_every<"0.25">)
  void on1(Int count) {
    const auto n10 = [&] { return walker_sees(Actor{0}, actor_find("Ball")); };
    const auto n20 = [&] { return walker_tof(Actor{0}); };
    const auto n30 = [&] { return walker_state(Actor{0}); };
    Looks += 1;
    Seen = n10().visible;
    BallBearing = n10().bearing;
    BallDistance = n10().distance;
    Blocked = n20().blocked;
    Near = n20().distance;
    Bearing = n20().bearing;
    Free = math_add(n20().left, n20().right);
    State = n30().state;
    Fallen = n30().fallen;
    Speed = n30().speed;
    Holding = walker_holding(Actor{0});
  }

  KERN_EVENT(on2, event_custom<"Drive">)
  void on2(Actor actor, Float value, Text text) {
    walker_autonomous(Actor{0}, false);
    walker_move(Actor{0}, value, 0.0, 20.0, 0.0);
    walker_look(Actor{0}, 30.0, 10.0, 0.5);
    walker_beak(Actor{0}, 1.0);
  }

  KERN_EVENT(on3, event_custom<"Play">)
  void on3(Actor actor, Float value, Text text) {
    walker_skill(Actor{0}, text);
    walker_pet(Actor{0}, value);
    walker_hear(Actor{0}, true);
    walker_quack(Actor{0});
  }

  KERN_EVENT(on4, event_custom<"Free">)
  void on4(Actor actor, Float value, Text text) {
    walker_autonomous(Actor{0}, true);
  }
};
)kn";

inline logic::Graph blueprint(const char* text, std::string* error) {
  logic::Graph g;
  std::string why;
  if (!logic::from_cpp(text, g, nullptr, &why) && error && error->empty()) *error = why;
  return g;
}

/// The Arena. `error`: a Blueprint above that is not the notation (empty: none).
inline game::Level arena(std::string* error = nullptr) {
  using namespace kern::game;
  (void)logic_nodes_registered();
  std::string why;
  Level l = basic_level();
  l.name = "Arena";
  for (Actor& a : l.actors)
    if (a.name == "Cube") a.transform.position = {40, 40, 0.5f};
  auto actor = [](const char* name, Vec3 at) {
    Actor a;
    a.name = name;
    a.transform.position = at;
    return a;
  };
  auto box = [](Vec3 size) {
    MeshComponent m;
    m.shape = Shape::box;
    m.size = size;
    return m;
  };
  auto sensor = [](Vec3 size, Vec3 center) {
    ColliderComponent c;
    c.shape = ColliderShape::box;
    c.size = size;
    c.center = center;
    c.sensor = true;
    return c;
  };
  {  // the player
    Actor body = actor("Soldier", {0, 0, 0.8f});
    body.mesh = MeshComponent{};
    body.mesh->shape = Shape::capsule;
    body.mesh->radius = 0.35f;
    body.mesh->height = 1.45f;
    body.body = BodyComponent{};
    body.body->mass = 80;
    body.controller = ControllerComponent{};
    body.controller->mode = ControlMode::soldier;
    body.controller->camera = CameraMode::first_person;
    body.controller->max_speed = 5.5f;
    body.controller->boost_speed = 8.1f;
    body.controller->jump_speed = 3.6f;
    body.weapon = WeaponComponent{};
    body.health = HealthComponent{};
    body.health->respawn_delay = 1;
    const ActorId id = l.add(body).id;
    Actor head = actor("Head", {0, 0, 0.83f});
    head.parent = id;
    head.mesh = MeshComponent{};
    head.mesh->shape = Shape::sphere;
    head.mesh->radius = 0.17f;
    head.tags.push_back("head");
    l.add(head);
  }
  {
    Actor zone = actor("Zone", {6, 0, 0});
    zone.colliders.push_back(sensor({2, 2, 2}, {0, 0, 1}));
    zone.logic = blueprint(kZone, &why);
    l.add(zone);
    Actor door = actor("Door", {10, 0, 1.5f});
    door.mesh = box({0.3f, 3, 3});
    door.logic = blueprint(kDoor, &why);
    l.add(door);
  }
  {
    Actor orb = actor("Orb", {-6, 0, 0.8f});
    orb.mesh = MeshComponent{};
    orb.mesh->shape = Shape::sphere;
    orb.mesh->radius = 0.4f;
    orb.mesh->collision = false;
    orb.pickup = PickupComponent{};
    orb.pickup->points = 3;
    ColliderComponent c;
    c.shape = ColliderShape::sphere;
    c.radius = 1;
    c.sensor = true;
    orb.colliders.push_back(c);
    l.add(orb);
    Actor pad = actor("Pad", {0, -6, 0});
    pad.trigger = TriggerComponent{TriggerAction::message, "On the pad", false};
    pad.colliders.push_back(sensor({2, 2, 2}, {0, 0, 1}));
    l.add(pad);
  }
  for (int i = 0; i < 3; ++i) {
    const char* names[] = {"Crate A", "Crate B", "Crate C"};
    Actor crate = actor(names[i], {3 + 1.5f * static_cast<f32>(i), 6, 3 + 2 * static_cast<f32>(i)});
    crate.mesh = box({0.6f, 0.6f, 0.6f});
    crate.body = BodyComponent{};
    crate.body->mass = 20;
    crate.tags.push_back("crate");
    crate.logic = blueprint(kCrate, &why);
    l.add(crate);
  }
  {
    Actor dummy = actor("Dummy", {-4, 6, 0.9f});
    dummy.mesh = MeshComponent{};
    dummy.mesh->shape = Shape::capsule;
    dummy.mesh->radius = 0.32f;
    dummy.mesh->height = 1.45f;
    dummy.health = HealthComponent{};
    dummy.health->respawn_delay = 0.5f;
    dummy.tags.push_back("enemy");
    dummy.logic = blueprint(kDummy, &why);
    l.add(dummy);
    Actor lamp = actor("Lamp", {0, 8, 3});
    lamp.light = LightComponent{};
    l.add(lamp);
    Actor sign = actor("Sign", {-2, -3, 1});
    sign.mesh = box({1, 0.1f, 0.6f});
    l.add(sign);
    Actor sleeper = actor("Sleeper", {-8, 6, 4});
    sleeper.mesh = box({0.5f, 0.5f, 0.5f});
    sleeper.body = BodyComponent{};
    sleeper.body->motion = Motion::kinematic;
    sleeper.body->mass = 5;
    l.add(sleeper);
    Actor flap = actor("Flap", {-8, -6, 1.5f});
    flap.mesh = box({1.6f, 0.1f, 0.8f});
    flap.body = BodyComponent{};
    flap.body->mass = 8;
    JointComponent hinge;
    hinge.kind = JointKind::hinge;
    hinge.anchor = {-0.8f, 0, 0};
    hinge.axis = {0, 0, 1};
    flap.joints.push_back(hinge);
    l.add(flap);
    Actor tpl = actor("Template", {30, 30, 0.5f});
    tpl.mesh = box({0.4f, 0.4f, 0.4f});
    tpl.tags.push_back("made");
    l.add(tpl);
  }
  l.logic = blueprint(kLevel, &why);
  if (error) *error = why;
  return l;
}

struct Fixture {
  Renderer renderer;
  game::NightWindows windows;
  std::unique_ptr<game::GameWorld> world;
  Fixture() {
    ShaderExtension* ext[1] = {&windows};
    RendererOptions o;
    o.shadows = true;
    renderer.init(o, Span<ShaderExtension* const>(ext, 1));
    world = std::make_unique<game::GameWorld>(renderer, &windows);
  }
  ~Fixture() {
    world.reset();
    renderer.shutdown();
  }
};

/// Everything the Blueprints did that can be seen: the HUD, every Blueprint's variables, every
/// actor's place and health, what the game reported.
inline std::string dump(game::GameWorld& w, const char* at) {
  char b[256];
  std::string s = std::string("-- ") + at + "\n";
  const game::Hud& hud = w.hud();
  std::snprintf(b, sizeof b, "hud: status %d score %.9g pickups %u message \"", static_cast<int>(hud.status), static_cast<double>(hud.score), hud.pickups);
  s += b + hud.message + "\"\n";
  if (logic::Runtime* rt = w.logic_runtime())
    for (const auto& inst : rt->instances()) {
      s += "blueprint " + inst->name() + " #" + std::to_string(inst->self()) + ":";
      for (const logic::Variable& v : inst->graph().variables) s += " " + v.name + "=" + native_show(inst->get(v.name));
      s += "\n";
    }
  for (const game::Actor& a : w.level().actors) {
    Mat4 m;
    const bool there = w.actor_matrix(a.id, m);
    const Vec3 p = there ? m.translation() : Vec3{};
    std::snprintf(b, sizeof b, "actor %u %s: %s at (%.9g, %.9g, %.9g) health %.9g\n", a.id, a.name.c_str(), w.actor_alive(a.id) ? "alive" : "gone", static_cast<double>(p.x),
                  static_cast<double>(p.y), static_cast<double>(p.z), static_cast<double>(w.health_of(a.id)));
    s += b;
  }
  for (const std::string& e : w.events()) s += "event: " + e + "\n";
  w.events().clear();
  return s;
}

/// A scenario at work: the world, time, and what it shows.
struct Stage {
  game::GameWorld& w;
  logic::Runtime& rt;
  std::string& text;
  Camera cam;
  game::PlayInput in;
  void tick(int n) {
    for (int i = 0; i < n; ++i) {
      w.step(1.0f / 60, in);
      w.update_camera(cam, 1.0f / 60, in);
    }
  }
  game::ActorId id(const char* name) const { return w.find_actor(name); }
  /// A Custom Event of every Blueprint, with its three values.
  void call(const char* event, game::ActorId actor, f64 value, const char* words) {
    rt.call(event, 0, {{"actor", logic::Value::actor(actor)}, {"value", logic::Value::number(value)}, {"text", logic::Value::text(words)}});
  }
  void show(const char* at) { text += dump(w, at); }
};

inline void arena_scenario(Stage& s) {
  game::GameWorld& w = s.w;
  const game::ActorId me = w.player();
  s.show("begin");
  s.tick(40);  // the crates fall: hits
  s.show("the crates fell");
  s.call("Values", s.id("Dummy"), 0, "crate");
  s.show("values");
  w.teleport(me, {6, 0, 0.8f});  // into the zone: overlap, score, the door opens
  s.tick(5);
  s.show("in the zone");
  s.tick(70);
  w.teleport(me, {0, 0, 0.8f});
  s.tick(5);
  s.show("out of the zone, the door is open");
  w.teleport(me, {-6, 0, 0.8f});  // the orb
  s.tick(5);
  w.teleport(me, {0, -6, 0.8f});  // the pad
  s.tick(5);
  s.show("pickup and trigger");
  s.call("Moves", s.id("Sign"), 30, "0.25");
  s.tick(2);
  s.show("moves");
  s.call("Physics", s.id("Crate A"), 120, "Crate B");
  s.tick(30);
  s.show("physics");
  s.call("Boom", s.id("Crate C"), 4, "");
  s.tick(20);
  s.show("boom");
  for (int i = 0; i < 3; ++i) s.call("Combat", s.id("Dummy"), 30, "");
  s.tick(2);
  s.show("combat");
  s.call("Kill", s.id("Dummy"), 0, "");
  s.tick(60);
  s.show("the dummy died and came back");
  s.call("Effects", s.id("Lamp"), 7, "");
  s.call("Spawn", 0, 2, "");
  s.tick(5);
  s.show("effects and a spawned actor");
  s.call("Each", 0, 0, "crate");
  s.call("Each", 0, 0, "nobody");
  s.tick(5);
  s.call("Remove", 0, 0, "");
  for (const char* crate : {"Crate A", "Crate B", "Crate C"}) s.rt.call("Home", s.id(crate));
  s.tick(2);
  s.show("each, remove, home");
  s.in.jump = true;  // input
  s.tick(2);
  s.in.jump = false;
  s.tick(30);
  w.key_event("KeyF", true);
  w.key_event("KeyF", false);
  w.key_event("KeyG", true);
  w.teleport(me, {-5.5f, 6, 0.8f});  // in front of the dummy
  s.tick(2);
  s.in.interact = true;
  s.tick(2);
  s.in.interact = false;
  s.tick(2);
  s.show("input, a key, interact");
  w.add_pawn();  // a second player
  s.tick(5);
  s.show("a player joined");
  s.call("Combat", me, 40, "");
  s.call("Kill", me, 0, "");
  s.tick(90);
  s.show("the player died and came back");
  s.call("Values", me, 0, "enemy");
  s.call("Win", 0, 0, "Done");
  s.tick(2);
  s.call("Lose", 0, 0, "Late");
  s.tick(2);
  s.show("the end");
}

inline void robot_scenario(Stage& s) {
  s.show("begin");
  s.tick(45);  // on its own
  s.show("on its own");
  s.call("Drive", 0, 0.3, "");
  s.tick(60);
  s.show("driven");
  s.call("Play", 0, 1, "sit");
  s.tick(45);
  s.show("sit, pet, hear, chirp");
  s.call("Free", 0, 0, "");
  s.tick(30);
  s.show("free again");
}

struct Case {
  const char* name;  // (the level file is <name>.klevel)
  game::Level level;
  void (*scenario)(Stage&);
};

/// The cases. `error`: a Blueprint above that is not the notation (empty: none).
inline std::vector<Case> cases(std::string* error = nullptr) {
  std::vector<Case> all;
  std::string why;
  all.push_back({"arena", arena(&why), arena_scenario});
  game::Level room = game::micro_duck_level();
  room.logic = blueprint(kRobot, &why);
  all.push_back({"robot", std::move(room), robot_scenario});
  if (error) *error = why;
  return all;
}

struct Played {
  std::string text;                // the dumps
  std::vector<std::string> tiers;  // how each Blueprint ran ("native C++", "interpreter" ...)
};

/// Plays `scenario` on `level`. `native`: its Blueprints as the classes the program has
/// (GameWorld::logic_native); otherwise in the interpreter.
inline Played play(const game::Level& level, void (*scenario)(Stage&), bool native) {
  Played out;
  Fixture f;
  game::GameWorld& w = *f.world;
  w.logic_native = native;
  if (!w.start(level, game::SimMode::play) || !w.logic_runtime()) {
    out.text = std::string("(the level did not start: ") + last_error() + ")\n";
    return out;
  }
  logic::Runtime& rt = *w.logic_runtime();
  rt.jit = false;
  Stage stage{w, rt, out.text, {}, {}};
  scenario(stage);
  for (const auto& inst : rt.instances()) out.tiers.push_back(inst->name() + ": " + inst->tier());
  w.stop();
  return out;
}

}  // namespace game_cases
