struct city_stream_cpp : virtual kern::Library, virtual city_jobs_cpp, virtual city_stream, virtual common, virtual engine_math_math, virtual terrain, virtual terrain_cpp, virtual tiles, virtual tiles_cpp {
  // ---- types ----
  KERN_STRUCT(CityStream_select_cells_Pick) { f32 d = 0.0f; u32 tile = 0; u64 cell = 0; u32 n = 0; };
  KERN_STRUCT(CityStream_update_collision_Pick) { f32 d2 = 0.0f; u64 tile = 0; u32 i = 0; };

  // ---- variables ----
  /// the building mesh chunks (BuildingOptions::chunk_size)
  KERN_CONST f32 kChunk = 160.0f;
  /// OpenFreeMap's planet tiles go up to z14 (overzoomed beyond)
  KERN_CONST i32 kTileZoom = 14;
  KERN_CONST u32 kTile = 1;
  /// The map features of a tile that is not loaded: none.
  KERN_CONST MapFeatures kNoFeatures = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
  /// 250 ms at the world's 240 Hz (world_building_collisions.mjs: 250 ms)
  KERN_CONST u32 kNearSteps = 60;
  /// buildings of a new set a step (480 in 30 steps, an eighth of a second)
  KERN_CONST u32 kNearPerStep = 16;
  /// vertex locations 14 and 15
  KERN_CONST u32 kMaxExtensionAttributes = 2;
  KERN_CONST u32 kBuildings = 2;
  KERN_CONST Fixed<Text, 9> kStateName = List<Text>{"requested", "decoding", "terrain_wait", "building", "roads", "areas", "collision", "ready", "failed"};

  // ---- functions ----
  struct CityStream_ground_heightResult { f32 Result = 0.0f; };
  /// Ground height: the terrain's static surface (elevation, water basins, craters).
  KERN_PURE CityStream_ground_heightResult CityStream_ground_height(const CityStream& self, f32 x = 0.0f, f32 y = 0.0f) {
    return {terrain_cpp::Terrain_height(self.terrain_, x, y).Result};
  }

