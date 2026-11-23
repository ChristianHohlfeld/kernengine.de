struct look_cpp : virtual kern::Library, virtual common, virtual engine_gfx_passes, virtual engine_gfx_renderer, virtual engine_math_math, virtual look {
  // ---- variables ----
  KERN_CONST Float kDecayPerS = 1.35;
  KERN_CONST Float kFreq = 19.0;
  KERN_CONST Float kMaxAngle = 0.055;
  KERN_CONST Float kMaxOffset = 0.09;
  KERN_CONST Float kTeleport = 0.35;
  KERN_CONST Float kTargetTeleport = 2.5;
  KERN_CONST Float kPositionHz = 55.0;
  KERN_CONST Float kOrientationHz = 45.0;
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST I32 kCount = 26;
  /// m/s
  KERN_CONST Float kStart = 13.0;
  /// m/s
  KERN_CONST Float kFull = 40.0;
  KERN_CONST Float kDeg = 0.017453292519943295;

  // ---- functions ----
  struct mix3Result { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE mix3Result mix3(Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Float t = 0.0) {
    return {vector_add(a, vector_scale(vector_sub(b, a), t))};
  }

  struct smoothResult { Float Result = 0.0; };
  KERN_PURE smoothResult smooth(Float e0 = 0.0, Float e1 = 0.0, Float x = 0.0) {
    Float t = 0.0;
    t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float e0, Float e1) -> Float { return kern::math::div((x - e0), (e1 - e0)); }, x, e0, e1), 0.0, 1.0));
    return {kern::formula([](Float t) -> Float { return ((t * t) * (3.0 - (2.0 * t))); }, t)};
  }

  struct noiseResult { Float Result = 0.0; };
  KERN_PURE noiseResult noise(Float t = 0.0, Float o = 0.0) {
    return {kern::formula([](Float t, Float o) -> Float { return (((kern::math::sin((t + o)) * 0.5) + (kern::math::sin(((t * 2.31) + (o * 1.7))) * 0.3)) + (kern::math::sin(((t * 4.17) + (o * 2.3))) * 0.2)); }, t, o)};
  }

  struct rotate_aboutResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rotate_aboutResult rotate_about(Vector v = Vector{0.0, 0.0, 0.0}, Vector axis = Vector{0.0, 0.0, 0.0}, Float a = 0.0) {
    Quat a_2 = Quat{0.0, 0.0, 0.0, 1.0};
    a_2 = engine_math_math::Quat_axis_angle(axis, a).Result;
    return {engine_math_math::Quat_rotate(a_2, v).Result};
  }

  void Shake_add(look_Shake& self, Float amount = 0.0, Float cap = 0.0) {
    if (logic_not(math_compare<">">(amount, 0.0))) {
      return;
    }
    self.trauma = kern::formula([](Float trauma, Float cap, Float amount) -> Float { return kern::math::max(trauma, kern::math::min(cap, (trauma + amount))); }, self.trauma, cap, amount);
  }

  struct Shake_sampleResult { Bool Result = false; };
  Shake_sampleResult Shake_sample(look_Shake& self, Vector& angles, Vector& offset, Float dt = 0.0, Float time_s = 0.0, Float scale = 0.0) {
    Float s = 0.0;
    Float t = 0.0;
    angles = Vector{0.0, 0.0, 0.0};
    offset = Vector{0.0, 0.0, 0.0};
    if (math_compare<"<=">(self.trauma, 0.0)) {
      return {false};
    }
    self.trauma = kern::formula([](Float trauma, Float kDecayPerS, Float dt) -> Float { return kern::math::max(0.0, (trauma - (kDecayPerS * (dt < 0.0 ? 0.0 : (0.1 < dt ? 0.1 : dt))))); }, self.trauma, kDecayPerS, dt);
    s = kern::formula([](Float trauma) -> Float { return (trauma * trauma); }, self.trauma);
    if (math_compare<"<">(s, 0.0001)) {
      return {false};
    }
    t = kern::formula([](Float time_s, Float kFreq) -> Float { return (kern::math::mod(time_s, 1000.0) * kFreq); }, time_s, kFreq);
    angles = vector_make(kern::formula([](Float kMaxAngle, Float s, Float scale, Float the_result) -> Float { return (((kMaxAngle * s) * scale) * the_result); }, kMaxAngle, s, scale, noise(t, self.seed).Result), kern::formula([](Float kMaxAngle, Float s, Float scale, Float the_result) -> Float { return (((kMaxAngle * s) * scale) * the_result); }, kMaxAngle, s, scale, noise(t, kern::formula([](Float seed) -> Float { return (seed + 11.0); }, self.seed)).Result), kern::formula([](Float kMaxAngle, Float s, Float scale, Float the_result) -> Float { return ((((kMaxAngle * 0.7) * s) * scale) * the_result); }, kMaxAngle, s, scale, noise(t, kern::formula([](Float seed) -> Float { return (seed + 23.0); }, self.seed)).Result));
    offset = vector_make(kern::formula([](Float kMaxOffset, Float s, Float scale, Float the_result) -> Float { return (((kMaxOffset * s) * scale) * the_result); }, kMaxOffset, s, scale, noise(kern::formula([](Float t) -> Float { return (t * 0.8); }, t), kern::formula([](Float seed) -> Float { return (seed + 37.0); }, self.seed)).Result), kern::formula([](Float kMaxOffset, Float s, Float scale, Float the_result) -> Float { return (((kMaxOffset * s) * scale) * the_result); }, kMaxOffset, s, scale, noise(kern::formula([](Float t) -> Float { return (t * 0.8); }, t), kern::formula([](Float seed) -> Float { return (seed + 51.0); }, self.seed)).Result), 0.0);
    return {true};
  }

  struct weapon_traumaResult { Float Result = 0.0; };
  weapon_traumaResult weapon_trauma(Float& cap, Bool full_auto = false, Bool heavy = false, Bool drone = false) {
    if (full_auto) {
      cap = 0.42;
      return {kern::formula([](Float drone) -> Float { return ((drone != 0.0) ? 0.07 : 0.035); }, kern::to<Float>(drone))};
    }
    cap = 1.0;
    return {kern::formula([](Float heavy) -> Float { return ((heavy != 0.0) ? 0.28 : 0.2); }, kern::to<Float>(heavy))};
  }

  struct blast_traumaResult { Float Result = 0.0; };
  KERN_PURE blast_traumaResult blast_trauma(Float distance = 0.0, Float radius = 0.0) {
    Float radius_ = 0.0;
    Float reach = 0.0;
    Float k = 0.0;
    Float size = 0.0;
    radius_ = radius;
    radius_ = kern::formula([](Float radius_) -> Float { return (radius_ < 1.0 ? 1.0 : (400.0 < radius_ ? 400.0 : radius_)); }, radius_);
    reach = kern::formula([](Float radius_) -> Float { return ((radius_ * 9.0) + 30.0); }, radius_);
    if (math_compare<">">(distance, reach)) {
      return {0.0};
    }
    k = kern::formula([](Float distance, Float reach) -> Float { return kern::math::pow((1.0 - kern::math::div(distance, reach)), 1.6); }, distance, reach);
    size = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float radius_) -> Float { return kern::math::div(radius_, 12.0); }, radius_), 0.25, 0.9));
    return {kern::formula([](Float k, Float size) -> Float { return (k * size); }, k, size)};
  }

  void shake_camera(Camera& c, Vector angles = Vector{0.0, 0.0, 0.0}, Vector offset = Vector{0.0, 0.0, 0.0}) {
    Float dist = 0.0;
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector r = Vector{0.0, 0.0, 0.0};
    Vector u = Vector{0.0, 0.0, 0.0};
    dist = kern::formula([](Float a) -> Float { return kern::math::max(0.5, a); }, vector_length(vector_sub(c.target, c.eye)));
    f = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(c.target, c.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(c.target, c.eye)), Vector{1.0, 0.0, 0.0});
    r = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(f, c.up)), 9.999999717180685e-10), vector_normalize(vector_cross(f, c.up)), Vector{0.0, -1.0, 0.0});
    u = vector_cross(r, f);
    f = rotate_about(f, r, vector_break(angles).x).Result;
    u = rotate_about(u, r, vector_break(angles).x).Result;
    f = rotate_about(f, u, vector_break(angles).y).Result;
    r = rotate_about(r, u, vector_break(angles).y).Result;
    u = rotate_about(u, f, vector_break(angles).z).Result;
    r = rotate_about(r, f, vector_break(angles).z).Result;
    c.eye = vector_add(c.eye, vector_add(vector_scale(r, vector_break(offset).x), vector_scale(u, vector_break(offset).y)));
    c.target = vector_add(c.eye, vector_scale(f, dist));
    c.up = u;
  }

  void Stabilizer_apply(look_Stabilizer& self, Camera& c, Float dt = 0.0) {
    Vector raw_pos = Vector{0.0, 0.0, 0.0};
    Vector raw_target = Vector{0.0, 0.0, 0.0};
    Vector raw_up = Vector{0.0, 0.0, 0.0};
    Bool teleport = false;
    Float pa = 0.0;
    Float oa = 0.0;
    raw_pos = c.eye;
    raw_target = c.target;
    raw_up = c.up;
    teleport = logic_or(logic_or(logic_not(self.init), math_compare<">">(vector_length(vector_sub(self.pos, raw_pos)), kTeleport)), math_compare<">">(vector_length(vector_sub(self.target, raw_target)), kTargetTeleport));
    if (teleport) {
      self.pos = raw_pos;
      self.target = raw_target;
      self.up = logic_select<Vector>(math_compare<">">(vector_length(raw_up), 9.999999717180685e-10), vector_normalize(raw_up), Vector{0.0, 0.0, 1.0});
      self.init = true;
    } else {
      pa = common::damp_alpha(kPositionHz, dt).Result;
      oa = common::damp_alpha(kOrientationHz, dt).Result;
      self.pos = vector_add(self.pos, vector_scale(vector_sub(raw_pos, self.pos), pa));
      self.target = vector_add(self.target, vector_scale(vector_sub(raw_target, self.target), oa));
      self.up = logic_select<Vector>(math_compare<">">(vector_length(vector_add(self.up, vector_scale(vector_sub(raw_up, self.up), oa))), 9.999999717180685e-10), vector_normalize(vector_add(self.up, vector_scale(vector_sub(raw_up, self.up), oa))), Vector{0.0, 0.0, 1.0});
    }
    self.lag = vector_length(vector_sub(self.pos, raw_pos));
    c.eye = self.pos;
    c.target = self.target;
    c.up = self.up;
  }

  void SpeedLines_respawn(look_SpeedLines& self, I32 i = 0) {
    n10 = common::Rng_unit(self.rng);
    self.s[i].a = kern::formula([](Float the_result, Float kPi) -> Float { return ((the_result * 2.0) * kPi); }, n10.Result, kPi);
    n11 = common::Rng_unit(self.rng);
    self.s[i].r = kern::formula([](Float the_result) -> Float { return (0.15 + (the_result * 0.5)); }, n11.Result);
    n12 = common::Rng_unit(self.rng);
    self.s[i].len = kern::formula([](Float the_result) -> Float { return (0.08 + (the_result * 0.22)); }, n12.Result);
    n13 = common::Rng_unit(self.rng);
    self.s[i].v = kern::formula([](Float the_result) -> Float { return (0.9 + (the_result * 1.4)); }, n13.Result);
    self.s[i].warm = math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::mod(i, 3.0); }, kern::to<Float>(i))))), 0.0);
  }

  struct SpeedLines_updateResult { Float Result = 0.0; };
  SpeedLines_updateResult SpeedLines_update(look_SpeedLines& self, Vector eye = Vector{0.0, 0.0, 0.0}, Float dt = 0.0) {
    Float d = 0.0;
    if (logic_not(self.seeded)) {
      for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kCount)))); it14.next();) {
        SpeedLines_respawn(self, n14.index);
      }
      self.seeded = true;
    }
    if (logic_and(logic_and(self.have_last, math_compare<">">(dt, 0.0)), math_compare<"<">(vector_length(vector_sub(eye, self.last)), 25.0))) {
      d = kern::formula([](Float a, Float dt) -> Float { return kern::math::div(a, dt); }, vector_length(vector_sub(eye, self.last)), dt);
      self.speed = kern::formula([](Float speed, Float d, Float dt) -> Float { return (speed + ((kern::math::min(d, 90.0) - speed) * kern::math::min(1.0, (dt * 4.0)))); }, self.speed, d, dt);
    }
    self.last = eye;
    self.have_last = true;
    self.k = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float speed, Float kStart, Float kFull) -> Float { return kern::math::div((speed - kStart), (kFull - kStart)); }, self.speed, kStart, kFull), 0.0, 1.0));
    if (math_compare<"<=">(self.k, 0.0)) {
      return {0.0};
    }
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(kCount)))); it15.next();) {
      self.s[n15.index].r = kern::formula([](Float a, Float a_2, Float dt, Float k) -> Float { return (a + ((a_2 * dt) * (0.6 + (1.8 * k)))); }, self.s[n15.index].r, self.s[n15.index].v, dt, self.k);
      if (math_compare<">">(self.s[n15.index].r, 1.15)) {
        SpeedLines_respawn(self, n15.index);
      }
    }
    return {self.k};
  }

  struct day_lighting_lumResult { Float Result = 0.0; };
  KERN_PURE day_lighting_lumResult day_lighting_lum(Vector c = Vector{0.0, 0.0, 0.0}) {
    return {kern::formula([](Float x, Float y, Float z) -> Float { return (((0.2126 * x) + (0.7152 * y)) + (0.0722 * z)); }, vector_break(c).x, vector_break(c).y, vector_break(c).z)};
  }

  struct day_lightingResult { Float Result = 0.0; };
  day_lightingResult day_lighting(Lighting& l, SkyState& sky, Float tod = 0.0, Float sun_azimuth_deg = 0.0, Float fog_density = 0.0, Float exposure = 0.0) {
    Float angle = 0.0;
    Float az = 0.0;
    Vector across = Vector{0.0, 0.0, 0.0};
    Vector toward = Vector{0.0, 0.0, 0.0};
    Vector sun = Vector{0.0, 0.0, 0.0};
    Vector moon = Vector{0.0, 0.0, 0.0};
    Float night = 0.0;
    Float dusk = 0.0;
    Vector day_sun = Vector{0.0, 0.0, 0.0};
    Vector dusk_sun = Vector{0.0, 0.0, 0.0};
    Vector moon_light = Vector{0.0, 0.0, 0.0};
    Float sky_radiance = 0.0;
    Float irradiance = 0.0;
    Vector bounce = Vector{0.0, 0.0, 0.0};
    angle = kern::formula([](Float tod, Float kPi) -> Float { return (((tod - 0.25) * 2.0) * kPi); }, tod, kPi);
    az = kern::formula([](Float sun_azimuth_deg, Float kDeg) -> Float { return (sun_azimuth_deg * kDeg); }, sun_azimuth_deg, kDeg);
    across = vector_make(kern::formula([](Float az, Float kPi) -> Float { return kern::math::cos((az + (kPi * 0.5))); }, az, kPi), kern::formula([](Float az, Float kPi) -> Float { return kern::math::sin((az + (kPi * 0.5))); }, az, kPi), 0.0);
    toward = vector_make(kern::formula([](Float az) -> Float { return kern::math::cos(az); }, az), kern::formula([](Float az) -> Float { return kern::math::sin(az); }, az), 0.0);
    sun = logic_select<Vector>(math_compare<">">(vector_length(vector_add(vector_add(vector_scale(across, kern::formula([](Float angle) -> Float { return ((-kern::math::cos(angle)) * 0.62); }, angle)), vector_scale(toward, 0.38)), vector_make(0.0, 0.0, kern::formula([](Float angle) -> Float { return (kern::math::sin(angle) + 0.3); }, angle)))), 9.999999717180685e-10), vector_normalize(vector_add(vector_add(vector_scale(across, kern::formula([](Float angle) -> Float { return ((-kern::math::cos(angle)) * 0.62); }, angle)), vector_scale(toward, 0.38)), vector_make(0.0, 0.0, kern::formula([](Float angle) -> Float { return (kern::math::sin(angle) + 0.3); }, angle)))), Vector{0.0, 0.0, 1.0});
    moon = logic_select<Vector>(math_compare<">">(vector_length(vector_make(kern::formula([](Float x) -> Float { return (-x); }, vector_break(sun).x), kern::formula([](Float y) -> Float { return (-y); }, vector_break(sun).y), kern::formula([](Float z) -> Float { return kern::math::max(0.35, (-z)); }, vector_break(sun).z))), 9.999999717180685e-10), vector_normalize(vector_make(kern::formula([](Float x) -> Float { return (-x); }, vector_break(sun).x), kern::formula([](Float y) -> Float { return (-y); }, vector_break(sun).y), kern::formula([](Float z) -> Float { return kern::math::max(0.35, (-z)); }, vector_break(sun).z))), Vector{0.0, 0.0, 1.0});
    night = smooth(0.06, -0.16, vector_break(sun).z).Result;
    dusk = kern::formula([](Float a, Float night) -> Float { return (a * (1.0 - (night * 0.6))); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float z) -> Float { return (1.0 - (kern::math::abs(z) * 4.0)); }, vector_break(sun).z), 0.0, 1.0), night);
    day_sun = Vector{4.6, 4.05, 3.46};
    dusk_sun = Vector{3.75, 2.05, 1.02};
    moon_light = Vector{0.36, 0.42, 0.62};
    l.sun_direction = logic_select<Vector>(math_compare<">">(vector_break(sun).z, -0.04), sun, moon);
    l.sun_color = mix3(mix3(day_sun, dusk_sun, dusk).Result, moon_light, night).Result;
    l.sky_color = vector_scale(Vector{0.07, 0.09, 0.16}, night);
    l.ground_color = vector_scale(Vector{0.025, 0.026, 0.04}, night);
    l.fog_color = mix3(mix3(Vector{0.474, 0.584, 0.716}, Vector{0.62, 0.45, 0.36}, kern::formula([](Float dusk) -> Float { return (dusk * 0.6); }, dusk)).Result, Vector{0.03, 0.04, 0.07}, night).Result;
    l.fog_density = fog_density;
    l.fog_height_falloff = 0.012;
    l.fog_sun_scatter = kern::formula([](Float night) -> Float { return (0.55 * (1.0 - (night * 0.7))); }, night);
    l.fog_horizon = 0.85;
    l.env_intensity = kern::formula([](Float night) -> Float { return (0.53 - (0.33 * night)); }, night);
    l.env_zenith = mix3(Vector{0.05, 0.2, 0.62}, Vector{0.012, 0.022, 0.06}, night).Result;
    l.env_horizon = mix3(Vector{0.52, 0.66, 0.86}, Vector{0.05, 0.075, 0.13}, night).Result;
    sky_radiance = kern::formula([](Float the_result, Float env_intensity) -> Float { return (the_result * env_intensity); }, day_lighting_lum(mix3(l.env_horizon, l.env_zenith, 0.35).Result).Result, l.env_intensity);
    irradiance = kern::formula([](Float the_result, Float z, Float kPi, Float sky_radiance) -> Float { return ((the_result * kern::math::max(z, 0.0)) + (kPi * sky_radiance)); }, day_lighting_lum(l.sun_color).Result, vector_break(sun).z, kPi, sky_radiance);
    bounce = vector_scale(Vector{1.0, 0.95, 0.85}, kern::formula([](Float irradiance, Float kPi, Float env_intensity) -> Float { return kern::math::div(kern::math::div((0.25 * irradiance), kPi), kern::math::max(env_intensity, 0.001)); }, irradiance, kPi, l.env_intensity));
    l.env_ground = mix3(bounce, Vector{0.105, 0.127, 0.084}, night).Result;
    l.exposure = kern::formula([](Float exposure, Float night) -> Float { return (exposure * (1.0 + (0.35 * night))); }, exposure, night);
    sky.sun = sun;
    sky.moon = moon;
    sky.night = night;
    return {night};
  }

  struct follow_phaseResult { Float Result = 0.0; };
  KERN_PURE follow_phaseResult follow_phase(Float mine = 0.0, Float authority = 0.0) {
    Float d = 0.0;
    Float p = 0.0;
    d = kern::formula([](Float authority, Float mine) -> Float { return (authority - mine); }, authority, mine);
    d = kern::formula([](Float d) -> Float { return (d - kern::math::round(d)); }, d);
    if (math_compare<"<">(kern::formula([](Float d) -> Float { return kern::math::abs(d); }, d), 0.0005)) {
      return {mine};
    }
    p = kern::formula([](Float mine, Float d) -> Float { return (mine + ((kern::math::abs(d) < 0.01) ? (d * 0.5) : d)); }, mine, d);
    p = kern::formula([](Float p) -> Float { return (p - kern::math::floor(p)); }, p);
    return {p};
  }

  struct parse_timeResult { Bool Result = false; };
  parse_timeResult parse_time(Float& phase, Text text = "") {
    Int at = 0;
    Int end = 0;
    Float a = 0.0;
    Float m = 0.0;
    at = 0;
    while (loop(math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(text, at)))))), 32.0))) {
      at += 1;
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(text, at)))))), 0.0))) {
      return {false};
    }
    end = 0;
    end = text_parse_float(text, at).end;
    a = text_parse_float(text, at).value;
    if (math_compare<"==">(kern::to<Float>(end), kern::to<Float>(at))) {
      return {false};
    }
    if (math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a >= 128.0 ? a - 256.0 : a; }, kern::to<Float>(text_code_at(text, end)))))), 58.0)) {
      end = text_parse_float(text, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(end)))).end;
      m = text_parse_float(text, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 1.0; }, kern::to<Float>(end)))).value;
      if (logic_or(logic_or(logic_or(math_compare<"<">(a, 0.0), math_compare<">">(a, 24.0)), math_compare<"<">(m, 0.0)), math_compare<">=">(m, 60.0))) {
        return {false};
      }
      phase = kern::formula([](Float a, Float m) -> Float { return kern::math::mod(kern::math::div((a + kern::math::div(m, 60.0)), 24.0), 1.0); }, a, m);
      return {true};
    }
    if (math_compare<"<">(a, 0.0)) {
      return {false};
    }
    phase = kern::formula([](Float a) -> Float { return ((a <= 1.0) ? kern::math::mod(a, 1.0) : kern::math::mod(kern::math::div(a, 24.0), 1.0)); }, a);
    return {true};
  }

  struct Kern_intensityResult { Float Result = 0.0; };
  KERN_PURE Kern_intensityResult Kern_intensity(Float three_intensity = 0.0, Float range = 0.0, Float d_ref = 0.0) {
    Float d2 = 0.0;
    Float r2 = 0.0;
    Float w = 0.0;
    Float att = 0.0;
    d2 = kern::formula([](Float d_ref) -> Float { return kern::math::max((d_ref * d_ref), 0.04); }, d_ref);
    r2 = kern::formula([](Float range) -> Float { return kern::math::max(0.01, (range * range)); }, range);
    w = kern::formula([](Float d2, Float r2) -> Float { return (1.0 - kern::math::div(d2, r2)); }, d2, r2);
    att = kern::formula([](Float w, Float d2, Float r2) -> Float { return ((w > 0.0) ? kern::math::div((w * w), (1.0 + kern::math::div(d2, r2))) : 0.0); }, w, d2, r2);
    if (math_compare<"<=">(att, 0.0)) {
      return {0.0};
    }
    return {kern::formula([](Float three_intensity, Float kPi, Float d2, Float att) -> Float { return kern::math::div(kern::math::div(three_intensity, (kPi * d2)), (0.12 * att)); }, three_intensity, kPi, d2, att)};
  }

  struct hex_linear_linResult { Float Result = 0.0; };
  KERN_PURE hex_linear_linResult hex_linear_lin(Float c = 0.0) {
    return {kern::formula([](Float c) -> Float { return ((c <= 0.04045) ? kern::math::div(c, 12.92) : kern::math::pow(kern::math::div((c + 0.055), 1.055), 2.4)); }, c)};
  }

  struct hex_linearResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE hex_linearResult hex_linear(U32 rgb = 0) {
    return {vector_make(hex_linear_lin(kern::formula([](Float rgb) -> Float { return kern::math::div(kern::math::band(kern::math::shr(rgb, 16.0), 255.0), 255.0); }, kern::to<Float>(rgb))).Result, hex_linear_lin(kern::formula([](Float rgb) -> Float { return kern::math::div(kern::math::band(kern::math::shr(rgb, 8.0), 255.0), 255.0); }, kern::to<Float>(rgb))).Result, hex_linear_lin(kern::formula([](Float rgb) -> Float { return kern::math::div(kern::math::band(rgb, 255.0), 255.0); }, kern::to<Float>(rgb))).Result)};
  }

  struct police_flashResult { I32 Result = 0; };
  KERN_PURE police_flashResult police_flash(Float now_s = 0.0) {
    Int64 slot = Int64{0x0};
    slot = int64_rem(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float now_s) -> Float { return kern::math::floor(kern::math::div((now_s * 1000.0), 110.0)); }, now_s))), Int64{0x6});
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return (((a != 0.0) || (a_2 != 0.0)) ? 1.0 : (((a_3 != 0.0) || (a_4 != 0.0)) ? 2.0 : 0.0)); }, kern::to<Float>(int64_equal(slot, Int64{0x0})), kern::to<Float>(int64_equal(slot, Int64{0x2})), kern::to<Float>(int64_equal(slot, Int64{0x3})), kern::to<Float>(int64_equal(slot, Int64{0x5}))))};
  }

  struct drone_flashResult { I32 Result = 0; };
  KERN_PURE drone_flashResult drone_flash(Float now_s = 0.0, I32 index = 0) {
    Int64 slot = Int64{0x0};
    slot = int64_rem(int64_add(int64_rem(int64_of_float(kern::formula([](Float x) -> Float { return kern::math::trunc(x); }, kern::formula([](Float now_s, Float index) -> Float { return kern::math::floor(kern::math::div(((now_s * 1000.0) + (index * 37.0)), 92.0)); }, now_s, kern::to<Float>(index)))), Int64{0x8}), Int64{0x8}), Int64{0x8});
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((a != 0.0) ? 1.0 : (((a_2 != 0.0) || (a_3 != 0.0)) ? 2.0 : 0.0)); }, kern::to<Float>(logic_not(int64_less(Int64{0x1}, slot))), kern::to<Float>(int64_equal(slot, Int64{0x4})), kern::to<Float>(int64_equal(slot, Int64{0x5}))))};
  }

  // ---- what nodes give and keep ----
  common::Rng_unitResult n10;
  common::Rng_unitResult n11;
  common::Rng_unitResult n12;
  common::Rng_unitResult n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
};
