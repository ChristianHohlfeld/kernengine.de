struct combat_art_cpp : virtual kern::Library, virtual combat_art, virtual engine_geo_mesh_data, virtual engine_math_math {
  // ---- variables ----
  /// vertex locations 14 and 15
  KERN_CONST U32 kMaxExtensionAttributes = 2;
  KERN_CONST Vector kOlive = Vector{0.29, 0.34, 0.2};
  KERN_CONST Vector kDark = Vector{0.13, 0.15, 0.11};
  KERN_CONST Vector kGlass = Vector{0.1, 0.13, 0.16};
  KERN_CONST Float kPi = 3.141592653589793;
  KERN_CONST Vector kSteel = Vector{0.16, 0.17, 0.18};
  KERN_CONST Vector kSand = Vector{0.71, 0.61, 0.42};
  KERN_CONST Vector kJetMetal = Vector{0.36, 0.39, 0.42};
  KERN_CONST Vector kJetDark = Vector{0.16, 0.18, 0.21};

  // ---- functions ----
  struct rotResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE rotResult rot(Vector axis = Vector{0.0, 0.0, 0.0}, Float a = 0.0) {
    return {engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(axis, a).Result, Vector{1.0, 1.0, 1.0}).Result};
  }

  struct atResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE atResult at(Vector p = Vector{0.0, 0.0, 0.0}) {
    return {engine_math_math::Mat4_translate(p).Result};
  }

  struct sc3Result { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE sc3Result sc3(Float x = 0.0, Float y = 0.0, Float z = 0.0) {
    return {engine_math_math::Mat4_scale(vector_make(x, y, z)).Result};
  }

  struct CombatArt_initResult { Bool Result = false; };
  CombatArt_initResult CombatArt_init(CombatArt& self, Int r = 0) {
    Int r_ = 0;
    Vector white = Vector{0.0, 0.0, 0.0};
    Fixed<MeshData, 9> meshes = Fixed<MeshData, 9>{};
    MeshData box = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cone = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData torus = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool upsert = false;
    r_ = r;
    self.r_ = r_;
    white = Vector{1.0, 1.0, 1.0};
    box = engine_primitives_box(Vector{1.0, 1.0, 1.0}, white);
    sphere = engine_primitives_sphere(0.5, 14, 10, white);
    cylinder = engine_primitives_cylinder(0.5, 1.0, 14, white);
    cone = engine_primitives_cone(0.5, 1.0, 14, white);
    box_2 = engine_primitives_box(Vector{1.0, 1.0, 1.0}, white);
    sphere_2 = engine_primitives_sphere(0.5, 12, 8, white);
    sphere_3 = engine_primitives_sphere(0.5, 10, 7, white);
    sphere_4 = engine_primitives_sphere(0.5, 12, 8, white);
    torus = engine_primitives_torus(0.8, 0.2, 24, 6, white);
    meshes = list_make<MeshData, "9">(box, sphere, cylinder, cone, box_2, sphere_2, sphere_3, sphere_4, torus);
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(enum_index<CombatArt_Shape>(CombatArt_Shape::kShapes))))); it10.next();) {
      m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
      m.roughness = 0.7;
      m.metalness = 0.25;
      if (logic_or(math_compare<"==">(kern::to<Float>(n10.index), kern::to<Float>(enum_index<CombatArt_Shape>(CombatArt_Shape::glow_box))), math_compare<"==">(kern::to<Float>(n10.index), kern::to<Float>(enum_index<CombatArt_Shape>(CombatArt_Shape::glow_sphere))))) {
        m.emissive = Vector{1.0, 1.0, 1.0};
        m.unlit = true;
        m.cast_shadow = false;
      }
      if (logic_or(math_compare<"==">(kern::to<Float>(n10.index), kern::to<Float>(enum_index<CombatArt_Shape>(CombatArt_Shape::fade_sphere))), math_compare<"==">(kern::to<Float>(n10.index), kern::to<Float>(enum_index<CombatArt_Shape>(CombatArt_Shape::fade_ring))))) {
        m.emissive = Vector{1.0, 1.0, 1.0};
        m.unlit = true;
        m.cast_shadow = false;
        m.double_sided = true;
        m.alpha = AlphaMode::blend;
      }
      if (math_compare<"==">(kern::to<Float>(n10.index), kern::to<Float>(enum_index<CombatArt_Shape>(CombatArt_Shape::smoke)))) {
        m.roughness = 1.0;
        m.metalness = 0.0;
        m.cast_shadow = false;
      }
      engine_renderer_allocate_id(n11, r_);
      allocate_id = n11.result;
      self.parts_[n10.index].id = allocate_id;
      look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
      look.material = m;
      look.extra_slot = kMaxExtensionAttributes;
      look.visible = false;
      parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
      engine_upsert(n12, r_, self.parts_[n10.index].id, meshes[n10.index], look, parked.m, List<Float>{});
      upsert = n12.result;
      if (logic_not(upsert)) {
        engine_renderer_release_id(r_, self.parts_[n10.index].id);
        self.parts_[n10.index].id = 0;
        return {false};
      }
    }
    return {true};
  }

  void CombatArt_begin(CombatArt& self) {
    for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CombatArt_Part>(self.parts_))))); it13.next();) {
      list_clear(self.parts_[n13.index].transforms);
      list_clear(self.parts_[n13.index].tints);
    }
  }

  void CombatArt_put(CombatArt& self, const Mat4& m, CombatArt_Shape s = CombatArt_Shape::box, Vector tint = Vector{0.0, 0.0, 0.0}) {
    Int place = 0;
    Int place_2 = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    place = list_length<Float>(self.parts_[enum_index<CombatArt_Shape>(s)].transforms);
    list_insert_list(self.parts_[enum_index<CombatArt_Shape>(s)].transforms, place, m.m);
    place_2 = list_length<Float>(self.parts_[enum_index<CombatArt_Shape>(s)].tints);
    item = vector_break(tint).x;
    item_2 = vector_break(tint).y;
    item_3 = vector_break(tint).z;
    item_4 = 1.0;
    list_insert_list(self.parts_[enum_index<CombatArt_Shape>(s)].tints, place_2, list_make<Float, "4">(item, item_2, item_3, item_4));
  }

  void CombatArt_put_2(CombatArt& self, const Mat4& m, CombatArt_Shape s = CombatArt_Shape::box, Vector tint = Vector{0.0, 0.0, 0.0}, Float alpha = 0.0) {
    Int place = 0;
    Int place_2 = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    place = list_length<Float>(self.parts_[enum_index<CombatArt_Shape>(s)].transforms);
    list_insert_list(self.parts_[enum_index<CombatArt_Shape>(s)].transforms, place, m.m);
    place_2 = list_length<Float>(self.parts_[enum_index<CombatArt_Shape>(s)].tints);
    item = vector_break(tint).x;
    item_2 = vector_break(tint).y;
    item_3 = vector_break(tint).z;
    item_4 = kern::formula([](Float alpha) -> Float { return (alpha < 0.0 ? 0.0 : (1.0 < alpha ? 1.0 : alpha)); }, alpha);
    list_insert_list(self.parts_[enum_index<CombatArt_Shape>(s)].tints, place_2, list_make<Float, "4">(item, item_2, item_3, item_4));
  }

  void CombatArt_beam(CombatArt& self, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Float width = 0.0, Vector tint = Vector{0.0, 0.0, 0.0}) {
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector axis = Vector{0.0, 0.0, 0.0};
    Float ang = 0.0;
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    d = vector_sub(b, a);
    l = vector_length(d);
    if (math_compare<"<">(l, 0.05)) {
      return;
    }
    n = vector_scale(d, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l));
    axis = vector_cross(Vector{0.0, 0.0, 1.0}, n);
    ang = kern::formula([](Float z) -> Float { return kern::math::acos((z < (-1.0) ? (-1.0) : (1.0 < z ? 1.0 : z))); }, vector_break(n).z);
    a_4 = at(vector_scale(vector_add(a, b), 0.5)).Result;
    a_5 = rot(logic_select<Vector>(math_compare<">">(vector_length(axis), 9.999999717180685e-10), vector_normalize(axis), Vector{1.0, 0.0, 0.0}), ang).Result;
    a_3 = engine_math_math::Mat4_mul(a_4, a_5).Result;
    a_6 = sc3(width, width, l).Result;
    a_2 = engine_math_math::Mat4_mul(a_3, a_6).Result;
    CombatArt_put(self, a_2, CombatArt_Shape::glow_box, tint);
  }

  void CombatArt_end(CombatArt& self) {
    Bool skip = false;
    Bool set_visible = false;
    Bool update_instances = false;
    Bool set_visible_2 = false;
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CombatArt_Part>(self.parts_))))); it14.next();) {
      skip = false;
      if (logic_not(math_compare<"!=">(kern::to<Float>(self.parts_[n14.index].id), 0.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (math_compare<"==">(kern::to<Float>(list_length<Float>(self.parts_[n14.index].transforms)), 0.0)) {
          engine_renderer_set_visible(n15, self.r_, self.parts_[n14.index].id, false);
          set_visible = n15.result;
          skip = true;
        }
        if (logic_not(skip)) {
          engine_renderer_update_instances(n16, self.r_, self.parts_[n14.index].id, self.parts_[n14.index].transforms, self.parts_[n14.index].tints);
          update_instances = n16.result;
          engine_renderer_set_visible(n17, self.r_, self.parts_[n14.index].id, true);
          set_visible_2 = n17.result;
        }
      }
    }
  }

  void CombatArt_heli(CombatArt& self, const Mat4& m, Float rotor = 0.0, Bool wreck = false) {
    Vector o = Vector{0.0, 0.0, 0.0};
    Vector dk = Vector{0.0, 0.0, 0.0};
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
    Float item = 0.0;
    Float item_2 = 0.0;
    Fixed<Float, 2> items = Fixed<Float, 2>{};
    Float s = 0.0;
    Mat4 a_27 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_28 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_29 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_30 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_31 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_32 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_33 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_34 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_35 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_36 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_37 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_38 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_39 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_40 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_41 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_42 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_43 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_44 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_45 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_46 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 mast = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
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
    Mat4 a_57 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_58 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_59 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_60 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_61 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_62 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_63 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_64 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_65 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    o = logic_select<Vector>(wreck, vector_scale(kOlive, 0.35), kOlive);
    dk = logic_select<Vector>(wreck, vector_scale(kDark, 0.5), kDark);
    a_2 = sc3(1.9, 5.6, 1.9).Result;
    a = engine_math_math::Mat4_mul(m, a_2).Result;
    CombatArt_put(self, a, CombatArt_Shape::box, o);
    a_5 = at(Vector{0.0, -2.8, 0.0}).Result;
    a_4 = engine_math_math::Mat4_mul(m, a_5).Result;
    a_6 = sc3(1.9, 1.9, 1.9).Result;
    a_3 = engine_math_math::Mat4_mul(a_4, a_6).Result;
    CombatArt_put(self, a_3, CombatArt_Shape::sphere, o);
    a_9 = at(Vector{0.0, 2.6, 0.2}).Result;
    a_8 = engine_math_math::Mat4_mul(m, a_9).Result;
    a_10 = sc3(1.5, 2.2, 1.6).Result;
    a_7 = engine_math_math::Mat4_mul(a_8, a_10).Result;
    CombatArt_put(self, a_7, CombatArt_Shape::sphere, o);
    a_13 = at(Vector{0.0, 2.3, 0.45}).Result;
    a_12 = engine_math_math::Mat4_mul(m, a_13).Result;
    a_14 = sc3(1.2, 1.9, 1.2).Result;
    a_11 = engine_math_math::Mat4_mul(a_12, a_14).Result;
    CombatArt_put(self, a_11, CombatArt_Shape::sphere, kGlass);
    a_19 = at(Vector{0.0, -2.2, 0.35}).Result;
    a_18 = engine_math_math::Mat4_mul(m, a_19).Result;
    a_20 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
    a_17 = engine_math_math::Mat4_mul(a_18, a_20).Result;
    a_21 = sc3(0.6, 0.6, 5.2).Result;
    a_16 = engine_math_math::Mat4_mul(a_17, a_21).Result;
    a_22 = at(Vector{0.0, 0.0, 0.0}).Result;
    a_15 = engine_math_math::Mat4_mul(a_16, a_22).Result;
    CombatArt_put(self, a_15, CombatArt_Shape::cylinder, o);
    a_25 = at(Vector{0.0, -7.0, 0.95}).Result;
    a_24 = engine_math_math::Mat4_mul(m, a_25).Result;
    a_26 = sc3(0.12, 1.1, 1.5).Result;
    a_23 = engine_math_math::Mat4_mul(a_24, a_26).Result;
    CombatArt_put(self, a_23, CombatArt_Shape::box, o);
    item = -1.0;
    item_2 = 1.0;
    items = list_make<Float, "2">(item, item_2);
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it18.next();) {
      s = items[n18.index];
      a_29 = at(vector_make(kern::formula([](Float s) -> Float { return (s * 1.4); }, s), 0.2, -0.15)).Result;
      a_28 = engine_math_math::Mat4_mul(m, a_29).Result;
      a_30 = sc3(1.7, 0.5, 0.12).Result;
      a_27 = engine_math_math::Mat4_mul(a_28, a_30).Result;
      CombatArt_put(self, a_27, CombatArt_Shape::box, o);
      a_34 = at(vector_make(kern::formula([](Float s) -> Float { return (s * 2.1); }, s), -0.4, -0.35)).Result;
      a_33 = engine_math_math::Mat4_mul(m, a_34).Result;
      a_35 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div((-kPi), 2.0); }, kPi)).Result;
      a_32 = engine_math_math::Mat4_mul(a_33, a_35).Result;
      a_36 = sc3(0.32, 0.32, 1.6).Result;
      a_31 = engine_math_math::Mat4_mul(a_32, a_36).Result;
      CombatArt_put(self, a_31, CombatArt_Shape::cylinder, dk);
      a_39 = at(vector_make(kern::formula([](Float s) -> Float { return (s * 0.9); }, s), 0.0, -1.3)).Result;
      a_38 = engine_math_math::Mat4_mul(m, a_39).Result;
      a_40 = sc3(0.08, 3.2, 0.08).Result;
      a_37 = engine_math_math::Mat4_mul(a_38, a_40).Result;
      CombatArt_put(self, a_37, CombatArt_Shape::box, dk);
    }
    a_44 = at(Vector{0.0, 2.7, -0.8}).Result;
    a_43 = engine_math_math::Mat4_mul(m, a_44).Result;
    a_45 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div((-kPi), 2.0); }, kPi)).Result;
    a_42 = engine_math_math::Mat4_mul(a_43, a_45).Result;
    a_46 = sc3(0.16, 0.16, 1.1).Result;
    a_41 = engine_math_math::Mat4_mul(a_42, a_46).Result;
    CombatArt_put(self, a_41, CombatArt_Shape::cylinder, dk);
    if (logic_not(wreck)) {
      a_48 = at(Vector{0.0, 0.2, 1.25}).Result;
      a_47 = engine_math_math::Mat4_mul(m, a_48).Result;
      a_49 = rot(Vector{0.0, 0.0, 1.0}, rotor).Result;
      mast = engine_math_math::Mat4_mul(a_47, a_49).Result;
      a_51 = sc3(12.5, 0.32, 0.04).Result;
      a_50 = engine_math_math::Mat4_mul(mast, a_51).Result;
      CombatArt_put(self, a_50, CombatArt_Shape::box, vector_scale(kSteel, 0.6));
      a_54 = rot(Vector{0.0, 0.0, 1.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
      a_53 = engine_math_math::Mat4_mul(mast, a_54).Result;
      a_55 = sc3(12.5, 0.32, 0.04).Result;
      a_52 = engine_math_math::Mat4_mul(a_53, a_55).Result;
      CombatArt_put(self, a_52, CombatArt_Shape::box, vector_scale(kSteel, 0.6));
      a_59 = at(Vector{0.18, -7.0, 1.1}).Result;
      a_58 = engine_math_math::Mat4_mul(m, a_59).Result;
      a_60 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float rotor) -> Float { return (rotor * 1.7); }, rotor)).Result;
      a_57 = engine_math_math::Mat4_mul(a_58, a_60).Result;
      a_61 = sc3(0.04, 0.22, 2.2).Result;
      a_56 = engine_math_math::Mat4_mul(a_57, a_61).Result;
      CombatArt_put(self, a_56, CombatArt_Shape::box, vector_scale(kSteel, 0.6));
      if (math_compare<"<">(kern::formula([](Float rotor) -> Float { return kern::math::mod(rotor, 4.4); }, rotor), 2.2)) {
        a_64 = at(Vector{0.0, -7.0, 1.75}).Result;
        a_63 = engine_math_math::Mat4_mul(m, a_64).Result;
        a_65 = sc3(0.25, 0.25, 0.25).Result;
        a_62 = engine_math_math::Mat4_mul(a_63, a_65).Result;
        CombatArt_put(self, a_62, CombatArt_Shape::glow_sphere, Vector{3.0, 0.2, 0.1});
      }
    }
  }

  void CombatArt_tank(CombatArt& self, const Mat4& m, Float turret = 0.0, Bool wreck = false) {
    Vector o = Vector{0.0, 0.0, 0.0};
    Vector dk = Vector{0.0, 0.0, 0.0};
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
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_13 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_14 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_15 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_16 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_17 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_18 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 t = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
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
    Mat4 a_31 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_32 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_33 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_34 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_35 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    o = logic_select<Vector>(wreck, vector_scale(kOlive, 0.3), kOlive);
    dk = logic_select<Vector>(wreck, vector_scale(kDark, 0.5), kDark);
    a_3 = at(Vector{0.0, 0.0, 0.95}).Result;
    a_2 = engine_math_math::Mat4_mul(m, a_3).Result;
    a_4 = sc3(6.6, 3.1, 1.0).Result;
    a = engine_math_math::Mat4_mul(a_2, a_4).Result;
    CombatArt_put(self, a, CombatArt_Shape::box, o);
    a_7 = at(Vector{0.0, 1.25, 0.55}).Result;
    a_6 = engine_math_math::Mat4_mul(m, a_7).Result;
    a_8 = sc3(6.9, 0.75, 0.85).Result;
    a_5 = engine_math_math::Mat4_mul(a_6, a_8).Result;
    CombatArt_put(self, a_5, CombatArt_Shape::box, dk);
    a_11 = at(Vector{0.0, -1.25, 0.55}).Result;
    a_10 = engine_math_math::Mat4_mul(m, a_11).Result;
    a_12 = sc3(6.9, 0.75, 0.85).Result;
    a_9 = engine_math_math::Mat4_mul(a_10, a_12).Result;
    CombatArt_put(self, a_9, CombatArt_Shape::box, dk);
    a_16 = at(Vector{2.9, 0.0, 1.25}).Result;
    a_15 = engine_math_math::Mat4_mul(m, a_16).Result;
    a_17 = rot(Vector{0.0, 1.0, 0.0}, -0.5).Result;
    a_14 = engine_math_math::Mat4_mul(a_15, a_17).Result;
    a_18 = sc3(1.4, 2.9, 0.5).Result;
    a_13 = engine_math_math::Mat4_mul(a_14, a_18).Result;
    CombatArt_put(self, a_13, CombatArt_Shape::box, o);
    a_20 = at(Vector{-0.3, 0.0, 1.65}).Result;
    a_19 = engine_math_math::Mat4_mul(m, a_20).Result;
    a_21 = rot(Vector{0.0, 0.0, 1.0}, turret).Result;
    t = engine_math_math::Mat4_mul(a_19, a_21).Result;
    a_24 = at(Vector{0.0, 0.0, 0.2}).Result;
    a_23 = engine_math_math::Mat4_mul(t, a_24).Result;
    a_25 = sc3(2.8, 2.3, 0.75).Result;
    a_22 = engine_math_math::Mat4_mul(a_23, a_25).Result;
    CombatArt_put(self, a_22, CombatArt_Shape::box, o);
    a_29 = at(Vector{0.8, 0.0, 0.3}).Result;
    a_28 = engine_math_math::Mat4_mul(t, a_29).Result;
    a_30 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
    a_27 = engine_math_math::Mat4_mul(a_28, a_30).Result;
    a_31 = sc3(0.3, 0.3, 4.4).Result;
    a_26 = engine_math_math::Mat4_mul(a_27, a_31).Result;
    CombatArt_put(self, a_26, CombatArt_Shape::cylinder, dk);
    a_34 = at(Vector{-0.6, 0.5, 0.55}).Result;
    a_33 = engine_math_math::Mat4_mul(t, a_34).Result;
    a_35 = sc3(0.7, 0.7, 0.4).Result;
    a_32 = engine_math_math::Mat4_mul(a_33, a_35).Result;
    CombatArt_put(self, a_32, CombatArt_Shape::cylinder, dk);
  }

  void CombatArt_flak(CombatArt& self, Vector p = Vector{0.0, 0.0, 0.0}, Float base_yaw = 0.0, Float yaw = 0.0, Float pitch = 0.0, Bool dead = false, Bool flash = false) {
    Float k = 0.0;
    Mat4 base = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool skip = false;
    Float a_3 = 0.0;
    Mat4 bag = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
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
    Mat4 a_17 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 y = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
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
    Mat4 a_31 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 pt = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_32 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_33 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_34 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_35 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_36 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector item = Vector{0.0, 0.0, 0.0};
    Vector item_2 = Vector{0.0, 0.0, 0.0};
    Vector item_3 = Vector{0.0, 0.0, 0.0};
    Vector item_4 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 4> items = Fixed<Vector, 4>{};
    Vector q = Vector{0.0, 0.0, 0.0};
    Mat4 a_37 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_38 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
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
    k = kern::formula([](Float dead) -> Float { return ((dead != 0.0) ? 0.35 : 1.0); }, kern::to<Float>(dead));
    a = at(p).Result;
    a_2 = rot(Vector{0.0, 0.0, 1.0}, base_yaw).Result;
    base = engine_math_math::Mat4_mul(a, a_2).Result;
    for (auto it19 = flow_for(n19, 0, 13); it19.next();) {
      skip = false;
      if (logic_or(math_compare<"==">(kern::to<Float>(n19.index), 3.0), math_compare<"==">(kern::to<Float>(n19.index), 4.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        a_3 = kern::formula([](Float index, Float kPi) -> Float { return ((kern::math::div(index, 14.0) * 2.0) * kPi); }, kern::to<Float>(n19.index), kPi);
        a_6 = at(vector_make(kern::formula([](Float a_3) -> Float { return (kern::math::cos(a_3) * 2.6); }, a_3), kern::formula([](Float a_3) -> Float { return (kern::math::sin(a_3) * 2.6); }, a_3), 0.3)).Result;
        a_5 = engine_math_math::Mat4_mul(base, a_6).Result;
        a_7 = rot(Vector{0.0, 0.0, 1.0}, kern::formula([](Float a_3, Float kPi) -> Float { return (a_3 + kern::math::div(kPi, 2.0)); }, a_3, kPi)).Result;
        a_4 = engine_math_math::Mat4_mul(a_5, a_7).Result;
        a_8 = sc3(1.2, 0.56, 0.5).Result;
        bag = engine_math_math::Mat4_mul(a_4, a_8).Result;
        CombatArt_put(self, bag, CombatArt_Shape::sphere, vector_scale(kSand, k));
        a_12 = at(vector_make(kern::formula([](Float a_3) -> Float { return (kern::math::cos(a_3) * 2.6); }, a_3), kern::formula([](Float a_3) -> Float { return (kern::math::sin(a_3) * 2.6); }, a_3), 0.75)).Result;
        a_11 = engine_math_math::Mat4_mul(base, a_12).Result;
        a_13 = rot(Vector{0.0, 0.0, 1.0}, kern::formula([](Float a_3, Float kPi) -> Float { return ((a_3 + kern::math::div(kPi, 2.0)) + 0.2); }, a_3, kPi)).Result;
        a_10 = engine_math_math::Mat4_mul(a_11, a_13).Result;
        a_14 = sc3(1.2, 0.56, 0.48).Result;
        a_9 = engine_math_math::Mat4_mul(a_10, a_14).Result;
        CombatArt_put(self, a_9, CombatArt_Shape::sphere, vector_scale(kSand, k));
      }
    }
    a_16 = at(p).Result;
    a_17 = sc3(2.4, 2.4, 0.35).Result;
    a_15 = engine_math_math::Mat4_mul(a_16, a_17).Result;
    CombatArt_put(self, a_15, CombatArt_Shape::cylinder, vector_scale(kOlive, k));
    a_18 = at(vector_add(p, Vector{0.0, 0.0, 0.35})).Result;
    a_19 = rot(Vector{0.0, 0.0, 1.0}, yaw).Result;
    y = engine_math_math::Mat4_mul(a_18, a_19).Result;
    a_22 = at(Vector{0.0, 0.0, 0.45}).Result;
    a_21 = engine_math_math::Mat4_mul(y, a_22).Result;
    a_23 = sc3(1.2, 1.4, 0.8).Result;
    a_20 = engine_math_math::Mat4_mul(a_21, a_23).Result;
    CombatArt_put(self, a_20, CombatArt_Shape::box, vector_scale(kOlive, k));
    a_26 = at(Vector{0.55, 0.0, 1.25}).Result;
    a_25 = engine_math_math::Mat4_mul(y, a_26).Result;
    a_27 = sc3(0.08, 1.9, 1.1).Result;
    a_24 = engine_math_math::Mat4_mul(a_25, a_27).Result;
    CombatArt_put(self, a_24, CombatArt_Shape::box, vector_scale(kOlive, k));
    a_30 = at(Vector{-0.5, 0.0, 0.95}).Result;
    a_29 = engine_math_math::Mat4_mul(y, a_30).Result;
    a_31 = sc3(0.4, 0.5, 0.12).Result;
    a_28 = engine_math_math::Mat4_mul(a_29, a_31).Result;
    CombatArt_put(self, a_28, CombatArt_Shape::box, vector_scale(kSteel, k));
    a_33 = at(Vector{0.1, 0.0, 1.15}).Result;
    a_32 = engine_math_math::Mat4_mul(y, a_33).Result;
    a_34 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float dead, Float pitch) -> Float { return (-((dead != 0.0) ? (-0.35) : pitch)); }, kern::to<Float>(dead), pitch)).Result;
    pt = engine_math_math::Mat4_mul(a_32, a_34).Result;
    a_36 = sc3(1.0, 0.9, 0.5).Result;
    a_35 = engine_math_math::Mat4_mul(pt, a_36).Result;
    CombatArt_put(self, a_35, CombatArt_Shape::box, vector_scale(kSteel, k));
    item = Vector{-0.28, 0.17, 0.0};
    item_2 = Vector{0.28, 0.17, 0.0};
    item_3 = Vector{-0.28, -0.17, 0.0};
    item_4 = Vector{0.28, -0.17, 0.0};
    items = list_make<Vector, "4">(item, item_2, item_3, item_4);
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(items))))); it20.next();) {
      q = items[n20.index];
      a_40 = at(vector_make(0.25, vector_break(q).x, vector_break(q).y)).Result;
      a_39 = engine_math_math::Mat4_mul(pt, a_40).Result;
      a_41 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
      a_38 = engine_math_math::Mat4_mul(a_39, a_41).Result;
      a_42 = sc3(0.1, 0.1, 2.3).Result;
      a_37 = engine_math_math::Mat4_mul(a_38, a_42).Result;
      CombatArt_put(self, a_37, CombatArt_Shape::cylinder, vector_scale(kSteel, k));
      a_46 = at(vector_make(2.45, vector_break(q).x, vector_break(q).y)).Result;
      a_45 = engine_math_math::Mat4_mul(pt, a_46).Result;
      a_47 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
      a_44 = engine_math_math::Mat4_mul(a_45, a_47).Result;
      a_48 = sc3(0.15, 0.15, 0.22).Result;
      a_43 = engine_math_math::Mat4_mul(a_44, a_48).Result;
      CombatArt_put(self, a_43, CombatArt_Shape::cylinder, vector_scale(kSteel, k));
      a_51 = at(vector_make(0.2, vector_break(q).x, kern::formula([](Float y) -> Float { return (y + ((y > 0.0) ? 0.28 : (-0.28))); }, vector_break(q).y))).Result;
      a_50 = engine_math_math::Mat4_mul(pt, a_51).Result;
      a_52 = sc3(0.25, 0.08, 0.3).Result;
      a_49 = engine_math_math::Mat4_mul(a_50, a_52).Result;
      CombatArt_put(self, a_49, CombatArt_Shape::box, vector_scale(kOlive, k));
    }
    if (flash) {
      a_55 = at(Vector{2.75, 0.0, 0.0}).Result;
      a_54 = engine_math_math::Mat4_mul(pt, a_55).Result;
      a_56 = sc3(1.5, 1.0, 1.0).Result;
      a_53 = engine_math_math::Mat4_mul(a_54, a_56).Result;
      CombatArt_put(self, a_53, CombatArt_Shape::glow_sphere, Vector{3.2, 2.2, 0.9});
    }
  }

  void CombatArt_fighter(CombatArt& self, Vector p = Vector{0.0, 0.0, 0.0}, Float flame = 0.0) {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
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
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_13 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_14 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_15 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_16 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float item = 0.0;
    Float item_2 = 0.0;
    Fixed<Float, 2> items = Fixed<Float, 2>{};
    Float s = 0.0;
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
    Mat4 a_31 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_32 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_33 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_34 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_35 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_36 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_37 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_38 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_39 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_40 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_41 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_42 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    m = at(p).Result;
    a_4 = at(Vector{0.0, -5.5, 0.0}).Result;
    a_3 = engine_math_math::Mat4_mul(m, a_4).Result;
    a_5 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div((-kPi), 2.0); }, kPi)).Result;
    a_2 = engine_math_math::Mat4_mul(a_3, a_5).Result;
    a_6 = sc3(1.5, 1.5, 11.0).Result;
    a = engine_math_math::Mat4_mul(a_2, a_6).Result;
    CombatArt_put(self, a, CombatArt_Shape::cylinder, kJetMetal);
    a_10 = at(Vector{0.0, 5.5, 0.0}).Result;
    a_9 = engine_math_math::Mat4_mul(m, a_10).Result;
    a_11 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div((-kPi), 2.0); }, kPi)).Result;
    a_8 = engine_math_math::Mat4_mul(a_9, a_11).Result;
    a_12 = sc3(1.24, 1.24, 3.6).Result;
    a_7 = engine_math_math::Mat4_mul(a_8, a_12).Result;
    CombatArt_put(self, a_7, CombatArt_Shape::cone, kJetMetal);
    a_15 = at(Vector{0.0, 3.4, 0.55}).Result;
    a_14 = engine_math_math::Mat4_mul(m, a_15).Result;
    a_16 = sc3(1.0, 3.0, 0.9).Result;
    a_13 = engine_math_math::Mat4_mul(a_14, a_16).Result;
    CombatArt_put(self, a_13, CombatArt_Shape::sphere, kGlass);
    item = -1.0;
    item_2 = 1.0;
    items = list_make<Float, "2">(item, item_2);
    for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it21.next();) {
      s = items[n21.index];
      a_20 = at(vector_make(kern::formula([](Float s) -> Float { return (s * 3.2); }, s), -1.6, -0.1)).Result;
      a_19 = engine_math_math::Mat4_mul(m, a_20).Result;
      a_21 = rot(Vector{0.0, 0.0, 1.0}, kern::formula([](Float s) -> Float { return (s * 0.62); }, s)).Result;
      a_18 = engine_math_math::Mat4_mul(a_19, a_21).Result;
      a_22 = sc3(5.8, 2.4, 0.16).Result;
      a_17 = engine_math_math::Mat4_mul(a_18, a_22).Result;
      CombatArt_put(self, a_17, CombatArt_Shape::box, kJetMetal);
      a_26 = at(vector_make(kern::formula([](Float s) -> Float { return (s * 0.9); }, s), -5.0, 1.3)).Result;
      a_25 = engine_math_math::Mat4_mul(m, a_26).Result;
      a_27 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float s) -> Float { return (s * 0.32); }, s)).Result;
      a_24 = engine_math_math::Mat4_mul(a_25, a_27).Result;
      a_28 = sc3(0.12, 2.6, 2.4).Result;
      a_23 = engine_math_math::Mat4_mul(a_24, a_28).Result;
      CombatArt_put(self, a_23, CombatArt_Shape::box, kJetDark);
      a_31 = at(vector_make(kern::formula([](Float s) -> Float { return (s * 1.6); }, s), -5.4, -0.1)).Result;
      a_30 = engine_math_math::Mat4_mul(m, a_31).Result;
      a_32 = sc3(2.2, 1.6, 0.1).Result;
      a_29 = engine_math_math::Mat4_mul(a_30, a_32).Result;
      CombatArt_put(self, a_29, CombatArt_Shape::box, kJetDark);
      a_36 = at(vector_make(kern::formula([](Float s) -> Float { return (s * 0.45); }, s), -6.2, -0.05)).Result;
      a_35 = engine_math_math::Mat4_mul(m, a_36).Result;
      a_37 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div((-kPi), 2.0); }, kPi)).Result;
      a_34 = engine_math_math::Mat4_mul(a_35, a_37).Result;
      a_38 = sc3(0.9, 0.9, 1.2).Result;
      a_33 = engine_math_math::Mat4_mul(a_34, a_38).Result;
      CombatArt_put(self, a_33, CombatArt_Shape::cylinder, kJetDark);
    }
    a_41 = at(Vector{0.0, -8.3, -0.05}).Result;
    a_40 = engine_math_math::Mat4_mul(m, a_41).Result;
    a_42 = sc3(0.8, kern::formula([](Float flame) -> Float { return (3.2 * flame); }, flame), 0.8).Result;
    a_39 = engine_math_math::Mat4_mul(a_40, a_42).Result;
    CombatArt_put(self, a_39, CombatArt_Shape::glow_sphere, Vector{3.2, 1.6, 0.5});
  }

  void CombatArt_gate(CombatArt& self, Vector c = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float w = 0.0, Float h = 0.0, Vector color = Vector{0.0, 0.0, 0.0}) {
    Float kPost = 0.0;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
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
    kPost = 0.055;
    a = at(c).Result;
    a_2 = rot(Vector{0.0, 0.0, 1.0}, yaw).Result;
    m = engine_math_math::Mat4_mul(a, a_2).Result;
    a_5 = at(vector_make(0.0, 0.0, kern::formula([](Float h) -> Float { return kern::math::div(h, 2.0); }, h))).Result;
    a_4 = engine_math_math::Mat4_mul(m, a_5).Result;
    a_6 = sc3(kern::formula([](Float w, Float kPost) -> Float { return (w + (kPost * 2.0)); }, w, kPost), kPost, kPost).Result;
    a_3 = engine_math_math::Mat4_mul(a_4, a_6).Result;
    CombatArt_put(self, a_3, CombatArt_Shape::glow_box, color);
    a_9 = at(vector_make(0.0, 0.0, kern::formula([](Float h) -> Float { return kern::math::div((-h), 2.0); }, h))).Result;
    a_8 = engine_math_math::Mat4_mul(m, a_9).Result;
    a_10 = sc3(kern::formula([](Float w, Float kPost) -> Float { return (w + (kPost * 2.0)); }, w, kPost), kPost, kPost).Result;
    a_7 = engine_math_math::Mat4_mul(a_8, a_10).Result;
    CombatArt_put(self, a_7, CombatArt_Shape::glow_box, color);
    a_13 = at(vector_make(kern::formula([](Float w) -> Float { return kern::math::div((-w), 2.0); }, w), 0.0, 0.0)).Result;
    a_12 = engine_math_math::Mat4_mul(m, a_13).Result;
    a_14 = sc3(kPost, kPost, kern::formula([](Float h, Float kPost) -> Float { return (h + (kPost * 2.0)); }, h, kPost)).Result;
    a_11 = engine_math_math::Mat4_mul(a_12, a_14).Result;
    CombatArt_put(self, a_11, CombatArt_Shape::glow_box, color);
    a_17 = at(vector_make(kern::formula([](Float w) -> Float { return kern::math::div(w, 2.0); }, w), 0.0, 0.0)).Result;
    a_16 = engine_math_math::Mat4_mul(m, a_17).Result;
    a_18 = sc3(kPost, kPost, kern::formula([](Float h, Float kPost) -> Float { return (h + (kPost * 2.0)); }, h, kPost)).Result;
    a_15 = engine_math_math::Mat4_mul(a_16, a_18).Result;
    CombatArt_put(self, a_15, CombatArt_Shape::glow_box, color);
    a_22 = at(vector_make(vector_break(c).x, vector_break(c).y, 0.0)).Result;
    a_23 = at(vector_make(kern::formula([](Float w, Float yaw) -> Float { return (kern::math::div(w, 2.0) * kern::math::cos(yaw)); }, w, yaw), kern::formula([](Float w, Float yaw) -> Float { return (kern::math::div(w, 2.0) * kern::math::sin(yaw)); }, w, yaw), 0.0)).Result;
    a_21 = engine_math_math::Mat4_mul(a_22, a_23).Result;
    a_24 = sc3(kPost, kPost, kern::formula([](Float z, Float h) -> Float { return kern::math::max(0.01, (z - kern::math::div(h, 2.0))); }, vector_break(c).z, h)).Result;
    a_20 = engine_math_math::Mat4_mul(a_21, a_24).Result;
    a_25 = at(Vector{0.0, 0.0, 0.5}).Result;
    a_19 = engine_math_math::Mat4_mul(a_20, a_25).Result;
    CombatArt_put(self, a_19, CombatArt_Shape::box, Vector{0.18, 0.25, 0.31});
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct { Int result = 0; } n11;
  struct { Bool result = false; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct { Bool result = false; } n15;
  struct { Bool result = false; Bool changed = false; } n16;
  struct { Bool result = false; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
};
