struct tiles_cpp : virtual kern::Library, virtual common, virtual engine_geo_roads, virtual engine_math_math, virtual mvt_cpp, virtual tiles {
  // ---- types ----
  KERN_STRUCT(FurnishIndex_Seg) { Vector a = Vector{0.0, 0.0, 0.0}; Vector b = Vector{0.0, 0.0, 0.0}; f32 half = 0.0f; u32 road = 0; bool paved = false; };
  KERN_STRUCT(Projector) { f64 x0 = 0.0; f64 x1 = 0.0; f64 y0 = 0.0; f64 ym = 0.0; f64 y1 = 0.0; f64 inv_extent = 0.0; };
  KERN_STRUCT(FurnishIndex_Ring) { bool apron = false; u32 index = 0; Vec4 box = Vec4{0.0f, 0.0f, 0.0f, 0.0f}; };
  KERN_STRUCT(parse_road_M) { Text n = ""; RoadClass c = RoadClass::motorway; };
  KERN_STRUCT(FurnishIndex) { List<FurnishIndex_Seg> segs = List<FurnishIndex_Seg>{}; List<FurnishIndex_Ring> rings = List<FurnishIndex_Ring>{}; Map<u64, List<u32>> seg_cells = Map<u64, List<u32>>{}; Map<u64, List<u32>> ring_cells = Map<u64, List<u32>>{}; };

  // ---- variables ----
  KERN_CONST f64 kPiD = 3.141592653589793;
  KERN_CONST f64 kEarthRadius = 6378137.0;
  KERN_CONST f64 kRad = 0.017453292519943295;
  KERN_CONST Fixed<parse_road_M, 12> map = List<parse_road_M>{parse_road_M{"motorway", RoadClass::motorway}, parse_road_M{"trunk", RoadClass::trunk}, parse_road_M{"primary", RoadClass::primary}, parse_road_M{"secondary", RoadClass::secondary}, parse_road_M{"tertiary", RoadClass::tertiary}, parse_road_M{"minor", RoadClass::minor}, parse_road_M{"service", RoadClass::service}, parse_road_M{"track", RoadClass::track}, parse_road_M{"path", RoadClass::path}, parse_road_M{"pedestrian", RoadClass::pedestrian}, parse_road_M{"raceway", RoadClass::tertiary}, parse_road_M{"busway", RoadClass::minor}};
  KERN_CONST f32 kCell = 20.0f;

  // ---- functions ----
  struct tile_atResult { TileKey Result = TileKey{14, 0, 0}; };
  KERN_PURE tile_atResult tile_at(f64 lon = 0.0, f64 lat = 0.0, i32 z = 0) {
    f64 n = 0.0;
    f64 lr = 0.0;
    TileKey t = TileKey{14, 0, 0};
    n = kern::formula<f64>([](auto a) { return a; }, math_ldexp<f64>(1.0, z));
    lr = kern::formula<f64, f64>([](auto lat, auto kPiD) { return ((lat * kPiD) / 180.0); }, lat, kPiD);
    t = TileKey{14, 0, 0};
    t.z = z;
    t.x = kern::formula<f64, f64>([](auto lon, auto n) { return num_cast<i32, f64>(floor((((lon + 180.0) / 360.0) * n))); }, lon, n);
    t.y = kern::formula<f64, f64, f64>([](auto lr, auto kPiD, auto n) { return num_cast<i32, f64>(floor((((1.0 - (asinh(tan(lr)) / kPiD)) / 2.0) * n))); }, lr, kPiD, n);
    return {t};
  }

  void tile_lonlat(const TileKey& t, f64& lon, f64& lat, f64 fx = 0.0, f64 fy = 0.0) {
    f64 n = 0.0;
    n = kern::formula<f64>([](auto a) { return a; }, math_ldexp<f64>(1.0, t.z));
    lon = kern::formula<i32, f64, f64>([](auto x, auto fx, auto n) { return ((((num_cast<f64, i32>(x) + fx) / n) * 360.0) - 180.0); }, t.x, fx, n);
    lat = kern::formula<f64, i32, f64, f64>([](auto kPiD, auto y, auto fy, auto n) { return ((atan(sinh((kPiD * (1.0 - ((2.0 * (num_cast<f64, i32>(y) + fy)) / n))))) * 180.0) / kPiD); }, kPiD, t.y, fy, n);
  }

  void unproject(const GeoProjector& p, f64& lon, f64& lat, f64 x = 0.0, f64 y = 0.0) {
    lon = kern::formula<f64, f64, f64, f64, f64>([](auto lon0, auto x, auto kEarthRadius, auto cos_lat, auto kRad) { return (lon0 + ((x / (kEarthRadius * cos_lat)) / kRad)); }, p.lon0, x, kEarthRadius, p.cos_lat, kRad);
    lat = kern::formula<f64, f64, f64, f64>([](auto lat0, auto y, auto kEarthRadius, auto kRad) { return (lat0 + ((y / kEarthRadius) / kRad)); }, p.lat0, y, kEarthRadius, kRad);
  }

  struct road_widthResult { f32 Result = 0.0f; };
  KERN_PURE road_widthResult road_width(RoadClass c = RoadClass::motorway) {
    switch (flow_switch_enum<RoadClass>(c)) {
      case RoadClass::motorway: {
        return {14.0f};
      } break;
      case RoadClass::trunk: {
        return {12.0f};
      } break;
      case RoadClass::primary: {
        return {11.0f};
      } break;
      case RoadClass::secondary: {
        return {9.0f};
      } break;
      case RoadClass::tertiary: {
        return {8.0f};
      } break;
      case RoadClass::minor: {
        return {6.5f};
      } break;
      case RoadClass::service: {
        return {4.2f};
      } break;
      case RoadClass::track: {
        return {3.0f};
      } break;
      case RoadClass::path: {
        return {2.2f};
      } break;
      case RoadClass::pedestrian: {
        return {3.6f};
      } break;
      case RoadClass::river: {
        return {18.0f};
      } break;
      case RoadClass::canal: {
        return {12.0f};
      } break;
      case RoadClass::stream: {
        return {4.5f};
      } break;
    }
  }

  struct road_is_waterResult { bool Result = false; };
  KERN_PURE road_is_waterResult road_is_water(RoadClass c = RoadClass::motorway) {
    return {logic_or(logic_or(enum_equal<RoadClass>(c, RoadClass::river), enum_equal<RoadClass>(c, RoadClass::canal)), enum_equal<RoadClass>(c, RoadClass::stream))};
  }

  struct road_drivableResult { bool Result = false; };
  KERN_PURE road_drivableResult road_drivable(RoadClass c = RoadClass::motorway) {
    return {math_compare<"<=", i32>(num_cast<i32, u8>(num_cast<u8, i64>(enum_index<RoadClass>(c))), num_cast<i32, u8>(num_cast<u8, i64>(enum_index<RoadClass>(RoadClass::service))))};
  }

  struct parse_roadResult { bool Result = false; };
  parse_roadResult parse_road(RoadClass& out, Text cls = "") {
    List<parse_road_M> list = List<parse_road_M>{};
    list = map;
    for (auto it10 = flow_for(n10, 0, math_sub<i64>(list_length<parse_road_M>(list), 1)); it10.next();) {
      if (text_equal(cls, list[n10.index].n)) {
        out = list[n10.index].c;
        return {true};
      }
    }
    return {false};
  }

