// The town's people: pedestrians on the pavements of the streamed streets and the police
// officers (SpawnCity world_procedural_population.mjs, pedestrian_agents.mjs, police_ground_units.mjs).
//
// People are light agents, not Box3D bodies (SpawnCity's are too): they walk along the pavement
// beside a street centre line, keep to it from street to street, and run away when something
// frightens them. A free move (fleeing, an officer closing in) tests the way with a Box3D ray at
// waist height so nobody walks through a wall. Shots are tested against each person's capsule,
// vehicles knock people down by their footprint and speed.
#pragma once

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
  enum class Kind : u8 { citizen, officer };
  enum class State : u8 { walk, wait, flee, down, gone, officer };
  struct Person {
    Kind kind = Kind::citizen;
    State state = State::walk;
    Vec3 pos;
    f32 yaw = 0, speed = 0, v = 0;  // heading (rad), walking pace, current speed
    f32 hp = 40;
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
  };

  void set_budget(u32 people) { budget_ = people; }
  void clear() { people_.clear(); }
  /// Walks, flees, spawns and retires people around the focus; knocks people down under vehicles.
  void update(f32 dt, f32 now, const PeopleView& view, Rng& rng);
  /// A bullet from `from` to `to`: the first person on that line takes `damage` (x3 on the head).
  /// Returns the index hit (-1: nobody) and shortens `to` to the hit point.
  i32 shot(Vec3 from, Vec3& to, f32 damage, bool by_player, f32 now);
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

 private:
  void spawn(const PeopleView& view, Rng& rng, f32 now);
  void walk(Person& p, f32 dt, f32 now, const PeopleView& view, Rng& rng);
  void kill(Person& p, Vec3 from_dir, bool by_player, f32 now, bool hit_by_car);
  bool route_point(const Person& p, const std::vector<Lane>& lanes, Vec3& out, f32& heading) const;
  std::vector<Person> people_;
  std::vector<CrimeReport> reports_;
  u32 budget_ = 60, serial_ = 1;
};

}  // namespace sc
