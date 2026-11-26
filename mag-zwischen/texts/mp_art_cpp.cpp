struct mp_art_cpp : virtual kern::Library, virtual common, virtual engine_geo_mesh_data, virtual engine_math_math, virtual mp_art {
  // ---- variables ----
  /// vertex locations 14 and 15
  KERN_CONST U32 kMaxExtensionAttributes = 2;
  KERN_CONST Float kPi = 3.141592653589793;

  // ---- functions ----
  struct glowResult { Material Result = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}; };
  KERN_PURE glowResult glow(Vector emissive = Vector{0.0, 0.0, 0.0}, Float alpha = 0.0) {
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    m.roughness = 0.5;
    m.emissive = emissive;
    m.unlit = true;
    m.cast_shadow = false;
    m.color = engine_math_math::Vec4_new(1.0, 1.0, 1.0, alpha).Result;
    return {m};
  }

  struct MpArt_makeResult { Bool Result = false; };
  MpArt_makeResult MpArt_make(MpArt_Part& p, const MeshData& mesh, const Material& m, Object<kern::Renderer> r = Object<kern::Renderer>{}) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
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

  struct MpArt_initResult { Bool Result = false; };
  MpArt_initResult MpArt_init(MpArt& self, Object<kern::Renderer> r = Object<kern::Renderer>{}) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Vector w = Vector{0.0, 0.0, 0.0};
    MeshData post = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_3 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData torus = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_5 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_6 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_7 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_8 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_9 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Material accent = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_10 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData a_11 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData torus_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_12 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_13 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_14 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    r_ = r;
    self.r_ = r_;
    w = Vector{1.0, 1.0, 1.0};
    post = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    cylinder = engine_primitives_cylinder(0.05, 1.0, 6, w);
    engine_primitives_append(n12, post, cylinder, engine_math_math::Mat4_translate(Vector{0.0, 0.0, 0.5}).Result);
    post = n12.dst;
    a = glow(Vector{1.2, 1.2, 1.2}, 0.55).Result;
    n13 = MpArt_make(self.post_, post, a, r_);
    if (logic_not(n13.Result)) {
      return {false};
    }
    sphere = engine_primitives_sphere(0.45, 4, 2, w);
    a_2 = sphere;
    a_3 = glow(Vector{1.6, 1.6, 1.6}, 0.95).Result;
    n14 = MpArt_make(self.gem_, a_2, a_3, r_);
    if (logic_not(n14.Result)) {
      return {false};
    }
    torus = engine_primitives_torus(0.65, 0.1, 24, 4, w);
    a_4 = torus;
    a_5 = glow(Vector{1.4, 1.4, 1.4}, 0.7).Result;
    n15 = MpArt_make(self.ring_, a_4, a_5, r_);
    if (logic_not(n15.Result)) {
      return {false};
    }
    cylinder_2 = engine_primitives_cylinder(0.03, 1.0, 6, w);
    a_6 = cylinder_2;
    a_7 = glow(Vector{3.0, 2.4, 1.2}, 0.9).Result;
    n16 = MpArt_make(self.tracer_, a_6, a_7, r_);
    if (logic_not(n16.Result)) {
      return {false};
    }
    sphere_2 = engine_primitives_sphere(1.0, 10, 6, w);
    a_8 = sphere_2;
    a_9 = glow(Vector{4.0, 1.5, 0.6}, 0.8).Result;
    n17 = MpArt_make(self.flash_, a_8, a_9, r_);
    if (logic_not(n17.Result)) {
      return {false};
    }
    accent = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    accent.roughness = 0.35;
    accent.metalness = 0.25;
    accent.emissive = Vector{0.35, 0.35, 0.35};
    box = engine_primitives_box(Vector{0.085, 0.06, 0.026}, w);
    a_10 = box;
    n18 = MpArt_make(self.canopy_, a_10, accent, r_);
    if (logic_not(n18.Result)) {
      return {false};
    }
    torus_2 = engine_primitives_torus(0.03, 0.004, 24, 6, w);
    a_11 = torus_2;
    a_12 = glow(Vector{2.0, 2.0, 2.0}, 1.0).Result;
    n19 = MpArt_make(self.led_, a_11, a_12, r_);
    if (logic_not(n19.Result)) {
      return {false};
    }
    sphere_3 = engine_primitives_sphere(1.0, 16, 10, w);
    a_13 = sphere_3;
    a_14 = glow(Vector{0.5, 0.8, 1.0}, 0.18).Result;
    n20 = MpArt_make(self.shield_, a_13, a_14, r_);
    if (logic_not(n20.Result)) {
      return {false};
    }
    return {true};
  }

  void MpArt_clear(MpArt_Part& p) {
    list_clear(p.transforms);
    list_clear(p.tints);
  }

  void MpArt_begin(MpArt& self) {
    MpArt_clear(self.post_);
    MpArt_clear(self.gem_);
    MpArt_clear(self.ring_);
    MpArt_clear(self.tracer_);
    MpArt_clear(self.flash_);
    MpArt_clear(self.canopy_);
    MpArt_clear(self.led_);
    MpArt_clear(self.shield_);
  }

  void MpArt_put(MpArt_Part& p, const Mat4& m, Vector t = Vector{0.0, 0.0, 0.0}) {
    Int place = 0;
    Int place_2 = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    place = list_length<Float>(p.transforms);
    list_insert_list(p.transforms, place, m.m);
    place_2 = list_length<Float>(p.tints);
    item = vector_break(t).x;
    item_2 = vector_break(t).y;
    item_3 = vector_break(t).z;
    item_4 = 1.0;
    list_insert_list(p.tints, place_2, list_make<Float, "4">(item, item_2, item_3, item_4));
  }

  void MpArt_ping(MpArt& self, Vector foot = Vector{0.0, 0.0, 0.0}, Float s = 0.0, Vector c = Vector{0.0, 0.0, 0.0}, Float fade = 0.0, Float now = 0.0) {
    Vector t = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float bob = 0.0;
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float rs = 0.0;
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    t = vector_scale(c, fade);
    a = engine_math_math::Mat4_trs(foot, struct_make<Quat>(0.0, 0.0, 0.0, 1.0), vector_make(s, s, kern::formula([](Float s) -> Float { return (2.2 * s); }, s))).Result;
    MpArt_put(self.post_, a, vector_scale(Vector{1.0, 1.0, 1.0}, fade));
    bob = kern::formula([](Float now, Float s) -> Float { return ((kern::math::sin(kern::math::div(now, 0.3)) * 0.15) * s); }, now, s);
    a_2 = engine_math_math::Mat4_trs(vector_add(foot, vector_make(0.0, 0.0, kern::formula([](Float s, Float bob) -> Float { return ((2.6 * s) + bob); }, s, bob))), common::yaw_quat(kern::formula([](Float now) -> Float { return kern::math::div(now, 0.7); }, now)).Result, vector_make(s, s, kern::formula([](Float s) -> Float { return (1.5 * s); }, s))).Result;
    MpArt_put(self.gem_, a_2, t);
    rs = kern::formula([](Float s, Float now) -> Float { return (s * (1.0 + (0.25 * kern::math::sin(kern::math::div(now, 0.22))))); }, s, now);
    a_3 = engine_math_math::Mat4_trs(vector_add(foot, Vector{0.0, 0.0, 0.05}), struct_make<Quat>(0.0, 0.0, 0.0, 1.0), vector_make(rs, rs, kern::formula([](Float rs) -> Float { return (0.3 * rs); }, rs))).Result;
    MpArt_put(self.ring_, a_3, t);
  }

  void MpArt_tracer(MpArt& self, Vector at = Vector{0.0, 0.0, 0.0}, Vector dir = Vector{0.0, 0.0, 0.0}, Float len = 0.0, Vector c = Vector{0.0, 0.0, 0.0}) {
    Float dl = 0.0;
    Vector d = Vector{0.0, 0.0, 0.0};
    Vector axis = Vector{0.0, 0.0, 0.0};
    Float s = 0.0;
    Float a = 0.0;
    Quat q = Quat{0.0, 0.0, 0.0, 1.0};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    dl = vector_length(dir);
    if (math_compare<"<">(dl, 1e-06)) {
      return;
    }
    d = vector_scale(dir, kern::formula([](Float dl) -> Float { return kern::math::div(1.0, dl); }, dl));
    axis = vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(d).y), vector_break(d).x, 0.0);
    s = vector_length(axis);
    a = kern::formula([](Float s, Float z) -> Float { return kern::math::atan2(s, z); }, s, vector_break(d).z);
    q = logic_select<Quat>(math_compare<">">(s, 1e-05), engine_math_math::Quat_axis_angle(vector_scale(axis, kern::formula([](Float s) -> Float { return kern::math::div(1.0, s); }, s)), a).Result, logic_select<Quat>(math_compare<"<">(vector_break(d).z, 0.0), engine_math_math::Quat_axis_angle(Vector{1.0, 0.0, 0.0}, kPi).Result, struct_make<Quat>(0.0, 0.0, 0.0, 1.0)));
    a_2 = engine_math_math::Mat4_trs(at, q, vector_make(1.0, 1.0, len)).Result;
    MpArt_put(self.tracer_, a_2, c);
  }

  void MpArt_flash(MpArt& self, Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float t = 0.0) {
    Float r = 0.0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r = kern::formula([](Float radius, Float t) -> Float { return (radius * (0.35 + (0.65 * t))); }, radius, t);
    a = engine_math_math::Mat4_trs(at, struct_make<Quat>(0.0, 0.0, 0.0, 1.0), vector_make(r, r, r)).Result;
    MpArt_put(self.flash_, a, vector_scale(Vector{1.0, 1.0, 1.0}, kern::formula([](Float t) -> Float { return kern::math::max(0.0, (1.0 - t)); }, t)));
  }

  void MpArt_drone_accent(MpArt& self, const Mat4& body, Vector c = Vector{0.0, 0.0, 0.0}) {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a_2 = engine_math_math::Mat4_translate(Vector{0.005, 0.0, 0.027}).Result;
    a = engine_math_math::Mat4_mul(body, a_2).Result;
    MpArt_put(self.canopy_, a, c);
    a_4 = engine_math_math::Mat4_translate(Vector{0.005, 0.0, 0.042}).Result;
    a_3 = engine_math_math::Mat4_mul(body, a_4).Result;
    MpArt_put(self.led_, a_3, c);
  }

  void MpArt_shield(MpArt& self, Vector center = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float now = 0.0) {
    Float r = 0.0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    r = kern::formula([](Float radius, Float now) -> Float { return (radius * (1.0 + (0.04 * kern::math::sin((now * 9.0))))); }, radius, now);
    a = engine_math_math::Mat4_trs(center, struct_make<Quat>(0.0, 0.0, 0.0, 1.0), vector_make(r, r, kern::formula([](Float r) -> Float { return (r * 1.3); }, r))).Result;
    MpArt_put(self.shield_, a, Vector{0.6, 0.85, 1.0});
  }

  void MpArt_upload(MpArt& self, MpArt_Part& p) {
    Bool set_visible = false;
    Bool update_instances = false;
    Bool set_visible_2 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(p.id), 0.0))) {
      return;
    }
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(p.transforms)), 0.0)) {
      engine_renderer_set_visible(n21, self.r_, p.id, false);
      set_visible = n21.result;
      return;
    }
    engine_renderer_update_instances(n22, self.r_, p.id, p.transforms, p.tints);
    update_instances = n22.result;
    engine_renderer_set_visible(n23, self.r_, p.id, true);
    set_visible_2 = n23.result;
  }

  void MpArt_end(MpArt& self) {
    MpArt_upload(self, self.post_);
    MpArt_upload(self, self.gem_);
    MpArt_upload(self, self.ring_);
    MpArt_upload(self, self.tracer_);
    MpArt_upload(self, self.flash_);
    MpArt_upload(self, self.canopy_);
    MpArt_upload(self, self.led_);
    MpArt_upload(self, self.shield_);
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool result = false; } n11;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n12;
  MpArt_makeResult n13;
  MpArt_makeResult n14;
  MpArt_makeResult n15;
  MpArt_makeResult n16;
  MpArt_makeResult n17;
  MpArt_makeResult n18;
  MpArt_makeResult n19;
  MpArt_makeResult n20;
  struct { Bool result = false; } n21;
  struct { Bool result = false; Bool changed = false; } n22;
  struct { Bool result = false; } n23;
};