  /// Groups MVT polygon parts into polygons (exterior ring: positive area in tile space, y down).
  void polygons(const mvt_Feature& f, List<Vector_row_row>& out) {
    bool skip = false;
    f64 a = 0.0;
    list_clear(out);
    for (auto it11 = flow_for(n11, 0, math_sub<i64>(list_length<Vector_row>(f.parts), 1)); it11.next();) {
      skip = false;
      if (math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(f.parts[n11.index]).items)), 3)) {
        skip = true;
      }
      if (logic_not(skip)) {
        a = mvt_cpp::ring_area(f.parts[n11.index].items).Result;
        if (logic_or(math_compare<">", f64>(a, 0.0), math_compare<"==", i64>(list_length<Vector_row_row>(out), 0))) {
          list_add(out, Vector_row_row{List<Vector_row>{}});
        }
        list_add(out[math_sub<i64>(list_length<Vector_row_row>(out), 1)].items, f.parts[n11.index]);
      }
    }
  }

  struct Projector_newResult { Projector Result = Projector{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; };
  Projector_newResult Projector_new(const TileKey& key, const GeoProjector& proj, f64 extent = 0.0) {
    Projector self = Projector{0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    f64 lon = 0.0;
    f64 lat = 0.0;
    f64 x = 0.0;
    self.inv_extent = kern::formula<f64>([](auto extent) { return (1.0 / extent); }, extent);
    lon = 0.0;
    lat = 0.0;
    x = 0.0;
    tile_lonlat(key, lon, lat, 0.0, 0.0);
    engine_geo_projector_project(n12, proj, lon, lat, x, self.y0);
    x = n12.x;
    self.y0 = n12.y;
    self.x0 = x;
    tile_lonlat(key, lon, lat, 1.0, 0.5);
    engine_geo_projector_project(n13, proj, lon, lat, x, self.ym);
    x = n13.x;
    self.ym = n13.y;
    self.x1 = x;
    tile_lonlat(key, lon, lat, 1.0, 1.0);
    engine_geo_projector_project(n14, proj, lon, lat, x, self.y1);
    x = n14.x;
    self.y1 = n14.y;
    return {self};
  }

  struct Projector_callResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Projector_callResult Projector_call(const Projector& self, Vector t = Vector{0.0, 0.0, 0.0}) {
    f64 fx = 0.0;
    f64 fy = 0.0;
    f64 y = 0.0;
    fx = kern::formula<f32, f64>([](auto x, auto inv_extent) { return (num_cast<f64, f32>(x) * inv_extent); }, vector_break(t).x, self.inv_extent);
    fy = kern::formula<f32, f64>([](auto y, auto inv_extent) { return (num_cast<f64, f32>(y) * inv_extent); }, vector_break(t).y, self.inv_extent);
    y = kern::formula<f64, f64, f64, f64>([](auto y0, auto fy, auto ym, auto y1) { return ((((y0 * ((2.0 * fy) - 1.0)) * (fy - 1.0)) + (((ym * 4.0) * fy) * (1.0 - fy))) + ((y1 * fy) * ((2.0 * fy) - 1.0))); }, self.y0, fy, self.ym, self.y1);
    return {vector_make(kern::formula<f64, f64, f64>([](auto x0, auto fx, auto x1) { return num_cast<f32, f64>((x0 + (fx * (x1 - x0)))); }, self.x0, fx, self.x1), num_cast<f32, f64>(y), 0.0f)};
  }

  struct inside_sideResult { bool Result = false; };
  /// The four sides of the square, in the order a ring is clipped to them: x >= 0, x <= ext, y >= 0, y <= ext.
  KERN_PURE inside_sideResult inside_side(i32 side = 0, Vector p = Vector{0.0, 0.0, 0.0}, f32 ext = 0.0f) {
    return {logic_select<bool>(math_compare<"==", i32>(side, 0), math_compare<">=", f32>(vector_break(p).x, 0.0f), logic_select<bool>(math_compare<"==", i32>(side, 1), math_compare<"<=", f32>(vector_break(p).x, ext), logic_select<bool>(math_compare<"==", i32>(side, 2), math_compare<">=", f32>(vector_break(p).y, 0.0f), math_compare<"<=", f32>(vector_break(p).y, ext))))};
  }

  struct cut_sideResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  /// Where the edge p..q crosses the side's line.
  KERN_PURE cut_sideResult cut_side(i32 side = 0, Vector p = Vector{0.0, 0.0, 0.0}, Vector q = Vector{0.0, 0.0, 0.0}, f32 ext = 0.0f) {
    f32 x = 0.0f;
    f32 t = 0.0f;
    f32 y = 0.0f;
    f32 t_2 = 0.0f;
    if (math_compare<"<", i32>(side, 2)) {
      x = kern::formula<i32, f32, f32>([](auto side, auto a0f, auto ext) { return ((side == 0) ? a0f : ext); }, side, 0.0f, ext);
      t = kern::formula<f32, f32, f32>([](auto x, auto x_2, auto x_3) { return ((x - x_2) / (x_3 - x_2)); }, x, vector_break(p).x, vector_break(q).x);
      return {vector_make(x, kern::formula<f32, f32, f32>([](auto y, auto y_2, auto t) { return (y + ((y_2 - y) * t)); }, vector_break(p).y, vector_break(q).y, t), 0.0f)};
    }
    y = kern::formula<i32, f32, f32>([](auto side, auto a0f, auto ext) { return ((side == 2) ? a0f : ext); }, side, 0.0f, ext);
    t_2 = kern::formula<f32, f32, f32>([](auto y, auto y_2, auto y_3) { return ((y - y_2) / (y_3 - y_2)); }, y, vector_break(p).y, vector_break(q).y);
    return {vector_make(kern::formula<f32, f32, f32>([](auto x, auto x_2, auto t_2) { return (x + ((x_2 - x) * t_2)); }, vector_break(p).x, vector_break(q).x, t_2), y, 0.0f)};
  }

  /// One pass of the clip: what of `a` lies inside the side, into `b` - then `a` is that.
  void clip_pass(List<Vector>& a, List<Vector>& b, i32 side = 0, f32 ext = 0.0f) {
    Vector p = Vector{0.0, 0.0, 0.0};
    Vector q = Vector{0.0, 0.0, 0.0};
    bool pi = false;
    bool qi = false;
    List<Vector> swapped = List<Vector>{};
    list_clear(b);
    for (auto it15 = flow_for(n15, 0, math_sub<i64>(list_length<Vector>(a), 1)); it15.next();) {
      p = a[n15.index];
      q = a[num_cast<i64, u64>(kern::formula<i64, u64>([](auto index, auto a) { return ((num_cast<u64, i64>(index) + 1) % a); }, n15.index, num_cast<u64, i64>(list_length<Vector>(a))))];
      pi = inside_side(side, p, ext).Result;
      qi = inside_side(side, q, ext).Result;
      if (pi) {
        list_add(b, p);
      }
      if (math_compare<"!=", i32>(num_cast<i32, bool>(pi), num_cast<i32, bool>(qi))) {
        list_add(b, cut_side(side, p, q, ext).Result);
      }
    }
    var_take(n16, a);
    swapped = n16.value;
    var_take(n17, b);
    a = n17.value;
    b = swapped;
  }

  struct clip_to_tileResult { List<Vector> Result = List<Vector>{}; };
  clip_to_tileResult clip_to_tile(const List<Vector>& ring, f32 ext = 0.0f) {
    List<Vector> a = List<Vector>{};
    List<Vector> b = List<Vector>{};
    a = ring;
    b = List<Vector>{};
    for (auto it18 = flow_for(n18, 0, 3); it18.next();) {
      if (math_compare<">=", u64>(num_cast<u64, i64>(list_length<Vector>(a)), 3)) {
        clip_pass(a, b, num_cast<i32, i64>(n18.index), ext);
      }
    }
    return {a};
  }

  struct hash_u32Result { u32 Result = 0; };
  KERN_PURE hash_u32Result hash_u32(u64 v = 0) {
    u64 v_ = 0;
    v_ = v;
    v_ = kern::formula<u64>([](auto v_) { return (v_ ^ (v_ >> 33)); }, v_);
    v_ = kern::formula<u64>([](auto v_) { return (v_ * 18397679294719823053u); }, v_);
    v_ = kern::formula<u64>([](auto v_) { return (v_ ^ (v_ >> 33)); }, v_);
    v_ = kern::formula<u64>([](auto v_) { return (v_ * 14181476777654086739u); }, v_);
    v_ = kern::formula<u64>([](auto v_) { return (v_ ^ (v_ >> 33)); }, v_);
    return {num_cast<u32, u64>(v_)};
  }

  struct extract_areas_kind_ofResult { bool Result = false; };
  extract_areas_kind_ofResult extract_areas_kind_of(const mvt_Layer& arg1, const mvt_Feature& arg2, AreaKind& k) {
    k = AreaKind::water;
    return {true};
  }

  void extract_areas(const mvt_Tile& tile, const TileKey& key, const GeoProjector& proj, List<Vector_row_row>& polys, MapFeatures& out, Text layer = "") {
    i32 at = 0;
    mvt_Layer l = mvt_Layer{"", 4096, List<Text>{}, List<mvt_Value>{}, List<mvt_Feature>{}};
    Projector P = Projector{0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    bool skip = false;
    AreaKind kind = AreaKind::water;
    MapFeatures_Area a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    List<Vector> m = List<Vector>{};
    Vector p = Vector{0.0, 0.0, 0.0};
    at = mvt_cpp::Tile_layer(tile, layer).Result;
    if (math_compare<"<", i32>(at, 0)) {
      return;
    }
    l = tile.layers[num_cast<i64, i32>(at)];
    n35 = Projector_new(key, proj, num_cast<f64, u32>(l.extent));
    P = n35.Result;
    for (auto it36 = flow_for(n36, 0, math_sub<i64>(list_length<mvt_Feature>(l.features), 1)); it36.next();) {
      skip = false;
      if (logic_not(enum_equal<mvt_GeomType>(l.features[n36.index].type, mvt_GeomType::polygon))) {
        skip = true;
      }
      if (logic_not(skip)) {
        kind = AreaKind::water;
        n37 = extract_areas_kind_of(l, l.features[n36.index], kind);
        if (logic_not(n37.Result)) {
          skip = true;
        }
        if (logic_not(skip)) {
          polygons(l.features[n36.index], polys);
          for (auto it38 = flow_for(n38, 0, math_sub<i64>(list_length<Vector_row_row>(polys), 1)); it38.next();) {
            a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
            a.kind = kind;
            for (auto it39 = flow_for(n39, 0, math_sub<i64>(list_length<Vector_row>(polys[n38.index].items), 1)); it39.next();) {
              m = List<Vector>{};
              for (auto it40 = flow_for(n40, 0, math_sub<i64>(list_length<Vector>(polys[n38.index].items[n39.index].items), 1)); it40.next();) {
                p = polys[n38.index].items[n39.index].items[n40.index];
                list_add(m, Projector_call(P, p).Result);
              }
              list_add(a.rings, struct_make<Vector_row>(m));
            }
            list_add(out.areas, a);
          }
        }
      }
    }
  }

  struct extract_areas_kind_of_2Result { bool Result = false; };
  extract_areas_kind_of_2Result extract_areas_kind_of_2(const mvt_Layer& arg1, const mvt_Feature& arg2, AreaKind& k) {
    k = AreaKind::park;
    return {true};
  }

  void extract_areas_2(const mvt_Tile& tile, const TileKey& key, const GeoProjector& proj, List<Vector_row_row>& polys, MapFeatures& out, Text layer = "") {
    i32 at = 0;
    mvt_Layer l = mvt_Layer{"", 4096, List<Text>{}, List<mvt_Value>{}, List<mvt_Feature>{}};
    Projector P = Projector{0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    bool skip = false;
    AreaKind kind = AreaKind::water;
    MapFeatures_Area a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    List<Vector> m = List<Vector>{};
    Vector p = Vector{0.0, 0.0, 0.0};
    at = mvt_cpp::Tile_layer(tile, layer).Result;
    if (math_compare<"<", i32>(at, 0)) {
      return;
    }
    l = tile.layers[num_cast<i64, i32>(at)];
    n41 = Projector_new(key, proj, num_cast<f64, u32>(l.extent));
    P = n41.Result;
    for (auto it42 = flow_for(n42, 0, math_sub<i64>(list_length<mvt_Feature>(l.features), 1)); it42.next();) {
      skip = false;
      if (logic_not(enum_equal<mvt_GeomType>(l.features[n42.index].type, mvt_GeomType::polygon))) {
        skip = true;
      }
      if (logic_not(skip)) {
        kind = AreaKind::water;
        n43 = extract_areas_kind_of_2(l, l.features[n42.index], kind);
        if (logic_not(n43.Result)) {
          skip = true;
        }
        if (logic_not(skip)) {
          polygons(l.features[n42.index], polys);
          for (auto it44 = flow_for(n44, 0, math_sub<i64>(list_length<Vector_row_row>(polys), 1)); it44.next();) {
            a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
            a.kind = kind;
            for (auto it45 = flow_for(n45, 0, math_sub<i64>(list_length<Vector_row>(polys[n44.index].items), 1)); it45.next();) {
              m = List<Vector>{};
              for (auto it46 = flow_for(n46, 0, math_sub<i64>(list_length<Vector>(polys[n44.index].items[n45.index].items), 1)); it46.next();) {
                p = polys[n44.index].items[n45.index].items[n46.index];
                list_add(m, Projector_call(P, p).Result);
              }
              list_add(a.rings, struct_make<Vector_row>(m));
            }
            list_add(out.areas, a);
          }
        }
      }
    }
  }

  struct extract_areas_kind_of_3Result { bool Result = false; };
  extract_areas_kind_of_3Result extract_areas_kind_of_3(const mvt_Layer& l, const mvt_Feature& f, AreaKind& k) {
    Text c = "";
    c = mvt_cpp::Layer_text(l, f, "class").Result;
    if (text_equal(c, "wood")) {
      k = AreaKind::wood;
    } else if (text_equal(c, "grass")) {
      k = AreaKind::grass;
    } else if (text_equal(c, "sand")) {
      k = AreaKind::sand;
    } else {
      return {false};
    }
    return {true};
  }

  void extract_areas_3(const mvt_Tile& tile, const TileKey& key, const GeoProjector& proj, List<Vector_row_row>& polys, MapFeatures& out, Text layer = "") {
    i32 at = 0;
    mvt_Layer l = mvt_Layer{"", 4096, List<Text>{}, List<mvt_Value>{}, List<mvt_Feature>{}};
    Projector P = Projector{0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    bool skip = false;
    AreaKind kind = AreaKind::water;
    MapFeatures_Area a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    List<Vector> m = List<Vector>{};
    Vector p = Vector{0.0, 0.0, 0.0};
    at = mvt_cpp::Tile_layer(tile, layer).Result;
    if (math_compare<"<", i32>(at, 0)) {
      return;
    }
    l = tile.layers[num_cast<i64, i32>(at)];
    n47 = Projector_new(key, proj, num_cast<f64, u32>(l.extent));
    P = n47.Result;
    for (auto it48 = flow_for(n48, 0, math_sub<i64>(list_length<mvt_Feature>(l.features), 1)); it48.next();) {
      skip = false;
      if (logic_not(enum_equal<mvt_GeomType>(l.features[n48.index].type, mvt_GeomType::polygon))) {
        skip = true;
      }
      if (logic_not(skip)) {
        kind = AreaKind::water;
        n49 = extract_areas_kind_of_3(l, l.features[n48.index], kind);
        if (logic_not(n49.Result)) {
          skip = true;
        }
        if (logic_not(skip)) {
          polygons(l.features[n48.index], polys);
          for (auto it50 = flow_for(n50, 0, math_sub<i64>(list_length<Vector_row_row>(polys), 1)); it50.next();) {
            a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
            a.kind = kind;
            for (auto it51 = flow_for(n51, 0, math_sub<i64>(list_length<Vector_row>(polys[n50.index].items), 1)); it51.next();) {
              m = List<Vector>{};
              for (auto it52 = flow_for(n52, 0, math_sub<i64>(list_length<Vector>(polys[n50.index].items[n51.index].items), 1)); it52.next();) {
                p = polys[n50.index].items[n51.index].items[n52.index];
                list_add(m, Projector_call(P, p).Result);
              }
              list_add(a.rings, struct_make<Vector_row>(m));
            }
            list_add(out.areas, a);
          }
        }
      }
    }
  }

  struct extract_areas_kind_of_4Result { bool Result = false; };
  extract_areas_kind_of_4Result extract_areas_kind_of_4(const mvt_Layer& l, const mvt_Feature& f, AreaKind& k) {
    Text c = "";
    c = mvt_cpp::Layer_text(l, f, "class").Result;
    if (logic_or(text_equal(c, "pitch"), text_equal(c, "playground"))) {
      k = AreaKind::pitch;
    } else if (logic_or(text_equal(c, "cemetery"), text_equal(c, "garden"))) {
      k = AreaKind::grass;
    } else {
      return {false};
    }
    return {true};
  }

  void extract_areas_4(const mvt_Tile& tile, const TileKey& key, const GeoProjector& proj, List<Vector_row_row>& polys, MapFeatures& out, Text layer = "") {
    i32 at = 0;
    mvt_Layer l = mvt_Layer{"", 4096, List<Text>{}, List<mvt_Value>{}, List<mvt_Feature>{}};
    Projector P = Projector{0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    bool skip = false;
    AreaKind kind = AreaKind::water;
    MapFeatures_Area a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    List<Vector> m = List<Vector>{};
    Vector p = Vector{0.0, 0.0, 0.0};
    at = mvt_cpp::Tile_layer(tile, layer).Result;
    if (math_compare<"<", i32>(at, 0)) {
      return;
    }
    l = tile.layers[num_cast<i64, i32>(at)];
    n53 = Projector_new(key, proj, num_cast<f64, u32>(l.extent));
    P = n53.Result;
    for (auto it54 = flow_for(n54, 0, math_sub<i64>(list_length<mvt_Feature>(l.features), 1)); it54.next();) {
      skip = false;
      if (logic_not(enum_equal<mvt_GeomType>(l.features[n54.index].type, mvt_GeomType::polygon))) {
        skip = true;
      }
      if (logic_not(skip)) {
        kind = AreaKind::water;
        n55 = extract_areas_kind_of_4(l, l.features[n54.index], kind);
        if (logic_not(n55.Result)) {
          skip = true;
        }
        if (logic_not(skip)) {
          polygons(l.features[n54.index], polys);
          for (auto it56 = flow_for(n56, 0, math_sub<i64>(list_length<Vector_row_row>(polys), 1)); it56.next();) {
            a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
            a.kind = kind;
            for (auto it57 = flow_for(n57, 0, math_sub<i64>(list_length<Vector_row>(polys[n56.index].items), 1)); it57.next();) {
              m = List<Vector>{};
              for (auto it58 = flow_for(n58, 0, math_sub<i64>(list_length<Vector>(polys[n56.index].items[n57.index].items), 1)); it58.next();) {
                p = polys[n56.index].items[n57.index].items[n58.index];
                list_add(m, Projector_call(P, p).Result);
              }
              list_add(a.rings, struct_make<Vector_row>(m));
            }
            list_add(out.areas, a);
          }
        }
      }
    }
  }

  struct FurnishIndex_cellResult { i32 Result = 0; };
  KERN_PURE FurnishIndex_cellResult FurnishIndex_cell(f32 v = 0.0f) {
    return {kern::formula<f32, f32>([](auto v, auto kCell) { return num_cast<i32, f32>(floor((v / kCell))); }, v, kCell)};
  }

  struct FurnishIndex_keyResult { u64 Result = 0; };
  KERN_PURE FurnishIndex_keyResult FurnishIndex_key(i32 x = 0, i32 y = 0) {
    return {kern::formula<i32, i32>([](auto x, auto y) { return ((num_cast<u64, u32>(num_cast<u32, i32>(x)) << 32) | num_cast<u64, u32>(num_cast<u32, i32>(y))); }, x, y)};
  }

  void FurnishIndex_add_seg(FurnishIndex& self, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, f32 half = 0.0f, u32 road = 0, bool paved = false) {
    u32 id = 0;
    f32 m = 0.0f;
    List<u32> t = List<u32>{};
    u64 key = 0;
    List<u32> put = List<u32>{};
    id = num_cast<u32, i64>(list_length<FurnishIndex_Seg>(self.segs));
    list_add(self.segs, struct_make<FurnishIndex_Seg>(a, b, half, road, paved));
    m = kern::formula<f32>([](auto half) { return (half + 3.0f); }, half);
    for (auto it60 = flow_for(n60, num_cast<i64, i32>(FurnishIndex_cell(kern::formula<f32, f32, f32>([](auto x, auto x_2, auto m) { return (fmin(x, x_2) - m); }, vector_break(a).x, vector_break(b).x, m)).Result), num_cast<i64, i32>(FurnishIndex_cell(kern::formula<f32, f32, f32>([](auto x, auto x_2, auto m) { return (fmax(x, x_2) + m); }, vector_break(a).x, vector_break(b).x, m)).Result)); it60.next();) {
      for (auto it61 = flow_for(n61, num_cast<i64, i32>(FurnishIndex_cell(kern::formula<f32, f32, f32>([](auto y, auto y_2, auto m) { return (fmin(y, y_2) - m); }, vector_break(a).y, vector_break(b).y, m)).Result), num_cast<i64, i32>(FurnishIndex_cell(kern::formula<f32, f32, f32>([](auto y, auto y_2, auto m) { return (fmax(y, y_2) + m); }, vector_break(a).y, vector_break(b).y, m)).Result)); it61.next();) {
        key = FurnishIndex_key(num_cast<i32, i64>(n60.index), num_cast<i32, i64>(n61.index)).Result;
        map_add(n62, self.seg_cells, key);
        t = self.seg_cells[n62.index];
        list_add(t, id);
        put = t;
        self.seg_cells[n62.index] = put;
      }
    }
  }

  void FurnishIndex_add_ring(FurnishIndex& self, const List<Vector>& r, bool apron = false, u32 index = 0) {
    Vec4 box = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vector p = Vector{0.0, 0.0, 0.0};
    u32 id = 0;
    List<u32> t = List<u32>{};
    u64 key = 0;
    List<u32> put = List<u32>{};
    if (math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(r)), 3)) {
      return;
    }
    box = engine_math_math::Vec4_new(1e+30f, 1e+30f, -1e+30f, -1e+30f).Result;
    for (auto it63 = flow_for(n63, 0, math_sub<i64>(list_length<Vector>(r), 1)); it63.next();) {
      p = r[n63.index];
      box = engine_math_math::Vec4_new(kern::formula<f32, f32>([](auto x, auto x_2) { return fmin(x, x_2); }, box.x, vector_break(p).x), kern::formula<f32, f32>([](auto y, auto y_2) { return fmin(y, y_2); }, box.y, vector_break(p).y), kern::formula<f32, f32>([](auto z, auto x) { return fmax(z, x); }, box.z, vector_break(p).x), kern::formula<f32, f32>([](auto w, auto y) { return fmax(w, y); }, box.w, vector_break(p).y)).Result;
    }
    if (logic_or(math_compare<">", f32>(kern::formula<f32, f32>([](auto z, auto x) { return (z - x); }, box.z, box.x), 4000.0f), math_compare<">", f32>(kern::formula<f32, f32>([](auto w, auto y) { return (w - y); }, box.w, box.y), 4000.0f))) {
      return;
    }
    id = num_cast<u32, i64>(list_length<FurnishIndex_Ring>(self.rings));
    list_add(self.rings, struct_make<FurnishIndex_Ring>(apron, index, box));
    for (auto it64 = flow_for(n64, num_cast<i64, i32>(FurnishIndex_cell(box.x).Result), num_cast<i64, i32>(FurnishIndex_cell(box.z).Result)); it64.next();) {
      for (auto it65 = flow_for(n65, num_cast<i64, i32>(FurnishIndex_cell(box.y).Result), num_cast<i64, i32>(FurnishIndex_cell(box.w).Result)); it65.next();) {
        key = FurnishIndex_key(num_cast<i32, i64>(n64.index), num_cast<i32, i64>(n65.index)).Result;
        map_add(n66, self.ring_cells, key);
        t = self.ring_cells[n66.index];
        list_add(t, id);
        put = t;
        self.ring_cells[n66.index] = put;
      }
    }
  }

  struct furnish_insideResult { bool Result = false; };
  KERN_PURE furnish_insideResult furnish_inside(const Vec4& sq, Vector p = Vector{0.0, 0.0, 0.0}) {
    return {logic_and(logic_and(logic_and(math_compare<">=", f32>(vector_break(p).x, sq.x), math_compare<"<", f32>(vector_break(p).x, sq.z)), math_compare<">=", f32>(vector_break(p).y, sq.y)), math_compare<"<", f32>(vector_break(p).y, sq.w))};
  }

  struct FurnishIndex_builtResult { bool Result = false; };
  /// Inside a house (of the tile's `buildings` or the `apron` the rings were added from).
  KERN_PURE FurnishIndex_builtResult FurnishIndex_built(const FurnishIndex& self, const List<MapFeatures_Building>& buildings, const List<Vector_row>& apron, Vector p = Vector{0.0, 0.0, 0.0}) {
    i64 it = 0;
    i64 at = 0;
    bool skip = false;
    u32 id = 0;
    List<Vector> r = List<Vector>{};
    bool c = false;
    u64 i = 0;
    u64 j = 0;
    u64 was = 0;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<u32>>(self.ring_cells, FurnishIndex_key(FurnishIndex_cell(vector_break(p).x).Result, FurnishIndex_cell(vector_break(p).y).Result).Result).index, map_length<u64, List<u32>>(self.ring_cells));
    if (math_compare<"==", i64>(it, map_length<u64, List<u32>>(self.ring_cells))) {
      return {false};
    }
    at = it;
    for (auto it67 = flow_for(n67, 0, math_sub<i64>(list_length<u32>(self.ring_cells[at]), 1)); it67.next();) {
      id = self.ring_cells[at][n67.index];
      skip = false;
      if (logic_or(logic_or(logic_or(math_compare<"<", f32>(vector_break(p).x, self.rings[num_cast<i64, u32>(id)].box.x), math_compare<">", f32>(vector_break(p).x, self.rings[num_cast<i64, u32>(id)].box.z)), math_compare<"<", f32>(vector_break(p).y, self.rings[num_cast<i64, u32>(id)].box.y)), math_compare<">", f32>(vector_break(p).y, self.rings[num_cast<i64, u32>(id)].box.w))) {
        skip = true;
      }
      if (logic_not(skip)) {
        r = logic_select<List<Vector>>(self.rings[num_cast<i64, u32>(id)].apron, apron[num_cast<i64, u32>(self.rings[num_cast<i64, u32>(id)].index)].items, buildings[num_cast<i64, u32>(self.rings[num_cast<i64, u32>(id)].index)].rings[0].items);
        c = false;
        i = 0;
        j = kern::formula<u64>([](auto a) { return (a - 1); }, num_cast<u64, i64>(list_length<Vector>(r)));
        while (loop(math_compare<"<", u64>(i, num_cast<u64, i64>(list_length<Vector>(r))))) {
          if (logic_and(math_compare<"!=", i32>(num_cast<i32, bool>(math_compare<">", f32>(vector_break(r[num_cast<i64, u64>(i)]).y, vector_break(p).y)), num_cast<i32, bool>(math_compare<">", f32>(vector_break(r[num_cast<i64, u64>(j)]).y, vector_break(p).y))), math_compare<"<", f32>(vector_break(p).x, kern::formula<f32, f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2, auto y_3) { return ((((x - x_2) * (y - y_2)) / ((y_3 - y_2) + 1e-12f)) + x_2); }, vector_break(r[num_cast<i64, u64>(j)]).x, vector_break(r[num_cast<i64, u64>(i)]).x, vector_break(p).y, vector_break(r[num_cast<i64, u64>(i)]).y, vector_break(r[num_cast<i64, u64>(j)]).y)))) {
            c = logic_not(c);
          }
          was = i;
          i += 1;
          j = was;
        }
        if (c) {
          return {true};
        }
      }
    }
    return {false};
  }

  void training_level_road(MapFeatures& out, f32 x = 0.0f, f32 y = 0.0f, f32 w = 0.0f, f32 h = 0.0f, RoadClass c = RoadClass::motorway) {
    MapFeatures_Road r = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    r = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    r.cls = c;
    if (math_compare<">=", f32>(w, h)) {
      r.points = list_make<Vector, "2">(vector_make(kern::formula<f32, f32>([](auto x, auto w) { return (x - (w / 2.0f)); }, x, w), y, 0.0f), vector_make(kern::formula<f32, f32>([](auto x, auto w) { return (x + (w / 2.0f)); }, x, w), y, 0.0f));
    } else {
      r.points = list_make<Vector, "2">(vector_make(x, kern::formula<f32, f32>([](auto y, auto h) { return (y - (h / 2.0f)); }, y, h), 0.0f), vector_make(x, kern::formula<f32, f32>([](auto y, auto h) { return (y + (h / 2.0f)); }, y, h), 0.0f));
    }
    list_add(out.roads, r);
  }

  struct training_level_rectResult { List<Vector> Result = List<Vector>{}; };
  KERN_PURE training_level_rectResult training_level_rect(f32 x = 0.0f, f32 y = 0.0f, f32 w = 0.0f, f32 h = 0.0f) {
    return {list_make<Vector, "4">(vector_make(kern::formula<f32, f32>([](auto x, auto w) { return (x - (w / 2.0f)); }, x, w), kern::formula<f32, f32>([](auto y, auto h) { return (y - (h / 2.0f)); }, y, h), 0.0f), vector_make(kern::formula<f32, f32>([](auto x, auto w) { return (x + (w / 2.0f)); }, x, w), kern::formula<f32, f32>([](auto y, auto h) { return (y - (h / 2.0f)); }, y, h), 0.0f), vector_make(kern::formula<f32, f32>([](auto x, auto w) { return (x + (w / 2.0f)); }, x, w), kern::formula<f32, f32>([](auto y, auto h) { return (y + (h / 2.0f)); }, y, h), 0.0f), vector_make(kern::formula<f32, f32>([](auto x, auto w) { return (x - (w / 2.0f)); }, x, w), kern::formula<f32, f32>([](auto y, auto h) { return (y + (h / 2.0f)); }, y, h), 0.0f))};
  }

  void procedural_city(MapFeatures& out, f64 cx = 0.0, f64 cy = 0.0, f64 size = 0.0, u32 seed = 0) {
    Rng rng = Rng{2654435769, 0.0f, false};
    f32 pitch = 0.0f;
    f32 street = 0.0f;
    i32 blocks = 0;
    f32 half = 0.0f;
    f32 x0 = 0.0f;
    f32 y0 = 0.0f;
    f32 t = 0.0f;
    MapFeatures_Road a = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    MapFeatures_Road b = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    bool ring = false;
    bool skip = false;
    f32 a0 = 0.0f;
    f32 b0 = 0.0f;
    f32 inner = 0.0f;
    bool market = false;
    bool park = false;
    MapFeatures_Area a_2 = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    f32 depth = 0.0f;
    f32 along = 0.0f;
    bool skip_2 = false;
    f32 w = 0.0f;
    f32 ax = 0.0f;
    f32 ay = 0.0f;
    f32 bx2 = 0.0f;
    f32 by2 = 0.0f;
    i32 selected = 0;
    MapFeatures_Building b_2 = MapFeatures_Building{List<Vector_row>{}, 0.0f, 8.0f, 0, 0.0f, false};
    MapFeatures_Area lake = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    f32 ly = 0.0f;
    rng = common::Rng_seeded(seed).Result;
    out.procedural = true;
    pitch = 76.0f;
    street = 12.0f;
    blocks = kern::formula<f64, f32>([](auto size, auto pitch) { return max(3, num_cast<i32, f64>((size / num_cast<f64, f32>(pitch)))); }, size, pitch);
    half = kern::formula<i32, f32>([](auto blocks, auto pitch) { return ((num_cast<f32, i32>(blocks) * pitch) * 0.5f); }, blocks, pitch);
    x0 = kern::formula<f64, f32>([](auto cx, auto half) { return (num_cast<f32, f64>(cx) - half); }, cx, half);
    y0 = kern::formula<f64, f32>([](auto cy, auto half) { return (num_cast<f32, f64>(cy) - half); }, cy, half);
    for (auto it71 = flow_for(n71, 0, num_cast<i64, i32>(blocks)); it71.next();) {
      t = kern::formula<f32, i64, f32>([](auto half, auto index, auto pitch) { return ((-half) + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * pitch)); }, half, n71.index, pitch);
      a = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
      b = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
      ring = logic_or(math_compare<"==", i32>(num_cast<i32, i64>(n71.index), 0), math_compare<"==", i32>(num_cast<i32, i64>(n71.index), blocks));
      b.cls = logic_select<RoadClass>(ring, RoadClass::secondary, logic_select<RoadClass>(math_compare<"==", i32>(num_cast<i32, i64>(n71.index), kern::formula<i32>([](auto blocks) { return (blocks / 2); }, blocks)), RoadClass::tertiary, RoadClass::minor));
      a.cls = b.cls;
      a.points = list_make<Vector, "2">(vector_make(kern::formula<f64, f32>([](auto cx, auto t) { return (num_cast<f32, f64>(cx) + t); }, cx, t), kern::formula<f64, f32>([](auto cy, auto half) { return (num_cast<f32, f64>(cy) - half); }, cy, half), 0.0f), vector_make(kern::formula<f64, f32>([](auto cx, auto t) { return (num_cast<f32, f64>(cx) + t); }, cx, t), kern::formula<f64, f32>([](auto cy, auto half) { return (num_cast<f32, f64>(cy) + half); }, cy, half), 0.0f));
      b.points = list_make<Vector, "2">(vector_make(kern::formula<f64, f32>([](auto cx, auto half) { return (num_cast<f32, f64>(cx) - half); }, cx, half), kern::formula<f64, f32>([](auto cy, auto t) { return (num_cast<f32, f64>(cy) + t); }, cy, t), 0.0f), vector_make(kern::formula<f64, f32>([](auto cx, auto half) { return (num_cast<f32, f64>(cx) + half); }, cx, half), kern::formula<f64, f32>([](auto cy, auto t) { return (num_cast<f32, f64>(cy) + t); }, cy, t), 0.0f));
      list_add(out.roads, a);
      list_add(out.roads, b);
    }
    for (auto it72 = flow_for(n72, 0, math_sub<i64>(num_cast<i64, i32>(blocks), 1)); it72.next();) {
      for (auto it73 = flow_for(n73, 0, math_sub<i64>(num_cast<i64, i32>(blocks), 1)); it73.next();) {
        skip = false;
        a0 = kern::formula<f32, i64, f32, f32>([](auto x0, auto index, auto pitch, auto street) { return (((x0 + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * pitch)) + (street * 0.5f)) + 2.0f); }, x0, n72.index, pitch, street);
        b0 = kern::formula<f32, i64, f32, f32>([](auto y0, auto index, auto pitch, auto street) { return (((y0 + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * pitch)) + (street * 0.5f)) + 2.0f); }, y0, n73.index, pitch, street);
        inner = kern::formula<f32, f32>([](auto pitch, auto street) { return ((pitch - street) - 4.0f); }, pitch, street);
        market = logic_and(math_compare<"==", i32>(num_cast<i32, i64>(n72.index), kern::formula<i32>([](auto blocks) { return (blocks / 2); }, blocks)), math_compare<"==", i32>(num_cast<i32, i64>(n73.index), kern::formula<i32>([](auto blocks) { return (blocks / 2); }, blocks)));
        park = logic_and(math_compare<"==", i32>(num_cast<i32, i64>(n72.index), 1), math_compare<"==", i32>(num_cast<i32, i64>(n73.index), kern::formula<i32>([](auto blocks) { return (blocks - 2); }, blocks)));
        if (logic_or(market, park)) {
          a_2 = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
          a_2.kind = logic_select<AreaKind>(market, AreaKind::sand, AreaKind::park);
          list_add(a_2.rings, struct_make<Vector_row>(list_make<Vector, "4">(vector_make(a0, b0, 0.0f), vector_make(kern::formula<f32, f32>([](auto a0, auto inner) { return (a0 + inner); }, a0, inner), b0, 0.0f), vector_make(kern::formula<f32, f32>([](auto a0, auto inner) { return (a0 + inner); }, a0, inner), kern::formula<f32, f32>([](auto b0, auto inner) { return (b0 + inner); }, b0, inner), 0.0f), vector_make(a0, kern::formula<f32, f32>([](auto b0, auto inner) { return (b0 + inner); }, b0, inner), 0.0f))));
          list_add(out.areas, a_2);
          skip = true;
        }
        if (logic_not(skip)) {
          n74 = common::Rng_range(rng, 10.0f, 14.0f);
          depth = n74.Result;
          for (auto it75 = flow_for(n75, 0, 3); it75.next();) {
            along = 0.0f;
            while (loop(math_compare<"<", f32>(along, kern::formula<f32>([](auto inner) { return (inner - 4.0f); }, inner)))) {
              skip_2 = false;
              n76 = common::Rng_range(rng, 7.0f, 13.0f);
              w = kern::formula<f32, f32, f32>([](auto the_result, auto inner, auto along) { return fmin(the_result, (inner - along)); }, n76.Result, inner, along);
              ax = 0.0f;
              ay = 0.0f;
              bx2 = 0.0f;
              by2 = 0.0f;
              selected = num_cast<i32, i64>(n75.index);
              if (math_compare<"==", i32>(selected, 0)) {
                ax = kern::formula<f32, f32>([](auto a0, auto along) { return (a0 + along); }, a0, along);
                ay = b0;
                bx2 = kern::formula<f32, f32>([](auto ax, auto w) { return (ax + w); }, ax, w);
                by2 = kern::formula<f32, f32>([](auto b0, auto depth) { return (b0 + depth); }, b0, depth);
              } else if (math_compare<"==", i32>(selected, 1)) {
                ax = kern::formula<f32, f32, f32>([](auto a0, auto inner, auto depth) { return ((a0 + inner) - depth); }, a0, inner, depth);
                ay = kern::formula<f32, f32>([](auto b0, auto along) { return (b0 + along); }, b0, along);
                bx2 = kern::formula<f32, f32>([](auto a0, auto inner) { return (a0 + inner); }, a0, inner);
                by2 = kern::formula<f32, f32>([](auto ay, auto w) { return (ay + w); }, ay, w);
              } else if (math_compare<"==", i32>(selected, 2)) {
                ax = kern::formula<f32, f32, f32, f32>([](auto a0, auto inner, auto along, auto w) { return (((a0 + inner) - along) - w); }, a0, inner, along, w);
                ay = kern::formula<f32, f32, f32>([](auto b0, auto inner, auto depth) { return ((b0 + inner) - depth); }, b0, inner, depth);
                bx2 = kern::formula<f32, f32>([](auto ax, auto w) { return (ax + w); }, ax, w);
                by2 = kern::formula<f32, f32>([](auto b0, auto inner) { return (b0 + inner); }, b0, inner);
              } else {
                ax = a0;
                ay = kern::formula<f32, f32, f32, f32>([](auto b0, auto inner, auto along, auto w) { return (((b0 + inner) - along) - w); }, b0, inner, along, w);
                bx2 = kern::formula<f32, f32>([](auto a0, auto depth) { return (a0 + depth); }, a0, depth);
                by2 = kern::formula<f32, f32>([](auto ay, auto w) { return (ay + w); }, ay, w);
              }
              if (logic_and(logic_or(math_compare<"==", i32>(num_cast<i32, i64>(n75.index), 1), math_compare<"==", i32>(num_cast<i32, i64>(n75.index), 3)), logic_or(math_compare<"<", f32>(ay, kern::formula<f32, f32>([](auto b0, auto depth) { return (b0 + depth); }, b0, depth)), math_compare<">", f32>(by2, kern::formula<f32, f32, f32>([](auto b0, auto inner, auto depth) { return ((b0 + inner) - depth); }, b0, inner, depth))))) {
                along += w;
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                b_2 = MapFeatures_Building{List<Vector_row>{}, 0.0f, 8.0f, 0, 0.0f, false};
                list_add(b_2.rings, struct_make<Vector_row>(list_make<Vector, "4">(vector_make(ax, ay, 0.0f), vector_make(bx2, ay, 0.0f), vector_make(bx2, by2, 0.0f), vector_make(ax, by2, 0.0f))));
                n77 = common::Rng_unit(rng);
                n78 = common::Rng_range(rng, 0.0f, 1.5f);
                b_2.top = kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return ((3.2f * num_cast<f32, i32>((3 + num_cast<i32, f32>((the_result * 4.0f))))) + the_result_2); }, n77.Result, n78.Result);
                n79 = common::Rng_next(rng);
                b_2.seed = n79.Result;
                list_add(out.buildings, b_2);
                along += w;
              }
            }
          }
        }
      }
    }
    lake = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    lake.kind = AreaKind::water;
    ly = kern::formula<f64, f32>([](auto cy, auto half) { return ((num_cast<f32, f64>(cy) + half) + 30.0f); }, cy, half);
    list_add(lake.rings, struct_make<Vector_row>(list_make<Vector, "4">(vector_make(kern::formula<f64, f32>([](auto cx, auto half) { return (num_cast<f32, f64>(cx) - (half * 2.0f)); }, cx, half), ly, 0.0f), vector_make(kern::formula<f64, f32>([](auto cx, auto half) { return (num_cast<f32, f64>(cx) + (half * 2.0f)); }, cx, half), ly, 0.0f), vector_make(kern::formula<f64, f32>([](auto cx, auto half) { return (num_cast<f32, f64>(cx) + (half * 2.0f)); }, cx, half), kern::formula<f32>([](auto ly) { return (ly + 900.0f); }, ly), 0.0f), vector_make(kern::formula<f64, f32>([](auto cx, auto half) { return (num_cast<f32, f64>(cx) - (half * 2.0f)); }, cx, half), kern::formula<f32>([](auto ly) { return (ly + 900.0f); }, ly), 0.0f))));
    list_add(out.areas, lake);
  }

  void clip_line_close(List<Vector>& cur, List<Vector_row>& out) {
    if (math_compare<">=", u64>(num_cast<u64, i64>(list_length<Vector>(cur)), 2)) {
      list_add(out, struct_make<Vector_row>(cur));
    }
    list_clear(cur);
  }

  struct clip_lineResult { List<Vector_row> Result = List<Vector_row>{}; };
  clip_lineResult clip_line(const List<Vector>& line, const Vec4& rect) {
    List<Vector_row> out = List<Vector_row>{};
    List<Vector> cur = List<Vector>{};
    u64 i = 0;
    bool skip = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    f32 dx = 0.0f;
    f32 dy = 0.0f;
    f32 t0 = 0.0f;
    f32 t1 = 0.0f;
    bool in = false;
    Fixed<f32, 4> p = Fixed<f32, 4>{};
    Fixed<f32, 4> q = Fixed<f32, 4>{};
    i32 k = 0;
    bool skip_2 = false;
    f32 r = 0.0f;
    Vector ca = Vector{0.0, 0.0, 0.0};
    Vector cb = Vector{0.0, 0.0, 0.0};
    out = List<Vector_row>{};
    cur = List<Vector>{};
    i = 0;
    while (loop(math_compare<"<", u64>(kern::formula<u64>([](auto i) { return (i + 1); }, i), num_cast<u64, i64>(list_length<Vector>(line))))) {
      skip = false;
      a = line[num_cast<i64, u64>(i)];
      b = line[num_cast<i64, u64>(kern::formula<u64>([](auto i) { return (i + 1); }, i))];
      dx = kern::formula<f32, f32>([](auto x, auto x_2) { return (x - x_2); }, vector_break(b).x, vector_break(a).x);
      dy = kern::formula<f32, f32>([](auto y, auto y_2) { return (y - y_2); }, vector_break(b).y, vector_break(a).y);
      t0 = 0.0f;
      t1 = 1.0f;
      in = true;
      p = list_make<f32, "4">(kern::formula<f32>([](auto dx) { return (-dx); }, dx), dx, kern::formula<f32>([](auto dy) { return (-dy); }, dy), dy);
      q = list_make<f32, "4">(kern::formula<f32, f32>([](auto x, auto x_2) { return (x - x_2); }, vector_break(a).x, rect.x), kern::formula<f32, f32>([](auto z, auto x) { return (z - x); }, rect.z, vector_break(a).x), kern::formula<f32, f32>([](auto y, auto y_2) { return (y - y_2); }, vector_break(a).y, rect.y), kern::formula<f32, f32>([](auto w, auto y) { return (w - y); }, rect.w, vector_break(a).y));
      k = 0;
      while (loop(logic_and(math_compare<"<", i32>(k, 4), in))) {
        skip_2 = false;
        if (math_compare<"==", f32>(p[num_cast<i64, i32>(k)], 0.0f)) {
          in = math_compare<">=", f32>(q[num_cast<i64, i32>(k)], 0.0f);
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          r = kern::formula<f32, f32>([](auto a, auto a_2) { return (a / a_2); }, q[num_cast<i64, i32>(k)], p[num_cast<i64, i32>(k)]);
          if (math_compare<"<", f32>(p[num_cast<i64, i32>(k)], 0.0f)) {
            if (math_compare<">", f32>(r, t1)) {
              in = false;
            } else if (math_compare<">", f32>(r, t0)) {
              t0 = r;
            }
          } else {
            if (math_compare<"<", f32>(r, t0)) {
              in = false;
            } else if (math_compare<"<", f32>(r, t1)) {
              t1 = r;
            }
          }
        }
        k += 1;
      }
      if (logic_or(logic_not(in), math_compare<"<=", f32>(t1, t0))) {
        clip_line_close(cur, out);
        skip = true;
      }
      if (logic_not(skip)) {
        ca = vector_make(kern::formula<f32, f32, f32>([](auto x, auto dx, auto t0) { return (x + (dx * t0)); }, vector_break(a).x, dx, t0), kern::formula<f32, f32, f32>([](auto y, auto dy, auto t0) { return (y + (dy * t0)); }, vector_break(a).y, dy, t0), 0.0f);
        cb = vector_make(kern::formula<f32, f32, f32>([](auto x, auto dx, auto t1) { return (x + (dx * t1)); }, vector_break(a).x, dx, t1), kern::formula<f32, f32, f32>([](auto y, auto dy, auto t1) { return (y + (dy * t1)); }, vector_break(a).y, dy, t1), 0.0f);
        if (math_compare<"==", i64>(list_length<Vector>(cur), 0)) {
          list_add(cur, ca);
        } else if (math_compare<">", f32>(t0, 0.0f)) {
          clip_line_close(cur, out);
          list_add(cur, ca);
        }
        list_add(cur, cb);
        if (math_compare<"<", f32>(t1, 1.0f)) {
          clip_line_close(cur, out);
        }
      }
      i += 1;
    }
    clip_line_close(cur, out);
    return {out};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
  struct { f64 x = 0.0; f64 y = 0.0; } n12;
  struct { f64 x = 0.0; f64 y = 0.0; } n13;
  struct { f64 x = 0.0; f64 y = 0.0; } n14;
  struct : kern::Node<15> { i64 index = 0; } n15;
  struct { List<Vector> value = List<Vector>{}; } n16;
  struct { List<Vector> value = List<Vector>{}; } n17;
  struct : kern::Node<18> { i64 index = 0; } n18;
  Projector_newResult n35;
  struct : kern::Node<36> { i64 index = 0; } n36;
  extract_areas_kind_ofResult n37;
  struct : kern::Node<38> { i64 index = 0; } n38;
  struct : kern::Node<39> { i64 index = 0; } n39;
  struct : kern::Node<40> { i64 index = 0; } n40;
  Projector_newResult n41;
  struct : kern::Node<42> { i64 index = 0; } n42;
  extract_areas_kind_of_2Result n43;
  struct : kern::Node<44> { i64 index = 0; } n44;
  struct : kern::Node<45> { i64 index = 0; } n45;
  struct : kern::Node<46> { i64 index = 0; } n46;
  Projector_newResult n47;
  struct : kern::Node<48> { i64 index = 0; } n48;
  extract_areas_kind_of_3Result n49;
  struct : kern::Node<50> { i64 index = 0; } n50;
  struct : kern::Node<51> { i64 index = 0; } n51;
  struct : kern::Node<52> { i64 index = 0; } n52;
  Projector_newResult n53;
  struct : kern::Node<54> { i64 index = 0; } n54;
  extract_areas_kind_of_4Result n55;
  struct : kern::Node<56> { i64 index = 0; } n56;
  struct : kern::Node<57> { i64 index = 0; } n57;
  struct : kern::Node<58> { i64 index = 0; } n58;
  struct : kern::Node<60> { i64 index = 0; } n60;
  struct : kern::Node<61> { i64 index = 0; } n61;
  struct { bool added = false; i64 index = 0; } n62;
  struct : kern::Node<63> { i64 index = 0; } n63;
  struct : kern::Node<64> { i64 index = 0; } n64;
  struct : kern::Node<65> { i64 index = 0; } n65;
  struct { bool added = false; i64 index = 0; } n66;
  struct : kern::Node<67> { i64 index = 0; } n67;
  struct : kern::Node<71> { i64 index = 0; } n71;
  struct : kern::Node<72> { i64 index = 0; } n72;
  struct : kern::Node<73> { i64 index = 0; } n73;
  common::Rng_rangeResult n74;
  struct : kern::Node<75> { i64 index = 0; } n75;
  common::Rng_rangeResult n76;
  common::Rng_unitResult n77;
  common::Rng_rangeResult n78;
  common::Rng_nextResult n79;
};
