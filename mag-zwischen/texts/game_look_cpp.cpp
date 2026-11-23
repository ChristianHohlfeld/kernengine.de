struct game_look_cpp : virtual kern::Library, virtual engine_geo_mesh_data, virtual engine_math_math, virtual weapons {
  // ---- variables ----
  KERN_CONST Float kPi = 3.141592653589793;
  /// Glow colours: linear HDR (above 1 the bloom picks them up), like the MeshBasicMaterial glows.
  KERN_CONST Vector kGlowOrange = Vector{3.0, 0.9, 0.18};
  /// Glow colours: linear HDR (above 1 the bloom picks them up), like the MeshBasicMaterial glows.
  KERN_CONST Vector kGlowRed = Vector{3.2, 0.12, 0.12};
  /// Glow colours: linear HDR (above 1 the bloom picks them up), like the MeshBasicMaterial glows.
  KERN_CONST Vector kGlowAmber = Vector{3.0, 1.25, 0.1};

  // ---- functions ----
  struct clock_msResult { Float Result = 0.0; };
  KERN_PURE clock_msResult clock_ms() {
    Float now_ms = 0.0;
    now_ms = engine_now_ms();
    return {now_ms};
  }

  struct srgbResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE srgbResult srgb(U32 hex = 0) {
    return {vector_make(kern::formula([](Float hex) -> Float { return kern::math::div(kern::math::band(kern::math::shr(hex, 16.0), 255.0), 255.0); }, kern::to<Float>(hex)), kern::formula([](Float hex) -> Float { return kern::math::div(kern::math::band(kern::math::shr(hex, 8.0), 255.0), 255.0); }, kern::to<Float>(hex)), kern::formula([](Float hex) -> Float { return kern::math::div(kern::math::band(hex, 255.0), 255.0); }, kern::to<Float>(hex)))};
  }

  /// Model builders in SpawnCity's view-model frame (weapon_models.mjs): x right, y up, barrel along -z.
  void box(MeshData& m, const Mat4& extra, Vector size = Vector{0.0, 0.0, 0.0}, U32 color = 0, Vector at = Vector{0.0, 0.0, 0.0}) {
    MeshData box_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    box_2 = engine_primitives_box(size, srgb(color).Result);
    a = engine_math_math::Mat4_translate(at).Result;
    engine_primitives_append(n10, m, box_2, engine_math_math::Mat4_mul(extra, a).Result);
    m = n10.dst;
  }

  /// three's CylinderGeometry turned onto the z axis (rotation.x = pi/2), centred at `at`.
  void cyl(MeshData& m, const Mat4& extra, Float r = 0.0, Float len = 0.0, U32 color = 0, Vector at = Vector{0.0, 0.0, 0.0}, U32 seg = 0) {
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    cylinder = engine_primitives_cylinder(r, len, seg, srgb(color).Result);
    a = engine_math_math::Mat4_translate(vector_make(vector_break(at).x, vector_break(at).y, kern::formula([](Float z, Float len) -> Float { return (z - (len * 0.5)); }, vector_break(at).z, len))).Result;
    engine_primitives_append(n11, m, cylinder, engine_math_math::Mat4_mul(extra, a).Result);
    m = n11.dst;
  }

  void glow_box(MeshData& m, Vector size = Vector{0.0, 0.0, 0.0}, Vector hdr = Vector{0.0, 0.0, 0.0}, Vector at = Vector{0.0, 0.0, 0.0}) {
    MeshData box_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    box_2 = engine_primitives_box(size, hdr);
    engine_primitives_append(n12, m, box_2, engine_math_math::Mat4_translate(at).Result);
    m = n12.dst;
  }

  void glow_cyl(MeshData& m, Float r = 0.0, Float len = 0.0, Vector hdr = Vector{0.0, 0.0, 0.0}, Vector at = Vector{0.0, 0.0, 0.0}) {
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    cylinder = engine_primitives_cylinder(r, len, 14, hdr);
    engine_primitives_append(n13, m, cylinder, engine_math_math::Mat4_translate(vector_make(vector_break(at).x, vector_break(at).y, kern::formula([](Float z, Float len) -> Float { return (z - (len * 0.5)); }, vector_break(at).z, len))).Result);
    m = n13.dst;
  }

  /// A gloved forearm and sleeve reaching back from a grip at `grip` (viewmodel_arms.mjs). `arms` false: the VR set (the controller is the hand: no forearm, no sleeve).
  void arm(MeshData& m, Vector grip = Vector{0.0, 0.0, 0.0}, Float side = 0.0, Bool arms = false) {
    Mat4 tilt = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_not(arms)) {
      return;
    }
    a = engine_math_math::Mat4_translate(grip).Result;
    a_2 = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(Vector{1.0, 0.0, 0.0}, -0.12).Result, Vector{1.0, 1.0, 1.0}).Result;
    tilt = engine_math_math::Mat4_mul(a, a_2).Result;
    box(m, tilt, Vector{0.075, 0.08, 0.1}, 1909548, Vector{0.0, 0.0, 0.0});
    cyl(m, tilt, 0.04, 0.3, 12946534, vector_make(kern::formula([](Float side) -> Float { return (side * 0.012); }, side), -0.03, 0.2), 10);
    cyl(m, tilt, 0.06, 0.16, 2896960, vector_make(kern::formula([](Float side) -> Float { return (side * 0.02); }, side), -0.05, 0.39), 10);
    cyl(m, tilt, 0.063, 0.03, 1909548, vector_make(kern::formula([](Float side) -> Float { return (side * 0.016); }, side), -0.043, 0.32), 10);
  }

  /// BOOMSTICK 40 mm (weapon_models.mjs buildBoomstick): drum launcher with hazard stripes.
  void boomstick(MeshData& m, MeshData& g, Bool arms = false) {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float a_6 = 0.0;
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector arg = Vector{0.0, 0.0, 0.0};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a, Vector{0.17, 0.125, 0.32}, 4872762, Vector{0.0, 0.012, -0.1});
    a_2 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_2, Vector{0.1, 0.2, 0.1}, 9067067, Vector{0.0, -0.155, -0.095});
    a_3 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_3, Vector{0.13, 0.12, 0.24}, 1908772, Vector{0.0, -0.02, 0.18});
    a_4 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_4, 0.11, 0.2, 4872762, Vector{0.0, -0.005, -0.36}, 10);
    for (auto it14 = flow_for(n14, 0, 3); it14.next();) {
      a_5 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
      cyl(m, a_5, 0.113, 0.03, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(((kern::math::mod(index, 2.0) != 0.0) ? 1184534.0 : 16765503.0)); }, kern::to<Float>(n14.index))), vector_make(0.0, -0.005, kern::formula([](Float index) -> Float { return ((-0.29) - (index * 0.045)); }, kern::to<Float>(n14.index))), 10);
    }
    for (auto it15 = flow_for(n15, 0, 5); it15.next();) {
      a_6 = kern::formula([](Float index, Float kPi) -> Float { return ((kern::math::div(index, 6.0) * 2.0) * kPi); }, kern::to<Float>(n15.index), kPi);
      arg = vector_make(kern::formula([](Float a_6) -> Float { return (kern::math::cos(a_6) * 0.07); }, a_6), kern::formula([](Float a_6) -> Float { return ((-0.005) + (kern::math::sin(a_6) * 0.07)); }, a_6), -0.36);
      a_7 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
      cyl(m, a_7, 0.026, 0.205, 1908772, arg, 8);
    }
    a_8 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_8, 0.058, 0.36, 1908772, Vector{0.0, 0.03, -0.62}, 14);
    a_9 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_9, 0.068, 0.06, 16742938, Vector{0.0, 0.03, -0.81}, 14);
    a_10 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_10, Vector{0.05, 0.03, 0.26}, 1184534, Vector{0.0, 0.13, -0.2});
    a_11 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_11, Vector{0.04, 0.06, 0.03}, 1184534, Vector{0.0, 0.1, -0.08});
    a_12 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_12, Vector{0.04, 0.06, 0.03}, 1184534, Vector{0.0, 0.1, -0.32});
    glow_cyl(g, 0.064, 0.012, kGlowOrange, Vector{0.0, 0.03, -0.842});
    glow_box(g, Vector{0.02, 0.04, 0.02}, kGlowOrange, Vector{0.0, 0.16, -0.3});
    arm(m, Vector{0.0, -0.15, -0.095}, 1.0, arms);
  }

  /// ROCKET LAUNCHER (Kern, after buildRocketLauncher): a compact olive shoulder tube - flared venturi behind, a shroud and the dark bore in front, two bands, an optic on top with its lens, the laser module with the red lens beside the tube, pistol and fore grip. Shorter than before (0.95 m instead of 1.13 m) and held lower and further right: it frames the view, not fills it.
  void rocket_launcher(MeshData& m, MeshData& g, Bool arms = false) {
    U32 olive = 0;
    U32 dark = 0;
    U32 the_band = 0;
    U32 steel = 0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float item = 0.0;
    Float item_2 = 0.0;
    Fixed<Float, 2> items = Fixed<Float, 2>{};
    Float z = 0.0;
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_13 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_14 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_15 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_16 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    olive = 4082224;
    dark = 1382427;
    the_band = 2501918;
    steel = 8225931;
    a = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a, 0.058, 0.76, olive, Vector{0.0, 0.03, -0.27}, 20);
    a_2 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_2, 0.071, 0.08, dark, Vector{0.0, 0.03, -0.67}, 20);
    a_3 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_3, 0.05, 0.012, 197379, Vector{0.0, 0.03, -0.712}, 16);
    item = -0.12;
    item_2 = -0.52;
    items = list_make<Float, "2">(item, item_2);
    for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it16.next();) {
      z = items[n16.index];
      a_4 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
      cyl(m, a_4, 0.061, 0.035, the_band, vector_make(0.0, 0.03, z), 20);
    }
    a_5 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_5, 0.066, 0.07, dark, Vector{0.0, 0.03, 0.13}, 20);
    a_6 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_6, 0.08, 0.05, dark, Vector{0.0, 0.03, 0.19}, 20);
    a_7 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_7, 0.068, 0.012, 197379, Vector{0.0, 0.03, 0.216}, 16);
    a_8 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_8, Vector{0.03, 0.03, 0.08}, dark, Vector{0.0, 0.098, -0.22});
    a_9 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_9, 0.026, 0.17, dark, Vector{0.0, 0.135, -0.23}, 14);
    a_10 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_10, 0.03, 0.03, 2304045, Vector{0.0, 0.135, -0.33}, 14);
    a_11 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_11, 0.022, 0.006, 1849938, Vector{0.0, 0.135, -0.348}, 14);
    a_12 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_12, Vector{0.05, 0.055, 0.15}, 2830392, Vector{-0.085, 0.03, -0.47});
    a_13 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_13, 0.018, 0.02, steel, Vector{-0.085, 0.03, -0.555}, 12);
    glow_cyl(g, 0.012, 0.01, kGlowRed, Vector{-0.085, 0.03, -0.566});
    glow_box(g, Vector{0.012, 0.012, 0.012}, kGlowRed, Vector{-0.112, 0.045, -0.43});
    glow_box(g, Vector{0.012, 0.012, 0.012}, kGlowAmber, Vector{-0.112, 0.045, -0.455});
    a_14 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_14, Vector{0.05, 0.15, 0.07}, dark, Vector{0.0, -0.1, -0.1});
    a_15 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_15, Vector{0.012, 0.045, 0.09}, dark, Vector{0.0, -0.035, -0.135});
    a_16 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_16, Vector{0.048, 0.11, 0.06}, dark, Vector{0.0, -0.075, -0.4});
    arm(m, Vector{0.0, -0.13, -0.105}, 1.0, arms);
  }

  /// FISTS (buildFists): one taped fist with forearm and sleeve; the left one is its mirror.
  void fist(MeshData& m, Bool arms = false) {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 back = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a, Vector{0.076, 0.078, 0.1}, 12946534, Vector{0.0, 0.002, -0.004});
    a_2 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_2, Vector{0.09, 0.05, 0.04}, 12946534, Vector{0.0, 0.012, -0.045});
    a_3 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_3, 0.0405, 0.018, 15328472, Vector{0.0, 0.002, 0.016}, 10);
    a_4 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    box(m, a_4, Vector{0.03, 0.03, 0.06}, 11104591, Vector{-0.022, -0.03, -0.03});
    a_5 = struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0});
    cyl(m, a_5, 0.038, 0.05, 15328472, Vector{0.0, -0.002, 0.066}, 10);
    if (logic_not(arms)) {
      return;
    }
    back = engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(Vector{1.0, 0.0, 0.0}, -0.12).Result, Vector{1.0, 1.0, 1.0}).Result;
    cyl(m, back, 0.043, 0.3, 12946534, Vector{0.012, -0.03, 0.24}, 10);
    cyl(m, back, 0.06, 0.16, 2896960, Vector{0.02, -0.05, 0.43}, 10);
    cyl(m, back, 0.062, 0.03, 1909548, Vector{0.016, -0.043, 0.36}, 10);
  }

  /// KNIFE (buildKnife): clip-point blade with fuller and guard in the right fist, blade forward-up.
  void knife(MeshData& m, MeshData& g, Bool arms = false) {
    Float s = 0.0;
    Mat4 grip = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Quat a = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_2 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_3 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_4 = Quat{0.0, 0.0, 0.0, 1.0};
    fist(m, arms);
    s = 1.566;
    a_2 = engine_math_math::Quat_axis_angle(Vector{1.0, 0.0, 0.0}, 0.32).Result;
    a_3 = engine_math_math::Quat_axis_angle(Vector{0.0, 1.0, 0.0}, 0.18).Result;
    a = engine_math_math::Quat_mul(a_2, a_3).Result;
    a_4 = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, -0.5).Result;
    grip = engine_math_math::Mat4_trs(Vector{0.0, 0.01, -0.02}, engine_math_math::Quat_mul(a, a_4).Result, vector_make(s, s, s)).Result;
    box(m, grip, Vector{0.0054, 0.032, 0.2}, 7239035, Vector{0.0, 0.002, -0.15});
    box(m, grip, Vector{0.006, 0.006, 0.17}, 14212578, Vector{0.0, -0.014, -0.14});
    box(m, grip, Vector{0.0062, 0.006, 0.1}, 1513756, Vector{0.0, 0.006, -0.105});
    box(m, grip, Vector{0.016, 0.05, 0.012}, 1513756, Vector{0.0, 0.0, -0.046});
    cyl(m, grip, 0.0135, 0.095, 1711134, Vector{0.0, 0.0, 0.0}, 10);
    cyl(m, grip, 0.014, 0.0125, 2764081, Vector{0.0, 0.0, 0.054}, 10);
  }

  struct vm_materialResult { Material Result = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}; };
  KERN_PURE vm_materialResult vm_material(Bool glow = false, Bool vr = false) {
    Material mt = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    mt = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    mt.viewmodel = logic_not(vr);
    mt.cast_shadow = false;
    mt.frustum_culled = false;
    mt.roughness = kern::formula([](Float glow) -> Float { return ((glow != 0.0) ? 1.0 : 0.55); }, kern::to<Float>(glow));
    mt.metalness = kern::formula([](Float glow) -> Float { return ((glow != 0.0) ? 0.0 : 0.25); }, kern::to<Float>(glow));
    mt.unlit = glow;
    mt.linear_colors = glow;
    mt.double_sided = true;
    return {mt};
  }

  struct vm_indexResult { I32 Result = 0; };
  KERN_PURE vm_indexResult vm_index(weapons_Id id = weapons_Id::smg) {
    switch (flow_switch_enum<weapons_Id>(id)) {
      case weapons_Id::smg: {
        return {-1};
      } break;
      case weapons_Id::glock: {
        return {-1};
      } break;
      case weapons_Id::sniper: {
        return {-1};
      } break;
      case weapons_Id::grenade_launcher: {
        return {0};
      } break;
      case weapons_Id::rocket: {
        return {1};
      } break;
      case weapons_Id::fists: {
        return {2};
      } break;
      case weapons_Id::knife: {
        return {3};
      } break;
      case weapons_Id::hand_grenade: {
        return {-1};
      } break;
      case weapons_Id::drone_gun: {
        return {-1};
      } break;
      case weapons_Id::drone_rockets: {
        return {-1};
      } break;
      case weapons_Id::drone_guided: {
        return {-1};
      } break;
      case weapons_Id::jet_cannon: {
        return {-1};
      } break;
      case weapons_Id::jet_missile: {
        return {-1};
      } break;
      case weapons_Id::jet_bomb: {
        return {-1};
      } break;
    }
  }

  struct camera_frameResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  /// Camera space (x right, y up, z back) -> world.
  KERN_PURE camera_frameResult camera_frame(const Camera& c) {
    Vector f = Vector{0.0, 0.0, 0.0};
    Vector r = Vector{0.0, 0.0, 0.0};
    Vector u = Vector{0.0, 0.0, 0.0};
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    f = logic_select<Vector>(math_compare<">">(vector_length(vector_sub(c.target, c.eye)), 9.999999717180685e-10), vector_normalize(vector_sub(c.target, c.eye)), Vector{1.0, 0.0, 0.0});
    r = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(f, c.up)), 9.999999717180685e-10), vector_normalize(vector_cross(f, c.up)), Vector{0.0, -1.0, 0.0});
    u = vector_cross(r, f);
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    m.m[0] = vector_break(r).x;
    m.m[1] = vector_break(r).y;
    m.m[2] = vector_break(r).z;
    m.m[3] = 0.0;
    m.m[4] = vector_break(u).x;
    m.m[5] = vector_break(u).y;
    m.m[6] = vector_break(u).z;
    m.m[7] = 0.0;
    m.m[8] = kern::formula([](Float x) -> Float { return (-x); }, vector_break(f).x);
    m.m[9] = kern::formula([](Float y) -> Float { return (-y); }, vector_break(f).y);
    m.m[10] = kern::formula([](Float z) -> Float { return (-z); }, vector_break(f).z);
    m.m[11] = 0.0;
    m.m[12] = vector_break(c.eye).x;
    m.m[13] = vector_break(c.eye).y;
    m.m[14] = vector_break(c.eye).z;
    m.m[15] = 1.0;
    return {m};
  }

  struct eulerResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE eulerResult euler(Float x = 0.0, Float y = 0.0, Float z = 0.0) {
    Quat a = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_2 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_3 = Quat{0.0, 0.0, 0.0, 1.0};
    Quat a_4 = Quat{0.0, 0.0, 0.0, 1.0};
    a_2 = engine_math_math::Quat_axis_angle(Vector{1.0, 0.0, 0.0}, x).Result;
    a_3 = engine_math_math::Quat_axis_angle(Vector{0.0, 1.0, 0.0}, y).Result;
    a = engine_math_math::Quat_mul(a_2, a_3).Result;
    a_4 = engine_math_math::Quat_axis_angle(Vector{0.0, 0.0, 1.0}, z).Result;
    return {engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_mul(a, a_4).Result, Vector{1.0, 1.0, 1.0}).Result};
  }

  // ---- what nodes give and keep ----
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n10;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n11;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n12;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
};
