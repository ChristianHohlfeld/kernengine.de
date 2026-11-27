struct city_jobs_cpp : virtual kern::Library, virtual mvt, virtual mvt_cpp, virtual tiles_cpp, virtual wire {
  // ---- types ----
  KERN_STRUCT(cityjobs_Reader) { u64 p = 0; bool ok = true; };

  // ---- variables ----
  KERN_CONST List<Text> wanted = List<Text>{"building", "transportation", "water", "waterway", "park", "landcover", "landuse"};
  KERN_CONST u32 kTile = 1;
  KERN_CONST u32 kBuildings = 2;

  // ---- functions ----
  void put_array_u32(List<u8>& b, const List<u32>& v) {
    u64 n = 0;
    wire::put_u32(b, num_cast<u32, i64>(list_length<u32>(v)));
    n = num_cast<u64, i64>(list_length<u8>(b));
    list_resize(b, num_cast<i64, u64>(kern::formula<u64, u64>([](auto n, auto a) { return (n + (a * 4)); }, n, num_cast<u64, i64>(list_length<u32>(v)))), 0);
    if (logic_not(math_compare<"==", i64>(list_length<u32>(v), 0))) {
      list_copy_bytes<"u8", "u32">(b, num_cast<i64, u64>(n), v, 0, num_cast<i64, u64>(kern::formula<u64>([](auto a) { return (a * 4); }, num_cast<u64, i64>(list_length<u32>(v)))));
    }
  }

  void put_array_f64(List<u8>& b, const List<f64>& v) {
    u64 n = 0;
    wire::put_u32(b, num_cast<u32, i64>(list_length<f64>(v)));
    n = num_cast<u64, i64>(list_length<u8>(b));
    list_resize(b, num_cast<i64, u64>(kern::formula<u64, u64>([](auto n, auto a) { return (n + (a * 8)); }, n, num_cast<u64, i64>(list_length<f64>(v)))), 0);
    if (logic_not(math_compare<"==", i64>(list_length<f64>(v), 0))) {
      list_copy_bytes<"u8", "f64">(b, num_cast<i64, u64>(n), v, 0, num_cast<i64, u64>(kern::formula<u64>([](auto a) { return (a * 8); }, num_cast<u64, i64>(list_length<f64>(v)))));
    }
  }

  void put_points(List<u8>& b, const List<Vector>& v) {
    Vector p = Vector{0.0, 0.0, 0.0};
    wire::put_u32(b, num_cast<u32, i64>(list_length<Vector>(v)));
    for (auto it10 = flow_for(n10, 0, math_sub<i64>(list_length<Vector>(v), 1)); it10.next();) {
      p = v[n10.index];
      wire::put_f32(b, vector_break(p).x);
      wire::put_f32(b, vector_break(p).y);
    }
  }

  void put_spots(List<u8>& b, const List<MapFeatures_Spot>& v) {
    wire::put_u32(b, num_cast<u32, i64>(list_length<MapFeatures_Spot>(v)));
    for (auto it11 = flow_for(n11, 0, math_sub<i64>(list_length<MapFeatures_Spot>(v), 1)); it11.next();) {
      wire::put_f32(b, vector_break(v[n11.index].p).x);
      wire::put_f32(b, vector_break(v[n11.index].p).y);
      wire::put_f32(b, v[n11.index].a);
    }
  }

  /// (An i32 goes as the u32 of its bits: the same four bytes.)
  void put_i32(List<u8>& b, i32 v = 0) {
    wire::put_u32(b, num_cast<u32, i32>(v));
  }

  struct Reader_get_u8Result { u8 Result = 0; };
  Reader_get_u8Result Reader_get_u8(cityjobs_Reader& self, List<u8> b = List<u8>{}) {
    u8 v = 0;
    if (math_compare<"<", u64>(num_cast<u64, i64>(list_length<u8>(b)), kern::formula<u64>([](auto p) { return (p + 1); }, self.p))) {
      self.ok = false;
      return {0};
    }
    v = b[num_cast<i64, u64>(self.p)];
    self.p = kern::formula<u64>([](auto p) { return (p + 1); }, self.p);
    return {v};
  }

  struct Reader_get_u32Result { u32 Result = 0; };
  Reader_get_u32Result Reader_get_u32(cityjobs_Reader& self, List<u8> b = List<u8>{}) {
    wire_U32 x = wire_U32{0};
    if (math_compare<"<", u64>(num_cast<u64, i64>(list_length<u8>(b)), kern::formula<u64>([](auto p) { return (p + 4); }, self.p))) {
      self.ok = false;
      return {0};
    }
    x = wire_U32{0};
    x = struct_of_bytes<wire_U32, "4: u32@0">(b, num_cast<i64, u64>(self.p));
    self.p = kern::formula<u64>([](auto p) { return (p + 4); }, self.p);
    return {x.v};
  }

  struct Reader_get_i32Result { i32 Result = 0; };
  /// (An i32 is the i32 of the u32 of its four bytes.)
  Reader_get_i32Result Reader_get_i32(cityjobs_Reader& self, List<u8> b = List<u8>{}) {
    n12 = Reader_get_u32(self, b);
    return {num_cast<i32, u32>(n12.Result)};
  }

  struct Reader_get_f64Result { f64 Result = 0.0; };
  Reader_get_f64Result Reader_get_f64(cityjobs_Reader& self, List<u8> b = List<u8>{}) {
    wire_F64 x = wire_F64{0.0};
    if (math_compare<"<", u64>(num_cast<u64, i64>(list_length<u8>(b)), kern::formula<u64>([](auto p) { return (p + 8); }, self.p))) {
      self.ok = false;
      return {0.0};
    }
    x = wire_F64{0.0};
    x = struct_of_bytes<wire_F64, "8: f64@0">(b, num_cast<i64, u64>(self.p));
    self.p = kern::formula<u64>([](auto p) { return (p + 8); }, self.p);
    return {x.v};
  }

  void Reader_array_u32(cityjobs_Reader& self, List<u32>& v, List<u8> b = List<u8>{}) {
    u32 n = 0;
    n13 = Reader_get_u32(self, b);
    n = n13.Result;
    if (logic_or(logic_not(self.ok), math_compare<"<", u64>(kern::formula<u64, u64>([](auto a, auto p) { return (a - p); }, num_cast<u64, i64>(list_length<u8>(b)), self.p), kern::formula<u32>([](auto n) { return (num_cast<u64, u32>(n) * 4); }, n)))) {
      self.ok = false;
      return;
    }
    list_resize(v, num_cast<i64, u32>(n), 0);
    if (num_cast<bool, u32>(n)) {
      list_copy_bytes<"u32", "u8">(v, 0, b, num_cast<i64, u64>(self.p), num_cast<i64, u64>(kern::formula<u32>([](auto n) { return (num_cast<u64, u32>(n) * 4); }, n)));
    }
    self.p = kern::formula<u64, u32>([](auto p, auto n) { return (p + (num_cast<u64, u32>(n) * 4)); }, self.p, n);
  }

  void Reader_array_f64(cityjobs_Reader& self, List<f64>& v, List<u8> b = List<u8>{}) {
    u32 n = 0;
    n14 = Reader_get_u32(self, b);
    n = n14.Result;
    if (logic_or(logic_not(self.ok), math_compare<"<", u64>(kern::formula<u64, u64>([](auto a, auto p) { return (a - p); }, num_cast<u64, i64>(list_length<u8>(b)), self.p), kern::formula<u32>([](auto n) { return (num_cast<u64, u32>(n) * 8); }, n)))) {
      self.ok = false;
      return;
    }
    list_resize(v, num_cast<i64, u32>(n), 0.0);
    if (num_cast<bool, u32>(n)) {
      list_copy_bytes<"f64", "u8">(v, 0, b, num_cast<i64, u64>(self.p), num_cast<i64, u64>(kern::formula<u32>([](auto n) { return (num_cast<u64, u32>(n) * 8); }, n)));
    }
    self.p = kern::formula<u64, u32>([](auto p, auto n) { return (p + (num_cast<u64, u32>(n) * 8)); }, self.p, n);
  }

  /// ---- kTile: MVT bytes -> features (mvt.cpp + tiles.cpp extract) ----
  void run_tile(List<u8>& out, List<u8> in = List<u8>{}) {
    cityjobs_Reader r = cityjobs_Reader{0, true};
    TileKey key = TileKey{14, 0, 0};
    f64 lon0 = 0.0;
    f64 lat0 = 0.0;
    GeoProjector proj = GeoProjector{0.0, 0.0, 1.0};
    bool init = false;
    mvt_Decoder d = mvt_Decoder{List<mvt_Piece>{}, List<mvt_Piece>{}, 0, 0, false, true, false, mvt_Tile{List<mvt_Layer>{}}};
    List<Text> a = List<Text>{};
    MapFeatures f = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
    r = cityjobs_Reader{0, true};
    key = TileKey{14, 0, 0};
    n15 = Reader_get_i32(r, in);
    key.z = n15.Result;
    n16 = Reader_get_i32(r, in);
    key.x = n16.Result;
    n17 = Reader_get_i32(r, in);
    key.y = n17.Result;
    n18 = Reader_get_f64(r, in);
    lon0 = n18.Result;
    n19 = Reader_get_f64(r, in);
    lat0 = n19.Result;
    if (logic_not(r.ok)) {
      return;
    }
    proj = GeoProjector{0.0, 0.0, 1.0};
    engine_geo_projector_init(n20, proj, lon0, lat0);
    init = n20.result;
    proj = n20.self;
    if (logic_not(init)) {
      return;
    }
    d = mvt_Decoder{List<mvt_Piece>{}, List<mvt_Piece>{}, 0, 0, false, true, false, mvt_Tile{List<mvt_Layer>{}}};
    a = wanted;
    mvt_cpp::Decoder_begin(d, a, in, r.p);
    n21 = mvt_cpp::Decoder_step(d, in, 0.0);
    if (mvt::Decoder_failed(d).Result) {
      wire::put_u8(out, 0);
      return;
    }
    f = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
    tiles_cpp::extract(d.tile_, key, proj, f);
    wire::put_u8(out, 1);
    wire::put_u8(out, kern::formula<bool, i32, i32>([](auto procedural, auto a1, auto a0) { return num_cast<u8, i32>((procedural ? a1 : a0)); }, f.procedural, 1, 0));
    wire::put_u32(out, num_cast<u32, i64>(list_length<MapFeatures_Building>(f.buildings)));
    for (auto it22 = flow_for(n22, 0, math_sub<i64>(list_length<MapFeatures_Building>(f.buildings), 1)); it22.next();) {
      wire::put_f32(out, f.buildings[n22.index].base);
      wire::put_f32(out, f.buildings[n22.index].top);
      wire::put_u32(out, f.buildings[n22.index].seed);
      wire::put_u32(out, num_cast<u32, i64>(list_length<Vector_row>(f.buildings[n22.index].rings)));
      for (auto it23 = flow_for(n23, 0, math_sub<i64>(list_length<Vector_row>(f.buildings[n22.index].rings), 1)); it23.next();) {
        put_points(out, f.buildings[n22.index].rings[n23.index].items);
      }
    }
    wire::put_u32(out, num_cast<u32, i64>(list_length<MapFeatures_Road>(f.roads)));
    for (auto it24 = flow_for(n24, 0, math_sub<i64>(list_length<MapFeatures_Road>(f.roads), 1)); it24.next();) {
      wire::put_u8(out, num_cast<u8, i64>(enum_index<RoadClass>(f.roads[n24.index].cls)));
      wire::put_u8(out, kern::formula<bool, i32, i32>([](auto a, auto a1, auto a0) { return num_cast<u8, i32>((a ? a1 : a0)); }, f.roads[n24.index].bridge, 1, 0));
      put_points(out, f.roads[n24.index].points);
    }
    wire::put_u32(out, num_cast<u32, i64>(list_length<MapFeatures_Area>(f.areas)));
    for (auto it25 = flow_for(n25, 0, math_sub<i64>(list_length<MapFeatures_Area>(f.areas), 1)); it25.next();) {
      wire::put_u8(out, num_cast<u8, i64>(enum_index<AreaKind>(f.areas[n25.index].kind)));
      wire::put_u32(out, num_cast<u32, i64>(list_length<Vector_row>(f.areas[n25.index].rings)));
      for (auto it26 = flow_for(n26, 0, math_sub<i64>(list_length<Vector_row>(f.areas[n25.index].rings), 1)); it26.next();) {
        put_points(out, f.areas[n25.index].rings[n26.index].items);
      }
    }
    put_spots(out, f.trees);
    put_spots(out, f.lamps);
  }

  struct tile_inputResult { List<u8> Result = List<u8>{}; };
  tile_inputResult tile_input(const TileKey& key, f64 lon0 = 0.0, f64 lat0 = 0.0, List<u8> mvt = List<u8>{}) {
    List<u8> b = List<u8>{};
    i64 place = 0;
    b = List<u8>{};
    list_reserve(b, num_cast<i64, u64>(kern::formula<u64>([](auto a) { return (a + 28); }, num_cast<u64, i64>(list_length<u8>(mvt)))));
    put_i32(b, key.z);
    put_i32(b, key.x);
    put_i32(b, key.y);
    wire::put_f64(b, lon0);
    wire::put_f64(b, lat0);
    place = list_length<u8>(b);
    list_insert_list(b, place, mvt);
    return {b};
  }

  void buildings_input_rgb(List<u8>& b, const Rgb& c) {
    wire::put_f64(b, c.r);
    wire::put_f64(b, c.g);
    wire::put_f64(b, c.b);
  }

  struct buildings_inputResult { List<u8> Result = List<u8>{}; };
  buildings_inputResult buildings_input(const BuildingOptions& o, Object<kern::FootprintStream> footprints = Object<kern::FootprintStream>{}) {
    List<u8> b = List<u8>{};
    List<f64> stream = List<f64>{};
    List<f64> values = List<f64>{};
    b = List<u8>{};
    wire::put_f64(b, o.chunk_size);
    wire::put_u32(b, o.max_buildings);
    wire::put_u8(b, kern::formula<bool, i32, i32>([](auto cornice, auto a1, auto a0) { return num_cast<u8, i32>((cornice ? a1 : a0)); }, o.cornice, 1, 0));
    wire::put_u8(b, kern::formula<bool, i32, i32>([](auto wall_records, auto a1, auto a0) { return num_cast<u8, i32>((wall_records ? a1 : a0)); }, o.wall_records, 1, 0));
    wire::put_u32(b, num_cast<u32, i64>(list_length<Rgb>(o.style.walls)));
    for (auto it27 = flow_for(n27, 0, math_sub<i64>(list_length<Rgb>(o.style.walls), 1)); it27.next();) {
      buildings_input_rgb(b, o.style.walls[n27.index]);
    }
    wire::put_u32(b, num_cast<u32, i64>(list_length<Rgb>(o.style.roofs)));
    for (auto it28 = flow_for(n28, 0, math_sub<i64>(list_length<Rgb>(o.style.roofs), 1)); it28.next();) {
      buildings_input_rgb(b, o.style.roofs[n28.index]);
    }
    buildings_input_rgb(b, o.style.trim);
    wire::put_f64(b, o.style.trim_mix);
    values = engine_footprint_stream_values(footprints);
    stream = values;
    put_array_f64(b, stream);
    return {b};
  }

  struct chunk_countResult { u32 Result = 0; };
  KERN_PURE chunk_countResult chunk_count(const List<u8>& output) {
    if (math_compare<"<", u64>(num_cast<u64, i64>(list_length<u8>(output)), 4)) {
      return {0};
    }
    return {kern::formula<u32, u32, u32, u32>([](auto a, auto a_2, auto a_3, auto a_4) { return (((a | (a_2 << 8)) | (a_3 << 16)) | (a_4 << 24)); }, num_cast<u32, u8>(output[0]), num_cast<u32, u8>(output[1]), num_cast<u32, u8>(output[2]), num_cast<u32, u8>(output[3]))};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
  Reader_get_u32Result n12;
  Reader_get_u32Result n13;
  Reader_get_u32Result n14;
  Reader_get_i32Result n15;
  Reader_get_i32Result n16;
  Reader_get_i32Result n17;
  Reader_get_f64Result n18;
  Reader_get_f64Result n19;
  struct { bool result = false; GeoProjector self = GeoProjector{0.0, 0.0, 1.0}; } n20;
  mvt_cpp::Decoder_stepResult n21;
  struct : kern::Node<22> { i64 index = 0; } n22;
  struct : kern::Node<23> { i64 index = 0; } n23;
  struct : kern::Node<24> { i64 index = 0; } n24;
  struct : kern::Node<25> { i64 index = 0; } n25;
  struct : kern::Node<26> { i64 index = 0; } n26;
  struct : kern::Node<27> { i64 index = 0; } n27;
  struct : kern::Node<28> { i64 index = 0; } n28;
};
