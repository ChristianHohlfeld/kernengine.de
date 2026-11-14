// State of the destruction sync (game_destruction_sync.cpp): the craters and the damage as this
// machine last sent or heard them in a session. One member of Game (`dsync_`), made when a session
// first needs it.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#pragma once

#include <utility>
#include <vector>

#include "city_stream.h"
#include "common.h"

namespace sc {

struct DestroySync {
  using Craters = std::vector<Terrain::CraterRec>;
  using Damage = std::vector<CityStream::DamageRec>;
  using Points = std::vector<std::pair<u32, f32>>;
  u32 epoch = 1;                 // the host's (a client: the one he has)
  f32 delta_clock = 0, digest_clock = 0;
  Craters sent;                  // host: the list as last sent; client: the host's list
  struct Mine { Terrain::CraterRec r; f32 at = 0; bool told = false; };
  std::vector<Mine> mine;        // client: own craters not yet in the host's list (when dug, told)
  Damage dmg_out;                // what this machine still has to tell (merged per seed)
  Points pts_out;
  std::vector<u8> known;         // host: the slots that have the whole state
  u32 mismatches = 0;
  f32 told_at = -99;              // client: when our last edit went (its echo may not be back yet)
  bool was_active = false, hosting = false;
  u32 sent_states = 0, got_states = 0, asks = 0;
};

}  // namespace sc
