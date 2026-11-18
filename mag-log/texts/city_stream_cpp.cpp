struct city_stream_cpp : virtual kern::Library, virtual city_stream, virtual engine_geo_buildings, virtual engine_geo_mesh_data, virtual engine_physics_physics {
  // ---- variables ----
  /// the building mesh chunks (BuildingOptions::chunk_size)
  KERN_CONST Float kChunk = 160.0;
  /// The map features of a tile that is not loaded: none.
  KERN_CONST MapFeatures kNoFeatures = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
  /// vertex locations 14 and 15
  KERN_CONST Int kMaxExtensionAttributes = 2;

  // ---- functions ----
  struct konstanz_style_hexResult { Rgb Result = Rgb{1.0, 1.0, 1.0}; };
  /// world_city_buildings.mjs WALLS / ROOFS (sRGB) and the cornice mix with #f4efe4 (0.45)
  KERN_PURE konstanz_style_hexResult konstanz_style_hex(Int h = 0) {
    return {struct_make<Rgb>(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(h)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(h)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(h)))};
  }

  struct konstanz_styleResult { BuildingStyle Result = BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}; };
  KERN_PURE konstanz_styleResult konstanz_style() {
    BuildingStyle s = BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45};
    s = BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45};
    s.walls = list_make<Rgb, "8">(konstanz_style_hex(13221806).Result, konstanz_style_hex(12167308).Result, konstanz_style_hex(9327683).Result, konstanz_style_hex(10263188).Result, konstanz_style_hex(14209732).Result, konstanz_style_hex(11041887).Result, konstanz_style_hex(8226192).Result, konstanz_style_hex(13350810).Result);
    s.roofs = list_make<Rgb, "4">(konstanz_style_hex(4869200).Result, konstanz_style_hex(3882304).Result, konstanz_style_hex(7226164).Result, konstanz_style_hex(5593180).Result);
    s.trim = konstanz_style_hex(16052196).Result;
    s.trim_mix = 0.45;
    return {s};
  }

  struct road_palette_rgbResult { Rgb Result = Rgb{1.0, 1.0, 1.0}; };
  KERN_PURE road_palette_rgbResult road_palette_rgb(Int h = 0) {
    return {struct_make<Rgb>(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(h)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(h)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(h)))};
  }

  struct road_paletteResult { List<Rgb> Result = List<Rgb>{}; };
  KERN_PURE road_paletteResult road_palette() {
    return {list_make<Rgb, "6">(road_palette_rgb(3948356).Result, road_palette_rgb(3422012).Result, road_palette_rgb(15131868).Result, road_palette_rgb(8815484).Result, road_palette_rgb(3104646).Result, road_palette_rgb(10130570).Result)};
  }

  struct area_colorResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE area_colorResult area_color(AreaKind k = AreaKind::water) {
    Int value = 0;
    Int value_2 = 0;
    Int value_3 = 0;
    Int value_4 = 0;
    Int value_5 = 0;
    Int value_6 = 0;
    switch (flow_switch_enum<AreaKind>(k)) {
      case AreaKind::water: {
        value = 3104646;
        return {vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value)))};
      } break;
      case AreaKind::park: {
        value_2 = 5205557;
        return {vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value_2)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value_2)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value_2)))};
      } break;
      case AreaKind::wood: {
        value_3 = 3823401;
        return {vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value_3)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value_3)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value_3)))};
      } break;
      case AreaKind::grass: {
        value_4 = 5929532;
        return {vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value_4)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value_4)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value_4)))};
      } break;
      case AreaKind::sand: {
        value_5 = 13219983;
        return {vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value_5)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value_5)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value_5)))};
      } break;
      case AreaKind::pitch: {
        value_6 = 5208629;
        return {vector_make(kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 16.0), 255.0), 255.0); }, kern::to<Float>(value_6)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(kern::math::shr(h, 8.0), 255.0), 255.0); }, kern::to<Float>(value_6)), kern::formula([](Float h) -> Float { return kern::math::div(kern::math::band(h, 255.0), 255.0); }, kern::to<Float>(value_6)))};
      } break;
    }
  }

  struct signed_areaResult { Float Result = 0.0; };
  KERN_PURE signed_areaResult signed_area(const List<Vector>& r) {
    Float a = 0.0;
    Int i = 0;
    Int j = 0;
    Int was = 0;
    a = 0.0;
    i = 0;
    j = kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Vector>(r))));
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(r))), 0.5))) {
      a += kern::formula([](Float x, Float y, Float x_2, Float y_2) -> Float { return ((x * y) - (x_2 * y_2)); }, vector_break(r[j]).x, vector_break(r[i]).y, vector_break(r[i]).x, vector_break(r[j]).y);
      was = i;
      i += 1;
      j = was;
    }
    return {kern::formula([](Float a) -> Float { return (a * 0.5); }, a)};
  }

  struct rect_distance_toResult { Float Result = 0.0; };
  KERN_PURE rect_distance_toResult rect_distance_to(const Vec4& r, Float x = 0.0, Float y = 0.0) {
    Float dx = 0.0;
    Float dy = 0.0;
    dx = kern::formula([](Float x, Float x_2, Float z) -> Float { return kern::math::max(kern::math::max((x - x_2), (x_2 - z)), 0.0); }, r.x, x, r.z);
    dy = kern::formula([](Float y, Float y_2, Float w) -> Float { return kern::math::max(kern::math::max((y - y_2), (y_2 - w)), 0.0); }, r.y, y, r.w);
    return {kern::formula([](Float dx, Float dy) -> Float { return kern::math::sqrt((dx * dx) + (dy * dy)); }, dx, dy)};
  }

  struct cell_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE cell_keyResult cell_key(Int x = 0, Int y = 0) {
    return {int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32(x); }, kern::to<Float>(x)))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::wrap32(y); }, kern::to<Float>(y)))))};
  }

  struct cell_ofResult { Int64 Result = Int64{0x0}; };
  KERN_PURE cell_ofResult cell_of(Vector c = Vector{0.0, 0.0, 0.0}) {
    return {cell_key(kern::to<Int>(kern::formula([](Float x, Float kChunk) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kChunk))); }, vector_break(c).x, kChunk)), kern::to<Int>(kern::formula([](Float y, Float kChunk) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kChunk))); }, vector_break(c).y, kChunk))).Result};
  }

  struct building_optionsResult { BuildingOptions Result = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}}; };
  KERN_PURE building_optionsResult building_options() {
    BuildingOptions bo = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
    bo = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
    bo.chunk_size = kChunk;
    bo.max_buildings = 12000;
    bo.style = konstanz_style().Result;
    return {bo};
  }

  /// world_city_buildings.mjs: at most MAX_VERTICES 96 per ring; the centre picks the chunk
  void put_footprint(const MapFeatures_Building& b, List<Vector>& thin, Int fs = 0, Vector c = Vector{0.0, 0.0, 0.0}) {
    Int fs_ = 0;
    Bool skip = false;
    Int step = 0;
    Int k = 0;
    fs_ = fs;
    engine_footprint_stream_begin_2(fs_, b.seed, b.base, b.top, vector_break(c).x, vector_break(c).y);
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(b.rings))))); it10.next();) {
      skip = false;
      if (math_compare<"<=">(kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(b.rings[n10.index]).items)), 96.0)) {
        engine_footprint_stream_ring(fs_, struct_break<Vector_row>(b.rings[n10.index]).items, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(b.rings[n10.index]).items)))));
        skip = true;
      }
      if (logic_not(skip)) {
        step = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div((a + 95.0), 96.0)); }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(b.rings[n10.index]).items))));
        list_clear(thin);
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(b.rings[n10.index]).items))), 0.5))) {
          list_add(thin, b.rings[n10.index].items[k]);
          k += step;
        }
        engine_footprint_stream_ring(fs_, thin, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(thin)))));
      }
    }
  }

  void CityStream_remove_drop(CityStream& self, const List<Int>& list) {
    Int id = 0;
    Bool remove = false;
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(list))))); it12.next();) {
      id = list[n12.index];
      engine_renderer_remove(n13, self.r_, id);
      remove = n13.result;
    }
  }

  void remove_cancel(Int& job) {
    if (math_compare<"!=">(kern::to<Float>(job), 0.0)) {
      engine_jobs_cancel(job);
      job = 0;
    }
  }

  void CityStream_furniture(const CityStream& self, List<Int64>& out) {
    list_clear(out);
    for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it19.next();) {
      if (self.tiles_[n19.index][0].prepared) {
        list_add(out, map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n19.index));
      }
    }
    list_sort<nullptr>(out);
  }

  struct CityStream_featuresResult { MapFeatures Result = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}; };
  KERN_PURE CityStream_featuresResult CityStream_features(const CityStream& self, Int64 tile = Int64{0x0}) {
    Int it = 0;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<CityStream_Tile>>(self.tiles_, tile).index), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))));
    return {logic_select<MapFeatures>(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_)))), self.tiles_[it][0].features, kNoFeatures)};
  }

  void CityStream_drop_cell(CityStream& self, CityStream_Tile& t, Int64 cell = Int64{0x0}) {
    Int it = 0;
    Int at = 0;
    Int id = 0;
    Bool remove = false;
    Int gone = 0;
    Int at_2 = 0;
    Int64 key = Int64{0x0};
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<Int>>(t.cell_ids, cell).index), kern::to<Float>(map_length<Int64, List<Int>>(t.cell_ids))));
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(t.cell_ids)))) {
      return;
    }
    at = it;
    for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(t.cell_ids[at]))))); it21.next();) {
      id = t.cell_ids[at][n21.index];
      engine_renderer_remove(n22, self.r_, id);
      remove = n22.result;
      gone = id;
      while (loop(list_contains<Int>(t.meshes, gone))) {
        list_remove(t.meshes, gone);
      }
    }
    at_2 = it;
    map_remove_at(t.cell_ids, at_2);
    key = cell;
    set_remove(n23, t.dirty_cells, key);
  }

  void CityStream_upload(CityStream& self, CityStream_Tile& t, GeoChunk& chunk, const Material& material, Bool walls = false) {
    WeldStats weld = WeldStats{0, 0, 0, 0, false};
    Int id = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    List<Int> t_2 = List<Int>{};
    Int64 key = Int64{0x0};
    List<Int> put = List<Int>{};
    if (math_compare<"==">(kern::to<Float>(engine_geo_mesh_data::MeshData_vertex_count(chunk.mesh).Result), 0.0)) {
      return;
    }
    if (logic_not(chunk.welded)) {
      engine_weld(n24, chunk.mesh, 0);
      weld = n24.result;
      chunk.mesh = n24.mesh;
    }
    engine_renderer_allocate_id(n25, self.r_);
    allocate_id = n25.result;
    id = allocate_id;
    self.upload_count_ = kern::to<Int>(kern::formula([](Float upload_count_) -> Float { return kern::math::wrap32(upload_count_ + 1.0); }, kern::to<Float>(self.upload_count_)));
    self.upload_vertices_ = int64_add(self.upload_vertices_, int64_of_int(engine_geo_mesh_data::MeshData_vertex_count(chunk.mesh).Result));
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = material;
    look.extra_slot = kern::to<Int>(kern::formula([](Float walls, Float kMaxExtensionAttributes) -> Float { return ((walls != 0.0) ? 0.0 : kMaxExtensionAttributes); }, kern::to<Float>(walls), kern::to<Float>(kMaxExtensionAttributes)));
    look.pick_bvh = false;
    engine_upsert(n26, self.r_, id, chunk.mesh, look, List<Float>{}, List<Float>{});
    upsert = n26.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, id);
      return;
    }
    list_add(t.meshes, id);
    if (walls) {
      key = cell_key(chunk.x, chunk.y).Result;
      map_add(n27, t.cell_ids, key);
      put = t_2;
      t_2 = t.cell_ids[n27.index];
      list_add(t_2, id);
      t.cell_ids[n27.index] = put;
    }
    t.triangles = kern::to<Int>(kern::formula([](Float triangles, Float a) -> Float { return kern::math::wrap32(triangles + a); }, kern::to<Float>(t.triangles), kern::to<Float>(engine_geo_mesh_data::MeshData_triangle_count(chunk.mesh).Result)));
  }

  struct CityStream_building_shapeResult { physics_Shape Result = physics_Shape{Int64{0x0}}; };
  CityStream_building_shapeResult CityStream_building_shape(CityStream& self, const MapFeatures_Building& b, physics_Body body = physics_Body{Int64{0x0}}) {
    Int earcut = 0;
    Int Earcut_2 = 0;
    List<Float> xy = List<Float>{};
    List<Int> holes = List<Int>{};
    List<Int> tris = List<Int>{};
    List<Int> idx = List<Int>{};
    List<Vector> pts = List<Vector>{};
    List<Vector_row> rings = List<Vector_row>{};
    Int step = 0;
    List<Vector> thin = List<Vector>{};
    Int k = 0;
    Int r = 0;
    Bool skip = false;
    Float a = 0.0;
    Int first = 0;
    Int last = 0;
    Vector held = Vector{0.0, 0.0, 0.0};
    Int at = 0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Int place = 0;
    Float item = 0.0;
    Float item_2 = 0.0;
    Vector p0 = Vector{0.0, 0.0, 0.0};
    Vector p1 = Vector{0.0, 0.0, 0.0};
    Int base = 0;
    Int place_2 = 0;
    Int item_3 = 0;
    Int item_4 = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    Int item_7 = 0;
    Int item_8 = 0;
    Int base_2 = 0;
    Int i = 0;
    Int a_2 = 0;
    Int c = 0;
    Int d = 0;
    Int swapped = 0;
    Int place_3 = 0;
    Int item_9 = 0;
    Int item_10 = 0;
    Int item_11 = 0;
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    physics_Shape returned = physics_Shape{Int64{0x0}};
    physics_Shape returned_2 = physics_Shape{Int64{0x0}};
    engine_earcut_make(n32);
    Earcut_2 = n32.result;
    earcut = Earcut_2;
    xy = List<Float>{};
    holes = List<Int>{};
    tris = List<Int>{};
    idx = List<Int>{};
    pts = List<Vector>{};
    list_clear(xy);
    list_clear(holes);
    list_clear(tris);
    list_clear(idx);
    list_clear(pts);
    rings = b.rings;
    for (auto it33 = flow_for(n33, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(rings))))); it33.next();) {
      if (math_compare<">">(kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(rings[n33.index]).items)), 64.0)) {
        step = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div((a + 63.0), 64.0)); }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(rings[n33.index]).items))));
        thin = List<Vector>{};
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(rings[n33.index]).items))), 0.5))) {
          list_add(thin, rings[n33.index].items[k]);
          k += step;
        }
        rings[n33.index].items = thin;
      }
    }
    r = 0;
    while (loop(math_compare<">">(kern::formula([](Float r, Float a) -> Float { return ((r < 0.0) != (a < 0.0) ? (a < 0.0) : (r < a)); }, kern::to<Float>(r), kern::to<Float>(list_length<Vector_row>(rings))), 0.5))) {
      skip = false;
      if (math_compare<"<">(kern::to<Float>(list_length<Vector>(rings[r].items)), 3.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        a = signed_area(rings[r].items).Result;
        if (math_compare<"<">(kern::formula([](Float a) -> Float { return kern::math::abs(a); }, a), 0.5)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(math_compare<"==">(kern::to<Float>(r), 0.0)))), kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(a, 0.0)))))) {
            first = 0;
            last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(rings[r].items))));
            while (loop(math_compare<"<">(kern::to<Float>(first), kern::to<Float>(last)))) {
              held = list_get<Vector>(rings[r].items, first);
              rings[r].items[first] = list_get<Vector>(rings[r].items, last);
              rings[r].items[last] = held;
              first += 1;
              last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(last)));
            }
          }
          if (math_compare<"!=">(kern::to<Float>(r), 0.0)) {
            list_add(holes, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::trunc(kern::math::div(a, 2.0))); }, kern::to<Float>(list_length<Float>(xy)))));
          }
          at = r;
          for (auto it34 = flow_for(n34, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(rings[at].items))))); it34.next();) {
            p = rings[at].items[n34.index];
            place = list_length<Float>(xy);
            item = vector_break(p).x;
            item_2 = vector_break(p).y;
            list_insert_list(xy, place, list_make<Float, "2">(item, item_2));
          }
          for (auto it35 = flow_for(n35, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(rings[r].items))))); it35.next();) {
            p0 = rings[r].items[n35.index];
            p1 = rings[r].items[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n35.index), kern::to<Float>(list_length<Vector>(rings[r].items))))];
            base = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(pts))));
            list_add(pts, vector_make(vector_break(p0).x, vector_break(p0).y, b.base));
            list_add(pts, vector_make(vector_break(p1).x, vector_break(p1).y, b.base));
            list_add(pts, vector_make(vector_break(p1).x, vector_break(p1).y, b.top));
            list_add(pts, vector_make(vector_break(p0).x, vector_break(p0).y, b.top));
            place_2 = list_length<Int>(idx);
            item_3 = base;
            item_4 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 1.0); }, kern::to<Float>(base)));
            item_5 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 2.0); }, kern::to<Float>(base)));
            item_6 = base;
            item_7 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 2.0); }, kern::to<Float>(base)));
            item_8 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 3.0); }, kern::to<Float>(base)));
            list_insert_list(idx, place_2, list_make<Int, "6">(item_3, item_4, item_5, item_6, item_7, item_8));
          }
        }
      }
      r += 1;
    }
    if (math_compare<">=">(kern::to<Float>(list_length<Float>(xy)), 6.0)) {
      engine_earcut_triangulate(n36, earcut, xy, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::trunc(kern::math::div(a, 2.0))); }, kern::to<Float>(list_length<Float>(xy)))), holes, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Int>(holes)))), tris);
      tris = n36.out;
      base_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(pts))));
      for (auto it37 = flow_for(n37, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div(a, 2.0)); }, kern::to<Float>(list_length<Float>(xy))))))))); it37.next();) {
        list_add(pts, vector_make(xy[kern::to<Int>(kern::formula([](Float index) -> Float { return (index * 2.0); }, kern::to<Float>(n37.index)))], xy[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index * 2.0) + 1.0); }, kern::to<Float>(n37.index)))], b.top));
      }
      i = 0;
      while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Int>(tris))), 0.5))) {
        a_2 = kern::to<Int>(kern::formula([](Float base_2, Float a) -> Float { return kern::math::wrap32(base_2 + a); }, kern::to<Float>(base_2), kern::to<Float>(tris[i])));
        c = kern::to<Int>(kern::formula([](Float base_2, Float a) -> Float { return kern::math::wrap32(base_2 + a); }, kern::to<Float>(base_2), kern::to<Float>(tris[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))])));
        d = kern::to<Int>(kern::formula([](Float base_2, Float a) -> Float { return kern::math::wrap32(base_2 + a); }, kern::to<Float>(base_2), kern::to<Float>(tris[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i)))])));
        if (math_compare<"<">(vector_break(vector_cross(vector_sub(pts[c], pts[a_2]), vector_sub(pts[d], pts[a_2]))).z, 0.0)) {
          swapped = c;
          c = d;
          d = swapped;
        }
        place_3 = list_length<Int>(idx);
        item_9 = a_2;
        item_10 = c;
        item_11 = d;
        list_insert_list(idx, place_3, list_make<Int, "3">(item_9, item_10, item_11));
        i += 3;
      }
    }
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<Int>(idx)), 0.0))) {
      ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
      ss.kind = physics_ShapeKind::mesh;
      ss.points = pts;
      ss.indices = idx;
      ss.material.friction = 0.7;
      ss.material.category_bits = Int64{0x1};
      engine_world_create_shape(n38, self.world_, body, ss);
      create_shape = n38.result;
      returned = create_shape;
      if (math_compare<"!=">(kern::to<Float>(earcut), 0.0)) {
        engine_earcut_release(earcut);
      }
      earcut = 0;
      return {returned};
    }
    returned_2 = struct_make<physics_Shape>(Int64{0x0});
    if (math_compare<"!=">(kern::to<Float>(earcut), 0.0)) {
      engine_earcut_release(earcut);
    }
    earcut = 0;
    return {returned_2};
  }

  struct CityStream_damage_buildingResult { Bool Result = false; };
  CityStream_damage_buildingResult CityStream_damage_building(CityStream& self, Int64 tile = Int64{0x0}, Int index = 0, Float new_top = 0.0, Bool leveled = false) {
    Int it = 0;
    Int key = 0;
    CityStream_Damage t = CityStream_Damage{0.0, false};
    Int key_2 = 0;
    CityStream_Damage put = CityStream_Damage{0.0, false};
    CityStream_Damage t_2 = CityStream_Damage{0.0, false};
    Int key_3 = 0;
    CityStream_Damage put_2 = CityStream_Damage{0.0, false};
    Int64 key_4 = Int64{0x0};
    Int k = 0;
    Bool stop = false;
    Bool skip = false;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<CityStream_Tile>>(self.tiles_, tile).index), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))));
    if (logic_or(logic_or(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))), logic_not(self.tiles_[it][0].prepared)), math_compare<">">(kern::formula([](Float index, Float a) -> Float { return (!((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a))); }, kern::to<Float>(index), kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[it][0].features.buildings))), 0.5))) {
      return {false};
    }
    if (logic_not(math_compare<"<">(new_top, kern::formula([](Float a) -> Float { return (a - 0.01); }, self.tiles_[it][0].features.buildings[index].top)))) {
      return {false};
    }
    self.tiles_[it][0].features.buildings[index].top = kern::formula([](Float a, Float new_top) -> Float { return kern::math::max((a + 0.3), new_top); }, self.tiles_[it][0].features.buildings[index].base, new_top);
    self.tiles_[it][0].features.buildings[index].leveled = logic_or(self.tiles_[it][0].features.buildings[index].leveled, leveled);
    self.damage_serial_ = kern::to<Int>(kern::formula([](Float damage_serial_) -> Float { return kern::math::wrap32(damage_serial_ + 1.0); }, kern::to<Float>(self.damage_serial_)));
    key = self.tiles_[it][0].features.buildings[index].seed;
    map_add(n40, self.damage_, key);
    key_2 = self.tiles_[it][0].features.buildings[index].seed;
    map_add(n41, self.damage_, key_2);
    put = t;
    t = self.damage_[n41.index];
    t.height = kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, self.tiles_[it][0].features.buildings[index].top, self.tiles_[it][0].features.buildings[index].base);
    self.damage_[n41.index] = put;
    key_3 = self.tiles_[it][0].features.buildings[index].seed;
    map_add(n42, self.damage_, key_3);
    put_2 = t_2;
    t_2 = self.damage_[n42.index];
    t_2.leveled = self.tiles_[it][0].features.buildings[index].leveled;
    self.damage_[n42.index] = put_2;
    if (logic_not(self.damage_quiet_)) {
      list_add(self.damage_log_, self.tiles_[it][0].features.buildings[index].seed);
    }
    if (logic_and(math_compare<">">(kern::formula([](Float index, Float a) -> Float { return ((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a)); }, kern::to<Float>(index), kern::to<Float>(list_length<Vector>(self.tiles_[it][0].cent))), 0.5), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(map_contains<Int64, List<Int>>(self.tiles_[it][0].cell_ids, cell_of(self.tiles_[it][0].cent[index]).Result)))), 0.0))) {
      key_4 = cell_of(self.tiles_[it][0].cent[index]).Result;
      set_add(n43, self.tiles_[it][0].dirty_cells, key_4);
    }
    if (self.near_.building) {
      self.near_.dirty = true;
      return {true};
    }
    k = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<Int64_Int_pair>(self.near_.list))), 0.5)))) {
      skip = false;
      if (logic_or(logic_not(int64_equal(self.near_.list[k].first, tile)), math_compare<"!=">(kern::to<Float>(self.near_.list[k].second), kern::to<Float>(index)))) {
        skip = true;
      }
      if (logic_not(logic_or(stop, skip))) {
        if (math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<physics_Shape>(self.near_.shapes))), 0.5)) {
          if (engine_physics_physics::Shape_valid(self.near_.shapes[k]).Result) {
            engine_world_destroy_shape(self.world_, self.near_.shapes[k]);
            self.near_.colliders = kern::to<Int>(kern::formula([](Float colliders) -> Float { return kern::math::wrap32(colliders - 1.0); }, kern::to<Float>(self.near_.colliders)));
          }
          n44 = CityStream_building_shape(self, self.tiles_[it][0].features.buildings[index], self.near_.body);
          self.near_.shapes[k] = n44.Result;
          if (engine_physics_physics::Shape_valid(self.near_.shapes[k]).Result) {
            self.near_.colliders = kern::to<Int>(kern::formula([](Float colliders) -> Float { return kern::math::wrap32(colliders + 1.0); }, kern::to<Float>(self.near_.colliders)));
          }
        }
        stop = true;
      }
      if (logic_not(stop)) {
        k += 1;
      }
    }
    return {true};
  }

  struct damage_list_matchResult { Bool Result = false; };
  KERN_PURE damage_list_matchResult damage_list_match(const CityStream_DamageRec& a, const CityStream_DamageRec& b) {
    return {math_compare<"<">(kern::to<Float>(a.seed), kern::to<Float>(b.seed))};
  }

  void the_CityStream_damage_list_(const CityStream& self, List<CityStream_DamageRec>& out) {
    list_clear(out);
    list_reserve(out, map_length<Int, CityStream_Damage>(self.damage_));
    for (auto it45 = flow_for(n45, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))))); it45.next();) {
      list_add(out, struct_make<CityStream_DamageRec>(map_key_at<Int, CityStream_Damage>(self.damage_, n45.index), self.damage_[n45.index].height, self.damage_[n45.index].leveled));
    }
    list_sort<&city_stream_cpp::damage_list_match>(*this, out);
  }

  struct CityStream_damage_ofResult { Bool Result = false; };
  CityStream_damage_ofResult CityStream_damage_of(const CityStream& self, CityStream_DamageRec& out, Int seed = 0) {
    Int it = 0;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, CityStream_Damage>(self.damage_, seed).index), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))));
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_)))) {
      return {false};
    }
    out = struct_make<CityStream_DamageRec>(seed, self.damage_[it].height, self.damage_[it].leveled);
    return {true};
  }

  struct CityStream_merge_damageResult { Bool Result = false; };
  CityStream_merge_damageResult CityStream_merge_damage(CityStream& self, const List<CityStream_DamageRec>& list, Bool the_log = false) {
    Map<Int, Int> want = Map<Int, Int>{};
    Bool changed = false;
    Bool skip = false;
    CityStream_DamageRec r = CityStream_DamageRec{0, 0.0, false};
    Int it = 0;
    Int put = 0;
    Int key = 0;
    Bool skip_2 = false;
    Bool skip_3 = false;
    Int w = 0;
    MapFeatures_Building b = MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false};
    CityStream_DamageRec r_2 = CityStream_DamageRec{0, 0.0, false};
    Bool skip_4 = false;
    CityStream_DamageRec r_3 = CityStream_DamageRec{0, 0.0, false};
    Int it_2 = 0;
    CityStream_Damage put_2 = CityStream_Damage{0.0, false};
    Int key_2 = 0;
    if (math_compare<"==">(kern::to<Float>(list_length<CityStream_DamageRec>(list)), 0.0)) {
      return {false};
    }
    want = Map<Int, Int>{};
    changed = false;
    for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<CityStream_DamageRec>(list))))); it46.next();) {
      skip = false;
      r = list[n46.index];
      if (logic_or(logic_not(math_compare<">=">(r.height, 0.0)), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, r.height), 0.0)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, CityStream_Damage>(self.damage_, r.seed).index), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))));
        if (logic_and(logic_and(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_)))), math_compare<"<=">(self.damage_[it].height, kern::formula([](Float height) -> Float { return (height + 0.01); }, r.height))), logic_or(self.damage_[it].leveled, logic_not(r.leveled)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          put = n46.index;
          key = r.seed;
          map_add(n47, want, key);
          want[n47.index] = put;
        }
      }
    }
    if (math_compare<"==">(kern::to<Float>(map_length<Int, Int>(want)), 0.0)) {
      return {false};
    }
    self.damage_quiet_ = logic_not(the_log);
    for (auto it48 = flow_for(n48, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it48.next();) {
      skip_2 = false;
      if (logic_not(self.tiles_[n48.index][0].prepared)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        for (auto it49 = flow_for(n49, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[n48.index][0].features.buildings))))); it49.next();) {
          skip_3 = false;
          w = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, Int>(want, self.tiles_[n48.index][0].features.buildings[n49.index].seed).index), kern::to<Float>(map_length<Int, Int>(want))));
          if (math_compare<"==">(kern::to<Float>(w), kern::to<Float>(map_length<Int, Int>(want)))) {
            skip_3 = true;
          }
          if (logic_not(skip_3)) {
            b = self.tiles_[n48.index][0].features.buildings[n49.index];
            r_2 = list[want[w]];
            n50 = CityStream_damage_building(self, int64_of_int(int64_to_int(map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n48.index))), n49.index, kern::formula([](Float base, Float height) -> Float { return (base + height); }, b.base, r_2.height), r_2.leveled);
            if (n50.Result) {
              changed = true;
            }
          }
        }
      }
    }
    for (auto it51 = flow_for(n51, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Int>(want))))); it51.next();) {
      skip_4 = false;
      r_3 = list[want[n51.index]];
      it_2 = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, CityStream_Damage>(self.damage_, map_key_at<Int, Int>(want, n51.index)).index), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))));
      if (math_compare<"==">(kern::to<Float>(it_2), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_)))) {
        put_2 = struct_make<CityStream_Damage>(r_3.height, r_3.leveled);
        key_2 = map_key_at<Int, Int>(want, n51.index);
        map_add(n52, self.damage_, key_2);
        self.damage_[n52.index] = put_2;
      } else if (logic_or(math_compare<"<">(r_3.height, kern::formula([](Float a) -> Float { return (a - 0.01); }, self.damage_[it_2].height)), logic_and(r_3.leveled, logic_not(self.damage_[it_2].leveled)))) {
        self.damage_[it_2].height = kern::formula([](Float a, Float height) -> Float { return kern::math::min(a, height); }, self.damage_[it_2].height, r_3.height);
        self.damage_[it_2].leveled = logic_or(self.damage_[it_2].leveled, r_3.leveled);
      } else {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        changed = true;
        if (the_log) {
          list_add(self.damage_log_, map_key_at<Int, Int>(want, n51.index));
        }
      }
    }
    self.damage_quiet_ = false;
    if (changed) {
      self.damage_serial_ = kern::to<Int>(kern::formula([](Float damage_serial_) -> Float { return kern::math::wrap32(damage_serial_ + 1.0); }, kern::to<Float>(self.damage_serial_)));
    }
    return {changed};
  }

  void CityStream_clear_damage(CityStream& self) {
    Bool skip = false;
    Bool skip_2 = false;
    Int64 key = Int64{0x0};
    map_clear(self.damage_);
    list_clear(self.damage_log_);
    for (auto it53 = flow_for(n53, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it53.next();) {
      skip = false;
      if (logic_not(self.tiles_[n53.index][0].prepared)) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it54 = flow_for(n54, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[n53.index][0].features.buildings))))); it54.next();) {
          skip_2 = false;
          if (logic_and(math_compare<">=">(self.tiles_[n53.index][0].features.buildings[n54.index].top, kern::formula([](Float a) -> Float { return (a - 0.01); }, self.tiles_[n53.index][0].features.buildings[n54.index].full_top)), logic_not(self.tiles_[n53.index][0].features.buildings[n54.index].leveled))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            self.tiles_[n53.index][0].features.buildings[n54.index].top = self.tiles_[n53.index][0].features.buildings[n54.index].full_top;
            self.tiles_[n53.index][0].features.buildings[n54.index].leveled = false;
            if (logic_and(math_compare<">">(kern::formula([](Float index, Float a) -> Float { return ((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a)); }, kern::to<Float>(n54.index), kern::to<Float>(list_length<Vector>(self.tiles_[n53.index][0].cent))), 0.5), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(map_contains<Int64, List<Int>>(self.tiles_[n53.index][0].cell_ids, cell_of(self.tiles_[n53.index][0].cent[n54.index]).Result)))), 0.0))) {
              key = cell_of(self.tiles_[n53.index][0].cent[n54.index]).Result;
              set_add(n55, self.tiles_[n53.index][0].dirty_cells, key);
            }
          }
        }
      }
    }
    self.near_.dirty = true;
  }

  struct begin_cells_matchResult { Bool Result = false; };
  KERN_PURE begin_cells_matchResult begin_cells_match(const Float_Int64_pair& a, const Float_Int64_pair& b) {
    return {logic_or(math_compare<"<">(a.first, b.first), logic_and(logic_not(math_compare<"<">(b.first, a.first)), int64_less_unsigned(a.second, b.second)))};
  }

  void CityStream_walls_near(const CityStream& self, List<CityStream_Wall>& out, Vector p = Vector{0.0, 0.0, 0.0}, Float r = 0.0) {
    Bool skip = false;
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, 0, 0, 0, 0, List<Int>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<Int>{}, List<Int>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<Int>>{}, Set<Int64>{}, Map<Int64, List<Int>>{}, false, 0, 0, Map<Int64, List<Int>>{}, Set<Int64>{}, List<Int64>{}, 0, List<Int>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
    Bool skip_2 = false;
    Vector q = Vector{0.0, 0.0, 0.0};
    Int edge = 0;
    Bool skip_3 = false;
    List<Vector> ring = List<Vector>{};
    Bool ccw = false;
    Float s = 0.0;
    Int i = 0;
    Bool skip_4 = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Vector d = Vector{0.0, 0.0, 0.0};
    Float u = 0.0;
    Vector n = Vector{0.0, 0.0, 0.0};
    Float side = 0.0;
    list_clear(out);
    for (auto it68 = flow_for(n68, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it68.next();) {
      skip = false;
      t = list_get<CityStream_Tile>(self.tiles_[n68.index], 0);
      if (logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x, Float r) -> Float { return ((x - r) - 200.0); }, t.bounds.x, r)), math_compare<">">(vector_break(p).x, kern::formula([](Float z, Float r) -> Float { return ((z + r) + 200.0); }, t.bounds.z, r))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y, Float r) -> Float { return ((y - r) - 200.0); }, t.bounds.y, r))), math_compare<">">(vector_break(p).y, kern::formula([](Float w, Float r) -> Float { return ((w + r) + 200.0); }, t.bounds.w, r)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it69 = flow_for(n69, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(t.features.buildings))))); it69.next();) {
            skip_2 = false;
            if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(t.features.buildings[n69.index].rings)), 0.0), math_compare<"==">(kern::to<Float>(list_length<Vector>(t.features.buildings[n69.index].rings[0].items)), 0.0))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              q = t.features.buildings[n69.index].rings[0].items[0];
              if (logic_or(math_compare<">">(kern::formula([](Float x, Float x_2) -> Float { return kern::math::abs((x - x_2)); }, vector_break(q).x, vector_break(p).x), kern::formula([](Float r) -> Float { return (r + 150.0); }, r)), math_compare<">">(kern::formula([](Float y, Float y_2) -> Float { return kern::math::abs((y - y_2)); }, vector_break(q).y, vector_break(p).y), kern::formula([](Float r) -> Float { return (r + 150.0); }, r)))) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                edge = 0;
                for (auto it70 = flow_for(n70, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector_row>(t.features.buildings[n69.index].rings))))); it70.next();) {
                  skip_3 = false;
                  ring = t.features.buildings[n69.index].rings[n70.index].items;
                  if (math_compare<"<">(kern::to<Float>(list_length<Vector>(ring)), 3.0)) {
                    skip_3 = true;
                  }
                  if (logic_not(skip_3)) {
                    ccw = math_compare<">">(signed_area(ring).Result, 0.0);
                    s = kern::formula([](Float ccw, Float index) -> Float { return ((ccw == (index == 0.0)) ? 1.0 : (-1.0)); }, kern::to<Float>(ccw), kern::to<Float>(n70.index));
                    i = 0;
                    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(ring))), 0.5))) {
                      skip_4 = false;
                      a = ring[i];
                      c = ring[kern::to<Int>(kern::formula([](Float i, Float a) -> Float { return kern::math::mod((i + 1.0), a); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(ring))))];
                      len = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(a).x, vector_break(c).y, vector_break(a).y);
                      if (math_compare<"<">(len, 0.05)) {
                        skip_4 = true;
                      }
                      if (logic_not(skip_4)) {
                        d = vector_make(kern::formula([](Float x, Float x_2, Float len) -> Float { return kern::math::div((x - x_2), len); }, vector_break(c).x, vector_break(a).x, len), kern::formula([](Float y, Float y_2, Float len) -> Float { return kern::math::div((y - y_2), len); }, vector_break(c).y, vector_break(a).y, len), 0.0);
                        u = kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3) -> Float { return (((x - x_2) * x_3) + ((y - y_2) * y_3)); }, vector_break(p).x, vector_break(a).x, vector_break(d).x, vector_break(p).y, vector_break(a).y, vector_break(d).y);
                        n = vector_make(kern::formula([](Float y, Float s) -> Float { return (y * s); }, vector_break(d).y, s), kern::formula([](Float x, Float s) -> Float { return ((-x) * s); }, vector_break(d).x, s), 0.0);
                        side = kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3) -> Float { return (((x - x_2) * x_3) + ((y - y_2) * y_3)); }, vector_break(p).x, vector_break(a).x, vector_break(n).x, vector_break(p).y, vector_break(a).y, vector_break(n).y);
                        if (logic_or(logic_or(math_compare<"<">(u, kern::formula([](Float r) -> Float { return (-r); }, r)), math_compare<">">(u, kern::formula([](Float len, Float r) -> Float { return (len + r); }, len, r))), math_compare<">">(kern::formula([](Float side) -> Float { return kern::math::abs(side); }, side), r))) {
                          skip_4 = true;
                        }
                        if (logic_not(skip_4)) {
                          list_add(out, struct_make<CityStream_Wall>(a, d, n, len, t.features.buildings[n69.index].base, t.features.buildings[n69.index].top, t.features.buildings[n69.index].seed, edge));
                        }
                      }
                      i += 1;
                      edge += 1;
                    }
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  struct CityStream_building_atResult { Bool Result = false; };
  KERN_PURE CityStream_building_atResult CityStream_building_at(const CityStream& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    Bool skip = false;
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, 0, 0, 0, 0, List<Int>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<Int>{}, List<Int>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<Int>>{}, Set<Int64>{}, Map<Int64, List<Int>>{}, false, 0, 0, Map<Int64, List<Int>>{}, Set<Int64>{}, List<Int64>{}, 0, List<Int>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
    Bool skip_2 = false;
    List<Vector> ring = List<Vector>{};
    Bool inside = false;
    Int i = 0;
    Int j = 0;
    Int was = 0;
    for (auto it71 = flow_for(n71, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it71.next();) {
      skip = false;
      t = list_get<CityStream_Tile>(self.tiles_[n71.index], 0);
      if (logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x) -> Float { return (x - 200.0); }, t.bounds.x)), math_compare<">">(vector_break(p).x, kern::formula([](Float z) -> Float { return (z + 200.0); }, t.bounds.z))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y) -> Float { return (y - 200.0); }, t.bounds.y))), math_compare<">">(vector_break(p).y, kern::formula([](Float w) -> Float { return (w + 200.0); }, t.bounds.w)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it72 = flow_for(n72, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(t.features.buildings))))); it72.next();) {
            skip_2 = false;
            if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(t.features.buildings[n72.index].rings)), 0.0), math_compare<"<">(kern::to<Float>(list_length<Vector>(t.features.buildings[n72.index].rings[0].items)), 3.0))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              ring = t.features.buildings[n72.index].rings[0].items;
              if (logic_or(math_compare<">">(kern::formula([](Float x, Float x_2) -> Float { return kern::math::abs((x - x_2)); }, vector_break(ring[0]).x, vector_break(p).x), 150.0), math_compare<">">(kern::formula([](Float y, Float y_2) -> Float { return kern::math::abs((y - y_2)); }, vector_break(ring[0]).y, vector_break(p).y), 150.0))) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                inside = false;
                i = 0;
                j = kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Vector>(ring))));
                while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(ring))), 0.5))) {
                  if (logic_and(math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(vector_break(ring[i]).y, vector_break(p).y)))), kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(vector_break(ring[j]).y, vector_break(p).y))))), math_compare<"<">(vector_break(p).x, kern::formula([](Float x, Float x_2, Float y, Float y_2, Float y_3) -> Float { return (kern::math::div(((x - x_2) * (y - y_2)), (y_3 - y_2)) + x_2); }, vector_break(ring[j]).x, vector_break(ring[i]).x, vector_break(p).y, vector_break(ring[i]).y, vector_break(ring[j]).y)))) {
                    inside = logic_not(inside);
                  }
                  was = i;
                  i += 1;
                  j = was;
                }
                if (logic_not(inside)) {
                  skip_2 = true;
                }
                if (logic_not(skip_2)) {
                  return {true};
                }
              }
            }
          }
        }
      }
    }
    return {false};
  }

  void CityStream_buildings_near(const CityStream& self, List<CityStream_BuildingRef>& out, Vector p = Vector{0.0, 0.0, 0.0}, Float radius = 0.0) {
    Bool skip = false;
    Bool skip_2 = false;
    MapFeatures_Building b = MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false};
    Float x0 = 0.0;
    Float y0 = 0.0;
    Float x1 = 0.0;
    Float y1 = 0.0;
    Vector q = Vector{0.0, 0.0, 0.0};
    for (auto it77 = flow_for(n77, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it77.next();) {
      skip = false;
      if (logic_and(logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n77.index][0].state, CityStream_Tile_State::ready)), logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n77.index][0].state, CityStream_Tile_State::collision)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it78 = flow_for(n78, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[n77.index][0].features.buildings))))); it78.next();) {
          skip_2 = false;
          b = self.tiles_[n77.index][0].features.buildings[n78.index];
          if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(b.rings)), 0.0), math_compare<"<">(kern::to<Float>(list_length<Vector>(b.rings[0].items)), 3.0))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            x0 = 1000000000.0;
            y0 = 1000000000.0;
            x1 = -1000000000.0;
            y1 = -1000000000.0;
            for (auto it79 = flow_for(n79, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(b.rings[0].items))))); it79.next();) {
              q = b.rings[0].items[n79.index];
              x0 = kern::formula([](Float x0, Float x) -> Float { return kern::math::min(x0, x); }, x0, vector_break(q).x);
              y0 = kern::formula([](Float y0, Float y) -> Float { return kern::math::min(y0, y); }, y0, vector_break(q).y);
              x1 = kern::formula([](Float x1, Float x) -> Float { return kern::math::max(x1, x); }, x1, vector_break(q).x);
              y1 = kern::formula([](Float y1, Float y) -> Float { return kern::math::max(y1, y); }, y1, vector_break(q).y);
            }
            if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x0, Float radius) -> Float { return (x0 - radius); }, x0, radius)), math_compare<">">(vector_break(p).x, kern::formula([](Float x1, Float radius) -> Float { return (x1 + radius); }, x1, radius))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y0, Float radius) -> Float { return (y0 - radius); }, y0, radius))), math_compare<">">(vector_break(p).y, kern::formula([](Float y1, Float radius) -> Float { return (y1 + radius); }, y1, radius)))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              list_add(out, struct_make<CityStream_BuildingRef>(int64_of_int(int64_to_int(map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n77.index))), kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n78.index)))));
            }
          }
        }
      }
    }
  }

  void CityStream_ready_tiles(const CityStream& self, List<Int64>& out) {
    for (auto it80 = flow_for(n80, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it80.next();) {
      if (self.tiles_[n80.index][0].prepared) {
        list_add(out, map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n80.index));
      }
    }
  }

  void CityStream_areas_near(const CityStream& self, List<CityStream_AreaRef>& out, Vector p = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, AreaKind kind = AreaKind::water) {
    Bool skip = false;
    Bool skip_2 = false;
    MapFeatures_Area a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    Float x0 = 0.0;
    Float y0 = 0.0;
    Float x1 = 0.0;
    Float y1 = 0.0;
    Vector q = Vector{0.0, 0.0, 0.0};
    for (auto it81 = flow_for(n81, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it81.next();) {
      skip = false;
      if (logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n81.index][0].state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it82 = flow_for(n82, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Area>(self.tiles_[n81.index][0].features.areas))))); it82.next();) {
          skip_2 = false;
          a = self.tiles_[n81.index][0].features.areas[n82.index];
          if (logic_or(logic_or(logic_not(enum_equal<AreaKind>(a.kind, kind)), math_compare<"==">(kern::to<Float>(list_length<Vector_row>(a.rings)), 0.0)), math_compare<"<">(kern::to<Float>(list_length<Vector>(a.rings[0].items)), 3.0))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            x0 = 1000000000.0;
            y0 = 1000000000.0;
            x1 = -1000000000.0;
            y1 = -1000000000.0;
            for (auto it83 = flow_for(n83, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(a.rings[0].items))))); it83.next();) {
              q = a.rings[0].items[n83.index];
              x0 = kern::formula([](Float x0, Float x) -> Float { return kern::math::min(x0, x); }, x0, vector_break(q).x);
              y0 = kern::formula([](Float y0, Float y) -> Float { return kern::math::min(y0, y); }, y0, vector_break(q).y);
              x1 = kern::formula([](Float x1, Float x) -> Float { return kern::math::max(x1, x); }, x1, vector_break(q).x);
              y1 = kern::formula([](Float y1, Float y) -> Float { return kern::math::max(y1, y); }, y1, vector_break(q).y);
            }
            if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x0, Float radius) -> Float { return (x0 - radius); }, x0, radius)), math_compare<">">(vector_break(p).x, kern::formula([](Float x1, Float radius) -> Float { return (x1 + radius); }, x1, radius))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y0, Float radius) -> Float { return (y0 - radius); }, y0, radius))), math_compare<">">(vector_break(p).y, kern::formula([](Float y1, Float radius) -> Float { return (y1 + radius); }, y1, radius)))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              list_add(out, struct_make<CityStream_AreaRef>(int64_of_int(kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return ((a * 65536.0) + index); }, int64_to_float_unsigned(map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n81.index)), kern::to<Float>(n82.index)))), int64_of_int(int64_to_int(map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n81.index))), kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n82.index)))));
            }
          }
        }
      }
    }
  }

  void CityStream_recollide(CityStream& self, CityStream_Tile& arg2) {
    self.near_.dirty = true;
    self.near_.at_ms = -1000000000.0;
  }

  void CityStream_exclude_building(CityStream& self, Int64 tile = Int64{0x0}, Int index = 0) {
    Int it = 0;
    Int it_2 = 0;
    if (logic_and(int64_equal(tile, self.excl_tile_), math_compare<"==">(kern::to<Float>(index), kern::to<Float>(self.excl_index_)))) {
      return;
    }
    if (logic_not(int64_equal(self.excl_tile_, Int64{0x0}))) {
      it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<CityStream_Tile>>(self.tiles_, self.excl_tile_).index), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))));
      if (logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))) {
        self.tiles_[it][0].skip = -1;
        CityStream_recollide(self, self.tiles_[it][0]);
      }
    }
    self.excl_tile_ = tile;
    self.excl_index_ = kern::to<Int>(kern::formula([](Float a, Float index) -> Float { return ((a != 0.0) ? index : (-1.0)); }, kern::to<Float>(logic_not(int64_equal(tile, Int64{0x0}))), kern::to<Float>(index)));
    if (logic_not(int64_equal(tile, Int64{0x0}))) {
      it_2 = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<CityStream_Tile>>(self.tiles_, tile).index), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))));
      if (logic_not(math_compare<"==">(kern::to<Float>(it_2), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))) {
        self.tiles_[it_2][0].skip = index;
        CityStream_recollide(self, self.tiles_[it_2][0]);
      }
    }
  }

  struct CityStream_exclusion_doneResult { Bool Result = false; };
  KERN_PURE CityStream_exclusion_doneResult CityStream_exclusion_done(const CityStream& self) {
    return {logic_and(logic_not(self.near_.building), logic_not(self.near_.dirty))};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { Bool result = false; } n13;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct { Bool result = false; } n22;
  struct { Bool removed = false; } n23;
  struct { WeldStats result = WeldStats{0, 0, 0, 0, false}; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n24;
  struct { Int result = 0; } n25;
  struct { Bool result = false; } n26;
  struct { Bool added = false; Int index = 0; } n27;
  struct { Int result = 0; } n32;
  struct : kern::Node<33> { Int index = 0; } n33;
  struct : kern::Node<34> { Int index = 0; } n34;
  struct : kern::Node<35> { Int index = 0; } n35;
  struct { List<Int> out = List<Int>{}; } n36;
  struct : kern::Node<37> { Int index = 0; } n37;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n38;
  struct { Bool added = false; Int index = 0; } n40;
  struct { Bool added = false; Int index = 0; } n41;
  struct { Bool added = false; Int index = 0; } n42;
  struct { Bool added = false; Int index = 0; } n43;
  CityStream_building_shapeResult n44;
  struct : kern::Node<45> { Int index = 0; } n45;
  struct : kern::Node<46> { Int index = 0; } n46;
  struct { Bool added = false; Int index = 0; } n47;
  struct : kern::Node<48> { Int index = 0; } n48;
  struct : kern::Node<49> { Int index = 0; } n49;
  CityStream_damage_buildingResult n50;
  struct : kern::Node<51> { Int index = 0; } n51;
  struct { Bool added = false; Int index = 0; } n52;
  struct : kern::Node<53> { Int index = 0; } n53;
  struct : kern::Node<54> { Int index = 0; } n54;
  struct { Bool added = false; Int index = 0; } n55;
  struct : kern::Node<68> { Int index = 0; } n68;
  struct : kern::Node<69> { Int index = 0; } n69;
  struct : kern::Node<70> { Int index = 0; } n70;
  struct : kern::Node<71> { Int index = 0; } n71;
  struct : kern::Node<72> { Int index = 0; } n72;
  struct : kern::Node<77> { Int index = 0; } n77;
  struct : kern::Node<78> { Int index = 0; } n78;
  struct : kern::Node<79> { Int index = 0; } n79;
  struct : kern::Node<80> { Int index = 0; } n80;
  struct : kern::Node<81> { Int index = 0; } n81;
  struct : kern::Node<82> { Int index = 0; } n82;
  struct : kern::Node<83> { Int index = 0; } n83;
};
