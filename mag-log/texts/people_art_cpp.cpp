struct people_art_cpp : virtual kern::Library, virtual engine_geo_mesh_data, virtual engine_math_math, virtual people, virtual people_art, virtual ragdoll, virtual weapons {
  // ---- variables ----
  /// vertex locations 14 and 15
  KERN_CONST Int kMaxExtensionAttributes = 2;
  KERN_CONST Float kPi = 3.141592653589793;
  /// hair by the person: black, dark and light brown, blond, auburn, grey
  KERN_CONST Fixed<Vector, 6> kHair = List<Vector>{Vector{0.05, 0.045, 0.04}, Vector{0.18, 0.11, 0.06}, Vector{0.36, 0.24, 0.13}, Vector{0.72, 0.58, 0.32}, Vector{0.45, 0.17, 0.08}, Vector{0.55, 0.55, 0.56}};

  // ---- functions ----
  struct rotResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE rotResult rot(Vector axis = Vector{0.0, 0.0, 0.0}, Float a = 0.0) {
    return {engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(axis, a).Result, Vector{1.0, 1.0, 1.0}).Result};
  }

  struct atResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE atResult at(Vector p = Vector{0.0, 0.0, 0.0}) {
    return {engine_math_math::Mat4_translate(p).Result};
  }

  struct matResult { Material Result = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}; };
  KERN_PURE matResult mat(Float roughness = 0.0, Float metalness = 0.0, Vector emissive = Vector{0.0, 0.0, 0.0}) {
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    m.roughness = roughness;
    m.metalness = metalness;
    m.emissive = emissive;
    return {m};
  }

  struct PeopleArt_makeResult { Bool Result = false; };
  PeopleArt_makeResult PeopleArt_make(PeopleArt_Part& p, const MeshData& mesh, const Material& m, Int r = 0, Bool tinted = false) {
    Int r_ = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool upsert = false;
    r_ = r;
    engine_renderer_allocate_id(n10, r_);
    allocate_id = n10.result;
    p.id = allocate_id;
    p.tinted = tinted;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
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

  struct init_sclResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE init_sclResult init_scl(Vector s = Vector{0.0, 0.0, 0.0}) {
    return {engine_math_math::Mat4_scale(s).Result};
  }

  struct PeopleArt_initResult { Bool Result = false; };
  PeopleArt_initResult PeopleArt_init(PeopleArt& self, Int r = 0) {
    Int r_ = 0;
    Vector white = Vector{0.0, 0.0, 0.0};
    Vector shoe = Vector{0.0, 0.0, 0.0};
    Vector sole = Vector{0.0, 0.0, 0.0};
    MeshData leg = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData sphere = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_3 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData torso = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData capsule_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    List<Float> list = List<Float>{};
    Float y = 0.0;
    MeshData sphere_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_10 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData arm = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_5 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_6 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_11 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData hand = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData sphere_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_13 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Material a_14 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData head_mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData sphere_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_5 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    List<Float> list_2 = List<Float>{};
    Float y_2 = 0.0;
    MeshData box_6 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData sphere_5 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_15 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_16 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_7 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_17 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData hair = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData sphere_6 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_18 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_19 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData sphere_7 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_20 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_21 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Material a_22 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_7 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_8 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_23 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData m_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_8 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_24 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_25 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData capsule_9 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_26 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_27 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    List<Float> list_3 = List<Float>{};
    Float y_3 = 0.0;
    MeshData sphere_8 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_28 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_29 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Material a_30 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData a_31 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_10 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_32 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData m_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_11 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData sphere_9 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_33 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_34 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Material a_35 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData a_36 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_12 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_37 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData m_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_13 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_9 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData sphere_10 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_38 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_39 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_10 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_40 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    Material bm = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData a_41 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Vector gm = Vector{0.0, 0.0, 0.0};
    Vector gd = Vector{0.0, 0.0, 0.0};
    Vector ol = Vector{0.0, 0.0, 0.0};
    MeshData m_5 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_11 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_12 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_42 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_43 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Material a_44 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData s2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_13 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_14 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_15 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_45 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_46 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_16 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_47 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData sn = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_48 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_49 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_17 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_50 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_51 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_18 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_52 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_53 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_19 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_54 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData gl = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder_5 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_55 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_56 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_20 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_57 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_58 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_21 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_59 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData rk = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder_6 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_60 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_61 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData cylinder_7 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_62 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_63 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_22 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_64 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_65 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Material a_66 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData kn = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_23 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_24 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_67 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData vest = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_25 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_26 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_68 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData door = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    List<Float> list_4 = List<Float>{};
    Float y_4 = 0.0;
    MeshData box_27 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_69 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData cap = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder_8 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_28 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_70 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData bar = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_29 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_71 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    Material lamp = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData a_72 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_30 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material flame = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData a_73 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData sphere_11 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData pd = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_14 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Mat4 a_74 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_75 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData box_31 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_32 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    List<Float> list_5 = List<Float>{};
    Float a_76 = 0.0;
    MeshData box_33 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Float a_77 = 0.0;
    MeshData cylinder_9 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData sphere_12 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_78 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    Material disc = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData a_79 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder_10 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData shell = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData capsule_15 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_80 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData pole = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData cylinder_11 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_34 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    Material a_81 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    Material head = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    MeshData a_82 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    MeshData box_35 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    r_ = r;
    self.r_ = r_;
    white = Vector{1.0, 1.0, 1.0};
    shoe = Vector{0.12, 0.1, 0.09};
    sole = Vector{0.05, 0.05, 0.05};
    leg = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    box = engine_primitives_box(Vector{0.25, 0.21, 0.2}, white);
    engine_primitives_append(n12, leg, box, at(Vector{0.0, 0.0, -0.04}).Result);
    leg = n12.dst;
    capsule = engine_primitives_capsule(0.088, 0.34, 12, 6, white);
    engine_primitives_append(n13, leg, capsule, at(Vector{0.0, 0.0, -0.23}).Result);
    leg = n13.dst;
    capsule_2 = engine_primitives_capsule(0.07, 0.36, 12, 6, white);
    engine_primitives_append(n14, leg, capsule_2, at(Vector{0.005, 0.0, -0.62}).Result);
    leg = n14.dst;
    box_2 = engine_primitives_box(Vector{0.22, 0.12, 0.075}, shoe);
    engine_primitives_append(n15, leg, box_2, at(Vector{0.035, 0.0, -0.875}).Result);
    leg = n15.dst;
    sphere = engine_primitives_sphere(0.062, 10, 6, shoe);
    a = at(Vector{0.14, 0.0, -0.88}).Result;
    a_2 = init_scl(Vector{1.1, 0.95, 0.62}).Result;
    engine_primitives_append(n16, leg, sphere, engine_math_math::Mat4_mul(a, a_2).Result);
    leg = n16.dst;
    box_3 = engine_primitives_box(Vector{0.27, 0.13, 0.02}, sole);
    engine_primitives_append(n17, leg, box_3, at(Vector{0.05, 0.0, -0.915}).Result);
    leg = n17.dst;
    a_3 = mat(0.85, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n18 = PeopleArt_make(self.leg_, leg, a_3, r_, true);
    if (logic_not(n18.Result)) {
      return {false};
    }
    torso = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    capsule_3 = engine_primitives_capsule(0.12, 0.16, 14, 6, white);
    a_4 = at(Vector{0.0, 0.0, 0.2}).Result;
    a_5 = init_scl(Vector{0.95, 1.6, 1.0}).Result;
    engine_primitives_append(n19, torso, capsule_3, engine_math_math::Mat4_mul(a_4, a_5).Result);
    torso = n19.dst;
    capsule_4 = engine_primitives_capsule(0.13, 0.14, 14, 6, white);
    a_6 = at(Vector{0.005, 0.0, 0.42}).Result;
    a_7 = init_scl(Vector{0.98, 1.62, 1.0}).Result;
    engine_primitives_append(n20, torso, capsule_4, engine_math_math::Mat4_mul(a_6, a_7).Result);
    torso = n20.dst;
    list = list_make<Float, "2">(0.225, -0.225);
    for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(list))))); it21.next();) {
      y = list[n21.index];
      sphere_2 = engine_primitives_sphere(0.066, 10, 6, white);
      a_8 = at(vector_make(0.0, y, 0.535)).Result;
      a_9 = init_scl(Vector{1.0, 1.0, 0.85}).Result;
      engine_primitives_append(n22, torso, sphere_2, engine_math_math::Mat4_mul(a_8, a_9).Result);
      torso = n22.dst;
    }
    box_4 = engine_primitives_box(Vector{0.2, 0.36, 0.05}, Vector{0.55, 0.55, 0.55});
    engine_primitives_append(n23, torso, box_4, at(Vector{0.0, 0.0, 0.03}).Result);
    torso = n23.dst;
    a_10 = mat(0.8, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n24 = PeopleArt_make(self.torso_, torso, a_10, r_, true);
    if (logic_not(n24.Result)) {
      return {false};
    }
    arm = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    capsule_5 = engine_primitives_capsule(0.056, 0.24, 10, 6, white);
    engine_primitives_append(n25, arm, capsule_5, at(Vector{0.0, 0.0, -0.15}).Result);
    arm = n25.dst;
    capsule_6 = engine_primitives_capsule(0.047, 0.22, 10, 6, white);
    engine_primitives_append(n26, arm, capsule_6, at(Vector{0.0, 0.0, -0.42}).Result);
    arm = n26.dst;
    a_11 = mat(0.8, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n27 = PeopleArt_make(self.arm_, arm, a_11, r_, true);
    if (logic_not(n27.Result)) {
      return {false};
    }
    hand = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    sphere_3 = engine_primitives_sphere(0.052, 10, 6, white);
    a_12 = at(Vector{0.0, 0.0, -0.6}).Result;
    a_13 = init_scl(Vector{0.75, 0.55, 1.15}).Result;
    engine_primitives_append(n28, hand, sphere_3, engine_math_math::Mat4_mul(a_12, a_13).Result);
    hand = n28.dst;
    a_14 = mat(0.6, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n29 = PeopleArt_make(self.hand_, hand, a_14, r_, true);
    if (logic_not(n29.Result)) {
      return {false};
    }
    head_mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    sphere_4 = engine_primitives_sphere(0.115, 16, 10, white);
    engine_primitives_append(n30, head_mesh, sphere_4, init_scl(Vector{0.94, 0.88, 1.05}).Result);
    head_mesh = n30.dst;
    cylinder = engine_primitives_cylinder(0.052, 0.13, 10, white);
    engine_primitives_append(n31, head_mesh, cylinder, at(Vector{-0.01, 0.0, -0.205}).Result);
    head_mesh = n31.dst;
    box_5 = engine_primitives_box(Vector{0.045, 0.03, 0.05}, white);
    engine_primitives_append(n32, head_mesh, box_5, at(Vector{0.11, 0.0, -0.015}).Result);
    head_mesh = n32.dst;
    list_2 = list_make<Float, "2">(0.038, -0.038);
    for (auto it33 = flow_for(n33, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(list_2))))); it33.next();) {
      y_2 = list_2[n33.index];
      box_6 = engine_primitives_box(Vector{0.012, 0.026, 0.02}, Vector{0.06, 0.05, 0.05});
      engine_primitives_append(n34, head_mesh, box_6, at(vector_make(0.104, y_2, 0.012)).Result);
      head_mesh = n34.dst;
      sphere_5 = engine_primitives_sphere(0.026, 6, 4, white);
      a_15 = at(vector_make(-0.005, kern::formula([](Float y_2) -> Float { return (y_2 * 2.65); }, y_2), 0.0)).Result;
      a_16 = init_scl(Vector{0.6, 0.5, 1.0}).Result;
      engine_primitives_append(n35, head_mesh, sphere_5, engine_math_math::Mat4_mul(a_15, a_16).Result);
      head_mesh = n35.dst;
    }
    box_7 = engine_primitives_box(Vector{0.012, 0.05, 0.012}, Vector{0.45, 0.28, 0.26});
    engine_primitives_append(n36, head_mesh, box_7, at(Vector{0.104, 0.0, -0.06}).Result);
    head_mesh = n36.dst;
    a_17 = mat(0.6, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n37 = PeopleArt_make(self.head_, head_mesh, a_17, r_, true);
    if (logic_not(n37.Result)) {
      return {false};
    }
    hair = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    sphere_6 = engine_primitives_sphere(0.123, 14, 8, white);
    a_18 = at(Vector{-0.018, 0.0, 0.088}).Result;
    a_19 = init_scl(Vector{1.0, 0.95, 0.5}).Result;
    engine_primitives_append(n38, hair, sphere_6, engine_math_math::Mat4_mul(a_18, a_19).Result);
    hair = n38.dst;
    sphere_7 = engine_primitives_sphere(0.118, 14, 8, white);
    a_20 = at(Vector{-0.04, 0.0, 0.0}).Result;
    a_21 = init_scl(Vector{0.82, 0.93, 1.0}).Result;
    engine_primitives_append(n39, hair, sphere_7, engine_math_math::Mat4_mul(a_20, a_21).Result);
    hair = n39.dst;
    a_22 = mat(0.9, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n40 = PeopleArt_make(self.hair_, hair, a_22, r_, true);
    if (logic_not(n40.Result)) {
      return {false};
    }
    m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    capsule_7 = engine_primitives_capsule(0.115, 0.11, 14, 6, white);
    engine_primitives_append(n41, m, capsule_7, rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result);
    m = n41.dst;
    box_8 = engine_primitives_box(Vector{0.2, 0.34, 0.05}, Vector{0.55, 0.55, 0.55});
    engine_primitives_append(n42, m, box_8, at(Vector{0.0, 0.0, 0.08}).Result);
    m = n42.dst;
    a_23 = mat(0.85, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n43 = PeopleArt_make(self.rd_pelvis_, m, a_23, r_, true);
    if (logic_not(n43.Result)) {
      return {false};
    }
    m_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    capsule_8 = engine_primitives_capsule(0.12, 0.16, 14, 6, white);
    a_24 = at(Vector{0.0, 0.0, -0.1}).Result;
    a_25 = init_scl(Vector{0.95, 1.6, 1.0}).Result;
    engine_primitives_append(n44, m_2, capsule_8, engine_math_math::Mat4_mul(a_24, a_25).Result);
    m_2 = n44.dst;
    capsule_9 = engine_primitives_capsule(0.13, 0.12, 14, 6, white);
    a_26 = at(Vector{0.005, 0.0, 0.1}).Result;
    a_27 = init_scl(Vector{0.98, 1.62, 1.0}).Result;
    engine_primitives_append(n45, m_2, capsule_9, engine_math_math::Mat4_mul(a_26, a_27).Result);
    m_2 = n45.dst;
    list_3 = list_make<Float, "2">(0.225, -0.225);
    for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(list_3))))); it46.next();) {
      y_3 = list_3[n46.index];
      sphere_8 = engine_primitives_sphere(0.066, 10, 6, white);
      a_28 = at(vector_make(0.0, y_3, 0.2)).Result;
      a_29 = init_scl(Vector{1.0, 1.0, 0.85}).Result;
      engine_primitives_append(n47, m_2, sphere_8, engine_math_math::Mat4_mul(a_28, a_29).Result);
      m_2 = n47.dst;
    }
    a_30 = mat(0.8, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n48 = PeopleArt_make(self.rd_chest_, m_2, a_30, r_, true);
    if (logic_not(n48.Result)) {
      return {false};
    }
    capsule_10 = engine_primitives_capsule(0.056, 0.22, 10, 6, white);
    a_31 = capsule_10;
    a_32 = mat(0.8, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n49 = PeopleArt_make(self.rd_upper_, a_31, a_32, r_, true);
    if (logic_not(n49.Result)) {
      return {false};
    }
    m_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    capsule_11 = engine_primitives_capsule(0.047, 0.22, 10, 6, white);
    engine_primitives_append(n50, m_3, capsule_11, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    m_3 = n50.dst;
    sphere_9 = engine_primitives_sphere(0.052, 10, 6, Vector{0.82, 0.66, 0.53});
    a_33 = at(Vector{0.0, 0.0, -0.2}).Result;
    a_34 = init_scl(Vector{0.75, 0.55, 1.15}).Result;
    engine_primitives_append(n51, m_3, sphere_9, engine_math_math::Mat4_mul(a_33, a_34).Result);
    m_3 = n51.dst;
    a_35 = mat(0.7, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n52 = PeopleArt_make(self.rd_fore_, m_3, a_35, r_, true);
    if (logic_not(n52.Result)) {
      return {false};
    }
    capsule_12 = engine_primitives_capsule(0.088, 0.32, 12, 6, white);
    a_36 = capsule_12;
    a_37 = mat(0.85, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n53 = PeopleArt_make(self.rd_thigh_, a_36, a_37, r_, true);
    if (logic_not(n53.Result)) {
      return {false};
    }
    m_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    capsule_13 = engine_primitives_capsule(0.07, 0.34, 12, 6, white);
    engine_primitives_append(n54, m_4, capsule_13, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    m_4 = n54.dst;
    box_9 = engine_primitives_box(Vector{0.22, 0.12, 0.075}, shoe);
    engine_primitives_append(n55, m_4, box_9, at(Vector{0.035, 0.0, -0.205}).Result);
    m_4 = n55.dst;
    sphere_10 = engine_primitives_sphere(0.062, 10, 6, shoe);
    a_38 = at(Vector{0.14, 0.0, -0.21}).Result;
    a_39 = init_scl(Vector{1.1, 0.95, 0.62}).Result;
    engine_primitives_append(n56, m_4, sphere_10, engine_math_math::Mat4_mul(a_38, a_39).Result);
    m_4 = n56.dst;
    box_10 = engine_primitives_box(Vector{0.27, 0.13, 0.02}, sole);
    engine_primitives_append(n57, m_4, box_10, at(Vector{0.05, 0.0, -0.245}).Result);
    m_4 = n57.dst;
    a_40 = mat(0.85, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n58 = PeopleArt_make(self.rd_shin_, m_4, a_40, r_, true);
    if (logic_not(n58.Result)) {
      return {false};
    }
    bm = mat(0.3, 0.0, Vector{1.6, 1.6, 1.6}).Result;
    bm.unlit = true;
    bm.cast_shadow = false;
    cylinder_2 = engine_primitives_cylinder(1.0, 1.0, 10, white);
    a_41 = cylinder_2;
    n59 = PeopleArt_make(self.beacon_, a_41, bm, r_, true);
    if (logic_not(n59.Result)) {
      return {false};
    }
    gm = Vector{0.09, 0.09, 0.1};
    gd = Vector{0.2, 0.21, 0.23};
    ol = Vector{0.26, 0.3, 0.2};
    m_5 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    box_11 = engine_primitives_box(Vector{0.17, 0.035, 0.045}, gm);
    engine_primitives_append(n60, m_5, box_11, at(Vector{0.06, 0.0, 0.03}).Result);
    m_5 = n60.dst;
    box_12 = engine_primitives_box(Vector{0.04, 0.032, 0.1}, gd);
    a_42 = at(Vector{-0.005, 0.0, -0.03}).Result;
    a_43 = rot(Vector{0.0, 1.0, 0.0}, -0.25).Result;
    engine_primitives_append(n61, m_5, box_12, engine_math_math::Mat4_mul(a_42, a_43).Result);
    m_5 = n61.dst;
    a_44 = mat(0.45, 0.4, Vector{0.0, 0.0, 0.0}).Result;
    n62 = PeopleArt_make(self.gun_pistol_, m_5, a_44, r_, false);
    if (logic_not(n62.Result)) {
      return {false};
    }
    s2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    box_13 = engine_primitives_box(Vector{0.42, 0.05, 0.08}, gm);
    engine_primitives_append(n63, s2, box_13, at(Vector{0.13, 0.0, 0.03}).Result);
    s2 = n63.dst;
    box_14 = engine_primitives_box(Vector{0.045, 0.04, 0.16}, gd);
    engine_primitives_append(n64, s2, box_14, at(Vector{0.12, 0.0, -0.07}).Result);
    s2 = n64.dst;
    box_15 = engine_primitives_box(Vector{0.04, 0.035, 0.1}, gd);
    a_45 = at(Vector{-0.01, 0.0, -0.03}).Result;
    a_46 = rot(Vector{0.0, 1.0, 0.0}, -0.25).Result;
    engine_primitives_append(n65, s2, box_15, engine_math_math::Mat4_mul(a_45, a_46).Result);
    s2 = n65.dst;
    box_16 = engine_primitives_box(Vector{0.16, 0.04, 0.06}, gd);
    engine_primitives_append(n66, s2, box_16, at(Vector{-0.14, 0.0, 0.02}).Result);
    s2 = n66.dst;
    a_47 = mat(0.45, 0.4, Vector{0.0, 0.0, 0.0}).Result;
    n67 = PeopleArt_make(self.gun_smg_, s2, a_47, r_, false);
    if (logic_not(n67.Result)) {
      return {false};
    }
    sn = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    cylinder_3 = engine_primitives_cylinder(0.012, 0.75, 8, gm);
    a_48 = at(Vector{0.18, 0.0, 0.04}).Result;
    a_49 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
    engine_primitives_append(n68, sn, cylinder_3, engine_math_math::Mat4_mul(a_48, a_49).Result);
    sn = n68.dst;
    box_17 = engine_primitives_box(Vector{0.42, 0.055, 0.08}, ol);
    engine_primitives_append(n69, sn, box_17, at(Vector{0.0, 0.0, 0.02}).Result);
    sn = n69.dst;
    cylinder_4 = engine_primitives_cylinder(0.025, 0.26, 10, gd);
    a_50 = at(Vector{-0.06, 0.0, 0.1}).Result;
    a_51 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
    engine_primitives_append(n70, sn, cylinder_4, engine_math_math::Mat4_mul(a_50, a_51).Result);
    sn = n70.dst;
    box_18 = engine_primitives_box(Vector{0.04, 0.035, 0.1}, gd);
    a_52 = at(Vector{-0.03, 0.0, -0.04}).Result;
    a_53 = rot(Vector{0.0, 1.0, 0.0}, -0.25).Result;
    engine_primitives_append(n71, sn, box_18, engine_math_math::Mat4_mul(a_52, a_53).Result);
    sn = n71.dst;
    box_19 = engine_primitives_box(Vector{0.22, 0.045, 0.09}, ol);
    engine_primitives_append(n72, sn, box_19, at(Vector{-0.3, 0.0, 0.0}).Result);
    sn = n72.dst;
    a_54 = mat(0.5, 0.3, Vector{0.0, 0.0, 0.0}).Result;
    n73 = PeopleArt_make(self.gun_sniper_, sn, a_54, r_, false);
    if (logic_not(n73.Result)) {
      return {false};
    }
    gl = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    cylinder_5 = engine_primitives_cylinder(0.045, 0.42, 12, ol);
    a_55 = at(Vector{-0.08, 0.0, 0.05}).Result;
    a_56 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
    engine_primitives_append(n74, gl, cylinder_5, engine_math_math::Mat4_mul(a_55, a_56).Result);
    gl = n74.dst;
    box_20 = engine_primitives_box(Vector{0.04, 0.035, 0.1}, gd);
    a_57 = at(Vector{0.0, 0.0, -0.03}).Result;
    a_58 = rot(Vector{0.0, 1.0, 0.0}, -0.25).Result;
    engine_primitives_append(n75, gl, box_20, engine_math_math::Mat4_mul(a_57, a_58).Result);
    gl = n75.dst;
    box_21 = engine_primitives_box(Vector{0.18, 0.05, 0.08}, gd);
    engine_primitives_append(n76, gl, box_21, at(Vector{-0.2, 0.0, 0.02}).Result);
    gl = n76.dst;
    a_59 = mat(0.55, 0.2, Vector{0.0, 0.0, 0.0}).Result;
    n77 = PeopleArt_make(self.gun_tube_, gl, a_59, r_, false);
    if (logic_not(n77.Result)) {
      return {false};
    }
    rk = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    cylinder_6 = engine_primitives_cylinder(0.06, 1.0, 12, ol);
    a_60 = at(Vector{-0.45, 0.0, 0.09}).Result;
    a_61 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
    engine_primitives_append(n78, rk, cylinder_6, engine_math_math::Mat4_mul(a_60, a_61).Result);
    rk = n78.dst;
    cylinder_7 = engine_primitives_cylinder(0.075, 0.12, 12, gd);
    a_62 = at(Vector{0.43, 0.0, 0.09}).Result;
    a_63 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
    engine_primitives_append(n79, rk, cylinder_7, engine_math_math::Mat4_mul(a_62, a_63).Result);
    rk = n79.dst;
    box_22 = engine_primitives_box(Vector{0.04, 0.035, 0.1}, gd);
    a_64 = at(Vector{0.0, 0.0, -0.01}).Result;
    a_65 = rot(Vector{0.0, 1.0, 0.0}, -0.25).Result;
    engine_primitives_append(n80, rk, box_22, engine_math_math::Mat4_mul(a_64, a_65).Result);
    rk = n80.dst;
    a_66 = mat(0.6, 0.15, Vector{0.0, 0.0, 0.0}).Result;
    n81 = PeopleArt_make(self.gun_rocket_, rk, a_66, r_, false);
    if (logic_not(n81.Result)) {
      return {false};
    }
    kn = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    box_23 = engine_primitives_box(Vector{0.17, 0.006, 0.032}, Vector{0.75, 0.77, 0.8});
    engine_primitives_append(n82, kn, box_23, at(Vector{0.11, 0.0, 0.0}).Result);
    kn = n82.dst;
    box_24 = engine_primitives_box(Vector{0.1, 0.025, 0.035}, gd);
    engine_primitives_append(n83, kn, box_24, at(Vector{-0.02, 0.0, 0.0}).Result);
    kn = n83.dst;
    a_67 = mat(0.3, 0.7, Vector{0.0, 0.0, 0.0}).Result;
    n84 = PeopleArt_make(self.gun_knife_, kn, a_67, r_, false);
    if (logic_not(n84.Result)) {
      return {false};
    }
    vest = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    box_25 = engine_primitives_box(Vector{0.29, 0.45, 0.36}, Vector{0.082, 0.094, 0.118});
    engine_primitives_append(n85, vest, box_25, at(Vector{0.005, 0.0, 0.36}).Result);
    vest = n85.dst;
    box_26 = engine_primitives_box(Vector{0.02, 0.3, 0.07}, Vector{0.85, 0.86, 0.88});
    engine_primitives_append(n86, vest, box_26, at(Vector{0.15, 0.0, 0.44}).Result);
    vest = n86.dst;
    a_68 = mat(0.7, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n87 = PeopleArt_make(self.vest_, vest, a_68, r_, false);
    if (logic_not(n87.Result)) {
      return {false};
    }
    door = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    list_4 = list_make<Float, "2">(0.835, -0.835);
    for (auto it88 = flow_for(n88, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(list_4))))); it88.next();) {
      y_4 = list_4[n88.index];
      box_27 = engine_primitives_box(Vector{1.7, 0.02, 0.42}, Vector{0.933, 0.941, 0.949});
      engine_primitives_append(n89, door, box_27, at(vector_make(-0.05, y_4, 0.02)).Result);
      door = n89.dst;
    }
    a_69 = mat(0.4, 0.2, Vector{0.0, 0.0, 0.0}).Result;
    n90 = PeopleArt_make(self.door_, door, a_69, r_, false);
    if (logic_not(n90.Result)) {
      return {false};
    }
    cap = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    cylinder_8 = engine_primitives_cylinder(0.125, 0.07, 12, Vector{0.08, 0.1, 0.16});
    engine_primitives_append(n91, cap, cylinder_8, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    cap = n91.dst;
    box_28 = engine_primitives_box(Vector{0.12, 0.2, 0.015}, Vector{0.05, 0.05, 0.06});
    engine_primitives_append(n92, cap, box_28, at(Vector{0.12, 0.0, 0.005}).Result);
    cap = n92.dst;
    a_70 = mat(0.5, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n93 = PeopleArt_make(self.cap_, cap, a_70, r_, false);
    if (logic_not(n93.Result)) {
      return {false};
    }
    bar = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    box_29 = engine_primitives_box(Vector{0.26, 1.1, 0.09}, Vector{0.08, 0.08, 0.09});
    engine_primitives_append(n94, bar, box_29, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    bar = n94.dst;
    a_71 = mat(0.4, 0.3, Vector{0.0, 0.0, 0.0}).Result;
    n95 = PeopleArt_make(self.bar_, bar, a_71, r_, false);
    if (logic_not(n95.Result)) {
      return {false};
    }
    lamp = mat(0.3, 0.0, Vector{2.4, 2.4, 2.4}).Result;
    lamp.unlit = true;
    box_30 = engine_primitives_box(Vector{0.22, 0.42, 0.1}, white);
    a_72 = box_30;
    n96 = PeopleArt_make(self.lamp_, a_72, lamp, r_, true);
    if (logic_not(n96.Result)) {
      return {false};
    }
    flame = mat(0.9, 0.0, Vector{4.0, 1.6, 0.3}).Result;
    flame.unlit = true;
    flame.cast_shadow = false;
    sphere_11 = engine_primitives_sphere(1.0, 10, 6, white);
    a_73 = sphere_11;
    n97 = PeopleArt_make(self.flame_, a_73, flame, r_, true);
    if (logic_not(n97.Result)) {
      return {false};
    }
    pd = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    capsule_14 = engine_primitives_capsule(0.12, 0.2, 14, 6, Vector{0.9, 0.91, 0.93});
    a_74 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
    a_75 = engine_math_math::Mat4_scale(Vector{1.0, 1.15, 0.62}).Result;
    engine_primitives_append(n98, pd, capsule_14, engine_math_math::Mat4_mul(a_74, a_75).Result);
    pd = n98.dst;
    box_31 = engine_primitives_box(Vector{0.36, 0.22, 0.05}, Vector{0.06, 0.065, 0.075});
    engine_primitives_append(n99, pd, box_31, at(Vector{0.0, 0.0, -0.06}).Result);
    pd = n99.dst;
    box_32 = engine_primitives_box(Vector{0.38, 0.235, 0.035}, Vector{0.12, 0.3, 0.85});
    engine_primitives_append(n100, pd, box_32, at(Vector{0.0, 0.0, 0.02}).Result);
    pd = n100.dst;
    list_5 = list_make<Float, "2">(kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 4.0); }, kPi), kern::formula([](Float kPi) -> Float { return kern::math::div((-kPi), 4.0); }, kPi));
    for (auto it101 = flow_for(n101, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(list_5))))); it101.next();) {
      a_76 = list_5[n101.index];
      box_33 = engine_primitives_box(Vector{0.9, 0.045, 0.035}, Vector{0.07, 0.07, 0.08});
      engine_primitives_append(n102, pd, box_33, rot(Vector{0.0, 0.0, 1.0}, a_76).Result);
      pd = n102.dst;
    }
    for (auto it103 = flow_for(n103, 0, 3); it103.next();) {
      a_77 = kern::formula([](Float kPi, Float index) -> Float { return (kern::math::div(kPi, 4.0) + kern::math::div((index * kPi), 2.0)); }, kPi, kern::to<Float>(n103.index));
      cylinder_9 = engine_primitives_cylinder(0.045, 0.06, 10, Vector{0.08, 0.08, 0.09});
      engine_primitives_append(n104, pd, cylinder_9, at(vector_make(kern::formula([](Float a_77) -> Float { return (kern::math::cos(a_77) * 0.32); }, a_77), kern::formula([](Float a_77) -> Float { return (kern::math::sin(a_77) * 0.32); }, a_77), -0.02)).Result);
      pd = n104.dst;
    }
    sphere_12 = engine_primitives_sphere(0.04, 10, 6, Vector{0.05, 0.06, 0.08});
    engine_primitives_append(n105, pd, sphere_12, at(Vector{-0.19, 0.0, -0.03}).Result);
    pd = n105.dst;
    a_78 = mat(0.35, 0.05, Vector{0.0, 0.0, 0.0}).Result;
    n106 = PeopleArt_make(self.pd_body_, pd, a_78, r_, false);
    if (logic_not(n106.Result)) {
      return {false};
    }
    disc = mat(0.6, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    disc.color = engine_math_math::Vec4_new(1.0, 1.0, 1.0, 0.5).Result;
    disc.cast_shadow = false;
    cylinder_10 = engine_primitives_cylinder(0.15, 0.01, 14, Vector{0.25, 0.25, 0.27});
    a_79 = cylinder_10;
    n107 = PeopleArt_make(self.pd_rotor_, a_79, disc, r_, false);
    if (logic_not(n107.Result)) {
      return {false};
    }
    shell = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    capsule_15 = engine_primitives_capsule(1.0, 2.0, 10, 5, Vector{0.25, 0.27, 0.22});
    engine_primitives_append(n108, shell, capsule_15, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    shell = n108.dst;
    a_80 = mat(0.6, 0.3, Vector{0.0, 0.0, 0.0}).Result;
    n109 = PeopleArt_make(self.shell_, shell, a_80, r_, false);
    if (logic_not(n109.Result)) {
      return {false};
    }
    pole = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    cylinder_11 = engine_primitives_cylinder(0.07, 6.0, 8, Vector{0.22, 0.24, 0.26});
    engine_primitives_append(n110, pole, cylinder_11, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    pole = n110.dst;
    box_34 = engine_primitives_box(Vector{1.3, 0.07, 0.07}, Vector{0.22, 0.24, 0.26});
    engine_primitives_append(n111, pole, box_34, at(Vector{0.62, 0.0, 5.95}).Result);
    pole = n111.dst;
    a_81 = mat(0.5, 0.6, Vector{0.0, 0.0, 0.0}).Result;
    n112 = PeopleArt_make(self.pole_, pole, a_81, r_, false);
    if (logic_not(n112.Result)) {
      return {false};
    }
    head = mat(0.3, 0.0, Vector{3.2, 2.7, 1.8}).Result;
    head.unlit = true;
    head.cast_shadow = false;
    box_35 = engine_primitives_box(Vector{0.42, 0.2, 0.08}, white);
    a_82 = box_35;
    n113 = PeopleArt_make(self.lamp_head_, a_82, head, r_, true);
    if (logic_not(n113.Result)) {
      return {false};
    }
    return {true};
  }

  void PeopleArt_put(PeopleArt_Part& p, const Mat4& m, Vector tint = Vector{0.0, 0.0, 0.0}) {
    Int place = 0;
    Int place_2 = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    place = list_length<Float>(p.transforms);
    list_insert_list(p.transforms, place, m.m);
    if (p.tinted) {
      place_2 = list_length<Float>(p.tints);
      item = vector_break(tint).x;
      item_2 = vector_break(tint).y;
      item_3 = vector_break(tint).z;
      item_4 = 1.0;
      list_insert_list(p.tints, place_2, list_make<Float, "4">(item, item_2, item_3, item_4));
    }
  }

  void PeopleArt_street_lamp(PeopleArt& self, Vector foot = Vector{0.0, 0.0, 0.0}, Float arm_yaw = 0.0, Float night = 0.0) {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float k = 0.0;
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a = at(foot).Result;
    a_2 = rot(Vector{0.0, 0.0, 1.0}, arm_yaw).Result;
    m = engine_math_math::Mat4_mul(a, a_2).Result;
    PeopleArt_put(self.pole_, m, Vector{1.0, 1.0, 1.0});
    k = kern::formula([](Float night) -> Float { return (0.18 + (0.82 * (night < 0.0 ? 0.0 : (1.0 < night ? 1.0 : night)))); }, night);
    a_4 = at(Vector{1.2, 0.0, 5.88}).Result;
    a_3 = engine_math_math::Mat4_mul(m, a_4).Result;
    PeopleArt_put(self.lamp_head_, a_3, vector_make(k, kern::formula([](Float k) -> Float { return (k * 0.92); }, k), kern::formula([](Float k) -> Float { return (k * 0.75); }, k)));
  }

  void PeopleArt_clear(PeopleArt_Part& p) {
    list_clear(p.transforms);
    list_clear(p.tints);
  }

  void PeopleArt_begin(PeopleArt& self) {
    PeopleArt_clear(self.leg_);
    PeopleArt_clear(self.torso_);
    PeopleArt_clear(self.arm_);
    PeopleArt_clear(self.head_);
    PeopleArt_clear(self.hand_);
    PeopleArt_clear(self.hair_);
    PeopleArt_clear(self.vest_);
    PeopleArt_clear(self.door_);
    PeopleArt_clear(self.cap_);
    PeopleArt_clear(self.bar_);
    PeopleArt_clear(self.lamp_);
    PeopleArt_clear(self.flame_);
    PeopleArt_clear(self.pd_body_);
    PeopleArt_clear(self.pd_rotor_);
    PeopleArt_clear(self.shell_);
    PeopleArt_clear(self.pole_);
    PeopleArt_clear(self.lamp_head_);
    PeopleArt_clear(self.rd_pelvis_);
    PeopleArt_clear(self.rd_chest_);
    PeopleArt_clear(self.rd_upper_);
    PeopleArt_clear(self.rd_fore_);
    PeopleArt_clear(self.rd_thigh_);
    PeopleArt_clear(self.rd_shin_);
    PeopleArt_clear(self.beacon_);
    PeopleArt_clear(self.gun_pistol_);
    PeopleArt_clear(self.gun_smg_);
    PeopleArt_clear(self.gun_sniper_);
    PeopleArt_clear(self.gun_tube_);
    PeopleArt_clear(self.gun_rocket_);
    PeopleArt_clear(self.gun_knife_);
  }

  struct person_reachResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  /// a VR mate: each arm from its shoulder to his real hand (the arm 0.6 m: stretched or bent within 0.5 .. 1.15 of it)
  KERN_PURE person_reachResult person_reach(const Mat4& shoulder, const People_Person& p, Float side_y = 0.0, Vector hand = Vector{0.0, 0.0, 0.0}) {
    Vector s0 = Vector{0.0, 0.0, 0.0};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Vector z = Vector{0.0, 0.0, 0.0};
    Vector fwd = Vector{0.0, 0.0, 0.0};
    Vector x = Vector{0.0, 0.0, 0.0};
    Vector y = Vector{0.0, 0.0, 0.0};
    Float k = 0.0;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a_2 = at(vector_make(0.0, side_y, 0.0)).Result;
    a = engine_math_math::Mat4_mul(shoulder, a_2).Result;
    s0 = engine_math_math::Mat4_translation(a).Result;
    d = vector_sub(hand, s0);
    len = vector_length(d);
    z = logic_select<Vector>(math_compare<">">(len, 0.001), vector_scale(d, kern::formula([](Float len) -> Float { return kern::math::div((-1.0), len); }, len)), Vector{0.0, 0.0, 1.0});
    fwd = vector_make(kern::formula([](Float yaw) -> Float { return kern::math::cos(yaw); }, p.yaw), kern::formula([](Float yaw) -> Float { return kern::math::sin(yaw); }, p.yaw), 0.0);
    x = logic_select<Vector>(math_compare<">">(vector_length(vector_cross(Vector{0.0, 0.0, 1.0}, z)), 9.999999717180685e-10), vector_normalize(vector_cross(Vector{0.0, 0.0, 1.0}, z)), fwd);
    if (math_compare<"<">(vector_length(vector_cross(Vector{0.0, 0.0, 1.0}, z)), 0.001)) {
      x = fwd;
    }
    y = vector_cross(z, x);
    k = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float len) -> Float { return kern::math::div(len, 0.6); }, len), 0.5, 1.15));
    m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    m.m[0] = vector_break(x).x;
    m.m[1] = vector_break(x).y;
    m.m[2] = vector_break(x).z;
    m.m[3] = 0.0;
    m.m[4] = vector_break(y).x;
    m.m[5] = vector_break(y).y;
    m.m[6] = vector_break(y).z;
    m.m[7] = 0.0;
    m.m[8] = kern::formula([](Float x, Float k) -> Float { return (x * k); }, vector_break(z).x, k);
    m.m[9] = kern::formula([](Float y, Float k) -> Float { return (y * k); }, vector_break(z).y, k);
    m.m[10] = kern::formula([](Float z, Float k) -> Float { return (z * k); }, vector_break(z).z, k);
    m.m[11] = 0.0;
    m.m[12] = vector_break(s0).x;
    m.m[13] = vector_break(s0).y;
    m.m[14] = vector_break(s0).z;
    m.m[15] = 1.0;
    return {m};
  }

  struct PeopleArt_hair_colorResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE PeopleArt_hair_colorResult PeopleArt_hair_color(const People_Person& p) {
    if (enum_equal<People_Kind>(p.kind, People_Kind::zombie)) {
      return {Vector{0.2, 0.22, 0.18}};
    }
    return {list_get<Vector>(kHair, kern::to<Int>(kern::formula([](Float seed) -> Float { return kern::math::mod(kern::math::shr(seed, 5.0), 6.0); }, kern::to<Float>(p.seed))))};
  }

  void PeopleArt_person(PeopleArt& self, const People_Person& p, Float now = 0.0) {
    Mat4 base = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool down = false;
    Float fall = 0.0;
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float pace = 0.0;
    Float swing = 0.0;
    Float lean = 0.0;
    Mat4 hip = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_13 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_14 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_15 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_16 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_17 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 body = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_18 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 shoulder = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_19 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool hands_up = false;
    Mat4 arm_l = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 arm_r = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_20 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_21 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_22 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_23 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_24 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_25 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float ap = 0.0;
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
    Bool r = false;
    Bool l = false;
    Bool slash = false;
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
    Mat4 a_47 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_48 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 chosen = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
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
    Mat4 chosen_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_62 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_63 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_64 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_65 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_66 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_67 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_68 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_69 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_70 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_71 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 head = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_72 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_73 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_74 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    weapons_Id w = weapons_Id::smg;
    Float ap_2 = 0.0;
    Mat4 g = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_75 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_76 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_77 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_78 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_79 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_80 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_81 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_82 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_83 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_84 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_85 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_86 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_87 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_88 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 chosen_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_89 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_90 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (enum_equal<People_State>(p.state, People_State::gone)) {
      return;
    }
    a = at(p.pos).Result;
    a_2 = rot(Vector{0.0, 0.0, 1.0}, p.yaw).Result;
    base = engine_math_math::Mat4_mul(a, a_2).Result;
    down = enum_equal<People_State>(p.state, People_State::down);
    if (down) {
      fall = kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(p.fall_dir).y, vector_break(p.fall_dir).x);
      a_5 = at(vector_add(p.pos, Vector{0.0, 0.0, 0.15})).Result;
      a_6 = rot(Vector{0.0, 0.0, 1.0}, fall).Result;
      a_4 = engine_math_math::Mat4_mul(a_5, a_6).Result;
      a_7 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
      a_3 = engine_math_math::Mat4_mul(a_4, a_7).Result;
      a_8 = at(Vector{0.0, 0.0, -0.95}).Result;
      base = engine_math_math::Mat4_mul(a_3, a_8).Result;
    }
    pace = kern::formula([](Float down, Float a) -> Float { return ((down != 0.0) ? 0.0 : a); }, kern::to<Float>(down), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float v) -> Float { return kern::math::div(v, 1.4); }, p.v), 0.0, 2.2));
    swing = kern::formula([](Float phase, Float a, Float pace) -> Float { return ((kern::math::sin(phase) * a) * ((pace > 0.05) ? 1.0 : 0.0)); }, p.phase, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float pace) -> Float { return (0.25 + (0.18 * pace)); }, pace), 0.0, 0.75), pace);
    lean = kern::formula([](Float down, Float a) -> Float { return ((down != 0.0) ? 0.0 : (a * 0.035)); }, kern::to<Float>(down), kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float v) -> Float { return (v - 2.5); }, p.v), 0.0, 5.0));
    a_9 = at(Vector{0.0, 0.0, 0.92}).Result;
    hip = engine_math_math::Mat4_mul(base, a_9).Result;
    a_12 = at(Vector{0.0, 0.1, 0.0}).Result;
    a_11 = engine_math_math::Mat4_mul(hip, a_12).Result;
    a_13 = rot(Vector{0.0, 1.0, 0.0}, swing).Result;
    a_10 = engine_math_math::Mat4_mul(a_11, a_13).Result;
    PeopleArt_put(self.leg_, a_10, p.pants);
    a_16 = at(Vector{0.0, -0.1, 0.0}).Result;
    a_15 = engine_math_math::Mat4_mul(hip, a_16).Result;
    a_17 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float swing) -> Float { return (-swing); }, swing)).Result;
    a_14 = engine_math_math::Mat4_mul(a_15, a_17).Result;
    PeopleArt_put(self.leg_, a_14, p.pants);
    a_18 = rot(Vector{0.0, 1.0, 0.0}, lean).Result;
    body = engine_math_math::Mat4_mul(hip, a_18).Result;
    PeopleArt_put(self.torso_, body, p.shirt);
    a_19 = at(Vector{0.0, 0.0, 0.56}).Result;
    shoulder = engine_math_math::Mat4_mul(body, a_19).Result;
    hands_up = logic_and(logic_and(logic_not(down), enum_equal<People_Kind>(p.kind, People_Kind::citizen)), math_compare<"<">(now, p.hands_up_until));
    arm_l = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    arm_r = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (hands_up) {
      a_21 = at(Vector{0.0, 0.27, 0.0}).Result;
      a_20 = engine_math_math::Mat4_mul(shoulder, a_21).Result;
      a_22 = rot(Vector{1.0, 0.0, 0.0}, 2.8).Result;
      arm_l = engine_math_math::Mat4_mul(a_20, a_22).Result;
      a_24 = at(Vector{0.0, -0.27, 0.0}).Result;
      a_23 = engine_math_math::Mat4_mul(shoulder, a_24).Result;
      a_25 = rot(Vector{1.0, 0.0, 0.0}, -2.8).Result;
      arm_r = engine_math_math::Mat4_mul(a_23, a_25).Result;
    } else if (logic_and(p.aiming, logic_not(down))) {
      ap = kern::formula([](Float aim_pitch) -> Float { return (aim_pitch < (-1.2) ? (-1.2) : (1.2 < aim_pitch ? 1.2 : aim_pitch)); }, p.aim_pitch);
      a_28 = at(Vector{0.0, 0.2, 0.0}).Result;
      a_27 = engine_math_math::Mat4_mul(shoulder, a_28).Result;
      a_29 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float ap) -> Float { return ((-1.45) + ap); }, ap)).Result;
      a_26 = engine_math_math::Mat4_mul(a_27, a_29).Result;
      a_30 = rot(Vector{1.0, 0.0, 0.0}, -0.25).Result;
      arm_l = engine_math_math::Mat4_mul(a_26, a_30).Result;
      a_33 = at(Vector{0.0, -0.2, 0.0}).Result;
      a_32 = engine_math_math::Mat4_mul(shoulder, a_33).Result;
      a_34 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float ap) -> Float { return ((-1.45) + ap); }, ap)).Result;
      a_31 = engine_math_math::Mat4_mul(a_32, a_34).Result;
      a_35 = rot(Vector{1.0, 0.0, 0.0}, 0.25).Result;
      arm_r = engine_math_math::Mat4_mul(a_31, a_35).Result;
    } else if (logic_and(p.hands, logic_not(down))) {
      arm_l = person_reach(shoulder, p, 0.2, p.hand_l).Result;
      arm_r = person_reach(shoulder, p, -0.2, p.hand_r).Result;
    } else if (logic_and(math_compare<"!=">(kern::to<Float>(p.punch), 0.0), logic_not(down))) {
      r = math_compare<">">(kern::formula([](Float punch) -> Float { return (kern::math::band(punch, 1.0) != 0.0); }, kern::to<Float>(p.punch)), 0.5);
      l = math_compare<">">(kern::formula([](Float punch) -> Float { return (kern::math::band(punch, 2.0) != 0.0); }, kern::to<Float>(p.punch)), 0.5);
      slash = math_compare<">">(kern::formula([](Float punch) -> Float { return (kern::math::band(punch, 4.0) != 0.0); }, kern::to<Float>(p.punch)), 0.5);
      if (logic_or(r, slash)) {
        a_41 = at(Vector{0.0, -0.2, 0.0}).Result;
        a_40 = engine_math_math::Mat4_mul(shoulder, a_41).Result;
        a_42 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float slash) -> Float { return ((slash != 0.0) ? (-1.2) : (-1.62)); }, kern::to<Float>(slash))).Result;
        a_39 = engine_math_math::Mat4_mul(a_40, a_42).Result;
        a_43 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float slash) -> Float { return ((slash != 0.0) ? 0.6 : 0.12); }, kern::to<Float>(slash))).Result;
        chosen = engine_math_math::Mat4_mul(a_39, a_43).Result;
      } else {
        a_46 = at(Vector{0.0, -0.22, 0.0}).Result;
        a_45 = engine_math_math::Mat4_mul(shoulder, a_46).Result;
        a_47 = rot(Vector{0.0, 1.0, 0.0}, -0.9).Result;
        a_44 = engine_math_math::Mat4_mul(a_45, a_47).Result;
        a_48 = rot(Vector{1.0, 0.0, 0.0}, 0.5).Result;
        chosen = engine_math_math::Mat4_mul(a_44, a_48).Result;
      }
      arm_r = chosen;
      if (l) {
        a_54 = at(Vector{0.0, 0.2, 0.0}).Result;
        a_53 = engine_math_math::Mat4_mul(shoulder, a_54).Result;
        a_55 = rot(Vector{0.0, 1.0, 0.0}, -1.62).Result;
        a_52 = engine_math_math::Mat4_mul(a_53, a_55).Result;
        a_56 = rot(Vector{1.0, 0.0, 0.0}, -0.12).Result;
        chosen_2 = engine_math_math::Mat4_mul(a_52, a_56).Result;
      } else {
        a_59 = at(Vector{0.0, 0.22, 0.0}).Result;
        a_58 = engine_math_math::Mat4_mul(shoulder, a_59).Result;
        a_60 = rot(Vector{0.0, 1.0, 0.0}, -0.9).Result;
        a_57 = engine_math_math::Mat4_mul(a_58, a_60).Result;
        a_61 = rot(Vector{1.0, 0.0, 0.0}, -0.5).Result;
        chosen_2 = engine_math_math::Mat4_mul(a_57, a_61).Result;
      }
      arm_l = chosen_2;
    } else {
      a_64 = at(Vector{0.0, 0.27, 0.0}).Result;
      a_63 = engine_math_math::Mat4_mul(shoulder, a_64).Result;
      a_65 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float swing) -> Float { return ((-swing) * 0.9); }, swing)).Result;
      a_62 = engine_math_math::Mat4_mul(a_63, a_65).Result;
      a_66 = rot(Vector{1.0, 0.0, 0.0}, 0.06).Result;
      arm_l = engine_math_math::Mat4_mul(a_62, a_66).Result;
      a_69 = at(Vector{0.0, -0.27, 0.0}).Result;
      a_68 = engine_math_math::Mat4_mul(shoulder, a_69).Result;
      a_70 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float swing) -> Float { return (swing * 0.9); }, swing)).Result;
      a_67 = engine_math_math::Mat4_mul(a_68, a_70).Result;
      a_71 = rot(Vector{1.0, 0.0, 0.0}, -0.06).Result;
      arm_r = engine_math_math::Mat4_mul(a_67, a_71).Result;
    }
    PeopleArt_put(self.arm_, arm_l, p.shirt);
    PeopleArt_put(self.arm_, arm_r, p.shirt);
    PeopleArt_put(self.hand_, arm_l, p.skin);
    PeopleArt_put(self.hand_, arm_r, p.skin);
    a_73 = at(Vector{0.0, 0.0, 0.73}).Result;
    a_72 = engine_math_math::Mat4_mul(body, a_73).Result;
    a_74 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float down, Float aim_pitch) -> Float { return ((down != 0.0) ? 0.0 : ((aim_pitch < (-1.2) ? (-1.2) : (1.2 < aim_pitch ? 1.2 : aim_pitch)) * 0.6)); }, kern::to<Float>(down), p.aim_pitch)).Result;
    head = engine_math_math::Mat4_mul(a_72, a_74).Result;
    if (logic_and(math_compare<">=">(kern::to<Float>(p.held), 0.0), logic_not(down))) {
      w = enum_from_index<weapons_Id>(p.held).value;
      ap_2 = kern::formula([](Float aiming, Float hands, Float aim_pitch) -> Float { return (((aiming != 0.0) || (hands != 0.0)) ? (aim_pitch < (-1.2) ? (-1.2) : (1.2 < aim_pitch ? 1.2 : aim_pitch)) : 0.5); }, kern::to<Float>(p.aiming), kern::to<Float>(p.hands), p.aim_pitch);
      if (p.hands) {
        a_80 = at(p.hand_r).Result;
        a_81 = rot(Vector{0.0, 0.0, 1.0}, p.yaw).Result;
        a_79 = engine_math_math::Mat4_mul(a_80, a_81).Result;
        a_82 = rot(Vector{0.0, 1.0, 0.0}, ap_2).Result;
        a_78 = engine_math_math::Mat4_mul(a_79, a_82).Result;
        a_83 = at(vector_make(kern::formula([](Float a) -> Float { return ((a != 0.0) ? (-0.35) : 0.0); }, kern::to<Float>(enum_equal<weapons_Id>(w, weapons_Id::rocket))), 0.0, kern::formula([](Float a) -> Float { return ((a != 0.0) ? 0.1 : 0.0); }, kern::to<Float>(enum_equal<weapons_Id>(w, weapons_Id::rocket))))).Result;
        chosen_3 = engine_math_math::Mat4_mul(a_78, a_83).Result;
      } else {
        a_86 = at(Vector{0.0, -0.06, 0.0}).Result;
        a_85 = engine_math_math::Mat4_mul(shoulder, a_86).Result;
        a_87 = rot(Vector{0.0, 1.0, 0.0}, ap_2).Result;
        a_84 = engine_math_math::Mat4_mul(a_85, a_87).Result;
        a_88 = at(vector_make(kern::formula([](Float a) -> Float { return ((a != 0.0) ? 0.05 : 0.5); }, kern::to<Float>(enum_equal<weapons_Id>(w, weapons_Id::rocket))), 0.0, kern::formula([](Float a) -> Float { return ((a != 0.0) ? 0.12 : (-0.02)); }, kern::to<Float>(enum_equal<weapons_Id>(w, weapons_Id::rocket))))).Result;
        chosen_3 = engine_math_math::Mat4_mul(a_84, a_88).Result;
      }
      g = chosen_3;
      if (enum_equal<weapons_Id>(w, weapons_Id::glock)) {
        PeopleArt_put(self.gun_pistol_, g, Vector{1.0, 1.0, 1.0});
      } else if (enum_equal<weapons_Id>(w, weapons_Id::smg)) {
        PeopleArt_put(self.gun_smg_, g, Vector{1.0, 1.0, 1.0});
      } else if (enum_equal<weapons_Id>(w, weapons_Id::sniper)) {
        PeopleArt_put(self.gun_sniper_, g, Vector{1.0, 1.0, 1.0});
      } else if (enum_equal<weapons_Id>(w, weapons_Id::grenade_launcher)) {
        PeopleArt_put(self.gun_tube_, g, Vector{1.0, 1.0, 1.0});
      } else if (enum_equal<weapons_Id>(w, weapons_Id::rocket)) {
        PeopleArt_put(self.gun_rocket_, g, Vector{1.0, 1.0, 1.0});
      } else if (enum_equal<weapons_Id>(w, weapons_Id::knife)) {
        PeopleArt_put(self.gun_knife_, g, Vector{1.0, 1.0, 1.0});
      }
    }
    PeopleArt_put(self.head_, head, p.skin);
    PeopleArt_put(self.hair_, head, PeopleArt_hair_color(p).Result);
    if (logic_and(enum_equal<People_Kind>(p.kind, People_Kind::officer), logic_not(down))) {
      PeopleArt_put(self.vest_, body, Vector{1.0, 1.0, 1.0});
    }
    if (logic_or(logic_or(enum_equal<People_Kind>(p.kind, People_Kind::officer), enum_equal<People_Kind>(p.kind, People_Kind::soldier)), enum_equal<People_Kind>(p.kind, People_Kind::enemy))) {
      a_90 = at(Vector{0.0, 0.0, 0.06}).Result;
      a_89 = engine_math_math::Mat4_mul(head, a_90).Result;
      PeopleArt_put(self.cap_, a_89, Vector{1.0, 1.0, 1.0});
    }
  }

  void PeopleArt_ragdolls(PeopleArt& self, const List<Ragdolls_Draw>& bones, const Ragdolls& dolls) {
    Ragdolls_Colors c = Ragdolls_Colors{Vector{0.35, 0.44, 0.53}, Vector{0.17, 0.19, 0.22}, Vector{0.82, 0.66, 0.53}, Vector{0.18, 0.11, 0.06}, false, false};
    Int selected = 0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    for (auto it114 = flow_for(n114, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Ragdolls_Draw>(bones))))); it114.next();) {
      c = dolls.dolls_[bones[n114.index].doll].colors;
      selected = bones[n114.index].bone;
      if (math_compare<"==">(kern::to<Float>(selected), 0.0)) {
        PeopleArt_put(self.rd_pelvis_, bones[n114.index].pose, c.pants);
      } else if (math_compare<"==">(kern::to<Float>(selected), 1.0)) {
        PeopleArt_put(self.rd_chest_, bones[n114.index].pose, c.shirt);
        if (c.vest) {
          a_2 = at(Vector{0.0, 0.0, -0.36}).Result;
          a = engine_math_math::Mat4_mul(bones[n114.index].pose, a_2).Result;
          PeopleArt_put(self.vest_, a, Vector{1.0, 1.0, 1.0});
        }
      } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
        PeopleArt_put(self.head_, bones[n114.index].pose, c.skin);
        PeopleArt_put(self.hair_, bones[n114.index].pose, c.hair);
        if (c.cap) {
          a_4 = at(Vector{0.0, 0.0, 0.06}).Result;
          a_3 = engine_math_math::Mat4_mul(bones[n114.index].pose, a_4).Result;
          PeopleArt_put(self.cap_, a_3, Vector{1.0, 1.0, 1.0});
        }
      } else if (logic_or(math_compare<"==">(kern::to<Float>(selected), 3.0), math_compare<"==">(kern::to<Float>(selected), 4.0))) {
        PeopleArt_put(self.rd_upper_, bones[n114.index].pose, c.shirt);
      } else if (logic_or(math_compare<"==">(kern::to<Float>(selected), 5.0), math_compare<"==">(kern::to<Float>(selected), 6.0))) {
        PeopleArt_put(self.rd_fore_, bones[n114.index].pose, c.shirt);
      } else if (logic_or(math_compare<"==">(kern::to<Float>(selected), 7.0), math_compare<"==">(kern::to<Float>(selected), 8.0))) {
        PeopleArt_put(self.rd_thigh_, bones[n114.index].pose, c.pants);
      } else {
        PeopleArt_put(self.rd_shin_, bones[n114.index].pose, c.pants);
      }
    }
  }

  void PeopleArt_beacon(PeopleArt& self, Vector foot = Vector{0.0, 0.0, 0.0}, Vector color = Vector{0.0, 0.0, 0.0}, Float alpha = 0.0) {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a_2 = at(vector_add(foot, Vector{0.0, 0.0, 2.6})).Result;
    a_3 = engine_math_math::Mat4_scale(Vector{0.18, 0.18, 90.0}).Result;
    a = engine_math_math::Mat4_mul(a_2, a_3).Result;
    PeopleArt_put(self.beacon_, a, vector_scale(color, kern::formula([](Float alpha) -> Float { return (0.6 + (1.4 * alpha)); }, alpha)));
  }

  void PeopleArt_light_bar(PeopleArt& self, const Mat4& chassis, Bool on = false, Float now = 0.0) {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int slot = 0;
    Bool red = false;
    Bool blue = false;
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a = at(Vector{-0.2, 0.0, 0.86}).Result;
    m = engine_math_math::Mat4_mul(chassis, a).Result;
    PeopleArt_put(self.bar_, m, Vector{1.0, 1.0, 1.0});
    PeopleArt_put(self.door_, chassis, Vector{1.0, 1.0, 1.0});
    slot = kern::to<Int>(kern::formula([](Float now) -> Float { return kern::math::mod(kern::math::trunc(kern::math::floor(kern::math::div((now * 1000.0), 110.0))), 6.0); }, now));
    red = logic_and(on, logic_or(math_compare<"==">(kern::to<Float>(slot), 0.0), math_compare<"==">(kern::to<Float>(slot), 2.0)));
    blue = logic_and(on, logic_or(math_compare<"==">(kern::to<Float>(slot), 3.0), math_compare<"==">(kern::to<Float>(slot), 5.0)));
    a_3 = at(Vector{0.0, 0.25, 0.05}).Result;
    a_2 = engine_math_math::Mat4_mul(m, a_3).Result;
    PeopleArt_put(self.lamp_, a_2, logic_select<Vector>(blue, Vector{0.16, 0.42, 3.0}, Vector{0.024, 0.063, 0.18}));
    a_5 = at(Vector{0.0, -0.25, 0.05}).Result;
    a_4 = engine_math_math::Mat4_mul(m, a_5).Result;
    PeopleArt_put(self.lamp_, a_4, logic_select<Vector>(red, Vector{3.0, 0.1, 0.16}, Vector{0.2, 0.024, 0.03}));
  }

  void PeopleArt_fire(PeopleArt& self, Vector p = Vector{0.0, 0.0, 0.0}, Float size = 0.0, Float now = 0.0) {
    Float t = 0.0;
    Float s = 0.0;
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    for (auto it115 = flow_for(n115, 0, 2); it115.next();) {
      t = kern::formula([](Float now, Float index) -> Float { return ((now * (3.1 + index)) + (index * 2.1)); }, now, kern::to<Float>(n115.index));
      s = kern::formula([](Float size, Float t) -> Float { return (size * (0.55 + (0.25 * kern::math::sin(t)))); }, size, t);
      a_2 = at(vector_add(p, vector_make(kern::formula([](Float t) -> Float { return (0.3 * kern::math::sin((t * 1.3))); }, t), kern::formula([](Float t) -> Float { return (0.3 * kern::math::cos((t * 0.9))); }, t), kern::formula([](Float s, Float index, Float size) -> Float { return ((s * 0.6) + ((index * size) * 0.35)); }, s, kern::to<Float>(n115.index), size)))).Result;
      a_3 = engine_math_math::Mat4_scale(vector_make(s, s, kern::formula([](Float s) -> Float { return (s * 1.4); }, s))).Result;
      a = engine_math_math::Mat4_mul(a_2, a_3).Result;
      PeopleArt_put(self.flame_, a, logic_select<Vector>(math_compare<"==">(kern::to<Float>(n115.index), 0.0), Vector{1.4, 0.6, 0.15}, Vector{1.0, 0.35, 0.08}));
    }
  }

  void PeopleArt_police_drone(PeopleArt& self, Vector p = Vector{0.0, 0.0, 0.0}, Float yaw = 0.0, Float tilt_x = 0.0, Float tilt_y = 0.0, Bool lights = false, Float now = 0.0, Int index = 0) {
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Float a_7 = 0.0;
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Int slot = 0;
    Vector off = Vector{0.0, 0.0, 0.0};
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_13 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_14 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_15 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_16 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_17 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a_3 = at(p).Result;
    a_4 = rot(Vector{0.0, 0.0, 1.0}, yaw).Result;
    a_2 = engine_math_math::Mat4_mul(a_3, a_4).Result;
    a_5 = rot(Vector{0.0, 1.0, 0.0}, tilt_y).Result;
    a = engine_math_math::Mat4_mul(a_2, a_5).Result;
    a_6 = rot(Vector{1.0, 0.0, 0.0}, tilt_x).Result;
    m = engine_math_math::Mat4_mul(a, a_6).Result;
    PeopleArt_put(self.pd_body_, m, Vector{1.0, 1.0, 1.0});
    for (auto it116 = flow_for(n116, 0, 3); it116.next();) {
      a_7 = kern::formula([](Float kPi, Float index) -> Float { return (kern::math::div(kPi, 4.0) + kern::math::div((index * kPi), 2.0)); }, kPi, kern::to<Float>(n116.index));
      a_9 = at(vector_make(kern::formula([](Float a_7) -> Float { return (kern::math::cos(a_7) * 0.32); }, a_7), kern::formula([](Float a_7) -> Float { return (kern::math::sin(a_7) * 0.32); }, a_7), 0.04)).Result;
      a_8 = engine_math_math::Mat4_mul(m, a_9).Result;
      PeopleArt_put(self.pd_rotor_, a_8, Vector{1.0, 1.0, 1.0});
    }
    slot = kern::to<Int>(kern::formula([](Float now, Float index) -> Float { return kern::math::mod(kern::math::trunc(kern::math::floor(kern::math::div(((now * 1000.0) + (index * 37.0)), 92.0))), 8.0); }, now, kern::to<Float>(index)));
    off = Vector{0.1, 0.1, 0.12};
    a_12 = at(Vector{-0.16, 0.1, 0.07}).Result;
    a_11 = engine_math_math::Mat4_mul(m, a_12).Result;
    a_13 = engine_math_math::Mat4_scale(Vector{0.25, 0.2, 0.4}).Result;
    a_10 = engine_math_math::Mat4_mul(a_11, a_13).Result;
    PeopleArt_put(self.lamp_, a_10, logic_select<Vector>(logic_and(lights, logic_or(math_compare<"==">(kern::to<Float>(slot), 0.0), math_compare<"==">(kern::to<Float>(slot), 1.0))), Vector{3.0, 0.1, 0.08}, off));
    a_16 = at(Vector{-0.16, -0.1, 0.07}).Result;
    a_15 = engine_math_math::Mat4_mul(m, a_16).Result;
    a_17 = engine_math_math::Mat4_scale(Vector{0.25, 0.2, 0.4}).Result;
    a_14 = engine_math_math::Mat4_mul(a_15, a_17).Result;
    PeopleArt_put(self.lamp_, a_14, logic_select<Vector>(logic_and(lights, logic_or(math_compare<"==">(kern::to<Float>(slot), 4.0), math_compare<"==">(kern::to<Float>(slot), 5.0))), Vector{0.1, 0.3, 3.0}, off));
  }

  void PeopleArt_projectile(PeopleArt& self, Vector p = Vector{0.0, 0.0, 0.0}, Vector dir = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float length = 0.0, Bool flame = false) {
    Float l = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector axis = Vector{0.0, 0.0, 0.0};
    Float ang = 0.0;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    l = vector_length(dir);
    n = logic_select<Vector>(math_compare<">">(l, 1e-05), vector_scale(dir, kern::formula([](Float l) -> Float { return kern::math::div(1.0, l); }, l)), Vector{0.0, 0.0, 1.0});
    axis = vector_cross(Vector{0.0, 0.0, 1.0}, n);
    ang = kern::formula([](Float z) -> Float { return kern::math::acos((z < (-1.0) ? (-1.0) : (1.0 < z ? 1.0 : z))); }, vector_break(n).z);
    a = at(p).Result;
    a_2 = rot(logic_select<Vector>(math_compare<">">(vector_length(axis), 9.999999717180685e-10), vector_normalize(axis), Vector{1.0, 0.0, 0.0}), ang).Result;
    m = engine_math_math::Mat4_mul(a, a_2).Result;
    a_4 = engine_math_math::Mat4_scale(vector_make(radius, radius, kern::formula([](Float length) -> Float { return kern::math::div(length, 4.0); }, length))).Result;
    a_3 = engine_math_math::Mat4_mul(m, a_4).Result;
    PeopleArt_put(self.shell_, a_3, Vector{1.0, 1.0, 1.0});
    if (flame) {
      a_7 = at(vector_make(0.0, 0.0, kern::formula([](Float length) -> Float { return ((-length) * 0.6); }, length))).Result;
      a_6 = engine_math_math::Mat4_mul(m, a_7).Result;
      a_8 = engine_math_math::Mat4_scale(vector_make(kern::formula([](Float radius) -> Float { return (radius * 1.6); }, radius), kern::formula([](Float radius) -> Float { return (radius * 1.6); }, radius), kern::formula([](Float radius) -> Float { return (radius * 3.2); }, radius))).Result;
      a_5 = engine_math_math::Mat4_mul(a_6, a_8).Result;
      PeopleArt_put(self.flame_, a_5, Vector{1.6, 0.7, 0.2});
    }
  }

  void PeopleArt_upload(PeopleArt& self, PeopleArt_Part& p) {
    Bool set_visible = false;
    Bool update_instances = false;
    Bool set_visible_2 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(p.id), 0.0))) {
      return;
    }
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(p.transforms)), 0.0)) {
      engine_renderer_set_visible(n117, self.r_, p.id, false);
      set_visible = n117.result;
      return;
    }
    engine_renderer_update_instances(n118, self.r_, p.id, p.transforms, p.tints);
    update_instances = n118.result;
    engine_renderer_set_visible(n119, self.r_, p.id, true);
    set_visible_2 = n119.result;
  }

  void PeopleArt_end(PeopleArt& self) {
    PeopleArt_upload(self, self.leg_);
    PeopleArt_upload(self, self.torso_);
    PeopleArt_upload(self, self.arm_);
    PeopleArt_upload(self, self.head_);
    PeopleArt_upload(self, self.hand_);
    PeopleArt_upload(self, self.hair_);
    PeopleArt_upload(self, self.vest_);
    PeopleArt_upload(self, self.door_);
    PeopleArt_upload(self, self.cap_);
    PeopleArt_upload(self, self.bar_);
    PeopleArt_upload(self, self.lamp_);
    PeopleArt_upload(self, self.flame_);
    PeopleArt_upload(self, self.pd_body_);
    PeopleArt_upload(self, self.pd_rotor_);
    PeopleArt_upload(self, self.shell_);
    PeopleArt_upload(self, self.pole_);
    PeopleArt_upload(self, self.lamp_head_);
    PeopleArt_upload(self, self.rd_pelvis_);
    PeopleArt_upload(self, self.rd_chest_);
    PeopleArt_upload(self, self.rd_upper_);
    PeopleArt_upload(self, self.rd_fore_);
    PeopleArt_upload(self, self.rd_thigh_);
    PeopleArt_upload(self, self.rd_shin_);
    PeopleArt_upload(self, self.beacon_);
    PeopleArt_upload(self, self.gun_pistol_);
    PeopleArt_upload(self, self.gun_smg_);
    PeopleArt_upload(self, self.gun_sniper_);
    PeopleArt_upload(self, self.gun_tube_);
    PeopleArt_upload(self, self.gun_rocket_);
    PeopleArt_upload(self, self.gun_knife_);
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool result = false; } n11;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n12;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n13;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n14;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n15;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n16;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n17;
  PeopleArt_makeResult n18;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n19;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n22;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n23;
  PeopleArt_makeResult n24;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n25;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n26;
  PeopleArt_makeResult n27;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n28;
  PeopleArt_makeResult n29;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n30;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n31;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n32;
  struct : kern::Node<33> { Int index = 0; } n33;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n34;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n35;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n36;
  PeopleArt_makeResult n37;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n38;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n39;
  PeopleArt_makeResult n40;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n41;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n42;
  PeopleArt_makeResult n43;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n44;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n45;
  struct : kern::Node<46> { Int index = 0; } n46;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n47;
  PeopleArt_makeResult n48;
  PeopleArt_makeResult n49;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n50;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n51;
  PeopleArt_makeResult n52;
  PeopleArt_makeResult n53;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n54;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n55;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n56;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n57;
  PeopleArt_makeResult n58;
  PeopleArt_makeResult n59;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n60;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n61;
  PeopleArt_makeResult n62;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n63;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n64;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n65;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n66;
  PeopleArt_makeResult n67;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n68;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n69;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n70;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n71;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n72;
  PeopleArt_makeResult n73;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n74;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n75;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n76;
  PeopleArt_makeResult n77;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n78;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n79;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n80;
  PeopleArt_makeResult n81;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n82;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n83;
  PeopleArt_makeResult n84;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n85;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n86;
  PeopleArt_makeResult n87;
  struct : kern::Node<88> { Int index = 0; } n88;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n89;
  PeopleArt_makeResult n90;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n91;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n92;
  PeopleArt_makeResult n93;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n94;
  PeopleArt_makeResult n95;
  PeopleArt_makeResult n96;
  PeopleArt_makeResult n97;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n98;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n99;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n100;
  struct : kern::Node<101> { Int index = 0; } n101;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n102;
  struct : kern::Node<103> { Int index = 0; } n103;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n104;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n105;
  PeopleArt_makeResult n106;
  PeopleArt_makeResult n107;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n108;
  PeopleArt_makeResult n109;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n110;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n111;
  PeopleArt_makeResult n112;
  PeopleArt_makeResult n113;
  struct : kern::Node<114> { Int index = 0; } n114;
  struct : kern::Node<115> { Int index = 0; } n115;
  struct : kern::Node<116> { Int index = 0; } n116;
  struct { Bool result = false; } n117;
  struct { Bool result = false; Bool changed = false; } n118;
  struct { Bool result = false; } n119;
};
