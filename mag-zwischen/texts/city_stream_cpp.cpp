struct city_stream_cpp : virtual kern::Library, virtual city_jobs_cpp, virtual city_stream, virtual common, virtual engine_geo_mesh_data, virtual engine_geo_roads, virtual engine_math_math, virtual engine_physics_physics, virtual terrain, virtual terrain_cpp, virtual tiles, virtual tiles_cpp {
  // ---- types ----
  KERN_STRUCT(CityStream_select_cells_Pick) { Float d = 0.0; U32 tile = 0; Int64 cell = Int64{0x0}; U32 n = 0; };
  KERN_STRUCT(CityStream_update_collision_Pick) { Float d2 = 0.0; Int64 tile = Int64{0x0}; U32 i = 0; };

  // ---- variables ----
  /// the building mesh chunks (BuildingOptions::chunk_size)
  KERN_CONST Float kChunk = 160.0;
  /// OpenFreeMap's planet tiles go up to z14 (overzoomed beyond)
  KERN_CONST I32 kTileZoom = 14;
  KERN_CONST U32 kTile = 1;
  /// The map features of a tile that is not loaded: none.
  KERN_CONST MapFeatures kNoFeatures = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
  /// 250 ms at the world's 240 Hz (world_building_collisions.mjs: 250 ms)
  KERN_CONST U32 kNearSteps = 60;
  /// buildings of a new set a step (480 in 30 steps, an eighth of a second)
  KERN_CONST U32 kNearPerStep = 16;
  /// vertex locations 14 and 15
  KERN_CONST U32 kMaxExtensionAttributes = 2;
  KERN_CONST U32 kBuildings = 2;
  KERN_CONST Fixed<Text, 9> kStateName = List<Text>{"requested", "decoding", "terrain_wait", "building", "roads", "areas", "collision", "ready", "failed"};

  // ---- functions ----
  struct CityStream_ground_heightResult { Float Result = 0.0; };
  /// Ground height: the terrain's static surface (elevation, water basins, craters).
  KERN_PURE CityStream_ground_heightResult CityStream_ground_height(const CityStream& self, Float x = 0.0, Float y = 0.0) {
    return {terrain_cpp::Terrain_height(self.terrain_, x, y).Result};
  }

  struct konstanz_style_hexResult { Rgb Result = Rgb{1.0, 1.0, 1.0}; };
  /// world_city_buildings.mjs WALLS / ROOFS (sRGB) and the cornice mix with #f4efe4 (0.45)
  KERN_PURE konstanz_style_hexResult konstanz_style_hex(U32 h = 0) {
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
  KERN_PURE road_palette_rgbResult road_palette_rgb(U32 h = 0) {
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
  KERN_PURE cell_keyResult cell_key(I32 x = 0, I32 y = 0) {
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
  void put_footprint(const MapFeatures_Building& b, List<Vector>& thin, Object<kern::FootprintStream> fs = Object<kern::FootprintStream>{}, Vector c = Vector{0.0, 0.0, 0.0}) {
    Object<kern::FootprintStream> fs_ = Object<kern::FootprintStream>{};
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

  void CityStream_remove_drop(CityStream& self, const List<U32>& list) {
    U32 id = 0;
    Bool remove = false;
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U32>(list))))); it12.next();) {
      id = list[n12.index];
      engine_renderer_remove(n13, self.r_, id);
      remove = n13.result;
    }
  }

  void remove_cancel(U32& job) {
    if (math_compare<"!=">(kern::to<Float>(job), 0.0)) {
      engine_jobs_cancel(job);
      job = 0;
    }
  }

  void CityStream_remove(CityStream& self, CityStream_Tile& t) {
    U32 id = 0;
    Bool remove = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    Bool body_valid_3 = false;
    Bool body_valid_4 = false;
    Bool both_2 = false;
    if (object_valid<kern::Renderer>(self.r_)) {
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
    if (object_valid<kern::Renderer>(self.r_)) {
      for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<U32>>(t.cell_old))))); it14.next();) {
        for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U32>(t.cell_old[n14.index]))))); it15.next();) {
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
      self.furniture_serial_ = kern::to<Int>(kern::formula([](Float furniture_serial_) -> Float { return (furniture_serial_ + 1.0); }, kern::to<Float>(self.furniture_serial_)));
    }
    terrain_cpp::Terrain_remove_tints(self.terrain_, tiles::TileKey_hash(t.key).Result);
    terrain_cpp::Terrain_remove_corridors(self.terrain_, tiles::TileKey_hash(t.key).Result);
    both = logic_and(object_valid<kern::physics::World>(self.world_), engine_physics_physics::Body_valid(t.body).Result);
    if (both) {
      body_valid_2 = engine_world_body_valid(self.world_, t.body);
      both = body_valid_2;
    }
    if (both) {
      engine_world_destroy_body(self.world_, t.body);
    }
    t.body = struct_make<physics_Body>(Int64{0x0});
    both_2 = logic_and(object_valid<kern::physics::World>(self.world_), engine_physics_physics::Body_valid(t.old_body).Result);
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
        if (object_valid<kern::RoadBuilder>(self.tiles_[at][0].road_builder)) {
          engine_road_builder_release(self.tiles_[at][0].road_builder);
        }
        if (object_valid<kern::BuildingBuilder>(self.tiles_[at][0].buildings)) {
          engine_building_builder_release(self.tiles_[at][0].buildings);
        }
        if (object_valid<kern::RoadStream>(self.tiles_[at][0].roads)) {
          engine_road_stream_release(self.tiles_[at][0].roads);
        }
        if (object_valid<kern::FootprintStream>(self.tiles_[at][0].footprints)) {
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
    if (object_valid<kern::physics::World>(self.world_)) {
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
    self.near_ = struct_make<CityStream_NearCollision>(struct_make<physics_Body>(Int64{0x0}), struct_make<physics_Body>(Int64{0x0}), Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, 1000000, List<Int64_Int_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0);
    map_clear(self.damage_);
    list_clear(self.damage_log_);
    self.vis_at_ = Vector{1000000000.0, 1000000000.0, 0.0};
    self.vis_ms_ = -1000000000.0;
  }

  void CityStream_start(CityStream& self, Object<kern::Renderer> renderer = Object<kern::Renderer>{}, Object<kern::physics::World> world = Object<kern::physics::World>{}, Float lon = 0.0, Float lat = 0.0, Bool dem = false) {
    Float now_ms = 0.0;
    CityStream_clear(self);
    self.r_ = renderer;
    self.world_ = world;
    now_ms = engine_now_ms();
    self.start_ms_ = now_ms;
    n20 = engine_geo_roads::GeoProjector_init(self.proj_, lon, lat);
    terrain_cpp::Terrain_start(self.terrain_, self.proj_, renderer, world, dem);
    self.origin_tile_ = tiles_cpp::tile_at(lon, lat, kTileZoom).Result;
    self.offline_ = false;
    self.real_tiles_ = 0;
    self.terrain_.corridors_expected = true;
  }

  struct request_around_matchResult { Bool Result = false; };
  KERN_PURE request_around_matchResult request_around_match(const Int_TileKey_pair& a, const Int_TileKey_pair& b) {
    return {math_compare<"<">(kern::to<Float>(a.first), kern::to<Float>(b.first))};
  }

  void CityStream_request_around(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, I32 ring = 0) {
    Float lon = 0.0;
    Float lat = 0.0;
    TileKey c = TileKey{14, 0, 0};
    List<Int_TileKey_pair> want = List<Int_TileKey_pair>{};
    Bool skip = false;
    List<CityStream_Tile> t = List<CityStream_Tile>{};
    CityStream_Tile made = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<U32>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<U32>{}, List<U32>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<U32>>{}, Set<Int64>{}, Map<Int64, List<U32>>{}, false, 0, 0, Map<Int64, List<U32>>{}, Set<Int64>{}, List<Int64>{}, 0, List<U8>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
    Object<kern::FootprintStream> footprint_stream = Object<kern::FootprintStream>{};
    Object<kern::RoadStream> road_stream = Object<kern::RoadStream>{};
    Object<kern::BuildingBuilder> building_builder = Object<kern::BuildingBuilder>{};
    Object<kern::RoadBuilder> road_builder = Object<kern::RoadBuilder>{};
    Float lo = 0.0;
    Float la = 0.0;
    Float x = 0.0;
    Float y = 0.0;
    Float x0 = 0.0;
    Float y0 = 0.0;
    Float x1 = 0.0;
    Float y1 = 0.0;
    Int64 key = Int64{0x0};
    List<CityStream_Tile> put = List<CityStream_Tile>{};
    if (self.offline_) {
      return;
    }
    lon = 0.0;
    lat = 0.0;
    tiles_cpp::unproject(self.proj_, lon, lat, vector_break(focus).x, vector_break(focus).y);
    c = tiles_cpp::tile_at(lon, lat, kTileZoom).Result;
    want = List<Int_TileKey_pair>{};
    for (auto it21 = flow_for(n21, kern::to<Int>(kern::formula([](Float ring) -> Float { return (-ring); }, kern::to<Float>(ring))), ring); it21.next();) {
      for (auto it22 = flow_for(n22, kern::to<Int>(kern::formula([](Float ring) -> Float { return (-ring); }, kern::to<Float>(ring))), ring); it22.next();) {
        list_add(want, struct_make<Int_TileKey_pair>(kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return ((index * index) + (index_2 * index_2)); }, kern::to<Float>(n22.index), kern::to<Float>(n21.index))), struct_make<TileKey>(c.z, kern::to<Int>(kern::formula([](Float x, Float index) -> Float { return (x + index); }, kern::to<Float>(c.x), kern::to<Float>(n22.index))), kern::to<Int>(kern::formula([](Float y, Float index) -> Float { return (y + index); }, kern::to<Float>(c.y), kern::to<Float>(n21.index))))));
      }
    }
    list_sort<&city_stream_cpp::request_around_match>(*this, want);
    for (auto it23 = flow_for(n23, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int_TileKey_pair>(want))))); it23.next();) {
      skip = false;
      if (logic_not(logic_not(map_contains<Int64, List<CityStream_Tile>>(self.tiles_, tiles::TileKey_hash(want[n23.index].second).Result)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        made = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<U32>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<U32>{}, List<U32>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<U32>>{}, Set<Int64>{}, Map<Int64, List<U32>>{}, false, 0, 0, Map<Int64, List<U32>>{}, Set<Int64>{}, List<Int64>{}, 0, List<U8>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
        engine_footprint_stream_make(n24);
        footprint_stream = n24.result;
        made.footprints = footprint_stream;
        engine_road_stream_make(n25);
        road_stream = n25.result;
        made.roads = road_stream;
        engine_building_builder_make(n26);
        building_builder = n26.result;
        made.buildings = building_builder;
        engine_road_builder_make(n27);
        road_builder = n27.result;
        made.road_builder = road_builder;
        t = list_make<CityStream_Tile, "1">(made);
        t[0].key = want[n23.index].second;
        lo = 0.0;
        la = 0.0;
        x = 0.0;
        y = 0.0;
        tiles_cpp::tile_lonlat(want[n23.index].second, lo, la, 0.5, 0.5);
        engine_geo_roads::GeoProjector_project(self.proj_, x, y, lo, la);
        t[0].center = vector_make(x, y, 0.0);
        x0 = 0.0;
        y0 = 0.0;
        x1 = 0.0;
        y1 = 0.0;
        tiles_cpp::tile_lonlat(want[n23.index].second, lo, la, 0.0, 1.0);
        engine_geo_roads::GeoProjector_project(self.proj_, x0, y0, lo, la);
        tiles_cpp::tile_lonlat(want[n23.index].second, lo, la, 1.0, 0.0);
        engine_geo_roads::GeoProjector_project(self.proj_, x1, y1, lo, la);
        t[0].bounds = engine_math_math::Vec4_new(x0, y0, x1, y1).Result;
        key = tiles::TileKey_hash(want[n23.index].second).Result;
        map_add(n28, self.tiles_, key);
        var_take(n29, t);
        put = n29.value;
        if (math_compare<">">(kern::to<Float>(list_length<CityStream_Tile>(self.tiles_[n28.index])), 0.0)) {
          if (object_valid<kern::RoadBuilder>(self.tiles_[n28.index][0].road_builder)) {
            engine_road_builder_release(self.tiles_[n28.index][0].road_builder);
          }
          if (object_valid<kern::BuildingBuilder>(self.tiles_[n28.index][0].buildings)) {
            engine_building_builder_release(self.tiles_[n28.index][0].buildings);
          }
          if (object_valid<kern::RoadStream>(self.tiles_[n28.index][0].roads)) {
            engine_road_stream_release(self.tiles_[n28.index][0].roads);
          }
          if (object_valid<kern::FootprintStream>(self.tiles_[n28.index][0].footprints)) {
            engine_footprint_stream_release(self.tiles_[n28.index][0].footprints);
          }
        }
        self.tiles_[n28.index] = put;
        list_add(self.requests_, want[n23.index].second);
        if (math_compare<">">(kern::to<Float>(list_length<CityStream_Tile>(t)), 0.0)) {
          if (object_valid<kern::RoadBuilder>(t[0].road_builder)) {
            engine_road_builder_release(t[0].road_builder);
          }
          if (object_valid<kern::BuildingBuilder>(t[0].buildings)) {
            engine_building_builder_release(t[0].buildings);
          }
          if (object_valid<kern::RoadStream>(t[0].roads)) {
            engine_road_stream_release(t[0].roads);
          }
          if (object_valid<kern::FootprintStream>(t[0].footprints)) {
            engine_footprint_stream_release(t[0].footprints);
          }
        }
      }
    }
  }

  struct CityStream_ring_boundsResult { Vec4 Result = Vec4{0.0, 0.0, 0.0, 0.0}; };
  CityStream_ring_boundsResult CityStream_ring_bounds(const CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, I32 r = 0) {
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
    I32 d = 0;
    Bool near_mate = false;
    Int at = 0;
    lon = 0.0;
    lat = 0.0;
    tiles_cpp::unproject(self.proj_, lon, lat, vector_break(focus).x, vector_break(focus).y);
    c = tiles_cpp::tile_at(lon, lat, kTileZoom).Result;
    mates = List<TileKey>{};
    for (auto it30 = flow_for(n30, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(self.extra_foci))))); it30.next();) {
      lo = 0.0;
      la = 0.0;
      tiles_cpp::unproject(self.proj_, lo, la, vector_break(self.extra_foci[n30.index]).x, vector_break(self.extra_foci[n30.index]).y);
      list_add(mates, tiles_cpp::tile_at(lo, la, kTileZoom).Result);
    }
    it = 0;
    while (loop(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_)))))) {
      d = kern::to<Int>(kern::formula([](Float a, Float x, Float a_2, Float y) -> Float { return kern::math::max(kern::math::abs((a - x)), kern::math::abs((a_2 - y))); }, kern::to<Float>(self.tiles_[it][0].key.x), kern::to<Float>(c.x), kern::to<Float>(self.tiles_[it][0].key.y), kern::to<Float>(c.y)));
      near_mate = false;
      for (auto it31 = flow_for(n31, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<TileKey>(mates))))); it31.next();) {
        near_mate = logic_or(near_mate, math_compare<"<=">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::max(kern::math::abs((a - a_2)), kern::math::abs((a_3 - a_4))); }, kern::to<Float>(self.tiles_[it][0].key.x), kern::to<Float>(mates[n31.index].x), kern::to<Float>(self.tiles_[it][0].key.y), kern::to<Float>(mates[n31.index].y))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float mate_ring) -> Float { return (mate_ring + 1.0); }, kern::to<Float>(self.mate_ring)))))));
      }
      if (logic_and(logic_and(logic_not(self.tiles_[it][0].procedural), math_compare<">">(kern::to<Float>(d), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float ring) -> Float { return (ring + 1.0); }, kern::to<Float>(self.ring))))))), logic_not(near_mate))) {
        CityStream_remove(self, self.tiles_[it][0]);
        at = it;
        if (math_compare<">">(kern::to<Float>(list_length<CityStream_Tile>(self.tiles_[at])), 0.0)) {
          if (object_valid<kern::RoadBuilder>(self.tiles_[at][0].road_builder)) {
            engine_road_builder_release(self.tiles_[at][0].road_builder);
          }
          if (object_valid<kern::BuildingBuilder>(self.tiles_[at][0].buildings)) {
            engine_building_builder_release(self.tiles_[at][0].buildings);
          }
          if (object_valid<kern::RoadStream>(self.tiles_[at][0].roads)) {
            engine_road_stream_release(self.tiles_[at][0].roads);
          }
          if (object_valid<kern::FootprintStream>(self.tiles_[at][0].footprints)) {
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
    for (auto it32 = flow_for(n32, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TileKey>(self.requests_))))); it32.next();) {
      b = "";
      b = text_format<"%s[%d,%d,%d]">(63, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n32.index), 0.0), ",", ""), self.requests_[n32.index].z, self.requests_[n32.index].x, self.requests_[n32.index].y);
      s = text_append(s, b, "");
    }
    if (logic_and(logic_not(math_compare<"==">(kern::to<Float>(list_length<TileKey>(self.requests_)), 0.0)), math_compare<"<">(self.ms_.t_request, 0.0))) {
      now_ms = engine_now_ms();
      self.ms_.t_request = kern::formula([](Float now_ms, Float start_ms_) -> Float { return (now_ms - start_ms_); }, now_ms, self.start_ms_);
    }
    list_clear(self.requests_);
    return {text_append(s, "]", "")};
  }

  void CityStream_provide(CityStream& self, const TileKey& key, List<U8> bytes = List<U8>{}) {
    Int it = 0;
    Float now_ms = 0.0;
    Int arg = 0;
    Int submit = 0;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<CityStream_Tile>>(self.tiles_, tiles::TileKey_hash(key).Result).index), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))));
    if (logic_or(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))), logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[it][0].state, CityStream_Tile_State::requested)))) {
      return;
    }
    if (logic_and(tiles::TileKey_equal(key, self.origin_tile_).Result, math_compare<"<">(self.ms_.t_data, 0.0))) {
      now_ms = engine_now_ms();
      self.ms_.t_data = kern::formula([](Float now_ms, Float start_ms_) -> Float { return (now_ms - start_ms_); }, now_ms, self.start_ms_);
    }
    arg = kTile;
    n33 = city_jobs_cpp::tile_input(key, self.proj_.lon0, self.proj_.lat0, bytes);
    engine_jobs_submit(n34, arg, n33.Result);
    submit = n34.result;
    self.tiles_[it][0].tile_job = submit;
    self.tiles_[it][0].state = CityStream_Tile_State::decoding;
  }

  void CityStream_feed_terrain(CityStream& self, CityStream_Tile& t) {
    Int i = 0;
    for (auto it41 = flow_for(n41, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Area>(t.features.areas))))); it41.next();) {
      if (enum_equal<AreaKind>(t.features.areas[n41.index].kind, AreaKind::water)) {
        terrain_cpp::Terrain_add_water_polygon(self.terrain_, t.features.areas[n41.index].rings);
      } else {
        terrain_cpp::Terrain_add_tint(self.terrain_, t.features.areas[n41.index].rings, tiles::TileKey_hash(t.key).Result, area_color(t.features.areas[n41.index].kind).Result);
      }
    }
    for (auto it42 = flow_for(n42, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Road>(t.features.roads))))); it42.next();) {
      if (tiles_cpp::road_is_water(t.features.roads[n42.index].cls).Result) {
        terrain_cpp::Terrain_add_waterway(self.terrain_, t.features.roads[n42.index].points, tiles_cpp::road_width(t.features.roads[n42.index].cls).Result, kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? 1.6 : ((a_2 != 0.0) ? 0.5 : 1.1)); }, kern::to<Float>(enum_equal<RoadClass>(t.features.roads[n42.index].cls, RoadClass::river)), kern::to<Float>(enum_equal<RoadClass>(t.features.roads[n42.index].cls, RoadClass::canal))));
      } else if (t.features.roads[n42.index].bridge) {
        i = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(t.features.roads[n42.index].points))), 0.5))) {
          terrain_cpp::Terrain_add_bridge(self.terrain_, t.features.roads[n42.index].points[i], t.features.roads[n42.index].points[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))], tiles_cpp::road_width(t.features.roads[n42.index].cls).Result);
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
    U32 id = 0;
    Bool remove = false;
    Int gone = 0;
    Int at_2 = 0;
    Int64 key = Int64{0x0};
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<U32>>(t.cell_ids, cell).index), kern::to<Float>(map_length<Int64, List<U32>>(t.cell_ids))));
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<U32>>(t.cell_ids)))) {
      return;
    }
    at = it;
    for (auto it55 = flow_for(n55, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U32>(t.cell_ids[at]))))); it55.next();) {
      id = t.cell_ids[at][n55.index];
      engine_renderer_remove(n56, self.r_, id);
      remove = n56.result;
      gone = id;
      while (loop(list_contains<U32>(t.meshes, gone))) {
        list_remove(t.meshes, gone);
      }
    }
    at_2 = it;
    map_remove_at(t.cell_ids, at_2);
    key = cell;
    set_remove(n57, t.dirty_cells, key);
  }

  void CityStream_select_cells_settle(CityStream& self, CityStream_Tile& t, const CityStream_select_cells_Pick& p, Float reach = 0.0, Float kKeep = 0.0) {
    Bool want = false;
    Bool built = false;
    Int64 key = Int64{0x0};
    Int64 key_2 = Int64{0x0};
    want = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(set_contains<Int64>(t.want_cells, p.cell)))), 0.0);
    built = math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(map_contains<Int64, List<U32>>(t.cell_ids, p.cell)))), 0.0);
    if (logic_and(want, logic_not(built))) {
      key = p.cell;
      set_add(n53, t.dirty_cells, key);
    }
    if (logic_and(logic_not(want), logic_not(built))) {
      key_2 = p.cell;
      set_remove(n54, t.dirty_cells, key_2);
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
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<U32>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<U32>{}, List<U32>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<U32>>{}, Set<Int64>{}, Map<Int64, List<U32>>{}, false, 0, 0, Map<Int64, List<U32>>{}, Set<Int64>{}, List<Int64>{}, 0, List<U8>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
    I32 cx = 0;
    I32 cy = 0;
    Float d = 0.0;
    U32 total = 0;
    Float reach = 0.0;
    Int at = 0;
    Int count = 0;
    Bool stop = false;
    Int64 key = Int64{0x0};
    Bool skip_2 = false;
    List<Int64> far = List<Int64>{};
    Bool skip_3 = false;
    I32 cx_2 = 0;
    I32 cy_2 = 0;
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
        for (auto it50 = flow_for(n50, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<U32>>(t.cell_items))))); it50.next();) {
          cx = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(map_key_at<Int64, List<U32>>(t.cell_items, n50.index), 32)))));
          cy = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(int64_low(map_key_at<Int64, List<U32>>(t.cell_items, n50.index)))));
          d = select_cells_way_distance(ab2, a, ab, vector_make(kern::formula([](Float cx, Float kChunk) -> Float { return ((cx + 0.5) * kChunk); }, kern::to<Float>(cx), kChunk), kern::formula([](Float cy, Float kChunk) -> Float { return ((cy + 0.5) * kChunk); }, kern::to<Float>(cy), kChunk), 0.0)).Result;
          if (math_compare<"<=">(d, kern::formula([](Float kR, Float kKeep) -> Float { return (kR + kKeep); }, kR, kKeep))) {
            list_add(picks, struct_make<CityStream_select_cells_Pick>(d, kern::to<Int>(kern::formula([](Float it) -> Float { return kern::math::wrap32(it); }, kern::to<Float>(it))), map_key_at<Int64, List<U32>>(t.cell_items, n50.index), kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<U32>(t.cell_items[n50.index]))))));
          }
        }
      }
      it += 1;
    }
    list_sort<&city_stream_cpp::select_cells_match>(*this, picks);
    for (auto it51 = flow_for(n51, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it51.next();) {
      set_clear(self.tiles_[n51.index][0].want_cells);
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
          set_add(n52, self.tiles_[picks[at].tile][0].want_cells, key);
          total += picks[at].n;
        }
      }
      if (logic_not(stop)) {
        at += 1;
      }
    }
    for (auto it58 = flow_for(n58, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CityStream_select_cells_Pick>(picks))))); it58.next();) {
      CityStream_select_cells_settle(self, self.tiles_[picks[n58.index].tile][0], picks[n58.index], reach, kKeep);
    }
    for (auto it59 = flow_for(n59, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it59.next();) {
      skip_2 = false;
      if (logic_or(math_compare<"!=">(kern::to<Float>(self.tiles_[n59.index][0].cell_job), 0.0), self.tiles_[n59.index][0].cell_uploading)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        far = List<Int64>{};
        for (auto it60 = flow_for(n60, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<U32>>(self.tiles_[n59.index][0].cell_ids))))); it60.next();) {
          skip_3 = false;
          if (math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(set_contains<Int64>(self.tiles_[n59.index][0].want_cells, map_key_at<Int64, List<U32>>(self.tiles_[n59.index][0].cell_ids, n60.index))))), 0.0)) {
            skip_3 = true;
          }
          if (logic_not(skip_3)) {
            cx_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(map_key_at<Int64, List<U32>>(self.tiles_[n59.index][0].cell_ids, n60.index), 32)))));
            cy_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(int64_low(map_key_at<Int64, List<U32>>(self.tiles_[n59.index][0].cell_ids, n60.index)))));
            if (math_compare<">">(select_cells_way_distance(ab2, a, ab, vector_make(kern::formula([](Float cx_2, Float kChunk) -> Float { return ((cx_2 + 0.5) * kChunk); }, kern::to<Float>(cx_2), kChunk), kern::formula([](Float cy_2, Float kChunk) -> Float { return ((cy_2 + 0.5) * kChunk); }, kern::to<Float>(cy_2), kChunk), 0.0)).Result, kern::formula([](Float kR, Float kKeep) -> Float { return (kR + kKeep); }, kR, kKeep))) {
              list_add(far, map_key_at<Int64, List<U32>>(self.tiles_[n59.index][0].cell_ids, n60.index));
            }
          }
        }
        for (auto it61 = flow_for(n61, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int64>(far))))); it61.next();) {
          c = far[n61.index];
          CityStream_drop_cell(self, self.tiles_[n59.index][0], c);
        }
      }
    }
  }

  struct CityStream_begin_roads_callbackResult { Bool Result = false; };
  /// over a crater the ribbon is draped finely (0.9 m quads): it lies in the bowl, not over it
  KERN_PURE CityStream_begin_roads_callbackResult CityStream_begin_roads_callback(const CityStream& self, Float x = 0.0, Float y = 0.0, Float r = 0.0) {
    return {terrain::Terrain_deformed(self.terrain_, x, y, r).Result};
  }

  void CityStream_begin_roads(CityStream& self, CityStream_Tile& t) {
    RoadOptions ro = RoadOptions{0.0, 0.0, 800.0, 420.0, 6.0, 3.4, 0.09, 0.112, 0.135, 0.42, 1.0, 3.0, 7.0, 0.2, Rgb{0.525, 0.514, 0.486}, List<Rgb>{}, Callback<Bool(Float, Float, Float)>{}, 0.9};
    Vector mid = Vector{0.0, 0.0, 0.0};
    List<Float> data = List<Float>{};
    Int size = 0;
    Bool begin = false;
    ro = RoadOptions{0.0, 0.0, 800.0, 420.0, 6.0, 3.4, 0.09, 0.112, 0.135, 0.42, 1.0, 3.0, 7.0, 0.2, Rgb{0.525, 0.514, 0.486}, List<Rgb>{}, Callback<Bool(Float, Float, Float)>{}, 0.9};
    mid = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x + (x_2 * 0.5)); }, vector_break(self.focus_).x, vector_break(self.view_lead).x), kern::formula([](Float y, Float y_2) -> Float { return (y + (y_2 * 0.5)); }, vector_break(self.focus_).y, vector_break(self.view_lead).y), 0.0);
    ro.center_x = vector_break(mid).x;
    ro.center_y = vector_break(mid).y;
    ro.radius = kern::formula([](Float x, Float y) -> Float { return (720.0 + (kern::math::sqrt((x * x) + (y * y)) * 0.5)); }, vector_break(self.view_lead).x, vector_break(self.view_lead).y);
    ro.segment_length = 4.0;
    ro.palette = road_palette().Result;
    ro.deformed = callback<&city_stream_cpp::CityStream_begin_roads_callback>(self);
    data = engine_road_stream_data(t.roads);
    size = engine_road_stream_size(t.roads);
    engine_road_builder_begin(n62, t.road_builder, data, kern::to<Int>(kern::formula([](Float size) -> Float { return kern::math::wrap32(size); }, kern::to<Float>(size))), ro, terrain_cpp::Terrain_sampler(self.terrain_, 0.0).Result);
    begin = n62.result;
    t.roads_at = mid;
    self.relays_ = kern::to<Int>(kern::formula([](Float relays_) -> Float { return (relays_ + 1.0); }, kern::to<Float>(self.relays_)));
    t.upload_next = 0;
    t.state = CityStream_Tile_State::roads;
  }

  void CityStream_finish_features(CityStream& self, CityStream_Tile& t) {
    List<Float> widths = List<Float>{};
    List<Float> made = List<Float>{};
    Int i = 0;
    MapFeatures_Road r = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    Float g = 0.0;
    Int d = 0;
    Vector c = Vector{0.0, 0.0, 0.0};
    Vector q = Vector{0.0, 0.0, 0.0};
    Bool skip = false;
    Float w = 0.0;
    Bool water = false;
    Bool major = false;
    U32 color = 0;
    List<Vector_row> list = List<Vector_row>{};
    Float len = 0.0;
    Int k = 0;
    U32 i_2 = 0;
    List<U32> t_2 = List<U32>{};
    Int64 key = Int64{0x0};
    List<U32> put = List<U32>{};
    made = List<Float>{};
    list_resize(made, list_length<MapFeatures_Road>(t.features.roads), 0.0);
    widths = made;
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Float>(widths))), 0.5))) {
      r = t.features.roads[i];
      if (logic_and(logic_and(logic_not(tiles_cpp::road_is_water(r.cls).Result), logic_not(enum_equal<RoadClass>(r.cls, RoadClass::path))), logic_not(enum_equal<RoadClass>(r.cls, RoadClass::track)))) {
        widths[i] = kern::formula([](Float the_result) -> Float { return (the_result + 3.4); }, tiles_cpp::road_width(r.cls).Result);
      }
      i += 1;
    }
    terrain_cpp::Terrain_set_corridors(self.terrain_, t.bounds, t.features.roads, widths, tiles::TileKey_hash(t.key).Result);
    list_clear(t.cent);
    for (auto it43 = flow_for(n43, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(t.features.buildings))))); it43.next();) {
      if (logic_not(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(t.features.buildings[n43.index].rings)), 0.0))) {
        g = terrain_cpp::Terrain_footprint_base(self.terrain_, t.features.buildings[n43.index].rings[0].items).Result;
        t.features.buildings[n43.index].base = kern::formula([](Float a, Float g) -> Float { return (a + g); }, t.features.buildings[n43.index].base, g);
        t.features.buildings[n43.index].top = kern::formula([](Float a, Float g) -> Float { return (a + g); }, t.features.buildings[n43.index].top, g);
      }
      t.features.buildings[n43.index].full_top = t.features.buildings[n43.index].top;
      d = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, CityStream_Damage>(self.damage_, t.features.buildings[n43.index].seed).index), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))));
      if (logic_not(math_compare<"==">(kern::to<Float>(d), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))))) {
        t.features.buildings[n43.index].top = kern::formula([](Float a, Float a_2, Float a_3) -> Float { return kern::math::min(a, (a_2 + a_3)); }, t.features.buildings[n43.index].top, t.features.buildings[n43.index].base, self.damage_[d].height);
        t.features.buildings[n43.index].leveled = self.damage_[d].leveled;
      }
      c = Vector{0.0, 0.0, 0.0};
      if (logic_and(logic_not(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(t.features.buildings[n43.index].rings)), 0.0)), logic_not(math_compare<"==">(kern::to<Float>(list_length<Vector>(t.features.buildings[n43.index].rings[0].items)), 0.0)))) {
        for (auto it44 = flow_for(n44, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(t.features.buildings[n43.index].rings[0].items))))); it44.next();) {
          q = t.features.buildings[n43.index].rings[0].items[n44.index];
          c = vector_add(c, q);
        }
        c = vector_scale(c, kern::formula([](Float a) -> Float { return kern::math::div(1.0, (a < 0.0 ? a + 18446744073709551616.0 : a)); }, kern::to<Float>(list_length<Vector>(t.features.buildings[n43.index].rings[0].items))));
      }
      list_add(t.cent, c);
    }
    t.prepared = true;
    self.furniture_serial_ = kern::to<Int>(kern::formula([](Float furniture_serial_) -> Float { return (furniture_serial_ + 1.0); }, kern::to<Float>(self.furniture_serial_)));
    for (auto it45 = flow_for(n45, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Road>(t.features.roads))))); it45.next();) {
      skip = false;
      w = tiles_cpp::road_width(t.features.roads[n45.index].cls).Result;
      water = tiles_cpp::road_is_water(t.features.roads[n45.index].cls).Result;
      if (water) {
        skip = true;
      }
      if (logic_not(skip)) {
        major = math_compare<"<=">(kern::to<Float>(enum_index<RoadClass>(t.features.roads[n45.index].cls)), kern::to<Float>(enum_index<RoadClass>(RoadClass::tertiary)));
        color = kern::to<Int>(kern::formula([](Float water, Float a, Float a_2, Float a_3, Float major) -> Float { return ((water != 0.0) ? 4.0 : ((a != 0.0) ? 5.0 : (((a_2 != 0.0) || (a_3 != 0.0)) ? 3.0 : ((major != 0.0) ? 1.0 : 0.0)))); }, kern::to<Float>(water), kern::to<Float>(enum_equal<RoadClass>(t.features.roads[n45.index].cls, RoadClass::pedestrian)), kern::to<Float>(enum_equal<RoadClass>(t.features.roads[n45.index].cls, RoadClass::path)), kern::to<Float>(enum_equal<RoadClass>(t.features.roads[n45.index].cls, RoadClass::track)), kern::to<Float>(major)));
        engine_road_stream_road(t.roads, t.features.roads[n45.index].points, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(t.features.roads[n45.index].points)))), w, logic_and(logic_not(water), tiles_cpp::road_drivable(t.features.roads[n45.index].cls).Result), color, kern::to<Int>(kern::formula([](Float major, Float water) -> Float { return (((major != 0.0) && (!(water != 0.0))) ? 2.0 : (-1.0)); }, kern::to<Float>(major), kern::to<Float>(water))));
        if (logic_not(tiles_cpp::road_drivable(t.features.roads[n45.index].cls).Result)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (t.procedural) {
            list_add(self.lanes_, struct_make<Lane>(t.features.roads[n45.index].points, w, tiles::TileKey_hash(t.key).Result, true));
            skip = true;
          }
          if (logic_not(skip)) {
            n46 = tiles_cpp::clip_line(t.features.roads[n45.index].points, t.bounds);
            list = n46.Result;
            for (auto it47 = flow_for(n47, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(list))))); it47.next();) {
              len = 0.0;
              k = 0;
              while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(list[n47.index]).items))), 0.5))) {
                len += kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(list[n47.index].items[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))]).x, vector_break(list[n47.index].items[k]).x, vector_break(list[n47.index].items[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))]).y, vector_break(list[n47.index].items[k]).y);
                k += 1;
              }
              if (math_compare<">">(len, 0.5)) {
                var_take(n48, list[n47.index].items);
                list_add(self.lanes_, struct_make<Lane>(n48.value, w, tiles::TileKey_hash(t.key).Result, true));
              }
            }
          }
        }
      }
    }
    map_clear(t.cell_items);
    i_2 = 0;
    while (loop(math_compare<">">(kern::formula([](Float i_2, Float a) -> Float { return ((i_2 < 0.0) != (a < 0.0) ? (a < 0.0) : (i_2 < a)); }, kern::to<Float>(i_2), kern::to<Float>(list_length<Vector>(t.cent))), 0.5))) {
      key = cell_of(t.cent[i_2]).Result;
      map_add(n49, t.cell_items, key);
      t_2 = t.cell_items[n49.index];
      list_add(t_2, i_2);
      put = t_2;
      t.cell_items[n49.index] = put;
      i_2 += 1;
    }
    CityStream_select_cells(self, self.focus_, true);
    CityStream_begin_roads(self, t);
  }

  void CityStream_go_offline(CityStream& self) {
    Int count = 0;
    Int at = 0;
    List<CityStream_Tile> t = List<CityStream_Tile>{};
    CityStream_Tile made = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<U32>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<U32>{}, List<U32>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<U32>>{}, Set<Int64>{}, Map<Int64, List<U32>>{}, false, 0, 0, Map<Int64, List<U32>>{}, Set<Int64>{}, List<Int64>{}, 0, List<U8>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
    Object<kern::FootprintStream> footprint_stream = Object<kern::FootprintStream>{};
    Object<kern::RoadStream> road_stream = Object<kern::RoadStream>{};
    Object<kern::BuildingBuilder> building_builder = Object<kern::BuildingBuilder>{};
    Object<kern::RoadBuilder> road_builder = Object<kern::RoadBuilder>{};
    List<Vector_row> a = List<Vector_row>{};
    Int64 key = Int64{0x0};
    List<CityStream_Tile> put = List<CityStream_Tile>{};
    if (self.offline_) {
      return;
    }
    self.offline_ = true;
    for (auto it35 = flow_for(n35, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it35.next();) {
      CityStream_remove(self, self.tiles_[n35.index][0]);
    }
    count = map_length<Int64, List<CityStream_Tile>>(self.tiles_);
    for (auto it36 = flow_for(n36, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(count)))); it36.next();) {
      at = kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(count))))), kern::to<Float>(n36.index)));
      if (math_compare<">">(kern::to<Float>(list_length<CityStream_Tile>(self.tiles_[at])), 0.0)) {
        if (object_valid<kern::RoadBuilder>(self.tiles_[at][0].road_builder)) {
          engine_road_builder_release(self.tiles_[at][0].road_builder);
        }
        if (object_valid<kern::BuildingBuilder>(self.tiles_[at][0].buildings)) {
          engine_building_builder_release(self.tiles_[at][0].buildings);
        }
        if (object_valid<kern::RoadStream>(self.tiles_[at][0].roads)) {
          engine_road_stream_release(self.tiles_[at][0].roads);
        }
        if (object_valid<kern::FootprintStream>(self.tiles_[at][0].footprints)) {
          engine_footprint_stream_release(self.tiles_[at][0].footprints);
        }
      }
    }
    map_clear(self.tiles_);
    list_clear(self.requests_);
    list_clear(self.lanes_);
    terrain_cpp::Terrain_clear(self.terrain_);
    terrain_cpp::Terrain_disable_dem(self.terrain_);
    made = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<U32>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<U32>{}, List<U32>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<U32>>{}, Set<Int64>{}, Map<Int64, List<U32>>{}, false, 0, 0, Map<Int64, List<U32>>{}, Set<Int64>{}, List<Int64>{}, 0, List<U8>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
    engine_footprint_stream_make(n37);
    footprint_stream = n37.result;
    made.footprints = footprint_stream;
    engine_road_stream_make(n38);
    road_stream = n38.result;
    made.roads = road_stream;
    engine_building_builder_make(n39);
    building_builder = n39.result;
    made.buildings = building_builder;
    engine_road_builder_make(n40);
    road_builder = n40.result;
    made.road_builder = road_builder;
    t = list_make<CityStream_Tile, "1">(made);
    t[0].procedural = true;
    t[0].key = struct_make<TileKey>(0, -1, -1);
    t[0].bounds = engine_math_math::Vec4_new(-1000000000.0, -1000000000.0, 1000000000.0, 1000000000.0).Result;
    if (self.training_offline) {
      tiles_cpp::training_level(t[0].features);
    } else {
      tiles_cpp::procedural_city(t[0].features, 0.0, 0.0, 760.0, 20261029);
    }
    a = List<Vector_row>{};
    tiles_cpp::furnish(t[0].features, t[0].bounds, a);
    CityStream_feed_terrain(self, t[0]);
    t[0].state = CityStream_Tile_State::building;
    CityStream_finish_features(self, t[0]);
    key = tiles::TileKey_hash(t[0].key).Result;
    map_add(n63, self.tiles_, key);
    var_take(n64, t);
    put = n64.value;
    if (math_compare<">">(kern::to<Float>(list_length<CityStream_Tile>(self.tiles_[n63.index])), 0.0)) {
      if (object_valid<kern::RoadBuilder>(self.tiles_[n63.index][0].road_builder)) {
        engine_road_builder_release(self.tiles_[n63.index][0].road_builder);
      }
      if (object_valid<kern::BuildingBuilder>(self.tiles_[n63.index][0].buildings)) {
        engine_building_builder_release(self.tiles_[n63.index][0].buildings);
      }
      if (object_valid<kern::RoadStream>(self.tiles_[n63.index][0].roads)) {
        engine_road_stream_release(self.tiles_[n63.index][0].roads);
      }
      if (object_valid<kern::FootprintStream>(self.tiles_[n63.index][0].footprints)) {
        engine_footprint_stream_release(self.tiles_[n63.index][0].footprints);
      }
    }
    self.tiles_[n63.index] = put;
    if (math_compare<">">(kern::to<Float>(list_length<CityStream_Tile>(t)), 0.0)) {
      if (object_valid<kern::RoadBuilder>(t[0].road_builder)) {
        engine_road_builder_release(t[0].road_builder);
      }
      if (object_valid<kern::BuildingBuilder>(t[0].buildings)) {
        engine_building_builder_release(t[0].buildings);
      }
      if (object_valid<kern::RoadStream>(t[0].roads)) {
        engine_road_stream_release(t[0].roads);
      }
      if (object_valid<kern::FootprintStream>(t[0].footprints)) {
        engine_footprint_stream_release(t[0].footprints);
      }
    }
  }

  void CityStream_fail(CityStream& self, const TileKey& key) {
    Int it = 0;
    Float now_ms = 0.0;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<CityStream_Tile>>(self.tiles_, tiles::TileKey_hash(key).Result).index), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))));
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_)))) {
      return;
    }
    self.tiles_[it][0].state = CityStream_Tile_State::failed;
    self.tiles_[it][0].tries = kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 1.0); }, kern::to<Float>(self.tiles_[it][0].tries)));
    now_ms = engine_now_ms();
    self.tiles_[it][0].retry_ms = kern::formula([](Float now_ms, Float a) -> Float { return (now_ms + kern::math::min(20000.0, (2000.0 * a))); }, now_ms, kern::to<Float>(self.tiles_[it][0].tries));
    if (logic_and(logic_and(tiles::TileKey_equal(key, self.origin_tile_).Result, math_compare<"==">(kern::to<Float>(self.real_tiles_), 0.0)), math_compare<">=">(kern::to<Float>(self.tiles_[it][0].tries), 3.0))) {
      CityStream_go_offline(self);
    }
  }

  void CityStream_furniture(const CityStream& self, List<Int64>& out) {
    list_clear(out);
    for (auto it65 = flow_for(n65, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it65.next();) {
      if (self.tiles_[n65.index][0].prepared) {
        list_add(out, map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n65.index));
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

  struct CityStream_loaded_atResult { Bool Result = false; };
  KERN_PURE CityStream_loaded_atResult CityStream_loaded_at(const CityStream& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    if (logic_not(terrain::Terrain_settled(self.terrain_).Result)) {
      return {false};
    }
    if (self.offline_) {
      for (auto it66 = flow_for(n66, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it66.next();) {
        if (self.tiles_[n66.index][0].procedural) {
          return {enum_equal<CityStream_Tile_State>(self.tiles_[n66.index][0].state, CityStream_Tile_State::ready)};
        }
      }
      return {false};
    }
    return {terrain_cpp::Terrain_corridors_at(self.terrain_, vector_break(p).x, vector_break(p).y).Result};
  }

  struct CityStream_ready_atResult { Bool Result = false; };
  KERN_PURE CityStream_ready_atResult CityStream_ready_at(const CityStream& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    Bool solid = false;
    if (logic_or(logic_not(CityStream_loaded_at(self, p).Result), logic_not(terrain_cpp::Terrain_ready_at(self.terrain_, p).Result))) {
      return {false};
    }
    solid = logic_and(logic_and(logic_and(self.near_.built, logic_not(self.near_.building)), logic_not(self.near_.dirty)), math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.near_.at).x, vector_break(p).x, vector_break(self.near_.at).y, vector_break(p).y), 140.0));
    return {logic_or(logic_not(self.offline_), solid)};
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
    Object<kern::Earcut> earcut = Object<kern::Earcut>{};
    Object<kern::Earcut> earcut_2 = Object<kern::Earcut>{};
    List<Float> xy = List<Float>{};
    List<U32> holes = List<U32>{};
    List<U32> tris = List<U32>{};
    List<U32> idx = List<U32>{};
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
    U32 base = 0;
    Int place_2 = 0;
    U32 item_3 = 0;
    U32 item_4 = 0;
    U32 item_5 = 0;
    U32 item_6 = 0;
    U32 item_7 = 0;
    U32 item_8 = 0;
    U32 base_2 = 0;
    Int i = 0;
    U32 a_2 = 0;
    U32 c = 0;
    U32 d = 0;
    Int swapped = 0;
    Int place_3 = 0;
    U32 item_9 = 0;
    U32 item_10 = 0;
    U32 item_11 = 0;
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    physics_Shape returned = physics_Shape{Int64{0x0}};
    physics_Shape returned_2 = physics_Shape{Int64{0x0}};
    engine_earcut_make(n72);
    earcut_2 = n72.result;
    earcut = earcut_2;
    xy = List<Float>{};
    holes = List<U32>{};
    tris = List<U32>{};
    idx = List<U32>{};
    pts = List<Vector>{};
    list_clear(xy);
    list_clear(holes);
    list_clear(tris);
    list_clear(idx);
    list_clear(pts);
    rings = b.rings;
    for (auto it73 = flow_for(n73, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(rings))))); it73.next();) {
      if (math_compare<">">(kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(rings[n73.index]).items)), 64.0)) {
        step = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div((a + 63.0), 64.0)); }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(rings[n73.index]).items))));
        thin = List<Vector>{};
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float k, Float a) -> Float { return ((k < 0.0) != (a < 0.0) ? (a < 0.0) : (k < a)); }, kern::to<Float>(k), kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(rings[n73.index]).items))), 0.5))) {
          list_add(thin, rings[n73.index].items[k]);
          k += step;
        }
        rings[n73.index].items = thin;
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
          for (auto it74 = flow_for(n74, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(rings[at].items))))); it74.next();) {
            p = rings[at].items[n74.index];
            place = list_length<Float>(xy);
            item = vector_break(p).x;
            item_2 = vector_break(p).y;
            list_insert_list(xy, place, list_make<Float, "2">(item, item_2));
          }
          for (auto it75 = flow_for(n75, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(rings[r].items))))); it75.next();) {
            p0 = rings[r].items[n75.index];
            p1 = rings[r].items[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n75.index), kern::to<Float>(list_length<Vector>(rings[r].items))))];
            base = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(pts))));
            list_add(pts, vector_make(vector_break(p0).x, vector_break(p0).y, b.base));
            list_add(pts, vector_make(vector_break(p1).x, vector_break(p1).y, b.base));
            list_add(pts, vector_make(vector_break(p1).x, vector_break(p1).y, b.top));
            list_add(pts, vector_make(vector_break(p0).x, vector_break(p0).y, b.top));
            place_2 = list_length<U32>(idx);
            item_3 = base;
            item_4 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 1.0); }, kern::to<Float>(base)));
            item_5 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 2.0); }, kern::to<Float>(base)));
            item_6 = base;
            item_7 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 2.0); }, kern::to<Float>(base)));
            item_8 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 3.0); }, kern::to<Float>(base)));
            list_insert_list(idx, place_2, list_make<U32, "6">(item_3, item_4, item_5, item_6, item_7, item_8));
          }
        }
      }
      r += 1;
    }
    if (math_compare<">=">(kern::to<Float>(list_length<Float>(xy)), 6.0)) {
      engine_earcut_triangulate(n76, earcut, xy, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::trunc(kern::math::div(a, 2.0))); }, kern::to<Float>(list_length<Float>(xy)))), holes, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<U32>(holes)))), tris);
      tris = n76.out;
      base_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(pts))));
      for (auto it77 = flow_for(n77, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::trunc(kern::math::div(a, 2.0)); }, kern::to<Float>(list_length<Float>(xy))))))))); it77.next();) {
        list_add(pts, vector_make(xy[kern::to<Int>(kern::formula([](Float index) -> Float { return (index * 2.0); }, kern::to<Float>(n77.index)))], xy[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index * 2.0) + 1.0); }, kern::to<Float>(n77.index)))], b.top));
      }
      i = 0;
      while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<U32>(tris))), 0.5))) {
        a_2 = kern::to<Int>(kern::formula([](Float base_2, Float a) -> Float { return (base_2 + a); }, kern::to<Float>(base_2), kern::to<Float>(tris[i])));
        c = kern::to<Int>(kern::formula([](Float base_2, Float a) -> Float { return (base_2 + a); }, kern::to<Float>(base_2), kern::to<Float>(tris[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))])));
        d = kern::to<Int>(kern::formula([](Float base_2, Float a) -> Float { return (base_2 + a); }, kern::to<Float>(base_2), kern::to<Float>(tris[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 2.0); }, kern::to<Float>(i)))])));
        if (math_compare<"<">(vector_break(vector_cross(vector_sub(pts[c], pts[a_2]), vector_sub(pts[d], pts[a_2]))).z, 0.0)) {
          swapped = c;
          c = d;
          d = swapped;
        }
        place_3 = list_length<U32>(idx);
        item_9 = a_2;
        item_10 = c;
        item_11 = d;
        list_insert_list(idx, place_3, list_make<U32, "3">(item_9, item_10, item_11));
        i += 3;
      }
    }
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<U32>(idx)), 0.0))) {
      ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<U32>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
      ss.kind = physics_ShapeKind::mesh;
      ss.points = pts;
      ss.indices = idx;
      ss.material.friction = 0.7;
      ss.material.category_bits = Int64{0x1};
      engine_world_create_shape(n78, self.world_, body, ss);
      create_shape = n78.result;
      returned = create_shape;
      if (object_valid<kern::Earcut>(earcut)) {
        engine_earcut_release(earcut);
      }
      earcut = Object<kern::Earcut>{};
      return {returned};
    }
    returned_2 = struct_make<physics_Shape>(Int64{0x0});
    if (object_valid<kern::Earcut>(earcut)) {
      engine_earcut_release(earcut);
    }
    earcut = Object<kern::Earcut>{};
    return {returned_2};
  }

  struct CityStream_build_nearResult { Bool Result = false; };
  CityStream_build_nearResult CityStream_build_near(CityStream& self, Bool whole = false) {
    U32 built = 0;
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body create_body = physics_Body{Int64{0x0}};
    Int count = 0;
    physics_Shape fill = physics_Shape{Int64{0x0}};
    Bool skip = false;
    Int slot = 0;
    Int was = 0;
    Int64_Int_pair v = Int64_Int_pair{Int64{0x0}, 0};
    Int it = 0;
    Bool both = false;
    built = 0;
    if (logic_not(engine_physics_physics::Body_valid(self.near_.body).Result)) {
      bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
      bs.type = physics_BodyType::fixed;
      engine_world_create_body(n71, self.world_, bs);
      create_body = n71.result;
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
        n79 = CityStream_building_shape(self, self.tiles_[it][0].features.buildings[v.second], self.near_.body);
        self.near_.shapes[slot] = n79.Result;
        if (engine_physics_physics::Shape_valid(self.near_.shapes[slot]).Result) {
          self.near_.colliders = kern::to<Int>(kern::formula([](Float colliders) -> Float { return (colliders + 1.0); }, kern::to<Float>(self.near_.colliders)));
        }
        both = logic_not(whole);
        if (both) {
          built += 1;
          both = math_compare<">=">(kern::to<Float>(built), kern::to<Float>(kNearPerStep));
        }
        if (both) {
          return {math_compare<">">(kern::formula([](Float next, Float a) -> Float { return (!((next < 0.0) != (a < 0.0) ? (a < 0.0) : (next < a))); }, kern::to<Float>(self.near_.next), kern::to<Float>(list_length<Int64_Int_pair>(self.near_.list))), 0.5)};
        }
      }
    }
    return {true};
  }

  void CityStream_update_collision(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, Bool whole = false) {
    Vector way_a = Vector{0.0, 0.0, 0.0};
    Vector way_ab = Vector{0.0, 0.0, 0.0};
    Float way_l2 = 0.0;
    Bool moved = false;
    Int i = 0;
    List<CityStream_update_collision_Pick> all = List<CityStream_update_collision_Pick>{};
    Bool skip = false;
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<U32>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<U32>{}, List<U32>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<U32>>{}, Set<Int64>{}, Map<Int64, List<U32>>{}, false, 0, 0, Map<Int64, List<U32>>{}, Set<Int64>{}, List<Int64>{}, 0, List<U8>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
    Bool skip_2 = false;
    Float d2 = 0.0;
    Float mx = 0.0;
    Float my = 0.0;
    Int cap = 0;
    Bool both = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both_2 = false;
    Float now_ms = 0.0;
    way_a = vector_make(vector_break(focus).x, vector_break(focus).y, 0.0);
    way_ab = vector_make(vector_break(self.view_lead).x, vector_break(self.view_lead).y, 0.0);
    way_l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(way_ab).x, vector_break(way_ab).y);
    moved = logic_or(logic_or(math_compare<">=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(focus).x, vector_break(self.near_.at).x, vector_break(focus).y, vector_break(self.near_.at).y), 140.0), math_compare<">=">(kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3) -> Float { return kern::math::sqrt((((x + x_2) - x_3) * ((x + x_2) - x_3)) + (((y + y_2) - y_3) * ((y + y_2) - y_3))); }, vector_break(focus).x, vector_break(self.view_lead).x, vector_break(self.near_.ahead).x, vector_break(focus).y, vector_break(self.view_lead).y, vector_break(self.near_.ahead).y), 140.0)), math_compare<"!=">(kern::to<Float>(list_length<Vector>(self.extra_foci)), kern::to<Float>(list_length<Vector>(self.near_.mates_at))));
    i = 0;
    while (loop(logic_and(logic_not(moved), math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(self.extra_foci))), 0.5)))) {
      moved = math_compare<">=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.extra_foci[i]).x, vector_break(self.near_.mates_at[i]).x, vector_break(self.extra_foci[i]).y, vector_break(self.near_.mates_at[i]).y), 140.0);
      i += 1;
    }
    if (logic_and(logic_not(self.near_.building), logic_or(moved, logic_and(self.near_.dirty, math_compare<">=">(kern::to<Float>(self.near_.steps), kern::to<Float>(kNearSteps)))))) {
      all = List<CityStream_update_collision_Pick>{};
      for (auto it67 = flow_for(n67, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it67.next();) {
        skip = false;
        t = list_get<CityStream_Tile>(self.tiles_[n67.index], 0);
        if (logic_and(logic_and(logic_and(logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready)), logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::building))), logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::roads))), logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::areas)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_not(t.prepared)) {
            skip = true;
          }
          if (logic_not(skip)) {
            for (auto it68 = flow_for(n68, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(t.cent))))); it68.next();) {
              skip_2 = false;
              if (logic_and(int64_equal(map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n67.index), self.excl_tile_), math_compare<"==">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n68.index))))), kern::to<Float>(self.excl_index_)))) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                d2 = update_collision_way_d2(way_l2, way_a, way_ab, t.cent[n68.index]).Result;
                for (auto it69 = flow_for(n69, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(self.extra_foci))))); it69.next();) {
                  mx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(t.cent[n68.index]).x, vector_break(self.extra_foci[n69.index]).x);
                  my = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(t.cent[n68.index]).y, vector_break(self.extra_foci[n69.index]).y);
                  d2 = kern::formula([](Float d2, Float mx, Float my) -> Float { return kern::math::min(d2, ((mx * mx) + (my * my))); }, d2, mx, my);
                }
                if (math_compare<"<=">(d2, 115600.0)) {
                  list_add(all, struct_make<CityStream_update_collision_Pick>(d2, map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n67.index), n68.index));
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
      for (auto it70 = flow_for(n70, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<CityStream_update_collision_Pick>(all))))); it70.next();) {
        list_add(self.near_.list, struct_make<Int64_Int_pair>(all[n70.index].tile, all[n70.index].i));
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
      self.near_.steps = 0;
    }
    both = self.near_.building;
    if (both) {
      n80 = CityStream_build_near(self, whole);
      both = n80.Result;
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
        now_ms = engine_now_ms();
        self.ms_.t_collision = kern::formula([](Float now_ms, Float start_ms_) -> Float { return (now_ms - start_ms_); }, now_ms, self.start_ms_);
      }
    }
  }

  void CityStream_collide(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, Bool whole = false) {
    if (logic_not(object_valid<kern::physics::World>(self.world_))) {
      return;
    }
    terrain_cpp::Terrain_collide(self.terrain_, focus, self.anchor, kern::to<Int>(kern::formula([](Float whole) -> Float { return ((whole != 0.0) ? 1000000.0 : 1.0); }, kern::to<Float>(whole))));
    if (math_compare<"<">(kern::to<Float>(self.near_.steps), kern::to<Float>(kNearSteps))) {
      self.near_.steps = kern::to<Int>(kern::formula([](Float steps) -> Float { return (steps + 1.0); }, kern::to<Float>(self.near_.steps)));
    }
    CityStream_update_collision(self, focus, whole);
    self.stats_.colliders = self.near_.colliders;
    self.stats_.t_collision = self.ms_.t_collision;
  }

  void CityStream_upload(CityStream& self, CityStream_Tile& t, GeoChunk& chunk, const Material& material, Bool walls = false) {
    WeldStats weld = WeldStats{0, 0, 0, 0, false};
    U32 id = 0;
    Int allocate_id = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    Bool upsert = false;
    List<U32> t_2 = List<U32>{};
    Int64 key = Int64{0x0};
    List<U32> put = List<U32>{};
    if (math_compare<"==">(kern::to<Float>(engine_geo_mesh_data::MeshData_vertex_count(chunk.mesh).Result), 0.0)) {
      return;
    }
    if (logic_not(chunk.welded)) {
      engine_weld(n81, chunk.mesh, 0);
      weld = n81.result;
      chunk.mesh = n81.mesh;
    }
    engine_renderer_allocate_id(n82, self.r_);
    allocate_id = n82.result;
    id = allocate_id;
    self.upload_count_ = kern::to<Int>(kern::formula([](Float upload_count_) -> Float { return (upload_count_ + 1.0); }, kern::to<Float>(self.upload_count_)));
    self.upload_vertices_ = int64_add(self.upload_vertices_, int64_of_int(engine_geo_mesh_data::MeshData_vertex_count(chunk.mesh).Result));
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}, 0, 0, true, 1, true};
    look.material = material;
    look.extra_slot = kern::to<Int>(kern::formula([](Float walls, Float kMaxExtensionAttributes) -> Float { return ((walls != 0.0) ? 0.0 : kMaxExtensionAttributes); }, kern::to<Float>(walls), kern::to<Float>(kMaxExtensionAttributes)));
    look.pick_bvh = false;
    engine_upsert(n83, self.r_, id, chunk.mesh, look, List<Float>{}, List<Float>{});
    upsert = n83.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, id);
      return;
    }
    list_add(t.meshes, id);
    if (walls) {
      key = cell_key(chunk.x, chunk.y).Result;
      map_add(n84, t.cell_ids, key);
      t_2 = t.cell_ids[n84.index];
      list_add(t_2, id);
      put = t_2;
      t.cell_ids[n84.index] = put;
    }
    t.triangles = kern::to<Int>(kern::formula([](Float triangles, Float a) -> Float { return (triangles + a); }, kern::to<Float>(t.triangles), kern::to<Float>(engine_geo_mesh_data::MeshData_triangle_count(chunk.mesh).Result)));
  }

  void CityStream_relay(CityStream& self, const Vec4& rect) {
    Bool skip = false;
    for (auto it85 = flow_for(n85, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it85.next();) {
      skip = false;
      if (logic_or(logic_or(logic_or(math_compare<">">(rect.x, self.tiles_[n85.index][0].bounds.z), math_compare<"<">(rect.z, self.tiles_[n85.index][0].bounds.x)), math_compare<">">(rect.y, self.tiles_[n85.index][0].bounds.w)), math_compare<"<">(rect.w, self.tiles_[n85.index][0].bounds.y))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(self.tiles_[n85.index][0].relay, logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n85.index][0].state, CityStream_Tile_State::ready)))) {
          if (self.tiles_[n85.index][0].relay) {
            self.tiles_[n85.index][0].relay_again = true;
          }
          skip = true;
        }
        if (logic_not(skip)) {
          self.tiles_[n85.index][0].relay = true;
          var_take(n86, self.tiles_[n85.index][0].lay_meshes);
          self.tiles_[n85.index][0].old_lay = n86.value;
          list_clear(self.tiles_[n85.index][0].lay_meshes);
          CityStream_begin_roads(self, self.tiles_[n85.index][0]);
        }
      }
    }
  }

  struct CityStream_damage_buildingResult { Bool Result = false; };
  CityStream_damage_buildingResult CityStream_damage_building(CityStream& self, Int64 tile = Int64{0x0}, U32 index = 0, Float new_top = 0.0, Bool leveled = false) {
    Int it = 0;
    Int key = 0;
    Int d_row = 0;
    Int64 key_2 = Int64{0x0};
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
    self.damage_serial_ = kern::to<Int>(kern::formula([](Float damage_serial_) -> Float { return (damage_serial_ + 1.0); }, kern::to<Float>(self.damage_serial_)));
    key = self.tiles_[it][0].features.buildings[index].seed;
    map_add(n87, self.damage_, key);
    d_row = n87.index;
    self.damage_[d_row].height = kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, self.tiles_[it][0].features.buildings[index].top, self.tiles_[it][0].features.buildings[index].base);
    self.damage_[d_row].leveled = self.tiles_[it][0].features.buildings[index].leveled;
    if (logic_not(self.damage_quiet_)) {
      list_add(self.damage_log_, self.tiles_[it][0].features.buildings[index].seed);
    }
    if (logic_and(math_compare<">">(kern::formula([](Float index, Float a) -> Float { return ((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a)); }, kern::to<Float>(index), kern::to<Float>(list_length<Vector>(self.tiles_[it][0].cent))), 0.5), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(map_contains<Int64, List<U32>>(self.tiles_[it][0].cell_ids, cell_of(self.tiles_[it][0].cent[index]).Result)))), 0.0))) {
      key_2 = cell_of(self.tiles_[it][0].cent[index]).Result;
      set_add(n88, self.tiles_[it][0].dirty_cells, key_2);
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
            self.near_.colliders = kern::to<Int>(kern::formula([](Float colliders) -> Float { return (colliders - 1.0); }, kern::to<Float>(self.near_.colliders)));
          }
          n89 = CityStream_building_shape(self, self.tiles_[it][0].features.buildings[index], self.near_.body);
          self.near_.shapes[k] = n89.Result;
          if (engine_physics_physics::Shape_valid(self.near_.shapes[k]).Result) {
            self.near_.colliders = kern::to<Int>(kern::formula([](Float colliders) -> Float { return (colliders + 1.0); }, kern::to<Float>(self.near_.colliders)));
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
    for (auto it90 = flow_for(n90, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))))); it90.next();) {
      list_add(out, struct_make<CityStream_DamageRec>(map_key_at<Int, CityStream_Damage>(self.damage_, n90.index), self.damage_[n90.index].height, self.damage_[n90.index].leveled));
    }
    list_sort<&city_stream_cpp::damage_list_match>(*this, out);
  }

  struct CityStream_damage_ofResult { Bool Result = false; };
  CityStream_damage_ofResult CityStream_damage_of(const CityStream& self, CityStream_DamageRec& out, U32 seed = 0) {
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
    for (auto it91 = flow_for(n91, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<CityStream_DamageRec>(list))))); it91.next();) {
      skip = false;
      r = list[n91.index];
      if (logic_or(logic_not(math_compare<">=">(r.height, 0.0)), logic_not(math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, r.height), 0.0)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, CityStream_Damage>(self.damage_, r.seed).index), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))));
        if (logic_and(logic_and(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_)))), math_compare<"<=">(self.damage_[it].height, kern::formula([](Float height) -> Float { return (height + 0.01); }, r.height))), logic_or(self.damage_[it].leveled, logic_not(r.leveled)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          put = n91.index;
          key = r.seed;
          map_add(n92, want, key);
          want[n92.index] = put;
        }
      }
    }
    if (math_compare<"==">(kern::to<Float>(map_length<Int, Int>(want)), 0.0)) {
      return {false};
    }
    self.damage_quiet_ = logic_not(the_log);
    for (auto it93 = flow_for(n93, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it93.next();) {
      skip_2 = false;
      if (logic_not(self.tiles_[n93.index][0].prepared)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        for (auto it94 = flow_for(n94, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[n93.index][0].features.buildings))))); it94.next();) {
          skip_3 = false;
          w = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, Int>(want, self.tiles_[n93.index][0].features.buildings[n94.index].seed).index), kern::to<Float>(map_length<Int, Int>(want))));
          if (math_compare<"==">(kern::to<Float>(w), kern::to<Float>(map_length<Int, Int>(want)))) {
            skip_3 = true;
          }
          if (logic_not(skip_3)) {
            b = self.tiles_[n93.index][0].features.buildings[n94.index];
            r_2 = list[want[w]];
            n95 = CityStream_damage_building(self, map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n93.index), n94.index, kern::formula([](Float base, Float height) -> Float { return (base + height); }, b.base, r_2.height), r_2.leveled);
            if (n95.Result) {
              changed = true;
            }
          }
        }
      }
    }
    for (auto it96 = flow_for(n96, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int, Int>(want))))); it96.next();) {
      skip_4 = false;
      r_3 = list[want[n96.index]];
      it_2 = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int, CityStream_Damage>(self.damage_, map_key_at<Int, Int>(want, n96.index)).index), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))));
      if (math_compare<"==">(kern::to<Float>(it_2), kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_)))) {
        put_2 = struct_make<CityStream_Damage>(r_3.height, r_3.leveled);
        key_2 = map_key_at<Int, Int>(want, n96.index);
        map_add(n97, self.damage_, key_2);
        self.damage_[n97.index] = put_2;
      } else if (logic_or(math_compare<"<">(r_3.height, kern::formula([](Float a) -> Float { return (a - 0.01); }, self.damage_[it_2].height)), logic_and(r_3.leveled, logic_not(self.damage_[it_2].leveled)))) {
        self.damage_[it_2].height = kern::formula([](Float a, Float height) -> Float { return kern::math::min(a, height); }, self.damage_[it_2].height, r_3.height);
        self.damage_[it_2].leveled = logic_or(self.damage_[it_2].leveled, r_3.leveled);
      } else {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        changed = true;
        if (the_log) {
          list_add(self.damage_log_, map_key_at<Int, Int>(want, n96.index));
        }
      }
    }
    self.damage_quiet_ = false;
    if (changed) {
      self.damage_serial_ = kern::to<Int>(kern::formula([](Float damage_serial_) -> Float { return (damage_serial_ + 1.0); }, kern::to<Float>(self.damage_serial_)));
    }
    return {changed};
  }

  void CityStream_clear_damage(CityStream& self) {
    Bool skip = false;
    Bool skip_2 = false;
    Int64 key = Int64{0x0};
    map_clear(self.damage_);
    list_clear(self.damage_log_);
    for (auto it98 = flow_for(n98, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it98.next();) {
      skip = false;
      if (logic_not(self.tiles_[n98.index][0].prepared)) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it99 = flow_for(n99, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[n98.index][0].features.buildings))))); it99.next();) {
          skip_2 = false;
          if (logic_and(math_compare<">=">(self.tiles_[n98.index][0].features.buildings[n99.index].top, kern::formula([](Float a) -> Float { return (a - 0.01); }, self.tiles_[n98.index][0].features.buildings[n99.index].full_top)), logic_not(self.tiles_[n98.index][0].features.buildings[n99.index].leveled))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            self.tiles_[n98.index][0].features.buildings[n99.index].top = self.tiles_[n98.index][0].features.buildings[n99.index].full_top;
            self.tiles_[n98.index][0].features.buildings[n99.index].leveled = false;
            if (logic_and(math_compare<">">(kern::formula([](Float index, Float a) -> Float { return ((index < 0.0) != (a < 0.0) ? (a < 0.0) : (index < a)); }, kern::to<Float>(n99.index), kern::to<Float>(list_length<Vector>(self.tiles_[n98.index][0].cent))), 0.5), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(map_contains<Int64, List<U32>>(self.tiles_[n98.index][0].cell_ids, cell_of(self.tiles_[n98.index][0].cent[n99.index]).Result)))), 0.0))) {
              key = cell_of(self.tiles_[n98.index][0].cent[n99.index]).Result;
              set_add(n100, self.tiles_[n98.index][0].dirty_cells, key);
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

  void CityStream_begin_cells(CityStream& self, CityStream_Tile& t) {
    List<Float_Int64_pair> order = List<Float_Int64_pair>{};
    Int64 c = Int64{0x0};
    I32 cx = 0;
    I32 cy = 0;
    Object<kern::FootprintStream> fs = Object<kern::FootprintStream>{};
    Object<kern::FootprintStream> footprint_stream = Object<kern::FootprintStream>{};
    List<Vector> thin = List<Vector>{};
    Bool skip = false;
    Int64 key = Int64{0x0};
    Int it = 0;
    Int at = 0;
    U32 i = 0;
    BuildingOptions a = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
    Int arg = 0;
    Int submit = 0;
    order = List<Float_Int64_pair>{};
    for (auto it101 = flow_for(n101, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(set_length<Int64>(t.dirty_cells))))); it101.next();) {
      c = set_key_at<Int64>(t.dirty_cells, n101.index);
      cx = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(c, 32)))));
      cy = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32s(a); }, kern::to<Float>(int64_low(c))));
      list_add(order, struct_make<Float_Int64_pair>(kern::formula([](Float cx, Float kChunk, Float x, Float cy, Float y) -> Float { return kern::math::sqrt(((((cx + 0.5) * kChunk) - x) * (((cx + 0.5) * kChunk) - x)) + ((((cy + 0.5) * kChunk) - y) * (((cy + 0.5) * kChunk) - y))); }, kern::to<Float>(cx), kChunk, vector_break(self.focus_).x, kern::to<Float>(cy), vector_break(self.focus_).y), c));
    }
    list_sort<&city_stream_cpp::begin_cells_match>(*this, order);
    if (math_compare<">">(kern::to<Float>(list_length<Float_Int64_pair>(order)), 6.0)) {
      list_resize(order, 6, Float_Int64_pair{0.0, Int64{0x0}});
    }
    engine_footprint_stream_make(n102);
    footprint_stream = n102.result;
    fs = footprint_stream;
    thin = List<Vector>{};
    list_clear(t.job_cells);
    for (auto it103 = flow_for(n103, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_Int64_pair>(order))))); it103.next();) {
      skip = false;
      list_add(t.job_cells, order[n103.index].second);
      key = order[n103.index].second;
      set_remove(n104, t.dirty_cells, key);
      it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<U32>>(t.cell_items, order[n103.index].second).index), kern::to<Float>(map_length<Int64, List<U32>>(t.cell_items))));
      if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<U32>>(t.cell_items)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        at = it;
        for (auto it105 = flow_for(n105, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U32>(t.cell_items[at]))))); it105.next();) {
          i = t.cell_items[at][n105.index];
          put_footprint(t.features.buildings[i], thin, fs, t.cent[i]);
        }
      }
    }
    arg = kBuildings;
    a = building_options().Result;
    n106 = city_jobs_cpp::buildings_input(a, fs);
    engine_jobs_submit(n107, arg, n106.Result);
    submit = n107.result;
    t.cell_job = submit;
    if (object_valid<kern::FootprintStream>(fs)) {
      engine_footprint_stream_release(fs);
    }
    fs = Object<kern::FootprintStream>{};
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
    List<U32> put = List<U32>{};
    Int64 key = Int64{0x0};
    Int at = 0;
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Bool stop = false;
    GeoChunk chunk = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}};
    Float t1 = 0.0;
    Float now_ms = 0.0;
    Float now_ms_2 = 0.0;
    Float now_ms_3 = 0.0;
    Bool both = false;
    U32 id = 0;
    Bool remove = false;
    Int gone = 0;
    Int64 c_2 = Int64{0x0};
    Int64 key_2 = Int64{0x0};
    Float now_ms_4 = 0.0;
    if (logic_not(t.cell_uploading)) {
      engine_jobs_take(n108, t.cell_job, t.built_bytes);
      take = n108.result;
      t.built_bytes = n108.out;
      if (logic_not(take)) {
        return {false};
      }
      t.cell_job = 0;
      for (auto it109 = flow_for(n109, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int64>(t.job_cells))))); it109.next();) {
        c = t.job_cells[n109.index];
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<U32>>(t.cell_ids, c).index), kern::to<Float>(map_length<Int64, List<U32>>(t.cell_ids))));
        if (logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<U32>>(t.cell_ids))))) {
          var_take(n110, t.cell_ids[it]);
          put = n110.value;
          key = c;
          map_add(n111, t.cell_old, key);
          t.cell_old[n111.index] = put;
          at = it;
          map_remove_at(t.cell_ids, at);
        }
      }
      t.built_at = 4;
      t.upload_next = city_jobs_cpp::chunk_count(t.built_bytes).Result;
      t.cell_uploading = true;
    }
    m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    m.roughness = 0.82;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">">(kern::formula([](Float upload_next) -> Float { return ((0.0 < 0.0) != (upload_next < 0.0) ? (upload_next < 0.0) : (0.0 < upload_next)); }, kern::to<Float>(t.upload_next)), 0.5)))) {
      chunk = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}};
      n112 = city_jobs_cpp::read_chunk(t.built_bytes, t.built_at, chunk);
      if (logic_not(n112.Result)) {
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
    for (auto it113 = flow_for(n113, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<U32>>(t.cell_old))))); it113.next();) {
      for (auto it114 = flow_for(n114, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U32>(t.cell_old[n113.index]))))); it114.next();) {
        id = t.cell_old[n113.index][n114.index];
        engine_renderer_remove(n115, self.r_, id);
        remove = n115.result;
        gone = id;
        while (loop(list_contains<U32>(t.meshes, gone))) {
          list_remove(t.meshes, gone);
        }
      }
    }
    map_clear(t.cell_old);
    for (auto it116 = flow_for(n116, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int64>(t.job_cells))))); it116.next();) {
      c_2 = t.job_cells[n116.index];
      key_2 = c_2;
      map_add(n117, t.cell_ids, key_2);
    }
    list_clear(t.job_cells);
    t.cell_uploading = false;
    self.rebuilds_ = kern::to<Int>(kern::formula([](Float rebuilds_) -> Float { return (rebuilds_ + 1.0); }, kern::to<Float>(self.rebuilds_)));
    if (logic_and(math_compare<"<">(self.ms_.t_built, 0.0), tiles::TileKey_equal(t.key, self.origin_tile_).Result)) {
      now_ms_4 = engine_now_ms();
      self.ms_.t_built = kern::formula([](Float now_ms_4, Float start_ms_) -> Float { return (now_ms_4 - start_ms_); }, now_ms_4, self.start_ms_);
    }
    return {true};
  }

  struct CityStream_workResult { Bool Result = false; };
  CityStream_workResult CityStream_work(CityStream& self, CityStream_Tile& t, Float deadline = 0.0) {
    List<U8> out = List<U8>{};
    Bool take = false;
    Float now_ms = 0.0;
    Float now_ms_2 = 0.0;
    Bool chosen = false;
    Vec4 a = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 a_2 = Vec4{0.0, 0.0, 0.0, 0.0};
    Bool either = false;
    Float now_ms_3 = 0.0;
    Float now_ms_4 = 0.0;
    Bool chosen_2 = false;
    Float now_ms_5 = 0.0;
    Bool chosen_3 = false;
    Float t1 = 0.0;
    Float now_ms_6 = 0.0;
    Bool built = false;
    Float now_ms_7 = 0.0;
    Bool step = false;
    Material m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
    Int before = 0;
    Float t1_2 = 0.0;
    Float now_ms_8 = 0.0;
    GeoChunk chunk = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}};
    Int place = 0;
    Int first = 0;
    Int last = 0;
    Int was = 0;
    Float now_ms_9 = 0.0;
    Float now_ms_10 = 0.0;
    Bool chosen_4 = false;
    U32 id = 0;
    Bool remove = false;
    Int at = 0;
    Int count = 0;
    Bool stop = false;
    Vector c = Vector{0.0, 0.0, 0.0};
    Float t1_3 = 0.0;
    Float now_ms_11 = 0.0;
    Bool done = false;
    Float now_ms_12 = 0.0;
    Float t1_4 = 0.0;
    Float now_ms_13 = 0.0;
    Vector mid = Vector{0.0, 0.0, 0.0};
    Float now_ms_14 = 0.0;
    Float now_ms_15 = 0.0;
    switch (flow_switch_enum<CityStream_Tile_State>(t.state)) {
      case CityStream_Tile_State::requested: {
        return {true};
      } break;
      case CityStream_Tile_State::decoding: {
        out = List<U8>{};
        engine_jobs_take(n118, t.tile_job, out);
        take = n118.result;
        out = n118.out;
        if (logic_not(take)) {
          return {true};
        }
        t.tile_job = 0;
        n119 = city_jobs_cpp::read_features(out, t.features);
        if (logic_not(n119.Result)) {
          t.state = CityStream_Tile_State::failed;
          return {true};
        }
        self.real_tiles_ = kern::to<Int>(kern::formula([](Float real_tiles_) -> Float { return (real_tiles_ + 1.0); }, kern::to<Float>(self.real_tiles_)));
        if (logic_and(tiles::TileKey_equal(t.key, self.origin_tile_).Result, math_compare<"<">(self.ms_.t_decoded, 0.0))) {
          now_ms = engine_now_ms();
          self.ms_.t_decoded = kern::formula([](Float now_ms, Float start_ms_) -> Float { return (now_ms - start_ms_); }, now_ms, self.start_ms_);
        }
        CityStream_feed_terrain(self, t);
        t.state = CityStream_Tile_State::terrain_wait;
        now_ms_2 = engine_now_ms();
        if (math_compare<">">(now_ms_2, deadline)) {
          chosen = false;
        } else {
          n120 = CityStream_work(self, t, deadline);
          chosen = n120.Result;
        }
        return {chosen};
      } break;
      case CityStream_Tile_State::terrain_wait: {
        either = logic_not(terrain::Terrain_settled(self.terrain_).Result);
        if (logic_not(either)) {
          a_2 = engine_math_math::Vec4_new(kern::formula([](Float x) -> Float { return (x - 80.0); }, t.bounds.x), kern::formula([](Float y) -> Float { return (y - 80.0); }, t.bounds.y), kern::formula([](Float z) -> Float { return (z + 80.0); }, t.bounds.z), kern::formula([](Float w) -> Float { return (w + 80.0); }, t.bounds.w)).Result;
          either = logic_not(terrain_cpp::Terrain_covers(self.terrain_, a_2).Result);
        }
        if (either) {
          return {true};
        }
        if (math_compare<"<">(self.ms_.t_terrain, 0.0)) {
          now_ms_3 = engine_now_ms();
          self.ms_.t_terrain = kern::formula([](Float now_ms_3, Float start_ms_) -> Float { return (now_ms_3 - start_ms_); }, now_ms_3, self.start_ms_);
        }
        CityStream_finish_features(self, t);
        now_ms_4 = engine_now_ms();
        if (math_compare<">">(now_ms_4, deadline)) {
          chosen_2 = false;
        } else {
          n121 = CityStream_work(self, t, deadline);
          chosen_2 = n121.Result;
        }
        return {chosen_2};
      } break;
      case CityStream_Tile_State::building: {
        CityStream_begin_roads(self, t);
        now_ms_5 = engine_now_ms();
        if (math_compare<">">(now_ms_5, deadline)) {
          chosen_3 = false;
        } else {
          n122 = CityStream_work(self, t, deadline);
          chosen_3 = n122.Result;
        }
        return {chosen_3};
      } break;
      case CityStream_Tile_State::roads: {
        now_ms_6 = engine_now_ms();
        t1 = now_ms_6;
        now_ms_7 = engine_now_ms();
        engine_road_builder_step(n123, t.road_builder, kern::formula([](Float deadline, Float now_ms_7) -> Float { return kern::math::max(0.5, (deadline - now_ms_7)); }, deadline, now_ms_7));
        step = n123.result;
        built = step;
        CityStream_step_time(self, "roads.build", t1);
        if (logic_not(built)) {
          return {false};
        }
        m = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}};
        m.roughness = 0.95;
        m.cast_shadow = false;
        m.double_sided = true;
        engine_road_builder_chunks(n124, t.road_builder);
        while (loop(math_compare<">">(kern::formula([](Float upload_next, Float a) -> Float { return ((upload_next < 0.0) != (a < 0.0) ? (a < 0.0) : (upload_next < a)); }, kern::to<Float>(t.upload_next), kern::to<Float>(list_length<GeoChunk>(n124.result))), 0.5))) {
          before = list_length<U32>(t.meshes);
          now_ms_8 = engine_now_ms();
          t1_2 = now_ms_8;
          var_take(n125, n124.result[t.upload_next]);
          n124.result[t.upload_next].x = struct_break<GeoChunk>(n125.value).x;
          n124.result[t.upload_next].y = struct_break<GeoChunk>(n125.value).y;
          n124.result[t.upload_next].items = struct_break<GeoChunk>(n125.value).items;
          n124.result[t.upload_next].triangles = struct_break<GeoChunk>(n125.value).triangles;
          n124.result[t.upload_next].hash = struct_break<GeoChunk>(n125.value).hash;
          n124.result[t.upload_next].welded = struct_break<GeoChunk>(n125.value).welded;
          n124.result[t.upload_next].mesh.extra_size = struct_break<MeshData>(struct_break<GeoChunk>(n125.value).mesh).extra_size;
          chunk = n125.value;
          CityStream_upload(self, t, chunk, m, false);
          CityStream_step_time(self, "roads.upload", t1_2);
          place = list_length<U32>(t.lay_meshes);
          first = before;
          last = kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U32>(t.meshes))));
          for (auto it126 = flow_for(n126, first, last); it126.next();) {
            list_insert(t.lay_meshes, kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a + b; }, kern::to<Float>(place), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float b) -> Float { return a - b; }, kern::to<Float>(n126.index), kern::to<Float>(first))))))), list_get<U32>(t.meshes, n126.index));
          }
          list_resize(t.meshes, before, 0);
          was = t.upload_next;
          t.upload_next = kern::to<Int>(kern::formula([](Float upload_next) -> Float { return (upload_next + 1.0); }, kern::to<Float>(t.upload_next)));
          n124.result[was].mesh = struct_make<MeshData>(List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{});
          now_ms_9 = engine_now_ms();
          if (math_compare<">">(now_ms_9, deadline)) {
            return {false};
          }
        }
        list_clear(n124.result);
        t.upload_next = 0;
        t.state = CityStream_Tile_State::areas;
        now_ms_10 = engine_now_ms();
        if (math_compare<">">(now_ms_10, deadline)) {
          chosen_4 = false;
        } else {
          n127 = CityStream_work(self, t, deadline);
          chosen_4 = n127.Result;
        }
        return {chosen_4};
      } break;
      case CityStream_Tile_State::areas: {
        if (t.relay) {
          for (auto it128 = flow_for(n128, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<U32>(t.old_lay))))); it128.next();) {
            id = t.old_lay[n128.index];
            engine_renderer_remove(n129, self.r_, id);
            remove = n129.result;
          }
          list_clear(t.old_lay);
          t.relay = false;
          t.state = CityStream_Tile_State::ready;
          if (t.relay_again) {
            t.relay_again = false;
            t.relay = true;
            var_take(n130, t.lay_meshes);
            t.old_lay = n130.value;
            list_clear(t.lay_meshes);
            CityStream_begin_roads(self, t);
          }
          return {true};
        }
        t.state = CityStream_Tile_State::ready;
        count = list_length<Vector>(t.cent);
        at = 0;
        stop = false;
        while (loop(logic_and(logic_not(stop), math_compare<"<">(kern::to<Float>(at), kern::to<Float>(count))))) {
          c = t.cent[at];
          if (math_compare<"<=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(self.focus_).x, vector_break(c).y, vector_break(self.focus_).y), 340.0)) {
            self.near_.dirty = true;
            stop = true;
          }
          if (logic_not(stop)) {
            at += 1;
          }
        }
        return {true};
      } break;
      case CityStream_Tile_State::collision: {
        t.state = CityStream_Tile_State::ready;
        return {true};
      } break;
      case CityStream_Tile_State::ready: {
        if (logic_or(math_compare<"!=">(kern::to<Float>(t.cell_job), 0.0), t.cell_uploading)) {
          now_ms_11 = engine_now_ms();
          t1_3 = now_ms_11;
          n131 = CityStream_finish_cells(self, t, deadline);
          done = n131.Result;
          CityStream_step_time(self, "cells.swap", t1_3);
          if (logic_not(done)) {
            now_ms_12 = engine_now_ms();
            return {math_compare<"<=">(now_ms_12, deadline)};
          }
        }
        if (logic_and(logic_and(logic_not(math_compare<"==">(kern::to<Float>(set_length<Int64>(t.dirty_cells)), 0.0)), logic_not(math_compare<"!=">(kern::to<Float>(t.cell_job), 0.0))), logic_not(t.cell_uploading))) {
          now_ms_13 = engine_now_ms();
          t1_4 = now_ms_13;
          CityStream_begin_cells(self, t);
          CityStream_step_time(self, "cells.begin", t1_4);
        }
        mid = vector_make(kern::formula([](Float x, Float x_2) -> Float { return (x + (x_2 * 0.5)); }, vector_break(self.focus_).x, vector_break(self.view_lead).x), kern::formula([](Float y, Float y_2) -> Float { return (y + (y_2 * 0.5)); }, vector_break(self.focus_).y, vector_break(self.view_lead).y), 0.0);
        if (logic_and(logic_and(logic_not(t.relay), math_compare<">=">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(mid).x, vector_break(t.roads_at).x, vector_break(mid).y, vector_break(t.roads_at).y), 260.0)), math_compare<"<">(rect_distance_to(t.bounds, vector_break(mid).x, vector_break(mid).y).Result, kern::formula([](Float x, Float y) -> Float { return ((720.0 + 260.0) + (kern::math::sqrt((x * x) + (y * y)) * 0.5)); }, vector_break(self.view_lead).x, vector_break(self.view_lead).y)))) {
          t.relay = true;
          var_take(n132, t.lay_meshes);
          t.old_lay = n132.value;
          list_clear(t.lay_meshes);
          CityStream_begin_roads(self, t);
          now_ms_14 = engine_now_ms();
          return {math_compare<"<=">(now_ms_14, deadline)};
        }
        now_ms_15 = engine_now_ms();
        return {math_compare<"<=">(now_ms_15, deadline)};
      } break;
      case CityStream_Tile_State::failed: {
        return {true};
      } break;
    }
  }

  struct update_grownResult { Vec4 Result = Vec4{0.0, 0.0, 0.0, 0.0}; };
  KERN_PURE update_grownResult update_grown(Vec4 r = Vec4{0.0, 0.0, 0.0, 0.0}) {
    return {engine_math_math::Vec4_new(kern::formula([](Float x) -> Float { return (x - 80.0); }, r.x), kern::formula([](Float y) -> Float { return (y - 80.0); }, r.y), kern::formula([](Float z) -> Float { return (z + 80.0); }, r.z), kern::formula([](Float w) -> Float { return (w + 80.0); }, r.w)).Result};
  }

  struct update_matchResult { Bool Result = false; };
  KERN_PURE update_matchResult update_match(const Float_Int64_pair& a, const Float_Int64_pair& b) {
    return {math_compare<"<">(a.first, b.first)};
  }

  void CityStream_update(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, Float budget_ms = 0.0) {
    Float start = 0.0;
    Float now_ms = 0.0;
    Float deadline = 0.0;
    Float t0 = 0.0;
    Float now_ms_2 = 0.0;
    Float now_ms_3 = 0.0;
    Float now_ms_4 = 0.0;
    Float now_ms_5 = 0.0;
    Float now_ms_6 = 0.0;
    List<Float_Int64_pair> order = List<Float_Int64_pair>{};
    Int at = 0;
    Int count = 0;
    Bool stop = false;
    Float now_ms_7 = 0.0;
    Float now_ms_8 = 0.0;
    Int it = 0;
    Text name = "";
    Bool more = false;
    Float now_ms_9 = 0.0;
    Float now_ms_10 = 0.0;
    Float now_ms_11 = 0.0;
    Float now_ms_12 = 0.0;
    if (logic_or(logic_not(object_valid<kern::Renderer>(self.r_)), logic_not(object_valid<kern::physics::World>(self.world_)))) {
      return;
    }
    now_ms = engine_now_ms();
    start = now_ms;
    deadline = kern::formula([](Float start, Float budget_ms) -> Float { return (start + budget_ms); }, start, budget_ms);
    now_ms_2 = engine_now_ms();
    t0 = now_ms_2;
    engine_jobs_pump(kern::formula([](Float budget_ms) -> Float { return (budget_ms * 0.5); }, budget_ms));
    CityStream_step_time(self, "jobs", t0);
    self.focus_ = focus;
    now_ms_3 = engine_now_ms();
    t0 = now_ms_3;
    CityStream_select_cells(self, focus, false);
    CityStream_step_time(self, "select_cells", t0);
    now_ms_4 = engine_now_ms();
    t0 = now_ms_4;
    CityStream_request_around(self, focus, self.ring);
    for (auto it133 = flow_for(n133, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(self.extra_foci))))); it133.next();) {
      CityStream_request_around(self, self.extra_foci[n133.index], self.mate_ring);
    }
    for (auto it134 = flow_for(n134, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it134.next();) {
      if (logic_and(enum_equal<CityStream_Tile_State>(self.tiles_[n134.index][0].state, CityStream_Tile_State::failed), math_compare<">=">(start, self.tiles_[n134.index][0].retry_ms))) {
        self.tiles_[n134.index][0].state = CityStream_Tile_State::requested;
        list_add(self.requests_, self.tiles_[n134.index][0].key);
      }
    }
    CityStream_unload_far(self, focus);
    CityStream_step_time(self, "request/unload", t0);
    now_ms_5 = engine_now_ms();
    t0 = now_ms_5;
    self.terrain_.extra = self.extra_foci;
    list_clear(self.terrain_.needs);
    if (logic_not(self.offline_)) {
      n135 = CityStream_ring_bounds(self, focus, self.ring);
      list_add(self.terrain_.needs, update_grown(n135.Result).Result);
      for (auto it136 = flow_for(n136, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(self.extra_foci))))); it136.next();) {
        n137 = CityStream_ring_bounds(self, self.extra_foci[n136.index], self.mate_ring);
        list_add(self.terrain_.needs, update_grown(n137.Result).Result);
      }
    }
    terrain_cpp::Terrain_update(self.terrain_, focus, self.anchor, kern::formula([](Float budget_ms) -> Float { return kern::math::max(1.0, (budget_ms * 0.45)); }, budget_ms));
    CityStream_step_time(self, "terrain", t0);
    now_ms_6 = engine_now_ms();
    t0 = now_ms_6;
    terrain::Terrain_take_changes(self.terrain_, self.terrain_changes_);
    for (auto it138 = flow_for(n138, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vec4>(self.terrain_changes_))))); it138.next();) {
      CityStream_relay(self, self.terrain_changes_[n138.index]);
      list_add(self.ground_changes_, self.terrain_changes_[n138.index]);
    }
    list_clear(self.terrain_changes_);
    CityStream_step_time(self, "relay", t0);
    order = List<Float_Int64_pair>{};
    for (auto it139 = flow_for(n139, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it139.next();) {
      list_add(order, struct_make<Float_Int64_pair>(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return (((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.tiles_[n139.index][0].center).x, vector_break(focus).x, vector_break(self.tiles_[n139.index][0].center).y, vector_break(focus).y), map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n139.index)));
    }
    list_sort<&city_stream_cpp::update_match>(*this, order);
    count = list_length<Float_Int64_pair>(order);
    at = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<">(kern::to<Float>(at), kern::to<Float>(count))))) {
      now_ms_7 = engine_now_ms();
      if (math_compare<">">(now_ms_7, deadline)) {
        stop = true;
      }
      if (logic_not(stop)) {
        now_ms_8 = engine_now_ms();
        t0 = now_ms_8;
        it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<CityStream_Tile>>(self.tiles_, order[at].second).index), kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))));
        name = list_get<Text>(kStateName, enum_index<CityStream_Tile_State>(self.tiles_[it][0].state));
        n140 = CityStream_work(self, self.tiles_[it][0], deadline);
        more = n140.Result;
        CityStream_step_time(self, name, t0);
        if (logic_not(more)) {
          stop = true;
        }
      }
      if (logic_not(stop)) {
        at += 1;
      }
    }
    self.stats_ = struct_make<StreamStats>(0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0);
    for (auto it141 = flow_for(n141, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it141.next();) {
      if (enum_equal<CityStream_Tile_State>(self.tiles_[n141.index][0].state, CityStream_Tile_State::ready)) {
        self.stats_.tiles_ready = kern::to<Int>(kern::formula([](Float tiles_ready) -> Float { return (tiles_ready + 1.0); }, kern::to<Float>(self.stats_.tiles_ready)));
      } else if (enum_equal<CityStream_Tile_State>(self.tiles_[n141.index][0].state, CityStream_Tile_State::failed)) {
        self.stats_.tiles_failed = kern::to<Int>(kern::formula([](Float tiles_failed) -> Float { return (tiles_failed + 1.0); }, kern::to<Float>(self.stats_.tiles_failed)));
      } else {
        self.stats_.tiles_loading = kern::to<Int>(kern::formula([](Float tiles_loading) -> Float { return (tiles_loading + 1.0); }, kern::to<Float>(self.stats_.tiles_loading)));
      }
      self.stats_.buildings = kern::to<Int>(kern::formula([](Float buildings, Float a) -> Float { return (buildings + kern::math::wrap32(a)); }, kern::to<Float>(self.stats_.buildings), kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[n141.index][0].features.buildings))));
      self.stats_.triangles = kern::to<Int>(kern::formula([](Float triangles, Float a) -> Float { return (triangles + a); }, kern::to<Float>(self.stats_.triangles), kern::to<Float>(self.tiles_[n141.index][0].triangles)));
      self.stats_.meshes = kern::to<Int>(kern::formula([](Float meshes, Float a, Float a_2) -> Float { return (meshes + kern::math::wrap32((a + a_2))); }, kern::to<Float>(self.stats_.meshes), kern::to<Float>(list_length<U32>(self.tiles_[n141.index][0].meshes)), kern::to<Float>(list_length<U32>(self.tiles_[n141.index][0].lay_meshes))));
    }
    self.stats_.colliders = self.near_.colliders;
    now_ms_9 = engine_now_ms();
    self.ms_.work_ms = kern::formula([](Float work_ms, Float now_ms_9, Float start) -> Float { return (work_ms + (now_ms_9 - start)); }, self.ms_.work_ms, now_ms_9, start);
    self.stats_.t_request = self.ms_.t_request;
    self.stats_.t_data = self.ms_.t_data;
    self.stats_.t_decoded = self.ms_.t_decoded;
    self.stats_.t_terrain = self.ms_.t_terrain;
    self.stats_.t_built = self.ms_.t_built;
    self.stats_.t_collision = self.ms_.t_collision;
    self.stats_.work_ms = self.ms_.work_ms;
    self.stats_.rebuilds = self.rebuilds_;
    self.stats_.uploads = self.upload_count_;
    self.stats_.upload_vertices = int64_low(int64_div_unsigned(self.upload_vertices_, Int64{0x3e8}));
    self.stats_.relays = self.relays_;
    now_ms_10 = engine_now_ms();
    self.stats_.last_slice_ms = kern::formula([](Float now_ms_10, Float start) -> Float { return (now_ms_10 - start); }, now_ms_10, start);
    now_ms_11 = engine_now_ms();
    if (math_compare<">">(kern::formula([](Float now_ms_11, Float worst_at_) -> Float { return (now_ms_11 - worst_at_); }, now_ms_11, self.worst_at_), 2000.0)) {
      self.worst_ = 0.0;
      now_ms_12 = engine_now_ms();
      self.worst_at_ = now_ms_12;
      self.worst_step_ = "";
      self.worst_step_ms_ = 0.0;
    }
    self.worst_ = kern::formula([](Float worst_, Float last_slice_ms) -> Float { return kern::math::max(worst_, last_slice_ms); }, self.worst_, self.stats_.last_slice_ms);
    self.stats_.worst_ms = self.worst_;
    self.stats_.worst_step = self.worst_step_;
    self.stats_.worst_step_ms = self.worst_step_ms_;
  }

  void CityStream_walls_near(const CityStream& self, List<CityStream_Wall>& out, Vector p = Vector{0.0, 0.0, 0.0}, Float r = 0.0) {
    Bool skip = false;
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<U32>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<U32>{}, List<U32>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<U32>>{}, Set<Int64>{}, Map<Int64, List<U32>>{}, false, 0, 0, Map<Int64, List<U32>>{}, Set<Int64>{}, List<Int64>{}, 0, List<U8>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
    Bool skip_2 = false;
    Vector q = Vector{0.0, 0.0, 0.0};
    U32 edge = 0;
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
    for (auto it142 = flow_for(n142, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it142.next();) {
      skip = false;
      t = list_get<CityStream_Tile>(self.tiles_[n142.index], 0);
      if (logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x, Float r) -> Float { return ((x - r) - 200.0); }, t.bounds.x, r)), math_compare<">">(vector_break(p).x, kern::formula([](Float z, Float r) -> Float { return ((z + r) + 200.0); }, t.bounds.z, r))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y, Float r) -> Float { return ((y - r) - 200.0); }, t.bounds.y, r))), math_compare<">">(vector_break(p).y, kern::formula([](Float w, Float r) -> Float { return ((w + r) + 200.0); }, t.bounds.w, r)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it143 = flow_for(n143, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(t.features.buildings))))); it143.next();) {
            skip_2 = false;
            if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(t.features.buildings[n143.index].rings)), 0.0), math_compare<"==">(kern::to<Float>(list_length<Vector>(t.features.buildings[n143.index].rings[0].items)), 0.0))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              q = t.features.buildings[n143.index].rings[0].items[0];
              if (logic_or(math_compare<">">(kern::formula([](Float x, Float x_2) -> Float { return kern::math::abs((x - x_2)); }, vector_break(q).x, vector_break(p).x), kern::formula([](Float r) -> Float { return (r + 150.0); }, r)), math_compare<">">(kern::formula([](Float y, Float y_2) -> Float { return kern::math::abs((y - y_2)); }, vector_break(q).y, vector_break(p).y), kern::formula([](Float r) -> Float { return (r + 150.0); }, r)))) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                edge = 0;
                for (auto it144 = flow_for(n144, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector_row>(t.features.buildings[n143.index].rings))))); it144.next();) {
                  skip_3 = false;
                  ring = t.features.buildings[n143.index].rings[n144.index].items;
                  if (math_compare<"<">(kern::to<Float>(list_length<Vector>(ring)), 3.0)) {
                    skip_3 = true;
                  }
                  if (logic_not(skip_3)) {
                    ccw = math_compare<">">(signed_area(ring).Result, 0.0);
                    s = kern::formula([](Float ccw, Float index) -> Float { return ((ccw == (index == 0.0)) ? 1.0 : (-1.0)); }, kern::to<Float>(ccw), kern::to<Float>(n144.index));
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
                          list_add(out, struct_make<CityStream_Wall>(a, d, n, len, t.features.buildings[n143.index].base, t.features.buildings[n143.index].top, t.features.buildings[n143.index].seed, edge));
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
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<U32>{}, physics_Body{Int64{0x0}}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, List<U32>{}, List<U32>{}, false, false, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, Map<Int64, List<U32>>{}, Set<Int64>{}, Map<Int64, List<U32>>{}, false, 0, 0, Map<Int64, List<U32>>{}, Set<Int64>{}, List<Int64>{}, 0, List<U8>{}, 0, false, -1, physics_Body{Int64{0x0}}, false};
    Bool skip_2 = false;
    List<Vector> ring = List<Vector>{};
    Bool inside = false;
    Int i = 0;
    Int j = 0;
    Int was = 0;
    for (auto it145 = flow_for(n145, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it145.next();) {
      skip = false;
      t = list_get<CityStream_Tile>(self.tiles_[n145.index], 0);
      if (logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x) -> Float { return (x - 200.0); }, t.bounds.x)), math_compare<">">(vector_break(p).x, kern::formula([](Float z) -> Float { return (z + 200.0); }, t.bounds.z))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y) -> Float { return (y - 200.0); }, t.bounds.y))), math_compare<">">(vector_break(p).y, kern::formula([](Float w) -> Float { return (w + 200.0); }, t.bounds.w)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it146 = flow_for(n146, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(t.features.buildings))))); it146.next();) {
            skip_2 = false;
            if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(t.features.buildings[n146.index].rings)), 0.0), math_compare<"<">(kern::to<Float>(list_length<Vector>(t.features.buildings[n146.index].rings[0].items)), 3.0))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              ring = t.features.buildings[n146.index].rings[0].items;
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
    for (auto it147 = flow_for(n147, 0, 47); it147.next();) {
      skip = false;
      n148 = common::Rng_next(rng);
      l_at = kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, a); }, kern::to<Float>(n148.Result), kern::to<Float>(list_length<Lane>(self.lanes_))));
      if (logic_or(logic_not(self.lanes_[l_at].alive), math_compare<"<">(kern::to<Float>(list_length<Vector>(self.lanes_[l_at].points)), 2.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        n149 = common::Rng_next(rng);
        i = kern::to<Int>(kern::formula([](Float the_result, Float a) -> Float { return kern::math::mod(the_result, (a - 1.0)); }, kern::to<Float>(n149.Result), kern::to<Float>(list_length<Vector>(self.lanes_[l_at].points))));
        a = self.lanes_[l_at].points[i];
        b = self.lanes_[l_at].points[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))];
        n150 = common::Rng_unit(rng);
        t = n150.Result;
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
    for (auto it151 = flow_for(n151, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it151.next();) {
      skip = false;
      if (logic_and(logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n151.index][0].state, CityStream_Tile_State::ready)), logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n151.index][0].state, CityStream_Tile_State::collision)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it152 = flow_for(n152, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(self.tiles_[n151.index][0].features.buildings))))); it152.next();) {
          skip_2 = false;
          b = self.tiles_[n151.index][0].features.buildings[n152.index];
          if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(b.rings)), 0.0), math_compare<"<">(kern::to<Float>(list_length<Vector>(b.rings[0].items)), 3.0))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            x0 = 1000000000.0;
            y0 = 1000000000.0;
            x1 = -1000000000.0;
            y1 = -1000000000.0;
            for (auto it153 = flow_for(n153, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(b.rings[0].items))))); it153.next();) {
              q = b.rings[0].items[n153.index];
              x0 = kern::formula([](Float x0, Float x) -> Float { return kern::math::min(x0, x); }, x0, vector_break(q).x);
              y0 = kern::formula([](Float y0, Float y) -> Float { return kern::math::min(y0, y); }, y0, vector_break(q).y);
              x1 = kern::formula([](Float x1, Float x) -> Float { return kern::math::max(x1, x); }, x1, vector_break(q).x);
              y1 = kern::formula([](Float y1, Float y) -> Float { return kern::math::max(y1, y); }, y1, vector_break(q).y);
            }
            if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x0, Float radius) -> Float { return (x0 - radius); }, x0, radius)), math_compare<">">(vector_break(p).x, kern::formula([](Float x1, Float radius) -> Float { return (x1 + radius); }, x1, radius))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y0, Float radius) -> Float { return (y0 - radius); }, y0, radius))), math_compare<">">(vector_break(p).y, kern::formula([](Float y1, Float radius) -> Float { return (y1 + radius); }, y1, radius)))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              list_add(out, struct_make<CityStream_BuildingRef>(map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n151.index), kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n152.index)))));
            }
          }
        }
      }
    }
  }

  void CityStream_ready_tiles(const CityStream& self, List<Int64>& out) {
    for (auto it154 = flow_for(n154, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it154.next();) {
      if (self.tiles_[n154.index][0].prepared) {
        list_add(out, map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n154.index));
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
    for (auto it155 = flow_for(n155, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, List<CityStream_Tile>>(self.tiles_))))); it155.next();) {
      skip = false;
      if (logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n155.index][0].state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it156 = flow_for(n156, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Area>(self.tiles_[n155.index][0].features.areas))))); it156.next();) {
          skip_2 = false;
          a = self.tiles_[n155.index][0].features.areas[n156.index];
          if (logic_or(logic_or(logic_not(enum_equal<AreaKind>(a.kind, kind)), math_compare<"==">(kern::to<Float>(list_length<Vector_row>(a.rings)), 0.0)), math_compare<"<">(kern::to<Float>(list_length<Vector>(a.rings[0].items)), 3.0))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            x0 = 1000000000.0;
            y0 = 1000000000.0;
            x1 = -1000000000.0;
            y1 = -1000000000.0;
            for (auto it157 = flow_for(n157, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(a.rings[0].items))))); it157.next();) {
              q = a.rings[0].items[n157.index];
              x0 = kern::formula([](Float x0, Float x) -> Float { return kern::math::min(x0, x); }, x0, vector_break(q).x);
              y0 = kern::formula([](Float y0, Float y) -> Float { return kern::math::min(y0, y); }, y0, vector_break(q).y);
              x1 = kern::formula([](Float x1, Float x) -> Float { return kern::math::max(x1, x); }, x1, vector_break(q).x);
              y1 = kern::formula([](Float y1, Float y) -> Float { return kern::math::max(y1, y); }, y1, vector_break(q).y);
            }
            if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, kern::formula([](Float x0, Float radius) -> Float { return (x0 - radius); }, x0, radius)), math_compare<">">(vector_break(p).x, kern::formula([](Float x1, Float radius) -> Float { return (x1 + radius); }, x1, radius))), math_compare<"<">(vector_break(p).y, kern::formula([](Float y0, Float radius) -> Float { return (y0 - radius); }, y0, radius))), math_compare<">">(vector_break(p).y, kern::formula([](Float y1, Float radius) -> Float { return (y1 + radius); }, y1, radius)))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              list_add(out, struct_make<CityStream_AreaRef>(int64_add(int64_mul(map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n155.index), Int64{0x10000}), int64_of_int(n156.index)), map_key_at<Int64, List<CityStream_Tile>>(self.tiles_, n155.index), kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32(index); }, kern::to<Float>(n156.index)))));
            }
          }
        }
      }
    }
  }

  void CityStream_recollide(CityStream& self, CityStream_Tile& arg2) {
    self.near_.dirty = true;
    self.near_.steps = kNearSteps;
  }

  void CityStream_exclude_building(CityStream& self, Int64 tile = Int64{0x0}, I32 index = 0) {
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
  engine_geo_roads::GeoProjector_initResult n20;
  struct : kern::Node<21> { Int index = 0; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct : kern::Node<23> { Int index = 0; } n23;
  struct { Object<kern::FootprintStream> result = Object<kern::FootprintStream>{}; } n24;
  struct { Object<kern::RoadStream> result = Object<kern::RoadStream>{}; } n25;
  struct { Object<kern::BuildingBuilder> result = Object<kern::BuildingBuilder>{}; } n26;
  struct { Object<kern::RoadBuilder> result = Object<kern::RoadBuilder>{}; } n27;
  struct { Bool added = false; Int index = 0; } n28;
  struct { List<CityStream_Tile> value = List<CityStream_Tile>{}; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
  struct : kern::Node<31> { Int index = 0; } n31;
  struct : kern::Node<32> { Int index = 0; } n32;
  city_jobs_cpp::tile_inputResult n33;
  struct { Int result = 0; } n34;
  struct : kern::Node<35> { Int index = 0; } n35;
  struct : kern::Node<36> { Int index = 0; } n36;
  struct { Object<kern::FootprintStream> result = Object<kern::FootprintStream>{}; } n37;
  struct { Object<kern::RoadStream> result = Object<kern::RoadStream>{}; } n38;
  struct { Object<kern::BuildingBuilder> result = Object<kern::BuildingBuilder>{}; } n39;
  struct { Object<kern::RoadBuilder> result = Object<kern::RoadBuilder>{}; } n40;
  struct : kern::Node<41> { Int index = 0; } n41;
  struct : kern::Node<42> { Int index = 0; } n42;
  struct : kern::Node<43> { Int index = 0; } n43;
  struct : kern::Node<44> { Int index = 0; } n44;
  struct : kern::Node<45> { Int index = 0; } n45;
  tiles_cpp::clip_lineResult n46;
  struct : kern::Node<47> { Int index = 0; } n47;
  struct { List<Vector> value = List<Vector>{}; } n48;
  struct { Bool added = false; Int index = 0; } n49;
  struct : kern::Node<50> { Int index = 0; } n50;
  struct : kern::Node<51> { Int index = 0; } n51;
  struct { Bool added = false; Int index = 0; } n52;
  struct { Bool added = false; Int index = 0; } n53;
  struct { Bool removed = false; } n54;
  struct : kern::Node<55> { Int index = 0; } n55;
  struct { Bool result = false; } n56;
  struct { Bool removed = false; } n57;
  struct : kern::Node<58> { Int index = 0; } n58;
  struct : kern::Node<59> { Int index = 0; } n59;
  struct : kern::Node<60> { Int index = 0; } n60;
  struct : kern::Node<61> { Int index = 0; } n61;
  struct { Bool result = false; } n62;
  struct { Bool added = false; Int index = 0; } n63;
  struct { List<CityStream_Tile> value = List<CityStream_Tile>{}; } n64;
  struct : kern::Node<65> { Int index = 0; } n65;
  struct : kern::Node<66> { Int index = 0; } n66;
  struct : kern::Node<67> { Int index = 0; } n67;
  struct : kern::Node<68> { Int index = 0; } n68;
  struct : kern::Node<69> { Int index = 0; } n69;
  struct : kern::Node<70> { Int index = 0; } n70;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n71;
  struct { Object<kern::Earcut> result = Object<kern::Earcut>{}; } n72;
  struct : kern::Node<73> { Int index = 0; } n73;
  struct : kern::Node<74> { Int index = 0; } n74;
  struct : kern::Node<75> { Int index = 0; } n75;
  struct { List<U32> out = List<U32>{}; } n76;
  struct : kern::Node<77> { Int index = 0; } n77;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n78;
  CityStream_building_shapeResult n79;
  CityStream_build_nearResult n80;
  struct { WeldStats result = WeldStats{0, 0, 0, 0, false}; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n81;
  struct { Int result = 0; } n82;
  struct { Bool result = false; } n83;
  struct { Bool added = false; Int index = 0; } n84;
  struct : kern::Node<85> { Int index = 0; } n85;
  struct { List<U32> value = List<U32>{}; } n86;
  struct { Bool added = false; Int index = 0; } n87;
  struct { Bool added = false; Int index = 0; } n88;
  CityStream_building_shapeResult n89;
  struct : kern::Node<90> { Int index = 0; } n90;
  struct : kern::Node<91> { Int index = 0; } n91;
  struct { Bool added = false; Int index = 0; } n92;
  struct : kern::Node<93> { Int index = 0; } n93;
  struct : kern::Node<94> { Int index = 0; } n94;
  CityStream_damage_buildingResult n95;
  struct : kern::Node<96> { Int index = 0; } n96;
  struct { Bool added = false; Int index = 0; } n97;
  struct : kern::Node<98> { Int index = 0; } n98;
  struct : kern::Node<99> { Int index = 0; } n99;
  struct { Bool added = false; Int index = 0; } n100;
  struct : kern::Node<101> { Int index = 0; } n101;
  struct { Object<kern::FootprintStream> result = Object<kern::FootprintStream>{}; } n102;
  struct : kern::Node<103> { Int index = 0; } n103;
  struct { Bool removed = false; } n104;
  struct : kern::Node<105> { Int index = 0; } n105;
  city_jobs_cpp::buildings_inputResult n106;
  struct { Int result = 0; } n107;
  struct { Bool result = false; List<U8> out = List<U8>{}; } n108;
  struct : kern::Node<109> { Int index = 0; } n109;
  struct { List<U32> value = List<U32>{}; } n110;
  struct { Bool added = false; Int index = 0; } n111;
  city_jobs_cpp::read_chunkResult n112;
  struct : kern::Node<113> { Int index = 0; } n113;
  struct : kern::Node<114> { Int index = 0; } n114;
  struct { Bool result = false; } n115;
  struct : kern::Node<116> { Int index = 0; } n116;
  struct { Bool added = false; Int index = 0; } n117;
  struct { Bool result = false; List<U8> out = List<U8>{}; } n118;
  city_jobs_cpp::read_featuresResult n119;
  CityStream_workResult n120;
  CityStream_workResult n121;
  CityStream_workResult n122;
  struct { Bool result = false; } n123;
  struct { engine_road_builder_chunks_place<List<GeoChunk>> result = {}; } n124;
  struct { GeoChunk value = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}}; } n125;
  struct : kern::Node<126> { Int index = 0; } n126;
  CityStream_workResult n127;
  struct : kern::Node<128> { Int index = 0; } n128;
  struct { Bool result = false; } n129;
  struct { List<U32> value = List<U32>{}; } n130;
  CityStream_finish_cellsResult n131;
  struct { List<U32> value = List<U32>{}; } n132;
  struct : kern::Node<133> { Int index = 0; } n133;
  struct : kern::Node<134> { Int index = 0; } n134;
  CityStream_ring_boundsResult n135;
  struct : kern::Node<136> { Int index = 0; } n136;
  CityStream_ring_boundsResult n137;
  struct : kern::Node<138> { Int index = 0; } n138;
  struct : kern::Node<139> { Int index = 0; } n139;
  CityStream_workResult n140;
  struct : kern::Node<141> { Int index = 0; } n141;
  struct : kern::Node<142> { Int index = 0; } n142;
  struct : kern::Node<143> { Int index = 0; } n143;
  struct : kern::Node<144> { Int index = 0; } n144;
  struct : kern::Node<145> { Int index = 0; } n145;
  struct : kern::Node<146> { Int index = 0; } n146;
  struct : kern::Node<147> { Int index = 0; } n147;
  common::Rng_nextResult n148;
  common::Rng_nextResult n149;
  common::Rng_unitResult n150;
  struct : kern::Node<151> { Int index = 0; } n151;
  struct : kern::Node<152> { Int index = 0; } n152;
  struct : kern::Node<153> { Int index = 0; } n153;
  struct : kern::Node<154> { Int index = 0; } n154;
  struct : kern::Node<155> { Int index = 0; } n155;
  struct : kern::Node<156> { Int index = 0; } n156;
  struct : kern::Node<157> { Int index = 0; } n157;
};
