// Nothing comes into existence where a player sees it happen (spawn_visibility_guard.mjs), and no
// two vehicles are spawned into each other (vehicle_spawn_clearance.mjs).
//
// The rules themselves are a Blueprint: the level's actor "SpawnGuard" (level/spawncity.klevel,
// open it in Kern Studio). Its variables hold the distances and angles, its functions the tests -
// and it asks the world itself whether buildings hide a point from an eye (Line Trace by Channels
// against the level's channel "World"). This header is only how the game's C++ calls them:
// nothing is computed here. The game hands in the viewers (its own presented camera and the other
// players' views).
#pragma once

#include <vector>

#include "common.h"
#include "kern/logic/logic.h"

namespace sc::spawn {

/// The running Blueprint of the actor "SpawnGuard" (the game binds it once its world has started;
/// tests bind the one they run). Unbound, nothing may spawn and nothing overlaps.
void bind(kern::logic::Instance* blueprint);
kern::logic::Instance* blueprint();

/// The Blueprint's variable DefaultFar.
f32 default_far();

/// The Blueprint's struct Viewer (the same fields).
struct Viewer {
  Vec3 eye, forward{1, 0, 0};  // forward: unit
  f32 half_angle = kPi / 2;
  f32 far = 1500;
  bool local = false;          // the presented camera (frustum sphere test as well as the cone)
};

/// Half angle of the view cone for a vertical field of view (radians) and aspect - the
/// Blueprint's function ConeHalfAngle.
f32 cone_half_angle(f32 vertical_fov, f32 aspect);

/// Would something of `height` standing at `point` be seen by `v`? (the Blueprint's SeenBy)
/// far_entry > 0: beyond this distance an arrival is allowed even in view (far traffic).
bool seen_by(const Viewer& v, Vec3 point, f32 far_entry, f32 height);
/// No viewer sees it (the Blueprint's CanSpawn).
bool can_spawn(const std::vector<Viewer>& viewers, Vec3 point, f32 far_entry, f32 height);

/// The Blueprint's struct Footprint: half extents along the body's x / y, turned by yaw.
struct Footprint { Vec2 center; f32 yaw = 0; Vec2 half; };
/// The Blueprint's function FootprintsOverlap (margin 0.35 m).
bool footprints_overlap(const Footprint& a, const Footprint& b, f32 margin = 0.35f);

}  // namespace sc::spawn
