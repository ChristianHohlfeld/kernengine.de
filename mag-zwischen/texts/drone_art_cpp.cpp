struct drone_art_cpp : virtual kern::Library, virtual drone_art, virtual engine_geo_mesh_data, virtual engine_math_math {
  // ---- variables ----
  /// vertex locations 14 and 15
  KERN_CONST u32 kMaxExtensionAttributes = 2;

  // ---- functions ----
  struct rotResult { Mat4 Result = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}; };
  KERN_PURE rotResult rot(Vector axis = Vector{0.0, 0.0, 0.0}, f32 a = 0.0f) {
    Quat axis_angle = Quat{0.0f, 0.0f, 0.0f, 1.0f};
    Mat4 trs = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    axis_angle = engine_quat_axis_angle(axis, a);
    trs = engine_mat4_trs(Vector{0.0, 0.0, 0.0}, axis_angle, Vector{1.0, 1.0, 1.0});
    return {trs};
  }

  struct atResult { Mat4 Result = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}}; };
  KERN_PURE atResult at(Vector p = Vector{0.0, 0.0, 0.0}) {
    Mat4 translate = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    translate = engine_mat4_translate(p);
    return {translate};
  }

  struct matResult { Material Result = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}}; };
  KERN_PURE matResult mat(f32 roughness = 0.0f, f32 metalness = 0.0f, Vector emissive = Vector{0.0, 0.0, 0.0}) {
    Material m = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}};
    m = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}};
    m.roughness = roughness;
    m.metalness = metalness;
    m.emissive = emissive;
    return {m};
  }

  struct DroneArt_makeResult { bool Result = false; };
  DroneArt_makeResult DroneArt_make(DroneArt_Part& p, const MeshData& mesh, const Material& m, Object<kern::Renderer> r = Object<kern::Renderer>{}, bool tinted = false) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    u32 allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 translate = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    bool upsert = false;
    r_ = r;
    engine_renderer_allocate_id(n10, r_);
    allocate_id = n10.result;
    p.id = allocate_id;
    p.tinted = tinted;
    look = MeshLook{Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    translate = engine_mat4_translate(Vector{0.0, 0.0, -1000.0});
    parked = translate;
    engine_upsert(n11, r_, p.id, mesh, look, parked.m, List<f32>{});
    upsert = n11.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(r_, p.id);
      p.id = 0;
      return {false};
    }
    return {true};
  }

  struct DroneArt_initResult { bool Result = false; };
  DroneArt_initResult DroneArt_init(DroneArt& self, Object<kern::Renderer> r = Object<kern::Renderer>{}) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Vector white = Vector{0.0, 0.0, 0.0};
    MeshData frame = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    MeshData box = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    MeshData box_2 = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    MeshData box_3 = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    f32 a = 0.0f;
    Vector item = Vector{0.0, 0.0, 0.0};
    Vector item_2 = Vector{0.0, 0.0, 0.0};
    Vector item_3 = Vector{0.0, 0.0, 0.0};
    Vector item_4 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 4> items = Fixed<Vector, 4>{};
    Vector m = Vector{0.0, 0.0, 0.0};
    f32 len = 0.0f;
    MeshData box_4 = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    Mat4 a_2 = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 a_3 = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Material a_4 = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}};
    MeshData a_5 = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    MeshData cylinder = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    Material a_6 = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}};
    Material prop = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}};
    MeshData a_7 = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    MeshData box_5 = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    MeshData a_8 = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    MeshData box_6 = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    Material led = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}};
    MeshData a_9 = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    MeshData box_7 = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    r_ = r;
    self.r_ = r_;
    white = Vector{1.0, 1.0, 1.0};
    frame = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    box = engine_primitives_box(Vector{0.11, 0.06, 0.03}, Vector{0.1, 0.1, 0.11});
    engine_primitives_append(n12, frame, box, struct_make<Mat4>(List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}));
    frame = n12.dst;
    box_2 = engine_primitives_box(Vector{0.045, 0.035, 0.035}, Vector{0.95, 0.45, 0.1});
    engine_primitives_append(n13, frame, box_2, at(Vector{-0.07, 0.0, 0.028}).Result);
    frame = n13.dst;
    box_3 = engine_primitives_box(Vector{0.1, 0.035, 0.03}, Vector{0.2, 0.25, 0.6});
    engine_primitives_append(n14, frame, box_3, at(Vector{0.015, 0.0, -0.03}).Result);
    frame = n14.dst;
    a = 0.079549514f;
    item = vector_make(kern::formula<f32>([](auto a) { return (-a); }, a), kern::formula<f32>([](auto a) { return (-a); }, a), 0.0f);
    item_2 = vector_make(kern::formula<f32>([](auto a) { return (-a); }, a), a, 0.0f);
    item_3 = vector_make(a, a, 0.0f);
    item_4 = vector_make(a, kern::formula<f32>([](auto a) { return (-a); }, a), 0.0f);
    items = list_make<Vector, "4">(item, item_2, item_3, item_4);
    for (auto it15 = flow_for(n15, 0, math_sub<i64>(list_length<Vector>(items), 1)); it15.next();) {
      m = items[n15.index];
      len = kern::formula<f32, f32>([](auto x, auto y) { return sqrt(((x * x) + (y * y))); }, vector_break(m).x, vector_break(m).y);
      box_4 = engine_primitives_box(vector_make(len, 0.016f, 0.008f), Vector{0.12, 0.12, 0.13});
      a_2 = at(vector_make(kern::formula<f32>([](auto x) { return (x / 2.0f); }, vector_break(m).x), kern::formula<f32>([](auto y) { return (y / 2.0f); }, vector_break(m).y), 0.0f)).Result;
      a_3 = rot(Vector{0.0, 0.0, 1.0}, kern::formula<f32, f32>([](auto y, auto x) { return atan2(y, x); }, vector_break(m).y, vector_break(m).x)).Result;
      engine_primitives_append(n16, frame, box_4, engine_math_math::Mat4_mul(a_2, a_3).Result);
      frame = n16.dst;
    }
    a_4 = mat(0.5f, 0.2f, Vector{0.0, 0.0, 0.0}).Result;
    n17 = DroneArt_make(self.frame_, frame, a_4, r_, false);
    if (logic_not(n17.Result)) {
      return {false};
    }
    cylinder = engine_primitives_cylinder(0.014f, 0.02f, 12, Vector{0.35, 0.35, 0.38});
    a_5 = cylinder;
    a_6 = mat(0.3f, 0.8f, Vector{0.0, 0.0, 0.0}).Result;
    n18 = DroneArt_make(self.motor_, a_5, a_6, r_, false);
    if (logic_not(n18.Result)) {
      return {false};
    }
    prop = mat(0.4f, 0.0f, Vector{0.0, 0.0, 0.0}).Result;
    prop.alpha = AlphaMode::blend;
    prop.color = engine_math_math::Vec4_new(1.0f, 1.0f, 1.0f, 0.72f).Result;
    prop.cast_shadow = false;
    box_5 = engine_primitives_box(Vector{1.0, 0.012, 0.002}, Vector{1.0, 0.64, 0.3});
    a_7 = box_5;
    n19 = DroneArt_make(self.prop_a_, a_7, prop, r_, false);
    if (logic_not(n19.Result)) {
      return {false};
    }
    box_6 = engine_primitives_box(Vector{1.0, 0.012, 0.002}, Vector{0.3, 0.84, 1.0});
    a_8 = box_6;
    n20 = DroneArt_make(self.prop_b_, a_8, prop, r_, false);
    if (logic_not(n20.Result)) {
      return {false};
    }
    led = mat(0.4f, 0.0f, Vector{0.2, 2.5, 0.6}).Result;
    led.unlit = true;
    box_7 = engine_primitives_box(Vector{0.012, 0.012, 0.006}, white);
    a_9 = box_7;
    n21 = DroneArt_make(self.led_, a_9, led, r_, true);
    if (logic_not(n21.Result)) {
      return {false};
    }
    return {true};
  }

  void DroneArt_clear(DroneArt_Part& p) {
    list_clear(p.transforms);
    list_clear(p.tints);
  }

  void DroneArt_put(DroneArt_Part& p, const Mat4& m, Vector tint = Vector{0.0, 0.0, 0.0}) {
    i64 place = 0;
    i64 place_2 = 0;
    f32 item = 0.0f;
    f32 item_2 = 0.0f;
    f32 item_3 = 0.0f;
    f32 item_4 = 0.0f;
    place = list_length<f32>(p.transforms);
    list_insert_list(p.transforms, place, m.m);
    if (p.tinted) {
      place_2 = list_length<f32>(p.tints);
      item = vector_break(tint).x;
      item_2 = vector_break(tint).y;
      item_3 = vector_break(tint).z;
      item_4 = 1.0f;
      list_insert_list(p.tints, place_2, list_make<f32, "4">(item, item_2, item_3, item_4));
    }
  }

  void DroneArt_upload(DroneArt& self, DroneArt_Part& p) {
    bool set_visible = false;
    bool update_instances = false;
    bool set_visible_2 = false;
    if (logic_not(num_cast<bool, u32>(p.id))) {
      return;
    }
    if (math_compare<"==", i64>(list_length<f32>(p.transforms), 0)) {
      engine_renderer_set_visible(n22, self.r_, p.id, false);
      set_visible = n22.result;
      return;
    }
    engine_renderer_update_instances(n23, self.r_, p.id, p.transforms, p.tints);
    update_instances = n23.result;
    engine_renderer_set_visible(n24, self.r_, p.id, true);
    set_visible_2 = n24.result;
  }

  void DroneArt_draw(DroneArt& self, const Mat4& body, const Fixed<Vector, 4>& motors, const Fixed<f32, 4>& rotor_angles, f32 prop_d = 0.0f, bool armed = false, bool fpv_view = false) {
    Mat4 a = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 a_2 = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 p = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 a_3 = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 a_4 = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 a_5 = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 a_6 = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 a_7 = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 scale = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 a_8 = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    Mat4 a_9 = Mat4{List<f32>{1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f}};
    DroneArt_clear(self.frame_);
    DroneArt_clear(self.motor_);
    DroneArt_clear(self.prop_a_);
    DroneArt_clear(self.prop_b_);
    DroneArt_clear(self.led_);
    if (logic_not(fpv_view)) {
      DroneArt_put(self.frame_, body, Vector{1.0, 1.0, 1.0});
      for (auto it25 = flow_for(n25, 0, 3); it25.next();) {
        a_2 = at(vector_add(motors[num_cast<i64, i32>(num_cast<i32, i64>(n25.index))], Vector{0.0, 0.0, -0.004})).Result;
        a = engine_math_math::Mat4_mul(body, a_2).Result;
        DroneArt_put(self.motor_, a, Vector{1.0, 1.0, 1.0});
        a_5 = at(vector_add(motors[num_cast<i64, i32>(num_cast<i32, i64>(n25.index))], Vector{0.0, 0.0, 0.018})).Result;
        a_4 = engine_math_math::Mat4_mul(body, a_5).Result;
        a_6 = rot(Vector{0.0, 0.0, 1.0}, rotor_angles[num_cast<i64, i32>(num_cast<i32, i64>(n25.index))]).Result;
        a_3 = engine_math_math::Mat4_mul(a_4, a_6).Result;
        scale = engine_mat4_scale(vector_make(prop_d, 1.0f, 1.0f));
        a_7 = scale;
        p = engine_math_math::Mat4_mul(a_3, a_7).Result;
        if (kern::formula<i64>([](auto index) { return num_cast<bool, i32>((num_cast<i32, i64>(index) % 2)); }, n25.index)) {
          DroneArt_put(self.prop_b_, p, Vector{1.0, 1.0, 1.0});
        } else {
          DroneArt_put(self.prop_a_, p, Vector{1.0, 1.0, 1.0});
        }
      }
      a_9 = at(Vector{0.06, 0.0, 0.018}).Result;
      a_8 = engine_math_math::Mat4_mul(body, a_9).Result;
      DroneArt_put(self.led_, a_8, logic_select<Vector>(armed, Vector{4.0, 0.2, 0.1}, Vector{0.3, 0.3, 0.3}));
    }
    DroneArt_upload(self, self.frame_);
    DroneArt_upload(self, self.motor_);
    DroneArt_upload(self, self.prop_a_);
    DroneArt_upload(self, self.prop_b_);
    DroneArt_upload(self, self.led_);
  }

  void DroneArt_hide(DroneArt& self) {
    DroneArt_clear(self.frame_);
    DroneArt_clear(self.motor_);
    DroneArt_clear(self.prop_a_);
    DroneArt_clear(self.prop_b_);
    DroneArt_clear(self.led_);
    DroneArt_upload(self, self.frame_);
    DroneArt_upload(self, self.motor_);
    DroneArt_upload(self, self.prop_a_);
    DroneArt_upload(self, self.prop_b_);
    DroneArt_upload(self, self.led_);
  }

  // ---- what nodes give and keep ----
  struct { u32 result = 0; } n10;
  struct { bool result = false; } n11;
  struct { MeshData dst = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n12;
  struct { MeshData dst = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n13;
  struct { MeshData dst = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n14;
  struct : kern::Node<15> { i64 index = 0; } n15;
  struct { MeshData dst = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n16;
  DroneArt_makeResult n17;
  DroneArt_makeResult n18;
  DroneArt_makeResult n19;
  DroneArt_makeResult n20;
  DroneArt_makeResult n21;
  struct { bool result = false; } n22;
  struct { bool result = false; bool changed = false; } n23;
  struct { bool result = false; } n24;
  struct : kern::Node<25> { i64 index = 0; } n25;
};
