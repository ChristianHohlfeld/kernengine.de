// The water surface as the original draws it (spawncity.de world_water.mjs makeMaterial): animated
// wave normals scrolling along the flow, deep water darker than the shallows by the distance to
// the shore, foam at the banks and in fast water, the shallow edge more transparent. On Kern's
// standard shader from one vertex stream the terrain's water mesh carries per 4 m node
// (aWater: shore distance + 1 (0 = not water), flow x, flow y in m/s).
#pragma once

#include "common.h"
#include "kern/gfx/shader.h"

namespace sc {

// Plain state with the functions of a shader extension: the renderer gets it as
// LookExtension<WaterLook> (shader_look.h).
class WaterLook final {
 public:
  f32 time = 0;  // seconds (uTime)
  const char* name() const { return "spawncity-water"; }
  kern::Span<const kern::ExtensionAttribute> attributes() const;
  const char* vertex_declarations() const;
  const char* vertex() const;
  const char* fragment_declarations() const;
  const char* fragment_surface() const;
  void bind(const kern::Program& p);
  void frame_uniforms(kern::UniformCache& cache, const kern::Renderer&);

 private:
  u32 u_time_ = 0;
};

}  // namespace sc