  struct konstanz_style_hexResult { Rgb Result = Rgb{1.0, 1.0, 1.0}; };
  /// world_city_buildings.mjs WALLS / ROOFS (sRGB) and the cornice mix with #f4efe4 (0.45)
  KERN_PURE konstanz_style_hexResult konstanz_style_hex(u32 h = 0) {
    return {struct_make<Rgb>(kern::formula<u32>([](auto h) { return (num_cast<f64, u32>(((h >> 16) & 255)) / 255.0); }, h), kern::formula<u32>([](auto h) { return (num_cast<f64, u32>(((h >> 8) & 255)) / 255.0); }, h), kern::formula<u32>([](auto h) { return (num_cast<f64, u32>((h & 255)) / 255.0); }, h))};
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
  KERN_PURE road_palette_rgbResult road_palette_rgb(u32 h = 0) {
    return {struct_make<Rgb>(kern::formula<u32>([](auto h) { return (num_cast<f64, u32>(((h >> 16) & 255)) / 255.0); }, h), kern::formula<u32>([](auto h) { return (num_cast<f64, u32>(((h >> 8) & 255)) / 255.0); }, h), kern::formula<u32>([](auto h) { return (num_cast<f64, u32>((h & 255)) / 255.0); }, h))};
  }

  struct road_paletteResult { List<Rgb> Result = List<Rgb>{}; };
  KERN_PURE road_paletteResult road_palette() {
    return {list_make<Rgb, "6">(road_palette_rgb(3948356).Result, road_palette_rgb(3422012).Result, road_palette_rgb(15131868).Result, road_palette_rgb(8815484).Result, road_palette_rgb(3104646).Result, road_palette_rgb(10130570).Result)};
  }

  struct area_colorResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE area_colorResult area_color(AreaKind k = AreaKind::water) {
    Vector hex_color = Vector{0.0, 0.0, 0.0};
    Vector hex_color_2 = Vector{0.0, 0.0, 0.0};
    Vector hex_color_3 = Vector{0.0, 0.0, 0.0};
    Vector hex_color_4 = Vector{0.0, 0.0, 0.0};
    Vector hex_color_5 = Vector{0.0, 0.0, 0.0};
    Vector hex_color_6 = Vector{0.0, 0.0, 0.0};
    switch (flow_switch_enum<AreaKind>(k)) {
      case AreaKind::water: {
        hex_color = engine_hex_color(3104646);
        return {hex_color};
      } break;
      case AreaKind::park: {
        hex_color_2 = engine_hex_color(5205557);
        return {hex_color_2};
      } break;
      case AreaKind::wood: {
        hex_color_3 = engine_hex_color(3823401);
        return {hex_color_3};
      } break;
      case AreaKind::grass: {
        hex_color_4 = engine_hex_color(5929532);
        return {hex_color_4};
      } break;
      case AreaKind::sand: {
        hex_color_5 = engine_hex_color(13219983);
        return {hex_color_5};
      } break;
      case AreaKind::pitch: {
        hex_color_6 = engine_hex_color(5208629);
        return {hex_color_6};
      } break;
    }
  }

  struct signed_areaResult { f64 Result = 0.0; };
  KERN_PURE signed_areaResult signed_area(const List<Vector>& r) {
    f64 a = 0.0;
    u64 i = 0;
    u64 j = 0;
    u64 was = 0;
    a = 0.0;
    i = 0;
    j = kern::formula<u64>([](auto a) { return (a - 1); }, num_cast<u64, i64>(list_length<Vector>(r)));
    while (loop(math_compare<"<", u64>(i, num_cast<u64, i64>(list_length<Vector>(r))))) {
      a += kern::formula<f32, f32, f32, f32>([](auto x, auto y, auto x_2, auto y_2) { return ((num_cast<f64, f32>(x) * num_cast<f64, f32>(y)) - (num_cast<f64, f32>(x_2) * num_cast<f64, f32>(y_2))); }, vector_break(r[num_cast<i64, u64>(j)]).x, vector_break(r[num_cast<i64, u64>(i)]).y, vector_break(r[num_cast<i64, u64>(i)]).x, vector_break(r[num_cast<i64, u64>(j)]).y);
      was = i;
      i += 1;
      j = was;
    }
    return {kern::formula<f64>([](auto a) { return (a * 0.5); }, a)};
  }

  struct rect_distance_toResult { f32 Result = 0.0f; };
  KERN_PURE rect_distance_toResult rect_distance_to(const Vec4& r, f32 x = 0.0f, f32 y = 0.0f) {
    f32 dx = 0.0f;
    f32 dy = 0.0f;
    dx = kern::formula<f32, f32, f32>([](auto x, auto x_2, auto z) { return fmax(fmax((x - x_2), (x_2 - z)), 0.0f); }, r.x, x, r.z);
    dy = kern::formula<f32, f32, f32>([](auto y, auto y_2, auto w) { return fmax(fmax((y - y_2), (y_2 - w)), 0.0f); }, r.y, y, r.w);
    return {kern::formula<f32, f32>([](auto dx, auto dy) { return hypot(dx, dy); }, dx, dy)};
  }

  struct cell_keyResult { u64 Result = 0; };
  KERN_PURE cell_keyResult cell_key(i32 x = 0, i32 y = 0) {
    return {kern::formula<i32, i32>([](auto x, auto y) { return ((num_cast<u64, u32>(num_cast<u32, i32>(x)) << 32) | num_cast<u64, u32>(num_cast<u32, i32>(y))); }, x, y)};
  }

  struct cell_ofResult { u64 Result = 0; };
  KERN_PURE cell_ofResult cell_of(Vector c = Vector{0.0, 0.0, 0.0}) {
    return {cell_key(kern::formula<f32, f32>([](auto x, auto kChunk) { return num_cast<i32, f32>(floor((x / kChunk))); }, vector_break(c).x, kChunk), kern::formula<f32, f32>([](auto y, auto kChunk) { return num_cast<i32, f32>(floor((y / kChunk))); }, vector_break(c).y, kChunk)).Result};
  }

  struct building_optionsResult { BuildingOptions Result = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}}; };
  KERN_PURE building_optionsResult building_options() {
    BuildingOptions bo = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
    bo = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
    bo.chunk_size = num_cast<f64, f32>(kChunk);
    bo.max_buildings = 12000;
    bo.style = konstanz_style().Result;
    return {bo};
  }

  /// world_city_buildings.mjs: at most MAX_VERTICES 96 per ring; the centre picks the chunk
  void put_footprint(const MapFeatures_Building& b, List<Vector>& thin, Object<kern::FootprintStream> fs = Object<kern::FootprintStream>{}, Vector c = Vector{0.0, 0.0, 0.0}) {
    Object<kern::FootprintStream> fs_ = Object<kern::FootprintStream>{};
    bool skip = false;
    u64 step = 0;
    u64 k = 0;
    fs_ = fs;
    engine_footprint_stream_begin_2(fs_, b.seed, num_cast<f64, f32>(b.base), num_cast<f64, f32>(b.top), num_cast<f64, f32>(vector_break(c).x), num_cast<f64, f32>(vector_break(c).y));
    for (auto it10 = flow_for(n10, 0, math_sub<i64>(list_length<Vector_row>(b.rings), 1)); it10.next();) {
      skip = false;
      if (math_compare<"<=", u64>(num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(b.rings[n10.index]).items)), 96)) {
        engine_footprint_stream_ring(fs_, struct_break<Vector_row>(b.rings[n10.index]).items, num_cast<u32, i64>(list_length<Vector>(struct_break<Vector_row>(b.rings[n10.index]).items)));
        skip = true;
      }
      if (logic_not(skip)) {
        step = kern::formula<u64>([](auto a) { return ((a + 95) / 96); }, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(b.rings[n10.index]).items)));
        list_clear(thin);
        k = 0;
        while (loop(math_compare<"<", u64>(k, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(b.rings[n10.index]).items))))) {
          list_add(thin, b.rings[n10.index].items[num_cast<i64, u64>(k)]);
          k += step;
        }
        engine_footprint_stream_ring(fs_, thin, num_cast<u32, i64>(list_length<Vector>(thin)));
      }
    }
  }

  void CityStream_remove_drop(CityStream& self, const List<u32>& list) {
    u32 id = 0;
    bool remove = false;
    for (auto it12 = flow_for(n12, 0, math_sub<i64>(list_length<u32>(list), 1)); it12.next();) {
      id = list[n12.index];
      engine_renderer_remove(n13, self.r_, id);
      remove = n13.result;
    }
  }

  void remove_cancel(u32& job) {
    if (num_cast<bool, u32>(job)) {
      engine_jobs_cancel(job);
      job = 0;
    }
  }

  void CityStream_remove(CityStream& self, CityStream_Tile& t) {
    u32 id = 0;
    bool remove = false;
    bool valid = false;
    bool valid_2 = false;
    bool both = false;
    bool body_valid = false;
    bool body_valid_2 = false;
    bool both_2 = false;
    bool valid_3 = false;
    bool valid_4 = false;
    bool both_3 = false;
    bool body_valid_3 = false;
    bool body_valid_4 = false;
    bool both_4 = false;
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
      for (auto it14 = flow_for(n14, 0, math_sub<i64>(map_length<u64, List<u32>>(t.cell_old), 1)); it14.next();) {
        for (auto it15 = flow_for(n15, 0, math_sub<i64>(list_length<u32>(t.cell_old[n14.index]), 1)); it15.next();) {
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
      self.furniture_serial_ = kern::formula<u32>([](auto furniture_serial_) { return (furniture_serial_ + 1); }, self.furniture_serial_);
    }
    terrain_cpp::Terrain_remove_tints(self.terrain_, tiles::TileKey_hash(t.key).Result);
    terrain_cpp::Terrain_remove_corridors(self.terrain_, tiles::TileKey_hash(t.key).Result);
    both = object_valid<kern::physics::World>(self.world_);
    if (both) {
      valid_2 = engine_body_valid(t.body);
      both = valid_2;
    }
    both_2 = both;
    if (both_2) {
      body_valid_2 = engine_world_body_valid(self.world_, t.body);
      both_2 = body_valid_2;
    }
    if (both_2) {
      engine_world_destroy_body(self.world_, t.body);
    }
    t.body = struct_make<physics_Body>(0);
    both_3 = object_valid<kern::physics::World>(self.world_);
    if (both_3) {
      valid_4 = engine_body_valid(t.old_body);
      both_3 = valid_4;
    }
    both_4 = both_3;
    if (both_4) {
      body_valid_4 = engine_world_body_valid(self.world_, t.old_body);
      both_4 = body_valid_4;
    }
    if (both_4) {
      engine_world_destroy_body(self.world_, t.old_body);
    }
    t.old_body = struct_make<physics_Body>(0);
    for (auto it17 = flow_for(n17, 0, math_sub<i64>(list_length<Lane>(self.lanes_), 1)); it17.next();) {
      if (math_compare<"==", u64>(self.lanes_[n17.index].tile, tiles::TileKey_hash(t.key).Result)) {
        self.lanes_[n17.index].alive = false;
      }
    }
  }

  void CityStream_clear(CityStream& self) {
    i64 count = 0;
    i64 at = 0;
    physics_Body item = physics_Body{0};
    physics_Body item_2 = physics_Body{0};
    Fixed<physics_Body, 2> items = Fixed<physics_Body, 2>{};
    physics_Body b = physics_Body{0};
    bool valid = false;
    bool body_valid = false;
    bool body_valid_2 = false;
    bool both = false;
    for (auto it11 = flow_for(n11, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it11.next();) {
      CityStream_remove(self, self.tiles_[n11.index][0]);
    }
    count = map_length<u64, List<CityStream_Tile>>(self.tiles_);
    for (auto it18 = flow_for(n18, 0, math_sub<i64>(count, 1)); it18.next();) {
      at = math_sub<i64>(math_sub<i64>(count, 1), n18.index);
      if (math_compare<">", i64>(list_length<CityStream_Tile>(self.tiles_[at]), 0)) {
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
      for (auto it19 = flow_for(n19, 0, math_sub<i64>(list_length<physics_Body>(items), 1)); it19.next();) {
        b = items[n19.index];
        valid = engine_body_valid(b);
        both = valid;
        if (both) {
          body_valid_2 = engine_world_body_valid(self.world_, b);
          both = body_valid_2;
        }
        if (both) {
          engine_world_destroy_body(self.world_, b);
        }
      }
    }
    self.near_ = struct_make<CityStream_NearCollision>(struct_make<physics_Body>(0), struct_make<physics_Body>(0), Vector{1e+09, 1e+09, 0.0}, Vector{1e+09, 1e+09, 0.0}, 1000000, List<u64_u32_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0);
    map_clear(self.damage_);
    list_clear(self.damage_log_);
    self.vis_at_ = Vector{1e+09, 1e+09, 0.0};
    self.vis_ms_ = -1000000000.0;
  }

  void CityStream_start(CityStream& self, Object<kern::Renderer> renderer = Object<kern::Renderer>{}, Object<kern::physics::World> world = Object<kern::physics::World>{}, f64 lon = 0.0, f64 lat = 0.0, bool dem = false) {
    f64 now_ms = 0.0;
    bool init = false;
    CityStream_clear(self);
    self.r_ = renderer;
    self.world_ = world;
    now_ms = engine_now_ms();
    self.start_ms_ = now_ms;
    engine_geo_projector_init(n20, self.proj_, lon, lat);
    init = n20.result;
    self.proj_ = n20.self;
    terrain_cpp::Terrain_start(self.terrain_, self.proj_, renderer, world, dem);
    self.origin_tile_ = tiles_cpp::tile_at(lon, lat, kTileZoom).Result;
    self.offline_ = false;
    self.real_tiles_ = 0;
    self.terrain_.corridors_expected = true;
  }

  struct request_around_matchResult { bool Result = false; };
  KERN_PURE request_around_matchResult request_around_match(const i32_TileKey_pair& a, const i32_TileKey_pair& b) {
    return {math_compare<"<", i32>(a.first, b.first)};
  }

  void CityStream_request_around(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, i32 ring = 0) {
    f64 lon = 0.0;
    f64 lat = 0.0;
    TileKey c = TileKey{14, 0, 0};
    List<i32_TileKey_pair> want = List<i32_TileKey_pair>{};
    bool skip = false;
    List<CityStream_Tile> t = List<CityStream_Tile>{};
    CityStream_Tile made = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<u32>{}, physics_Body{0}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0f, 0.0f, 0.0f, 0.0f}, List<u32>{}, List<u32>{}, false, false, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, Map<u64, List<u32>>{}, Set<u64>{}, Map<u64, List<u32>>{}, false, 0, 0, Map<u64, List<u32>>{}, Set<u64>{}, List<u64>{}, 0, List<u8>{}, 0, false, -1, physics_Body{0}, false};
    Object<kern::FootprintStream> footprint_stream = Object<kern::FootprintStream>{};
    Object<kern::RoadStream> road_stream = Object<kern::RoadStream>{};
    Object<kern::BuildingBuilder> building_builder = Object<kern::BuildingBuilder>{};
    Object<kern::RoadBuilder> road_builder = Object<kern::RoadBuilder>{};
    f64 lo = 0.0;
    f64 la = 0.0;
    f64 x = 0.0;
    f64 y = 0.0;
    f64 x0 = 0.0;
    f64 y0 = 0.0;
    f64 x1 = 0.0;
    f64 y1 = 0.0;
    u64 key = 0;
    List<CityStream_Tile> put = List<CityStream_Tile>{};
    if (self.offline_) {
      return;
    }
    lon = 0.0;
    lat = 0.0;
    tiles_cpp::unproject(self.proj_, lon, lat, num_cast<f64, f32>(vector_break(focus).x), num_cast<f64, f32>(vector_break(focus).y));
    c = tiles_cpp::tile_at(lon, lat, kTileZoom).Result;
    want = List<i32_TileKey_pair>{};
    for (auto it21 = flow_for(n21, num_cast<i64, i32>(kern::formula<i32>([](auto ring) { return (-ring); }, ring)), num_cast<i64, i32>(ring)); it21.next();) {
      for (auto it22 = flow_for(n22, num_cast<i64, i32>(kern::formula<i32>([](auto ring) { return (-ring); }, ring)), num_cast<i64, i32>(ring)); it22.next();) {
        list_add(want, struct_make<i32_TileKey_pair>(kern::formula<i64, i64>([](auto index, auto index_2) { return ((num_cast<i32, i64>(index) * num_cast<i32, i64>(index)) + (num_cast<i32, i64>(index_2) * num_cast<i32, i64>(index_2))); }, n22.index, n21.index), struct_make<TileKey>(c.z, kern::formula<i32, i64>([](auto x, auto index) { return (x + num_cast<i32, i64>(index)); }, c.x, n22.index), kern::formula<i32, i64>([](auto y, auto index) { return (y + num_cast<i32, i64>(index)); }, c.y, n21.index))));
      }
    }
    list_sort<&city_stream_cpp::request_around_match>(*this, want);
    for (auto it23 = flow_for(n23, 0, math_sub<i64>(list_length<i32_TileKey_pair>(want), 1)); it23.next();) {
      skip = false;
      if (logic_not(logic_not(map_contains<u64, List<CityStream_Tile>>(self.tiles_, tiles::TileKey_hash(want[n23.index].second).Result)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        made = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<u32>{}, physics_Body{0}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0f, 0.0f, 0.0f, 0.0f}, List<u32>{}, List<u32>{}, false, false, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, Map<u64, List<u32>>{}, Set<u64>{}, Map<u64, List<u32>>{}, false, 0, 0, Map<u64, List<u32>>{}, Set<u64>{}, List<u64>{}, 0, List<u8>{}, 0, false, -1, physics_Body{0}, false};
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
        engine_geo_projector_project(n28, self.proj_, lo, la, x, y);
        x = n28.x;
        y = n28.y;
        t[0].center = vector_make(num_cast<f32, f64>(x), num_cast<f32, f64>(y), 0.0f);
        x0 = 0.0;
        y0 = 0.0;
        x1 = 0.0;
        y1 = 0.0;
        tiles_cpp::tile_lonlat(want[n23.index].second, lo, la, 0.0, 1.0);
        engine_geo_projector_project(n29, self.proj_, lo, la, x0, y0);
        x0 = n29.x;
        y0 = n29.y;
        tiles_cpp::tile_lonlat(want[n23.index].second, lo, la, 1.0, 0.0);
        engine_geo_projector_project(n30, self.proj_, lo, la, x1, y1);
        x1 = n30.x;
        y1 = n30.y;
        t[0].bounds = engine_math_math::Vec4_new(num_cast<f32, f64>(x0), num_cast<f32, f64>(y0), num_cast<f32, f64>(x1), num_cast<f32, f64>(y1)).Result;
        key = tiles::TileKey_hash(want[n23.index].second).Result;
        map_add(n31, self.tiles_, key);
        var_take(n32, t);
        put = n32.value;
        if (math_compare<">", i64>(list_length<CityStream_Tile>(self.tiles_[n31.index]), 0)) {
          if (object_valid<kern::RoadBuilder>(self.tiles_[n31.index][0].road_builder)) {
            engine_road_builder_release(self.tiles_[n31.index][0].road_builder);
          }
          if (object_valid<kern::BuildingBuilder>(self.tiles_[n31.index][0].buildings)) {
            engine_building_builder_release(self.tiles_[n31.index][0].buildings);
          }
          if (object_valid<kern::RoadStream>(self.tiles_[n31.index][0].roads)) {
            engine_road_stream_release(self.tiles_[n31.index][0].roads);
          }
          if (object_valid<kern::FootprintStream>(self.tiles_[n31.index][0].footprints)) {
            engine_footprint_stream_release(self.tiles_[n31.index][0].footprints);
          }
        }
        self.tiles_[n31.index] = put;
        list_add(self.requests_, want[n23.index].second);
        if (math_compare<">", i64>(list_length<CityStream_Tile>(t), 0)) {
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

  struct CityStream_ring_boundsResult { Vec4 Result = Vec4{0.0f, 0.0f, 0.0f, 0.0f}; };
  CityStream_ring_boundsResult CityStream_ring_bounds(const CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, i32 r = 0) {
    f64 lon = 0.0;
    f64 lat = 0.0;
    f64 x0 = 0.0;
    f64 y0 = 0.0;
    f64 x1 = 0.0;
    f64 y1 = 0.0;
    TileKey c = TileKey{14, 0, 0};
    TileKey a = TileKey{14, 0, 0};
    TileKey a_2 = TileKey{14, 0, 0};
    lon = 0.0;
    lat = 0.0;
    x0 = 0.0;
    y0 = 0.0;
    x1 = 0.0;
    y1 = 0.0;
    tiles_cpp::unproject(self.proj_, lon, lat, num_cast<f64, f32>(vector_break(focus).x), num_cast<f64, f32>(vector_break(focus).y));
    c = tiles_cpp::tile_at(lon, lat, kTileZoom).Result;
    a = struct_make<TileKey>(c.z, kern::formula<i32, i32>([](auto x, auto r) { return (x - r); }, c.x, r), kern::formula<i32, i32>([](auto y, auto r) { return (y + r); }, c.y, r));
    tiles_cpp::tile_lonlat(a, lon, lat, 0.0, 1.0);
    engine_geo_projector_project(n33, self.proj_, lon, lat, x0, y0);
    x0 = n33.x;
    y0 = n33.y;
    a_2 = struct_make<TileKey>(c.z, kern::formula<i32, i32>([](auto x, auto r) { return (x + r); }, c.x, r), kern::formula<i32, i32>([](auto y, auto r) { return (y - r); }, c.y, r));
    tiles_cpp::tile_lonlat(a_2, lon, lat, 1.0, 0.0);
    engine_geo_projector_project(n34, self.proj_, lon, lat, x1, y1);
    x1 = n34.x;
    y1 = n34.y;
    return {engine_math_math::Vec4_new(num_cast<f32, f64>(x0), num_cast<f32, f64>(y0), num_cast<f32, f64>(x1), num_cast<f32, f64>(y1)).Result};
  }

  void CityStream_unload_far(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}) {
    f64 lon = 0.0;
    f64 lat = 0.0;
    TileKey c = TileKey{14, 0, 0};
    List<TileKey> mates = List<TileKey>{};
    f64 lo = 0.0;
    f64 la = 0.0;
    i64 it = 0;
    i32 d = 0;
    bool near_mate = false;
    i64 at = 0;
    lon = 0.0;
    lat = 0.0;
    tiles_cpp::unproject(self.proj_, lon, lat, num_cast<f64, f32>(vector_break(focus).x), num_cast<f64, f32>(vector_break(focus).y));
    c = tiles_cpp::tile_at(lon, lat, kTileZoom).Result;
    mates = List<TileKey>{};
    for (auto it35 = flow_for(n35, 0, math_sub<i64>(list_length<Vector>(self.extra_foci), 1)); it35.next();) {
      lo = 0.0;
      la = 0.0;
      tiles_cpp::unproject(self.proj_, lo, la, num_cast<f64, f32>(vector_break(self.extra_foci[n35.index]).x), num_cast<f64, f32>(vector_break(self.extra_foci[n35.index]).y));
      list_add(mates, tiles_cpp::tile_at(lo, la, kTileZoom).Result);
    }
    it = 0;
    while (loop(logic_not(math_compare<"==", i64>(it, map_length<u64, List<CityStream_Tile>>(self.tiles_))))) {
      d = kern::formula<i32, i32, i32, i32>([](auto a, auto x, auto a_2, auto y) { return max(abs((a - x)), abs((a_2 - y))); }, self.tiles_[it][0].key.x, c.x, self.tiles_[it][0].key.y, c.y);
      near_mate = false;
      for (auto it36 = flow_for(n36, 0, math_sub<i64>(list_length<TileKey>(mates), 1)); it36.next();) {
        near_mate = logic_or(near_mate, math_compare<"<=", i32>(kern::formula<i32, i32, i32, i32>([](auto a, auto a_2, auto a_3, auto a_4) { return max(abs((a - a_2)), abs((a_3 - a_4))); }, self.tiles_[it][0].key.x, mates[n36.index].x, self.tiles_[it][0].key.y, mates[n36.index].y), kern::formula<i32>([](auto mate_ring) { return (mate_ring + 1); }, self.mate_ring)));
      }
      if (logic_and(logic_and(logic_not(self.tiles_[it][0].procedural), math_compare<">", i32>(d, kern::formula<i32>([](auto ring) { return (ring + 1); }, self.ring))), logic_not(near_mate))) {
        CityStream_remove(self, self.tiles_[it][0]);
        at = it;
        if (math_compare<">", i64>(list_length<CityStream_Tile>(self.tiles_[at]), 0)) {
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
    f64 now_ms = 0.0;
    s = "[";
    for (auto it37 = flow_for(n37, 0, math_sub<i64>(list_length<TileKey>(self.requests_), 1)); it37.next();) {
      b = "";
      b = text_format<"%s[%d,%d,%d]">(63, logic_select<Text>(num_cast<bool, u64>(num_cast<u64, i64>(n37.index)), ",", ""), num_cast<i64, i32>(self.requests_[n37.index].z), num_cast<i64, i32>(self.requests_[n37.index].x), num_cast<i64, i32>(self.requests_[n37.index].y));
      s = text_append(s, b, "");
    }
    if (logic_and(logic_not(math_compare<"==", i64>(list_length<TileKey>(self.requests_), 0)), math_compare<"<", f64>(self.ms_.t_request, 0.0))) {
      now_ms = engine_now_ms();
      self.ms_.t_request = kern::formula<f64, f64>([](auto now_ms, auto start_ms_) { return (now_ms - start_ms_); }, now_ms, self.start_ms_);
    }
    list_clear(self.requests_);
    return {text_append(s, "]", "")};
  }

  void CityStream_provide(CityStream& self, const TileKey& key, List<u8> bytes = List<u8>{}) {
    i64 it = 0;
    f64 now_ms = 0.0;
    u32 arg = 0;
    u32 submit = 0;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<CityStream_Tile>>(self.tiles_, tiles::TileKey_hash(key).Result).index, map_length<u64, List<CityStream_Tile>>(self.tiles_));
    if (logic_or(math_compare<"==", i64>(it, map_length<u64, List<CityStream_Tile>>(self.tiles_)), logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[it][0].state, CityStream_Tile_State::requested)))) {
      return;
    }
    if (logic_and(tiles::TileKey_equal(key, self.origin_tile_).Result, math_compare<"<", f64>(self.ms_.t_data, 0.0))) {
      now_ms = engine_now_ms();
      self.ms_.t_data = kern::formula<f64, f64>([](auto now_ms, auto start_ms_) { return (now_ms - start_ms_); }, now_ms, self.start_ms_);
    }
    arg = kTile;
    n38 = city_jobs_cpp::tile_input(key, self.proj_.lon0, self.proj_.lat0, bytes);
    engine_jobs_submit(n39, arg, n38.Result);
    submit = n39.result;
    self.tiles_[it][0].tile_job = submit;
    self.tiles_[it][0].state = CityStream_Tile_State::decoding;
  }

  void CityStream_feed_terrain(CityStream& self, CityStream_Tile& t) {
    u64 i = 0;
    for (auto it46 = flow_for(n46, 0, math_sub<i64>(list_length<MapFeatures_Area>(t.features.areas), 1)); it46.next();) {
      if (enum_equal<AreaKind>(t.features.areas[n46.index].kind, AreaKind::water)) {
        terrain_cpp::Terrain_add_water_polygon(self.terrain_, t.features.areas[n46.index].rings);
      } else {
        terrain_cpp::Terrain_add_tint(self.terrain_, t.features.areas[n46.index].rings, tiles::TileKey_hash(t.key).Result, area_color(t.features.areas[n46.index].kind).Result);
      }
    }
    for (auto it47 = flow_for(n47, 0, math_sub<i64>(list_length<MapFeatures_Road>(t.features.roads), 1)); it47.next();) {
      if (tiles_cpp::road_is_water(t.features.roads[n47.index].cls).Result) {
        terrain_cpp::Terrain_add_waterway(self.terrain_, t.features.roads[n47.index].points, tiles_cpp::road_width(t.features.roads[n47.index].cls).Result, kern::formula<bool, f32, bool, f32, f32>([](auto a, auto a6f, auto a_2, auto a5f, auto a1f) { return (a ? a6f : (a_2 ? a5f : a1f)); }, enum_equal<RoadClass>(t.features.roads[n47.index].cls, RoadClass::river), 1.6f, enum_equal<RoadClass>(t.features.roads[n47.index].cls, RoadClass::canal), 0.5f, 1.1f));
      } else if (t.features.roads[n47.index].bridge) {
        i = 0;
        while (loop(math_compare<"<", u64>(kern::formula<u64>([](auto i) { return (i + 1); }, i), num_cast<u64, i64>(list_length<Vector>(t.features.roads[n47.index].points))))) {
          terrain_cpp::Terrain_add_bridge(self.terrain_, t.features.roads[n47.index].points[num_cast<i64, u64>(i)], t.features.roads[n47.index].points[num_cast<i64, u64>(kern::formula<u64>([](auto i) { return (i + 1); }, i))], tiles_cpp::road_width(t.features.roads[n47.index].cls).Result);
          i += 1;
        }
      }
    }
  }

  struct select_cells_way_distanceResult { f32 Result = 0.0f; };
  KERN_PURE select_cells_way_distanceResult select_cells_way_distance(f32 ab2 = 0.0f, Vector a = Vector{0.0, 0.0, 0.0}, Vector ab = Vector{0.0, 0.0, 0.0}, Vector p = Vector{0.0, 0.0, 0.0}) {
    f32 t = 0.0f;
    t = kern::formula<f32, f32, f32>([](auto ab2, auto the_result, auto a0f) { return ((ab2 > 1.0f) ? the_result : a0f); }, ab2, common::clampf(kern::formula<f32, f32, f32, f32, f32, f32, f32>([](auto x, auto x_2, auto x_3, auto y, auto y_2, auto y_3, auto ab2) { return ((((x - x_2) * x_3) + ((y - y_2) * y_3)) / ab2); }, vector_break(p).x, vector_break(a).x, vector_break(ab).x, vector_break(p).y, vector_break(a).y, vector_break(ab).y, ab2), 0.0f, 1.0f).Result, 0.0f);
    return {kern::formula<f32, f32, f32, f32, f32, f32, f32>([](auto x, auto x_2, auto x_3, auto t, auto y, auto y_2, auto y_3) { return hypot(((x - x_2) - (x_3 * t)), ((y - y_2) - (y_3 * t))); }, vector_break(p).x, vector_break(a).x, vector_break(ab).x, t, vector_break(p).y, vector_break(a).y, vector_break(ab).y)};
  }

  struct select_cells_matchResult { bool Result = false; };
  KERN_PURE select_cells_matchResult select_cells_match(const CityStream_select_cells_Pick& x, const CityStream_select_cells_Pick& y) {
    return {logic_or(math_compare<"<", f32>(x.d, y.d), logic_and(math_compare<"==", f32>(x.d, y.d), math_compare<"<", u64>(x.cell, y.cell)))};
  }

  void CityStream_drop_cell(CityStream& self, CityStream_Tile& t, u64 cell = 0) {
    i64 it = 0;
    i64 at = 0;
    u32 id = 0;
    bool remove = false;
    u32 gone = 0;
    i64 at_2 = 0;
    u64 key = 0;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<u32>>(t.cell_ids, cell).index, map_length<u64, List<u32>>(t.cell_ids));
    if (math_compare<"==", i64>(it, map_length<u64, List<u32>>(t.cell_ids))) {
      return;
    }
    at = it;
    for (auto it60 = flow_for(n60, 0, math_sub<i64>(list_length<u32>(t.cell_ids[at]), 1)); it60.next();) {
      id = t.cell_ids[at][n60.index];
      engine_renderer_remove(n61, self.r_, id);
      remove = n61.result;
      gone = id;
      while (loop(list_contains<u32>(t.meshes, gone))) {
        list_remove(t.meshes, gone);
      }
    }
    at_2 = it;
    map_remove_at(t.cell_ids, at_2);
    key = cell;
    set_remove(n62, t.dirty_cells, key);
  }

  void CityStream_select_cells_settle(CityStream& self, CityStream_Tile& t, const CityStream_select_cells_Pick& p, f32 reach = 0.0f, f32 kKeep = 0.0f) {
    bool want = false;
    bool built = false;
    u64 key = 0;
    u64 key_2 = 0;
    want = math_compare<"!=", u64>(num_cast<u64, i64>(num_cast<i64, bool>(set_contains<u64>(t.want_cells, p.cell))), 0);
    built = math_compare<"!=", u64>(num_cast<u64, i64>(num_cast<i64, bool>(map_contains<u64, List<u32>>(t.cell_ids, p.cell))), 0);
    if (logic_and(want, logic_not(built))) {
      key = p.cell;
      set_add(n58, t.dirty_cells, key);
    }
    if (logic_and(logic_not(want), logic_not(built))) {
      key_2 = p.cell;
      set_remove(n59, t.dirty_cells, key_2);
    }
    if (logic_and(logic_and(logic_and(logic_and(logic_not(want), built), math_compare<">", f32>(p.d, kern::formula<f32, f32>([](auto reach, auto kKeep) { return (reach + (kKeep * 0.5f)); }, reach, kKeep))), logic_not(num_cast<bool, u32>(t.cell_job))), logic_not(t.cell_uploading))) {
      CityStream_drop_cell(self, t, p.cell);
    }
  }

  void CityStream_select_cells(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, bool force = false) {
    f64 now = 0.0;
    f64 now_ms = 0.0;
    Vector ahead = Vector{0.0, 0.0, 0.0};
    f32 kR = 0.0f;
    f32 kKeep = 0.0f;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector ab = Vector{0.0, 0.0, 0.0};
    f32 ab2 = 0.0f;
    List<CityStream_select_cells_Pick> picks = List<CityStream_select_cells_Pick>{};
    i64 it = 0;
    bool skip = false;
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<u32>{}, physics_Body{0}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0f, 0.0f, 0.0f, 0.0f}, List<u32>{}, List<u32>{}, false, false, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, Map<u64, List<u32>>{}, Set<u64>{}, Map<u64, List<u32>>{}, false, 0, 0, Map<u64, List<u32>>{}, Set<u64>{}, List<u64>{}, 0, List<u8>{}, 0, false, -1, physics_Body{0}, false};
    i32 cx = 0;
    i32 cy = 0;
    f32 d = 0.0f;
    u32 total = 0;
    f32 reach = 0.0f;
    i64 at = 0;
    i64 count = 0;
    bool stop = false;
    u64 key = 0;
    bool skip_2 = false;
    List<u64> far = List<u64>{};
    bool skip_3 = false;
    i32 cx_2 = 0;
    i32 cy_2 = 0;
    u64 c = 0;
    now_ms = engine_now_ms();
    now = now_ms;
    ahead = vector_make(kern::formula<f32, f32>([](auto x, auto x_2) { return (x + x_2); }, vector_break(focus).x, vector_break(self.view_lead).x), kern::formula<f32, f32>([](auto y, auto y_2) { return (y + y_2); }, vector_break(focus).y, vector_break(self.view_lead).y), 0.0f);
    if (logic_and(logic_and(logic_and(logic_not(force), math_compare<"<", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(focus).x, vector_break(self.vis_at_).x, vector_break(focus).y, vector_break(self.vis_at_).y), 40.0f)), math_compare<"<", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(ahead).x, vector_break(self.vis_ahead_).x, vector_break(ahead).y, vector_break(self.vis_ahead_).y), 60.0f)), math_compare<"<", f64>(kern::formula<f64, f64>([](auto now, auto vis_ms_) { return (now - vis_ms_); }, now, self.vis_ms_), 1000.0))) {
      return;
    }
    self.vis_at_ = vector_make(vector_break(focus).x, vector_break(focus).y, 0.0f);
    self.vis_ahead_ = ahead;
    self.vis_ms_ = now;
    kR = 900.0f;
    kKeep = 220.0f;
    a = vector_make(vector_break(focus).x, vector_break(focus).y, 0.0f);
    ab = vector_sub(ahead, a);
    ab2 = kern::formula<f32, f32>([](auto x, auto y) { return ((x * x) + (y * y)); }, vector_break(ab).x, vector_break(ab).y);
    picks = List<CityStream_select_cells_Pick>{};
    it = 0;
    while (loop(logic_not(math_compare<"==", i64>(it, map_length<u64, List<CityStream_Tile>>(self.tiles_))))) {
      skip = false;
      t = list_get<CityStream_Tile>(self.tiles_[it], 0);
      if (logic_not(t.prepared)) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it55 = flow_for(n55, 0, math_sub<i64>(map_length<u64, List<u32>>(t.cell_items), 1)); it55.next();) {
          cx = kern::formula<u64>([](auto a) { return num_cast<i32, u32>(num_cast<u32, u64>((a >> 32))); }, map_key_at<u64, List<u32>>(t.cell_items, n55.index));
          cy = num_cast<i32, u32>(num_cast<u32, u64>(map_key_at<u64, List<u32>>(t.cell_items, n55.index)));
          d = select_cells_way_distance(ab2, a, ab, vector_make(kern::formula<i32, f32>([](auto cx, auto kChunk) { return ((num_cast<f32, i32>(cx) + 0.5f) * kChunk); }, cx, kChunk), kern::formula<i32, f32>([](auto cy, auto kChunk) { return ((num_cast<f32, i32>(cy) + 0.5f) * kChunk); }, cy, kChunk), 0.0f)).Result;
          if (math_compare<"<=", f32>(d, kern::formula<f32, f32>([](auto kR, auto kKeep) { return (kR + kKeep); }, kR, kKeep))) {
            list_add(picks, struct_make<CityStream_select_cells_Pick>(d, num_cast<u32, i64>(it), map_key_at<u64, List<u32>>(t.cell_items, n55.index), num_cast<u32, i64>(list_length<u32>(t.cell_items[n55.index]))));
          }
        }
      }
      it += 1;
    }
    list_sort<&city_stream_cpp::select_cells_match>(*this, picks);
    for (auto it56 = flow_for(n56, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it56.next();) {
      set_clear(self.tiles_[n56.index][0].want_cells);
    }
    total = 0;
    reach = kR;
    count = list_length<CityStream_select_cells_Pick>(picks);
    at = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<", i64>(at, count)))) {
      if (math_compare<">", f32>(picks[at].d, kR)) {
        stop = true;
      }
      if (logic_not(stop)) {
        if (logic_and(math_compare<">=", u32>(total, self.building_budget), math_compare<">", f32>(picks[at].d, 240.0f))) {
          reach = picks[at].d;
          stop = true;
        }
        if (logic_not(stop)) {
          key = picks[at].cell;
          set_add(n57, self.tiles_[num_cast<i64, u32>(picks[at].tile)][0].want_cells, key);
          total += picks[at].n;
        }
      }
      if (logic_not(stop)) {
        at += 1;
      }
    }
    for (auto it63 = flow_for(n63, 0, math_sub<i64>(list_length<CityStream_select_cells_Pick>(picks), 1)); it63.next();) {
      CityStream_select_cells_settle(self, self.tiles_[num_cast<i64, u32>(picks[n63.index].tile)][0], picks[n63.index], reach, kKeep);
    }
    for (auto it64 = flow_for(n64, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it64.next();) {
      skip_2 = false;
      if (logic_or(num_cast<bool, u32>(self.tiles_[n64.index][0].cell_job), self.tiles_[n64.index][0].cell_uploading)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        far = List<u64>{};
        for (auto it65 = flow_for(n65, 0, math_sub<i64>(map_length<u64, List<u32>>(self.tiles_[n64.index][0].cell_ids), 1)); it65.next();) {
          skip_3 = false;
          if (num_cast<bool, i64>(num_cast<i64, bool>(set_contains<u64>(self.tiles_[n64.index][0].want_cells, map_key_at<u64, List<u32>>(self.tiles_[n64.index][0].cell_ids, n65.index))))) {
            skip_3 = true;
          }
          if (logic_not(skip_3)) {
            cx_2 = kern::formula<u64>([](auto a) { return num_cast<i32, u32>(num_cast<u32, u64>((a >> 32))); }, map_key_at<u64, List<u32>>(self.tiles_[n64.index][0].cell_ids, n65.index));
            cy_2 = num_cast<i32, u32>(num_cast<u32, u64>(map_key_at<u64, List<u32>>(self.tiles_[n64.index][0].cell_ids, n65.index)));
            if (math_compare<">", f32>(select_cells_way_distance(ab2, a, ab, vector_make(kern::formula<i32, f32>([](auto cx_2, auto kChunk) { return ((num_cast<f32, i32>(cx_2) + 0.5f) * kChunk); }, cx_2, kChunk), kern::formula<i32, f32>([](auto cy_2, auto kChunk) { return ((num_cast<f32, i32>(cy_2) + 0.5f) * kChunk); }, cy_2, kChunk), 0.0f)).Result, kern::formula<f32, f32>([](auto kR, auto kKeep) { return (kR + kKeep); }, kR, kKeep))) {
              list_add(far, map_key_at<u64, List<u32>>(self.tiles_[n64.index][0].cell_ids, n65.index));
            }
          }
        }
        for (auto it66 = flow_for(n66, 0, math_sub<i64>(list_length<u64>(far), 1)); it66.next();) {
          c = far[n66.index];
          CityStream_drop_cell(self, self.tiles_[n64.index][0], c);
        }
      }
    }
  }

  struct CityStream_begin_roads_callbackResult { bool Result = false; };
  /// over a crater the ribbon is draped finely (0.9 m quads): it lies in the bowl, not over it
  KERN_PURE CityStream_begin_roads_callbackResult CityStream_begin_roads_callback(const CityStream& self, f64 x = 0.0, f64 y = 0.0, f64 r = 0.0) {
    return {terrain::Terrain_deformed(self.terrain_, num_cast<f32, f64>(x), num_cast<f32, f64>(y), num_cast<f32, f64>(r)).Result};
  }

  void CityStream_begin_roads(CityStream& self, CityStream_Tile& t) {
    RoadOptions ro = RoadOptions{0.0, 0.0, 800.0, 420.0, 6.0, 3.4, 0.09, 0.112, 0.135, 0.42, 1.0, 3.0, 7.0, 0.2, Rgb{0.525, 0.514, 0.486}, List<Rgb>{}, Callback<bool(f64, f64, f64)>{}, 0.9};
    Vector mid = Vector{0.0, 0.0, 0.0};
    List<f64> data = List<f64>{};
    u64 size = 0;
    bool begin = false;
    ro = RoadOptions{0.0, 0.0, 800.0, 420.0, 6.0, 3.4, 0.09, 0.112, 0.135, 0.42, 1.0, 3.0, 7.0, 0.2, Rgb{0.525, 0.514, 0.486}, List<Rgb>{}, Callback<bool(f64, f64, f64)>{}, 0.9};
    mid = vector_make(kern::formula<f32, f32>([](auto x, auto x_2) { return (x + (x_2 * 0.5f)); }, vector_break(self.focus_).x, vector_break(self.view_lead).x), kern::formula<f32, f32>([](auto y, auto y_2) { return (y + (y_2 * 0.5f)); }, vector_break(self.focus_).y, vector_break(self.view_lead).y), 0.0f);
    ro.center_x = num_cast<f64, f32>(vector_break(mid).x);
    ro.center_y = num_cast<f64, f32>(vector_break(mid).y);
    ro.radius = kern::formula<f32, f32>([](auto x, auto y) { return num_cast<f64, f32>((720.0f + (hypot(x, y) * 0.5f))); }, vector_break(self.view_lead).x, vector_break(self.view_lead).y);
    ro.segment_length = 4.0;
    ro.palette = road_palette().Result;
    ro.deformed = callback<&city_stream_cpp::CityStream_begin_roads_callback>(self);
    data = engine_road_stream_data(t.roads);
    size = engine_road_stream_size(t.roads);
    engine_road_builder_begin(n67, t.road_builder, data, num_cast<u32, u64>(size), ro, terrain_cpp::Terrain_sampler(self.terrain_, 0.0f).Result);
    begin = n67.result;
    t.roads_at = mid;
    self.relays_ = kern::formula<u32>([](auto relays_) { return (relays_ + 1); }, self.relays_);
    t.upload_next = 0;
    t.state = CityStream_Tile_State::roads;
  }

  void CityStream_finish_features(CityStream& self, CityStream_Tile& t) {
    List<f32> widths = List<f32>{};
    List<f32> made = List<f32>{};
    u64 i = 0;
    MapFeatures_Road r = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    f32 g = 0.0f;
    i64 d = 0;
    Vector c = Vector{0.0, 0.0, 0.0};
    Vector q = Vector{0.0, 0.0, 0.0};
    bool skip = false;
    f32 w = 0.0f;
    bool water = false;
    bool major = false;
    u32 color = 0;
    List<Vector_row> list = List<Vector_row>{};
    f32 len = 0.0f;
    u64 k = 0;
    u32 i_2 = 0;
    List<u32> t_2 = List<u32>{};
    u64 key = 0;
    List<u32> put = List<u32>{};
    made = List<f32>{};
    list_resize(made, list_length<MapFeatures_Road>(t.features.roads), 0.0f);
    widths = made;
    i = 0;
    while (loop(math_compare<"<", u64>(i, num_cast<u64, i64>(list_length<f32>(widths))))) {
      r = t.features.roads[num_cast<i64, u64>(i)];
      if (logic_and(logic_and(logic_not(tiles_cpp::road_is_water(r.cls).Result), logic_not(enum_equal<RoadClass>(r.cls, RoadClass::path))), logic_not(enum_equal<RoadClass>(r.cls, RoadClass::track)))) {
        widths[num_cast<i64, u64>(i)] = kern::formula<f32>([](auto the_result) { return (the_result + 3.4f); }, tiles_cpp::road_width(r.cls).Result);
      }
      i += 1;
    }
    terrain_cpp::Terrain_set_corridors(self.terrain_, t.bounds, t.features.roads, widths, tiles::TileKey_hash(t.key).Result);
    list_clear(t.cent);
    for (auto it48 = flow_for(n48, 0, math_sub<i64>(list_length<MapFeatures_Building>(t.features.buildings), 1)); it48.next();) {
      if (logic_not(math_compare<"==", i64>(list_length<Vector_row>(t.features.buildings[n48.index].rings), 0))) {
        g = terrain_cpp::Terrain_footprint_base(self.terrain_, t.features.buildings[n48.index].rings[0].items).Result;
        t.features.buildings[n48.index].base = kern::formula<f32, f32>([](auto a, auto g) { return (a + g); }, t.features.buildings[n48.index].base, g);
        t.features.buildings[n48.index].top = kern::formula<f32, f32>([](auto a, auto g) { return (a + g); }, t.features.buildings[n48.index].top, g);
      }
      t.features.buildings[n48.index].full_top = t.features.buildings[n48.index].top;
      d = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u32, CityStream_Damage>(self.damage_, t.features.buildings[n48.index].seed).index, map_length<u32, CityStream_Damage>(self.damage_));
      if (logic_not(math_compare<"==", i64>(d, map_length<u32, CityStream_Damage>(self.damage_)))) {
        t.features.buildings[n48.index].top = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto a_3) { return fmin(a, (a_2 + a_3)); }, t.features.buildings[n48.index].top, t.features.buildings[n48.index].base, self.damage_[d].height);
        t.features.buildings[n48.index].leveled = self.damage_[d].leveled;
      }
      c = Vector{0.0, 0.0, 0.0};
      if (logic_and(logic_not(math_compare<"==", i64>(list_length<Vector_row>(t.features.buildings[n48.index].rings), 0)), logic_not(math_compare<"==", i64>(list_length<Vector>(t.features.buildings[n48.index].rings[0].items), 0)))) {
        for (auto it49 = flow_for(n49, 0, math_sub<i64>(list_length<Vector>(t.features.buildings[n48.index].rings[0].items), 1)); it49.next();) {
          q = t.features.buildings[n48.index].rings[0].items[n49.index];
          c = vector_add(c, q);
        }
        c = vector_scale(c, kern::formula<u64>([](auto a) { return (1.0f / num_cast<f32, u64>(a)); }, num_cast<u64, i64>(list_length<Vector>(t.features.buildings[n48.index].rings[0].items))));
      }
      list_add(t.cent, c);
    }
    t.prepared = true;
    self.furniture_serial_ = kern::formula<u32>([](auto furniture_serial_) { return (furniture_serial_ + 1); }, self.furniture_serial_);
    for (auto it50 = flow_for(n50, 0, math_sub<i64>(list_length<MapFeatures_Road>(t.features.roads), 1)); it50.next();) {
      skip = false;
      w = tiles_cpp::road_width(t.features.roads[n50.index].cls).Result;
      water = tiles_cpp::road_is_water(t.features.roads[n50.index].cls).Result;
      if (water) {
        skip = true;
      }
      if (logic_not(skip)) {
        major = math_compare<"<=", i32>(num_cast<i32, u8>(num_cast<u8, i64>(enum_index<RoadClass>(t.features.roads[n50.index].cls))), num_cast<i32, u8>(num_cast<u8, i64>(enum_index<RoadClass>(RoadClass::tertiary))));
        color = kern::formula<bool, u32, bool, u32, bool, bool, u32, bool, u32, u32>([](auto water, auto a4, auto a, auto a5, auto a_2, auto a_3, auto a3, auto major, auto a1, auto a0) { return (water ? a4 : (a ? a5 : ((a_2 || a_3) ? a3 : (major ? a1 : a0)))); }, water, 4, enum_equal<RoadClass>(t.features.roads[n50.index].cls, RoadClass::pedestrian), 5, enum_equal<RoadClass>(t.features.roads[n50.index].cls, RoadClass::path), enum_equal<RoadClass>(t.features.roads[n50.index].cls, RoadClass::track), 3, major, 1, 0);
        engine_road_stream_road(t.roads, t.features.roads[n50.index].points, num_cast<u32, i64>(list_length<Vector>(t.features.roads[n50.index].points)), num_cast<f64, f32>(w), logic_and(logic_not(water), tiles_cpp::road_drivable(t.features.roads[n50.index].cls).Result), color, kern::formula<bool, bool, i32, i32>([](auto major, auto water, auto a2, auto a) { return ((major && (!water)) ? a2 : a); }, major, water, 2, -1));
        if (logic_not(tiles_cpp::road_drivable(t.features.roads[n50.index].cls).Result)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (t.procedural) {
            list_add(self.lanes_, struct_make<Lane>(t.features.roads[n50.index].points, w, tiles::TileKey_hash(t.key).Result, true));
            skip = true;
          }
          if (logic_not(skip)) {
            n51 = tiles_cpp::clip_line(t.features.roads[n50.index].points, t.bounds);
            list = n51.Result;
            for (auto it52 = flow_for(n52, 0, math_sub<i64>(list_length<Vector_row>(list), 1)); it52.next();) {
              len = 0.0f;
              k = 0;
              while (loop(math_compare<"<", u64>(kern::formula<u64>([](auto k) { return (k + 1); }, k), num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(list[n52.index]).items))))) {
                len += kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(list[n52.index].items[num_cast<i64, u64>(kern::formula<u64>([](auto k) { return (k + 1); }, k))]).x, vector_break(list[n52.index].items[num_cast<i64, u64>(k)]).x, vector_break(list[n52.index].items[num_cast<i64, u64>(kern::formula<u64>([](auto k) { return (k + 1); }, k))]).y, vector_break(list[n52.index].items[num_cast<i64, u64>(k)]).y);
                k += 1;
              }
              if (math_compare<">", f32>(len, 0.5f)) {
                var_take(n53, list[n52.index].items);
                list_add(self.lanes_, struct_make<Lane>(n53.value, w, tiles::TileKey_hash(t.key).Result, true));
              }
            }
          }
        }
      }
    }
    map_clear(t.cell_items);
    i_2 = 0;
    while (loop(math_compare<"<", u64>(num_cast<u64, u32>(i_2), num_cast<u64, i64>(list_length<Vector>(t.cent))))) {
      key = cell_of(t.cent[num_cast<i64, u32>(i_2)]).Result;
      map_add(n54, t.cell_items, key);
      t_2 = t.cell_items[n54.index];
      list_add(t_2, i_2);
      put = t_2;
      t.cell_items[n54.index] = put;
      i_2 += 1;
    }
    CityStream_select_cells(self, self.focus_, true);
    CityStream_begin_roads(self, t);
  }

  void CityStream_go_offline(CityStream& self) {
    i64 count = 0;
    i64 at = 0;
    List<CityStream_Tile> t = List<CityStream_Tile>{};
    CityStream_Tile made = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<u32>{}, physics_Body{0}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0f, 0.0f, 0.0f, 0.0f}, List<u32>{}, List<u32>{}, false, false, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, Map<u64, List<u32>>{}, Set<u64>{}, Map<u64, List<u32>>{}, false, 0, 0, Map<u64, List<u32>>{}, Set<u64>{}, List<u64>{}, 0, List<u8>{}, 0, false, -1, physics_Body{0}, false};
    Object<kern::FootprintStream> footprint_stream = Object<kern::FootprintStream>{};
    Object<kern::RoadStream> road_stream = Object<kern::RoadStream>{};
    Object<kern::BuildingBuilder> building_builder = Object<kern::BuildingBuilder>{};
    Object<kern::RoadBuilder> road_builder = Object<kern::RoadBuilder>{};
    List<Vector_row> a = List<Vector_row>{};
    u64 key = 0;
    List<CityStream_Tile> put = List<CityStream_Tile>{};
    if (self.offline_) {
      return;
    }
    self.offline_ = true;
    for (auto it40 = flow_for(n40, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it40.next();) {
      CityStream_remove(self, self.tiles_[n40.index][0]);
    }
    count = map_length<u64, List<CityStream_Tile>>(self.tiles_);
    for (auto it41 = flow_for(n41, 0, math_sub<i64>(count, 1)); it41.next();) {
      at = math_sub<i64>(math_sub<i64>(count, 1), n41.index);
      if (math_compare<">", i64>(list_length<CityStream_Tile>(self.tiles_[at]), 0)) {
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
    made = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<u32>{}, physics_Body{0}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0f, 0.0f, 0.0f, 0.0f}, List<u32>{}, List<u32>{}, false, false, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, Map<u64, List<u32>>{}, Set<u64>{}, Map<u64, List<u32>>{}, false, 0, 0, Map<u64, List<u32>>{}, Set<u64>{}, List<u64>{}, 0, List<u8>{}, 0, false, -1, physics_Body{0}, false};
    engine_footprint_stream_make(n42);
    footprint_stream = n42.result;
    made.footprints = footprint_stream;
    engine_road_stream_make(n43);
    road_stream = n43.result;
    made.roads = road_stream;
    engine_building_builder_make(n44);
    building_builder = n44.result;
    made.buildings = building_builder;
    engine_road_builder_make(n45);
    road_builder = n45.result;
    made.road_builder = road_builder;
    t = list_make<CityStream_Tile, "1">(made);
    t[0].procedural = true;
    t[0].key = struct_make<TileKey>(0, -1, -1);
    t[0].bounds = engine_math_math::Vec4_new(-1e+09f, -1e+09f, 1e+09f, 1e+09f).Result;
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
    map_add(n68, self.tiles_, key);
    var_take(n69, t);
    put = n69.value;
    if (math_compare<">", i64>(list_length<CityStream_Tile>(self.tiles_[n68.index]), 0)) {
      if (object_valid<kern::RoadBuilder>(self.tiles_[n68.index][0].road_builder)) {
        engine_road_builder_release(self.tiles_[n68.index][0].road_builder);
      }
      if (object_valid<kern::BuildingBuilder>(self.tiles_[n68.index][0].buildings)) {
        engine_building_builder_release(self.tiles_[n68.index][0].buildings);
      }
      if (object_valid<kern::RoadStream>(self.tiles_[n68.index][0].roads)) {
        engine_road_stream_release(self.tiles_[n68.index][0].roads);
      }
      if (object_valid<kern::FootprintStream>(self.tiles_[n68.index][0].footprints)) {
        engine_footprint_stream_release(self.tiles_[n68.index][0].footprints);
      }
    }
    self.tiles_[n68.index] = put;
    if (math_compare<">", i64>(list_length<CityStream_Tile>(t), 0)) {
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
    i64 it = 0;
    f64 now_ms = 0.0;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<CityStream_Tile>>(self.tiles_, tiles::TileKey_hash(key).Result).index, map_length<u64, List<CityStream_Tile>>(self.tiles_));
    if (math_compare<"==", i64>(it, map_length<u64, List<CityStream_Tile>>(self.tiles_))) {
      return;
    }
    self.tiles_[it][0].state = CityStream_Tile_State::failed;
    self.tiles_[it][0].tries = kern::formula<u32>([](auto a) { return (a + 1); }, self.tiles_[it][0].tries);
    now_ms = engine_now_ms();
    self.tiles_[it][0].retry_ms = kern::formula<f64, u32>([](auto now_ms, auto a) { return (now_ms + min(20000.0, (2000.0 * num_cast<f64, u32>(a)))); }, now_ms, self.tiles_[it][0].tries);
    if (logic_and(logic_and(tiles::TileKey_equal(key, self.origin_tile_).Result, math_compare<"==", u32>(self.real_tiles_, 0)), math_compare<">=", u32>(self.tiles_[it][0].tries, 3))) {
      CityStream_go_offline(self);
    }
  }

  void CityStream_furniture(const CityStream& self, List<u64>& out) {
    list_clear(out);
    for (auto it70 = flow_for(n70, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it70.next();) {
      if (self.tiles_[n70.index][0].prepared) {
        list_add(out, map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n70.index));
      }
    }
    list_sort<nullptr>(out);
  }

  struct CityStream_featuresResult { MapFeatures Result = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}; };
  KERN_PURE CityStream_featuresResult CityStream_features(const CityStream& self, u64 tile = 0) {
    i64 it = 0;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<CityStream_Tile>>(self.tiles_, tile).index, map_length<u64, List<CityStream_Tile>>(self.tiles_));
    return {logic_select<MapFeatures>(logic_not(math_compare<"==", i64>(it, map_length<u64, List<CityStream_Tile>>(self.tiles_))), self.tiles_[it][0].features, kNoFeatures)};
  }

  struct CityStream_loaded_atResult { bool Result = false; };
  KERN_PURE CityStream_loaded_atResult CityStream_loaded_at(const CityStream& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    if (logic_not(terrain::Terrain_settled(self.terrain_).Result)) {
      return {false};
    }
    if (self.offline_) {
      for (auto it71 = flow_for(n71, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it71.next();) {
        if (self.tiles_[n71.index][0].procedural) {
          return {enum_equal<CityStream_Tile_State>(self.tiles_[n71.index][0].state, CityStream_Tile_State::ready)};
        }
      }
      return {false};
    }
    return {terrain_cpp::Terrain_corridors_at(self.terrain_, vector_break(p).x, vector_break(p).y).Result};
  }

  struct CityStream_ready_atResult { bool Result = false; };
  KERN_PURE CityStream_ready_atResult CityStream_ready_at(const CityStream& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    bool solid = false;
    if (logic_or(logic_not(CityStream_loaded_at(self, p).Result), logic_not(terrain_cpp::Terrain_ready_at(self.terrain_, p).Result))) {
      return {false};
    }
    solid = logic_and(logic_and(logic_and(self.near_.built, logic_not(self.near_.building)), logic_not(self.near_.dirty)), math_compare<"<", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(self.near_.at).x, vector_break(p).x, vector_break(self.near_.at).y, vector_break(p).y), 140.0f));
    return {logic_or(logic_not(self.offline_), solid)};
  }

  struct update_collision_way_d2Result { f32 Result = 0.0f; };
  KERN_PURE update_collision_way_d2Result update_collision_way_d2(f32 way_l2 = 0.0f, Vector way_a = Vector{0.0, 0.0, 0.0}, Vector way_ab = Vector{0.0, 0.0, 0.0}, Vector p = Vector{0.0, 0.0, 0.0}) {
    f32 t = 0.0f;
    f32 dx = 0.0f;
    f32 dy = 0.0f;
    t = kern::formula<f32, f32, f32>([](auto way_l2, auto the_result, auto a0f) { return ((way_l2 > 1.0f) ? the_result : a0f); }, way_l2, common::clampf(kern::formula<f32, f32, f32, f32, f32, f32, f32>([](auto x, auto x_2, auto x_3, auto y, auto y_2, auto y_3, auto way_l2) { return ((((x - x_2) * x_3) + ((y - y_2) * y_3)) / way_l2); }, vector_break(p).x, vector_break(way_a).x, vector_break(way_ab).x, vector_break(p).y, vector_break(way_a).y, vector_break(way_ab).y, way_l2), 0.0f, 1.0f).Result, 0.0f);
    dx = kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto x_3, auto t) { return ((x - x_2) - (x_3 * t)); }, vector_break(p).x, vector_break(way_a).x, vector_break(way_ab).x, t);
    dy = kern::formula<f32, f32, f32, f32>([](auto y, auto y_2, auto y_3, auto t) { return ((y - y_2) - (y_3 * t)); }, vector_break(p).y, vector_break(way_a).y, vector_break(way_ab).y, t);
    return {kern::formula<f32, f32>([](auto dx, auto dy) { return ((dx * dx) + (dy * dy)); }, dx, dy)};
  }

  struct update_collision_matchResult { bool Result = false; };
  KERN_PURE update_collision_matchResult update_collision_match(const CityStream_update_collision_Pick& a, const CityStream_update_collision_Pick& b) {
    return {math_compare<"<", f32>(a.d2, b.d2)};
  }

  struct CityStream_building_shapeResult { physics_Shape Result = physics_Shape{0}; };
  CityStream_building_shapeResult CityStream_building_shape(CityStream& self, const MapFeatures_Building& b, physics_Body body = physics_Body{0}) {
    Object<kern::Earcut> earcut = Object<kern::Earcut>{};
    Object<kern::Earcut> earcut_2 = Object<kern::Earcut>{};
    List<f64> xy = List<f64>{};
    List<u32> holes = List<u32>{};
    List<u32> tris = List<u32>{};
    List<u32> idx = List<u32>{};
    List<Vector> pts = List<Vector>{};
    List<Vector_row> rings = List<Vector_row>{};
    u64 step = 0;
    List<Vector> thin = List<Vector>{};
    u64 k = 0;
    u64 r = 0;
    bool skip = false;
    f64 a = 0.0;
    i64 first = 0;
    i64 last = 0;
    Vector held = Vector{0.0, 0.0, 0.0};
    i64 at = 0;
    Vector p = Vector{0.0, 0.0, 0.0};
    i64 place = 0;
    f64 item = 0.0;
    f64 item_2 = 0.0;
    Vector p0 = Vector{0.0, 0.0, 0.0};
    Vector p1 = Vector{0.0, 0.0, 0.0};
    u32 base = 0;
    i64 place_2 = 0;
    u32 item_3 = 0;
    u32 item_4 = 0;
    u32 item_5 = 0;
    u32 item_6 = 0;
    u32 item_7 = 0;
    u32 item_8 = 0;
    u32 base_2 = 0;
    u64 i = 0;
    u32 a_2 = 0;
    u32 c = 0;
    u32 d = 0;
    Vector cross = Vector{0.0, 0.0, 0.0};
    u32 swapped = 0;
    i64 place_3 = 0;
    u32 item_9 = 0;
    u32 item_10 = 0;
    u32 item_11 = 0;
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, List<u32>{}, physics_ShapeMaterial{1.0f, 0.6f, 0.0f, 0.0f, false, 1, 18446744073709551615u, 0, true, true, false, 0, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{0};
    physics_Shape returned = physics_Shape{0};
    physics_Shape returned_2 = physics_Shape{0};
    engine_earcut_make(n76);
    earcut_2 = n76.result;
    earcut = earcut_2;
    xy = List<f64>{};
    holes = List<u32>{};
    tris = List<u32>{};
    idx = List<u32>{};
    pts = List<Vector>{};
    list_clear(xy);
    list_clear(holes);
    list_clear(tris);
    list_clear(idx);
    list_clear(pts);
    rings = b.rings;
    for (auto it77 = flow_for(n77, 0, math_sub<i64>(list_length<Vector_row>(rings), 1)); it77.next();) {
      if (math_compare<">", u64>(num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(rings[n77.index]).items)), 64)) {
        step = kern::formula<u64>([](auto a) { return ((a + 63) / 64); }, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(rings[n77.index]).items)));
        thin = List<Vector>{};
        k = 0;
        while (loop(math_compare<"<", u64>(k, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(rings[n77.index]).items))))) {
          list_add(thin, rings[n77.index].items[num_cast<i64, u64>(k)]);
          k += step;
        }
        rings[n77.index].items = thin;
      }
    }
    r = 0;
    while (loop(math_compare<"<", u64>(r, num_cast<u64, i64>(list_length<Vector_row>(rings))))) {
      skip = false;
      if (math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(rings[num_cast<i64, u64>(r)].items)), 3)) {
        skip = true;
      }
      if (logic_not(skip)) {
        a = signed_area(rings[num_cast<i64, u64>(r)].items).Result;
        if (math_compare<"<", f64>(kern::formula<f64>([](auto a) { return abs(a); }, a), 0.5)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (math_compare<"!=", i32>(num_cast<i32, bool>(math_compare<"==", u64>(r, 0)), num_cast<i32, bool>(math_compare<">", f64>(a, 0.0)))) {
            first = 0;
            last = math_sub<i64>(list_length<Vector>(rings[num_cast<i64, u64>(r)].items), 1);
            while (loop(math_compare<"<", i64>(first, last))) {
              held = list_get<Vector>(rings[num_cast<i64, u64>(r)].items, first);
              rings[num_cast<i64, u64>(r)].items[first] = list_get<Vector>(rings[num_cast<i64, u64>(r)].items, last);
              rings[num_cast<i64, u64>(r)].items[last] = held;
              first += 1;
              last = math_sub<i64>(last, 1);
            }
          }
          if (num_cast<bool, u64>(r)) {
            list_add(holes, kern::formula<u64>([](auto a) { return num_cast<u32, u64>((a / 2)); }, num_cast<u64, i64>(list_length<f64>(xy))));
          }
          at = num_cast<i64, u64>(r);
          for (auto it78 = flow_for(n78, 0, math_sub<i64>(list_length<Vector>(rings[at].items), 1)); it78.next();) {
            p = rings[at].items[n78.index];
            place = list_length<f64>(xy);
            item = num_cast<f64, f32>(vector_break(p).x);
            item_2 = num_cast<f64, f32>(vector_break(p).y);
            list_insert_list(xy, place, list_make<f64, "2">(item, item_2));
          }
          for (auto it79 = flow_for(n79, 0, math_sub<i64>(list_length<Vector>(rings[num_cast<i64, u64>(r)].items), 1)); it79.next();) {
            p0 = rings[num_cast<i64, u64>(r)].items[n79.index];
            p1 = rings[num_cast<i64, u64>(r)].items[num_cast<i64, u64>(kern::formula<i64, u64>([](auto index, auto a) { return ((num_cast<u64, i64>(index) + 1) % a); }, n79.index, num_cast<u64, i64>(list_length<Vector>(rings[num_cast<i64, u64>(r)].items))))];
            base = num_cast<u32, i64>(list_length<Vector>(pts));
            list_add(pts, vector_make(vector_break(p0).x, vector_break(p0).y, b.base));
            list_add(pts, vector_make(vector_break(p1).x, vector_break(p1).y, b.base));
            list_add(pts, vector_make(vector_break(p1).x, vector_break(p1).y, b.top));
            list_add(pts, vector_make(vector_break(p0).x, vector_break(p0).y, b.top));
            place_2 = list_length<u32>(idx);
            item_3 = base;
            item_4 = kern::formula<u32>([](auto base) { return (base + 1); }, base);
            item_5 = kern::formula<u32>([](auto base) { return (base + 2); }, base);
            item_6 = base;
            item_7 = kern::formula<u32>([](auto base) { return (base + 2); }, base);
            item_8 = kern::formula<u32>([](auto base) { return (base + 3); }, base);
            list_insert_list(idx, place_2, list_make<u32, "6">(item_3, item_4, item_5, item_6, item_7, item_8));
          }
        }
      }
      r += 1;
    }
    if (math_compare<">=", u64>(num_cast<u64, i64>(list_length<f64>(xy)), 6)) {
      engine_earcut_triangulate(n80, earcut, xy, kern::formula<u64>([](auto a) { return num_cast<u32, u64>((a / 2)); }, num_cast<u64, i64>(list_length<f64>(xy))), holes, num_cast<u32, i64>(list_length<u32>(holes)), tris);
      tris = n80.out;
      base_2 = num_cast<u32, i64>(list_length<Vector>(pts));
      for (auto it81 = flow_for(n81, 0, math_sub<i64>(num_cast<i64, u64>(kern::formula<u64>([](auto a) { return (a / 2); }, num_cast<u64, i64>(list_length<f64>(xy)))), 1)); it81.next();) {
        list_add(pts, vector_make(num_cast<f32, f64>(xy[num_cast<i64, u64>(kern::formula<i64>([](auto index) { return (num_cast<u64, i64>(index) * 2); }, n81.index))]), num_cast<f32, f64>(xy[num_cast<i64, u64>(kern::formula<i64>([](auto index) { return ((num_cast<u64, i64>(index) * 2) + 1); }, n81.index))]), b.top));
      }
      i = 0;
      while (loop(math_compare<"<", u64>(kern::formula<u64>([](auto i) { return (i + 2); }, i), num_cast<u64, i64>(list_length<u32>(tris))))) {
        a_2 = kern::formula<u32, u32>([](auto base_2, auto a) { return (base_2 + a); }, base_2, tris[num_cast<i64, u64>(i)]);
        c = kern::formula<u32, u32>([](auto base_2, auto a) { return (base_2 + a); }, base_2, tris[num_cast<i64, u64>(kern::formula<u64>([](auto i) { return (i + 1); }, i))]);
        d = kern::formula<u32, u32>([](auto base_2, auto a) { return (base_2 + a); }, base_2, tris[num_cast<i64, u64>(kern::formula<u64>([](auto i) { return (i + 2); }, i))]);
        cross = engine_cross_2(vector_sub(pts[num_cast<i64, u32>(c)], pts[num_cast<i64, u32>(a_2)]), vector_sub(pts[num_cast<i64, u32>(d)], pts[num_cast<i64, u32>(a_2)]));
        if (math_compare<"<", f32>(vector_break(cross).z, 0.0f)) {
          swapped = c;
          c = d;
          d = swapped;
        }
        place_3 = list_length<u32>(idx);
        item_9 = a_2;
        item_10 = c;
        item_11 = d;
        list_insert_list(idx, place_3, list_make<u32, "3">(item_9, item_10, item_11));
        i += 3;
      }
    }
    if (logic_not(math_compare<"==", i64>(list_length<u32>(idx), 0))) {
      ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, List<u32>{}, physics_ShapeMaterial{1.0f, 0.6f, 0.0f, 0.0f, false, 1, 18446744073709551615u, 0, true, true, false, 0, physics_Surface::none}};
      ss.kind = physics_ShapeKind::mesh;
      ss.points = pts;
      ss.indices = idx;
      ss.material.friction = 0.7f;
      ss.material.category_bits = 1;
      engine_world_create_shape(n82, self.world_, body, ss);
      create_shape = n82.result;
      returned = create_shape;
      if (object_valid<kern::Earcut>(earcut)) {
        engine_earcut_release(earcut);
      }
      earcut = Object<kern::Earcut>{};
      return {returned};
    }
    returned_2 = struct_make<physics_Shape>(0);
    if (object_valid<kern::Earcut>(earcut)) {
      engine_earcut_release(earcut);
    }
    earcut = Object<kern::Earcut>{};
    return {returned_2};
  }

  struct CityStream_build_nearResult { bool Result = false; };
  CityStream_build_nearResult CityStream_build_near(CityStream& self, bool whole = false) {
    u32 built = 0;
    bool valid = false;
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f, 1.0f, true, true, true, false, 0};
    physics_Body create_body = physics_Body{0};
    i64 count = 0;
    physics_Shape fill = physics_Shape{0};
    bool skip = false;
    u64 slot = 0;
    u64 was = 0;
    u64_u32_pair v = u64_u32_pair{0, 0};
    i64 it = 0;
    bool valid_2 = false;
    bool both = false;
    built = 0;
    valid = engine_body_valid(self.near_.body);
    if (logic_not(valid)) {
      bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f, 1.0f, true, true, true, false, 0};
      bs.type = physics_BodyType::fixed;
      engine_world_create_body(n75, self.world_, bs);
      create_body = n75.result;
      self.near_.body = create_body;
      count = list_length<u64_u32_pair>(self.near_.list);
      fill = struct_make<physics_Shape>(0);
      list_clear(self.near_.shapes);
      list_resize(self.near_.shapes, count, fill);
    }
    while (loop(math_compare<"<", u64>(self.near_.next, num_cast<u64, i64>(list_length<u64_u32_pair>(self.near_.list))))) {
      skip = false;
      was = self.near_.next;
      self.near_.next = kern::formula<u64>([](auto next) { return (next + 1); }, self.near_.next);
      slot = was;
      v = self.near_.list[num_cast<i64, u64>(slot)];
      it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<CityStream_Tile>>(self.tiles_, v.first).index, map_length<u64, List<CityStream_Tile>>(self.tiles_));
      if (logic_or(math_compare<"==", i64>(it, map_length<u64, List<CityStream_Tile>>(self.tiles_)), math_compare<">=", u64>(num_cast<u64, u32>(v.second), num_cast<u64, i64>(list_length<MapFeatures_Building>(self.tiles_[it][0].features.buildings))))) {
        skip = true;
      }
      if (logic_not(skip)) {
        n83 = CityStream_building_shape(self, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(v.second)], self.near_.body);
        self.near_.shapes[num_cast<i64, u64>(slot)] = n83.Result;
        valid_2 = engine_shape_valid(self.near_.shapes[num_cast<i64, u64>(slot)]);
        if (valid_2) {
          self.near_.colliders = kern::formula<u32>([](auto colliders) { return (colliders + 1); }, self.near_.colliders);
        }
        both = logic_not(whole);
        if (both) {
          built += 1;
          both = math_compare<">=", u32>(built, kNearPerStep);
        }
        if (both) {
          return {math_compare<">=", u64>(self.near_.next, num_cast<u64, i64>(list_length<u64_u32_pair>(self.near_.list)))};
        }
      }
    }
    return {true};
  }

  void CityStream_update_collision(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, bool whole = false) {
    Vector way_a = Vector{0.0, 0.0, 0.0};
    Vector way_ab = Vector{0.0, 0.0, 0.0};
    f32 way_l2 = 0.0f;
    bool moved = false;
    u64 i = 0;
    List<CityStream_update_collision_Pick> all = List<CityStream_update_collision_Pick>{};
    bool skip = false;
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<u32>{}, physics_Body{0}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0f, 0.0f, 0.0f, 0.0f}, List<u32>{}, List<u32>{}, false, false, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, Map<u64, List<u32>>{}, Set<u64>{}, Map<u64, List<u32>>{}, false, 0, 0, Map<u64, List<u32>>{}, Set<u64>{}, List<u64>{}, 0, List<u8>{}, 0, false, -1, physics_Body{0}, false};
    u32 i_2 = 0;
    bool skip_2 = false;
    f32 d2 = 0.0f;
    f32 mx = 0.0f;
    f32 my = 0.0f;
    u64 cap = 0;
    bool both = false;
    bool valid = false;
    bool body_valid = false;
    bool body_valid_2 = false;
    bool both_2 = false;
    f64 now_ms = 0.0;
    way_a = vector_make(vector_break(focus).x, vector_break(focus).y, 0.0f);
    way_ab = vector_make(vector_break(self.view_lead).x, vector_break(self.view_lead).y, 0.0f);
    way_l2 = kern::formula<f32, f32>([](auto x, auto y) { return ((x * x) + (y * y)); }, vector_break(way_ab).x, vector_break(way_ab).y);
    moved = logic_or(logic_or(math_compare<">=", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(focus).x, vector_break(self.near_.at).x, vector_break(focus).y, vector_break(self.near_.at).y), 140.0f), math_compare<">=", f32>(kern::formula<f32, f32, f32, f32, f32, f32>([](auto x, auto x_2, auto x_3, auto y, auto y_2, auto y_3) { return hypot(((x + x_2) - x_3), ((y + y_2) - y_3)); }, vector_break(focus).x, vector_break(self.view_lead).x, vector_break(self.near_.ahead).x, vector_break(focus).y, vector_break(self.view_lead).y, vector_break(self.near_.ahead).y), 140.0f)), math_compare<"!=", u64>(num_cast<u64, i64>(list_length<Vector>(self.extra_foci)), num_cast<u64, i64>(list_length<Vector>(self.near_.mates_at))));
    i = 0;
    while (loop(logic_and(logic_not(moved), math_compare<"<", u64>(i, num_cast<u64, i64>(list_length<Vector>(self.extra_foci)))))) {
      moved = math_compare<">=", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(self.extra_foci[num_cast<i64, u64>(i)]).x, vector_break(self.near_.mates_at[num_cast<i64, u64>(i)]).x, vector_break(self.extra_foci[num_cast<i64, u64>(i)]).y, vector_break(self.near_.mates_at[num_cast<i64, u64>(i)]).y), 140.0f);
      i += 1;
    }
    if (logic_and(logic_not(self.near_.building), logic_or(moved, logic_and(self.near_.dirty, math_compare<">=", u32>(self.near_.steps, kNearSteps))))) {
      all = List<CityStream_update_collision_Pick>{};
      for (auto it72 = flow_for(n72, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it72.next();) {
        skip = false;
        t = list_get<CityStream_Tile>(self.tiles_[n72.index], 0);
        if (logic_and(logic_and(logic_and(logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready)), logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::building))), logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::roads))), logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::areas)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (logic_not(t.prepared)) {
            skip = true;
          }
          if (logic_not(skip)) {
            i_2 = 0;
            while (loop(math_compare<"<", u64>(num_cast<u64, u32>(i_2), num_cast<u64, i64>(list_length<Vector>(t.cent))))) {
              skip_2 = false;
              if (logic_and(math_compare<"==", u64>(map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n72.index), self.excl_tile_), math_compare<"==", i32>(num_cast<i32, u32>(i_2), self.excl_index_))) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                d2 = update_collision_way_d2(way_l2, way_a, way_ab, t.cent[num_cast<i64, u32>(i_2)]).Result;
                for (auto it73 = flow_for(n73, 0, math_sub<i64>(list_length<Vector>(self.extra_foci), 1)); it73.next();) {
                  mx = kern::formula<f32, f32>([](auto x, auto x_2) { return (x - x_2); }, vector_break(t.cent[num_cast<i64, u32>(i_2)]).x, vector_break(self.extra_foci[n73.index]).x);
                  my = kern::formula<f32, f32>([](auto y, auto y_2) { return (y - y_2); }, vector_break(t.cent[num_cast<i64, u32>(i_2)]).y, vector_break(self.extra_foci[n73.index]).y);
                  d2 = kern::formula<f32, f32, f32>([](auto d2, auto mx, auto my) { return fmin(d2, ((mx * mx) + (my * my))); }, d2, mx, my);
                }
                if (math_compare<"<=", f32>(d2, 115600.0f)) {
                  list_add(all, struct_make<CityStream_update_collision_Pick>(d2, map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n72.index), i_2));
                }
              }
              i_2 += 1;
            }
          }
        }
      }
      list_sort<&city_stream_cpp::update_collision_match>(*this, all);
      cap = kern::formula<u64>([](auto a) { return (480 + (320 * a)); }, num_cast<u64, i64>(list_length<Vector>(self.extra_foci)));
      if (math_compare<">", u64>(num_cast<u64, i64>(list_length<CityStream_update_collision_Pick>(all)), cap)) {
        list_resize(all, num_cast<i64, u64>(cap), CityStream_update_collision_Pick{0.0f, 0, 0});
      }
      list_clear(self.near_.list);
      for (auto it74 = flow_for(n74, 0, math_sub<i64>(list_length<CityStream_update_collision_Pick>(all), 1)); it74.next();) {
        list_add(self.near_.list, struct_make<u64_u32_pair>(all[n74.index].tile, all[n74.index].i));
      }
      self.near_.old_body = self.near_.body;
      self.near_.body = struct_make<physics_Body>(0);
      self.near_.next = 0;
      self.near_.colliders = 0;
      self.near_.building = true;
      self.near_.dirty = false;
      self.near_.at = vector_make(vector_break(focus).x, vector_break(focus).y, 0.0f);
      self.near_.ahead = vector_make(kern::formula<f32, f32>([](auto x, auto x_2) { return (x + x_2); }, vector_break(focus).x, vector_break(self.view_lead).x), kern::formula<f32, f32>([](auto y, auto y_2) { return (y + y_2); }, vector_break(focus).y, vector_break(self.view_lead).y), 0.0f);
      self.near_.mates_at = self.extra_foci;
      self.near_.steps = 0;
    }
    both = self.near_.building;
    if (both) {
      n84 = CityStream_build_near(self, whole);
      both = n84.Result;
    }
    if (both) {
      valid = engine_body_valid(self.near_.old_body);
      both_2 = valid;
      if (both_2) {
        body_valid_2 = engine_world_body_valid(self.world_, self.near_.old_body);
        both_2 = body_valid_2;
      }
      if (both_2) {
        engine_world_destroy_body(self.world_, self.near_.old_body);
      }
      self.near_.old_body = struct_make<physics_Body>(0);
      self.near_.building = false;
      self.near_.built = true;
      if (logic_and(math_compare<"<", f64>(self.ms_.t_collision, 0.0), logic_not(math_compare<"==", i64>(list_length<u64_u32_pair>(self.near_.list), 0)))) {
        now_ms = engine_now_ms();
        self.ms_.t_collision = kern::formula<f64, f64>([](auto now_ms, auto start_ms_) { return (now_ms - start_ms_); }, now_ms, self.start_ms_);
      }
    }
  }

  void CityStream_collide(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, bool whole = false) {
    if (logic_not(object_valid<kern::physics::World>(self.world_))) {
      return;
    }
    terrain_cpp::Terrain_collide(self.terrain_, focus, self.anchor, kern::formula<bool, u32, u32>([](auto whole, auto a1000000, auto a1) { return (whole ? a1000000 : a1); }, whole, 1000000, 1));
    if (math_compare<"<", u32>(self.near_.steps, kNearSteps)) {
      self.near_.steps = kern::formula<u32>([](auto steps) { return (steps + 1); }, self.near_.steps);
    }
    CityStream_update_collision(self, focus, whole);
    self.stats_.colliders = self.near_.colliders;
    self.stats_.t_collision = self.ms_.t_collision;
  }

  void CityStream_upload(CityStream& self, CityStream_Tile& t, GeoChunk& chunk, const Material& material, bool walls = false) {
    u32 vertex_count = 0;
    WeldStats weld = WeldStats{0, 0, 0, 0, false};
    u32 id = 0;
    u32 allocate_id = 0;
    u32 vertex_count_2 = 0;
    MeshLook look = MeshLook{Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}}, 0, 0, true, 1, true};
    bool upsert = false;
    List<u32> t_2 = List<u32>{};
    u64 key = 0;
    List<u32> put = List<u32>{};
    u32 triangle_count = 0;
    vertex_count = engine_mesh_data_vertex_count(chunk.mesh);
    if (math_compare<"==", u32>(vertex_count, 0)) {
      return;
    }
    if (logic_not(chunk.welded)) {
      engine_weld(n85, chunk.mesh, 0);
      weld = n85.result;
      chunk.mesh = n85.mesh;
    }
    engine_renderer_allocate_id(n86, self.r_);
    allocate_id = n86.result;
    id = allocate_id;
    self.upload_count_ = kern::formula<u32>([](auto upload_count_) { return (upload_count_ + 1); }, self.upload_count_);
    vertex_count_2 = engine_mesh_data_vertex_count(chunk.mesh);
    self.upload_vertices_ = kern::formula<u64, u32>([](auto upload_vertices_, auto vertex_count_2) { return (upload_vertices_ + num_cast<u64, u32>(vertex_count_2)); }, self.upload_vertices_, vertex_count_2);
    look = MeshLook{Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}}, 0, 0, true, 1, true};
    look.material = material;
    look.extra_slot = kern::formula<bool, u32, u32>([](auto walls, auto a0, auto kMaxExtensionAttributes) { return (walls ? a0 : kMaxExtensionAttributes); }, walls, 0, kMaxExtensionAttributes);
    look.pick_bvh = false;
    engine_upsert(n87, self.r_, id, chunk.mesh, look, List<f32>{}, List<f32>{});
    upsert = n87.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, id);
      return;
    }
    list_add(t.meshes, id);
    if (walls) {
      key = cell_key(chunk.x, chunk.y).Result;
      map_add(n88, t.cell_ids, key);
      t_2 = t.cell_ids[n88.index];
      list_add(t_2, id);
      put = t_2;
      t.cell_ids[n88.index] = put;
    }
    triangle_count = engine_mesh_data_triangle_count(chunk.mesh);
    t.triangles = kern::formula<u32, u32>([](auto triangles, auto triangle_count) { return (triangles + triangle_count); }, t.triangles, triangle_count);
  }

  void CityStream_relay(CityStream& self, const Vec4& rect) {
    bool skip = false;
    for (auto it89 = flow_for(n89, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it89.next();) {
      skip = false;
      if (logic_or(logic_or(logic_or(math_compare<">", f32>(rect.x, self.tiles_[n89.index][0].bounds.z), math_compare<"<", f32>(rect.z, self.tiles_[n89.index][0].bounds.x)), math_compare<">", f32>(rect.y, self.tiles_[n89.index][0].bounds.w)), math_compare<"<", f32>(rect.w, self.tiles_[n89.index][0].bounds.y))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(self.tiles_[n89.index][0].relay, logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n89.index][0].state, CityStream_Tile_State::ready)))) {
          if (self.tiles_[n89.index][0].relay) {
            self.tiles_[n89.index][0].relay_again = true;
          }
          skip = true;
        }
        if (logic_not(skip)) {
          self.tiles_[n89.index][0].relay = true;
          var_take(n90, self.tiles_[n89.index][0].lay_meshes);
          self.tiles_[n89.index][0].old_lay = n90.value;
          list_clear(self.tiles_[n89.index][0].lay_meshes);
          CityStream_begin_roads(self, self.tiles_[n89.index][0]);
        }
      }
    }
  }

  struct CityStream_damage_buildingResult { bool Result = false; };
  CityStream_damage_buildingResult CityStream_damage_building(CityStream& self, u64 tile = 0, u32 index = 0, f32 new_top = 0.0f, bool leveled = false) {
    i64 it = 0;
    u32 key = 0;
    i64 d_row = 0;
    u64 key_2 = 0;
    u64 k = 0;
    bool stop = false;
    bool skip = false;
    bool valid = false;
    bool valid_2 = false;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<CityStream_Tile>>(self.tiles_, tile).index, map_length<u64, List<CityStream_Tile>>(self.tiles_));
    if (logic_or(logic_or(math_compare<"==", i64>(it, map_length<u64, List<CityStream_Tile>>(self.tiles_)), logic_not(self.tiles_[it][0].prepared)), math_compare<">=", u64>(num_cast<u64, u32>(index), num_cast<u64, i64>(list_length<MapFeatures_Building>(self.tiles_[it][0].features.buildings))))) {
      return {false};
    }
    if (logic_not(math_compare<"<", f32>(new_top, kern::formula<f32>([](auto a) { return (a - 0.01f); }, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].top)))) {
      return {false};
    }
    self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].top = kern::formula<f32, f32>([](auto a, auto new_top) { return fmax((a + 0.3f), new_top); }, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].base, new_top);
    self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].leveled = logic_or(self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].leveled, leveled);
    self.damage_serial_ = kern::formula<u32>([](auto damage_serial_) { return (damage_serial_ + 1); }, self.damage_serial_);
    key = self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].seed;
    map_add(n91, self.damage_, key);
    d_row = n91.index;
    self.damage_[d_row].height = kern::formula<f32, f32>([](auto a, auto a_2) { return (a - a_2); }, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].top, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].base);
    self.damage_[d_row].leveled = self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].leveled;
    if (logic_not(self.damage_quiet_)) {
      list_add(self.damage_log_, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].seed);
    }
    if (logic_and(math_compare<"<", u64>(num_cast<u64, u32>(index), num_cast<u64, i64>(list_length<Vector>(self.tiles_[it][0].cent))), num_cast<bool, i64>(num_cast<i64, bool>(map_contains<u64, List<u32>>(self.tiles_[it][0].cell_ids, cell_of(self.tiles_[it][0].cent[num_cast<i64, u32>(index)]).Result))))) {
      key_2 = cell_of(self.tiles_[it][0].cent[num_cast<i64, u32>(index)]).Result;
      set_add(n92, self.tiles_[it][0].dirty_cells, key_2);
    }
    if (self.near_.building) {
      self.near_.dirty = true;
      return {true};
    }
    k = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<", u64>(k, num_cast<u64, i64>(list_length<u64_u32_pair>(self.near_.list)))))) {
      skip = false;
      if (logic_or(math_compare<"!=", u64>(self.near_.list[num_cast<i64, u64>(k)].first, tile), math_compare<"!=", u32>(self.near_.list[num_cast<i64, u64>(k)].second, index))) {
        skip = true;
      }
      if (logic_not(logic_or(stop, skip))) {
        if (math_compare<"<", u64>(k, num_cast<u64, i64>(list_length<physics_Shape>(self.near_.shapes)))) {
          valid = engine_shape_valid(self.near_.shapes[num_cast<i64, u64>(k)]);
          if (valid) {
            engine_world_destroy_shape(self.world_, self.near_.shapes[num_cast<i64, u64>(k)]);
            self.near_.colliders = kern::formula<u32>([](auto colliders) { return (colliders - 1); }, self.near_.colliders);
          }
          n93 = CityStream_building_shape(self, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)], self.near_.body);
          self.near_.shapes[num_cast<i64, u64>(k)] = n93.Result;
          valid_2 = engine_shape_valid(self.near_.shapes[num_cast<i64, u64>(k)]);
          if (valid_2) {
            self.near_.colliders = kern::formula<u32>([](auto colliders) { return (colliders + 1); }, self.near_.colliders);
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

  struct damage_list_matchResult { bool Result = false; };
  KERN_PURE damage_list_matchResult damage_list_match(const CityStream_DamageRec& a, const CityStream_DamageRec& b) {
    return {math_compare<"<", u32>(a.seed, b.seed)};
  }

  void the_CityStream_damage_list_(const CityStream& self, List<CityStream_DamageRec>& out) {
    list_clear(out);
    list_reserve(out, map_length<u32, CityStream_Damage>(self.damage_));
    for (auto it94 = flow_for(n94, 0, math_sub<i64>(map_length<u32, CityStream_Damage>(self.damage_), 1)); it94.next();) {
      list_add(out, struct_make<CityStream_DamageRec>(map_key_at<u32, CityStream_Damage>(self.damage_, n94.index), self.damage_[n94.index].height, self.damage_[n94.index].leveled));
    }
    list_sort<&city_stream_cpp::damage_list_match>(*this, out);
  }

  struct CityStream_damage_ofResult { bool Result = false; };
  CityStream_damage_ofResult CityStream_damage_of(const CityStream& self, CityStream_DamageRec& out, u32 seed = 0) {
    i64 it = 0;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u32, CityStream_Damage>(self.damage_, seed).index, map_length<u32, CityStream_Damage>(self.damage_));
    if (math_compare<"==", i64>(it, map_length<u32, CityStream_Damage>(self.damage_))) {
      return {false};
    }
    out = struct_make<CityStream_DamageRec>(seed, self.damage_[it].height, self.damage_[it].leveled);
    return {true};
  }

  struct CityStream_merge_damageResult { bool Result = false; };
  CityStream_merge_damageResult CityStream_merge_damage(CityStream& self, const List<CityStream_DamageRec>& list, bool the_log = false) {
    Map<u32, u32> want = Map<u32, u32>{};
    bool changed = false;
    u32 i = 0;
    bool skip = false;
    CityStream_DamageRec r = CityStream_DamageRec{0, 0.0f, false};
    i64 it = 0;
    u32 put = 0;
    u32 key = 0;
    bool skip_2 = false;
    u32 i_2 = 0;
    bool skip_3 = false;
    i64 w = 0;
    MapFeatures_Building b = MapFeatures_Building{List<Vector_row>{}, 0.0f, 8.0f, 0, 0.0f, false};
    CityStream_DamageRec r_2 = CityStream_DamageRec{0, 0.0f, false};
    bool skip_4 = false;
    CityStream_DamageRec r_3 = CityStream_DamageRec{0, 0.0f, false};
    i64 it_2 = 0;
    CityStream_Damage put_2 = CityStream_Damage{0.0f, false};
    u32 key_2 = 0;
    if (math_compare<"==", i64>(list_length<CityStream_DamageRec>(list), 0)) {
      return {false};
    }
    want = Map<u32, u32>{};
    changed = false;
    i = 0;
    while (loop(math_compare<"<", u64>(num_cast<u64, u32>(i), num_cast<u64, i64>(list_length<CityStream_DamageRec>(list))))) {
      skip = false;
      r = list[num_cast<i64, u32>(i)];
      if (logic_or(logic_not(math_compare<">=", f32>(r.height, 0.0f)), logic_not(kern::formula<f32>([](auto height) { return isfinite(height); }, r.height)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u32, CityStream_Damage>(self.damage_, r.seed).index, map_length<u32, CityStream_Damage>(self.damage_));
        if (logic_and(logic_and(logic_not(math_compare<"==", i64>(it, map_length<u32, CityStream_Damage>(self.damage_))), math_compare<"<=", f32>(self.damage_[it].height, kern::formula<f32>([](auto height) { return (height + 0.01f); }, r.height))), logic_or(self.damage_[it].leveled, logic_not(r.leveled)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          put = i;
          key = r.seed;
          map_add(n95, want, key);
          want[n95.index] = put;
        }
      }
      i += 1;
    }
    if (math_compare<"==", i64>(map_length<u32, u32>(want), 0)) {
      return {false};
    }
    self.damage_quiet_ = logic_not(the_log);
    for (auto it96 = flow_for(n96, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it96.next();) {
      skip_2 = false;
      if (logic_not(self.tiles_[n96.index][0].prepared)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        i_2 = 0;
        while (loop(math_compare<"<", u64>(num_cast<u64, u32>(i_2), num_cast<u64, i64>(list_length<MapFeatures_Building>(self.tiles_[n96.index][0].features.buildings))))) {
          skip_3 = false;
          w = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u32, u32>(want, self.tiles_[n96.index][0].features.buildings[num_cast<i64, u32>(i_2)].seed).index, map_length<u32, u32>(want));
          if (math_compare<"==", i64>(w, map_length<u32, u32>(want))) {
            skip_3 = true;
          }
          if (logic_not(skip_3)) {
            b = self.tiles_[n96.index][0].features.buildings[num_cast<i64, u32>(i_2)];
            r_2 = list[num_cast<i64, u32>(want[w])];
            n97 = CityStream_damage_building(self, map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n96.index), i_2, kern::formula<f32, f32>([](auto base, auto height) { return (base + height); }, b.base, r_2.height), r_2.leveled);
            if (n97.Result) {
              changed = true;
            }
          }
          i_2 += 1;
        }
      }
    }
    for (auto it98 = flow_for(n98, 0, math_sub<i64>(map_length<u32, u32>(want), 1)); it98.next();) {
      skip_4 = false;
      r_3 = list[num_cast<i64, u32>(want[n98.index])];
      it_2 = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u32, CityStream_Damage>(self.damage_, map_key_at<u32, u32>(want, n98.index)).index, map_length<u32, CityStream_Damage>(self.damage_));
      if (math_compare<"==", i64>(it_2, map_length<u32, CityStream_Damage>(self.damage_))) {
        put_2 = struct_make<CityStream_Damage>(r_3.height, r_3.leveled);
        key_2 = map_key_at<u32, u32>(want, n98.index);
        map_add(n99, self.damage_, key_2);
        self.damage_[n99.index] = put_2;
      } else if (logic_or(math_compare<"<", f32>(r_3.height, kern::formula<f32>([](auto a) { return (a - 0.01f); }, self.damage_[it_2].height)), logic_and(r_3.leveled, logic_not(self.damage_[it_2].leveled)))) {
        self.damage_[it_2].height = kern::formula<f32, f32>([](auto a, auto height) { return fmin(a, height); }, self.damage_[it_2].height, r_3.height);
        self.damage_[it_2].leveled = logic_or(self.damage_[it_2].leveled, r_3.leveled);
      } else {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        changed = true;
        if (the_log) {
          list_add(self.damage_log_, map_key_at<u32, u32>(want, n98.index));
        }
      }
    }
    self.damage_quiet_ = false;
    if (changed) {
      self.damage_serial_ = kern::formula<u32>([](auto damage_serial_) { return (damage_serial_ + 1); }, self.damage_serial_);
    }
    return {changed};
  }

  void CityStream_clear_damage(CityStream& self) {
    bool skip = false;
    u32 i = 0;
    bool skip_2 = false;
    u64 key = 0;
    map_clear(self.damage_);
    list_clear(self.damage_log_);
    for (auto it100 = flow_for(n100, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it100.next();) {
      skip = false;
      if (logic_not(self.tiles_[n100.index][0].prepared)) {
        skip = true;
      }
      if (logic_not(skip)) {
        i = 0;
        while (loop(math_compare<"<", u64>(num_cast<u64, u32>(i), num_cast<u64, i64>(list_length<MapFeatures_Building>(self.tiles_[n100.index][0].features.buildings))))) {
          skip_2 = false;
          if (logic_and(math_compare<">=", f32>(self.tiles_[n100.index][0].features.buildings[num_cast<i64, u32>(i)].top, kern::formula<f32>([](auto a) { return (a - 0.01f); }, self.tiles_[n100.index][0].features.buildings[num_cast<i64, u32>(i)].full_top)), logic_not(self.tiles_[n100.index][0].features.buildings[num_cast<i64, u32>(i)].leveled))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            self.tiles_[n100.index][0].features.buildings[num_cast<i64, u32>(i)].top = self.tiles_[n100.index][0].features.buildings[num_cast<i64, u32>(i)].full_top;
            self.tiles_[n100.index][0].features.buildings[num_cast<i64, u32>(i)].leveled = false;
            if (logic_and(math_compare<"<", u64>(num_cast<u64, u32>(i), num_cast<u64, i64>(list_length<Vector>(self.tiles_[n100.index][0].cent))), num_cast<bool, i64>(num_cast<i64, bool>(map_contains<u64, List<u32>>(self.tiles_[n100.index][0].cell_ids, cell_of(self.tiles_[n100.index][0].cent[num_cast<i64, u32>(i)]).Result))))) {
              key = cell_of(self.tiles_[n100.index][0].cent[num_cast<i64, u32>(i)]).Result;
              set_add(n101, self.tiles_[n100.index][0].dirty_cells, key);
            }
          }
          i += 1;
        }
      }
    }
    self.near_.dirty = true;
  }

  struct begin_cells_matchResult { bool Result = false; };
  KERN_PURE begin_cells_matchResult begin_cells_match(const f32_u64_pair& a, const f32_u64_pair& b) {
    return {logic_or(math_compare<"<", f32>(a.first, b.first), logic_and(logic_not(math_compare<"<", f32>(b.first, a.first)), math_compare<"<", u64>(a.second, b.second)))};
  }

  void CityStream_begin_cells(CityStream& self, CityStream_Tile& t) {
    List<f32_u64_pair> order = List<f32_u64_pair>{};
    u64 c = 0;
    i32 cx = 0;
    i32 cy = 0;
    Object<kern::FootprintStream> fs = Object<kern::FootprintStream>{};
    Object<kern::FootprintStream> footprint_stream = Object<kern::FootprintStream>{};
    List<Vector> thin = List<Vector>{};
    bool skip = false;
    u64 key = 0;
    i64 it = 0;
    i64 at = 0;
    u32 i = 0;
    BuildingOptions a = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
    u32 arg = 0;
    u32 submit = 0;
    order = List<f32_u64_pair>{};
    for (auto it102 = flow_for(n102, 0, math_sub<i64>(set_length<u64>(t.dirty_cells), 1)); it102.next();) {
      c = set_key_at<u64>(t.dirty_cells, n102.index);
      cx = kern::formula<u64>([](auto c) { return num_cast<i32, u32>(num_cast<u32, u64>((c >> 32))); }, c);
      cy = num_cast<i32, u32>(num_cast<u32, u64>(c));
      list_add(order, struct_make<f32_u64_pair>(kern::formula<i32, f32, f32, i32, f32>([](auto cx, auto kChunk, auto x, auto cy, auto y) { return hypot((((num_cast<f32, i32>(cx) + 0.5f) * kChunk) - x), (((num_cast<f32, i32>(cy) + 0.5f) * kChunk) - y)); }, cx, kChunk, vector_break(self.focus_).x, cy, vector_break(self.focus_).y), c));
    }
    list_sort<&city_stream_cpp::begin_cells_match>(*this, order);
    if (math_compare<">", u64>(num_cast<u64, i64>(list_length<f32_u64_pair>(order)), 6)) {
      list_resize(order, 6, f32_u64_pair{0.0f, 0});
    }
    engine_footprint_stream_make(n103);
    footprint_stream = n103.result;
    fs = footprint_stream;
    thin = List<Vector>{};
    list_clear(t.job_cells);
    for (auto it104 = flow_for(n104, 0, math_sub<i64>(list_length<f32_u64_pair>(order), 1)); it104.next();) {
      skip = false;
      list_add(t.job_cells, order[n104.index].second);
      key = order[n104.index].second;
      set_remove(n105, t.dirty_cells, key);
      it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<u32>>(t.cell_items, order[n104.index].second).index, map_length<u64, List<u32>>(t.cell_items));
      if (math_compare<"==", i64>(it, map_length<u64, List<u32>>(t.cell_items))) {
        skip = true;
      }
      if (logic_not(skip)) {
        at = it;
        for (auto it106 = flow_for(n106, 0, math_sub<i64>(list_length<u32>(t.cell_items[at]), 1)); it106.next();) {
          i = t.cell_items[at][n106.index];
          put_footprint(t.features.buildings[num_cast<i64, u32>(i)], thin, fs, t.cent[num_cast<i64, u32>(i)]);
        }
      }
    }
    arg = kBuildings;
    a = building_options().Result;
    n107 = city_jobs_cpp::buildings_input(a, fs);
    engine_jobs_submit(n108, arg, n107.Result);
    submit = n108.result;
    t.cell_job = submit;
    if (object_valid<kern::FootprintStream>(fs)) {
      engine_footprint_stream_release(fs);
    }
    fs = Object<kern::FootprintStream>{};
  }

  void CityStream_step_time(CityStream& self, Text name = "", f64 t0 = 0.0) {
    f64 took = 0.0;
    f64 now_ms = 0.0;
    now_ms = engine_now_ms();
    took = kern::formula<f64, f64>([](auto now_ms, auto t0) { return (now_ms - t0); }, now_ms, t0);
    if (math_compare<">", f64>(took, self.worst_step_ms_)) {
      self.worst_step_ms_ = took;
      self.worst_step_ = name;
    }
  }

  struct CityStream_finish_cellsResult { bool Result = false; };
  CityStream_finish_cellsResult CityStream_finish_cells(CityStream& self, CityStream_Tile& t, f64 deadline = 0.0) {
    bool take = false;
    u64 c = 0;
    i64 it = 0;
    List<u32> put = List<u32>{};
    u64 key = 0;
    i64 at = 0;
    Material m = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}};
    bool stop = false;
    GeoChunk chunk = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}};
    f64 t1 = 0.0;
    f64 now_ms = 0.0;
    f64 now_ms_2 = 0.0;
    f64 now_ms_3 = 0.0;
    bool both = false;
    u32 id = 0;
    bool remove = false;
    u32 gone = 0;
    u64 c_2 = 0;
    u64 key_2 = 0;
    f64 now_ms_4 = 0.0;
    if (logic_not(t.cell_uploading)) {
      engine_jobs_take(n109, t.cell_job, t.built_bytes);
      take = n109.result;
      t.built_bytes = n109.out;
      if (logic_not(take)) {
        return {false};
      }
      t.cell_job = 0;
      for (auto it110 = flow_for(n110, 0, math_sub<i64>(list_length<u64>(t.job_cells), 1)); it110.next();) {
        c = t.job_cells[n110.index];
        it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<u32>>(t.cell_ids, c).index, map_length<u64, List<u32>>(t.cell_ids));
        if (logic_not(math_compare<"==", i64>(it, map_length<u64, List<u32>>(t.cell_ids)))) {
          var_take(n111, t.cell_ids[it]);
          put = n111.value;
          key = c;
          map_add(n112, t.cell_old, key);
          t.cell_old[n112.index] = put;
          at = it;
          map_remove_at(t.cell_ids, at);
        }
      }
      t.built_at = 4;
      t.upload_next = num_cast<u64, u32>(city_jobs_cpp::chunk_count(t.built_bytes).Result);
      t.cell_uploading = true;
    }
    m = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}};
    m.roughness = 0.82f;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<">", u64>(t.upload_next, 0)))) {
      chunk = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}};
      n113 = city_jobs_cpp::read_chunk(t.built_bytes, t.built_at, chunk);
      if (logic_not(n113.Result)) {
        t.upload_next = 0;
        stop = true;
      }
      if (logic_not(stop)) {
        t.upload_next = kern::formula<u64>([](auto upload_next) { return (upload_next - 1); }, t.upload_next);
        now_ms = engine_now_ms();
        t1 = now_ms;
        CityStream_upload(self, t, chunk, m, true);
        CityStream_step_time(self, "building.upload", t1);
        both = math_compare<">", u64>(t.upload_next, 0);
        if (both) {
          now_ms_3 = engine_now_ms();
          both = math_compare<">", f64>(now_ms_3, deadline);
        }
        if (both) {
          return {false};
        }
      }
    }
    list_clear(t.built_bytes);
    for (auto it114 = flow_for(n114, 0, math_sub<i64>(map_length<u64, List<u32>>(t.cell_old), 1)); it114.next();) {
      for (auto it115 = flow_for(n115, 0, math_sub<i64>(list_length<u32>(t.cell_old[n114.index]), 1)); it115.next();) {
        id = t.cell_old[n114.index][n115.index];
        engine_renderer_remove(n116, self.r_, id);
        remove = n116.result;
        gone = id;
        while (loop(list_contains<u32>(t.meshes, gone))) {
          list_remove(t.meshes, gone);
        }
      }
    }
    map_clear(t.cell_old);
    for (auto it117 = flow_for(n117, 0, math_sub<i64>(list_length<u64>(t.job_cells), 1)); it117.next();) {
      c_2 = t.job_cells[n117.index];
      key_2 = c_2;
      map_add(n118, t.cell_ids, key_2);
    }
    list_clear(t.job_cells);
    t.cell_uploading = false;
    self.rebuilds_ = kern::formula<u32>([](auto rebuilds_) { return (rebuilds_ + 1); }, self.rebuilds_);
    if (logic_and(math_compare<"<", f64>(self.ms_.t_built, 0.0), tiles::TileKey_equal(t.key, self.origin_tile_).Result)) {
      now_ms_4 = engine_now_ms();
      self.ms_.t_built = kern::formula<f64, f64>([](auto now_ms_4, auto start_ms_) { return (now_ms_4 - start_ms_); }, now_ms_4, self.start_ms_);
    }
    return {true};
  }

  struct CityStream_workResult { bool Result = false; };
  CityStream_workResult CityStream_work(CityStream& self, CityStream_Tile& t, f64 deadline = 0.0) {
    List<u8> out = List<u8>{};
    bool take = false;
    f64 now_ms = 0.0;
    f64 now_ms_2 = 0.0;
    bool chosen = false;
    Vec4 a = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vec4 a_2 = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    bool either = false;
    f64 now_ms_3 = 0.0;
    f64 now_ms_4 = 0.0;
    bool chosen_2 = false;
    f64 now_ms_5 = 0.0;
    bool chosen_3 = false;
    f64 t1 = 0.0;
    f64 now_ms_6 = 0.0;
    bool built = false;
    f64 now_ms_7 = 0.0;
    bool step = false;
    Material m = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}};
    u64 before = 0;
    f64 t1_2 = 0.0;
    f64 now_ms_8 = 0.0;
    GeoChunk chunk = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}};
    i64 place = 0;
    i64 first = 0;
    i64 last = 0;
    u64 was = 0;
    f64 now_ms_9 = 0.0;
    f64 now_ms_10 = 0.0;
    bool chosen_4 = false;
    u32 id = 0;
    bool remove = false;
    i64 at = 0;
    i64 count = 0;
    bool stop = false;
    Vector c = Vector{0.0, 0.0, 0.0};
    f64 t1_3 = 0.0;
    f64 now_ms_11 = 0.0;
    bool done = false;
    f64 now_ms_12 = 0.0;
    f64 t1_4 = 0.0;
    f64 now_ms_13 = 0.0;
    Vector mid = Vector{0.0, 0.0, 0.0};
    f64 now_ms_14 = 0.0;
    f64 now_ms_15 = 0.0;
    switch (flow_switch_enum<CityStream_Tile_State>(t.state)) {
      case CityStream_Tile_State::requested: {
        return {true};
      } break;
      case CityStream_Tile_State::decoding: {
        out = List<u8>{};
        engine_jobs_take(n119, t.tile_job, out);
        take = n119.result;
        out = n119.out;
        if (logic_not(take)) {
          return {true};
        }
        t.tile_job = 0;
        n120 = city_jobs_cpp::read_features(out, t.features);
        if (logic_not(n120.Result)) {
          t.state = CityStream_Tile_State::failed;
          return {true};
        }
        self.real_tiles_ = kern::formula<u32>([](auto real_tiles_) { return (real_tiles_ + 1); }, self.real_tiles_);
        if (logic_and(tiles::TileKey_equal(t.key, self.origin_tile_).Result, math_compare<"<", f64>(self.ms_.t_decoded, 0.0))) {
          now_ms = engine_now_ms();
          self.ms_.t_decoded = kern::formula<f64, f64>([](auto now_ms, auto start_ms_) { return (now_ms - start_ms_); }, now_ms, self.start_ms_);
        }
        CityStream_feed_terrain(self, t);
        t.state = CityStream_Tile_State::terrain_wait;
        now_ms_2 = engine_now_ms();
        if (math_compare<">", f64>(now_ms_2, deadline)) {
          chosen = false;
        } else {
          n121 = CityStream_work(self, t, deadline);
          chosen = n121.Result;
        }
        return {chosen};
      } break;
      case CityStream_Tile_State::terrain_wait: {
        either = logic_not(terrain::Terrain_settled(self.terrain_).Result);
        if (logic_not(either)) {
          a_2 = engine_math_math::Vec4_new(kern::formula<f32>([](auto x) { return (x - 80.0f); }, t.bounds.x), kern::formula<f32>([](auto y) { return (y - 80.0f); }, t.bounds.y), kern::formula<f32>([](auto z) { return (z + 80.0f); }, t.bounds.z), kern::formula<f32>([](auto w) { return (w + 80.0f); }, t.bounds.w)).Result;
          either = logic_not(terrain_cpp::Terrain_covers(self.terrain_, a_2).Result);
        }
        if (either) {
          return {true};
        }
        if (math_compare<"<", f64>(self.ms_.t_terrain, 0.0)) {
          now_ms_3 = engine_now_ms();
          self.ms_.t_terrain = kern::formula<f64, f64>([](auto now_ms_3, auto start_ms_) { return (now_ms_3 - start_ms_); }, now_ms_3, self.start_ms_);
        }
        CityStream_finish_features(self, t);
        now_ms_4 = engine_now_ms();
        if (math_compare<">", f64>(now_ms_4, deadline)) {
          chosen_2 = false;
        } else {
          n122 = CityStream_work(self, t, deadline);
          chosen_2 = n122.Result;
        }
        return {chosen_2};
      } break;
      case CityStream_Tile_State::building: {
        CityStream_begin_roads(self, t);
        now_ms_5 = engine_now_ms();
        if (math_compare<">", f64>(now_ms_5, deadline)) {
          chosen_3 = false;
        } else {
          n123 = CityStream_work(self, t, deadline);
          chosen_3 = n123.Result;
        }
        return {chosen_3};
      } break;
      case CityStream_Tile_State::roads: {
        now_ms_6 = engine_now_ms();
        t1 = now_ms_6;
        now_ms_7 = engine_now_ms();
        engine_road_builder_step(n124, t.road_builder, kern::formula<f64, f64>([](auto deadline, auto now_ms_7) { return max(0.5, (deadline - now_ms_7)); }, deadline, now_ms_7));
        step = n124.result;
        built = step;
        CityStream_step_time(self, "roads.build", t1);
        if (logic_not(built)) {
          return {false};
        }
        m = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}};
        m.roughness = 0.95f;
        m.cast_shadow = false;
        m.double_sided = true;
        engine_road_builder_chunks(n125, t.road_builder);
        while (loop(math_compare<"<", u64>(t.upload_next, num_cast<u64, i64>(list_length<GeoChunk>(n125.result))))) {
          before = num_cast<u64, i64>(list_length<u32>(t.meshes));
          now_ms_8 = engine_now_ms();
          t1_2 = now_ms_8;
          var_take(n126, n125.result[num_cast<i64, u64>(t.upload_next)]);
          n125.result[num_cast<i64, u64>(t.upload_next)].x = struct_break<GeoChunk>(n126.value).x;
          n125.result[num_cast<i64, u64>(t.upload_next)].y = struct_break<GeoChunk>(n126.value).y;
          n125.result[num_cast<i64, u64>(t.upload_next)].items = struct_break<GeoChunk>(n126.value).items;
          n125.result[num_cast<i64, u64>(t.upload_next)].triangles = struct_break<GeoChunk>(n126.value).triangles;
          n125.result[num_cast<i64, u64>(t.upload_next)].hash = struct_break<GeoChunk>(n126.value).hash;
          n125.result[num_cast<i64, u64>(t.upload_next)].welded = struct_break<GeoChunk>(n126.value).welded;
          n125.result[num_cast<i64, u64>(t.upload_next)].mesh.extra_size = struct_break<MeshData>(struct_break<GeoChunk>(n126.value).mesh).extra_size;
          chunk = n126.value;
          CityStream_upload(self, t, chunk, m, false);
          CityStream_step_time(self, "roads.upload", t1_2);
          place = list_length<u32>(t.lay_meshes);
          first = num_cast<i64, u64>(before);
          last = math_sub<i64>(list_length<u32>(t.meshes), 1);
          for (auto it127 = flow_for(n127, first, last); it127.next();) {
            list_insert(t.lay_meshes, math_add<i64>(place, math_sub<i64>(n127.index, first)), list_get<u32>(t.meshes, n127.index));
          }
          list_resize(t.meshes, num_cast<i64, u64>(before), 0);
          was = t.upload_next;
          t.upload_next = kern::formula<u64>([](auto upload_next) { return (upload_next + 1); }, t.upload_next);
          n125.result[num_cast<i64, u64>(was)].mesh = struct_make<MeshData>(List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{});
          now_ms_9 = engine_now_ms();
          if (math_compare<">", f64>(now_ms_9, deadline)) {
            return {false};
          }
        }
        list_clear(n125.result);
        t.upload_next = 0;
        t.state = CityStream_Tile_State::areas;
        now_ms_10 = engine_now_ms();
        if (math_compare<">", f64>(now_ms_10, deadline)) {
          chosen_4 = false;
        } else {
          n128 = CityStream_work(self, t, deadline);
          chosen_4 = n128.Result;
        }
        return {chosen_4};
      } break;
      case CityStream_Tile_State::areas: {
        if (t.relay) {
          for (auto it129 = flow_for(n129, 0, math_sub<i64>(list_length<u32>(t.old_lay), 1)); it129.next();) {
            id = t.old_lay[n129.index];
            engine_renderer_remove(n130, self.r_, id);
            remove = n130.result;
          }
          list_clear(t.old_lay);
          t.relay = false;
          t.state = CityStream_Tile_State::ready;
          if (t.relay_again) {
            t.relay_again = false;
            t.relay = true;
            var_take(n131, t.lay_meshes);
            t.old_lay = n131.value;
            list_clear(t.lay_meshes);
            CityStream_begin_roads(self, t);
          }
          return {true};
        }
        t.state = CityStream_Tile_State::ready;
        count = list_length<Vector>(t.cent);
        at = 0;
        stop = false;
        while (loop(logic_and(logic_not(stop), math_compare<"<", i64>(at, count)))) {
          c = t.cent[at];
          if (math_compare<"<=", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(c).x, vector_break(self.focus_).x, vector_break(c).y, vector_break(self.focus_).y), 340.0f)) {
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
        if (logic_or(num_cast<bool, u32>(t.cell_job), t.cell_uploading)) {
          now_ms_11 = engine_now_ms();
          t1_3 = now_ms_11;
          n132 = CityStream_finish_cells(self, t, deadline);
          done = n132.Result;
          CityStream_step_time(self, "cells.swap", t1_3);
          if (logic_not(done)) {
            now_ms_12 = engine_now_ms();
            return {math_compare<"<=", f64>(now_ms_12, deadline)};
          }
        }
        if (logic_and(logic_and(logic_not(math_compare<"==", i64>(set_length<u64>(t.dirty_cells), 0)), logic_not(num_cast<bool, u32>(t.cell_job))), logic_not(t.cell_uploading))) {
          now_ms_13 = engine_now_ms();
          t1_4 = now_ms_13;
          CityStream_begin_cells(self, t);
          CityStream_step_time(self, "cells.begin", t1_4);
        }
        mid = vector_make(kern::formula<f32, f32>([](auto x, auto x_2) { return (x + (x_2 * 0.5f)); }, vector_break(self.focus_).x, vector_break(self.view_lead).x), kern::formula<f32, f32>([](auto y, auto y_2) { return (y + (y_2 * 0.5f)); }, vector_break(self.focus_).y, vector_break(self.view_lead).y), 0.0f);
        if (logic_and(logic_and(logic_not(t.relay), math_compare<">=", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(mid).x, vector_break(t.roads_at).x, vector_break(mid).y, vector_break(t.roads_at).y), 260.0f)), math_compare<"<", f32>(rect_distance_to(t.bounds, vector_break(mid).x, vector_break(mid).y).Result, kern::formula<f32, f32>([](auto x, auto y) { return (980.0f + (hypot(x, y) * 0.5f)); }, vector_break(self.view_lead).x, vector_break(self.view_lead).y)))) {
          t.relay = true;
          var_take(n133, t.lay_meshes);
          t.old_lay = n133.value;
          list_clear(t.lay_meshes);
          CityStream_begin_roads(self, t);
          now_ms_14 = engine_now_ms();
          return {math_compare<"<=", f64>(now_ms_14, deadline)};
        }
        now_ms_15 = engine_now_ms();
        return {math_compare<"<=", f64>(now_ms_15, deadline)};
      } break;
      case CityStream_Tile_State::failed: {
        return {true};
      } break;
    }
  }

  struct update_grownResult { Vec4 Result = Vec4{0.0f, 0.0f, 0.0f, 0.0f}; };
  KERN_PURE update_grownResult update_grown(Vec4 r = Vec4{0.0f, 0.0f, 0.0f, 0.0f}) {
    return {engine_math_math::Vec4_new(kern::formula<f32>([](auto x) { return (x - 80.0f); }, r.x), kern::formula<f32>([](auto y) { return (y - 80.0f); }, r.y), kern::formula<f32>([](auto z) { return (z + 80.0f); }, r.z), kern::formula<f32>([](auto w) { return (w + 80.0f); }, r.w)).Result};
  }

  struct update_matchResult { bool Result = false; };
  KERN_PURE update_matchResult update_match(const f32_u64_pair& a, const f32_u64_pair& b) {
    return {math_compare<"<", f32>(a.first, b.first)};
  }

  void CityStream_update(CityStream& self, Vector focus = Vector{0.0, 0.0, 0.0}, f64 budget_ms = 0.0) {
    f64 start = 0.0;
    f64 now_ms = 0.0;
    f64 deadline = 0.0;
    f64 t0 = 0.0;
    f64 now_ms_2 = 0.0;
    f64 now_ms_3 = 0.0;
    f64 now_ms_4 = 0.0;
    f64 now_ms_5 = 0.0;
    f64 now_ms_6 = 0.0;
    List<f32_u64_pair> order = List<f32_u64_pair>{};
    i64 at = 0;
    i64 count = 0;
    bool stop = false;
    f64 now_ms_7 = 0.0;
    f64 now_ms_8 = 0.0;
    i64 it = 0;
    Text name = "";
    bool more = false;
    f64 now_ms_9 = 0.0;
    f64 now_ms_10 = 0.0;
    f64 now_ms_11 = 0.0;
    f64 now_ms_12 = 0.0;
    if (logic_or(logic_not(object_valid<kern::Renderer>(self.r_)), logic_not(object_valid<kern::physics::World>(self.world_)))) {
      return;
    }
    now_ms = engine_now_ms();
    start = now_ms;
    deadline = kern::formula<f64, f64>([](auto start, auto budget_ms) { return (start + budget_ms); }, start, budget_ms);
    now_ms_2 = engine_now_ms();
    t0 = now_ms_2;
    engine_jobs_pump(kern::formula<f64>([](auto budget_ms) { return (budget_ms * 0.5); }, budget_ms));
    CityStream_step_time(self, "jobs", t0);
    self.focus_ = focus;
    now_ms_3 = engine_now_ms();
    t0 = now_ms_3;
    CityStream_select_cells(self, focus, false);
    CityStream_step_time(self, "select_cells", t0);
    now_ms_4 = engine_now_ms();
    t0 = now_ms_4;
    CityStream_request_around(self, focus, self.ring);
    for (auto it134 = flow_for(n134, 0, math_sub<i64>(list_length<Vector>(self.extra_foci), 1)); it134.next();) {
      CityStream_request_around(self, self.extra_foci[n134.index], self.mate_ring);
    }
    for (auto it135 = flow_for(n135, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it135.next();) {
      if (logic_and(enum_equal<CityStream_Tile_State>(self.tiles_[n135.index][0].state, CityStream_Tile_State::failed), math_compare<">=", f64>(start, self.tiles_[n135.index][0].retry_ms))) {
        self.tiles_[n135.index][0].state = CityStream_Tile_State::requested;
        list_add(self.requests_, self.tiles_[n135.index][0].key);
      }
    }
    CityStream_unload_far(self, focus);
    CityStream_step_time(self, "request/unload", t0);
    now_ms_5 = engine_now_ms();
    t0 = now_ms_5;
    self.terrain_.extra = self.extra_foci;
    list_clear(self.terrain_.needs);
    if (logic_not(self.offline_)) {
      n136 = CityStream_ring_bounds(self, focus, self.ring);
      list_add(self.terrain_.needs, update_grown(n136.Result).Result);
      for (auto it137 = flow_for(n137, 0, math_sub<i64>(list_length<Vector>(self.extra_foci), 1)); it137.next();) {
        n138 = CityStream_ring_bounds(self, self.extra_foci[n137.index], self.mate_ring);
        list_add(self.terrain_.needs, update_grown(n138.Result).Result);
      }
    }
    terrain_cpp::Terrain_update(self.terrain_, focus, self.anchor, kern::formula<f64>([](auto budget_ms) { return fmax(1.0, (budget_ms * 0.45)); }, budget_ms));
    CityStream_step_time(self, "terrain", t0);
    now_ms_6 = engine_now_ms();
    t0 = now_ms_6;
    terrain::Terrain_take_changes(self.terrain_, self.terrain_changes_);
    for (auto it139 = flow_for(n139, 0, math_sub<i64>(list_length<Vec4>(self.terrain_changes_), 1)); it139.next();) {
      CityStream_relay(self, self.terrain_changes_[n139.index]);
      list_add(self.ground_changes_, self.terrain_changes_[n139.index]);
    }
    list_clear(self.terrain_changes_);
    CityStream_step_time(self, "relay", t0);
    order = List<f32_u64_pair>{};
    for (auto it140 = flow_for(n140, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it140.next();) {
      list_add(order, struct_make<f32_u64_pair>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return (((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(self.tiles_[n140.index][0].center).x, vector_break(focus).x, vector_break(self.tiles_[n140.index][0].center).y, vector_break(focus).y), map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n140.index)));
    }
    list_sort<&city_stream_cpp::update_match>(*this, order);
    count = list_length<f32_u64_pair>(order);
    at = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<", i64>(at, count)))) {
      now_ms_7 = engine_now_ms();
      if (math_compare<">", f64>(now_ms_7, deadline)) {
        stop = true;
      }
      if (logic_not(stop)) {
        now_ms_8 = engine_now_ms();
        t0 = now_ms_8;
        it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<CityStream_Tile>>(self.tiles_, order[at].second).index, map_length<u64, List<CityStream_Tile>>(self.tiles_));
        name = list_get<Text>(kStateName, num_cast<i64, u32>(num_cast<u32, i64>(enum_index<CityStream_Tile_State>(self.tiles_[it][0].state))));
        n141 = CityStream_work(self, self.tiles_[it][0], deadline);
        more = n141.Result;
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
    for (auto it142 = flow_for(n142, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it142.next();) {
      if (enum_equal<CityStream_Tile_State>(self.tiles_[n142.index][0].state, CityStream_Tile_State::ready)) {
        self.stats_.tiles_ready = kern::formula<u32>([](auto tiles_ready) { return (tiles_ready + 1); }, self.stats_.tiles_ready);
      } else if (enum_equal<CityStream_Tile_State>(self.tiles_[n142.index][0].state, CityStream_Tile_State::failed)) {
        self.stats_.tiles_failed = kern::formula<u32>([](auto tiles_failed) { return (tiles_failed + 1); }, self.stats_.tiles_failed);
      } else {
        self.stats_.tiles_loading = kern::formula<u32>([](auto tiles_loading) { return (tiles_loading + 1); }, self.stats_.tiles_loading);
      }
      self.stats_.buildings = kern::formula<u32, u64>([](auto buildings, auto a) { return (buildings + num_cast<u32, u64>(a)); }, self.stats_.buildings, num_cast<u64, i64>(list_length<MapFeatures_Building>(self.tiles_[n142.index][0].features.buildings)));
      self.stats_.triangles = kern::formula<u32, u32>([](auto triangles, auto a) { return (triangles + a); }, self.stats_.triangles, self.tiles_[n142.index][0].triangles);
      self.stats_.meshes = kern::formula<u32, u64, u64>([](auto meshes, auto a, auto a_2) { return (meshes + num_cast<u32, u64>((a + a_2))); }, self.stats_.meshes, num_cast<u64, i64>(list_length<u32>(self.tiles_[n142.index][0].meshes)), num_cast<u64, i64>(list_length<u32>(self.tiles_[n142.index][0].lay_meshes)));
    }
    self.stats_.colliders = self.near_.colliders;
    now_ms_9 = engine_now_ms();
    self.ms_.work_ms = kern::formula<f64, f64, f64>([](auto work_ms, auto now_ms_9, auto start) { return (work_ms + (now_ms_9 - start)); }, self.ms_.work_ms, now_ms_9, start);
    self.stats_.t_request = self.ms_.t_request;
    self.stats_.t_data = self.ms_.t_data;
    self.stats_.t_decoded = self.ms_.t_decoded;
    self.stats_.t_terrain = self.ms_.t_terrain;
    self.stats_.t_built = self.ms_.t_built;
    self.stats_.t_collision = self.ms_.t_collision;
    self.stats_.work_ms = self.ms_.work_ms;
    self.stats_.rebuilds = self.rebuilds_;
    self.stats_.uploads = self.upload_count_;
    self.stats_.upload_vertices = kern::formula<u64>([](auto upload_vertices_) { return num_cast<u32, u64>((upload_vertices_ / 1000)); }, self.upload_vertices_);
    self.stats_.relays = self.relays_;
    now_ms_10 = engine_now_ms();
    self.stats_.last_slice_ms = kern::formula<f64, f64>([](auto now_ms_10, auto start) { return (now_ms_10 - start); }, now_ms_10, start);
    now_ms_11 = engine_now_ms();
    if (math_compare<">", f64>(kern::formula<f64, f64>([](auto now_ms_11, auto worst_at_) { return (now_ms_11 - worst_at_); }, now_ms_11, self.worst_at_), 2000.0)) {
      self.worst_ = 0.0;
      now_ms_12 = engine_now_ms();
      self.worst_at_ = now_ms_12;
      self.worst_step_ = "";
      self.worst_step_ms_ = 0.0;
    }
    self.worst_ = kern::formula<f64, f64>([](auto worst_, auto last_slice_ms) { return fmax(worst_, last_slice_ms); }, self.worst_, self.stats_.last_slice_ms);
    self.stats_.worst_ms = self.worst_;
    self.stats_.worst_step = self.worst_step_;
    self.stats_.worst_step_ms = self.worst_step_ms_;
  }

  void CityStream_walls_near(const CityStream& self, List<CityStream_Wall>& out, Vector p = Vector{0.0, 0.0, 0.0}, f32 r = 0.0f) {
    bool skip = false;
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<u32>{}, physics_Body{0}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0f, 0.0f, 0.0f, 0.0f}, List<u32>{}, List<u32>{}, false, false, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, Map<u64, List<u32>>{}, Set<u64>{}, Map<u64, List<u32>>{}, false, 0, 0, Map<u64, List<u32>>{}, Set<u64>{}, List<u64>{}, 0, List<u8>{}, 0, false, -1, physics_Body{0}, false};
    bool skip_2 = false;
    Vector q = Vector{0.0, 0.0, 0.0};
    u32 edge = 0;
    bool skip_3 = false;
    List<Vector> ring = List<Vector>{};
    bool ccw = false;
    f32 s = 0.0f;
    u64 i = 0;
    bool skip_4 = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    f32 len = 0.0f;
    Vector d = Vector{0.0, 0.0, 0.0};
    f32 u = 0.0f;
    Vector n = Vector{0.0, 0.0, 0.0};
    f32 side = 0.0f;
    list_clear(out);
    for (auto it143 = flow_for(n143, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it143.next();) {
      skip = false;
      t = list_get<CityStream_Tile>(self.tiles_[n143.index], 0);
      if (logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(logic_or(logic_or(math_compare<"<", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto x, auto r) { return ((x - r) - 200.0f); }, t.bounds.x, r)), math_compare<">", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto z, auto r) { return ((z + r) + 200.0f); }, t.bounds.z, r))), math_compare<"<", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto y, auto r) { return ((y - r) - 200.0f); }, t.bounds.y, r))), math_compare<">", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto w, auto r) { return ((w + r) + 200.0f); }, t.bounds.w, r)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it144 = flow_for(n144, 0, math_sub<i64>(list_length<MapFeatures_Building>(t.features.buildings), 1)); it144.next();) {
            skip_2 = false;
            if (logic_or(math_compare<"==", i64>(list_length<Vector_row>(t.features.buildings[n144.index].rings), 0), math_compare<"==", i64>(list_length<Vector>(t.features.buildings[n144.index].rings[0].items), 0))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              q = t.features.buildings[n144.index].rings[0].items[0];
              if (logic_or(math_compare<">", f32>(kern::formula<f32, f32>([](auto x, auto x_2) { return abs((x - x_2)); }, vector_break(q).x, vector_break(p).x), kern::formula<f32>([](auto r) { return (r + 150.0f); }, r)), math_compare<">", f32>(kern::formula<f32, f32>([](auto y, auto y_2) { return abs((y - y_2)); }, vector_break(q).y, vector_break(p).y), kern::formula<f32>([](auto r) { return (r + 150.0f); }, r)))) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                edge = 0;
                for (auto it145 = flow_for(n145, 0, math_sub<i64>(list_length<Vector_row>(t.features.buildings[n144.index].rings), 1)); it145.next();) {
                  skip_3 = false;
                  ring = t.features.buildings[n144.index].rings[n145.index].items;
                  if (math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(ring)), 3)) {
                    skip_3 = true;
                  }
                  if (logic_not(skip_3)) {
                    ccw = math_compare<">", f64>(signed_area(ring).Result, 0.0);
                    s = kern::formula<bool, i64, f32, f32>([](auto ccw, auto index, auto a0f, auto a) { return ((num_cast<i32, bool>(ccw) == num_cast<i32, bool>((num_cast<u64, i64>(index) == 0))) ? a0f : a); }, ccw, n145.index, 1.0f, -1.0f);
                    i = 0;
                    while (loop(math_compare<"<", u64>(i, num_cast<u64, i64>(list_length<Vector>(ring))))) {
                      skip_4 = false;
                      a = ring[num_cast<i64, u64>(i)];
                      c = ring[num_cast<i64, u64>(kern::formula<u64, u64>([](auto i, auto a) { return ((i + 1) % a); }, i, num_cast<u64, i64>(list_length<Vector>(ring))))];
                      len = kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(c).x, vector_break(a).x, vector_break(c).y, vector_break(a).y);
                      if (math_compare<"<", f32>(len, 0.05f)) {
                        skip_4 = true;
                      }
                      if (logic_not(skip_4)) {
                        d = vector_make(kern::formula<f32, f32, f32>([](auto x, auto x_2, auto len) { return ((x - x_2) / len); }, vector_break(c).x, vector_break(a).x, len), kern::formula<f32, f32, f32>([](auto y, auto y_2, auto len) { return ((y - y_2) / len); }, vector_break(c).y, vector_break(a).y, len), 0.0f);
                        u = kern::formula<f32, f32, f32, f32, f32, f32>([](auto x, auto x_2, auto x_3, auto y, auto y_2, auto y_3) { return (((x - x_2) * x_3) + ((y - y_2) * y_3)); }, vector_break(p).x, vector_break(a).x, vector_break(d).x, vector_break(p).y, vector_break(a).y, vector_break(d).y);
                        n = vector_make(kern::formula<f32, f32>([](auto y, auto s) { return (y * s); }, vector_break(d).y, s), kern::formula<f32, f32>([](auto x, auto s) { return ((-x) * s); }, vector_break(d).x, s), 0.0f);
                        side = kern::formula<f32, f32, f32, f32, f32, f32>([](auto x, auto x_2, auto x_3, auto y, auto y_2, auto y_3) { return (((x - x_2) * x_3) + ((y - y_2) * y_3)); }, vector_break(p).x, vector_break(a).x, vector_break(n).x, vector_break(p).y, vector_break(a).y, vector_break(n).y);
                        if (logic_or(logic_or(math_compare<"<", f32>(u, kern::formula<f32>([](auto r) { return (-r); }, r)), math_compare<">", f32>(u, kern::formula<f32, f32>([](auto len, auto r) { return (len + r); }, len, r))), math_compare<">", f32>(kern::formula<f32>([](auto side) { return abs(side); }, side), r))) {
                          skip_4 = true;
                        }
                        if (logic_not(skip_4)) {
                          list_add(out, struct_make<CityStream_Wall>(a, d, n, len, t.features.buildings[n144.index].base, t.features.buildings[n144.index].top, t.features.buildings[n144.index].seed, edge));
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

  struct CityStream_building_atResult { bool Result = false; };
  KERN_PURE CityStream_building_atResult CityStream_building_at(const CityStream& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    bool skip = false;
    CityStream_Tile t = CityStream_Tile{CityStream_Tile_State::requested, TileKey{14, 0, 0}, 0, 0.0, false, MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}, Object<kern::FootprintStream>{}, Object<kern::RoadStream>{}, Object<kern::BuildingBuilder>{}, Object<kern::RoadBuilder>{}, List<u32>{}, physics_Body{0}, 0, 0, 0, 0, Vector{0.0, 0.0, 0.0}, Vec4{0.0f, 0.0f, 0.0f, 0.0f}, List<u32>{}, List<u32>{}, false, false, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, Map<u64, List<u32>>{}, Set<u64>{}, Map<u64, List<u32>>{}, false, 0, 0, Map<u64, List<u32>>{}, Set<u64>{}, List<u64>{}, 0, List<u8>{}, 0, false, -1, physics_Body{0}, false};
    bool skip_2 = false;
    List<Vector> ring = List<Vector>{};
    bool inside = false;
    u64 i = 0;
    u64 j = 0;
    u64 was = 0;
    for (auto it146 = flow_for(n146, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it146.next();) {
      skip = false;
      t = list_get<CityStream_Tile>(self.tiles_[n146.index], 0);
      if (logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(logic_or(logic_or(math_compare<"<", f32>(vector_break(p).x, kern::formula<f32>([](auto x) { return (x - 200.0f); }, t.bounds.x)), math_compare<">", f32>(vector_break(p).x, kern::formula<f32>([](auto z) { return (z + 200.0f); }, t.bounds.z))), math_compare<"<", f32>(vector_break(p).y, kern::formula<f32>([](auto y) { return (y - 200.0f); }, t.bounds.y))), math_compare<">", f32>(vector_break(p).y, kern::formula<f32>([](auto w) { return (w + 200.0f); }, t.bounds.w)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it147 = flow_for(n147, 0, math_sub<i64>(list_length<MapFeatures_Building>(t.features.buildings), 1)); it147.next();) {
            skip_2 = false;
            if (logic_or(math_compare<"==", i64>(list_length<Vector_row>(t.features.buildings[n147.index].rings), 0), math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(t.features.buildings[n147.index].rings[0].items)), 3))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              ring = t.features.buildings[n147.index].rings[0].items;
              if (logic_or(math_compare<">", f32>(kern::formula<f32, f32>([](auto x, auto x_2) { return abs((x - x_2)); }, vector_break(ring[0]).x, vector_break(p).x), 150.0f), math_compare<">", f32>(kern::formula<f32, f32>([](auto y, auto y_2) { return abs((y - y_2)); }, vector_break(ring[0]).y, vector_break(p).y), 150.0f))) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                inside = false;
                i = 0;
                j = kern::formula<u64>([](auto a) { return (a - 1); }, num_cast<u64, i64>(list_length<Vector>(ring)));
                while (loop(math_compare<"<", u64>(i, num_cast<u64, i64>(list_length<Vector>(ring))))) {
                  if (logic_and(math_compare<"!=", i32>(num_cast<i32, bool>(math_compare<">", f32>(vector_break(ring[num_cast<i64, u64>(i)]).y, vector_break(p).y)), num_cast<i32, bool>(math_compare<">", f32>(vector_break(ring[num_cast<i64, u64>(j)]).y, vector_break(p).y))), math_compare<"<", f32>(vector_break(p).x, kern::formula<f32, f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2, auto y_3) { return ((((x - x_2) * (y - y_2)) / (y_3 - y_2)) + x_2); }, vector_break(ring[num_cast<i64, u64>(j)]).x, vector_break(ring[num_cast<i64, u64>(i)]).x, vector_break(p).y, vector_break(ring[num_cast<i64, u64>(i)]).y, vector_break(ring[num_cast<i64, u64>(j)]).y)))) {
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

  struct CityStream_street_pointResult { bool Result = false; };
  CityStream_street_pointResult CityStream_street_point(const CityStream& self, Rng& rng, Vector& point, f32& heading, Vector near = Vector{0.0, 0.0, 0.0}, f32 radius = 0.0f) {
    bool skip = false;
    u64 l_at = 0;
    u64 i = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    f32 t = 0.0f;
    Vector p = Vector{0.0, 0.0, 0.0};
    f32 d = 0.0f;
    f32 off = 0.0f;
    if (math_compare<"==", i64>(list_length<Lane>(self.lanes_), 0)) {
      return {false};
    }
    for (auto it148 = flow_for(n148, 0, 47); it148.next();) {
      skip = false;
      n149 = common::Rng_next(rng);
      l_at = kern::formula<u32, u64>([](auto the_result, auto a) { return (num_cast<u64, u32>(the_result) % a); }, n149.Result, num_cast<u64, i64>(list_length<Lane>(self.lanes_)));
      if (logic_or(logic_not(self.lanes_[num_cast<i64, u64>(l_at)].alive), math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(self.lanes_[num_cast<i64, u64>(l_at)].points)), 2))) {
        skip = true;
      }
      if (logic_not(skip)) {
        n150 = common::Rng_next(rng);
        i = kern::formula<u32, u64>([](auto the_result, auto a) { return (num_cast<u64, u32>(the_result) % (a - 1)); }, n150.Result, num_cast<u64, i64>(list_length<Vector>(self.lanes_[num_cast<i64, u64>(l_at)].points)));
        a = self.lanes_[num_cast<i64, u64>(l_at)].points[num_cast<i64, u64>(i)];
        b = self.lanes_[num_cast<i64, u64>(l_at)].points[num_cast<i64, u64>(kern::formula<u64>([](auto i) { return (i + 1); }, i))];
        n151 = common::Rng_unit(rng);
        t = n151.Result;
        p = vector_add(a, vector_scale(vector_sub(b, a), t));
        d = kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(p).x, vector_break(near).x, vector_break(p).y, vector_break(near).y);
        if (logic_or(math_compare<">", f32>(d, radius), math_compare<"<", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(b).x, vector_break(a).x, vector_break(b).y, vector_break(a).y), 4.0f))) {
          skip = true;
        }
        if (logic_not(skip)) {
          heading = kern::formula<f32, f32, f32, f32>([](auto y, auto y_2, auto x, auto x_2) { return atan2((y - y_2), (x - x_2)); }, vector_break(b).y, vector_break(a).y, vector_break(b).x, vector_break(a).x);
          off = kern::formula<f32>([](auto a) { return fmin((a * 0.25f), 1.8f); }, self.lanes_[num_cast<i64, u64>(l_at)].width);
          point = vector_make(kern::formula<f32, f32, f32>([](auto x, auto heading, auto off) { return (x + (sin(heading) * off)); }, vector_break(p).x, heading, off), kern::formula<f32, f32, f32>([](auto y, auto heading, auto off) { return (y - (cos(heading) * off)); }, vector_break(p).y, heading, off), CityStream_ground_height(self, vector_break(p).x, vector_break(p).y).Result);
          return {true};
        }
      }
    }
    return {false};
  }

  void CityStream_buildings_near(const CityStream& self, List<CityStream_BuildingRef>& out, Vector p = Vector{0.0, 0.0, 0.0}, f32 radius = 0.0f) {
    bool skip = false;
    bool skip_2 = false;
    MapFeatures_Building b = MapFeatures_Building{List<Vector_row>{}, 0.0f, 8.0f, 0, 0.0f, false};
    f32 x0 = 0.0f;
    f32 y0 = 0.0f;
    f32 x1 = 0.0f;
    f32 y1 = 0.0f;
    Vector q = Vector{0.0, 0.0, 0.0};
    for (auto it152 = flow_for(n152, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it152.next();) {
      skip = false;
      if (logic_and(logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n152.index][0].state, CityStream_Tile_State::ready)), logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n152.index][0].state, CityStream_Tile_State::collision)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it153 = flow_for(n153, 0, math_sub<i64>(list_length<MapFeatures_Building>(self.tiles_[n152.index][0].features.buildings), 1)); it153.next();) {
          skip_2 = false;
          b = self.tiles_[n152.index][0].features.buildings[n153.index];
          if (logic_or(math_compare<"==", i64>(list_length<Vector_row>(b.rings), 0), math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(b.rings[0].items)), 3))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            x0 = 1e+09f;
            y0 = 1e+09f;
            x1 = -1e+09f;
            y1 = -1e+09f;
            for (auto it154 = flow_for(n154, 0, math_sub<i64>(list_length<Vector>(b.rings[0].items), 1)); it154.next();) {
              q = b.rings[0].items[n154.index];
              x0 = kern::formula<f32, f32>([](auto x0, auto x) { return fmin(x0, x); }, x0, vector_break(q).x);
              y0 = kern::formula<f32, f32>([](auto y0, auto y) { return fmin(y0, y); }, y0, vector_break(q).y);
              x1 = kern::formula<f32, f32>([](auto x1, auto x) { return fmax(x1, x); }, x1, vector_break(q).x);
              y1 = kern::formula<f32, f32>([](auto y1, auto y) { return fmax(y1, y); }, y1, vector_break(q).y);
            }
            if (logic_or(logic_or(logic_or(math_compare<"<", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto x0, auto radius) { return (x0 - radius); }, x0, radius)), math_compare<">", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto x1, auto radius) { return (x1 + radius); }, x1, radius))), math_compare<"<", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto y0, auto radius) { return (y0 - radius); }, y0, radius))), math_compare<">", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto y1, auto radius) { return (y1 + radius); }, y1, radius)))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              list_add(out, struct_make<CityStream_BuildingRef>(map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n152.index), num_cast<u32, i64>(n153.index)));
            }
          }
        }
      }
    }
  }

  void CityStream_ready_tiles(const CityStream& self, List<u64>& out) {
    for (auto it155 = flow_for(n155, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it155.next();) {
      if (self.tiles_[n155.index][0].prepared) {
        list_add(out, map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n155.index));
      }
    }
  }

  void CityStream_areas_near(const CityStream& self, List<CityStream_AreaRef>& out, Vector p = Vector{0.0, 0.0, 0.0}, f32 radius = 0.0f, AreaKind kind = AreaKind::water) {
    bool skip = false;
    bool skip_2 = false;
    MapFeatures_Area a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    f32 x0 = 0.0f;
    f32 y0 = 0.0f;
    f32 x1 = 0.0f;
    f32 y1 = 0.0f;
    Vector q = Vector{0.0, 0.0, 0.0};
    for (auto it156 = flow_for(n156, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it156.next();) {
      skip = false;
      if (logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n156.index][0].state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it157 = flow_for(n157, 0, math_sub<i64>(list_length<MapFeatures_Area>(self.tiles_[n156.index][0].features.areas), 1)); it157.next();) {
          skip_2 = false;
          a = self.tiles_[n156.index][0].features.areas[n157.index];
          if (logic_or(logic_or(logic_not(enum_equal<AreaKind>(a.kind, kind)), math_compare<"==", i64>(list_length<Vector_row>(a.rings), 0)), math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(a.rings[0].items)), 3))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            x0 = 1e+09f;
            y0 = 1e+09f;
            x1 = -1e+09f;
            y1 = -1e+09f;
            for (auto it158 = flow_for(n158, 0, math_sub<i64>(list_length<Vector>(a.rings[0].items), 1)); it158.next();) {
              q = a.rings[0].items[n158.index];
              x0 = kern::formula<f32, f32>([](auto x0, auto x) { return fmin(x0, x); }, x0, vector_break(q).x);
              y0 = kern::formula<f32, f32>([](auto y0, auto y) { return fmin(y0, y); }, y0, vector_break(q).y);
              x1 = kern::formula<f32, f32>([](auto x1, auto x) { return fmax(x1, x); }, x1, vector_break(q).x);
              y1 = kern::formula<f32, f32>([](auto y1, auto y) { return fmax(y1, y); }, y1, vector_break(q).y);
            }
            if (logic_or(logic_or(logic_or(math_compare<"<", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto x0, auto radius) { return (x0 - radius); }, x0, radius)), math_compare<">", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto x1, auto radius) { return (x1 + radius); }, x1, radius))), math_compare<"<", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto y0, auto radius) { return (y0 - radius); }, y0, radius))), math_compare<">", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto y1, auto radius) { return (y1 + radius); }, y1, radius)))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              list_add(out, struct_make<CityStream_AreaRef>(kern::formula<u64, i64>([](auto a, auto index) { return ((a * 65536) + num_cast<u64, i64>(index)); }, map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n156.index), n157.index), map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n156.index), num_cast<u32, i64>(n157.index)));
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

  void CityStream_exclude_building(CityStream& self, u64 tile = 0, i32 index = 0) {
    i64 it = 0;
    i64 it_2 = 0;
    if (logic_and(math_compare<"==", u64>(tile, self.excl_tile_), math_compare<"==", i32>(index, self.excl_index_))) {
      return;
    }
    if (num_cast<bool, u64>(self.excl_tile_)) {
      it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<CityStream_Tile>>(self.tiles_, self.excl_tile_).index, map_length<u64, List<CityStream_Tile>>(self.tiles_));
      if (logic_not(math_compare<"==", i64>(it, map_length<u64, List<CityStream_Tile>>(self.tiles_)))) {
        self.tiles_[it][0].skip = -1;
        CityStream_recollide(self, self.tiles_[it][0]);
      }
    }
    self.excl_tile_ = tile;
    self.excl_index_ = kern::formula<u64, i32, i32>([](auto tile, auto index, auto a) { return (num_cast<bool, u64>(tile) ? index : a); }, tile, index, -1);
    if (num_cast<bool, u64>(tile)) {
      it_2 = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<CityStream_Tile>>(self.tiles_, tile).index, map_length<u64, List<CityStream_Tile>>(self.tiles_));
      if (logic_not(math_compare<"==", i64>(it_2, map_length<u64, List<CityStream_Tile>>(self.tiles_)))) {
        self.tiles_[it_2][0].skip = index;
        CityStream_recollide(self, self.tiles_[it_2][0]);
      }
    }
  }

  struct CityStream_exclusion_doneResult { bool Result = false; };
  KERN_PURE CityStream_exclusion_doneResult CityStream_exclusion_done(const CityStream& self) {
    return {logic_and(logic_not(self.near_.building), logic_not(self.near_.dirty))};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
  struct : kern::Node<12> { i64 index = 0; } n12;
  struct { bool result = false; } n13;
  struct : kern::Node<14> { i64 index = 0; } n14;
  struct : kern::Node<15> { i64 index = 0; } n15;
  struct { bool result = false; } n16;
  struct : kern::Node<17> { i64 index = 0; } n17;
  struct : kern::Node<18> { i64 index = 0; } n18;
  struct : kern::Node<19> { i64 index = 0; } n19;
  struct { bool result = false; GeoProjector self = GeoProjector{0.0, 0.0, 1.0}; } n20;
  struct : kern::Node<21> { i64 index = 0; } n21;
  struct : kern::Node<22> { i64 index = 0; } n22;
  struct : kern::Node<23> { i64 index = 0; } n23;
  struct { Object<kern::FootprintStream> result = Object<kern::FootprintStream>{}; } n24;
  struct { Object<kern::RoadStream> result = Object<kern::RoadStream>{}; } n25;
  struct { Object<kern::BuildingBuilder> result = Object<kern::BuildingBuilder>{}; } n26;
  struct { Object<kern::RoadBuilder> result = Object<kern::RoadBuilder>{}; } n27;
  struct { f64 x = 0.0; f64 y = 0.0; } n28;
  struct { f64 x = 0.0; f64 y = 0.0; } n29;
  struct { f64 x = 0.0; f64 y = 0.0; } n30;
  struct { bool added = false; i64 index = 0; } n31;
  struct { List<CityStream_Tile> value = List<CityStream_Tile>{}; } n32;
  struct { f64 x = 0.0; f64 y = 0.0; } n33;
  struct { f64 x = 0.0; f64 y = 0.0; } n34;
  struct : kern::Node<35> { i64 index = 0; } n35;
  struct : kern::Node<36> { i64 index = 0; } n36;
  struct : kern::Node<37> { i64 index = 0; } n37;
  city_jobs_cpp::tile_inputResult n38;
  struct { u32 result = 0; } n39;
  struct : kern::Node<40> { i64 index = 0; } n40;
  struct : kern::Node<41> { i64 index = 0; } n41;
  struct { Object<kern::FootprintStream> result = Object<kern::FootprintStream>{}; } n42;
  struct { Object<kern::RoadStream> result = Object<kern::RoadStream>{}; } n43;
  struct { Object<kern::BuildingBuilder> result = Object<kern::BuildingBuilder>{}; } n44;
  struct { Object<kern::RoadBuilder> result = Object<kern::RoadBuilder>{}; } n45;
  struct : kern::Node<46> { i64 index = 0; } n46;
  struct : kern::Node<47> { i64 index = 0; } n47;
  struct : kern::Node<48> { i64 index = 0; } n48;
  struct : kern::Node<49> { i64 index = 0; } n49;
  struct : kern::Node<50> { i64 index = 0; } n50;
  tiles_cpp::clip_lineResult n51;
  struct : kern::Node<52> { i64 index = 0; } n52;
  struct { List<Vector> value = List<Vector>{}; } n53;
  struct { bool added = false; i64 index = 0; } n54;
  struct : kern::Node<55> { i64 index = 0; } n55;
  struct : kern::Node<56> { i64 index = 0; } n56;
  struct { bool added = false; i64 index = 0; } n57;
  struct { bool added = false; i64 index = 0; } n58;
  struct { bool removed = false; } n59;
  struct : kern::Node<60> { i64 index = 0; } n60;
  struct { bool result = false; } n61;
  struct { bool removed = false; } n62;
  struct : kern::Node<63> { i64 index = 0; } n63;
  struct : kern::Node<64> { i64 index = 0; } n64;
  struct : kern::Node<65> { i64 index = 0; } n65;
  struct : kern::Node<66> { i64 index = 0; } n66;
  struct { bool result = false; } n67;
  struct { bool added = false; i64 index = 0; } n68;
  struct { List<CityStream_Tile> value = List<CityStream_Tile>{}; } n69;
  struct : kern::Node<70> { i64 index = 0; } n70;
  struct : kern::Node<71> { i64 index = 0; } n71;
  struct : kern::Node<72> { i64 index = 0; } n72;
  struct : kern::Node<73> { i64 index = 0; } n73;
  struct : kern::Node<74> { i64 index = 0; } n74;
  struct { physics_Body result = physics_Body{0}; } n75;
  struct { Object<kern::Earcut> result = Object<kern::Earcut>{}; } n76;
  struct : kern::Node<77> { i64 index = 0; } n77;
  struct : kern::Node<78> { i64 index = 0; } n78;
  struct : kern::Node<79> { i64 index = 0; } n79;
  struct { List<u32> out = List<u32>{}; } n80;
  struct : kern::Node<81> { i64 index = 0; } n81;
  struct { physics_Shape result = physics_Shape{0}; } n82;
  CityStream_building_shapeResult n83;
  CityStream_build_nearResult n84;
  struct { WeldStats result = WeldStats{0, 0, 0, 0, false}; MeshData mesh = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n85;
  struct { u32 result = 0; } n86;
  struct { bool result = false; } n87;
  struct { bool added = false; i64 index = 0; } n88;
  struct : kern::Node<89> { i64 index = 0; } n89;
  struct { List<u32> value = List<u32>{}; } n90;
  struct { bool added = false; i64 index = 0; } n91;
  struct { bool added = false; i64 index = 0; } n92;
  CityStream_building_shapeResult n93;
  struct : kern::Node<94> { i64 index = 0; } n94;
  struct { bool added = false; i64 index = 0; } n95;
  struct : kern::Node<96> { i64 index = 0; } n96;
  CityStream_damage_buildingResult n97;
  struct : kern::Node<98> { i64 index = 0; } n98;
  struct { bool added = false; i64 index = 0; } n99;
  struct : kern::Node<100> { i64 index = 0; } n100;
  struct { bool added = false; i64 index = 0; } n101;
  struct : kern::Node<102> { i64 index = 0; } n102;
  struct { Object<kern::FootprintStream> result = Object<kern::FootprintStream>{}; } n103;
  struct : kern::Node<104> { i64 index = 0; } n104;
  struct { bool removed = false; } n105;
  struct : kern::Node<106> { i64 index = 0; } n106;
  city_jobs_cpp::buildings_inputResult n107;
  struct { u32 result = 0; } n108;
  struct { bool result = false; List<u8> out = List<u8>{}; } n109;
  struct : kern::Node<110> { i64 index = 0; } n110;
  struct { List<u32> value = List<u32>{}; } n111;
  struct { bool added = false; i64 index = 0; } n112;
  city_jobs_cpp::read_chunkResult n113;
  struct : kern::Node<114> { i64 index = 0; } n114;
  struct : kern::Node<115> { i64 index = 0; } n115;
  struct { bool result = false; } n116;
  struct : kern::Node<117> { i64 index = 0; } n117;
  struct { bool added = false; i64 index = 0; } n118;
  struct { bool result = false; List<u8> out = List<u8>{}; } n119;
  city_jobs_cpp::read_featuresResult n120;
  CityStream_workResult n121;
  CityStream_workResult n122;
  CityStream_workResult n123;
  struct { bool result = false; } n124;
  struct { engine_road_builder_chunks_place<List<GeoChunk>> result = {}; } n125;
  struct { GeoChunk value = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}}; } n126;
  struct : kern::Node<127> { i64 index = 0; } n127;
  CityStream_workResult n128;
  struct : kern::Node<129> { i64 index = 0; } n129;
  struct { bool result = false; } n130;
  struct { List<u32> value = List<u32>{}; } n131;
  CityStream_finish_cellsResult n132;
  struct { List<u32> value = List<u32>{}; } n133;
  struct : kern::Node<134> { i64 index = 0; } n134;
  struct : kern::Node<135> { i64 index = 0; } n135;
  CityStream_ring_boundsResult n136;
  struct : kern::Node<137> { i64 index = 0; } n137;
  CityStream_ring_boundsResult n138;
  struct : kern::Node<139> { i64 index = 0; } n139;
  struct : kern::Node<140> { i64 index = 0; } n140;
  CityStream_workResult n141;
  struct : kern::Node<142> { i64 index = 0; } n142;
  struct : kern::Node<143> { i64 index = 0; } n143;
  struct : kern::Node<144> { i64 index = 0; } n144;
  struct : kern::Node<145> { i64 index = 0; } n145;
  struct : kern::Node<146> { i64 index = 0; } n146;
  struct : kern::Node<147> { i64 index = 0; } n147;
  struct : kern::Node<148> { i64 index = 0; } n148;
  common::Rng_nextResult n149;
  common::Rng_nextResult n150;
  common::Rng_unitResult n151;
  struct : kern::Node<152> { i64 index = 0; } n152;
  struct : kern::Node<153> { i64 index = 0; } n153;
  struct : kern::Node<154> { i64 index = 0; } n154;
  struct : kern::Node<155> { i64 index = 0; } n155;
  struct : kern::Node<156> { i64 index = 0; } n156;
  struct : kern::Node<157> { i64 index = 0; } n157;
  struct : kern::Node<158> { i64 index = 0; } n158;
};
