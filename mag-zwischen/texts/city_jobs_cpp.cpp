struct city_jobs_cpp : virtual kern::Library, virtual engine_geo_buildings, virtual tiles, virtual wire {
  // ---- types ----
  KERN_STRUCT(cityjobs_Reader) { Int p = 0; Bool ok = true; };

  // ---- variables ----
  KERN_CONST U32 kTile = 1;
  KERN_CONST U32 kBuildings = 2;

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
  Reader_get_u8Result Reader_get_u8(cityjobs_Reader& self, List<U8> b = List<U8>{}) {
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
  Reader_get_u32Result Reader_get_u32(cityjobs_Reader& self, List<U8> b = List<U8>{}) {
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
  Reader_get_i32Result Reader_get_i32(cityjobs_Reader& self, List<U8> b = List<U8>{}) {
    n12 = Reader_get_u32(self, b);
    return {kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::wrap32s(the_result); }, kern::to<Float>(n12.Result)))};
  }

  struct Reader_get_f32Result { Float Result = 0.0; };
  Reader_get_f32Result Reader_get_f32(cityjobs_Reader& self, List<U8> b = List<U8>{}) {
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
  Reader_get_f64Result Reader_get_f64(cityjobs_Reader& self, List<U8> b = List<U8>{}) {
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

  void Reader_array_f32(cityjobs_Reader& self, List<Float>& v, List<U8> b = List<U8>{}) {
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

  void Reader_array_u32(cityjobs_Reader& self, List<U32>& v, List<U8> b = List<U8>{}) {
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

  void Reader_array_f64(cityjobs_Reader& self, List<Float>& v, List<U8> b = List<U8>{}) {
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

  void Reader_points(cityjobs_Reader& self, List<Vector>& v, List<U8> b = List<U8>{}) {
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
  Reader_spotsResult Reader_spots(cityjobs_Reader& self, List<MapFeatures_Spot>& v, List<U8> b = List<U8>{}) {
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
  void read_one_chunk(cityjobs_Reader& r, GeoChunk& c, List<U8> in = List<U8>{}) {
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

  struct run_buildings_rgbResult { Rgb Result = Rgb{1.0, 1.0, 1.0}; };
  run_buildings_rgbResult run_buildings_rgb(cityjobs_Reader& r, const List<U8>& in) {
    Rgb c = Rgb{1.0, 1.0, 1.0};
    c = Rgb{1.0, 1.0, 1.0};
    n41 = Reader_get_f64(r, in);
    c.r = n41.Result;
    n42 = Reader_get_f64(r, in);
    c.g = n42.Result;
    n43 = Reader_get_f64(r, in);
    c.b = n43.Result;
    return {c};
  }

  /// ---- kBuildings: footprints -> welded chunk meshes (kern::BuildingBuilder + kern::weld) ----
  void run_buildings(List<U8>& out, List<U8> in = List<U8>{}) {
    cityjobs_Reader r = cityjobs_Reader{0, true};
    BuildingOptions o = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
    List<Float> stream = List<Float>{};
    Object<kern::BuildingBuilder> b = Object<kern::BuildingBuilder>{};
    Object<kern::BuildingBuilder> building_builder = Object<kern::BuildingBuilder>{};
    Bool begin = false;
    Bool step = false;
    Bool go = false;
    WeldStats weld = WeldStats{0, 0, 0, 0, false};
    List<Float> a = List<Float>{};
    List<Float> a_2 = List<Float>{};
    List<Float> a_3 = List<Float>{};
    List<Float> a_4 = List<Float>{};
    List<Float> a_5 = List<Float>{};
    List<U32> a_6 = List<U32>{};
    r = cityjobs_Reader{0, true};
    o = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
    n37 = Reader_get_f64(r, in);
    o.chunk_size = n37.Result;
    n38 = Reader_get_u32(r, in);
    o.max_buildings = n38.Result;
    n39 = Reader_get_u8(r, in);
    o.cornice = math_compare<"!=">(kern::to<Float>(n39.Result), 0.0);
    n40 = Reader_get_u8(r, in);
    o.wall_records = math_compare<"!=">(kern::to<Float>(n40.Result), 0.0);
    n44 = Reader_get_u32(r, in);
    list_resize(o.style.walls, n44.Result, Rgb{1.0, 1.0, 1.0});
    for (auto it45 = flow_for(n45, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Rgb>(o.style.walls))))); it45.next();) {
      n46 = run_buildings_rgb(r, in);
      o.style.walls[n45.index] = n46.Result;
    }
    n47 = Reader_get_u32(r, in);
    list_resize(o.style.roofs, n47.Result, Rgb{1.0, 1.0, 1.0});
    for (auto it48 = flow_for(n48, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Rgb>(o.style.roofs))))); it48.next();) {
      n49 = run_buildings_rgb(r, in);
      o.style.roofs[n48.index] = n49.Result;
    }
    n50 = run_buildings_rgb(r, in);
    o.style.trim = n50.Result;
    n51 = Reader_get_f64(r, in);
    o.style.trim_mix = n51.Result;
    stream = List<Float>{};
    Reader_array_f64(r, stream, in);
    if (logic_or(logic_or(logic_not(r.ok), math_compare<"==">(kern::to<Float>(list_length<Rgb>(o.style.walls)), 0.0)), math_compare<"==">(kern::to<Float>(list_length<Rgb>(o.style.roofs)), 0.0))) {
      return;
    }
    engine_building_builder_make(n52);
    building_builder = n52.result;
    b = building_builder;
    engine_building_builder_begin(n53, b, stream, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(stream)))), o);
    begin = n53.result;
    if (logic_not(begin)) {
      if (object_valid<kern::BuildingBuilder>(b)) {
        engine_building_builder_release(b);
      }
      b = Object<kern::BuildingBuilder>{};
      return;
    }
    go = true;
    while (loop(go)) {
      engine_building_builder_step(n54, b, 1000000000.0);
      step = n54.result;
      if (logic_not(step)) {
      } else {
        go = false;
      }
    }
    engine_building_builder_chunks(n55, b);
    wire::put_u32(out, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<GeoChunk>(n55.result)))));
    for (auto it56 = flow_for(n56, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GeoChunk>(n55.result))))); it56.next();) {
      engine_weld(n57, n55.result[n56.index].mesh, 0);
      weld = n57.result;
      n55.result[n56.index].mesh = n57.mesh;
      put_i32(out, n55.result[n56.index].x);
      put_i32(out, n55.result[n56.index].y);
      wire::put_u32(out, n55.result[n56.index].items);
      wire::put_u32(out, n55.result[n56.index].triangles);
      wire::put_u32(out, n55.result[n56.index].mesh.extra_size);
      a = n55.result[n56.index].mesh.positions;
      put_array_f32(out, a);
      a_2 = n55.result[n56.index].mesh.normals;
      put_array_f32(out, a_2);
      a_3 = n55.result[n56.index].mesh.colors;
      put_array_f32(out, a_3);
      a_4 = n55.result[n56.index].mesh.uvs;
      put_array_f32(out, a_4);
      a_5 = n55.result[n56.index].mesh.extra;
      put_array_f32(out, a_5);
      a_6 = n55.result[n56.index].mesh.indices;
      put_array_u32(out, a_6);
    }
    if (object_valid<kern::BuildingBuilder>(b)) {
      engine_building_builder_release(b);
    }
    b = Object<kern::BuildingBuilder>{};
  }

  struct tile_inputResult { List<U8> Result = List<U8>{}; };
  tile_inputResult tile_input(const TileKey& key, Float lon0 = 0.0, Float lat0 = 0.0, List<U8> mvt = List<U8>{}) {
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
    n58 = Reader_get_u8(r, in);
    if (math_compare<"!=">(kern::to<Float>(n58.Result), 1.0)) {
      return {false};
    }
    n59 = Reader_get_u8(r, in);
    f.procedural = math_compare<"!=">(kern::to<Float>(n59.Result), 0.0);
    n60 = Reader_get_u32(r, in);
    list_resize(f.buildings, n60.Result, MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false});
    for (auto it61 = flow_for(n61, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(f.buildings))))); it61.next();) {
      n62 = Reader_get_f32(r, in);
      f.buildings[n61.index].base = n62.Result;
      n63 = Reader_get_f32(r, in);
      f.buildings[n61.index].top = n63.Result;
      n64 = Reader_get_u32(r, in);
      f.buildings[n61.index].seed = n64.Result;
      n65 = Reader_get_u32(r, in);
      list_resize(f.buildings[n61.index].rings, n65.Result, Vector_row{List<Vector>{}});
      for (auto it66 = flow_for(n66, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(f.buildings[n61.index].rings))))); it66.next();) {
        Reader_points(r, f.buildings[n61.index].rings[n66.index].items, in);
      }
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n67 = Reader_get_u32(r, in);
    list_resize(f.roads, n67.Result, MapFeatures_Road{List<Vector>{}, RoadClass::minor, false});
    for (auto it68 = flow_for(n68, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Road>(f.roads))))); it68.next();) {
      n69 = Reader_get_u8(r, in);
      f.roads[n68.index].cls = enum_from_index<RoadClass>(n69.Result).value;
      n70 = Reader_get_u8(r, in);
      f.roads[n68.index].bridge = math_compare<"!=">(kern::to<Float>(n70.Result), 0.0);
      Reader_points(r, f.roads[n68.index].points, in);
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n71 = Reader_get_u32(r, in);
    list_resize(f.areas, n71.Result, MapFeatures_Area{List<Vector_row>{}, AreaKind::park});
    for (auto it72 = flow_for(n72, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Area>(f.areas))))); it72.next();) {
      n73 = Reader_get_u8(r, in);
      f.areas[n72.index].kind = enum_from_index<AreaKind>(n73.Result).value;
      n74 = Reader_get_u32(r, in);
      list_resize(f.areas[n72.index].rings, n74.Result, Vector_row{List<Vector>{}});
      for (auto it75 = flow_for(n75, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(f.areas[n72.index].rings))))); it75.next();) {
        Reader_points(r, f.areas[n72.index].rings[n75.index].items, in);
      }
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n76 = Reader_spots(r, f.trees, in);
    either = logic_not(n76.Result);
    if (logic_not(either)) {
      n78 = Reader_spots(r, f.lamps, in);
      either = logic_not(n78.Result);
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

  struct buildings_inputResult { List<U8> Result = List<U8>{}; };
  buildings_inputResult buildings_input(const BuildingOptions& o, Object<kern::FootprintStream> footprints = Object<kern::FootprintStream>{}) {
    List<U8> b = List<U8>{};
    List<Float> stream = List<Float>{};
    List<Float> values = List<Float>{};
    b = List<U8>{};
    wire::put_f64(b, o.chunk_size);
    wire::put_u32(b, o.max_buildings);
    wire::put_u8(b, kern::to<Int>(kern::formula([](Float cornice) -> Float { return kern::math::band(((cornice != 0.0) ? 1.0 : 0.0), 255.0); }, kern::to<Float>(o.cornice))));
    wire::put_u8(b, kern::to<Int>(kern::formula([](Float wall_records) -> Float { return kern::math::band(((wall_records != 0.0) ? 1.0 : 0.0), 255.0); }, kern::to<Float>(o.wall_records))));
    wire::put_u32(b, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Rgb>(o.style.walls)))));
    for (auto it79 = flow_for(n79, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Rgb>(o.style.walls))))); it79.next();) {
      buildings_input_rgb(b, o.style.walls[n79.index]);
    }
    wire::put_u32(b, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Rgb>(o.style.roofs)))));
    for (auto it80 = flow_for(n80, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Rgb>(o.style.roofs))))); it80.next();) {
      buildings_input_rgb(b, o.style.roofs[n80.index]);
    }
    buildings_input_rgb(b, o.style.trim);
    wire::put_f64(b, o.style.trim_mix);
    values = engine_footprint_stream_values(footprints);
    stream = values;
    put_array_f64(b, stream);
    return {b};
  }

  struct read_chunksResult { Bool Result = false; };
  read_chunksResult read_chunks(const List<U8>& output, List<GeoChunk>& out) {
    List<U8> in = List<U8>{};
    cityjobs_Reader r = cityjobs_Reader{0, true};
    in = output;
    r = cityjobs_Reader{0, true};
    n81 = Reader_get_u32(r, in);
    list_resize(out, n81.Result, GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}});
    for (auto it82 = flow_for(n82, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GeoChunk>(out))))); it82.next();) {
      read_one_chunk(r, out[n82.index], in);
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
    read_one_chunk(r, c, output);
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
  Reader_get_f64Result n37;
  Reader_get_u32Result n38;
  Reader_get_u8Result n39;
  Reader_get_u8Result n40;
  Reader_get_f64Result n41;
  Reader_get_f64Result n42;
  Reader_get_f64Result n43;
  Reader_get_u32Result n44;
  struct : kern::Node<45> { Int index = 0; } n45;
  run_buildings_rgbResult n46;
  Reader_get_u32Result n47;
  struct : kern::Node<48> { Int index = 0; } n48;
  run_buildings_rgbResult n49;
  run_buildings_rgbResult n50;
  Reader_get_f64Result n51;
  struct { Object<kern::BuildingBuilder> result = Object<kern::BuildingBuilder>{}; } n52;
  struct { Bool result = false; } n53;
  struct { Bool result = false; } n54;
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n55;
  struct : kern::Node<56> { Int index = 0; } n56;
  struct { WeldStats result = WeldStats{0, 0, 0, 0, false}; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n57;
  Reader_get_u8Result n58;
  Reader_get_u8Result n59;
  Reader_get_u32Result n60;
  struct : kern::Node<61> { Int index = 0; } n61;
  Reader_get_f32Result n62;
  Reader_get_f32Result n63;
  Reader_get_u32Result n64;
  Reader_get_u32Result n65;
  struct : kern::Node<66> { Int index = 0; } n66;
  Reader_get_u32Result n67;
  struct : kern::Node<68> { Int index = 0; } n68;
  Reader_get_u8Result n69;
  Reader_get_u8Result n70;
  Reader_get_u32Result n71;
  struct : kern::Node<72> { Int index = 0; } n72;
  Reader_get_u8Result n73;
  Reader_get_u32Result n74;
  struct : kern::Node<75> { Int index = 0; } n75;
  Reader_spotsResult n76;
  Reader_spotsResult n78;
  struct : kern::Node<79> { Int index = 0; } n79;
  struct : kern::Node<80> { Int index = 0; } n80;
  Reader_get_u32Result n81;
  struct : kern::Node<82> { Int index = 0; } n82;
};
