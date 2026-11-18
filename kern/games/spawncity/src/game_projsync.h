// State of the rounds in flight in a session (game_projsync.cpp): the mates' rounds on their
// tracks, ours as last sent. One member of Game (`projsync_`), made when a session first needs it.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#pragma once

#include "kern/core/map.h"
#include <utility>
#include <vector>

#include "common.h"
#include "mp_rules.h"

namespace sc {

struct ProjSync {
  struct Ghost { mp::PoseTimeline line; Vec3 v; f32 heard = 0; bool gone = false; };
  kern::Map<u32, Ghost> ghosts;            // (shooter << 16 | id)
  std::vector<std::pair<u16, u8>> sent;   // ours in the last message, and how often "gone" went out
  f32 clock = 0;
  u32 sent_msgs = 0, got = 0;
};

}  // namespace sc
