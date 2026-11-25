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

// Plain state with the functions of a shader extension: the renderer gets it as
// LookExtension<Facades> (shader_look.h).
class Facades final {
 public:
  f32 night = 0;  // 0 day .. 1 night (game_look.cpp)
  const char* name() const { return "spawncity-facades"; }
  kern::Span<const kern::ExtensionAttribute> attributes() const;
  const char* vertex_declarations() const;
  const char* vertex() const;
  const char* fragment_declarations() const;
  const char* fragment_surface() const;
  void bind(const kern::Program& p);
  void frame_uniforms(kern::UniformCache& cache, const kern::Renderer&);

 private:
  u32 u_night_ = 0;
};

}  // namespace sc
