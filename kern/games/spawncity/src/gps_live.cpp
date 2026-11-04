// LIVE GPS filter, the numbers of gps_live_mode.mjs (onFix / frame).
#include "gps_live.h"

#include <cmath>

namespace sc {

void GpsLive::start(f64 now) {
  const View v = view;
  *this = GpsLive{};
  view = v;
  active_ = true;
  started_ = now;
}

bool GpsLive::fix(f64 now, f32 x, f32 y, f32 accuracy, f32 speed, f32 course) {
  if (!active_ || !std::isfinite(x) || !std::isfinite(y)) return false;
  accuracy_ = std::fmax(0.0f, accuracy);
  if (accuracy_ > kMaxAccuracy) { ++rejected; return false; }  // weak fix
  const f32 dt = static_cast<f32>(now - last_fix_);
  if (!has_fix_ || dt > kGapReset) {
    // First fix, or back after a long gap (screen off): start cleanly there.
    x_ = x; y_ = y; vx_ = vy_ = 0;
    has_fix_ = true;
    has_shown_ = false;
    last_fix_ = now;
    ++accepted;
    return true;
  }
  // Predict, then correct with a gain that trusts precise fixes more.
  const f32 h = std::fmax(0.2f, dt);
  const f32 px = x_ + vx_ * dt, py = y_ + vy_ * dt, ex = x - px, ey = y - py;
  const f32 jump = std::hypot(x - x_, y - y_) / h;
  if (jump > kMaxSpeed && accuracy_ > 8) { ++rejected; return false; }
  const f32 g = clampf(1 - accuracy_ / (accuracy_ + 12), 0.25f, 0.85f), gv = clampf(g * 0.6f, 0.15f, 0.6f) / h;
  const f32 ox = x_, oy = y_;
  x_ = px + ex * g;
  y_ = py + ey * g;
  vx_ += ex * gv;
  vy_ += ey * gv;
  // The phone's own speed is better than differencing when it has one.
  if (speed >= 0) {
    const f32 sp = std::hypot(vx_, vy_);
    if (sp > 0.05f) { const f32 k = speed / sp; vx_ = vx_ * 0.5f + vx_ * k * 0.5f; vy_ = vy_ * 0.5f + vy_ * k * 0.5f; }
    else if (speed < 0.3f) { vx_ *= 0.5f; vy_ *= 0.5f; }
  }
  speed_ = std::hypot(vx_, vy_);
  if (speed_ > 1.2f) heading_ = course >= 0 && speed > 1 ? course : std::atan2(vx_, vy_);
  distance_ += std::fmin(std::hypot(x_ - ox, y_ - oy), kMaxSpeed * h);
  last_fix_ = now;
  ++accepted;
  return true;
}

void GpsLive::frame(f64 now, f32 dt) {
  if (!active_ || !has_fix_) return;
  const f32 ahead = static_cast<f32>(std::fmin(2.0, now - last_fix_));
  const Vec2 target{x_ + vx_ * ahead, y_ + vy_ * ahead};
  if (!has_shown_ || std::hypot(target.x - shown_.x, target.y - shown_.y) > 60) { shown_ = target; has_shown_ = true; }
  else {
    const f32 k = 1 - std::exp(-dt * 4);
    shown_ = shown_ + (target - shown_) * k;
  }
  speed_avg_ += (speed_ - speed_avg_) * std::fmin(1.0f, dt / 4);
  if (speed_avg_ < 1 && has_compass_) heading_ = compass_;
}

GpsLive::Activity GpsLive::activity() const {
  const f32 s = speed_avg_;
  return s < 0.5f ? Activity::stand : s < 2.3f ? Activity::walk : s < 5.2f ? Activity::run : s < 11 ? Activity::bike : Activity::car;
}

const char* GpsLive::activity_label() const {
  switch (activity()) {
    case Activity::stand: return "STEHT";
    case Activity::walk: return "GEHEN";
    case Activity::run: return "LAUFEN";
    case Activity::bike: return "RAD";
    case Activity::car: return "FAHRT";
  }
  return "";
}

}  // namespace sc
