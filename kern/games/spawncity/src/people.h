// The town's people: pedestrians on the pavements of the streamed streets and the police
// officers (SpawnCity world_procedural_population.mjs, pedestrian_agents.mjs, police_ground_units.mjs).
//
// People are light agents, not Box3D bodies (SpawnCity's are too): they walk along the pavement
// beside a street centre line, keep to it from street to street, and run away when something
// frightens them. A free move (fleeing, an officer closing in) tests the way with a Box3D ray at
// waist height so nobody walks through a wall. Shots are tested against each person's capsule,
// vehicles knock people down by their footprint and speed.
#pragma once

#include <functional>
#include <vector>

#include "city_stream.h"
#include "common.h"
#include "wanted.h"

namespace sc {

/// A vehicle as the people see it (for being run over and for scattering from low flights).
struct VehicleView {
  Vec3 position, velocity;
  Quat rotation;
  Vec2 half;              // footprint half extents (body X forward, Y left)
  bool player = false;    // driven / flown by the player (crimes count)
  bool airborne = false;  // drone / jet: frightens from above, does not run people over
};

struct PeopleView {
  const kern::physics::World* physics = nullptr;
  const CityStream* city = nullptr;
  Vec3 focus;             // where the player is (spawn ring around it)
  Vec3 player;            // the player's feet (or vehicle)
  f32 player_speed = 0;
  bool player_on_foot = true, player_alive = true;
  std::function<bool(Vec3)> can_spawn;  // spawn_visibility_guard.mjs: may someone appear here unseen?
  std::vector<VehicleView> vehicles;
};

/// What the people saw that the wanted tracker hears about.
struct CrimeReport {
  wanted::Crime crime;
  Vec3 at;
  bool witnessed;
};

class People {
 public:
  // soldier / zombie / enemy: driven by game_military.cpp / game_events.cpp like the officers
  // (military_response.mjs, zombie_nights.mjs, sandbox_game.mjs); `fight`: a citizen who squares
  // up to the player (pedestrian_agents.mjs provoke), moved by game_events.cpp.
  enum class Kind : u8 { citizen, officer, soldier, zombie, enemy };
  enum class State : u8 { walk, wait, flee, down, gone, officer, fight };
  struct Person {
    Kind kind = Kind::citizen;
    State state = State::walk;
    Vec3 pos;
    f32 yaw = 0, speed = 0, v = 0;  // heading (rad), walking pace, current speed
    f32 hp = 100;                   // world_procedural_population.mjs: a person has 100
    f32 phase = 0;                  // gait phase (drawing)
    Vec3 shirt, pants, skin;
    // pavement route
    i32 lane = -1, seg = 0, dir = 1;
    f32 along = 0, side = 1, off = 4;
    // fright
    f32 flee_until = 0, burst_until = 0, flee_speed = 0, hands_up_until = 0, shaken_until = 0, wait_until = 0;
    Vec2 flee_dir;
    f32 fitness = 1;
    f32 down_at = 0;                // death time (bodies stay a while)
    Vec3 fall_dir;                  // which way the body fell
    // officer (driven by Police)
    i32 unit = -1;
    bool aiming = false;
    u32 seed = 0;
    // fighting back (pedestrian_agents.mjs) and the scripted kinds
    f32 provoked_at = -99, fight_until = 0, last_attack = -99;
    bool provoked_hit = false;
    u8 role = 0;                    // enemy type / soldier slot (drawing, rules)
    f32 knocked_until = 0;          // thrown by a car and getting up again (ragdoll.h, M3 life)
  };
  /// A death since the last take_deaths() (points, kill feed).
  struct Death { i32 index; Kind kind; bool by_player; bool head; };

