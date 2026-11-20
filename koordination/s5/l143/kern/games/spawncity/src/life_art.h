// Drawing of street life (life.h) and ragdolls (ragdoll.h): instanced, tinted unit primitives
// (box, ball, cylinder) like people_art.h - cows, pigeons, birds, trees, ragdoll bones, doors.
#pragma once

#include <vector>

#include "kern/gfx/renderer.h"
#include "life.h"

namespace sc {

class LifeArt {
 public:
  bool init(kern::Renderer& r);
  void begin();
  void box(const kern::Mat4& m, Vec3 tint) { put(box_, m, tint); }
  void ball(const kern::Mat4& m, Vec3 tint) { put(ball_, m, tint); }
  void cylinder(const kern::Mat4& m, Vec3 tint) { put(cyl_, m, tint); }
  void glow(const kern::Mat4& m, Vec3 tint) { put(glow_, m, tint); }
  /// Everything of street life around `eye` (cows, pigeons, birds, trees, ragdolls).
  void life(const Life& life, f32 now, Vec3 eye, f32 range);
  void end();

 private:
  struct Part {
    u32 id = 0;
    std::vector<f32> transforms, tints;
  };
  bool make(kern::Renderer& r, Part& p, const kern::MeshData& mesh, const kern::Material& m);
  void put(Part& p, const kern::Mat4& m, Vec3 tint);
  void cow(const Life::Cow& c, f32 now);
  kern::Renderer* r_ = nullptr;
  Part box_, ball_, cyl_, glow_;
  std::vector<Ragdolls::Draw> bones_;
};

}  // namespace sc
