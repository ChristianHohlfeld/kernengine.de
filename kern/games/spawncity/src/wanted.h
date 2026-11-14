// Wanted level and police response - SpawnCity's sim/wanted_system_logic.mjs (the relaxed values
// of Oct 15/16).
//
// The rules themselves are a Blueprint: the level's actor "Wanted" (level/spawncity.klevel, open
// it in Kern Studio). Its functions hold every formula and number, its variables the heat. This
// header is only how the game's C++ calls them: nothing is computed here.
#pragma once

#include "common.h"
#include "kern/logic/logic.h"

namespace sc::wanted {

/// The running Blueprint of the actor "Wanted" (the game binds it once its world has started;
/// tests bind the one they run). Unbound, every rule answers 0.
void bind(kern::logic::Instance* blueprint);
kern::logic::Instance* blueprint();

/// The Blueprint's crime numbers (its function Severity).
enum class Crime : u8 { person, person_hit, player, car, bus, police_drone, police_officer, police_car, carjacking, bird };

// The Blueprint's variables MaxStars, EmpRange, EmpCooldownMs and MaxFireRange.
i32 max_stars();
f32 emp_range();
f32 emp_cooldown_ms();
f32 max_fire_range();

// The Blueprint's functions of the same names (Severity, StarsForHeat, PoliceCount ...).
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

/// The heat - the Blueprint's variables Heat and HiddenMs and its functions Report, Update, Clear
/// and Stars. Crimes seen by witnesses or police add heat; it is gone once the player has been
/// out of sight for escape_ms(stars). (The Blueprint holds it: the game keeps nothing of it.)
void report(Crime c, bool witnessed);
/// `seen`: a police unit or witness sees the player right now.
void update(f32 dt, bool seen);
void clear();
/// How long the police have not seen the player (the escape clock, wanted_police_drones.mjs searching).
f32 hidden_ms();
f32 heat();
i32 stars();

}  // namespace sc::wanted
