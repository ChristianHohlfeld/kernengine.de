// State of the thrown things in a session (game_toss.cpp): the bodies our hand took, and the
// mates' on their tracks. One member of Game (`toss_`), made when a session first needs it.
//
// Licensed under the Apache License 2.0 (see LICENSE in the repository root).
#pragma once

#include <vector>

#include "common.h"
#include "mp_rules.h"

namespace sc {

struct TossState {
  struct Own { u8 kind = 0; u64 id = 0; i32 local = -1; f32 still = 0, born = 0; u8 done_sent = 0; };
  struct Remote { u8 from = 0, kind = 0, flags = 0; u64 id = 0; Vec3 p, v; Quat q; f32 at = 0; i32 local = -1; bool released = false; mp::PoseTimeline line; };
  std::vector<Own> own;
  std::vector<Remote> remote;
  f32 clock = 0;
  u32 sent = 0, got = 0;
};

}  // namespace sc
