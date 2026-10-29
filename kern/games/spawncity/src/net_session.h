// Multiplayer over Kern's NetGame (server-authoritative, snapshot interpolation) and the Kern relay
// (tools/net_relay.mjs) or BroadcastChannel - the interface for the next milestone.
//
// Plan, from what is already in place:
//  * The game world is one kern::game::GameWorld level: the soldier, the drone, the jet and the
//    traffic cars are level actors, so NetGame replicates their poses, spawns and health as is.
//    Every remote player gets a pawn copy of the soldier (GameWorld::add_pawn).
//  * Vehicle forces run in the fixed step on the authority only (Game::fixed_step); clients run
//    the world as a replica (SimMode::replica) and send their input at 30 Hz.
//  * What NetGame does not carry yet travels as custom messages (net::Server/Client::send):
//    who sits in which vehicle (mode + vehicle actor id, SpawnCity's pose field `pm`), the drone's
//    stick state, jet seats, wanted stars, the map origin (lon/lat, so everybody streams the same
//    city: SpawnCity's canonical origin).
//  * The page moves datagrams with kern/web/studio/game.mjs KernNet (it only needs push/take on the
//    engine): the game will add exports sc_net_push / sc_net_take next to the others in main.cpp.
#pragma once

#include <vector>

#include "common.h"

namespace sc::net {

enum class PlayerMode : u8 { foot, drone, car, jet, parachute };

/// One player's state beyond NetGame's actor poses (SpawnCity vs_pose_sync fields).
struct PlayerState {
  u32 pawn = 0;           // soldier actor
  PlayerMode mode = PlayerMode::foot;
  u32 vehicle = 0;        // actor id of the drone / car / jet
  u8 seat = 0;            // jet: 0 pilot, 1 passenger
  f64 lon = 0, lat = 0;   // the map origin this player streams
  i32 stars = 0;
};

/// Transport hooks the host page drives (implemented in the next milestone).
class Session {
 public:
  virtual ~Session() = default;
  virtual bool host(const char* room) = 0;
  virtual bool join(const char* room) = 0;
  virtual void push(u64 from, const u8* bytes, u32 length) = 0;            // datagram in
  virtual bool take(u64& to, std::vector<u8>& bytes) = 0;                  // datagram out
  virtual void update(f32 dt) = 0;
};

}  // namespace sc::net
