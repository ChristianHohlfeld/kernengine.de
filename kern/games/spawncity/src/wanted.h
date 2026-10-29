// Wanted level and police response rules: SpawnCity's sim/wanted_system_logic.mjs (the relaxed
// values of Oct 15/16) as pure functions plus a small heat tracker. The police units that act on
// them (patrol cars with A* over the street lanes, officers, police drones, sirens) are the next
// milestone; this is the interface they will read.
#pragma once

#include "common.h"

namespace sc::wanted {

constexpr i32 kMaxStars = 5;
constexpr f32 kEmpRange = 30, kEmpCooldownMs = 18000, kMaxFireRange = 35;

enum class Crime : u8 { person, person_hit, player, car, bus, police_drone, police_officer, police_car, carjacking, bird };

i32 severity(Crime c);
i32 stars_for_heat(f32 heat);
i32 police_count(i32 stars);
f32 detection_radius(i32 stars);
f32 escape_ms(i32 stars);
f32 police_damage(i32 stars);
f32 spawn_radius(i32 index);
f32 arrival_delay_ms(i32 stars);
f32 engage_delay_ms(i32 stars);
f32 wave_break_ms(i32 stars);
f32 fire_range(i32 stars);
f32 fire_interval_ms(i32 stars);
/// Chance a police shot hits: distance and the target's movement make it harder.
f32 hit_chance(i32 stars, f32 distance_m, f32 player_speed);
/// EMP impulse (N*s) at a distance (0 beyond the range).
f32 emp_impulse(f32 distance_m);

/// Heat from crimes seen by witnesses or police; it cools down once the player has been out of
/// sight for escape_ms(stars).
class Tracker {
 public:
  void report(Crime c, bool witnessed);
  /// `seen`: a police unit or witness sees the player right now.
  void update(f32 dt, bool seen);
  void clear() { heat_ = 0; hidden_ms_ = 0; }
  f32 heat() const { return heat_; }
  i32 stars() const { return stars_for_heat(heat_); }

 private:
  f32 heat_ = 0, hidden_ms_ = 0;
};

}  // namespace sc::wanted
