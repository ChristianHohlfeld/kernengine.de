struct life_art_cpp : virtual kern::Library, virtual engine_math_math, virtual life, virtual life_art, virtual life_cpp {
  // ---- variables ----
  /// vertex locations 14 and 15
  KERN_CONST U32 kMaxExtensionAttributes = 2;
  KERN_CONST Float kPi = 3.141592653589793;

  // ---- functions ----
  void LifeArt_put(LifeArt_Part& p, const Mat4& m, Vector tint = Vector{0.0, 0.0, 0.0}) {
    Int place = 0;
    Int place_2 = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    place = list_length<Float>(p.transforms);
    list_insert_list(p.transforms, place, m.m);
    place_2 = list_length<Float>(p.tints);
    item = vector_break(tint).x;
    item_2 = vector_break(tint).y;
    item_3 = vector_break(tint).z;
    item_4 = 1.0;
    list_insert_list(p.tints, place_2, list_make<Float, "4">(item, item_2, item_3, item_4));
  }

  void LifeArt_box(LifeArt& self, const Mat4& m, Vector tint = Vector{0.0, 0.0, 0.0}) {
    LifeArt_put(self.box_, m, tint);
  }

  void LifeArt_ball(LifeArt& self, const Mat4& m, Vector tint = Vector{0.0, 0.0, 0.0}) {
    LifeArt_put(self.ball_, m, tint);
  }

  void LifeArt_cylinder(LifeArt& self, const Mat4& m, Vector tint = Vector{0.0, 0.0, 0.0}) {
    LifeArt_put(self.cyl_, m, tint);
  }

  void LifeArt_glow(LifeArt& self, const Mat4& m, Vector tint = Vector{0.0, 0.0, 0.0}) {
    LifeArt_put(self.glow_, m, tint);
  }

  struct rotResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE rotResult rot(Vector axis = Vector{0.0, 0.0, 0.0}, Float a = 0.0) {
    return {engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(axis, a).Result, Vector{1.0, 1.0, 1.0}).Result};
  }

  struct atResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE atResult at(Vector p = Vector{0.0, 0.0, 0.0}) {
    return {engine_math_math::Mat4_translate(p).Result};
  }

  struct sc3Result { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE sc3Result sc3(Vector s = Vector{0.0, 0.0, 0.0}) {
    return {engine_math_math::Mat4_scale(s).Result};
  }

  struct rgbResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE rgbResult rgb(U32 h = 0) {
    return {vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(h)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(h)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(h)))};
  }

  struct LifeArt_makeResult { Bool Result = false; };
  LifeArt_makeResult LifeArt_make(LifeArt_Part& p, const MeshData& mesh, const Material& m, Int r = 0) {
    Int r_ = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool upsert = false;
    r_ = r;
    engine_renderer_allocate_id(n10, r_);
    allocate_id = n10.result;
    p.id = allocate_id;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    engine_upsert(n11, r_, p.id, mesh, look, parked.m, List<Float>{});
    upsert = n11.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, p.id);
      p.id = 0;
      return {false};
    }
    return {true};
  }

  struct LifeArt_initResult { Bool Result = false; };
  LifeArt_initResult LifeArt_init(LifeArt& self, Int r = 0) {
    Int r_ = 0;
    Vector white = Vector{0.0, 0.0, 0.0};
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData a_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData a_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material g = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    r_ = r;
    self.r_ = r_;
    white = Vector{1.0, 1.0, 1.0};
    m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    m.roughness = 0.85;
    box = engine_primitives_box(Vector{1.0, 1.0, 1.0}, white);
    a = box;
    n12 = LifeArt_make(self.box_, a, m, r_);
    if (logic_not(n12.Result)) {
      return {false};
    }
    sphere = engine_primitives_sphere(0.5, 10, 7, white);
    a_2 = sphere;
    n13 = LifeArt_make(self.ball_, a_2, m, r_);
    if (logic_not(n13.Result)) {
      return {false};
    }
    cylinder = engine_primitives_cylinder(0.5, 1.0, 8, white);
    a_3 = cylinder;
    n14 = LifeArt_make(self.cyl_, a_3, m, r_);
    if (logic_not(n14.Result)) {
      return {false};
    }
    g = m;
    g.emissive = Vector{2.6, 2.4, 2.0};
    g.unlit = true;
    g.cast_shadow = false;
    box_2 = engine_primitives_box(Vector{1.0, 1.0, 1.0}, white);
    a_4 = box_2;
    n15 = LifeArt_make(self.glow_, a_4, g, r_);
    return {n15.Result};
  }

  void begin_empty(LifeArt_Part& p) {
    list_clear(p.transforms);
    list_clear(p.tints);
  }

  void LifeArt_begin(LifeArt& self) {
    begin_empty(self.box_);
    begin_empty(self.ball_);
    begin_empty(self.cyl_);
    begin_empty(self.glow_);
  }

  void LifeArt_cow_part(LifeArt& self, const Mat4& M, Vector size = Vector{0.0, 0.0, 0.0}, Vector p = Vector{0.0, 0.0, 0.0}, Vector tint = Vector{0.0, 0.0, 0.0}) {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a_3 = at(p).Result;
    a_2 = engine_math_math::Mat4_mul(M, a_3).Result;
    a_4 = sc3(size).Result;
    a = engine_math_math::Mat4_mul(a_2, a_4).Result;
    LifeArt_box(self, a, tint);
  }

  void LifeArt_cow(LifeArt& self, const Life_Cow& c, Float now = 0.0) {
    Mat4 M = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float bob = 0.0;
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float ft = 0.0;
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector coat = Vector{0.0, 0.0, 0.0};
    Vector dark = Vector{0.0, 0.0, 0.0};
    Bool graze = false;
    Float pitch = 0.0;
    Mat4 H = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
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
    Mat4 a_17 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_18 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_19 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_20 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_21 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_22 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_23 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_24 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float amp = 0.0;
    Float hx = 0.0;
    Float hy = 0.0;
    Float sw = 0.0;
    Mat4 L = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_25 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_26 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_27 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_28 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_29 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_30 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_31 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_32 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_33 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_34 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_35 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    M = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (logic_or(c.dead, c.carried)) {
      a = engine_math_math::Mat4_trs(c.pos, c.rot, Vector{1.0, 1.0, 1.0}).Result;
      a_2 = at(Vector{0.0, 0.0, -1.12}).Result;
      M = engine_math_math::Mat4_mul(a, a_2).Result;
    } else {
      bob = kern::formula([](Float walking, Float now, Float flee_until, Float phase) -> Float { return (((walking != 0.0) && (now < flee_until)) ? (kern::math::abs(kern::math::sin(phase)) * 0.05) : 0.0); }, kern::to<Float>(c.walking), now, c.flee_until, c.phase);
      a_3 = at(vector_add(c.pos, vector_make(0.0, 0.0, bob))).Result;
      a_4 = rot(Vector{0.0, 0.0, 1.0}, c.yaw).Result;
      M = engine_math_math::Mat4_mul(a_3, a_4).Result;
      ft = kern::formula([](Float now, Float flinch_at) -> Float { return kern::math::div((now - flinch_at), 0.26); }, now, c.flinch_at);
      if (math_compare<"<">(ft, 1.0)) {
        a_5 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float ft, Float kPi) -> Float { return (kern::math::sin((ft * kPi)) * 0.12); }, ft, kPi)).Result;
        M = engine_math_math::Mat4_mul(M, a_5).Result;
      }
    }
    coat = c.color;
    dark = rgb(1709844).Result;
    LifeArt_cow_part(self, M, Vector{1.62, 0.66, 0.7}, Vector{0.0, 0.0, 1.15}, coat);
    LifeArt_cow_part(self, M, Vector{0.5, 0.672, 0.4}, Vector{-0.25, 0.0, 1.28}, dark);
    LifeArt_cow_part(self, M, Vector{0.36, 0.672, 0.3}, Vector{0.42, 0.0, 1.08}, dark);
    LifeArt_cow_part(self, M, Vector{0.3, 0.3, 0.16}, Vector{-0.35, 0.0, 0.74}, rgb(14260890).Result);
    LifeArt_cow_part(self, M, Vector{0.05, 0.05, 0.6}, Vector{-0.83, 0.0, 1.05}, coat);
    LifeArt_cow_part(self, M, Vector{0.3, 0.5, 0.12}, Vector{0.72, 0.0, 1.5}, coat);
    graze = logic_and(logic_not(c.dead), logic_not(c.walking));
    pitch = kern::formula([](Float dead, Float graze, Float now, Float phase) -> Float { return ((dead != 0.0) ? 0.2 : ((graze != 0.0) ? (0.95 + (0.08 * kern::math::sin((kern::math::div(now, 0.6) + phase)))) : 0.12)); }, kern::to<Float>(c.dead), kern::to<Float>(graze), now, c.phase);
    a_7 = at(Vector{0.86, 0.0, 1.36}).Result;
    a_6 = engine_math_math::Mat4_mul(M, a_7).Result;
    a_8 = rot(Vector{0.0, 1.0, 0.0}, pitch).Result;
    H = engine_math_math::Mat4_mul(a_6, a_8).Result;
    a_11 = at(Vector{0.2, 0.0, 0.0}).Result;
    a_10 = engine_math_math::Mat4_mul(H, a_11).Result;
    a_12 = sc3(Vector{0.42, 0.32, 0.34}).Result;
    a_9 = engine_math_math::Mat4_mul(a_10, a_12).Result;
    LifeArt_box(self, a_9, coat);
    a_15 = at(Vector{0.46, 0.0, -0.07}).Result;
    a_14 = engine_math_math::Mat4_mul(H, a_15).Result;
    a_16 = sc3(Vector{0.16, 0.3, 0.2}).Result;
    a_13 = engine_math_math::Mat4_mul(a_14, a_16).Result;
    LifeArt_box(self, a_13, rgb(14854050).Result);
    a_19 = at(Vector{0.08, 0.15, 0.18}).Result;
    a_18 = engine_math_math::Mat4_mul(H, a_19).Result;
    a_20 = sc3(Vector{0.04, 0.04, 0.12}).Result;
    a_17 = engine_math_math::Mat4_mul(a_18, a_20).Result;
    LifeArt_box(self, a_17, rgb(15327951).Result);
    a_23 = at(Vector{0.08, -0.15, 0.18}).Result;
    a_22 = engine_math_math::Mat4_mul(H, a_23).Result;
    a_24 = sc3(Vector{0.04, 0.04, 0.12}).Result;
    a_21 = engine_math_math::Mat4_mul(a_22, a_24).Result;
    LifeArt_box(self, a_21, rgb(15327951).Result);
    amp = kern::formula([](Float dead, Float now, Float flee_until, Float walking) -> Float { return ((dead != 0.0) ? 0.0 : ((now < flee_until) ? 0.6 : ((walking != 0.0) ? 0.32 : 0.0))); }, kern::to<Float>(c.dead), now, c.flee_until, kern::to<Float>(c.walking));
    for (auto it16 = flow_for(n16, 0, 3); it16.next();) {
      hx = kern::formula([](Float index) -> Float { return ((index < 2.0) ? 0.6 : (-0.58)); }, kern::to<Float>(n16.index));
      hy = kern::formula([](Float index) -> Float { return ((kern::math::mod(index, 2.0) != 0.0) ? 0.2 : (-0.2)); }, kern::to<Float>(n16.index));
      sw = kern::formula([](Float amp, Float phase, Float index, Float kPi) -> Float { return (amp * kern::math::sin((phase + (((index == 0.0) || (index == 3.0)) ? 0.0 : kPi)))); }, amp, c.phase, kern::to<Float>(n16.index), kPi);
      a_26 = at(vector_make(hx, hy, 0.8)).Result;
      a_25 = engine_math_math::Mat4_mul(M, a_26).Result;
      a_27 = rot(Vector{0.0, 1.0, 0.0}, sw).Result;
      L = engine_math_math::Mat4_mul(a_25, a_27).Result;
      a_30 = at(Vector{0.0, 0.0, -0.35}).Result;
      a_29 = engine_math_math::Mat4_mul(L, a_30).Result;
      a_31 = sc3(Vector{0.14, 0.14, 0.7}).Result;
      a_28 = engine_math_math::Mat4_mul(a_29, a_31).Result;
      LifeArt_box(self, a_28, coat);
      a_34 = at(Vector{0.0, 0.0, -0.75}).Result;
      a_33 = engine_math_math::Mat4_mul(L, a_34).Result;
      a_35 = sc3(Vector{0.15, 0.15, 0.1}).Result;
      a_32 = engine_math_math::Mat4_mul(a_33, a_35).Result;
      LifeArt_box(self, a_32, rgb(2762018).Result);
    }
  }

  struct life_nearResult { Bool Result = false; };
  KERN_PURE life_nearResult life_near(Vector eye = Vector{0.0, 0.0, 0.0}, Float range = 0.0, Vector p = Vector{0.0, 0.0, 0.0}) {
    return {logic_and(math_compare<"<">(kern::formula([](Float x, Float x_2) -> Float { return kern::math::abs((x - x_2)); }, vector_break(p).x, vector_break(eye).x), range), math_compare<"<">(kern::formula([](Float y, Float y_2) -> Float { return kern::math::abs((y - y_2)); }, vector_break(p).y, vector_break(eye).y), range))};
  }

  void LifeArt_life(LifeArt& self, const Life& life, Float now = 0.0, Vector eye = Vector{0.0, 0.0, 0.0}, Float range = 0.0) {
    Bool skip = false;
    Vector grey = Vector{0.0, 0.0, 0.0};
    Vector head = Vector{0.0, 0.0, 0.0};
    Vector breast = Vector{0.0, 0.0, 0.0};
    Vector wing = Vector{0.0, 0.0, 0.0};
    Bool skip_2 = false;
    Bool moving = false;
    Float nod = 0.0;
    Mat4 M = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 chosen = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_13 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_14 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_15 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_16 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_17 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_18 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_19 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_20 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_21 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_22 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_23 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_24 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_25 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_26 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_27 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_28 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_29 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_30 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float flap = 0.0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Fixed<Float, 2> items = Fixed<Float, 2>{};
    Float s = 0.0;
    Mat4 a_31 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_32 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_33 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_34 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_35 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_36 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_37 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_38 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip_3 = false;
    Mat4 M_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_39 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_40 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_41 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_42 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_43 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_44 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_45 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_46 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_47 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_48 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_49 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_50 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_51 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_52 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_53 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_54 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_55 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_56 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float flap_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    Fixed<Float, 2> items_2 = Fixed<Float, 2>{};
    Float s_2 = 0.0;
    Mat4 a_57 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_58 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_59 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_60 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_61 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_62 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_63 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_64 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip_4 = false;
    Float s_3 = 0.0;
    Vector col = Vector{0.0, 0.0, 0.0};
    Mat4 M_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_65 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_66 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_67 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_68 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_69 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_70 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float flap_3 = 0.0;
    Float item_5 = 0.0;
    Float item_6 = 0.0;
    Fixed<Float, 2> items_3 = Fixed<Float, 2>{};
    Float sd = 0.0;
    Mat4 a_71 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_72 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_73 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_74 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_75 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_76 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip_5 = false;
    Life_Pet p = Life_Pet{20.0, 0.05, 0.0, 0.2, 2.0, 1.0, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, -1.0, false, false, false, 0.0, Vector{0.0, 0.0, 0.0}, 0, Vector{0.0, 0.0, 0.0}, physics_Body{Int64{0x0}}, Quat{0.0, 0.0, 0.0, 1.0}, Quat{0.0, 0.0, 0.0, 1.0}};
    Bool dead = false;
    Bool dog = false;
    Float sc = 0.0;
    Mat4 M_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_77 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_78 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_79 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_80 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_81 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_82 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_83 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 chosen_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Life_CatAttack cat = Life_CatAttack{false, 0, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0, -9.0, 0.0, 0.0, 0};
    Mat4 a_84 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float bob = 0.0;
    Float torso = 0.0;
    Mat4 a_85 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_86 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_87 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_88 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_89 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_90 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_91 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_92 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_93 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_94 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_95 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_96 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_97 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_98 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_99 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_100 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 H = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_101 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_102 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_103 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_104 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_105 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_106 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_107 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float item_7 = 0.0;
    Float item_8 = 0.0;
    Fixed<Float, 2> items_4 = Fixed<Float, 2>{};
    Float s_4 = 0.0;
    Mat4 a_108 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_109 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_110 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_111 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float amp = 0.0;
    Fixed<Float, 4> offs = Fixed<Float, 4>{};
    Fixed<Float_2_row, 4> hips = Fixed<Float_2_row, 4>{};
    Float sw = 0.0;
    Mat4 a_112 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_113 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_114 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_115 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_116 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_117 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_118 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_119 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip_6 = false;
    Float s_5 = 0.0;
    Mat4 base = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float u = 0.0;
    Float ang = 0.0;
    Mat4 a_120 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_121 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_122 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_123 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_124 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_125 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_126 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_127 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_128 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Cow>(life.cows_))))); it17.next();) {
      skip = false;
      if (logic_not(life_near(eye, range, life.cows_[n17.index].pos).Result)) {
        skip = true;
      }
      if (logic_not(skip)) {
        LifeArt_cow(self, life.cows_[n17.index], now);
      }
    }
    grey = rgb(9278368).Result;
    head = rgb(6120821).Result;
    breast = rgb(8220556).Result;
    wing = rgb(8357266).Result;
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Flock>(life.flocks_))))); it18.next();) {
      for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Pigeon>(life.flocks_[n18.index].birds))))); it19.next();) {
        skip_2 = false;
        if (logic_not(life_near(eye, range, life.flocks_[n18.index].birds[n19.index].pos).Result)) {
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          moving = math_compare<">">(kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(life.flocks_[n18.index].birds[n19.index].v).x, vector_break(life.flocks_[n18.index].birds[n19.index].v).y), 0.05);
          nod = kern::formula([](Float a, Float moving, Float a_2) -> Float { return ((a != 0.0) ? (-0.25) : ((moving != 0.0) ? (kern::math::sin(a_2) * 0.12) : ((kern::math::sin((a_2 * 0.7)) > 0.85) ? 0.55 : 0.0))); }, kern::to<Float>(life.flocks_[n18.index].air), kern::to<Float>(moving), life.flocks_[n18.index].birds[n19.index].peck);
          if (math_compare<">=">(life.flocks_[n18.index].birds[n19.index].dead_at, 0.0)) {
            a_4 = at(vector_add(life.flocks_[n18.index].birds[n19.index].pos, Vector{0.0, 0.0, 0.06})).Result;
            a_5 = rot(Vector{0.0, 0.0, 1.0}, life.flocks_[n18.index].birds[n19.index].yaw).Result;
            a_3 = engine_math_math::Mat4_mul(a_4, a_5).Result;
            a_6 = rot(Vector{1.0, 0.0, 0.0}, 1.45).Result;
            chosen = engine_math_math::Mat4_mul(a_3, a_6).Result;
          } else {
            a_8 = at(life.flocks_[n18.index].birds[n19.index].pos).Result;
            a_9 = rot(Vector{0.0, 0.0, 1.0}, life.flocks_[n18.index].birds[n19.index].yaw).Result;
            a_7 = engine_math_math::Mat4_mul(a_8, a_9).Result;
            a_10 = rot(Vector{0.0, 1.0, 0.0}, nod).Result;
            chosen = engine_math_math::Mat4_mul(a_7, a_10).Result;
          }
          M = chosen;
          a_13 = at(Vector{0.0, 0.0, 0.13}).Result;
          a_12 = engine_math_math::Mat4_mul(M, a_13).Result;
          a_14 = sc3(Vector{0.3, 0.19, 0.18}).Result;
          a_11 = engine_math_math::Mat4_mul(a_12, a_14).Result;
          LifeArt_ball(self, a_11, grey);
          a_17 = at(Vector{0.07, 0.0, 0.15}).Result;
          a_16 = engine_math_math::Mat4_mul(M, a_17).Result;
          a_18 = sc3(Vector{0.14, 0.14, 0.14}).Result;
          a_15 = engine_math_math::Mat4_mul(a_16, a_18).Result;
          LifeArt_ball(self, a_15, breast);
          a_21 = at(Vector{0.15, 0.0, 0.24}).Result;
          a_20 = engine_math_math::Mat4_mul(M, a_21).Result;
          a_22 = sc3(Vector{0.1, 0.1, 0.1}).Result;
          a_19 = engine_math_math::Mat4_mul(a_20, a_22).Result;
          LifeArt_ball(self, a_19, head);
          a_25 = at(Vector{0.205, 0.0, 0.235}).Result;
          a_24 = engine_math_math::Mat4_mul(M, a_25).Result;
          a_26 = sc3(Vector{0.04, 0.015, 0.015}).Result;
          a_23 = engine_math_math::Mat4_mul(a_24, a_26).Result;
          LifeArt_box(self, a_23, rgb(3814192).Result);
          a_29 = at(Vector{-0.17, 0.0, 0.12}).Result;
          a_28 = engine_math_math::Mat4_mul(M, a_29).Result;
          a_30 = sc3(Vector{0.13, 0.09, 0.018}).Result;
          a_27 = engine_math_math::Mat4_mul(a_28, a_30).Result;
          LifeArt_box(self, a_27, rgb(5199202).Result);
          if (logic_and(life.flocks_[n18.index].air, math_compare<"<">(life.flocks_[n18.index].birds[n19.index].dead_at, 0.0))) {
            flap = kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.9); }, life.flocks_[n18.index].birds[n19.index].flap);
            item = -1.0;
            item_2 = 1.0;
            items = list_make<Float, "2">(item, item_2);
            for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it20.next();) {
              s = items[n20.index];
              a_35 = at(vector_make(0.0, kern::formula([](Float s) -> Float { return (s * 0.02); }, s), 0.16)).Result;
              a_34 = engine_math_math::Mat4_mul(M, a_35).Result;
              a_36 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float s, Float flap) -> Float { return (s * flap); }, s, flap)).Result;
              a_33 = engine_math_math::Mat4_mul(a_34, a_36).Result;
              a_37 = at(vector_make(0.0, kern::formula([](Float s) -> Float { return (s * 0.18); }, s), 0.0)).Result;
              a_32 = engine_math_math::Mat4_mul(a_33, a_37).Result;
              a_38 = sc3(Vector{0.16, 0.36, 0.01}).Result;
              a_31 = engine_math_math::Mat4_mul(a_32, a_38).Result;
              LifeArt_box(self, a_31, wing);
            }
          }
        }
      }
    }
    for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_LooseBird>(life.loose_))))); it21.next();) {
      skip_3 = false;
      if (logic_not(life_near(eye, range, life.loose_[n21.index].pos).Result)) {
        skip_3 = true;
      }
      if (logic_not(skip_3)) {
        a_39 = at(vector_sub(life.loose_[n21.index].pos, Vector{0.0, 0.0, 0.15})).Result;
        a_40 = rot(Vector{0.0, 0.0, 1.0}, life.loose_[n21.index].yaw).Result;
        M_2 = engine_math_math::Mat4_mul(a_39, a_40).Result;
        a_43 = at(Vector{0.0, 0.0, 0.13}).Result;
        a_42 = engine_math_math::Mat4_mul(M_2, a_43).Result;
        a_44 = sc3(Vector{0.3, 0.19, 0.18}).Result;
        a_41 = engine_math_math::Mat4_mul(a_42, a_44).Result;
        LifeArt_ball(self, a_41, grey);
        a_47 = at(Vector{0.07, 0.0, 0.15}).Result;
        a_46 = engine_math_math::Mat4_mul(M_2, a_47).Result;
        a_48 = sc3(Vector{0.14, 0.14, 0.14}).Result;
        a_45 = engine_math_math::Mat4_mul(a_46, a_48).Result;
        LifeArt_ball(self, a_45, breast);
        a_51 = at(Vector{0.15, 0.0, 0.24}).Result;
        a_50 = engine_math_math::Mat4_mul(M_2, a_51).Result;
        a_52 = sc3(Vector{0.1, 0.1, 0.1}).Result;
        a_49 = engine_math_math::Mat4_mul(a_50, a_52).Result;
        LifeArt_ball(self, a_49, head);
        a_55 = at(Vector{0.205, 0.0, 0.235}).Result;
        a_54 = engine_math_math::Mat4_mul(M_2, a_55).Result;
        a_56 = sc3(Vector{0.04, 0.015, 0.015}).Result;
        a_53 = engine_math_math::Mat4_mul(a_54, a_56).Result;
        LifeArt_box(self, a_53, rgb(3814192).Result);
        flap_2 = kern::formula([](Float a) -> Float { return (kern::math::sin(a) * 0.9); }, life.loose_[n21.index].flap);
        item_3 = -1.0;
        item_4 = 1.0;
        items_2 = list_make<Float, "2">(item_3, item_4);
        for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items_2))))); it22.next();) {
          s_2 = items_2[n22.index];
          a_61 = at(vector_make(0.0, kern::formula([](Float s_2) -> Float { return (s_2 * 0.02); }, s_2), 0.16)).Result;
          a_60 = engine_math_math::Mat4_mul(M_2, a_61).Result;
          a_62 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float s_2, Float flap_2) -> Float { return (s_2 * flap_2); }, s_2, flap_2)).Result;
          a_59 = engine_math_math::Mat4_mul(a_60, a_62).Result;
          a_63 = at(vector_make(0.0, kern::formula([](Float s_2) -> Float { return (s_2 * 0.18); }, s_2), 0.0)).Result;
          a_58 = engine_math_math::Mat4_mul(a_59, a_63).Result;
          a_64 = sc3(Vector{0.16, 0.36, 0.01}).Result;
          a_57 = engine_math_math::Mat4_mul(a_58, a_64).Result;
          LifeArt_box(self, a_57, wing);
        }
      }
    }
    for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Bird>(life.birds_))))); it23.next();) {
      skip_4 = false;
      if (logic_and(math_compare<"<">(now, life.birds_[n23.index].dead_until), math_compare<">=">(now, life.birds_[n23.index].dying_until))) {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        if (logic_not(life_near(eye, range, life.birds_[n23.index].pos).Result)) {
          skip_4 = true;
        }
        if (logic_not(skip_4)) {
          s_3 = kern::formula([](Float a) -> Float { return (0.48 + kern::math::div(kern::math::mod(a, 14.0), 100.0)); }, kern::to<Float>(life.birds_[n23.index].seed));
          col = rgb(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(3094850.0 + kern::math::mul32(kern::math::mod(a, 5.0), 1052688.0)); }, kern::to<Float>(life.birds_[n23.index].seed)))).Result;
          a_66 = at(life.birds_[n23.index].pos).Result;
          a_67 = rot(Vector{0.0, 0.0, 1.0}, life.birds_[n23.index].yaw).Result;
          a_65 = engine_math_math::Mat4_mul(a_66, a_67).Result;
          a_68 = sc3(vector_make(s_3, s_3, s_3)).Result;
          M_3 = engine_math_math::Mat4_mul(a_65, a_68).Result;
          a_70 = sc3(Vector{0.6, 0.12, 0.08}).Result;
          a_69 = engine_math_math::Mat4_mul(M_3, a_70).Result;
          LifeArt_box(self, a_69, col);
          flap_3 = kern::formula([](Float now, Float a, Float a_2) -> Float { return ((now < a) ? 0.2 : ((a_2 - 0.82) * 3.0)); }, now, life.birds_[n23.index].dying_until, life.birds_[n23.index].flap);
          item_5 = -1.0;
          item_6 = 1.0;
          items_3 = list_make<Float, "2">(item_5, item_6);
          for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items_3))))); it24.next();) {
            sd = items_3[n24.index];
            a_74 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float sd, Float flap_3) -> Float { return (sd * flap_3); }, sd, flap_3)).Result;
            a_73 = engine_math_math::Mat4_mul(M_3, a_74).Result;
            a_75 = at(vector_make(-0.08, kern::formula([](Float sd) -> Float { return (sd * 0.42); }, sd), 0.0)).Result;
            a_72 = engine_math_math::Mat4_mul(a_73, a_75).Result;
            a_76 = sc3(Vector{0.32, 0.78, 0.03}).Result;
            a_71 = engine_math_math::Mat4_mul(a_72, a_76).Result;
            LifeArt_box(self, a_71, col);
          }
        }
      }
    }
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Life_Pet>(life.pets_))))); it25.next();) {
      skip_5 = false;
      p = life.pets_[n25.index];
      if (logic_or(logic_not(p.shown), logic_not(life_near(eye, range, p.pos).Result))) {
        skip_5 = true;
      }
      if (logic_not(skip_5)) {
        dead = math_compare<">=">(p.dead_at, 0.0);
        if (logic_and(dead, math_compare<">">(kern::formula([](Float now, Float dead_at) -> Float { return (now - dead_at); }, now, p.dead_at), 7.0))) {
          skip_5 = true;
        }
        if (logic_not(skip_5)) {
          dog = enum_equal<Life_Species>(life_cpp::Life_species(life, kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n25.index))), life::Life_epoch(life).Result).Result, Life_Species::dog);
          sc = kern::formula([](Float dog, Float scale) -> Float { return ((dog != 0.0) ? scale : (scale * 0.62)); }, kern::to<Float>(dog), p.scale);
          if (logic_or(dead, p.carried)) {
            a_78 = engine_math_math::Mat4_trs(p.pos, p.rot, vector_make(sc, sc, sc)).Result;
            a_79 = at(Vector{0.0, 0.0, -0.27}).Result;
            chosen_2 = engine_math_math::Mat4_mul(a_78, a_79).Result;
          } else {
            a_81 = at(p.pos).Result;
            a_82 = rot(Vector{0.0, 0.0, 1.0}, p.yaw).Result;
            a_80 = engine_math_math::Mat4_mul(a_81, a_82).Result;
            a_83 = sc3(vector_make(sc, sc, sc)).Result;
            chosen_2 = engine_math_math::Mat4_mul(a_80, a_83).Result;
          }
          M_4 = chosen_2;
          cat = life.cat_;
          if (logic_and(logic_and(logic_and(logic_not(dead), math_compare<"==">(kern::to<Float>(n25.index), 1.0)), cat.on), math_compare<"==">(kern::to<Float>(cat.phase), 1.0))) {
            a_84 = rot(Vector{0.0, 1.0, 0.0}, -0.5).Result;
            M_4 = engine_math_math::Mat4_mul(M_4, a_84).Result;
          }
          bob = kern::formula([](Float dead, Float gait, Float kPi, Float speed) -> Float { return ((dead != 0.0) ? 0.0 : (kern::math::abs(kern::math::sin(((gait * kPi) * 2.0))) * kern::math::min(0.03, (speed * 0.01)))); }, kern::to<Float>(dead), p.gait, kPi, p.speed);
          torso = kern::formula([](Float bob) -> Float { return ((0.25 + 0.11) + bob); }, bob);
          a_87 = at(vector_make(0.0, 0.0, torso)).Result;
          a_86 = engine_math_math::Mat4_mul(M_4, a_87).Result;
          a_88 = sc3(Vector{0.66, 0.26, 0.27}).Result;
          a_85 = engine_math_math::Mat4_mul(a_86, a_88).Result;
          LifeArt_box(self, a_85, p.color);
          a_92 = at(vector_make(-0.44, 0.0, kern::formula([](Float torso) -> Float { return (torso + 0.1); }, torso))).Result;
          a_91 = engine_math_math::Mat4_mul(M_4, a_92).Result;
          a_93 = rot(Vector{0.0, 1.0, 0.0}, -0.6).Result;
          a_90 = engine_math_math::Mat4_mul(a_91, a_93).Result;
          a_94 = sc3(Vector{0.3, 0.055, 0.055}).Result;
          a_89 = engine_math_math::Mat4_mul(a_90, a_94).Result;
          LifeArt_box(self, a_89, p.color);
          a_98 = at(vector_make(0.3, 0.0, kern::formula([](Float torso) -> Float { return (torso + 0.12); }, torso))).Result;
          a_97 = engine_math_math::Mat4_mul(M_4, a_98).Result;
          a_99 = rot(Vector{0.0, 1.0, 0.0}, 0.5).Result;
          a_96 = engine_math_math::Mat4_mul(a_97, a_99).Result;
          a_100 = sc3(Vector{0.16, 0.16, 0.2}).Result;
          a_95 = engine_math_math::Mat4_mul(a_96, a_100).Result;
          LifeArt_box(self, a_95, p.color);
          a_101 = at(vector_make(0.43, 0.0, kern::formula([](Float torso) -> Float { return (torso + 0.16); }, torso))).Result;
          H = engine_math_math::Mat4_mul(M_4, a_101).Result;
          a_103 = sc3(Vector{0.3, 0.3, 0.3}).Result;
          a_102 = engine_math_math::Mat4_mul(H, a_103).Result;
          LifeArt_ball(self, a_102, p.color);
          a_106 = at(Vector{0.13, 0.0, -0.04}).Result;
          a_105 = engine_math_math::Mat4_mul(H, a_106).Result;
          a_107 = sc3(Vector{0.15, 0.1, 0.09}).Result;
          a_104 = engine_math_math::Mat4_mul(a_105, a_107).Result;
          LifeArt_box(self, a_104, p.color);
          item_7 = -1.0;
          item_8 = 1.0;
          items_4 = list_make<Float, "2">(item_7, item_8);
          for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items_4))))); it26.next();) {
            s_4 = items_4[n26.index];
            a_110 = at(vector_make(-0.02, kern::formula([](Float s_4) -> Float { return (s_4 * 0.08); }, s_4), 0.14)).Result;
            a_109 = engine_math_math::Mat4_mul(H, a_110).Result;
            a_111 = sc3(Vector{0.06, 0.05, 0.11}).Result;
            a_108 = engine_math_math::Mat4_mul(a_109, a_111).Result;
            LifeArt_box(self, a_108, p.color);
          }
          amp = kern::formula([](Float dead, Float speed) -> Float { return ((dead != 0.0) ? 0.0 : kern::math::min(0.7, (speed * 0.35))); }, kern::to<Float>(dead), p.speed);
          offs = list_make<Float, "4">(0.25, 0.75, 0.0, 0.5);
          hips = list_make<Float_2_row, "4">(struct_make<Float_2_row>(list_make<Float, "2">(0.24, 0.09)), struct_make<Float_2_row>(list_make<Float, "2">(0.24, -0.09)), struct_make<Float_2_row>(list_make<Float, "2">(-0.24, 0.09)), struct_make<Float_2_row>(list_make<Float, "2">(-0.24, -0.09)));
          for (auto it27 = flow_for(n27, 0, 3); it27.next();) {
            sw = kern::formula([](Float dead, Float index, Float amp, Float gait, Float a, Float kPi) -> Float { return ((dead != 0.0) ? (0.35 * ((index < 2.0) ? 1.0 : (-1.0))) : (amp * kern::math::sin((((gait + a) * kPi) * 2.0)))); }, kern::to<Float>(dead), kern::to<Float>(n27.index), amp, p.gait, offs[n27.index], kPi);
            a_116 = at(vector_make(hips[n27.index].items[0], hips[n27.index].items[1], kern::formula([](Float torso) -> Float { return (torso - 0.1); }, torso))).Result;
            a_115 = engine_math_math::Mat4_mul(M_4, a_116).Result;
            a_117 = rot(Vector{0.0, 1.0, 0.0}, sw).Result;
            a_114 = engine_math_math::Mat4_mul(a_115, a_117).Result;
            a_118 = at(Vector{0.0, 0.0, -0.135}).Result;
            a_113 = engine_math_math::Mat4_mul(a_114, a_118).Result;
            a_119 = sc3(Vector{0.075, 0.075, 0.27}).Result;
            a_112 = engine_math_math::Mat4_mul(a_113, a_119).Result;
            LifeArt_box(self, a_112, p.color);
          }
        }
      }
    }
    for (auto it28 = flow_for(n28, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Life_Tree>(life.trees_))))); it28.next();) {
      skip_6 = false;
      if (logic_not(life_near(eye, range, life.trees_[n28.index].foot).Result)) {
        skip_6 = true;
      }
      if (logic_not(skip_6)) {
        s_5 = life.trees_[n28.index].scale;
        base = at(life.trees_[n28.index].foot).Result;
        if (math_compare<">=">(life.trees_[n28.index].fell_at, 0.0)) {
          u = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float now, Float a) -> Float { return kern::math::div((now - a), 1.1); }, now, life.trees_[n28.index].fell_at), 0.0, 1.0));
          ang = kern::formula([](Float u) -> Float { return ((1.5 * u) * u); }, u);
          a_120 = rot(logic_select<Vector>(math_compare<">">(vector_length(vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(life.trees_[n28.index].fall_dir).y), vector_break(life.trees_[n28.index].fall_dir).x, 0.0)), 9.999999717180685e-10), vector_normalize(vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(life.trees_[n28.index].fall_dir).y), vector_break(life.trees_[n28.index].fall_dir).x, 0.0)), Vector{1.0, 0.0, 0.0}), ang).Result;
          base = engine_math_math::Mat4_mul(base, a_120).Result;
        }
        a_123 = at(Vector{0.0, 0.0, -0.4}).Result;
        a_122 = engine_math_math::Mat4_mul(base, a_123).Result;
        a_124 = sc3(vector_make(kern::formula([](Float s_5) -> Float { return (0.25 * s_5); }, s_5), kern::formula([](Float s_5) -> Float { return (0.25 * s_5); }, s_5), kern::formula([](Float s_5) -> Float { return ((2.3 * s_5) + 0.4); }, s_5))).Result;
        a_121 = engine_math_math::Mat4_mul(a_122, a_124).Result;
        LifeArt_cylinder(self, a_121, rgb(6967869).Result);
        a_127 = at(vector_make(0.0, 0.0, kern::formula([](Float s_5) -> Float { return (2.75 * s_5); }, s_5))).Result;
        a_126 = engine_math_math::Mat4_mul(base, a_127).Result;
        a_128 = sc3(vector_make(kern::formula([](Float s_5) -> Float { return (1.95 * s_5); }, s_5), kern::formula([](Float s_5) -> Float { return (1.95 * s_5); }, s_5), kern::formula([](Float s_5) -> Float { return (2.35 * s_5); }, s_5))).Result;
        a_125 = engine_math_math::Mat4_mul(a_126, a_128).Result;
        LifeArt_ball(self, a_125, rgb(5211736).Result);
      }
    }
  }

  void LifeArt_end_show(LifeArt& self, const LifeArt_Part& p) {
    Bool any = false;
    Bool update_instances = false;
    Bool set_visible = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(p.id), 0.0))) {
      return;
    }
    any = logic_not(math_compare<"==">(kern::to<Float>(list_length<Float>(p.transforms)), 0.0));
    if (any) {
      engine_renderer_update_instances(n29, self.r_, p.id, p.transforms, p.tints);
      update_instances = n29.result;
    }
    engine_renderer_set_visible(n30, self.r_, p.id, any);
    set_visible = n30.result;
  }

  void LifeArt_end(LifeArt& self) {
    LifeArt_end_show(self, self.box_);
    LifeArt_end_show(self, self.ball_);
    LifeArt_end_show(self, self.cyl_);
    LifeArt_end_show(self, self.glow_);
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool result = false; } n11;
  LifeArt_makeResult n12;
  LifeArt_makeResult n13;
  LifeArt_makeResult n14;
  LifeArt_makeResult n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct { Bool result = false; Bool changed = false; } n29;
  struct { Bool result = false; } n30;
};
