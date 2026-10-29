// SpawnCity's weapon set as data (gameplay_final_runtime_v2.mjs, hand_grenade.mjs,
// drone_weapon_modes.mjs, jet_mode.mjs, ballistics.mjs). Milestone 1 plays the hitscan weapons
// through Kern's soldier (WeaponComponent: the pistol is the starting weapon, as in SpawnCity);
// projectiles, melee and the drone / jet weapons use this table in the next milestone.
#pragma once

#include "common.h"

namespace sc::weapons {

enum class Id : u8 { smg, glock, sniper, grenade_launcher, rocket, fists, knife, hand_grenade, drone_gun, drone_rockets, drone_guided, jet_cannon, jet_missile, jet_bomb };

struct Spec {
  const char* name;
  f32 interval_ms;     // between shots / attacks
  f32 range_m;
  f32 damage;          // per hit (explosives: at the centre)
  f32 radius_m;        // explosives: blast radius (0 = hitscan / melee)
  f32 speed_mps;       // projectiles / tracers (0 = instant)
  u8 key;              // desktop key (1..8), 0 = none
};

/// SpawnCity's numbers. Damage of the hitscan guns is SpawnCity's multiplayer damage
/// (VS_WEAPON_DAMAGE smg 13, glock 26, sniper 95) times the zone (head 3, torso 1, legs .65).
constexpr Spec kTable[] = {
    {"MP", 55, 240, 13, 0, 240, 1},
    {"Glock", 110, 120, 26, 0, 0, 2},
    {"Sniper .50", 1150, 1500, 95, 0, 850, 3},
    {"Granatwerfer 40 mm", 760, 300, 125, 8.5f, 40, 5},
    {"Raketenwerfer", 900, 420, 160, 9.5f, 40, 6},
    {"Faeuste", 300, 1.8f, 24, 0, 0, 7},
    {"Messer", 380, 1.6f, 45, 0, 0, 8},
    {"Handgranate", 4000, 40, 125, 8.5f, 0, 0},
    {"Drohnen-MG", 92, 650, 13, 0, 0, 0},
    {"Drohnen-Raketen", 520, 300, 70, 5, 44, 0},
    {"GUIDED", 5000, 600, 34, 4, 34, 0},
    {"Bordkanone", 55, 1800, 30, 0, 0, 0},
    {"Luft-Rakete", 2400, 1300, 120, 8, 260, 0},
    {"Bombe", 450, 0, 170, 17, 0, 0},
};
inline const Spec& spec(Id id) { return kTable[static_cast<u8>(id)]; }

/// Explosion falloff (applyBlast): smoothstep(1 - d / R), buildings in between x 0.18.
inline f32 blast_factor(f32 distance, f32 radius, bool occluded) {
  const f32 t = clampf(1 - distance / radius, 0, 1), s = t * t * (3 - 2 * t);
  return occluded ? s * 0.18f : s;
}

}  // namespace sc::weapons
