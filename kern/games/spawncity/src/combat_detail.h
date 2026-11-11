// Small combat rules of SpawnCity, used by game_details.cpp:
//  * critical_damage_logic.mjs: when a car, bus, police drone or drone turns critical, how long
//    until it blows, and how a further hit brings that forward,
//  * ballistics.mjs: what a bullet hands to the body it hits (p = m * v along its path),
//  * rules_vote_engine.mjs: the unanimous ballot over a game rule (the network carries the votes).
//
// The rules themselves are a Blueprint: the level's actor "CombatDetail" (level/spawncity.klevel,
// open it in Kern Studio). Its enums, tables and functions hold every number and formula. This
// header is only how the game's C++ calls them: nothing is computed here.
#pragma once
#include <string>
#include <vector>

#include "common.h"
#include "kern/logic/logic.h"

namespace sc::detail {

/// The running Blueprint of the actor "CombatDetail" (the game binds it once its world has
/// started; tests bind the one they run). Unbound, every rule answers 0 / false.
void bind(kern::logic::Instance* blueprint);
kern::logic::Instance* blueprint();

// ---------------------------------------------------------------- critical_damage_logic.mjs
/// The Blueprint's enum Critical (the same order).
enum class Critical : u8 { car, bus, police_drone, drone };
// The Blueprint's functions IsCritical, DetonationAt and Accelerate.
bool is_critical(Critical k, f32 hp, f32 max_hp);
f32 detonation_at(Critical k, f32 now);
f32 accelerate(Critical k, f32 current_at, f32 now);

/// One thing that can turn critical: call update() with its hp every frame; it returns true
/// once, when the countdown has run out. (What the clock holds is here; what it does with it is
/// the Blueprint's function ClockUpdate.)
struct CriticalClock {
  f32 at = -1, last_hp = -1;
  bool armed() const { return at >= 0; }
  void reset() { at = -1; last_hp = -1; }
  bool update(Critical k, f32 hp, f32 max_hp, f32 now);
};

// ---------------------------------------------------------------- ballistics.mjs
/// The Blueprint's enum Round (the same order).
enum class Round : u8 { nine_mm, five_five_six, fifty_bmg };
// The Blueprint's functions MomentumNs and BulletImpulse, and its variable ProjectileGravity.
f32 momentum_ns(Round r);
/// The impulse (N*s) a bullet that stops in a body hands over, along its flight direction.
Vec3 bullet_impulse(Vec3 direction, Round r);
f32 projectile_gravity();

// ---------------------------------------------------------------- rules_vote_engine.mjs
/// The Blueprint's enum Ballot (the same order) and its struct Vote (the same fields).
enum class Ballot : u8 { pending, approved, rejected, cancelled, ignored, invalid };
struct Vote {
  std::string id, key;
  bool value = false;
  u32 host = 0;
  std::vector<u32> members, yes;
  f64 expires = 0;
  Ballot status = Ballot::invalid;
};
// The Blueprint's functions CreateBallot, CastVote and BallotName.
/// Invalid (status invalid) without id/key, a host outside the roster or < 2 voters.
Vote create_ballot(const std::string& id, const std::string& key, bool value, u32 host, std::vector<u32> members, f64 now);
/// The new status (ignored for a stranger or a second yes).
Ballot cast_vote(Vote& b, u32 peer, bool approve, std::vector<u32> current_members, f64 now);
std::string ballot_name(Ballot b);

}  // namespace sc::detail
