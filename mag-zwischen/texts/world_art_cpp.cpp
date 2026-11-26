struct world_art_cpp : virtual kern::Library, virtual engine_geo_mesh_data, virtual engine_math_math, virtual world_art {
  // ---- variables ----
  /// vertex locations 14 and 15
  KERN_CONST U32 kMaxExtensionAttributes = 2;

  // ---- functions ----
  struct WorldArt_partResult { U32 Result = 0; };
  WorldArt_partResult WorldArt_part(WorldArt& self, const MeshData& mesh, const Material& m, Bool tinted = false) {
    WorldArt_Part p = WorldArt_Part{0, List<Float>{}, List<Float>{}, false};
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Mat4 parked = Mat4{List<Float>{1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0}};
    Bool upsert = false;
    p = WorldArt_Part{0, List<Float>{}, List<Float>{}, false};
    engine_renderer_allocate_id(n10, self.r_);
    allocate_id = n10.result;
    p.id = allocate_id;
    p.tinted = tinted;
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    look.material = m;
    look.extra_slot = kMaxExtensionAttributes;
    look.visible = false;
    parked = engine_math_math::Mat4_translate(Vector{0.0, 0.0, -1000.0}).Result;
    engine_upsert(n11, self.r_, p.id, mesh, look, parked.m, List<Float>{});
    upsert = n11.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, p.id);
      return {0};
    }
    list_add(self.parts_, p);
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32((a - 1.0)); }, kern::to<Float>(list_length<WorldArt_Part>(self.parts_))))};
  }

  struct WorldArt_initResult { Bool Result = false; };
  WorldArt_initResult WorldArt_init(WorldArt& self, Object<kern::Renderer> r = Object<kern::Renderer>{}) {
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData box = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData a_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cyl = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData cylinder = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    Material lit = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    MeshData a_3 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    MeshData sphere_2 = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    self.r_ = r;
    list_clear(self.parts_);
    list_add(self.parts_, struct_make<WorldArt_Part>(0, List<Float>{}, List<Float>{}, false));
    m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    m.roughness = 0.85;
    box = engine_primitives_box(Vector{1.0, 1.0, 1.0}, Vector{1.0, 1.0, 1.0});
    a = box;
    n12 = WorldArt_part(self, a, m, true);
    self.cube = n12.Result;
    sphere = engine_primitives_sphere(1.0, 12, 8, Vector{1.0, 1.0, 1.0});
    a_2 = sphere;
    n13 = WorldArt_part(self, a_2, m, true);
    self.sphere = n13.Result;
    cyl = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}};
    cylinder = engine_primitives_cylinder(1.0, 1.0, 12, Vector{1.0, 1.0, 1.0});
    engine_primitives_append(n14, cyl, cylinder, engine_math_math::Mat4_translate(Vector{0.0, 0.0, 0.5}).Result);
    cyl = n14.dst;
    n15 = WorldArt_part(self, cyl, m, true);
    self.cylinder = n15.Result;
    lit = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    lit.unlit = true;
    lit.emissive = Vector{2.2, 2.2, 2.2};
    lit.cast_shadow = false;
    sphere_2 = engine_primitives_sphere(1.0, 10, 6, Vector{1.0, 1.0, 1.0});
    a_3 = sphere_2;
    n16 = WorldArt_part(self, a_3, lit, true);
    self.glow = n16.Result;
    return {logic_and(logic_and(logic_and(math_compare<"!=">(kern::to<Float>(self.cube), 0.0), math_compare<"!=">(kern::to<Float>(self.sphere), 0.0)), math_compare<"!=">(kern::to<Float>(self.cylinder), 0.0)), math_compare<"!=">(kern::to<Float>(self.glow), 0.0))};
  }

  void WorldArt_begin(WorldArt& self) {
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldArt_Part>(self.parts_))))); it17.next();) {
      list_clear(self.parts_[n17.index].transforms);
      list_clear(self.parts_[n17.index].tints);
    }
  }

  void WorldArt_put(WorldArt& self, const Mat4& m, U32 id = 0, Vector tint = Vector{0.0, 0.0, 0.0}) {
    Int place = 0;
    Int place_2 = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Float item_4 = 0.0;
    if (logic_or(math_compare<"==">(kern::to<Float>(id), 0.0), math_compare<">">(kern::formula([](Float id, Float a) -> Float { return (!((id < 0.0) != (a < 0.0) ? (a < 0.0) : (id < a))); }, kern::to<Float>(id), kern::to<Float>(list_length<WorldArt_Part>(self.parts_))), 0.5))) {
      return;
    }
    place = list_length<Float>(self.parts_[id].transforms);
    list_insert_list(self.parts_[id].transforms, place, m.m);
    if (self.parts_[id].tinted) {
      place_2 = list_length<Float>(self.parts_[id].tints);
      item = vector_break(tint).x;
      item_2 = vector_break(tint).y;
      item_3 = vector_break(tint).z;
      item_4 = 1.0;
      list_insert_list(self.parts_[id].tints, place_2, list_make<Float, "4">(item, item_2, item_3, item_4));
    }
  }

  void WorldArt_end(WorldArt& self) {
    Bool skip = false;
    Bool set_visible = false;
    Bool update_instances = false;
    Bool set_visible_2 = false;
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WorldArt_Part>(self.parts_))))); it18.next();) {
      skip = false;
      if (logic_not(math_compare<"!=">(kern::to<Float>(self.parts_[n18.index].id), 0.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (math_compare<"==">(kern::to<Float>(list_length<Float>(self.parts_[n18.index].transforms)), 0.0)) {
          engine_renderer_set_visible(n19, self.r_, self.parts_[n18.index].id, false);
          set_visible = n19.result;
          skip = true;
        }
        if (logic_not(skip)) {
          engine_renderer_update_instances(n20, self.r_, self.parts_[n18.index].id, self.parts_[n18.index].transforms, self.parts_[n18.index].tints);
          update_instances = n20.result;
          engine_renderer_set_visible(n21, self.r_, self.parts_[n18.index].id, true);
          set_visible_2 = n21.result;
        }
      }
    }
  }

  void WorldArt_set_mesh(WorldArt& self, U32& slot, const MeshData& mesh, const Material& m) {
    Bool remove = false;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Int allocate_id = 0;
    Bool upsert = false;
    if (math_compare<"==">(kern::to<Float>(engine_geo_mesh_data::MeshData_vertex_count(mesh).Result), 0.0)) {
      if (math_compare<"!=">(kern::to<Float>(slot), 0.0)) {
        engine_renderer_remove(n22, self.r_, slot);
        remove = n22.result;
      }
      slot = 0;
      return;
    }
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    look.material = m;
    if (logic_not(math_compare<"!=">(kern::to<Float>(slot), 0.0))) {
      engine_renderer_allocate_id(n23, self.r_);
      allocate_id = n23.result;
      slot = allocate_id;
    }
    engine_upsert(n24, self.r_, slot, mesh, look, List<Float>{}, List<Float>{});
    upsert = n24.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, slot);
      slot = 0;
    }
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n10;
  struct { Bool result = false; } n11;
  WorldArt_partResult n12;
  WorldArt_partResult n13;
  struct { MeshData dst = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n14;
  WorldArt_partResult n15;
  WorldArt_partResult n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct { Bool result = false; } n19;
  struct { Bool result = false; Bool changed = false; } n20;
  struct { Bool result = false; } n21;
  struct { Bool result = false; } n22;
  struct { Int result = 0; } n23;
  struct { Bool result = false; } n24;
};
