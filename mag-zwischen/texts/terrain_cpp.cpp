struct terrain_cpp : virtual kern::Library, virtual engine_geo_mesh_data, virtual engine_geo_roads, virtual engine_math_math, virtual engine_physics_physics, virtual terrain, virtual tiles, virtual tiles_cpp {
  // ---- types ----
  KERN_STRUCT(DemOrigin) { Float lon0 = 0.0; Float lat0 = 0.0; };
  KERN_STRUCT(DemTileHead) { Int x = 0; Int y = 0; Int w = 0; Int h = 0; };
  KERN_STRUCT(DemWindow) { Float x0 = 0.0; Float y0 = 0.0; Float step = 10.0; Int n = 0; Float open = 160.0; Float blur = 30.0; Int count = 0; };
  KERN_STRUCT(WantedTile) { Float d = 0.0; Int64 k = Int64{0x0}; };

  // ---- variables ----
  /// the bare-earth filter's reach from a window's edge (open 160 m + blur): inside it two windows agree
  KERN_CONST Float kDemMargin = 300.0;
  /// terrain_craters.mjs WATER_LEVEL_M
  KERN_CONST Float kWaterLevel = -0.18;
  KERN_CONST Float kPiD = 3.141592653589793;
  /// see README: the city ring is ~5 km, bare earth is smooth below 160 m
  KERN_CONST Int kDemZoom = 13;
  /// and the largest (low latitudes: a map tile is 2.4 km wide there)
  KERN_CONST Float kDemMaxSize = 24000.0;
  /// the smallest elevation window (m); it grows with the map tiles it must hold
  KERN_CONST Float kDemSize = 8000.0;
  /// grid spacing of the sampled elevation (m)
  KERN_CONST Float kDemStep = 10.0;
  /// terrain_elevation.mjs OPEN_M, BLUR_M
  KERN_CONST Float kOpen = 160.0;
  /// terrain_elevation.mjs OPEN_M, BLUR_M
  KERN_CONST Float kBlur = 30.0;
  /// the elevation grid job (kern/core/jobs.h; city jobs are 1, 2)
  KERN_CONST Int kDemJob = 3;
  /// terrain_craters.mjs TERRAIN_FIELD_STEP_M
  KERN_CONST Float kStep = 5.0;
  /// far render tile
  KERN_CONST Float kBlock = 640.0;
  /// collision / near render tile (32 cells of 5 m: terrain_tiles.mjs TILE_CELLS)
  KERN_CONST Float kTile = 160.0;
  KERN_CONST Float kWaterChunk = 320.0;
  /// terrain_craters.mjs RG, ROAD_BLEND_M
  KERN_CONST Float kRoadBlend = 4.5;
  /// terrain_craters.mjs RG, ROAD_BLEND_M
  KERN_CONST Float kRoadGrid = 24.0;
  /// world_water.mjs CELL
  KERN_CONST Float kWaterCell = 4.0;
  /// terrain_craters.mjs WATER_BED_M
  KERN_CONST Float kWaterBed = 2.6;
  /// ---- ground colour (world_ground.mjs): aerial imagery of the 1600 m around the player, one albedo per 5 m node (never shown as a texture); rebuilt after 350 m of travel ----
  KERN_CONST Int kImageZoom = 15;
  /// ---- ground colour (world_ground.mjs): aerial imagery of the 1600 m around the player, one albedo per 5 m node (never shown as a texture); rebuilt after 350 m of travel ----
  KERN_CONST Int kImagePixels = 64;
  /// 16 x 16 water cells
  KERN_CONST Float kWaterBlock = 64.0;
  KERN_CONST Int kCells = 32;
  KERN_CONST Vector kBed = Vector{0.3, 0.29, 0.22};
  /// world_impacts.mjs ground chip colour
  KERN_CONST Vector kSoil = Vector{0.4196078431372549, 0.35294117647058826, 0.26666666666666666};
  /// world_ground.mjs fallbackColors (.16, .2, .1 linear) in sRGB
  KERN_CONST Vector kGround = Vector{0.437, 0.485, 0.349};
  /// terrain_tiles.mjs SKIRT_DROP_M
  KERN_CONST Float kSkirtDrop = 0.05;
  /// world_water.mjs WATER_RENDER_Z (above the level)
  KERN_CONST Float kWaterRenderZ = 0.052;

  // ---- functions ----
  struct elevation_inResult { Bool Result = false; };
  /// the window that knows the place (its interior); else any that holds it; else the player's, held at its edge (far ground nobody stands on: the horizon)
  KERN_PURE elevation_inResult elevation_in(const Vec4& r, Float x = 0.0, Float y = 0.0) {
    return {logic_and(logic_and(logic_and(math_compare<">=">(x, r.x), math_compare<">=">(y, r.y)), math_compare<"<=">(x, r.z)), math_compare<"<=">(y, r.w))};
  }

  struct interior_ofResult { Vec4 Result = Vec4{0.0, 0.0, 0.0, 0.0}; };
  KERN_PURE interior_ofResult interior_of(Float x0 = 0.0, Float y0 = 0.0, Float span = 0.0, Float margin = 0.0) {
    return {engine_math_math::Vec4_new(kern::formula([](Float x0, Float margin) -> Float { return (x0 + margin); }, x0, margin), kern::formula([](Float y0, Float margin) -> Float { return (y0 + margin); }, y0, margin), kern::formula([](Float x0, Float span, Float margin) -> Float { return ((x0 + span) - margin); }, x0, span, margin), kern::formula([](Float y0, Float span, Float margin) -> Float { return ((y0 + span) - margin); }, y0, span, margin)).Result};
  }

  struct Terrain_interiorResult { Vec4 Result = Vec4{0.0, 0.0, 0.0, 0.0}; };
  KERN_PURE Terrain_interiorResult Terrain_interior(const Terrain_Grid& g) {
    return {interior_of(g.x0, g.y0, kern::formula([](Float step, Float n) -> Float { return (step * ((n > 0.0) ? kern::math::wrap32(n - 1.0) : 0.0)); }, g.step, kern::to<Float>(g.n)), kDemMargin).Result};
  }

  struct Terrain_grid_atResult { Float Result = 0.0; };
  KERN_PURE Terrain_grid_atResult Terrain_grid_at(const Terrain_Grid& g, Float x = 0.0, Float y = 0.0) {
    Float lim = 0.0;
    Float fx = 0.0;
    Float fy = 0.0;
    Int i = 0;
    Int j = 0;
    Float tx = 0.0;
    Float ty = 0.0;
    Int at = 0;
    if (math_compare<"<">(kern::to<Float>(g.n), 2.0)) {
      return {0.0};
    }
    lim = kern::formula([](Float n) -> Float { return (n - 1.001); }, kern::to<Float>(g.n));
    fx = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x0, Float step) -> Float { return kern::math::div((x - x0), step); }, x, g.x0, g.step), 0.0, lim));
    fy = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float y, Float y0, Float step) -> Float { return kern::math::div((y - y0), step); }, y, g.y0, g.step), 0.0, lim));
    i = kern::to<Int>(kern::formula([](Float fx) -> Float { return kern::math::trunc(fx); }, fx));
    j = kern::to<Int>(kern::formula([](Float fy) -> Float { return kern::math::trunc(fy); }, fy));
    tx = kern::formula([](Float fx, Float i) -> Float { return (fx - i); }, fx, kern::to<Float>(i));
    ty = kern::formula([](Float fy, Float j) -> Float { return (fy - j); }, fy, kern::to<Float>(j));
    at = kern::to<Int>(kern::formula([](Float j, Float n, Float i) -> Float { return ((j * n) + i); }, kern::to<Float>(j), kern::to<Float>(g.n), kern::to<Float>(i)));
    return {kern::formula([](Float a, Float tx, Float a_2, Float ty, Float a_3, Float a_4) -> Float { return ((((a * (1.0 - tx)) + (a_2 * tx)) * (1.0 - ty)) + (((a_3 * (1.0 - tx)) + (a_4 * tx)) * ty)); }, g.h[at], tx, g.h[kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 1.0); }, kern::to<Float>(at)))], ty, g.h[kern::to<Int>(kern::formula([](Float at, Float n) -> Float { return (at + n); }, kern::to<Float>(at), kern::to<Float>(g.n)))], g.h[kern::to<Int>(kern::formula([](Float at, Float n) -> Float { return ((at + n) + 1.0); }, kern::to<Float>(at), kern::to<Float>(g.n)))])};
  }

  struct Terrain_elevationResult { Float Result = 0.0; };
  KERN_PURE Terrain_elevationResult Terrain_elevation(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    Vec4 a = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 a_2 = Vec4{0.0, 0.0, 0.0, 0.0};
    Bool either = false;
    Vec4 a_3 = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 a_4 = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 a_5 = Vec4{0.0, 0.0, 0.0, 0.0};
    if (logic_not(self.grid_ready_)) {
      return {0.0};
    }
    either = math_compare<"==">(kern::to<Float>(list_length<Terrain_Grid>(self.side_)), 0.0);
    if (logic_not(either)) {
      a_2 = Terrain_interior(self.grid_).Result;
      either = elevation_in(a_2, x, y).Result;
    }
    if (either) {
      return {kern::formula([](Float the_result, Float ref_height_) -> Float { return (the_result - ref_height_); }, Terrain_grid_at(self.grid_, x, y).Result, self.ref_height_)};
    }
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_Grid>(self.side_))))); it10.next();) {
      a_3 = Terrain_interior(self.side_[n10.index]).Result;
      if (elevation_in(a_3, x, y).Result) {
        return {kern::formula([](Float the_result, Float ref_height_) -> Float { return (the_result - ref_height_); }, Terrain_grid_at(self.side_[n10.index], x, y).Result, self.ref_height_)};
      }
    }
    a_4 = interior_of(self.grid_.x0, self.grid_.y0, kern::formula([](Float step, Float n) -> Float { return (step * kern::math::wrap32(n - 1.0)); }, self.grid_.step, kern::to<Float>(self.grid_.n)), 0.0).Result;
    if (elevation_in(a_4, x, y).Result) {
      return {kern::formula([](Float the_result, Float ref_height_) -> Float { return (the_result - ref_height_); }, Terrain_grid_at(self.grid_, x, y).Result, self.ref_height_)};
    }
    for (auto it11 = flow_for(n11, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_Grid>(self.side_))))); it11.next();) {
      a_5 = interior_of(self.side_[n11.index].x0, self.side_[n11.index].y0, kern::formula([](Float a, Float a_2) -> Float { return (a * kern::math::wrap32(a_2 - 1.0)); }, self.side_[n11.index].step, kern::to<Float>(self.side_[n11.index].n)), 0.0).Result;
      if (elevation_in(a_5, x, y).Result) {
        return {kern::formula([](Float the_result, Float ref_height_) -> Float { return (the_result - ref_height_); }, Terrain_grid_at(self.side_[n11.index], x, y).Result, self.ref_height_)};
      }
    }
    return {kern::formula([](Float the_result, Float ref_height_) -> Float { return (the_result - ref_height_); }, Terrain_grid_at(self.grid_, x, y).Result, self.ref_height_)};
  }

  struct Terrain_water_levelResult { Float Result = 0.0; };
  KERN_PURE Terrain_water_levelResult Terrain_water_level(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    return {kern::formula([](Float the_result, Float kWaterLevel) -> Float { return (the_result + kWaterLevel); }, Terrain_elevation(self, x, y).Result, kWaterLevel)};
  }

  struct key2Result { Int64 Result = Int64{0x0}; };
  KERN_PURE key2Result key2(Int i = 0, Int j = 0) {
    return {int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32(i); }, kern::to<Float>(i)))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float j) -> Float { return kern::math::wrap32(j); }, kern::to<Float>(j)))))};
  }

  struct key3Result { Int64 Result = Int64{0x0}; };
  KERN_PURE key3Result key3(Int level = 0, Int i = 0, Int j = 0) {
    return {int64_or(int64_or(int64_shift_left(int64_of_int(level), 56), int64_shift_left(int64_and(int64_of_int(kern::to<Int>(kern::formula([](Float i) -> Float { return kern::math::wrap32(i); }, kern::to<Float>(i)))), Int64{0xfffffff}), 28)), int64_and(int64_of_int(kern::to<Int>(kern::formula([](Float j) -> Float { return kern::math::wrap32(j); }, kern::to<Float>(j)))), Int64{0xfffffff}))};
  }

  struct sx28Result { Int Result = 0; };
  KERN_PURE sx28Result sx28(Int64 v = Int64{0x0}) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::shr(kern::math::wrap32s(a), 4.0); }, kern::to<Float>(int64_low(int64_shift_left(v, 4)))))};
  }

  void unkey3(Int& level, Int& i, Int& j, Int64 k = Int64{0x0}) {
    level = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(k, 56)))));
    i = sx28(int64_and(int64_shift_right_unsigned(k, 28), Int64{0xfffffff})).Result;
    j = sx28(int64_and(k, Int64{0xfffffff})).Result;
  }

  struct rect_distanceResult { Float Result = 0.0; };
  KERN_PURE rect_distanceResult rect_distance(Float x = 0.0, Float y = 0.0, Float x0 = 0.0, Float y0 = 0.0, Float x1 = 0.0, Float y1 = 0.0) {
    Float dx = 0.0;
    Float dy = 0.0;
    dx = kern::formula([](Float x0, Float x, Float x1) -> Float { return kern::math::max(0.0, kern::math::max((x0 - x), (x - x1))); }, x0, x, x1);
    dy = kern::formula([](Float y0, Float y, Float y1) -> Float { return kern::math::max(0.0, kern::math::max((y0 - y), (y - y1))); }, y0, y, y1);
    return {kern::formula([](Float dx, Float dy) -> Float { return kern::math::sqrt(((dx * dx) + (dy * dy))); }, dx, dy)};
  }

  struct merc_yResult { Float Result = 0.0; };
  KERN_PURE merc_yResult merc_y(Float lat = 0.0, Int z = 0) {
    Float r = 0.0;
    r = kern::formula([](Float lat, Float kPiD) -> Float { return kern::math::div((lat * kPiD), 180.0); }, lat, kPiD);
    return {kern::formula([](Float r, Float kPiD, Float z) -> Float { return (kern::math::div((1.0 - kern::math::div(kern::math::log(kern::math::tan(r) + kern::math::sqrt((kern::math::tan(r) * kern::math::tan(r)) + 1.0)), kPiD)), 2.0) * (1.0 * kern::math::pow(2.0, z))); }, r, kPiD, kern::to<Float>(z))};
  }

  struct merc_xResult { Float Result = 0.0; };
  KERN_PURE merc_xResult merc_x(Float lon = 0.0, Int z = 0) {
    return {kern::formula([](Float lon, Float z) -> Float { return (kern::math::div((lon + 180.0), 360.0) * (1.0 * kern::math::pow(2.0, z))); }, lon, kern::to<Float>(z))};
  }

  /// Sliding-window min (op < 0) / max (op > 0) along lines (monotonic deque), window 2r+1.
  void slide(const List<Float>& src, List<Float>& dst, List<Int>& q, Int n = 0, Int stride = 0, Int line_stride = 0, Int r = 0, Int op = 0) {
    Int base = 0;
    Int qh = 0;
    Int qt = 0;
    Int j = 0;
    Int hi = 0;
    Float v = 0.0;
    Int was = 0;
    list_resize(q, n, 0);
    for (auto it12 = flow_for(n12, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it12.next();) {
      base = kern::to<Int>(kern::formula([](Float index, Float line_stride) -> Float { return kern::math::mul32(index, line_stride); }, kern::to<Float>(n12.index), kern::to<Float>(line_stride)));
      qh = 0;
      qt = 0;
      j = 0;
      for (auto it13 = flow_for(n13, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::wrap32s(n); }, kern::to<Float>(n)))))))); it13.next();) {
        hi = kern::to<Int>(kern::formula([](Float n, Float index, Float r) -> Float { return kern::math::min((kern::math::wrap32s(n) - 1.0), (index + r)); }, kern::to<Float>(n), kern::to<Float>(n13.index), kern::to<Float>(r)));
        while (loop(math_compare<"<=">(kern::to<Float>(j), kern::to<Float>(hi)))) {
          v = src[kern::to<Int>(kern::formula([](Float base, Float j, Float stride) -> Float { return kern::math::wrap32(base + kern::math::mul32(kern::math::wrap32(j), stride)); }, kern::to<Float>(base), kern::to<Float>(j), kern::to<Float>(stride)))];
          while (loop(logic_and(math_compare<">">(kern::to<Float>(qt), kern::to<Float>(qh)), logic_select<Bool>(math_compare<">">(kern::to<Float>(op), 0.0), math_compare<"<=">(src[kern::to<Int>(kern::formula([](Float base, Float a, Float stride) -> Float { return kern::math::wrap32(base + kern::math::mul32(kern::math::wrap32(a), stride)); }, kern::to<Float>(base), kern::to<Float>(q[kern::to<Int>(kern::formula([](Float qt) -> Float { return (qt - 1.0); }, kern::to<Float>(qt)))]), kern::to<Float>(stride)))], v), math_compare<">=">(src[kern::to<Int>(kern::formula([](Float base, Float a, Float stride) -> Float { return kern::math::wrap32(base + kern::math::mul32(kern::math::wrap32(a), stride)); }, kern::to<Float>(base), kern::to<Float>(q[kern::to<Int>(kern::formula([](Float qt) -> Float { return (qt - 1.0); }, kern::to<Float>(qt)))]), kern::to<Float>(stride)))], v))))) {
            qt = kern::to<Int>(kern::formula([](Float qt) -> Float { return (qt - 1.0); }, kern::to<Float>(qt)));
          }
          was = qt;
          qt += 1;
          q[was] = j;
          j += 1;
        }
        while (loop(math_compare<"<">(kern::to<Float>(q[qh]), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index, Float r) -> Float { return (index - r); }, kern::to<Float>(n13.index), kern::to<Float>(r)))))))) {
          qh += 1;
        }
        dst[kern::to<Int>(kern::formula([](Float base, Float index, Float stride) -> Float { return kern::math::wrap32(base + kern::math::mul32(kern::math::wrap32(index), stride)); }, kern::to<Float>(base), kern::to<Float>(n13.index), kern::to<Float>(stride)))] = src[kern::to<Int>(kern::formula([](Float base, Float a, Float stride) -> Float { return kern::math::wrap32(base + kern::math::mul32(kern::math::wrap32(a), stride)); }, kern::to<Float>(base), kern::to<Float>(q[qh]), kern::to<Float>(stride)))];
      }
    }
  }

  void box_blur(const List<Float>& src, List<Float>& dst, Int n = 0, Int stride = 0, Int line_stride = 0, Int r = 0) {
    Int base = 0;
    Float sum = 0.0;
    Int cnt = 0;
    Int add = 0;
    Int rem = 0;
    for (auto it14 = flow_for(n14, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it14.next();) {
      base = kern::to<Int>(kern::formula([](Float index, Float line_stride) -> Float { return kern::math::mul32(index, line_stride); }, kern::to<Float>(n14.index), kern::to<Float>(line_stride)));
      sum = 0.0;
      cnt = 0;
      for (auto it15 = flow_for(n15, 0, kern::to<Int>(kern::formula([](Float n, Float r) -> Float { return kern::math::min((kern::math::wrap32s(n) - 1.0), r); }, kern::to<Float>(n), kern::to<Float>(r)))); it15.next();) {
        sum += src[kern::to<Int>(kern::formula([](Float base, Float index, Float stride) -> Float { return kern::math::wrap32(base + kern::math::mul32(kern::math::wrap32(index), stride)); }, kern::to<Float>(base), kern::to<Float>(n15.index), kern::to<Float>(stride)))];
        cnt += 1;
      }
      for (auto it16 = flow_for(n16, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::wrap32s(n); }, kern::to<Float>(n)))))))); it16.next();) {
        dst[kern::to<Int>(kern::formula([](Float base, Float index, Float stride) -> Float { return kern::math::wrap32(base + kern::math::mul32(kern::math::wrap32(index), stride)); }, kern::to<Float>(base), kern::to<Float>(n16.index), kern::to<Float>(stride)))] = kern::formula([](Float sum, Float cnt) -> Float { return kern::math::div(sum, cnt); }, sum, kern::to<Float>(cnt));
        add = kern::to<Int>(kern::formula([](Float index, Float r) -> Float { return ((index + r) + 1.0); }, kern::to<Float>(n16.index), kern::to<Float>(r)));
        rem = kern::to<Int>(kern::formula([](Float index, Float r) -> Float { return (index - r); }, kern::to<Float>(n16.index), kern::to<Float>(r)));
        if (math_compare<"<">(kern::to<Float>(add), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::wrap32s(n); }, kern::to<Float>(n))))))) {
          sum += src[kern::to<Int>(kern::formula([](Float base, Float add, Float stride) -> Float { return kern::math::wrap32(base + kern::math::mul32(kern::math::wrap32(add), stride)); }, kern::to<Float>(base), kern::to<Float>(add), kern::to<Float>(stride)))];
          cnt += 1;
        }
        if (math_compare<">=">(kern::to<Float>(rem), 0.0)) {
          sum = kern::formula([](Float sum, Float a) -> Float { return (sum - a); }, sum, src[kern::to<Int>(kern::formula([](Float base, Float rem, Float stride) -> Float { return kern::math::wrap32(base + kern::math::mul32(kern::math::wrap32(rem), stride)); }, kern::to<Float>(base), kern::to<Float>(rem), kern::to<Float>(stride)))]);
          cnt = kern::to<Int>(kern::formula([](Float cnt) -> Float { return (cnt - 1.0); }, kern::to<Float>(cnt)));
        }
      }
    }
  }

  /// One filter pass of bare_earth (0..7).
  void filter_pass(List<Float>& h, List<Float>& t, List<Int>& q, Int n = 0, Int pass = 0, Int ro = 0, Int rb = 0) {
    Int selected = 0;
    selected = pass;
    if (math_compare<"==">(kern::to<Float>(selected), 0.0)) {
      slide(h, t, q, n, 1, n, ro, -1);
    } else if (math_compare<"==">(kern::to<Float>(selected), 1.0)) {
      slide(t, h, q, n, n, 1, ro, -1);
    } else if (math_compare<"==">(kern::to<Float>(selected), 2.0)) {
      slide(h, t, q, n, 1, n, ro, 1);
    } else if (math_compare<"==">(kern::to<Float>(selected), 3.0)) {
      slide(t, h, q, n, n, 1, ro, 1);
    } else if (logic_or(math_compare<"==">(kern::to<Float>(selected), 4.0), math_compare<"==">(kern::to<Float>(selected), 6.0))) {
      box_blur(h, t, n, 1, n, rb);
    } else if (logic_or(math_compare<"==">(kern::to<Float>(selected), 5.0), math_compare<"==">(kern::to<Float>(selected), 7.0))) {
      box_blur(t, h, n, n, 1, rb);
    }
  }

  struct crater_profileResult { Float Result = 0.0; };
  KERN_PURE crater_profileResult crater_profile(Float r = 0.0, Float radius = 0.0, Float depth = 0.0, Float rim = 0.0) {
    Float bowl = 0.0;
    Float x = 0.0;
    Float k = 0.0;
    bowl = kern::formula([](Float radius) -> Float { return (radius * 0.5); }, radius);
    if (math_compare<"<">(r, bowl)) {
      return {kern::formula([](Float depth, Float r, Float bowl) -> Float { return ((-depth) * (1.0 - (kern::math::div(r, bowl) * kern::math::div(r, bowl)))); }, depth, r, bowl)};
    }
    if (math_compare<">=">(r, radius)) {
      return {0.0};
    }
    x = kern::formula([](Float r, Float bowl, Float radius) -> Float { return kern::math::div((r - bowl), (radius - bowl)); }, r, bowl, radius);
    k = kern::formula([](Float x) -> Float { return kern::math::div((x - 0.12), 0.14); }, x);
    return {kern::formula([](Float rim, Float k, Float x) -> Float { return ((rim * kern::math::exp((-(k * k)))) * (1.0 - x)); }, rim, k, x)};
  }

  void bare_earth(List<Float>& h, Int n = 0, Float step = 0.0, Float open_m = 0.0, Float blur_m = 0.0) {
    List<Float> t = List<Float>{};
    List<Float> made = List<Float>{};
    List<Int> q = List<Int>{};
    Int ro = 0;
    Int rb = 0;
    made = List<Float>{};
    list_resize(made, list_length<Float>(h), 0.0);
    t = made;
    q = List<Int>{};
    ro = kern::to<Int>(kern::formula([](Float open_m, Float step) -> Float { return kern::math::max(1.0, kern::math::wrap32s(kern::math::round(kern::math::div(kern::math::div(open_m, step), 2.0)))); }, open_m, step));
    rb = kern::to<Int>(kern::formula([](Float blur_m, Float step) -> Float { return kern::math::max(1.0, kern::math::wrap32s(kern::math::round(kern::math::div(kern::math::div(blur_m, step), 2.0)))); }, blur_m, step));
    for (auto it17 = flow_for(n17, 0, 7); it17.next();) {
      filter_pass(h, t, q, n, n17.index, ro, rb);
    }
  }

  void Terrain_clear(Terrain& self) {
    Bool remove = false;
    Bool remove_2 = false;
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    if (math_compare<"!=">(kern::to<Float>(self.r_), 0.0)) {
      for (auto it20 = flow_for(n20, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Terrain_RenderTile>(self.render_))))); it20.next();) {
        if (math_compare<"!=">(kern::to<Float>(self.render_[n20.index].mesh), 0.0)) {
          engine_renderer_remove(n21, self.r_, self.render_[n20.index].mesh);
          remove = n21.result;
        }
      }
      for (auto it22 = flow_for(n22, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Int>(self.water_meshes_))))); it22.next();) {
        if (math_compare<"!=">(kern::to<Float>(self.water_meshes_[n22.index]), 0.0)) {
          engine_renderer_remove(n23, self.r_, self.water_meshes_[n22.index]);
          remove_2 = n23.result;
        }
      }
    }
    if (math_compare<"!=">(kern::to<Float>(self.world_), 0.0)) {
      for (auto it24 = flow_for(n24, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Terrain_CollisionTile>(self.collision_))))); it24.next();) {
        both = engine_physics_physics::Body_valid(self.collision_[n24.index].body).Result;
        if (both) {
          body_valid_2 = engine_world_body_valid(self.world_, self.collision_[n24.index].body);
          both = body_valid_2;
        }
        if (both) {
          engine_world_destroy_body(self.world_, self.collision_[n24.index].body);
        }
      }
    }
    map_clear(self.render_);
    map_clear(self.collision_);
    map_clear(self.water_meshes_);
    map_clear(self.water_detail_);
    set_clear(self.dirty_render_);
    set_clear(self.dirty_collision_);
    set_clear(self.dirty_water_);
    list_clear(self.want_render_);
    list_clear(self.want_collision_);
    list_clear(self.want_water_);
    map_clear(self.water_);
    list_clear(self.bridges_);
    list_clear(self.craters_);
    list_clear(self.tints_);
    map_clear(self.images_);
    list_clear(self.image_requests_);
    self.image_at_ = Vector{1000000000.0, 1000000000.0, 0.0};
    map_clear(self.dem_tiles_);
    list_clear(self.requests_);
    if (logic_and(math_compare<">">(kern::to<Float>(list_length<Terrain_Job>(self.job_)), 0.0), math_compare<"!=">(kern::to<Float>(self.job_[0].id), 0.0))) {
      engine_jobs_cancel(self.job_[0].id);
    }
    self.job_ = List<Terrain_Job>{};
    self.grid_ = struct_make<Terrain_Grid>(0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0);
    list_clear(self.side_);
    list_clear(self.corridors_);
    self.grid_ready_ = false;
    self.ref_ready_ = false;
    self.ref_height_ = 0.0;
    self.dem_failures_ = 0;
    self.retry_at_ = 0.0;
    list_clear(self.changes_);
    self.replan_ = true;
    self.planned_anchor_ = Vector{1000000000.0, 1000000000.0, 0.0};
    self.planned_focus_ = self.planned_anchor_;
    self.stats_ = struct_make<TerrainStats>(0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false);
    self.settled_ = logic_not(self.dem_);
  }

  void Terrain_start(Terrain& self, const GeoProjector& proj, Int renderer = 0, Int world = 0, Bool dem = false) {
    Terrain_clear(self);
    self.r_ = renderer;
    self.world_ = world;
    self.proj_ = proj;
    self.dem_ = dem;
    self.settled_ = logic_not(dem);
  }

  void Terrain_disable_dem(Terrain& self) {
    self.dem_ = false;
    if (logic_and(math_compare<">">(kern::to<Float>(list_length<Terrain_Job>(self.job_)), 0.0), math_compare<"!=">(kern::to<Float>(self.job_[0].id), 0.0))) {
      engine_jobs_cancel(self.job_[0].id);
    }
    self.job_ = List<Terrain_Job>{};
    list_clear(self.requests_);
    self.settled_ = true;
  }

  struct Terrain_take_requestsResult { Text Result = ""; };
  Terrain_take_requestsResult Terrain_take_requests(Terrain& self) {
    Text s = "";
    Text b = "";
    s = "[";
    for (auto it25 = flow_for(n25, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TileKey>(self.requests_))))); it25.next();) {
      b = "";
      b = text_format<"%s[%d,%d,%d]">(63, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n25.index), 0.0), ",", ""), self.requests_[n25.index].z, self.requests_[n25.index].x, self.requests_[n25.index].y);
      s = text_append(s, b, "");
    }
    list_clear(self.requests_);
    return {text_append(s, "]", "")};
  }

  void Terrain_provide(Terrain& self, const TileKey& key, const List<Int>& rgba, Int width = 0, Int height = 0) {
    Int it = 0;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Terrain_DemTile>(self.dem_tiles_, tiles::TileKey_hash(key).Result).index), kern::to<Float>(map_length<Int64, Terrain_DemTile>(self.dem_tiles_))));
    if (logic_or(logic_or(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Terrain_DemTile>(self.dem_tiles_))), math_compare<"==">(kern::to<Float>(width), 0.0)), math_compare<"==">(kern::to<Float>(height), 0.0))) {
      return;
    }
    self.dem_tiles_[it].w = width;
    self.dem_tiles_[it].hgt = height;
    list_resize(self.dem_tiles_[it].h, kern::to<Int>(kern::formula([](Float width, Float height) -> Float { return (width * height); }, kern::to<Float>(width), kern::to<Float>(height))), 0.0);
    for (auto it26 = flow_for(n26, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Float>(self.dem_tiles_[it].h))))); it26.next();) {
      self.dem_tiles_[it].h[n26.index] = terrain::terrarium_height(rgba[kern::to<Int>(kern::formula([](Float index) -> Float { return (index * 4.0); }, kern::to<Float>(n26.index)))], rgba[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index * 4.0) + 1.0); }, kern::to<Float>(n26.index)))], rgba[kern::to<Int>(kern::formula([](Float index) -> Float { return ((index * 4.0) + 2.0); }, kern::to<Float>(n26.index)))]).Result;
    }
    self.dem_tiles_[it].failed = false;
  }

  void Terrain_fail(Terrain& self, const TileKey& key) {
    Int it = 0;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Terrain_DemTile>(self.dem_tiles_, tiles::TileKey_hash(key).Result).index), kern::to<Float>(map_length<Int64, Terrain_DemTile>(self.dem_tiles_))));
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Terrain_DemTile>(self.dem_tiles_)))) {
      return;
    }
    self.dem_tiles_[it].failed = true;
    self.stats_.dem_failed = kern::to<Int>(kern::formula([](Float dem_failed) -> Float { return kern::math::wrap32(dem_failed + 1.0); }, kern::to<Float>(self.stats_.dem_failed)));
  }

  struct Terrain_sample_demResult { Float Result = 0.0; };
  Terrain_sample_demResult Terrain_sample_dem(const Terrain& self, Bool& ok, Float lon = 0.0, Float lat = 0.0) {
    Float fx = 0.0;
    Float fy = 0.0;
    Int tx = 0;
    Int ty = 0;
    Int it = 0;
    TileKey a = TileKey{14, 0, 0};
    Terrain_DemTile t = Terrain_DemTile{TileKey{14, 0, 0}, List<Float>{}, 0, 0, false, false};
    Float px = 0.0;
    Float py = 0.0;
    Int i = 0;
    Int j = 0;
    Float ax = 0.0;
    Float ay = 0.0;
    Int at = 0;
    fx = merc_x(lon, kDemZoom).Result;
    fy = merc_y(lat, kDemZoom).Result;
    tx = kern::to<Int>(kern::formula([](Float fx) -> Float { return kern::math::trunc(kern::math::floor(fx)); }, fx));
    ty = kern::to<Int>(kern::formula([](Float fy) -> Float { return kern::math::trunc(kern::math::floor(fy)); }, fy));
    a = struct_make<TileKey>(kDemZoom, tx, ty);
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Terrain_DemTile>(self.dem_tiles_, tiles::TileKey_hash(a).Result).index), kern::to<Float>(map_length<Int64, Terrain_DemTile>(self.dem_tiles_))));
    if (logic_or(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Terrain_DemTile>(self.dem_tiles_))), math_compare<"==">(kern::to<Float>(list_length<Float>(self.dem_tiles_[it].h)), 0.0))) {
      ok = false;
      return {0.0};
    }
    t = self.dem_tiles_[it];
    px = kern::formula([](Float w, Float fx, Float tx) -> Float { return kern::math::min((w - 1.001), ((fx - tx) * w)); }, kern::to<Float>(t.w), fx, kern::to<Float>(tx));
    py = kern::formula([](Float hgt, Float fy, Float ty) -> Float { return kern::math::min((hgt - 1.001), ((fy - ty) * hgt)); }, kern::to<Float>(t.hgt), fy, kern::to<Float>(ty));
    i = kern::to<Int>(kern::formula([](Float px) -> Float { return kern::math::trunc(px); }, px));
    j = kern::to<Int>(kern::formula([](Float py) -> Float { return kern::math::trunc(py); }, py));
    ax = kern::formula([](Float px, Float i) -> Float { return (px - i); }, px, kern::to<Float>(i));
    ay = kern::formula([](Float py, Float j) -> Float { return (py - j); }, py, kern::to<Float>(j));
    at = kern::to<Int>(kern::formula([](Float j, Float w, Float i) -> Float { return ((j * w) + i); }, kern::to<Float>(j), kern::to<Float>(t.w), kern::to<Float>(i)));
    return {kern::formula([](Float a, Float ax, Float a_2, Float ay, Float a_3, Float a_4) -> Float { return ((((a * (1.0 - ax)) + (a_2 * ax)) * (1.0 - ay)) + (((a_3 * (1.0 - ax)) + (a_4 * ax)) * ay)); }, t.h[at], ax, t.h[kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 1.0); }, kern::to<Float>(at)))], ay, t.h[kern::to<Int>(kern::formula([](Float at, Float w) -> Float { return (at + w); }, kern::to<Float>(at), kern::to<Float>(t.w)))], t.h[kern::to<Int>(kern::formula([](Float at, Float w) -> Float { return ((at + w) + 1.0); }, kern::to<Float>(at), kern::to<Float>(t.w)))])};
  }

  struct rect_inResult { Bool Result = false; };
  KERN_PURE rect_inResult rect_in(const Vec4& in, const Vec4& out) {
    return {logic_and(logic_and(logic_and(math_compare<">=">(in.x, out.x), math_compare<">=">(in.y, out.y)), math_compare<"<=">(in.z, out.z)), math_compare<"<=">(in.w, out.w))};
  }

  struct Terrain_coversResult { Bool Result = false; };
  KERN_PURE Terrain_coversResult Terrain_covers(const Terrain& self, const Vec4& rect) {
    Vec4 a = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 a_2 = Vec4{0.0, 0.0, 0.0, 0.0};
    if (logic_not(self.settled_)) {
      return {false};
    }
    if (logic_not(self.dem_)) {
      return {true};
    }
    if (logic_not(self.grid_ready_)) {
      return {false};
    }
    a = Terrain_interior(self.grid_).Result;
    if (rect_in(rect, a).Result) {
      return {true};
    }
    for (auto it27 = flow_for(n27, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_Grid>(self.side_))))); it27.next();) {
      a_2 = Terrain_interior(self.side_[n27.index]).Result;
      if (rect_in(rect, a_2).Result) {
        return {true};
      }
    }
    return {false};
  }

  struct Terrain_plan_demResult { Bool Result = false; };
  Terrain_plan_demResult Terrain_plan_dem(Terrain& self, Vector centre = Vector{0.0, 0.0, 0.0}, Float half = 0.0, Bool side = false) {
    Float now = 0.0;
    Float now_ms = 0.0;
    Float cx = 0.0;
    Float cy = 0.0;
    Float lon_a = 0.0;
    Float lat_a = 0.0;
    Float lon_b = 0.0;
    Float lat_b = 0.0;
    Int ix0 = 0;
    Int iy0 = 0;
    Int ix1 = 0;
    Int iy1 = 0;
    Bool all = false;
    Bool failed = false;
    TileKey k = TileKey{14, 0, 0};
    Int64 key = Int64{0x0};
    Int64 key_2 = Int64{0x0};
    Terrain_DemTile t = Terrain_DemTile{TileKey{14, 0, 0}, List<Float>{}, 0, 0, false, false};
    Int64 key_3 = Int64{0x0};
    Terrain_DemTile put = Terrain_DemTile{TileKey{14, 0, 0}, List<Float>{}, 0, 0, false, false};
    Terrain_DemTile t_2 = Terrain_DemTile{TileKey{14, 0, 0}, List<Float>{}, 0, 0, false, false};
    Int64 key_4 = Int64{0x0};
    Terrain_DemTile put_2 = Terrain_DemTile{TileKey{14, 0, 0}, List<Float>{}, 0, 0, false, false};
    Int64 key_5 = Int64{0x0};
    Int64 key_6 = Int64{0x0};
    Int it = 0;
    Int x = 0;
    Int y = 0;
    Int at = 0;
    if (logic_or(logic_not(self.dem_), math_compare<">">(kern::to<Float>(list_length<Terrain_Job>(self.job_)), 0.0))) {
      return {false};
    }
    now_ms = engine_now_ms();
    now = now_ms;
    if (math_compare<"<">(now, self.retry_at_)) {
      return {false};
    }
    cx = kern::formula([](Float x) -> Float { return (kern::math::round(kern::math::div(x, 50.0)) * 50.0); }, vector_break(centre).x);
    cy = kern::formula([](Float y) -> Float { return (kern::math::round(kern::math::div(y, 50.0)) * 50.0); }, vector_break(centre).y);
    lon_a = 0.0;
    lat_a = 0.0;
    lon_b = 0.0;
    lat_b = 0.0;
    tiles_cpp::unproject(self.proj_, lon_a, lat_a, kern::formula([](Float cx, Float half) -> Float { return (cx - half); }, cx, half), kern::formula([](Float cy, Float half) -> Float { return (cy + half); }, cy, half));
    tiles_cpp::unproject(self.proj_, lon_b, lat_b, kern::formula([](Float cx, Float half) -> Float { return (cx + half); }, cx, half), kern::formula([](Float cy, Float half) -> Float { return (cy - half); }, cy, half));
    ix0 = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::trunc(kern::math::floor(the_result)); }, merc_x(lon_a, kDemZoom).Result));
    iy0 = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::trunc(kern::math::floor(the_result)); }, merc_y(lat_a, kDemZoom).Result));
    ix1 = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::trunc(kern::math::floor(the_result)); }, merc_x(lon_b, kDemZoom).Result));
    iy1 = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::trunc(kern::math::floor(the_result)); }, merc_y(lat_b, kDemZoom).Result));
    if (math_compare<">">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float ix1, Float ix0, Float iy1, Float iy0) -> Float { return (((ix1 - ix0) + 1.0) * ((iy1 - iy0) + 1.0)); }, kern::to<Float>(ix1), kern::to<Float>(ix0), kern::to<Float>(iy1), kern::to<Float>(iy0))))), 100.0)) {
      return {false};
    }
    all = true;
    failed = false;
    for (auto it29 = flow_for(n29, iy0, iy1); it29.next();) {
      for (auto it30 = flow_for(n30, ix0, ix1); it30.next();) {
        k = struct_make<TileKey>(kDemZoom, n30.index, n29.index);
        key = tiles::TileKey_hash(k).Result;
        map_add(n31, self.dem_tiles_, key);
        key_2 = tiles::TileKey_hash(k).Result;
        map_add(n32, self.dem_tiles_, key_2);
        if (logic_not(struct_break<Terrain_DemTile>(self.dem_tiles_[n32.index]).requested)) {
          key_3 = tiles::TileKey_hash(k).Result;
          map_add(n33, self.dem_tiles_, key_3);
          t = self.dem_tiles_[n33.index];
          t.requested = true;
          put = t;
          self.dem_tiles_[n33.index] = put;
          key_4 = tiles::TileKey_hash(k).Result;
          map_add(n34, self.dem_tiles_, key_4);
          t_2 = self.dem_tiles_[n34.index];
          t_2.key = k;
          put_2 = t_2;
          self.dem_tiles_[n34.index] = put_2;
          list_add(self.requests_, k);
        }
        key_5 = tiles::TileKey_hash(k).Result;
        map_add(n35, self.dem_tiles_, key_5);
        if (struct_break<Terrain_DemTile>(self.dem_tiles_[n35.index]).failed) {
          failed = true;
        }
        key_6 = tiles::TileKey_hash(k).Result;
        map_add(n36, self.dem_tiles_, key_6);
        if (math_compare<"==">(kern::to<Float>(list_length<Float>(struct_break<Terrain_DemTile>(self.dem_tiles_[n36.index]).h)), 0.0)) {
          all = false;
        }
      }
    }
    if (failed) {
      self.dem_failures_ = kern::to<Int>(kern::formula([](Float dem_failures_) -> Float { return kern::math::wrap32(dem_failures_ + 1.0); }, kern::to<Float>(self.dem_failures_)));
      if (logic_not(self.grid_ready_)) {
        Terrain_disable_dem(self);
        return {false};
      }
      for (auto it37 = flow_for(n37, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Terrain_DemTile>(self.dem_tiles_))))); it37.next();) {
        if (self.dem_tiles_[n37.index].failed) {
          self.dem_tiles_[n37.index].failed = false;
          self.dem_tiles_[n37.index].requested = false;
        }
      }
      self.retry_at_ = kern::formula([](Float now, Float dem_failures_) -> Float { return (now + kern::math::min(15000.0, (1500.0 * dem_failures_))); }, now, kern::to<Float>(self.dem_failures_));
      return {false};
    }
    if (logic_not(all)) {
      return {false};
    }
    self.dem_failures_ = 0;
    if (logic_not(side)) {
      it = 0;
      while (loop(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Terrain_DemTile>(self.dem_tiles_)))))) {
        x = self.dem_tiles_[it].key.x;
        y = self.dem_tiles_[it].key.y;
        if (logic_or(logic_or(logic_or(math_compare<"<">(kern::to<Float>(x), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float ix0) -> Float { return (ix0 - 1.0); }, kern::to<Float>(ix0)))))), math_compare<">">(kern::to<Float>(x), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float ix1) -> Float { return (ix1 + 1.0); }, kern::to<Float>(ix1))))))), math_compare<"<">(kern::to<Float>(y), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float iy0) -> Float { return (iy0 - 1.0); }, kern::to<Float>(iy0))))))), math_compare<">">(kern::to<Float>(y), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float iy1) -> Float { return (iy1 + 1.0); }, kern::to<Float>(iy1)))))))) {
          at = it;
          map_remove_at(self.dem_tiles_, at);
          it = at;
        } else {
          it += 1;
        }
      }
    }
    self.job_ = list_make<Terrain_Job, "1">(Terrain_Job{Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}, 0, false, 0, 0, 0, 0});
    self.job_[0].side = side;
    self.job_[0].x0 = ix0;
    self.job_[0].y0 = iy0;
    self.job_[0].x1 = ix1;
    self.job_[0].y1 = iy1;
    self.job_[0].g.step = kDemStep;
    self.job_[0].g.n = kern::to<Int>(kern::formula([](Float half, Float kDemStep) -> Float { return kern::math::wrap32(kern::math::wrap32(kern::math::round(kern::math::div((half * 2.0), kDemStep))) + 1.0); }, half, kDemStep));
    self.job_[0].g.x0 = kern::formula([](Float cx, Float half) -> Float { return (cx - half); }, cx, half);
    self.job_[0].g.y0 = kern::formula([](Float cy, Float half) -> Float { return (cy - half); }, cy, half);
    self.job_[0].g.cx = cx;
    self.job_[0].g.cy = cy;
    list_resize(self.job_[0].g.h, kern::to<Int>(kern::formula([](Float a) -> Float { return (a * a); }, kern::to<Float>(self.job_[0].g.n))), 0.0);
    return {true};
  }

  struct Terrain_plan_windows_matchResult { Bool Result = false; };
  /// far mates: their own windows (the ground there is the real one for the host's traffic too)
  Terrain_plan_windows_matchResult Terrain_plan_windows_match(Terrain& self, const Terrain_Grid& g) {
    Vec4 a = Vec4{0.0, 0.0, 0.0, 0.0};
    for (auto it39 = flow_for(n39, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vec4>(self.needs))))); it39.next();) {
      a = Terrain_interior(g).Result;
      if (rect_in(self.needs[n39.index], a).Result) {
        return {false};
      }
    }
    return {true};
  }

  void Terrain_plan_windows(Terrain& self, Vector focus = Vector{0.0, 0.0, 0.0}) {
    Vec4 own = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 all = Vec4{0.0, 0.0, 0.0, 0.0};
    Bool together = false;
    Vec4 want = Vec4{0.0, 0.0, 0.0, 0.0};
    Float tile = 0.0;
    Float slack = 0.0;
    Bool planned = false;
    Vec4 a = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 a_2 = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 a_3 = Vec4{0.0, 0.0, 0.0, 0.0};
    Bool either = false;
    Vector c = Vector{0.0, 0.0, 0.0};
    Float lead_max = 0.0;
    Float half_need = 0.0;
    Float half = 0.0;
    Int at = 0;
    Int kept = 0;
    Int end = 0;
    Bool skip = false;
    Float half_2 = 0.0;
    if (logic_or(logic_not(self.dem_), math_compare<">">(kern::to<Float>(list_length<Terrain_Job>(self.job_)), 0.0))) {
      return;
    }
    own = engine_math_math::Vec4_new(kern::formula([](Float x) -> Float { return (x - 1500.0); }, vector_break(focus).x), kern::formula([](Float y) -> Float { return (y - 1500.0); }, vector_break(focus).y), kern::formula([](Float x) -> Float { return (x + 1500.0); }, vector_break(focus).x), kern::formula([](Float y) -> Float { return (y + 1500.0); }, vector_break(focus).y)).Result;
    if (logic_not(math_compare<"==">(kern::to<Float>(list_length<Vec4>(self.needs)), 0.0))) {
      own = self.needs[0];
    }
    all = own;
    for (auto it28 = flow_for(n28, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vec4>(self.needs))))); it28.next();) {
      all = engine_math_math::Vec4_new(kern::formula([](Float x, Float a) -> Float { return kern::math::min(x, a); }, all.x, self.needs[n28.index].x), kern::formula([](Float y, Float a) -> Float { return kern::math::min(y, a); }, all.y, self.needs[n28.index].y), kern::formula([](Float z, Float a) -> Float { return kern::math::max(z, a); }, all.z, self.needs[n28.index].z), kern::formula([](Float w, Float a) -> Float { return kern::math::max(w, a); }, all.w, self.needs[n28.index].w)).Result;
    }
    together = logic_and(logic_and(math_compare<">">(kern::to<Float>(list_length<Vec4>(self.needs)), 1.0), math_compare<"<">(kern::formula([](Float z, Float x) -> Float { return (z - x); }, all.z, all.x), kern::formula([](Float kDemMaxSize, Float kDemMargin) -> Float { return ((kDemMaxSize - (2.0 * kDemMargin)) - 800.0); }, kDemMaxSize, kDemMargin))), math_compare<"<">(kern::formula([](Float w, Float y) -> Float { return (w - y); }, all.w, all.y), kern::formula([](Float kDemMaxSize, Float kDemMargin) -> Float { return ((kDemMaxSize - (2.0 * kDemMargin)) - 800.0); }, kDemMaxSize, kDemMargin)));
    want = logic_select<Vec4>(together, all, own);
    tile = kern::formula([](Float z, Float x) -> Float { return kern::math::max(400.0, kern::math::div((z - x), 3.0)); }, own.z, own.x);
    slack = kern::formula([](Float tile) -> Float { return kern::math::min(400.0, (tile * 0.25)); }, tile);
    planned = false;
    either = logic_not(self.grid_ready_);
    if (logic_not(either)) {
      a_2 = engine_math_math::Vec4_new(kern::formula([](Float x, Float slack) -> Float { return (x - slack); }, want.x, slack), kern::formula([](Float y, Float slack) -> Float { return (y - slack); }, want.y, slack), kern::formula([](Float z, Float slack) -> Float { return (z + slack); }, want.z, slack), kern::formula([](Float w, Float slack) -> Float { return (w + slack); }, want.w, slack)).Result;
      a_3 = Terrain_interior(self.grid_).Result;
      either = logic_not(rect_in(a_2, a_3).Result);
    }
    if (either) {
      c = vector_make(kern::formula([](Float x, Float z) -> Float { return kern::math::div((x + z), 2.0); }, want.x, want.z), kern::formula([](Float y, Float w) -> Float { return kern::math::div((y + w), 2.0); }, want.y, want.w), 0.0);
      lead_max = tile;
      c = vector_make(kern::formula([](Float x, Float x_2, Float lead_max) -> Float { return (x + (x_2 < (-lead_max) ? (-lead_max) : (lead_max < x_2 ? lead_max : x_2))); }, vector_break(c).x, vector_break(self.lead).x, lead_max), vector_break(c).y, vector_break(c).z);
      c = vector_make(vector_break(c).x, kern::formula([](Float y, Float y_2, Float lead_max) -> Float { return (y + (y_2 < (-lead_max) ? (-lead_max) : (lead_max < y_2 ? lead_max : y_2))); }, vector_break(c).y, vector_break(self.lead).y, lead_max), vector_break(c).z);
      half_need = kern::formula([](Float z, Float x, Float w, Float y, Float lead_max, Float kDemMargin, Float tile) -> Float { return (((kern::math::div(kern::math::max((z - x), (w - y)), 2.0) + lead_max) + kDemMargin) + tile); }, want.z, want.x, want.w, want.y, lead_max, kDemMargin, tile);
      half = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float kDemSize, Float half_need) -> Float { return (kern::math::ceil(kern::math::div(kern::math::max(kern::math::div(kDemSize, 2.0), half_need), 50.0)) * 50.0); }, kDemSize, half_need), kern::formula([](Float kDemSize) -> Float { return kern::math::div(kDemSize, 2.0); }, kDemSize), kern::formula([](Float kDemMaxSize) -> Float { return kern::math::div(kDemMaxSize, 2.0); }, kDemMaxSize)));
      n38 = Terrain_plan_dem(self, c, half, false);
      planned = n38.Result;
      if (logic_or(planned, math_compare<">">(kern::to<Float>(list_length<Terrain_Job>(self.job_)), 0.0))) {
        return;
      }
    }
    if (logic_or(together, logic_not(self.grid_ready_))) {
      list_clear(self.side_);
      return;
    }
    at = 0;
    kept = at;
    end = list_length<Terrain_Grid>(self.side_);
    while (loop(math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end)))) {
      n40 = Terrain_plan_windows_match(self, self.side_[at]);
      if (logic_not(n40.Result)) {
        if (math_compare<"<">(kern::to<Float>(kept), kern::to<Float>(at))) {
          var_take(n41, self.side_[at]);
          self.side_[kept] = n41.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.side_, kept, Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0});
    for (auto it42 = flow_for(n42, 1, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vec4>(self.needs))))); it42.next();) {
      skip = false;
      if (logic_or(Terrain_covers(self, self.needs[n42.index]).Result, math_compare<">=">(kern::to<Float>(list_length<Terrain_Grid>(self.side_)), 3.0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        half_2 = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float a, Float a_2, Float a_3, Float a_4, Float kDemMargin) -> Float { return (kern::math::ceil(kern::math::div(((kern::math::div(kern::math::max((a - a_2), (a_3 - a_4)), 2.0) + kDemMargin) + 400.0), 50.0)) * 50.0); }, self.needs[n42.index].z, self.needs[n42.index].x, self.needs[n42.index].w, self.needs[n42.index].y, kDemMargin), 1500.0, kern::formula([](Float kDemMaxSize) -> Float { return kern::math::div(kDemMaxSize, 2.0); }, kDemMaxSize)));
        n43 = Terrain_plan_dem(self, vector_make(kern::formula([](Float a, Float a_2) -> Float { return kern::math::div((a + a_2), 2.0); }, self.needs[n42.index].x, self.needs[n42.index].z), kern::formula([](Float a, Float a_2) -> Float { return kern::math::div((a + a_2), 2.0); }, self.needs[n42.index].y, self.needs[n42.index].w), 0.0), half_2, true);
        if (logic_or(n43.Result, math_compare<">">(kern::to<Float>(list_length<Terrain_Job>(self.job_)), 0.0))) {
          return;
        }
      }
    }
  }

  struct Terrain_run_job_mineResult { Bool Result = false; };
  /// Sampling the window (641k points) and the bare-earth filter run off the frame (a Web Worker / thread), like MapLibre's raster-dem workers do it for the original. (kDemJob's input - see dem_grid_job: the numbers of each struct as their bytes, the heights of each tile as theirs, one after the other)
  Terrain_run_job_mineResult Terrain_run_job_mine(Terrain& self, const Terrain_DemTile& t) {
    return {logic_and(logic_and(logic_and(logic_and(logic_not(math_compare<"==">(kern::to<Float>(list_length<Float>(t.h)), 0.0)), math_compare<">=">(kern::to<Float>(t.key.x), kern::to<Float>(self.job_[0].x0))), math_compare<"<=">(kern::to<Float>(t.key.x), kern::to<Float>(self.job_[0].x1))), math_compare<">=">(kern::to<Float>(t.key.y), kern::to<Float>(self.job_[0].y0))), math_compare<"<=">(kern::to<Float>(t.key.y), kern::to<Float>(self.job_[0].y1)))};
  }

  void Terrain_dirty(Terrain& self, Float x0 = 0.0, Float y0 = 0.0, Float x1 = 0.0, Float y1 = 0.0, Bool notify = false) {
    Float x0_ = 0.0;
    Float y0_ = 0.0;
    Float x1_ = 0.0;
    Float y1_ = 0.0;
    Float m = 0.0;
    Int level = 0;
    Int i = 0;
    Int j = 0;
    Float size = 0.0;
    Int64 key = Int64{0x0};
    Int i_2 = 0;
    Int j_2 = 0;
    Int64 key_2 = Int64{0x0};
    Int i_3 = 0;
    Int j_3 = 0;
    Int64 key_3 = Int64{0x0};
    x0_ = x0;
    y0_ = y0;
    x1_ = x1;
    y1_ = y1;
    m = kern::formula([](Float kStep) -> Float { return (kStep * 2.0); }, kStep);
    x0_ = kern::formula([](Float x0_, Float m) -> Float { return (x0_ - m); }, x0_, m);
    y0_ = kern::formula([](Float y0_, Float m) -> Float { return (y0_ - m); }, y0_, m);
    x1_ += m;
    y1_ += m;
    for (auto it50 = flow_for(n50, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Terrain_RenderTile>(self.render_))))); it50.next();) {
      level = 0;
      i = 0;
      j = 0;
      unkey3(level, i, j, map_key_at<Int64, Terrain_RenderTile>(self.render_, n50.index));
      size = kern::formula([](Float level, Float kBlock, Float kTile) -> Float { return ((level == 2.0) ? kBlock : kTile); }, kern::to<Float>(level), kBlock, kTile);
      if (logic_and(logic_and(logic_and(math_compare<"<=">(kern::formula([](Float i, Float size) -> Float { return (i * size); }, kern::to<Float>(i), size), x1_), math_compare<">=">(kern::formula([](Float i, Float size) -> Float { return ((i + 1.0) * size); }, kern::to<Float>(i), size), x0_)), math_compare<"<=">(kern::formula([](Float j, Float size) -> Float { return (j * size); }, kern::to<Float>(j), size), y1_)), math_compare<">=">(kern::formula([](Float j, Float size) -> Float { return ((j + 1.0) * size); }, kern::to<Float>(j), size), y0_))) {
        key = map_key_at<Int64, Terrain_RenderTile>(self.render_, n50.index);
        set_add(n51, self.dirty_render_, key);
      }
    }
    for (auto it52 = flow_for(n52, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Terrain_CollisionTile>(self.collision_))))); it52.next();) {
      i_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(map_key_at<Int64, Terrain_CollisionTile>(self.collision_, n52.index), 32)))))))));
      j_2 = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(int64_and(map_key_at<Int64, Terrain_CollisionTile>(self.collision_, n52.index), Int64{0xffffffff})))))))));
      if (logic_and(logic_and(logic_and(math_compare<"<=">(kern::formula([](Float i_2, Float kTile, Float kStep) -> Float { return ((i_2 * kTile) - kStep); }, kern::to<Float>(i_2), kTile, kStep), x1_), math_compare<">=">(kern::formula([](Float i_2, Float kTile, Float kStep) -> Float { return (((i_2 + 1.0) * kTile) + kStep); }, kern::to<Float>(i_2), kTile, kStep), x0_)), math_compare<"<=">(kern::formula([](Float j_2, Float kTile, Float kStep) -> Float { return ((j_2 * kTile) - kStep); }, kern::to<Float>(j_2), kTile, kStep), y1_)), math_compare<">=">(kern::formula([](Float j_2, Float kTile, Float kStep) -> Float { return (((j_2 + 1.0) * kTile) + kStep); }, kern::to<Float>(j_2), kTile, kStep), y0_))) {
        key_2 = map_key_at<Int64, Terrain_CollisionTile>(self.collision_, n52.index);
        set_add(n53, self.dirty_collision_, key_2);
      }
    }
    for (auto it54 = flow_for(n54, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Int>(self.water_meshes_))))); it54.next();) {
      i_3 = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(map_key_at<Int64, Int>(self.water_meshes_, n54.index), 32)))))))));
      j_3 = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(int64_and(map_key_at<Int64, Int>(self.water_meshes_, n54.index), Int64{0xffffffff})))))))));
      if (logic_and(logic_and(logic_and(math_compare<"<=">(kern::formula([](Float i_3, Float kWaterChunk) -> Float { return (i_3 * kWaterChunk); }, kern::to<Float>(i_3), kWaterChunk), x1_), math_compare<">=">(kern::formula([](Float i_3, Float kWaterChunk) -> Float { return ((i_3 + 1.0) * kWaterChunk); }, kern::to<Float>(i_3), kWaterChunk), x0_)), math_compare<"<=">(kern::formula([](Float j_3, Float kWaterChunk) -> Float { return (j_3 * kWaterChunk); }, kern::to<Float>(j_3), kWaterChunk), y1_)), math_compare<">=">(kern::formula([](Float j_3, Float kWaterChunk) -> Float { return ((j_3 + 1.0) * kWaterChunk); }, kern::to<Float>(j_3), kWaterChunk), y0_))) {
        key_3 = map_key_at<Int64, Int>(self.water_meshes_, n54.index);
        set_add(n55, self.dirty_water_, key_3);
      }
    }
    if (notify) {
      list_add(self.changes_, engine_math_math::Vec4_new(x0_, y0_, x1_, y1_).Result);
    }
    self.replan_ = true;
  }

  struct Terrain_run_jobResult { Bool Result = false; };
  Terrain_run_jobResult Terrain_run_job(Terrain& self, Float deadline = 0.0) {
    Int count = 0;
    DemOrigin origin = DemOrigin{0.0, 0.0};
    DemWindow win = DemWindow{0.0, 0.0, 10.0, 0, 160.0, 30.0, 0};
    List<Int> in = List<Int>{};
    List<Int> made = List<Int>{};
    Fixed<Int, 16> bytes = Fixed<Int, 16>{};
    Fixed<Int, 28> bytes_2 = Fixed<Int, 28>{};
    Bool skip = false;
    DemTileHead head = DemTileHead{0, 0, 0, 0};
    Int at = 0;
    Fixed<Int, 16> bytes_3 = Fixed<Int, 16>{};
    Int submit = 0;
    List<Int> out = List<Int>{};
    Bool take = false;
    Float now_ms = 0.0;
    Float span = 0.0;
    Float x0 = 0.0;
    Float y0 = 0.0;
    Bool reload = false;
    Vec4 known = Vec4{0.0, 0.0, 0.0, 0.0};
    Float lo = 0.0;
    Float hi = 0.0;
    Float v = 0.0;
    Float big = 0.0;
    if (logic_not(math_compare<"!=">(kern::to<Float>(self.job_[0].id), 0.0))) {
      count = 0;
      for (auto it44 = flow_for(n44, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Terrain_DemTile>(self.dem_tiles_))))); it44.next();) {
        n45 = Terrain_run_job_mine(self, self.dem_tiles_[n44.index]);
        count = kern::to<Int>(kern::formula([](Float count, Float the_result) -> Float { return kern::math::wrap32(count + kern::math::wrap32(((the_result != 0.0) ? 1.0 : 0.0))); }, kern::to<Float>(count), kern::to<Float>(n45.Result)));
      }
      origin = struct_make<DemOrigin>(self.proj_.lon0, self.proj_.lat0);
      win = struct_make<DemWindow>(self.job_[0].g.x0, self.job_[0].g.y0, self.job_[0].g.step, self.job_[0].g.n, kOpen, kBlur, count);
      made = List<Int>{};
      list_resize(made, 44, 0);
      in = made;
      bytes = struct_bytes<DemOrigin, "16: f64@0 f64@8">(origin);
      list_copy_bytes<"u8", "u8">(in, 0, bytes, 0, 16);
      bytes_2 = struct_bytes<DemWindow, "28: f32@0 f32@4 f32@8 u32@12 f32@16 f32@20 u32@24">(win);
      list_copy_bytes<"u8", "u8">(in, 16, bytes_2, 0, 28);
      for (auto it46 = flow_for(n46, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Terrain_DemTile>(self.dem_tiles_))))); it46.next();) {
        skip = false;
        n47 = Terrain_run_job_mine(self, self.dem_tiles_[n46.index]);
        if (logic_not(n47.Result)) {
          skip = true;
        }
        if (logic_not(skip)) {
          head = struct_make<DemTileHead>(kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(self.dem_tiles_[n46.index].key.x))), kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(self.dem_tiles_[n46.index].key.y))), self.dem_tiles_[n46.index].w, self.dem_tiles_[n46.index].hgt);
          at = list_length<Int>(in);
          list_resize(in, kern::to<Int>(kern::formula([](Float at, Float a) -> Float { return ((at + 16.0) + (a * 4.0)); }, kern::to<Float>(at), kern::to<Float>(list_length<Float>(self.dem_tiles_[n46.index].h)))), 0);
          bytes_3 = struct_bytes<DemTileHead, "16: u32@0 u32@4 u32@8 u32@12">(head);
          list_copy_bytes<"u8", "u8">(in, at, bytes_3, 0, 16);
          list_copy_bytes<"u8", "f32">(in, kern::to<Int>(kern::formula([](Float a) -> Float { return a + 16.0; }, kern::to<Float>(at))), self.dem_tiles_[n46.index].h, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return (a * 4.0); }, kern::to<Float>(list_length<Float>(self.dem_tiles_[n46.index].h)))));
        }
      }
      engine_jobs_submit(n48, kDemJob, in);
      submit = n48.result;
      self.job_[0].id = submit;
      return {false};
    }
    out = List<Int>{};
    engine_jobs_take(n49, self.job_[0].id, out);
    take = n49.result;
    out = n49.out;
    if (logic_not(take)) {
      return {false};
    }
    self.job_[0].id = 0;
    if (math_compare<"!=">(kern::to<Float>(list_length<Int>(out)), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float a) -> Float { return ((a * a) * 4.0); }, kern::to<Float>(self.job_[0].g.n))))))) {
      self.job_ = List<Terrain_Job>{};
      now_ms = engine_now_ms();
      self.retry_at_ = kern::formula([](Float now_ms) -> Float { return (now_ms + 1500.0); }, now_ms);
      return {true};
    }
    list_copy_bytes<"f32", "u8">(self.job_[0].g.h, 0, out, 0, list_length<Int>(out));
    if (self.job_[0].side) {
      span = kern::formula([](Float a, Float a_2) -> Float { return (a * kern::math::wrap32(a_2 - 1.0)); }, self.job_[0].g.step, kern::to<Float>(self.job_[0].g.n));
      x0 = self.job_[0].g.x0;
      y0 = self.job_[0].g.y0;
      list_add(self.side_, self.job_[0].g);
      self.job_ = List<Terrain_Job>{};
      Terrain_dirty(self, x0, y0, kern::formula([](Float x0, Float span) -> Float { return (x0 + span); }, x0, span), kern::formula([](Float y0, Float span) -> Float { return (y0 + span); }, y0, span), true);
      return {true};
    }
    if (logic_not(self.ref_ready_)) {
      self.ref_height_ = Terrain_grid_at(self.job_[0].g, 0.0, 0.0).Result;
      self.ref_ready_ = true;
    }
    reload = self.grid_ready_;
    known = logic_select<Vec4>(reload, Terrain_interior(self.grid_).Result, engine_math_math::Vec4_new(0.0, 0.0, 0.0, 0.0).Result);
    var_take(n56, self.job_[0].g);
    self.job_[0].g.x0 = struct_break<Terrain_Grid>(n56.value).x0;
    self.job_[0].g.y0 = struct_break<Terrain_Grid>(n56.value).y0;
    self.job_[0].g.step = struct_break<Terrain_Grid>(n56.value).step;
    self.job_[0].g.n = struct_break<Terrain_Grid>(n56.value).n;
    self.job_[0].g.cx = struct_break<Terrain_Grid>(n56.value).cx;
    self.job_[0].g.cy = struct_break<Terrain_Grid>(n56.value).cy;
    self.grid_ = n56.value;
    self.grid_ready_ = true;
    self.settled_ = true;
    self.job_ = List<Terrain_Job>{};
    lo = 1000000000.0;
    hi = -1000000000.0;
    for (auto it57 = flow_for(n57, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Float>(self.grid_.h))))); it57.next();) {
      v = self.grid_.h[n57.index];
      lo = kern::formula([](Float lo, Float v, Float ref_height_) -> Float { return kern::math::min(lo, (v - ref_height_)); }, lo, v, self.ref_height_);
      hi = kern::formula([](Float hi, Float v, Float ref_height_) -> Float { return kern::math::max(hi, (v - ref_height_)); }, hi, v, self.ref_height_);
    }
    self.stats_.min_height = lo;
    self.stats_.max_height = hi;
    self.stats_.windows = kern::to<Int>(kern::formula([](Float windows) -> Float { return kern::math::wrap32(windows + 1.0); }, kern::to<Float>(self.stats_.windows)));
    if (reload) {
      big = 1000000000.0;
      Terrain_dirty(self, kern::formula([](Float big) -> Float { return (-big); }, big), kern::formula([](Float big) -> Float { return (-big); }, big), known.x, big, true);
      Terrain_dirty(self, known.z, kern::formula([](Float big) -> Float { return (-big); }, big), big, big, true);
      Terrain_dirty(self, known.x, kern::formula([](Float big) -> Float { return (-big); }, big), known.z, known.y, true);
      Terrain_dirty(self, known.x, known.w, known.z, big, true);
    }
    return {true};
  }

  struct road_keyResult { Int64 Result = Int64{0x0}; };
  KERN_PURE road_keyResult road_key(Int ix = 0, Int iy = 0) {
    return {int64_or(int64_shift_left(int64_of_int(kern::to<Int>(kern::formula([](Float ix) -> Float { return kern::math::wrap32(ix); }, kern::to<Float>(ix)))), 32), int64_of_int(kern::to<Int>(kern::formula([](Float iy) -> Float { return kern::math::wrap32(iy); }, kern::to<Float>(iy)))))};
  }

  struct Terrain_road_fieldResult { Float Result = 0.0; };
  KERN_PURE Terrain_road_fieldResult Terrain_road_field(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    Float sum = 0.0;
    Float wsum = 0.0;
    Float w = 0.0;
    sum = 0.0;
    wsum = 0.0;
    for (auto it58 = flow_for(n58, -2, 2); it58.next();) {
      for (auto it59 = flow_for(n59, -2, 2); it59.next();) {
        w = kern::formula([](Float index, Float index_2) -> Float { return ((3.0 - kern::math::abs(index)) * (3.0 - kern::math::abs(index_2))); }, kern::to<Float>(n59.index), kern::to<Float>(n58.index));
        sum += kern::formula([](Float the_result, Float w) -> Float { return (the_result * w); }, Terrain_elevation(self, kern::formula([](Float x, Float index) -> Float { return (x + (index * 9.0)); }, x, kern::to<Float>(n59.index)), kern::formula([](Float y, Float index) -> Float { return (y + (index * 9.0)); }, y, kern::to<Float>(n58.index))).Result, w);
        wsum += w;
      }
    }
    return {kern::formula([](Float sum, Float wsum) -> Float { return kern::math::div(sum, wsum); }, sum, wsum)};
  }

  void Terrain_remove_corridors(Terrain& self, Int64 owner = Int64{0x0}) {
    Int it = 0;
    Bool skip = false;
    Vec4 box = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 bounds = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 c = Vec4{0.0, 0.0, 0.0, 0.0};
    it = 0;
    while (loop(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(list_length<Terrain_CorridorSet_row>(self.corridors_)))))) {
      skip = false;
      if (logic_not(int64_equal(self.corridors_[it].items[0].owner, owner))) {
        skip = true;
      }
      if (logic_not(skip)) {
        box = self.corridors_[it].items[0].box;
        bounds = self.corridors_[it].items[0].bounds;
        list_remove_at(self.corridors_, it);
        if (math_compare<">">(box.z, box.x)) {
          c = engine_math_math::Vec4_new(kern::formula([](Float x, Float x_2) -> Float { return kern::math::max(x, x_2); }, box.x, bounds.x), kern::formula([](Float y, Float y_2) -> Float { return kern::math::max(y, y_2); }, box.y, bounds.y), kern::formula([](Float z, Float z_2) -> Float { return kern::math::min(z, z_2); }, box.z, bounds.z), kern::formula([](Float w, Float w_2) -> Float { return kern::math::min(w, w_2); }, box.w, bounds.w)).Result;
          if (logic_and(math_compare<">">(c.z, c.x), math_compare<">">(c.w, c.y))) {
            Terrain_dirty(self, c.x, c.y, c.z, c.w, false);
          }
        }
        return;
      }
      it += 1;
    }
  }

  struct set_corridors_nearResult { Bool Result = false; };
  /// (only what can reach into the tile's own square)
  KERN_PURE set_corridors_nearResult set_corridors_near(const Vec4& bounds, Float reach = 0.0, Vector p = Vector{0.0, 0.0, 0.0}) {
    return {logic_and(logic_and(logic_and(math_compare<">">(vector_break(p).x, kern::formula([](Float x, Float reach) -> Float { return ((x - reach) - 6.0); }, bounds.x, reach)), math_compare<"<">(vector_break(p).x, kern::formula([](Float z, Float reach) -> Float { return ((z + reach) + 6.0); }, bounds.z, reach))), math_compare<">">(vector_break(p).y, kern::formula([](Float y, Float reach) -> Float { return ((y - reach) - 6.0); }, bounds.y, reach))), math_compare<"<">(vector_break(p).y, kern::formula([](Float w, Float reach) -> Float { return ((w + reach) + 6.0); }, bounds.w, reach)))};
  }

  void Terrain_set_corridors(Terrain& self, const Vec4& bounds, const List<MapFeatures_Road>& roads, const List<Float>& widths, Int64 owner = Int64{0x0}) {
    List<Terrain_CorridorSet> set = List<Terrain_CorridorSet>{};
    List<Vector> pts = List<Vector>{};
    List<Float> hs = List<Float>{};
    Bool skip = false;
    List<Vector> points = List<Vector>{};
    Float width = 0.0;
    Int i = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Int k = 0;
    Float half = 0.0;
    Float reach = 0.0;
    Int i_2 = 0;
    Bool skip_2 = false;
    Vector a_2 = Vector{0.0, 0.0, 0.0};
    Vector b_2 = Vector{0.0, 0.0, 0.0};
    Float dx = 0.0;
    Float dy = 0.0;
    Float l2 = 0.0;
    Terrain_CorridorSeg seg = Terrain_CorridorSeg{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    Float x0 = 0.0;
    Float x1 = 0.0;
    Float y0 = 0.0;
    Float y1 = 0.0;
    List<Terrain_CorridorSeg> t = List<Terrain_CorridorSeg>{};
    Int64 key = Int64{0x0};
    List<Terrain_CorridorSeg> put = List<Terrain_CorridorSeg>{};
    Vec4 box = Vec4{0.0, 0.0, 0.0, 0.0};
    Vec4 c = Vec4{0.0, 0.0, 0.0, 0.0};
    Terrain_remove_corridors(self, owner);
    set = list_make<Terrain_CorridorSet, "1">(Terrain_CorridorSet{Int64{0x0}, Vec4{0.0, 0.0, 0.0, 0.0}, Vec4{0.0, 0.0, 0.0, 0.0}, Map<Int64, List<Terrain_CorridorSeg>>{}});
    set[0].owner = owner;
    set[0].bounds = bounds;
    set[0].box = engine_math_math::Vec4_new(1e+30, 1e+30, -1e+30, -1e+30).Result;
    pts = List<Vector>{};
    hs = List<Float>{};
    for (auto it60 = flow_for(n60, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<MapFeatures_Road>(roads))))); it60.next();) {
      skip = false;
      points = roads[n60.index].points;
      width = widths[n60.index];
      if (logic_or(math_compare<"<">(kern::to<Float>(list_length<Vector>(points)), 2.0), logic_not(math_compare<">">(width, 0.0)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        list_clear(pts);
        i = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i))))), kern::to<Float>(list_length<Vector>(points))), 0.5))) {
          a = points[i];
          b = points[kern::to<Int>(kern::formula([](Float i) -> Float { return (i + 1.0); }, kern::to<Float>(i)))];
          k = kern::to<Int>(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::max(1.0, kern::math::trunc(kern::math::ceil(kern::math::div(kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))), 5.0)))); }, vector_break(b).x, vector_break(a).x, vector_break(b).y, vector_break(a).y));
          for (auto it61 = flow_for(n61, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(k)))); it61.next();) {
            list_add(pts, vector_add(a, vector_scale(vector_sub(b, a), kern::formula([](Float index, Float k) -> Float { return kern::math::div(index, k); }, kern::to<Float>(n61.index), kern::to<Float>(k)))));
          }
          i += 1;
        }
        list_add(pts, points[kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(points))))]);
        list_resize(hs, list_length<Vector>(pts), 0.0);
        half = kern::formula([](Float width) -> Float { return kern::math::div(width, 2.0); }, width);
        reach = kern::formula([](Float half, Float kRoadBlend) -> Float { return (half + kRoadBlend); }, half, kRoadBlend);
        for (auto it62 = flow_for(n62, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(pts))))); it62.next();) {
          hs[n62.index] = kern::formula([](Float a, Float the_result) -> Float { return ((a != 0.0) ? the_result : 0.0); }, kern::to<Float>(set_corridors_near(bounds, reach, pts[n62.index]).Result), Terrain_road_field(self, vector_break(pts[n62.index]).x, vector_break(pts[n62.index]).y).Result);
        }
        i_2 = 0;
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float i_2) -> Float { return (i_2 + 1.0); }, kern::to<Float>(i_2))))), kern::to<Float>(list_length<Vector>(pts))), 0.5))) {
          skip_2 = false;
          a_2 = pts[i_2];
          b_2 = pts[kern::to<Int>(kern::formula([](Float i_2) -> Float { return (i_2 + 1.0); }, kern::to<Float>(i_2)))];
          if (logic_or(logic_not(set_corridors_near(bounds, reach, a_2).Result), logic_not(set_corridors_near(bounds, reach, b_2).Result))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            dx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, vector_break(b_2).x, vector_break(a_2).x);
            dy = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, vector_break(b_2).y, vector_break(a_2).y);
            l2 = kern::formula([](Float dx, Float dy) -> Float { return ((dx * dx) + (dy * dy)); }, dx, dy);
            if (math_compare<"<">(l2, 0.0001)) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              seg = struct_make<Terrain_CorridorSeg>(vector_break(a_2).x, vector_break(a_2).y, dx, dy, l2, hs[i_2], hs[kern::to<Int>(kern::formula([](Float i_2) -> Float { return (i_2 + 1.0); }, kern::to<Float>(i_2)))], half);
              x0 = kern::formula([](Float x, Float x_2, Float reach) -> Float { return (kern::math::min(x, x_2) - reach); }, vector_break(a_2).x, vector_break(b_2).x, reach);
              x1 = kern::formula([](Float x, Float x_2, Float reach) -> Float { return (kern::math::max(x, x_2) + reach); }, vector_break(a_2).x, vector_break(b_2).x, reach);
              y0 = kern::formula([](Float y, Float y_2, Float reach) -> Float { return (kern::math::min(y, y_2) - reach); }, vector_break(a_2).y, vector_break(b_2).y, reach);
              y1 = kern::formula([](Float y, Float y_2, Float reach) -> Float { return (kern::math::max(y, y_2) + reach); }, vector_break(a_2).y, vector_break(b_2).y, reach);
              set[0].box = engine_math_math::Vec4_new(kern::formula([](Float a, Float x0) -> Float { return kern::math::min(a, x0); }, set[0].box.x, x0), kern::formula([](Float a, Float y0) -> Float { return kern::math::min(a, y0); }, set[0].box.y, y0), kern::formula([](Float a, Float x1) -> Float { return kern::math::max(a, x1); }, set[0].box.z, x1), kern::formula([](Float a, Float y1) -> Float { return kern::math::max(a, y1); }, set[0].box.w, y1)).Result;
              for (auto it63 = flow_for(n63, kern::to<Int>(kern::formula([](Float x0, Float kRoadGrid) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x0, kRoadGrid))); }, x0, kRoadGrid)), kern::to<Int>(kern::formula([](Float x1, Float kRoadGrid) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x1, kRoadGrid))); }, x1, kRoadGrid))); it63.next();) {
                for (auto it64 = flow_for(n64, kern::to<Int>(kern::formula([](Float y0, Float kRoadGrid) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y0, kRoadGrid))); }, y0, kRoadGrid)), kern::to<Int>(kern::formula([](Float y1, Float kRoadGrid) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y1, kRoadGrid))); }, y1, kRoadGrid))); it64.next();) {
                  key = road_key(n63.index, n64.index).Result;
                  map_add(n65, set[0].grid, key);
                  t = set[0].grid[n65.index];
                  list_add(t, seg);
                  put = t;
                  set[0].grid[n65.index] = put;
                }
              }
            }
          }
          i_2 += 1;
        }
      }
    }
    box = set[0].box;
    var_take(n66, set);
    list_add(self.corridors_, struct_make<Terrain_CorridorSet_row>(n66.value));
    if (math_compare<">">(box.z, box.x)) {
      c = engine_math_math::Vec4_new(kern::formula([](Float x, Float x_2) -> Float { return kern::math::max(x, x_2); }, box.x, bounds.x), kern::formula([](Float y, Float y_2) -> Float { return kern::math::max(y, y_2); }, box.y, bounds.y), kern::formula([](Float z, Float z_2) -> Float { return kern::math::min(z, z_2); }, box.z, bounds.z), kern::formula([](Float w, Float w_2) -> Float { return kern::math::min(w, w_2); }, box.w, bounds.w)).Result;
      if (logic_and(math_compare<">">(c.z, c.x), math_compare<">">(c.w, c.y))) {
        Terrain_dirty(self, c.x, c.y, c.z, c.w, false);
      }
    }
  }

  struct Terrain_corridor_setResult { Int Result = 0; };
  KERN_PURE Terrain_corridor_setResult Terrain_corridor_set(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    for (auto it67 = flow_for(n67, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Terrain_CorridorSet_row>(self.corridors_))))); it67.next();) {
      if (logic_and(logic_and(logic_and(math_compare<">=">(x, self.corridors_[n67.index].items[0].bounds.x), math_compare<"<">(x, self.corridors_[n67.index].items[0].bounds.z)), math_compare<">=">(y, self.corridors_[n67.index].items[0].bounds.y)), math_compare<"<">(y, self.corridors_[n67.index].items[0].bounds.w))) {
        return {kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::wrap32s(index); }, kern::to<Float>(n67.index)))};
      }
    }
    return {-1};
  }

  struct Terrain_corridors_atResult { Bool Result = false; };
  KERN_PURE Terrain_corridors_atResult Terrain_corridors_at(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    return {logic_or(logic_not(self.corridors_expected), math_compare<">=">(kern::to<Float>(Terrain_corridor_set(self, x, y).Result), 0.0))};
  }

  struct Terrain_road_levelResult { Float Result = 0.0; };
  KERN_PURE Terrain_road_levelResult Terrain_road_level(const Terrain& self, Float x = 0.0, Float y = 0.0, Float h = 0.0) {
    Int at = 0;
    Int it = 0;
    Float best = 0.0;
    Float sw = 0.0;
    Float sh = 0.0;
    Int at_2 = 0;
    Int at_3 = 0;
    Bool skip = false;
    Float t = 0.0;
    Float px = 0.0;
    Float py = 0.0;
    Float d = 0.0;
    Float w = 0.0;
    Float ws = 0.0;
    at = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return ((a != 0.0) ? (-1.0) : a_2); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<Terrain_CorridorSet_row>(self.corridors_)), 0.0)), kern::to<Float>(Terrain_corridor_set(self, x, y).Result)));
    if (math_compare<"<">(kern::to<Float>(at), 0.0)) {
      return {h};
    }
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, List<Terrain_CorridorSeg>>(self.corridors_[at].items[0].grid, road_key(kern::to<Int>(kern::formula([](Float x, Float kRoadGrid) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kRoadGrid))); }, x, kRoadGrid)), kern::to<Int>(kern::formula([](Float y, Float kRoadGrid) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kRoadGrid))); }, y, kRoadGrid))).Result).index), kern::to<Float>(map_length<Int64, List<Terrain_CorridorSeg>>(self.corridors_[at].items[0].grid))));
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, List<Terrain_CorridorSeg>>(self.corridors_[at].items[0].grid)))) {
      return {h};
    }
    best = 0.0;
    sw = 0.0;
    sh = 0.0;
    at_2 = at;
    at_3 = it;
    for (auto it68 = flow_for(n68, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_CorridorSeg>(self.corridors_[at_2].items[0].grid[at_3]))))); it68.next();) {
      skip = false;
      t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float a, Float a_2, Float y, Float a_3, Float a_4, Float a_5) -> Float { return kern::math::div((((x - a) * a_2) + ((y - a_3) * a_4)), a_5); }, x, self.corridors_[at_2].items[0].grid[at_3][n68.index].ax, self.corridors_[at_2].items[0].grid[at_3][n68.index].dx, y, self.corridors_[at_2].items[0].grid[at_3][n68.index].ay, self.corridors_[at_2].items[0].grid[at_3][n68.index].dy, self.corridors_[at_2].items[0].grid[at_3][n68.index].l2), 0.0, 1.0));
      px = kern::formula([](Float a, Float a_2, Float t) -> Float { return (a + (a_2 * t)); }, self.corridors_[at_2].items[0].grid[at_3][n68.index].ax, self.corridors_[at_2].items[0].grid[at_3][n68.index].dx, t);
      py = kern::formula([](Float a, Float a_2, Float t) -> Float { return (a + (a_2 * t)); }, self.corridors_[at_2].items[0].grid[at_3][n68.index].ay, self.corridors_[at_2].items[0].grid[at_3][n68.index].dy, t);
      d = kern::formula([](Float x, Float px, Float y, Float py) -> Float { return kern::math::sqrt(((x - px) * (x - px)) + ((y - py) * (y - py))); }, x, px, y, py);
      w = kern::formula([](Float d, Float a, Float kRoadBlend) -> Float { return ((d <= a) ? 1.0 : ((d >= (a + kRoadBlend)) ? 0.0 : (1.0 - kern::math::div((d - a), kRoadBlend)))); }, d, self.corridors_[at_2].items[0].grid[at_3][n68.index].half, kRoadBlend);
      if (math_compare<"<=">(w, 0.0)) {
        skip = true;
      }
      if (logic_not(skip)) {
        ws = kern::formula([](Float w) -> Float { return ((w * w) * (3.0 - (2.0 * w))); }, w);
        sw += ws;
        sh += kern::formula([](Float ws, Float a, Float a_2, Float t) -> Float { return (ws * (a + ((a_2 - a) * t))); }, ws, self.corridors_[at_2].items[0].grid[at_3][n68.index].ha, self.corridors_[at_2].items[0].grid[at_3][n68.index].hb, t);
        best = kern::formula([](Float best, Float w) -> Float { return kern::math::max(best, w); }, best, w);
      }
    }
    if (math_compare<"<=">(sw, 0.0)) {
      return {h};
    }
    return {kern::formula([](Float h, Float sh, Float sw, Float best) -> Float { return (h + ((kern::math::div(sh, sw) - h) * ((best * best) * (3.0 - (2.0 * best))))); }, h, sh, sw, best)};
  }

  struct Terrain_water_cellResult { Bool Result = false; };
  KERN_PURE Terrain_water_cellResult Terrain_water_cell(const Terrain& self, Int cx = 0, Int cy = 0) {
    Int it = 0;
    Int idx = 0;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Terrain_WaterBlock>(self.water_, key2(kern::to<Int>(kern::formula([](Float cx) -> Float { return kern::math::shr(cx, 4.0); }, kern::to<Float>(cx))), kern::to<Int>(kern::formula([](Float cy) -> Float { return kern::math::shr(cy, 4.0); }, kern::to<Float>(cy)))).Result).index), kern::to<Float>(map_length<Int64, Terrain_WaterBlock>(self.water_))));
    if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Terrain_WaterBlock>(self.water_)))) {
      return {false};
    }
    idx = kern::to<Int>(kern::formula([](Float cy, Float cx) -> Float { return kern::math::wrap32(((kern::math::band(cy, 15.0) * 16.0) + kern::math::band(cx, 15.0))); }, kern::to<Float>(cy), kern::to<Float>(cx)));
    return {logic_not(int64_equal(int64_and(int64_shift_right_unsigned(self.water_[it].bits[kern::to<Int>(kern::formula([](Float idx) -> Float { return kern::math::shr(idx, 6.0); }, kern::to<Float>(idx)))], kern::to<Int>(kern::formula([](Float idx) -> Float { return kern::math::band(idx, 63.0); }, kern::to<Float>(idx)))), Int64{0x1}), Int64{0x0}))};
  }

  struct Terrain_on_bridgeResult { Bool Result = false; };
  KERN_PURE Terrain_on_bridgeResult Terrain_on_bridge(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    Bool skip = false;
    Float dx = 0.0;
    Float dy = 0.0;
    Float c = 0.0;
    Float s = 0.0;
    Float u = 0.0;
    Float v = 0.0;
    for (auto it70 = flow_for(n70, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_Bridge>(self.bridges_))))); it70.next();) {
      skip = false;
      dx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, x, vector_break(self.bridges_[n70.index].c).x);
      dy = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, y, vector_break(self.bridges_[n70.index].c).y);
      if (logic_or(math_compare<">">(kern::formula([](Float dx) -> Float { return kern::math::abs(dx); }, dx), kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, self.bridges_[n70.index].hl, self.bridges_[n70.index].hw)), math_compare<">">(kern::formula([](Float dy) -> Float { return kern::math::abs(dy); }, dy), kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, self.bridges_[n70.index].hl, self.bridges_[n70.index].hw)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        c = kern::formula([](Float a) -> Float { return kern::math::cos(a); }, self.bridges_[n70.index].yaw);
        s = kern::formula([](Float a) -> Float { return kern::math::sin(a); }, self.bridges_[n70.index].yaw);
        u = kern::formula([](Float dx, Float c, Float dy, Float s) -> Float { return ((dx * c) + (dy * s)); }, dx, c, dy, s);
        v = kern::formula([](Float dx, Float s, Float dy, Float c) -> Float { return (((-dx) * s) + (dy * c)); }, dx, s, dy, c);
        if (logic_and(math_compare<"<=">(kern::formula([](Float u) -> Float { return kern::math::abs(u); }, u), self.bridges_[n70.index].hl), math_compare<"<=">(kern::formula([](Float v) -> Float { return kern::math::abs(v); }, v), self.bridges_[n70.index].hw))) {
          return {true};
        }
      }
    }
    return {false};
  }

  struct Terrain_water_atResult { Bool Result = false; };
  KERN_PURE Terrain_water_atResult Terrain_water_at(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    if (math_compare<"==">(kern::to<Float>(map_length<Int64, Terrain_WaterBlock>(self.water_)), 0.0)) {
      return {false};
    }
    if (logic_not(Terrain_water_cell(self, kern::to<Int>(kern::formula([](Float x, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kWaterCell))); }, x, kWaterCell)), kern::to<Int>(kern::formula([](Float y, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kWaterCell))); }, y, kWaterCell))).Result)) {
      return {false};
    }
    return {logic_or(math_compare<"==">(kern::to<Float>(list_length<Terrain_Bridge>(self.bridges_)), 0.0), logic_not(Terrain_on_bridge(self, x, y).Result))};
  }

  struct Terrain_node_heightResult { Float Result = 0.0; };
  KERN_PURE Terrain_node_heightResult Terrain_node_height(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    Float e = 0.0;
    Float h = 0.0;
    Bool skip = false;
    Float dx = 0.0;
    Float dy = 0.0;
    Float r = 0.0;
    e = Terrain_elevation(self, x, y).Result;
    h = Terrain_road_level(self, x, y, e).Result;
    for (auto it69 = flow_for(n69, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_Crater>(self.craters_))))); it69.next();) {
      skip = false;
      dx = kern::formula([](Float x, Float x_2) -> Float { return (x - x_2); }, x, vector_break(self.craters_[n69.index].c).x);
      dy = kern::formula([](Float y, Float y_2) -> Float { return (y - y_2); }, y, vector_break(self.craters_[n69.index].c).y);
      if (logic_or(math_compare<">=">(kern::formula([](Float dx) -> Float { return kern::math::abs(dx); }, dx), self.craters_[n69.index].r), math_compare<">=">(kern::formula([](Float dy) -> Float { return kern::math::abs(dy); }, dy), self.craters_[n69.index].r))) {
        skip = true;
      }
      if (logic_not(skip)) {
        r = kern::formula([](Float dx, Float dy) -> Float { return kern::math::sqrt(((dx * dx) + (dy * dy))); }, dx, dy);
        if (math_compare<"<">(r, self.craters_[n69.index].r)) {
          h += crater_profile(r, self.craters_[n69.index].r, self.craters_[n69.index].depth, self.craters_[n69.index].rim).Result;
        }
      }
    }
    if (logic_and(logic_not(math_compare<"==">(kern::to<Float>(map_length<Int64, Terrain_WaterBlock>(self.water_)), 0.0)), Terrain_water_at(self, x, y).Result)) {
      return {kern::formula([](Float h, Float e, Float kWaterBed) -> Float { return kern::math::min(h, (e - kWaterBed)); }, h, e, kWaterBed)};
    }
    return {h};
  }

  struct Terrain_heightResult { Float Result = 0.0; };
  KERN_PURE Terrain_heightResult Terrain_height(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    Float x0 = 0.0;
    Float y0 = 0.0;
    Float u = 0.0;
    Float v = 0.0;
    Float a = 0.0;
    Float d = 0.0;
    x0 = kern::formula([](Float x, Float kStep) -> Float { return (kern::math::floor(kern::math::div(x, kStep)) * kStep); }, x, kStep);
    y0 = kern::formula([](Float y, Float kStep) -> Float { return (kern::math::floor(kern::math::div(y, kStep)) * kStep); }, y, kStep);
    u = kern::formula([](Float x, Float x0, Float kStep) -> Float { return kern::math::div((x - x0), kStep); }, x, x0, kStep);
    v = kern::formula([](Float y, Float y0, Float kStep) -> Float { return kern::math::div((y - y0), kStep); }, y, y0, kStep);
    a = Terrain_node_height(self, x0, y0).Result;
    d = Terrain_node_height(self, kern::formula([](Float x0, Float kStep) -> Float { return (x0 + kStep); }, x0, kStep), kern::formula([](Float y0, Float kStep) -> Float { return (y0 + kStep); }, y0, kStep)).Result;
    return {kern::formula([](Float u, Float v, Float a, Float the_result, Float d, Float the_result_2) -> Float { return ((u >= v) ? ((a + ((the_result - a) * (u - v))) + ((d - a) * v)) : ((a + ((the_result_2 - a) * (v - u))) + ((d - a) * u))); }, u, v, a, Terrain_node_height(self, kern::formula([](Float x0, Float kStep) -> Float { return (x0 + kStep); }, x0, kStep), y0).Result, d, Terrain_node_height(self, x0, kern::formula([](Float y0, Float kStep) -> Float { return (y0 + kStep); }, y0, kStep)).Result)};
  }

  struct Terrain_normalResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Terrain_normalResult Terrain_normal(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    Float e = 0.0;
    Float hx = 0.0;
    Float hy = 0.0;
    e = 0.5;
    hx = kern::formula([](Float the_result, Float the_result_2) -> Float { return (the_result - the_result_2); }, Terrain_height(self, kern::formula([](Float x, Float e) -> Float { return (x + e); }, x, e), y).Result, Terrain_height(self, kern::formula([](Float x, Float e) -> Float { return (x - e); }, x, e), y).Result);
    hy = kern::formula([](Float the_result, Float the_result_2) -> Float { return (the_result - the_result_2); }, Terrain_height(self, x, kern::formula([](Float y, Float e) -> Float { return (y + e); }, y, e)).Result, Terrain_height(self, x, kern::formula([](Float y, Float e) -> Float { return (y - e); }, y, e)).Result);
    return {logic_select<Vector>(math_compare<">">(vector_length(vector_make(kern::formula([](Float hx) -> Float { return (-hx); }, hx), kern::formula([](Float hy) -> Float { return (-hy); }, hy), kern::formula([](Float e) -> Float { return (2.0 * e); }, e))), 9.999999717180685e-10), vector_normalize(vector_make(kern::formula([](Float hx) -> Float { return (-hx); }, hx), kern::formula([](Float hy) -> Float { return (-hy); }, hy), kern::formula([](Float e) -> Float { return (2.0 * e); }, e))), Vector{0.0, 0.0, 1.0})};
  }

  struct Terrain_footprint_baseResult { Float Result = 0.0; };
  KERN_PURE Terrain_footprint_baseResult Terrain_footprint_base(const Terrain& self, const List<Vector>& ring) {
    Float m = 0.0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    Float ea = 0.0;
    Float ec = 0.0;
    if (logic_and(logic_not(self.grid_ready_), math_compare<"==">(kern::to<Float>(list_length<Terrain_Crater>(self.craters_)), 0.0))) {
      return {0.0};
    }
    m = 1000000000.0;
    for (auto it71 = flow_for(n71, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<Vector>(ring))))); it71.next();) {
      a = ring[n71.index];
      b = ring[kern::to<Int>(kern::formula([](Float index, Float a) -> Float { return kern::math::mod((index + 1.0), a); }, kern::to<Float>(n71.index), kern::to<Float>(list_length<Vector>(ring))))];
      c = vector_scale(vector_add(a, b), 0.5);
      ea = Terrain_elevation(self, vector_break(a).x, vector_break(a).y).Result;
      ec = Terrain_elevation(self, vector_break(c).x, vector_break(c).y).Result;
      m = kern::formula([](Float m, Float ea, Float the_result, Float ec, Float the_result_2) -> Float { return kern::math::min(m, kern::math::min(kern::math::min(ea, the_result), kern::math::min(ec, the_result_2))); }, m, ea, Terrain_road_level(self, vector_break(a).x, vector_break(a).y, ea).Result, ec, Terrain_road_level(self, vector_break(c).x, vector_break(c).y, ec).Result);
    }
    return {kern::formula([](Float m) -> Float { return ((m < 100000000.0) ? m : 0.0); }, m)};
  }

  void Terrain_set_water_cell(Terrain& self, Int cx = 0, Int cy = 0) {
    Int64 key = Int64{0x0};
    Int idx = 0;
    Int64 key_2 = Int64{0x0};
    List<Int64> t = List<Int64>{};
    Int64 key_3 = Int64{0x0};
    Terrain_WaterBlock t_2 = Terrain_WaterBlock{List<Int64>{}, List<Vector>{}, List<Float>{}};
    Terrain_WaterBlock put = Terrain_WaterBlock{List<Int64>{}, List<Vector>{}, List<Float>{}};
    key = key2(kern::to<Int>(kern::formula([](Float cx) -> Float { return kern::math::shr(cx, 4.0); }, kern::to<Float>(cx))), kern::to<Int>(kern::formula([](Float cy) -> Float { return kern::math::shr(cy, 4.0); }, kern::to<Float>(cy)))).Result;
    map_add(n72, self.water_, key);
    idx = kern::to<Int>(kern::formula([](Float cy, Float cx) -> Float { return kern::math::wrap32(((kern::math::band(cy, 15.0) * 16.0) + kern::math::band(cx, 15.0))); }, kern::to<Float>(cy), kern::to<Float>(cx)));
    key_2 = key2(kern::to<Int>(kern::formula([](Float cx) -> Float { return kern::math::shr(cx, 4.0); }, kern::to<Float>(cx))), kern::to<Int>(kern::formula([](Float cy) -> Float { return kern::math::shr(cy, 4.0); }, kern::to<Float>(cy)))).Result;
    map_add(n73, self.water_, key_2);
    if (logic_not(logic_not(int64_equal(int64_and(int64_shift_right_unsigned(list_get<Int64>(struct_break<Terrain_WaterBlock>(self.water_[n73.index]).bits, kern::to<Int>(kern::formula([](Float idx) -> Float { return kern::math::shr(idx, 6.0); }, kern::to<Float>(idx)))), kern::to<Int>(kern::formula([](Float idx) -> Float { return kern::math::band(idx, 63.0); }, kern::to<Float>(idx)))), Int64{0x1}), Int64{0x0})))) {
      self.stats_.water_cells = kern::to<Int>(kern::formula([](Float water_cells) -> Float { return kern::math::wrap32(water_cells + 1.0); }, kern::to<Float>(self.stats_.water_cells)));
    }
    key_3 = key2(kern::to<Int>(kern::formula([](Float cx) -> Float { return kern::math::shr(cx, 4.0); }, kern::to<Float>(cx))), kern::to<Int>(kern::formula([](Float cy) -> Float { return kern::math::shr(cy, 4.0); }, kern::to<Float>(cy)))).Result;
    map_add(n74, self.water_, key_3);
    t = struct_break<Terrain_WaterBlock>(self.water_[n74.index]).bits;
    t[kern::to<Int>(kern::formula([](Float idx) -> Float { return kern::math::shr(idx, 6.0); }, kern::to<Float>(idx)))] = int64_or(list_get<Int64>(struct_break<Terrain_WaterBlock>(self.water_[n74.index]).bits, kern::to<Int>(kern::formula([](Float idx) -> Float { return kern::math::shr(idx, 6.0); }, kern::to<Float>(idx)))), int64_shift_left(Int64{0x1}, kern::to<Int>(kern::formula([](Float idx) -> Float { return kern::math::band(idx, 63.0); }, kern::to<Float>(idx)))));
    t_2 = self.water_[n74.index];
    t_2.bits = t;
    put = t_2;
    self.water_[n74.index] = put;
  }

  struct Terrain_flow_atResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Terrain_flow_atResult Terrain_flow_at(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    Int cx = 0;
    Int cy = 0;
    Int it = 0;
    Int idx = 0;
    cx = kern::to<Int>(kern::formula([](Float x, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kWaterCell))); }, x, kWaterCell));
    cy = kern::to<Int>(kern::formula([](Float y, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kWaterCell))); }, y, kWaterCell));
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Terrain_WaterBlock>(self.water_, key2(kern::to<Int>(kern::formula([](Float cx) -> Float { return kern::math::shr(cx, 4.0); }, kern::to<Float>(cx))), kern::to<Int>(kern::formula([](Float cy) -> Float { return kern::math::shr(cy, 4.0); }, kern::to<Float>(cy)))).Result).index), kern::to<Float>(map_length<Int64, Terrain_WaterBlock>(self.water_))));
    if (logic_or(logic_or(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Terrain_WaterBlock>(self.water_))), math_compare<"==">(kern::to<Float>(list_length<Vector>(self.water_[it].flow)), 0.0)), logic_not(Terrain_water_at(self, x, y).Result))) {
      return {Vector{0.0, 0.0, 0.0}};
    }
    idx = kern::to<Int>(kern::formula([](Float cy, Float cx) -> Float { return kern::math::wrap32(((kern::math::band(cy, 15.0) * 16.0) + kern::math::band(cx, 15.0))); }, kern::to<Float>(cy), kern::to<Float>(cx)));
    return {vector_scale(self.water_[it].flow[idx], self.water_[it].speed[idx])};
  }

  struct Terrain_surface_atResult { Int Result = 0; };
  KERN_PURE Terrain_surface_atResult Terrain_surface_at(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    if (Terrain_water_at(self, x, y).Result) {
      return {1};
    }
    for (auto it75 = flow_for(n75, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_Crater>(self.craters_))))); it75.next();) {
      if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, x, vector_break(self.craters_[n75.index].c).x, y, vector_break(self.craters_[n75.index].c).y), kern::formula([](Float a) -> Float { return (a * 0.8); }, self.craters_[n75.index].r))) {
        return {2};
      }
    }
    return {0};
  }

  void Terrain_add_water_polygon(Terrain& self, const List<Vector_row>& rings) {
    Float y_lo = 0.0;
    Float y_hi = 0.0;
    Float x_lo = 0.0;
    Float x_hi = 0.0;
    Vector p = Vector{0.0, 0.0, 0.0};
    List<Float> xs = List<Float>{};
    Int j0 = 0;
    Int j1 = 0;
    Float y = 0.0;
    Int a = 0;
    Int c = 0;
    Int was = 0;
    Float ya = 0.0;
    Float yc = 0.0;
    Int k = 0;
    Int ia = 0;
    Int ib = 0;
    if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(rings)), 0.0), math_compare<"<">(kern::to<Float>(list_length<Vector>(rings[0].items)), 3.0))) {
      return;
    }
    y_lo = 1000000000.0;
    y_hi = -1000000000.0;
    x_lo = 1000000000.0;
    x_hi = -1000000000.0;
    for (auto it76 = flow_for(n76, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(rings[0].items))))); it76.next();) {
      p = rings[0].items[n76.index];
      y_lo = kern::formula([](Float y_lo, Float y) -> Float { return kern::math::min(y_lo, y); }, y_lo, vector_break(p).y);
      y_hi = kern::formula([](Float y_hi, Float y) -> Float { return kern::math::max(y_hi, y); }, y_hi, vector_break(p).y);
      x_lo = kern::formula([](Float x_lo, Float x) -> Float { return kern::math::min(x_lo, x); }, x_lo, vector_break(p).x);
      x_hi = kern::formula([](Float x_hi, Float x) -> Float { return kern::math::max(x_hi, x); }, x_hi, vector_break(p).x);
    }
    xs = List<Float>{};
    j0 = kern::to<Int>(kern::formula([](Float y_lo, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y_lo, kWaterCell))); }, y_lo, kWaterCell));
    j1 = kern::to<Int>(kern::formula([](Float y_hi, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y_hi, kWaterCell))); }, y_hi, kWaterCell));
    for (auto it77 = flow_for(n77, j0, j1); it77.next();) {
      y = kern::formula([](Float index, Float kWaterCell) -> Float { return ((index + 0.5) * kWaterCell); }, kern::to<Float>(n77.index), kWaterCell);
      list_clear(xs);
      for (auto it78 = flow_for(n78, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(rings))))); it78.next();) {
        a = 0;
        c = kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(rings[n78.index]).items))));
        while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(a), kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(rings[n78.index]).items))), 0.5))) {
          ya = vector_break(rings[n78.index].items[a]).y;
          yc = vector_break(rings[n78.index].items[c]).y;
          if (math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(ya, y)))), kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(yc, y)))))) {
            list_add(xs, kern::formula([](Float x, Float y, Float ya, Float yc, Float x_2) -> Float { return (x + (kern::math::div((y - ya), (yc - ya)) * (x_2 - x))); }, vector_break(rings[n78.index].items[a]).x, y, ya, yc, vector_break(rings[n78.index].items[c]).x));
          }
          was = a;
          a += 1;
          c = was;
        }
      }
      list_sort<nullptr>(xs);
      k = 0;
      while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Float>(xs))), 0.5))) {
        ia = kern::to<Int>(kern::formula([](Float a, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::ceil((kern::math::div(a, kWaterCell) - 0.5))); }, xs[k], kWaterCell));
        ib = kern::to<Int>(kern::formula([](Float a, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::floor((kern::math::div(a, kWaterCell) - 0.5))); }, xs[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))], kWaterCell));
        for (auto it79 = flow_for(n79, ia, ib); it79.next();) {
          Terrain_set_water_cell(self, n79.index, n77.index);
        }
        k += 2;
      }
    }
    Terrain_dirty(self, x_lo, y_lo, x_hi, y_hi, true);
  }

  void Terrain_add_waterway(Terrain& self, const List<Vector>& pts, Float width = 0.0, Float speed = 0.0) {
    Float x_lo = 0.0;
    Float y_lo = 0.0;
    Float x_hi = 0.0;
    Float y_hi = 0.0;
    Int k = 0;
    Bool skip = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    Float l = 0.0;
    Vector u = Vector{0.0, 0.0, 0.0};
    Float half = 0.0;
    Float reach = 0.0;
    Int i0 = 0;
    Int i1 = 0;
    Int j0 = 0;
    Int j1 = 0;
    Bool skip_2 = false;
    Float x = 0.0;
    Float y = 0.0;
    Float t = 0.0;
    Float d = 0.0;
    Int it = 0;
    Int count = 0;
    Vector fill = Vector{0.0, 0.0, 0.0};
    Int count_2 = 0;
    Float fill_2 = 0.0;
    Int idx = 0;
    Float sp = 0.0;
    x_lo = 1000000000.0;
    y_lo = 1000000000.0;
    x_hi = -1000000000.0;
    y_hi = -1000000000.0;
    k = 0;
    while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k))))), kern::to<Float>(list_length<Vector>(pts))), 0.5))) {
      skip = false;
      a = pts[k];
      c = pts[kern::to<Int>(kern::formula([](Float k) -> Float { return (k + 1.0); }, kern::to<Float>(k)))];
      l = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(c).x, vector_break(a).x, vector_break(c).y, vector_break(a).y);
      if (math_compare<"<">(l, 0.2)) {
        skip = true;
      }
      if (logic_not(skip)) {
        u = vector_make(kern::formula([](Float x, Float x_2, Float l) -> Float { return kern::math::div((x - x_2), l); }, vector_break(c).x, vector_break(a).x, l), kern::formula([](Float y, Float y_2, Float l) -> Float { return kern::math::div((y - y_2), l); }, vector_break(c).y, vector_break(a).y, l), 0.0);
        half = kern::formula([](Float width) -> Float { return kern::math::div(width, 2.0); }, width);
        reach = kern::formula([](Float half) -> Float { return (half + 30.0); }, half);
        i0 = kern::to<Int>(kern::formula([](Float x, Float x_2, Float reach, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((kern::math::min(x, x_2) - reach), kWaterCell))); }, vector_break(a).x, vector_break(c).x, reach, kWaterCell));
        i1 = kern::to<Int>(kern::formula([](Float x, Float x_2, Float reach, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((kern::math::max(x, x_2) + reach), kWaterCell))); }, vector_break(a).x, vector_break(c).x, reach, kWaterCell));
        j0 = kern::to<Int>(kern::formula([](Float y, Float y_2, Float reach, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((kern::math::min(y, y_2) - reach), kWaterCell))); }, vector_break(a).y, vector_break(c).y, reach, kWaterCell));
        j1 = kern::to<Int>(kern::formula([](Float y, Float y_2, Float reach, Float kWaterCell) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((kern::math::max(y, y_2) + reach), kWaterCell))); }, vector_break(a).y, vector_break(c).y, reach, kWaterCell));
        x_lo = kern::formula([](Float x_lo, Float x, Float x_2, Float half) -> Float { return kern::math::min(x_lo, (kern::math::min(x, x_2) - half)); }, x_lo, vector_break(a).x, vector_break(c).x, half);
        x_hi = kern::formula([](Float x_hi, Float x, Float x_2, Float half) -> Float { return kern::math::max(x_hi, (kern::math::max(x, x_2) + half)); }, x_hi, vector_break(a).x, vector_break(c).x, half);
        y_lo = kern::formula([](Float y_lo, Float y, Float y_2, Float half) -> Float { return kern::math::min(y_lo, (kern::math::min(y, y_2) - half)); }, y_lo, vector_break(a).y, vector_break(c).y, half);
        y_hi = kern::formula([](Float y_hi, Float y, Float y_2, Float half) -> Float { return kern::math::max(y_hi, (kern::math::max(y, y_2) + half)); }, y_hi, vector_break(a).y, vector_break(c).y, half);
        for (auto it80 = flow_for(n80, i0, i1); it80.next();) {
          for (auto it81 = flow_for(n81, j0, j1); it81.next();) {
            skip_2 = false;
            x = kern::formula([](Float index, Float kWaterCell) -> Float { return ((index + 0.5) * kWaterCell); }, kern::to<Float>(n80.index), kWaterCell);
            y = kern::formula([](Float index, Float kWaterCell) -> Float { return ((index + 0.5) * kWaterCell); }, kern::to<Float>(n81.index), kWaterCell);
            t = kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::formula([](Float x, Float x_2, Float x_3, Float y, Float y_2, Float y_3) -> Float { return (((x - x_2) * x_3) + ((y - y_2) * y_3)); }, x, vector_break(a).x, vector_break(u).x, y, vector_break(a).y, vector_break(u).y), 0.0, l));
            d = kern::formula([](Float x, Float x_2, Float x_3, Float t, Float y, Float y_2, Float y_3) -> Float { return kern::math::sqrt((((x - x_2) - (x_3 * t)) * ((x - x_2) - (x_3 * t))) + (((y - y_2) - (y_3 * t)) * ((y - y_2) - (y_3 * t)))); }, x, vector_break(a).x, vector_break(u).x, t, y, vector_break(a).y, vector_break(u).y);
            if (math_compare<"<=">(d, half)) {
              Terrain_set_water_cell(self, n80.index, n81.index);
            }
            if (math_compare<"<">(d, reach)) {
              it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Terrain_WaterBlock>(self.water_, key2(kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::shr(index, 4.0); }, kern::to<Float>(n80.index))), kern::to<Int>(kern::formula([](Float index) -> Float { return kern::math::shr(index, 4.0); }, kern::to<Float>(n81.index)))).Result).index), kern::to<Float>(map_length<Int64, Terrain_WaterBlock>(self.water_))));
              if (math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Terrain_WaterBlock>(self.water_)))) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                if (math_compare<"==">(kern::to<Float>(list_length<Vector>(self.water_[it].flow)), 0.0)) {
                  count = 256;
                  fill = Vector{0.0, 0.0, 0.0};
                  list_clear(self.water_[it].flow);
                  list_resize(self.water_[it].flow, count, fill);
                  count_2 = 256;
                  fill_2 = 0.0;
                  list_clear(self.water_[it].speed);
                  list_resize(self.water_[it].speed, count_2, fill_2);
                }
                idx = kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return kern::math::wrap32(((kern::math::band(index, 15.0) * 16.0) + kern::math::band(index_2, 15.0))); }, kern::to<Float>(n81.index), kern::to<Float>(n80.index)));
                sp = kern::formula([](Float speed, Float d, Float reach) -> Float { return (speed * (1.0 - kern::math::div(d, reach))); }, speed, d, reach);
                if (math_compare<"<">(self.water_[it].speed[idx], sp)) {
                  self.water_[it].flow[idx] = u;
                  self.water_[it].speed[idx] = sp;
                }
              }
            }
          }
        }
      }
      k += 1;
    }
    if (math_compare<"<">(x_lo, x_hi)) {
      Terrain_dirty(self, x_lo, y_lo, x_hi, y_hi, true);
    }
  }

  void Terrain_add_bridge(Terrain& self, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, Float width = 0.0) {
    Float l = 0.0;
    Float r = 0.0;
    l = kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(b).x, vector_break(a).x, vector_break(b).y, vector_break(a).y);
    if (math_compare<"<">(l, 0.5)) {
      return;
    }
    list_add(self.bridges_, struct_make<Terrain_Bridge>(vector_scale(vector_add(a, b), 0.5), kern::formula([](Float l) -> Float { return (kern::math::div(l, 2.0) + 1.0); }, l), kern::formula([](Float width) -> Float { return (kern::math::div(width, 2.0) + 1.5); }, width), kern::formula([](Float y, Float y_2, Float x, Float x_2) -> Float { return kern::math::atan2((y - y_2), (x - x_2)); }, vector_break(b).y, vector_break(a).y, vector_break(b).x, vector_break(a).x)));
    r = kern::formula([](Float l, Float width) -> Float { return (kern::math::div(l, 2.0) + width); }, l, width);
    Terrain_dirty(self, kern::formula([](Float x, Float x_2, Float r) -> Float { return (kern::math::div((x + x_2), 2.0) - r); }, vector_break(a).x, vector_break(b).x, r), kern::formula([](Float y, Float y_2, Float r) -> Float { return (kern::math::div((y + y_2), 2.0) - r); }, vector_break(a).y, vector_break(b).y, r), kern::formula([](Float x, Float x_2, Float r) -> Float { return (kern::math::div((x + x_2), 2.0) + r); }, vector_break(a).x, vector_break(b).x, r), kern::formula([](Float y, Float y_2, Float r) -> Float { return (kern::math::div((y + y_2), 2.0) + r); }, vector_break(a).y, vector_break(b).y, r), true);
  }

  struct albedoResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  /// Satellite RGB -> albedo (world_ground.mjs albedo): lift shadows, remove haze, plausible ranges
  KERN_PURE albedoResult albedo(Vector c = Vector{0.0, 0.0, 0.0}) {
    Float l = 0.0;
    Float target = 0.0;
    Float k = 0.0;
    Float R = 0.0;
    Float G = 0.0;
    Float B = 0.0;
    Float m = 0.0;
    Float sat = 0.0;
    l = kern::formula([](Float x, Float y, Float z) -> Float { return (((0.2126 * x) + (0.7152 * y)) + (0.0722 * z)); }, vector_break(c).x, vector_break(c).y, vector_break(c).z);
    target = kern::formula([](Float l) -> Float { return kern::math::min(0.24, kern::math::max(0.07, ((l * 0.8) + 0.03))); }, l);
    k = kern::formula([](Float target, Float l) -> Float { return kern::math::div(target, kern::math::max(0.02, l)); }, target, l);
    R = kern::formula([](Float x, Float k) -> Float { return (x * k); }, vector_break(c).x, k);
    G = kern::formula([](Float y, Float k) -> Float { return (y * k); }, vector_break(c).y, k);
    B = kern::formula([](Float z, Float k) -> Float { return (z * k); }, vector_break(c).z, k);
    m = kern::formula([](Float r, Float g, Float b) -> Float { return kern::math::div(((r + g) + b), 3.0); }, R, G, B);
    sat = kern::formula([](Float r, Float g, Float b) -> Float { return (kern::math::max(r, kern::math::max(g, b)) - kern::math::min(r, kern::math::min(g, b))); }, R, G, B);
    if (math_compare<"<">(sat, 0.05)) {
      R = kern::formula([](Float r, Float m) -> Float { return ((r * 0.85) + (m * 0.15)); }, R, m);
      G = kern::formula([](Float g, Float m) -> Float { return ((g * 0.85) + (m * 0.15)); }, G, m);
      B = kern::formula([](Float b, Float m) -> Float { return ((b * 0.85) + (m * 0.15)); }, B, m);
    }
    if (logic_and(logic_and(math_compare<">">(B, G), math_compare<">">(B, R)), math_compare<">">(sat, 0.04))) {
      B = kern::formula([](Float b) -> Float { return (b * 0.85); }, B);
    }
    return {vector_make(kern::formula([](Float r) -> Float { return kern::math::min(0.45, r); }, R), kern::formula([](Float g) -> Float { return kern::math::min(0.45, g); }, G), kern::formula([](Float b) -> Float { return kern::math::min(0.45, b); }, B))};
  }

  struct srgb_lin8Result { Float Result = 0.0; };
  KERN_PURE srgb_lin8Result srgb_lin8(Int v = 0) {
    Float x = 0.0;
    x = kern::formula([](Float v) -> Float { return kern::math::div(v, 255.0); }, kern::to<Float>(v));
    return {kern::formula([](Float x) -> Float { return ((x <= 0.04045) ? kern::math::div(x, 12.92) : kern::math::pow(kern::math::div((x + 0.055), 1.055), 2.4)); }, x)};
  }

  struct Terrain_take_image_requestsResult { Text Result = ""; };
  Terrain_take_image_requestsResult Terrain_take_image_requests(Terrain& self) {
    Text s = "";
    Text b = "";
    s = "[";
    for (auto it82 = flow_for(n82, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(list_length<TileKey>(self.image_requests_))))); it82.next();) {
      b = "";
      b = text_format<"%s[%d,%d,%d]">(63, logic_select<Text>(math_compare<"!=">(kern::to<Float>(n82.index), 0.0), ",", ""), self.image_requests_[n82.index].z, self.image_requests_[n82.index].x, self.image_requests_[n82.index].y);
      s = text_append(s, b, "");
    }
    list_clear(self.image_requests_);
    return {text_append(s, "]", "")};
  }

  struct Terrain_image_boundsResult { Vec4 Result = Vec4{0.0, 0.0, 0.0, 0.0}; };
  Terrain_image_boundsResult Terrain_image_bounds(const Terrain& self, const TileKey& k) {
    Float lon0 = 0.0;
    Float lat0 = 0.0;
    Float lon1 = 0.0;
    Float lat1 = 0.0;
    Float ax = 0.0;
    Float ay = 0.0;
    Float bx = 0.0;
    Float by = 0.0;
    lon0 = 0.0;
    lat0 = 0.0;
    lon1 = 0.0;
    lat1 = 0.0;
    tiles_cpp::tile_lonlat(k, lon0, lat0, 0.0, 0.0);
    tiles_cpp::tile_lonlat(k, lon1, lat1, 1.0, 1.0);
    ax = 0.0;
    ay = 0.0;
    bx = 0.0;
    by = 0.0;
    engine_geo_roads::GeoProjector_project(self.proj_, ax, ay, lon0, lat0);
    engine_geo_roads::GeoProjector_project(self.proj_, bx, by, lon1, lat1);
    return {engine_math_math::Vec4_new(kern::formula([](Float ax, Float bx) -> Float { return kern::math::min(ax, bx); }, ax, bx), kern::formula([](Float ay, Float by) -> Float { return kern::math::min(ay, by); }, ay, by), kern::formula([](Float ax, Float bx) -> Float { return kern::math::max(ax, bx); }, ax, bx), kern::formula([](Float ay, Float by) -> Float { return kern::math::max(ay, by); }, ay, by)).Result};
  }

  void Terrain_repaint(Terrain& self, const Vec4& b) {
    Int level = 0;
    Int i = 0;
    Int j = 0;
    Float size = 0.0;
    Int64 key = Int64{0x0};
    for (auto it89 = flow_for(n89, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Terrain_RenderTile>(self.render_))))); it89.next();) {
      level = 0;
      i = 0;
      j = 0;
      unkey3(level, i, j, map_key_at<Int64, Terrain_RenderTile>(self.render_, n89.index));
      size = kern::formula([](Float level, Float kBlock, Float kTile) -> Float { return ((level == 2.0) ? kBlock : kTile); }, kern::to<Float>(level), kBlock, kTile);
      if (logic_and(logic_and(logic_and(math_compare<"<=">(kern::formula([](Float i, Float size) -> Float { return (i * size); }, kern::to<Float>(i), size), b.z), math_compare<">=">(kern::formula([](Float i, Float size) -> Float { return ((i + 1.0) * size); }, kern::to<Float>(i), size), b.x)), math_compare<"<=">(kern::formula([](Float j, Float size) -> Float { return (j * size); }, kern::to<Float>(j), size), b.w)), math_compare<">=">(kern::formula([](Float j, Float size) -> Float { return ((j + 1.0) * size); }, kern::to<Float>(j), size), b.y))) {
        key = map_key_at<Int64, Terrain_RenderTile>(self.render_, n89.index);
        set_add(n90, self.dirty_render_, key);
      }
    }
  }

  void Terrain_plan_images(Terrain& self, Vector focus = Vector{0.0, 0.0, 0.0}) {
    Float half = 0.0;
    Float lon_a = 0.0;
    Float lat_a = 0.0;
    Float lon_b = 0.0;
    Float lat_b = 0.0;
    Int ix0 = 0;
    Int iy0 = 0;
    Int ix1 = 0;
    Int iy1 = 0;
    TileKey k = TileKey{14, 0, 0};
    Int64 key = Int64{0x0};
    Int64 key_2 = Int64{0x0};
    Terrain_Image t = Terrain_Image{TileKey{14, 0, 0}, List<Float>{}, false, false};
    Int64 key_3 = Int64{0x0};
    Terrain_Image put = Terrain_Image{TileKey{14, 0, 0}, List<Float>{}, false, false};
    Terrain_Image t_2 = Terrain_Image{TileKey{14, 0, 0}, List<Float>{}, false, false};
    Int64 key_4 = Int64{0x0};
    Terrain_Image put_2 = Terrain_Image{TileKey{14, 0, 0}, List<Float>{}, false, false};
    Int it = 0;
    Vec4 a = Vec4{0.0, 0.0, 0.0, 0.0};
    Int at = 0;
    if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, vector_break(focus).x, vector_break(self.image_at_).x, vector_break(focus).y, vector_break(self.image_at_).y), 350.0)) {
      return;
    }
    self.image_at_ = vector_make(kern::formula([](Float x) -> Float { return (kern::math::round(kern::math::div(x, 10.0)) * 10.0); }, vector_break(focus).x), kern::formula([](Float y) -> Float { return (kern::math::round(kern::math::div(y, 10.0)) * 10.0); }, vector_break(focus).y), 0.0);
    half = 800.0;
    lon_a = 0.0;
    lat_a = 0.0;
    lon_b = 0.0;
    lat_b = 0.0;
    tiles_cpp::unproject(self.proj_, lon_a, lat_a, kern::formula([](Float x, Float half) -> Float { return (x - half); }, vector_break(self.image_at_).x, half), kern::formula([](Float y, Float half) -> Float { return (y + half); }, vector_break(self.image_at_).y, half));
    tiles_cpp::unproject(self.proj_, lon_b, lat_b, kern::formula([](Float x, Float half) -> Float { return (x + half); }, vector_break(self.image_at_).x, half), kern::formula([](Float y, Float half) -> Float { return (y - half); }, vector_break(self.image_at_).y, half));
    ix0 = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::trunc(kern::math::floor(the_result)); }, merc_x(lon_a, kImageZoom).Result));
    iy0 = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::trunc(kern::math::floor(the_result)); }, merc_y(lat_a, kImageZoom).Result));
    ix1 = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::trunc(kern::math::floor(the_result)); }, merc_x(lon_b, kImageZoom).Result));
    iy1 = kern::to<Int>(kern::formula([](Float the_result) -> Float { return kern::math::trunc(kern::math::floor(the_result)); }, merc_y(lat_b, kImageZoom).Result));
    if (math_compare<">">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float ix1, Float ix0, Float iy1, Float iy0) -> Float { return (((ix1 - ix0) + 1.0) * ((iy1 - iy0) + 1.0)); }, kern::to<Float>(ix1), kern::to<Float>(ix0), kern::to<Float>(iy1), kern::to<Float>(iy0))))), 25.0)) {
      return;
    }
    for (auto it83 = flow_for(n83, iy0, iy1); it83.next();) {
      for (auto it84 = flow_for(n84, ix0, ix1); it84.next();) {
        k = struct_make<TileKey>(kImageZoom, n84.index, n83.index);
        key = tiles::TileKey_hash(k).Result;
        map_add(n85, self.images_, key);
        key_2 = tiles::TileKey_hash(k).Result;
        map_add(n86, self.images_, key_2);
        if (logic_not(struct_break<Terrain_Image>(self.images_[n86.index]).requested)) {
          key_3 = tiles::TileKey_hash(k).Result;
          map_add(n87, self.images_, key_3);
          t = self.images_[n87.index];
          t.requested = true;
          put = t;
          self.images_[n87.index] = put;
          key_4 = tiles::TileKey_hash(k).Result;
          map_add(n88, self.images_, key_4);
          t_2 = self.images_[n88.index];
          t_2.key = k;
          put_2 = t_2;
          self.images_[n88.index] = put_2;
          list_add(self.image_requests_, k);
        }
      }
    }
    it = 0;
    while (loop(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Terrain_Image>(self.images_)))))) {
      if (logic_or(logic_or(logic_or(math_compare<"<">(kern::to<Float>(self.images_[it].key.x), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float ix0) -> Float { return (ix0 - 2.0); }, kern::to<Float>(ix0)))))), math_compare<">">(kern::to<Float>(self.images_[it].key.x), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float ix1) -> Float { return (ix1 + 2.0); }, kern::to<Float>(ix1))))))), math_compare<"<">(kern::to<Float>(self.images_[it].key.y), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float iy0) -> Float { return (iy0 - 2.0); }, kern::to<Float>(iy0))))))), math_compare<">">(kern::to<Float>(self.images_[it].key.y), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float iy1) -> Float { return (iy1 + 2.0); }, kern::to<Float>(iy1)))))))) {
        if (logic_not(math_compare<"==">(kern::to<Float>(list_length<Float>(self.images_[it].albedo)), 0.0))) {
          n91 = Terrain_image_bounds(self, self.images_[it].key);
          a = n91.Result;
          Terrain_repaint(self, a);
        }
        at = it;
        map_remove_at(self.images_, at);
        it = at;
      } else {
        it += 1;
      }
    }
  }

  void Terrain_provide_image(Terrain& self, const TileKey& key, const List<Int>& rgba, Int width = 0, Int height = 0) {
    Int it = 0;
    Int P = 0;
    List<Int> small = List<Int>{};
    List<Int> made = List<Int>{};
    Int x0 = 0;
    Int x1 = 0;
    Int y0 = 0;
    Int y1 = 0;
    Fixed<Int, 4> sum = Fixed<Int, 4>{};
    Int n = 0;
    Int at = 0;
    Int count = 0;
    Float fill = 0.0;
    Vector s = Vector{0.0, 0.0, 0.0};
    Int n_2 = 0;
    Bool skip = false;
    Int qx = 0;
    Int qy = 0;
    Int at_2 = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vec4 a_2 = Vec4{0.0, 0.0, 0.0, 0.0};
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Terrain_Image>(self.images_, tiles::TileKey_hash(key).Result).index), kern::to<Float>(map_length<Int64, Terrain_Image>(self.images_))));
    if (logic_or(logic_or(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Terrain_Image>(self.images_))), math_compare<"==">(kern::to<Float>(width), 0.0)), math_compare<"==">(kern::to<Float>(height), 0.0))) {
      return;
    }
    P = kern::to<Int>(kern::formula([](Float kImagePixels) -> Float { return kern::math::wrap32(kImagePixels); }, kern::to<Float>(kImagePixels)));
    made = List<Int>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float p) -> Float { return kern::math::mul32(kern::math::mul32(p, p), 4.0); }, kern::to<Float>(P))), 0);
    small = made;
    for (auto it92 = flow_for(n92, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(P)))); it92.next();) {
      for (auto it93 = flow_for(n93, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(P)))); it93.next();) {
        x0 = kern::to<Int>(kern::formula([](Float index, Float width, Float p) -> Float { return kern::math::trunc(kern::math::div(kern::math::mul32(index, width), p)); }, kern::to<Float>(n93.index), kern::to<Float>(width), kern::to<Float>(P)));
        x1 = kern::to<Int>(kern::formula([](Float x0, Float index, Float width, Float p) -> Float { return kern::math::max(kern::math::wrap32(x0 + 1.0), kern::math::trunc(kern::math::div(kern::math::mul32(kern::math::wrap32(index + 1.0), width), p))); }, kern::to<Float>(x0), kern::to<Float>(n93.index), kern::to<Float>(width), kern::to<Float>(P)));
        y0 = kern::to<Int>(kern::formula([](Float index, Float height, Float p) -> Float { return kern::math::trunc(kern::math::div(kern::math::mul32(index, height), p)); }, kern::to<Float>(n92.index), kern::to<Float>(height), kern::to<Float>(P)));
        y1 = kern::to<Int>(kern::formula([](Float y0, Float index, Float height, Float p) -> Float { return kern::math::max(kern::math::wrap32(y0 + 1.0), kern::math::trunc(kern::math::div(kern::math::mul32(kern::math::wrap32(index + 1.0), height), p))); }, kern::to<Float>(y0), kern::to<Float>(n92.index), kern::to<Float>(height), kern::to<Float>(P)));
        sum = List<Int>{};
        n = 0;
        for (auto it94 = flow_for(n94, y0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(y1)))); it94.next();) {
          for (auto it95 = flow_for(n95, x0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(x1)))); it95.next();) {
            at = kern::to<Int>(kern::formula([](Float index, Float width, Float index_2) -> Float { return (((index * width) + index_2) * 4.0); }, kern::to<Float>(n94.index), kern::to<Float>(width), kern::to<Float>(n95.index)));
            for (auto it96 = flow_for(n96, 0, 3); it96.next();) {
              sum[n96.index] = kern::to<Int>(kern::formula([](Float a, Float a_2) -> Float { return kern::math::wrap32(a + a_2); }, kern::to<Float>(sum[n96.index]), kern::to<Float>(rgba[kern::to<Int>(kern::formula([](Float at, Float index) -> Float { return (at + index); }, kern::to<Float>(at), kern::to<Float>(n96.index)))])));
            }
            n = kern::to<Int>(kern::formula([](Float n) -> Float { return kern::math::wrap32(n + 1.0); }, kern::to<Float>(n)));
          }
        }
        for (auto it97 = flow_for(n97, 0, 3); it97.next();) {
          small[kern::to<Int>(kern::formula([](Float index, Float p, Float index_2, Float index_3) -> Float { return kern::math::wrap32(kern::math::mul32(kern::math::wrap32(kern::math::mul32(index, p) + index_2), 4.0) + kern::math::wrap32(index_3)); }, kern::to<Float>(n92.index), kern::to<Float>(P), kern::to<Float>(n93.index), kern::to<Float>(n97.index)))] = kern::to<Int>(kern::formula([](Float a, Float n) -> Float { return kern::math::band(kern::math::trunc(kern::math::div(a, kern::math::max(1.0, n))), 255.0); }, kern::to<Float>(sum[n97.index]), kern::to<Float>(n)));
        }
      }
    }
    count = kern::to<Int>(kern::formula([](Float p) -> Float { return kern::math::mul32(kern::math::mul32(p, p), 3.0); }, kern::to<Float>(P)));
    fill = 0.0;
    list_clear(self.images_[it].albedo);
    list_resize(self.images_[it].albedo, count, fill);
    for (auto it98 = flow_for(n98, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(P)))); it98.next();) {
      for (auto it99 = flow_for(n99, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(P)))); it99.next();) {
        s = Vector{0.0, 0.0, 0.0};
        n_2 = 0;
        for (auto it100 = flow_for(n100, -1, 1); it100.next();) {
          for (auto it101 = flow_for(n101, -1, 1); it101.next();) {
            skip = false;
            qx = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return (kern::math::wrap32s(index) + index_2); }, kern::to<Float>(n99.index), kern::to<Float>(n101.index))))), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float p) -> Float { return kern::math::wrap32s(kern::math::wrap32(p - 1.0)); }, kern::to<Float>(P))))))));
            qy = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return (kern::math::wrap32s(index) + index_2); }, kern::to<Float>(n98.index), kern::to<Float>(n100.index))))), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float p) -> Float { return kern::math::wrap32s(kern::math::wrap32(p - 1.0)); }, kern::to<Float>(P))))))));
            at_2 = kern::to<Int>(kern::formula([](Float qy, Float p, Float qx) -> Float { return kern::math::mul32(kern::math::wrap32(kern::math::mul32(qy, p) + qx), 4.0); }, kern::to<Float>(qy), kern::to<Float>(P), kern::to<Float>(qx)));
            if (math_compare<"<">(kern::to<Float>(small[kern::to<Int>(kern::formula([](Float at_2) -> Float { return (at_2 + 3.0); }, kern::to<Float>(at_2)))]), 10.0)) {
              skip = true;
            }
            if (logic_not(skip)) {
              s = vector_add(s, vector_make(srgb_lin8(small[at_2]).Result, srgb_lin8(small[kern::to<Int>(kern::formula([](Float at_2) -> Float { return (at_2 + 1.0); }, kern::to<Float>(at_2)))]).Result, srgb_lin8(small[kern::to<Int>(kern::formula([](Float at_2) -> Float { return (at_2 + 2.0); }, kern::to<Float>(at_2)))]).Result));
              n_2 = kern::to<Int>(kern::formula([](Float n_2) -> Float { return kern::math::wrap32(n_2 + 1.0); }, kern::to<Float>(n_2)));
            }
          }
        }
        a = albedo(logic_select<Vector>(math_compare<"!=">(kern::to<Float>(n_2), 0.0), vector_scale(s, kern::formula([](Float n_2) -> Float { return kern::math::div(1.0, n_2); }, kern::to<Float>(n_2))), Vector{0.12, 0.16, 0.08})).Result;
        self.images_[it].albedo[kern::to<Int>(kern::formula([](Float index, Float p, Float index_2) -> Float { return kern::math::mul32(kern::math::wrap32(kern::math::mul32(index, p) + index_2), 3.0); }, kern::to<Float>(n98.index), kern::to<Float>(P), kern::to<Float>(n99.index)))] = vector_break(a).x;
        self.images_[it].albedo[kern::to<Int>(kern::formula([](Float index, Float p, Float index_2) -> Float { return kern::math::wrap32(kern::math::mul32(kern::math::wrap32(kern::math::mul32(index, p) + index_2), 3.0) + 1.0); }, kern::to<Float>(n98.index), kern::to<Float>(P), kern::to<Float>(n99.index)))] = vector_break(a).y;
        self.images_[it].albedo[kern::to<Int>(kern::formula([](Float index, Float p, Float index_2) -> Float { return kern::math::wrap32(kern::math::mul32(kern::math::wrap32(kern::math::mul32(index, p) + index_2), 3.0) + 2.0); }, kern::to<Float>(n98.index), kern::to<Float>(P), kern::to<Float>(n99.index)))] = vector_break(a).z;
      }
    }
    n102 = Terrain_image_bounds(self, key);
    a_2 = n102.Result;
    Terrain_repaint(self, a_2);
  }

  void Terrain_fail_image(Terrain& self, const TileKey& key) {
    Int it = 0;
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Terrain_Image>(self.images_, tiles::TileKey_hash(key).Result).index), kern::to<Float>(map_length<Int64, Terrain_Image>(self.images_))));
    if (logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Terrain_Image>(self.images_))))) {
      self.images_[it].failed = true;
    }
  }

  struct Terrain_ground_albedoResult { Bool Result = false; };
  Terrain_ground_albedoResult Terrain_ground_albedo(const Terrain& self, Vector& lin, Float x = 0.0, Float y = 0.0) {
    Float lon = 0.0;
    Float lat = 0.0;
    Float fx = 0.0;
    Float fy = 0.0;
    Int tx = 0;
    Int ty = 0;
    Int it = 0;
    TileKey a = TileKey{14, 0, 0};
    Int px = 0;
    Int py = 0;
    List<Float> texels = List<Float>{};
    Int at = 0;
    if (math_compare<"==">(kern::to<Float>(map_length<Int64, Terrain_Image>(self.images_)), 0.0)) {
      return {false};
    }
    lon = 0.0;
    lat = 0.0;
    tiles_cpp::unproject(self.proj_, lon, lat, x, y);
    fx = merc_x(lon, kImageZoom).Result;
    fy = merc_y(lat, kImageZoom).Result;
    tx = kern::to<Int>(kern::formula([](Float fx) -> Float { return kern::math::trunc(kern::math::floor(fx)); }, fx));
    ty = kern::to<Int>(kern::formula([](Float fy) -> Float { return kern::math::trunc(kern::math::floor(fy)); }, fy));
    a = struct_make<TileKey>(kImageZoom, tx, ty);
    it = kern::to<Int>(kern::formula([](Float at, Float count) -> Float { return at < 0.0 ? count : at; }, kern::to<Float>(map_find<Int64, Terrain_Image>(self.images_, tiles::TileKey_hash(a).Result).index), kern::to<Float>(map_length<Int64, Terrain_Image>(self.images_))));
    if (logic_or(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(map_length<Int64, Terrain_Image>(self.images_))), math_compare<"==">(kern::to<Float>(list_length<Float>(self.images_[it].albedo)), 0.0))) {
      return {false};
    }
    px = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float fx, Float tx, Float kImagePixels) -> Float { return kern::math::trunc(((fx - tx) * kImagePixels)); }, fx, kern::to<Float>(tx), kern::to<Float>(kImagePixels))))), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float kImagePixels) -> Float { return (kImagePixels - 1.0); }, kern::to<Float>(kImagePixels))))))));
    py = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::formula([](Float v, Float lo, Float hi) -> Float { return v < lo ? lo : (hi < v ? hi : v); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float fy, Float ty, Float kImagePixels) -> Float { return kern::math::trunc(((fy - ty) * kImagePixels)); }, fy, kern::to<Float>(ty), kern::to<Float>(kImagePixels))))), 0.0, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float kImagePixels) -> Float { return (kImagePixels - 1.0); }, kern::to<Float>(kImagePixels))))))));
    texels = self.images_[it].albedo;
    at = kern::to<Int>(kern::formula([](Float py, Float kImagePixels, Float px) -> Float { return (((py * kImagePixels) + px) * 3.0); }, kern::to<Float>(py), kern::to<Float>(kImagePixels), kern::to<Float>(px)));
    lin = vector_make(texels[at], texels[kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 1.0); }, kern::to<Float>(at)))], texels[kern::to<Int>(kern::formula([](Float at) -> Float { return (at + 2.0); }, kern::to<Float>(at)))]);
    return {true};
  }

  struct srgb_to_linearResult { Float Result = 0.0; };
  KERN_PURE srgb_to_linearResult srgb_to_linear(Float v = 0.0) {
    return {kern::formula([](Float v) -> Float { return ((v <= 0.04045) ? kern::math::div(v, 12.92) : kern::math::pow(kern::math::div((v + 0.055), 1.055), 2.4)); }, v)};
  }

  struct linear_to_srgbResult { Float Result = 0.0; };
  KERN_PURE linear_to_srgbResult linear_to_srgb(Float v = 0.0) {
    return {kern::formula([](Float v) -> Float { return ((v <= 0.0031308) ? (v * 12.92) : ((1.055 * kern::math::pow(v, kern::math::div(1.0, 2.4))) - 0.055)); }, v)};
  }

  struct to_linearResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE to_linearResult to_linear(Vector c = Vector{0.0, 0.0, 0.0}) {
    return {vector_make(srgb_to_linear(vector_break(c).x).Result, srgb_to_linear(vector_break(c).y).Result, srgb_to_linear(vector_break(c).z).Result)};
  }

  struct to_srgbResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE to_srgbResult to_srgb(Vector c = Vector{0.0, 0.0, 0.0}) {
    return {vector_make(linear_to_srgb(vector_break(c).x).Result, linear_to_srgb(vector_break(c).y).Result, linear_to_srgb(vector_break(c).z).Result)};
  }

  void Terrain_add_tint(Terrain& self, const List<Vector_row>& rings, Int64 owner = Int64{0x0}, Vector srgb = Vector{0.0, 0.0, 0.0}) {
    Terrain_Tint t = Terrain_Tint{Int64{0x0}, Vec4{0.0, 0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, List<Vector_row>{}};
    Vector p = Vector{0.0, 0.0, 0.0};
    if (logic_or(math_compare<"==">(kern::to<Float>(list_length<Vector_row>(rings)), 0.0), math_compare<"<">(kern::to<Float>(list_length<Vector>(rings[0].items)), 3.0))) {
      return;
    }
    t = struct_make<Terrain_Tint>(owner, engine_math_math::Vec4_new(1e+30, 1e+30, -1e+30, -1e+30).Result, to_linear(srgb).Result, rings);
    for (auto it103 = flow_for(n103, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(rings[0].items))))); it103.next();) {
      p = rings[0].items[n103.index];
      t.box.x = kern::formula([](Float x, Float x_2) -> Float { return kern::math::min(x, x_2); }, t.box.x, vector_break(p).x);
      t.box.y = kern::formula([](Float y, Float y_2) -> Float { return kern::math::min(y, y_2); }, t.box.y, vector_break(p).y);
      t.box.z = kern::formula([](Float z, Float x) -> Float { return kern::math::max(z, x); }, t.box.z, vector_break(p).x);
      t.box.w = kern::formula([](Float w, Float y) -> Float { return kern::math::max(w, y); }, t.box.w, vector_break(p).y);
    }
    Terrain_repaint(self, t.box);
    list_add(self.tints_, t);
  }

  void Terrain_remove_tints(Terrain& self, Int64 owner = Int64{0x0}) {
    Int it = 0;
    Bool skip = false;
    Int at = 0;
    it = 0;
    while (loop(logic_not(math_compare<"==">(kern::to<Float>(it), kern::to<Float>(list_length<Terrain_Tint>(self.tints_)))))) {
      skip = false;
      if (logic_not(int64_equal(self.tints_[it].owner, owner))) {
        it += 1;
        skip = true;
      }
      if (logic_not(skip)) {
        Terrain_repaint(self, self.tints_[it].box);
        at = it;
        list_remove_at(self.tints_, at);
        it = at;
      }
    }
  }

  void Terrain_add_crater(Terrain& self, Vector at = Vector{0.0, 0.0, 0.0}, Float radius = 0.0, Float depth = 0.0) {
    Terrain_Crater old = Terrain_Crater{Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 0.0};
    if (logic_not(math_compare<">">(radius, 0.5))) {
      return;
    }
    list_add(self.craters_, struct_make<Terrain_Crater>(at, radius, depth, kern::formula([](Float depth) -> Float { return kern::math::div((depth * 7.0), 22.0); }, depth)));
    while (loop(math_compare<">">(kern::to<Float>(list_length<Terrain_Crater>(self.craters_)), 32.0))) {
      old = self.craters_[0];
      list_remove_at(self.craters_, 0);
      Terrain_dirty(self, kern::formula([](Float x, Float r) -> Float { return (x - r); }, vector_break(old.c).x, old.r), kern::formula([](Float y, Float r) -> Float { return (y - r); }, vector_break(old.c).y, old.r), kern::formula([](Float x, Float r) -> Float { return (x + r); }, vector_break(old.c).x, old.r), kern::formula([](Float y, Float r) -> Float { return (y + r); }, vector_break(old.c).y, old.r), true);
    }
    self.stats_.craters = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Terrain_Crater>(self.craters_))));
    Terrain_dirty(self, kern::formula([](Float x, Float radius) -> Float { return (x - radius); }, vector_break(at).x, radius), kern::formula([](Float y, Float radius) -> Float { return (y - radius); }, vector_break(at).y, radius), kern::formula([](Float x, Float radius) -> Float { return (x + radius); }, vector_break(at).x, radius), kern::formula([](Float y, Float radius) -> Float { return (y + radius); }, vector_break(at).y, radius), true);
  }

  void the_Terrain_crater_list_(const Terrain& self, List<Terrain_CraterRec>& out) {
    list_clear(out);
    for (auto it104 = flow_for(n104, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_Crater>(self.craters_))))); it104.next();) {
      list_add(out, struct_make<Terrain_CraterRec>(vector_break(self.craters_[n104.index].c).x, vector_break(self.craters_[n104.index].c).y, self.craters_[n104.index].r, self.craters_[n104.index].depth));
    }
  }

  struct set_craters_hasResult { Bool Result = false; };
  KERN_PURE set_craters_hasResult set_craters_has(const List<Terrain_CraterRec>& v, const Terrain_CraterRec& c) {
    Int at = 0;
    Int end = 0;
    Bool go = false;
    at = 0;
    end = list_length<Terrain_CraterRec>(v);
    go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
    while (loop(go)) {
      if (terrain::CraterRec_equal(v[at], c).Result) {
        go = false;
      } else {
        at += 1;
        go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      }
    }
    return {logic_not(math_compare<"==">(kern::to<Float>(at), kern::to<Float>(list_length<Terrain_CraterRec>(v))))};
  }

  void Terrain_set_craters(Terrain& self, const List<Terrain_CraterRec>& list) {
    List<Terrain_CraterRec> mine = List<Terrain_CraterRec>{};
    mine = List<Terrain_CraterRec>{};
    the_Terrain_crater_list_(self, mine);
    if (list_equal<Terrain_CraterRec>(mine, list)) {
      return;
    }
    for (auto it105 = flow_for(n105, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_CraterRec>(mine))))); it105.next();) {
      if (logic_not(set_craters_has(list, mine[n105.index]).Result)) {
        Terrain_dirty(self, kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, mine[n105.index].x, mine[n105.index].r), kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, mine[n105.index].y, mine[n105.index].r), kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, mine[n105.index].x, mine[n105.index].r), kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, mine[n105.index].y, mine[n105.index].r), true);
      }
    }
    for (auto it106 = flow_for(n106, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_CraterRec>(list))))); it106.next();) {
      if (logic_not(set_craters_has(mine, list[n106.index]).Result)) {
        Terrain_dirty(self, kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, list[n106.index].x, list[n106.index].r), kern::formula([](Float a, Float a_2) -> Float { return (a - a_2); }, list[n106.index].y, list[n106.index].r), kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, list[n106.index].x, list[n106.index].r), kern::formula([](Float a, Float a_2) -> Float { return (a + a_2); }, list[n106.index].y, list[n106.index].r), true);
      }
    }
    list_clear(self.craters_);
    for (auto it107 = flow_for(n107, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_CraterRec>(list))))); it107.next();) {
      if (logic_and(logic_and(logic_and(math_compare<">">(list[n107.index].r, 0.5), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, list[n107.index].x), 0.0)), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, list[n107.index].y), 0.0)), math_compare<"==">(kern::formula([](Float x) -> Float { return x - x; }, list[n107.index].depth), 0.0))) {
        list_add(self.craters_, struct_make<Terrain_Crater>(vector_make(list[n107.index].x, list[n107.index].y, 0.0), list[n107.index].r, list[n107.index].depth, kern::formula([](Float a) -> Float { return kern::math::div((a * 7.0), 22.0); }, list[n107.index].depth)));
      }
    }
    self.stats_.craters = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Terrain_Crater>(self.craters_))));
  }

  struct Terrain_has_collisionResult { Bool Result = false; };
  KERN_PURE Terrain_has_collisionResult Terrain_has_collision(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    Int64 k = Int64{0x0};
    k = key2(kern::to<Int>(kern::formula([](Float x, Float kTile) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kTile))); }, x, kTile)), kern::to<Int>(kern::formula([](Float y, Float kTile) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kTile))); }, y, kTile))).Result;
    return {logic_and(math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(map_contains<Int64, Terrain_CollisionTile>(self.collision_, k)))), 0.0), logic_not(math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(set_contains<Int64>(self.dirty_collision_, k)))), 0.0)))};
  }

  struct Terrain_ground_readyResult { Bool Result = false; };
  KERN_PURE Terrain_ground_readyResult Terrain_ground_ready(const Terrain& self, Float x = 0.0, Float y = 0.0) {
    Vec4 a = Vec4{0.0, 0.0, 0.0, 0.0};
    a = engine_math_math::Vec4_new(x, y, x, y).Result;
    if (logic_or(logic_not(Terrain_covers(self, a).Result), logic_not(Terrain_corridors_at(self, x, y).Result))) {
      return {false};
    }
    return {Terrain_has_collision(self, x, y).Result};
  }

  struct want_tiles_finish_matchResult { Bool Result = false; };
  KERN_PURE want_tiles_finish_matchResult want_tiles_finish_match(const WantedTile& a, const WantedTile& b) {
    return {math_compare<"<">(a.d, b.d)};
  }

  void want_tiles_finish(List<WantedTile>& v, List<Int64>& out) {
    Int at = 0;
    Int end = 0;
    Bool go = false;
    list_sort<&terrain_cpp::want_tiles_finish_match>(*this, v);
    list_clear(out);
    for (auto it119 = flow_for(n119, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<WantedTile>(v))))); it119.next();) {
      at = 0;
      end = list_length<Int64>(out);
      go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
      while (loop(go)) {
        if (int64_equal(out[at], v[n119.index].k)) {
          go = false;
        } else {
          at += 1;
          go = math_compare<"<">(kern::to<Float>(at), kern::to<Float>(end));
        }
      }
      if (math_compare<"==">(kern::to<Float>(at), kern::to<Float>(list_length<Int64>(out)))) {
        list_add(out, v[n119.index].k);
      }
    }
  }

  void Terrain_want_tiles(Terrain& self, Vector focus = Vector{0.0, 0.0, 0.0}, Vector anchor = Vector{0.0, 0.0, 0.0}) {
    List<WantedTile> render = List<WantedTile>{};
    List<WantedTile> coll = List<WantedTile>{};
    List<WantedTile> water = List<WantedTile>{};
    Int bi = 0;
    Int bj = 0;
    Bool skip = false;
    Float d = 0.0;
    Int ti = 0;
    Int tj = 0;
    Float td = 0.0;
    List<Vector_Float_pair> around = List<Vector_Float_pair>{};
    Int ci = 0;
    Int cj = 0;
    Int rr = 0;
    Float d_2 = 0.0;
    Set<Int64> chunks = Set<Int64>{};
    Int bx = 0;
    Int by = 0;
    Int ci_2 = 0;
    Int cj_2 = 0;
    Int64 key = Int64{0x0};
    Int64 k = Int64{0x0};
    Int i = 0;
    Int j = 0;
    Float d_3 = 0.0;
    render = List<WantedTile>{};
    coll = List<WantedTile>{};
    water = List<WantedTile>{};
    bi = kern::to<Int>(kern::formula([](Float x, Float kBlock) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kBlock))); }, vector_break(focus).x, kBlock));
    bj = kern::to<Int>(kern::formula([](Float y, Float kBlock) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kBlock))); }, vector_break(focus).y, kBlock));
    for (auto it108 = flow_for(n108, kern::to<Int>(kern::formula([](Float bj) -> Float { return (bj - 5.0); }, kern::to<Float>(bj))), kern::to<Int>(kern::formula([](Float bj) -> Float { return (bj + 5.0); }, kern::to<Float>(bj)))); it108.next();) {
      for (auto it109 = flow_for(n109, kern::to<Int>(kern::formula([](Float bi) -> Float { return (bi - 5.0); }, kern::to<Float>(bi))), kern::to<Int>(kern::formula([](Float bi) -> Float { return (bi + 5.0); }, kern::to<Float>(bi)))); it109.next();) {
        skip = false;
        d = rect_distance(vector_break(focus).x, vector_break(focus).y, kern::formula([](Float index, Float kBlock) -> Float { return (index * kBlock); }, kern::to<Float>(n109.index), kBlock), kern::formula([](Float index, Float kBlock) -> Float { return (index * kBlock); }, kern::to<Float>(n108.index), kBlock), kern::formula([](Float index, Float kBlock) -> Float { return ((index + 1.0) * kBlock); }, kern::to<Float>(n109.index), kBlock), kern::formula([](Float index, Float kBlock) -> Float { return ((index + 1.0) * kBlock); }, kern::to<Float>(n108.index), kBlock)).Result;
        if (math_compare<">">(d, 3200.0)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (math_compare<"<">(d, 200.0)) {
            for (auto it110 = flow_for(n110, 0, 3); it110.next();) {
              for (auto it111 = flow_for(n111, 0, 3); it111.next();) {
                ti = kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return ((index * 4.0) + index_2); }, kern::to<Float>(n109.index), kern::to<Float>(n111.index)));
                tj = kern::to<Int>(kern::formula([](Float index, Float index_2) -> Float { return ((index * 4.0) + index_2); }, kern::to<Float>(n108.index), kern::to<Float>(n110.index)));
                td = rect_distance(vector_break(focus).x, vector_break(focus).y, kern::formula([](Float ti, Float kTile) -> Float { return (ti * kTile); }, kern::to<Float>(ti), kTile), kern::formula([](Float tj, Float kTile) -> Float { return (tj * kTile); }, kern::to<Float>(tj), kTile), kern::formula([](Float ti, Float kTile) -> Float { return ((ti + 1.0) * kTile); }, kern::to<Float>(ti), kTile), kern::formula([](Float tj, Float kTile) -> Float { return ((tj + 1.0) * kTile); }, kern::to<Float>(tj), kTile)).Result;
                list_add(render, struct_make<WantedTile>(td, key3(kern::to<Int>(kern::formula([](Float td) -> Float { return ((td < 260.0) ? 0.0 : 1.0); }, td)), ti, tj).Result));
              }
            }
          } else {
            list_add(render, struct_make<WantedTile>(d, key3(2, n109.index, n108.index).Result));
          }
        }
      }
    }
    around = list_make<Vector_Float_pair, "2">(struct_make<Vector_Float_pair>(focus, 420.0), struct_make<Vector_Float_pair>(anchor, 200.0));
    for (auto it112 = flow_for(n112, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(self.extra))))); it112.next();) {
      list_add(around, struct_make<Vector_Float_pair>(self.extra[n112.index], 300.0));
    }
    for (auto it113 = flow_for(n113, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_Float_pair>(around))))); it113.next();) {
      ci = kern::to<Int>(kern::formula([](Float x, Float kTile) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kTile))); }, vector_break(around[n113.index].first).x, kTile));
      cj = kern::to<Int>(kern::formula([](Float y, Float kTile) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kTile))); }, vector_break(around[n113.index].first).y, kTile));
      rr = kern::to<Int>(kern::formula([](Float a, Float kTile) -> Float { return kern::math::trunc(kern::math::ceil(kern::math::div(a, kTile))); }, around[n113.index].second, kTile));
      for (auto it114 = flow_for(n114, kern::to<Int>(kern::formula([](Float cj, Float rr) -> Float { return (cj - rr); }, kern::to<Float>(cj), kern::to<Float>(rr))), kern::to<Int>(kern::formula([](Float cj, Float rr) -> Float { return (cj + rr); }, kern::to<Float>(cj), kern::to<Float>(rr)))); it114.next();) {
        for (auto it115 = flow_for(n115, kern::to<Int>(kern::formula([](Float ci, Float rr) -> Float { return (ci - rr); }, kern::to<Float>(ci), kern::to<Float>(rr))), kern::to<Int>(kern::formula([](Float ci, Float rr) -> Float { return (ci + rr); }, kern::to<Float>(ci), kern::to<Float>(rr)))); it115.next();) {
          d_2 = rect_distance(vector_break(around[n113.index].first).x, vector_break(around[n113.index].first).y, kern::formula([](Float index, Float kTile) -> Float { return (index * kTile); }, kern::to<Float>(n115.index), kTile), kern::formula([](Float index, Float kTile) -> Float { return (index * kTile); }, kern::to<Float>(n114.index), kTile), kern::formula([](Float index, Float kTile) -> Float { return ((index + 1.0) * kTile); }, kern::to<Float>(n115.index), kTile), kern::formula([](Float index, Float kTile) -> Float { return ((index + 1.0) * kTile); }, kern::to<Float>(n114.index), kTile)).Result;
          if (math_compare<"<=">(d_2, around[n113.index].second)) {
            list_add(coll, struct_make<WantedTile>(d_2, key2(n115.index, n114.index).Result));
          }
        }
      }
    }
    chunks = Set<Int64>{};
    for (auto it116 = flow_for(n116, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(map_length<Int64, Terrain_WaterBlock>(self.water_))))); it116.next();) {
      bx = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(map_key_at<Int64, Terrain_WaterBlock>(self.water_, n116.index), 32)))))))));
      by = kern::to<Int>(kern::formula([](Float a) -> Float { return a; }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(int64_and(map_key_at<Int64, Terrain_WaterBlock>(self.water_, n116.index), Int64{0xffffffff})))))))));
      ci_2 = kern::to<Int>(kern::formula([](Float bx, Float kWaterBlock, Float kWaterChunk) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((bx * kWaterBlock), kWaterChunk))); }, kern::to<Float>(bx), kWaterBlock, kWaterChunk));
      cj_2 = kern::to<Int>(kern::formula([](Float by, Float kWaterBlock, Float kWaterChunk) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div((by * kWaterBlock), kWaterChunk))); }, kern::to<Float>(by), kWaterBlock, kWaterChunk));
      key = key2(ci_2, cj_2).Result;
      set_add(n117, chunks, key);
    }
    for (auto it118 = flow_for(n118, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(set_length<Int64>(chunks))))); it118.next();) {
      k = set_key_at<Int64>(chunks, n118.index);
      i = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(int64_shift_right_unsigned(k, 32)))));
      j = kern::to<Int>(kern::formula([](Float x) -> Float { return kern::math::wrap32s(x); }, kern::to<Float>(int64_low(int64_and(k, Int64{0xffffffff})))));
      d_3 = rect_distance(vector_break(focus).x, vector_break(focus).y, kern::formula([](Float i, Float kWaterChunk) -> Float { return (i * kWaterChunk); }, kern::to<Float>(i), kWaterChunk), kern::formula([](Float j, Float kWaterChunk) -> Float { return (j * kWaterChunk); }, kern::to<Float>(j), kWaterChunk), kern::formula([](Float i, Float kWaterChunk) -> Float { return ((i + 1.0) * kWaterChunk); }, kern::to<Float>(i), kWaterChunk), kern::formula([](Float j, Float kWaterChunk) -> Float { return ((j + 1.0) * kWaterChunk); }, kern::to<Float>(j), kWaterChunk)).Result;
      if (math_compare<"<">(d_3, 2400.0)) {
        list_add(water, struct_make<WantedTile>(d_3, k));
      }
    }
    want_tiles_finish(render, self.want_render_);
    want_tiles_finish(coll, self.want_collision_);
    want_tiles_finish(water, self.want_water_);
    self.planned_focus_ = focus;
    self.planned_anchor_ = anchor;
    self.planned_extra_ = self.extra;
    self.replan_ = false;
  }

  struct build_render_tile_HResult { Float Result = 0.0; };
  KERN_PURE build_render_tile_HResult build_render_tile_H(const List<Float>& h, Int n = 0, Int i = 0, Int j = 0) {
    return {h[kern::to<Int>(kern::formula([](Float j, Float n, Float i) -> Float { return (((j + 1.0) * (n + 2.0)) + (i + 1.0)); }, kern::to<Float>(j), kern::to<Float>(n), kern::to<Float>(i)))]};
  }

  void build_render_tile_skirt(MeshData& m, Int cells = 0, Int n = 0, Float drop = 0.0, Int i0 = 0, Int j0 = 0, Int di = 0, Int dj = 0, Bool flip = false) {
    Int ia = 0;
    Int ja = 0;
    Int ib = 0;
    Int jb = 0;
    Int top_a = 0;
    Int top_b = 0;
    Int base = 0;
    Int item = 0;
    Int item_2 = 0;
    Fixed<Int, 2> items = Fixed<Int, 2>{};
    Int v = 0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Int place = 0;
    Int item_3 = 0;
    Int item_4 = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    Int item_7 = 0;
    Int item_8 = 0;
    Int place_2 = 0;
    Int item_9 = 0;
    Int item_10 = 0;
    Int item_11 = 0;
    Int item_12 = 0;
    Int item_13 = 0;
    Int item_14 = 0;
    for (auto it133 = flow_for(n133, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(cells)))); it133.next();) {
      ia = kern::to<Int>(kern::formula([](Float i0, Float di, Float index) -> Float { return (i0 + (di * index)); }, kern::to<Float>(i0), kern::to<Float>(di), kern::to<Float>(n133.index)));
      ja = kern::to<Int>(kern::formula([](Float j0, Float dj, Float index) -> Float { return (j0 + (dj * index)); }, kern::to<Float>(j0), kern::to<Float>(dj), kern::to<Float>(n133.index)));
      ib = kern::to<Int>(kern::formula([](Float ia, Float di) -> Float { return (ia + di); }, kern::to<Float>(ia), kern::to<Float>(di)));
      jb = kern::to<Int>(kern::formula([](Float ja, Float dj) -> Float { return (ja + dj); }, kern::to<Float>(ja), kern::to<Float>(dj)));
      top_a = kern::to<Int>(kern::formula([](Float ja, Float n, Float ia) -> Float { return kern::math::wrap32(((ja * n) + ia)); }, kern::to<Float>(ja), kern::to<Float>(n), kern::to<Float>(ia)));
      top_b = kern::to<Int>(kern::formula([](Float jb, Float n, Float ib) -> Float { return kern::math::wrap32(((jb * n) + ib)); }, kern::to<Float>(jb), kern::to<Float>(n), kern::to<Float>(ib)));
      base = engine_geo_mesh_data::MeshData_vertex_count(m).Result;
      item = top_a;
      item_2 = top_b;
      items = list_make<Int, "2">(item, item_2);
      for (auto it134 = flow_for(n134, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Int>(items))))); it134.next();) {
        v = items[n134.index];
        p = vector_make(m.positions[kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::mul32(v, 3.0); }, kern::to<Float>(v)))], m.positions[kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(kern::math::mul32(v, 3.0) + 1.0); }, kern::to<Float>(v)))], kern::formula([](Float a, Float drop) -> Float { return (a - drop); }, m.positions[kern::to<Int>(kern::formula([](Float v) -> Float { return kern::math::wrap32(kern::math::mul32(v, 3.0) + 2.0); }, kern::to<Float>(v)))], drop));
        engine_geo_mesh_data::MeshData_push(m, p, Vector{0.0, 0.0, 1.0}, kGround, vector_make(kern::formula([](Float x) -> Float { return (x * 0.1); }, vector_break(p).x), kern::formula([](Float y) -> Float { return (y * 0.1); }, vector_break(p).y), 0.0));
      }
      if (flip) {
        place = list_length<Int>(m.indices);
        item_3 = top_a;
        item_4 = base;
        item_5 = top_b;
        item_6 = top_b;
        item_7 = base;
        item_8 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 1.0); }, kern::to<Float>(base)));
        list_insert_list(m.indices, place, list_make<Int, "6">(item_3, item_4, item_5, item_6, item_7, item_8));
      } else {
        place_2 = list_length<Int>(m.indices);
        item_9 = top_a;
        item_10 = top_b;
        item_11 = base;
        item_12 = top_b;
        item_13 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 1.0); }, kern::to<Float>(base)));
        item_14 = base;
        list_insert_list(m.indices, place_2, list_make<Int, "6">(item_9, item_10, item_11, item_12, item_13, item_14));
      }
    }
  }

  void Terrain_build_render_tile(Terrain& self, Int level = 0, Int ti = 0, Int tj = 0) {
    Float size = 0.0;
    Int cells = 0;
    Float step = 0.0;
    Float x0 = 0.0;
    Float y0 = 0.0;
    Int n = 0;
    MeshData m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    List<Float> h = List<Float>{};
    List<Float> made = List<Float>{};
    List<Vector> tint = List<Vector>{};
    List<Vector> made_2 = List<Vector>{};
    List<Float> xs = List<Float>{};
    Float x1 = 0.0;
    Float y1 = 0.0;
    Bool skip = false;
    Bool skip_2 = false;
    Float y = 0.0;
    Int q = 0;
    Int r = 0;
    Int was = 0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Vector o = Vector{0.0, 0.0, 0.0};
    Int q_2 = 0;
    Int i0 = 0;
    Int i1 = 0;
    Float x = 0.0;
    Float y_2 = 0.0;
    Float z = 0.0;
    Float dx = 0.0;
    Float dy = 0.0;
    Vector nn = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    Int a = 0;
    Int b = 0;
    Int c_2 = 0;
    Int d = 0;
    Int place = 0;
    Int item = 0;
    Int item_2 = 0;
    Int item_3 = 0;
    Int item_4 = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    Float drop = 0.0;
    MeshLook look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    Int id = 0;
    Int allocate_id = 0;
    Bool upsert = false;
    Int64 key = Int64{0x0};
    Int64 key_2 = Int64{0x0};
    Int64 key_3 = Int64{0x0};
    Bool remove = false;
    Terrain_RenderTile t = Terrain_RenderTile{0, true};
    Int64 key_4 = Int64{0x0};
    Terrain_RenderTile put = Terrain_RenderTile{0, true};
    size = kern::formula([](Float level, Float kBlock, Float kTile) -> Float { return ((level == 2.0) ? kBlock : kTile); }, kern::to<Float>(level), kBlock, kTile);
    cells = kern::to<Int>(kern::formula([](Float level, Float kCells) -> Float { return ((level == 0.0) ? kCells : ((level == 1.0) ? 8.0 : 16.0)); }, kern::to<Float>(level), kern::to<Float>(kCells)));
    step = kern::formula([](Float size, Float cells) -> Float { return kern::math::div(size, cells); }, size, kern::to<Float>(cells));
    x0 = kern::formula([](Float ti, Float size) -> Float { return (ti * size); }, kern::to<Float>(ti), size);
    y0 = kern::formula([](Float tj, Float size) -> Float { return (tj * size); }, kern::to<Float>(tj), size);
    n = kern::to<Int>(kern::formula([](Float cells) -> Float { return (cells + 1.0); }, kern::to<Float>(cells)));
    m = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
    made = List<Float>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float n) -> Float { return ((n + 2.0) * (n + 2.0)); }, kern::to<Float>(n))), 0.0);
    h = made;
    for (auto it120 = flow_for(n120, -1, n); it120.next();) {
      for (auto it121 = flow_for(n121, -1, n); it121.next();) {
        h[kern::to<Int>(kern::formula([](Float index, Float n, Float index_2) -> Float { return (((index + 1.0) * (n + 2.0)) + (index_2 + 1.0)); }, kern::to<Float>(n120.index), kern::to<Float>(n), kern::to<Float>(n121.index)))] = Terrain_node_height(self, kern::formula([](Float x0, Float index, Float step) -> Float { return (x0 + (index * step)); }, x0, kern::to<Float>(n121.index), step), kern::formula([](Float y0, Float index, Float step) -> Float { return (y0 + (index * step)); }, y0, kern::to<Float>(n120.index), step)).Result;
      }
    }
    made_2 = List<Vector>{};
    list_resize(made_2, kern::to<Int>(kern::formula([](Float n) -> Float { return (n * n); }, kern::to<Float>(n))), Vector{0.0, 0.0, 0.0});
    tint = made_2;
    for (auto it122 = flow_for(n122, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it122.next();) {
      for (auto it123 = flow_for(n123, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it123.next();) {
        n124 = Terrain_ground_albedo(self, tint[kern::to<Int>(kern::formula([](Float index, Float n, Float index_2) -> Float { return ((index * n) + index_2); }, kern::to<Float>(n122.index), kern::to<Float>(n), kern::to<Float>(n123.index)))], kern::formula([](Float x0, Float index, Float step) -> Float { return (x0 + (index * step)); }, x0, kern::to<Float>(n123.index), step), kern::formula([](Float y0, Float index, Float step) -> Float { return (y0 + (index * step)); }, y0, kern::to<Float>(n122.index), step));
        if (logic_not(n124.Result)) {
          tint[kern::to<Int>(kern::formula([](Float index, Float n, Float index_2) -> Float { return ((index * n) + index_2); }, kern::to<Float>(n122.index), kern::to<Float>(n), kern::to<Float>(n123.index)))] = Vector{0.16, 0.2, 0.1};
        }
      }
    }
    xs = List<Float>{};
    x1 = kern::formula([](Float x0, Float size) -> Float { return (x0 + size); }, x0, size);
    y1 = kern::formula([](Float y0, Float size) -> Float { return (y0 + size); }, y0, size);
    for (auto it125 = flow_for(n125, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_Tint>(self.tints_))))); it125.next();) {
      skip = false;
      if (logic_or(logic_or(logic_or(math_compare<">">(self.tints_[n125.index].box.x, x1), math_compare<"<">(self.tints_[n125.index].box.z, x0)), math_compare<">">(self.tints_[n125.index].box.y, y1)), math_compare<"<">(self.tints_[n125.index].box.w, y0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it126 = flow_for(n126, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it126.next();) {
          skip_2 = false;
          y = kern::formula([](Float y0, Float index, Float step) -> Float { return (y0 + (index * step)); }, y0, kern::to<Float>(n126.index), step);
          if (logic_or(math_compare<"<">(y, self.tints_[n125.index].box.y), math_compare<">">(y, self.tints_[n125.index].box.w))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            list_clear(xs);
            for (auto it127 = flow_for(n127, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector_row>(self.tints_[n125.index].rings))))); it127.next();) {
              q = 0;
              r = kern::to<Int>(kern::formula([](Float a) -> Float { return (a - 1.0); }, kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(self.tints_[n125.index].rings[n127.index]).items))));
              while (loop(math_compare<">">(kern::formula([](Float q, Float a) -> Float { return ((q < 0.0) != (a < 0.0) ? (a < 0.0) : (q < a)); }, kern::to<Float>(q), kern::to<Float>(list_length<Vector>(struct_break<Vector_row>(self.tints_[n125.index].rings[n127.index]).items))), 0.5))) {
                p = self.tints_[n125.index].rings[n127.index].items[q];
                o = self.tints_[n125.index].rings[n127.index].items[r];
                if (math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(vector_break(p).y, y)))), kern::to<Float>(value_int(kern::to<Int>(math_compare<">">(vector_break(o).y, y)))))) {
                  list_add(xs, kern::formula([](Float x, Float y, Float y_2, Float x_2, Float y_3) -> Float { return (x + kern::math::div(((y - y_2) * (x_2 - x)), (y_3 - y_2))); }, vector_break(p).x, y, vector_break(p).y, vector_break(o).x, vector_break(o).y));
                }
                was = q;
                q += 1;
                r = was;
              }
            }
            list_sort<nullptr>(xs);
            q_2 = 0;
            while (loop(math_compare<">">(kern::formula([](Float a, Float a_2) -> Float { return ((a < 0.0) != (a_2 < 0.0) ? (a_2 < 0.0) : (a < a_2)); }, kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float q_2) -> Float { return (q_2 + 1.0); }, kern::to<Float>(q_2))))), kern::to<Float>(list_length<Float>(xs))), 0.5))) {
              i0 = kern::to<Int>(kern::formula([](Float a, Float x0, Float step) -> Float { return kern::math::max(0.0, kern::math::trunc(kern::math::ceil(kern::math::div((a - x0), step)))); }, xs[q_2], x0, step));
              i1 = kern::to<Int>(kern::formula([](Float n, Float a, Float x0, Float step) -> Float { return kern::math::min((n - 1.0), kern::math::trunc(kern::math::floor(kern::math::div((a - x0), step)))); }, kern::to<Float>(n), xs[kern::to<Int>(kern::formula([](Float q_2) -> Float { return (q_2 + 1.0); }, kern::to<Float>(q_2)))], x0, step));
              for (auto it128 = flow_for(n128, i0, i1); it128.next();) {
                tint[kern::to<Int>(kern::formula([](Float index, Float n, Float index_2) -> Float { return ((index * n) + index_2); }, kern::to<Float>(n126.index), kern::to<Float>(n), kern::to<Float>(n128.index)))] = vector_add(vector_scale(tint[kern::to<Int>(kern::formula([](Float index, Float n, Float index_2) -> Float { return ((index * n) + index_2); }, kern::to<Float>(n126.index), kern::to<Float>(n), kern::to<Float>(n128.index)))], 0.45), vector_scale(self.tints_[n125.index].lin, 0.55));
              }
              q_2 += 2;
            }
          }
        }
      }
    }
    for (auto it129 = flow_for(n129, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it129.next();) {
      for (auto it130 = flow_for(n130, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(n)))); it130.next();) {
        x = kern::formula([](Float x0, Float index, Float step) -> Float { return (x0 + (index * step)); }, x0, kern::to<Float>(n130.index), step);
        y_2 = kern::formula([](Float y0, Float index, Float step) -> Float { return (y0 + (index * step)); }, y0, kern::to<Float>(n129.index), step);
        z = build_render_tile_H(h, n, n130.index, n129.index).Result;
        dx = kern::formula([](Float the_result, Float the_result_2) -> Float { return (the_result - the_result_2); }, build_render_tile_H(h, n, kern::to<Int>(kern::formula([](Float index) -> Float { return (index + 1.0); }, kern::to<Float>(n130.index))), n129.index).Result, build_render_tile_H(h, n, kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n130.index))), n129.index).Result);
        dy = kern::formula([](Float the_result, Float the_result_2) -> Float { return (the_result - the_result_2); }, build_render_tile_H(h, n, n130.index, kern::to<Int>(kern::formula([](Float index) -> Float { return (index + 1.0); }, kern::to<Float>(n129.index)))).Result, build_render_tile_H(h, n, n130.index, kern::to<Int>(kern::formula([](Float index) -> Float { return (index - 1.0); }, kern::to<Float>(n129.index)))).Result);
        nn = logic_select<Vector>(math_compare<">">(vector_length(vector_make(kern::formula([](Float dx) -> Float { return (-dx); }, dx), kern::formula([](Float dy) -> Float { return (-dy); }, dy), kern::formula([](Float step) -> Float { return (2.0 * step); }, step))), 9.999999717180685e-10), vector_normalize(vector_make(kern::formula([](Float dx) -> Float { return (-dx); }, dx), kern::formula([](Float dy) -> Float { return (-dy); }, dy), kern::formula([](Float step) -> Float { return (2.0 * step); }, step))), Vector{0.0, 0.0, 1.0});
        c = to_srgb(tint[kern::to<Int>(kern::formula([](Float index, Float n, Float index_2) -> Float { return ((index * n) + index_2); }, kern::to<Float>(n129.index), kern::to<Float>(n), kern::to<Float>(n130.index)))]).Result;
        if (math_compare<"<">(z, kern::formula([](Float the_result) -> Float { return (the_result - 0.6); }, Terrain_elevation(self, x, y_2).Result))) {
          c = logic_select<Vector>(Terrain_water_at(self, x, y_2).Result, kBed, kSoil);
        } else if (math_compare<"==">(kern::to<Float>(Terrain_surface_at(self, x, y_2).Result), 2.0)) {
          c = kSoil;
        }
        engine_geo_mesh_data::MeshData_push(m, vector_make(x, y_2, z), nn, c, vector_make(kern::formula([](Float x) -> Float { return (x * 0.1); }, x), kern::formula([](Float y_2) -> Float { return (y_2 * 0.1); }, y_2), 0.0));
      }
    }
    for (auto it131 = flow_for(n131, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(cells)))); it131.next();) {
      for (auto it132 = flow_for(n132, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(cells)))); it132.next();) {
        a = kern::to<Int>(kern::formula([](Float index, Float n, Float index_2) -> Float { return kern::math::wrap32(((index * n) + index_2)); }, kern::to<Float>(n131.index), kern::to<Float>(n), kern::to<Float>(n132.index)));
        b = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(a)));
        c_2 = kern::to<Int>(kern::formula([](Float a, Float n) -> Float { return kern::math::wrap32(a + kern::math::wrap32(n)); }, kern::to<Float>(a), kern::to<Float>(n)));
        d = kern::to<Int>(kern::formula([](Float c_2) -> Float { return kern::math::wrap32(c_2 + 1.0); }, kern::to<Float>(c_2)));
        place = list_length<Int>(m.indices);
        item = a;
        item_2 = b;
        item_3 = d;
        item_4 = a;
        item_5 = d;
        item_6 = c_2;
        list_insert_list(m.indices, place, list_make<Int, "6">(item, item_2, item_3, item_4, item_5, item_6));
      }
    }
    drop = kern::formula([](Float level) -> Float { return ((level == 0.0) ? 2.0 : ((level == 1.0) ? 4.0 : 10.0)); }, kern::to<Float>(level));
    build_render_tile_skirt(m, cells, n, drop, 0, 0, 1, 0, false);
    build_render_tile_skirt(m, cells, n, drop, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n))), 1, 0, true);
    build_render_tile_skirt(m, cells, n, drop, 0, 0, 0, 1, true);
    build_render_tile_skirt(m, cells, n, drop, kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(n))), 0, 0, 1, false);
    look = MeshLook{Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}, 0, 0, true, 1, true};
    look.material.roughness = 1.0;
    look.material.cast_shadow = false;
    look.material.double_sided = true;
    engine_renderer_allocate_id(n135, self.r_);
    allocate_id = n135.result;
    id = allocate_id;
    engine_upsert(n136, self.r_, id, m, look, List<Float>{}, List<Float>{});
    upsert = n136.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, id);
      return;
    }
    key = key3(level, ti, tj).Result;
    map_add(n137, self.render_, key);
    key_2 = key3(level, ti, tj).Result;
    map_add(n138, self.render_, key_2);
    if (math_compare<"!=">(kern::to<Float>(struct_break<Terrain_RenderTile>(self.render_[n138.index]).mesh), 0.0)) {
      key_3 = key3(level, ti, tj).Result;
      map_add(n139, self.render_, key_3);
      engine_renderer_remove(n140, self.r_, struct_break<Terrain_RenderTile>(self.render_[n139.index]).mesh);
      remove = n140.result;
    }
    key_4 = key3(level, ti, tj).Result;
    map_add(n141, self.render_, key_4);
    t = self.render_[n141.index];
    t.mesh = id;
    put = t;
    self.render_[n141.index] = put;
  }

  void Terrain_build_collision_tile(Terrain& self, Int ti = 0, Int tj = 0) {
    Int N = 0;
    Float x0 = 0.0;
    Float y0 = 0.0;
    List<Vector> pts = List<Vector>{};
    List<Vector> made = List<Vector>{};
    Float x = 0.0;
    Float y = 0.0;
    Float z = 0.0;
    List<Int> idx = List<Int>{};
    Int r = 0;
    Int c = 0;
    Int a = 0;
    Int b = 0;
    Int cc = 0;
    Int d = 0;
    Int place = 0;
    Int item = 0;
    Int item_2 = 0;
    Int item_3 = 0;
    Int item_4 = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    physics_Body body = physics_Body{Int64{0x0}};
    physics_Body create_body = physics_Body{Int64{0x0}};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{Int64{0x0}};
    Int64 key = Int64{0x0};
    physics_Body a_2 = physics_Body{Int64{0x0}};
    Int64 key_2 = Int64{0x0};
    Bool body_valid = false;
    Bool body_valid_2 = false;
    Bool both = false;
    Int64 key_3 = Int64{0x0};
    Terrain_CollisionTile t = Terrain_CollisionTile{physics_Body{Int64{0x0}}};
    Int64 key_4 = Int64{0x0};
    Terrain_CollisionTile put = Terrain_CollisionTile{physics_Body{Int64{0x0}}};
    N = kern::to<Int>(kern::formula([](Float kCells) -> Float { return (kCells + 3.0); }, kern::to<Float>(kCells)));
    x0 = kern::formula([](Float ti, Float kTile, Float kStep) -> Float { return ((ti * kTile) - kStep); }, kern::to<Float>(ti), kTile, kStep);
    y0 = kern::formula([](Float tj, Float kTile, Float kStep) -> Float { return ((tj * kTile) - kStep); }, kern::to<Float>(tj), kTile, kStep);
    made = List<Vector>{};
    list_resize(made, kern::to<Int>(kern::formula([](Float n) -> Float { return (n * n); }, kern::to<Float>(N))), Vector{0.0, 0.0, 0.0});
    pts = made;
    for (auto it142 = flow_for(n142, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(N)))); it142.next();) {
      for (auto it143 = flow_for(n143, 0, kern::to<Int>(kern::formula([](Float n) -> Float { return n - 1.0; }, kern::to<Float>(N)))); it143.next();) {
        x = kern::formula([](Float x0, Float index, Float kStep) -> Float { return (x0 + (index * kStep)); }, x0, kern::to<Float>(n143.index), kStep);
        y = kern::formula([](Float y0, Float index, Float kStep) -> Float { return (y0 + (index * kStep)); }, y0, kern::to<Float>(n142.index), kStep);
        z = Terrain_node_height(self, x, y).Result;
        if (logic_or(logic_or(logic_or(math_compare<"==">(kern::to<Float>(n142.index), 0.0), math_compare<"==">(kern::to<Float>(n143.index), 0.0)), math_compare<"==">(kern::to<Float>(n142.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(N))))))), math_compare<"==">(kern::to<Float>(n143.index), kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float n) -> Float { return (n - 1.0); }, kern::to<Float>(N)))))))) {
          z = kern::formula([](Float z, Float kSkirtDrop) -> Float { return (z - kSkirtDrop); }, z, kSkirtDrop);
        }
        pts[kern::to<Int>(kern::formula([](Float index, Float n, Float index_2) -> Float { return ((index * n) + index_2); }, kern::to<Float>(n142.index), kern::to<Float>(N), kern::to<Float>(n143.index)))] = vector_make(x, y, z);
      }
    }
    idx = List<Int>{};
    list_reserve(idx, kern::to<Int>(kern::formula([](Float n) -> Float { return (((n - 1.0) * (n - 1.0)) * 6.0); }, kern::to<Float>(N))));
    r = 0;
    while (loop(math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float r) -> Float { return (r + 1.0); }, kern::to<Float>(r))))), kern::to<Float>(N)))) {
      c = 0;
      while (loop(math_compare<"<">(kern::to<Float>(value_int(kern::to<Int>(kern::formula([](Float c) -> Float { return (c + 1.0); }, kern::to<Float>(c))))), kern::to<Float>(N)))) {
        a = kern::to<Int>(kern::formula([](Float r, Float n, Float c) -> Float { return kern::math::wrap32(((r * n) + c)); }, kern::to<Float>(r), kern::to<Float>(N), kern::to<Float>(c)));
        b = kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a + 1.0); }, kern::to<Float>(a)));
        cc = kern::to<Int>(kern::formula([](Float a, Float n) -> Float { return kern::math::wrap32(a + kern::math::wrap32(n)); }, kern::to<Float>(a), kern::to<Float>(N)));
        d = kern::to<Int>(kern::formula([](Float cc) -> Float { return kern::math::wrap32(cc + 1.0); }, kern::to<Float>(cc)));
        place = list_length<Int>(idx);
        item = a;
        item_2 = b;
        item_3 = d;
        item_4 = a;
        item_5 = d;
        item_6 = cc;
        list_insert_list(idx, place, list_make<Int, "6">(item, item_2, item_3, item_4, item_5, item_6));
        c += 1;
      }
      r += 1;
    }
    bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0, 0.0, 1.0, true, true, true, false, Int64{0x0}};
    bs.type = physics_BodyType::fixed;
    engine_world_create_body(n144, self.world_, bs);
    create_body = n144.result;
    body = create_body;
    ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0, 0.0, 0.0, 1.0}, Vector{1.0, 1.0, 1.0}, 0.5, 1.0, List<Vector>{}, List<Int>{}, physics_ShapeMaterial{1.0, 0.6, 0.0, 0.0, false, Int64{0x1}, Int64{0xffffffffffffffff}, 0, true, true, false, Int64{0x0}, physics_Surface::none}};
    ss.kind = physics_ShapeKind::mesh;
    ss.points = pts;
    ss.indices = idx;
    ss.material.friction = 0.8;
    ss.material.category_bits = Int64{0x1};
    engine_world_create_shape(n145, self.world_, body, ss);
    create_shape = n145.result;
    key = key2(ti, tj).Result;
    map_add(n146, self.collision_, key);
    key_2 = key2(ti, tj).Result;
    map_add(n147, self.collision_, key_2);
    a_2 = struct_break<Terrain_CollisionTile>(self.collision_[n147.index]).body;
    both = engine_physics_physics::Body_valid(a_2).Result;
    if (both) {
      body_valid_2 = engine_world_body_valid(self.world_, struct_break<Terrain_CollisionTile>(self.collision_[n147.index]).body);
      both = body_valid_2;
    }
    if (both) {
      key_3 = key2(ti, tj).Result;
      map_add(n148, self.collision_, key_3);
      engine_world_destroy_body(self.world_, struct_break<Terrain_CollisionTile>(self.collision_[n148.index]).body);
    }
    key_4 = key2(ti, tj).Result;
    map_add(n149, self.collision_, key_4);
    t = self.collision_[n149.index];
    t.body = body;
    put = t;
    self.collision_[n149.index] = put;
  }

  struct Terrain_build_water_chunk_wetResult { Bool Result = false; };
  Terrain_build_water_chunk_wetResult Terrain_build_water_chunk_wet(Terrain& self, Int cx = 0, Int cy = 0) {
    if (logic_not(Terrain_water_cell(self, cx, cy).Result)) {
      return {false};
    }
    return {logic_or(math_compare<"==">(kern::to<Float>(list_length<Terrain_Bridge>(self.bridges_)), 0.0), logic_not(Terrain_on_bridge(self, kern::formula([](Float cx, Float kWaterCell) -> Float { return ((cx + 0.5) * kWaterCell); }, kern::to<Float>(cx), kWaterCell), kern::formula([](Float cy, Float kWaterCell) -> Float { return ((cy + 0.5) * kWaterCell); }, kern::to<Float>(cy), kWaterCell)).Result))};
  }

  void Terrain_build_water_chunk_quad(Terrain& self, MeshData& m, const Fixed<Float, 3>& deep, Vector color = Vector{0.0, 0.0, 0.0}, Float xa = 0.0, Float ya = 0.0, Float xb = 0.0, Float yb = 0.0) {
    Int base = 0;
    Vector item = Vector{0.0, 0.0, 0.0};
    Vector item_2 = Vector{0.0, 0.0, 0.0};
    Vector item_3 = Vector{0.0, 0.0, 0.0};
    Vector item_4 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 4> items = Fixed<Vector, 4>{};
    Vector p = Vector{0.0, 0.0, 0.0};
    Int place = 0;
    Int item_5 = 0;
    Int item_6 = 0;
    Int item_7 = 0;
    Int item_8 = 0;
    Int item_9 = 0;
    Int item_10 = 0;
    base = engine_geo_mesh_data::MeshData_vertex_count(m).Result;
    item = vector_make(xa, ya, 0.0);
    item_2 = vector_make(xb, ya, 0.0);
    item_3 = vector_make(xb, yb, 0.0);
    item_4 = vector_make(xa, yb, 0.0);
    items = list_make<Vector, "4">(item, item_2, item_3, item_4);
    for (auto it150 = flow_for(n150, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Vector>(items))))); it150.next();) {
      p = items[n150.index];
      engine_geo_mesh_data::MeshData_push(m, vector_make(vector_break(p).x, vector_break(p).y, kern::formula([](Float the_result, Float kWaterRenderZ) -> Float { return (the_result + kWaterRenderZ); }, Terrain_water_level(self, vector_break(p).x, vector_break(p).y).Result, kWaterRenderZ)), Vector{0.0, 0.0, 1.0}, color, vector_make(kern::formula([](Float x) -> Float { return (x * 0.05); }, vector_break(p).x), kern::formula([](Float y) -> Float { return (y * 0.05); }, vector_break(p).y), 0.0));
      engine_geo_mesh_data::MeshData_push_extra(m, deep, 0);
    }
    place = list_length<Int>(m.indices);
    item_5 = base;
    item_6 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 1.0); }, kern::to<Float>(base)));
    item_7 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 2.0); }, kern::to<Float>(base)));
    item_8 = base;
    item_9 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 2.0); }, kern::to<Float>(base)));
    item_10 = kern::to<Int>(kern::formula([](Float base) -> Float { return kern::math::wrap32(base + 3.0); }, kern::to<Float>(base)));
    list_insert_list(m.indices, place, list_make<Int, "6">(item_5, item_6, item_7, item_8, item_9, item_10));
  }

  struct Terrain_ready_atResult { Bool Result = false; };
  KERN_PURE Terrain_ready_atResult Terrain_ready_at(const Terrain& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    Int ci = 0;
    Int cj = 0;
    Int64 k = Int64{0x0};
    if (logic_not(self.settled_)) {
      return {false};
    }
    if (logic_not(math_compare<"!=">(kern::to<Float>(self.world_), 0.0))) {
      return {true};
    }
    ci = kern::to<Int>(kern::formula([](Float x, Float kTile) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(x, kTile))); }, vector_break(p).x, kTile));
    cj = kern::to<Int>(kern::formula([](Float y, Float kTile) -> Float { return kern::math::trunc(kern::math::floor(kern::math::div(y, kTile))); }, vector_break(p).y, kTile));
    for (auto it158 = flow_for(n158, kern::to<Int>(kern::formula([](Float cj) -> Float { return (cj - 1.0); }, kern::to<Float>(cj))), kern::to<Int>(kern::formula([](Float cj) -> Float { return (cj + 1.0); }, kern::to<Float>(cj)))); it158.next();) {
      for (auto it159 = flow_for(n159, kern::to<Int>(kern::formula([](Float ci) -> Float { return (ci - 1.0); }, kern::to<Float>(ci))), kern::to<Int>(kern::formula([](Float ci) -> Float { return (ci + 1.0); }, kern::to<Float>(ci)))); it159.next();) {
        k = key2(n159.index, n158.index).Result;
        if (logic_or(logic_not(math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(map_contains<Int64, Terrain_CollisionTile>(self.collision_, k)))), 0.0)), math_compare<"!=">(kern::to<Float>(value_int(kern::to<Int>(set_contains<Int64>(self.dirty_collision_, k)))), 0.0))) {
          return {false};
        }
      }
    }
    return {true};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
  struct : kern::Node<11> { Int index = 0; } n11;
  struct : kern::Node<12> { Int index = 0; } n12;
  struct : kern::Node<13> { Int index = 0; } n13;
  struct : kern::Node<14> { Int index = 0; } n14;
  struct : kern::Node<15> { Int index = 0; } n15;
  struct : kern::Node<16> { Int index = 0; } n16;
  struct : kern::Node<17> { Int index = 0; } n17;
  struct : kern::Node<20> { Int index = 0; } n20;
  struct { Bool result = false; } n21;
  struct : kern::Node<22> { Int index = 0; } n22;
  struct { Bool result = false; } n23;
  struct : kern::Node<24> { Int index = 0; } n24;
  struct : kern::Node<25> { Int index = 0; } n25;
  struct : kern::Node<26> { Int index = 0; } n26;
  struct : kern::Node<27> { Int index = 0; } n27;
  struct : kern::Node<28> { Int index = 0; } n28;
  struct : kern::Node<29> { Int index = 0; } n29;
  struct : kern::Node<30> { Int index = 0; } n30;
  struct { Bool added = false; Int index = 0; } n31;
  struct { Bool added = false; Int index = 0; } n32;
  struct { Bool added = false; Int index = 0; } n33;
  struct { Bool added = false; Int index = 0; } n34;
  struct { Bool added = false; Int index = 0; } n35;
  struct { Bool added = false; Int index = 0; } n36;
  struct : kern::Node<37> { Int index = 0; } n37;
  Terrain_plan_demResult n38;
  struct : kern::Node<39> { Int index = 0; } n39;
  Terrain_plan_windows_matchResult n40;
  struct { Terrain_Grid value = Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}; } n41;
  struct : kern::Node<42> { Int index = 0; } n42;
  Terrain_plan_demResult n43;
  struct : kern::Node<44> { Int index = 0; } n44;
  Terrain_run_job_mineResult n45;
  struct : kern::Node<46> { Int index = 0; } n46;
  Terrain_run_job_mineResult n47;
  struct { Int result = 0; } n48;
  struct { Bool result = false; List<Int> out = List<Int>{}; } n49;
  struct : kern::Node<50> { Int index = 0; } n50;
  struct { Bool added = false; Int index = 0; } n51;
  struct : kern::Node<52> { Int index = 0; } n52;
  struct { Bool added = false; Int index = 0; } n53;
  struct : kern::Node<54> { Int index = 0; } n54;
  struct { Bool added = false; Int index = 0; } n55;
  struct { Terrain_Grid value = Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}; } n56;
  struct : kern::Node<57> { Int index = 0; } n57;
  struct : kern::Node<58> { Int index = 0; } n58;
  struct : kern::Node<59> { Int index = 0; } n59;
  struct : kern::Node<60> { Int index = 0; } n60;
  struct : kern::Node<61> { Int index = 0; } n61;
  struct : kern::Node<62> { Int index = 0; } n62;
  struct : kern::Node<63> { Int index = 0; } n63;
  struct : kern::Node<64> { Int index = 0; } n64;
  struct { Bool added = false; Int index = 0; } n65;
  struct { List<Terrain_CorridorSet> value = List<Terrain_CorridorSet>{}; } n66;
  struct : kern::Node<67> { Int index = 0; } n67;
  struct : kern::Node<68> { Int index = 0; } n68;
  struct : kern::Node<69> { Int index = 0; } n69;
  struct : kern::Node<70> { Int index = 0; } n70;
  struct : kern::Node<71> { Int index = 0; } n71;
  struct { Bool added = false; Int index = 0; } n72;
  struct { Bool added = false; Int index = 0; } n73;
  struct { Bool added = false; Int index = 0; } n74;
  struct : kern::Node<75> { Int index = 0; } n75;
  struct : kern::Node<76> { Int index = 0; } n76;
  struct : kern::Node<77> { Int index = 0; } n77;
  struct : kern::Node<78> { Int index = 0; } n78;
  struct : kern::Node<79> { Int index = 0; } n79;
  struct : kern::Node<80> { Int index = 0; } n80;
  struct : kern::Node<81> { Int index = 0; } n81;
  struct : kern::Node<82> { Int index = 0; } n82;
  struct : kern::Node<83> { Int index = 0; } n83;
  struct : kern::Node<84> { Int index = 0; } n84;
  struct { Bool added = false; Int index = 0; } n85;
  struct { Bool added = false; Int index = 0; } n86;
  struct { Bool added = false; Int index = 0; } n87;
  struct { Bool added = false; Int index = 0; } n88;
  struct : kern::Node<89> { Int index = 0; } n89;
  struct { Bool added = false; Int index = 0; } n90;
  Terrain_image_boundsResult n91;
  struct : kern::Node<92> { Int index = 0; } n92;
  struct : kern::Node<93> { Int index = 0; } n93;
  struct : kern::Node<94> { Int index = 0; } n94;
  struct : kern::Node<95> { Int index = 0; } n95;
  struct : kern::Node<96> { Int index = 0; } n96;
  struct : kern::Node<97> { Int index = 0; } n97;
  struct : kern::Node<98> { Int index = 0; } n98;
  struct : kern::Node<99> { Int index = 0; } n99;
  struct : kern::Node<100> { Int index = 0; } n100;
  struct : kern::Node<101> { Int index = 0; } n101;
  Terrain_image_boundsResult n102;
  struct : kern::Node<103> { Int index = 0; } n103;
  struct : kern::Node<104> { Int index = 0; } n104;
  struct : kern::Node<105> { Int index = 0; } n105;
  struct : kern::Node<106> { Int index = 0; } n106;
  struct : kern::Node<107> { Int index = 0; } n107;
  struct : kern::Node<108> { Int index = 0; } n108;
  struct : kern::Node<109> { Int index = 0; } n109;
  struct : kern::Node<110> { Int index = 0; } n110;
  struct : kern::Node<111> { Int index = 0; } n111;
  struct : kern::Node<112> { Int index = 0; } n112;
  struct : kern::Node<113> { Int index = 0; } n113;
  struct : kern::Node<114> { Int index = 0; } n114;
  struct : kern::Node<115> { Int index = 0; } n115;
  struct : kern::Node<116> { Int index = 0; } n116;
  struct { Bool added = false; Int index = 0; } n117;
  struct : kern::Node<118> { Int index = 0; } n118;
  struct : kern::Node<119> { Int index = 0; } n119;
  struct : kern::Node<120> { Int index = 0; } n120;
  struct : kern::Node<121> { Int index = 0; } n121;
  struct : kern::Node<122> { Int index = 0; } n122;
  struct : kern::Node<123> { Int index = 0; } n123;
  Terrain_ground_albedoResult n124;
  struct : kern::Node<125> { Int index = 0; } n125;
  struct : kern::Node<126> { Int index = 0; } n126;
  struct : kern::Node<127> { Int index = 0; } n127;
  struct : kern::Node<128> { Int index = 0; } n128;
  struct : kern::Node<129> { Int index = 0; } n129;
  struct : kern::Node<130> { Int index = 0; } n130;
  struct : kern::Node<131> { Int index = 0; } n131;
  struct : kern::Node<132> { Int index = 0; } n132;
  struct : kern::Node<133> { Int index = 0; } n133;
  struct : kern::Node<134> { Int index = 0; } n134;
  struct { Int result = 0; } n135;
  struct { Bool result = false; } n136;
  struct { Bool added = false; Int index = 0; } n137;
  struct { Bool added = false; Int index = 0; } n138;
  struct { Bool added = false; Int index = 0; } n139;
  struct { Bool result = false; } n140;
  struct { Bool added = false; Int index = 0; } n141;
  struct : kern::Node<142> { Int index = 0; } n142;
  struct : kern::Node<143> { Int index = 0; } n143;
  struct { physics_Body result = physics_Body{Int64{0x0}}; } n144;
  struct { physics_Shape result = physics_Shape{Int64{0x0}}; } n145;
  struct { Bool added = false; Int index = 0; } n146;
  struct { Bool added = false; Int index = 0; } n147;
  struct { Bool added = false; Int index = 0; } n148;
  struct { Bool added = false; Int index = 0; } n149;
  struct : kern::Node<150> { Int index = 0; } n150;
  struct : kern::Node<158> { Int index = 0; } n158;
  struct : kern::Node<159> { Int index = 0; } n159;
};
