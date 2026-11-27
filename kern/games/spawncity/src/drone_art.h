// The drone's model (simulator.mjs airframe) drawn from code as instanced Kern meshes: one mesh per part, its poses
// from the interpolated Box3D body every frame. (Taken from vehicle_art.h on 26.11.2026: the cars, the jet and the
// parachute are out of the small game.)
#pragma once

#include <array>
#include <vector>

#include "common.h"
#include "kern/gfx/renderer.h"

namespace sc {

class DroneArt {
 public:
  bool init(kern::Renderer& r);
  /// The quad (body frame -X nose); rotor angles per motor. Nothing in the FPV view (the camera sits on it).
  void draw(const kern::Mat4& body, const std::array<Vec3, 4>& motors, const std::array<f32, 4>& rotor_angles, f32 prop_d, bool armed, bool fpv_view);
  /// Hides it (the drone is not in the world).
  void hide();

 private:
  struct Part {
    u32 id = 0;
    std::vector<f32> transforms, tints;
    bool tinted = false;
  };
  bool make(kern::Renderer& r, Part& p, const kern::MeshData& mesh, const kern::Material& m, bool tinted);
  void put(Part& p, const kern::Mat4& m, Vec3 tint = {1, 1, 1});
  static void clear(Part& p);
  void upload(Part& p);

  kern::Renderer* r_ = nullptr;
  Part frame_, motor_, prop_a_, prop_b_, led_;
};

}  // namespace sc
