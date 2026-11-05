// Nothing comes into existence where a player sees it happen (spawn_visibility_guard.mjs), and no
// two vehicles are spawned into each other (vehicle_spawn_clearance.mjs). Pure rules, no GameWorld:
// the game hands in the viewers (its own presented camera and the other players' views) and a
// callback that says whether buildings hide a point from an eye (Box3D rays in the game).
#pragma once

#include <functional>
#include <vector>

#include "common.h"

namespace sc::spawn {

// spawn_visibility_guard.mjs
constexpr f32 kMinDistance = 24;                // MIN_SPAWN_DISTANCE_M: never closer to any viewer
constexpr f32 kConeMargin = 20 * kDeg;          // CONE_MARGIN_RAD on top of the half field of view
constexpr f32 kOcclusionMax = 420;              // OCCLUSION_MAX_M: buildings only hide what is nearer
constexpr f32 kDefaultFar = 1500;               // DEFAULT_FAR_M
constexpr f32 kDefaultHeight = 2.2f;            // canSpawnAt heightM
constexpr f32 kFrustumSphere = 1.5f;            // the sphere tested against the local frustum

struct Viewer {
  Vec3 eye, forward;       // forward: unit
  f32 half_angle = kPi / 2;
  f32 far = kDefaultFar;
  bool local = false;      // the presented camera (frustum sphere test as well as the cone)
};

/// Half angle of the view cone for a vertical field of view (radians) and aspect:
/// max(vertical, horizontal) / 2 + 20 degrees, at most 0.92 pi (localViewer coneHalf).
f32 cone_half_angle(f32 vertical_fov, f32 aspect);

/// Buildings between eye and point? (true: hidden)
using Blocked = std::function<bool(Vec3 eye, Vec3 point)>;

/// Would something of `height` standing at `point` be seen by `v`? (seenBy)
/// far_entry > 0: beyond this distance an arrival is allowed even in view (far traffic).
bool seen_by(const Viewer& v, Vec3 point, f32 far_entry, f32 height, const Blocked& blocked);
/// No viewer sees it (safeSpawn / canSpawnAt).
bool can_spawn(const std::vector<Viewer>& viewers, Vec3 point, f32 far_entry, f32 height, const Blocked& blocked);

// vehicle_spawn_clearance.mjs: separating-axis test of two yawed footprints (half extents along
// the body's x / y), with a margin (default 0.35 m).
struct Footprint { Vec2 center; f32 yaw = 0; Vec2 half; };
bool footprints_overlap(const Footprint& a, const Footprint& b, f32 margin = 0.35f);

}  // namespace sc::spawn
