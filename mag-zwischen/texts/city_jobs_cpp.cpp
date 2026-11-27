struct city_jobs_cpp : virtual kern::Library, virtual mvt, virtual mvt_cpp, virtual tiles_cpp, virtual wire {
  // ---- types ----
  KERN_STRUCT(cityjobs_Reader) { u64 p = 0; bool ok = true; };

  // ---- variables ----
  KERN_CONST List<Text> wanted = List<Text>{"building", "transportation", "water", "waterway", "park", "landcover", "landuse"};
  KERN_CONST u32 kTile = 1;
  KERN_CONST u32 kBuildings = 2;

  // ---- functions ----
  /// ---- writing a job's bytes: every value as it lies in memory, one after the other (wire.h: little-endian, as on every target of the engine). A list of numbers is its count (u32), then its entries' bytes in one go.
  void put_array_f32(List<u8>& b, const List<f32>& v) {
    u64 n = 0;
    wire::put_u32(b, num_cast<u32, i64>(list_length<f32>(v)));
    n = num_cast<u64, i64>(list_length<u8>(b));
    list_resize(b, num_cast<i64, u64>(kern::formula<u64, u64>([](auto n, auto a) { return (n + (a * 4)); }, n, num_cast<u64, i64>(list_length<f32>(v)))), 0);
    if (logic_not(math_compare<"==", i64>(list_length<f32>(v), 0))) {
      list_copy_bytes<"u8", "f32">(b, num_cast<i64, u64>(n), v, 0, num_cast<i64, u64>(kern::formula<u64>([](auto a) { return (a * 4); }, num_cast<u64, i64>(list_length<f32>(v)))));
    }
  }

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

  struct Reader_get_f32Result { f32 Result = 0.0f; };
  Reader_get_f32Result Reader_get_f32(cityjobs_Reader& self, List<u8> b = List<u8>{}) {
    wire_F32 x = wire_F32{0.0f};
    if (math_compare<"<", u64>(num_cast<u64, i64>(list_length<u8>(b)), kern::formula<u64>([](auto p) { return (p + 4); }, self.p))) {
      self.ok = false;
      return {0.0f};
    }
    x = wire_F32{0.0f};
    x = struct_of_bytes<wire_F32, "4: f32@0">(b, num_cast<i64, u64>(self.p));
    self.p = kern::formula<u64>([](auto p) { return (p + 4); }, self.p);
    return {x.v};
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

  void Reader_array_f32(cityjobs_Reader& self, List<f32>& v, List<u8> b = List<u8>{}) {
    u32 n = 0;
    n13 = Reader_get_u32(self, b);
    n = n13.Result;
    if (logic_or(logic_not(self.ok), math_compare<"<", u64>(kern::formula<u64, u64>([](auto a, auto p) { return (a - p); }, num_cast<u64, i64>(list_length<u8>(b)), self.p), kern::formula<u32>([](auto n) { return (num_cast<u64, u32>(n) * 4); }, n)))) {
      self.ok = false;
      return;
    }
    list_resize(v, num_cast<i64, u32>(n), 0.0f);
    if (num_cast<bool, u32>(n)) {
      list_copy_bytes<"f32", "u8">(v, 0, b, num_cast<i64, u64>(self.p), num_cast<i64, u64>(kern::formula<u32>([](auto n) { return (num_cast<u64, u32>(n) * 4); }, n)));
    }
    self.p = kern::formula<u64, u32>([](auto p, auto n) { return (p + (num_cast<u64, u32>(n) * 4)); }, self.p, n);
  }

  void Reader_array_u32(cityjobs_Reader& self, List<u32>& v, List<u8> b = List<u8>{}) {
    u32 n = 0;
    n14 = Reader_get_u32(self, b);
    n = n14.Result;
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
    n15 = Reader_get_u32(self, b);
    n = n15.Result;
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

  void Reader_points(cityjobs_Reader& self, List<Vector>& v, List<u8> b = List<u8>{}) {
    u32 n = 0;
    n16 = Reader_get_u32(self, b);
    n = n16.Result;
    if (logic_or(logic_not(self.ok), math_compare<"<", u64>(kern::formula<u64, u64>([](auto a, auto p) { return (a - p); }, num_cast<u64, i64>(list_length<u8>(b)), self.p), kern::formula<u32>([](auto n) { return (num_cast<u64, u32>(n) * 8); }, n)))) {
      self.ok = false;
      return;
    }
    list_resize(v, num_cast<i64, u32>(n), Vector{0.0, 0.0, 0.0});
    for (auto it17 = flow_for(n17, 0, math_sub<i64>(num_cast<i64, u32>(n), 1)); it17.next();) {
      n18 = Reader_get_f32(self, b);
      v[num_cast<i64, u32>(num_cast<u32, i64>(n17.index))] = vector_make(n18.Result, vector_break(v[num_cast<i64, u32>(num_cast<u32, i64>(n17.index))]).y, vector_break(v[num_cast<i64, u32>(num_cast<u32, i64>(n17.index))]).z);
      n19 = Reader_get_f32(self, b);
      v[num_cast<i64, u32>(num_cast<u32, i64>(n17.index))] = vector_make(vector_break(v[num_cast<i64, u32>(num_cast<u32, i64>(n17.index))]).x, n19.Result, vector_break(v[num_cast<i64, u32>(num_cast<u32, i64>(n17.index))]).z);
    }
  }

  struct Reader_spotsResult { bool Result = false; };
  /// False: a broken count (the reader may still be ok).
  Reader_spotsResult Reader_spots(cityjobs_Reader& self, List<MapFeatures_Spot>& v, List<u8> b = List<u8>{}) {
    u32 n = 0;
    n20 = Reader_get_u32(self, b);
    n = n20.Result;
    if (logic_or(logic_not(self.ok), math_compare<">", u32>(n, 200000))) {
      return {false};
    }
    list_resize(v, num_cast<i64, u32>(n), MapFeatures_Spot{Vector{0.0, 0.0, 0.0}, 0.0f});
    for (auto it21 = flow_for(n21, 0, math_sub<i64>(list_length<MapFeatures_Spot>(v), 1)); it21.next();) {
      n22 = Reader_get_f32(self, b);
      v[n21.index].p = vector_make(n22.Result, vector_break(v[n21.index].p).y, vector_break(v[n21.index].p).z);
      n23 = Reader_get_f32(self, b);
      v[n21.index].p = vector_make(vector_break(v[n21.index].p).x, n23.Result, vector_break(v[n21.index].p).z);
      n24 = Reader_get_f32(self, b);
      v[n21.index].a = n24.Result;
    }
    return {true};
  }

  /// One chunk of the kBuildings output, as the job wrote it.
  void read_one_chunk(cityjobs_Reader& r, GeoChunk& c, List<u8> in = List<u8>{}) {
    n25 = Reader_get_i32(r, in);
    c.x = n25.Result;
    n26 = Reader_get_i32(r, in);
    c.y = n26.Result;
    n27 = Reader_get_u32(r, in);
    c.items = n27.Result;
    n28 = Reader_get_u32(r, in);
    c.triangles = n28.Result;
    n29 = Reader_get_u32(r, in);
    c.mesh.extra_size = n29.Result;
    Reader_array_f32(r, c.mesh.positions, in);
    Reader_array_f32(r, c.mesh.normals, in);
    Reader_array_f32(r, c.mesh.colors, in);
    Reader_array_f32(r, c.mesh.uvs, in);
    Reader_array_f32(r, c.mesh.extra, in);
    Reader_array_u32(r, c.mesh.indices, in);
    c.welded = true;
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
    n30 = Reader_get_i32(r, in);
    key.z = n30.Result;
    n31 = Reader_get_i32(r, in);
    key.x = n31.Result;
    n32 = Reader_get_i32(r, in);
    key.y = n32.Result;
    n33 = Reader_get_f64(r, in);
    lon0 = n33.Result;
    n34 = Reader_get_f64(r, in);
    lat0 = n34.Result;
    if (logic_not(r.ok)) {
      return;
    }
    proj = GeoProjector{0.0, 0.0, 1.0};
    engine_geo_projector_init(n35, proj, lon0, lat0);
    init = n35.result;
    proj = n35.self;
    if (logic_not(init)) {
      return;
    }
    d = mvt_Decoder{List<mvt_Piece>{}, List<mvt_Piece>{}, 0, 0, false, true, false, mvt_Tile{List<mvt_Layer>{}}};
    a = wanted;
    mvt_cpp::Decoder_begin(d, a, in, r.p);
    n36 = mvt_cpp::Decoder_step(d, in, 0.0);
    if (mvt::Decoder_failed(d).Result) {
      wire::put_u8(out, 0);
      return;
    }
    f = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
    tiles_cpp::extract(d.tile_, key, proj, f);
    wire::put_u8(out, 1);
    wire::put_u8(out, kern::formula<bool, i32, i32>([](auto procedural, auto a1, auto a0) { return num_cast<u8, i32>((procedural ? a1 : a0)); }, f.procedural, 1, 0));
    wire::put_u32(out, num_cast<u32, i64>(list_length<MapFeatures_Building>(f.buildings)));
    for (auto it37 = flow_for(n37, 0, math_sub<i64>(list_length<MapFeatures_Building>(f.buildings), 1)); it37.next();) {
      wire::put_f32(out, f.buildings[n37.index].base);
      wire::put_f32(out, f.buildings[n37.index].top);
      wire::put_u32(out, f.buildings[n37.index].seed);
      wire::put_u32(out, num_cast<u32, i64>(list_length<Vector_row>(f.buildings[n37.index].rings)));
      for (auto it38 = flow_for(n38, 0, math_sub<i64>(list_length<Vector_row>(f.buildings[n37.index].rings), 1)); it38.next();) {
        put_points(out, f.buildings[n37.index].rings[n38.index].items);
      }
    }
    wire::put_u32(out, num_cast<u32, i64>(list_length<MapFeatures_Road>(f.roads)));
    for (auto it39 = flow_for(n39, 0, math_sub<i64>(list_length<MapFeatures_Road>(f.roads), 1)); it39.next();) {
      wire::put_u8(out, num_cast<u8, i64>(enum_index<RoadClass>(f.roads[n39.index].cls)));
      wire::put_u8(out, kern::formula<bool, i32, i32>([](auto a, auto a1, auto a0) { return num_cast<u8, i32>((a ? a1 : a0)); }, f.roads[n39.index].bridge, 1, 0));
      put_points(out, f.roads[n39.index].points);
    }
    wire::put_u32(out, num_cast<u32, i64>(list_length<MapFeatures_Area>(f.areas)));
    for (auto it40 = flow_for(n40, 0, math_sub<i64>(list_length<MapFeatures_Area>(f.areas), 1)); it40.next();) {
      wire::put_u8(out, num_cast<u8, i64>(enum_index<AreaKind>(f.areas[n40.index].kind)));
      wire::put_u32(out, num_cast<u32, i64>(list_length<Vector_row>(f.areas[n40.index].rings)));
      for (auto it41 = flow_for(n41, 0, math_sub<i64>(list_length<Vector_row>(f.areas[n40.index].rings), 1)); it41.next();) {
        put_points(out, f.areas[n40.index].rings[n41.index].items);
      }
    }
    put_spots(out, f.trees);
    put_spots(out, f.lamps);
  }

  struct run_buildings_rgbResult { Rgb Result = Rgb{1.0, 1.0, 1.0}; };
  run_buildings_rgbResult run_buildings_rgb(cityjobs_Reader& r, const List<u8>& in) {
    Rgb c = Rgb{1.0, 1.0, 1.0};
    c = Rgb{1.0, 1.0, 1.0};
    n46 = Reader_get_f64(r, in);
    c.r = n46.Result;
    n47 = Reader_get_f64(r, in);
    c.g = n47.Result;
    n48 = Reader_get_f64(r, in);
    c.b = n48.Result;
    return {c};
  }

  /// ---- kBuildings: footprints -> welded chunk meshes (kern::BuildingBuilder + kern::weld) ----
  void run_buildings(List<u8>& out, List<u8> in = List<u8>{}) {
    cityjobs_Reader r = cityjobs_Reader{0, true};
    BuildingOptions o = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
    List<f64> stream = List<f64>{};
    Object<kern::BuildingBuilder> b = Object<kern::BuildingBuilder>{};
    Object<kern::BuildingBuilder> building_builder = Object<kern::BuildingBuilder>{};
    bool begin = false;
    bool step = false;
    bool go = false;
    WeldStats weld = WeldStats{0, 0, 0, 0, false};
    List<f32> a = List<f32>{};
    List<f32> a_2 = List<f32>{};
    List<f32> a_3 = List<f32>{};
    List<f32> a_4 = List<f32>{};
    List<f32> a_5 = List<f32>{};
    List<u32> a_6 = List<u32>{};
    r = cityjobs_Reader{0, true};
    o = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
    n42 = Reader_get_f64(r, in);
    o.chunk_size = n42.Result;
    n43 = Reader_get_u32(r, in);
    o.max_buildings = n43.Result;
    n44 = Reader_get_u8(r, in);
    o.cornice = math_compare<"!=", i32>(num_cast<i32, u8>(n44.Result), 0);
    n45 = Reader_get_u8(r, in);
    o.wall_records = math_compare<"!=", i32>(num_cast<i32, u8>(n45.Result), 0);
    n49 = Reader_get_u32(r, in);
    list_resize(o.style.walls, num_cast<i64, u32>(n49.Result), Rgb{1.0, 1.0, 1.0});
    for (auto it50 = flow_for(n50, 0, math_sub<i64>(list_length<Rgb>(o.style.walls), 1)); it50.next();) {
      n51 = run_buildings_rgb(r, in);
      o.style.walls[n50.index] = n51.Result;
    }
    n52 = Reader_get_u32(r, in);
    list_resize(o.style.roofs, num_cast<i64, u32>(n52.Result), Rgb{1.0, 1.0, 1.0});
    for (auto it53 = flow_for(n53, 0, math_sub<i64>(list_length<Rgb>(o.style.roofs), 1)); it53.next();) {
      n54 = run_buildings_rgb(r, in);
      o.style.roofs[n53.index] = n54.Result;
    }
    n55 = run_buildings_rgb(r, in);
    o.style.trim = n55.Result;
    n56 = Reader_get_f64(r, in);
    o.style.trim_mix = n56.Result;
    stream = List<f64>{};
    Reader_array_f64(r, stream, in);
    if (logic_or(logic_or(logic_not(r.ok), math_compare<"==", i64>(list_length<Rgb>(o.style.walls), 0)), math_compare<"==", i64>(list_length<Rgb>(o.style.roofs), 0))) {
      return;
    }
    engine_building_builder_make(n57);
    building_builder = n57.result;
    b = building_builder;
    engine_building_builder_begin(n58, b, stream, num_cast<u32, i64>(list_length<f64>(stream)), o);
    begin = n58.result;
    if (logic_not(begin)) {
      if (object_valid<kern::BuildingBuilder>(b)) {
        engine_building_builder_release(b);
      }
      b = Object<kern::BuildingBuilder>{};
      return;
    }
    go = true;
    while (loop(go)) {
      engine_building_builder_step(n59, b, 1000000000.0);
      step = n59.result;
      if (logic_not(step)) {
      } else {
        go = false;
      }
    }
    engine_building_builder_chunks(n60, b);
    wire::put_u32(out, num_cast<u32, i64>(list_length<GeoChunk>(n60.result)));
    for (auto it61 = flow_for(n61, 0, math_sub<i64>(list_length<GeoChunk>(n60.result), 1)); it61.next();) {
      engine_weld(n62, n60.result[n61.index].mesh, 0);
      weld = n62.result;
      n60.result[n61.index].mesh = n62.mesh;
      put_i32(out, n60.result[n61.index].x);
      put_i32(out, n60.result[n61.index].y);
      wire::put_u32(out, n60.result[n61.index].items);
      wire::put_u32(out, n60.result[n61.index].triangles);
      wire::put_u32(out, n60.result[n61.index].mesh.extra_size);
      a = n60.result[n61.index].mesh.positions;
      put_array_f32(out, a);
      a_2 = n60.result[n61.index].mesh.normals;
      put_array_f32(out, a_2);
      a_3 = n60.result[n61.index].mesh.colors;
      put_array_f32(out, a_3);
      a_4 = n60.result[n61.index].mesh.uvs;
      put_array_f32(out, a_4);
      a_5 = n60.result[n61.index].mesh.extra;
      put_array_f32(out, a_5);
      a_6 = n60.result[n61.index].mesh.indices;
      put_array_u32(out, a_6);
    }
    if (object_valid<kern::BuildingBuilder>(b)) {
      engine_building_builder_release(b);
    }
    b = Object<kern::BuildingBuilder>{};
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

  struct read_featuresResult { bool Result = false; };
  read_featuresResult read_features(const List<u8>& output, MapFeatures& f) {
    List<u8> in = List<u8>{};
    cityjobs_Reader r = cityjobs_Reader{0, true};
    bool either = false;
    in = output;
    r = cityjobs_Reader{0, true};
    n63 = Reader_get_u8(r, in);
    if (math_compare<"!=", i32>(num_cast<i32, u8>(n63.Result), 1)) {
      return {false};
    }
    n64 = Reader_get_u8(r, in);
    f.procedural = math_compare<"!=", i32>(num_cast<i32, u8>(n64.Result), 0);
    n65 = Reader_get_u32(r, in);
    list_resize(f.buildings, num_cast<i64, u32>(n65.Result), MapFeatures_Building{List<Vector_row>{}, 0.0f, 8.0f, 0, 0.0f, false});
    for (auto it66 = flow_for(n66, 0, math_sub<i64>(list_length<MapFeatures_Building>(f.buildings), 1)); it66.next();) {
      n67 = Reader_get_f32(r, in);
      f.buildings[n66.index].base = n67.Result;
      n68 = Reader_get_f32(r, in);
      f.buildings[n66.index].top = n68.Result;
      n69 = Reader_get_u32(r, in);
      f.buildings[n66.index].seed = n69.Result;
      n70 = Reader_get_u32(r, in);
      list_resize(f.buildings[n66.index].rings, num_cast<i64, u32>(n70.Result), Vector_row{List<Vector>{}});
      for (auto it71 = flow_for(n71, 0, math_sub<i64>(list_length<Vector_row>(f.buildings[n66.index].rings), 1)); it71.next();) {
        Reader_points(r, f.buildings[n66.index].rings[n71.index].items, in);
      }
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n72 = Reader_get_u32(r, in);
    list_resize(f.roads, num_cast<i64, u32>(n72.Result), MapFeatures_Road{List<Vector>{}, RoadClass::minor, false});
    for (auto it73 = flow_for(n73, 0, math_sub<i64>(list_length<MapFeatures_Road>(f.roads), 1)); it73.next();) {
      n74 = Reader_get_u8(r, in);
      f.roads[n73.index].cls = enum_from_index<RoadClass>(num_cast<i64, u8>(n74.Result)).value;
      n75 = Reader_get_u8(r, in);
      f.roads[n73.index].bridge = math_compare<"!=", i32>(num_cast<i32, u8>(n75.Result), 0);
      Reader_points(r, f.roads[n73.index].points, in);
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n76 = Reader_get_u32(r, in);
    list_resize(f.areas, num_cast<i64, u32>(n76.Result), MapFeatures_Area{List<Vector_row>{}, AreaKind::park});
    for (auto it77 = flow_for(n77, 0, math_sub<i64>(list_length<MapFeatures_Area>(f.areas), 1)); it77.next();) {
      n78 = Reader_get_u8(r, in);
      f.areas[n77.index].kind = enum_from_index<AreaKind>(num_cast<i64, u8>(n78.Result)).value;
      n79 = Reader_get_u32(r, in);
      list_resize(f.areas[n77.index].rings, num_cast<i64, u32>(n79.Result), Vector_row{List<Vector>{}});
      for (auto it80 = flow_for(n80, 0, math_sub<i64>(list_length<Vector_row>(f.areas[n77.index].rings), 1)); it80.next();) {
        Reader_points(r, f.areas[n77.index].rings[n80.index].items, in);
      }
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n81 = Reader_spots(r, f.trees, in);
    either = logic_not(n81.Result);
    if (logic_not(either)) {
      n83 = Reader_spots(r, f.lamps, in);
      either = logic_not(n83.Result);
    }
    if (either) {
      return {false};
    }
    return {r.ok};
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
    for (auto it84 = flow_for(n84, 0, math_sub<i64>(list_length<Rgb>(o.style.walls), 1)); it84.next();) {
      buildings_input_rgb(b, o.style.walls[n84.index]);
    }
    wire::put_u32(b, num_cast<u32, i64>(list_length<Rgb>(o.style.roofs)));
    for (auto it85 = flow_for(n85, 0, math_sub<i64>(list_length<Rgb>(o.style.roofs), 1)); it85.next();) {
      buildings_input_rgb(b, o.style.roofs[n85.index]);
    }
    buildings_input_rgb(b, o.style.trim);
    wire::put_f64(b, o.style.trim_mix);
    values = engine_footprint_stream_values(footprints);
    stream = values;
    put_array_f64(b, stream);
    return {b};
  }

  struct read_chunksResult { bool Result = false; };
  read_chunksResult read_chunks(const List<u8>& output, List<GeoChunk>& out) {
    List<u8> in = List<u8>{};
    cityjobs_Reader r = cityjobs_Reader{0, true};
    in = output;
    r = cityjobs_Reader{0, true};
    n86 = Reader_get_u32(r, in);
    list_resize(out, num_cast<i64, u32>(n86.Result), GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}});
    for (auto it87 = flow_for(n87, 0, math_sub<i64>(list_length<GeoChunk>(out), 1)); it87.next();) {
      read_one_chunk(r, out[n87.index], in);
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    return {r.ok};
  }

  struct chunk_countResult { u32 Result = 0; };
  KERN_PURE chunk_countResult chunk_count(const List<u8>& output) {
    if (math_compare<"<", u64>(num_cast<u64, i64>(list_length<u8>(output)), 4)) {
      return {0};
    }
    return {kern::formula<u32, u32, u32, u32>([](auto a, auto a_2, auto a_3, auto a_4) { return (((a | (a_2 << 8)) | (a_3 << 16)) | (a_4 << 24)); }, num_cast<u32, u8>(output[0]), num_cast<u32, u8>(output[1]), num_cast<u32, u8>(output[2]), num_cast<u32, u8>(output[3]))};
  }

  struct read_chunkResult { bool Result = false; };
  read_chunkResult read_chunk(const List<u8>& output, u64& at, GeoChunk& c) {
    cityjobs_Reader r = cityjobs_Reader{0, true};
    if (math_compare<">=", u64>(at, num_cast<u64, i64>(list_length<u8>(output)))) {
      return {false};
    }
    r = struct_make<cityjobs_Reader>(at, true);
    read_one_chunk(r, c, output);
    if (logic_not(r.ok)) {
      return {false};
    }
    at = r.p;
    return {true};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
  Reader_get_u32Result n12;
  Reader_get_u32Result n13;
  Reader_get_u32Result n14;
  Reader_get_u32Result n15;
  Reader_get_u32Result n16;
  struct : kern::Node<17> { i64 index = 0; } n17;
  Reader_get_f32Result n18;
  Reader_get_f32Result n19;
  Reader_get_u32Result n20;
  struct : kern::Node<21> { i64 index = 0; } n21;
  Reader_get_f32Result n22;
  Reader_get_f32Result n23;
  Reader_get_f32Result n24;
  Reader_get_i32Result n25;
  Reader_get_i32Result n26;
  Reader_get_u32Result n27;
  Reader_get_u32Result n28;
  Reader_get_u32Result n29;
  Reader_get_i32Result n30;
  Reader_get_i32Result n31;
  Reader_get_i32Result n32;
  Reader_get_f64Result n33;
  Reader_get_f64Result n34;
  struct { bool result = false; GeoProjector self = GeoProjector{0.0, 0.0, 1.0}; } n35;
  mvt_cpp::Decoder_stepResult n36;
  struct : kern::Node<37> { i64 index = 0; } n37;
  struct : kern::Node<38> { i64 index = 0; } n38;
  struct : kern::Node<39> { i64 index = 0; } n39;
  struct : kern::Node<40> { i64 index = 0; } n40;
  struct : kern::Node<41> { i64 index = 0; } n41;
  Reader_get_f64Result n42;
  Reader_get_u32Result n43;
  Reader_get_u8Result n44;
  Reader_get_u8Result n45;
  Reader_get_f64Result n46;
  Reader_get_f64Result n47;
  Reader_get_f64Result n48;
  Reader_get_u32Result n49;
  struct : kern::Node<50> { i64 index = 0; } n50;
  run_buildings_rgbResult n51;
  Reader_get_u32Result n52;
  struct : kern::Node<53> { i64 index = 0; } n53;
  run_buildings_rgbResult n54;
  run_buildings_rgbResult n55;
  Reader_get_f64Result n56;
  struct { Object<kern::BuildingBuilder> result = Object<kern::BuildingBuilder>{}; } n57;
  struct { bool result = false; } n58;
  struct { bool result = false; } n59;
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n60;
  struct : kern::Node<61> { i64 index = 0; } n61;
  struct { WeldStats result = WeldStats{0, 0, 0, 0, false}; MeshData mesh = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n62;
  Reader_get_u8Result n63;
  Reader_get_u8Result n64;
  Reader_get_u32Result n65;
  struct : kern::Node<66> { i64 index = 0; } n66;
  Reader_get_f32Result n67;
  Reader_get_f32Result n68;
  Reader_get_u32Result n69;
  Reader_get_u32Result n70;
  struct : kern::Node<71> { i64 index = 0; } n71;
  Reader_get_u32Result n72;
  struct : kern::Node<73> { i64 index = 0; } n73;
  Reader_get_u8Result n74;
  Reader_get_u8Result n75;
  Reader_get_u32Result n76;
  struct : kern::Node<77> { i64 index = 0; } n77;
  Reader_get_u8Result n78;
  Reader_get_u32Result n79;
  struct : kern::Node<80> { i64 index = 0; } n80;
  Reader_spotsResult n81;
  Reader_spotsResult n83;
  struct : kern::Node<84> { i64 index = 0; } n84;
  struct : kern::Node<85> { i64 index = 0; } n85;
  Reader_get_u32Result n86;
  struct : kern::Node<87> { i64 index = 0; } n87;
};
