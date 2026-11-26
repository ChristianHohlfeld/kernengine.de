#include "numbers_engine.h"
namespace cases {
kern::Quat flipped(kern::Quat q) { return {-q.x, -q.y, -q.z, q.w}; }
// (an f32 sum: in 64 bit and then rounded it is the f32 sum - exact in both passes)
kern::physics::Pose lifted(kern::physics::Pose p, float by) { p.position.z += by; return p; }
kern::physics::BodyType kind_of(int i) { return i > 0 ? kern::physics::BodyType::dynamic : kern::physics::BodyType::fixed; }
}
