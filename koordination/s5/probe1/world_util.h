// Test worlds: flat ground (a large static box, top at z = 0) and optional walls.
#pragma once

#include "../src/common.h"

namespace sc {

inline kern::physics::Body make_static_box(kern::physics::World& w, Vec3 center, Vec3 size, f32 friction = 0.7f) {
  kern::physics::BodySettings bs;
  bs.type = kern::physics::BodyType::fixed;
  bs.position = center;
  const kern::physics::Body b = w.create_body(bs);
  kern::physics::ShapeSettings ss;
  ss.kind = kern::physics::ShapeKind::box;
  ss.size = size;
  ss.material.friction = friction;
  w.create_shape(b, ss);
  return b;
}

inline void make_flat_world(kern::physics::World& w) {
  kern::physics::WorldSettings ws;
  w.create(ws);
  make_static_box(w, {0, 0, -1}, {6000, 6000, 2});
}

}  // namespace sc
