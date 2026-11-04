// LIVE GPS (gps_live_mode.mjs): the world follows you in real life. The page passes the phone's
// fixes (watchPosition, already in local metres of the map); this filter rejects jumps, predicts
// with a constant velocity between fixes, glides the shown position and names the activity from
// the speed. Pure logic (tests/test_start.cpp).
#pragma once

#include "common.h"

namespace sc {

class GpsLive {
 public:
  // gps_live_mode.mjs: MAX_ACCURACY_M 65, MAX_SPEED_MPS 75, GAP_RESET_MS 12000, REANCHOR_KM 40
  static constexpr f32 kMaxAccuracy = 65, kMaxSpeed = 75, kGapReset = 12, kReanchorKm = 40;
  enum class Activity : u8 { stand, walk, run, bike, car };
  enum class View : u8 { eye, follow, map };

  void start(f64 now);
  void stop() { active_ = false; }
  bool active() const { return active_; }
  /// A fix in local metres. speed / course < 0: unknown (course radians from +y clockwise, compass style).
  /// Returns false when it is rejected (weak, or an impossible jump).
  bool fix(f64 now, f32 x, f32 y, f32 accuracy, f32 speed = -1, f32 course = -1);
  /// The phone's compass (radians, clockwise from north), used while standing.
  void compass(f32 heading) { compass_ = heading; has_compass_ = true; }
  /// Per frame: glide towards the predicted track.
  void frame(f64 now, f32 dt);
  bool has_fix() const { return has_fix_; }
  Vec2 shown() const { return shown_; }
  Vec2 position() const { return {x_, y_}; }  // the filtered track (trail points)
  f32 heading() const { return heading_; }       // radians, clockwise from +y (north)
  f32 speed() const { return speed_avg_; }
  f32 distance() const { return distance_; }
  f32 accuracy() const { return accuracy_; }
  f64 elapsed(f64 now) const { return active_ ? now - started_ : 0; }
  Activity activity() const;
  const char* activity_label() const;
  View view = View::eye;
  u32 rejected = 0, accepted = 0;

 private:
  bool active_ = false, has_fix_ = false, has_shown_ = false, has_compass_ = false;
  f64 started_ = 0, last_fix_ = -1e9;
  f32 x_ = 0, y_ = 0, vx_ = 0, vy_ = 0, speed_ = 0, speed_avg_ = 0, heading_ = 0, distance_ = 0, accuracy_ = 0, compass_ = 0;
  Vec2 shown_;
};

}  // namespace sc
