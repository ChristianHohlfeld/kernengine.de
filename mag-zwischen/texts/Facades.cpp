struct Facades : virtual kern::Blueprint, virtual engine_gfx_shader, kern::Implements<kern::ShaderExtension> {
  // ---- variables ----
  /// 0 day .. 1 night (game_look.cpp: set_night)
  Float night = 0.0;
  U32 u_night_ = 0;
  /// The same vertex stream Kern's BuildingBuilder fills (key "wall", 4 floats).
  KERN_CONST Fixed<ExtensionAttribute, 1> kWall = List<ExtensionAttribute>{ExtensionAttribute{"wall", 4, "aWall"}};

  // ---- functions ----
  struct nameResult { Text Result = ""; };
  KERN_OVERRIDE KERN_PURE nameResult name() {
    return {"spawncity-facades"};
  }

  void set_night(Float n = 0.0) {
    night = n;
  }

  struct attributesResult { List<ExtensionAttribute> Result = List<ExtensionAttribute>{}; };
  KERN_OVERRIDE KERN_PURE attributesResult attributes() {
    return {kWall};
  }

  struct vertex_declarationsResult { Text Result = ""; };
  KERN_OVERRIDE KERN_PURE vertex_declarationsResult vertex_declarations() {
    return {"flat out vec4 vWall;\n"};
  }

  struct vertexResult { Text Result = ""; };
  KERN_OVERRIDE KERN_PURE vertexResult vertex() {
    return {"  vWall = aWall;\n"};
  }

  struct fragment_declarationsResult { Text Result = ""; };
  KERN_OVERRIDE KERN_PURE fragment_declarationsResult fragment_declarations() {
    return {"\n// flat: constant per wall (world_city_buildings.mjs snaps the seed for the same reason)\nflat in vec4 vWall;\nuniform float uNight;\nfloat fh(float n) { return fract(sin(n * 127.1) * 43758.5453); }\n"};
  }

  struct fragment_surfaceResult { Text Result = ""; };
  KERN_OVERRIDE KERN_PURE fragment_surfaceResult fragment_surface() {
    return {"\n  {\n    float sdq = floor(vWall.z * 997.0 + 0.5) / 997.0;\n    vec3 wn = normalize(cross(dFdx(vWorld), dFdy(vWorld)));\n    float u = vUV.x, len = vWall.x, z = vUV.y, H = vWall.y;\n    // only Kern's building walls carry a record (len > 0); other meshes read the default (0,0,0,1)\n    if (len > 0.0 && abs(wn.z) < 0.5) {\n      vec3 glassC = vec3(0.2, 0.26, 0.32);\n      float aa = clamp(1.6 - fwidth(u) * 1.2, 0.0, 1.0);\n      float pil = 1.0 - step(0.38, u) * step(u, len - 0.38);\n      float tower = step(26.0, H);\n      float pitch = mix(2.7, 3.7, fh(sdq * 7.0)), floorH = vWall.w;\n      float nWin = max(1.0, floor((len - 0.8) / pitch)), margin = (len - nWin * pitch) * 0.5;\n      float lu = (u - margin) / pitch, fu = fract(lu), inRow = step(0.0, lu) * step(lu, nWin);\n      float gl = 0.0, trimM = 0.0;\n      if (z < 4.0 && len > 3.0) {\n        float sp = 4.4, ns = max(1.0, floor(len / sp)), sm = (len - ns * sp) * 0.5, su = fract((u - sm) / sp), inS = step(0.0, u - sm) * step(u - sm, ns * sp);\n        float win = inS * step(.08, su) * step(su, .92) * step(.35, z) * step(z, 3.0);\n        float frame = inS * step(.05, su) * step(su, .95) * step(.25, z) * step(z, 3.1) - win;\n        base = mix(base * 0.9, vec3(0.12, 0.12, 0.13), frame * aa); trimM = frame; gl = win * aa;\n        float k = fh(sdq * 11.0);\n        vec3 awn = k < .25 ? vec3(0.45, 0.09, 0.07) : k < .5 ? vec3(0.08, 0.24, 0.15) : k < .75 ? vec3(0.08, 0.15, 0.32) : vec3(0.5, 0.36, 0.08);\n        float aw = step(3.15, z) * step(z, 3.75) * aa; base = mix(base, awn * mix(1.0, 1.15, step(0.5, fract(u * 1.4))), aw);\n        base = mix(base, base * 0.78, step(3.75, z) * step(z, 4.0));\n      } else if (tower > 0.5) {\n        float fz = fract((z - 4.0) / floorH), mull = step(.95, fract(u / 1.6)), spandrel = step(.8, fz);\n        gl = (1.0 - spandrel) * (1.0 - mull) * aa * (1.0 - pil); base = mix(base, base * 0.85, spandrel * aa); trimM = mull;\n      } else {\n        float fz = fract((z - 4.0) / floorH);\n        float ww = mix(.38, .6, fh(sdq * 5.0)), wh = mix(.42, .6, fh(sdq * 9.0)), inTop = step(z, H - 1.2);\n        float win = inRow * inTop * step(.5 - ww * .5, fu) * step(fu, .5 + ww * .5) * step(.3, fz) * step(fz, .3 + wh);\n        float frame = inRow * inTop * step(.5 - ww * .5 - .05, fu) * step(fu, .5 + ww * .5 + .05) * step(.25, fz) * step(fz, .35 + wh) - win;\n        float sill = inRow * inTop * step(.5 - ww * .5 - .09, fu) * step(fu, .5 + ww * .5 + .09) * step(.2, fz) * step(fz, .25);\n        vec3 frameC = fh(sdq * 17.0) < .5 ? vec3(0.85, 0.84, 0.8) : vec3(0.14, 0.14, 0.15);\n        base = mix(base, frameC, frame * aa); base = mix(base, base * 1.12 + 0.04, sill * aa); trimM = frame + sill; gl = win * aa;\n        base *= 1.0 - 0.08 * step(.965, fz) * aa;\n      }\n      base = mix(base, base * 0.86, pil * aa);\n      base = mix(base, glassC, gl);\n      surfaceGlass = max(surfaceGlass, gl);\n      base *= mix(0.72, 1.0, smoothstep(0.0, 2.5, z));  // grime / contact darkening near the ground\n      // night: some windows lit (warm), per window and storey\n      float lit = step(0.42, fh(floor(lu) * 7.31 + floor((z - 4.0) / floorH) * 17.0 + sdq * 997.0));\n      emissive += gl * lit * uNight * vec3(1.0, 0.74, 0.42) * 1.6;\n    } else if (len < -1.5) {\n      // cornice (roughness 0.7 in the original)\n    } else if (len < -0.5 && abs(wn.z) >= 0.5) {  // roofs\n      float t = fract(sin(dot(floor(vWorld.xy * 0.7), vec2(12.99, 78.23))) * 43758.5);\n      base *= 0.88 + 0.12 * t;\n    }\n  }\n"};
  }

  KERN_OVERRIDE void bind(Int p = 0) {
    Int location = 0;
    location = engine_program_location(p, "uNight");
    u_night_ = location;
  }

  KERN_OVERRIDE void frame_uniforms(Int cache = 0, Int arg2 = 0) {
    Int cache_ = 0;
    cache_ = cache;
    engine_uniform_cache_set_float(cache_, u_night_, night);
  }
};
