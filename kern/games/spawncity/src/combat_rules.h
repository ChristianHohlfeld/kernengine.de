// Combat and event rules of SpawnCity: the military escalation, flak placement, fighter-jet strike
// timing, the pedestrians' tempers, zombie rounds, the sandbox enemies and chaos score, the
// gameplay contracts (score at risk, banking on escape) and the race track. The numbers are
// SpawnCity's (nostack-dev/spawncity-de, sim/*.mjs, file named at each).
//
// The rules are Blueprints of the level (level/spawncity.klevel, open it in Kern Studio): the
// actors "Military", "Zombies", "Sandbox", "Fun" and "Race" - their variables hold the numbers
// and tables, their functions the laws. Here is what the game keeps between two frames (each
// struct is the Blueprint's struct of the same content), how its C++ asks, and the numbers as the
// game reads them: every k... below is the Blueprint's variable of that name (without the k), read
// when the Blueprint is bound - 0 before. The hashes that give a cell or a person its own random
// number stay here (a Blueprint's whole numbers are exact only up to 2^53).
// game_military.cpp and game_events.cpp put the rules into the world.
#pragma once

#include <string>
#include <vector>

#include "common.h"
#include "kern/logic/logic.h"

namespace sc::combat {

/// The running Blueprints of the actors "Military", "Zombies", "Sandbox", "Fun" and "Race" (the
/// game binds them once its world has started; tests bind the ones they run). False when a table
/// is not as the game needs it.
bool bind_military(kern::logic::Instance* blueprint);
bool bind_zombies(kern::logic::Instance* blueprint);
bool bind_sandbox(kern::logic::Instance* blueprint);
bool bind_fun(kern::logic::Instance* blueprint);
bool bind_race(kern::logic::Instance* blueprint);

// ---------------------------------------------------------------- military_response.mjs
extern f32 kTriggerHeat, kTriggerHoldS, kTriggerExtremeHeat, kAlertS, kReinforceS, kFirstReinforceS;
extern i32 kEndStars, kSquad, kMaxSoldiers;
extern f32 kSoldierHp, kMilHit, kHeliHp, kTankHp;
extern f32 kHeliDmg, kHeliDmgDrone, kSoldierDmg, kSoldierDmgDrone, kShellDmg;
extern f32 kSoldierReactS, kBurstWindowS;
extern i32 kSoldierBursts;

enum class MilEvent : u8 { none, alert, cancelled, start, stop };

/// When the army comes: five stars held for 40 s with heat >= 36 (or heat >= 55 at once), then a
/// 15 s alert in which losing the fifth star calls it off; at three stars or fewer it withdraws.
struct MilitaryClock {
  bool active = false;
  f32 since5 = -1, alert_at = -1, next_reinforce = 0;
  MilEvent update(f32 now, i32 stars, f32 heat, bool enabled = true);
};
f32 heli_hit_chance(f32 d, f32 speed);
f32 soldier_hit_chance(f32 d, f32 speed);
f32 tank_aim_error(f32 d, f32 speed);

// ---------------------------------------------------------------- flak_battery.mjs
extern f32 kFlakCell, kFlakChance, kFlakRange, kFlakMinAgl, kFlakMuzzle, kFlakRps;
extern f32 kFlakOn, kFlakOff, kFlakSpread, kFlakBurstR, kFlakHp;
/// FNV-1a over the text (the cell key), as SpawnCity's hash().
u32 hash_text(const std::string& s);
/// SpawnCity's rand(seed, i): a deterministic number in [0, 1).
f32 cell_rand(u32 seed, u32 i);
/// Whether the 700 m cell (cx, cy) has a gun; `seed` for its candidate spots.
bool flak_cell(i64 cx, i64 cy, u32& seed);
/// Damage of an airburst `d` metres from the aircraft (0 beyond 7 m).
f32 flak_burst_damage(f32 d);

// ---------------------------------------------------------------- fighter_jets.mjs
extern f32 kJetSpeed, kSoundMps, kJetAltMin, kJetAltMax, kJetSpan;
extern f32 kAmbientMinS, kAmbientMaxS, kStrikeCooldownS, kBombRadius, kBombDamage, kStickLen;
extern i32 kStick;
/// When the strike pair has to start (SPAN metres south) so the first bomb falls 3.5 s after the call.
f32 strike_launch_time(f32 now);

// ---------------------------------------------------------------- pedestrian_agents.mjs, gangsters.mjs
enum class Temper : u8 { fearful, runner, aggressive };
enum class Arms : u8 { fists, knife, gun };
/// agentRandom: a fixed number in [0, 1) per person.
f32 agent_random(u32 seed);
Temper temper_of(u32 seed);   // 6 % aggressive, 30 % runner, the rest fearful
Arms arms_of(u32 seed);       // of the aggressive: 3 % gun, 17 % knife, else fists
extern f32 kPunchDmg, kStabDmg, kGangGunDmg;
f32 gangster_hit_chance(f32 d, f32 speed);

// ---------------------------------------------------------------- zombie_nights.mjs
extern i32 kZombieCap;
extern f32 kZombieHit, kZombieMelee, kZombieMeleeS, kZombieSpawnMin, kZombieSpawnMax;
extern i32 kZombieHitPts, kZombieKillPts;
enum class ZombieEvent : u8 { none, night, round, round_done, dawn, spawn };
struct ZombieRounds {
  bool active = false;
  i32 round = 0, to_spawn = 0, points = 0;
  f32 next_spawn = 0, break_until = -1;
  /// One step; `alive` zombies are up. Returns what happened (spawn: the caller places one, and
  /// calls spawned() when it found an unseen spot).
  ZombieEvent update(f32 now, bool night, i32 alive);
  void spawned(f32 now);
  void next_round(f32 now);
  f32 hp() const;
  /// From round 3 a growing share sprints (3..4.1 m/s), the others shamble.
  f32 speed(f32 r_fast, f32 r_speed) const;
};

// ---------------------------------------------------------------- sandbox_game.mjs
enum class EnemyType : u8 { cop, swat, heavy };
struct EnemySpec { const char* name; f32 hp, speed, range, shot_s; i32 burst; f32 dmg, acc; i32 score; f32 scale; Vec3 color, stripe; };
const EnemySpec& enemy_spec(EnemyType t);
extern f32 kEnemySpawnS, kComboS;
/// Which enemy comes at `stars` (r: a random number).
EnemyType choose_enemy(i32 stars, f32 r, i32 heavies);
f32 enemy_hit_chance(const EnemySpec& s, f32 d, f32 moving);
/// How many enemies are up at `stars`.
i32 enemy_budget(i32 stars);
/// SCORE with a COMBO multiplier (events within 3.2 s, up to x8) and the FLOW meter.
struct Chaos {
  i32 score = 0, combo = 1;
  f32 combo_until = -1, flow = 0;
  i32 award(i32 points, f32 now);
  void tick(f32 dt, f32 now);
};

// ---------------------------------------------------------------- gameplay_fun_logic.mjs
extern f32 kFunComboS, kOverdriveS;
extern i32 kMomentumTarget;
struct Contract { const char* id; const char* name; const char* hint; i32 target, reward; };
extern i32 kContracts;  // (rows of the Blueprint's table Contracts)
const Contract& contract(i32 index);
enum class FunEvent : u8 { tick, world_kill_person, world_kill_car, world_kill_bus, world_kill_player, police_kill, emp, escape, damage, drone_destroyed, player_death, cycle_contract };
/// Score at risk: skill awards pile up (multiplied by heat, combo and overdrive) and are banked
/// only when the player escapes; death insures half, a lost drone costs 20 %.
struct Fun {
  i32 score = 0, at_risk = 0, high_score = 0, stars = 0, max_stars = 0, combo = 0, best_combo = 0, momentum = 0;
  f32 combo_expires = -1e9f, overdrive_until = -1e9f;
  i32 contract_index = 0, contract_progress = 0, contracts_completed = 0, mercy = 0, last_award = 0;
  f32 run_damage = 0;
  bool contract_complete = false;
  std::string last_label = "READY";
  /// Applies one event; returns a short German/English label to show ("" for none).
  std::string apply(FunEvent e, f32 now, i32 stars_now, f32 amount = 0);
  f32 multiplier(f32 now) const;
  f32 police_damage_scale() const;
};

// ---------------------------------------------------------------- race_track.mjs
extern i32 kRaceLaps, kGates;
extern f32 kCourseScale;
struct Gate { const char* name; Vec3 center; f32 width, height; bool start; };
const Gate& gate(i32 i);
/// Gate frame: race-forward normal (towards the next gate) and the horizontal opening axis.
void gate_axes(i32 i, Vec2& normal, Vec2& right);
struct Race {
  bool started = false, finished = false;
  i32 lap = 0, next = 0;
  f32 race_start = 0, lap_start = 0, finish_time = -1, last_lap = -1, best_lap = -1, last_pass = -99;
  bool have_prev = false;
  Vec3 prev;
  void reset();
  /// `p` relative to the course origin; returns the gate passed (-1: none).
  i32 update(Vec3 p, f32 t);
  static bool crossed(i32 gate_index, Vec3 a, Vec3 b);
};
std::string race_time(f32 seconds);

}  // namespace sc::combat
