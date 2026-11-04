// The drone's GUIDED missile (LENKRAKETE) as pure logic, SpawnCity guided_missile.mjs (press-fires-v3)
// with the numbers of guided_balance.mjs:
//  * a press marks what is closest to the crosshair on screen (70 px, a little slack for small far
//    things, at most 600 m away): a person, a car or bus, a police drone, another player; with
//    nothing there it flies to the aimed point (ground, wall),
//  * it drops off the rail for 0.32 s, then the motor lights and it flies like a cruise missile:
//    up to max(launch + 8 m, target + 28 m) and at least 18 m above the ground, flat at 34 m/s,
//    turning with at most 2.2 g; inside 70 m it dives steeply (1.4 x the turn, 1.12 x the speed),
//  * proximity fuze 2.4 m, 4 m blast with 34 damage, 22 s at most, 5 s between launches,
//  * a pedestrian who sprints (>= 5.8 m/s for 0.35 s) shakes the lock off: the missile flies on to
//    the last place it knew.
#pragma once
#include "common.h"

namespace sc::guided {

// guided_balance.mjs GUIDED_BALANCE
constexpr f32 kSpeed = 34, kTurnG = 2.2f, kBlastRadius = 4, kDamage = 34, kCooldown = 5, kDodgeSpeed = 5.8f, kDodgeHold = 0.35f;
// guided_missile.mjs
constexpr f32 kPickPx = 70, kRange = 600, kTtl = 22, kFuze = 2.4f, kDiveRange = 70, kIgnite = 0.32f;
constexpr f32 kCruiseAboveLaunch = 8, kCruiseAboveTarget = 28, kCruiseAgl = 18;

/// updatePedestrianDodge: seconds spent sprinting; `lost` once held long enough (it stays lost).
struct Dodge {
  f32 seconds = 0;
  bool lost = false;
};
Dodge update_dodge(Dodge state, Vec2 previous, Vec2 current, f32 dt, bool pedestrian);

/// Screen distance (px) of `target` from the screen centre for a camera at `eye` looking along
/// `forward` (unit) with vertical field of view `fov` (rad) on a view `height_px` tall; < 0 when it
/// is behind the camera. Includes SpawnCity's slack: minus min(18, 600 / distance).
f32 pick_score(Vec3 eye, Vec3 forward, f32 fov, f32 height_px, Vec3 target);

enum class Result : u8 { flying, fuze, ground, timeout };

struct Missile {
  Vec3 p, v;
  Vec3 goal;          // where it flies: the target's last known place
  f32 born = 0, cruise_z = 0;
  bool motor = false;
  bool pedestrian = false;  // the target can sprint away (a person or a player on foot)
  Dodge dodge;
  Vec2 dodge_last;
  bool tracking = true;     // false: lock lost (dodged, or the target is gone)
};

/// What a missile homes on (game_guided.cpp resolves it each frame).
enum class TargetKind : u8 { point, person, car, police_drone, player };
struct Target {
  TargetKind kind = TargetKind::point;
  i32 index = -1;
  u8 slot = 0;            // players: the network slot (the remote list may reorder)
  const char* label = "ZIELPUNKT";
};
struct Flight {
  Missile m;
  Target t;
  Vec3 prev;
};

/// A missile dropped from `start` towards `target` (rail push 6 m/s horizontal, 3 m/s down).
Missile launch(Vec3 start, Vec3 target, f32 now, bool pedestrian);
/// The target moved to `at` (only while tracking); dt for the dodge check.
void track(Missile& m, Vec3 at, f32 dt);
/// One step; `ground` is the ground height under the missile.
Result step(Missile& m, f32 now, f32 dt, f32 ground);

}  // namespace sc::guided
