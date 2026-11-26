#pragma once
#include "kern/math/math.h"
#include "kern/physics/physics.h"
namespace cases {
/// The engine's own types in a function of the program: a quaternion, a pose (Vec3 + Quat), an enum.
kern::Quat flipped(kern::Quat q);
kern::physics::Pose lifted(kern::physics::Pose p, float by);
kern::physics::BodyType kind_of(int i);
}
