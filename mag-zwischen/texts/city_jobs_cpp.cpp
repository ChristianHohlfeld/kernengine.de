struct city_jobs_cpp : virtual kern::Library, virtual engine_geo_roads, virtual mvt, virtual mvt_cpp, virtual tiles_cpp, virtual wire {
  // ---- types ----
  KERN_STRUCT(cityjobs_Reader) { Int p = 0; Bool ok = true; };

  // ---- variables ----
  KERN_CONST List<Text> wanted = List<Text>{"building", "transportation", "water", "waterway", "park", "landcover", "landuse"};

  // ---- functions ----
  /// ---- writing a job's bytes: every value as it lies in memory, one after the other (wire.h: little-endian, as on every target of the engine). A list of numbers is its count (u32), then its entries' bytes in one go.
  void put_array_f32(List<U8>& b, const List<Float>& v) {
    Int n = 0;
    wire::put_u32(b, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(v)))));
    n = list_length<U8>(b);
    list_resize(b, kern::to<Int>(kern::formula([](Float n, Float a) -> Float { return (n + (a * 4.0)); }, kern::to<Float>(n), kern::to<Float>(list_length<Float>(v)))), 0);
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<Float>(v)), 0.0))) {
      list_copy_bytes<"u8", "f32">(b, n, v, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return (a * 4.0); }, kern::to<Float>(list_length<Float>(v)))));
    }
  }

  void put_array_u32(List<U8>& b, const List<U32>& v) {
    Int n = 0;
    wire::put_u32(b, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<U32>(v)))));
    n = list_length<U8>(b);
    list_resize(b, kern::to<Int>(kern::formula([](Float n, Float a) -> Float { return (n + (a * 4.0)); }, kern::to<Float>(n), kern::to<Float>(list_length<U32>(v)))), 0);
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<U32>(v)), 0.0))) {
      list_copy_bytes<"u8", "u32">(b, n, v, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return (a * 4.0); }, kern::to<Float>(list_length<U32>(v)))));
    }
  }

  void put_array_f64(List<U8>& b, const List<Float>& v) {
    Int n = 0;
    wire::put_u32(b, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(v)))));
    n = list_length<U8>(b);
    list_resize(b, kern::to<Int>(kern::formula([](Float n, Float a) -> Float { return (n + (a * 8.0)); }, kern::to<Float>(n), kern::to<Float>(list_length<Float>(v)))), 0);
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<Float>(v)), 0.0))) {
      list_copy_bytes<"u8", "f64">(b, n, v, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return (a * 8.0); }, kern::to<Float>(list_length<Float>(v)))));
    }
  }

  void put_points(List<U8>& b, const List<Vector>& v) {
    Vector p = Vector{0.0, 0.0, 0.0};
    wire::put_u32(b, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector>(v)))));
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(v))))); it10.next();) {
      p = v[n10.index];
      wire::put_f32(b, vector_break(p).x);
      wire::put_f32(b, vector_break(p).y);
    }
  }

  void put_spots(List<U8>& b, const List<MapFeatures_Spot>& v) {
    wire::put_u32(b, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<MapFeatures_Spot>(v)))));
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Spot>(v))))); it11.next();) {
      wire::put_f32(b, vector_break(v[n11.index].p).x);
      wire::put_f32(b, vector_break(v[n11.index].p).y);
      wire::put_f32(b, v[n11.index].a);
    }
  }

  /// (An i32 goes as the u32 of its bits: the same four bytes.)
  void put_i32(List<U8>& b, I32 v = 0) {
    wire::put_u32(b, kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(v); }, kern::to<Float>(v))));
  }

  struct Reader_get_u8Result { U8 Result = 0; };
  Reader_get_u8Result Reader_get_u8(cityjobs_Reader& self, const List<U8>& b) {
    U8 v = 0;
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<U8>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 1.0); }, kern::to<Float>(self.p)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    v = b[self.p];
    self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 1.0); }, kern::to<Float>(self.p)));
    return {v};
  }

  struct Reader_get_u32Result { U32 Result = 0; };
  Reader_get_u32Result Reader_get_u32(cityjobs_Reader& self, const List<U8>& b) {
    wire_U32 x = wire_U32{0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<U8>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 4.0); }, kern::to<Float>(self.p)))))), 0.5)) {
      self.ok = false;
      return {0};
    }
    x = wire_U32{0};
    x = struct_of_bytes<wire_U32, "4: u32@0">(b, self.p);
    self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 4.0); }, kern::to<Float>(self.p)));
    return {x.v};
  }

  struct Reader_get_i32Result { I32 Result = 0; };
  /// (An i32 is the i32 of the u32 of its four bytes.)
  Reader_get_i32Result Reader_get_i32(cityjobs_Reader& self, const List<U8>& b) {
    n12 = Reader_get_u32(self, b);
    return {kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::wrap32s(the_result); }, kern::to<Float>(n12.Result)))};
  }

  struct Reader_get_f32Result { Float Result = 0.0; };
  Reader_get_f32Result Reader_get_f32(cityjobs_Reader& self, const List<U8>& b) {
    wire_F32 x = wire_F32{0.0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<U8>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 4.0); }, kern::to<Float>(self.p)))))), 0.5)) {
      self.ok = false;
      return {0.0};
    }
    x = wire_F32{0.0};
    x = struct_of_bytes<wire_F32, "4: f32@0">(b, self.p);
    self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 4.0); }, kern::to<Float>(self.p)));
    return {x.v};
  }

  struct Reader_get_f64Result { Float Result = 0.0; };
  Reader_get_f64Result Reader_get_f64(cityjobs_Reader& self, const List<U8>& b) {
    wire_F64 x = wire_F64{0.0};
    if (math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(list_length<U8>(b)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 8.0); }, kern::to<Float>(self.p)))))), 0.5)) {
      self.ok = false;
      return {0.0};
    }
    x = wire_F64{0.0};
    x = struct_of_bytes<wire_F64, "8: f64@0">(b, self.p);
    self.p = kern::to<Int>(kern::formula([](Float p) -> Float { return (p + 8.0); }, kern::to<Float>(self.p)));
    return {x.v};
  }

  void Reader_array_f32(cityjobs_Reader& self, const List<U8>& b, List<Float>& v) {
    U32 n = 0;
    n13 = Reader_get_u32(self, b);
    n = n13.Result;
    if (logic_or(logic_not(self.ok), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float p) -> Float { return (a - p); }, kern::to<Float>(list_length<U8>(b)), kern::to<Float>(self.p))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n * 4.0); }, kern::to<Float>(n)))))), 0.5))) {
      self.ok = false;
      return;
    }
    list_resize(v, n, 0.0);
    if (math_compare<"!=">(kern::to<Float>(n), 0.0)) {
      list_copy_bytes<"f32", "u8">(v, 0, b, self.p, kern::to<Int>(kern::formula([](Float n) -> Float { return (n * 4.0); }, kern::to<Float>(n))));
    }
    self.p = kern::to<Int>(kern::formula([](Float p, Float n) -> Float { return (p + (n * 4.0)); }, kern::to<Float>(self.p), kern::to<Float>(n)));
  }

  void Reader_array_u32(cityjobs_Reader& self, const List<U8>& b, List<U32>& v) {
    U32 n = 0;
    n14 = Reader_get_u32(self, b);
    n = n14.Result;
    if (logic_or(logic_not(self.ok), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float p) -> Float { return (a - p); }, kern::to<Float>(list_length<U8>(b)), kern::to<Float>(self.p))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n * 4.0); }, kern::to<Float>(n)))))), 0.5))) {
      self.ok = false;
      return;
    }
    list_resize(v, n, 0);
    if (math_compare<"!=">(kern::to<Float>(n), 0.0)) {
      list_copy_bytes<"u32", "u8">(v, 0, b, self.p, kern::to<Int>(kern::formula([](Float n) -> Float { return (n * 4.0); }, kern::to<Float>(n))));
    }
    self.p = kern::to<Int>(kern::formula([](Float p, Float n) -> Float { return (p + (n * 4.0)); }, kern::to<Float>(self.p), kern::to<Float>(n)));
  }

  void Reader_array_f64(cityjobs_Reader& self, const List<U8>& b, List<Float>& v) {
    U32 n = 0;
    n15 = Reader_get_u32(self, b);
    n = n15.Result;
    if (logic_or(logic_not(self.ok), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float p) -> Float { return (a - p); }, kern::to<Float>(list_length<U8>(b)), kern::to<Float>(self.p))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n * 8.0); }, kern::to<Float>(n)))))), 0.5))) {
      self.ok = false;
      return;
    }
    list_resize(v, n, 0.0);
    if (math_compare<"!=">(kern::to<Float>(n), 0.0)) {
      list_copy_bytes<"f64", "u8">(v, 0, b, self.p, kern::to<Int>(kern::formula([](Float n) -> Float { return (n * 8.0); }, kern::to<Float>(n))));
    }
    self.p = kern::to<Int>(kern::formula([](Float p, Float n) -> Float { return (p + (n * 8.0)); }, kern::to<Float>(self.p), kern::to<Float>(n)));
  }

  void Reader_points(cityjobs_Reader& self, const List<U8>& b, List<Vector>& v) {
    U32 n = 0;
    n16 = Reader_get_u32(self, b);
    n = n16.Result;
    if (logic_or(logic_not(self.ok), math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a, Float p) -> Float { return (a - p); }, kern::to<Float>(list_length<U8>(b)), kern::to<Float>(self.p))))), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n * 8.0); }, kern::to<Float>(n)))))), 0.5))) {
      self.ok = false;
      return;
    }
    list_resize(v, n, Vector{0.0, 0.0, 0.0});
    for (auto it17 = flow_for(n17, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it17.next();) {
      n18 = Reader_get_f32(self, b);
      v[n17.index] = vector_make(n18.Result, vector_break(v[n17.index]).y, vector_break(v[n17.index]).z);
      n19 = Reader_get_f32(self, b);
      v[n17.index] = vector_make(vector_break(v[n17.index]).x, n19.Result, vector_break(v[n17.index]).z);
    }
  }

  struct Reader_spotsResult { Bool Result = false; };
  /// False: a broken count (the reader may still be ok).
  Reader_spotsResult Reader_spots(cityjobs_Reader& self, const List<U8>& b, List<MapFeatures_Spot>& v) {
    U32 n = 0;
    n20 = Reader_get_u32(self, b);
    n = n20.Result;
    if (logic_or(logic_not(self.ok), math_compare<">">(kern::to<Float>(n), 200000.0))) {
      return {false};
    }
    list_resize(v, n, MapFeatures_Spot{Vector{0.0, 0.0, 0.0}, 0.0});
    for (auto it21 = flow_for(n21, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Spot>(v))))); it21.next();) {
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
  void read_one_chunk(const List<U8>& in, cityjobs_Reader& r, GeoChunk& c) {
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
    Reader_array_f32(r, in, c.mesh.positions);
    Reader_array_f32(r, in, c.mesh.normals);
    Reader_array_f32(r, in, c.mesh.colors);
    Reader_array_f32(r, in, c.mesh.uvs);
    Reader_array_f32(r, in, c.mesh.extra);
    Reader_array_u32(r, in, c.mesh.indices);
    c.welded = true;
  }

  /// ---- kTile: MVT bytes -> features (mvt.cpp + tiles.cpp extract) ----
  void run_tile(const List<U8>& in, List<U8>& out) {
    cityjobs_Reader r = cityjobs_Reader{0, true};
    TileKey key = TileKey{14, 0, 0};
    Float lon0 = 0.0;
    Float lat0 = 0.0;
    GeoProjector proj = GeoProjector{0.0, 0.0, 1.0};
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
    n35 = engine_geo_roads::GeoProjector_init(proj, lon0, lat0);
    if (logic_not(n35.Result)) {
      return;
    }
    d = mvt_Decoder{List<mvt_Piece>{}, List<mvt_Piece>{}, 0, 0, false, true, false, mvt_Tile{List<mvt_Layer>{}}};
    a = wanted;
    mvt_cpp::Decoder_begin(d, in, a, r.p);
    n36 = mvt_cpp::Decoder_step(d, in, 0.0);
    if (mvt::Decoder_failed(d).Result) {
      wire::put_u8(out, 0);
      return;
    }
    f = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false};
    tiles_cpp::extract(d.tile_, key, proj, f);
    wire::put_u8(out, 1);
    wire::put_u8(out, kern::to<Int>(kern::formula([](Float procedural) -> Float { return kern::math::band(((procedural != 0.0) ? 1.0 : 0.0), 255.0); }, kern::to<Float>(f.procedural))));
    wire::put_u32(out, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<MapFeatures_Building>(f.buildings)))));
    for (auto it37 = flow_for(n37, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(f.buildings))))); it37.next();) {
      wire::put_f32(out, f.buildings[n37.index].base);
      wire::put_f32(out, f.buildings[n37.index].top);
      wire::put_u32(out, f.buildings[n37.index].seed);
      wire::put_u32(out, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector_row>(f.buildings[n37.index].rings)))));
      for (auto it38 = flow_for(n38, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(f.buildings[n37.index].rings))))); it38.next();) {
        put_points(out, f.buildings[n37.index].rings[n38.index].items);
      }
    }
    wire::put_u32(out, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<MapFeatures_Road>(f.roads)))));
    for (auto it39 = flow_for(n39, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Road>(f.roads))))); it39.next();) {
      wire::put_u8(out, enum_index<RoadClass>(f.roads[n39.index].cls));
      wire::put_u8(out, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::band(((a != 0.0) ? 1.0 : 0.0), 255.0); }, kern::to<Float>(f.roads[n39.index].bridge))));
      put_points(out, f.roads[n39.index].points);
    }
    wire::put_u32(out, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<MapFeatures_Area>(f.areas)))));
    for (auto it40 = flow_for(n40, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Area>(f.areas))))); it40.next();) {
      wire::put_u8(out, enum_index<AreaKind>(f.areas[n40.index].kind));
      wire::put_u32(out, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Vector_row>(f.areas[n40.index].rings)))));
      for (auto it41 = flow_for(n41, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(f.areas[n40.index].rings))))); it41.next();) {
        put_points(out, f.areas[n40.index].rings[n41.index].items);
      }
    }
    put_spots(out, f.trees);
    put_spots(out, f.lamps);
  }

  struct run_buildings_rgbResult { Rgb Result = Rgb{1.0, 1.0, 1.0}; };
  run_buildings_rgbResult run_buildings_rgb(cityjobs_Reader& r, const List<U8>& in) {
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

  struct tile_inputResult { List<U8> Result = List<U8>{}; };
  tile_inputResult tile_input(const TileKey& key, const List<U8>& mvt, Float lon0 = 0.0, Float lat0 = 0.0) {
    List<U8> b = List<U8>{};
    Int place = 0;
    b = List<U8>{};
    list_reserve(b, kern::to<Int>(kern::formula([](Float a) -> Float { return (a + 28.0); }, kern::to<Float>(list_length<U8>(mvt)))));
    put_i32(b, key.z);
    put_i32(b, key.x);
    put_i32(b, key.y);
    wire::put_f64(b, lon0);
    wire::put_f64(b, lat0);
    place = list_length<U8>(b);
    list_insert_list(b, place, mvt);
    return {b};
  }

  struct read_featuresResult { Bool Result = false; };
  read_featuresResult read_features(const List<U8>& output, MapFeatures& f) {
    List<U8> in = List<U8>{};
    cityjobs_Reader r = cityjobs_Reader{0, true};
    Bool either = false;
    in = output;
    r = cityjobs_Reader{0, true};
    n60 = Reader_get_u8(r, in);
    if (math_compare<"!=">(kern::to<Float>(n60.Result), 1.0)) {
      return {false};
    }
    n61 = Reader_get_u8(r, in);
    f.procedural = math_compare<"!=">(kern::to<Float>(n61.Result), 0.0);
    n62 = Reader_get_u32(r, in);
    list_resize(f.buildings, n62.Result, MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false});
    for (auto it63 = flow_for(n63, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(f.buildings))))); it63.next();) {
      n64 = Reader_get_f32(r, in);
      f.buildings[n63.index].base = n64.Result;
      n65 = Reader_get_f32(r, in);
      f.buildings[n63.index].top = n65.Result;
      n66 = Reader_get_u32(r, in);
      f.buildings[n63.index].seed = n66.Result;
      n67 = Reader_get_u32(r, in);
      list_resize(f.buildings[n63.index].rings, n67.Result, Vector_row{List<Vector>{}});
      for (auto it68 = flow_for(n68, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(f.buildings[n63.index].rings))))); it68.next();) {
        Reader_points(r, in, f.buildings[n63.index].rings[n68.index].items);
      }
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n69 = Reader_get_u32(r, in);
    list_resize(f.roads, n69.Result, MapFeatures_Road{List<Vector>{}, RoadClass::minor, false});
    for (auto it70 = flow_for(n70, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Road>(f.roads))))); it70.next();) {
      n71 = Reader_get_u8(r, in);
      f.roads[n70.index].cls = enum_from_index<RoadClass>(n71.Result).value;
      n72 = Reader_get_u8(r, in);
      f.roads[n70.index].bridge = math_compare<"!=">(kern::to<Float>(n72.Result), 0.0);
      Reader_points(r, in, f.roads[n70.index].points);
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n73 = Reader_get_u32(r, in);
    list_resize(f.areas, n73.Result, MapFeatures_Area{List<Vector_row>{}, AreaKind::park});
    for (auto it74 = flow_for(n74, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Area>(f.areas))))); it74.next();) {
      n75 = Reader_get_u8(r, in);
      f.areas[n74.index].kind = enum_from_index<AreaKind>(n75.Result).value;
      n76 = Reader_get_u32(r, in);
      list_resize(f.areas[n74.index].rings, n76.Result, Vector_row{List<Vector>{}});
      for (auto it77 = flow_for(n77, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(f.areas[n74.index].rings))))); it77.next();) {
        Reader_points(r, in, f.areas[n74.index].rings[n77.index].items);
      }
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n78 = Reader_spots(r, in, f.trees);
    either = logic_not(n78.Result);
    if (logic_not(either)) {
      n80 = Reader_spots(r, in, f.lamps);
      either = logic_not(n80.Result);
    }
    if (either) {
      return {false};
    }
    return {r.ok};
  }

  void buildings_input_rgb(List<U8>& b, const Rgb& c) {
    wire::put_f64(b, c.r);
    wire::put_f64(b, c.g);
    wire::put_f64(b, c.b);
  }

  struct read_chunksResult { Bool Result = false; };
  read_chunksResult read_chunks(const List<U8>& output, List<GeoChunk>& out) {
    List<U8> in = List<U8>{};
    cityjobs_Reader r = cityjobs_Reader{0, true};
    in = output;
    r = cityjobs_Reader{0, true};
    n83 = Reader_get_u32(r, in);
    list_resize(out, n83.Result, GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}});
    for (auto it84 = flow_for(n84, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GeoChunk>(out))))); it84.next();) {
      read_one_chunk(in, r, out[n84.index]);
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    return {r.ok};
  }

  struct chunk_countResult { U32 Result = 0; };
  KERN_PURE chunk_countResult chunk_count(const List<U8>& output) {
    if (math_compare<"<">(kern::to<Float>(list_length<U8>(output)), 4.0)) {
      return {0};
    }
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3, Float a_4) -> Float { return kern::math::bor(kern::math::bor(kern::math::bor(a, kern::math::wrap32(kern::math::shl(a_2, 8.0))), kern::math::wrap32(kern::math::shl(a_3, 16.0))), kern::math::wrap32(kern::math::shl(a_4, 24.0))); }, kern::to<Float>(output[0]), kern::to<Float>(output[1]), kern::to<Float>(output[2]), kern::to<Float>(output[3])))};
  }

  struct read_chunkResult { Bool Result = false; };
  read_chunkResult read_chunk(const List<U8>& output, Int& at, GeoChunk& c) {
    cityjobs_Reader r = cityjobs_Reader{0, true};
    if (math_compare<">">(kern::formula([](Float at, Float a) -> Float { return (!((at < 0.0) != (a < 0.0) ? (a < 0.0) : (at < a))); }, kern::to<Float>(at), kern::to<Float>(list_length<U8>(output))), 0.5)) {
      return {false};
    }
    r = struct_make<cityjobs_Reader>(at, true);
    read_one_chunk(output, r, c);
    if (logic_not(r.ok)) {
      return {false};
    }
    at = r.p;
    return {true};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  Reader_get_u32Result n12;
  Reader_get_u32Result n13;
  Reader_get_u32Result n14;
  Reader_get_u32Result n15;
  Reader_get_u32Result n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  Reader_get_f32Result n18;
  Reader_get_f32Result n19;
  Reader_get_u32Result n20;
  struct : kern::Node<21> { Int index = 0; } n21;
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
  engine_geo_roads::GeoProjector_initResult n35;
  mvt_cpp::Decoder_stepResult n36;
  struct : kern::Node<37> { Int index = 0; } n37;
  struct : kern::Node<38> { Int index = 0; } n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  struct : kern::Node<40> { Int index = 0; } n40;
  struct : kern::Node<41> { Int index = 0; } n41;
  Reader_get_f64Result n46;
  Reader_get_f64Result n47;
  Reader_get_f64Result n48;
  Reader_get_u8Result n60;
  Reader_get_u8Result n61;
  Reader_get_u32Result n62;
  struct : kern::Node<63> { Int index = 0; } n63;
  Reader_get_f32Result n64;
  Reader_get_f32Result n65;
  Reader_get_u32Result n66;
  Reader_get_u32Result n67;
  struct : kern::Node<68> { Int index = 0; } n68;
  Reader_get_u32Result n69;
  struct : kern::Node<70> { Int index = 0; } n70;
  Reader_get_u8Result n71;
  Reader_get_u8Result n72;
  Reader_get_u32Result n73;
  struct : kern::Node<74> { Int index = 0; } n74;
  Reader_get_u8Result n75;
  Reader_get_u32Result n76;
  struct : kern::Node<77> { Int index = 0; } n77;
  Reader_spotsResult n78;
  Reader_spotsResult n80;
  Reader_get_u32Result n83;
  struct : kern::Node<84> { Int index = 0; } n84;
};
