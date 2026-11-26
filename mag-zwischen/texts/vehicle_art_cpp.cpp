struct vehicle_art_cpp : virtual kern::Library, virtual car, virtual engine_geo_mesh_data, virtual engine_math_math, virtual jet, virtual vehicle_art {
  // ---- variables ----
  /// vertex locations 14 and 15
  KERN_CONST U32 kMaxExtensionAttributes = 2;
  KERN_CONST Float kPi = 3.141592653589793;

  // ---- functions ----
  struct rotResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE rotResult rot(Vector axis = Vector{0.0, 0.0, 0.0}, Float a = 0.0) {
    return {engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(axis, a).Result, Vector{1.0, 1.0, 1.0}).Result};
  }

  struct atResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE atResult at(Vector p = Vector{0.0, 0.0, 0.0}) {
    return {engine_math_math::Mat4_translate(p).Result};
  }

  struct hullResult { MeshData Result = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; };
  hullResult hull(const List<Vector>& pts, Vector color = Vector{0.0, 0.0, 0.0}) {
    MeshData m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Bool hull_mesh = false;
    m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    engine_physics_hull_mesh(n10, pts, m, color);
    hull_mesh = n10.result;
    m = n10.out;
    return {m};
  }

  struct matResult { Material Result = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}; };
  KERN_PURE matResult mat(Float roughness = 0.0, Float metalness = 0.0, Vector emissive = Vector{0.0, 0.0, 0.0}) {
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    m.roughness = roughness;
    m.metalness = metalness;
    m.emissive = emissive;
    return {m};
  }

  struct VehicleArt_makeResult { Bool Result = false; };
  VehicleArt_makeResult VehicleArt_make(VehicleArt_Part& p, const MeshData& mesh, const Material& m, Object<kern::Renderer> r = Object<kern::Renderer>{}, Bool tinted = false) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool upsert = false;
    r_ = r;
    engine_renderer_allocate_id(n11, r_);
    allocate_id = n11.result;
    p.id = allocate_id;
    p.tinted = tinted;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    engine_upsert(n12, r_, p.id, mesh, look, parked.m, List<Float>{});
    upsert = n12.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, p.id);
      p.id = 0;
      return {false};
    }
    return {true};
  }

  struct VehicleArt_initResult { Bool Result = false; };
  VehicleArt_initResult VehicleArt_init(VehicleArt& self, Object<kern::Renderer> r = Object<kern::Renderer>{}) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Vector white = Vector{0.0, 0.0, 0.0};
    MeshData body = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData bumpers = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_5 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    List<Vector> g = List<Vector>{};
    List<Vector> prof = List<Vector>{};
    Float item = 0.0;
    Float item_2 = 0.0;
    Fixed<Float, 2> items = Fixed<Float, 2>{};
    Float y = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    MeshData a_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_3 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData lights = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_6 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_7 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_4 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData tail = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_8 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_9 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_5 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData wheel = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    MeshData cylinder_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Material a_10 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData bus = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_10 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_11 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_11 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData bus_glass = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_12 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_13 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_12 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Vector grey = Vector{0.0, 0.0, 0.0};
    MeshData body_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    List<Vector> a_13 = List<Vector>{};
    MeshData arg = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    List<Vector> a_14 = List<Vector>{};
    MeshData arg_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    Fixed<Float, 2> items_2 = Fixed<Float, 2>{};
    Float s = 0.0;
    List<Vector> a_15 = List<Vector>{};
    MeshData arg_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    List<Vector> a_16 = List<Vector>{};
    MeshData arg_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Mat4 a_17 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_18 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Material a_19 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData canopy = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_20 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData gear = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder_5 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Mat4 a_21 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_22 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Material a_23 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Material flame = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_24 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cone = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData frame = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_14 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_15 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_16 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Float a_25 = 0.0;
    Vector item_5 = Vector{0.0, 0.0, 0.0};
    Vector item_6 = Vector{0.0, 0.0, 0.0};
    Vector item_7 = Vector{0.0, 0.0, 0.0};
    Vector item_8 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 4> items_3 = Fixed<Vector, 4>{};
    Vector m = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    MeshData box_17 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Mat4 a_26 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_27 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Material a_28 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_29 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder_6 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_30 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Material prop = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_31 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_18 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData a_32 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_19 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material led = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_33 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_20 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData canopy_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material m_2 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_34 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_21 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_35 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_36 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData capsule = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_37 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    r_ = r;
    self.r_ = r_;
    white = Vector{1.0, 1.0, 1.0};
    body = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    box = engine_primitives_box(Vector{3.56, 1.64, 0.5}, white);
    engine_primitives_append(n13, body, box, at(Vector{0.0, 0.0, -0.04}).Result);
    body = n13.dst;
    box_2 = engine_primitives_box(Vector{3.3, 1.6, 0.14}, white);
    engine_primitives_append(n14, body, box_2, at(Vector{-0.05, 0.0, 0.27}).Result);
    body = n14.dst;
    box_3 = engine_primitives_box(Vector{0.9, 1.5, 0.1}, white);
    engine_primitives_append(n15, body, box_3, at(Vector{1.25, 0.0, 0.26}).Result);
    body = n15.dst;
    bumpers = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    box_4 = engine_primitives_box(Vector{0.16, 1.66, 0.2}, Vector{0.12, 0.12, 0.13});
    engine_primitives_append(n16, bumpers, box_4, at(Vector{1.79, 0.0, -0.18}).Result);
    bumpers = n16.dst;
    box_5 = engine_primitives_box(Vector{0.16, 1.66, 0.2}, Vector{0.12, 0.12, 0.13});
    engine_primitives_append(n17, bumpers, box_5, at(Vector{-1.79, 0.0, -0.18}).Result);
    bumpers = n17.dst;
    engine_primitives_append(n18, body, bumpers, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    body = n18.dst;
    a = mat(0.32, 0.35, Vector{0.0, 0.0, 0.0}).Result;
    n19 = VehicleArt_make(self.car_body_, body, a, r_, true);
    if (logic_not(n19.Result)) {
      return {false};
    }
    g = List<Vector>{};
    prof = list_make<Vector, "4">(Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0});
    item = -0.71;
    item_2 = 0.71;
    items = list_make<Float, "2">(item, item_2);
    for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items))))); it20.next();) {
      y = items[n20.index];
      for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(prof))))); it21.next();) {
        p = prof[n21.index];
        list_add(g, vector_make(vector_break(p).x, y, kern::formula([](Float y) -> Float { return (y - 0.54); }, vector_break(p).y)));
      }
    }
    n22 = hull(g, Vector{0.08, 0.1, 0.13});
    a_2 = n22.Result;
    a_3 = mat(0.08, 0.2, Vector{0.0, 0.0, 0.0}).Result;
    n23 = VehicleArt_make(self.car_glass_, a_2, a_3, r_, false);
    if (logic_not(n23.Result)) {
      return {false};
    }
    lights = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    box_6 = engine_primitives_box(Vector{0.06, 0.36, 0.12}, white);
    engine_primitives_append(n24, lights, box_6, at(Vector{1.85, 0.56, 0.06}).Result);
    lights = n24.dst;
    box_7 = engine_primitives_box(Vector{0.06, 0.36, 0.12}, white);
    engine_primitives_append(n25, lights, box_7, at(Vector{1.85, -0.56, 0.06}).Result);
    lights = n25.dst;
    a_4 = mat(0.2, 0.0, Vector{2.5, 2.4, 2.1}).Result;
    n26 = VehicleArt_make(self.car_lights_, lights, a_4, r_, false);
    if (logic_not(n26.Result)) {
      return {false};
    }
    tail = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    box_8 = engine_primitives_box(Vector{0.06, 0.4, 0.1}, Vector{0.7, 0.05, 0.04});
    engine_primitives_append(n27, tail, box_8, at(Vector{-1.85, 0.55, 0.06}).Result);
    tail = n27.dst;
    box_9 = engine_primitives_box(Vector{0.06, 0.4, 0.1}, Vector{0.7, 0.05, 0.04});
    engine_primitives_append(n28, tail, box_9, at(Vector{-1.85, -0.55, 0.06}).Result);
    tail = n28.dst;
    a_5 = mat(0.3, 0.0, Vector{0.9, 0.05, 0.03}).Result;
    n29 = VehicleArt_make(self.car_tail_, tail, a_5, r_, true);
    if (logic_not(n29.Result)) {
      return {false};
    }
    wheel = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    cylinder = engine_primitives_cylinder(1.0, 0.62, 18, Vector{0.07, 0.07, 0.08});
    a_6 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div((-kPi), 2.0); }, kPi)).Result;
    a_7 = at(Vector{0.0, 0.0, -0.31}).Result;
    engine_primitives_append(n30, wheel, cylinder, engine_math_math::Mat4_mul(a_6, a_7).Result);
    wheel = n30.dst;
    cylinder_2 = engine_primitives_cylinder(0.62, 0.66, 10, Vector{0.62, 0.63, 0.66});
    a_8 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div((-kPi), 2.0); }, kPi)).Result;
    a_9 = at(Vector{0.0, 0.0, -0.33}).Result;
    engine_primitives_append(n31, wheel, cylinder_2, engine_math_math::Mat4_mul(a_8, a_9).Result);
    wheel = n31.dst;
    a_10 = mat(0.7, 0.2, Vector{0.0, 0.0, 0.0}).Result;
    n32 = VehicleArt_make(self.wheel_, wheel, a_10, r_, false);
    if (logic_not(n32.Result)) {
      return {false};
    }
    bus = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    box_10 = engine_primitives_box(Vector{8.0, 2.42, 2.0}, white);
    engine_primitives_append(n33, bus, box_10, at(Vector{0.0, 0.0, 0.1}).Result);
    bus = n33.dst;
    box_11 = engine_primitives_box(Vector{7.9, 2.38, 0.32}, white);
    engine_primitives_append(n34, bus, box_11, at(Vector{0.0, 0.0, 1.4}).Result);
    bus = n34.dst;
    a_11 = mat(0.4, 0.2, Vector{0.0, 0.0, 0.0}).Result;
    n35 = VehicleArt_make(self.bus_body_, bus, a_11, r_, true);
    if (logic_not(n35.Result)) {
      return {false};
    }
    bus_glass = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    box_12 = engine_primitives_box(Vector{7.4, 2.46, 0.85}, Vector{0.08, 0.1, 0.13});
    engine_primitives_append(n36, bus_glass, box_12, at(Vector{0.1, 0.0, 0.75}).Result);
    bus_glass = n36.dst;
    box_13 = engine_primitives_box(Vector{0.08, 2.2, 1.2}, Vector{0.08, 0.1, 0.13});
    engine_primitives_append(n37, bus_glass, box_13, at(Vector{4.0, 0.0, 0.55}).Result);
    bus_glass = n37.dst;
    a_12 = mat(0.08, 0.2, Vector{0.0, 0.0, 0.0}).Result;
    n38 = VehicleArt_make(self.bus_glass_, bus_glass, a_12, r_, false);
    if (logic_not(n38.Result)) {
      return {false};
    }
    grey = Vector{0.46, 0.49, 0.52};
    body_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    arg = body_2;
    a_13 = list_make<Vector, "12">(Vector{-0.8, -7.2, -0.8}, Vector{0.8, -7.2, -0.8}, Vector{0.8, -7.2, 0.7}, Vector{-0.8, -7.2, 0.7}, Vector{-0.9, 4.5, -0.75}, Vector{0.9, 4.5, -0.75}, Vector{0.75, 4.5, 0.65}, Vector{-0.75, 4.5, 0.65}, Vector{-0.12, 9.0, -0.1}, Vector{0.12, 9.0, -0.1}, Vector{0.1, 9.0, 0.1}, Vector{-0.1, 9.0, 0.1});
    n39 = hull(a_13, grey);
    engine_primitives_append(n40, arg, n39.Result, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    body_2 = n40.dst;
    arg_2 = body_2;
    a_14 = list_make<Vector, "16">(Vector{-6.0, -3.4, -0.15}, Vector{-6.0, -4.3, -0.15}, Vector{-0.6, 1.8, -0.2}, Vector{-0.6, -4.2, -0.2}, Vector{6.0, -3.4, -0.15}, Vector{6.0, -4.3, -0.15}, Vector{0.6, 1.8, -0.2}, Vector{0.6, -4.2, -0.2}, Vector{-6.0, -3.4, -0.05}, Vector{-6.0, -4.3, -0.05}, Vector{-0.6, 1.8, 0.05}, Vector{-0.6, -4.2, 0.05}, Vector{6.0, -3.4, -0.05}, Vector{6.0, -4.3, -0.05}, Vector{0.6, 1.8, 0.05}, Vector{0.6, -4.2, 0.05});
    n41 = hull(a_14, grey);
    engine_primitives_append(n42, arg_2, n41.Result, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    body_2 = n42.dst;
    item_3 = -1.0;
    item_4 = 1.0;
    items_2 = list_make<Float, "2">(item_3, item_4);
    for (auto it43 = flow_for(n43, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(items_2))))); it43.next();) {
      s = items_2[n43.index];
      arg_3 = body_2;
      a_15 = list_make<Vector, "8">(vector_make(kern::formula([](Float s) -> Float { return (s * 0.7); }, s), -7.2, 0.5), vector_make(kern::formula([](Float s) -> Float { return (s * 0.7); }, s), -4.6, 0.5), vector_make(kern::formula([](Float s) -> Float { return (s * 1.4); }, s), -7.4, 3.0), vector_make(kern::formula([](Float s) -> Float { return (s * 1.35); }, s), -6.5, 3.0), vector_make(kern::formula([](Float s) -> Float { return (s * 0.78); }, s), -7.2, 0.5), vector_make(kern::formula([](Float s) -> Float { return (s * 0.78); }, s), -4.6, 0.5), vector_make(kern::formula([](Float s) -> Float { return (s * 1.48); }, s), -7.4, 3.0), vector_make(kern::formula([](Float s) -> Float { return (s * 1.43); }, s), -6.5, 3.0));
      n44 = hull(a_15, grey);
      engine_primitives_append(n45, arg_3, n44.Result, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
      body_2 = n45.dst;
      arg_4 = body_2;
      a_16 = list_make<Vector, "8">(vector_make(kern::formula([](Float s) -> Float { return (s * 0.6); }, s), -7.5, -0.1), vector_make(kern::formula([](Float s) -> Float { return (s * 0.6); }, s), -5.8, -0.1), vector_make(kern::formula([](Float s) -> Float { return (s * 3.4); }, s), -7.9, -0.15), vector_make(kern::formula([](Float s) -> Float { return (s * 3.4); }, s), -7.0, -0.15), vector_make(kern::formula([](Float s) -> Float { return (s * 0.6); }, s), -7.5, 0.0), vector_make(kern::formula([](Float s) -> Float { return (s * 0.6); }, s), -5.8, 0.0), vector_make(kern::formula([](Float s) -> Float { return (s * 3.4); }, s), -7.9, -0.05), vector_make(kern::formula([](Float s) -> Float { return (s * 3.4); }, s), -7.0, -0.05));
      n46 = hull(a_16, grey);
      engine_primitives_append(n47, arg_4, n46.Result, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
      body_2 = n47.dst;
    }
    cylinder_3 = engine_primitives_cylinder(0.62, 0.9, 16, Vector{0.18, 0.18, 0.2});
    a_17 = at(Vector{0.0, -7.0, 0.0}).Result;
    a_18 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
    engine_primitives_append(n48, body_2, cylinder_3, engine_math_math::Mat4_mul(a_17, a_18).Result);
    body_2 = n48.dst;
    a_19 = mat(0.45, 0.4, Vector{0.0, 0.0, 0.0}).Result;
    n49 = VehicleArt_make(self.jet_body_, body_2, a_19, r_, false);
    if (logic_not(n49.Result)) {
      return {false};
    }
    sphere = engine_primitives_sphere(1.0, 18, 10, Vector{0.15, 0.2, 0.12});
    canopy = sphere;
    a_20 = mat(0.06, 0.5, Vector{0.0, 0.0, 0.0}).Result;
    n50 = VehicleArt_make(self.jet_glass_, canopy, a_20, r_, false);
    if (logic_not(n50.Result)) {
      return {false};
    }
    gear = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    cylinder_4 = engine_primitives_cylinder(0.08, 1.45, 8, Vector{0.7, 0.7, 0.72});
    engine_primitives_append(n51, gear, cylinder_4, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    gear = n51.dst;
    cylinder_5 = engine_primitives_cylinder(0.38, 0.28, 14, Vector{0.07, 0.07, 0.08});
    a_21 = at(Vector{0.14, 0.0, 0.0}).Result;
    a_22 = rot(Vector{0.0, 1.0, 0.0}, kern::formula([](Float kPi) -> Float { return kern::math::div(kPi, 2.0); }, kPi)).Result;
    engine_primitives_append(n52, gear, cylinder_5, engine_math_math::Mat4_mul(a_21, a_22).Result);
    gear = n52.dst;
    a_23 = mat(0.6, 0.3, Vector{0.0, 0.0, 0.0}).Result;
    n53 = VehicleArt_make(self.jet_gear_, gear, a_23, r_, false);
    if (logic_not(n53.Result)) {
      return {false};
    }
    flame = mat(1.0, 0.0, Vector{6.0, 2.6, 0.9}).Result;
    flame.unlit = true;
    flame.cast_shadow = false;
    flame.alpha = AlphaMode::blend;
    flame.color = engine_math_math::Vec4_new(1.0, 0.75, 0.45, 0.55).Result;
    cone = engine_primitives_cone(0.5, 1.0, 14, Vector{1.0, 0.7, 0.4});
    a_24 = cone;
    n54 = VehicleArt_make(self.jet_flame_, a_24, flame, r_, false);
    if (logic_not(n54.Result)) {
      return {false};
    }
    frame = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    box_14 = engine_primitives_box(Vector{0.11, 0.06, 0.03}, Vector{0.1, 0.1, 0.11});
    engine_primitives_append(n55, frame, box_14, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    frame = n55.dst;
    box_15 = engine_primitives_box(Vector{0.045, 0.035, 0.035}, Vector{0.95, 0.45, 0.1});
    engine_primitives_append(n56, frame, box_15, at(Vector{-0.07, 0.0, 0.028}).Result);
    frame = n56.dst;
    box_16 = engine_primitives_box(Vector{0.1, 0.035, 0.03}, Vector{0.2, 0.25, 0.6});
    engine_primitives_append(n57, frame, box_16, at(Vector{0.015, 0.0, -0.03}).Result);
    frame = n57.dst;
    a_25 = 0.07954951288348659;
    item_5 = vector_make(kern::formula([](Float a_25) -> Float { return (-a_25); }, a_25), kern::formula([](Float a_25) -> Float { return (-a_25); }, a_25), 0.0);
    item_6 = vector_make(kern::formula([](Float a_25) -> Float { return (-a_25); }, a_25), a_25, 0.0);
    item_7 = vector_make(a_25, a_25, 0.0);
    item_8 = vector_make(a_25, kern::formula([](Float a_25) -> Float { return (-a_25); }, a_25), 0.0);
    items_3 = list_make<Vector, "4">(item_5, item_6, item_7, item_8);
    for (auto it58 = flow_for(n58, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(items_3))))); it58.next();) {
      m = items_3[n58.index];
      len = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt(((x * x) + (y * y))); }, vector_break(m).x, vector_break(m).y);
      box_17 = engine_primitives_box(vector_make(len, 0.016, 0.008), Vector{0.12, 0.12, 0.13});
      a_26 = at(vector_make(kern::formula([](Float x) -> Float { return kern::math::div(x, 2.0); }, vector_break(m).x), kern::formula([](Float y) -> Float { return kern::math::div(y, 2.0); }, vector_break(m).y), 0.0)).Result;
      a_27 = rot(Vector{0.0, 0.0, 1.0}, kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(m).y, vector_break(m).x)).Result;
      engine_primitives_append(n59, frame, box_17, engine_math_math::Mat4_mul(a_26, a_27).Result);
      frame = n59.dst;
    }
    a_28 = mat(0.5, 0.2, Vector{0.0, 0.0, 0.0}).Result;
    n60 = VehicleArt_make(self.drone_frame_, frame, a_28, r_, false);
    if (logic_not(n60.Result)) {
      return {false};
    }
    cylinder_6 = engine_primitives_cylinder(0.014, 0.02, 12, Vector{0.35, 0.35, 0.38});
    a_29 = cylinder_6;
    a_30 = mat(0.3, 0.8, Vector{0.0, 0.0, 0.0}).Result;
    n61 = VehicleArt_make(self.drone_motor_, a_29, a_30, r_, false);
    if (logic_not(n61.Result)) {
      return {false};
    }
    prop = mat(0.4, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    prop.alpha = AlphaMode::blend;
    prop.color = engine_math_math::Vec4_new(1.0, 1.0, 1.0, 0.72).Result;
    prop.cast_shadow = false;
    box_18 = engine_primitives_box(Vector{1.0, 0.012, 0.002}, Vector{1.0, 0.64, 0.3});
    a_31 = box_18;
    n62 = VehicleArt_make(self.drone_prop_a_, a_31, prop, r_, false);
    if (logic_not(n62.Result)) {
      return {false};
    }
    box_19 = engine_primitives_box(Vector{1.0, 0.012, 0.002}, Vector{0.3, 0.84, 1.0});
    a_32 = box_19;
    n63 = VehicleArt_make(self.drone_prop_b_, a_32, prop, r_, false);
    if (logic_not(n63.Result)) {
      return {false};
    }
    led = mat(0.4, 0.0, Vector{0.2, 2.5, 0.6}).Result;
    led.unlit = true;
    box_20 = engine_primitives_box(Vector{0.012, 0.012, 0.006}, white);
    a_33 = box_20;
    n64 = VehicleArt_make(self.drone_led_, a_33, led, r_, true);
    if (logic_not(n64.Result)) {
      return {false};
    }
    sphere_2 = engine_primitives_sphere(1.0, 20, 8, Vector{0.95, 0.45, 0.1});
    canopy_2 = sphere_2;
    m_2 = mat(0.8, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    m_2.double_sided = true;
    n65 = VehicleArt_make(self.chute_, canopy_2, m_2, r_, false);
    if (logic_not(n65.Result)) {
      return {false};
    }
    box_21 = engine_primitives_box(Vector{0.02, 0.02, 1.0}, Vector{0.9, 0.9, 0.85});
    a_34 = box_21;
    a_35 = mat(0.8, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n66 = VehicleArt_make(self.chute_lines_, a_34, a_35, r_, false);
    if (logic_not(n66.Result)) {
      return {false};
    }
    capsule = engine_primitives_capsule(0.28, 1.1, 10, 6, Vector{0.3, 0.36, 0.28});
    a_36 = capsule;
    a_37 = mat(0.8, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    n67 = VehicleArt_make(self.pilot_, a_36, a_37, r_, false);
    if (logic_not(n67.Result)) {
      return {false};
    }
    return {true};
  }

  void VehicleArt_clear(VehicleArt_Part& p) {
    list_clear(p.transforms);
    list_clear(p.tints);
  }

  void VehicleArt_begin(VehicleArt& self) {
    VehicleArt_clear(self.car_body_);
    VehicleArt_clear(self.car_glass_);
    VehicleArt_clear(self.car_lights_);
    VehicleArt_clear(self.car_tail_);
    VehicleArt_clear(self.wheel_);
    VehicleArt_clear(self.bus_body_);
    VehicleArt_clear(self.bus_glass_);
    VehicleArt_clear(self.jet_body_);
    VehicleArt_clear(self.jet_glass_);
    VehicleArt_clear(self.jet_gear_);
    VehicleArt_clear(self.jet_flame_);
    VehicleArt_clear(self.drone_frame_);
    VehicleArt_clear(self.drone_motor_);
    VehicleArt_clear(self.drone_prop_a_);
    VehicleArt_clear(self.drone_prop_b_);
    VehicleArt_clear(self.drone_led_);
    VehicleArt_clear(self.chute_);
    VehicleArt_clear(self.chute_lines_);
    VehicleArt_clear(self.pilot_);
  }

  void VehicleArt_put(VehicleArt_Part& p, const Mat4& m, Vector tint = Vector{0.0, 0.0, 0.0}) {
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

  void VehicleArt_car(VehicleArt& self, const Mat4& chassis, const Car& c, Vector color = Vector{0.0, 0.0, 0.0}, Bool cockpit_view = false, Bool brake_lights = false) {
    Bool bus = false;
    Float r = 0.0;
    Float w = 0.0;
    WheelState ws = WheelState{0.0, 0.0, 0.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, false, false, Vector{0.0, 0.0, 0.0}};
    CarSpec s = CarSpec{"car", Vector{1.78, 0.82, 0.36}, 1350.0, -0.06, 0.15, 0.34, 0.22, 22.0, -0.26, List<CarSpec_Wheel>{CarSpec_Wheel{1.2, 0.78, true}, CarSpec_Wheel{1.2, -0.78, true}, CarSpec_Wheel{-1.15, 0.78, false}, CarSpec_Wheel{-1.15, -0.78, false}}, -0.15, 0.11, 0.06, 550.0, 850.0, 140000.0, 0.15, 15.0, 0.68, 0.013, 1.3, 2400.0, 3200.0, 30.0, 0.58, 430.0, 600.0, 6.0, 1.1, 2.8, 3000.0, 95.0, List<Vector>{Vector{-1.02, 0.8, 0.0}, Vector{-0.64, 1.37, 0.0}, Vector{0.3, 1.37, 0.0}, Vector{0.86, 0.8, 0.0}}, 0.71, 0.54};
    Float travel = 0.0;
    Float steer = 0.0;
    Mat4 m = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    bus = math_compare<">">(c.s_.radius, 0.4);
    if (bus) {
      VehicleArt_put(self.bus_body_, chassis, color);
      VehicleArt_put(self.bus_glass_, chassis, Vector{1.0, 1.0, 1.0});
    } else {
      VehicleArt_put(self.car_glass_, chassis, Vector{1.0, 1.0, 1.0});
      if (logic_not(cockpit_view)) {
        VehicleArt_put(self.car_body_, chassis, color);
        VehicleArt_put(self.car_lights_, chassis, Vector{1.0, 1.0, 1.0});
        VehicleArt_put(self.car_tail_, chassis, logic_select<Vector>(brake_lights, Vector{3.0, 1.0, 1.0}, Vector{1.0, 1.0, 1.0}));
      }
    }
    if (logic_and(cockpit_view, logic_not(bus))) {
      return;
    }
    r = c.s_.radius;
    w = c.s_.wheel_width;
    for (auto it68 = flow_for(n68, 0, 3); it68.next();) {
      ws = c.w_[n68.index];
      s = c.s_;
      travel = kern::formula([](Float travel, Float susp_lower, Float susp_upper) -> Float { return (travel < susp_lower ? susp_lower : (susp_upper < travel ? susp_upper : travel)); }, ws.travel, s.susp_lower, s.susp_upper);
      steer = kern::formula([](Float a, Float the_result) -> Float { return ((a != 0.0) ? the_result : 0.0); }, kern::to<Float>(s.wheels[n68.index].front), car::Car_steer_angle(c).Result);
      a_4 = at(vector_make(s.wheels[n68.index].x, s.wheels[n68.index].y, kern::formula([](Float attach_z, Float travel) -> Float { return (attach_z + travel); }, s.attach_z, travel))).Result;
      a_3 = engine_math_math::Mat4_mul(chassis, a_4).Result;
      a_5 = rot(Vector{0.0, 0.0, 1.0}, steer).Result;
      a_2 = engine_math_math::Mat4_mul(a_3, a_5).Result;
      a_6 = rot(Vector{0.0, 1.0, 0.0}, ws.angle).Result;
      a = engine_math_math::Mat4_mul(a_2, a_6).Result;
      a_7 = engine_math_math::Mat4_scale(vector_make(r, kern::formula([](Float w) -> Float { return kern::math::div(w, 0.62); }, w), r)).Result;
      m = engine_math_math::Mat4_mul(a, a_7).Result;
      VehicleArt_put(self.wheel_, m, Vector{1.0, 1.0, 1.0});
    }
  }

  void VehicleArt_jet(VehicleArt& self, const Mat4& body, Bool gear_down = false, Float nozzle = 0.0, Float thrust01 = 0.0, Bool cockpit_view = false) {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    JetSpec s = JetSpec{12000.0, Vector{100000.0, 18000.0, 115000.0}, 26.0, 9.2, 3.3, 105000.0, 185000.0, 0.34, 0.3, 0.1, 4.0, 0.42, 0.022, 0.14, -0.8, 0.02, 0.15, 0.0, -0.4, -12.0, -0.9, -0.06, -0.35, 0.08, 0.14, 0.12, -0.25, -0.03, 0.07, 0.48, 0.48, 0.5, 7.5, Vector{70000.0, 45000.0, 150000.0}, 1.0, List<JetSpec_Gear>{JetSpec_Gear{Vector{0.0, 5.2, -0.6}, 0.212, true}, JetSpec_Gear{Vector{1.9, -1.4, -0.6}, 0.394, false}, JetSpec_Gear{Vector{-1.9, -1.4, -0.6}, 0.394, false}}, 1.6, 2.05, 0.15, 0.4, 0.6, 0.8, 0.02, 0.5, 9.0, 3.0, 0.38, 3.4, 1.7, 22.0, 10.0, 6.0, 0.35, 85.0, 50.0, 1.05, 1.71};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 n = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (cockpit_view) {
      return;
    }
    VehicleArt_put(self.jet_body_, body, Vector{1.0, 1.0, 1.0});
    a_3 = at(Vector{0.0, 3.2, 0.75}).Result;
    a_2 = engine_math_math::Mat4_mul(body, a_3).Result;
    a_4 = engine_math_math::Mat4_scale(Vector{0.62, 1.7, 0.55}).Result;
    a = engine_math_math::Mat4_mul(a_2, a_4).Result;
    VehicleArt_put(self.jet_glass_, a, Vector{1.0, 1.0, 1.0});
    if (gear_down) {
      s = JetSpec{12000.0, Vector{100000.0, 18000.0, 115000.0}, 26.0, 9.2, 3.3, 105000.0, 185000.0, 0.34, 0.3, 0.1, 4.0, 0.42, 0.022, 0.14, -0.8, 0.02, 0.15, 0.0, -0.4, -12.0, -0.9, -0.06, -0.35, 0.08, 0.14, 0.12, -0.25, -0.03, 0.07, 0.48, 0.48, 0.5, 7.5, Vector{70000.0, 45000.0, 150000.0}, 1.0, List<JetSpec_Gear>{JetSpec_Gear{Vector{0.0, 5.2, -0.6}, 0.212, true}, JetSpec_Gear{Vector{1.9, -1.4, -0.6}, 0.394, false}, JetSpec_Gear{Vector{-1.9, -1.4, -0.6}, 0.394, false}}, 1.6, 2.05, 0.15, 0.4, 0.6, 0.8, 0.02, 0.5, 9.0, 3.0, 0.38, 3.4, 1.7, 22.0, 10.0, 6.0, 0.35, 85.0, 50.0, 1.05, 1.71};
      for (auto it69 = flow_for(n69, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<JetSpec_Gear>(s.gear))))); it69.next();) {
        a_6 = at(vector_add(s.gear[n69.index].p, Vector{0.0, 0.0, -1.45})).Result;
        a_5 = engine_math_math::Mat4_mul(body, a_6).Result;
        VehicleArt_put(self.jet_gear_, a_5, Vector{1.0, 1.0, 1.0});
      }
    }
    if (math_compare<">">(thrust01, 0.02)) {
      a_9 = at(Vector{0.0, -7.3, 0.0}).Result;
      a_8 = engine_math_math::Mat4_mul(body, a_9).Result;
      a_10 = rot(Vector{1.0, 0.0, 0.0}, kern::formula([](Float kPi, Float nozzle) -> Float { return (kern::math::div(kPi, 2.0) + nozzle); }, kPi, nozzle)).Result;
      a_7 = engine_math_math::Mat4_mul(a_8, a_10).Result;
      a_11 = engine_math_math::Mat4_scale(vector_make(1.0, 1.0, kern::formula([](Float thrust01) -> Float { return (1.0 + (4.0 * thrust01)); }, thrust01))).Result;
      n = engine_math_math::Mat4_mul(a_7, a_11).Result;
      VehicleArt_put(self.jet_flame_, n, Vector{1.0, 1.0, 1.0});
    }
  }

  void VehicleArt_drone(VehicleArt& self, const Mat4& body, const Fixed<Vector, 4>& motors, const Fixed<Float, 4>& rotor_angles, Float prop_d = 0.0, Bool armed = false, Bool fpv_view = false) {
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 p = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_7 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    if (fpv_view) {
      return;
    }
    VehicleArt_put(self.drone_frame_, body, Vector{1.0, 1.0, 1.0});
    for (auto it70 = flow_for(n70, 0, 3); it70.next();) {
      a_2 = at(vector_add(motors[n70.index], Vector{0.0, 0.0, -0.004})).Result;
      a = engine_math_math::Mat4_mul(body, a_2).Result;
      VehicleArt_put(self.drone_motor_, a, Vector{1.0, 1.0, 1.0});
      a_5 = at(vector_add(motors[n70.index], Vector{0.0, 0.0, 0.018})).Result;
      a_4 = engine_math_math::Mat4_mul(body, a_5).Result;
      a_6 = rot(Vector{0.0, 0.0, 1.0}, rotor_angles[n70.index]).Result;
      a_3 = engine_math_math::Mat4_mul(a_4, a_6).Result;
      a_7 = engine_math_math::Mat4_scale(vector_make(prop_d, 1.0, 1.0)).Result;
      p = engine_math_math::Mat4_mul(a_3, a_7).Result;
      if (math_compare<">">(kern::formula([](Float index) -> Float { return (kern::math::mod(index, 2.0) != 0.0); }, kern::to<Float>(n70.index)), 0.5)) {
        VehicleArt_put(self.drone_prop_b_, p, Vector{1.0, 1.0, 1.0});
      } else {
        VehicleArt_put(self.drone_prop_a_, p, Vector{1.0, 1.0, 1.0});
      }
    }
    a_9 = at(Vector{0.06, 0.0, 0.018}).Result;
    a_8 = engine_math_math::Mat4_mul(body, a_9).Result;
    VehicleArt_put(self.drone_led_, a_8, logic_select<Vector>(armed, Vector{4.0, 0.2, 0.1}, Vector{0.3, 0.3, 0.3}));
  }

  void VehicleArt_parachute(VehicleArt& self, Vector pilot = Vector{0.0, 0.0, 0.0}, Float heading = 0.0, Bool open = false) {
    Mat4 base = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector top = Vector{0.0, 0.0, 0.0};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_4 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_5 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_6 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Vector item = Vector{0.0, 0.0, 0.0};
    Vector item_2 = Vector{0.0, 0.0, 0.0};
    Vector item_3 = Vector{0.0, 0.0, 0.0};
    Vector item_4 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 4> items = Fixed<Vector, 4>{};
    Vector rim = Vector{0.0, 0.0, 0.0};
    Vector a_7 = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Vector axis = Vector{0.0, 0.0, 0.0};
    Float ang = 0.0;
    Mat4 a_8 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_9 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_10 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_11 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_12 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_13 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    a = at(pilot).Result;
    a_2 = rot(Vector{0.0, 0.0, 1.0}, heading).Result;
    base = engine_math_math::Mat4_mul(a, a_2).Result;
    if (logic_not(open)) {
      return;
    }
    top = Vector{0.0, 0.0, 4.6};
    a_5 = at(top).Result;
    a_4 = engine_math_math::Mat4_mul(base, a_5).Result;
    a_6 = engine_math_math::Mat4_scale(Vector{3.4, 2.5, 0.9}).Result;
    a_3 = engine_math_math::Mat4_mul(a_4, a_6).Result;
    VehicleArt_put(self.chute_, a_3, Vector{1.0, 1.0, 1.0});
    item = Vector{3.1, 2.1, 0.0};
    item_2 = Vector{-3.1, 2.1, 0.0};
    item_3 = Vector{3.1, -2.1, 0.0};
    item_4 = Vector{-3.1, -2.1, 0.0};
    items = list_make<Vector, "4">(item, item_2, item_3, item_4);
    for (auto it71 = flow_for(n71, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(items))))); it71.next();) {
      rim = items[n71.index];
      a_7 = vector_make(kern::formula([](Float x) -> Float { return (x * 0.08); }, vector_break(rim).x), kern::formula([](Float y) -> Float { return (y * 0.08); }, vector_break(rim).y), 0.55);
      b = vector_make(vector_break(rim).x, vector_break(rim).y, kern::formula([](Float z) -> Float { return (z - 0.3); }, vector_break(top).z));
      d = vector_sub(b, a_7);
      len = vector_length(d);
      n = vector_scale(d, kern::formula([](Float len) -> Float { return kern::math::div(1.0, len); }, len));
      axis = vector_cross(Vector{0.0, 0.0, 1.0}, n);
      ang = kern::formula([](Float z) -> Float { return kern::math::acos((z < (-1.0) ? (-1.0) : (1.0 < z ? 1.0 : z))); }, vector_break(n).z);
      a_11 = at(vector_scale(vector_add(a_7, b), 0.5)).Result;
      a_10 = engine_math_math::Mat4_mul(base, a_11).Result;
      a_12 = rot(logic_select<Vector>(math_compare<">">(vector_length(axis), 9.999999717180685e-10), vector_normalize(axis), Vector{1.0, 0.0, 0.0}), ang).Result;
      a_9 = engine_math_math::Mat4_mul(a_10, a_12).Result;
      a_13 = engine_math_math::Mat4_scale(vector_make(1.0, 1.0, len)).Result;
      a_8 = engine_math_math::Mat4_mul(a_9, a_13).Result;
      VehicleArt_put(self.chute_lines_, a_8, Vector{1.0, 1.0, 1.0});
    }
  }

  void VehicleArt_upload(VehicleArt& self, VehicleArt_Part& p) {
    Bool set_visible = false;
    Bool update_instances = false;
    Bool set_visible_2 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(p.id), 0.0))) {
      return;
    }
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(p.transforms)), 0.0)) {
      engine_renderer_set_visible(n72, self.r_, p.id, false);
      set_visible = n72.result;
      return;
    }
    engine_renderer_update_instances(n73, self.r_, p.id, p.transforms, p.tints);
    update_instances = n73.result;
    engine_renderer_set_visible(n74, self.r_, p.id, true);
    set_visible_2 = n74.result;
  }

  void VehicleArt_end(VehicleArt& self) {
    VehicleArt_upload(self, self.car_body_);
    VehicleArt_upload(self, self.car_glass_);
    VehicleArt_upload(self, self.car_lights_);
    VehicleArt_upload(self, self.car_tail_);
    VehicleArt_upload(self, self.wheel_);
    VehicleArt_upload(self, self.bus_body_);
    VehicleArt_upload(self, self.bus_glass_);
    VehicleArt_upload(self, self.jet_body_);
    VehicleArt_upload(self, self.jet_glass_);
    VehicleArt_upload(self, self.jet_gear_);
    VehicleArt_upload(self, self.jet_flame_);
    VehicleArt_upload(self, self.drone_frame_);
    VehicleArt_upload(self, self.drone_motor_);
    VehicleArt_upload(self, self.drone_prop_a_);
    VehicleArt_upload(self, self.drone_prop_b_);
    VehicleArt_upload(self, self.drone_led_);
    VehicleArt_upload(self, self.chute_);
    VehicleArt_upload(self, self.chute_lines_);
    VehicleArt_upload(self, self.pilot_);
  }

  // ---- what nodes give and keep ----
  struct { Bool result = false; MeshData out = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n10;
  struct { Int result = 0; } n11;
  struct { Bool result = false; } n12;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n13;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n14;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n15;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n16;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n17;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n18;
  VehicleArt_makeResult n19;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  hullResult n22;
  VehicleArt_makeResult n23;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n24;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n25;
  VehicleArt_makeResult n26;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n27;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n28;
  VehicleArt_makeResult n29;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n30;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n31;
  VehicleArt_makeResult n32;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n33;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n34;
  VehicleArt_makeResult n35;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n36;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n37;
  VehicleArt_makeResult n38;
  hullResult n39;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n40;
  hullResult n41;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n42;
  struct : kern::Node<43> { Int index = 0; } n43;
  hullResult n44;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n45;
  hullResult n46;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n47;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n48;
  VehicleArt_makeResult n49;
  VehicleArt_makeResult n50;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n51;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n52;
  VehicleArt_makeResult n53;
  VehicleArt_makeResult n54;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n55;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n56;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n57;
  struct : kern::Node<58> { Int index = 0; } n58;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n59;
  VehicleArt_makeResult n60;
  VehicleArt_makeResult n61;
  VehicleArt_makeResult n62;
  VehicleArt_makeResult n63;
  VehicleArt_makeResult n64;
  VehicleArt_makeResult n65;
  VehicleArt_makeResult n66;
  VehicleArt_makeResult n67;
  struct : kern::Node<68> { Int index = 0; } n68;
  struct : kern::Node<69> { Int index = 0; } n69;
  struct : kern::Node<70> { Int index = 0; } n70;
  struct : kern::Node<71> { Int index = 0; } n71;
  struct { Bool result = false; } n72;
  struct { Bool result = false; Bool changed = false; } n73;
  struct { Bool result = false; } n74;
};
