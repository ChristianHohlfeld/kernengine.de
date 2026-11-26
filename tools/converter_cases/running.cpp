#include "running.h"
namespace cases {
Runner& runner() { static Runner r; return r; }
kern::i32 Runner::running() const {
  kern::i32 n = 0;
  if (kern::logic::Runtime* rt = world_->logic_runtime())
    for (const kern::game::BlueprintRunning& r : kern::game::blueprints_running(*rt)) n += static_cast<kern::i32>(r.functions);
  return n;
}
}
