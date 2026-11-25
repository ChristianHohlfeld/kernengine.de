// The original's water shader on Kern's standard surface shader (see water_look.h).
#include "water_look.h"

#include "kern/gfx/program.h"

namespace sc {

namespace {
constexpr kern::ExtensionAttribute kWater[] = {{"water", 3, "aWater"}};
}  // namespace

kern::Span<const kern::ExtensionAttribute> WaterLook::attributes() const { return {kWater, 1}; }

const char* WaterLook::vertex_declarations() const { return "out vec3 vWater;\n"; }
const char* WaterLook::vertex() const { return "  vWater = aWater;\n"; }

const char* WaterLook::fragment_declarations() const {
  return R"GLSL(
in vec3 vWater;
uniform float uWaterTime;
vec2 waveGrad(vec2 p, vec2 dir, float k, float sp, float a) { float ph = dot(p, dir) * k - uWaterTime * sp; return dir * cos(ph) * k * a; }
float wh(vec2 p) { return fract(sin(dot(floor(p), vec2(41.3, 289.1))) * 43758.5); }
float wn(vec2 p) { vec2 i = floor(p), f = fract(p); f = f * f * (3.0 - 2.0 * f); return mix(mix(wh(i), wh(i + vec2(1, 0)), f.x), mix(wh(i + vec2(0, 1)), wh(i + vec2(1, 1)), f.x), f.y); }
)GLSL";
}

const char* WaterLook::fragment_surface() const {
  // world_water.mjs makeMaterial, line for line (diffuseColor is linear there, as `base` here)
  return R"GLSL(
  if (vWater.x > 0.5) {
    float shore = vWater.x - 1.0;
    vec2 flow = vWater.yz;
    float fl = length(flow);
    vec2 fd = fl > 0.01 ? flow / fl : normalize(vec2(0.8, 0.6));
    vec2 side = vec2(-fd.y, fd.x);
    vec2 adv = vWorld.xy - flow * uWaterTime * 2.2;
    float wDepth = clamp(shore / 9.0, 0.0, 1.0);
    base = mix(vec3(0.08, 0.43, 0.62), vec3(0.025, 0.20, 0.36), wDepth);
    float foam = smoothstep(0.55, 0.95, wn(adv * 0.6 + uWaterTime * 0.15)) * (1.0 - smoothstep(0.0, 2.4, shore))
               + smoothstep(0.75, 1.0, wn(adv * 1.3)) * smoothstep(0.6, 1.6, fl) * 0.6;
    base = mix(base, vec3(0.9, 0.94, 0.95), clamp(foam, 0.0, 0.85));
    alpha = mix(0.55, 0.94, smoothstep(0.0, 3.0, shore));
    vec2 g = waveGrad(adv, fd, 0.9, 2.1, 0.05) + waveGrad(adv, normalize(fd + side * 0.6), 1.7, 3.0, 0.03)
           + waveGrad(adv, normalize(side - fd * 0.3), 2.9, 4.2, 0.018) + waveGrad(vWorld.xy, normalize(vec2(0.6, -0.8)), 5.3, 6.0, 0.008);
    n = normalize(vec3(-g.x, -g.y, 1.0));
  }
)GLSL";
}

void WaterLook::bind(const kern::Program& p) { u_time_ = p.location("uWaterTime"); }
void WaterLook::frame_uniforms(kern::UniformCache& cache, const kern::Renderer&) { cache.set_float(u_time_, time); }

}  // namespace sc
