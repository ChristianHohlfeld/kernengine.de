// Drawing for the M3 world (game_world*.cpp): instanced Kern meshes made from code (rubble,
// animals, traffic lights, tow truck parts, ragdoll limbs) and rebuilt decal meshes (scorch on the
// ground, soot and breaches on facades). No foreign assets.
#pragma once

#include <vector>

#include "common.h"
#include "kern/geo/mesh_data.h"
#include "kern/gfx/renderer.h"

namespace sc {

class WorldArt {
 public:
  using PartId = u32;
  bool init(kern::Renderer& r);
  /// A new instanced part (mesh in its own frame); tinted parts take a colour per instance.
  PartId part(const kern::MeshData& mesh, const kern::Material& m, bool tinted);
  void begin();
  void put(PartId p, const kern::Mat4& m, Vec3 tint = {1, 1, 1});
  void end();
  /// Replaces a whole one-off mesh (decals); empty data hides it. `slot` keeps the mesh id.
  void set_mesh(u32& slot, const kern::MeshData& mesh, const kern::Material& m);
  kern::Renderer* renderer() { return r_; }

  // common parts (made in init)
  PartId cube = 0;       // unit cube, tinted
  PartId sphere = 0;     // unit sphere (radius 1), tinted
  PartId cylinder = 0;   // radius 1, height 1, base at 0, tinted
  PartId glow = 0;       // unit sphere, unlit emissive (lamps), tinted

 private:
  struct Part {
    u32 id = 0;
    std::vector<f32> transforms, tints;
    bool tinted = false;
  };
  kern::Renderer* r_ = nullptr;
  std::vector<Part> parts_;
};

}  // namespace sc
