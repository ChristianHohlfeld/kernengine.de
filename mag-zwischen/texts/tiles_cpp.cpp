struct tiles_cpp : virtual kern::Library, virtual common, virtual engine_geo_roads, virtual engine_math_math, virtual mvt_cpp, virtual tiles {
  // ---- types ----
  KERN_STRUCT(FurnishIndex_Seg) { Vector a = Vector{0.0, 0.0, 0.0}; Vector b = Vector{0.0, 0.0, 0.0}; Float half = 0.0; Int road = 0; Bool paved = false; };
  KERN_STRUCT(Projector) { Float x0 = 0.0; Float x1 = 0.0; Float y0 = 0.0; Float ym = 0.0; Float y1 = 0.0; Float inv_extent = 0.0; };
  KERN_STRUCT(FurnishIndex_Ring) { Bool apron = false; Int index = 0; Vec4 box = Vec4{0.0, 0.0, 0.0, 0.0}; };
  KERN_STRUCT(parse_road_M) { Text n = ""; RoadClass c = RoadClass::motorway; };
  KERN_STRUCT(FurnishIndex) { List<FurnishIndex_Seg> segs = List<FurnishIndex_Seg>{}; List<FurnishIndex_Ring> rings = List<FurnishIndex_Ring>{}; Map<Int64, List<Int>> seg_cells = Map<Int64, List<Int>>{}; Map<Int64, List<Int>> ring_cells = Map<Int64, List<Int>>{}; };

  // ---- variables ----
  KERN_CONST Float kPiD = 3.141592653589793;
  KERN_CONST Float kEarthRadius = 6378137.0;
  KERN_CONST Float kRad = 0.017453292519943295;
  KERN_CONST Fixed<parse_road_M, 12> map = List<parse_road_M>{parse_road_M{"motorway", RoadClass::motorway}, parse_road_M{"trunk", RoadClass::trunk}, parse_road_M{"primary", RoadClass::primary}, parse_road_M{"secondary", RoadClass::secondary}, parse_road_M{"tertiary", RoadClass::tertiary}, parse_road_M{"minor", RoadClass::minor}, parse_road_M{"service", RoadClass::service}, parse_road_M{"track", RoadClass::track}, parse_road_M{"path", RoadClass::path}, parse_road_M{"pedestrian", RoadClass::pedestrian}, parse_road_M{"raceway", RoadClass::tertiary}, parse_road_M{"busway", RoadClass::minor}};
  KERN_CONST Float kCell = 20.0;
  KERN_CONST Float kPi = 3.141592653589793;

  // ---- functions ----
  struct tile_atResult { TileKey Result = TileKey{14, 0, 0}; };
  KERN_PURE tile_atResult tile_at(Float lon = 0.0, Float lat = 0.0, Int z = 0) {
    Float n = 0.0;
    Float lr = 0.0;
    TileKey t = TileKey{14, 0, 0};
    n = kern::formula([](Float z) -> Float { return (1.0 * kern::math::pow(2.0, z)); }, kern::to<Float>(z));
    lr = kern::formula([](Float lat, Float kPiD) -> Float { return kern::math::div((lat * kPiD), 180.0); }, lat, kPiD);
    t = TileKey{14, 0, 0};
    t.z = z;
    t.x = kern::to<Int>(kern::formula([](Float lon, Float n) -> Float { return kern::math::trunc(kern::math::floor((kern::math::div((lon + 180.0), 360.0) * n))); }, lon, n));
    t.y = kern::to<Int>(kern::formula([](Float lr, Float kPiD, Float n) -> Float { return kern::math::trunc(kern::math::floor((kern::math::div((1.0 - kern::math::div(kern::math::log(kern::math::tan(lr) + kern::math::sqrt((kern::math::tan(lr) * kern::math::tan(lr)) + 1.0)), kPiD)), 2.0) * n))); }, lr, kPiD, n));
    return {t};
  }

  void tile_lonlat(const TileKey& t, Float& lon, Float& lat, Float fx = 0.0, Float fy = 0.0) {
    Float n = 0.0;
    n = kern::formula([](Float z) -> Float { return (1.0 * kern::math::pow(2.0, z)); }, kern::to<Float>(t.z));
    lon = kern::formula([](Float x, Float fx, Float n) -> Float { return ((kern::math::div((x + fx), n) * 360.0) - 180.0); }, kern::to<Float>(t.x), fx, n);
    lat = kern::formula([](Float kPiD, Float y, Float fy, Float n) -> Float { return kern::math::div((kern::math::atan(((kern::math::exp((kPiD * (1.0 - kern::math::div((2.0 * (y + fy)), n)))) - kern::math::exp(-(kPiD * (1.0 - kern::math::div((2.0 * (y + fy)), n))))) * 0.5)) * 180.0), kPiD); }, kPiD, kern::to<Float>(t.y), fy, n);
  }

  void unproject(const GeoProjector& p, Float& lon, Float& lat, Float x = 0.0, Float y = 0.0) {
    lon = kern::formula([](Float lon0, Float x, Float kEarthRadius, Float cos_lat, Float kRad) -> Float { return (lon0 + kern::math::div(kern::math::div(x, (kEarthRadius * cos_lat)), kRad)); }, p.lon0, x, kEarthRadius, p.cos_lat, kRad);
    lat = kern::formula([](Float lat0, Float y, Float kEarthRadius, Float kRad) -> Float { return (lat0 + kern::math::div(kern::math::div(y, kEarthRadius), kRad)); }, p.lat0, y, kEarthRadius, kRad);
  }

  struct road_widthResult { Float Result = 0.0; };
  KERN_PURE road_widthResult road_width(RoadClass c = RoadClass::motorway) {
    switch (flow_switch_enum<RoadClass>(c)) {
      case RoadClass::motorway: {
        return {14.0};
      } break;
      case RoadClass::trunk: {
        return {12.0};
      } break;
      case RoadClass::primary: {
        return {11.0};
      } break;
      case RoadClass::secondary: {
        return {9.0};
      } break;
      case RoadClass::tertiary: {
        return {8.0};
      } break;
      case RoadClass::minor: {
        return {6.5};
      } break;
      case RoadClass::service: {
        return {4.2};
      } break;
      case RoadClass::track: {
        return {3.0};
      } break;
      case RoadClass::path: {
        return {2.2};
      } break;
      case RoadClass::pedestrian: {
        return {3.6};
      } break;
      case RoadClass::river: {
        return {18.0};
      } break;
      case RoadClass::canal: {
        return {12.0};
      } break;
      case RoadClass::stream: {
        return {4.5};
      } break;
    }
  }

  struct road_is_waterResult { Bool Result = false; };
  KERN_PURE road_is_waterResult road_is_water(RoadClass c = RoadClass::motorway) {
    return {logic_or(logic_or(enum_equal<RoadClass>(c, RoadClass::river), enum_equal<RoadClass>(c, RoadClass::canal)), enum_equal<RoadClass>(c, RoadClass::stream))};
  }

  struct road_drivableResult { Bool Result = false; };
  KERN_PURE road_drivableResult road_drivable(RoadClass c = RoadClass::motorway) {
    return {math_compare<"<=">(kern::to<Float>(enum_index<RoadClass>(c)), kern::to<Float>(enum_index<RoadClass>(RoadClass::service)))};
  }

  struct parse_roadResult { Bool Result = false; };
  parse_roadResult parse_road(RoadClass& out, Text cls = "") {
    List<parse_road_M> list = List<parse_road_M>{};
    list = map;
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<parse_road_M>(list))))); it10.next();) {
      if (text_equal(cls, list[n10.index].n)) {
        out = list[n10.index].c;
        return {true};
      }
    }
    return {false};
  }

  /// Groups MVT polygon parts into polygons (exterior ring: positive area in tile space, y down).
  void polygons(const mvt_Feature& f, List<Vector_row_row>& out) {
    Bool skip = false;
    Float a = 0.0;
    list_clear(out);
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(f.parts))))); it11.next();) {
      skip = false;
      if (math_compare<"<">(kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(f.parts[n11.index]).items)), 3.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        a = mvt_cpp::ring_area(f.parts[n11.index].items).Result;
        if (logic_or(math_compare<">">(a, 0.0), math_compare<"==">(kern::to<Float>(list_length<Vector_row_row>(out)), 0.0))) {
          list_add(out, Vector_row_row{List<Vector_row>{}});
        }
        list_add(out[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row_row>(out))))].items, f.parts[n11.index]);
      }
    }
  }

  struct Projector_newResult { Projector Result = Projector{0.0, 0.0, 0.0, 0.0, 0.0, 0.0}; };
  Projector_newResult Projector_new(const TileKey& key, const GeoProjector& proj, Float extent = 0.0) {
    Projector self = Projector{0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    Float lon = 0.0;
    Float lat = 0.0;
    Float x = 0.0;
    self.inv_extent = kern::formula([](Float extent) -> Float { return kern::math::div(1.0, extent); }, extent);
    lon = 0.0;
    lat = 0.0;
    x = 0.0;
    tile_lonlat(key, lon, lat, 0.0, 0.0);
    engine_geo_roads::GeoProjector_project(proj, x, self.y0, lon, lat);
    self.x0 = x;
    tile_lonlat(key, lon, lat, 1.0, 0.5);
    engine_geo_roads::GeoProjector_project(proj, x, self.ym, lon, lat);
    self.x1 = x;
    tile_lonlat(key, lon, lat, 1.0, 1.0);
    engine_geo_roads::GeoProjector_project(proj, x, self.y1, lon, lat);
    return {self};
  }

  struct Projector_callResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Projector_callResult Projector_call(const Projector& self, Vector t = Vector{0.0, 0.0, 0.0}) {
    Float fx = 0.0;
    Float fy = 0.0;
    Float y = 0.0;
    fx = kern::formula([](Float x, Float inv_extent) -> Float { return (x * inv_extent); }, vector_break(t).x, self.inv_extent);
    fy = kern::formula([](Float y, Float inv_extent) -> Float { return (y * inv_extent); }, vector_break(t).y, self.inv_extent);
    y = kern::formula([](Float y0, Float fy, Float ym, Float y1) -> Float { return ((((y0 * ((2.0 * fy) - 1.0)) * (fy - 1.0)) + (((ym * 4.0) * fy) * (1.0 - fy))) + ((y1 * fy) * ((2.0 * fy) - 1.0))); }, self.y0, fy, self.ym, self.y1);
    return {vector_make(kern::formula([](Float x0, Float fx, Float x1) -> Float { return (x0 + (fx * (x1 - x0))); }, self.x0, fx, self.x1), y, 0.0)};
  }

  struct inside_sideResult { Bool Result = false; };
  /// The four sides of the square, in the order a ring is clipped to them: x >= 0, x <= ext, y >= 0, y <= ext.
  KERN_PURE inside_sideResult inside_side(Int side = 0, Vector p = Vector{0.0, 0.0, 0.0}, Float ext = 0.0) {
    return {logic_select<Bool>(math_compare<"==">(kern::to<Float>(side), 0.0), math_compare<">=">(vector_break(p).x, 0.0), logic_select<Bool>(math_compare<"==">(kern::to<Float>(side), 1.0), math_compare<"<=">(vector_break(p).x, ext), logic_select<Bool>(math_compare<"==">(kern::to<Float>(side), 2.0), math_compare<">=">(vector_break(p).y, 0.0), math_compare<"<=">(vector_break(p).y, ext))))};
  }

  struct cut_sideResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  /// Where the edge p..q crosses the side's line.
  KERN_PURE cut_sideResult cut_side(Int side = 0, Vector p = Vector{0.0, 0.0, 0.0}, Vector q = Vector{0.0, 0.0, 0.0}, Float ext = 0.0) {
    Float x = 0.0;
    Float t = 0.0;
    Float y = 0.0;
    Float t_2 = 0.0;
    if (math_compare<"<">(kern::to<Float>(side), 2.0)) {
      x = kern::formula([](Float side, Float ext) -> Float { return ((side == 0.0) ? 0.0 : ext); }, kern::to<Float>(side), ext);
      t = kern::formula([](Float x, Float x_2, Float x_3) -> Float { return kern::math::div((x - x_2), (x_3 - x_2)); }, x, vector_break(p).x, vector_break(q).x);
      return {vector_make(x, kern::formula([](Float y, Float y_2, Float t) -> Float { return (y + ((y_2 - y) * t)); }, vector_break(p).y, vector_break(q).y, t), 0.0)};
    }
    y = kern::formula([](Float side, Float ext) -> Float { return ((side == 2.0) ? 0.0 : ext); }, kern::to<Float>(side), ext);
    t_2 = kern::formula([](Float y, Float y_2, Float y_3) -> Float { return kern::math::div((y - y_2), (y_3 - y_2)); }, y, vector_break(p).y, vector_break(q).y);
    return {vector_make(kern::formula([](Float x, Float x_2, Float t_2) -> Float { return (x + ((x_2 - x) * t_2)); }, vector_break(p).x, vector_break(q).x, t_2), y, 0.0)};
  }

  /// One pass of the clip: what of `a` lies inside the side, into `b` - then `a` is that.
  void clip_pass(List<Vector>& a, List<Vector>& b, Int side = 0, Float ext = 0.0) {
    Vector p = Vector{0.0, 0.0, 0.0};
    Vector q = Vector{0.0, 0.0, 0.0};
    Bool the_pi = false;
    Bool qi = false;
    List<Vector> swapped = List<Vector>{};
    list_clear(b);
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(a))))); it12.next();) {
      p = a[n12.index];
      q = a[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n12.index), kern::to<Float>(list_length<Vector>(a))))];
      the_pi = inside_side(side, p, ext).Result;
      qi = inside_side(side, q, ext).Result;
      if (the_pi) {
        list_add(b, p);
      }
      if (math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(the_pi))), kern::to<Float>(value_int(kern::to<Int>(qi))))) {
        list_add(b, cut_side(side, p, q, ext).Result);
      }
    }
    var_take(n13, a);
    swapped = n13.value;
    var_take(n14, b);
    a = n14.value;
    b = swapped;
  }

  struct clip_to_tileResult { List<Vector> Result = List<Vector>{}; };
  clip_to_tileResult clip_to_tile(const List<Vector>& ring, Float ext = 0.0) {
    List<Vector> a = List<Vector>{};
    List<Vector> b = List<Vector>{};
    a = ring;
    b = List<Vector>{};
    for (auto it15 = flow_for(n15, 0, 3); it15.next();) {
      if (math_compare<">=">(kern::to<Float>(list_length<Vector>(a)), 3.0)) {
        clip_pass(a, b, n15.index, ext);
      }
    }
    return {a};
  }

  struct hash_u32Result { Int Result = 0; };
  KERN_PURE hash_u32Result hash_u32(Int64 v = Int64{0x0}) {
    Int64 v_ = Int64{0x0};
    v_ = v;
    v_ = int64_xor(v_, int64_shift_right_unsigned(v_, 33));
    v_ = int64_mul(v_, Int64{0xff51afd7ed558ccd});
    v_ = int64_xor(v_, int64_shift_right_unsigned(v_, 33));
    v_ = int64_mul(v_, Int64{0xc4ceb9fe1a85ec53});
    v_ = int64_xor(v_, int64_shift_right_unsigned(v_, 33));
    return {int64_low(v_)};
  }

  struct FurnishIndex_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE FurnishIndex_keyResult FurnishIndex_key(Int x = 0, Int y = 0) {
    return {int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32(x); }, kern::to<Float>(x)))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::wrap32(y); }, kern::to<Float>(y)))))};
  }

  struct FurnishIndex_cellResult { Int Result = 0; };
  KERN_PURE FurnishIndex_cellResult FurnishIndex_cell(Float v = 0.0) {
    return {kern::to<Int>(kern::formula([](Float v, Float kCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(v, kCell))); }, v, kCell))};
  }

  void FurnishIndex_add_seg(FurnishIndex& self, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Float half = 0.0, Int road = 0, Bool paved = false) {
    Int id = 0;
    Float m = 0.0;
    List<Int> t = List<Int>{};
    Int64 key = Int64{0x0};
    List<Int> put = List<Int>{};
    id = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<FurnishIndex_Seg>(self.segs))));
    list_add(self.segs, struct_make<FurnishIndex_Seg>(a, b, half, road, paved));
    m = kern::formula([](Float half) -> Float { return (half + 3.0); }, half);
    for (auto it32 = flow_for(n32, FurnishIndex_cell(kern::formula([](Float x, Float x_2, Float m) -> Float { return (kern::math::min(x, x_2) - m); }, vector_break(a).x, vector_break(b).x, m)).Result, FurnishIndex_cell(kern::formula([](Float x, Float x_2, Float m) -> Float { return (kern::math::max(x, x_2) + m); }, vector_break(a).x, vector_break(b).x, m)).Result); it32.next();) {
      for (auto it33 = flow_for(n33, FurnishIndex_cell(kern::formula([](Float y, Float y_2, Float m) -> Float { return (kern::math::min(y, y_2) - m); }, vector_break(a).y, vector_break(b).y, m)).Result, FurnishIndex_cell(kern::formula([](Float y, Float y_2, Float m) -> Float { return (kern::math::max(y, y_2) + m); }, vector_break(a).y, vector_break(b).y, m)).Result); it33.next();) {
        key = FurnishIndex_key(n32.index, n33.index).Result;
        map_add(n34, self.seg_cells, key);
        t = self.seg_cells[n34.index];
        list_add(t, id);
        put = t;
        self.seg_cells[n34.index] = put;
      }
    }
  }

  void FurnishIndex_add_ring(FurnishIndex& self, const List<Vector>& r, Bool apron = false, Int index = 0) {
    Vec4 box = Vec4{0.0, 0.0, 0.0, 0.0};
    Vector p = Vector{0.0, 0.0, 0.0};
    Int id = 0;
    List<Int> t = List<Int>{};
    Int64 key = Int64{0x0};
    List<Int> put = List<Int>{};
    if (math_compare<"<">(kern::to<Float>(list_length<Vector>(r)), 3.0)) {
      return;
    }
    box = engine_math_math::Vec4_new(1e+30, 1e+30, -1e+30, -1e+30).Result;
    for (auto it35 = flow_for(n35, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(r))))); it35.next();) {
      p = r[n35.index];
      box = engine_math_math::Vec4_new(kern::formula([](Float x, Float x_2) -> Float { return kern::math::min(x, x_2); }, box.x, vector_break(p).x), kern::formula([](Float y, Float y_2) -> Float { return kern::math::min(y, y_2); }, box.y, vector_break(p).y), kern::formula([](Float z, Float x) -> Float { return kern::math::max(z, x); }, box.z, vector_break(p).x), kern::formula([](Float w, Float y) -> Float { return kern::math::max(w, y); }, box.w, vector_break(p).y)).Result;
    }
    if (logic_or(math_compare<">">(kern::formula([](Float z, Float x) -> Float { return (z - x); }, box.z, box.x), 4000.0), math_compare<">">(kern::formula([](Float w, Float y) -> Float { return (w - y); }, box.w, box.y), 4000.0))) {
      return;
    }
    id = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<FurnishIndex_Ring>(self.rings))));
    list_add(self.rings, struct_make<FurnishIndex_Ring>(apron, index, box));
    for (auto it36 = flow_for(n36, FurnishIndex_cell(box.x).Result, FurnishIndex_cell(box.z).Result); it36.next();) {
      for (auto it37 = flow_for(n37, FurnishIndex_cell(box.y).Result, FurnishIndex_cell(box.w).Result); it37.next();) {
        key = FurnishIndex_key(n36.index, n37.index).Result;
        map_add(n38, self.ring_cells, key);
        t = self.ring_cells[n38.index];
        list_add(t, id);
        put = t;
        self.ring_cells[n38.index] = put;
      }
    }
  }

  struct FurnishIndex_builtResult { Bool Result = false; };
  /// Inside a house (of the tile's `buildings` or the `apron` the rings were added from).
  KERN_PURE FurnishIndex_builtResult FurnishIndex_built(const FurnishIndex& self, const List<MapFeatures_Building>& buildings, const List<Vector_row>& apron, Vector p = Vector{0.0, 0.0, 0.0}) {
    Int it = 0;
    Int at = 0;
    Bool skip = false;
    Int id = 0;
    List<Vector> r = List<Vector>{};
    Bool c = false;
    Int i = 0;
    Int j = 0;
    Int was = 0;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<Int>>(self.ring_cells, FurnishIndex_key(FurnishIndex_cell(vector_break(p).x).Result, FurnishIndex_cell(vector_break(p).y).Result).Result).index), kern::to<Float>(map_length<Int64, List<Int>>(self.ring_cells))));
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(self.ring_cells)))) {
      return {false};
    }
    at = it;
    for (auto it39 = flow_for(n39, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(self.ring_cells[at]))))); it39.next();) {
      id = self.ring_cells[at][n39.index];
      skip = false;
      if (logic_or(logic_or(logic_or(math_compare<"<">(vector_break(p).x, self.rings[id].box.x), math_compare<">">(vector_break(p).x, self.rings[id].box.z)), math_compare<"<">(vector_break(p).y, self.rings[id].box.y)), math_compare<">">(vector_break(p).y, self.rings[id].box.w))) {
        skip = true;
      }
      if (logic_not(skip)) {
        r = logic_select<List<Vector>>(self.rings[id].apron, apron[self.rings[id].index].items, buildings[self.rings[id].index].rings[0].items);
        c = false;
        i = 0;
        j = kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Vector>(r))));
        while (loop(math_compare<">">(kern::formula([](Float i, Float a) -> Float { return ((i < 0.0) != (a < 0.0) ? (a < 0.0) : (i < a)); }, kern::to<Float>(i), kern::to<Float>(list_length<Vector>(r))), 0.5))) {
          if (logic_and(math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(vector_break(r[i]).y, vector_break(p).y)))), kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(vector_break(r[j]).y, vector_break(p).y))))), math_compare<"<">(vector_break(p).x, kern::formula([](Float x, Float x_2, Float y, Float y_2, Float y_3) -> Float { return (kern::math::div(((x - x_2) * (y - y_2)), ((y_3 - y_2) + 1e-12)) + x_2); }, vector_break(r[j]).x, vector_break(r[i]).x, vector_break(p).y, vector_break(r[i]).y, vector_break(r[j]).y)))) {
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

  struct FurnishIndex_off_roadsResult { Bool Result = false; };
  /// Clear of every street by `paved` beyond its carriageway and of every path by `unpaved`.
  KERN_PURE FurnishIndex_off_roadsResult FurnishIndex_off_roads(const FurnishIndex& self, Vector p = Vector{0.0, 0.0, 0.0}, Float paved = 0.0, Float unpaved = 0.0) {
    Int it = 0;
    Int at = 0;
    Int id = 0;
    Vector d = Vector{0.0, 0.0, 0.0};
    Float l2 = 0.0;
    Float t = 0.0;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<Int>>(self.seg_cells, FurnishIndex_key(FurnishIndex_cell(vector_break(p).x).Result, FurnishIndex_cell(vector_break(p).y).Result).Result).index), kern::to<Float>(map_length<Int64, List<Int>>(self.seg_cells))));
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Int>>(self.seg_cells)))) {
      return {true};
    }
    at = it;
    for (auto it40 = flow_for(n40, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(self.seg_cells[at]))))); it40.next();) {
      id = self.seg_cells[at][n40.index];
      d = vector_sub(self.segs[id].b, self.segs[id].a);
      l2 = kern::formula([](Float x, Float y) -> Float { return ((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y);
      t = kern::formula([](Float l2, Float a) -> Float { return ((l2 > 1e-06) ? a : 0.0); }, l2, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3, Float l2) -> Float { return kern::math::div((((x - x_2) * x_3) + ((y - y_2) * y_3)), l2); }, vector_break(p).x, vector_break(self.segs[id].a).x, vector_break(d).x, vector_break(p).y, vector_break(self.segs[id].a).y, vector_break(d).y, l2), 0.0, 1.0));
      if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float x_3, Float t, Float y, Float y_2, Float y_3) -> Float { return kern::math::sqrt((((x - x_2) - (x_3 * t)) * ((x - x_2) - (x_3 * t))) + (((y - y_2) - (y_3 * t)) * ((y - y_2) - (y_3 * t)))); }, vector_break(p).x, vector_break(self.segs[id].a).x, vector_break(d).x, t, vector_break(p).y, vector_break(self.segs[id].a).y, vector_break(d).y), kern::formula([](Float a, Float a_2, Float paved, Float unpaved) -> Float { return (a + ((a_2 != 0.0) ? paved : unpaved)); }, self.segs[id].half, kern::to<Float>(self.segs[id].paved), paved, unpaved))) {
        return {false};
      }
    }
    return {true};
  }

  struct furnish_insideResult { Bool Result = false; };
  KERN_PURE furnish_insideResult furnish_inside(const Vec4& sq, Vector p = Vector{0.0, 0.0, 0.0}) {
    return {logic_and(logic_and(logic_and(math_compare<">=">(vector_break(p).x, sq.x), math_compare<"<">(vector_break(p).x, sq.z)), math_compare<">=">(vector_break(p).y, sq.y)), math_compare<"<">(vector_break(p).y, sq.w))};
  }

  void furnish(MapFeatures& out, const Vec4& sq, const List<Vector_row>& apron) {
    FurnishIndex ix = FurnishIndex{List<FurnishIndex_Seg>{}, List<FurnishIndex_Ring>{}, Map<Int64, List<Int>>{}, Map<Int64, List<Int>>{}};
    Bool skip = false;
    MapFeatures_Road r = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    Bool paved = false;
    Int k = 0;
    Float kPi2 = 0.0;
    Set<Int64> lamp_cells = Set<Int64>{};
    Int ri = 0;
    Bool skip_2 = false;
    Float w = 0.0;
    Float carry = 0.0;
    Int i = 0;
    Int k_2 = 0;
    Bool skip_3 = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector d = Vector{0.0, 0.0, 0.0};
    Float len = 0.0;
    Vector u = Vector{0.0, 0.0, 0.0};
    Vector right = Vector{0.0, 0.0, 0.0};
    Float t = 0.0;
    Bool skip_4 = false;
    Float side = 0.0;
    Float off = 0.0;
    Vector c = Vector{0.0, 0.0, 0.0};
    Bool free = false;
    Int j = 0;
    Float carry_2 = 0.0;
    Float side_2 = 0.0;
    Int k_3 = 0;
    Bool skip_5 = false;
    Vector a_2 = Vector{0.0, 0.0, 0.0};
    Vector d_2 = Vector{0.0, 0.0, 0.0};
    Float len_2 = 0.0;
    Vector u_2 = Vector{0.0, 0.0, 0.0};
    Vector n = Vector{0.0, 0.0, 0.0};
    Float t_2 = 0.0;
    Bool skip_6 = false;
    Vector c_2 = Vector{0.0, 0.0, 0.0};
    Int64 cell = Int64{0x0};
    Int64 key = Int64{0x0};
    list_clear(out.trees);
    list_clear(out.lamps);
    ix = FurnishIndex{List<FurnishIndex_Seg>{}, List<FurnishIndex_Ring>{}, Map<Int64, List<Int>>{}, Map<Int64, List<Int>>{}};
    for (auto it41 = flow_for(n41, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Road>(out.roads))))); it41.next();) {
      skip = false;
      r = out.roads[n41.index];
      if (road_is_water(r.cls).Result) {
        skip = true;
      }
      if (logic_not(skip)) {
        paved = logic_and(logic_not(enum_equal<RoadClass>(r.cls, RoadClass::path)), logic_not(enum_equal<RoadClass>(r.cls, RoadClass::track)));
        k = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(r.points))), 0.5))) {
          FurnishIndex_add_seg(ix, r.points[k], r.points[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))], kern::formula([](Float the_result) -> Float { return kern::math::div(the_result, 2.0); }, road_width(r.cls).Result), n41.index, paved);
          k += 1;
        }
      }
    }
    for (auto it42 = flow_for(n42, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Building>(out.buildings))))); it42.next();) {
      if (logic_not(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(out.buildings[n42.index].rings)), 0.0))) {
        FurnishIndex_add_ring(ix, out.buildings[n42.index].rings[0].items, false, n42.index);
      }
    }
    for (auto it43 = flow_for(n43, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector_row>(apron))))); it43.next();) {
      FurnishIndex_add_ring(ix, apron[n43.index].items, true, n43.index);
    }
    kPi2 = 6.2831853;
    lamp_cells = Set<Int64>{};
    ri = 0;
    while (loop(math_compare<">">(kern::formula([](Float ri, Float a) -> Float { return ((ri < 0.0) != (a < 0.0) ? (a < 0.0) : (ri < a)); }, kern::to<Float>(ri), kern::to<Float>(list_length<MapFeatures_Road>(out.roads))), 0.5))) {
      skip_2 = false;
      if (logic_or(road_is_water(out.roads[ri].cls).Result, math_compare<"<">(kern::to<Float>(list_length<Vector>(out.roads[ri].points)), 2.0))) {
        skip_2 = true;
      }
      if (logic_not(skip_2)) {
        w = road_width(out.roads[ri].cls).Result;
        if (logic_and(logic_and(road_drivable(out.roads[ri].cls).Result, logic_not(out.roads[ri].bridge)), math_compare<"<=">(w, 14.0))) {
          carry = 17.0;
          i = kern::to<Int>(kern::formula([](Float ri) -> Float { return kern::math::mul32(ri, 7.0); }, kern::to<Float>(ri)));
          k_2 = 0;
          while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k_2) -> Float { return (k_2 + 1.0); }, kern::to<Float>(k_2))))), kern::to<Float>(list_length<Vector>(out.roads[ri].points))), 0.5))) {
            skip_3 = false;
            a = out.roads[ri].points[k_2];
            d = vector_sub(out.roads[ri].points[kern::to<Int>(kern::formula([](Float k_2) -> Float { return (k_2 + 1.0); }, kern::to<Float>(k_2)))], a);
            len = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d).x, vector_break(d).y);
            if (math_compare<"<">(len, 1.0)) {
              skip_3 = true;
            }
            if (logic_not(skip_3)) {
              u = vector_scale(d, kern::formula([](Float len) -> Float { return kern::math::div(1.0, len); }, len));
              right = vector_make(vector_break(u).y, kern::formula([](Float x) -> Float { return (-x); }, vector_break(u).x), 0.0);
              t = carry;
              while (loop(math_compare<"<">(t, len))) {
                skip_4 = false;
                side = kern::formula([](Float i) -> Float { return ((kern::math::band(i, 1.0) != 0.0) ? 1.0 : (-1.0)); }, kern::to<Float>(i));
                off = kern::formula([](Float w, Float i) -> Float { return (((w * 0.5) + 2.2) + (kern::math::mod(i, 4.0) * 0.4)); }, w, kern::to<Float>(i));
                c = vector_add(vector_add(a, vector_scale(u, t)), vector_scale(right, kern::formula([](Float side, Float off) -> Float { return (side * off); }, side, off)));
                if (logic_or(logic_or(logic_not(furnish_inside(sq, c).Result), logic_not(FurnishIndex_off_roads(ix, c, 1.9, 0.6).Result)), FurnishIndex_built(ix, out.buildings, apron, c).Result)) {
                  skip_4 = true;
                }
                if (logic_not(skip_4)) {
                  free = true;
                  j = 0;
                  while (loop(logic_and(math_compare<"<">(kern::to<Float>(j), 6.0), free))) {
                    free = logic_not(FurnishIndex_built(ix, out.buildings, apron, vector_add(c, vector_scale(vector_make(kern::formula([](Float j, Float kPi2) -> Float { return kern::math::cos(kern::math::div((j * kPi2), 6.0)); }, kern::to<Float>(j), kPi2), kern::formula([](Float j, Float kPi2) -> Float { return kern::math::sin(kern::math::div((j * kPi2), 6.0)); }, kern::to<Float>(j), kPi2), 0.0), 2.4))).Result);
                    j += 1;
                  }
                  if (free) {
                    list_add(out.trees, struct_make<MapFeatures_Spot>(c, kern::formula([](Float i) -> Float { return (0.82 + (kern::math::mod(i, 5.0) * 0.045)); }, kern::to<Float>(i))));
                  }
                }
                t += 37.0;
                i = kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32(i + 1.0); }, kern::to<Float>(i)));
              }
              carry = kern::formula([](Float carry, Float len) -> Float { return kern::math::mod(((carry + 37.0) - kern::math::mod(len, 37.0)), 37.0); }, carry, len);
            }
            k_2 += 1;
          }
        }
        if (logic_and(logic_not(enum_equal<RoadClass>(out.roads[ri].cls, RoadClass::path)), logic_not(enum_equal<RoadClass>(out.roads[ri].cls, RoadClass::track)))) {
          carry_2 = 17.0;
          side_2 = 1.0;
          k_3 = 0;
          while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k_3) -> Float { return (k_3 + 1.0); }, kern::to<Float>(k_3))))), kern::to<Float>(list_length<Vector>(out.roads[ri].points))), 0.5))) {
            skip_5 = false;
            a_2 = out.roads[ri].points[k_3];
            d_2 = vector_sub(out.roads[ri].points[kern::to<Int>(kern::formula([](Float k_3) -> Float { return (k_3 + 1.0); }, kern::to<Float>(k_3)))], a_2);
            len_2 = kern::formula([](Float x, Float y) -> Float { return kern::math::sqrt((x * x) + (y * y)); }, vector_break(d_2).x, vector_break(d_2).y);
            if (math_compare<"<">(len_2, 0.5)) {
              skip_5 = true;
            }
            if (logic_not(skip_5)) {
              u_2 = vector_scale(d_2, kern::formula([](Float len_2) -> Float { return kern::math::div(1.0, len_2); }, len_2));
              n = vector_make(kern::formula([](Float y) -> Float { return (-y); }, vector_break(u_2).y), vector_break(u_2).x, 0.0);
              t_2 = carry_2;
              while (loop(math_compare<"<">(t_2, len_2))) {
                skip_6 = false;
                side_2 = kern::formula([](Float side_2) -> Float { return (-side_2); }, side_2);
                c_2 = vector_add(vector_add(a_2, vector_scale(u_2, t_2)), vector_scale(n, kern::formula([](Float w, Float side_2) -> Float { return ((kern::math::div(w, 2.0) + 1.15) * side_2); }, w, side_2)));
                if (logic_not(furnish_inside(sq, c_2).Result)) {
                  skip_6 = true;
                }
                if (logic_not(skip_6)) {
                  cell = int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32(kern::math::round(kern::math::div(x, 8.0))); }, vector_break(c_2).x))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float y) -> Float { return kern::math::wrap32(kern::math::round(kern::math::div(y, 8.0))); }, vector_break(c_2).y))));
                  if (logic_or(logic_not(FurnishIndex_off_roads(ix, c_2, 0.35, 0.3).Result), FurnishIndex_built(ix, out.buildings, apron, c_2).Result)) {
                    skip_6 = true;
                  }
                  if (logic_not(skip_6)) {
                    key = cell;
                    set_add(n44, lamp_cells, key);
                    if (logic_not(struct_break<Int_Bool_pair>(struct_make<Int_Bool_pair>(n44.index, n44.added)).second)) {
                      skip_6 = true;
                    }
                    if (logic_not(skip_6)) {
                      list_add(out.lamps, struct_make<MapFeatures_Spot>(c_2, kern::formula([](Float y, Float side_2, Float x) -> Float { return kern::math::atan2(((-y) * side_2), ((-x) * side_2)); }, vector_break(n).y, side_2, vector_break(n).x)));
                    }
                  }
                }
                t_2 += 34.0;
              }
              carry_2 = kern::formula([](Float carry_2, Float len_2) -> Float { return kern::math::mod((kern::math::mod((carry_2 - len_2), 34.0) + 34.0), 34.0); }, carry_2, len_2);
            }
            k_3 += 1;
          }
        }
      }
      ri = kern::to<Int>(kern::formula([](Float ri) -> Float { return kern::math::wrap32(ri + 1.0); }, kern::to<Float>(ri)));
    }
  }

  void training_level_road(MapFeatures& out, Float x = 0.0, Float y = 0.0, Float w = 0.0, Float h = 0.0, RoadClass c = RoadClass::motorway) {
    MapFeatures_Road r = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    r = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    r.cls = c;
    if (math_compare<">=">(w, h)) {
      r.points = list_make<Vector, "2">(vector_make(kern::formula([](Float x, Float w) -> Float { return (x - kern::math::div(w, 2.0)); }, x, w), y, 0.0), vector_make(kern::formula([](Float x, Float w) -> Float { return (x + kern::math::div(w, 2.0)); }, x, w), y, 0.0));
    } else {
      r.points = list_make<Vector, "2">(vector_make(x, kern::formula([](Float y, Float h) -> Float { return (y - kern::math::div(h, 2.0)); }, y, h), 0.0), vector_make(x, kern::formula([](Float y, Float h) -> Float { return (y + kern::math::div(h, 2.0)); }, y, h), 0.0));
    }
    list_add(out.roads, r);
  }

  struct training_level_rectResult { List<Vector> Result = List<Vector>{}; };
  KERN_PURE training_level_rectResult training_level_rect(Float x = 0.0, Float y = 0.0, Float w = 0.0, Float h = 0.0) {
    return {list_make<Vector, "4">(vector_make(kern::formula([](Float x, Float w) -> Float { return (x - kern::math::div(w, 2.0)); }, x, w), kern::formula([](Float y, Float h) -> Float { return (y - kern::math::div(h, 2.0)); }, y, h), 0.0), vector_make(kern::formula([](Float x, Float w) -> Float { return (x + kern::math::div(w, 2.0)); }, x, w), kern::formula([](Float y, Float h) -> Float { return (y - kern::math::div(h, 2.0)); }, y, h), 0.0), vector_make(kern::formula([](Float x, Float w) -> Float { return (x + kern::math::div(w, 2.0)); }, x, w), kern::formula([](Float y, Float h) -> Float { return (y + kern::math::div(h, 2.0)); }, y, h), 0.0), vector_make(kern::formula([](Float x, Float w) -> Float { return (x - kern::math::div(w, 2.0)); }, x, w), kern::formula([](Float y, Float h) -> Float { return (y + kern::math::div(h, 2.0)); }, y, h), 0.0))};
  }

  void training_level(MapFeatures& out) {
    MapFeatures_Road b = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    List<Float_4_row> list = List<Float_4_row>{};
    MapFeatures_Area a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    MapFeatures_Area pond = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    List<Vector> ring = List<Vector>{};
    Float a_2 = 0.0;
    Fixed<Float_5_row, 9> houses = Fixed<Float_5_row, 9>{};
    Int n = 0;
    MapFeatures_Building b_2 = MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false};
    Int was = 0;
    out.procedural = true;
    training_level_road(out, 0.0, -38.0, 124.0, 8.0, RoadClass::secondary);
    training_level_road(out, 0.0, 38.0, 124.0, 8.0, RoadClass::secondary);
    training_level_road(out, -62.0, 0.0, 8.0, 76.0, RoadClass::secondary);
    training_level_road(out, 62.0, 0.0, 8.0, 76.0, RoadClass::secondary);
    training_level_road(out, 0.0, 0.0, 168.0, 11.0, RoadClass::primary);
    training_level_road(out, 0.0, 0.0, 10.0, 152.0, RoadClass::primary);
    training_level_road(out, 0.0, 54.0, 140.0, 8.0, RoadClass::tertiary);
    b = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    b.cls = RoadClass::pedestrian;
    b.bridge = true;
    b.points = list_make<Vector, "2">(Vector{34.0, 10.0, 0.0}, Vector{34.0, 52.0, 0.0});
    list_add(out.roads, b);
    list = list_make<Float_4_row, "2">(struct_make<Float_4_row>(list_make<Float, "4">(-38.0, 28.0, 42.0, 30.0)), struct_make<Float_4_row>(list_make<Float, "4">(48.0, -25.0, 32.0, 22.0)));
    for (auto it45 = flow_for(n45, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_4_row>(list))))); it45.next();) {
      a = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
      a.kind = AreaKind::park;
      list_add(a.rings, struct_make<Vector_row>(training_level_rect(list[n45.index].items[0], list[n45.index].items[1], list[n45.index].items[2], list[n45.index].items[3]).Result));
      list_add(out.areas, a);
    }
    pond = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    pond.kind = AreaKind::water;
    ring = List<Vector>{};
    for (auto it46 = flow_for(n46, 0, 47); it46.next();) {
      a_2 = kern::formula([](Float kPi, Float index) -> Float { return kern::math::div(((2.0 * kPi) * index), 48.0); }, kPi, kern::to<Float>(n46.index));
      list_add(ring, vector_make(kern::formula([](Float a_2) -> Float { return (34.0 + ((kern::math::cos(a_2) * 20.0) * 1.55)); }, a_2), kern::formula([](Float a_2) -> Float { return (31.0 + ((kern::math::sin(a_2) * 20.0) * 0.92)); }, a_2), 0.0));
    }
    list_add(pond.rings, struct_make<Vector_row>(ring));
    list_add(out.areas, pond);
    houses = list_make<Float_5_row, "9">(struct_make<Float_5_row>(list_make<Float, "5">(-47.0, -22.0, 15.0, 13.0, 10.0)), struct_make<Float_5_row>(list_make<Float, "5">(-27.0, -23.0, 13.0, 12.0, 15.0)), struct_make<Float_5_row>(list_make<Float, "5">(31.0, -23.0, 15.0, 13.0, 12.0)), struct_make<Float_5_row>(list_make<Float, "5">(50.0, -22.0, 12.0, 12.0, 18.0)), struct_make<Float_5_row>(list_make<Float, "5">(-51.0, 18.0, 13.0, 12.0, 14.0)), struct_make<Float_5_row>(list_make<Float, "5">(-18.0, 24.0, 14.0, 11.0, 20.0)), struct_make<Float_5_row>(list_make<Float, "5">(54.0, 15.0, 13.0, 12.0, 11.0)), struct_make<Float_5_row>(list_make<Float, "5">(-55.0, 61.0, 16.0, 11.0, 9.0)), struct_make<Float_5_row>(list_make<Float, "5">(55.0, 64.0, 18.0, 12.0, 13.0)));
    n = 0;
    for (auto it47 = flow_for(n47, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float_5_row>(houses))))); it47.next();) {
      b_2 = MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false};
      list_add(b_2.rings, struct_make<Vector_row>(training_level_rect(houses[n47.index].items[0], houses[n47.index].items[1], houses[n47.index].items[2], houses[n47.index].items[3]).Result));
      b_2.base = 0.0;
      b_2.top = houses[n47.index].items[4];
      was = n;
      n = kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::wrap32(n + 1.0); }, kern::to<Float>(n)));
      b_2.seed = kern::to<Int>(kern::formula([](Float was) -> Float { return kern::math::wrap32(500192.0 + kern::math::mul32(was, 2654435761.0)); }, kern::to<Float>(was)));
      list_add(out.buildings, b_2);
    }
  }

  void procedural_city(MapFeatures& out, Float cx = 0.0, Float cy = 0.0, Float size = 0.0, Int seed = 0) {
    Rng rng = Rng{2654435769, 0.0, false};
    Float pitch = 0.0;
    Float street = 0.0;
    Int blocks = 0;
    Float half = 0.0;
    Float x0 = 0.0;
    Float y0 = 0.0;
    Float t = 0.0;
    MapFeatures_Road a = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    MapFeatures_Road b = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
    Bool ring = false;
    Bool skip = false;
    Float a0 = 0.0;
    Float b0 = 0.0;
    Float inner = 0.0;
    Bool market = false;
    Bool park = false;
    MapFeatures_Area a_2 = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    Float depth = 0.0;
    Float along = 0.0;
    Bool skip_2 = false;
    Float w = 0.0;
    Float ax = 0.0;
    Float ay = 0.0;
    Float bx2 = 0.0;
    Float by2 = 0.0;
    Int selected = 0;
    MapFeatures_Building b_2 = MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false};
    MapFeatures_Area lake = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
    Float ly = 0.0;
    rng = common::Rng_seeded(seed).Result;
    out.procedural = true;
    pitch = 76.0;
    street = 12.0;
    blocks = kern::to<Int>(kern::formula([](Float size, Float pitch) -> Float { return kern::math::max(3.0, kern::math::trunc(kern::math::div(size, pitch))); }, size, pitch));
    half = kern::formula([](Float blocks, Float pitch) -> Float { return ((blocks * pitch) * 0.5); }, kern::to<Float>(blocks), pitch);
    x0 = kern::formula([](Float cx, Float half) -> Float { return (cx - half); }, cx, half);
    y0 = kern::formula([](Float cy, Float half) -> Float { return (cy - half); }, cy, half);
    for (auto it48 = flow_for(n48, 0, blocks); it48.next();) {
      t = kern::formula([](Float half, Float index, Float pitch) -> Float { return ((-half) + (index * pitch)); }, half, kern::to<Float>(n48.index), pitch);
      a = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
      b = MapFeatures_Road{List<Vector>{}, RoadClass::minor, false};
      ring = logic_or(math_compare<"==">(kern::to<Float>(n48.index), 0.0), math_compare<"==">(kern::to<Float>(n48.index), kern::to<Float>(blocks)));
      b.cls = logic_select<RoadClass>(ring, RoadClass::secondary, logic_select<RoadClass>(math_compare<"==">(kern::to<Float>(n48.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float blocks) -> Float { return kern::math::trunc(kern::math::div(blocks, 2.0)); }, kern::to<Float>(blocks)))))), RoadClass::tertiary, RoadClass::minor));
      a.cls = b.cls;
      a.points = list_make<Vector, "2">(vector_make(kern::formula([](Float cx, Float t) -> Float { return (cx + t); }, cx, t), kern::formula([](Float cy, Float half) -> Float { return (cy - half); }, cy, half), 0.0), vector_make(kern::formula([](Float cx, Float t) -> Float { return (cx + t); }, cx, t), kern::formula([](Float cy, Float half) -> Float { return (cy + half); }, cy, half), 0.0));
      b.points = list_make<Vector, "2">(vector_make(kern::formula([](Float cx, Float half) -> Float { return (cx - half); }, cx, half), kern::formula([](Float cy, Float t) -> Float { return (cy + t); }, cy, t), 0.0), vector_make(kern::formula([](Float cx, Float half) -> Float { return (cx + half); }, cx, half), kern::formula([](Float cy, Float t) -> Float { return (cy + t); }, cy, t), 0.0));
      list_add(out.roads, a);
      list_add(out.roads, b);
    }
    for (auto it49 = flow_for(n49, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(blocks)))); it49.next();) {
      for (auto it50 = flow_for(n50, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(blocks)))); it50.next();) {
        skip = false;
        a0 = kern::formula([](Float x0, Float index, Float pitch, Float street) -> Float { return (((x0 + (index * pitch)) + (street * 0.5)) + 2.0); }, x0, kern::to<Float>(n49.index), pitch, street);
        b0 = kern::formula([](Float y0, Float index, Float pitch, Float street) -> Float { return (((y0 + (index * pitch)) + (street * 0.5)) + 2.0); }, y0, kern::to<Float>(n50.index), pitch, street);
        inner = kern::formula([](Float pitch, Float street) -> Float { return ((pitch - street) - 4.0); }, pitch, street);
        market = logic_and(math_compare<"==">(kern::to<Float>(n49.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float blocks) -> Float { return kern::math::trunc(kern::math::div(blocks, 2.0)); }, kern::to<Float>(blocks)))))), math_compare<"==">(kern::to<Float>(n50.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float blocks) -> Float { return kern::math::trunc(kern::math::div(blocks, 2.0)); }, kern::to<Float>(blocks)))))));
        park = logic_and(math_compare<"==">(kern::to<Float>(n49.index), 1.0), math_compare<"==">(kern::to<Float>(n50.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float blocks) -> Float { return (blocks - 2.0); }, kern::to<Float>(blocks)))))));
        if (logic_or(market, park)) {
          a_2 = MapFeatures_Area{List<Vector_row>{}, AreaKind::park};
          a_2.kind = logic_select<AreaKind>(market, AreaKind::sand, AreaKind::park);
          list_add(a_2.rings, struct_make<Vector_row>(list_make<Vector, "4">(vector_make(a0, b0, 0.0), vector_make(kern::formula([](Float a0, Float inner) -> Float { return (a0 + inner); }, a0, inner), b0, 0.0), vector_make(kern::formula([](Float a0, Float inner) -> Float { return (a0 + inner); }, a0, inner), kern::formula([](Float b0, Float inner) -> Float { return (b0 + inner); }, b0, inner), 0.0), vector_make(a0, kern::formula([](Float b0, Float inner) -> Float { return (b0 + inner); }, b0, inner), 0.0))));
          list_add(out.areas, a_2);
          skip = true;
        }
        if (logic_not(skip)) {
          n51 = common::Rng_range(rng, 10.0, 14.0);
          depth = n51.Result;
          for (auto it52 = flow_for(n52, 0, 3); it52.next();) {
            along = 0.0;
            while (loop(math_compare<"<">(along, kern::formula([](Float inner) -> Float { return (inner - 4.0); }, inner)))) {
              skip_2 = false;
              n53 = common::Rng_range(rng, 7.0, 13.0);
              w = kern::formula([](Float the_result, Float inner, Float along) -> Float { return kern::math::min(the_result, (inner - along)); }, n53.Result, inner, along);
              ax = 0.0;
              ay = 0.0;
              bx2 = 0.0;
              by2 = 0.0;
              selected = n52.index;
              if (math_compare<"==">(kern::to<Float>(selected), 0.0)) {
                ax = kern::formula([](Float a0, Float along) -> Float { return (a0 + along); }, a0, along);
                ay = b0;
                bx2 = kern::formula([](Float ax, Float w) -> Float { return (ax + w); }, ax, w);
                by2 = kern::formula([](Float b0, Float depth) -> Float { return (b0 + depth); }, b0, depth);
              } else if (math_compare<"==">(kern::to<Float>(selected), 1.0)) {
                ax = kern::formula([](Float a0, Float inner, Float depth) -> Float { return ((a0 + inner) - depth); }, a0, inner, depth);
                ay = kern::formula([](Float b0, Float along) -> Float { return (b0 + along); }, b0, along);
                bx2 = kern::formula([](Float a0, Float inner) -> Float { return (a0 + inner); }, a0, inner);
                by2 = kern::formula([](Float ay, Float w) -> Float { return (ay + w); }, ay, w);
              } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
                ax = kern::formula([](Float a0, Float inner, Float along, Float w) -> Float { return (((a0 + inner) - along) - w); }, a0, inner, along, w);
                ay = kern::formula([](Float b0, Float inner, Float depth) -> Float { return ((b0 + inner) - depth); }, b0, inner, depth);
                bx2 = kern::formula([](Float ax, Float w) -> Float { return (ax + w); }, ax, w);
                by2 = kern::formula([](Float b0, Float inner) -> Float { return (b0 + inner); }, b0, inner);
              } else {
                ax = a0;
                ay = kern::formula([](Float b0, Float inner, Float along, Float w) -> Float { return (((b0 + inner) - along) - w); }, b0, inner, along, w);
                bx2 = kern::formula([](Float a0, Float depth) -> Float { return (a0 + depth); }, a0, depth);
                by2 = kern::formula([](Float ay, Float w) -> Float { return (ay + w); }, ay, w);
              }
              if (logic_and(logic_or(math_compare<"==">(kern::to<Float>(n52.index), 1.0), math_compare<"==">(kern::to<Float>(n52.index), 3.0)), logic_or(math_compare<"<">(ay, kern::formula([](Float b0, Float depth) -> Float { return (b0 + depth); }, b0, depth)), math_compare<">">(by2, kern::formula([](Float b0, Float inner, Float depth) -> Float { return ((b0 + inner) - depth); }, b0, inner, depth))))) {
                along += w;
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                b_2 = MapFeatures_Building{List<Vector_row>{}, 0.0, 8.0, 0, 0.0, false};
                list_add(b_2.rings, struct_make<Vector_row>(list_make<Vector, "4">(vector_make(ax, ay, 0.0), vector_make(bx2, ay, 0.0), vector_make(bx2, by2, 0.0), vector_make(ax, by2, 0.0))));
                n54 = common::Rng_unit(rng);
                n55 = common::Rng_range(rng, 0.0, 1.5);
                b_2.top = kern::formula([](Float the_result, Float the_result_2) -> Float { return ((3.2 * (3.0 + kern::math::trunc((the_result * 4.0)))) + the_result_2); }, n54.Result, n55.Result);
                n56 = common::Rng_next(rng);
                b_2.seed = n56.Result;
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
    ly = kern::formula([](Float cy, Float half) -> Float { return ((cy + half) + 30.0); }, cy, half);
    list_add(lake.rings, struct_make<Vector_row>(list_make<Vector, "4">(vector_make(kern::formula([](Float cx, Float half) -> Float { return (cx - (half * 2.0)); }, cx, half), ly, 0.0), vector_make(kern::formula([](Float cx, Float half) -> Float { return (cx + (half * 2.0)); }, cx, half), ly, 0.0), vector_make(kern::formula([](Float cx, Float half) -> Float { return (cx + (half * 2.0)); }, cx, half), kern::formula([](Float ly) -> Float { return (ly + 900.0); }, ly), 0.0), vector_make(kern::formula([](Float cx, Float half) -> Float { return (cx - (half * 2.0)); }, cx, half), kern::formula([](Float ly) -> Float { return (ly + 900.0); }, ly), 0.0))));
    list_add(out.areas, lake);
  }

  void clip_line_close(List<Vector>& cur, List<Vector_row>& out) {
    if (math_compare<">=">(kern::to<Float>(list_length<Vector>(cur)), 2.0)) {
      list_add(out, struct_make<Vector_row>(cur));
    }
    list_clear(cur);
  }

  struct clip_lineResult { List<Vector_row> Result = List<Vector_row>{}; };
  clip_lineResult clip_line(const List<Vector>& line, const Vec4& rect) {
    List<Vector_row> out = List<Vector_row>{};
    List<Vector> cur = List<Vector>{};
    Int i = 0;
    Bool skip = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Float dx = 0.0;
    Float dy = 0.0;
    Float t0 = 0.0;
    Float t1 = 0.0;
    Bool in = false;
    Fixed<Float, 4> p = Fixed<Float, 4>{};
    Fixed<Float, 4> q = Fixed<Float, 4>{};
    Int k = 0;
    Bool skip_2 = false;
    Float r = 0.0;
    Vector ca = Vector{0.0, 0.0, 0.0};
    Vector cb = Vector{0.0, 0.0, 0.0};
    out = List<Vector_row>{};
    cur = List<Vector>{};
    i = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(line))), 0.5))) {
      skip = false;
      a = line[i];
      b = line[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))];
      dx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(b).x, vector_break(a).x);
      dy = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(b).y, vector_break(a).y);
      t0 = 0.0;
      t1 = 1.0;
      in = true;
      p = list_make<Float, "4">(kern::formula([](Float dx) -> Float { return (-dx); }, dx), dx, kern::formula([](Float dy) -> Float { return (-dy); }, dy), dy);
      q = list_make<Float, "4">(kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(a).x, rect.x), kern::formula([](Float z, Float x) -> Float { return (z - x); }, rect.z, vector_break(a).x), kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(a).y, rect.y), kern::formula([](Float w, Float y) -> Float { return (w - y); }, rect.w, vector_break(a).y));
      k = 0;
      while (loop(logic_and(math_compare<"<">(kern::to<Float>(k), 4.0), in))) {
        skip_2 = false;
        if (math_compare<">">(kern::formula([](Float a) -> Float { return (a == 0.0); }, p[k]), 0.5)) {
          in = math_compare<">=">(q[k], 0.0);
          skip_2 = true;
        }
        if (logic_not(skip_2)) {
          r = kern::formula([](Float a, Float a_2) -> Float { return kern::math::div(a, a_2); }, q[k], p[k]);
          if (math_compare<"<">(p[k], 0.0)) {
            if (math_compare<">">(r, t1)) {
              in = false;
            } else if (math_compare<">">(r, t0)) {
              t0 = r;
            }
          } else {
            if (math_compare<"<">(r, t0)) {
              in = false;
            } else if (math_compare<"<">(r, t1)) {
              t1 = r;
            }
          }
        }
        k += 1;
      }
      if (logic_or(logic_not(in), math_compare<"<=">(t1, t0))) {
        clip_line_close(cur, out);
        skip = true;
      }
      if (logic_not(skip)) {
        ca = vector_make(kern::formula([](Float x, Float dx, Float t0) -> Float { return (x + (dx * t0)); }, vector_break(a).x, dx, t0), kern::formula([](Float y, Float dy, Float t0) -> Float { return (y + (dy * t0)); }, vector_break(a).y, dy, t0), 0.0);
        cb = vector_make(kern::formula([](Float x, Float dx, Float t1) -> Float { return (x + (dx * t1)); }, vector_break(a).x, dx, t1), kern::formula([](Float y, Float dy, Float t1) -> Float { return (y + (dy * t1)); }, vector_break(a).y, dy, t1), 0.0);
        if (math_compare<"==">(kern::to<Float>(list_length<Vector>(cur)), 0.0)) {
          list_add(cur, ca);
        } else if (math_compare<">">(t0, 0.0)) {
          clip_line_close(cur, out);
          list_add(cur, ca);
        }
        list_add(cur, cb);
        if (math_compare<"<">(t1, 1.0)) {
          clip_line_close(cur, out);
        }
      }
      i += 1;
    }
    clip_line_close(cur, out);
    return {out};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct { List<Vector> value = List<Vector>{}; } n13;
  struct { List<Vector> value = List<Vector>{}; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<32> { Int index = 0; } n32;
  struct : kern::Node<33> { Int index = 0; } n33;
  struct { Bool added = false; Int index = 0; } n34;
  struct : kern::Node<35> { Int index = 0; } n35;
  struct : kern::Node<36> { Int index = 0; } n36;
  struct : kern::Node<37> { Int index = 0; } n37;
  struct { Bool added = false; Int index = 0; } n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  struct : kern::Node<40> { Int index = 0; } n40;
  struct : kern::Node<41> { Int index = 0; } n41;
  struct : kern::Node<42> { Int index = 0; } n42;
  struct : kern::Node<43> { Int index = 0; } n43;
  struct { Bool added = false; Int index = 0; } n44;
  struct : kern::Node<45> { Int index = 0; } n45;
  struct : kern::Node<46> { Int index = 0; } n46;
  struct : kern::Node<47> { Int index = 0; } n47;
  struct : kern::Node<48> { Int index = 0; } n48;
  struct : kern::Node<49> { Int index = 0; } n49;
  struct : kern::Node<50> { Int index = 0; } n50;
  common::Rng_rangeResult n51;
  struct : kern::Node<52> { Int index = 0; } n52;
  common::Rng_rangeResult n53;
  common::Rng_unitResult n54;
  common::Rng_rangeResult n55;
  common::Rng_nextResult n56;
};
