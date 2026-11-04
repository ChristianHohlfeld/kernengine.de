// Small combat rules of SpawnCity as pure logic (no GameWorld), used by game_details.cpp:
//  * critical_damage_logic.mjs: when a car, bus, police drone or drone turns critical, how long
//    until it blows, and how a further hit brings that forward,
//  * ballistics.mjs: what a bullet hands to the body it hits (p = m * v along its path),
//  * rules_vote_engine.mjs: the unanimous ballot over a game rule (the network carries the votes).
#pragma once
#include <string>
#include <vector>

#include "common.h"

namespace sc::detail {

// ---------------------------------------------------------------- critical_damage_logic.mjs
enum class Critical : u8 { car, bus, police_drone, drone };
struct CriticalProfile { f32 ratio, delay_s, accelerated_s; };
constexpr CriticalProfile kCritical[] = {
    {0.20f, 2.45f, 0.62f},  // car
    {0.20f, 3.10f, 0.72f},  // bus
    {0.34f, 2.20f, 0.56f},  // police-drone
    {0.28f, 2.05f, 0.54f},  // drone
};
inline const CriticalProfile& critical(Critical k) { return kCritical[static_cast<u8>(k)]; }
inline bool is_critical(Critical k, f32 hp, f32 max_hp) { return max_hp > 0 && clampf(hp, 0, max_hp) <= max_hp * critical(k).ratio; }
inline f32 detonation_at(Critical k, f32 now) { return now + critical(k).delay_s; }
inline f32 accelerate(Critical k, f32 current_at, f32 now) { return std::fmin(current_at, now + critical(k).accelerated_s); }

/// One thing that can turn critical: call update() with its hp every frame; it returns true
/// once, when the countdown has run out.
struct CriticalClock {
  f32 at = -1, last_hp = -1;
  bool armed() const { return at >= 0; }
  void reset() { at = -1; last_hp = -1; }
  bool update(Critical k, f32 hp, f32 max_hp, f32 now);
};

// ---------------------------------------------------------------- ballistics.mjs
enum class Round : u8 { nine_mm, five_five_six, fifty_bmg };
struct RoundSpec { const char* name; f32 mass_kg, speed_mps; };
constexpr RoundSpec kRounds[] = {
    {"9mm", 0.008f, 360},     // Glock 17 / SMG (9x19 mm, 124 gr)
    {"5.56", 0.004f, 940},    // drone gun (5.56x45 mm)
    {".50bmg", 0.0458f, 890}, // anti-materiel sniper rifle (12.7x99 mm, 706 gr): 41 N*s
};
inline f32 momentum_ns(Round r) { return kRounds[static_cast<u8>(r)].mass_kg * kRounds[static_cast<u8>(r)].speed_mps; }
/// The impulse (N*s) a bullet that stops in a body hands over, along its flight direction.
inline Vec3 bullet_impulse(Vec3 direction, Round r) { return kern::normalize(direction, {1, 0, 0}) * momentum_ns(r); }
constexpr f32 kProjectileGravity = 9.80665f;

// ---------------------------------------------------------------- rules_vote_engine.mjs
/// Pure unanimous-vote state machine. Only the current roster votes; nobody joins a running ballot.
/// The host (proposer) has voted yes; the ballot expires after 30 s.
enum class Ballot : u8 { pending, approved, rejected, cancelled, ignored, invalid };
struct Vote {
  std::string id, key;
  bool value = false;
  u32 host = 0;
  std::vector<u32> members, yes;  // members sorted
  f64 expires = 0;
  Ballot status = Ballot::invalid;
};
constexpr f64 kBallotSeconds = 30;
/// createBallot: invalid (status invalid) without id/key, a host outside the roster or < 2 voters.
Vote create_ballot(const std::string& id, const std::string& key, bool value, u32 host, std::vector<u32> members, f64 now);
/// vote(): the new status (ignored for a stranger or a second yes).
Ballot cast_vote(Vote& b, u32 peer, bool approve, std::vector<u32> current_members, f64 now);
const char* ballot_name(Ballot b);

}  // namespace sc::detail
