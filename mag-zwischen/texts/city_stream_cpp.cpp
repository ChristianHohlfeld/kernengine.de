struct city_stream_cpp : virtual kern::Library, virtual city_jobs_cpp, virtual city_stream, virtual common, virtual engine_geo_mesh_data, virtual engine_geo_roads, virtual engine_math_math, virtual engine_physics_physics, virtual terrain_cpp, virtual tiles, virtual tiles_cpp {
  // ---- types ----
  KERN_STRUCT(CityStream_select_cells_Pick) { Float d = 0.0; Int tile = 0; Int64 cell = Int64{0x0}; Int n = 0; };
  KERN_STRUCT(CityStream_update_collision_Pick) { Float d2 = 0.0; Int64 tile = Int64{0x0}; Int i = 0; };

  // ---- variables ----
  /// the building mesh chunks (BuildingOptions::chunk_size)
  KERN_CONST Float kChunk = 160.0;
  /// OpenFreeMap's planet tiles go up to z14 (overzoomed beyond)
  KERN_CONST Int kTileZoom = 14;
  KERN_CONST Int kTile = 1;
  /// The map features of a tile that is not loaded: none.
  KERN_CONST MapFeatures kNoFeatures = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
  /// vertex locations 14 and 15
  KERN_CONST Int kMaxExtensionAttributes = 2;

  // ---- functions ----
  struct CityStream_ground_heightResult { Float Result = 0.0; };
  /// Ground height: the terrain's static surface (elevation, water basins, craters).
  KERN_PURE CityStream_ground_heightResult CityStream_ground_height(const CityStream& self, Float x = 0.0, Float y = 0.0) {
    return {terrain_cpp::Terrain_height(self.terrain_, x, y).Result};
  }

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

  void CityStream_remove(CityStream& self, CityStream_Tile& t) {
    Int id = 0;
    Bool remove = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both_2 = false;
    if (math_compare<"!=">(kern::to<Float>(self.r_), 0.0)) {
      CityStream_remove_drop(self, t.meshes);
      CityStream_remove_drop(self, t.lay_meshes);
      CityStream_remove_drop(self, t.old_lay);
    }
    remove_cancel(t.tile_job);
    remove_cancel(t.build_job);
    remove_cancel(t.cell_job);
    map_clear(t.cell_ids);
    set_clear(t.dirty_cells);
    list_clear(t.job_cells);
    if (math_compare<"!=">(kern::to<Float>(self.r_), 0.0)) {
      for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<Int>>(t.cell_old))))); it14.next();) {
        for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(t.cell_old[n14.index]))))); it15.next();) {
          id = t.cell_old[n14.index][n15.index];
          engine_renderer_remove(n16, self.r_, id);
          remove = n16.result;
        }
      }
    }
    map_clear(t.cell_old);
    set_clear(t.want_cells);
    t.cell_uploading = false;
    list_clear(t.meshes);
    list_clear(t.lay_meshes);
    list_clear(t.old_lay);
    self.near_.dirty = true;
    if (t.prepared) {
      self.furniture_serial_ = kern::to<Int>(kern::formula([](Float furniture_serial_) -> Float { return kern::math::wrap32(furniture_serial_ + 1.0); }, kern::to<Float>(self.furniture_serial_)));
    }
    terrain_cpp::Terrain_remove_tints(self.terrain_, tiles::TileKey_hash(t.key).Result);
    terrain_cpp::Terrain_remove_corridors(self.terrain_, tiles::TileKey_hash(t.key).Result);
    both = logic_and(math_compare<"!=">(kern::to<Float>(self.world_), 0.0), engine_physics_physics::Body_valid(t.body).Result);
    if (both) {
      body_valid_2 = engine_world_body_valid(self.world_, t.body);
      both = body_valid_2;
    }
    if (both) {
      engine_world_destroy_body(self.world_, t.body);
    }
    t.body = struct_make<physics_Body>(Int64{0x0});
    both_2 = logic_and(math_compare<"!=">(kern::to<Float>(self.world_), 0.0), engine_physics_physics::Body_valid(t.old_body).Result);
    if (both_2) {
      body_valid_4 = engine_world_body_valid(self.world_, t.old_body);
      both_2 = body_valid_4;
    }
    if (both_2) {
      engine_world_destroy_body(self.world_, t.old_body);
    }
    t.old_body = struct_make<physics_Body>(Int64{0x0});
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Lane>(self.lanes_))))); it17.next();) {
      if (int64_equal(self.lanes_[n17.index].tile, tiles::TileKey_hash(t.key).Result)) {
        self.lanes_[n17.index].alive = false;
      }
    }
  }

  void CityStream_clear(CityStream& self) {
    Int count = 0;
    Int at = 0;
    physics_Body item = physics_Body{Int64{0x0}};
    physics_Body item_2 = physics_Body{Int64{0x0}};
    Fixed<physics_Body, 2> items = Fixed<physics_Body, 2>{};
    physics_Body b = physics_Body{Int64{0x0}};
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it11.next();) {
      CityStream_remove(self, self.tiles_[n11.index][0]);
    }
    count = map_length<Int64, List<CityStream_Tile>>(self.tiles_);
    for (auto it18 = flow_for(n18, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(count)))); it18.next();) {
      at = kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(count))))), kern::to<Float>(n18.index)));
      if (math_compare<">">(kern::to<Float>(list_length<CityStream_Tile>(self.tiles_[at])), 0.0)) {
        if (math_compare<"!=">(kern::to<Float>(self.tiles_[at][0].road_builder), 0.0)) {
          engine_road_builder_release(self.tiles_[at][0].road_builder);
        }
        if (math_compare<"!=">(kern::to<Float>(self.tiles_[at][0].buildings), 0.0)) {
          engine_building_builder_release(self.tiles_[at][0].buildings);
        }
        if (math_compare<"!=">(kern::to<Float>(self.tiles_[at][0].roads), 0.0)) {
          engine_road_stream_release(self.tiles_[at][0].roads);
        }
        if (math_compare<"!=">(kern::to<Float>(self.tiles_[at][0].footprints), 0.0)) {
          engine_footprint_stream_release(self.tiles_[at][0].footprints);
        }
      }
    }
    map_clear(self.tiles_);
    list_clear(self.requests_);
    list_clear(self.lanes_);
    self.stats_ = struct_make<StreamStats>(0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0);
    self.ms_ = struct_make<StreamStats>(0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0);
    terrain_cpp::Terrain_clear(self.terrain_);
    list_clear(self.terrain_changes_);
    if (math_compare<"!=">(kern::to<Float>(self.world_), 0.0)) {
      item = self.near_.body;
      item_2 = self.near_.old_body;
      items = list_make<physics_Body, "2">(item, item_2);
      for (auto it19 = flow_for(n19, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<physics_Body>(items))))); it19.next();) {
        b = items[n19.index];
        both = engine_physics_physics::Body_valid(b).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(self.world_, b);
          both = body_valid_2;
        }
        if (both) {
          engine_world_destroy_body(self.world_, b);
        }
      }
    }
    self.near_ = struct_make<CityStream_NearCollision>(struct_make<physics_Body>(Int64{0x0}), struct_make<physics_Body>(Int64{0x0}), Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, List<Int64_Int_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0);
    map_clear(self.damage_);
    list_clear(self.damage_log_);
    self.vis_at_ = Vector{1000000000.0, 1000000000.0, 0.0};
    self.vis_ms_ = -1000000000.0;
  }

  struct request_around_matchResult { Bool Result = false; };
  KERN_PURE request_around_matchResult request_around_match(const Int_TileKey_pair& a, const Int_TileKey_pair& b) {
    return {math_compare<"<">(kern::to<Float>(a.first), kern::to<Float>(b.first))};
  }

  struct CityStream_ring_boundsResult { Vec4 Result = Vec4{0.0, 0.0, 0.0, 0.0}; };
  CityStream_ring_boundsResult CityStream_ring_bounds(const CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, Int r = 0) {
    Float lon = 0.0;
    Float lat = 0.0;
    Float x0 = 0.0;
    Float y0 = 0.0;
    Float x1 = 0.0;
    Float y1 = 0.0;
    TileKey c = TileKey{14, 0, 0};
    TileKey a = TileKey{14, 0, 0};
    TileKey a_2 = TileKey{14, 0, 0};
    lon = 0.0;
    lat = 0.0;
    x0 = 0.0;
    y0 = 0.0;
    x1 = 0.0;
    y1 = 0.0;
    tiles_cpp::unproject(self.proj_, lon, lat, vector_break(focus).x, vector_break(focus).y);
    c = tiles_cpp::tile_at(lon, lat, kTileZoom).Result;
    a = struct_make<TileKey>(c.z, kern::to<Int>(kern::formula([](Float x, Float r) -> Float { return (x - r); }, kern::to<Float>(c.x), kern::to<Float>(r))), kern::to<Int>(kern::formula([](Float y, Float r) -> Float { return (y + r); }, kern::to<Float>(c.y), kern::to<Float>(r))));
    tiles_cpp::tile_lonlat(a, lon, lat, 0.0, 1.0);
    engine_geo_roads::GeoProjector_project(self.proj_, x0, y0, lon, lat);
    a_2 = struct_make<TileKey>(c.z, kern::to<Int>(kern::formula([](Float x, Float r) -> Float { return (x + r); }, kern::to<Float>(c.x), kern::to<Float>(r))), kern::to<Int>(kern::formula([](Float y, Float r) -> Float { return (y - r); }, kern::to<Float>(c.y), kern::to<Float>(r))));
    tiles_cpp::tile_lonlat(a_2, lon, lat, 1.0, 0.0);
    engine_geo_roads::GeoProjector_project(self.proj_, x1, y1, lon, lat);
    return {engine_math_math::Vec4_new(x0, y0, x1, y1).Result};
  }

  void CityStream_unload_far(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}) {
    Float lon = 0.0;
    Float lat = 0.0;
    TileKey c = TileKey{14, 0, 0};
    List<TileKey> mates = List<TileKey>{};
    Float lo = 0.0;
    Float la = 0.0;
    Int it = 0;
    Int d = 0;
    Bool near_mate = false;
    Int at = 0;
    lon = 0.0;
    lat = 0.0;
    tiles_cpp::unproject(self.proj_, lon, lat, vector_break(focus).x, vector_break(focus).y);
    c = tiles_cpp::tile_at(lon, lat, kTileZoom).Result;
    mates = List<TileKey>{};
    for (auto it27 = flow_for(n27, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(self.extra_foci))))); it27.next();) {
      lo = 0.0;
      la = 0.0;
      tiles_cpp::unproject(self.proj_, lo, la, vector_break(self.extra_foci[n27.index]).x, vector_break(self.extra_foci[n27.index]).y);
      list_add(mates, tiles_cpp::tile_at(lo, la, kTileZoom).Result);
    }
    it = 0;
    while (loop(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_)))))) {
      d = kern::to<Int>(kern::formula([](Float a, Float x, Float a_2, Float y) -> Float { return kern::math::max(kern::math::abs((a - x)), kern::math::abs((a_2 - y))); }, kern::to<Float>(self.tiles_[it][0].key.x), kern::to<Float>(c.x), kern::to<Float>(self.tiles_[it][0].key.y), kern::to<Float>(c.y)));
      near_mate = false;
      for (auto it28 = flow_for(n28, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TileKey>(mates))))); it28.next();) {
        near_mate = logic_or(near_mate, math_compare<"<=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::max(kern::math::abs((a - a_2)), kern::math::abs((a_3 - a_4))); }, kern::to<Float>(self.tiles_[it][0].key.x), kern::to<Float>(mates[n28.index].x), kern::to<Float>(self.tiles_[it][0].key.y), kern::to<Float>(mates[n28.index].y))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float mate_ring) -> Float { return (mate_ring + 1.0); }, kern::to<Float>(self.mate_ring)))))));
      }
      if (logic_and(logic_and(logic_not(self.tiles_[it][0].procedural), math_compare<">">(kern::to<Float>(d), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float ring) -> Float { return (ring + 1.0); }, kern::to<Float>(self.ring))))))), logic_not(near_mate))) {
        CityStream_remove(self, self.tiles_[it][0]);
        at = it;
        if (math_compare<">">(kern::to<Float>(list_length<CityStream_Tile>(self.tiles_[at])), 0.0)) {
          if (math_compare<"!=">(kern::to<Float>(self.tiles_[at][0].road_builder), 0.0)) {
            engine_road_builder_release(self.tiles_[at][0].road_builder);
          }
          if (math_compare<"!=">(kern::to<Float>(self.tiles_[at][0].buildings), 0.0)) {
            engine_building_builder_release(self.tiles_[at][0].buildings);
          }
          if (math_compare<"!=">(kern::to<Float>(self.tiles_[at][0].roads), 0.0)) {
            engine_road_stream_release(self.tiles_[at][0].roads);
          }
          if (math_compare<"!=">(kern::to<Float>(self.tiles_[at][0].footprints), 0.0)) {
            engine_footprint_stream_release(self.tiles_[at][0].footprints);
          }
        }
        map_remove_at(self.tiles_, at);
        it = at;
      } else {
        it += 1;
      }
    }
  }

  struct CityStream_take_requestsResult { Text Result = ""; };
  CityStream_take_requestsResult CityStream_take_requests(CityStream& self) {
    Text s = "";
    Text b = "";
    Float now_ms = 0.0;
    s = "[";
    for (auto it29 = flow_for(n29, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TileKey>(self.requests_))))); it29.next();) {
      b = "";
      b = text_format<"%s[%d,%d,%d]">(63, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n29.index), 0.0), ",", ""), self.requests_[n29.index].z, self.requests_[n29.index].x, self.requests_[n29.index].y);
      s = text_append(s, b, "");
    }
    if (logic_and(logic_not(math_compare<"==">(kern::to<Float>(list_length<TileKey>(self.requests_)), 0.0)), math_compare<"<">(self.ms_.t_request, 0.0))) {
      now_ms = engine_now_ms();
      self.ms_.t_request = kern::formula([](Float now_ms, Float start_ms_) -> Float { return (now_ms - start_ms_); }, now_ms, self.start_ms_);
    }
    list_clear(self.requests_);
    return {text_append(s, "]", "")};
  }

  void CityStream_provide(CityStream& self, const TileKey& key, const List<Int>& bytes) {
    Int it = 0;
    Float now_ms = 0.0;
    Int submit = 0;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<CityStream_Tile>>(self.tiles_, tiles::TileKey_hash(key).Result).index), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))));
    if (logic_or(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))), logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[it][0].state, CityStream_Tile_State::requested)))) {
      return;
    }
    if (logic_and(tiles::TileKey_equal(key, self.origin_tile_).Result, math_compare<"<">(self.ms_.t_data, 0.0))) {
      now_ms = engine_now_ms();
      self.ms_.t_data = kern::formula([](Float now_ms, Float start_ms_) -> Float { return (now_ms - start_ms_); }, now_ms, self.start_ms_);
    }
    n30 = city_jobs_cpp::tile_input(key, bytes, self.proj_.lon0, self.proj_.lat0);
    engine_jobs_submit(n31, kTile, n30.Result);
    submit = n31.result;
    self.tiles_[it][0].tile_job = submit;
    self.tiles_[it][0].state = CityStream_Tile_State::decoding;
  }

  void CityStream_feed_terrain(CityStream& self, CityStream_Tile& t) {
    Int i = 0;
    for (auto it38 = flow_for(n38, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Area>(t.features.areas))))); it38.next();) {
      if (enum_equal<AreaKind>(t.features.areas[n38.index].kind, AreaKind::water)) {
        terrain_cpp::Terrain_add_water_polygon(self.terrain_, t.features.areas[n38.index].rings);
      } else {
        terrain_cpp::Terrain_add_tint(self.terrain_, t.features.areas[n38.index].rings, tiles::TileKey_hash(t.key).Result, area_color(t.features.areas[n38.index].kind).Result);
      }
    }
    for (auto it39 = flow_for(n39, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Road>(t.features.roads))))); it39.next();) {
      if (tiles_cpp::road_is_water(t.features.roads[n39.index].cls).Result) {
        terrain_cpp::Terrain_add_waterway(self.terrain_, t.features.roads[n39.index].points, tiles_cpp::road_width(t.features.roads[n39.index].cls).Result, kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? 1.6 : ((a_2 != 0.0) ? 0.5 : 1.1)); }, kern::to<Float>(enum_equal<RoadClass>(t.features.roads[n39.index].cls, RoadClass::river)), kern::to<Float>(enum_equal<RoadClass>(t.features.roads[n39.index].cls, RoadClass::canal))));
      } else if (t.features.roads[n39.index].bridge) {
        i = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(t.features.roads[n39.index].points))), 0.5))) {
          terrain_cpp::Terrain_add_bridge(self.terrain_, t.features.roads[n39.index].points[i], t.features.roads[n39.index].points[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))], tiles_cpp::road_width(t.features.roads[n39.index].cls).Result);
          i += 1;
        }
      }
    }
  }

  struct select_cells_way_distanceResult { Float Result = 0.0; };
  KERN_PURE select_cells_way_distanceResult select_cells_way_distance(Float ab2 = 0.0, Vector a = Vector{0.0, 0.0, 0.0}, Vector ab = Vector{0.0, 0.0, 0.0}, Vector p = Vector{0.0, 0.0, 0.0}) {
    Float t = 0.0;
    t = kern::formula([](Float ab2, Float a) -> Float { return ((ab2 > 1.0) ? a : 0.0); }, ab2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float ab2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), ab2); }, vector_break(p).x, vector_break(a).x, vector_break(ab).x, vector_break(p).y, vector_break(a).y, vector_break(ab).y, ab2), 0.0, 1.0));
    return {kern::formula([](Float x, Float x_2, Float x_3, Float t, Float y, Float y_2, Float y_3) -> Float { return kern::math::sqrt((((x - x_2) - (x_3 * t)) * ((x - x_2) - (x_3 * t))) + (((y - y_2) - (y_3 * t)) * ((y - y_2) - (y_3 * t)))); }, vector_break(p).x, vector_break(a).x, vector_break(ab).x, t, vector_break(p).y, vector_break(a).y, vector_break(ab).y)};
  }

  struct select_cells_matchResult { Bool Result = false; };
  KERN_PURE select_cells_matchResult select_cells_match(const CityStream_select_cells_Pick& x, const CityStream_select_cells_Pick& y) {
    return {logic_or(math_compare<"<">(x.d, y.d), logic_and(math_compare<">">(kern::formula([](Float d, Float d_2) -> Float { return (d == d_2); }, x.d, y.d), 0.5), int64_less_unsigned(x.cell, y.cell)))};
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
    for (auto it52 = flow_for(n52, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(t.cell_ids[at]))))); it52.next();) {
      id = t.cell_ids[at][n52.index];
      engine_renderer_remove(n53, self.r_, id);
      remove = n53.result;
      gone = id;
      while (loop(list_contains<Int>(t.meshes, gone))) {
        list_remove(t.meshes, gone);
      }
    }
    at_2 = it;
    map_remove_at(t.cell_ids, at_2);
    key = cell;
    set_remove(n54, t.dirty_cells, key);
  }

  void CityStream_select_cells_settle(CityStream& self, CityStream_Tile& t, const CityStream_select_cells_Pick& p, Float reach = 0.0, Float kKeep = 0.0) {
    Bool want = false;
    Bool built = false;
    Int64 key = Int64{0x0};
    Int64 key_2 = Int64{0x0};
    want = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(set_contains<Int64>(t.want_cells, p.cell)))), 0.0);
    built = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(map_contains<Int64, List<Int>>(t.cell_ids, p.cell)))), 0.0);
    if (logic_and(want, logic_not(built))) {
      key = p.cell;
      set_add(n50, t.dirty_cells, key);
    }
    if (logic_and(logic_not(want), logic_not(built))) {
      key_2 = p.cell;
      set_remove(n51, t.dirty_cells, key_2);
    }
    if (logic_and(logic_and(logic_and(logic_and(logic_not(want), built), math_compare<">">(p.d, kern::formula([](Float reach, Float kKeep) -> Float { return (reach + (kKeep * 0.5)); }, reach, kKeep))), logic_not(math_compare<"!=">(kern::to<Float>(t.cell_job), 0.0))), logic_not(t.cell_uploading))) {
      CityStream_drop_cell(self, t, p.cell);
    }
  }

  void CityStream_select_cells(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, Bool force = false) {
    Float now = 0.0;
    Float now_ms = 0.0;
    Vector ahead = Vector{0.0, 0.0, 0.0};
    Float kR = 0.0;
    Float kKeep = 0.0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector ab = Vector{0.0, 0.0, 0.0};
    Float ab2 = 0.0;
    List<CityStream_select_cells_Pick> picks = List<CityStream_select_cells_Pick>{};
    Int it = 0;
    Bool skip = false;
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, 0, 0, 0, 0, List<Int>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<Int>{}, List<Int>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<Int>>{}, Set<Int64>{}, Map<Int64, List<Int>>{}, false, 0, 0, Map<Int64, List<Int>>{}, Set<Int64>{}, List<Int64>{}, 0, List<Int>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
    Int cx = 0;
    Int cy = 0;
    Float d = 0.0;
    Int total = 0;
    Float reach = 0.0;
    Int at = 0;
    Int count = 0;
    Bool stop = false;
    Int64 key = Int64{0x0};
    Bool skip_2 = false;
    List<Int64> far = List<Int64>{};
    Bool skip_3 = false;
    Int cx_2 = 0;
    Int cy_2 = 0;
    Int64 c = Int64{0x0};
    now_ms = engine_now_ms();
    now = now_ms;
    ahead = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x + x_2); }, vector_break(focus).x, vector_break(self.view_lead).x), kern::formula([](Float y, Float y_2) -> Float { return (y + y_2); }, vector_break(focus).y, vector_break(self.view_lead).y), 0.0);
    if (logic_and(logic_and(logic_and(logic_not(force), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(focus).x, vector_break(self.vis_at_).x, vector_break(focus).y, vector_break(self.vis_at_).y), 40.0)), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(ahead).x, vector_break(self.vis_ahead_).x, vector_break(ahead).y, vector_break(self.vis_ahead_).y), 60.0)), math_compare<"<">(kern::formula([](Float now, Float vis_ms_) -> Float { return (now - vis_ms_); }, now, self.vis_ms_), 1000.0))) {
      return;
    }
    self.vis_at_ = vector_make(vector_break(focus).x, vector_break(focus).y, 0.0);
    self.vis_ahead_ = ahead;
    self.vis_ms_ = now;
    kR = 900.0;
    kKeep = 220.0;
    a = vector_make(vector_break(focus).x, vector_break(focus).y, 0.0);
    ab = vector_sub(ahead, a);
    ab2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(ab).x, vector_break(ab).y);
    picks = List<CityStream_select_cells_Pick>{};
    it = 0;
    while (loop(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_)))))) {
      skip = false;
      t = list_get<CityStream_Tile>(self.tiles_[it], 0);
      if (logic_not(t.prepared)) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it47 = flow_for(n47, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<Int>>(t.cell_items))))); it47.next();) {
          cx = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(map_key_at<Int64, List<Int>>(t.cell_items, n47.index), 32)))));
          cy = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(int64_low(map_key_at<Int64, List<Int>>(t.cell_items, n47.index)))));
          d = select_cells_way_distance(ab2, a, ab, vector_make(kern::formula([](Float cx, Float kChunk) -> Float { return ((cx + 0.5) * kChunk); }, kern::to<Float>(cx), kChunk), kern::formula([](Float cy, Float kChunk) -> Float { return ((cy + 0.5) * kChunk); }, kern::to<Float>(cy), kChunk), 0.0)).Result;
          if (math_compare<"<=">(d, kern::formula([](Float kR, Float kKeep) -> Float { return (kR + kKeep); }, kR, kKeep))) {
            list_add(picks, struct_make<CityStream_select_cells_Pick>(d, kern::to<Int>(kern::formula([](Float it) -> Float { return kern::math::wrap32(it); }, kern::to<Float>(it))), map_key_at<Int64, List<Int>>(t.cell_items, n47.index), kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Int>(t.cell_items[n47.index]))))));
          }
        }
      }
      it += 1;
    }
    list_sort<&city_stream_cpp::select_cells_match>(*this, picks);
    for (auto it48 = flow_for(n48, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it48.next();) {
      set_clear(self.tiles_[n48.index][0].want_cells);
    }
    total = 0;
    reach = kR;
    count = list_length<CityStream_select_cells_Pick>(picks);
    at = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<">(kern::to<Float>(at), kern::to<Float>(count))))) {
      if (math_compare<">">(picks[at].d, kR)) {
        stop = true;
      }
      if (logic_not(stop)) {
        if (logic_and(math_compare<">=">(kern::to<Float>(total), kern::to<Float>(self.building_budget)), math_compare<">">(picks[at].d, 240.0))) {
          reach = picks[at].d;
          stop = true;
        }
        if (logic_not(stop)) {
          key = picks[at].cell;
          set_add(n49, self.tiles_[picks[at].tile][0].want_cells, key);
          total = kern::to<Int>(kern::formula([](Float total, Float a) -> Float { return kern::math::wrap32(total + a); }, kern::to<Float>(total), kern::to<Float>(picks[at].n)));
        }
      }
      if (logic_not(stop)) {
        at += 1;
      }
    }
    for (auto it55 = flow_for(n55, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CityStream_select_cells_Pick>(picks))))); it55.next();) {
      CityStream_select_cells_settle(self, self.tiles_[picks[n55.index].tile][0], picks[n55.index], reach, kKeep);
    }
    for (auto it56 = flow_for(n56, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it56.next();) {
      skip_2 = false;
      if (logic_or(math_compare<"!=">(kern::to<Float>(self.tiles_[n56.index][0].cell_job), 0.0), self.tiles_[n56.index][0].cell_uploading)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        far = List<Int64>{};
        for (auto it57 = flow_for(n57, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<Int>>(self.tiles_[n56.index][0].cell_ids))))); it57.next();) {
          skip_3 = false;
          if (math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(set_contains<Int64>(self.tiles_[n56.index][0].want_cells, map_key_at<Int64, List<Int>>(self.tiles_[n56.index][0].cell_ids, n57.index))))), 0.0)) {
            skip_3 = true;
          }
          if (logic_not(skip_3)) {
            cx_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(map_key_at<Int64, List<Int>>(self.tiles_[n56.index][0].cell_ids, n57.index), 32)))));
            cy_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(int64_low(map_key_at<Int64, List<Int>>(self.tiles_[n56.index][0].cell_ids, n57.index)))));
            if (math_compare<">">(select_cells_way_distance(ab2, a, ab, vector_make(kern::formula([](Float cx_2, Float kChunk) -> Float { return ((cx_2 + 0.5) * kChunk); }, kern::to<Float>(cx_2), kChunk), kern::formula([](Float cy_2, Float kChunk) -> Float { return ((cy_2 + 0.5) * kChunk); }, kern::to<Float>(cy_2), kChunk), 0.0)).Result, kern::formula([](Float kR, Float kKeep) -> Float { return (kR + kKeep); }, kR, kKeep))) {
              list_add(far, map_key_at<Int64, List<Int>>(self.tiles_[n56.index][0].cell_ids, n57.index));
            }
          }
        }
        for (auto it58 = flow_for(n58, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int64>(far))))); it58.next();) {
          c = far[n58.index];
          CityStream_drop_cell(self, self.tiles_[n56.index][0], c);
        }
      }
    }
  }

  void CityStream_furniture(const CityStream& self, List<Int64>& out) {
    list_clear(out);
    for (auto it60 = flow_for(n60, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it60.next();) {
      if (self.tiles_[n60.index][0].prepared) {
        list_add(out, map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n60.index));
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

  struct CityStream_ready_atResult { Bool Result = false; };
  KERN_PURE CityStream_ready_atResult CityStream_ready_at(const CityStream& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    Bool solid = false;
    if (logic_not(terrain_cpp::Terrain_ready_at(self.terrain_, p).Result)) {
      return {false};
    }
    solid = logic_and(logic_and(logic_and(self.near_.built, logic_not(self.near_.building)), logic_not(self.near_.dirty)), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.near_.at).x, vector_break(p).x, vector_break(self.near_.at).y, vector_break(p).y), 140.0));
    if (self.offline_) {
      for (auto it61 = flow_for(n61, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it61.next();) {
        if (self.tiles_[n61.index][0].procedural) {
          return {logic_and(enum_equal<CityStream_Tile_State>(self.tiles_[n61.index][0].state, CityStream_Tile_State::ready), solid)};
        }
      }
      return {false};
    }
    return {terrain_cpp::Terrain_corridors_at(self.terrain_, vector_break(p).x, vector_break(p).y).Result};
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
      engine_weld(n62, chunk.mesh, 0);
      weld = n62.result;
      chunk.mesh = n62.mesh;
    }
    engine_renderer_allocate_id(n63, self.r_);
    allocate_id = n63.result;
    id = allocate_id;
    self.upload_count_ = kern::to<Int>(kern::formula([](Float upload_count_) -> Float { return kern::math::wrap32(upload_count_ + 1.0); }, kern::to<Float>(self.upload_count_)));
    self.upload_vertices_ = int64_add(self.upload_vertices_, int64_of_int(engine_geo_mesh_data::MeshData_vertex_count(chunk.mesh).Result));
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material = material;
    look.extra_slot = kern::to<Int>(kern::formula([](Float walls, Float kMaxExtensionAttributes) -> Float { return ((walls != 0.0) ? 0.0 : kMaxExtensionAttributes); }, kern::to<Float>(walls), kern::to<Float>(kMaxExtensionAttributes)));
    look.pick_bvh = false;
    engine_upsert(n64, self.r_, id, chunk.mesh, look, List<Float>{}, List<Float>{});
    upsert = n64.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, id);
      return;
    }
    list_add(t.meshes, id);
    if (walls) {
      key = cell_key(chunk.x, chunk.y).Result;
      map_add(n65, t.cell_ids, key);
      t_2 = t.cell_ids[n65.index];
      list_add(t_2, id);
      put = t_2;
      t.cell_ids[n65.index] = put;
    }
    t.triangles = kern::to<Int>(kern::formula([](Float triangles, Float a) -> Float { return kern::math::wrap32(triangles + a); }, kern::to<Float>(t.triangles), kern::to<Float>(engine_geo_mesh_data::MeshData_triangle_count(chunk.mesh).Result)));
  }

  struct update_collision_way_d2Result { Float Result = 0.0; };
  KERN_PURE update_collision_way_d2Result update_collision_way_d2(Float way_l2 = 0.0, Vector way_a = Vector{0.0, 0.0, 0.0}, Vector way_ab = Vector{0.0, 0.0, 0.0}, Vector p = Vector{0.0, 0.0, 0.0}) {
    Float t = 0.0;
    Float dx = 0.0;
    Float dy = 0.0;
    t = kern::formula([](Float way_l2, Float a) -> Float { return ((way_l2 > 1.0) ? a : 0.0); }, way_l2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float way_l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), way_l2); }, vector_break(p).x, vector_break(way_a).x, vector_break(way_ab).x, vector_break(p).y, vector_break(way_a).y, vector_break(way_ab).y, way_l2), 0.0, 1.0));
    dx = kern::formula([](Float x, Float x_2, Float x_3, Float t) -> Float { return ((x - x_2) - (x_3 * t)); }, vector_break(p).x, vector_break(way_a).x, vector_break(way_ab).x, t);
    dy = kern::formula([](Float y, Float y_2, Float y_3, Float t) -> Float { return ((y - y_2) - (y_3 * t)); }, vector_break(p).y, vector_break(way_a).y, vector_break(way_ab).y, t);
    return {kern::formula([](Float dx, Float dy) -> Float { return ((dx * dx) + (dy * dy)); }, dx, dy)};
  }

  struct update_collision_matchResult { Bool Result = false; };
  KERN_PURE update_collision_matchResult update_collision_match(const CityStream_update_collision_Pick& a, const CityStream_update_collision_Pick& b) {
    return {math_compare<"<">(a.d2, b.d2)};
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
    engine_earcut_make(n73);
    Earcut_2 = n73.result;
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
    for (auto it74 = flow_for(n74, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(rings))))); it74.next();) {
      if (math_compare<">">(kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(rings[n74.index]).items)), 64.0)) {
        step = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div((a + 63.0), 64.0)); }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(rings[n74.index]).items))));
        thin = List<Vector>{};
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(rings[n74.index]).items))), 0.5))) {
          list_add(thin, rings[n74.index].items[k]);
          k += step;
        }
        rings[n74.index].items = thin;
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
          for (auto it75 = flow_for(n75, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(rings[at].items))))); it75.next();) {
            p = rings[at].items[n75.index];
            place = list_length<Float>(xy);
            item = vector_break(p).x;
            item_2 = vector_break(p).y;
            list_insert_list(xy, place, list_make<Float, "2">(item, item_2));
          }
          for (auto it76 = flow_for(n76, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(rings[r].items))))); it76.next();) {
            p0 = rings[r].items[n76.index];
            p1 = rings[r].items[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n76.index), kern::to<Float>(list_length<Vector>(rings[r].items))))];
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
      engine_earcut_triangulate(n77, earcut, xy, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::trunc(kern::math::div(a, 2.0))); }, kern::to<Float>(list_length<Float>(xy)))), holes, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Int>(holes)))), tris);
      tris = n77.out;
      base_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(pts))));
      for (auto it78 = flow_for(n78, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div(a, 2.0)); }, kern::to<Float>(list_length<Float>(xy))))))))); it78.next();) {
        list_add(pts, vector_make(xy[kern::to<Int>(kern::formula([](Float index) -> Float { return (index * 2.0); }, kern::to<Float>(n78.index)))], xy[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index * 2.0) + 1.0); }, kern::to<Float>(n78.index)))], b.top));
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
      engine_world_create_shape(n79, self.world_, body, ss);
      create_shape = n79.result;
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

  struct CityStream_build_nearResult { Bool Result = false; };
  CityStream_build_nearResult CityStream_build_near(CityStream& self, Float deadline = 0.0) {
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body create_body = physics_Body{Int64{0x0}};
    Int count = 0;
    physics_Shape fill = physics_Shape{Int64{0x0}};
    Bool skip = false;
    Int slot = 0;
    Int was = 0;
    Int64_Int_pair v = Int64_Int_pair{Int64{0x0}, 0};
    Int it = 0;
    Float now_ms = 0.0;
    if (logic_not(engine_physics_physics::Body_valid(self.near_.body).Result)) {
      bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
      bs.type = physics_BodyType::fixed;
      engine_world_create_body(n72, self.world_, bs);
      create_body = n72.result;
      self.near_.body = create_body;
      count = list_length<Int64_Int_pair>(self.near_.list);
      fill = struct_make<physics_Shape>(Int64{0x0});
      list_clear(self.near_.shapes);
      list_resize(self.near_.shapes, count, fill);
    }
    while (loop(math_compare<">">(kern::formula([](Float next, Float a) -> Float { return ((next < 0.0) != (a < 0.0) ? (a < 0.0) : (next < a)); }, kern::to<Float>(self.near_.next), kern::to<Float>(list_length<Int64_Int_pair>(self.near_.list))), 0.5))) {
      skip = false;
      was = self.near_.next;
      self.near_.next = kern::to<Int>(kern::formula([](Float next) -> Float { return (next + 1.0); }, kern::to<Float>(self.near_.next)));
      slot = was;
      v = self.near_.list[slot];
      it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<CityStream_Tile>>(self.tiles_, v.first).index), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))));
      if (logic_or(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))), math_compare<">">(kern::formula([](Float second, Float a) -> Float { return (!((second < 0.0) != (a < 0.0) ? (a < 0.0) : (second < a))); }, kern::to<Float>(v.second), kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[it][0].features.buildings))), 0.5))) {
        skip = true;
      }
      if (logic_not(skip)) {
        n80 = CityStream_building_shape(self, self.tiles_[it][0].features.buildings[v.second], self.near_.body);
        self.near_.shapes[slot] = n80.Result;
        if (engine_physics_physics::Shape_valid(self.near_.shapes[slot]).Result) {
          self.near_.colliders = kern::to<Int>(kern::formula([](Float colliders) -> Float { return kern::math::wrap32(colliders + 1.0); }, kern::to<Float>(self.near_.colliders)));
        }
        now_ms = engine_now_ms();
        if (math_compare<">">(now_ms, deadline)) {
          return {false};
        }
      }
    }
    return {true};
  }

  void CityStream_update_collision(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, Float deadline = 0.0) {
    Float now = 0.0;
    Float now_ms = 0.0;
    Vector way_a = Vector{0.0, 0.0, 0.0};
    Vector way_ab = Vector{0.0, 0.0, 0.0};
    Float way_l2 = 0.0;
    Bool moved = false;
    Int i = 0;
    List<CityStream_update_collision_Pick> all = List<CityStream_update_collision_Pick>{};
    Bool skip = false;
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, 0, 0, 0, 0, List<Int>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<Int>{}, List<Int>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<Int>>{}, Set<Int64>{}, Map<Int64, List<Int>>{}, false, 0, 0, Map<Int64, List<Int>>{}, Set<Int64>{}, List<Int64>{}, 0, List<Int>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
    Bool skip_2 = false;
    Float d2 = 0.0;
    Float mx = 0.0;
    Float my = 0.0;
    Int cap = 0;
    Bool both = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both_2 = false;
    Float now_ms_2 = 0.0;
    now_ms = engine_now_ms();
    now = now_ms;
    way_a = vector_make(vector_break(focus).x, vector_break(focus).y, 0.0);
    way_ab = vector_make(vector_break(self.view_lead).x, vector_break(self.view_lead).y, 0.0);
    way_l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(way_ab).x, vector_break(way_ab).y);
    moved = logic_or(logic_or(math_compare<">=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(focus).x, vector_break(self.near_.at).x, vector_break(focus).y, vector_break(self.near_.at).y), 140.0), math_compare<">=">(kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3) -> Float { return kern::math::sqrt((((x + x_2) - x_3) * ((x + x_2) - x_3)) + (((y + y_2) - y_3) * ((y + y_2) - y_3))); }, vector_break(focus).x, vector_break(self.view_lead).x, vector_break(self.near_.ahead).x, vector_break(focus).y, vector_break(self.view_lead).y, vector_break(self.near_.ahead).y), 140.0)), math_compare<"!=">(kern::to<Float>(list_length<Vector>(self.extra_foci)), kern::to<Float>(list_length<Vector>(self.near_.mates_at))));
    i = 0;
    while (loop(logic_and(logic_not(moved), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(self.extra_foci))), 0.5)))) {
      moved = math_compare<">=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.extra_foci[i]).x, vector_break(self.near_.mates_at[i]).x, vector_break(self.extra_foci[i]).y, vector_break(self.near_.mates_at[i]).y), 140.0);
      i += 1;
    }
    if (logic_and(logic_not(self.near_.building), logic_or(moved, logic_and(self.near_.dirty, math_compare<">=">(kern::formula([](Float now, Float at_ms) -> Float { return (now - at_ms); }, now, self.near_.at_ms), 250.0))))) {
      all = List<CityStream_update_collision_Pick>{};
      for (auto it68 = flow_for(n68, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it68.next();) {
        skip = false;
        t = list_get<CityStream_Tile>(self.tiles_[n68.index], 0);
        if (logic_and(logic_and(logic_and(logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready)), logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::building))), logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::roads))), logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::areas)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_not(t.prepared)) {
            skip = true;
          }
          if (logic_not(skip)) {
            for (auto it69 = flow_for(n69, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(t.cent))))); it69.next();) {
              skip_2 = false;
              if (logic_and(int64_equal(map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n68.index), self.excl_tile_), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n69.index))))), kern::to<Float>(self.excl_index_)))) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                d2 = update_collision_way_d2(way_l2, way_a, way_ab, t.cent[n69.index]).Result;
                for (auto it70 = flow_for(n70, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(self.extra_foci))))); it70.next();) {
                  mx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(t.cent[n69.index]).x, vector_break(self.extra_foci[n70.index]).x);
                  my = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(t.cent[n69.index]).y, vector_break(self.extra_foci[n70.index]).y);
                  d2 = kern::formula([](Float d2, Float mx, Float my) -> Float { return kern::math::min(d2, ((mx * mx) + (my * my))); }, d2, mx, my);
                }
                if (math_compare<"<=">(d2, 115600.0)) {
                  list_add(all, struct_make<CityStream_update_collision_Pick>(d2, map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n68.index), n69.index));
                }
              }
            }
          }
        }
      }
      list_sort<&city_stream_cpp::update_collision_match>(*this, all);
      cap = kern::to<Int>(kern::formula([](Float a) -> Float { return (480.0 + (320.0 * a)); }, kern::to<Float>(list_length<Vector>(self.extra_foci))));
      if (math_compare<">">(kern::formula([](Float cap, Float a) -> Float { return ((cap < 0.0) != (a < 0.0) ? (a < 0.0) : (cap < a)); }, kern::to<Float>(cap), kern::to<Float>(list_length<CityStream_update_collision_Pick>(all))), 0.5)) {
        list_resize(all, cap, CityStream_update_collision_Pick{0.0, Int64{0x0}, 0});
      }
      list_clear(self.near_.list);
      for (auto it71 = flow_for(n71, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CityStream_update_collision_Pick>(all))))); it71.next();) {
        list_add(self.near_.list, struct_make<Int64_Int_pair>(all[n71.index].tile, all[n71.index].i));
      }
      self.near_.old_body = self.near_.body;
      self.near_.body = struct_make<physics_Body>(Int64{0x0});
      self.near_.next = 0;
      self.near_.colliders = 0;
      self.near_.building = true;
      self.near_.dirty = false;
      self.near_.at = vector_make(vector_break(focus).x, vector_break(focus).y, 0.0);
      self.near_.ahead = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x + x_2); }, vector_break(focus).x, vector_break(self.view_lead).x), kern::formula([](Float y, Float y_2) -> Float { return (y + y_2); }, vector_break(focus).y, vector_break(self.view_lead).y), 0.0);
      self.near_.mates_at = self.extra_foci;
      self.near_.at_ms = now;
    }
    both = self.near_.building;
    if (both) {
      n82 = CityStream_build_near(self, deadline);
      both = n82.Result;
    }
    if (both) {
      both_2 = engine_physics_physics::Body_valid(self.near_.old_body).Result;
      if (both_2) {
        body_valid_2 = engine_world_body_valid(self.world_, self.near_.old_body);
        both_2 = body_valid_2;
      }
      if (both_2) {
        engine_world_destroy_body(self.world_, self.near_.old_body);
      }
      self.near_.old_body = struct_make<physics_Body>(Int64{0x0});
      self.near_.building = false;
      self.near_.built = true;
      if (logic_and(math_compare<"<">(self.ms_.t_collision, 0.0), logic_not(math_compare<"==">(kern::to<Float>(list_length<Int64_Int_pair>(self.near_.list)), 0.0)))) {
        now_ms_2 = engine_now_ms();
        self.ms_.t_collision = kern::formula([](Float now_ms_2, Float start_ms_) -> Float { return (now_ms_2 - start_ms_); }, now_ms_2, self.start_ms_);
      }
    }
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
    map_add(n83, self.damage_, key);
    key_2 = self.tiles_[it][0].features.buildings[index].seed;
    map_add(n84, self.damage_, key_2);
    t = self.damage_[n84.index];
    t.height = kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, self.tiles_[it][0].features.buildings[index].top, self.tiles_[it][0].features.buildings[index].base);
    put = t;
    self.damage_[n84.index] = put;
    key_3 = self.tiles_[it][0].features.buildings[index].seed;
    map_add(n85, self.damage_, key_3);
    t_2 = self.damage_[n85.index];
    t_2.leveled = self.tiles_[it][0].features.buildings[index].leveled;
    put_2 = t_2;
    self.damage_[n85.index] = put_2;
    if (logic_not(self.damage_quiet_)) {
      list_add(self.damage_log_, self.tiles_[it][0].features.buildings[index].seed);
    }
    if (logic_and(math_compare<">">(kern::formula([](Float index, Float a) -> Float { return ((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a)); }, kern::to<Float>(index), kern::to<Float>(list_length<Vector>(self.tiles_[it][0].cent))), 0.5), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(map_contains<Int64, List<Int>>(self.tiles_[it][0].cell_ids, cell_of(self.tiles_[it][0].cent[index]).Result)))), 0.0))) {
      key_4 = cell_of(self.tiles_[it][0].cent[index]).Result;
      set_add(n86, self.tiles_[it][0].dirty_cells, key_4);
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
          n87 = CityStream_building_shape(self, self.tiles_[it][0].features.buildings[index], self.near_.body);
          self.near_.shapes[k] = n87.Result;
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
    for (auto it88 = flow_for(n88, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))))); it88.next();) {
      list_add(out, struct_make<CityStream_DamageRec>(map_key_at<Int, CityStream_Damage>(self.damage_, n88.index), self.damage_[n88.index].height, self.damage_[n88.index].leveled));
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
    for (auto it89 = flow_for(n89, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<CityStream_DamageRec>(list))))); it89.next();) {
      skip = false;
      r = list[n89.index];
      if (logic_or(logic_not(math_compare<">=">(r.height, 0.0)), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, r.height), 0.0)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, CityStream_Damage>(self.damage_, r.seed).index), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))));
        if (logic_and(logic_and(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_)))), math_compare<"<=">(self.damage_[it].height, kern::formula([](Float height) -> Float { return (height + 0.01); }, r.height))), logic_or(self.damage_[it].leveled, logic_not(r.leveled)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          put = n89.index;
          key = r.seed;
          map_add(n90, want, key);
          want[n90.index] = put;
        }
      }
    }
    if (math_compare<"==">(kern::to<Float>(map_length<Int, Int>(want)), 0.0)) {
      return {false};
    }
    self.damage_quiet_ = logic_not(the_log);
    for (auto it91 = flow_for(n91, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it91.next();) {
      skip_2 = false;
      if (logic_not(self.tiles_[n91.index][0].prepared)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        for (auto it92 = flow_for(n92, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[n91.index][0].features.buildings))))); it92.next();) {
          skip_3 = false;
          w = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, Int>(want, self.tiles_[n91.index][0].features.buildings[n92.index].seed).index), kern::to<Float>(map_length<Int, Int>(want))));
          if (math_compare<"==">(kern::to<Float>(w), kern::to<Float>(map_length<Int, Int>(want)))) {
            skip_3 = true;
          }
          if (logic_not(skip_3)) {
            b = self.tiles_[n91.index][0].features.buildings[n92.index];
            r_2 = list[want[w]];
            n93 = CityStream_damage_building(self, map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n91.index), n92.index, kern::formula([](Float base, Float height) -> Float { return (base + height); }, b.base, r_2.height), r_2.leveled);
            if (n93.Result) {
              changed = true;
            }
          }
        }
      }
    }
    for (auto it94 = flow_for(n94, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Int>(want))))); it94.next();) {
      skip_4 = false;
      r_3 = list[want[n94.index]];
      it_2 = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, CityStream_Damage>(self.damage_, map_key_at<Int, Int>(want, n94.index)).index), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))));
      if (math_compare<"==">(kern::to<Float>(it_2), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_)))) {
        put_2 = struct_make<CityStream_Damage>(r_3.height, r_3.leveled);
        key_2 = map_key_at<Int, Int>(want, n94.index);
        map_add(n95, self.damage_, key_2);
        self.damage_[n95.index] = put_2;
      } else if (logic_or(math_compare<"<">(r_3.height, kern::formula([](Float a) -> Float { return (a - 0.01); }, self.damage_[it_2].height)), logic_and(r_3.leveled, logic_not(self.damage_[it_2].leveled)))) {
        self.damage_[it_2].height = kern::formula([](Float a, Float height) -> Float { return kern::math::min(a, height); }, self.damage_[it_2].height, r_3.height);
        self.damage_[it_2].leveled = logic_or(self.damage_[it_2].leveled, r_3.leveled);
      } else {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        changed = true;
        if (the_log) {
          list_add(self.damage_log_, map_key_at<Int, Int>(want, n94.index));
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
    for (auto it96 = flow_for(n96, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it96.next();) {
      skip = false;
      if (logic_not(self.tiles_[n96.index][0].prepared)) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it97 = flow_for(n97, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[n96.index][0].features.buildings))))); it97.next();) {
          skip_2 = false;
          if (logic_and(math_compare<">=">(self.tiles_[n96.index][0].features.buildings[n97.index].top, kern::formula([](Float a) -> Float { return (a - 0.01); }, self.tiles_[n96.index][0].features.buildings[n97.index].full_top)), logic_not(self.tiles_[n96.index][0].features.buildings[n97.index].leveled))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            self.tiles_[n96.index][0].features.buildings[n97.index].top = self.tiles_[n96.index][0].features.buildings[n97.index].full_top;
            self.tiles_[n96.index][0].features.buildings[n97.index].leveled = false;
            if (logic_and(math_compare<">">(kern::formula([](Float index, Float a) -> Float { return ((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a)); }, kern::to<Float>(n97.index), kern::to<Float>(list_length<Vector>(self.tiles_[n96.index][0].cent))), 0.5), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(map_contains<Int64, List<Int>>(self.tiles_[n96.index][0].cell_ids, cell_of(self.tiles_[n96.index][0].cent[n97.index]).Result)))), 0.0))) {
              key = cell_of(self.tiles_[n96.index][0].cent[n97.index]).Result;
              set_add(n98, self.tiles_[n96.index][0].dirty_cells, key);
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

  void CityStream_step_time(CityStream& self, Text name = "", Float t0 = 0.0) {
    Float took = 0.0;
    Float now_ms = 0.0;
    now_ms = engine_now_ms();
    took = kern::formula([](Float now_ms, Float t0) -> Float { return (now_ms - t0); }, now_ms, t0);
    if (math_compare<">">(took, self.worst_step_ms_)) {
      self.worst_step_ms_ = took;
      self.worst_step_ = name;
    }
  }

  struct CityStream_finish_cellsResult { Bool Result = false; };
  CityStream_finish_cellsResult CityStream_finish_cells(CityStream& self, CityStream_Tile& t, Float deadline = 0.0) {
    Bool take = false;
    Int64 c = Int64{0x0};
    Int it = 0;
    List<Int> put = List<Int>{};
    Int64 key = Int64{0x0};
    Int at = 0;
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    Bool stop = false;
    GeoChunk chunk = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}};
    Float t1 = 0.0;
    Float now_ms = 0.0;
    Float now_ms_2 = 0.0;
    Float now_ms_3 = 0.0;
    Bool both = false;
    Int id = 0;
    Bool remove = false;
    Int gone = 0;
    Int64 c_2 = Int64{0x0};
    Int64 key_2 = Int64{0x0};
    Float now_ms_4 = 0.0;
    if (logic_not(t.cell_uploading)) {
      engine_jobs_take(n104, t.cell_job, t.built_bytes);
      take = n104.result;
      t.built_bytes = n104.out;
      if (logic_not(take)) {
        return {false};
      }
      t.cell_job = 0;
      for (auto it105 = flow_for(n105, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int64>(t.job_cells))))); it105.next();) {
        c = t.job_cells[n105.index];
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<Int>>(t.cell_ids, c).index), kern::to<Float>(map_length<Int64, List<Int>>(t.cell_ids))));
        if (logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(t.cell_ids))))) {
          var_take(n106, t.cell_ids[it]);
          put = n106.value;
          key = c;
          map_add(n107, t.cell_old, key);
          t.cell_old[n107.index] = put;
          at = it;
          map_remove_at(t.cell_ids, at);
        }
      }
      t.built_at = 4;
      t.upload_next = city_jobs_cpp::chunk_count(t.built_bytes).Result;
      t.cell_uploading = true;
    }
    m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}};
    m.roughness = 0.82;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float upload_next) -> Float { return ((0.0 < 0.0) != (upload_next < 0.0) ? (upload_next < 0.0) : (0.0 < upload_next)); }, kern::to<Float>(t.upload_next)), 0.5)))) {
      chunk = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}};
      n108 = city_jobs_cpp::read_chunk(t.built_bytes, t.built_at, chunk);
      if (logic_not(n108.Result)) {
        t.upload_next = 0;
        stop = true;
      }
      if (logic_not(stop)) {
        t.upload_next = kern::to<Int>(kern::formula([](Float upload_next) -> Float { return (upload_next - 1.0); }, kern::to<Float>(t.upload_next)));
        now_ms = engine_now_ms();
        t1 = now_ms;
        CityStream_upload(self, t, chunk, m, true);
        CityStream_step_time(self, "building.upload", t1);
        both = math_compare<">">(kern::formula([](Float upload_next) -> Float { return ((0.0 < 0.0) != (upload_next < 0.0) ? (upload_next < 0.0) : (0.0 < upload_next)); }, kern::to<Float>(t.upload_next)), 0.5);
        if (both) {
          now_ms_3 = engine_now_ms();
          both = math_compare<">">(now_ms_3, deadline);
        }
        if (both) {
          return {false};
        }
      }
    }
    list_clear(t.built_bytes);
    for (auto it109 = flow_for(n109, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<Int>>(t.cell_old))))); it109.next();) {
      for (auto it110 = flow_for(n110, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(t.cell_old[n109.index]))))); it110.next();) {
        id = t.cell_old[n109.index][n110.index];
        engine_renderer_remove(n111, self.r_, id);
        remove = n111.result;
        gone = id;
        while (loop(list_contains<Int>(t.meshes, gone))) {
          list_remove(t.meshes, gone);
        }
      }
    }
    map_clear(t.cell_old);
    for (auto it112 = flow_for(n112, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int64>(t.job_cells))))); it112.next();) {
      c_2 = t.job_cells[n112.index];
      key_2 = c_2;
      map_add(n113, t.cell_ids, key_2);
    }
    list_clear(t.job_cells);
    t.cell_uploading = false;
    self.rebuilds_ = kern::to<Int>(kern::formula([](Float rebuilds_) -> Float { return kern::math::wrap32(rebuilds_ + 1.0); }, kern::to<Float>(self.rebuilds_)));
    if (logic_and(math_compare<"<">(self.ms_.t_built, 0.0), tiles::TileKey_equal(t.key, self.origin_tile_).Result)) {
      now_ms_4 = engine_now_ms();
      self.ms_.t_built = kern::formula([](Float now_ms_4, Float start_ms_) -> Float { return (now_ms_4 - start_ms_); }, now_ms_4, self.start_ms_);
    }
    return {true};
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
    for (auto it116 = flow_for(n116, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it116.next();) {
      skip = false;
      t = list_get<CityStream_Tile>(self.tiles_[n116.index], 0);
      if (logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x, Float r) -> Float { return ((x - r) - 200.0); }, t.bounds.x, r)), math_compare<">">(vector_break(p).x, kern::formula([](Float z, Float r) -> Float { return ((z + r) + 200.0); }, t.bounds.z, r))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y, Float r) -> Float { return ((y - r) - 200.0); }, t.bounds.y, r))), math_compare<">">(vector_break(p).y, kern::formula([](Float w, Float r) -> Float { return ((w + r) + 200.0); }, t.bounds.w, r)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it117 = flow_for(n117, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(t.features.buildings))))); it117.next();) {
            skip_2 = false;
            if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(t.features.buildings[n117.index].rings)), 0.0), math_compare<"==">(kern::to<Float>(list_length<Vector>(t.features.buildings[n117.index].rings[0].items)), 0.0))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              q = t.features.buildings[n117.index].rings[0].items[0];
              if (logic_or(math_compare<">">(kern::formula([](Float x, Float x_2) -> Float { return kern::math::abs((x - x_2)); }, vector_break(q).x, vector_break(p).x), kern::formula([](Float r) -> Float { return (r + 150.0); }, r)), math_compare<">">(kern::formula([](Float y, Float y_2) -> Float { return kern::math::abs((y - y_2)); }, vector_break(q).y, vector_break(p).y), kern::formula([](Float r) -> Float { return (r + 150.0); }, r)))) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                edge = 0;
                for (auto it118 = flow_for(n118, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector_row>(t.features.buildings[n117.index].rings))))); it118.next();) {
                  skip_3 = false;
                  ring = t.features.buildings[n117.index].rings[n118.index].items;
                  if (math_compare<"<">(kern::to<Float>(list_length<Vector>(ring)), 3.0)) {
                    skip_3 = true;
                  }
                  if (logic_not(skip_3)) {
                    ccw = math_compare<">">(signed_area(ring).Result, 0.0);
                    s = kern::formula([](Float ccw, Float index) -> Float { return ((ccw == (index == 0.0)) ? 1.0 : (-1.0)); }, kern::to<Float>(ccw), kern::to<Float>(n118.index));
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
                          list_add(out, struct_make<CityStream_Wall>(a, d, n, len, t.features.buildings[n117.index].base, t.features.buildings[n117.index].top, t.features.buildings[n117.index].seed, edge));
                        }
                      }
                      i += 1;
                      edge = kern::to<Int>(kern::formula([](Float edge) -> Float { return kern::math::wrap32(edge + 1.0); }, kern::to<Float>(edge)));
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
    for (auto it119 = flow_for(n119, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it119.next();) {
      skip = false;
      t = list_get<CityStream_Tile>(self.tiles_[n119.index], 0);
      if (logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x) -> Float { return (x - 200.0); }, t.bounds.x)), math_compare<">">(vector_break(p).x, kern::formula([](Float z) -> Float { return (z + 200.0); }, t.bounds.z))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y) -> Float { return (y - 200.0); }, t.bounds.y))), math_compare<">">(vector_break(p).y, kern::formula([](Float w) -> Float { return (w + 200.0); }, t.bounds.w)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it120 = flow_for(n120, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(t.features.buildings))))); it120.next();) {
            skip_2 = false;
            if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(t.features.buildings[n120.index].rings)), 0.0), math_compare<"<">(kern::to<Float>(list_length<Vector>(t.features.buildings[n120.index].rings[0].items)), 3.0))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              ring = t.features.buildings[n120.index].rings[0].items;
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

  struct CityStream_street_pointResult { Bool Result = false; };
  CityStream_street_pointResult CityStream_street_point(const CityStream& self, Rng& rng, Vector& point, Float& heading, Vector near = Vector{0.0, 0.0, 0.0}, Float radius = 0.0) {
    Bool skip = false;
    Int l_at = 0;
    Int i = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Float t = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Float d = 0.0;
    Float off = 0.0;
    if (math_compare<"==">(kern::to<Float>(list_length<Lane>(self.lanes_)), 0.0)) {
      return {false};
    }
    for (auto it121 = flow_for(n121, 0, 47); it121.next();) {
      skip = false;
      n122 = common::Rng_next(rng);
      l_at = kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n122.Result), kern::to<Float>(list_length<Lane>(self.lanes_))));
      if (logic_or(logic_not(self.lanes_[l_at].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(self.lanes_[l_at].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        n123 = common::Rng_next(rng);
        i = kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, (a - 1.0)); }, kern::to<Float>(n123.Result), kern::to<Float>(list_length<Vector>(self.lanes_[l_at].points))));
        a = self.lanes_[l_at].points[i];
        b = self.lanes_[l_at].points[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))];
        n124 = common::Rng_unit(rng);
        t = n124.Result;
        p = vector_add(a, vector_scale(vector_sub(b, a), t));
        d = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(p).x, vector_break(near).x, vector_break(p).y, vector_break(near).y);
        if (logic_or(math_compare<">">(d, radius), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(b).x, vector_break(a).x, vector_break(b).y, vector_break(a).y), 4.0))) {
          skip = true;
        }
        if (logic_not(skip)) {
          heading = kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(b).y, vector_break(a).y, vector_break(b).x, vector_break(a).x);
          off = kern::formula([](Float a) -> Float { return kern::math::min((a * 0.25), 1.8); }, self.lanes_[l_at].width);
          point = vector_make(kern::formula([](Float x, Float heading, Float off) -> Float { return (x + (kern::math::sin(heading) * off)); }, vector_break(p).x, heading, off), kern::formula([](Float y, Float heading, Float off) -> Float { return (y - (kern::math::cos(heading) * off)); }, vector_break(p).y, heading, off), CityStream_ground_height(self, vector_break(p).x, vector_break(p).y).Result);
          return {true};
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
    for (auto it125 = flow_for(n125, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it125.next();) {
      skip = false;
      if (logic_and(logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n125.index][0].state, CityStream_Tile_State::ready)), logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n125.index][0].state, CityStream_Tile_State::collision)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it126 = flow_for(n126, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[n125.index][0].features.buildings))))); it126.next();) {
          skip_2 = false;
          b = self.tiles_[n125.index][0].features.buildings[n126.index];
          if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(b.rings)), 0.0), math_compare<"<">(kern::to<Float>(list_length<Vector>(b.rings[0].items)), 3.0))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            x0 = 1000000000.0;
            y0 = 1000000000.0;
            x1 = -1000000000.0;
            y1 = -1000000000.0;
            for (auto it127 = flow_for(n127, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(b.rings[0].items))))); it127.next();) {
              q = b.rings[0].items[n127.index];
              x0 = kern::formula([](Float x0, Float x) -> Float { return kern::math::min(x0, x); }, x0, vector_break(q).x);
              y0 = kern::formula([](Float y0, Float y) -> Float { return kern::math::min(y0, y); }, y0, vector_break(q).y);
              x1 = kern::formula([](Float x1, Float x) -> Float { return kern::math::max(x1, x); }, x1, vector_break(q).x);
              y1 = kern::formula([](Float y1, Float y) -> Float { return kern::math::max(y1, y); }, y1, vector_break(q).y);
            }
            if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x0, Float radius) -> Float { return (x0 - radius); }, x0, radius)), math_compare<">">(vector_break(p).x, kern::formula([](Float x1, Float radius) -> Float { return (x1 + radius); }, x1, radius))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y0, Float radius) -> Float { return (y0 - radius); }, y0, radius))), math_compare<">">(vector_break(p).y, kern::formula([](Float y1, Float radius) -> Float { return (y1 + radius); }, y1, radius)))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              list_add(out, struct_make<CityStream_BuildingRef>(map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n125.index), kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n126.index)))));
            }
          }
        }
      }
    }
  }

  void CityStream_ready_tiles(const CityStream& self, List<Int64>& out) {
    for (auto it128 = flow_for(n128, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it128.next();) {
      if (self.tiles_[n128.index][0].prepared) {
        list_add(out, map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n128.index));
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
    for (auto it129 = flow_for(n129, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it129.next();) {
      skip = false;
      if (logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n129.index][0].state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it130 = flow_for(n130, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Area>(self.tiles_[n129.index][0].features.areas))))); it130.next();) {
          skip_2 = false;
          a = self.tiles_[n129.index][0].features.areas[n130.index];
          if (logic_or(logic_or(logic_not(enum_equal<AreaKind>(a.kind, kind)), math_compare<"==">(kern::to<Float>(list_length<Vector_row>(a.rings)), 0.0)), math_compare<"<">(kern::to<Float>(list_length<Vector>(a.rings[0].items)), 3.0))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            x0 = 1000000000.0;
            y0 = 1000000000.0;
            x1 = -1000000000.0;
            y1 = -1000000000.0;
            for (auto it131 = flow_for(n131, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(a.rings[0].items))))); it131.next();) {
              q = a.rings[0].items[n131.index];
              x0 = kern::formula([](Float x0, Float x) -> Float { return kern::math::min(x0, x); }, x0, vector_break(q).x);
              y0 = kern::formula([](Float y0, Float y) -> Float { return kern::math::min(y0, y); }, y0, vector_break(q).y);
              x1 = kern::formula([](Float x1, Float x) -> Float { return kern::math::max(x1, x); }, x1, vector_break(q).x);
              y1 = kern::formula([](Float y1, Float y) -> Float { return kern::math::max(y1, y); }, y1, vector_break(q).y);
            }
            if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x0, Float radius) -> Float { return (x0 - radius); }, x0, radius)), math_compare<">">(vector_break(p).x, kern::formula([](Float x1, Float radius) -> Float { return (x1 + radius); }, x1, radius))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y0, Float radius) -> Float { return (y0 - radius); }, y0, radius))), math_compare<">">(vector_break(p).y, kern::formula([](Float y1, Float radius) -> Float { return (y1 + radius); }, y1, radius)))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              list_add(out, struct_make<CityStream_AreaRef>(int64_add(int64_mul(map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n129.index), Int64{0x10000}), int64_of_int(n130.index)), map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n129.index), kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n130.index)))));
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
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { Bool result = false; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct { Bool result = false; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<18> { Int index = 0; } n18;
  struct : kern::Node<19> { Int index = 0; } n19;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  city_jobs_cpp::tile_inputResult n30;
  struct { Int result = 0; } n31;
  struct : kern::Node<38> { Int index = 0; } n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  struct : kern::Node<47> { Int index = 0; } n47;
  struct : kern::Node<48> { Int index = 0; } n48;
  struct { Bool added = false; Int index = 0; } n49;
  struct { Bool added = false; Int index = 0; } n50;
  struct { Bool removed = false; } n51;
  struct : kern::Node<52> { Int index = 0; } n52;
  struct { Bool result = false; } n53;
  struct { Bool removed = false; } n54;
  struct : kern::Node<55> { Int index = 0; } n55;
  struct : kern::Node<56> { Int index = 0; } n56;
  struct : kern::Node<57> { Int index = 0; } n57;
  struct : kern::Node<58> { Int index = 0; } n58;
  struct : kern::Node<60> { Int index = 0; } n60;
  struct : kern::Node<61> { Int index = 0; } n61;
  struct { WeldStats result = WeldStats{0, 0, 0, 0, false}; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; } n62;
  struct { Int result = 0; } n63;
  struct { Bool result = false; } n64;
  struct { Bool added = false; Int index = 0; } n65;
  struct : kern::Node<68> { Int index = 0; } n68;
  struct : kern::Node<69> { Int index = 0; } n69;
  struct : kern::Node<70> { Int index = 0; } n70;
  struct : kern::Node<71> { Int index = 0; } n71;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n72;
  struct { Int result = 0; } n73;
  struct : kern::Node<74> { Int index = 0; } n74;
  struct : kern::Node<75> { Int index = 0; } n75;
  struct : kern::Node<76> { Int index = 0; } n76;
  struct { List<Int> out = List<Int>{}; } n77;
  struct : kern::Node<78> { Int index = 0; } n78;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n79;
  CityStream_building_shapeResult n80;
  CityStream_build_nearResult n82;
  struct { Bool added = false; Int index = 0; } n83;
  struct { Bool added = false; Int index = 0; } n84;
  struct { Bool added = false; Int index = 0; } n85;
  struct { Bool added = false; Int index = 0; } n86;
  CityStream_building_shapeResult n87;
  struct : kern::Node<88> { Int index = 0; } n88;
  struct : kern::Node<89> { Int index = 0; } n89;
  struct { Bool added = false; Int index = 0; } n90;
  struct : kern::Node<91> { Int index = 0; } n91;
  struct : kern::Node<92> { Int index = 0; } n92;
  CityStream_damage_buildingResult n93;
  struct : kern::Node<94> { Int index = 0; } n94;
  struct { Bool added = false; Int index = 0; } n95;
  struct : kern::Node<96> { Int index = 0; } n96;
  struct : kern::Node<97> { Int index = 0; } n97;
  struct { Bool added = false; Int index = 0; } n98;
  struct { Bool result = false; List<Int> out = List<Int>{}; } n104;
  struct : kern::Node<105> { Int index = 0; } n105;
  struct { List<Int> value = List<Int>{}; } n106;
  struct { Bool added = false; Int index = 0; } n107;
  city_jobs_cpp::read_chunkResult n108;
  struct : kern::Node<109> { Int index = 0; } n109;
  struct : kern::Node<110> { Int index = 0; } n110;
  struct { Bool result = false; } n111;
  struct : kern::Node<112> { Int index = 0; } n112;
  struct { Bool added = false; Int index = 0; } n113;
  struct : kern::Node<116> { Int index = 0; } n116;
  struct : kern::Node<117> { Int index = 0; } n117;
  struct : kern::Node<118> { Int index = 0; } n118;
  struct : kern::Node<119> { Int index = 0; } n119;
  struct : kern::Node<120> { Int index = 0; } n120;
  struct : kern::Node<121> { Int index = 0; } n121;
  common::Rng_nextResult n122;
  common::Rng_nextResult n123;
  common::Rng_unitResult n124;
  struct : kern::Node<125> { Int index = 0; } n125;
  struct : kern::Node<126> { Int index = 0; } n126;
  struct : kern::Node<127> { Int index = 0; } n127;
  struct : kern::Node<128> { Int index = 0; } n128;
  struct : kern::Node<129> { Int index = 0; } n129;
  struct : kern::Node<130> { Int index = 0; } n130;
  struct : kern::Node<131> { Int index = 0; } n131;
};
