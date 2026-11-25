// The original's facade shader on Kern's standard surface shader (see facades.h).
#include "facades.h"

#include "kern/gfx/program.h"

namespace sc {

namespace {
// The same vertex stream Kern's BuildingBuilder fills (key "wall", 4 floats).
constexpr kern::ExtensionAttribute kWall[] = {{"wall", 4, "aWall"}};
}  // namespace

kern::Span<const kern::ExtensionAttribute> Facades::attributes() const { return {kWall, 1}; }

const char* Facades::vertex_declarations() const { return "flat out vec4 vWall;\n"; }
const char* Facades::vertex() const { return "  vWall = aWall;\n"; }

const char* Facades::fragment_declarations() const {
  return R"GLSL(
// flat: constant per wall (world_city_buildings.mjs snaps the seed for the same reason)
flat in vec4 vWall;
uniform float uNight;
float fh(float n) { return fract(sin(n * 127.1) * 43758.5453); }
)GLSL";
}

const char* Facades::fragment_surface() const {
  // FACADE_GLSL of world_city_buildings.mjs; u = vUV.x, len = vWall.x, z = vUV.y, H = vWall.y,
  // seed = vWall.z, floorH = vWall.w. Glass: Kern's surfaceGlass (sky reflection) for the
  // original's roughness 0.07 / metalness 0.35 / Fresnel sky emissive.
  return R"GLSL(
  {
    float sdq = floor(vWall.z * 997.0 + 0.5) / 997.0;
    vec3 wn = normalize(cross(dFdx(vWorld), dFdy(vWorld)));
    float u = vUV.x, len = vWall.x, z = vUV.y, H = vWall.y;
    // only Kern's building walls carry a record (len > 0); other meshes read the default (0,0,0,1)
    if (len > 0.0 && abs(wn.z) < 0.5) {
      vec3 glassC = vec3(0.2, 0.26, 0.32);
      float aa = clamp(1.6 - fwidth(u) * 1.2, 0.0, 1.0);
      float pil = 1.0 - step(0.38, u) * step(u, len - 0.38);
      float tower = step(26.0, H);
      float pitch = mix(2.7, 3.7, fh(sdq * 7.0)), floorH = vWall.w;
      float nWin = max(1.0, floor((len - 0.8) / pitch)), margin = (len - nWin * pitch) * 0.5;
      float lu = (u - margin) / pitch, fu = fract(lu), inRow = step(0.0, lu) * step(lu, nWin);
      float gl = 0.0, trimM = 0.0;
      if (z < 4.0 && len > 3.0) {
        float sp = 4.4, ns = max(1.0, floor(len / sp)), sm = (len - ns * sp) * 0.5, su = fract((u - sm) / sp), inS = step(0.0, u - sm) * step(u - sm, ns * sp);
        float win = inS * step(.08, su) * step(su, .92) * step(.35, z) * step(z, 3.0);
        float frame = inS * step(.05, su) * step(su, .95) * step(.25, z) * step(z, 3.1) - win;
        base = mix(base * 0.9, vec3(0.12, 0.12, 0.13), frame * aa); trimM = frame; gl = win * aa;
        float k = fh(sdq * 11.0);
        vec3 awn = k < .25 ? vec3(0.45, 0.09, 0.07) : k < .5 ? vec3(0.08, 0.24, 0.15) : k < .75 ? vec3(0.08, 0.15, 0.32) : vec3(0.5, 0.36, 0.08);
        float aw = step(3.15, z) * step(z, 3.75) * aa; base = mix(base, awn * mix(1.0, 1.15, step(0.5, fract(u * 1.4))), aw);
        base = mix(base, base * 0.78, step(3.75, z) * step(z, 4.0));
      } else if (tower > 0.5) {
        float fz = fract((z - 4.0) / floorH), mull = step(.95, fract(u / 1.6)), spandrel = step(.8, fz);
        gl = (1.0 - spandrel) * (1.0 - mull) * aa * (1.0 - pil); base = mix(base, base * 0.85, spandrel * aa); trimM = mull;
      } else {
        float fz = fract((z - 4.0) / floorH);
        float ww = mix(.38, .6, fh(sdq * 5.0)), wh = mix(.42, .6, fh(sdq * 9.0)), inTop = step(z, H - 1.2);
        float win = inRow * inTop * step(.5 - ww * .5, fu) * step(fu, .5 + ww * .5) * step(.3, fz) * step(fz, .3 + wh);
        float frame = inRow * inTop * step(.5 - ww * .5 - .05, fu) * step(fu, .5 + ww * .5 + .05) * step(.25, fz) * step(fz, .35 + wh) - win;
        float sill = inRow * inTop * step(.5 - ww * .5 - .09, fu) * step(fu, .5 + ww * .5 + .09) * step(.2, fz) * step(fz, .25);
        vec3 frameC = fh(sdq * 17.0) < .5 ? vec3(0.85, 0.84, 0.8) : vec3(0.14, 0.14, 0.15);
        base = mix(base, frameC, frame * aa); base = mix(base, base * 1.12 + 0.04, sill * aa); trimM = frame + sill; gl = win * aa;
        base *= 1.0 - 0.08 * step(.965, fz) * aa;
      }
      base = mix(base, base * 0.86, pil * aa);
      base = mix(base, glassC, gl);
      surfaceGlass = max(surfaceGlass, gl);
      base *= mix(0.72, 1.0, smoothstep(0.0, 2.5, z));  // grime / contact darkening near the ground
      // night: some windows lit (warm), per window and storey
      float lit = step(0.42, fh(floor(lu) * 7.31 + floor((z - 4.0) / floorH) * 17.0 + sdq * 997.0));
      emissive += gl * lit * uNight * vec3(1.0, 0.74, 0.42) * 1.6;
    } else if (len < -1.5) {
      // cornice (roughness 0.7 in the original)
    } else if (len < -0.5 && abs(wn.z) >= 0.5) {  // roofs
      float t = fract(sin(dot(floor(vWorld.xy * 0.7), vec2(12.99, 78.23))) * 43758.5);
      base *= 0.88 + 0.12 * t;
    }
  }
)GLSL";
}

void Facades::bind(const kern::Program& p) { u_night_ = p.location("uNight"); }
void Facades::frame_uniforms(kern::UniformCache& cache, const kern::Renderer&) { cache.set_float(u_night_, night); }

}  // namespace sc