  void set_budget(u32 people) { budget_ = people; }
  void set_ranges(f32 show, f32 hide) { show_ = show; hide_ = hide; }
  void clear() { people_.clear(); }
  /// Walks, flees, spawns and retires people around the focus; knocks people down under vehicles.
  void update(f32 dt, f32 now, const PeopleView& view, Rng& rng);
  /// A bullet from `from` to `to`: the first person on that line takes `damage` (x3 on the head).
  /// Returns the index hit (-1: nobody) and shortens `to` to the hit point.
  /// Passers-by take hits as world_procedural_population.mjs handleProceduralHit: a round does 55
  /// (sniper 200, head 165 / Glock and sniper 999), a hit that is not fatal knocks the person down
  /// (ragdoll, gets up again); someone lying there is no target. `melee_ns` >= 0: a fist or knife
  /// (punch: damage, at hp <= 35 down, else a stagger of .22 x max(.5, ns / 30) m).
  i32 shot(Vec3 from, Vec3& to, f32 damage, bool by_player, f32 now, f32 melee_ns = -1);
  /// An explosion: damage falling off over `radius`, everyone in 2.5 x radius runs.
  void blast(Vec3 at, f32 radius, f32 damage, bool by_player, f32 now);
  /// Everyone within `radius` runs away from `at` (shots, low flights, crashes).
  void frighten(Vec3 at, f32 radius, f32 now, f32 strength = 1);
  /// Crimes since the last call (to the wanted tracker).
  void take_reports(std::vector<CrimeReport>& out) { out.insert(out.end(), reports_.begin(), reports_.end()); reports_.clear(); }

  // officers (Police places and moves them; they share hits and drawing with everybody else)
  i32 add_officer(Vec3 at, f32 yaw, i32 unit, u32 seed);
  /// A citizen put somewhere in plain sight (a driver who gets out of his car); the caller sets
  /// his pavement route (lane, seg, along, side, off) and what he does.
  i32 add_citizen(Vec3 at, f32 yaw, u32 seed);
  Person& at(i32 i) { return people_[static_cast<std::size_t>(i)]; }
  const std::vector<Person>& all() const { return people_; }
  /// A free step along `dir` that does not go through walls (tries turned directions).
  bool free_move(const kern::physics::World& w, Person& p, Vec2 dir, f32 step, bool keep);
  /// Someone sees the place (a living citizen within 45 m): crimes there are witnessed.
  bool witnessed(Vec3 at) const;
  u32 count(Kind k, bool alive_only = true) const;
  /// A scripted person (soldier, zombie, enemy) at `at`; returns its index.
  i32 add_scripted(Kind k, Vec3 at, f32 yaw, f32 hp, u32 seed, Vec3 shirt, Vec3 pants, Vec3 skin, u8 role);
  void take_deaths(std::vector<Death>& out) { out.insert(out.end(), deaths_.begin(), deaths_.end()); deaths_.clear(); }
  /// A fall since the last take_falls() (ragdolls, M3 life): `push` is the car's throw (m/s,
  /// people_impacts.mjs) for `car`, else the direction of the shot or blast; `alive`: knocked down
  /// by a car and getting up again (the person waits until stand_up()).
  struct Fall { i32 index; Vec3 push; bool car; bool alive; };
  void take_falls(std::vector<Fall>& out) { out.insert(out.end(), falls_.begin(), falls_.end()); falls_.clear(); }
  void stand_up(i32 i, Vec3 feet, f32 yaw) { Person& p = at(i); p.pos = feet; p.yaw = yaw; p.knocked_until = 0; }
  /// Whether the last hit of shot() was on the head (headshot_feedback.mjs).
  bool last_head() const { return last_head_; }

 private:
  void spawn(const PeopleView& view, Rng& rng, f32 now);
  void walk(Person& p, f32 dt, f32 now, const PeopleView& view, Rng& rng);
  void kill(Person& p, Vec3 from_dir, bool by_player, f32 now, bool hit_by_car);
  /// Down as a ragdoll that gets up again (world_procedural_population.mjs knockPerson).
  void knock(Person& p, Vec3 push, f32 now) {
    falls_.push_back({static_cast<i32>(&p - people_.data()), push, false, true});
    p.knocked_until = now + 6;
    p.v = 0;
  }
  bool route_point(const Person& p, const std::vector<Lane>& lanes, Vec3& out, f32& heading) const;
  std::vector<Person> people_;
  std::vector<CrimeReport> reports_;
  std::vector<Death> deaths_;
  std::vector<Fall> falls_;
  bool last_head_ = false;
  u32 budget_ = 60, serial_ = 1;
  f32 show_ = 170, hide_ = 190;
};

}  // namespace sc
