// Combat and event rules of SpawnCity as pure logic (no GameWorld): the military escalation,
// flak placement, fighter-jet strike timing, the pedestrians' tempers, zombie rounds, the sandbox
// enemies and chaos score, the gameplay contracts (score at risk, banking on escape) and the race
// track. The numbers are SpawnCity's (nostack-dev/spawncity-de, sim/*.mjs, file named at each).
// game_military.cpp and game_events.cpp put them into the world.
#pragma once

#include <string>
#include <vector>

#include "common.h"

namespace sc::combat {

// ---------------------------------------------------------------- military_response.mjs
constexpr f32 kTriggerHeat = 36, kTriggerHoldS = 40, kTriggerExtremeHeat = 55, kAlertS = 15, kReinforceS = 45, kFirstReinforceS = 50;
constexpr i32 kEndStars = 3, kSquad = 4, kMaxSoldiers = 8;
constexpr f32 kSoldierHp = 100, kMilHit = 25, kHeliHp = 420, kTankHp = 900;
constexpr f32 kHeliDmg = 4, kHeliDmgDrone = 6, kSoldierDmg = 3, kSoldierDmgDrone = 4, kShellDmg = 30;
constexpr f32 kSoldierReactS = 0.9f, kBurstWindowS = 1.4f;
constexpr i32 kSoldierBursts = 3;

enum class MilEvent : u8 { none, alert, cancelled, start, stop };

/// When the army comes: five stars held for 40 s with heat >= 36 (or heat >= 55 at once), then a
/// 15 s alert in which losing the fifth star calls it off; at three stars or fewer it withdraws.
struct MilitaryClock {
  bool active = false;
  f32 since5 = -1, alert_at = -1, next_reinforce = 0;
  MilEvent update(f32 now, i32 stars, f32 heat, bool enabled = true);
};
inline f32 heli_hit_chance(f32 d, f32 speed) { return clampf(0.32f - d / 560 - speed * 0.025f, 0.05f, 0.32f); }
inline f32 soldier_hit_chance(f32 d, f32 speed) { return clampf(0.24f - d / 270 - speed * 0.03f, 0.04f, 0.24f); }
inline f32 tank_aim_error(f32 d, f32 speed) { return 2 + d * 0.03f + speed * 0.6f; }

// ---------------------------------------------------------------- flak_battery.mjs
constexpr f32 kFlakCell = 700, kFlakChance = 0.32f, kFlakRange = 1100, kFlakMinAgl = 18, kFlakMuzzle = 900, kFlakRps = 30;
constexpr f32 kFlakOn = 2.6f, kFlakOff = 1.7f, kFlakSpread = 0.012f, kFlakBurstR = 7, kFlakHp = 60;
/// FNV-1a over the text (the cell key), as SpawnCity's hash().
u32 hash_text(const std::string& s);
/// SpawnCity's rand(seed, i): a deterministic number in [0, 1).
f32 cell_rand(u32 seed, u32 i);
/// Whether the 700 m cell (cx, cy) has a gun; `seed` for its candidate spots.
bool flak_cell(i64 cx, i64 cy, u32& seed);
/// Damage of an airburst `d` metres from the aircraft (0 beyond 7 m).
f32 flak_burst_damage(f32 d);

// ---------------------------------------------------------------- fighter_jets.mjs
constexpr f32 kJetSpeed = 255, kSoundMps = 343, kJetAltMin = 55, kJetAltMax = 85, kJetSpan = 2600;
constexpr f32 kAmbientMinS = 75, kAmbientMaxS = 150, kStrikeCooldownS = 30, kBombRadius = 17, kBombDamage = 170, kStickLen = 130;
constexpr i32 kStick = 10;
/// When the strike pair has to start (SPAN metres south) so the first bomb falls 3.5 s after the call.
inline f32 strike_launch_time(f32 now) { return now + 3.5f - kJetSpan / kJetSpeed + (380 + kStickLen / 2) / kJetSpeed; }

// ---------------------------------------------------------------- pedestrian_agents.mjs, gangsters.mjs
enum class Temper : u8 { fearful, runner, aggressive };
enum class Arms : u8 { fists, knife, gun };
/// agentRandom: a fixed number in [0, 1) per person.
f32 agent_random(u32 seed);
Temper temper_of(u32 seed);   // 6 % aggressive, 30 % runner, the rest fearful
Arms arms_of(u32 seed);       // of the aggressive: 3 % gun, 17 % knife, else fists
constexpr f32 kPunchDmg = 8, kStabDmg = 16, kGangGunDmg = 10;
inline f32 gangster_hit_chance(f32 d, f32 speed) { return clampf(0.52f - d * 0.014f - speed * 0.045f, 0.08f, 0.48f); }

// ---------------------------------------------------------------- zombie_nights.mjs
constexpr i32 kZombieCap = 28;
constexpr f32 kZombieHit = 34, kZombieMelee = 11, kZombieMeleeS = 1.15f, kZombieSpawnMin = 34, kZombieSpawnMax = 70;
constexpr i32 kZombieHitPts = 10, kZombieKillPts = 60;
enum class ZombieEvent : u8 { none, night, round, round_done, dawn, spawn };
struct ZombieRounds {
  bool active = false;
  i32 round = 0, to_spawn = 0, points = 0;
  f32 next_spawn = 0, break_until = -1;
  /// One step; `alive` zombies are up. Returns what happened (spawn: the caller places one, and
  /// calls spawned() when it found an unseen spot).
  ZombieEvent update(f32 now, bool night, i32 alive);
  void spawned(f32 now) { --to_spawn; next_spawn = now + std::fmax(0.35f, 1.1f - static_cast<f32>(round) * 0.06f); }
  void next_round(f32 now) { ++round; to_spawn = 6 + round * 4; next_spawn = now + 1.8f; break_until = -1; }
  f32 hp() const { return 100 + static_cast<f32>(round) * 12; }
  /// From round 3 a growing share sprints (3..4.1 m/s), the others shamble.
  f32 speed(f32 r_fast, f32 r_speed) const;
};

// ---------------------------------------------------------------- sandbox_game.mjs
enum class EnemyType : u8 { cop, swat, heavy };
struct EnemySpec { const char* name; f32 hp, speed, range, shot_s; i32 burst; f32 dmg, acc; i32 score; f32 scale; Vec3 color, stripe; };
const EnemySpec& enemy_spec(EnemyType t);
constexpr i32 kEnemyBudget[6] = {0, 1, 2, 4, 6, 8};
constexpr i32 kMaxEnemies = 8;
constexpr f32 kEnemySpawnS = 6, kComboS = 3.2f;
/// Which enemy comes at `stars` (r: a random number).
bool choose_enemy(i32 stars, f32 r, i32 heavies, EnemyType& out);
inline f32 enemy_hit_chance(const EnemySpec& s, f32 d, f32 moving) { return clampf(s.acc - d / 90 - moving * 0.02f, 0.08f, 0.85f); }
/// SCORE with a COMBO multiplier (events within 3.2 s, up to x8) and the FLOW meter.
struct Chaos {
  i32 score = 0, combo = 1;
  f32 combo_until = -1, flow = 0;
  i32 award(i32 points, f32 now);
  void tick(f32 dt, f32 now);
};

// ---------------------------------------------------------------- gameplay_fun_logic.mjs
constexpr f32 kFunComboS = 5.5f, kOverdriveS = 8;
constexpr i32 kMomentumTarget = 4;
struct Contract { const char* id; const char* name; const char* hint; i32 target, reward; };
constexpr i32 kContracts = 5;
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
  f32 police_damage_scale() const { return mercy <= 0 ? 1.0f : mercy == 1 ? 0.88f : 0.76f; }

 private:
  void clock(f32 now, i32 st);
  std::string skill(FunEvent e, f32 now, f32 base, const char* label, i32 momentum_add, i32 affected = 0);
  void progress(FunEvent e, i32 affected, std::string& out);
};

// ---------------------------------------------------------------- race_track.mjs
constexpr i32 kRaceLaps = 3, kGates = 8;
constexpr f32 kCourseScale = 2.25f;
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
