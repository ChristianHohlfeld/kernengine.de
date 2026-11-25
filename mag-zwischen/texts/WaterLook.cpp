struct WaterLook : virtual kern::Blueprint, virtual engine_gfx_shader, kern::Implements<"ShaderExtension"> {
  // ---- variables ----
  /// seconds (uTime; game_look.cpp: set_time)
  Float time = 0.0;
  U32 u_time_ = 0;
  KERN_CONST Fixed<ExtensionAttribute, 1> kWater = List<ExtensionAttribute>{ExtensionAttribute{"water", 3, "aWater"}};

  // ---- functions ----
  struct nameResult { Text Result = ""; };
  KERN_PURE nameResult name() {
    return {"spawncity-water"};
  }

  void set_time(Float t = 0.0) {
    time = t;
  }

  struct attributesResult { List<ExtensionAttribute> Result = List<ExtensionAttribute>{}; };
  KERN_PURE attributesResult attributes() {
    return {kWater};
  }

  struct vertex_declarationsResult { Text Result = ""; };
  KERN_PURE vertex_declarationsResult vertex_declarations() {
    return {"out vec3 vWater;\n"};
  }

  struct vertexResult { Text Result = ""; };
  KERN_PURE vertexResult vertex() {
    return {"  vWater = aWater;\n"};
  }

  struct fragment_declarationsResult { Text Result = ""; };
  KERN_PURE fragment_declarationsResult fragment_declarations() {
    return {"\nin vec3 vWater;\nuniform float uWaterTime;\nvec2 waveGrad(vec2 p, vec2 dir, float k, float sp, float a) { float ph = dot(p, dir) * k - uWaterTime * sp; return dir * cos(ph) * k * a; }\nfloat wh(vec2 p) { return fract(sin(dot(floor(p), vec2(41.3, 289.1))) * 43758.5); }\nfloat wn(vec2 p) { vec2 i = floor(p), f = fract(p); f = f * f * (3.0 - 2.0 * f); return mix(mix(wh(i), wh(i + vec2(1, 0)), f.x), mix(wh(i + vec2(0, 1)), wh(i + vec2(1, 1)), f.x), f.y); }\n"};
  }

  struct fragment_surfaceResult { Text Result = ""; };
  KERN_PURE fragment_surfaceResult fragment_surface() {
    return {"\n  if (vWater.x > 0.5) {\n    float shore = vWater.x - 1.0;\n    vec2 flow = vWater.yz;\n    float fl = length(flow);\n    vec2 fd = fl > 0.01 ? flow / fl : normalize(vec2(0.8, 0.6));\n    vec2 side = vec2(-fd.y, fd.x);\n    vec2 adv = vWorld.xy - flow * uWaterTime * 2.2;\n    float wDepth = clamp(shore / 9.0, 0.0, 1.0);\n    base = mix(vec3(0.08, 0.43, 0.62), vec3(0.025, 0.20, 0.36), wDepth);\n    float foam = smoothstep(0.55, 0.95, wn(adv * 0.6 + uWaterTime * 0.15)) * (1.0 - smoothstep(0.0, 2.4, shore))\n               + smoothstep(0.75, 1.0, wn(adv * 1.3)) * smoothstep(0.6, 1.6, fl) * 0.6;\n    base = mix(base, vec3(0.9, 0.94, 0.95), clamp(foam, 0.0, 0.85));\n    alpha = mix(0.55, 0.94, smoothstep(0.0, 3.0, shore));\n    vec2 g = waveGrad(adv, fd, 0.9, 2.1, 0.05) + waveGrad(adv, normalize(fd + side * 0.6), 1.7, 3.0, 0.03)\n           + waveGrad(adv, normalize(side - fd * 0.3), 2.9, 4.2, 0.018) + waveGrad(vWorld.xy, normalize(vec2(0.6, -0.8)), 5.3, 6.0, 0.008);\n    n = normalize(vec3(-g.x, -g.y, 1.0));\n  }\n"};
  }

  void bind(Int p = 0) {
    Int location = 0;
    location = engine_program_location(p, "uWaterTime");
    u_time_ = location;
  }

  void frame_uniforms(Int cache = 0, Int arg2 = 0) {
    Int cache_ = 0;
    cache_ = cache;
    engine_uniform_cache_set_float(cache_, u_time_, time);
  }
};
