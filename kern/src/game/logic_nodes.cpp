// Kern Logic: what the gameplay nodes do. Each is a typed function on the GameWorld
// (kern::game::nodes, declared in kern/game/blueprint_nodes.h): bound here to its node of the
// library for the interpreter (the node's names and pins are in logic_node_shapes.cpp); a
// Blueprint's C++ calls the function directly. Bound at start-up.
#include <cmath>
#include <cstdlib>
#include <type_traits>
#include <utility>

#include <functional>

#include "kern/game/blueprint_nodes.h"
#include "kern/game/logic_nodes.h"
#include "kern/game/runtime.h"
#include "kern/logic/nodes.h"
#include "kern/logic/logic.h"

namespace kern::game {

// ---------------------------------------------------------------------------
// What the nodes do
// ---------------------------------------------------------------------------
namespace nodes {

namespace {
/// An Actor input: the actor given, or Self when it is left empty.
ActorId or_self(Actor target, Self self) { return target.id ? target.id : self; }
Vec3 position(GameWorld& w, ActorId id) {
  Mat4 m;
  return w.actor_matrix(id, m) ? m.translation() : Vec3{};
}
}  // namespace

Actor actor_player(GameWorld& w, Self, Int index) {
  const std::vector<ActorId> p = w.pawns();
  ActorId a = index == 0 ? w.player() : 0;
  if (!a && index >= 0 && static_cast<std::size_t>(index) < p.size()) a = p[static_cast<std::size_t>(index)];
  return {a};
}
Int actor_player_count(GameWorld& w, Self) { return static_cast<Int>(w.pawns().size()); }
Actor actor_find(GameWorld& w, Self, std::string_view name) { return {w.find_actor(name)}; }
Found actor_find_tag(GameWorld& w, Self, std::string_view tag) {
  std::vector<ActorId> v;
  w.actors_with_tag(tag, v);
  return {{v.empty() ? 0 : v.front()}, static_cast<Int>(v.size())};
}
std::vector<Actor> actor_all_with_tag(GameWorld& w, Self, std::string_view tag) {
  std::vector<ActorId> v;
  w.actors_with_tag(tag, v);
  std::vector<Actor> all;
  for (ActorId id : v) all.push_back({id});
  return all;
}
Actor actor_random_tag(GameWorld& w, Self, f64* memory, u32 seed, u32 node, std::string_view tag) {
  std::vector<ActorId> v;
  w.actors_with_tag(tag, v);
  return {v.empty() ? 0 : v[logic::random::next(memory, seed, node) % v.size()]};
}
Bool actor_has_tag(GameWorld& w, Self self, Actor target, std::string_view tag) { return w.has_tag(or_self(target, self), tag); }
Text actor_name(GameWorld& w, Self self, Actor target) {
  const game::Actor* a = w.level().find(or_self(target, self));
  return a ? a->name : Text();
}
Bool actor_valid(GameWorld& w, Self, Actor target) { return target.id && w.level().find(target.id) && w.actor_alive(target.id); }
Vector actor_location(GameWorld& w, Self self, Actor target) { return position(w, or_self(target, self)); }
Vector actor_rotation(GameWorld& w, Self self, Actor target) {
  Mat4 m;
  return w.actor_matrix(or_self(target, self), m) ? matrix_transform(m).rotation : Vec3{};
}
Vector actor_forward(GameWorld& w, Self self, Actor target) {
  Mat4 m;
  return w.actor_matrix(or_self(target, self), m) ? normalize(m.transform_vector({1, 0, 0}), {1, 0, 0}) : Vec3{1, 0, 0};
}
Velocity actor_velocity(GameWorld& w, Self self, Actor target) {
  const Vec3 v = w.actor_velocity(or_self(target, self));
  return {v, length(v)};
}
Float actor_distance(GameWorld& w, Self self, Actor a, Actor b) { return distance(position(w, or_self(a, self)), position(w, b.id)); }
Health actor_health(GameWorld& w, Self self, Actor target) {
  const f32 h = w.health_of(or_self(target, self));
  return {h, h == 0};
}
Score game_score(GameWorld& w, Self) { return {w.hud().score, static_cast<Int>(w.hud().pickups)}; }

void actor_set_location(GameWorld& w, Self self, Actor target, Vector location) { w.teleport(or_self(target, self), location); }
void actor_set_rotation(GameWorld& w, Self self, Actor target, Vector rotation) {
  const ActorId t = or_self(target, self);
  w.teleport(t, position(w, t), &rotation);
}
void actor_move_by(GameWorld& w, Self self, Actor target, Vector offset) {
  const ActorId t = or_self(target, self);
  w.teleport(t, position(w, t) + offset);
}
void actor_set_velocity(GameWorld& w, Self self, Actor target, Vector velocity) { w.set_actor_velocity(or_self(target, self), velocity); }
void actor_impulse(GameWorld& w, Self self, Actor target, Vector impulse) { w.push_actor(or_self(target, self), impulse, true); }
void actor_force(GameWorld& w, Self self, Actor target, Vector force) { w.push_actor(or_self(target, self), force, false); }
void actor_launch(GameWorld& w, Self self, Actor target, Vector velocity) {
  const ActorId t = or_self(target, self);
  w.set_actor_velocity(t, w.actor_velocity(t) * Vec3{1, 1, 0} + velocity);
}
void actor_simulate(GameWorld& w, Self self, Actor target, Bool simulate) { w.set_simulate_physics(or_self(target, self), simulate); }
void actor_joint_motor(GameWorld& w, Self self, Actor target, Int joint, Bool enable, Float speed, Float force) {
  w.set_joint_motor(or_self(target, self), static_cast<i32>(joint), enable, static_cast<f32>(speed), static_cast<f32>(force));
}
void actor_explode(GameWorld& w, Self self, Vector location, Float radius, Float impulse, Float damage) {
  const f32 r = static_cast<f32>(max_of(0.1, radius));
  w.physics().explode(location, r, r * 0.5f, static_cast<f32>(impulse));
  w.particle_burst(location, {3.0f, 1.2f, 0.3f});
  w.play_sound("builtin:explosion", location, 1, 1, true);
  w.shake_camera(0.6f);
  const f32 dmg = static_cast<f32>(damage);
  if (dmg > 0)
    for (const game::Actor& a : w.level().actors) {
      if (w.health_of(a.id) <= 0) continue;
      const f32 d = distance(position(w, a.id), location);
      if (d < r) w.damage(a.id, dmg * (1 - d / r), self, false, location);
    }
}
Actor actor_spawn(GameWorld& w, Self, logic::Host& host, std::string_view named, Actor tpl, Vector location, Vector rotation) {
  ActorId source = tpl.id;
  if (!source) source = w.find_actor(named);
  const ActorId id = source ? w.spawn_copy(source, location, rotation) : 0;
  if (!id) host.error("Spawn Actor: no template \"" + std::string(named) + "\"");
  return {id};
}
void actor_destroy(GameWorld& w, Self self, Actor target) { w.destroy_actor(or_self(target, self)); }
void actor_enable(GameWorld& w, Self self, Actor target, Bool enabled) { w.set_alive(or_self(target, self), enabled); }
void actor_set_property(GameWorld& w, Self self, logic::Host& host, Actor target, std::string_view path, std::string_view value) {
  json::Value v;
  if (!json::parse(value, v)) v = json::Value(value);
  if (!w.set_property(or_self(target, self), std::string(path), v)) host.error(std::string("Set Property: ") + last_error());
}
void actor_set_light(GameWorld& w, Self self, Actor target, Vector color, Float intensity, Float range) {
  const ActorId t = or_self(target, self);
  json::Value rgb = json::Value::array();
  rgb.push(static_cast<f64>(color.x));
  rgb.push(static_cast<f64>(color.y));
  rgb.push(static_cast<f64>(color.z));
  w.set_property(t, "components.light.color", rgb);
  w.set_property(t, "components.light.intensity", json::Value(intensity));
  w.set_property(t, "components.light.range", json::Value(range));
}
void actor_set_color(GameWorld& w, Self self, Actor target, Vector color) {
  char hex[8];
  auto b = [](f32 v) { return static_cast<int>(std::lround(clamp(v, 0.0f, 1.0f) * 255)); };
  std::snprintf(hex, sizeof hex, "#%02x%02x%02x", b(color.x), b(color.y), b(color.z));
  w.set_property(or_self(target, self), "components.mesh.material.color", json::Value(hex));
}

void game_message(GameWorld& w, Self, std::string_view text) { w.show_message(std::string(text)); }
void game_add_score(GameWorld& w, Self, Float points) { w.add_score(static_cast<f32>(points)); }
void game_win(GameWorld& w, Self, std::string_view message) { w.end_game(true, std::string(message)); }
void game_lose(GameWorld& w, Self, std::string_view message) { w.end_game(false, std::string(message)); }
void game_sound(GameWorld& w, Self self, std::string_view sound, Vector location, Float volume, Float pitch, Bool spatial) {
  Vec3 at = location;
  if (length_sq(at) == 0 && self) at = position(w, self);
  w.play_sound(std::string(sound), at, static_cast<f32>(volume), static_cast<f32>(pitch), spatial, self);
}
void game_particles(GameWorld& w, Self, Vector location, Vector color) { w.particle_burst(location, color * 1.6f); }
void game_shake(GameWorld& w, Self, Float amount) { w.shake_camera(static_cast<f32>(amount)); }
void combat_damage(GameWorld& w, Self self, Actor target, Float amount, Actor instigator) {
  const ActorId t = or_self(target, self);
  w.damage(t, static_cast<f32>(amount), instigator.id, false, position(w, self ? self : t));
}
void combat_heal(GameWorld& w, Self self, Actor target, Float amount) { w.heal(or_self(target, self), static_cast<f32>(amount)); }
void combat_kill(GameWorld& w, Self self, Actor target) { w.damage(or_self(target, self), 1e9f, 0, false, {}); }
void combat_ammo(GameWorld& w, Self self, Actor target, Int rounds) { w.give_ammo(or_self(target, self), static_cast<i32>(rounds)); }
Trace physics_trace(GameWorld& w, Self, Vector start, Vector end, Actor ignore) {
  ActorId a = 0;
  Vec3 p, n;
  const bool hit = w.line_trace(start, end, ignore.id, a, p, n);
  return {hit, {a}, p, n};
}
namespace {
/// A Blueprint's whole number as channel bits (a level has 32; anything else: all of them).
u64 as_bits(Int v) { return v < 0 || v > 0xffffffffll ? ~0ull : static_cast<u64>(v); }
}  // namespace
Trace physics_trace_channels(GameWorld& w, Self, Vector start, Vector end, Actor ignore, Int mask, Int category) {
  ActorId a = 0;
  Vec3 p, n;
  const bool hit = w.line_trace(start, end, ignore.id, a, p, n, physics::QueryFilter(as_bits(mask), as_bits(category)));
  return {hit, {a}, p, n};
}
Int physics_channels(GameWorld& w, Self, std::string_view names) { return static_cast<Int>(channel_bits(w.level().world, names)); }
std::vector<logic::ActorRef> actors_with_tag(GameWorld& w, std::string_view tag) {
  std::vector<ActorId> v;
  w.actors_with_tag(tag, v);
  return v;
}

void walker_move(GameWorld& w, Self self, Actor target, Float forward, Float strafe, Float turn, Float duration) {
  w.walker_command(or_self(target, self), static_cast<f32>(forward), static_cast<f32>(strafe), static_cast<f32>(turn * 3.14159265358979 / 180), static_cast<f32>(duration));
}
void walker_look(GameWorld& w, Self self, Actor target, Float yaw, Float pitch, Float duration) {
  constexpr f64 k = 3.14159265358979 / 180;
  w.walker_look(or_self(target, self), static_cast<f32>(yaw * k), static_cast<f32>(pitch * k), static_cast<f32>(duration));
}
void walker_autonomous(GameWorld& w, Self self, Actor target, Bool enable) { w.walker_set_autonomous(or_self(target, self), enable); }
void walker_skill(GameWorld& w, Self self, Actor target, std::string_view move) { w.walker_skill(or_self(target, self), move); }
void walker_beak(GameWorld& w, Self self, Actor target, Float open) { w.walker_mouth(or_self(target, self), static_cast<f32>(open)); }
void walker_pet(GameWorld& w, Self self, Actor target, Float seconds) { w.walker_pet(or_self(target, self), static_cast<f32>(seconds)); }
void walker_hear(GameWorld& w, Self self, Actor target, Bool voice) { w.walker_hear(or_self(target, self), voice); }
Bool walker_holding(GameWorld& w, Self self, Actor target) { return w.walker_holding(or_self(target, self)); }
void walker_quack(GameWorld& w, Self self, Actor target) { w.walker_quack(or_self(target, self)); }
Tof walker_tof(GameWorld& w, Self self, Actor target) {
  GameWorld::WalkerSensors s;
  const bool ok = w.walker_sensors(or_self(target, self), s);
  return {ok && s.nearest >= 0, ok ? s.nearest : -1.0, ok ? s.nearest_bearing * 180 / 3.14159265358979 : 0.0, ok ? s.free_left : 0.0, ok ? s.free_right : 0.0};
}
Sees walker_sees(GameWorld& w, Self self, Actor target, Actor other) {
  bool visible = false;
  f32 bearing = 0, distance = -1;
  w.walker_sees(or_self(target, self), other.id, visible, bearing, distance);
  return {visible, bearing * 180 / 3.14159265358979, distance};
}
Robot walker_state(GameWorld& w, Self self, Actor target) {
  const ActorId robot = or_self(target, self);
  GameWorld::WalkerSensors s;
  const bool ok = w.walker_sensors(robot, s);
  return {w.walker_state(robot), ok && s.fallen, ok ? s.speed : 0.0};
}

}  // namespace nodes

// ---------------------------------------------------------------------------
// The functions as the nodes of the library
// ---------------------------------------------------------------------------
namespace {

using logic::Context;
using logic::NodeDef;
using logic::NodeKind;
using logic::Value;
using logic::kStop;

GameWorld* world(Context& c) { return static_cast<GameWorld*>(c.host().game()); }

// What a function gives are the node's data outputs: one value, or a struct with a field per output.
template <class T> void give(Context& c, std::size_t first, const T& r) { c.out(first, logic::to_value(r)); }
void give(Context& c, std::size_t first, const nodes::Found& r) {
  c.out(first, Value::actor(r.actor.id));
  c.out(first + 1, Value::integer(r.count));
}
void give(Context& c, std::size_t first, const nodes::Velocity& r) {
  c.out(first, Value::vector(r.velocity));
  c.out(first + 1, Value::number(r.speed));
}
void give(Context& c, std::size_t first, const nodes::Health& r) {
  c.out(first, Value::number(r.health));
  c.out(first + 1, Value::boolean(r.dead));
}
void give(Context& c, std::size_t first, const nodes::Score& r) {
  c.out(first, Value::number(r.score));
  c.out(first + 1, Value::integer(r.pickups));
}
void give(Context& c, std::size_t first, const nodes::Trace& r) {
  c.out(first, Value::boolean(r.hit));
  c.out(first + 1, Value::actor(r.actor.id));
  c.out(first + 2, Value::vector(r.location));
  c.out(first + 3, Value::vector(r.normal));
}
void give(Context& c, std::size_t first, const nodes::Tof& r) {
  c.out(first, Value::boolean(r.blocked));
  c.out(first + 1, Value::number(r.distance));
  c.out(first + 2, Value::number(r.bearing));
  c.out(first + 3, Value::number(r.left));
  c.out(first + 4, Value::number(r.right));
}
void give(Context& c, std::size_t first, const nodes::Sees& r) {
  c.out(first, Value::boolean(r.visible));
  c.out(first + 1, Value::number(r.bearing));
  c.out(first + 2, Value::number(r.distance));
}
void give(Context& c, std::size_t first, const nodes::Robot& r) {
  c.out(first, Value::text(r.state));
  c.out(first + 1, Value::boolean(r.fallen));
  c.out(first + 2, Value::number(r.speed));
}
/// An input of a node as its function takes it. A text is read where the value holds it
/// (std::string_view): nothing is copied.
template <class T> T taken(Value& v) {
  if constexpr (std::is_same_v<T, std::string_view>) {
    if (v.type != logic::PinType::text) v = Value::text(v.as_text());
    return v.s;
  } else {
    return logic::from_value<T>(v);
  }
}
/// Runs `f` for the node: every input is read once, in its order - with a game or without (the
/// arguments of a call); without a game the node does nothing and gives nothing.
template <class R, class... A, std::size_t... I>
void run_typed(R (*f)(GameWorld&, logic::ActorRef, A...), Context& c, std::size_t first, std::index_sequence<I...>) {
  Value v[sizeof...(A) + 1];
  for (std::size_t i = 0; i < sizeof...(A); ++i) v[i] = c.in(first + i);
  GameWorld* w = world(c);
  if (!w) return;
  if constexpr (std::is_void_v<R>) f(*w, c.self(), taken<std::decay_t<A>>(v[I])...);
  else give(c, first, f(*w, c.self(), taken<std::decay_t<A>>(v[I])...));
}

/// The node to bind: described in logic_node_shapes.cpp, not bound yet. (A node that stays
/// unbound - a wrong name here, or a function that does not fit its pins - is found by the test
/// game_blueprints_as_classes: every node of the library does something.)
NodeDef* node(logic::Library& lib, const char* type) {
  NodeDef* d = lib.edit(type);
  return d && !d->run ? d : nullptr;
}
/// Binds a function of kern::game::nodes to its node: its arguments after the game and Self are
/// the node's data inputs in their order (a step's first pin is its exec pin).
template <class R, class... A> void bind(logic::Library& lib, const char* type, R (*f)(GameWorld&, logic::ActorRef, A...)) {
  NodeDef* d = node(lib, type);
  if (!d) return;
  const bool value = d->kind == NodeKind::pure;
  if (d->inputs.size() != sizeof...(A) + (value ? 0 : 1)) return;
  d->run = [f, value](Context& c) {
    run_typed(f, c, value ? 0 : 1, std::index_sequence_for<A...>{});
    return value ? kStop : 0;
  };
}
/// Binds what a node does that is written for the interpreter alone.
void bind(logic::Library& lib, const char* type, std::function<int(Context&)> run) {
  if (NodeDef* d = node(lib, type)) d->run = std::move(run);
}
void bind_actor_nodes(logic::Library& lib) {
  bind(lib, "actor.self", [](Context& c) {
    if (world(c)) c.out("actor", Value::actor(c.self()));
    return kStop;
  });
  bind(lib, "actor.player", &nodes::actor_player);
  bind(lib, "actor.player_count", &nodes::actor_player_count);
  bind(lib, "actor.find", &nodes::actor_find);
  bind(lib, "actor.find_tag", &nodes::actor_find_tag);
  bind(lib, "actor.all_with_tag", &nodes::actor_all_with_tag);
  bind(lib, "actor.random_tag", [](Context& c) {
    const std::string tag = c.in_text("tag");
    if (GameWorld* w = world(c)) c.out("actor", logic::to_value(nodes::actor_random_tag(*w, c.self(), &c.slot(0), c.instance().seed(), c.node().id, tag)));
    return kStop;
  });
  bind(lib, "actor.has_tag", &nodes::actor_has_tag);
  bind(lib, "actor.name", &nodes::actor_name);
  bind(lib, "actor.valid", &nodes::actor_valid);
  bind(lib, "actor.location", &nodes::actor_location);
  bind(lib, "actor.rotation", &nodes::actor_rotation);
  bind(lib, "actor.forward", &nodes::actor_forward);
  bind(lib, "actor.velocity", &nodes::actor_velocity);
  bind(lib, "actor.distance", &nodes::actor_distance);
  bind(lib, "actor.health", &nodes::actor_health);
  bind(lib, "game.score", &nodes::game_score);

  bind(lib, "actor.set_location", &nodes::actor_set_location);
  bind(lib, "actor.set_rotation", &nodes::actor_set_rotation);
  bind(lib, "actor.move_by", &nodes::actor_move_by);
  bind(lib, "actor.set_velocity", &nodes::actor_set_velocity);
  bind(lib, "actor.impulse", &nodes::actor_impulse);
  bind(lib, "actor.force", &nodes::actor_force);
  bind(lib, "actor.launch", &nodes::actor_launch);
  bind(lib, "actor.simulate", &nodes::actor_simulate);
  bind(lib, "actor.joint_motor", &nodes::actor_joint_motor);
  bind(lib, "actor.explode", &nodes::actor_explode);
  // (its setting names the template; errors go to the host)
  bind(lib, "actor.spawn", [](Context& c) {
    const logic::ActorHandle tpl{c.in_actor("template")};
    const Vec3 location = c.in_vector("location"), rotation = c.in_vector("rotation");
    if (GameWorld* w = world(c)) c.out("spawned", logic::to_value(nodes::actor_spawn(*w, c.self(), c.host(), c.param("template"), tpl, location, rotation)));
    return 0;
  });
  bind(lib, "actor.destroy", &nodes::actor_destroy);
  bind(lib, "actor.enable", &nodes::actor_enable);
  bind(lib, "actor.set_property", [](Context& c) {
    const logic::ActorHandle to{c.in_actor("target")};
    const std::string path = c.in_text("path"), text = c.in_text("value");
    if (GameWorld* w = world(c)) nodes::actor_set_property(*w, c.self(), c.host(), to, path, text);
    return 0;
  });
  bind(lib, "actor.set_light", &nodes::actor_set_light);
  bind(lib, "actor.set_color", &nodes::actor_set_color);
}

void bind_game_nodes(logic::Library& lib) {
  bind(lib, "game.message", &nodes::game_message);
  bind(lib, "game.add_score", &nodes::game_add_score);
  bind(lib, "game.win", &nodes::game_win);
  bind(lib, "game.lose", &nodes::game_lose);
  bind(lib, "game.sound", [](Context& c) {
    const Vec3 location = c.in_vector("location");
    const f64 volume = c.in_number("volume"), pitch = c.in_number("pitch");
    const bool spatial = c.in_bool("spatial");
    if (GameWorld* w = world(c)) nodes::game_sound(*w, c.self(), c.param("sound", "builtin:confirm"), location, volume, pitch, spatial);
    return 0;
  });
  bind(lib, "game.particles", &nodes::game_particles);
  bind(lib, "game.shake", &nodes::game_shake);
  bind(lib, "combat.damage", &nodes::combat_damage);
  bind(lib, "combat.heal", &nodes::combat_heal);
  bind(lib, "combat.kill", &nodes::combat_kill);
  bind(lib, "combat.ammo", &nodes::combat_ammo);
  bind(lib, "physics.trace", &nodes::physics_trace);
  bind(lib, "physics.trace_channels", &nodes::physics_trace_channels);
  bind(lib, "physics.channels", &nodes::physics_channels);
  bind(lib, "flow.for_each_tag", [](Context& c) {
    // Where the loop is, is the node's: the actors as the loop started, and the pass.
    logic::NodeState& s = c.state();
    if (c.entry() != Context::kResume) {
      const std::string tag = c.in_text("tag");
      GameWorld* w = world(c);
      s.list = w ? nodes::actors_with_tag(*w, tag) : std::vector<logic::ActorRef>();
      s.index = 0;
    } else {
      ++s.index;
    }
    if (s.index >= static_cast<i64>(s.list.size()) || !c.loop_back(Context::kResume)) return c.exec("completed");
    c.out("actor", Value::actor(s.list[static_cast<std::size_t>(s.index)]));
    c.out("index", Value::integer(s.index));
    return c.exec("body");
  });
}

/// Robots (walkers, the Micro Duck).
void bind_walker_nodes(logic::Library& lib) {
  bind(lib, "walker.move", &nodes::walker_move);
  bind(lib, "walker.look", &nodes::walker_look);
  bind(lib, "walker.autonomous", &nodes::walker_autonomous);
  bind(lib, "walker.skill", &nodes::walker_skill);
  bind(lib, "walker.beak", &nodes::walker_beak);
  bind(lib, "walker.pet", &nodes::walker_pet);
  bind(lib, "walker.hear", &nodes::walker_hear);
  bind(lib, "walker.holding", &nodes::walker_holding);
  bind(lib, "walker.quack", &nodes::walker_quack);
  bind(lib, "walker.tof", &nodes::walker_tof);
  bind(lib, "walker.sees", &nodes::walker_sees);
  bind(lib, "walker.state", &nodes::walker_state);
}

const bool kRegistered = [] {
  (void)logic_node_shapes_registered();  // (the nodes themselves: names, pins, settings)
  logic::Library& lib = logic::library();
  bind_actor_nodes(lib);
  bind_game_nodes(lib);
  bind_walker_nodes(lib);
  return true;
}();

}  // namespace

/// Referenced from GameWorld so the binding above is always linked.
bool logic_nodes_registered() { return kRegistered; }

}  // namespace kern::game
