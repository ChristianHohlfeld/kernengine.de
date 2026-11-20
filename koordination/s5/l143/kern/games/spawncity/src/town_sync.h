// One town in multiplayer, the part that is not a pose: what a client does to the host's town and
// the town's lasting state, as the bytes that travel (net_session.h town_to_host / town_to_all /
// town_to; the game side is game_townsync.cpp). Pure packing - no GameWorld.
//  * a client's hit on a traffic or parked car (the host's index of it, the damage, what did it) and
//    on a passer-by who survives it (the host's index and the person's seed, the damage, what did it,
//    the push): the host applies them as if they had happened on his machine - who did it is the
//    connection the message came on
//  * street lamps: broken ones by their 8 m cell, only ever worse (dark, then down) - whoever breaks
//    one tells the host, the host tells everybody
//  * barricade planks: per building and door; a plank broken or nailed back is a step the host
//    applies, the door's count he then has goes to everybody
//  * the blasts that left scorch and wall marks, in their order (the newest 64): a joiner runs them
//    again and has the marks the others have
//  * the whole state for a joiner (and for everybody after a RESET: a new epoch), in as many
//    messages as it takes, the first and the last marked
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#pragma once

#include <vector>

#include "common.h"

namespace sc::town {

enum class Kind : u8 { none = 0, car_hit = 1, person_hit = 2, lamps = 3, plank_step = 4, planks = 5, state = 6, ask = 7 };
/// What a message is (its first byte), or none.
Kind kind_of(const std::vector<u8>& blob);

enum class Cause : u8 { other = 0, round = 1, blast = 2, kLast = blast };
struct CarHit { u8 car = 0; f32 damage = 0; Cause cause = Cause::other; };
/// by: People::HurtBy (0 a round, 1 a blow, 2 a vehicle).
struct PersonHit { u8 person = 0; u32 seed = 0; f32 damage = 0; u8 by = 0; Vec3 push; };
struct LampRec { i64 key = 0; u8 state = 0; bool operator==(const LampRec&) const = default; };
/// One door's planks; `doors`: how many doors its building has.
struct PlankRec { i64 key = 0; u8 door = 0, doors = 0, planks = 0; bool operator==(const PlankRec&) const = default; };
struct PlankStep { i64 key = 0; u8 door = 0, doors = 0; kern::i8 step = 0; };
struct MarkRec { Vec3 at; f32 radius = 0, strength = 0; };
constexpr std::size_t kMarks = 64;       // the blasts kept (scorches 40, soot 48, breaches 64 - world_marks.h)
constexpr u8 kPlanksMax = 6, kDoorsMax = 64;

std::vector<u8> pack(const CarHit& h);
bool unpack(const std::vector<u8>& blob, CarHit& h);
std::vector<u8> pack(const PersonHit& h);
bool unpack(const std::vector<u8>& blob, PersonHit& h);
/// Lamp changes (at most 255 a message; more go in further messages).
std::vector<std::vector<u8>> pack_lamps(const std::vector<LampRec>& lamps);
bool unpack_lamps(const std::vector<u8>& blob, std::vector<LampRec>& lamps);
std::vector<u8> pack(const PlankStep& s);
bool unpack(const std::vector<u8>& blob, PlankStep& s);
std::vector<u8> pack(const PlankRec& r);
bool unpack(const std::vector<u8>& blob, PlankRec& r);
std::vector<u8> pack_ask();

/// The whole state: one part of it as it arrives. `first`: the receiver starts from nothing (his
/// own broken lamps and planks go - the town is the host's); `last`: that was all of it.
struct State {
  u32 epoch = 0;
  bool first = false, last = false;
  std::vector<LampRec> lamps;
  std::vector<PlankRec> planks;
  std::vector<MarkRec> marks;
};
constexpr std::size_t kStateBytes = 1000;  // a part's size at most (reliable, but small parts interleave with the rest)
std::vector<std::vector<u8>> pack_state(u32 epoch, const std::vector<LampRec>& lamps, const std::vector<PlankRec>& planks, const std::vector<MarkRec>& marks);
bool unpack_state(const std::vector<u8>& blob, State& s);

}  // namespace sc::town
