struct engine_geo_mesh_data : virtual kern::Library, virtual engine_gfx_mesh {
  // ---- types ----
  KERN_STRUCT(MeshData) { List<Float> positions = List<Float>{}; List<Float> normals = List<Float>{}; List<Float> colors = List<Float>{}; List<Float> uvs = List<Float>{}; List<Float> extra = List<Float>{}; Int extra_size = 0; List<Int> indices = List<Int>{}; };
  KERN_STRUCT(WeldStats) { Int source_vertices = 0; Int unique_vertices = 0; Int bytes_before = 0; Int bytes_after = 0; Bool indexed = false; };
  KERN_STRUCT(MeshLook) { Material material = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}; Int extra_slot = 0; Int texture = 0; Bool visible = true; Int layers = 1; Bool pick_bvh = true; };

  // ---- functions ----
  struct MeshData_vertex_countResult { Int Result = 0; };
  KERN_PURE MeshData_vertex_countResult MeshData_vertex_count(const MeshData& self) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::trunc(kern::math::div(a, 3.0))); }, kern::to<Float>(list_length<Float>(self.positions))))};
  }

  struct MeshData_triangle_countResult { Int Result = 0; };
  KERN_PURE MeshData_triangle_countResult MeshData_triangle_count(const MeshData& self) {
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((a != 0.0) ? kern::math::trunc(kern::math::div(a_2, 3.0)) : kern::math::wrap32(kern::math::trunc(kern::math::div(a_3, 3.0)))); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<Int>(self.indices)), 0.0)), kern::to<Float>(MeshData_vertex_count(self).Result), kern::to<Float>(list_length<Int>(self.indices))))};
  }

  void MeshData_push(MeshData& self, Vector p = Vector{0.0, 0.0, 0.0}, Vector n = Vector{0.0, 0.0, 0.0}, Vector c = Vector{0.0, 0.0, 0.0}, Vector uv = Vector{0.0, 0.0, 0.0}) {
    Int place = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Float item_3 = 0.0;
    Int place_2 = 0;
    Float item_4 = 0.0;
    Float item_5 = 0.0;
    Float item_6 = 0.0;
    Int place_3 = 0;
    Float item_7 = 0.0;
    Float item_8 = 0.0;
    Float item_9 = 0.0;
    Int place_4 = 0;
    Float item_10 = 0.0;
    Float item_11 = 0.0;
    place = list_length<Float>(self.positions);
    item = vector_break(p).x;
    item_2 = vector_break(p).y;
    item_3 = vector_break(p).z;
    list_insert_list(self.positions, place, list_make<Float, "3">(item, item_2, item_3));
    place_2 = list_length<Float>(self.normals);
    item_4 = vector_break(n).x;
    item_5 = vector_break(n).y;
    item_6 = vector_break(n).z;
    list_insert_list(self.normals, place_2, list_make<Float, "3">(item_4, item_5, item_6));
    place_3 = list_length<Float>(self.colors);
    item_7 = vector_break(c).x;
    item_8 = vector_break(c).y;
    item_9 = vector_break(c).z;
    list_insert_list(self.colors, place_3, list_make<Float, "3">(item_7, item_8, item_9));
    place_4 = list_length<Float>(self.uvs);
    item_10 = vector_break(uv).x;
    item_11 = vector_break(uv).y;
    list_insert_list(self.uvs, place_4, list_make<Float, "2">(item_10, item_11));
  }

  void MeshData_push_extra(MeshData& self, const Fixed<Float, 3>& values, Int values_at = 0) {
    Int place = 0;
    Int first = 0;
    Int last = 0;
    place = list_length<Float>(self.extra);
    first = values_at;
    last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(values_at), kern::to<Float>(self.extra_size)))))));
    for (auto it10 = flow_for(n10, first, last); it10.next();) {
      list_insert(self.extra, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(place), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(n10.index), kern::to<Float>(first))))))), list_get<Float>(values, n10.index));
    }
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
