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
      engine_renderer_set_visible(n16, self.r_, p.id, false);
      set_visible = n16.result;
      return;
    }
    engine_renderer_update_instances(n17, self.r_, p.id, p.transforms, p.tints);
    update_instances = n17.result;
    engine_renderer_set_visible(n18, self.r_, p.id, true);
    set_visible_2 = n18.result;
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
      for (auto it19 = flow_for(n19, 0, 3); it19.next();) {
        a_2 = at(vector_add(motors[num_cast<i64, i32>(num_cast<i32, i64>(n19.index))], Vector{0.0, 0.0, -0.004})).Result;
        a = engine_math_math::Mat4_mul(body, a_2).Result;
        DroneArt_put(self.motor_, a, Vector{1.0, 1.0, 1.0});
        a_5 = at(vector_add(motors[num_cast<i64, i32>(num_cast<i32, i64>(n19.index))], Vector{0.0, 0.0, 0.018})).Result;
        a_4 = engine_math_math::Mat4_mul(body, a_5).Result;
        a_6 = rot(Vector{0.0, 0.0, 1.0}, rotor_angles[num_cast<i64, i32>(num_cast<i32, i64>(n19.index))]).Result;
        a_3 = engine_math_math::Mat4_mul(a_4, a_6).Result;
        scale = engine_mat4_scale(vector_make(prop_d, 1.0f, 1.0f));
        a_7 = scale;
        p = engine_math_math::Mat4_mul(a_3, a_7).Result;
        if (kern::formula<i64>([](auto index) { return num_cast<bool, i32>((num_cast<i32, i64>(index) % 2)); }, n19.index)) {
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
  struct { bool result = false; } n16;
  struct { bool result = false; bool changed = false; } n17;
  struct { bool result = false; } n18;
  struct : kern::Node<19> { i64 index = 0; } n19;
};
