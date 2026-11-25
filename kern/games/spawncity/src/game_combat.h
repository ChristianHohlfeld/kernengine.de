// State of combat and events in the town (game_military.cpp, game_events.cpp): the army at high
// wanted levels, flak guns, fighter jets and air strikes, people who fight back, zombie nights,
// the sandbox enemies and scores, the race track, car debris. One member of Game (`combat_`).
#pragma once

#include <string>
#include <vector>

#include "combat_rules.h"
#include "common.h"

namespace sc {

// (A unit's hit points are a rule of the "Military" Blueprint, read when the game binds it
// (combat::kHeliHp, kTankHp, kFlakHp): whoever makes a unit gives it them - Game::spawn_heli,
// spawn_tank, update_flak.)
struct Heli {
  enum class State : u8 { inbound, orbit, leave, down };
  State state = State::inbound;
  Vec3 p, v;
  f32 yaw = 0, hp = 0, orbit = 0, rotor = 0, next_burst = 0, next_shot = 0;
  i32 burst = 0;
  bool dead = false;
  u16 id = 0;  // in the snapshots to the mates (game_swarm.cpp)
};

struct Tank {
  Vec3 p;
  f32 yaw = 0, turret = 0, hp = 0, speed = 0, next_shell = 0, path_at = -99, recover_until = -1, recover_start = -99, stuck_since = -1,
      progress_t = 0, flipped = 0, wreck_at = -1;
  Vec2 progress_p, exit;
  i32 fails = 0;
  std::vector<Vec2> path;
  bool leaving = false, wreck = false, dead = false, has_exit = false;
  u16 id = 0;
  kern::physics::Body body;  // military_response.mjs upsertBody: the hull as a body cars and people hit
};

struct Shell { Vec3 p, v; f32 life = 3; bool remote = false; };  // remote: a mate's tank (seen, no damage here)

struct SoldierAi {
  i32 person = -1;
  f32 hold = 20, next_shot = 0, los_at = -99, last_shot = -99, path_at = -99;
  i32 burst = 0;
  bool los = false;
  std::vector<Vec2> path;
  u16 id = 0;
};

struct FlakGun {
  i64 cx = 0, cy = 0;
  Vec3 p;
  f32 base_yaw = 0, yaw = 0, pitch = 0.2f, burst_t = 0, next_shot = 0, hp = 0, los_at = -99, flash_at = -99;
  u32 barrel = 0;
  bool los = false, dead = false;
};
struct FlakRound { Vec3 p, v; f32 until = 0; };
struct Puff { Vec3 p; f32 born = 0; };

struct FighterJet { f32 x = 0, y0 = 0, z = 0, t0 = 0; bool strike = false; Vec2 target; i32 dropped = 0; bool shook = false, heard = false, remote = false; };  // remote: a mate's (fighter_jets.mjs onFx)
struct Bomb { Vec3 from, to; f32 t0 = 0, dur = 1.5f; bool remote = false; };
struct Blast { Vec3 p; f32 born = 0, scale = 1; };
struct Flare { Vec3 p; f32 born = 0, until = 0; };

struct Tracer { Vec3 a, b; f32 until = 0; Vec3 color{3, 2.4f, 1.2f}; };

struct ZombieAi { i32 person = -1; f32 next_hit = 0, swing = -99, speed = 1, last_hp = 0; u16 id = 0; };

struct EnemyAi {
  i32 person = -1;
  combat::EnemyType type = combat::EnemyType::cop;
  f32 next_shot = 0, hits = 0, strafe = 1, last_hp = 0;
  i32 burst_left = 0;
  bool retreat = false;
  bool hold = false;  // tests: a dummy that stands and never shoots
  u16 id = 0;         // in the snapshots to the mates (game_swarm.cpp)
};
struct EnemyRocket { Vec3 p, v; f32 life = 0; };

struct Debris { Vec3 p, v, spin, size, color; Quat q; f32 born = 0, life = 6.5f; };

struct CombatState {
  // military_response.mjs
  combat::MilitaryClock military;
  i32 pending_heli = 0, pending_squad = 0, pending_tank = 0;
  f32 pending_at = 0;
  std::vector<Heli> helis;
  std::vector<Tank> tanks;
  std::vector<Shell> shells;
  std::vector<SoldierAi> soldiers;
  std::vector<f32> burst_starts;
  bool military_enabled = true;
  // flak_battery.mjs
  std::vector<FlakGun> flak;
  std::vector<FlakRound> flak_rounds;
  std::vector<Puff> puffs;
  f32 flak_scan = -99;
  Vec3 target_last;
  Vec3 target_vel;
  bool target_had = false;
  u32 flak_hits = 0, flak_destroyed = 0;
  // fighter_jets.mjs
  std::vector<FighterJet> jets;
  std::vector<Bomb> bombs;
  std::vector<Blast> blasts;
  std::vector<Flare> flares;
  f32 next_ambient = 50, last_strike = -1e9f;
  // effects
  std::vector<Tracer> tracers;
  std::vector<Debris> debris;
  std::vector<u8> car_wrecked;  // per traffic car: wreck already blown apart
  bool pd_was[5] = {}, pd_emp[5] = {};
  bool was_dead = false;
  // people who fight back (pedestrian_agents.mjs, gangsters.mjs)
  u32 gangster_shots = 0, stabs = 0, punches = 0;
  // zombie_nights.mjs
  combat::ZombieRounds zombies;
  std::vector<ZombieAi> zombie_ai;
  bool zombies_enabled = true, zombie_force = false;
  f32 last_groan = -99;
  // sandbox_game.mjs
  std::vector<EnemyAi> enemies;
  std::vector<EnemyRocket> rockets;
  combat::Chaos chaos;
  f32 last_spawn = -99, no_wanted_since = -1;
  i32 last_stars = 0;
  // gameplay_fun_logic.mjs
  combat::Fun fun;
  f32 last_health = -1;
  // race_track.mjs
  combat::Race race;
  bool race_on = false;
  Vec3 race_origin;
  // banner (military / zombies / race) and pop texts
  std::string banner, pop;
  f32 banner_at = -99, pop_at = -99;
  bool scope = false;
  f32 drone_ready_at = 0;  // player_vitals_logic.mjs: 8 s until a new drone
};

}  // namespace sc
