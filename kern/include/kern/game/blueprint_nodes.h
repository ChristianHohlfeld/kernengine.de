// Kern Logic: the gameplay nodes as typed C++ functions (kern::game::nodes) and as the functions
// a Blueprint's class calls (kern::game::BlueprintNodesOf, a part of kern::Blueprint).
//
// A node has one meaning: the node library (src/game/logic_nodes.cpp) wraps these functions as
// nodes for the interpreter, a Blueprint's C++ (docs/blueprint_cpp.md) calls them directly. They
// take the game, the actor the Blueprint belongs to (Self: what an Actor input means when it is
// left empty) and the node's inputs in their order; a node with several outputs gives a struct
// whose fields are named like its output pins.
//
// Included by kern/logic/blueprint.h. The events of the game are handlers like any other event
// (the runtime decides who gets which).
#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "kern/logic/nodes.h"

namespace kern::game {

class GameWorld;

namespace nodes {

using Bool = bool;
using Int = i64;
using Float = f64;
using Vector = Vec3;
using Text = std::string;
using Actor = logic::ActorHandle;
using Self = logic::ActorRef;

struct Found { Actor actor; Int count = 0; };
struct Velocity { Vector velocity{}; Float speed = 0; };
struct Health { Float health = 0; Bool dead = false; };
struct Score { Float score = 0; Int pickups = 0; };
struct Trace { Bool hit = false; Actor actor; Vector location{}; Vector normal{}; };
struct Tof { Bool blocked = false; Float distance = 0; Float bearing = 0; Float left = 0; Float right = 0; };
struct Sees { Bool visible = false; Float bearing = 0; Float distance = 0; };
struct Robot { Text state; Bool fallen = false; Float speed = 0; };

// ---- actors: values ----
Actor actor_player(GameWorld& w, Self self, Int index);
Int actor_player_count(GameWorld& w, Self self);
Actor actor_find(GameWorld& w, Self self, const Text& name);
Found actor_find_tag(GameWorld& w, Self self, const Text& tag);
/// Every living actor with the tag, as a list.
std::vector<Actor> actor_all_with_tag(GameWorld& w, Self self, const Text& tag);
/// One living actor with the tag, at random: the node's own sequence (logic::random; `memory`
/// is the node's, `seed` the Blueprint's).
Actor actor_random_tag(GameWorld& w, Self self, f64* memory, u32 seed, u32 node, const Text& tag);
Bool actor_has_tag(GameWorld& w, Self self, Actor target, const Text& tag);
Text actor_name(GameWorld& w, Self self, Actor target);
Bool actor_valid(GameWorld& w, Self self, Actor target);
Vector actor_location(GameWorld& w, Self self, Actor target);
Vector actor_rotation(GameWorld& w, Self self, Actor target);
Vector actor_forward(GameWorld& w, Self self, Actor target);
Velocity actor_velocity(GameWorld& w, Self self, Actor target);
Float actor_distance(GameWorld& w, Self self, Actor a, Actor b);
Health actor_health(GameWorld& w, Self self, Actor target);
Score game_score(GameWorld& w, Self self);

// ---- actors: steps ----
void actor_set_location(GameWorld& w, Self self, Actor target, Vector location);
void actor_set_rotation(GameWorld& w, Self self, Actor target, Vector rotation);
void actor_move_by(GameWorld& w, Self self, Actor target, Vector offset);
void actor_set_velocity(GameWorld& w, Self self, Actor target, Vector velocity);
void actor_impulse(GameWorld& w, Self self, Actor target, Vector impulse);
void actor_force(GameWorld& w, Self self, Actor target, Vector force);
void actor_launch(GameWorld& w, Self self, Actor target, Vector velocity);
void actor_simulate(GameWorld& w, Self self, Actor target, Bool simulate);
void actor_joint_motor(GameWorld& w, Self self, Actor target, Int joint, Bool enable, Float speed, Float force);
void actor_explode(GameWorld& w, Self self, Vector location, Float radius, Float impulse, Float damage);
/// `named`: the node's setting (a template's name), used when no Template is given.
Actor actor_spawn(GameWorld& w, Self self, logic::Host& host, std::string_view named, Actor tpl, Vector location, Vector rotation);
void actor_destroy(GameWorld& w, Self self, Actor target);
void actor_enable(GameWorld& w, Self self, Actor target, Bool enabled);
void actor_set_property(GameWorld& w, Self self, logic::Host& host, Actor target, const Text& path, const Text& value);
void actor_set_light(GameWorld& w, Self self, Actor target, Vector color, Float intensity, Float range);
void actor_set_color(GameWorld& w, Self self, Actor target, Vector color);

// ---- the game ----
void game_message(GameWorld& w, Self self, const Text& text);
void game_add_score(GameWorld& w, Self self, Float points);
void game_win(GameWorld& w, Self self, const Text& message);
void game_lose(GameWorld& w, Self self, const Text& message);
void game_sound(GameWorld& w, Self self, std::string_view sound, Vector location, Float volume, Float pitch, Bool spatial);
void game_particles(GameWorld& w, Self self, Vector location, Vector color);
void game_shake(GameWorld& w, Self self, Float amount);
void combat_damage(GameWorld& w, Self self, Actor target, Float amount, Actor instigator);
void combat_heal(GameWorld& w, Self self, Actor target, Float amount);
void combat_kill(GameWorld& w, Self self, Actor target);
void combat_ammo(GameWorld& w, Self self, Actor target, Int rounds);
Trace physics_trace(GameWorld& w, Self self, Vector start, Vector end, Actor ignore);
Trace physics_trace_channels(GameWorld& w, Self self, Vector start, Vector end, Actor ignore, Int mask, Int category);
Int physics_channels(GameWorld& w, Self self, const Text& names);
/// For Each Actor with Tag: the living actors with the tag, as the loop starts.
std::vector<logic::ActorRef> actors_with_tag(GameWorld& w, const Text& tag);

// ---- robots (walkers, the Micro Duck): the robot's own units (m/s, degrees); Robot 0 in the
// level's Blueprint is the first robot ----
void walker_move(GameWorld& w, Self self, Actor target, Float forward, Float strafe, Float turn, Float duration);
void walker_look(GameWorld& w, Self self, Actor target, Float yaw, Float pitch, Float duration);
void walker_autonomous(GameWorld& w, Self self, Actor target, Bool enable);
void walker_skill(GameWorld& w, Self self, Actor target, const Text& move);
void walker_beak(GameWorld& w, Self self, Actor target, Float open);
void walker_pet(GameWorld& w, Self self, Actor target, Float seconds);
void walker_hear(GameWorld& w, Self self, Actor target, Bool voice);
Bool walker_holding(GameWorld& w, Self self, Actor target);
void walker_quack(GameWorld& w, Self self, Actor target);
Tof walker_tof(GameWorld& w, Self self, Actor target);
Sees walker_sees(GameWorld& w, Self self, Actor target, Actor other);
Robot walker_state(GameWorld& w, Self self, Actor target);

}  // namespace nodes

/// The gameplay nodes of a Blueprint's class: each calls the node's function with the game the
/// class runs in (kern_world) and its own actor. A class without a game does nothing there and
/// gets nothing (the values' zeros).
template <class Core> struct BlueprintNodesOf : Core {
  using Bool = bool;
  using Int = logic::Whole;
  using Float = f64;
  using Vector = Vec3;
  using Text = std::string;
  using Actor = logic::ActorHandle;
  template <class T> using List = std::vector<T>;

  GameWorld* kern_game() const { return this->kern_world.host ? static_cast<GameWorld*>(this->kern_world.host->game()) : nullptr; }
  logic::ActorRef kern_self() const { return this->kern_world.self; }

  // ---- actors: values ----
  Actor actor_self() const { return kern_game() ? Actor{kern_self()} : Actor{}; }
  Actor actor_player(Int index) const { GameWorld* g = kern_game(); return g ? nodes::actor_player(*g, kern_self(), index) : Actor{}; }
  Int actor_player_count() const { GameWorld* g = kern_game(); return g ? nodes::actor_player_count(*g, kern_self()) : 0; }
  Actor actor_find(const Text& name) const { GameWorld* g = kern_game(); return g ? nodes::actor_find(*g, kern_self(), name) : Actor{}; }
  nodes::Found actor_find_tag(const Text& tag) const { GameWorld* g = kern_game(); return g ? nodes::actor_find_tag(*g, kern_self(), tag) : nodes::Found{}; }
  List<Actor> actor_all_with_tag(const Text& tag) const { GameWorld* g = kern_game(); return g ? nodes::actor_all_with_tag(*g, kern_self(), tag) : List<Actor>(); }
  template <class N> Actor actor_random_tag(N& n, const Text& tag) {
    GameWorld* g = kern_game();
    return g ? nodes::actor_random_tag(*g, kern_self(), n.memory, this->kern_seed(), N::id, tag) : Actor{};
  }
  Bool actor_has_tag(Actor target, const Text& tag) const { GameWorld* g = kern_game(); return g && nodes::actor_has_tag(*g, kern_self(), target, tag); }
  Text actor_name(Actor target) const { GameWorld* g = kern_game(); return g ? nodes::actor_name(*g, kern_self(), target) : Text(); }
  Bool actor_valid(Actor target) const { GameWorld* g = kern_game(); return g && nodes::actor_valid(*g, kern_self(), target); }
  Vector actor_location(Actor target) const { GameWorld* g = kern_game(); return g ? nodes::actor_location(*g, kern_self(), target) : Vector{}; }
  Vector actor_rotation(Actor target) const { GameWorld* g = kern_game(); return g ? nodes::actor_rotation(*g, kern_self(), target) : Vector{}; }
  Vector actor_forward(Actor target) const { GameWorld* g = kern_game(); return g ? nodes::actor_forward(*g, kern_self(), target) : Vector{}; }
  nodes::Velocity actor_velocity(Actor target) const { GameWorld* g = kern_game(); return g ? nodes::actor_velocity(*g, kern_self(), target) : nodes::Velocity{}; }
  Float actor_distance(Actor a, Actor b) const { GameWorld* g = kern_game(); return g ? nodes::actor_distance(*g, kern_self(), a, b) : 0.0; }
  nodes::Health actor_health(Actor target) const { GameWorld* g = kern_game(); return g ? nodes::actor_health(*g, kern_self(), target) : nodes::Health{}; }
  nodes::Score game_score() const { GameWorld* g = kern_game(); return g ? nodes::game_score(*g, kern_self()) : nodes::Score{}; }

  // ---- actors: steps ----
  void actor_set_location(Actor target, Vector location) { if (GameWorld* g = kern_game()) nodes::actor_set_location(*g, kern_self(), target, location); }
  void actor_set_rotation(Actor target, Vector rotation) { if (GameWorld* g = kern_game()) nodes::actor_set_rotation(*g, kern_self(), target, rotation); }
  void actor_move_by(Actor target, Vector offset) { if (GameWorld* g = kern_game()) nodes::actor_move_by(*g, kern_self(), target, offset); }
  void actor_set_velocity(Actor target, Vector velocity) { if (GameWorld* g = kern_game()) nodes::actor_set_velocity(*g, kern_self(), target, velocity); }
  void actor_impulse(Actor target, Vector impulse) { if (GameWorld* g = kern_game()) nodes::actor_impulse(*g, kern_self(), target, impulse); }
  void actor_force(Actor target, Vector force) { if (GameWorld* g = kern_game()) nodes::actor_force(*g, kern_self(), target, force); }
  void actor_launch(Actor target, Vector velocity) { if (GameWorld* g = kern_game()) nodes::actor_launch(*g, kern_self(), target, velocity); }
  void actor_simulate(Actor target, Bool simulate) { if (GameWorld* g = kern_game()) nodes::actor_simulate(*g, kern_self(), target, simulate); }
  void actor_joint_motor(Actor target, Int joint, Bool enable, Float speed, Float force) {
    if (GameWorld* g = kern_game()) nodes::actor_joint_motor(*g, kern_self(), target, joint, enable, speed, force);
  }
  void actor_explode(Vector location, Float radius, Float impulse, Float damage) { if (GameWorld* g = kern_game()) nodes::actor_explode(*g, kern_self(), location, radius, impulse, damage); }
  /// (with n: the node's member, when what it gives is read - n.spawned)
  template <Setting Template> void actor_spawn(Actor tpl, Vector location, Vector rotation) {
    if (GameWorld* g = kern_game()) nodes::actor_spawn(*g, kern_self(), *this->kern_world.host, Template.view(), tpl, location, rotation);
  }
  template <Setting Template, class N> void actor_spawn(N& n, Actor tpl, Vector location, Vector rotation) {
    if (GameWorld* g = kern_game()) n.spawned = nodes::actor_spawn(*g, kern_self(), *this->kern_world.host, Template.view(), tpl, location, rotation);
  }
  void actor_destroy(Actor target) { if (GameWorld* g = kern_game()) nodes::actor_destroy(*g, kern_self(), target); }
  void actor_enable(Actor target, Bool enabled) { if (GameWorld* g = kern_game()) nodes::actor_enable(*g, kern_self(), target, enabled); }
  void actor_set_property(Actor target, const Text& path, const Text& value) {
    if (GameWorld* g = kern_game()) nodes::actor_set_property(*g, kern_self(), *this->kern_world.host, target, path, value);
  }
  void actor_set_light(Actor target, Vector color, Float intensity, Float range) { if (GameWorld* g = kern_game()) nodes::actor_set_light(*g, kern_self(), target, color, intensity, range); }
  void actor_set_color(Actor target, Vector color) { if (GameWorld* g = kern_game()) nodes::actor_set_color(*g, kern_self(), target, color); }

  // ---- the game ----
  void game_message(const Text& text) { if (GameWorld* g = kern_game()) nodes::game_message(*g, kern_self(), text); }
  void game_add_score(Float points) { if (GameWorld* g = kern_game()) nodes::game_add_score(*g, kern_self(), points); }
  void game_win(const Text& message) { if (GameWorld* g = kern_game()) nodes::game_win(*g, kern_self(), message); }
  void game_lose(const Text& message) { if (GameWorld* g = kern_game()) nodes::game_lose(*g, kern_self(), message); }
  template <Setting Sound> void game_sound(Vector location, Float volume, Float pitch, Bool spatial) {
    if (GameWorld* g = kern_game()) nodes::game_sound(*g, kern_self(), Sound.view(), location, volume, pitch, spatial);
  }
  void game_particles(Vector location, Vector color) { if (GameWorld* g = kern_game()) nodes::game_particles(*g, kern_self(), location, color); }
  void game_shake(Float amount) { if (GameWorld* g = kern_game()) nodes::game_shake(*g, kern_self(), amount); }
  void combat_damage(Actor target, Float amount, Actor instigator) { if (GameWorld* g = kern_game()) nodes::combat_damage(*g, kern_self(), target, amount, instigator); }
  void combat_heal(Actor target, Float amount) { if (GameWorld* g = kern_game()) nodes::combat_heal(*g, kern_self(), target, amount); }
  void combat_kill(Actor target) { if (GameWorld* g = kern_game()) nodes::combat_kill(*g, kern_self(), target); }
  void combat_ammo(Actor target, Int rounds) { if (GameWorld* g = kern_game()) nodes::combat_ammo(*g, kern_self(), target, rounds); }
  void physics_trace(Vector start, Vector end, Actor ignore) { if (GameWorld* g = kern_game()) nodes::physics_trace(*g, kern_self(), start, end, ignore); }
  void physics_trace_channels(Vector start, Vector end, Actor ignore, Int mask, Int category) {
    if (GameWorld* g = kern_game()) nodes::physics_trace_channels(*g, kern_self(), start, end, ignore, mask, category);
  }
  template <class N> void physics_trace_channels(N& n, Vector start, Vector end, Actor ignore, Int mask, Int category) {
    GameWorld* g = kern_game();
    if (!g) return;
    const nodes::Trace t = nodes::physics_trace_channels(*g, kern_self(), start, end, ignore, mask, category);
    n.hit = t.hit;
    n.actor = t.actor;
    n.location = t.location;
    n.normal = t.normal;
  }
  Int physics_channels(const Text& names) const { GameWorld* g = kern_game(); return g ? nodes::physics_channels(*g, kern_self(), names) : 0; }
  template <class N> void physics_trace(N& n, Vector start, Vector end, Actor ignore) {
    GameWorld* g = kern_game();
    if (!g) return;
    const nodes::Trace t = nodes::physics_trace(*g, kern_self(), start, end, ignore);
    n.hit = t.hit;
    n.actor = t.actor;
    n.location = t.location;
    n.normal = t.normal;
  }

  // ---- robots ----
  void walker_move(Actor target, Float forward, Float strafe, Float turn, Float duration) {
    if (GameWorld* g = kern_game()) nodes::walker_move(*g, kern_self(), target, forward, strafe, turn, duration);
  }
  void walker_look(Actor target, Float yaw, Float pitch, Float duration) { if (GameWorld* g = kern_game()) nodes::walker_look(*g, kern_self(), target, yaw, pitch, duration); }
  void walker_autonomous(Actor target, Bool enable) { if (GameWorld* g = kern_game()) nodes::walker_autonomous(*g, kern_self(), target, enable); }
  void walker_skill(Actor target, const Text& move) { if (GameWorld* g = kern_game()) nodes::walker_skill(*g, kern_self(), target, move); }
  void walker_beak(Actor target, Float open) { if (GameWorld* g = kern_game()) nodes::walker_beak(*g, kern_self(), target, open); }
  void walker_pet(Actor target, Float seconds) { if (GameWorld* g = kern_game()) nodes::walker_pet(*g, kern_self(), target, seconds); }
  void walker_hear(Actor target, Bool voice) { if (GameWorld* g = kern_game()) nodes::walker_hear(*g, kern_self(), target, voice); }
  Bool walker_holding(Actor target) const { GameWorld* g = kern_game(); return g && nodes::walker_holding(*g, kern_self(), target); }
  void walker_quack(Actor target) { if (GameWorld* g = kern_game()) nodes::walker_quack(*g, kern_self(), target); }
  nodes::Tof walker_tof(Actor target) const { GameWorld* g = kern_game(); return g ? nodes::walker_tof(*g, kern_self(), target) : nodes::Tof{}; }
  nodes::Sees walker_sees(Actor target, Actor other) const { GameWorld* g = kern_game(); return g ? nodes::walker_sees(*g, kern_self(), target, other) : nodes::Sees{}; }
  nodes::Robot walker_state(Actor target) const { GameWorld* g = kern_game(); return g ? nodes::walker_state(*g, kern_self(), target) : nodes::Robot{}; }

  /// for (auto it = flow_for_each_tag(n, tag); it.next();): every living actor with the tag, as the
  /// loop starts; n.actor and n.index are the pass's. Where the loop is, is the node's (n.list,
  /// n.memory[0]) - like the index of a For Loop.
  template <class N> struct ActorLoop {
    BlueprintNodesOf& self;
    N& n;
    bool first = true;
    bool next() {
      if (!first) n.memory[0] += 1;
      first = false;
      const std::size_t at = static_cast<std::size_t>(n.memory[0]);
      if (at >= n.list.size() || !self.kern_pass()) return false;
      n.actor = {n.list[at]};
      n.index = static_cast<i64>(at);
      return true;
    }
  };
  template <class N> ActorLoop<N> flow_for_each_tag(N& n, const Text& tag) {
    GameWorld* g = kern_game();
    n.list = g ? nodes::actors_with_tag(*g, tag) : std::vector<logic::ActorRef>();
    n.memory[0] = 0;
    return {*this, n};
  }
};

}  // namespace kern::game
