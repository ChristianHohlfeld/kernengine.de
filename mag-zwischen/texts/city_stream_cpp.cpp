struct city_stream_cpp : virtual kern::Library, virtual city_jobs_cpp, virtual city_stream, virtual common, virtual engine_math_math, virtual terrain, virtual terrain_cpp, virtual tiles, virtual tiles_cpp {
  // ---- variables ----
  /// the building mesh chunks (BuildingOptions::chunk_size)
  KERN_CONST f32 kChunk = 160.0f;
  /// OpenFreeMap's planet tiles go up to z14 (overzoomed beyond)
  KERN_CONST i32 kTileZoom = 14;
  KERN_CONST u32 kTile = 1;
  /// The map features of a tile that is not loaded: none.
  KERN_CONST MapFeatures kNoFeatures = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
  /// vertex locations 14 and 15
  KERN_CONST u32 kMaxExtensionAttributes = 2;
  /// buildings of a new set a step (480 in 30 steps, an eighth of a second)
  KERN_CONST u32 kNearPerStep = 16;
  /// 250 ms at the world's 240 Hz (world_building_collisions.mjs: 250 ms)
  KERN_CONST u32 kNearSteps = 60;

  // ---- functions ----
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

  void CityStream_furniture(const CityStream& self, List<u64>& out) {
    list_clear(out);
    for (auto it46 = flow_for(n46, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it46.next();) {
      if (self.tiles_[n46.index][0].prepared) {
        list_add(out, map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n46.index));
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
      for (auto it47 = flow_for(n47, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it47.next();) {
        if (self.tiles_[n47.index][0].procedural) {
          return {enum_equal<CityStream_Tile_State>(self.tiles_[n47.index][0].state, CityStream_Tile_State::ready)};
        }
      }
      return {false};
    }
    return {terrain_cpp::Terrain_corridors_at(self.terrain_, vector_break(p).x, vector_break(p).y).Result};
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
    for (auto it49 = flow_for(n49, 0, math_sub<i64>(list_length<u32>(t.cell_ids[at]), 1)); it49.next();) {
      id = t.cell_ids[at][n49.index];
      engine_renderer_remove(n50, self.r_, id);
      remove = n50.result;
      gone = id;
      while (loop(list_contains<u32>(t.meshes, gone))) {
        list_remove(t.meshes, gone);
      }
    }
    at_2 = it;
    map_remove_at(t.cell_ids, at_2);
    key = cell;
    set_remove(n51, t.dirty_cells, key);
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
      engine_weld(n52, chunk.mesh, 0);
      weld = n52.result;
      chunk.mesh = n52.mesh;
    }
    engine_renderer_allocate_id(n53, self.r_);
    allocate_id = n53.result;
    id = allocate_id;
    self.upload_count_ = kern::formula<u32>([](auto upload_count_) { return (upload_count_ + 1); }, self.upload_count_);
    vertex_count_2 = engine_mesh_data_vertex_count(chunk.mesh);
    self.upload_vertices_ = kern::formula<u64, u32>([](auto upload_vertices_, auto vertex_count_2) { return (upload_vertices_ + num_cast<u64, u32>(vertex_count_2)); }, self.upload_vertices_, vertex_count_2);
    look = MeshLook{Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}}, 0, 0, true, 1, true};
    look.material = material;
    look.extra_slot = kern::formula<bool, u32, u32>([](auto walls, auto a0, auto kMaxExtensionAttributes) { return (walls ? a0 : kMaxExtensionAttributes); }, walls, 0, kMaxExtensionAttributes);
    look.pick_bvh = false;
    engine_upsert(n54, self.r_, id, chunk.mesh, look, List<f32>{}, List<f32>{});
    upsert = n54.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, id);
      return;
    }
    list_add(t.meshes, id);
    if (walls) {
      key = cell_key(chunk.x, chunk.y).Result;
      map_add(n55, t.cell_ids, key);
      t_2 = t.cell_ids[n55.index];
      list_add(t_2, id);
      put = t_2;
      t.cell_ids[n55.index] = put;
    }
    triangle_count = engine_mesh_data_triangle_count(chunk.mesh);
    t.triangles = kern::formula<u32, u32>([](auto triangles, auto triangle_count) { return (triangles + triangle_count); }, t.triangles, triangle_count);
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
    engine_earcut_make(n59);
    earcut_2 = n59.result;
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
    for (auto it60 = flow_for(n60, 0, math_sub<i64>(list_length<Vector_row>(rings), 1)); it60.next();) {
      if (math_compare<">", u64>(num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(rings[n60.index]).items)), 64)) {
        step = kern::formula<u64>([](auto a) { return ((a + 63) / 64); }, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(rings[n60.index]).items)));
        thin = List<Vector>{};
        k = 0;
        while (loop(math_compare<"<", u64>(k, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(rings[n60.index]).items))))) {
          list_add(thin, rings[n60.index].items[num_cast<i64, u64>(k)]);
          k += step;
        }
        rings[n60.index].items = thin;
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
          for (auto it61 = flow_for(n61, 0, math_sub<i64>(list_length<Vector>(rings[at].items), 1)); it61.next();) {
            p = rings[at].items[n61.index];
            place = list_length<f64>(xy);
            item = num_cast<f64, f32>(vector_break(p).x);
            item_2 = num_cast<f64, f32>(vector_break(p).y);
            list_insert_list(xy, place, list_make<f64, "2">(item, item_2));
          }
          for (auto it62 = flow_for(n62, 0, math_sub<i64>(list_length<Vector>(rings[num_cast<i64, u64>(r)].items), 1)); it62.next();) {
            p0 = rings[num_cast<i64, u64>(r)].items[n62.index];
            p1 = rings[num_cast<i64, u64>(r)].items[num_cast<i64, u64>(kern::formula<i64, u64>([](auto index, auto a) { return ((num_cast<u64, i64>(index) + 1) % a); }, n62.index, num_cast<u64, i64>(list_length<Vector>(rings[num_cast<i64, u64>(r)].items))))];
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
      engine_earcut_triangulate(n63, earcut, xy, kern::formula<u64>([](auto a) { return num_cast<u32, u64>((a / 2)); }, num_cast<u64, i64>(list_length<f64>(xy))), holes, num_cast<u32, i64>(list_length<u32>(holes)), tris);
      tris = n63.out;
      base_2 = num_cast<u32, i64>(list_length<Vector>(pts));
      for (auto it64 = flow_for(n64, 0, math_sub<i64>(num_cast<i64, u64>(kern::formula<u64>([](auto a) { return (a / 2); }, num_cast<u64, i64>(list_length<f64>(xy)))), 1)); it64.next();) {
        list_add(pts, vector_make(num_cast<f32, f64>(xy[num_cast<i64, u64>(kern::formula<i64>([](auto index) { return (num_cast<u64, i64>(index) * 2); }, n64.index))]), num_cast<f32, f64>(xy[num_cast<i64, u64>(kern::formula<i64>([](auto index) { return ((num_cast<u64, i64>(index) * 2) + 1); }, n64.index))]), b.top));
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
      engine_world_create_shape(n65, self.world_, body, ss);
      create_shape = n65.result;
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
      engine_world_create_body(n58, self.world_, bs);
      create_body = n58.result;
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
        n66 = CityStream_building_shape(self, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(v.second)], self.near_.body);
        self.near_.shapes[num_cast<i64, u64>(slot)] = n66.Result;
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
    map_add(n67, self.damage_, key);
    d_row = n67.index;
    self.damage_[d_row].height = kern::formula<f32, f32>([](auto a, auto a_2) { return (a - a_2); }, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].top, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].base);
    self.damage_[d_row].leveled = self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].leveled;
    if (logic_not(self.damage_quiet_)) {
      list_add(self.damage_log_, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)].seed);
    }
    if (logic_and(math_compare<"<", u64>(num_cast<u64, u32>(index), num_cast<u64, i64>(list_length<Vector>(self.tiles_[it][0].cent))), num_cast<bool, i64>(num_cast<i64, bool>(map_contains<u64, List<u32>>(self.tiles_[it][0].cell_ids, cell_of(self.tiles_[it][0].cent[num_cast<i64, u32>(index)]).Result))))) {
      key_2 = cell_of(self.tiles_[it][0].cent[num_cast<i64, u32>(index)]).Result;
      set_add(n68, self.tiles_[it][0].dirty_cells, key_2);
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
          n69 = CityStream_building_shape(self, self.tiles_[it][0].features.buildings[num_cast<i64, u32>(index)], self.near_.body);
          self.near_.shapes[num_cast<i64, u64>(k)] = n69.Result;
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
    for (auto it70 = flow_for(n70, 0, math_sub<i64>(map_length<u32, CityStream_Damage>(self.damage_), 1)); it70.next();) {
      list_add(out, struct_make<CityStream_DamageRec>(map_key_at<u32, CityStream_Damage>(self.damage_, n70.index), self.damage_[n70.index].height, self.damage_[n70.index].leveled));
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
          map_add(n71, want, key);
          want[n71.index] = put;
        }
      }
      i += 1;
    }
    if (math_compare<"==", i64>(map_length<u32, u32>(want), 0)) {
      return {false};
    }
    self.damage_quiet_ = logic_not(the_log);
    for (auto it72 = flow_for(n72, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it72.next();) {
      skip_2 = false;
      if (logic_not(self.tiles_[n72.index][0].prepared)) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        i_2 = 0;
        while (loop(math_compare<"<", u64>(num_cast<u64, u32>(i_2), num_cast<u64, i64>(list_length<MapFeatures_Building>(self.tiles_[n72.index][0].features.buildings))))) {
          skip_3 = false;
          w = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u32, u32>(want, self.tiles_[n72.index][0].features.buildings[num_cast<i64, u32>(i_2)].seed).index, map_length<u32, u32>(want));
          if (math_compare<"==", i64>(w, map_length<u32, u32>(want))) {
            skip_3 = true;
          }
          if (logic_not(skip_3)) {
            b = self.tiles_[n72.index][0].features.buildings[num_cast<i64, u32>(i_2)];
            r_2 = list[num_cast<i64, u32>(want[w])];
            n73 = CityStream_damage_building(self, map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n72.index), i_2, kern::formula<f32, f32>([](auto base, auto height) { return (base + height); }, b.base, r_2.height), r_2.leveled);
            if (n73.Result) {
              changed = true;
            }
          }
          i_2 += 1;
        }
      }
    }
    for (auto it74 = flow_for(n74, 0, math_sub<i64>(map_length<u32, u32>(want), 1)); it74.next();) {
      skip_4 = false;
      r_3 = list[num_cast<i64, u32>(want[n74.index])];
      it_2 = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u32, CityStream_Damage>(self.damage_, map_key_at<u32, u32>(want, n74.index)).index, map_length<u32, CityStream_Damage>(self.damage_));
      if (math_compare<"==", i64>(it_2, map_length<u32, CityStream_Damage>(self.damage_))) {
        put_2 = struct_make<CityStream_Damage>(r_3.height, r_3.leveled);
        key_2 = map_key_at<u32, u32>(want, n74.index);
        map_add(n75, self.damage_, key_2);
        self.damage_[n75.index] = put_2;
      } else if (logic_or(math_compare<"<", f32>(r_3.height, kern::formula<f32>([](auto a) { return (a - 0.01f); }, self.damage_[it_2].height)), logic_and(r_3.leveled, logic_not(self.damage_[it_2].leveled)))) {
        self.damage_[it_2].height = kern::formula<f32, f32>([](auto a, auto height) { return fmin(a, height); }, self.damage_[it_2].height, r_3.height);
        self.damage_[it_2].leveled = logic_or(self.damage_[it_2].leveled, r_3.leveled);
      } else {
        skip_4 = true;
      }
      if (logic_not(skip_4)) {
        changed = true;
        if (the_log) {
          list_add(self.damage_log_, map_key_at<u32, u32>(want, n74.index));
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
    for (auto it76 = flow_for(n76, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it76.next();) {
      skip = false;
      if (logic_not(self.tiles_[n76.index][0].prepared)) {
        skip = true;
      }
      if (logic_not(skip)) {
        i = 0;
        while (loop(math_compare<"<", u64>(num_cast<u64, u32>(i), num_cast<u64, i64>(list_length<MapFeatures_Building>(self.tiles_[n76.index][0].features.buildings))))) {
          skip_2 = false;
          if (logic_and(math_compare<">=", f32>(self.tiles_[n76.index][0].features.buildings[num_cast<i64, u32>(i)].top, kern::formula<f32>([](auto a) { return (a - 0.01f); }, self.tiles_[n76.index][0].features.buildings[num_cast<i64, u32>(i)].full_top)), logic_not(self.tiles_[n76.index][0].features.buildings[num_cast<i64, u32>(i)].leveled))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            self.tiles_[n76.index][0].features.buildings[num_cast<i64, u32>(i)].top = self.tiles_[n76.index][0].features.buildings[num_cast<i64, u32>(i)].full_top;
            self.tiles_[n76.index][0].features.buildings[num_cast<i64, u32>(i)].leveled = false;
            if (logic_and(math_compare<"<", u64>(num_cast<u64, u32>(i), num_cast<u64, i64>(list_length<Vector>(self.tiles_[n76.index][0].cent))), num_cast<bool, i64>(num_cast<i64, bool>(map_contains<u64, List<u32>>(self.tiles_[n76.index][0].cell_ids, cell_of(self.tiles_[n76.index][0].cent[num_cast<i64, u32>(i)]).Result))))) {
              key = cell_of(self.tiles_[n76.index][0].cent[num_cast<i64, u32>(i)]).Result;
              set_add(n77, self.tiles_[n76.index][0].dirty_cells, key);
            }
          }
          i += 1;
        }
      }
    }
    self.near_.dirty = true;
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
    for (auto it87 = flow_for(n87, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it87.next();) {
      skip = false;
      t = list_get<CityStream_Tile>(self.tiles_[n87.index], 0);
      if (logic_not(enum_equal<CityStream_Tile_State>(t.state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        if (logic_or(logic_or(logic_or(math_compare<"<", f32>(vector_break(p).x, kern::formula<f32>([](auto x) { return (x - 200.0f); }, t.bounds.x)), math_compare<">", f32>(vector_break(p).x, kern::formula<f32>([](auto z) { return (z + 200.0f); }, t.bounds.z))), math_compare<"<", f32>(vector_break(p).y, kern::formula<f32>([](auto y) { return (y - 200.0f); }, t.bounds.y))), math_compare<">", f32>(vector_break(p).y, kern::formula<f32>([](auto w) { return (w + 200.0f); }, t.bounds.w)))) {
          skip = true;
        }
        if (logic_not(skip)) {
          for (auto it88 = flow_for(n88, 0, math_sub<i64>(list_length<MapFeatures_Building>(t.features.buildings), 1)); it88.next();) {
            skip_2 = false;
            if (logic_or(math_compare<"==", i64>(list_length<Vector_row>(t.features.buildings[n88.index].rings), 0), math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(t.features.buildings[n88.index].rings[0].items)), 3))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              ring = t.features.buildings[n88.index].rings[0].items;
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

  void CityStream_buildings_near(const CityStream& self, List<CityStream_BuildingRef>& out, Vector p = Vector{0.0, 0.0, 0.0}, f32 radius = 0.0f) {
    bool skip = false;
    bool skip_2 = false;
    MapFeatures_Building b = MapFeatures_Building{List<Vector_row>{}, 0.0f, 8.0f, 0, 0.0f, false};
    f32 x0 = 0.0f;
    f32 y0 = 0.0f;
    f32 x1 = 0.0f;
    f32 y1 = 0.0f;
    Vector q = Vector{0.0, 0.0, 0.0};
    for (auto it93 = flow_for(n93, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it93.next();) {
      skip = false;
      if (logic_and(logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n93.index][0].state, CityStream_Tile_State::ready)), logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n93.index][0].state, CityStream_Tile_State::collision)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it94 = flow_for(n94, 0, math_sub<i64>(list_length<MapFeatures_Building>(self.tiles_[n93.index][0].features.buildings), 1)); it94.next();) {
          skip_2 = false;
          b = self.tiles_[n93.index][0].features.buildings[n94.index];
          if (logic_or(math_compare<"==", i64>(list_length<Vector_row>(b.rings), 0), math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(b.rings[0].items)), 3))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            x0 = 1e+09f;
            y0 = 1e+09f;
            x1 = -1e+09f;
            y1 = -1e+09f;
            for (auto it95 = flow_for(n95, 0, math_sub<i64>(list_length<Vector>(b.rings[0].items), 1)); it95.next();) {
              q = b.rings[0].items[n95.index];
              x0 = kern::formula<f32, f32>([](auto x0, auto x) { return fmin(x0, x); }, x0, vector_break(q).x);
              y0 = kern::formula<f32, f32>([](auto y0, auto y) { return fmin(y0, y); }, y0, vector_break(q).y);
              x1 = kern::formula<f32, f32>([](auto x1, auto x) { return fmax(x1, x); }, x1, vector_break(q).x);
              y1 = kern::formula<f32, f32>([](auto y1, auto y) { return fmax(y1, y); }, y1, vector_break(q).y);
            }
            if (logic_or(logic_or(logic_or(math_compare<"<", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto x0, auto radius) { return (x0 - radius); }, x0, radius)), math_compare<">", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto x1, auto radius) { return (x1 + radius); }, x1, radius))), math_compare<"<", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto y0, auto radius) { return (y0 - radius); }, y0, radius))), math_compare<">", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto y1, auto radius) { return (y1 + radius); }, y1, radius)))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              list_add(out, struct_make<CityStream_BuildingRef>(map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n93.index), num_cast<u32, i64>(n94.index)));
            }
          }
        }
      }
    }
  }

  void CityStream_ready_tiles(const CityStream& self, List<u64>& out) {
    for (auto it96 = flow_for(n96, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it96.next();) {
      if (self.tiles_[n96.index][0].prepared) {
        list_add(out, map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n96.index));
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
    for (auto it97 = flow_for(n97, 0, math_sub<i64>(map_length<u64, List<CityStream_Tile>>(self.tiles_), 1)); it97.next();) {
      skip = false;
      if (logic_not(enum_equal<CityStream_Tile_State>(self.tiles_[n97.index][0].state, CityStream_Tile_State::ready))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it98 = flow_for(n98, 0, math_sub<i64>(list_length<MapFeatures_Area>(self.tiles_[n97.index][0].features.areas), 1)); it98.next();) {
          skip_2 = false;
          a = self.tiles_[n97.index][0].features.areas[n98.index];
          if (logic_or(logic_or(logic_not(enum_equal<AreaKind>(a.kind, kind)), math_compare<"==", i64>(list_length<Vector_row>(a.rings), 0)), math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(a.rings[0].items)), 3))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            x0 = 1e+09f;
            y0 = 1e+09f;
            x1 = -1e+09f;
            y1 = -1e+09f;
            for (auto it99 = flow_for(n99, 0, math_sub<i64>(list_length<Vector>(a.rings[0].items), 1)); it99.next();) {
              q = a.rings[0].items[n99.index];
              x0 = kern::formula<f32, f32>([](auto x0, auto x) { return fmin(x0, x); }, x0, vector_break(q).x);
              y0 = kern::formula<f32, f32>([](auto y0, auto y) { return fmin(y0, y); }, y0, vector_break(q).y);
              x1 = kern::formula<f32, f32>([](auto x1, auto x) { return fmax(x1, x); }, x1, vector_break(q).x);
              y1 = kern::formula<f32, f32>([](auto y1, auto y) { return fmax(y1, y); }, y1, vector_break(q).y);
            }
            if (logic_or(logic_or(logic_or(math_compare<"<", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto x0, auto radius) { return (x0 - radius); }, x0, radius)), math_compare<">", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto x1, auto radius) { return (x1 + radius); }, x1, radius))), math_compare<"<", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto y0, auto radius) { return (y0 - radius); }, y0, radius))), math_compare<">", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto y1, auto radius) { return (y1 + radius); }, y1, radius)))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              list_add(out, struct_make<CityStream_AreaRef>(kern::formula<u64, i64>([](auto a, auto index) { return ((a * 65536) + num_cast<u64, i64>(index)); }, map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n97.index), n98.index), map_key_at<u64, List<CityStream_Tile>>(self.tiles_, n97.index), num_cast<u32, i64>(n98.index)));
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
  struct : kern::Node<46> { i64 index = 0; } n46;
  struct : kern::Node<47> { i64 index = 0; } n47;
  struct : kern::Node<49> { i64 index = 0; } n49;
  struct { bool result = false; } n50;
  struct { bool removed = false; } n51;
  struct { WeldStats result = WeldStats{0, 0, 0, 0, false}; MeshData mesh = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n52;
  struct { u32 result = 0; } n53;
  struct { bool result = false; } n54;
  struct { bool added = false; i64 index = 0; } n55;
  struct { physics_Body result = physics_Body{0}; } n58;
  struct { Object<kern::Earcut> result = Object<kern::Earcut>{}; } n59;
  struct : kern::Node<60> { i64 index = 0; } n60;
  struct : kern::Node<61> { i64 index = 0; } n61;
  struct : kern::Node<62> { i64 index = 0; } n62;
  struct { List<u32> out = List<u32>{}; } n63;
  struct : kern::Node<64> { i64 index = 0; } n64;
  struct { physics_Shape result = physics_Shape{0}; } n65;
  CityStream_building_shapeResult n66;
  struct { bool added = false; i64 index = 0; } n67;
  struct { bool added = false; i64 index = 0; } n68;
  CityStream_building_shapeResult n69;
  struct : kern::Node<70> { i64 index = 0; } n70;
  struct { bool added = false; i64 index = 0; } n71;
  struct : kern::Node<72> { i64 index = 0; } n72;
  CityStream_damage_buildingResult n73;
  struct : kern::Node<74> { i64 index = 0; } n74;
  struct { bool added = false; i64 index = 0; } n75;
  struct : kern::Node<76> { i64 index = 0; } n76;
  struct { bool added = false; i64 index = 0; } n77;
  struct : kern::Node<87> { i64 index = 0; } n87;
  struct : kern::Node<88> { i64 index = 0; } n88;
  struct : kern::Node<93> { i64 index = 0; } n93;
  struct : kern::Node<94> { i64 index = 0; } n94;
  struct : kern::Node<95> { i64 index = 0; } n95;
  struct : kern::Node<96> { i64 index = 0; } n96;
  struct : kern::Node<97> { i64 index = 0; } n97;
  struct : kern::Node<98> { i64 index = 0; } n98;
  struct : kern::Node<99> { i64 index = 0; } n99;
};
