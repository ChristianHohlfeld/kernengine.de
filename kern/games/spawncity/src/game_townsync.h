// State of the town's sync in a session (game_townsync.cpp, town_sync.h): the lamps, planks and
// marks still to tell, and who has the whole state. One member of Game (`townsync_`), made when a
// session first needs it.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#pragma once

#include <vector>

#include "common.h"
#include "town_sync.h"

namespace sc {

struct TownSync {
  u32 epoch = 1;                          // the host's (a client: the one he has; 0 none yet)
  bool was_active = false, hosting = false;
  bool marks_taken = false;               // client: the host's marks were run here (once a session)
  std::vector<u8> known;                  // host: the slots that have the whole state
  std::vector<town::LampRec> lamps_out;   // lamps that broke here (a host: or at a client's), still to tell
  std::vector<town::MarkRec> marks;       // the blasts that left marks here, the newest last
  std::vector<town::MarkRec> marks_in;    // client: the host's, to run once the world stands
  u32 car_hits[3] = {0, 0, 0}, person_hits = 0, sent_states = 0, got_states = 0, asks = 0;
};

}  // namespace sc
