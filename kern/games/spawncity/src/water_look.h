// The water surface as the original draws it (spawncity.de world_water.mjs makeMaterial): animated
// wave normals scrolling along the flow, deep water darker than the shallows by the distance to
// the shore, foam at the banks and in fast water, the shallow edge more transparent. On Kern's
// standard shader from one vertex stream the terrain's water mesh carries per 4 m node
// (aWater: shore distance + 1 (0 = not water), flow x, flow y in m/s).
#pragma once

#include "common.h"
#include "kern/gfx/shader.h"

namespace sc {

class WaterLook final : public kern::ShaderExtension {
 public:
  f32 time = 0;  // seconds (uTime)
  const char* name() const override { return "spawncity-water"; }
  kern::Span<const kern::ExtensionAttribute> attributes() const override;
  const char* vertex_declarations() const override;
  const char* vertex() const override;
  const char* fragment_declarations() const override;
  const char* fragment_surface() const override;
  void bind(const kern::Program& p) override;
  void frame_uniforms(kern::UniformCache& cache, const kern::Renderer&) override;

 private:
  u32 u_time_ = 0;
};

}  // namespace sc
