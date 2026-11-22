struct city_jobs_cpp : virtual kern::Library, virtual engine_geo_buildings, virtual tiles, virtual wire {
  // ---- types ----
  KERN_STRUCT(cityjobs_Reader) { Int p = 0; Bool ok = true; };

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

  struct run_buildings_rgbResult { Rgb Result = Rgb{1.0, 1.0, 1.0}; };
  run_buildings_rgbResult run_buildings_rgb(cityjobs_Reader& r, const List<U8>& in) {
    Rgb c = Rgb{1.0, 1.0, 1.0};
    c = Rgb{1.0, 1.0, 1.0};
    n39 = Reader_get_f64(r, in);
    c.r = n39.Result;
    n40 = Reader_get_f64(r, in);
    c.g = n40.Result;
    n41 = Reader_get_f64(r, in);
    c.b = n41.Result;
    return {c};
  }

  /// ---- kBuildings: footprints -> welded chunk meshes (kern::BuildingBuilder + kern::weld) ----
  void run_buildings(const List<U8>& in, List<U8>& out) {
    cityjobs_Reader r = cityjobs_Reader{0, true};
    BuildingOptions o = BuildingOptions{320.0, 2600, true, true, BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}};
    List<Float> stream = List<Float>{};
    Int b = 0;
    Int BuildingBuilder_2 = 0;
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
    n35 = Reader_get_f64(r, in);
    o.chunk_size = n35.Result;
    n36 = Reader_get_u32(r, in);
    o.max_buildings = n36.Result;
    n37 = Reader_get_u8(r, in);
    o.cornice = math_compare<"!=">(kern::to<Float>(n37.Result), 0.0);
    n38 = Reader_get_u8(r, in);
    o.wall_records = math_compare<"!=">(kern::to<Float>(n38.Result), 0.0);
    n42 = Reader_get_u32(r, in);
    list_resize(o.style.walls, n42.Result, Rgb{1.0, 1.0, 1.0});
    for (auto it43 = flow_for(n43, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Rgb>(o.style.walls))))); it43.next();) {
      n44 = run_buildings_rgb(r, in);
      o.style.walls[n43.index] = n44.Result;
    }
    n45 = Reader_get_u32(r, in);
    list_resize(o.style.roofs, n45.Result, Rgb{1.0, 1.0, 1.0});
    for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Rgb>(o.style.roofs))))); it46.next();) {
      n47 = run_buildings_rgb(r, in);
      o.style.roofs[n46.index] = n47.Result;
    }
    n48 = run_buildings_rgb(r, in);
    o.style.trim = n48.Result;
    n49 = Reader_get_f64(r, in);
    o.style.trim_mix = n49.Result;
    stream = List<Float>{};
    Reader_array_f64(r, in, stream);
    if (logic_or(logic_or(logic_not(r.ok), math_compare<"==">(kern::to<Float>(list_length<Rgb>(o.style.walls)), 0.0)), math_compare<"==">(kern::to<Float>(list_length<Rgb>(o.style.roofs)), 0.0))) {
      return;
    }
    engine_building_builder_make(n50);
    BuildingBuilder_2 = n50.result;
    b = BuildingBuilder_2;
    engine_building_builder_begin(n51, b, stream, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Float>(stream)))), o);
    begin = n51.result;
    if (logic_not(begin)) {
      if (math_compare<"!=">(kern::to<Float>(b), 0.0)) {
        engine_building_builder_release(b);
      }
      b = 0;
      return;
    }
    go = true;
    while (loop(go)) {
      engine_building_builder_step(n52, b, 1000000000.0);
      step = n52.result;
      if (logic_not(step)) {
      } else {
        go = false;
      }
    }
    engine_building_builder_chunks(n53, b);
    wire::put_u32(out, kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<GeoChunk>(n53.result)))));
    for (auto it54 = flow_for(n54, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GeoChunk>(n53.result))))); it54.next();) {
      engine_weld(n55, n53.result[n54.index].mesh, 0);
      weld = n55.result;
      n53.result[n54.index].mesh = n55.mesh;
      put_i32(out, n53.result[n54.index].x);
      put_i32(out, n53.result[n54.index].y);
      wire::put_u32(out, n53.result[n54.index].items);
      wire::put_u32(out, n53.result[n54.index].triangles);
      wire::put_u32(out, n53.result[n54.index].mesh.extra_size);
      a = n53.result[n54.index].mesh.positions;
      put_array_f32(out, a);
      a_2 = n53.result[n54.index].mesh.normals;
      put_array_f32(out, a_2);
      a_3 = n53.result[n54.index].mesh.colors;
      put_array_f32(out, a_3);
      a_4 = n53.result[n54.index].mesh.uvs;
      put_array_f32(out, a_4);
      a_5 = n53.result[n54.index].mesh.extra;
      put_array_f32(out, a_5);
      a_6 = n53.result[n54.index].mesh.indices;
      put_array_u32(out, a_6);
    }
    if (math_compare<"!=">(kern::to<Float>(b), 0.0)) {
      engine_building_builder_release(b);
    }
    b = 0;
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
    n56 = Reader_get_u8(r, in);
    if (math_compare<"!=">(kern::to<Float>(n56.Result), 1.0)) {
      return {false};
    }
    n57 = Reader_get_u8(r, in);
    f.procedural = math_compare<"!=">(kern::to<Float>(n57.Result), 0.0);
    n58 = Reader_get_u32(r, in);
    list_resize(f.buildings, n58.Result, MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false});
    for (auto it59 = flow_for(n59, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(f.buildings))))); it59.next();) {
      n60 = Reader_get_f32(r, in);
      f.buildings[n59.index].base = n60.Result;
      n61 = Reader_get_f32(r, in);
      f.buildings[n59.index].top = n61.Result;
      n62 = Reader_get_u32(r, in);
      f.buildings[n59.index].seed = n62.Result;
      n63 = Reader_get_u32(r, in);
      list_resize(f.buildings[n59.index].rings, n63.Result, Vector_row{List<Vector>{}});
      for (auto it64 = flow_for(n64, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(f.buildings[n59.index].rings))))); it64.next();) {
        Reader_points(r, in, f.buildings[n59.index].rings[n64.index].items);
      }
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n65 = Reader_get_u32(r, in);
    list_resize(f.roads, n65.Result, MapFeatures_Road{List<Vector>{}, RoadClass::minor, false});
    for (auto it66 = flow_for(n66, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Road>(f.roads))))); it66.next();) {
      n67 = Reader_get_u8(r, in);
      f.roads[n66.index].cls = enum_from_index<RoadClass>(n67.Result).value;
      n68 = Reader_get_u8(r, in);
      f.roads[n66.index].bridge = math_compare<"!=">(kern::to<Float>(n68.Result), 0.0);
      Reader_points(r, in, f.roads[n66.index].points);
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n69 = Reader_get_u32(r, in);
    list_resize(f.areas, n69.Result, MapFeatures_Area{List<Vector_row>{}, AreaKind::park});
    for (auto it70 = flow_for(n70, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<MapFeatures_Area>(f.areas))))); it70.next();) {
      n71 = Reader_get_u8(r, in);
      f.areas[n70.index].kind = enum_from_index<AreaKind>(n71.Result).value;
      n72 = Reader_get_u32(r, in);
      list_resize(f.areas[n70.index].rings, n72.Result, Vector_row{List<Vector>{}});
      for (auto it73 = flow_for(n73, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(f.areas[n70.index].rings))))); it73.next();) {
        Reader_points(r, in, f.areas[n70.index].rings[n73.index].items);
      }
      if (logic_not(r.ok)) {
        return {false};
      }
    }
    n74 = Reader_spots(r, in, f.trees);
    either = logic_not(n74.Result);
    if (logic_not(either)) {
      n76 = Reader_spots(r, in, f.lamps);
      either = logic_not(n76.Result);
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
    n79 = Reader_get_u32(r, in);
    list_resize(out, n79.Result, GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}});
    for (auto it80 = flow_for(n80, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<GeoChunk>(out))))); it80.next();) {
      read_one_chunk(in, r, out[n80.index]);
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
  Reader_get_f64Result n35;
  Reader_get_u32Result n36;
  Reader_get_u8Result n37;
  Reader_get_u8Result n38;
  Reader_get_f64Result n39;
  Reader_get_f64Result n40;
  Reader_get_f64Result n41;
  Reader_get_u32Result n42;
  struct : kern::Node<43> { Int index = 0; } n43;
  run_buildings_rgbResult n44;
  Reader_get_u32Result n45;
  struct : kern::Node<46> { Int index = 0; } n46;
  run_buildings_rgbResult n47;
  run_buildings_rgbResult n48;
  Reader_get_f64Result n49;
  struct { Int result = 0; } n50;
  struct { Bool result = false; } n51;
  struct { Bool result = false; } n52;
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n53;
  struct : kern::Node<54> { Int index = 0; } n54;
  struct { WeldStats result = WeldStats{0, 0, 0, 0, false}; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; } n55;
  Reader_get_u8Result n56;
  Reader_get_u8Result n57;
  Reader_get_u32Result n58;
  struct : kern::Node<59> { Int index = 0; } n59;
  Reader_get_f32Result n60;
  Reader_get_f32Result n61;
  Reader_get_u32Result n62;
  Reader_get_u32Result n63;
  struct : kern::Node<64> { Int index = 0; } n64;
  Reader_get_u32Result n65;
  struct : kern::Node<66> { Int index = 0; } n66;
  Reader_get_u8Result n67;
  Reader_get_u8Result n68;
  Reader_get_u32Result n69;
  struct : kern::Node<70> { Int index = 0; } n70;
  Reader_get_u8Result n71;
  Reader_get_u32Result n72;
  struct : kern::Node<73> { Int index = 0; } n73;
  Reader_spotsResult n74;
  Reader_spotsResult n76;
  Reader_get_u32Result n79;
  struct : kern::Node<80> { Int index = 0; } n80;
};
