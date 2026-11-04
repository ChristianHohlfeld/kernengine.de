// SpawnCity on Kern: frame-time meter and automatic quality level (perf.h).
#include "perf.h"

namespace sc::perf {

const char* slot_name(Slot s) {
  switch (s) {
    case Slot::logic: return "logic";
    case Slot::physics: return "physics";
    case Slot::stream: return "stream";
    case Slot::draw: return "draw";
    default: return "?";
  }
}

void Meter::lap(Slot s, f64 now_ms) {
  if (s >= Slot::count) return;
  cur_[static_cast<u32>(s)] += static_cast<f32>(now_ms - last_);
  last_ = now_ms;
}

void Meter::end_frame(f64 now_ms, f32 render_cpu_ms, f32 gpu_ms) {
  constexpr f32 a = 0.1f;
  for (u32 i = 0; i < static_cast<u32>(Slot::count); ++i) {
    avg_[i] += (cur_[i] - avg_[i]) * a;
    cur_[i] = 0;
  }
  update_ += (static_cast<f32>(now_ms - start_) - update_) * a;
  if (render_cpu_ms >= 0) render_ += (render_cpu_ms - render_) * a;
  gpu_ = gpu_ms < 0 ? -1 : (gpu_ < 0 ? gpu_ms : gpu_ + (gpu_ms - gpu_) * a);
}

void Governor::reset(i32 tier, i32 max_tier) {
  max_ = max_tier < 0 ? 0 : (max_tier >= kTiers ? kTiers - 1 : max_tier);
  tier_ = tier < 0 ? 0 : (tier > max_ ? max_ : tier);
  auto_ = true;
  avg_dt_ = 1.0f / 60;
  clock_ = 0;
  since_change_ = kSettle - kWarmup;  // the first decision waits for the warm-up
  low_ = high_ = 0;
  for (i32 i = 0; i < kTiers; ++i) block_until_[i] = block_len_[i] = 0;
}

void Governor::set_fixed(i32 tier) {
  auto_ = false;
  tier_ = tier < 0 ? 0 : (tier >= kTiers ? kTiers - 1 : tier);
}

bool Governor::update(f32 dt) {
  if (!(dt > 0)) return false;
  clock_ += dt;
  since_change_ += dt;
  // Frame-time average (the rate is its inverse): robust against single long frames.
  avg_dt_ += (dt - avg_dt_) * damp_alpha(1.0f / kAverageTau, dt);
  if (!auto_) return false;
  if (since_change_ < kSettle) { low_ = high_ = 0; return false; }
  const f32 fps = 1.0f / avg_dt_;
  low_ = fps < kDownFps ? low_ + dt : 0;
  high_ = fps >= kUpFps ? high_ + dt : 0;
  if (low_ >= kDownHold && tier_ > 0) {
    // The level we leave was too heavy: block it for a while, longer each time.
    f32& len = block_len_[tier_];
    len = len <= 0 ? kBlockFirst : std::fmin(kBlockMax, len * 2);
    block_until_[tier_] = clock_ + len;
    --tier_;
    since_change_ = 0;
    low_ = high_ = 0;
    ++changes_;
    return true;
  }
  if (high_ >= kUpHold && tier_ < max_ && clock_ >= block_until_[tier_ + 1]) {
    ++tier_;
    since_change_ = 0;
    low_ = high_ = 0;
    ++changes_;
    return true;
  }
  return false;
}

}  // namespace sc::perf
