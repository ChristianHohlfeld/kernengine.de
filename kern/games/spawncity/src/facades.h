// The original's facades (spawncity.de world_city_buildings.mjs realFacades / FACADE_GLSL,
// "real-facades-v6"): corner pilasters, a ground-floor shop band with storefront glass and
// awnings, framed windows with sills (style per building), curtain walls on towers, grime near the
// ground - all in the shader from the wall record Kern's BuildingBuilder writes per vertex
// (aWall: wall length or -1 roof / -2 cornice, wall height, building seed, storey height) and the
// wall coordinates in vUV (metres along the wall, metres above the base). At night lit windows
// glow (the window mask of the facade; the old NightWindows look on the original's windows).
#pragma once

#include "common.h"
#include "kern/gfx/shader.h"

namespace sc {

class Facades final : public kern::ShaderExtension {
 public:
  f32 night = 0;  // 0 day .. 1 night (game_look.cpp)
  const char* name() const override { return "spawncity-facades"; }
  kern::Span<const kern::ExtensionAttribute> attributes() const override;
  const char* vertex_declarations() const override;
  const char* vertex() const override;
  const char* fragment_declarations() const override;
  const char* fragment_surface() const override;
  void bind(const kern::Program& p) override;
  void frame_uniforms(kern::UniformCache& cache, const kern::Renderer&) override;

 private:
  u32 u_night_ = 0;
};

}  // namespace sc
