// SpawnCity on Kern - shared helpers for every game module.
//
// Conventions (Kern's): metres, seconds, kilograms, Z up. The world's X axis
// points east and Y north (kern::GeoProjector). Licensed under the Apache
// License 2.0 like Kern (see LICENSE in the repository root).
#pragma once

#include <cmath>
#include <cstdint>

#include "kern/game/vehicles/base.h"
#include "kern/math/math.h"
#include "kern/physics/physics.h"

namespace sc {

// Units, the small helpers (clampf, to_local, yaw_quat ...), BodyState, RigidSpec and Rng are the
// engine's (kern/game/vehicles/base.h), as are the vehicle models themselves (Car, Drone, Jet):
// the game speaks that vocabulary.
using namespace kern::game::vehicles;

using kern::f32;
using kern::f64;
using kern::i32;
using kern::i64;
using kern::u32;
using kern::u64;
using kern::u8;
using kern::u16;
using kern::Quat;
using kern::Vec2;
using kern::Vec3;

// Collision categories (SpawnCity world_rigid_body_physics.mjs: terrain 1, vehicle 2, drone 4,
// query 8). Kern's GameWorld puts the ground and every level collider in category 1; the
// streamed city uses it too.
enum : u64 { kCatWorld = 1, kCatVehicle = 2, kCatDrone = 4, kCatQuery = 8 };

}  // namespace sc
