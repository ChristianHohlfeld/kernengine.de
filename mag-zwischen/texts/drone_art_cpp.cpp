struct drone_art_cpp : virtual kern::Library, virtual drone_art, virtual engine_geo_mesh_data, virtual engine_math_math {
  // ---- variables ----
  /// vertex locations 14 and 15
  KERN_CONST U32 kMaxExtensionAttributes = 2;

  // ---- functions ----
  struct rotResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE rotResult rot(Vector axis = Vector{0.0, 0.0, 0.0}, Float a = 0.0) {
    return {engine_math_math::Mat4_trs(Vector{0.0, 0.0, 0.0}, engine_math_math::Quat_axis_angle(axis, a).Result, Vector{1.0, 1.0, 1.0}).Result};
  }

  struct atResult { Mat4 Result = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}}; };
  KERN_PURE atResult at(Vector p = Vector{0.0, 0.0, 0.0}) {
    return {engine_math_math::Mat4_translate(p).Result};
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

  struct DroneArt_makeResult { Bool Result = false; };
  DroneArt_makeResult DroneArt_make(DroneArt_Part& p, const MeshData& mesh, const Material& m, Object<kern::Renderer> r = Object<kern::Renderer>{}, Bool tinted = false) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool upsert = false;
    r_ = r;
    engine_renderer_allocate_id(n10, r_);
    allocate_id = n10.result;
    p.id = allocate_id;
    p.tinted = tinted;
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

  struct DroneArt_initResult { Bool Result = false; };
  DroneArt_initResult DroneArt_init(DroneArt& self, Object<kern::Renderer> r = Object<kern::Renderer>{}) {
    Object<kern::Renderer> r_ = Object<kern::Renderer>{};
    Vector white = Vector{0.0, 0.0, 0.0};
    MeshData frame = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Float a = 0.0;
    Vector item = Vector{0.0, 0.0, 0.0};
    Vector item_2 = Vector{0.0, 0.0, 0.0};
    Vector item_3 = Vector{0.0, 0.0, 0.0};
    Vector item_4 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 4> items = Fixed<Vector, 4>{};
    Vector m = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    MeshData box_4 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Mat4 a_2 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Mat4 a_3 = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Material a_4 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_5 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material a_6 = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Material prop = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_7 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_5 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData a_8 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_6 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material led = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_9 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box_7 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    r_ = r;
    self.r_ = r_;
    white = Vector{1.0, 1.0, 1.0};
    frame = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    box = engine_primitives_box(Vector{0.11, 0.06, 0.03}, Vector{0.1, 0.1, 0.11});
    engine_primitives_append(n12, frame, box, struct_make<Mat4>(List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}));
    frame = n12.dst;
    box_2 = engine_primitives_box(Vector{0.045, 0.035, 0.035}, Vector{0.95, 0.45, 0.1});
    engine_primitives_append(n13, frame, box_2, at(Vector{-0.07, 0.0, 0.028}).Result);
    frame = n13.dst;
    box_3 = engine_primitives_box(Vector{0.1, 0.035, 0.03}, Vector{0.2, 0.25, 0.6});
    engine_primitives_append(n14, frame, box_3, at(Vector{0.015, 0.0, -0.03}).Result);
    frame = n14.dst;
    a = 0.07954951288348659;
    item = vector_make(kern::formula([](Float a) -> Float { return (-a); }, a), kern::formula([](Float a) -> Float { return (-a); }, a), 0.0);
    item_2 = vector_make(kern::formula([](Float a) -> Float { return (-a); }, a), a, 0.0);
    item_3 = vector_make(a, a, 0.0);
    item_4 = vector_make(a, kern::formula([](Float a) -> Float { return (-a); }, a), 0.0);
    items = list_make<Vector, "4">(item, item_2, item_3, item_4);
    for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(items))))); it15.next();) {
      m = items[n15.index];
      len = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt(((x * x) + (y * y))); }, vector_break(m).x, vector_break(m).y);
      box_4 = engine_primitives_box(vector_make(len, 0.016, 0.008), Vector{0.12, 0.12, 0.13});
      a_2 = at(vector_make(kern::formula([](Float x) -> Float { return kern::math::div(x, 2.0); }, vector_break(m).x), kern::formula([](Float y) -> Float { return kern::math::div(y, 2.0); }, vector_break(m).y), 0.0)).Result;
      a_3 = rot(Vector{0.0, 0.0, 1.0}, kern::formula([](Float y, Float x) -> Float { return kern::math::atan2(y, x); }, vector_break(m).y, vector_break(m).x)).Result;
      engine_primitives_append(n16, frame, box_4, engine_math_math::Mat4_mul(a_2, a_3).Result);
      frame = n16.dst;
    }
    a_4 = mat(0.5, 0.2, Vector{0.0, 0.0, 0.0}).Result;
    n17 = DroneArt_make(self.frame_, frame, a_4, r_, false);
    if (logic_not(n17.Result)) {
      return {false};
    }
    cylinder = engine_primitives_cylinder(0.014, 0.02, 12, Vector{0.35, 0.35, 0.38});
    a_5 = cylinder;
    a_6 = mat(0.3, 0.8, Vector{0.0, 0.0, 0.0}).Result;
    n18 = DroneArt_make(self.motor_, a_5, a_6, r_, false);
    if (logic_not(n18.Result)) {
      return {false};
    }
    prop = mat(0.4, 0.0, Vector{0.0, 0.0, 0.0}).Result;
    prop.alpha = AlphaMode::blend;
    prop.color = engine_math_math::Vec4_new(1.0, 1.0, 1.0, 0.72).Result;
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
    led = mat(0.4, 0.0, Vector{0.2, 2.5, 0.6}).Result;
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

  void DroneArt_upload(DroneArt& self, DroneArt_Part& p) {
    Bool set_visible = false;
    Bool update_instances = false;
    Bool set_visible_2 = false;
    if (logic_not(math_compare<"!=">(kern::to<Float>(p.id), 0.0))) {
      return;
    }
    if (math_compare<"==">(kern::to<Float>(list_length<Float>(p.transforms)), 0.0)) {
      engine_renderer_set_visible(n22, self.r_, p.id, false);
      set_visible = n22.result;
      return;
    }
    engine_renderer_update_instances(n23, self.r_, p.id, p.transforms, p.tints);
    update_instances = n23.result;
    engine_renderer_set_visible(n24, self.r_, p.id, true);
    set_visible_2 = n24.result;
  }

  void DroneArt_draw(DroneArt& self, const Mat4& body, const Fixed<Vector, 4>& motors, const Fixed<Float, 4>& rotor_angles, Float prop_d = 0.0, Bool armed = false, Bool fpv_view = false) {
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
    DroneArt_clear(self.frame_);
    DroneArt_clear(self.motor_);
    DroneArt_clear(self.prop_a_);
    DroneArt_clear(self.prop_b_);
    DroneArt_clear(self.led_);
    if (logic_not(fpv_view)) {
      DroneArt_put(self.frame_, body, Vector{1.0, 1.0, 1.0});
      for (auto it25 = flow_for(n25, 0, 3); it25.next();) {
        a_2 = at(vector_add(motors[n25.index], Vector{0.0, 0.0, -0.004})).Result;
        a = engine_math_math::Mat4_mul(body, a_2).Result;
        DroneArt_put(self.motor_, a, Vector{1.0, 1.0, 1.0});
        a_5 = at(vector_add(motors[n25.index], Vector{0.0, 0.0, 0.018})).Result;
        a_4 = engine_math_math::Mat4_mul(body, a_5).Result;
        a_6 = rot(Vector{0.0, 0.0, 1.0}, rotor_angles[n25.index]).Result;
        a_3 = engine_math_math::Mat4_mul(a_4, a_6).Result;
        a_7 = engine_math_math::Mat4_scale(vector_make(prop_d, 1.0, 1.0)).Result;
        p = engine_math_math::Mat4_mul(a_3, a_7).Result;
        if (math_compare<">">(kern::formula([](Float index) -> Float { return (kern::math::mod(index, 2.0) != 0.0); }, kern::to<Float>(n25.index)), 0.5)) {
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
  struct { Int result = 0; } n10;
  struct { Bool result = false; } n11;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n12;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n13;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n16;
  DroneArt_makeResult n17;
  DroneArt_makeResult n18;
  DroneArt_makeResult n19;
  DroneArt_makeResult n20;
  DroneArt_makeResult n21;
  struct { Bool result = false; } n22;
  struct { Bool result = false; Bool changed = false; } n23;
  struct { Bool result = false; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
};
