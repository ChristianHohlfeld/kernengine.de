struct terrain_cpp : virtual kern::Library, virtual common, virtual engine_math_math, virtual terrain, virtual tiles, virtual tiles_cpp {
  // ---- types ----
  KERN_STRUCT(DemBytes) { u32 w = 0; u32 h = 0; u64 at = 0; };
  KERN_STRUCT(DemOrigin) { f64 lon0 = 0.0; f64 lat0 = 0.0; };
  KERN_STRUCT(DemTileHead) { u32 x = 0; u32 y = 0; u32 w = 0; u32 h = 0; };
  KERN_STRUCT(DemWindow) { f32 x0 = 0.0f; f32 y0 = 0.0f; f32 step = 10.0f; u32 n = 0; f32 open = 160.0f; f32 blur = 30.0f; u32 count = 0; };
  KERN_STRUCT(WantedTile) { f32 d = 0.0f; u64 k = 0; };

  // ---- variables ----
  /// the bare-earth filter's reach from a window's edge (open 160 m + blur): inside it two windows agree
  KERN_CONST f32 kDemMargin = 300.0f;
  /// terrain_craters.mjs WATER_LEVEL_M
  KERN_CONST f32 kWaterLevel = -0.18f;
  KERN_CONST f64 kPiD = 3.141592653589793;
  /// see README: the city ring is ~5 km, bare earth is smooth below 160 m
  KERN_CONST i32 kDemZoom = 13;
  /// and the largest (low latitudes: a map tile is 2.4 km wide there)
  KERN_CONST f32 kDemMaxSize = 24000.0f;
  /// the smallest elevation window (m); it grows with the map tiles it must hold
  KERN_CONST f32 kDemSize = 8000.0f;
  /// grid spacing of the sampled elevation (m)
  KERN_CONST f32 kDemStep = 10.0f;
  /// terrain_elevation.mjs OPEN_M, BLUR_M
  KERN_CONST f32 kOpen = 160.0f;
  /// terrain_elevation.mjs OPEN_M, BLUR_M
  KERN_CONST f32 kBlur = 30.0f;
  /// the elevation grid job (kern/core/jobs.h; city jobs are 1, 2)
  KERN_CONST u32 kDemJob = 3;
  /// terrain_craters.mjs TERRAIN_FIELD_STEP_M
  KERN_CONST f32 kStep = 5.0f;
  /// far render tile
  KERN_CONST f32 kBlock = 640.0f;
  /// collision / near render tile (32 cells of 5 m: terrain_tiles.mjs TILE_CELLS)
  KERN_CONST f32 kTile = 160.0f;
  KERN_CONST f32 kWaterChunk = 320.0f;
  /// terrain_craters.mjs RG, ROAD_BLEND_M
  KERN_CONST f32 kRoadBlend = 4.5f;
  /// terrain_craters.mjs RG, ROAD_BLEND_M
  KERN_CONST f32 kRoadGrid = 24.0f;
  /// world_water.mjs CELL
  KERN_CONST f32 kWaterCell = 4.0f;
  /// terrain_craters.mjs WATER_BED_M
  KERN_CONST f32 kWaterBed = 2.6f;
  /// ---- ground colour (world_ground.mjs): aerial imagery of the 1600 m around the player, one albedo per 5 m node (never shown as a texture); rebuilt after 350 m of travel ----
  KERN_CONST i32 kImageZoom = 15;
  /// ---- ground colour (world_ground.mjs): aerial imagery of the 1600 m around the player, one albedo per 5 m node (never shown as a texture); rebuilt after 350 m of travel ----
  KERN_CONST i32 kImagePixels = 64;
  /// 16 x 16 water cells
  KERN_CONST f32 kWaterBlock = 64.0f;
  KERN_CONST i32 kCells = 32;
  KERN_CONST Vector kBed = Vector{0.3, 0.29, 0.22};
  /// world_impacts.mjs ground chip colour
  KERN_CONST Vector kSoil = Vector{0.41960785, 0.3529412, 0.26666668};
  /// world_ground.mjs fallbackColors (.16, .2, .1 linear) in sRGB
  KERN_CONST Vector kGround = Vector{0.437, 0.485, 0.349};
  /// terrain_tiles.mjs SKIRT_DROP_M
  KERN_CONST f32 kSkirtDrop = 0.05f;
  /// world_water.mjs WATER_RENDER_Z (above the level)
  KERN_CONST f32 kWaterRenderZ = 0.052f;

  // ---- functions ----
  struct elevation_inResult { bool Result = false; };
  /// the window that knows the place (its interior); else any that holds it; else the player's, held at its edge (far ground nobody stands on: the horizon)
  KERN_PURE elevation_inResult elevation_in(const Vec4& r, f32 x = 0.0f, f32 y = 0.0f) {
    return {logic_and(logic_and(logic_and(math_compare<">=", f32>(x, r.x), math_compare<">=", f32>(y, r.y)), math_compare<"<=", f32>(x, r.z)), math_compare<"<=", f32>(y, r.w))};
  }

  struct interior_ofResult { Vec4 Result = Vec4{0.0f, 0.0f, 0.0f, 0.0f}; };
  KERN_PURE interior_ofResult interior_of(f32 x0 = 0.0f, f32 y0 = 0.0f, f32 span = 0.0f, f32 margin = 0.0f) {
    return {engine_math_math::Vec4_new(kern::formula<f32, f32>([](auto x0, auto margin) { return (x0 + margin); }, x0, margin), kern::formula<f32, f32>([](auto y0, auto margin) { return (y0 + margin); }, y0, margin), kern::formula<f32, f32, f32>([](auto x0, auto span, auto margin) { return ((x0 + span) - margin); }, x0, span, margin), kern::formula<f32, f32, f32>([](auto y0, auto span, auto margin) { return ((y0 + span) - margin); }, y0, span, margin)).Result};
  }

  struct Terrain_interiorResult { Vec4 Result = Vec4{0.0f, 0.0f, 0.0f, 0.0f}; };
  KERN_PURE Terrain_interiorResult Terrain_interior(const Terrain_Grid& g) {
    return {interior_of(g.x0, g.y0, kern::formula<f32, u32, u32>([](auto step, auto n, auto a0) { return (step * num_cast<f32, u32>(((n > 0) ? (n - 1) : a0))); }, g.step, g.n, 0), kDemMargin).Result};
  }

  struct Terrain_grid_atResult { f32 Result = 0.0f; };
  KERN_PURE Terrain_grid_atResult Terrain_grid_at(const Terrain_Grid& g, f32 x = 0.0f, f32 y = 0.0f) {
    f32 lim = 0.0f;
    f32 fx = 0.0f;
    f32 fy = 0.0f;
    u32 i = 0;
    u32 j = 0;
    f32 tx = 0.0f;
    f32 ty = 0.0f;
    u64 at = 0;
    if (math_compare<"<", u32>(g.n, 2)) {
      return {0.0f};
    }
    lim = kern::formula<u32>([](auto n) { return (num_cast<f32, u32>(n) - 1.001f); }, g.n);
    fx = common::clampf(kern::formula<f32, f32, f32>([](auto x, auto x0, auto step) { return ((x - x0) / step); }, x, g.x0, g.step), 0.0f, lim).Result;
    fy = common::clampf(kern::formula<f32, f32, f32>([](auto y, auto y0, auto step) { return ((y - y0) / step); }, y, g.y0, g.step), 0.0f, lim).Result;
    i = num_cast<u32, f32>(fx);
    j = num_cast<u32, f32>(fy);
    tx = kern::formula<f32, u32>([](auto fx, auto i) { return (fx - num_cast<f32, u32>(i)); }, fx, i);
    ty = kern::formula<f32, u32>([](auto fy, auto j) { return (fy - num_cast<f32, u32>(j)); }, fy, j);
    at = kern::formula<u32, u32, u32>([](auto j, auto n, auto i) { return ((num_cast<u64, u32>(j) * num_cast<u64, u32>(n)) + num_cast<u64, u32>(i)); }, j, g.n, i);
    return {kern::formula<f32, f32, f32, f32, f32, f32>([](auto a, auto tx, auto a_2, auto ty, auto a_3, auto a_4) { return ((((a * (1.0f - tx)) + (a_2 * tx)) * (1.0f - ty)) + (((a_3 * (1.0f - tx)) + (a_4 * tx)) * ty)); }, g.h[num_cast<i64, u64>(at)], tx, g.h[num_cast<i64, u64>(kern::formula<u64>([](auto at) { return (at + 1); }, at))], ty, g.h[num_cast<i64, u64>(kern::formula<u64, u32>([](auto at, auto n) { return (at + num_cast<u64, u32>(n)); }, at, g.n))], g.h[num_cast<i64, u64>(kern::formula<u64, u32>([](auto at, auto n) { return ((at + num_cast<u64, u32>(n)) + 1); }, at, g.n))])};
  }

  struct Terrain_elevationResult { f32 Result = 0.0f; };
  KERN_PURE Terrain_elevationResult Terrain_elevation(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    Vec4 a = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vec4 a_2 = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    bool either = false;
    Vec4 a_3 = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vec4 a_4 = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vec4 a_5 = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    if (logic_not(self.grid_ready_)) {
      return {0.0f};
    }
    either = math_compare<"==", i64>(list_length<Terrain_Grid>(self.side_), 0);
    if (logic_not(either)) {
      a_2 = Terrain_interior(self.grid_).Result;
      either = elevation_in(a_2, x, y).Result;
    }
    if (either) {
      return {kern::formula<f32, f32>([](auto the_result, auto ref_height_) { return (the_result - ref_height_); }, Terrain_grid_at(self.grid_, x, y).Result, self.ref_height_)};
    }
    for (auto it10 = flow_for(n10, 0, math_sub<i64>(list_length<Terrain_Grid>(self.side_), 1)); it10.next();) {
      a_3 = Terrain_interior(self.side_[n10.index]).Result;
      if (elevation_in(a_3, x, y).Result) {
        return {kern::formula<f32, f32>([](auto the_result, auto ref_height_) { return (the_result - ref_height_); }, Terrain_grid_at(self.side_[n10.index], x, y).Result, self.ref_height_)};
      }
    }
    a_4 = interior_of(self.grid_.x0, self.grid_.y0, kern::formula<f32, u32>([](auto step, auto n) { return (step * num_cast<f32, u32>((n - 1))); }, self.grid_.step, self.grid_.n), 0.0f).Result;
    if (elevation_in(a_4, x, y).Result) {
      return {kern::formula<f32, f32>([](auto the_result, auto ref_height_) { return (the_result - ref_height_); }, Terrain_grid_at(self.grid_, x, y).Result, self.ref_height_)};
    }
    for (auto it11 = flow_for(n11, 0, math_sub<i64>(list_length<Terrain_Grid>(self.side_), 1)); it11.next();) {
      a_5 = interior_of(self.side_[n11.index].x0, self.side_[n11.index].y0, kern::formula<f32, u32>([](auto a, auto a_2) { return (a * num_cast<f32, u32>((a_2 - 1))); }, self.side_[n11.index].step, self.side_[n11.index].n), 0.0f).Result;
      if (elevation_in(a_5, x, y).Result) {
        return {kern::formula<f32, f32>([](auto the_result, auto ref_height_) { return (the_result - ref_height_); }, Terrain_grid_at(self.side_[n11.index], x, y).Result, self.ref_height_)};
      }
    }
    return {kern::formula<f32, f32>([](auto the_result, auto ref_height_) { return (the_result - ref_height_); }, Terrain_grid_at(self.grid_, x, y).Result, self.ref_height_)};
  }

  struct Terrain_water_levelResult { f32 Result = 0.0f; };
  KERN_PURE Terrain_water_levelResult Terrain_water_level(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    return {kern::formula<f32, f32>([](auto the_result, auto kWaterLevel) { return (the_result + kWaterLevel); }, Terrain_elevation(self, x, y).Result, kWaterLevel)};
  }

  struct key2Result { u64 Result = 0; };
  KERN_PURE key2Result key2(i32 i = 0, i32 j = 0) {
    return {kern::formula<i32, i32>([](auto i, auto j) { return ((num_cast<u64, u32>(num_cast<u32, i32>(i)) << 32) | num_cast<u64, u32>(num_cast<u32, i32>(j))); }, i, j)};
  }

  struct key3Result { u64 Result = 0; };
  KERN_PURE key3Result key3(i32 level = 0, i32 i = 0, i32 j = 0) {
    return {kern::formula<i32, i32, i32>([](auto level, auto i, auto j) { return (((num_cast<u64, i32>(level) << 56) | ((num_cast<u64, u32>(num_cast<u32, i32>(i)) & 268435455) << 28)) | (num_cast<u64, u32>(num_cast<u32, i32>(j)) & 268435455)); }, level, i, j)};
  }

  struct sx28Result { i32 Result = 0; };
  KERN_PURE sx28Result sx28(u64 v = 0) {
    return {kern::formula<u64>([](auto v) { return (num_cast<i32, u32>(num_cast<u32, u64>((v << 4))) >> 4); }, v)};
  }

  void unkey3(i32& level, i32& i, i32& j, u64 k = 0) {
    level = kern::formula<u64>([](auto k) { return num_cast<i32, u64>((k >> 56)); }, k);
    i = sx28(kern::formula<u64>([](auto k) { return ((k >> 28) & 268435455); }, k)).Result;
    j = sx28(kern::formula<u64>([](auto k) { return (k & 268435455); }, k)).Result;
  }

  struct rect_distanceResult { f32 Result = 0.0f; };
  KERN_PURE rect_distanceResult rect_distance(f32 x = 0.0f, f32 y = 0.0f, f32 x0 = 0.0f, f32 y0 = 0.0f, f32 x1 = 0.0f, f32 y1 = 0.0f) {
    f32 dx = 0.0f;
    f32 dy = 0.0f;
    dx = kern::formula<f32, f32, f32>([](auto x0, auto x, auto x1) { return fmax(0.0f, fmax((x0 - x), (x - x1))); }, x0, x, x1);
    dy = kern::formula<f32, f32, f32>([](auto y0, auto y, auto y1) { return fmax(0.0f, fmax((y0 - y), (y - y1))); }, y0, y, y1);
    return {kern::formula<f32, f32>([](auto dx, auto dy) { return sqrt(((dx * dx) + (dy * dy))); }, dx, dy)};
  }

  struct merc_yResult { f64 Result = 0.0; };
  KERN_PURE merc_yResult merc_y(f64 lat = 0.0, i32 z = 0) {
    f64 r = 0.0;
    r = kern::formula<f64, f64>([](auto lat, auto kPiD) { return ((lat * kPiD) / 180.0); }, lat, kPiD);
    return {kern::formula<f64, f64, f64>([](auto r, auto kPiD, auto a) { return (((1.0 - (asinh(tan(r)) / kPiD)) / 2.0) * a); }, r, kPiD, math_ldexp<f64>(1.0, z))};
  }

  struct merc_xResult { f64 Result = 0.0; };
  KERN_PURE merc_xResult merc_x(f64 lon = 0.0, i32 z = 0) {
    return {kern::formula<f64, f64>([](auto lon, auto a) { return (((lon + 180.0) / 360.0) * a); }, lon, math_ldexp<f64>(1.0, z))};
  }

  /// Sliding-window min (op < 0) / max (op > 0) along lines (monotonic deque), window 2r+1.
  void slide(const List<f32>& src, List<f32>& dst, List<i32>& q, u32 n = 0, u32 stride = 0, u32 line_stride = 0, i32 r = 0, i32 op = 0) {
    u32 base = 0;
    i32 qh = 0;
    i32 qt = 0;
    i32 j = 0;
    i32 hi = 0;
    f32 v = 0.0f;
    i32 was = 0;
    list_resize(q, num_cast<i64, u32>(n), 0);
    for (auto it12 = flow_for(n12, 0, math_sub<i64>(num_cast<i64, u32>(n), 1)); it12.next();) {
      base = kern::formula<i64, u32>([](auto index, auto line_stride) { return (num_cast<u32, i64>(index) * line_stride); }, n12.index, line_stride);
      qh = 0;
      qt = 0;
      j = 0;
      for (auto it13 = flow_for(n13, 0, math_sub<i64>(num_cast<i64, i32>(num_cast<i32, u32>(n)), 1)); it13.next();) {
        hi = kern::formula<u32, i64, i32>([](auto n, auto index, auto r) { return min((num_cast<i32, u32>(n) - 1), (num_cast<i32, i64>(index) + r)); }, n, n13.index, r);
        while (loop(math_compare<"<=", i32>(j, hi))) {
          v = src[num_cast<i64, u64>(kern::formula<u32, i32, u32>([](auto base, auto j, auto stride) { return num_cast<u64, u32>((base + (num_cast<u32, i32>(j) * stride))); }, base, j, stride))];
          while (loop(logic_and(math_compare<">", i32>(qt, qh), logic_select<bool>(math_compare<">", i32>(op, 0), math_compare<"<=", f32>(src[num_cast<i64, u64>(kern::formula<u32, i32, u32>([](auto base, auto a, auto stride) { return num_cast<u64, u32>((base + (num_cast<u32, i32>(a) * stride))); }, base, q[num_cast<i64, u64>(kern::formula<i32>([](auto qt) { return num_cast<u64, i32>((qt - 1)); }, qt))], stride))], v), math_compare<">=", f32>(src[num_cast<i64, u64>(kern::formula<u32, i32, u32>([](auto base, auto a, auto stride) { return num_cast<u64, u32>((base + (num_cast<u32, i32>(a) * stride))); }, base, q[num_cast<i64, u64>(kern::formula<i32>([](auto qt) { return num_cast<u64, i32>((qt - 1)); }, qt))], stride))], v))))) {
            qt = kern::formula<i32>([](auto qt) { return (qt - 1); }, qt);
          }
          was = qt;
          qt += 1;
          q[num_cast<i64, i32>(was)] = j;
          j += 1;
        }
        while (loop(math_compare<"<", i32>(q[num_cast<i64, i32>(qh)], kern::formula<i64, i32>([](auto index, auto r) { return (num_cast<i32, i64>(index) - r); }, n13.index, r)))) {
          qh += 1;
        }
        dst[num_cast<i64, u64>(kern::formula<u32, i64, u32>([](auto base, auto index, auto stride) { return num_cast<u64, u32>((base + (num_cast<u32, i32>(num_cast<i32, i64>(index)) * stride))); }, base, n13.index, stride))] = src[num_cast<i64, u64>(kern::formula<u32, i32, u32>([](auto base, auto a, auto stride) { return num_cast<u64, u32>((base + (num_cast<u32, i32>(a) * stride))); }, base, q[num_cast<i64, i32>(qh)], stride))];
      }
    }
  }

  void box_blur(const List<f32>& src, List<f32>& dst, u32 n = 0, u32 stride = 0, u32 line_stride = 0, i32 r = 0) {
    u32 base = 0;
    f64 sum = 0.0;
    i32 cnt = 0;
    i32 add = 0;
    i32 rem = 0;
    for (auto it14 = flow_for(n14, 0, math_sub<i64>(num_cast<i64, u32>(n), 1)); it14.next();) {
      base = kern::formula<i64, u32>([](auto index, auto line_stride) { return (num_cast<u32, i64>(index) * line_stride); }, n14.index, line_stride);
      sum = 0.0;
      cnt = 0;
      for (auto it15 = flow_for(n15, 0, num_cast<i64, i32>(kern::formula<u32, i32>([](auto n, auto r) { return min((num_cast<i32, u32>(n) - 1), r); }, n, r))); it15.next();) {
        sum += num_cast<f64, f32>(src[num_cast<i64, u64>(kern::formula<u32, i64, u32>([](auto base, auto index, auto stride) { return num_cast<u64, u32>((base + (num_cast<u32, i32>(num_cast<i32, i64>(index)) * stride))); }, base, n15.index, stride))]);
        cnt += 1;
      }
      for (auto it16 = flow_for(n16, 0, math_sub<i64>(num_cast<i64, i32>(num_cast<i32, u32>(n)), 1)); it16.next();) {
        dst[num_cast<i64, u64>(kern::formula<u32, i64, u32>([](auto base, auto index, auto stride) { return num_cast<u64, u32>((base + (num_cast<u32, i32>(num_cast<i32, i64>(index)) * stride))); }, base, n16.index, stride))] = kern::formula<f64, i32>([](auto sum, auto cnt) { return num_cast<f32, f64>((sum / num_cast<f64, i32>(cnt))); }, sum, cnt);
        add = kern::formula<i64, i32>([](auto index, auto r) { return ((num_cast<i32, i64>(index) + r) + 1); }, n16.index, r);
        rem = kern::formula<i64, i32>([](auto index, auto r) { return (num_cast<i32, i64>(index) - r); }, n16.index, r);
        if (math_compare<"<", i32>(add, num_cast<i32, u32>(n))) {
          sum += num_cast<f64, f32>(src[num_cast<i64, u64>(kern::formula<u32, i32, u32>([](auto base, auto add, auto stride) { return num_cast<u64, u32>((base + (num_cast<u32, i32>(add) * stride))); }, base, add, stride))]);
          cnt += 1;
        }
        if (math_compare<">=", i32>(rem, 0)) {
          sum = kern::formula<f64, f32>([](auto sum, auto a) { return (sum - num_cast<f64, f32>(a)); }, sum, src[num_cast<i64, u64>(kern::formula<u32, i32, u32>([](auto base, auto rem, auto stride) { return num_cast<u64, u32>((base + (num_cast<u32, i32>(rem) * stride))); }, base, rem, stride))]);
          cnt = kern::formula<i32>([](auto cnt) { return (cnt - 1); }, cnt);
        }
      }
    }
  }

  /// One filter pass of bare_earth (0..7).
  void filter_pass(List<f32>& h, List<f32>& t, List<i32>& q, u32 n = 0, i32 pass = 0, i32 ro = 0, i32 rb = 0) {
    i32 selected = 0;
    selected = pass;
    if (math_compare<"==", i32>(selected, 0)) {
      slide(h, t, q, n, 1, n, ro, -1);
    } else if (math_compare<"==", i32>(selected, 1)) {
      slide(t, h, q, n, n, 1, ro, -1);
    } else if (math_compare<"==", i32>(selected, 2)) {
      slide(h, t, q, n, 1, n, ro, 1);
    } else if (math_compare<"==", i32>(selected, 3)) {
      slide(t, h, q, n, n, 1, ro, 1);
    } else if (logic_or(math_compare<"==", i32>(selected, 4), math_compare<"==", i32>(selected, 6))) {
      box_blur(h, t, n, 1, n, rb);
    } else if (logic_or(math_compare<"==", i32>(selected, 5), math_compare<"==", i32>(selected, 7))) {
      box_blur(t, h, n, n, 1, rb);
    }
  }

  struct crater_profileResult { f32 Result = 0.0f; };
  KERN_PURE crater_profileResult crater_profile(f32 r = 0.0f, f32 radius = 0.0f, f32 depth = 0.0f, f32 rim = 0.0f) {
    f32 bowl = 0.0f;
    f32 x = 0.0f;
    f32 k = 0.0f;
    bowl = kern::formula<f32>([](auto radius) { return (radius * 0.5f); }, radius);
    if (math_compare<"<", f32>(r, bowl)) {
      return {kern::formula<f32, f32, f32>([](auto depth, auto r, auto bowl) { return ((-depth) * (1.0f - ((r / bowl) * (r / bowl)))); }, depth, r, bowl)};
    }
    if (math_compare<">=", f32>(r, radius)) {
      return {0.0f};
    }
    x = kern::formula<f32, f32, f32>([](auto r, auto bowl, auto radius) { return ((r - bowl) / (radius - bowl)); }, r, bowl, radius);
    k = kern::formula<f32>([](auto x) { return ((x - 0.12f) / 0.14f); }, x);
    return {kern::formula<f32, f32, f32>([](auto rim, auto k, auto x) { return ((rim * exp((-(k * k)))) * (1.0f - x)); }, rim, k, x)};
  }

  void bare_earth(List<f32>& h, u32 n = 0, f32 step = 0.0f, f32 open_m = 0.0f, f32 blur_m = 0.0f) {
    List<f32> t = List<f32>{};
    List<f32> made = List<f32>{};
    List<i32> q = List<i32>{};
    i32 ro = 0;
    i32 rb = 0;
    made = List<f32>{};
    list_resize(made, list_length<f32>(h), 0.0f);
    t = made;
    q = List<i32>{};
    ro = kern::formula<f32, f32>([](auto open_m, auto step) { return max(1, num_cast<i32, i64>(num_cast<i64, f32>(round(((open_m / step) / 2.0f))))); }, open_m, step);
    rb = kern::formula<f32, f32>([](auto blur_m, auto step) { return max(1, num_cast<i32, i64>(num_cast<i64, f32>(round(((blur_m / step) / 2.0f))))); }, blur_m, step);
    for (auto it17 = flow_for(n17, 0, 7); it17.next();) {
      filter_pass(h, t, q, n, num_cast<i32, i64>(n17.index), ro, rb);
    }
  }

  /// kDemJob: Terrarium tiles -> the bare-earth grid (sample_dem + bare_earth, without engine state).
  void dem_grid_job(List<u8>& out, List<u8> in = List<u8>{}) {
    DemOrigin origin = DemOrigin{0.0, 0.0};
    DemWindow win = DemWindow{0.0f, 0.0f, 10.0f, 0, 160.0f, 30.0f, 0};
    u32 n = 0;
    f32 x0 = 0.0f;
    f32 y0 = 0.0f;
    f32 step = 0.0f;
    u64 p = 0;
    Map<u64, DemBytes> tiles = Map<u64, DemBytes>{};
    DemTileHead head = DemTileHead{0, 0, 0, 0};
    u64 bytes = 0;
    DemBytes put = DemBytes{0, 0, 0};
    u64 key = 0;
    TileKey a = TileKey{14, 0, 0};
    GeoProjector proj = GeoProjector{0.0, 0.0, 1.0};
    bool init = false;
    List<f32> grid = List<f32>{};
    List<f32> made = List<f32>{};
    f32 y = 0.0f;
    f64 lon = 0.0;
    f64 lat = 0.0;
    f64 fx = 0.0;
    f64 fy = 0.0;
    i32 tx = 0;
    i32 ty = 0;
    i64 it = 0;
    TileKey a_2 = TileKey{14, 0, 0};
    DemBytes t = DemBytes{0, 0, 0};
    f64 px = 0.0;
    f64 py = 0.0;
    u32 a_3 = 0;
    u32 b = 0;
    f32 ax = 0.0f;
    f32 ay = 0.0f;
    Fixed<f32, 4> v = Fixed<f32, 4>{};
    u64 h = 0;
    origin = DemOrigin{0.0, 0.0};
    win = DemWindow{0.0f, 0.0f, 10.0f, 0, 160.0f, 30.0f, 0};
    if (math_compare<"<", u64>(num_cast<u64, i64>(list_length<u8>(in)), 44)) {
      return;
    }
    origin = struct_of_bytes<DemOrigin, "16: f64@0 f64@8">(in, 0);
    win = struct_of_bytes<DemWindow, "28: f32@0 f32@4 f32@8 u32@12 f32@16 f32@20 u32@24">(in, 16);
    if (logic_or(math_compare<"<", u32>(win.n, 2), math_compare<">", u32>(win.n, 4096))) {
      return;
    }
    n = win.n;
    x0 = win.x0;
    y0 = win.y0;
    step = win.step;
    p = 44;
    tiles = Map<u64, DemBytes>{};
    for (auto it18 = flow_for(n18, 0, math_sub<i64>(num_cast<i64, u32>(win.count), 1)); it18.next();) {
      head = DemTileHead{0, 0, 0, 0};
      if (math_compare<"<", u64>(kern::formula<u64, u64>([](auto a, auto p) { return (a - p); }, num_cast<u64, i64>(list_length<u8>(in)), p), 16)) {
        return;
      }
      head = struct_of_bytes<DemTileHead, "16: u32@0 u32@4 u32@8 u32@12">(in, num_cast<i64, u64>(p));
      p += 16;
      bytes = kern::formula<u32, u32>([](auto w, auto h) { return ((num_cast<u64, u32>(w) * num_cast<u64, u32>(h)) * 4); }, head.w, head.h);
      if (logic_or(logic_or(math_compare<"<", u32>(head.w, 2), math_compare<"<", u32>(head.h, 2)), math_compare<"<", u64>(kern::formula<u64, u64>([](auto a, auto p) { return (a - p); }, num_cast<u64, i64>(list_length<u8>(in)), p), bytes))) {
        return;
      }
      put = struct_make<DemBytes>(head.w, head.h, p);
      a = struct_make<TileKey>(kDemZoom, num_cast<i32, u32>(head.x), num_cast<i32, u32>(head.y));
      key = tiles::TileKey_hash(a).Result;
      map_add(n19, tiles, key);
      tiles[n19.index] = put;
      p += bytes;
    }
    proj = GeoProjector{0.0, 0.0, 1.0};
    engine_geo_projector_init(n20, proj, origin.lon0, origin.lat0);
    init = n20.result;
    proj = n20.self;
    if (logic_not(init)) {
      return;
    }
    made = List<f32>{};
    list_resize(made, num_cast<i64, u64>(kern::formula<u32>([](auto n) { return (num_cast<u64, u32>(n) * num_cast<u64, u32>(n)); }, n)), 0.0f);
    grid = made;
    for (auto it21 = flow_for(n21, 0, math_sub<i64>(num_cast<i64, u32>(n), 1)); it21.next();) {
      y = kern::formula<f32, i64, f32>([](auto y0, auto index, auto step) { return (y0 + (num_cast<f32, u32>(num_cast<u32, i64>(index)) * step)); }, y0, n21.index, step);
      for (auto it22 = flow_for(n22, 0, math_sub<i64>(num_cast<i64, u32>(n), 1)); it22.next();) {
        lon = 0.0;
        lat = 0.0;
        tiles_cpp::unproject(proj, lon, lat, kern::formula<f32, i64, f32>([](auto x0, auto index, auto step) { return num_cast<f64, f32>((x0 + (num_cast<f32, u32>(num_cast<u32, i64>(index)) * step))); }, x0, n22.index, step), num_cast<f64, f32>(y));
        fx = merc_x(lon, kDemZoom).Result;
        fy = merc_y(lat, kDemZoom).Result;
        tx = kern::formula<f64>([](auto fx) { return num_cast<i32, f64>(floor(fx)); }, fx);
        ty = kern::formula<f64>([](auto fy) { return num_cast<i32, f64>(floor(fy)); }, fy);
        a_2 = struct_make<TileKey>(kDemZoom, tx, ty);
        it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, DemBytes>(tiles, tiles::TileKey_hash(a_2).Result).index, map_length<u64, DemBytes>(tiles));
        if (math_compare<"==", i64>(it, map_length<u64, DemBytes>(tiles))) {
          return;
        }
        t = tiles[it];
        px = kern::formula<u32, f64, i32>([](auto w, auto fx, auto tx) { return min((num_cast<f64, u32>(w) - 1.001), ((fx - num_cast<f64, i32>(tx)) * num_cast<f64, u32>(w))); }, t.w, fx, tx);
        py = kern::formula<u32, f64, i32>([](auto h, auto fy, auto ty) { return min((num_cast<f64, u32>(h) - 1.001), ((fy - num_cast<f64, i32>(ty)) * num_cast<f64, u32>(h))); }, t.h, fy, ty);
        a_3 = num_cast<u32, f64>(px);
        b = num_cast<u32, f64>(py);
        ax = kern::formula<f64, u32>([](auto px, auto a_3) { return num_cast<f32, f64>((px - num_cast<f64, u32>(a_3))); }, px, a_3);
        ay = kern::formula<f64, u32>([](auto py, auto b) { return num_cast<f32, f64>((py - num_cast<f64, u32>(b))); }, py, b);
        v = List<f32>{};
        h = kern::formula<u64, u32, u32, u32>([](auto at, auto b, auto w, auto a_3) { return (at + (((num_cast<u64, u32>(b) * num_cast<u64, u32>(w)) + num_cast<u64, u32>(a_3)) * 4)); }, t.at, b, t.w, a_3);
        list_copy_bytes<"f32", "u8">(v, 0, in, num_cast<i64, u64>(h), 8);
        list_copy_bytes<"f32", "u8">(v, 2, in, math_add<i64>(num_cast<i64, u64>(h), num_cast<i64, u64>(kern::formula<u32>([](auto w) { return (num_cast<u64, u32>(w) * 4); }, t.w))), 8);
        grid[num_cast<i64, u64>(kern::formula<i64, u32, i64>([](auto index, auto n, auto index_2) { return ((num_cast<u64, u32>(num_cast<u32, i64>(index)) * num_cast<u64, u32>(n)) + num_cast<u64, u32>(num_cast<u32, i64>(index_2))); }, n21.index, n, n22.index))] = kern::formula<f32, f32, f32, f32, f32, f32>([](auto a, auto ax, auto a_2, auto ay, auto a_3, auto a_4) { return ((((a * (1.0f - ax)) + (a_2 * ax)) * (1.0f - ay)) + (((a_3 * (1.0f - ax)) + (a_4 * ax)) * ay)); }, v[0], ax, v[1], ay, v[2], v[3]);
      }
    }
    bare_earth(grid, n, step, win.open, win.blur);
    list_resize(out, num_cast<i64, u64>(kern::formula<u64>([](auto a) { return (a * 4); }, num_cast<u64, i64>(list_length<f32>(grid)))), 0);
    list_copy_bytes<"u8", "f32">(out, 0, grid, 0, list_length<u8>(out));
  }

  void Terrain_clear(Terrain& self) {
    bool remove = false;
    bool remove_2 = false;
    bool valid = false;
    bool body_valid = false;
    bool body_valid_2 = false;
    bool both = false;
    if (object_valid<kern::Renderer>(self.r_)) {
      for (auto it23 = flow_for(n23, 0, math_sub<i64>(map_length<u64, Terrain_RenderTile>(self.render_), 1)); it23.next();) {
        if (num_cast<bool, u32>(self.render_[n23.index].mesh)) {
          engine_renderer_remove(n24, self.r_, self.render_[n23.index].mesh);
          remove = n24.result;
        }
      }
      for (auto it25 = flow_for(n25, 0, math_sub<i64>(map_length<u64, u32>(self.water_meshes_), 1)); it25.next();) {
        if (num_cast<bool, u32>(self.water_meshes_[n25.index])) {
          engine_renderer_remove(n26, self.r_, self.water_meshes_[n25.index]);
          remove_2 = n26.result;
        }
      }
    }
    if (object_valid<kern::physics::World>(self.world_)) {
      for (auto it27 = flow_for(n27, 0, math_sub<i64>(map_length<u64, Terrain_CollisionTile>(self.collision_), 1)); it27.next();) {
        valid = engine_body_valid(self.collision_[n27.index].body);
        both = valid;
        if (both) {
          body_valid_2 = engine_world_body_valid(self.world_, self.collision_[n27.index].body);
          both = body_valid_2;
        }
        if (both) {
          engine_world_destroy_body(self.world_, self.collision_[n27.index].body);
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
    self.image_at_ = Vector{1e+09, 1e+09, 0.0};
    map_clear(self.dem_tiles_);
    list_clear(self.requests_);
    if (logic_and(math_compare<">", i64>(list_length<Terrain_Job>(self.job_), 0), num_cast<bool, u32>(self.job_[0].id))) {
      engine_jobs_cancel(self.job_[0].id);
    }
    self.job_ = List<Terrain_Job>{};
    self.grid_ = struct_make<Terrain_Grid>(0.0f, 0.0f, 10.0f, 0, List<f32>{}, 0.0f, 0.0f);
    list_clear(self.side_);
    list_clear(self.corridors_);
    self.grid_ready_ = false;
    self.ref_ready_ = false;
    self.ref_height_ = 0.0f;
    self.dem_failures_ = 0;
    self.retry_at_ = 0.0;
    list_clear(self.changes_);
    self.replan_ = true;
    self.planned_anchor_ = Vector{1e+09, 1e+09, 0.0};
    self.planned_focus_ = self.planned_anchor_;
    self.stats_ = struct_make<TerrainStats>(0, 0, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, false, false);
    self.settled_ = logic_not(self.dem_);
  }

  void Terrain_start(Terrain& self, const GeoProjector& proj, Object<kern::Renderer> renderer = Object<kern::Renderer>{}, Object<kern::physics::World> world = Object<kern::physics::World>{}, bool dem = false) {
    Terrain_clear(self);
    self.r_ = renderer;
    self.world_ = world;
    self.proj_ = proj;
    self.dem_ = dem;
    self.settled_ = logic_not(dem);
  }

  void Terrain_disable_dem(Terrain& self) {
    self.dem_ = false;
    if (logic_and(math_compare<">", i64>(list_length<Terrain_Job>(self.job_), 0), num_cast<bool, u32>(self.job_[0].id))) {
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
    for (auto it28 = flow_for(n28, 0, math_sub<i64>(list_length<TileKey>(self.requests_), 1)); it28.next();) {
      b = "";
      b = text_format<"%s[%d,%d,%d]">(63, logic_select<Text>(num_cast<bool, u64>(num_cast<u64, i64>(n28.index)), ",", ""), num_cast<i64, i32>(self.requests_[n28.index].z), num_cast<i64, i32>(self.requests_[n28.index].x), num_cast<i64, i32>(self.requests_[n28.index].y));
      s = text_append(s, b, "");
    }
    list_clear(self.requests_);
    return {text_append(s, "]", "")};
  }

  void Terrain_provide(Terrain& self, const TileKey& key, List<u8> rgba = List<u8>{}, u32 width = 0, u32 height = 0) {
    i64 it = 0;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, Terrain_DemTile>(self.dem_tiles_, tiles::TileKey_hash(key).Result).index, map_length<u64, Terrain_DemTile>(self.dem_tiles_));
    if (logic_or(logic_or(math_compare<"==", i64>(it, map_length<u64, Terrain_DemTile>(self.dem_tiles_)), math_compare<"==", u32>(width, 0)), math_compare<"==", u32>(height, 0))) {
      return;
    }
    self.dem_tiles_[it].w = width;
    self.dem_tiles_[it].hgt = height;
    list_resize(self.dem_tiles_[it].h, num_cast<i64, u64>(kern::formula<u32, u32>([](auto width, auto height) { return (num_cast<u64, u32>(width) * num_cast<u64, u32>(height)); }, width, height)), 0.0f);
    for (auto it29 = flow_for(n29, 0, math_sub<i64>(list_length<f32>(self.dem_tiles_[it].h), 1)); it29.next();) {
      self.dem_tiles_[it].h[n29.index] = terrain::terrarium_height(rgba[num_cast<i64, u64>(kern::formula<i64>([](auto index) { return (num_cast<u64, i64>(index) * 4); }, n29.index))], rgba[num_cast<i64, u64>(kern::formula<i64>([](auto index) { return ((num_cast<u64, i64>(index) * 4) + 1); }, n29.index))], rgba[num_cast<i64, u64>(kern::formula<i64>([](auto index) { return ((num_cast<u64, i64>(index) * 4) + 2); }, n29.index))]).Result;
    }
    self.dem_tiles_[it].failed = false;
  }

  void Terrain_fail(Terrain& self, const TileKey& key) {
    i64 it = 0;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, Terrain_DemTile>(self.dem_tiles_, tiles::TileKey_hash(key).Result).index, map_length<u64, Terrain_DemTile>(self.dem_tiles_));
    if (math_compare<"==", i64>(it, map_length<u64, Terrain_DemTile>(self.dem_tiles_))) {
      return;
    }
    self.dem_tiles_[it].failed = true;
    self.stats_.dem_failed = kern::formula<u32>([](auto dem_failed) { return (dem_failed + 1); }, self.stats_.dem_failed);
  }

  struct Terrain_sample_demResult { f32 Result = 0.0f; };
  Terrain_sample_demResult Terrain_sample_dem(const Terrain& self, bool& ok, f64 lon = 0.0, f64 lat = 0.0) {
    f64 fx = 0.0;
    f64 fy = 0.0;
    i32 tx = 0;
    i32 ty = 0;
    i64 it = 0;
    TileKey a = TileKey{14, 0, 0};
    Terrain_DemTile t = Terrain_DemTile{TileKey{14, 0, 0}, List<f32>{}, 0, 0, false, false};
    f64 px = 0.0;
    f64 py = 0.0;
    u32 i = 0;
    u32 j = 0;
    f32 ax = 0.0f;
    f32 ay = 0.0f;
    u64 at = 0;
    fx = merc_x(lon, kDemZoom).Result;
    fy = merc_y(lat, kDemZoom).Result;
    tx = kern::formula<f64>([](auto fx) { return num_cast<i32, f64>(floor(fx)); }, fx);
    ty = kern::formula<f64>([](auto fy) { return num_cast<i32, f64>(floor(fy)); }, fy);
    a = struct_make<TileKey>(kDemZoom, tx, ty);
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, Terrain_DemTile>(self.dem_tiles_, tiles::TileKey_hash(a).Result).index, map_length<u64, Terrain_DemTile>(self.dem_tiles_));
    if (logic_or(math_compare<"==", i64>(it, map_length<u64, Terrain_DemTile>(self.dem_tiles_)), math_compare<"==", i64>(list_length<f32>(self.dem_tiles_[it].h), 0))) {
      ok = false;
      return {0.0f};
    }
    t = self.dem_tiles_[it];
    px = kern::formula<u32, f64, i32>([](auto w, auto fx, auto tx) { return min((num_cast<f64, u32>(w) - 1.001), ((fx - num_cast<f64, i32>(tx)) * num_cast<f64, u32>(w))); }, t.w, fx, tx);
    py = kern::formula<u32, f64, i32>([](auto hgt, auto fy, auto ty) { return min((num_cast<f64, u32>(hgt) - 1.001), ((fy - num_cast<f64, i32>(ty)) * num_cast<f64, u32>(hgt))); }, t.hgt, fy, ty);
    i = num_cast<u32, f64>(px);
    j = num_cast<u32, f64>(py);
    ax = kern::formula<f64, u32>([](auto px, auto i) { return num_cast<f32, f64>((px - num_cast<f64, u32>(i))); }, px, i);
    ay = kern::formula<f64, u32>([](auto py, auto j) { return num_cast<f32, f64>((py - num_cast<f64, u32>(j))); }, py, j);
    at = kern::formula<u32, u32, u32>([](auto j, auto w, auto i) { return ((num_cast<u64, u32>(j) * num_cast<u64, u32>(w)) + num_cast<u64, u32>(i)); }, j, t.w, i);
    return {kern::formula<f32, f32, f32, f32, f32, f32>([](auto a, auto ax, auto a_2, auto ay, auto a_3, auto a_4) { return ((((a * (1.0f - ax)) + (a_2 * ax)) * (1.0f - ay)) + (((a_3 * (1.0f - ax)) + (a_4 * ax)) * ay)); }, t.h[num_cast<i64, u64>(at)], ax, t.h[num_cast<i64, u64>(kern::formula<u64>([](auto at) { return (at + 1); }, at))], ay, t.h[num_cast<i64, u64>(kern::formula<u64, u32>([](auto at, auto w) { return (at + num_cast<u64, u32>(w)); }, at, t.w))], t.h[num_cast<i64, u64>(kern::formula<u64, u32>([](auto at, auto w) { return ((at + num_cast<u64, u32>(w)) + 1); }, at, t.w))])};
  }

  struct rect_inResult { bool Result = false; };
  KERN_PURE rect_inResult rect_in(const Vec4& in, const Vec4& out) {
    return {logic_and(logic_and(logic_and(math_compare<">=", f32>(in.x, out.x), math_compare<">=", f32>(in.y, out.y)), math_compare<"<=", f32>(in.z, out.z)), math_compare<"<=", f32>(in.w, out.w))};
  }

  struct Terrain_coversResult { bool Result = false; };
  KERN_PURE Terrain_coversResult Terrain_covers(const Terrain& self, const Vec4& rect) {
    Vec4 a = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vec4 a_2 = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
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
    for (auto it30 = flow_for(n30, 0, math_sub<i64>(list_length<Terrain_Grid>(self.side_), 1)); it30.next();) {
      a_2 = Terrain_interior(self.side_[n30.index]).Result;
      if (rect_in(rect, a_2).Result) {
        return {true};
      }
    }
    return {false};
  }

  struct Terrain_plan_demResult { bool Result = false; };
  Terrain_plan_demResult Terrain_plan_dem(Terrain& self, Vector centre = Vector{0.0, 0.0, 0.0}, f32 half = 0.0f, bool side = false) {
    f64 now = 0.0;
    f64 now_ms = 0.0;
    f32 cx = 0.0f;
    f32 cy = 0.0f;
    f64 lon_a = 0.0;
    f64 lat_a = 0.0;
    f64 lon_b = 0.0;
    f64 lat_b = 0.0;
    i32 ix0 = 0;
    i32 iy0 = 0;
    i32 ix1 = 0;
    i32 iy1 = 0;
    bool all = false;
    bool failed = false;
    TileKey k = TileKey{14, 0, 0};
    u64 key = 0;
    i64 t_row = 0;
    i64 it = 0;
    i32 x = 0;
    i32 y = 0;
    i64 at = 0;
    if (logic_or(logic_not(self.dem_), math_compare<">", i64>(list_length<Terrain_Job>(self.job_), 0))) {
      return {false};
    }
    now_ms = engine_now_ms();
    now = now_ms;
    if (math_compare<"<", f64>(now, self.retry_at_)) {
      return {false};
    }
    cx = kern::formula<f32>([](auto x) { return (round((x / 50.0f)) * 50.0f); }, vector_break(centre).x);
    cy = kern::formula<f32>([](auto y) { return (round((y / 50.0f)) * 50.0f); }, vector_break(centre).y);
    lon_a = 0.0;
    lat_a = 0.0;
    lon_b = 0.0;
    lat_b = 0.0;
    tiles_cpp::unproject(self.proj_, lon_a, lat_a, kern::formula<f32, f32>([](auto cx, auto half) { return num_cast<f64, f32>((cx - half)); }, cx, half), kern::formula<f32, f32>([](auto cy, auto half) { return num_cast<f64, f32>((cy + half)); }, cy, half));
    tiles_cpp::unproject(self.proj_, lon_b, lat_b, kern::formula<f32, f32>([](auto cx, auto half) { return num_cast<f64, f32>((cx + half)); }, cx, half), kern::formula<f32, f32>([](auto cy, auto half) { return num_cast<f64, f32>((cy - half)); }, cy, half));
    ix0 = kern::formula<f64>([](auto the_result) { return num_cast<i32, f64>(floor(the_result)); }, merc_x(lon_a, kDemZoom).Result);
    iy0 = kern::formula<f64>([](auto the_result) { return num_cast<i32, f64>(floor(the_result)); }, merc_y(lat_a, kDemZoom).Result);
    ix1 = kern::formula<f64>([](auto the_result) { return num_cast<i32, f64>(floor(the_result)); }, merc_x(lon_b, kDemZoom).Result);
    iy1 = kern::formula<f64>([](auto the_result) { return num_cast<i32, f64>(floor(the_result)); }, merc_y(lat_b, kDemZoom).Result);
    if (math_compare<">", i32>(kern::formula<i32, i32, i32, i32>([](auto ix1, auto ix0, auto iy1, auto iy0) { return (((ix1 - ix0) + 1) * ((iy1 - iy0) + 1)); }, ix1, ix0, iy1, iy0), 100)) {
      return {false};
    }
    all = true;
    failed = false;
    for (auto it32 = flow_for(n32, num_cast<i64, i32>(iy0), num_cast<i64, i32>(iy1)); it32.next();) {
      for (auto it33 = flow_for(n33, num_cast<i64, i32>(ix0), num_cast<i64, i32>(ix1)); it33.next();) {
        k = struct_make<TileKey>(kDemZoom, num_cast<i32, i64>(n33.index), num_cast<i32, i64>(n32.index));
        key = tiles::TileKey_hash(k).Result;
        map_add(n34, self.dem_tiles_, key);
        t_row = n34.index;
        if (logic_not(self.dem_tiles_[t_row].requested)) {
          self.dem_tiles_[t_row].requested = true;
          self.dem_tiles_[t_row].key = k;
          list_add(self.requests_, k);
        }
        if (self.dem_tiles_[t_row].failed) {
          failed = true;
        }
        if (math_compare<"==", i64>(list_length<f32>(self.dem_tiles_[t_row].h), 0)) {
          all = false;
        }
      }
    }
    if (failed) {
      self.dem_failures_ = kern::formula<u32>([](auto dem_failures_) { return (dem_failures_ + 1); }, self.dem_failures_);
      if (logic_not(self.grid_ready_)) {
        Terrain_disable_dem(self);
        return {false};
      }
      for (auto it35 = flow_for(n35, 0, math_sub<i64>(map_length<u64, Terrain_DemTile>(self.dem_tiles_), 1)); it35.next();) {
        if (self.dem_tiles_[n35.index].failed) {
          self.dem_tiles_[n35.index].failed = false;
          self.dem_tiles_[n35.index].requested = false;
        }
      }
      self.retry_at_ = kern::formula<f64, u32>([](auto now, auto dem_failures_) { return (now + min(15000.0, (1500.0 * num_cast<f64, u32>(dem_failures_)))); }, now, self.dem_failures_);
      return {false};
    }
    if (logic_not(all)) {
      return {false};
    }
    self.dem_failures_ = 0;
    if (logic_not(side)) {
      it = 0;
      while (loop(logic_not(math_compare<"==", i64>(it, map_length<u64, Terrain_DemTile>(self.dem_tiles_))))) {
        x = self.dem_tiles_[it].key.x;
        y = self.dem_tiles_[it].key.y;
        if (logic_or(logic_or(logic_or(math_compare<"<", i32>(x, kern::formula<i32>([](auto ix0) { return (ix0 - 1); }, ix0)), math_compare<">", i32>(x, kern::formula<i32>([](auto ix1) { return (ix1 + 1); }, ix1))), math_compare<"<", i32>(y, kern::formula<i32>([](auto iy0) { return (iy0 - 1); }, iy0))), math_compare<">", i32>(y, kern::formula<i32>([](auto iy1) { return (iy1 + 1); }, iy1)))) {
          at = it;
          map_remove_at(self.dem_tiles_, at);
          it = at;
        } else {
          it += 1;
        }
      }
    }
    self.job_ = list_make<Terrain_Job, "1">(Terrain_Job{Terrain_Grid{0.0f, 0.0f, 10.0f, 0, List<f32>{}, 0.0f, 0.0f}, 0, false, 0, 0, 0, 0});
    self.job_[0].side = side;
    self.job_[0].x0 = ix0;
    self.job_[0].y0 = iy0;
    self.job_[0].x1 = ix1;
    self.job_[0].y1 = iy1;
    self.job_[0].g.step = kDemStep;
    self.job_[0].g.n = kern::formula<f32, f32>([](auto half, auto kDemStep) { return (num_cast<u32, i64>(num_cast<i64, f32>(round(((half * 2.0f) / kDemStep)))) + 1); }, half, kDemStep);
    self.job_[0].g.x0 = kern::formula<f32, f32>([](auto cx, auto half) { return (cx - half); }, cx, half);
    self.job_[0].g.y0 = kern::formula<f32, f32>([](auto cy, auto half) { return (cy - half); }, cy, half);
    self.job_[0].g.cx = cx;
    self.job_[0].g.cy = cy;
    list_resize(self.job_[0].g.h, num_cast<i64, u64>(kern::formula<u32>([](auto a) { return (num_cast<u64, u32>(a) * num_cast<u64, u32>(a)); }, self.job_[0].g.n)), 0.0f);
    return {true};
  }

  struct Terrain_plan_windows_matchResult { bool Result = false; };
  /// far mates: their own windows (the ground there is the real one for the host's traffic too)
  Terrain_plan_windows_matchResult Terrain_plan_windows_match(Terrain& self, const Terrain_Grid& g) {
    Vec4 a = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    for (auto it37 = flow_for(n37, 1, math_sub<i64>(list_length<Vec4>(self.needs), 1)); it37.next();) {
      a = Terrain_interior(g).Result;
      if (rect_in(self.needs[n37.index], a).Result) {
        return {false};
      }
    }
    return {true};
  }

  void Terrain_plan_windows(Terrain& self, Vector focus = Vector{0.0, 0.0, 0.0}) {
    Vec4 own = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vec4 all = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    bool together = false;
    Vec4 want = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    f32 tile = 0.0f;
    f32 slack = 0.0f;
    bool planned = false;
    Vec4 a = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vec4 a_2 = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vec4 a_3 = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    bool either = false;
    Vector c = Vector{0.0, 0.0, 0.0};
    f32 lead_max = 0.0f;
    f32 half_need = 0.0f;
    f32 half = 0.0f;
    i64 at = 0;
    i64 kept = 0;
    i64 end = 0;
    bool skip = false;
    f32 half_2 = 0.0f;
    if (logic_or(logic_not(self.dem_), math_compare<">", i64>(list_length<Terrain_Job>(self.job_), 0))) {
      return;
    }
    own = engine_math_math::Vec4_new(kern::formula<f32>([](auto x) { return (x - 1500.0f); }, vector_break(focus).x), kern::formula<f32>([](auto y) { return (y - 1500.0f); }, vector_break(focus).y), kern::formula<f32>([](auto x) { return (x + 1500.0f); }, vector_break(focus).x), kern::formula<f32>([](auto y) { return (y + 1500.0f); }, vector_break(focus).y)).Result;
    if (logic_not(math_compare<"==", i64>(list_length<Vec4>(self.needs), 0))) {
      own = self.needs[0];
    }
    all = own;
    for (auto it31 = flow_for(n31, 1, math_sub<i64>(list_length<Vec4>(self.needs), 1)); it31.next();) {
      all = engine_math_math::Vec4_new(kern::formula<f32, f32>([](auto x, auto a) { return fmin(x, a); }, all.x, self.needs[n31.index].x), kern::formula<f32, f32>([](auto y, auto a) { return fmin(y, a); }, all.y, self.needs[n31.index].y), kern::formula<f32, f32>([](auto z, auto a) { return fmax(z, a); }, all.z, self.needs[n31.index].z), kern::formula<f32, f32>([](auto w, auto a) { return fmax(w, a); }, all.w, self.needs[n31.index].w)).Result;
    }
    together = logic_and(logic_and(math_compare<">", u64>(num_cast<u64, i64>(list_length<Vec4>(self.needs)), 1), math_compare<"<", f32>(kern::formula<f32, f32>([](auto z, auto x) { return (z - x); }, all.z, all.x), kern::formula<f32, f32>([](auto kDemMaxSize, auto kDemMargin) { return ((kDemMaxSize - (2.0f * kDemMargin)) - 800.0f); }, kDemMaxSize, kDemMargin))), math_compare<"<", f32>(kern::formula<f32, f32>([](auto w, auto y) { return (w - y); }, all.w, all.y), kern::formula<f32, f32>([](auto kDemMaxSize, auto kDemMargin) { return ((kDemMaxSize - (2.0f * kDemMargin)) - 800.0f); }, kDemMaxSize, kDemMargin)));
    want = logic_select<Vec4>(together, all, own);
    tile = kern::formula<f32, f32>([](auto z, auto x) { return fmax(400.0f, ((z - x) / 3.0f)); }, own.z, own.x);
    slack = kern::formula<f32>([](auto tile) { return fmin(400.0f, (tile * 0.25f)); }, tile);
    planned = false;
    either = logic_not(self.grid_ready_);
    if (logic_not(either)) {
      a_2 = engine_math_math::Vec4_new(kern::formula<f32, f32>([](auto x, auto slack) { return (x - slack); }, want.x, slack), kern::formula<f32, f32>([](auto y, auto slack) { return (y - slack); }, want.y, slack), kern::formula<f32, f32>([](auto z, auto slack) { return (z + slack); }, want.z, slack), kern::formula<f32, f32>([](auto w, auto slack) { return (w + slack); }, want.w, slack)).Result;
      a_3 = Terrain_interior(self.grid_).Result;
      either = logic_not(rect_in(a_2, a_3).Result);
    }
    if (either) {
      c = vector_make(kern::formula<f32, f32>([](auto x, auto z) { return ((x + z) / 2.0f); }, want.x, want.z), kern::formula<f32, f32>([](auto y, auto w) { return ((y + w) / 2.0f); }, want.y, want.w), 0.0f);
      lead_max = tile;
      c = vector_make(kern::formula<f32, f32>([](auto x, auto the_result) { return (x + the_result); }, vector_break(c).x, common::clampf(vector_break(self.lead).x, kern::formula<f32>([](auto lead_max) { return (-lead_max); }, lead_max), lead_max).Result), vector_break(c).y, vector_break(c).z);
      c = vector_make(vector_break(c).x, kern::formula<f32, f32>([](auto y, auto the_result) { return (y + the_result); }, vector_break(c).y, common::clampf(vector_break(self.lead).y, kern::formula<f32>([](auto lead_max) { return (-lead_max); }, lead_max), lead_max).Result), vector_break(c).z);
      half_need = kern::formula<f32, f32, f32, f32, f32, f32, f32>([](auto z, auto x, auto w, auto y, auto lead_max, auto kDemMargin, auto tile) { return ((((fmax((z - x), (w - y)) / 2.0f) + lead_max) + kDemMargin) + tile); }, want.z, want.x, want.w, want.y, lead_max, kDemMargin, tile);
      half = common::clampf(kern::formula<f32, f32>([](auto kDemSize, auto half_need) { return (ceil((fmax((kDemSize / 2.0f), half_need) / 50.0f)) * 50.0f); }, kDemSize, half_need), kern::formula<f32>([](auto kDemSize) { return (kDemSize / 2.0f); }, kDemSize), kern::formula<f32>([](auto kDemMaxSize) { return (kDemMaxSize / 2.0f); }, kDemMaxSize)).Result;
      n36 = Terrain_plan_dem(self, c, half, false);
      planned = n36.Result;
      if (logic_or(planned, math_compare<">", i64>(list_length<Terrain_Job>(self.job_), 0))) {
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
    while (loop(math_compare<"<", i64>(at, end))) {
      n38 = Terrain_plan_windows_match(self, self.side_[at]);
      if (logic_not(n38.Result)) {
        if (math_compare<"<", i64>(kept, at)) {
          var_take(n39, self.side_[at]);
          self.side_[kept] = n39.value;
        }
        kept += 1;
      }
      at += 1;
    }
    list_resize(self.side_, kept, Terrain_Grid{0.0f, 0.0f, 10.0f, 0, List<f32>{}, 0.0f, 0.0f});
    for (auto it40 = flow_for(n40, 1, math_sub<i64>(list_length<Vec4>(self.needs), 1)); it40.next();) {
      skip = false;
      if (logic_or(Terrain_covers(self, self.needs[n40.index]).Result, math_compare<">=", u64>(num_cast<u64, i64>(list_length<Terrain_Grid>(self.side_)), 3))) {
        skip = true;
      }
      if (logic_not(skip)) {
        half_2 = common::clampf(kern::formula<f32, f32, f32, f32, f32>([](auto a, auto a_2, auto a_3, auto a_4, auto kDemMargin) { return (ceil(((((fmax((a - a_2), (a_3 - a_4)) / 2.0f) + kDemMargin) + 400.0f) / 50.0f)) * 50.0f); }, self.needs[n40.index].z, self.needs[n40.index].x, self.needs[n40.index].w, self.needs[n40.index].y, kDemMargin), 1500.0f, kern::formula<f32>([](auto kDemMaxSize) { return (kDemMaxSize / 2.0f); }, kDemMaxSize)).Result;
        n41 = Terrain_plan_dem(self, vector_make(kern::formula<f32, f32>([](auto a, auto a_2) { return ((a + a_2) / 2.0f); }, self.needs[n40.index].x, self.needs[n40.index].z), kern::formula<f32, f32>([](auto a, auto a_2) { return ((a + a_2) / 2.0f); }, self.needs[n40.index].y, self.needs[n40.index].w), 0.0f), half_2, true);
        if (logic_or(n41.Result, math_compare<">", i64>(list_length<Terrain_Job>(self.job_), 0))) {
          return;
        }
      }
    }
  }

  struct Terrain_run_job_mineResult { bool Result = false; };
  /// Sampling the window (641k points) and the bare-earth filter run off the frame (a Web Worker / thread), like MapLibre's raster-dem workers do it for the original. (kDemJob's input - see dem_grid_job: the numbers of each struct as their bytes, the heights of each tile as theirs, one after the other)
  Terrain_run_job_mineResult Terrain_run_job_mine(Terrain& self, const Terrain_DemTile& t) {
    return {logic_and(logic_and(logic_and(logic_and(logic_not(math_compare<"==", i64>(list_length<f32>(t.h), 0)), math_compare<">=", i32>(t.key.x, self.job_[0].x0)), math_compare<"<=", i32>(t.key.x, self.job_[0].x1)), math_compare<">=", i32>(t.key.y, self.job_[0].y0)), math_compare<"<=", i32>(t.key.y, self.job_[0].y1))};
  }

  void Terrain_dirty(Terrain& self, f32 x0 = 0.0f, f32 y0 = 0.0f, f32 x1 = 0.0f, f32 y1 = 0.0f, bool notify = false) {
    f32 x0_ = 0.0f;
    f32 y0_ = 0.0f;
    f32 x1_ = 0.0f;
    f32 y1_ = 0.0f;
    f32 m = 0.0f;
    i32 level = 0;
    i32 i = 0;
    i32 j = 0;
    f32 size = 0.0f;
    u64 key = 0;
    i32 i_2 = 0;
    i32 j_2 = 0;
    u64 key_2 = 0;
    i32 i_3 = 0;
    i32 j_3 = 0;
    u64 key_3 = 0;
    x0_ = x0;
    y0_ = y0;
    x1_ = x1;
    y1_ = y1;
    m = kern::formula<f32>([](auto kStep) { return (kStep * 2.0f); }, kStep);
    x0_ = kern::formula<f32, f32>([](auto x0_, auto m) { return (x0_ - m); }, x0_, m);
    y0_ = kern::formula<f32, f32>([](auto y0_, auto m) { return (y0_ - m); }, y0_, m);
    x1_ += m;
    y1_ += m;
    for (auto it48 = flow_for(n48, 0, math_sub<i64>(map_length<u64, Terrain_RenderTile>(self.render_), 1)); it48.next();) {
      level = 0;
      i = 0;
      j = 0;
      unkey3(level, i, j, map_key_at<u64, Terrain_RenderTile>(self.render_, n48.index));
      size = kern::formula<i32, f32, f32>([](auto level, auto kBlock, auto kTile) { return ((level == 2) ? kBlock : kTile); }, level, kBlock, kTile);
      if (logic_and(logic_and(logic_and(math_compare<"<=", f32>(kern::formula<i32, f32>([](auto i, auto size) { return (num_cast<f32, i32>(i) * size); }, i, size), x1_), math_compare<">=", f32>(kern::formula<i32, f32>([](auto i, auto size) { return (num_cast<f32, i32>((i + 1)) * size); }, i, size), x0_)), math_compare<"<=", f32>(kern::formula<i32, f32>([](auto j, auto size) { return (num_cast<f32, i32>(j) * size); }, j, size), y1_)), math_compare<">=", f32>(kern::formula<i32, f32>([](auto j, auto size) { return (num_cast<f32, i32>((j + 1)) * size); }, j, size), y0_))) {
        key = map_key_at<u64, Terrain_RenderTile>(self.render_, n48.index);
        set_add(n49, self.dirty_render_, key);
      }
    }
    for (auto it50 = flow_for(n50, 0, math_sub<i64>(map_length<u64, Terrain_CollisionTile>(self.collision_), 1)); it50.next();) {
      i_2 = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a >> 32)); }, map_key_at<u64, Terrain_CollisionTile>(self.collision_, n50.index));
      j_2 = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a & 4294967295)); }, map_key_at<u64, Terrain_CollisionTile>(self.collision_, n50.index));
      if (logic_and(logic_and(logic_and(math_compare<"<=", f32>(kern::formula<i32, f32, f32>([](auto i_2, auto kTile, auto kStep) { return ((num_cast<f32, i32>(i_2) * kTile) - kStep); }, i_2, kTile, kStep), x1_), math_compare<">=", f32>(kern::formula<i32, f32, f32>([](auto i_2, auto kTile, auto kStep) { return ((num_cast<f32, i32>((i_2 + 1)) * kTile) + kStep); }, i_2, kTile, kStep), x0_)), math_compare<"<=", f32>(kern::formula<i32, f32, f32>([](auto j_2, auto kTile, auto kStep) { return ((num_cast<f32, i32>(j_2) * kTile) - kStep); }, j_2, kTile, kStep), y1_)), math_compare<">=", f32>(kern::formula<i32, f32, f32>([](auto j_2, auto kTile, auto kStep) { return ((num_cast<f32, i32>((j_2 + 1)) * kTile) + kStep); }, j_2, kTile, kStep), y0_))) {
        key_2 = map_key_at<u64, Terrain_CollisionTile>(self.collision_, n50.index);
        set_add(n51, self.dirty_collision_, key_2);
      }
    }
    for (auto it52 = flow_for(n52, 0, math_sub<i64>(map_length<u64, u32>(self.water_meshes_), 1)); it52.next();) {
      i_3 = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a >> 32)); }, map_key_at<u64, u32>(self.water_meshes_, n52.index));
      j_3 = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a & 4294967295)); }, map_key_at<u64, u32>(self.water_meshes_, n52.index));
      if (logic_and(logic_and(logic_and(math_compare<"<=", f32>(kern::formula<i32, f32>([](auto i_3, auto kWaterChunk) { return (num_cast<f32, i32>(i_3) * kWaterChunk); }, i_3, kWaterChunk), x1_), math_compare<">=", f32>(kern::formula<i32, f32>([](auto i_3, auto kWaterChunk) { return (num_cast<f32, i32>((i_3 + 1)) * kWaterChunk); }, i_3, kWaterChunk), x0_)), math_compare<"<=", f32>(kern::formula<i32, f32>([](auto j_3, auto kWaterChunk) { return (num_cast<f32, i32>(j_3) * kWaterChunk); }, j_3, kWaterChunk), y1_)), math_compare<">=", f32>(kern::formula<i32, f32>([](auto j_3, auto kWaterChunk) { return (num_cast<f32, i32>((j_3 + 1)) * kWaterChunk); }, j_3, kWaterChunk), y0_))) {
        key_3 = map_key_at<u64, u32>(self.water_meshes_, n52.index);
        set_add(n53, self.dirty_water_, key_3);
      }
    }
    if (notify) {
      list_add(self.changes_, engine_math_math::Vec4_new(x0_, y0_, x1_, y1_).Result);
    }
    self.replan_ = true;
  }

  struct Terrain_run_jobResult { bool Result = false; };
  Terrain_run_jobResult Terrain_run_job(Terrain& self, f64 deadline = 0.0) {
    u32 count = 0;
    DemOrigin origin = DemOrigin{0.0, 0.0};
    DemWindow win = DemWindow{0.0f, 0.0f, 10.0f, 0, 160.0f, 30.0f, 0};
    List<u8> in = List<u8>{};
    List<u8> made = List<u8>{};
    Fixed<u8, 16> bytes = Fixed<u8, 16>{};
    Fixed<u8, 28> bytes_2 = Fixed<u8, 28>{};
    bool skip = false;
    DemTileHead head = DemTileHead{0, 0, 0, 0};
    u64 at = 0;
    Fixed<u8, 16> bytes_3 = Fixed<u8, 16>{};
    u32 submit = 0;
    List<u8> out = List<u8>{};
    bool take = false;
    f64 now_ms = 0.0;
    f32 span = 0.0f;
    f32 x0 = 0.0f;
    f32 y0 = 0.0f;
    bool reload = false;
    Vec4 known = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    f32 lo = 0.0f;
    f32 hi = 0.0f;
    f32 v = 0.0f;
    f32 big = 0.0f;
    if (logic_not(num_cast<bool, u32>(self.job_[0].id))) {
      count = 0;
      for (auto it42 = flow_for(n42, 0, math_sub<i64>(map_length<u64, Terrain_DemTile>(self.dem_tiles_), 1)); it42.next();) {
        n43 = Terrain_run_job_mine(self, self.dem_tiles_[n42.index]);
        count += kern::formula<bool, i32, i32>([](auto the_result, auto a1, auto a0) { return num_cast<u32, i32>((the_result ? a1 : a0)); }, n43.Result, 1, 0);
      }
      origin = struct_make<DemOrigin>(self.proj_.lon0, self.proj_.lat0);
      win = struct_make<DemWindow>(self.job_[0].g.x0, self.job_[0].g.y0, self.job_[0].g.step, self.job_[0].g.n, kOpen, kBlur, count);
      made = List<u8>{};
      list_resize(made, 44, 0);
      in = made;
      bytes = struct_bytes<DemOrigin, "16: f64@0 f64@8">(origin);
      list_copy_bytes<"u8", "u8">(in, 0, bytes, 0, 16);
      bytes_2 = struct_bytes<DemWindow, "28: f32@0 f32@4 f32@8 u32@12 f32@16 f32@20 u32@24">(win);
      list_copy_bytes<"u8", "u8">(in, 16, bytes_2, 0, 28);
      for (auto it44 = flow_for(n44, 0, math_sub<i64>(map_length<u64, Terrain_DemTile>(self.dem_tiles_), 1)); it44.next();) {
        skip = false;
        n45 = Terrain_run_job_mine(self, self.dem_tiles_[n44.index]);
        if (logic_not(n45.Result)) {
          skip = true;
        }
        if (logic_not(skip)) {
          head = struct_make<DemTileHead>(num_cast<u32, i32>(self.dem_tiles_[n44.index].key.x), num_cast<u32, i32>(self.dem_tiles_[n44.index].key.y), self.dem_tiles_[n44.index].w, self.dem_tiles_[n44.index].hgt);
          at = num_cast<u64, i64>(list_length<u8>(in));
          list_resize(in, num_cast<i64, u64>(kern::formula<u64, u64>([](auto at, auto a) { return ((at + 16) + (a * 4)); }, at, num_cast<u64, i64>(list_length<f32>(self.dem_tiles_[n44.index].h)))), 0);
          bytes_3 = struct_bytes<DemTileHead, "16: u32@0 u32@4 u32@8 u32@12">(head);
          list_copy_bytes<"u8", "u8">(in, num_cast<i64, u64>(at), bytes_3, 0, 16);
          list_copy_bytes<"u8", "f32">(in, math_add<i64>(num_cast<i64, u64>(at), 16), self.dem_tiles_[n44.index].h, 0, num_cast<i64, u64>(kern::formula<u64>([](auto a) { return (a * 4); }, num_cast<u64, i64>(list_length<f32>(self.dem_tiles_[n44.index].h)))));
        }
      }
      engine_jobs_submit(n46, kDemJob, in);
      submit = n46.result;
      self.job_[0].id = submit;
      return {false};
    }
    out = List<u8>{};
    engine_jobs_take(n47, self.job_[0].id, out);
    take = n47.result;
    out = n47.out;
    if (logic_not(take)) {
      return {false};
    }
    self.job_[0].id = 0;
    if (math_compare<"!=", u64>(num_cast<u64, i64>(list_length<u8>(out)), kern::formula<u32>([](auto a) { return ((num_cast<u64, u32>(a) * num_cast<u64, u32>(a)) * 4); }, self.job_[0].g.n))) {
      self.job_ = List<Terrain_Job>{};
      now_ms = engine_now_ms();
      self.retry_at_ = kern::formula<f64>([](auto now_ms) { return (now_ms + 1500.0); }, now_ms);
      return {true};
    }
    list_copy_bytes<"f32", "u8">(self.job_[0].g.h, 0, out, 0, list_length<u8>(out));
    if (self.job_[0].side) {
      span = kern::formula<f32, u32>([](auto a, auto a_2) { return (a * num_cast<f32, u32>((a_2 - 1))); }, self.job_[0].g.step, self.job_[0].g.n);
      x0 = self.job_[0].g.x0;
      y0 = self.job_[0].g.y0;
      list_add(self.side_, self.job_[0].g);
      self.job_ = List<Terrain_Job>{};
      Terrain_dirty(self, x0, y0, kern::formula<f32, f32>([](auto x0, auto span) { return (x0 + span); }, x0, span), kern::formula<f32, f32>([](auto y0, auto span) { return (y0 + span); }, y0, span), true);
      return {true};
    }
    if (logic_not(self.ref_ready_)) {
      self.ref_height_ = Terrain_grid_at(self.job_[0].g, 0.0f, 0.0f).Result;
      self.ref_ready_ = true;
    }
    reload = self.grid_ready_;
    known = logic_select<Vec4>(reload, Terrain_interior(self.grid_).Result, engine_math_math::Vec4_new(0.0f, 0.0f, 0.0f, 0.0f).Result);
    var_take(n54, self.job_[0].g);
    self.job_[0].g.x0 = struct_break<Terrain_Grid>(n54.value).x0;
    self.job_[0].g.y0 = struct_break<Terrain_Grid>(n54.value).y0;
    self.job_[0].g.step = struct_break<Terrain_Grid>(n54.value).step;
    self.job_[0].g.n = struct_break<Terrain_Grid>(n54.value).n;
    self.job_[0].g.cx = struct_break<Terrain_Grid>(n54.value).cx;
    self.job_[0].g.cy = struct_break<Terrain_Grid>(n54.value).cy;
    self.grid_ = n54.value;
    self.grid_ready_ = true;
    self.settled_ = true;
    self.job_ = List<Terrain_Job>{};
    lo = 1e+09f;
    hi = -1e+09f;
    for (auto it55 = flow_for(n55, 0, math_sub<i64>(list_length<f32>(self.grid_.h), 1)); it55.next();) {
      v = self.grid_.h[n55.index];
      lo = kern::formula<f32, f32, f32>([](auto lo, auto v, auto ref_height_) { return fmin(lo, (v - ref_height_)); }, lo, v, self.ref_height_);
      hi = kern::formula<f32, f32, f32>([](auto hi, auto v, auto ref_height_) { return fmax(hi, (v - ref_height_)); }, hi, v, self.ref_height_);
    }
    self.stats_.min_height = lo;
    self.stats_.max_height = hi;
    self.stats_.windows = kern::formula<u32>([](auto windows) { return (windows + 1); }, self.stats_.windows);
    if (reload) {
      big = 1e+09f;
      Terrain_dirty(self, kern::formula<f32>([](auto big) { return (-big); }, big), kern::formula<f32>([](auto big) { return (-big); }, big), known.x, big, true);
      Terrain_dirty(self, known.z, kern::formula<f32>([](auto big) { return (-big); }, big), big, big, true);
      Terrain_dirty(self, known.x, kern::formula<f32>([](auto big) { return (-big); }, big), known.z, known.y, true);
      Terrain_dirty(self, known.x, known.w, known.z, big, true);
    }
    return {true};
  }

  struct road_keyResult { u64 Result = 0; };
  KERN_PURE road_keyResult road_key(i32 ix = 0, i32 iy = 0) {
    return {kern::formula<i32, i32>([](auto ix, auto iy) { return ((num_cast<u64, u32>(num_cast<u32, i32>(ix)) << 32) | num_cast<u64, u32>(num_cast<u32, i32>(iy))); }, ix, iy)};
  }

  struct Terrain_road_fieldResult { f32 Result = 0.0f; };
  KERN_PURE Terrain_road_fieldResult Terrain_road_field(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    f32 sum = 0.0f;
    f32 wsum = 0.0f;
    f32 w = 0.0f;
    sum = 0.0f;
    wsum = 0.0f;
    for (auto it56 = flow_for(n56, -2, 2); it56.next();) {
      for (auto it57 = flow_for(n57, -2, 2); it57.next();) {
        w = kern::formula<i64, i64>([](auto index, auto index_2) { return num_cast<f32, i32>(((3 - abs(num_cast<i32, i64>(index))) * (3 - abs(num_cast<i32, i64>(index_2))))); }, n57.index, n56.index);
        sum += kern::formula<f32, f32>([](auto the_result, auto w) { return (the_result * w); }, Terrain_elevation(self, kern::formula<f32, i64>([](auto x, auto index) { return (x + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * 9.0f)); }, x, n57.index), kern::formula<f32, i64>([](auto y, auto index) { return (y + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * 9.0f)); }, y, n56.index)).Result, w);
        wsum += w;
      }
    }
    return {kern::formula<f32, f32>([](auto sum, auto wsum) { return (sum / wsum); }, sum, wsum)};
  }

  void Terrain_remove_corridors(Terrain& self, u64 owner = 0) {
    i64 it = 0;
    bool skip = false;
    Vec4 box = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vec4 bounds = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vec4 c = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    it = 0;
    while (loop(logic_not(math_compare<"==", i64>(it, list_length<Terrain_CorridorSet_row>(self.corridors_))))) {
      skip = false;
      if (math_compare<"!=", u64>(self.corridors_[it].items[0].owner, owner)) {
        skip = true;
      }
      if (logic_not(skip)) {
        box = self.corridors_[it].items[0].box;
        bounds = self.corridors_[it].items[0].bounds;
        list_remove_at(self.corridors_, it);
        if (math_compare<">", f32>(box.z, box.x)) {
          c = engine_math_math::Vec4_new(kern::formula<f32, f32>([](auto x, auto x_2) { return fmax(x, x_2); }, box.x, bounds.x), kern::formula<f32, f32>([](auto y, auto y_2) { return fmax(y, y_2); }, box.y, bounds.y), kern::formula<f32, f32>([](auto z, auto z_2) { return fmin(z, z_2); }, box.z, bounds.z), kern::formula<f32, f32>([](auto w, auto w_2) { return fmin(w, w_2); }, box.w, bounds.w)).Result;
          if (logic_and(math_compare<">", f32>(c.z, c.x), math_compare<">", f32>(c.w, c.y))) {
            Terrain_dirty(self, c.x, c.y, c.z, c.w, false);
          }
        }
        return;
      }
      it += 1;
    }
  }

  struct set_corridors_nearResult { bool Result = false; };
  /// (only what can reach into the tile's own square)
  KERN_PURE set_corridors_nearResult set_corridors_near(const Vec4& bounds, f32 reach = 0.0f, Vector p = Vector{0.0, 0.0, 0.0}) {
    return {logic_and(logic_and(logic_and(math_compare<">", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto x, auto reach) { return ((x - reach) - 6.0f); }, bounds.x, reach)), math_compare<"<", f32>(vector_break(p).x, kern::formula<f32, f32>([](auto z, auto reach) { return ((z + reach) + 6.0f); }, bounds.z, reach))), math_compare<">", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto y, auto reach) { return ((y - reach) - 6.0f); }, bounds.y, reach))), math_compare<"<", f32>(vector_break(p).y, kern::formula<f32, f32>([](auto w, auto reach) { return ((w + reach) + 6.0f); }, bounds.w, reach)))};
  }

  void Terrain_set_corridors(Terrain& self, const Vec4& bounds, const List<MapFeatures_Road>& roads, const List<f32>& widths, u64 owner = 0) {
    List<Terrain_CorridorSet> set = List<Terrain_CorridorSet>{};
    List<Vector> pts = List<Vector>{};
    List<f32> hs = List<f32>{};
    bool skip = false;
    List<Vector> points = List<Vector>{};
    f32 width = 0.0f;
    u64 i = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    i32 k = 0;
    f32 half = 0.0f;
    f32 reach = 0.0f;
    u64 i_2 = 0;
    bool skip_2 = false;
    Vector a_2 = Vector{0.0, 0.0, 0.0};
    Vector b_2 = Vector{0.0, 0.0, 0.0};
    f32 dx = 0.0f;
    f32 dy = 0.0f;
    f32 l2 = 0.0f;
    Terrain_CorridorSeg seg = Terrain_CorridorSeg{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};
    f32 x0 = 0.0f;
    f32 x1 = 0.0f;
    f32 y0 = 0.0f;
    f32 y1 = 0.0f;
    List<Terrain_CorridorSeg> t = List<Terrain_CorridorSeg>{};
    u64 key = 0;
    List<Terrain_CorridorSeg> put = List<Terrain_CorridorSeg>{};
    Vec4 box = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Vec4 c = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    Terrain_remove_corridors(self, owner);
    set = list_make<Terrain_CorridorSet, "1">(Terrain_CorridorSet{0, Vec4{0.0f, 0.0f, 0.0f, 0.0f}, Vec4{0.0f, 0.0f, 0.0f, 0.0f}, Map<u64, List<Terrain_CorridorSeg>>{}});
    set[0].owner = owner;
    set[0].bounds = bounds;
    set[0].box = engine_math_math::Vec4_new(1e+30f, 1e+30f, -1e+30f, -1e+30f).Result;
    pts = List<Vector>{};
    hs = List<f32>{};
    for (auto it58 = flow_for(n58, 0, math_sub<i64>(list_length<MapFeatures_Road>(roads), 1)); it58.next();) {
      skip = false;
      points = roads[n58.index].points;
      width = widths[n58.index];
      if (logic_or(math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(points)), 2), logic_not(math_compare<">", f32>(width, 0.0f)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        list_clear(pts);
        i = 0;
        while (loop(math_compare<"<", u64>(kern::formula<u64>([](auto i) { return (i + 1); }, i), num_cast<u64, i64>(list_length<Vector>(points))))) {
          a = points[num_cast<i64, u64>(i)];
          b = points[num_cast<i64, u64>(kern::formula<u64>([](auto i) { return (i + 1); }, i))];
          k = kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return max(1, num_cast<i32, f32>(ceil((hypot((x - x_2), (y - y_2)) / 5.0f)))); }, vector_break(b).x, vector_break(a).x, vector_break(b).y, vector_break(a).y);
          for (auto it59 = flow_for(n59, 0, math_sub<i64>(num_cast<i64, i32>(k), 1)); it59.next();) {
            list_add(pts, vector_add(a, vector_scale(vector_sub(b, a), kern::formula<i64, i32>([](auto index, auto k) { return (num_cast<f32, i32>(num_cast<i32, i64>(index)) / num_cast<f32, i32>(k)); }, n59.index, k))));
          }
          i += 1;
        }
        list_add(pts, points[math_sub<i64>(list_length<Vector>(points), 1)]);
        list_resize(hs, list_length<Vector>(pts), 0.0f);
        half = kern::formula<f32>([](auto width) { return (width / 2.0f); }, width);
        reach = kern::formula<f32, f32>([](auto half, auto kRoadBlend) { return (half + kRoadBlend); }, half, kRoadBlend);
        for (auto it60 = flow_for(n60, 0, math_sub<i64>(list_length<Vector>(pts), 1)); it60.next();) {
          hs[n60.index] = kern::formula<bool, f32, f32>([](auto the_result, auto the_result_2, auto a0f) { return (the_result ? the_result_2 : a0f); }, set_corridors_near(bounds, reach, pts[n60.index]).Result, Terrain_road_field(self, vector_break(pts[n60.index]).x, vector_break(pts[n60.index]).y).Result, 0.0f);
        }
        i_2 = 0;
        while (loop(math_compare<"<", u64>(kern::formula<u64>([](auto i_2) { return (i_2 + 1); }, i_2), num_cast<u64, i64>(list_length<Vector>(pts))))) {
          skip_2 = false;
          a_2 = pts[num_cast<i64, u64>(i_2)];
          b_2 = pts[num_cast<i64, u64>(kern::formula<u64>([](auto i_2) { return (i_2 + 1); }, i_2))];
          if (logic_or(logic_not(set_corridors_near(bounds, reach, a_2).Result), logic_not(set_corridors_near(bounds, reach, b_2).Result))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            dx = kern::formula<f32, f32>([](auto x, auto x_2) { return (x - x_2); }, vector_break(b_2).x, vector_break(a_2).x);
            dy = kern::formula<f32, f32>([](auto y, auto y_2) { return (y - y_2); }, vector_break(b_2).y, vector_break(a_2).y);
            l2 = kern::formula<f32, f32>([](auto dx, auto dy) { return ((dx * dx) + (dy * dy)); }, dx, dy);
            if (math_compare<"<", f32>(l2, 0.0001f)) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              seg = struct_make<Terrain_CorridorSeg>(vector_break(a_2).x, vector_break(a_2).y, dx, dy, l2, hs[num_cast<i64, u64>(i_2)], hs[num_cast<i64, u64>(kern::formula<u64>([](auto i_2) { return (i_2 + 1); }, i_2))], half);
              x0 = kern::formula<f32, f32, f32>([](auto x, auto x_2, auto reach) { return (fmin(x, x_2) - reach); }, vector_break(a_2).x, vector_break(b_2).x, reach);
              x1 = kern::formula<f32, f32, f32>([](auto x, auto x_2, auto reach) { return (fmax(x, x_2) + reach); }, vector_break(a_2).x, vector_break(b_2).x, reach);
              y0 = kern::formula<f32, f32, f32>([](auto y, auto y_2, auto reach) { return (fmin(y, y_2) - reach); }, vector_break(a_2).y, vector_break(b_2).y, reach);
              y1 = kern::formula<f32, f32, f32>([](auto y, auto y_2, auto reach) { return (fmax(y, y_2) + reach); }, vector_break(a_2).y, vector_break(b_2).y, reach);
              set[0].box = engine_math_math::Vec4_new(kern::formula<f32, f32>([](auto a, auto x0) { return fmin(a, x0); }, set[0].box.x, x0), kern::formula<f32, f32>([](auto a, auto y0) { return fmin(a, y0); }, set[0].box.y, y0), kern::formula<f32, f32>([](auto a, auto x1) { return fmax(a, x1); }, set[0].box.z, x1), kern::formula<f32, f32>([](auto a, auto y1) { return fmax(a, y1); }, set[0].box.w, y1)).Result;
              for (auto it61 = flow_for(n61, num_cast<i64, i32>(kern::formula<f32, f32>([](auto x0, auto kRoadGrid) { return num_cast<i32, f32>(floor((x0 / kRoadGrid))); }, x0, kRoadGrid)), num_cast<i64, i32>(kern::formula<f32, f32>([](auto x1, auto kRoadGrid) { return num_cast<i32, f32>(floor((x1 / kRoadGrid))); }, x1, kRoadGrid))); it61.next();) {
                for (auto it62 = flow_for(n62, num_cast<i64, i32>(kern::formula<f32, f32>([](auto y0, auto kRoadGrid) { return num_cast<i32, f32>(floor((y0 / kRoadGrid))); }, y0, kRoadGrid)), num_cast<i64, i32>(kern::formula<f32, f32>([](auto y1, auto kRoadGrid) { return num_cast<i32, f32>(floor((y1 / kRoadGrid))); }, y1, kRoadGrid))); it62.next();) {
                  key = road_key(num_cast<i32, i64>(n61.index), num_cast<i32, i64>(n62.index)).Result;
                  map_add(n63, set[0].grid, key);
                  t = set[0].grid[n63.index];
                  list_add(t, seg);
                  put = t;
                  set[0].grid[n63.index] = put;
                }
              }
            }
          }
          i_2 += 1;
        }
      }
    }
    box = set[0].box;
    var_take(n64, set);
    list_add(self.corridors_, struct_make<Terrain_CorridorSet_row>(n64.value));
    if (math_compare<">", f32>(box.z, box.x)) {
      c = engine_math_math::Vec4_new(kern::formula<f32, f32>([](auto x, auto x_2) { return fmax(x, x_2); }, box.x, bounds.x), kern::formula<f32, f32>([](auto y, auto y_2) { return fmax(y, y_2); }, box.y, bounds.y), kern::formula<f32, f32>([](auto z, auto z_2) { return fmin(z, z_2); }, box.z, bounds.z), kern::formula<f32, f32>([](auto w, auto w_2) { return fmin(w, w_2); }, box.w, bounds.w)).Result;
      if (logic_and(math_compare<">", f32>(c.z, c.x), math_compare<">", f32>(c.w, c.y))) {
        Terrain_dirty(self, c.x, c.y, c.z, c.w, false);
      }
    }
  }

  struct Terrain_corridor_setResult { i32 Result = 0; };
  KERN_PURE Terrain_corridor_setResult Terrain_corridor_set(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    for (auto it65 = flow_for(n65, 0, math_sub<i64>(list_length<Terrain_CorridorSet_row>(self.corridors_), 1)); it65.next();) {
      if (logic_and(logic_and(logic_and(math_compare<">=", f32>(x, self.corridors_[n65.index].items[0].bounds.x), math_compare<"<", f32>(x, self.corridors_[n65.index].items[0].bounds.z)), math_compare<">=", f32>(y, self.corridors_[n65.index].items[0].bounds.y)), math_compare<"<", f32>(y, self.corridors_[n65.index].items[0].bounds.w))) {
        return {num_cast<i32, i64>(n65.index)};
      }
    }
    return {-1};
  }

  struct Terrain_corridors_atResult { bool Result = false; };
  KERN_PURE Terrain_corridors_atResult Terrain_corridors_at(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    return {logic_or(logic_not(self.corridors_expected), math_compare<">=", i32>(Terrain_corridor_set(self, x, y).Result, 0))};
  }

  struct Terrain_road_levelResult { f32 Result = 0.0f; };
  KERN_PURE Terrain_road_levelResult Terrain_road_level(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f, f32 h = 0.0f) {
    i32 at = 0;
    i64 it = 0;
    f32 best = 0.0f;
    f32 sw = 0.0f;
    f32 sh = 0.0f;
    i64 at_2 = 0;
    i64 at_3 = 0;
    bool skip = false;
    f32 t = 0.0f;
    f32 px = 0.0f;
    f32 py = 0.0f;
    f32 d = 0.0f;
    f32 w = 0.0f;
    f32 ws = 0.0f;
    at = kern::formula<bool, i32, i32>([](auto a, auto a_2, auto the_result) { return (a ? a_2 : the_result); }, math_compare<"==", i64>(list_length<Terrain_CorridorSet_row>(self.corridors_), 0), -1, Terrain_corridor_set(self, x, y).Result);
    if (math_compare<"<", i32>(at, 0)) {
      return {h};
    }
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, List<Terrain_CorridorSeg>>(self.corridors_[num_cast<i64, i32>(at)].items[0].grid, road_key(kern::formula<f32, f32>([](auto x, auto kRoadGrid) { return num_cast<i32, f32>(floor((x / kRoadGrid))); }, x, kRoadGrid), kern::formula<f32, f32>([](auto y, auto kRoadGrid) { return num_cast<i32, f32>(floor((y / kRoadGrid))); }, y, kRoadGrid)).Result).index, map_length<u64, List<Terrain_CorridorSeg>>(self.corridors_[num_cast<i64, i32>(at)].items[0].grid));
    if (math_compare<"==", i64>(it, map_length<u64, List<Terrain_CorridorSeg>>(self.corridors_[num_cast<i64, i32>(at)].items[0].grid))) {
      return {h};
    }
    best = 0.0f;
    sw = 0.0f;
    sh = 0.0f;
    at_2 = num_cast<i64, i32>(at);
    at_3 = it;
    for (auto it66 = flow_for(n66, 0, math_sub<i64>(list_length<Terrain_CorridorSeg>(self.corridors_[at_2].items[0].grid[at_3]), 1)); it66.next();) {
      skip = false;
      t = common::clampf(kern::formula<f32, f32, f32, f32, f32, f32, f32>([](auto x, auto a, auto a_2, auto y, auto a_3, auto a_4, auto a_5) { return ((((x - a) * a_2) + ((y - a_3) * a_4)) / a_5); }, x, self.corridors_[at_2].items[0].grid[at_3][n66.index].ax, self.corridors_[at_2].items[0].grid[at_3][n66.index].dx, y, self.corridors_[at_2].items[0].grid[at_3][n66.index].ay, self.corridors_[at_2].items[0].grid[at_3][n66.index].dy, self.corridors_[at_2].items[0].grid[at_3][n66.index].l2), 0.0f, 1.0f).Result;
      px = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto t) { return (a + (a_2 * t)); }, self.corridors_[at_2].items[0].grid[at_3][n66.index].ax, self.corridors_[at_2].items[0].grid[at_3][n66.index].dx, t);
      py = kern::formula<f32, f32, f32>([](auto a, auto a_2, auto t) { return (a + (a_2 * t)); }, self.corridors_[at_2].items[0].grid[at_3][n66.index].ay, self.corridors_[at_2].items[0].grid[at_3][n66.index].dy, t);
      d = kern::formula<f32, f32, f32, f32>([](auto x, auto px, auto y, auto py) { return hypot((x - px), (y - py)); }, x, px, y, py);
      w = kern::formula<f32, f32, f32, f32, f32>([](auto d, auto a, auto a0f, auto kRoadBlend, auto a0f_2) { return ((d <= a) ? a0f : ((d >= (a + kRoadBlend)) ? a0f_2 : (1.0f - ((d - a) / kRoadBlend)))); }, d, self.corridors_[at_2].items[0].grid[at_3][n66.index].half, 1.0f, kRoadBlend, 0.0f);
      if (math_compare<"<=", f32>(w, 0.0f)) {
        skip = true;
      }
      if (logic_not(skip)) {
        ws = kern::formula<f32>([](auto w) { return ((w * w) * (3.0f - (2.0f * w))); }, w);
        sw += ws;
        sh += kern::formula<f32, f32, f32, f32>([](auto ws, auto a, auto a_2, auto t) { return (ws * (a + ((a_2 - a) * t))); }, ws, self.corridors_[at_2].items[0].grid[at_3][n66.index].ha, self.corridors_[at_2].items[0].grid[at_3][n66.index].hb, t);
        best = kern::formula<f32, f32>([](auto best, auto w) { return fmax(best, w); }, best, w);
      }
    }
    if (math_compare<"<=", f32>(sw, 0.0f)) {
      return {h};
    }
    return {kern::formula<f32, f32, f32, f32>([](auto h, auto sh, auto sw, auto best) { return (h + (((sh / sw) - h) * ((best * best) * (3.0f - (2.0f * best))))); }, h, sh, sw, best)};
  }

  struct Terrain_water_cellResult { bool Result = false; };
  KERN_PURE Terrain_water_cellResult Terrain_water_cell(const Terrain& self, i32 cx = 0, i32 cy = 0) {
    i64 it = 0;
    u32 idx = 0;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, Terrain_WaterBlock>(self.water_, key2(kern::formula<i32>([](auto cx) { return (cx >> 4); }, cx), kern::formula<i32>([](auto cy) { return (cy >> 4); }, cy)).Result).index, map_length<u64, Terrain_WaterBlock>(self.water_));
    if (math_compare<"==", i64>(it, map_length<u64, Terrain_WaterBlock>(self.water_))) {
      return {false};
    }
    idx = kern::formula<i32, i32>([](auto cy, auto cx) { return num_cast<u32, i32>((((cy & 15) * 16) + (cx & 15))); }, cy, cx);
    return {kern::formula<u64, u32>([](auto a, auto idx) { return num_cast<bool, u64>(((a >> (idx & 63)) & 1)); }, self.water_[it].bits[num_cast<i64, u32>(kern::formula<u32>([](auto idx) { return (idx >> 6); }, idx))], idx)};
  }

  struct Terrain_on_bridgeResult { bool Result = false; };
  KERN_PURE Terrain_on_bridgeResult Terrain_on_bridge(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    bool skip = false;
    f32 dx = 0.0f;
    f32 dy = 0.0f;
    f32 c = 0.0f;
    f32 s = 0.0f;
    f32 u = 0.0f;
    f32 v = 0.0f;
    for (auto it68 = flow_for(n68, 0, math_sub<i64>(list_length<Terrain_Bridge>(self.bridges_), 1)); it68.next();) {
      skip = false;
      dx = kern::formula<f32, f32>([](auto x, auto x_2) { return (x - x_2); }, x, vector_break(self.bridges_[n68.index].c).x);
      dy = kern::formula<f32, f32>([](auto y, auto y_2) { return (y - y_2); }, y, vector_break(self.bridges_[n68.index].c).y);
      if (logic_or(math_compare<">", f32>(kern::formula<f32>([](auto dx) { return abs(dx); }, dx), kern::formula<f32, f32>([](auto a, auto a_2) { return (a + a_2); }, self.bridges_[n68.index].hl, self.bridges_[n68.index].hw)), math_compare<">", f32>(kern::formula<f32>([](auto dy) { return abs(dy); }, dy), kern::formula<f32, f32>([](auto a, auto a_2) { return (a + a_2); }, self.bridges_[n68.index].hl, self.bridges_[n68.index].hw)))) {
        skip = true;
      }
      if (logic_not(skip)) {
        c = kern::formula<f32>([](auto a) { return cos(a); }, self.bridges_[n68.index].yaw);
        s = kern::formula<f32>([](auto a) { return sin(a); }, self.bridges_[n68.index].yaw);
        u = kern::formula<f32, f32, f32, f32>([](auto dx, auto c, auto dy, auto s) { return ((dx * c) + (dy * s)); }, dx, c, dy, s);
        v = kern::formula<f32, f32, f32, f32>([](auto dx, auto s, auto dy, auto c) { return (((-dx) * s) + (dy * c)); }, dx, s, dy, c);
        if (logic_and(math_compare<"<=", f32>(kern::formula<f32>([](auto u) { return abs(u); }, u), self.bridges_[n68.index].hl), math_compare<"<=", f32>(kern::formula<f32>([](auto v) { return abs(v); }, v), self.bridges_[n68.index].hw))) {
          return {true};
        }
      }
    }
    return {false};
  }

  struct Terrain_water_atResult { bool Result = false; };
  KERN_PURE Terrain_water_atResult Terrain_water_at(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    if (math_compare<"==", i64>(map_length<u64, Terrain_WaterBlock>(self.water_), 0)) {
      return {false};
    }
    if (logic_not(Terrain_water_cell(self, kern::formula<f32, f32>([](auto x, auto kWaterCell) { return num_cast<i32, f32>(floor((x / kWaterCell))); }, x, kWaterCell), kern::formula<f32, f32>([](auto y, auto kWaterCell) { return num_cast<i32, f32>(floor((y / kWaterCell))); }, y, kWaterCell)).Result)) {
      return {false};
    }
    return {logic_or(math_compare<"==", i64>(list_length<Terrain_Bridge>(self.bridges_), 0), logic_not(Terrain_on_bridge(self, x, y).Result))};
  }

  struct Terrain_node_heightResult { f32 Result = 0.0f; };
  KERN_PURE Terrain_node_heightResult Terrain_node_height(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    f32 e = 0.0f;
    f32 h = 0.0f;
    bool skip = false;
    f32 dx = 0.0f;
    f32 dy = 0.0f;
    f32 r = 0.0f;
    e = Terrain_elevation(self, x, y).Result;
    h = Terrain_road_level(self, x, y, e).Result;
    for (auto it67 = flow_for(n67, 0, math_sub<i64>(list_length<Terrain_Crater>(self.craters_), 1)); it67.next();) {
      skip = false;
      dx = kern::formula<f32, f32>([](auto x, auto x_2) { return (x - x_2); }, x, vector_break(self.craters_[n67.index].c).x);
      dy = kern::formula<f32, f32>([](auto y, auto y_2) { return (y - y_2); }, y, vector_break(self.craters_[n67.index].c).y);
      if (logic_or(math_compare<">=", f32>(kern::formula<f32>([](auto dx) { return abs(dx); }, dx), self.craters_[n67.index].r), math_compare<">=", f32>(kern::formula<f32>([](auto dy) { return abs(dy); }, dy), self.craters_[n67.index].r))) {
        skip = true;
      }
      if (logic_not(skip)) {
        r = kern::formula<f32, f32>([](auto dx, auto dy) { return sqrt(((dx * dx) + (dy * dy))); }, dx, dy);
        if (math_compare<"<", f32>(r, self.craters_[n67.index].r)) {
          h += crater_profile(r, self.craters_[n67.index].r, self.craters_[n67.index].depth, self.craters_[n67.index].rim).Result;
        }
      }
    }
    if (logic_and(logic_not(math_compare<"==", i64>(map_length<u64, Terrain_WaterBlock>(self.water_), 0)), Terrain_water_at(self, x, y).Result)) {
      return {kern::formula<f32, f32, f32>([](auto h, auto e, auto kWaterBed) { return fmin(h, (e - kWaterBed)); }, h, e, kWaterBed)};
    }
    return {h};
  }

  struct Terrain_heightResult { f32 Result = 0.0f; };
  KERN_PURE Terrain_heightResult Terrain_height(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    f32 x0 = 0.0f;
    f32 y0 = 0.0f;
    f32 u = 0.0f;
    f32 v = 0.0f;
    f32 a = 0.0f;
    f32 d = 0.0f;
    x0 = kern::formula<f32, f32>([](auto x, auto kStep) { return (floor((x / kStep)) * kStep); }, x, kStep);
    y0 = kern::formula<f32, f32>([](auto y, auto kStep) { return (floor((y / kStep)) * kStep); }, y, kStep);
    u = kern::formula<f32, f32, f32>([](auto x, auto x0, auto kStep) { return ((x - x0) / kStep); }, x, x0, kStep);
    v = kern::formula<f32, f32, f32>([](auto y, auto y0, auto kStep) { return ((y - y0) / kStep); }, y, y0, kStep);
    a = Terrain_node_height(self, x0, y0).Result;
    d = Terrain_node_height(self, kern::formula<f32, f32>([](auto x0, auto kStep) { return (x0 + kStep); }, x0, kStep), kern::formula<f32, f32>([](auto y0, auto kStep) { return (y0 + kStep); }, y0, kStep)).Result;
    return {kern::formula<f32, f32, f32, f32, f32, f32>([](auto u, auto v, auto a, auto the_result, auto d, auto the_result_2) { return ((u >= v) ? ((a + ((the_result - a) * (u - v))) + ((d - a) * v)) : ((a + ((the_result_2 - a) * (v - u))) + ((d - a) * u))); }, u, v, a, Terrain_node_height(self, kern::formula<f32, f32>([](auto x0, auto kStep) { return (x0 + kStep); }, x0, kStep), y0).Result, d, Terrain_node_height(self, x0, kern::formula<f32, f32>([](auto y0, auto kStep) { return (y0 + kStep); }, y0, kStep)).Result)};
  }

  struct Terrain_normalResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Terrain_normalResult Terrain_normal(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    f32 e = 0.0f;
    f32 hx = 0.0f;
    f32 hy = 0.0f;
    Vector normalize = Vector{0.0, 0.0, 0.0};
    e = 0.5f;
    hx = kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return (the_result - the_result_2); }, Terrain_height(self, kern::formula<f32, f32>([](auto x, auto e) { return (x + e); }, x, e), y).Result, Terrain_height(self, kern::formula<f32, f32>([](auto x, auto e) { return (x - e); }, x, e), y).Result);
    hy = kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return (the_result - the_result_2); }, Terrain_height(self, x, kern::formula<f32, f32>([](auto y, auto e) { return (y + e); }, y, e)).Result, Terrain_height(self, x, kern::formula<f32, f32>([](auto y, auto e) { return (y - e); }, y, e)).Result);
    normalize = engine_normalize(vector_make(kern::formula<f32>([](auto hx) { return (-hx); }, hx), kern::formula<f32>([](auto hy) { return (-hy); }, hy), kern::formula<f32>([](auto e) { return (2.0f * e); }, e)), Vector{0.0, 0.0, 1.0});
    return {normalize};
  }

  struct Terrain_footprint_baseResult { f32 Result = 0.0f; };
  KERN_PURE Terrain_footprint_baseResult Terrain_footprint_base(const Terrain& self, const List<Vector>& ring) {
    f32 m = 0.0f;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector b = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    f32 ea = 0.0f;
    f32 ec = 0.0f;
    if (logic_and(logic_not(self.grid_ready_), math_compare<"==", i64>(list_length<Terrain_Crater>(self.craters_), 0))) {
      return {0.0f};
    }
    m = 1e+09f;
    for (auto it69 = flow_for(n69, 0, math_sub<i64>(list_length<Vector>(ring), 1)); it69.next();) {
      a = ring[n69.index];
      b = ring[num_cast<i64, u64>(kern::formula<i64, u64>([](auto index, auto a) { return ((num_cast<u64, i64>(index) + 1) % a); }, n69.index, num_cast<u64, i64>(list_length<Vector>(ring))))];
      c = vector_scale(vector_add(a, b), 0.5f);
      ea = Terrain_elevation(self, vector_break(a).x, vector_break(a).y).Result;
      ec = Terrain_elevation(self, vector_break(c).x, vector_break(c).y).Result;
      m = kern::formula<f32, f32, f32, f32, f32>([](auto m, auto ea, auto the_result, auto ec, auto the_result_2) { return fmin(m, fmin(fmin(ea, the_result), fmin(ec, the_result_2))); }, m, ea, Terrain_road_level(self, vector_break(a).x, vector_break(a).y, ea).Result, ec, Terrain_road_level(self, vector_break(c).x, vector_break(c).y, ec).Result);
    }
    return {kern::formula<f32, f32>([](auto m, auto a0f) { return ((m < 1e+08f) ? m : a0f); }, m, 0.0f)};
  }

  struct sampler_callbackResult { f64 Result = 0.0; };
  KERN_PURE sampler_callbackResult sampler_callback(const Terrain& u, f64 x = 0.0, f64 y = 0.0) {
    return {num_cast<f64, f32>(Terrain_height(u, num_cast<f32, f64>(x), num_cast<f32, f64>(y)).Result)};
  }

  struct Terrain_samplerResult { Callback<f64(f64, f64)> Result = Callback<f64(f64, f64)>{}; };
  KERN_PURE Terrain_samplerResult Terrain_sampler(const Terrain& self, f32 offset = 0.0f) {
    Callback<f64(f64, f64)> s = Callback<f64(f64, f64)>{};
    s = Callback<f64(f64, f64)>{};
    s = callback<&terrain_cpp::sampler_callback>(self);
    return {s};
  }

  void Terrain_set_water_cell(Terrain& self, i32 cx = 0, i32 cy = 0) {
    u64 key = 0;
    i64 b_row = 0;
    u32 idx = 0;
    key = key2(kern::formula<i32>([](auto cx) { return (cx >> 4); }, cx), kern::formula<i32>([](auto cy) { return (cy >> 4); }, cy)).Result;
    map_add(n70, self.water_, key);
    b_row = n70.index;
    idx = kern::formula<i32, i32>([](auto cy, auto cx) { return num_cast<u32, i32>((((cy & 15) * 16) + (cx & 15))); }, cy, cx);
    if (logic_not(kern::formula<u64, u32>([](auto a, auto idx) { return num_cast<bool, u64>(((a >> (idx & 63)) & 1)); }, self.water_[b_row].bits[num_cast<i64, u32>(kern::formula<u32>([](auto idx) { return (idx >> 6); }, idx))], idx))) {
      self.stats_.water_cells = kern::formula<u32>([](auto water_cells) { return (water_cells + 1); }, self.stats_.water_cells);
    }
    self.water_[b_row].bits[num_cast<i64, u32>(kern::formula<u32>([](auto idx) { return (idx >> 6); }, idx))] = kern::formula<u64, u64, u32>([](auto a, auto a1, auto idx) { return (a | (a1 << (idx & 63))); }, self.water_[b_row].bits[num_cast<i64, u32>(kern::formula<u32>([](auto idx) { return (idx >> 6); }, idx))], 1, idx);
  }

  struct Terrain_flow_atResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE Terrain_flow_atResult Terrain_flow_at(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    i32 cx = 0;
    i32 cy = 0;
    i64 it = 0;
    u32 idx = 0;
    cx = kern::formula<f32, f32>([](auto x, auto kWaterCell) { return num_cast<i32, f32>(floor((x / kWaterCell))); }, x, kWaterCell);
    cy = kern::formula<f32, f32>([](auto y, auto kWaterCell) { return num_cast<i32, f32>(floor((y / kWaterCell))); }, y, kWaterCell);
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, Terrain_WaterBlock>(self.water_, key2(kern::formula<i32>([](auto cx) { return (cx >> 4); }, cx), kern::formula<i32>([](auto cy) { return (cy >> 4); }, cy)).Result).index, map_length<u64, Terrain_WaterBlock>(self.water_));
    if (logic_or(logic_or(math_compare<"==", i64>(it, map_length<u64, Terrain_WaterBlock>(self.water_)), math_compare<"==", i64>(list_length<Vector>(self.water_[it].flow), 0)), logic_not(Terrain_water_at(self, x, y).Result))) {
      return {Vector{0.0, 0.0, 0.0}};
    }
    idx = kern::formula<i32, i32>([](auto cy, auto cx) { return num_cast<u32, i32>((((cy & 15) * 16) + (cx & 15))); }, cy, cx);
    return {vector_scale(self.water_[it].flow[num_cast<i64, u32>(idx)], self.water_[it].speed[num_cast<i64, u32>(idx)])};
  }

  struct Terrain_surface_atResult { i32 Result = 0; };
  KERN_PURE Terrain_surface_atResult Terrain_surface_at(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    if (Terrain_water_at(self, x, y).Result) {
      return {1};
    }
    for (auto it71 = flow_for(n71, 0, math_sub<i64>(list_length<Terrain_Crater>(self.craters_), 1)); it71.next();) {
      if (math_compare<"<", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, x, vector_break(self.craters_[n71.index].c).x, y, vector_break(self.craters_[n71.index].c).y), kern::formula<f32>([](auto a) { return (a * 0.8f); }, self.craters_[n71.index].r))) {
        return {2};
      }
    }
    return {0};
  }

  void Terrain_add_water_polygon(Terrain& self, const List<Vector_row>& rings) {
    f32 y_lo = 0.0f;
    f32 y_hi = 0.0f;
    f32 x_lo = 0.0f;
    f32 x_hi = 0.0f;
    Vector p = Vector{0.0, 0.0, 0.0};
    List<f32> xs = List<f32>{};
    i32 j0 = 0;
    i32 j1 = 0;
    f32 y = 0.0f;
    u64 a = 0;
    u64 c = 0;
    u64 was = 0;
    f32 ya = 0.0f;
    f32 yc = 0.0f;
    u64 k = 0;
    i32 ia = 0;
    i32 ib = 0;
    if (logic_or(math_compare<"==", i64>(list_length<Vector_row>(rings), 0), math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(rings[0].items)), 3))) {
      return;
    }
    y_lo = 1e+09f;
    y_hi = -1e+09f;
    x_lo = 1e+09f;
    x_hi = -1e+09f;
    for (auto it72 = flow_for(n72, 0, math_sub<i64>(list_length<Vector>(rings[0].items), 1)); it72.next();) {
      p = rings[0].items[n72.index];
      y_lo = kern::formula<f32, f32>([](auto y_lo, auto y) { return fmin(y_lo, y); }, y_lo, vector_break(p).y);
      y_hi = kern::formula<f32, f32>([](auto y_hi, auto y) { return fmax(y_hi, y); }, y_hi, vector_break(p).y);
      x_lo = kern::formula<f32, f32>([](auto x_lo, auto x) { return fmin(x_lo, x); }, x_lo, vector_break(p).x);
      x_hi = kern::formula<f32, f32>([](auto x_hi, auto x) { return fmax(x_hi, x); }, x_hi, vector_break(p).x);
    }
    xs = List<f32>{};
    j0 = kern::formula<f32, f32>([](auto y_lo, auto kWaterCell) { return num_cast<i32, f32>(floor((y_lo / kWaterCell))); }, y_lo, kWaterCell);
    j1 = kern::formula<f32, f32>([](auto y_hi, auto kWaterCell) { return num_cast<i32, f32>(floor((y_hi / kWaterCell))); }, y_hi, kWaterCell);
    for (auto it73 = flow_for(n73, num_cast<i64, i32>(j0), num_cast<i64, i32>(j1)); it73.next();) {
      y = kern::formula<i64, f32>([](auto index, auto kWaterCell) { return ((num_cast<f32, i32>(num_cast<i32, i64>(index)) + 0.5f) * kWaterCell); }, n73.index, kWaterCell);
      list_clear(xs);
      for (auto it74 = flow_for(n74, 0, math_sub<i64>(list_length<Vector_row>(rings), 1)); it74.next();) {
        a = 0;
        c = kern::formula<u64>([](auto a) { return (a - 1); }, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(rings[n74.index]).items)));
        while (loop(math_compare<"<", u64>(a, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(rings[n74.index]).items))))) {
          ya = vector_break(rings[n74.index].items[num_cast<i64, u64>(a)]).y;
          yc = vector_break(rings[n74.index].items[num_cast<i64, u64>(c)]).y;
          if (math_compare<"!=", i32>(num_cast<i32, bool>(math_compare<">", f32>(ya, y)), num_cast<i32, bool>(math_compare<">", f32>(yc, y)))) {
            list_add(xs, kern::formula<f32, f32, f32, f32, f32>([](auto x, auto y, auto ya, auto yc, auto x_2) { return (x + (((y - ya) / (yc - ya)) * (x_2 - x))); }, vector_break(rings[n74.index].items[num_cast<i64, u64>(a)]).x, y, ya, yc, vector_break(rings[n74.index].items[num_cast<i64, u64>(c)]).x));
          }
          was = a;
          a += 1;
          c = was;
        }
      }
      list_sort<nullptr>(xs);
      k = 0;
      while (loop(math_compare<"<", u64>(kern::formula<u64>([](auto k) { return (k + 1); }, k), num_cast<u64, i64>(list_length<f32>(xs))))) {
        ia = kern::formula<f32, f32>([](auto a, auto kWaterCell) { return num_cast<i32, f32>(ceil(((a / kWaterCell) - 0.5f))); }, xs[num_cast<i64, u64>(k)], kWaterCell);
        ib = kern::formula<f32, f32>([](auto a, auto kWaterCell) { return num_cast<i32, f32>(floor(((a / kWaterCell) - 0.5f))); }, xs[num_cast<i64, u64>(kern::formula<u64>([](auto k) { return (k + 1); }, k))], kWaterCell);
        for (auto it75 = flow_for(n75, num_cast<i64, i32>(ia), num_cast<i64, i32>(ib)); it75.next();) {
          Terrain_set_water_cell(self, num_cast<i32, i64>(n75.index), num_cast<i32, i64>(n73.index));
        }
        k += 2;
      }
    }
    Terrain_dirty(self, x_lo, y_lo, x_hi, y_hi, true);
  }

  void Terrain_add_waterway(Terrain& self, const List<Vector>& pts, f32 width = 0.0f, f32 speed = 0.0f) {
    f32 x_lo = 0.0f;
    f32 y_lo = 0.0f;
    f32 x_hi = 0.0f;
    f32 y_hi = 0.0f;
    u64 k = 0;
    bool skip = false;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    f32 l = 0.0f;
    Vector u = Vector{0.0, 0.0, 0.0};
    f32 half = 0.0f;
    f32 reach = 0.0f;
    i32 i0 = 0;
    i32 i1 = 0;
    i32 j0 = 0;
    i32 j1 = 0;
    bool skip_2 = false;
    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 t = 0.0f;
    f32 d = 0.0f;
    i64 it = 0;
    i64 count = 0;
    Vector fill = Vector{0.0, 0.0, 0.0};
    i64 count_2 = 0;
    f32 fill_2 = 0.0f;
    u32 idx = 0;
    f32 sp = 0.0f;
    x_lo = 1e+09f;
    y_lo = 1e+09f;
    x_hi = -1e+09f;
    y_hi = -1e+09f;
    k = 0;
    while (loop(math_compare<"<", u64>(kern::formula<u64>([](auto k) { return (k + 1); }, k), num_cast<u64, i64>(list_length<Vector>(pts))))) {
      skip = false;
      a = pts[num_cast<i64, u64>(k)];
      c = pts[num_cast<i64, u64>(kern::formula<u64>([](auto k) { return (k + 1); }, k))];
      l = kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(c).x, vector_break(a).x, vector_break(c).y, vector_break(a).y);
      if (math_compare<"<", f32>(l, 0.2f)) {
        skip = true;
      }
      if (logic_not(skip)) {
        u = vector_make(kern::formula<f32, f32, f32>([](auto x, auto x_2, auto l) { return ((x - x_2) / l); }, vector_break(c).x, vector_break(a).x, l), kern::formula<f32, f32, f32>([](auto y, auto y_2, auto l) { return ((y - y_2) / l); }, vector_break(c).y, vector_break(a).y, l), 0.0f);
        half = kern::formula<f32>([](auto width) { return (width / 2.0f); }, width);
        reach = kern::formula<f32>([](auto half) { return (half + 30.0f); }, half);
        i0 = kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto reach, auto kWaterCell) { return num_cast<i32, f32>(floor(((fmin(x, x_2) - reach) / kWaterCell))); }, vector_break(a).x, vector_break(c).x, reach, kWaterCell);
        i1 = kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto reach, auto kWaterCell) { return num_cast<i32, f32>(floor(((fmax(x, x_2) + reach) / kWaterCell))); }, vector_break(a).x, vector_break(c).x, reach, kWaterCell);
        j0 = kern::formula<f32, f32, f32, f32>([](auto y, auto y_2, auto reach, auto kWaterCell) { return num_cast<i32, f32>(floor(((fmin(y, y_2) - reach) / kWaterCell))); }, vector_break(a).y, vector_break(c).y, reach, kWaterCell);
        j1 = kern::formula<f32, f32, f32, f32>([](auto y, auto y_2, auto reach, auto kWaterCell) { return num_cast<i32, f32>(floor(((fmax(y, y_2) + reach) / kWaterCell))); }, vector_break(a).y, vector_break(c).y, reach, kWaterCell);
        x_lo = kern::formula<f32, f32, f32, f32>([](auto x_lo, auto x, auto x_2, auto half) { return fmin(x_lo, (fmin(x, x_2) - half)); }, x_lo, vector_break(a).x, vector_break(c).x, half);
        x_hi = kern::formula<f32, f32, f32, f32>([](auto x_hi, auto x, auto x_2, auto half) { return fmax(x_hi, (fmax(x, x_2) + half)); }, x_hi, vector_break(a).x, vector_break(c).x, half);
        y_lo = kern::formula<f32, f32, f32, f32>([](auto y_lo, auto y, auto y_2, auto half) { return fmin(y_lo, (fmin(y, y_2) - half)); }, y_lo, vector_break(a).y, vector_break(c).y, half);
        y_hi = kern::formula<f32, f32, f32, f32>([](auto y_hi, auto y, auto y_2, auto half) { return fmax(y_hi, (fmax(y, y_2) + half)); }, y_hi, vector_break(a).y, vector_break(c).y, half);
        for (auto it76 = flow_for(n76, num_cast<i64, i32>(i0), num_cast<i64, i32>(i1)); it76.next();) {
          for (auto it77 = flow_for(n77, num_cast<i64, i32>(j0), num_cast<i64, i32>(j1)); it77.next();) {
            skip_2 = false;
            x = kern::formula<i64, f32>([](auto index, auto kWaterCell) { return ((num_cast<f32, i32>(num_cast<i32, i64>(index)) + 0.5f) * kWaterCell); }, n76.index, kWaterCell);
            y = kern::formula<i64, f32>([](auto index, auto kWaterCell) { return ((num_cast<f32, i32>(num_cast<i32, i64>(index)) + 0.5f) * kWaterCell); }, n77.index, kWaterCell);
            t = common::clampf(kern::formula<f32, f32, f32, f32, f32, f32>([](auto x, auto x_2, auto x_3, auto y, auto y_2, auto y_3) { return (((x - x_2) * x_3) + ((y - y_2) * y_3)); }, x, vector_break(a).x, vector_break(u).x, y, vector_break(a).y, vector_break(u).y), 0.0f, l).Result;
            d = kern::formula<f32, f32, f32, f32, f32, f32, f32>([](auto x, auto x_2, auto x_3, auto t, auto y, auto y_2, auto y_3) { return hypot(((x - x_2) - (x_3 * t)), ((y - y_2) - (y_3 * t))); }, x, vector_break(a).x, vector_break(u).x, t, y, vector_break(a).y, vector_break(u).y);
            if (math_compare<"<=", f32>(d, half)) {
              Terrain_set_water_cell(self, num_cast<i32, i64>(n76.index), num_cast<i32, i64>(n77.index));
            }
            if (math_compare<"<", f32>(d, reach)) {
              it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, Terrain_WaterBlock>(self.water_, key2(kern::formula<i64>([](auto index) { return (num_cast<i32, i64>(index) >> 4); }, n76.index), kern::formula<i64>([](auto index) { return (num_cast<i32, i64>(index) >> 4); }, n77.index)).Result).index, map_length<u64, Terrain_WaterBlock>(self.water_));
              if (math_compare<"==", i64>(it, map_length<u64, Terrain_WaterBlock>(self.water_))) {
                skip_2 = true;
              }
              if (logic_not(skip_2)) {
                if (math_compare<"==", i64>(list_length<Vector>(self.water_[it].flow), 0)) {
                  count = 256;
                  fill = Vector{0.0, 0.0, 0.0};
                  list_clear(self.water_[it].flow);
                  list_resize(self.water_[it].flow, count, fill);
                  count_2 = 256;
                  fill_2 = 0.0f;
                  list_clear(self.water_[it].speed);
                  list_resize(self.water_[it].speed, count_2, fill_2);
                }
                idx = kern::formula<i64, i64>([](auto index, auto index_2) { return num_cast<u32, i32>((((num_cast<i32, i64>(index) & 15) * 16) + (num_cast<i32, i64>(index_2) & 15))); }, n77.index, n76.index);
                sp = kern::formula<f32, f32, f32>([](auto speed, auto d, auto reach) { return (speed * (1.0f - (d / reach))); }, speed, d, reach);
                if (math_compare<"<", f32>(self.water_[it].speed[num_cast<i64, u32>(idx)], sp)) {
                  self.water_[it].flow[num_cast<i64, u32>(idx)] = u;
                  self.water_[it].speed[num_cast<i64, u32>(idx)] = sp;
                }
              }
            }
          }
        }
      }
      k += 1;
    }
    if (math_compare<"<", f32>(x_lo, x_hi)) {
      Terrain_dirty(self, x_lo, y_lo, x_hi, y_hi, true);
    }
  }

  void Terrain_add_bridge(Terrain& self, Vector a = Vector{0.0, 0.0, 0.0}, Vector b = Vector{0.0, 0.0, 0.0}, f32 width = 0.0f) {
    f32 l = 0.0f;
    f32 r = 0.0f;
    l = kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(b).x, vector_break(a).x, vector_break(b).y, vector_break(a).y);
    if (math_compare<"<", f32>(l, 0.5f)) {
      return;
    }
    list_add(self.bridges_, struct_make<Terrain_Bridge>(vector_scale(vector_add(a, b), 0.5f), kern::formula<f32>([](auto l) { return ((l / 2.0f) + 1.0f); }, l), kern::formula<f32>([](auto width) { return ((width / 2.0f) + 1.5f); }, width), kern::formula<f32, f32, f32, f32>([](auto y, auto y_2, auto x, auto x_2) { return atan2((y - y_2), (x - x_2)); }, vector_break(b).y, vector_break(a).y, vector_break(b).x, vector_break(a).x)));
    r = kern::formula<f32, f32>([](auto l, auto width) { return ((l / 2.0f) + width); }, l, width);
    Terrain_dirty(self, kern::formula<f32, f32, f32>([](auto x, auto x_2, auto r) { return (((x + x_2) / 2.0f) - r); }, vector_break(a).x, vector_break(b).x, r), kern::formula<f32, f32, f32>([](auto y, auto y_2, auto r) { return (((y + y_2) / 2.0f) - r); }, vector_break(a).y, vector_break(b).y, r), kern::formula<f32, f32, f32>([](auto x, auto x_2, auto r) { return (((x + x_2) / 2.0f) + r); }, vector_break(a).x, vector_break(b).x, r), kern::formula<f32, f32, f32>([](auto y, auto y_2, auto r) { return (((y + y_2) / 2.0f) + r); }, vector_break(a).y, vector_break(b).y, r), true);
  }

  struct albedoResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  /// Satellite RGB -> albedo (world_ground.mjs albedo): lift shadows, remove haze, plausible ranges
  KERN_PURE albedoResult albedo(Vector c = Vector{0.0, 0.0, 0.0}) {
    f32 l = 0.0f;
    f32 target = 0.0f;
    f32 k = 0.0f;
    f32 R = 0.0f;
    f32 G = 0.0f;
    f32 B = 0.0f;
    f32 m = 0.0f;
    f32 sat = 0.0f;
    l = kern::formula<f32, f32, f32>([](auto x, auto y, auto z) { return (((0.2126f * x) + (0.7152f * y)) + (0.0722f * z)); }, vector_break(c).x, vector_break(c).y, vector_break(c).z);
    target = kern::formula<f32>([](auto l) { return fmin(0.24f, fmax(0.07f, ((l * 0.8f) + 0.03f))); }, l);
    k = kern::formula<f32, f32>([](auto target, auto l) { return (target / fmax(0.02f, l)); }, target, l);
    R = kern::formula<f32, f32>([](auto x, auto k) { return (x * k); }, vector_break(c).x, k);
    G = kern::formula<f32, f32>([](auto y, auto k) { return (y * k); }, vector_break(c).y, k);
    B = kern::formula<f32, f32>([](auto z, auto k) { return (z * k); }, vector_break(c).z, k);
    m = kern::formula<f32, f32, f32>([](auto r, auto g, auto b) { return (((r + g) + b) / 3.0f); }, R, G, B);
    sat = kern::formula<f32, f32, f32>([](auto r, auto g, auto b) { return (fmax(r, fmax(g, b)) - fmin(r, fmin(g, b))); }, R, G, B);
    if (math_compare<"<", f32>(sat, 0.05f)) {
      R = kern::formula<f32, f32>([](auto r, auto m) { return ((r * 0.85f) + (m * 0.15f)); }, R, m);
      G = kern::formula<f32, f32>([](auto g, auto m) { return ((g * 0.85f) + (m * 0.15f)); }, G, m);
      B = kern::formula<f32, f32>([](auto b, auto m) { return ((b * 0.85f) + (m * 0.15f)); }, B, m);
    }
    if (logic_and(logic_and(math_compare<">", f32>(B, G), math_compare<">", f32>(B, R)), math_compare<">", f32>(sat, 0.04f))) {
      B = kern::formula<f32>([](auto b) { return (b * 0.85f); }, B);
    }
    return {vector_make(kern::formula<f32>([](auto r) { return fmin(0.45f, r); }, R), kern::formula<f32>([](auto g) { return fmin(0.45f, g); }, G), kern::formula<f32>([](auto b) { return fmin(0.45f, b); }, B))};
  }

  struct srgb_lin8Result { f32 Result = 0.0f; };
  KERN_PURE srgb_lin8Result srgb_lin8(u8 v = 0) {
    f32 x = 0.0f;
    x = kern::formula<u8>([](auto v) { return (num_cast<f32, i32>(num_cast<i32, u8>(v)) / 255.0f); }, v);
    return {kern::formula<f32>([](auto x) { return ((x <= 0.04045f) ? (x / 12.92f) : pow(((x + 0.055f) / 1.055f), 2.4f)); }, x)};
  }

  struct Terrain_take_image_requestsResult { Text Result = ""; };
  Terrain_take_image_requestsResult Terrain_take_image_requests(Terrain& self) {
    Text s = "";
    Text b = "";
    s = "[";
    for (auto it78 = flow_for(n78, 0, math_sub<i64>(list_length<TileKey>(self.image_requests_), 1)); it78.next();) {
      b = "";
      b = text_format<"%s[%d,%d,%d]">(63, logic_select<Text>(num_cast<bool, u64>(num_cast<u64, i64>(n78.index)), ",", ""), num_cast<i64, i32>(self.image_requests_[n78.index].z), num_cast<i64, i32>(self.image_requests_[n78.index].x), num_cast<i64, i32>(self.image_requests_[n78.index].y));
      s = text_append(s, b, "");
    }
    list_clear(self.image_requests_);
    return {text_append(s, "]", "")};
  }

  struct Terrain_image_boundsResult { Vec4 Result = Vec4{0.0f, 0.0f, 0.0f, 0.0f}; };
  Terrain_image_boundsResult Terrain_image_bounds(const Terrain& self, const TileKey& k) {
    f64 lon0 = 0.0;
    f64 lat0 = 0.0;
    f64 lon1 = 0.0;
    f64 lat1 = 0.0;
    f64 ax = 0.0;
    f64 ay = 0.0;
    f64 bx = 0.0;
    f64 by = 0.0;
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
    engine_geo_projector_project(n79, self.proj_, lon0, lat0, ax, ay);
    ax = n79.x;
    ay = n79.y;
    engine_geo_projector_project(n80, self.proj_, lon1, lat1, bx, by);
    bx = n80.x;
    by = n80.y;
    return {engine_math_math::Vec4_new(kern::formula<f64, f64>([](auto ax, auto bx) { return num_cast<f32, f64>(fmin(ax, bx)); }, ax, bx), kern::formula<f64, f64>([](auto ay, auto by) { return num_cast<f32, f64>(fmin(ay, by)); }, ay, by), kern::formula<f64, f64>([](auto ax, auto bx) { return num_cast<f32, f64>(fmax(ax, bx)); }, ax, bx), kern::formula<f64, f64>([](auto ay, auto by) { return num_cast<f32, f64>(fmax(ay, by)); }, ay, by)).Result};
  }

  void Terrain_repaint(Terrain& self, const Vec4& b) {
    i32 level = 0;
    i32 i = 0;
    i32 j = 0;
    f32 size = 0.0f;
    u64 key = 0;
    for (auto it84 = flow_for(n84, 0, math_sub<i64>(map_length<u64, Terrain_RenderTile>(self.render_), 1)); it84.next();) {
      level = 0;
      i = 0;
      j = 0;
      unkey3(level, i, j, map_key_at<u64, Terrain_RenderTile>(self.render_, n84.index));
      size = kern::formula<i32, f32, f32>([](auto level, auto kBlock, auto kTile) { return ((level == 2) ? kBlock : kTile); }, level, kBlock, kTile);
      if (logic_and(logic_and(logic_and(math_compare<"<=", f32>(kern::formula<i32, f32>([](auto i, auto size) { return (num_cast<f32, i32>(i) * size); }, i, size), b.z), math_compare<">=", f32>(kern::formula<i32, f32>([](auto i, auto size) { return (num_cast<f32, i32>((i + 1)) * size); }, i, size), b.x)), math_compare<"<=", f32>(kern::formula<i32, f32>([](auto j, auto size) { return (num_cast<f32, i32>(j) * size); }, j, size), b.w)), math_compare<">=", f32>(kern::formula<i32, f32>([](auto j, auto size) { return (num_cast<f32, i32>((j + 1)) * size); }, j, size), b.y))) {
        key = map_key_at<u64, Terrain_RenderTile>(self.render_, n84.index);
        set_add(n85, self.dirty_render_, key);
      }
    }
  }

  void Terrain_plan_images(Terrain& self, Vector focus = Vector{0.0, 0.0, 0.0}) {
    f32 half = 0.0f;
    f64 lon_a = 0.0;
    f64 lat_a = 0.0;
    f64 lon_b = 0.0;
    f64 lat_b = 0.0;
    i32 ix0 = 0;
    i32 iy0 = 0;
    i32 ix1 = 0;
    i32 iy1 = 0;
    TileKey k = TileKey{14, 0, 0};
    u64 key = 0;
    i64 im_row = 0;
    i64 it = 0;
    Vec4 a = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    i64 at = 0;
    if (math_compare<"<", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(focus).x, vector_break(self.image_at_).x, vector_break(focus).y, vector_break(self.image_at_).y), 350.0f)) {
      return;
    }
    self.image_at_ = vector_make(kern::formula<f32>([](auto x) { return (round((x / 10.0f)) * 10.0f); }, vector_break(focus).x), kern::formula<f32>([](auto y) { return (round((y / 10.0f)) * 10.0f); }, vector_break(focus).y), 0.0f);
    half = 800.0f;
    lon_a = 0.0;
    lat_a = 0.0;
    lon_b = 0.0;
    lat_b = 0.0;
    tiles_cpp::unproject(self.proj_, lon_a, lat_a, kern::formula<f32, f32>([](auto x, auto half) { return num_cast<f64, f32>((x - half)); }, vector_break(self.image_at_).x, half), kern::formula<f32, f32>([](auto y, auto half) { return num_cast<f64, f32>((y + half)); }, vector_break(self.image_at_).y, half));
    tiles_cpp::unproject(self.proj_, lon_b, lat_b, kern::formula<f32, f32>([](auto x, auto half) { return num_cast<f64, f32>((x + half)); }, vector_break(self.image_at_).x, half), kern::formula<f32, f32>([](auto y, auto half) { return num_cast<f64, f32>((y - half)); }, vector_break(self.image_at_).y, half));
    ix0 = kern::formula<f64>([](auto the_result) { return num_cast<i32, f64>(floor(the_result)); }, merc_x(lon_a, kImageZoom).Result);
    iy0 = kern::formula<f64>([](auto the_result) { return num_cast<i32, f64>(floor(the_result)); }, merc_y(lat_a, kImageZoom).Result);
    ix1 = kern::formula<f64>([](auto the_result) { return num_cast<i32, f64>(floor(the_result)); }, merc_x(lon_b, kImageZoom).Result);
    iy1 = kern::formula<f64>([](auto the_result) { return num_cast<i32, f64>(floor(the_result)); }, merc_y(lat_b, kImageZoom).Result);
    if (math_compare<">", i32>(kern::formula<i32, i32, i32, i32>([](auto ix1, auto ix0, auto iy1, auto iy0) { return (((ix1 - ix0) + 1) * ((iy1 - iy0) + 1)); }, ix1, ix0, iy1, iy0), 25)) {
      return;
    }
    for (auto it81 = flow_for(n81, num_cast<i64, i32>(iy0), num_cast<i64, i32>(iy1)); it81.next();) {
      for (auto it82 = flow_for(n82, num_cast<i64, i32>(ix0), num_cast<i64, i32>(ix1)); it82.next();) {
        k = struct_make<TileKey>(kImageZoom, num_cast<i32, i64>(n82.index), num_cast<i32, i64>(n81.index));
        key = tiles::TileKey_hash(k).Result;
        map_add(n83, self.images_, key);
        im_row = n83.index;
        if (logic_not(self.images_[im_row].requested)) {
          self.images_[im_row].requested = true;
          self.images_[im_row].key = k;
          list_add(self.image_requests_, k);
        }
      }
    }
    it = 0;
    while (loop(logic_not(math_compare<"==", i64>(it, map_length<u64, Terrain_Image>(self.images_))))) {
      if (logic_or(logic_or(logic_or(math_compare<"<", i32>(self.images_[it].key.x, kern::formula<i32>([](auto ix0) { return (ix0 - 2); }, ix0)), math_compare<">", i32>(self.images_[it].key.x, kern::formula<i32>([](auto ix1) { return (ix1 + 2); }, ix1))), math_compare<"<", i32>(self.images_[it].key.y, kern::formula<i32>([](auto iy0) { return (iy0 - 2); }, iy0))), math_compare<">", i32>(self.images_[it].key.y, kern::formula<i32>([](auto iy1) { return (iy1 + 2); }, iy1)))) {
        if (logic_not(math_compare<"==", i64>(list_length<f32>(self.images_[it].albedo), 0))) {
          n86 = Terrain_image_bounds(self, self.images_[it].key);
          a = n86.Result;
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

  void Terrain_provide_image(Terrain& self, const TileKey& key, List<u8> rgba = List<u8>{}, u32 width = 0, u32 height = 0) {
    i64 it = 0;
    u32 P = 0;
    List<u8> small = List<u8>{};
    List<u8> made = List<u8>{};
    u32 x0 = 0;
    u32 x1 = 0;
    u32 y0 = 0;
    u32 y1 = 0;
    Fixed<u32, 4> sum = Fixed<u32, 4>{};
    u32 n = 0;
    u64 at = 0;
    i64 count = 0;
    f32 fill = 0.0f;
    Vector s = Vector{0.0, 0.0, 0.0};
    u32 n_2 = 0;
    bool skip = false;
    u32 qx = 0;
    u32 qy = 0;
    u64 at_2 = 0;
    Vector a = Vector{0.0, 0.0, 0.0};
    Vec4 a_2 = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, Terrain_Image>(self.images_, tiles::TileKey_hash(key).Result).index, map_length<u64, Terrain_Image>(self.images_));
    if (logic_or(logic_or(math_compare<"==", i64>(it, map_length<u64, Terrain_Image>(self.images_)), math_compare<"==", u32>(width, 0)), math_compare<"==", u32>(height, 0))) {
      return;
    }
    P = num_cast<u32, i32>(kImagePixels);
    made = List<u8>{};
    list_resize(made, num_cast<i64, u64>(kern::formula<u32>([](auto p) { return num_cast<u64, u32>(((p * p) * 4)); }, P)), 0);
    small = made;
    for (auto it87 = flow_for(n87, 0, math_sub<i64>(num_cast<i64, u32>(P), 1)); it87.next();) {
      for (auto it88 = flow_for(n88, 0, math_sub<i64>(num_cast<i64, u32>(P), 1)); it88.next();) {
        x0 = kern::formula<i64, u32, u32>([](auto index, auto width, auto p) { return ((num_cast<u32, i64>(index) * width) / p); }, n88.index, width, P);
        x1 = kern::formula<u32, i64, u32, u32>([](auto x0, auto index, auto width, auto p) { return max((x0 + 1), (((num_cast<u32, i64>(index) + 1) * width) / p)); }, x0, n88.index, width, P);
        y0 = kern::formula<i64, u32, u32>([](auto index, auto height, auto p) { return ((num_cast<u32, i64>(index) * height) / p); }, n87.index, height, P);
        y1 = kern::formula<u32, i64, u32, u32>([](auto y0, auto index, auto height, auto p) { return max((y0 + 1), (((num_cast<u32, i64>(index) + 1) * height) / p)); }, y0, n87.index, height, P);
        sum = List<u32>{};
        n = 0;
        for (auto it89 = flow_for(n89, num_cast<i64, u32>(y0), math_sub<i64>(num_cast<i64, u32>(y1), 1)); it89.next();) {
          for (auto it90 = flow_for(n90, num_cast<i64, u32>(x0), math_sub<i64>(num_cast<i64, u32>(x1), 1)); it90.next();) {
            at = kern::formula<i64, u32, i64>([](auto index, auto width, auto index_2) { return (((num_cast<u64, u32>(num_cast<u32, i64>(index)) * num_cast<u64, u32>(width)) + num_cast<u64, u32>(num_cast<u32, i64>(index_2))) * 4); }, n89.index, width, n90.index);
            for (auto it91 = flow_for(n91, 0, 3); it91.next();) {
              sum[num_cast<i64, i32>(num_cast<i32, i64>(n91.index))] = kern::formula<u32, u8>([](auto a, auto a_2) { return (a + num_cast<u32, u8>(a_2)); }, sum[num_cast<i64, i32>(num_cast<i32, i64>(n91.index))], rgba[num_cast<i64, u64>(kern::formula<u64, i64>([](auto at, auto index) { return (at + num_cast<u64, i32>(num_cast<i32, i64>(index))); }, at, n91.index))]);
            }
            n += 1;
          }
        }
        for (auto it92 = flow_for(n92, 0, 3); it92.next();) {
          small[num_cast<i64, u64>(kern::formula<i64, u32, i64, i64>([](auto index, auto p, auto index_2, auto index_3) { return num_cast<u64, u32>(((((num_cast<u32, i64>(index) * p) + num_cast<u32, i64>(index_2)) * 4) + num_cast<u32, i32>(num_cast<i32, i64>(index_3)))); }, n87.index, P, n88.index, n92.index))] = kern::formula<u32, u32>([](auto a, auto n) { return num_cast<u8, u32>((a / max(1, n))); }, sum[num_cast<i64, i32>(num_cast<i32, i64>(n92.index))], n);
        }
      }
    }
    count = num_cast<i64, u64>(kern::formula<u32>([](auto p) { return num_cast<u64, u32>(((p * p) * 3)); }, P));
    fill = 0.0f;
    list_clear(self.images_[it].albedo);
    list_resize(self.images_[it].albedo, count, fill);
    for (auto it93 = flow_for(n93, 0, math_sub<i64>(num_cast<i64, u32>(P), 1)); it93.next();) {
      for (auto it94 = flow_for(n94, 0, math_sub<i64>(num_cast<i64, u32>(P), 1)); it94.next();) {
        s = Vector{0.0, 0.0, 0.0};
        n_2 = 0;
        for (auto it95 = flow_for(n95, -1, 1); it95.next();) {
          for (auto it96 = flow_for(n96, -1, 1); it96.next();) {
            skip = false;
            qx = kern::formula<i32, i32>([](auto a, auto a_2) { return num_cast<u32, i32>((a < 0 ? 0 : (a_2 < a ? a_2 : a))); }, kern::formula<i64, i64>([](auto index, auto index_2) { return (num_cast<i32, u32>(num_cast<u32, i64>(index)) + num_cast<i32, i64>(index_2)); }, n94.index, n96.index), kern::formula<u32>([](auto p) { return num_cast<i32, u32>((p - 1)); }, P));
            qy = kern::formula<i32, i32>([](auto a, auto a_2) { return num_cast<u32, i32>((a < 0 ? 0 : (a_2 < a ? a_2 : a))); }, kern::formula<i64, i64>([](auto index, auto index_2) { return (num_cast<i32, u32>(num_cast<u32, i64>(index)) + num_cast<i32, i64>(index_2)); }, n93.index, n95.index), kern::formula<u32>([](auto p) { return num_cast<i32, u32>((p - 1)); }, P));
            at_2 = kern::formula<u32, u32, u32>([](auto qy, auto p, auto qx) { return num_cast<u64, u32>((((qy * p) + qx) * 4)); }, qy, P, qx);
            if (math_compare<"<", i32>(num_cast<i32, u8>(small[num_cast<i64, u64>(kern::formula<u64>([](auto at_2) { return (at_2 + 3); }, at_2))]), 10)) {
              skip = true;
            }
            if (logic_not(skip)) {
              s = vector_add(s, vector_make(srgb_lin8(small[num_cast<i64, u64>(at_2)]).Result, srgb_lin8(small[num_cast<i64, u64>(kern::formula<u64>([](auto at_2) { return (at_2 + 1); }, at_2))]).Result, srgb_lin8(small[num_cast<i64, u64>(kern::formula<u64>([](auto at_2) { return (at_2 + 2); }, at_2))]).Result));
              n_2 += 1;
            }
          }
        }
        a = albedo(logic_select<Vector>(num_cast<bool, u32>(n_2), vector_scale(s, kern::formula<u32>([](auto n_2) { return (1.0f / num_cast<f32, u32>(n_2)); }, n_2)), Vector{0.12, 0.16, 0.08})).Result;
        self.images_[it].albedo[num_cast<i64, u64>(kern::formula<i64, u32, i64>([](auto index, auto p, auto index_2) { return num_cast<u64, u32>((((num_cast<u32, i64>(index) * p) + num_cast<u32, i64>(index_2)) * 3)); }, n93.index, P, n94.index))] = vector_break(a).x;
        self.images_[it].albedo[num_cast<i64, u64>(kern::formula<i64, u32, i64>([](auto index, auto p, auto index_2) { return num_cast<u64, u32>(((((num_cast<u32, i64>(index) * p) + num_cast<u32, i64>(index_2)) * 3) + 1)); }, n93.index, P, n94.index))] = vector_break(a).y;
        self.images_[it].albedo[num_cast<i64, u64>(kern::formula<i64, u32, i64>([](auto index, auto p, auto index_2) { return num_cast<u64, u32>(((((num_cast<u32, i64>(index) * p) + num_cast<u32, i64>(index_2)) * 3) + 2)); }, n93.index, P, n94.index))] = vector_break(a).z;
      }
    }
    n97 = Terrain_image_bounds(self, key);
    a_2 = n97.Result;
    Terrain_repaint(self, a_2);
  }

  void Terrain_fail_image(Terrain& self, const TileKey& key) {
    i64 it = 0;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, Terrain_Image>(self.images_, tiles::TileKey_hash(key).Result).index, map_length<u64, Terrain_Image>(self.images_));
    if (logic_not(math_compare<"==", i64>(it, map_length<u64, Terrain_Image>(self.images_)))) {
      self.images_[it].failed = true;
    }
  }

  struct Terrain_ground_albedoResult { bool Result = false; };
  Terrain_ground_albedoResult Terrain_ground_albedo(const Terrain& self, Vector& lin, f32 x = 0.0f, f32 y = 0.0f) {
    f64 lon = 0.0;
    f64 lat = 0.0;
    f64 fx = 0.0;
    f64 fy = 0.0;
    i32 tx = 0;
    i32 ty = 0;
    i64 it = 0;
    TileKey a = TileKey{14, 0, 0};
    i32 px = 0;
    i32 py = 0;
    List<f32> texels = List<f32>{};
    u64 at = 0;
    if (math_compare<"==", i64>(map_length<u64, Terrain_Image>(self.images_), 0)) {
      return {false};
    }
    lon = 0.0;
    lat = 0.0;
    tiles_cpp::unproject(self.proj_, lon, lat, num_cast<f64, f32>(x), num_cast<f64, f32>(y));
    fx = merc_x(lon, kImageZoom).Result;
    fy = merc_y(lat, kImageZoom).Result;
    tx = kern::formula<f64>([](auto fx) { return num_cast<i32, f64>(floor(fx)); }, fx);
    ty = kern::formula<f64>([](auto fy) { return num_cast<i32, f64>(floor(fy)); }, fy);
    a = struct_make<TileKey>(kImageZoom, tx, ty);
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, Terrain_Image>(self.images_, tiles::TileKey_hash(a).Result).index, map_length<u64, Terrain_Image>(self.images_));
    if (logic_or(math_compare<"==", i64>(it, map_length<u64, Terrain_Image>(self.images_)), math_compare<"==", i64>(list_length<f32>(self.images_[it].albedo), 0))) {
      return {false};
    }
    px = kern::formula<i32, i32>([](auto a, auto a_2) { return (a < 0 ? 0 : (a_2 < a ? a_2 : a)); }, kern::formula<f64, i32, i32>([](auto fx, auto tx, auto kImagePixels) { return num_cast<i32, f64>(((fx - num_cast<f64, i32>(tx)) * num_cast<f64, i32>(kImagePixels))); }, fx, tx, kImagePixels), kern::formula<i32>([](auto kImagePixels) { return (kImagePixels - 1); }, kImagePixels));
    py = kern::formula<i32, i32>([](auto a, auto a_2) { return (a < 0 ? 0 : (a_2 < a ? a_2 : a)); }, kern::formula<f64, i32, i32>([](auto fy, auto ty, auto kImagePixels) { return num_cast<i32, f64>(((fy - num_cast<f64, i32>(ty)) * num_cast<f64, i32>(kImagePixels))); }, fy, ty, kImagePixels), kern::formula<i32>([](auto kImagePixels) { return (kImagePixels - 1); }, kImagePixels));
    texels = self.images_[it].albedo;
    at = kern::formula<i32, i32, i32>([](auto py, auto kImagePixels, auto px) { return (((num_cast<u64, i32>(py) * num_cast<u64, i32>(kImagePixels)) + num_cast<u64, i32>(px)) * 3); }, py, kImagePixels, px);
    lin = vector_make(texels[num_cast<i64, u64>(at)], texels[num_cast<i64, u64>(kern::formula<u64>([](auto at) { return (at + 1); }, at))], texels[num_cast<i64, u64>(kern::formula<u64>([](auto at) { return (at + 2); }, at))]);
    return {true};
  }

  struct srgb_to_linearResult { f32 Result = 0.0f; };
  KERN_PURE srgb_to_linearResult srgb_to_linear(f32 v = 0.0f) {
    return {kern::formula<f32>([](auto v) { return ((v <= 0.04045f) ? (v / 12.92f) : pow(((v + 0.055f) / 1.055f), 2.4f)); }, v)};
  }

  struct linear_to_srgbResult { f32 Result = 0.0f; };
  KERN_PURE linear_to_srgbResult linear_to_srgb(f32 v = 0.0f) {
    return {kern::formula<f32>([](auto v) { return ((v <= 0.0031308f) ? (v * 12.92f) : ((1.055f * pow(v, 0.41666666f)) - 0.055f)); }, v)};
  }

  struct to_linearResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE to_linearResult to_linear(Vector c = Vector{0.0, 0.0, 0.0}) {
    return {vector_make(srgb_to_linear(vector_break(c).x).Result, srgb_to_linear(vector_break(c).y).Result, srgb_to_linear(vector_break(c).z).Result)};
  }

  struct to_srgbResult { Vector Result = Vector{0.0, 0.0, 0.0}; };
  KERN_PURE to_srgbResult to_srgb(Vector c = Vector{0.0, 0.0, 0.0}) {
    return {vector_make(linear_to_srgb(vector_break(c).x).Result, linear_to_srgb(vector_break(c).y).Result, linear_to_srgb(vector_break(c).z).Result)};
  }

  void Terrain_add_tint(Terrain& self, const List<Vector_row>& rings, u64 owner = 0, Vector srgb = Vector{0.0, 0.0, 0.0}) {
    Terrain_Tint t = Terrain_Tint{0, Vec4{0.0f, 0.0f, 0.0f, 0.0f}, Vector{0.0, 0.0, 0.0}, List<Vector_row>{}};
    Vector p = Vector{0.0, 0.0, 0.0};
    if (logic_or(math_compare<"==", i64>(list_length<Vector_row>(rings), 0), math_compare<"<", u64>(num_cast<u64, i64>(list_length<Vector>(rings[0].items)), 3))) {
      return;
    }
    t = struct_make<Terrain_Tint>(owner, engine_math_math::Vec4_new(1e+30f, 1e+30f, -1e+30f, -1e+30f).Result, to_linear(srgb).Result, rings);
    for (auto it98 = flow_for(n98, 0, math_sub<i64>(list_length<Vector>(rings[0].items), 1)); it98.next();) {
      p = rings[0].items[n98.index];
      t.box.x = kern::formula<f32, f32>([](auto x, auto x_2) { return fmin(x, x_2); }, t.box.x, vector_break(p).x);
      t.box.y = kern::formula<f32, f32>([](auto y, auto y_2) { return fmin(y, y_2); }, t.box.y, vector_break(p).y);
      t.box.z = kern::formula<f32, f32>([](auto z, auto x) { return fmax(z, x); }, t.box.z, vector_break(p).x);
      t.box.w = kern::formula<f32, f32>([](auto w, auto y) { return fmax(w, y); }, t.box.w, vector_break(p).y);
    }
    Terrain_repaint(self, t.box);
    list_add(self.tints_, t);
  }

  void Terrain_remove_tints(Terrain& self, u64 owner = 0) {
    i64 it = 0;
    bool skip = false;
    i64 at = 0;
    it = 0;
    while (loop(logic_not(math_compare<"==", i64>(it, list_length<Terrain_Tint>(self.tints_))))) {
      skip = false;
      if (math_compare<"!=", u64>(self.tints_[it].owner, owner)) {
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

  void Terrain_add_crater(Terrain& self, Vector at = Vector{0.0, 0.0, 0.0}, f32 radius = 0.0f, f32 depth = 0.0f) {
    Terrain_Crater old = Terrain_Crater{Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f, 0.0f};
    if (logic_not(math_compare<">", f32>(radius, 0.5f))) {
      return;
    }
    list_add(self.craters_, struct_make<Terrain_Crater>(at, radius, depth, kern::formula<f32>([](auto depth) { return ((depth * 7.0f) / 22.0f); }, depth)));
    while (loop(math_compare<">", u64>(num_cast<u64, i64>(list_length<Terrain_Crater>(self.craters_)), 32))) {
      old = self.craters_[0];
      list_remove_at(self.craters_, 0);
      Terrain_dirty(self, kern::formula<f32, f32>([](auto x, auto r) { return (x - r); }, vector_break(old.c).x, old.r), kern::formula<f32, f32>([](auto y, auto r) { return (y - r); }, vector_break(old.c).y, old.r), kern::formula<f32, f32>([](auto x, auto r) { return (x + r); }, vector_break(old.c).x, old.r), kern::formula<f32, f32>([](auto y, auto r) { return (y + r); }, vector_break(old.c).y, old.r), true);
    }
    self.stats_.craters = num_cast<u32, i64>(list_length<Terrain_Crater>(self.craters_));
    Terrain_dirty(self, kern::formula<f32, f32>([](auto x, auto radius) { return (x - radius); }, vector_break(at).x, radius), kern::formula<f32, f32>([](auto y, auto radius) { return (y - radius); }, vector_break(at).y, radius), kern::formula<f32, f32>([](auto x, auto radius) { return (x + radius); }, vector_break(at).x, radius), kern::formula<f32, f32>([](auto y, auto radius) { return (y + radius); }, vector_break(at).y, radius), true);
  }

  void the_Terrain_crater_list_(const Terrain& self, List<Terrain_CraterRec>& out) {
    list_clear(out);
    for (auto it99 = flow_for(n99, 0, math_sub<i64>(list_length<Terrain_Crater>(self.craters_), 1)); it99.next();) {
      list_add(out, struct_make<Terrain_CraterRec>(vector_break(self.craters_[n99.index].c).x, vector_break(self.craters_[n99.index].c).y, self.craters_[n99.index].r, self.craters_[n99.index].depth));
    }
  }

  struct set_craters_hasResult { bool Result = false; };
  KERN_PURE set_craters_hasResult set_craters_has(const List<Terrain_CraterRec>& v, const Terrain_CraterRec& c) {
    i64 at = 0;
    i64 end = 0;
    bool go = false;
    at = 0;
    end = list_length<Terrain_CraterRec>(v);
    go = math_compare<"<", i64>(at, end);
    while (loop(go)) {
      if (terrain::CraterRec_equal(v[at], c).Result) {
        go = false;
      } else {
        at += 1;
        go = math_compare<"<", i64>(at, end);
      }
    }
    return {logic_not(math_compare<"==", i64>(at, list_length<Terrain_CraterRec>(v)))};
  }

  void Terrain_set_craters(Terrain& self, const List<Terrain_CraterRec>& list) {
    List<Terrain_CraterRec> mine = List<Terrain_CraterRec>{};
    mine = List<Terrain_CraterRec>{};
    the_Terrain_crater_list_(self, mine);
    if (list_equal<Terrain_CraterRec>(mine, list)) {
      return;
    }
    for (auto it100 = flow_for(n100, 0, math_sub<i64>(list_length<Terrain_CraterRec>(mine), 1)); it100.next();) {
      if (logic_not(set_craters_has(list, mine[n100.index]).Result)) {
        Terrain_dirty(self, kern::formula<f32, f32>([](auto a, auto a_2) { return (a - a_2); }, mine[n100.index].x, mine[n100.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a - a_2); }, mine[n100.index].y, mine[n100.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a + a_2); }, mine[n100.index].x, mine[n100.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a + a_2); }, mine[n100.index].y, mine[n100.index].r), true);
      }
    }
    for (auto it101 = flow_for(n101, 0, math_sub<i64>(list_length<Terrain_CraterRec>(list), 1)); it101.next();) {
      if (logic_not(set_craters_has(mine, list[n101.index]).Result)) {
        Terrain_dirty(self, kern::formula<f32, f32>([](auto a, auto a_2) { return (a - a_2); }, list[n101.index].x, list[n101.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a - a_2); }, list[n101.index].y, list[n101.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a + a_2); }, list[n101.index].x, list[n101.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a + a_2); }, list[n101.index].y, list[n101.index].r), true);
      }
    }
    list_clear(self.craters_);
    for (auto it102 = flow_for(n102, 0, math_sub<i64>(list_length<Terrain_CraterRec>(list), 1)); it102.next();) {
      if (logic_and(logic_and(logic_and(math_compare<">", f32>(list[n102.index].r, 0.5f), kern::formula<f32>([](auto a) { return isfinite(a); }, list[n102.index].x)), kern::formula<f32>([](auto a) { return isfinite(a); }, list[n102.index].y)), kern::formula<f32>([](auto a) { return isfinite(a); }, list[n102.index].depth))) {
        list_add(self.craters_, struct_make<Terrain_Crater>(vector_make(list[n102.index].x, list[n102.index].y, 0.0f), list[n102.index].r, list[n102.index].depth, kern::formula<f32>([](auto a) { return ((a * 7.0f) / 22.0f); }, list[n102.index].depth)));
      }
    }
    self.stats_.craters = num_cast<u32, i64>(list_length<Terrain_Crater>(self.craters_));
  }

  struct Terrain_has_collisionResult { bool Result = false; };
  KERN_PURE Terrain_has_collisionResult Terrain_has_collision(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    u64 k = 0;
    k = key2(kern::formula<f32, f32>([](auto x, auto kTile) { return num_cast<i32, f32>(floor((x / kTile))); }, x, kTile), kern::formula<f32, f32>([](auto y, auto kTile) { return num_cast<i32, f32>(floor((y / kTile))); }, y, kTile)).Result;
    return {logic_and(num_cast<bool, i64>(num_cast<i64, bool>(map_contains<u64, Terrain_CollisionTile>(self.collision_, k))), logic_not(num_cast<bool, i64>(num_cast<i64, bool>(set_contains<u64>(self.dirty_collision_, k)))))};
  }

  struct Terrain_ground_readyResult { bool Result = false; };
  KERN_PURE Terrain_ground_readyResult Terrain_ground_ready(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    Vec4 a = Vec4{0.0f, 0.0f, 0.0f, 0.0f};
    a = engine_math_math::Vec4_new(x, y, x, y).Result;
    if (logic_or(logic_not(Terrain_covers(self, a).Result), logic_not(Terrain_corridors_at(self, x, y).Result))) {
      return {false};
    }
    return {Terrain_has_collision(self, x, y).Result};
  }

  struct want_tiles_finish_matchResult { bool Result = false; };
  KERN_PURE want_tiles_finish_matchResult want_tiles_finish_match(const WantedTile& a, const WantedTile& b) {
    return {math_compare<"<", f32>(a.d, b.d)};
  }

  void want_tiles_finish(List<WantedTile>& v, List<u64>& out) {
    i64 at = 0;
    i64 end = 0;
    bool go = false;
    list_sort<&terrain_cpp::want_tiles_finish_match>(*this, v);
    list_clear(out);
    for (auto it114 = flow_for(n114, 0, math_sub<i64>(list_length<WantedTile>(v), 1)); it114.next();) {
      at = 0;
      end = list_length<u64>(out);
      go = math_compare<"<", i64>(at, end);
      while (loop(go)) {
        if (math_compare<"==", u64>(out[at], v[n114.index].k)) {
          go = false;
        } else {
          at += 1;
          go = math_compare<"<", i64>(at, end);
        }
      }
      if (math_compare<"==", i64>(at, list_length<u64>(out))) {
        list_add(out, v[n114.index].k);
      }
    }
  }

  void Terrain_want_tiles(Terrain& self, Vector focus = Vector{0.0, 0.0, 0.0}, Vector anchor = Vector{0.0, 0.0, 0.0}) {
    List<WantedTile> render = List<WantedTile>{};
    List<WantedTile> coll = List<WantedTile>{};
    List<WantedTile> water = List<WantedTile>{};
    i32 bi = 0;
    i32 bj = 0;
    bool skip = false;
    f32 d = 0.0f;
    i32 ti = 0;
    i32 tj = 0;
    f32 td = 0.0f;
    List<Vector_f32_pair> around = List<Vector_f32_pair>{};
    i32 ci = 0;
    i32 cj = 0;
    i32 rr = 0;
    f32 d_2 = 0.0f;
    Set<u64> chunks = Set<u64>{};
    i32 bx = 0;
    i32 by = 0;
    i32 ci_2 = 0;
    i32 cj_2 = 0;
    u64 key = 0;
    u64 k = 0;
    i32 i = 0;
    i32 j = 0;
    f32 d_3 = 0.0f;
    render = List<WantedTile>{};
    coll = List<WantedTile>{};
    water = List<WantedTile>{};
    bi = kern::formula<f32, f32>([](auto x, auto kBlock) { return num_cast<i32, f32>(floor((x / kBlock))); }, vector_break(focus).x, kBlock);
    bj = kern::formula<f32, f32>([](auto y, auto kBlock) { return num_cast<i32, f32>(floor((y / kBlock))); }, vector_break(focus).y, kBlock);
    for (auto it103 = flow_for(n103, num_cast<i64, i32>(kern::formula<i32>([](auto bj) { return (bj - 5); }, bj)), num_cast<i64, i32>(kern::formula<i32>([](auto bj) { return (bj + 5); }, bj))); it103.next();) {
      for (auto it104 = flow_for(n104, num_cast<i64, i32>(kern::formula<i32>([](auto bi) { return (bi - 5); }, bi)), num_cast<i64, i32>(kern::formula<i32>([](auto bi) { return (bi + 5); }, bi))); it104.next();) {
        skip = false;
        d = rect_distance(vector_break(focus).x, vector_break(focus).y, kern::formula<i64, f32>([](auto index, auto kBlock) { return (num_cast<f32, i32>(num_cast<i32, i64>(index)) * kBlock); }, n104.index, kBlock), kern::formula<i64, f32>([](auto index, auto kBlock) { return (num_cast<f32, i32>(num_cast<i32, i64>(index)) * kBlock); }, n103.index, kBlock), kern::formula<i64, f32>([](auto index, auto kBlock) { return (num_cast<f32, i32>((num_cast<i32, i64>(index) + 1)) * kBlock); }, n104.index, kBlock), kern::formula<i64, f32>([](auto index, auto kBlock) { return (num_cast<f32, i32>((num_cast<i32, i64>(index) + 1)) * kBlock); }, n103.index, kBlock)).Result;
        if (math_compare<">", f32>(d, 3200.0f)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (math_compare<"<", f32>(d, 200.0f)) {
            for (auto it105 = flow_for(n105, 0, 3); it105.next();) {
              for (auto it106 = flow_for(n106, 0, 3); it106.next();) {
                ti = kern::formula<i64, i64>([](auto index, auto index_2) { return ((num_cast<i32, i64>(index) * 4) + num_cast<i32, i64>(index_2)); }, n104.index, n106.index);
                tj = kern::formula<i64, i64>([](auto index, auto index_2) { return ((num_cast<i32, i64>(index) * 4) + num_cast<i32, i64>(index_2)); }, n103.index, n105.index);
                td = rect_distance(vector_break(focus).x, vector_break(focus).y, kern::formula<i32, f32>([](auto ti, auto kTile) { return (num_cast<f32, i32>(ti) * kTile); }, ti, kTile), kern::formula<i32, f32>([](auto tj, auto kTile) { return (num_cast<f32, i32>(tj) * kTile); }, tj, kTile), kern::formula<i32, f32>([](auto ti, auto kTile) { return (num_cast<f32, i32>((ti + 1)) * kTile); }, ti, kTile), kern::formula<i32, f32>([](auto tj, auto kTile) { return (num_cast<f32, i32>((tj + 1)) * kTile); }, tj, kTile)).Result;
                list_add(render, struct_make<WantedTile>(td, key3(kern::formula<f32, i32, i32>([](auto td, auto a0, auto a1) { return ((td < 260.0f) ? a0 : a1); }, td, 0, 1), ti, tj).Result));
              }
            }
          } else {
            list_add(render, struct_make<WantedTile>(d, key3(2, num_cast<i32, i64>(n104.index), num_cast<i32, i64>(n103.index)).Result));
          }
        }
      }
    }
    around = list_make<Vector_f32_pair, "2">(struct_make<Vector_f32_pair>(focus, 420.0f), struct_make<Vector_f32_pair>(anchor, 200.0f));
    for (auto it107 = flow_for(n107, 0, math_sub<i64>(list_length<Vector>(self.extra), 1)); it107.next();) {
      list_add(around, struct_make<Vector_f32_pair>(self.extra[n107.index], 300.0f));
    }
    for (auto it108 = flow_for(n108, 0, math_sub<i64>(list_length<Vector_f32_pair>(around), 1)); it108.next();) {
      ci = kern::formula<f32, f32>([](auto x, auto kTile) { return num_cast<i32, f32>(floor((x / kTile))); }, vector_break(around[n108.index].first).x, kTile);
      cj = kern::formula<f32, f32>([](auto y, auto kTile) { return num_cast<i32, f32>(floor((y / kTile))); }, vector_break(around[n108.index].first).y, kTile);
      rr = kern::formula<f32, f32>([](auto a, auto kTile) { return num_cast<i32, f32>(ceil((a / kTile))); }, around[n108.index].second, kTile);
      for (auto it109 = flow_for(n109, num_cast<i64, i32>(kern::formula<i32, i32>([](auto cj, auto rr) { return (cj - rr); }, cj, rr)), num_cast<i64, i32>(kern::formula<i32, i32>([](auto cj, auto rr) { return (cj + rr); }, cj, rr))); it109.next();) {
        for (auto it110 = flow_for(n110, num_cast<i64, i32>(kern::formula<i32, i32>([](auto ci, auto rr) { return (ci - rr); }, ci, rr)), num_cast<i64, i32>(kern::formula<i32, i32>([](auto ci, auto rr) { return (ci + rr); }, ci, rr))); it110.next();) {
          d_2 = rect_distance(vector_break(around[n108.index].first).x, vector_break(around[n108.index].first).y, kern::formula<i64, f32>([](auto index, auto kTile) { return (num_cast<f32, i32>(num_cast<i32, i64>(index)) * kTile); }, n110.index, kTile), kern::formula<i64, f32>([](auto index, auto kTile) { return (num_cast<f32, i32>(num_cast<i32, i64>(index)) * kTile); }, n109.index, kTile), kern::formula<i64, f32>([](auto index, auto kTile) { return (num_cast<f32, i32>((num_cast<i32, i64>(index) + 1)) * kTile); }, n110.index, kTile), kern::formula<i64, f32>([](auto index, auto kTile) { return (num_cast<f32, i32>((num_cast<i32, i64>(index) + 1)) * kTile); }, n109.index, kTile)).Result;
          if (math_compare<"<=", f32>(d_2, around[n108.index].second)) {
            list_add(coll, struct_make<WantedTile>(d_2, key2(num_cast<i32, i64>(n110.index), num_cast<i32, i64>(n109.index)).Result));
          }
        }
      }
    }
    chunks = Set<u64>{};
    for (auto it111 = flow_for(n111, 0, math_sub<i64>(map_length<u64, Terrain_WaterBlock>(self.water_), 1)); it111.next();) {
      bx = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a >> 32)); }, map_key_at<u64, Terrain_WaterBlock>(self.water_, n111.index));
      by = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a & 4294967295)); }, map_key_at<u64, Terrain_WaterBlock>(self.water_, n111.index));
      ci_2 = kern::formula<i32, f32, f32>([](auto bx, auto kWaterBlock, auto kWaterChunk) { return num_cast<i32, f32>(floor(((num_cast<f32, i32>(bx) * kWaterBlock) / kWaterChunk))); }, bx, kWaterBlock, kWaterChunk);
      cj_2 = kern::formula<i32, f32, f32>([](auto by, auto kWaterBlock, auto kWaterChunk) { return num_cast<i32, f32>(floor(((num_cast<f32, i32>(by) * kWaterBlock) / kWaterChunk))); }, by, kWaterBlock, kWaterChunk);
      key = key2(ci_2, cj_2).Result;
      set_add(n112, chunks, key);
    }
    for (auto it113 = flow_for(n113, 0, math_sub<i64>(set_length<u64>(chunks), 1)); it113.next();) {
      k = set_key_at<u64>(chunks, n113.index);
      i = kern::formula<u64>([](auto k) { return num_cast<i32, u64>((k >> 32)); }, k);
      j = kern::formula<u64>([](auto k) { return num_cast<i32, u64>((k & 4294967295)); }, k);
      d_3 = rect_distance(vector_break(focus).x, vector_break(focus).y, kern::formula<i32, f32>([](auto i, auto kWaterChunk) { return (num_cast<f32, i32>(i) * kWaterChunk); }, i, kWaterChunk), kern::formula<i32, f32>([](auto j, auto kWaterChunk) { return (num_cast<f32, i32>(j) * kWaterChunk); }, j, kWaterChunk), kern::formula<i32, f32>([](auto i, auto kWaterChunk) { return (num_cast<f32, i32>((i + 1)) * kWaterChunk); }, i, kWaterChunk), kern::formula<i32, f32>([](auto j, auto kWaterChunk) { return (num_cast<f32, i32>((j + 1)) * kWaterChunk); }, j, kWaterChunk)).Result;
      if (math_compare<"<", f32>(d_3, 2400.0f)) {
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

  struct build_render_tile_HResult { f32 Result = 0.0f; };
  KERN_PURE build_render_tile_HResult build_render_tile_H(const List<f32>& h, i32 n = 0, i32 i = 0, i32 j = 0) {
    return {h[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto j, auto n, auto i) { return ((num_cast<u64, i32>((j + 1)) * num_cast<u64, i32>((n + 2))) + num_cast<u64, i32>((i + 1))); }, j, n, i))]};
  }

  void build_render_tile_skirt(MeshData& m, i32 cells = 0, i32 n = 0, f32 drop = 0.0f, i32 i0 = 0, i32 j0 = 0, i32 di = 0, i32 dj = 0, bool flip = false) {
    i32 ia = 0;
    i32 ja = 0;
    i32 ib = 0;
    i32 jb = 0;
    u32 top_a = 0;
    u32 top_b = 0;
    u32 base = 0;
    u32 vertex_count = 0;
    u32 item = 0;
    u32 item_2 = 0;
    Fixed<u32, 2> items = Fixed<u32, 2>{};
    u32 v = 0;
    Vector p = Vector{0.0, 0.0, 0.0};
    i64 place = 0;
    u32 item_3 = 0;
    u32 item_4 = 0;
    u32 item_5 = 0;
    u32 item_6 = 0;
    u32 item_7 = 0;
    u32 item_8 = 0;
    i64 place_2 = 0;
    u32 item_9 = 0;
    u32 item_10 = 0;
    u32 item_11 = 0;
    u32 item_12 = 0;
    u32 item_13 = 0;
    u32 item_14 = 0;
    for (auto it129 = flow_for(n129, 0, math_sub<i64>(num_cast<i64, i32>(cells), 1)); it129.next();) {
      ia = kern::formula<i32, i32, i64>([](auto i0, auto di, auto index) { return (i0 + (di * num_cast<i32, i64>(index))); }, i0, di, n129.index);
      ja = kern::formula<i32, i32, i64>([](auto j0, auto dj, auto index) { return (j0 + (dj * num_cast<i32, i64>(index))); }, j0, dj, n129.index);
      ib = kern::formula<i32, i32>([](auto ia, auto di) { return (ia + di); }, ia, di);
      jb = kern::formula<i32, i32>([](auto ja, auto dj) { return (ja + dj); }, ja, dj);
      top_a = kern::formula<i32, i32, i32>([](auto ja, auto n, auto ia) { return num_cast<u32, i32>(((ja * n) + ia)); }, ja, n, ia);
      top_b = kern::formula<i32, i32, i32>([](auto jb, auto n, auto ib) { return num_cast<u32, i32>(((jb * n) + ib)); }, jb, n, ib);
      vertex_count = engine_mesh_data_vertex_count(m);
      base = vertex_count;
      item = top_a;
      item_2 = top_b;
      items = list_make<u32, "2">(item, item_2);
      for (auto it130 = flow_for(n130, 0, math_sub<i64>(list_length<u32>(items), 1)); it130.next();) {
        v = items[n130.index];
        p = vector_make(m.positions[num_cast<i64, u64>(kern::formula<u32>([](auto v) { return num_cast<u64, u32>((v * 3)); }, v))], m.positions[num_cast<i64, u64>(kern::formula<u32>([](auto v) { return num_cast<u64, u32>(((v * 3) + 1)); }, v))], kern::formula<f32, f32>([](auto a, auto drop) { return (a - drop); }, m.positions[num_cast<i64, u64>(kern::formula<u32>([](auto v) { return num_cast<u64, u32>(((v * 3) + 2)); }, v))], drop));
        engine_mesh_data_push(n131, m, p, Vector{0.0, 0.0, 1.0}, kGround, vector_make(kern::formula<f32>([](auto x) { return (x * 0.1f); }, vector_break(p).x), kern::formula<f32>([](auto y) { return (y * 0.1f); }, vector_break(p).y), 0.0f));
        m = n131.self;
      }
      if (flip) {
        place = list_length<u32>(m.indices);
        item_3 = top_a;
        item_4 = base;
        item_5 = top_b;
        item_6 = top_b;
        item_7 = base;
        item_8 = kern::formula<u32>([](auto base) { return (base + 1); }, base);
        list_insert_list(m.indices, place, list_make<u32, "6">(item_3, item_4, item_5, item_6, item_7, item_8));
      } else {
        place_2 = list_length<u32>(m.indices);
        item_9 = top_a;
        item_10 = top_b;
        item_11 = base;
        item_12 = top_b;
        item_13 = kern::formula<u32>([](auto base) { return (base + 1); }, base);
        item_14 = base;
        list_insert_list(m.indices, place_2, list_make<u32, "6">(item_9, item_10, item_11, item_12, item_13, item_14));
      }
    }
  }

  void Terrain_build_render_tile(Terrain& self, i32 level = 0, i32 ti = 0, i32 tj = 0) {
    f32 size = 0.0f;
    i32 cells = 0;
    f32 step = 0.0f;
    f32 x0 = 0.0f;
    f32 y0 = 0.0f;
    i32 n = 0;
    MeshData m = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    List<f32> h = List<f32>{};
    List<f32> made = List<f32>{};
    List<Vector> tint = List<Vector>{};
    List<Vector> made_2 = List<Vector>{};
    List<f32> xs = List<f32>{};
    f32 x1 = 0.0f;
    f32 y1 = 0.0f;
    bool skip = false;
    bool skip_2 = false;
    f32 y = 0.0f;
    u64 q = 0;
    u64 r = 0;
    u64 was = 0;
    Vector p = Vector{0.0, 0.0, 0.0};
    Vector o = Vector{0.0, 0.0, 0.0};
    u64 q_2 = 0;
    i32 i0 = 0;
    i32 i1 = 0;
    f32 x = 0.0f;
    f32 y_2 = 0.0f;
    f32 z = 0.0f;
    f32 dx = 0.0f;
    f32 dy = 0.0f;
    Vector nn = Vector{0.0, 0.0, 0.0};
    Vector normalize = Vector{0.0, 0.0, 0.0};
    Vector c = Vector{0.0, 0.0, 0.0};
    u32 a = 0;
    u32 b = 0;
    u32 c_2 = 0;
    u32 d = 0;
    i64 place = 0;
    u32 item = 0;
    u32 item_2 = 0;
    u32 item_3 = 0;
    u32 item_4 = 0;
    u32 item_5 = 0;
    u32 item_6 = 0;
    f32 drop = 0.0f;
    MeshLook look = MeshLook{Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}}, 0, 0, true, 1, true};
    u32 id = 0;
    u32 allocate_id = 0;
    bool upsert = false;
    u64 key = 0;
    i64 t_row = 0;
    bool remove = false;
    size = kern::formula<i32, f32, f32>([](auto level, auto kBlock, auto kTile) { return ((level == 2) ? kBlock : kTile); }, level, kBlock, kTile);
    cells = kern::formula<i32, i32, i32, i32>([](auto level, auto kCells, auto a8, auto a16) { return ((level == 0) ? kCells : ((level == 1) ? a8 : a16)); }, level, kCells, 8, 16);
    step = kern::formula<f32, i32>([](auto size, auto cells) { return (size / num_cast<f32, i32>(cells)); }, size, cells);
    x0 = kern::formula<i32, f32>([](auto ti, auto size) { return (num_cast<f32, i32>(ti) * size); }, ti, size);
    y0 = kern::formula<i32, f32>([](auto tj, auto size) { return (num_cast<f32, i32>(tj) * size); }, tj, size);
    n = kern::formula<i32>([](auto cells) { return (cells + 1); }, cells);
    m = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    made = List<f32>{};
    list_resize(made, num_cast<i64, u64>(kern::formula<i32>([](auto n) { return (num_cast<u64, i32>((n + 2)) * num_cast<u64, i32>((n + 2))); }, n)), 0.0f);
    h = made;
    for (auto it115 = flow_for(n115, -1, num_cast<i64, i32>(n)); it115.next();) {
      for (auto it116 = flow_for(n116, -1, num_cast<i64, i32>(n)); it116.next();) {
        h[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto n, auto index_2) { return ((num_cast<u64, i32>((num_cast<i32, i64>(index) + 1)) * num_cast<u64, i32>((n + 2))) + num_cast<u64, i32>((num_cast<i32, i64>(index_2) + 1))); }, n115.index, n, n116.index))] = Terrain_node_height(self, kern::formula<f32, i64, f32>([](auto x0, auto index, auto step) { return (x0 + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * step)); }, x0, n116.index, step), kern::formula<f32, i64, f32>([](auto y0, auto index, auto step) { return (y0 + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * step)); }, y0, n115.index, step)).Result;
      }
    }
    made_2 = List<Vector>{};
    list_resize(made_2, num_cast<i64, u64>(kern::formula<i32>([](auto n) { return (num_cast<u64, i32>(n) * num_cast<u64, i32>(n)); }, n)), Vector{0.0, 0.0, 0.0});
    tint = made_2;
    for (auto it117 = flow_for(n117, 0, math_sub<i64>(num_cast<i64, i32>(n), 1)); it117.next();) {
      for (auto it118 = flow_for(n118, 0, math_sub<i64>(num_cast<i64, i32>(n), 1)); it118.next();) {
        n119 = Terrain_ground_albedo(self, tint[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto n, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(n)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n117.index, n, n118.index))], kern::formula<f32, i64, f32>([](auto x0, auto index, auto step) { return (x0 + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * step)); }, x0, n118.index, step), kern::formula<f32, i64, f32>([](auto y0, auto index, auto step) { return (y0 + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * step)); }, y0, n117.index, step));
        if (logic_not(n119.Result)) {
          tint[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto n, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(n)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n117.index, n, n118.index))] = Vector{0.16, 0.2, 0.1};
        }
      }
    }
    xs = List<f32>{};
    x1 = kern::formula<f32, f32>([](auto x0, auto size) { return (x0 + size); }, x0, size);
    y1 = kern::formula<f32, f32>([](auto y0, auto size) { return (y0 + size); }, y0, size);
    for (auto it120 = flow_for(n120, 0, math_sub<i64>(list_length<Terrain_Tint>(self.tints_), 1)); it120.next();) {
      skip = false;
      if (logic_or(logic_or(logic_or(math_compare<">", f32>(self.tints_[n120.index].box.x, x1), math_compare<"<", f32>(self.tints_[n120.index].box.z, x0)), math_compare<">", f32>(self.tints_[n120.index].box.y, y1)), math_compare<"<", f32>(self.tints_[n120.index].box.w, y0))) {
        skip = true;
      }
      if (logic_not(skip)) {
        for (auto it121 = flow_for(n121, 0, math_sub<i64>(num_cast<i64, i32>(n), 1)); it121.next();) {
          skip_2 = false;
          y = kern::formula<f32, i64, f32>([](auto y0, auto index, auto step) { return (y0 + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * step)); }, y0, n121.index, step);
          if (logic_or(math_compare<"<", f32>(y, self.tints_[n120.index].box.y), math_compare<">", f32>(y, self.tints_[n120.index].box.w))) {
            skip_2 = true;
          }
          if (logic_not(skip_2)) {
            list_clear(xs);
            for (auto it122 = flow_for(n122, 0, math_sub<i64>(list_length<Vector_row>(self.tints_[n120.index].rings), 1)); it122.next();) {
              q = 0;
              r = kern::formula<u64>([](auto a) { return (a - 1); }, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(self.tints_[n120.index].rings[n122.index]).items)));
              while (loop(math_compare<"<", u64>(q, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(self.tints_[n120.index].rings[n122.index]).items))))) {
                p = self.tints_[n120.index].rings[n122.index].items[num_cast<i64, u64>(q)];
                o = self.tints_[n120.index].rings[n122.index].items[num_cast<i64, u64>(r)];
                if (math_compare<"!=", i32>(num_cast<i32, bool>(math_compare<">", f32>(vector_break(p).y, y)), num_cast<i32, bool>(math_compare<">", f32>(vector_break(o).y, y)))) {
                  list_add(xs, kern::formula<f32, f32, f32, f32, f32>([](auto x, auto y, auto y_2, auto x_2, auto y_3) { return (x + (((y - y_2) * (x_2 - x)) / (y_3 - y_2))); }, vector_break(p).x, y, vector_break(p).y, vector_break(o).x, vector_break(o).y));
                }
                was = q;
                q += 1;
                r = was;
              }
            }
            list_sort<nullptr>(xs);
            q_2 = 0;
            while (loop(math_compare<"<", u64>(kern::formula<u64>([](auto q_2) { return (q_2 + 1); }, q_2), num_cast<u64, i64>(list_length<f32>(xs))))) {
              i0 = kern::formula<f32, f32, f32>([](auto a, auto x0, auto step) { return max(0, num_cast<i32, f32>(ceil(((a - x0) / step)))); }, xs[num_cast<i64, u64>(q_2)], x0, step);
              i1 = kern::formula<i32, f32, f32, f32>([](auto n, auto a, auto x0, auto step) { return min((n - 1), num_cast<i32, f32>(floor(((a - x0) / step)))); }, n, xs[num_cast<i64, u64>(kern::formula<u64>([](auto q_2) { return (q_2 + 1); }, q_2))], x0, step);
              for (auto it123 = flow_for(n123, num_cast<i64, i32>(i0), num_cast<i64, i32>(i1)); it123.next();) {
                tint[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto n, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(n)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n121.index, n, n123.index))] = vector_add(vector_scale(tint[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto n, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(n)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n121.index, n, n123.index))], 0.45f), vector_scale(self.tints_[n120.index].lin, 0.55f));
              }
              q_2 += 2;
            }
          }
        }
      }
    }
    for (auto it124 = flow_for(n124, 0, math_sub<i64>(num_cast<i64, i32>(n), 1)); it124.next();) {
      for (auto it125 = flow_for(n125, 0, math_sub<i64>(num_cast<i64, i32>(n), 1)); it125.next();) {
        x = kern::formula<f32, i64, f32>([](auto x0, auto index, auto step) { return (x0 + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * step)); }, x0, n125.index, step);
        y_2 = kern::formula<f32, i64, f32>([](auto y0, auto index, auto step) { return (y0 + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * step)); }, y0, n124.index, step);
        z = build_render_tile_H(h, n, num_cast<i32, i64>(n125.index), num_cast<i32, i64>(n124.index)).Result;
        dx = kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return (the_result - the_result_2); }, build_render_tile_H(h, n, kern::formula<i64>([](auto index) { return (num_cast<i32, i64>(index) + 1); }, n125.index), num_cast<i32, i64>(n124.index)).Result, build_render_tile_H(h, n, kern::formula<i64>([](auto index) { return (num_cast<i32, i64>(index) - 1); }, n125.index), num_cast<i32, i64>(n124.index)).Result);
        dy = kern::formula<f32, f32>([](auto the_result, auto the_result_2) { return (the_result - the_result_2); }, build_render_tile_H(h, n, num_cast<i32, i64>(n125.index), kern::formula<i64>([](auto index) { return (num_cast<i32, i64>(index) + 1); }, n124.index)).Result, build_render_tile_H(h, n, num_cast<i32, i64>(n125.index), kern::formula<i64>([](auto index) { return (num_cast<i32, i64>(index) - 1); }, n124.index)).Result);
        normalize = engine_normalize(vector_make(kern::formula<f32>([](auto dx) { return (-dx); }, dx), kern::formula<f32>([](auto dy) { return (-dy); }, dy), kern::formula<f32>([](auto step) { return (2.0f * step); }, step)), Vector{0.0, 0.0, 1.0});
        nn = normalize;
        c = to_srgb(tint[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto n, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(n)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n124.index, n, n125.index))]).Result;
        if (math_compare<"<", f32>(z, kern::formula<f32>([](auto the_result) { return (the_result - 0.6f); }, Terrain_elevation(self, x, y_2).Result))) {
          c = logic_select<Vector>(Terrain_water_at(self, x, y_2).Result, kBed, kSoil);
        } else if (math_compare<"==", i32>(Terrain_surface_at(self, x, y_2).Result, 2)) {
          c = kSoil;
        }
        engine_mesh_data_push(n126, m, vector_make(x, y_2, z), nn, c, vector_make(kern::formula<f32>([](auto x) { return (x * 0.1f); }, x), kern::formula<f32>([](auto y_2) { return (y_2 * 0.1f); }, y_2), 0.0f));
        m = n126.self;
      }
    }
    for (auto it127 = flow_for(n127, 0, math_sub<i64>(num_cast<i64, i32>(cells), 1)); it127.next();) {
      for (auto it128 = flow_for(n128, 0, math_sub<i64>(num_cast<i64, i32>(cells), 1)); it128.next();) {
        a = kern::formula<i64, i32, i64>([](auto index, auto n, auto index_2) { return num_cast<u32, i32>(((num_cast<i32, i64>(index) * n) + num_cast<i32, i64>(index_2))); }, n127.index, n, n128.index);
        b = kern::formula<u32>([](auto a) { return (a + 1); }, a);
        c_2 = kern::formula<u32, i32>([](auto a, auto n) { return (a + num_cast<u32, i32>(n)); }, a, n);
        d = kern::formula<u32>([](auto c_2) { return (c_2 + 1); }, c_2);
        place = list_length<u32>(m.indices);
        item = a;
        item_2 = b;
        item_3 = d;
        item_4 = a;
        item_5 = d;
        item_6 = c_2;
        list_insert_list(m.indices, place, list_make<u32, "6">(item, item_2, item_3, item_4, item_5, item_6));
      }
    }
    drop = kern::formula<i32, f32, f32, f32>([](auto level, auto a0f, auto a0f_2, auto a0f_3) { return ((level == 0) ? a0f : ((level == 1) ? a0f_2 : a0f_3)); }, level, 2.0f, 4.0f, 10.0f);
    build_render_tile_skirt(m, cells, n, drop, 0, 0, 1, 0, false);
    build_render_tile_skirt(m, cells, n, drop, 0, kern::formula<i32>([](auto n) { return (n - 1); }, n), 1, 0, true);
    build_render_tile_skirt(m, cells, n, drop, 0, 0, 0, 1, true);
    build_render_tile_skirt(m, cells, n, drop, kern::formula<i32>([](auto n) { return (n - 1); }, n), 0, 0, 1, false);
    look = MeshLook{Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}}, 0, 0, true, 1, true};
    look.material.roughness = 1.0f;
    look.material.cast_shadow = false;
    look.material.double_sided = true;
    engine_renderer_allocate_id(n132, self.r_);
    allocate_id = n132.result;
    id = allocate_id;
    engine_upsert(n133, self.r_, id, m, look, List<f32>{}, List<f32>{});
    upsert = n133.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, id);
      return;
    }
    key = key3(level, ti, tj).Result;
    map_add(n134, self.render_, key);
    t_row = n134.index;
    if (num_cast<bool, u32>(self.render_[t_row].mesh)) {
      engine_renderer_remove(n135, self.r_, self.render_[t_row].mesh);
      remove = n135.result;
    }
    self.render_[t_row].mesh = id;
  }

  void Terrain_build_collision_tile(Terrain& self, i32 ti = 0, i32 tj = 0) {
    i32 N = 0;
    f32 x0 = 0.0f;
    f32 y0 = 0.0f;
    List<Vector> pts = List<Vector>{};
    List<Vector> made = List<Vector>{};
    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 z = 0.0f;
    List<u32> idx = List<u32>{};
    i32 r = 0;
    i32 c = 0;
    u32 a = 0;
    u32 b = 0;
    u32 cc = 0;
    u32 d = 0;
    i64 place = 0;
    u32 item = 0;
    u32 item_2 = 0;
    u32 item_3 = 0;
    u32 item_4 = 0;
    u32 item_5 = 0;
    u32 item_6 = 0;
    u64 key = 0;
    i64 t_row = 0;
    bool valid = false;
    bool body_valid = false;
    bool body_valid_2 = false;
    bool both = false;
    bool same = false;
    u64 q = 0;
    physics_BodySettings bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f, 1.0f, true, true, true, false, 0};
    physics_Body body = physics_Body{0};
    physics_Body create_body = physics_Body{0};
    physics_ShapeSettings ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, List<u32>{}, physics_ShapeMaterial{1.0f, 0.6f, 0.0f, 0.0f, false, 1, 18446744073709551615u, 0, true, true, false, 0, physics_Surface::none}};
    physics_Shape create_shape = physics_Shape{0};
    bool valid_2 = false;
    bool body_valid_3 = false;
    bool body_valid_4 = false;
    bool both_2 = false;
    N = kern::formula<i32>([](auto kCells) { return (kCells + 3); }, kCells);
    x0 = kern::formula<i32, f32, f32>([](auto ti, auto kTile, auto kStep) { return ((num_cast<f32, i32>(ti) * kTile) - kStep); }, ti, kTile, kStep);
    y0 = kern::formula<i32, f32, f32>([](auto tj, auto kTile, auto kStep) { return ((num_cast<f32, i32>(tj) * kTile) - kStep); }, tj, kTile, kStep);
    made = List<Vector>{};
    list_resize(made, num_cast<i64, u64>(kern::formula<i32>([](auto n) { return (num_cast<u64, i32>(n) * num_cast<u64, i32>(n)); }, N)), Vector{0.0, 0.0, 0.0});
    pts = made;
    for (auto it136 = flow_for(n136, 0, math_sub<i64>(num_cast<i64, i32>(N), 1)); it136.next();) {
      for (auto it137 = flow_for(n137, 0, math_sub<i64>(num_cast<i64, i32>(N), 1)); it137.next();) {
        x = kern::formula<f32, i64, f32>([](auto x0, auto index, auto kStep) { return (x0 + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * kStep)); }, x0, n137.index, kStep);
        y = kern::formula<f32, i64, f32>([](auto y0, auto index, auto kStep) { return (y0 + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * kStep)); }, y0, n136.index, kStep);
        z = Terrain_node_height(self, x, y).Result;
        if (logic_or(logic_or(logic_or(math_compare<"==", i32>(num_cast<i32, i64>(n136.index), 0), math_compare<"==", i32>(num_cast<i32, i64>(n137.index), 0)), math_compare<"==", i32>(num_cast<i32, i64>(n136.index), kern::formula<i32>([](auto n) { return (n - 1); }, N))), math_compare<"==", i32>(num_cast<i32, i64>(n137.index), kern::formula<i32>([](auto n) { return (n - 1); }, N)))) {
          z = kern::formula<f32, f32>([](auto z, auto kSkirtDrop) { return (z - kSkirtDrop); }, z, kSkirtDrop);
        }
        pts[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto n, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(n)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n136.index, N, n137.index))] = vector_make(x, y, z);
      }
    }
    idx = List<u32>{};
    list_reserve(idx, num_cast<i64, u64>(kern::formula<i32>([](auto n) { return ((num_cast<u64, i32>((n - 1)) * num_cast<u64, i32>((n - 1))) * 6); }, N)));
    r = 0;
    while (loop(math_compare<"<", i32>(kern::formula<i32>([](auto r) { return (r + 1); }, r), N))) {
      c = 0;
      while (loop(math_compare<"<", i32>(kern::formula<i32>([](auto c) { return (c + 1); }, c), N))) {
        a = kern::formula<i32, i32, i32>([](auto r, auto n, auto c) { return num_cast<u32, i32>(((r * n) + c)); }, r, N, c);
        b = kern::formula<u32>([](auto a) { return (a + 1); }, a);
        cc = kern::formula<u32, i32>([](auto a, auto n) { return (a + num_cast<u32, i32>(n)); }, a, N);
        d = kern::formula<u32>([](auto cc) { return (cc + 1); }, cc);
        place = list_length<u32>(idx);
        item = a;
        item_2 = b;
        item_3 = d;
        item_4 = a;
        item_5 = d;
        item_6 = cc;
        list_insert_list(idx, place, list_make<u32, "6">(item, item_2, item_3, item_4, item_5, item_6));
        c += 1;
      }
      r += 1;
    }
    key = key2(ti, tj).Result;
    map_add(n138, self.collision_, key);
    t_row = n138.index;
    valid = engine_body_valid(self.collision_[t_row].body);
    both = valid;
    if (both) {
      body_valid_2 = engine_world_body_valid(self.world_, self.collision_[t_row].body);
      both = body_valid_2;
    }
    if (logic_and(both, math_compare<"==", u64>(num_cast<u64, i64>(list_length<Vector>(self.collision_[t_row].pts)), num_cast<u64, i64>(list_length<Vector>(pts))))) {
      same = true;
      q = 0;
      while (loop(logic_and(math_compare<"<", u64>(q, num_cast<u64, i64>(list_length<Vector>(pts))), same))) {
        same = logic_and(logic_and(math_compare<"==", f32>(vector_break(self.collision_[t_row].pts[num_cast<i64, u64>(q)]).x, vector_break(pts[num_cast<i64, u64>(q)]).x), math_compare<"==", f32>(vector_break(self.collision_[t_row].pts[num_cast<i64, u64>(q)]).y, vector_break(pts[num_cast<i64, u64>(q)]).y)), math_compare<"==", f32>(vector_break(self.collision_[t_row].pts[num_cast<i64, u64>(q)]).z, vector_break(pts[num_cast<i64, u64>(q)]).z));
        q += 1;
      }
      if (same) {
        return;
      }
    }
    bs = physics_BodySettings{physics_BodyType::dynamic, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{0.0, 0.0, 0.0}, Vector{0.0, 0.0, 0.0}, 0.0f, 0.0f, 1.0f, true, true, true, false, 0};
    bs.type = physics_BodyType::fixed;
    engine_world_create_body(n139, self.world_, bs);
    create_body = n139.result;
    body = create_body;
    ss = physics_ShapeSettings{physics_ShapeKind::box, Vector{0.0, 0.0, 0.0}, Quat{0.0f, 0.0f, 0.0f, 1.0f}, Vector{1.0, 1.0, 1.0}, 0.5f, 1.0f, List<Vector>{}, List<u32>{}, physics_ShapeMaterial{1.0f, 0.6f, 0.0f, 0.0f, false, 1, 18446744073709551615u, 0, true, true, false, 0, physics_Surface::none}};
    ss.kind = physics_ShapeKind::mesh;
    ss.points = pts;
    ss.indices = idx;
    ss.material.friction = 0.8f;
    ss.material.category_bits = 1;
    engine_world_create_shape(n140, self.world_, body, ss);
    create_shape = n140.result;
    valid_2 = engine_body_valid(self.collision_[t_row].body);
    both_2 = valid_2;
    if (both_2) {
      body_valid_4 = engine_world_body_valid(self.world_, self.collision_[t_row].body);
      both_2 = body_valid_4;
    }
    if (both_2) {
      engine_world_destroy_body(self.world_, self.collision_[t_row].body);
    }
    self.collision_[t_row].body = body;
    self.collision_[t_row].pts = pts;
  }

  struct Terrain_build_water_chunk_wetResult { bool Result = false; };
  Terrain_build_water_chunk_wetResult Terrain_build_water_chunk_wet(Terrain& self, i32 cx = 0, i32 cy = 0) {
    if (logic_not(Terrain_water_cell(self, cx, cy).Result)) {
      return {false};
    }
    return {logic_or(math_compare<"==", i64>(list_length<Terrain_Bridge>(self.bridges_), 0), logic_not(Terrain_on_bridge(self, kern::formula<i32, f32>([](auto cx, auto kWaterCell) { return ((num_cast<f32, i32>(cx) + 0.5f) * kWaterCell); }, cx, kWaterCell), kern::formula<i32, f32>([](auto cy, auto kWaterCell) { return ((num_cast<f32, i32>(cy) + 0.5f) * kWaterCell); }, cy, kWaterCell)).Result))};
  }

  void Terrain_build_water_chunk_quad(Terrain& self, MeshData& m, const Fixed<f32, 3>& deep, Vector color = Vector{0.0, 0.0, 0.0}, f32 xa = 0.0f, f32 ya = 0.0f, f32 xb = 0.0f, f32 yb = 0.0f) {
    u32 base = 0;
    u32 vertex_count = 0;
    Vector item = Vector{0.0, 0.0, 0.0};
    Vector item_2 = Vector{0.0, 0.0, 0.0};
    Vector item_3 = Vector{0.0, 0.0, 0.0};
    Vector item_4 = Vector{0.0, 0.0, 0.0};
    Fixed<Vector, 4> items = Fixed<Vector, 4>{};
    Vector p = Vector{0.0, 0.0, 0.0};
    i64 place = 0;
    u32 item_5 = 0;
    u32 item_6 = 0;
    u32 item_7 = 0;
    u32 item_8 = 0;
    u32 item_9 = 0;
    u32 item_10 = 0;
    vertex_count = engine_mesh_data_vertex_count(m);
    base = vertex_count;
    item = vector_make(xa, ya, 0.0f);
    item_2 = vector_make(xb, ya, 0.0f);
    item_3 = vector_make(xb, yb, 0.0f);
    item_4 = vector_make(xa, yb, 0.0f);
    items = list_make<Vector, "4">(item, item_2, item_3, item_4);
    for (auto it141 = flow_for(n141, 0, math_sub<i64>(list_length<Vector>(items), 1)); it141.next();) {
      p = items[n141.index];
      engine_mesh_data_push(n142, m, vector_make(vector_break(p).x, vector_break(p).y, kern::formula<f32, f32>([](auto the_result, auto kWaterRenderZ) { return (the_result + kWaterRenderZ); }, Terrain_water_level(self, vector_break(p).x, vector_break(p).y).Result, kWaterRenderZ)), Vector{0.0, 0.0, 1.0}, color, vector_make(kern::formula<f32>([](auto x) { return (x * 0.05f); }, vector_break(p).x), kern::formula<f32>([](auto y) { return (y * 0.05f); }, vector_break(p).y), 0.0f));
      m = n142.self;
      engine_mesh_data_push_extra(n143, m, deep);
      m = n143.self;
    }
    place = list_length<u32>(m.indices);
    item_5 = base;
    item_6 = kern::formula<u32>([](auto base) { return (base + 1); }, base);
    item_7 = kern::formula<u32>([](auto base) { return (base + 2); }, base);
    item_8 = base;
    item_9 = kern::formula<u32>([](auto base) { return (base + 2); }, base);
    item_10 = kern::formula<u32>([](auto base) { return (base + 3); }, base);
    list_insert_list(m.indices, place, list_make<u32, "6">(item_5, item_6, item_7, item_8, item_9, item_10));
  }

  struct Terrain_build_water_chunk_vtxResult { u32 Result = 0; };
  Terrain_build_water_chunk_vtxResult Terrain_build_water_chunk_vtx(Terrain& self, List<i32>& corner, const List<u8>& water, const List<f32>& dist, MeshData& m, i32 per = 0, i32 B = 0, i32 W = 0, i32 cx0 = 0, i32 cy0 = 0, Vector color = Vector{0.0, 0.0, 0.0}, i32 a = 0, i32 b = 0) {
    f32 d = 0.0f;
    Vector fl = Vector{0.0, 0.0, 0.0};
    i32 n = 0;
    i32_i32_pair item = i32_i32_pair{0, 0};
    i32_i32_pair item_2 = i32_i32_pair{0, 0};
    i32_i32_pair item_3 = i32_i32_pair{0, 0};
    i32_i32_pair item_4 = i32_i32_pair{0, 0};
    Fixed<i32_i32_pair, 4> items = Fixed<i32_i32_pair, 4>{};
    bool skip = false;
    i32_i32_pair v = i32_i32_pair{0, 0};
    i32 i = 0;
    i32 j = 0;
    f32 px = 0.0f;
    f32 py = 0.0f;
    f32 x = 0.0f;
    f32 y = 0.0f;
    f32 shore = 0.0f;
    Fixed<f32, 3> rec = Fixed<f32, 3>{};
    u32 vertex_count = 0;
    if (math_compare<">=", i32>(corner[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto b, auto per, auto a) { return ((num_cast<u64, i32>(b) * num_cast<u64, i32>((per + 1))) + num_cast<u64, i32>(a)); }, b, per, a))], 0)) {
      return {num_cast<u32, i32>(corner[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto b, auto per, auto a) { return ((num_cast<u64, i32>(b) * num_cast<u64, i32>((per + 1))) + num_cast<u64, i32>(a)); }, b, per, a))])};
    }
    d = 0.0f;
    fl = Vector{0.0, 0.0, 0.0};
    n = 0;
    item = struct_make<i32_i32_pair>(0, 0);
    item_2 = struct_make<i32_i32_pair>(-1, 0);
    item_3 = struct_make<i32_i32_pair>(0, -1);
    item_4 = struct_make<i32_i32_pair>(-1, -1);
    items = list_make<i32_i32_pair, "4">(item, item_2, item_3, item_4);
    for (auto it153 = flow_for(n153, 0, math_sub<i64>(list_length<i32_i32_pair>(items), 1)); it153.next();) {
      v = items[n153.index];
      skip = false;
      i = kern::formula<i32, i32, i32>([](auto a, auto first, auto b) { return ((a + first) + b); }, a, v.first, B);
      j = kern::formula<i32, i32, i32>([](auto b, auto second, auto b_2) { return ((b + second) + b_2); }, b, v.second, B);
      if (logic_not(num_cast<bool, u8>(water[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto j, auto w, auto i) { return ((num_cast<u64, i32>(j) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(i)); }, j, W, i))]))) {
        skip = true;
      }
      if (logic_not(skip)) {
        d += dist[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto j, auto w, auto i) { return ((num_cast<u64, i32>(j) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(i)); }, j, W, i))];
        px = kern::formula<i32, i32, i32, f32>([](auto cx0, auto a, auto first, auto kWaterCell) { return ((num_cast<f32, i32>(((cx0 + a) + first)) + 0.5f) * kWaterCell); }, cx0, a, v.first, kWaterCell);
        py = kern::formula<i32, i32, i32, f32>([](auto cy0, auto b, auto second, auto kWaterCell) { return ((num_cast<f32, i32>(((cy0 + b) + second)) + 0.5f) * kWaterCell); }, cy0, b, v.second, kWaterCell);
        fl = vector_add(fl, Terrain_flow_at(self, px, py).Result);
        n += 1;
      }
    }
    x = kern::formula<i32, i32, f32>([](auto cx0, auto a, auto kWaterCell) { return (num_cast<f32, i32>((cx0 + a)) * kWaterCell); }, cx0, a, kWaterCell);
    y = kern::formula<i32, i32, f32>([](auto cy0, auto b, auto kWaterCell) { return (num_cast<f32, i32>((cy0 + b)) * kWaterCell); }, cy0, b, kWaterCell);
    shore = kern::formula<i32, f32, f32, f32>([](auto n, auto d, auto kWaterCell, auto a0f) { return ((n == 4) ? fmin(((d / 4.0f) * kWaterCell), 40.0f) : a0f); }, n, d, kWaterCell, 0.0f);
    rec = list_make<f32, "3">(kern::formula<f32>([](auto shore) { return (shore + 1.0f); }, shore), kern::formula<i32, f32, f32>([](auto n, auto x, auto a0f) { return (num_cast<bool, i32>(n) ? (x / num_cast<f32, i32>(n)) : a0f); }, n, vector_break(fl).x, 0.0f), kern::formula<i32, f32, f32>([](auto n, auto y, auto a0f) { return (num_cast<bool, i32>(n) ? (y / num_cast<f32, i32>(n)) : a0f); }, n, vector_break(fl).y, 0.0f));
    vertex_count = engine_mesh_data_vertex_count(m);
    corner[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto b, auto per, auto a) { return ((num_cast<u64, i32>(b) * num_cast<u64, i32>((per + 1))) + num_cast<u64, i32>(a)); }, b, per, a))] = num_cast<i32, u32>(vertex_count);
    engine_mesh_data_push(n154, m, vector_make(x, y, kern::formula<f32, f32>([](auto the_result, auto kWaterRenderZ) { return (the_result + kWaterRenderZ); }, Terrain_water_level(self, x, y).Result, kWaterRenderZ)), Vector{0.0, 0.0, 1.0}, color, vector_make(kern::formula<f32>([](auto x) { return (x * 0.05f); }, x), kern::formula<f32>([](auto y) { return (y * 0.05f); }, y), 0.0f));
    m = n154.self;
    engine_mesh_data_push_extra(n155, m, rec);
    m = n155.self;
    return {num_cast<u32, i32>(corner[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto b, auto per, auto a) { return ((num_cast<u64, i32>(b) * num_cast<u64, i32>((per + 1))) + num_cast<u64, i32>(a)); }, b, per, a))])};
  }

  void Terrain_build_water_chunk(Terrain& self, i32 ci = 0, i32 cj = 0, bool detail = false) {
    MeshData m = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    Vector color = Vector{0.0, 0.0, 0.0};
    Vector hex_color = Vector{0.0, 0.0, 0.0};
    i32 per = 0;
    i32 cx0 = 0;
    i32 cy0 = 0;
    Fixed<f32, 3> deep = Fixed<f32, 3>{};
    i32 x = 0;
    bool skip = false;
    i32 e = 0;
    bool both = false;
    bool go = false;
    i32 B = 0;
    i32 W = 0;
    List<u8> water = List<u8>{};
    List<u8> made = List<u8>{};
    List<f32> dist = List<f32>{};
    List<f32> made_2 = List<f32>{};
    bool any = false;
    bool w = false;
    bool skip_2 = false;
    f32 d = 0.0f;
    i32 j = 0;
    i32 i = 0;
    bool skip_3 = false;
    f32 d_2 = 0.0f;
    List<i32> corner = List<i32>{};
    List<i32> made_3 = List<i32>{};
    bool skip_4 = false;
    u32 p = 0;
    u32 q = 0;
    u32 r = 0;
    u32 s = 0;
    i64 place = 0;
    u32 item = 0;
    u32 item_2 = 0;
    u32 item_3 = 0;
    u32 item_4 = 0;
    u32 item_5 = 0;
    u32 item_6 = 0;
    u64 key = 0;
    u64 key_2 = 0;
    i64 slot_row = 0;
    bool put = false;
    u64 key_3 = 0;
    u32 vertex_count = 0;
    bool remove = false;
    MeshLook look = MeshLook{Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}}, 0, 0, true, 1, true};
    u32 id = 0;
    u32 allocate_id = 0;
    bool upsert = false;
    bool remove_2 = false;
    m = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}};
    m.extra_size = 3;
    hex_color = engine_hex_color(1534872);
    color = hex_color;
    per = kern::formula<f32, f32>([](auto kWaterChunk, auto kWaterCell) { return num_cast<i32, f32>((kWaterChunk / kWaterCell)); }, kWaterChunk, kWaterCell);
    cx0 = kern::formula<i32, i32>([](auto ci, auto per) { return (ci * per); }, ci, per);
    cy0 = kern::formula<i32, i32>([](auto cj, auto per) { return (cj * per); }, cj, per);
    if (logic_not(detail)) {
      deep = list_make<f32, "3">(13.0f, 0.0f, 0.0f);
      for (auto it144 = flow_for(n144, num_cast<i64, i32>(cy0), math_sub<i64>(num_cast<i64, i32>(kern::formula<i32, i32>([](auto cy0, auto per) { return (cy0 + per); }, cy0, per)), 1)); it144.next();) {
        x = cx0;
        while (loop(math_compare<"<", i32>(x, kern::formula<i32, i32>([](auto cx0, auto per) { return (cx0 + per); }, cx0, per)))) {
          skip = false;
          n145 = Terrain_build_water_chunk_wet(self, x, num_cast<i32, i64>(n144.index));
          if (logic_not(n145.Result)) {
            x += 1;
            skip = true;
          }
          if (logic_not(skip)) {
            e = x;
            go = true;
            while (loop(go)) {
              both = math_compare<"<", i32>(e, kern::formula<i32, i32>([](auto cx0, auto per) { return (cx0 + per); }, cx0, per));
              if (both) {
                n147 = Terrain_build_water_chunk_wet(self, e, num_cast<i32, i64>(n144.index));
                both = n147.Result;
              }
              if (both) {
                e += 1;
              } else {
                go = false;
              }
            }
            Terrain_build_water_chunk_quad(self, m, deep, color, kern::formula<i32, f32>([](auto x, auto kWaterCell) { return (num_cast<f32, i32>(x) * kWaterCell); }, x, kWaterCell), kern::formula<i64, f32>([](auto index, auto kWaterCell) { return (num_cast<f32, i32>(num_cast<i32, i64>(index)) * kWaterCell); }, n144.index, kWaterCell), kern::formula<i32, f32>([](auto e, auto kWaterCell) { return (num_cast<f32, i32>(e) * kWaterCell); }, e, kWaterCell), kern::formula<i64, f32>([](auto index, auto kWaterCell) { return (num_cast<f32, i32>((num_cast<i32, i64>(index) + 1)) * kWaterCell); }, n144.index, kWaterCell));
            x = e;
          }
        }
      }
    } else {
      B = 3;
      W = kern::formula<i32, i32>([](auto per, auto b) { return (per + (2 * b)); }, per, B);
      made = List<u8>{};
      list_resize(made, num_cast<i64, u64>(kern::formula<i32>([](auto w) { return (num_cast<u64, i32>(w) * num_cast<u64, i32>(w)); }, W)), 0);
      water = made;
      made_2 = List<f32>{};
      list_resize(made_2, list_length<u8>(water), 0.0f);
      dist = made_2;
      any = false;
      for (auto it148 = flow_for(n148, 0, math_sub<i64>(num_cast<i64, i32>(W), 1)); it148.next();) {
        for (auto it149 = flow_for(n149, 0, math_sub<i64>(num_cast<i64, i32>(W), 1)); it149.next();) {
          n150 = Terrain_build_water_chunk_wet(self, kern::formula<i32, i32, i64>([](auto cx0, auto b, auto index) { return ((cx0 - b) + num_cast<i32, i64>(index)); }, cx0, B, n149.index), kern::formula<i32, i32, i64>([](auto cy0, auto b, auto index) { return ((cy0 - b) + num_cast<i32, i64>(index)); }, cy0, B, n148.index));
          w = n150.Result;
          water[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto w, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n148.index, W, n149.index))] = num_cast<u8, bool>(w);
          dist[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto w, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n148.index, W, n149.index))] = kern::formula<bool, f32, f32>([](auto w, auto a, auto a0f) { return (w ? a : a0f); }, w, 1e+06f, 0.0f);
          any = logic_or(any, logic_and(logic_and(logic_and(logic_and(w, math_compare<">=", i32>(num_cast<i32, i64>(n149.index), B)), math_compare<">=", i32>(num_cast<i32, i64>(n148.index), B)), math_compare<"<", i32>(num_cast<i32, i64>(n149.index), kern::formula<i32, i32>([](auto w, auto b) { return (w - b); }, W, B))), math_compare<"<", i32>(num_cast<i32, i64>(n148.index), kern::formula<i32, i32>([](auto w, auto b) { return (w - b); }, W, B))));
        }
      }
      if (any) {
        for (auto it151 = flow_for(n151, 0, math_sub<i64>(num_cast<i64, i32>(W), 1)); it151.next();) {
          for (auto it152 = flow_for(n152, 0, math_sub<i64>(num_cast<i64, i32>(W), 1)); it152.next();) {
            skip_2 = false;
            if (logic_not(num_cast<bool, u8>(water[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto w, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n151.index, W, n152.index))]))) {
              skip_2 = true;
            }
            if (logic_not(skip_2)) {
              d = dist[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto w, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n151.index, W, n152.index))];
              if (math_compare<">", i32>(num_cast<i32, i64>(n152.index), 0)) {
                d = kern::formula<f32, f32>([](auto d, auto a) { return fmin(d, (a + 1.0f)); }, d, dist[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto w, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>((num_cast<i32, i64>(index_2) - 1))); }, n151.index, W, n152.index))]);
              }
              if (math_compare<">", i32>(num_cast<i32, i64>(n151.index), 0)) {
                d = kern::formula<f32, f32>([](auto d, auto a) { return fmin(d, (a + 1.0f)); }, d, dist[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto w, auto index_2) { return ((num_cast<u64, i32>((num_cast<i32, i64>(index) - 1)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n151.index, W, n152.index))]);
              }
              if (logic_and(math_compare<">", i32>(num_cast<i32, i64>(n152.index), 0), math_compare<">", i32>(num_cast<i32, i64>(n151.index), 0))) {
                d = kern::formula<f32, f32>([](auto d, auto a) { return fmin(d, (a + 1.414f)); }, d, dist[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto w, auto index_2) { return ((num_cast<u64, i32>((num_cast<i32, i64>(index) - 1)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>((num_cast<i32, i64>(index_2) - 1))); }, n151.index, W, n152.index))]);
              }
              if (logic_and(math_compare<"<", i32>(num_cast<i32, i64>(n152.index), kern::formula<i32>([](auto w) { return (w - 1); }, W)), math_compare<">", i32>(num_cast<i32, i64>(n151.index), 0))) {
                d = kern::formula<f32, f32>([](auto d, auto a) { return fmin(d, (a + 1.414f)); }, d, dist[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto w, auto index_2) { return ((num_cast<u64, i32>((num_cast<i32, i64>(index) - 1)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>((num_cast<i32, i64>(index_2) + 1))); }, n151.index, W, n152.index))]);
              }
              dist[num_cast<i64, u64>(kern::formula<i64, i32, i64>([](auto index, auto w, auto index_2) { return ((num_cast<u64, i32>(num_cast<i32, i64>(index)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(num_cast<i32, i64>(index_2))); }, n151.index, W, n152.index))] = d;
            }
          }
        }
        j = kern::formula<i32>([](auto w) { return (w - 1); }, W);
        while (loop(math_compare<">=", i32>(j, 0))) {
          i = kern::formula<i32>([](auto w) { return (w - 1); }, W);
          while (loop(math_compare<">=", i32>(i, 0))) {
            skip_3 = false;
            if (logic_not(num_cast<bool, u8>(water[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto j, auto w, auto i) { return ((num_cast<u64, i32>(j) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(i)); }, j, W, i))]))) {
              skip_3 = true;
            }
            if (logic_not(skip_3)) {
              d_2 = dist[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto j, auto w, auto i) { return ((num_cast<u64, i32>(j) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(i)); }, j, W, i))];
              if (math_compare<"<", i32>(i, kern::formula<i32>([](auto w) { return (w - 1); }, W))) {
                d_2 = kern::formula<f32, f32>([](auto d_2, auto a) { return fmin(d_2, (a + 1.0f)); }, d_2, dist[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto j, auto w, auto i) { return ((num_cast<u64, i32>(j) * num_cast<u64, i32>(w)) + num_cast<u64, i32>((i + 1))); }, j, W, i))]);
              }
              if (math_compare<"<", i32>(j, kern::formula<i32>([](auto w) { return (w - 1); }, W))) {
                d_2 = kern::formula<f32, f32>([](auto d_2, auto a) { return fmin(d_2, (a + 1.0f)); }, d_2, dist[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto j, auto w, auto i) { return ((num_cast<u64, i32>((j + 1)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(i)); }, j, W, i))]);
              }
              if (logic_and(math_compare<"<", i32>(i, kern::formula<i32>([](auto w) { return (w - 1); }, W)), math_compare<"<", i32>(j, kern::formula<i32>([](auto w) { return (w - 1); }, W)))) {
                d_2 = kern::formula<f32, f32>([](auto d_2, auto a) { return fmin(d_2, (a + 1.414f)); }, d_2, dist[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto j, auto w, auto i) { return ((num_cast<u64, i32>((j + 1)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>((i + 1))); }, j, W, i))]);
              }
              if (logic_and(math_compare<">", i32>(i, 0), math_compare<"<", i32>(j, kern::formula<i32>([](auto w) { return (w - 1); }, W)))) {
                d_2 = kern::formula<f32, f32>([](auto d_2, auto a) { return fmin(d_2, (a + 1.414f)); }, d_2, dist[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto j, auto w, auto i) { return ((num_cast<u64, i32>((j + 1)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>((i - 1))); }, j, W, i))]);
              }
              dist[num_cast<i64, u64>(kern::formula<i32, i32, i32>([](auto j, auto w, auto i) { return ((num_cast<u64, i32>(j) * num_cast<u64, i32>(w)) + num_cast<u64, i32>(i)); }, j, W, i))] = d_2;
            }
            i = kern::formula<i32>([](auto i) { return (i - 1); }, i);
          }
          j = kern::formula<i32>([](auto j) { return (j - 1); }, j);
        }
        made_3 = List<i32>{};
        list_resize(made_3, num_cast<i64, u64>(kern::formula<i32>([](auto per) { return (num_cast<u64, i32>((per + 1)) * num_cast<u64, i32>((per + 1))); }, per)), -1);
        corner = made_3;
        for (auto it156 = flow_for(n156, 0, math_sub<i64>(num_cast<i64, i32>(per), 1)); it156.next();) {
          for (auto it157 = flow_for(n157, 0, math_sub<i64>(num_cast<i64, i32>(per), 1)); it157.next();) {
            skip_4 = false;
            if (logic_not(num_cast<bool, u8>(water[num_cast<i64, u64>(kern::formula<i64, i32, i32, i64>([](auto index, auto b, auto w, auto index_2) { return ((num_cast<u64, i32>((num_cast<i32, i64>(index) + b)) * num_cast<u64, i32>(w)) + num_cast<u64, i32>((num_cast<i32, i64>(index_2) + b))); }, n156.index, B, W, n157.index))]))) {
              skip_4 = true;
            }
            if (logic_not(skip_4)) {
              n158 = Terrain_build_water_chunk_vtx(self, corner, water, dist, m, per, B, W, cx0, cy0, color, num_cast<i32, i64>(n157.index), num_cast<i32, i64>(n156.index));
              p = n158.Result;
              n159 = Terrain_build_water_chunk_vtx(self, corner, water, dist, m, per, B, W, cx0, cy0, color, kern::formula<i64>([](auto index) { return (num_cast<i32, i64>(index) + 1); }, n157.index), num_cast<i32, i64>(n156.index));
              q = n159.Result;
              n160 = Terrain_build_water_chunk_vtx(self, corner, water, dist, m, per, B, W, cx0, cy0, color, kern::formula<i64>([](auto index) { return (num_cast<i32, i64>(index) + 1); }, n157.index), kern::formula<i64>([](auto index) { return (num_cast<i32, i64>(index) + 1); }, n156.index));
              r = n160.Result;
              n161 = Terrain_build_water_chunk_vtx(self, corner, water, dist, m, per, B, W, cx0, cy0, color, num_cast<i32, i64>(n157.index), kern::formula<i64>([](auto index) { return (num_cast<i32, i64>(index) + 1); }, n156.index));
              s = n161.Result;
              place = list_length<u32>(m.indices);
              item = p;
              item_2 = q;
              item_3 = r;
              item_4 = p;
              item_5 = r;
              item_6 = s;
              list_insert_list(m.indices, place, list_make<u32, "6">(item, item_2, item_3, item_4, item_5, item_6));
            }
          }
        }
      }
    }
    key = key2(ci, cj).Result;
    key_2 = key;
    map_add(n162, self.water_meshes_, key_2);
    slot_row = n162.index;
    put = detail;
    key_3 = key;
    map_add(n163, self.water_detail_, key_3);
    self.water_detail_[n163.index] = put;
    vertex_count = engine_mesh_data_vertex_count(m);
    if (math_compare<"==", u32>(vertex_count, 0)) {
      if (num_cast<bool, u32>(self.water_meshes_[slot_row])) {
        engine_renderer_remove(n164, self.r_, self.water_meshes_[slot_row]);
        remove = n164.result;
      }
      self.water_meshes_[slot_row] = 0;
      return;
    }
    look = MeshLook{Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}}, 0, 0, true, 1, true};
    look.material.roughness = 0.08f;
    look.material.metalness = 0.0f;
    look.material.cast_shadow = false;
    look.material.alpha = AlphaMode::blend;
    look.extra_slot = 1;
    engine_renderer_allocate_id(n165, self.r_);
    allocate_id = n165.result;
    id = allocate_id;
    engine_upsert(n166, self.r_, id, m, look, List<f32>{}, List<f32>{});
    upsert = n166.result;
    if (logic_not(upsert)) {
      engine_renderer_release_id(self.r_, id);
      return;
    }
    if (num_cast<bool, u32>(self.water_meshes_[slot_row])) {
      engine_renderer_remove(n167, self.r_, self.water_meshes_[slot_row]);
      remove_2 = n167.result;
    }
    self.water_meshes_[slot_row] = id;
  }

  struct Terrain_ready_atResult { bool Result = false; };
  KERN_PURE Terrain_ready_atResult Terrain_ready_at(const Terrain& self, Vector p = Vector{0.0, 0.0, 0.0}) {
    i32 ci = 0;
    i32 cj = 0;
    u64 k = 0;
    if (logic_not(self.settled_)) {
      return {false};
    }
    if (logic_not(object_valid<kern::physics::World>(self.world_))) {
      return {true};
    }
    ci = kern::formula<f32, f32>([](auto x, auto kTile) { return num_cast<i32, f32>(floor((x / kTile))); }, vector_break(p).x, kTile);
    cj = kern::formula<f32, f32>([](auto y, auto kTile) { return num_cast<i32, f32>(floor((y / kTile))); }, vector_break(p).y, kTile);
    for (auto it168 = flow_for(n168, num_cast<i64, i32>(kern::formula<i32>([](auto cj) { return (cj - 1); }, cj)), num_cast<i64, i32>(kern::formula<i32>([](auto cj) { return (cj + 1); }, cj))); it168.next();) {
      for (auto it169 = flow_for(n169, num_cast<i64, i32>(kern::formula<i32>([](auto ci) { return (ci - 1); }, ci)), num_cast<i64, i32>(kern::formula<i32>([](auto ci) { return (ci + 1); }, ci))); it169.next();) {
        k = key2(num_cast<i32, i64>(n169.index), num_cast<i32, i64>(n168.index)).Result;
        if (logic_or(logic_not(num_cast<bool, i64>(num_cast<i64, bool>(map_contains<u64, Terrain_CollisionTile>(self.collision_, k)))), num_cast<bool, i64>(num_cast<i64, bool>(set_contains<u64>(self.dirty_collision_, k))))) {
          return {false};
        }
      }
    }
    return {true};
  }

  void Terrain_update(Terrain& self, Vector focus = Vector{0.0, 0.0, 0.0}, Vector anchor = Vector{0.0, 0.0, 0.0}, f64 budget_ms = 0.0) {
    f64 deadline = 0.0;
    f64 now_ms = 0.0;
    bool extra_moved = false;
    u64 i = 0;
    bool missing = false;
    i64 at = 0;
    i64 count = 0;
    bool stop = false;
    bool skip = false;
    u64 k = 0;
    f64 now_ms_2 = 0.0;
    i32 level = 0;
    i32 i_2 = 0;
    i32 j = 0;
    u64 key = 0;
    i64 at_2 = 0;
    i64 count_2 = 0;
    bool stop_2 = false;
    bool skip_2 = false;
    u64 k_2 = 0;
    i32 wi = 0;
    i32 wj = 0;
    bool detail = false;
    u64 key_2 = 0;
    u64 key_3 = 0;
    bool both = false;
    f64 now_ms_3 = 0.0;
    u64 key_4 = 0;
    Set<u64> keep = Set<u64>{};
    Set<u64> made = Set<u64>{};
    i64 first = 0;
    i64 last = 0;
    i64 it = 0;
    bool remove = false;
    u64 key_5 = 0;
    i64 at_3 = 0;
    Set<u64> made_2 = Set<u64>{};
    i64 first_2 = 0;
    i64 last_2 = 0;
    i64 it_2 = 0;
    bool remove_2 = false;
    u64 key_6 = 0;
    u64 key_7 = 0;
    i64 at_4 = 0;
    if (logic_or(logic_not(object_valid<kern::Renderer>(self.r_)), logic_not(object_valid<kern::physics::World>(self.world_)))) {
      return;
    }
    now_ms = engine_now_ms();
    deadline = kern::formula<f64, f64>([](auto now_ms, auto budget_ms) { return (now_ms + budget_ms); }, now_ms, budget_ms);
    Terrain_plan_windows(self, focus);
    Terrain_plan_images(self, focus);
    if (math_compare<">", i64>(list_length<Terrain_Job>(self.job_), 0)) {
      n170 = Terrain_run_job(self, deadline);
    }
    if (logic_not(self.settled_)) {
      return;
    }
    extra_moved = math_compare<"!=", u64>(num_cast<u64, i64>(list_length<Vector>(self.extra)), num_cast<u64, i64>(list_length<Vector>(self.planned_extra_)));
    i = 0;
    while (loop(logic_and(logic_not(extra_moved), math_compare<"<", u64>(i, num_cast<u64, i64>(list_length<Vector>(self.extra)))))) {
      extra_moved = math_compare<">", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(self.extra[num_cast<i64, u64>(i)]).x, vector_break(self.planned_extra_[num_cast<i64, u64>(i)]).x, vector_break(self.extra[num_cast<i64, u64>(i)]).y, vector_break(self.planned_extra_[num_cast<i64, u64>(i)]).y), 40.0f);
      i += 1;
    }
    if (logic_or(logic_or(logic_or(self.replan_, extra_moved), math_compare<">", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(focus).x, vector_break(self.planned_focus_).x, vector_break(focus).y, vector_break(self.planned_focus_).y), 40.0f)), math_compare<">", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(anchor).x, vector_break(self.planned_anchor_).x, vector_break(anchor).y, vector_break(self.planned_anchor_).y), 40.0f))) {
      Terrain_want_tiles(self, focus, anchor);
    }
    missing = false;
    count = list_length<u64>(self.want_render_);
    at = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<", i64>(at, count)))) {
      k = self.want_render_[at];
      skip = false;
      if (logic_and(num_cast<bool, i64>(num_cast<i64, bool>(map_contains<u64, Terrain_RenderTile>(self.render_, k))), logic_not(num_cast<bool, i64>(num_cast<i64, bool>(set_contains<u64>(self.dirty_render_, k)))))) {
        skip = true;
      }
      if (logic_not(logic_or(stop, skip))) {
        now_ms_2 = engine_now_ms();
        if (math_compare<">", f64>(now_ms_2, deadline)) {
          missing = true;
          stop = true;
        }
        if (logic_not(logic_or(stop, skip))) {
          level = 0;
          i_2 = 0;
          j = 0;
          unkey3(level, i_2, j, k);
          Terrain_build_render_tile(self, level, i_2, j);
          key = k;
          set_remove(n171, self.dirty_render_, key);
          self.stats_.rebuilds = kern::formula<u32>([](auto rebuilds) { return (rebuilds + 1); }, self.stats_.rebuilds);
        }
      }
      if (logic_not(stop)) {
        at += 1;
      }
    }
    count_2 = list_length<u64>(self.want_water_);
    at_2 = 0;
    stop_2 = false;
    while (loop(logic_and(logic_not(stop_2), math_compare<"<", i64>(at_2, count_2)))) {
      k_2 = self.want_water_[at_2];
      skip_2 = false;
      wi = kern::formula<u64>([](auto k_2) { return num_cast<i32, u64>((k_2 >> 32)); }, k_2);
      wj = kern::formula<u64>([](auto k_2) { return num_cast<i32, u64>((k_2 & 4294967295)); }, k_2);
      detail = math_compare<"<", f32>(rect_distance(vector_break(focus).x, vector_break(focus).y, kern::formula<i32, f32>([](auto wi, auto kWaterChunk) { return (num_cast<f32, i32>(wi) * kWaterChunk); }, wi, kWaterChunk), kern::formula<i32, f32>([](auto wj, auto kWaterChunk) { return (num_cast<f32, i32>(wj) * kWaterChunk); }, wj, kWaterChunk), kern::formula<i32, f32>([](auto wi, auto kWaterChunk) { return (num_cast<f32, i32>((wi + 1)) * kWaterChunk); }, wi, kWaterChunk), kern::formula<i32, f32>([](auto wj, auto kWaterChunk) { return (num_cast<f32, i32>((wj + 1)) * kWaterChunk); }, wj, kWaterChunk)).Result, 1000.0f);
      both = logic_and(num_cast<bool, i64>(num_cast<i64, bool>(map_contains<u64, u32>(self.water_meshes_, k_2))), logic_not(num_cast<bool, i64>(num_cast<i64, bool>(set_contains<u64>(self.dirty_water_, k_2)))));
      if (both) {
        key_3 = k_2;
        map_add(n172, self.water_detail_, key_3);
        both = math_compare<"==", i32>(num_cast<i32, bool>(self.water_detail_[n172.index]), num_cast<i32, bool>(detail));
      }
      if (both) {
        skip_2 = true;
      }
      if (logic_not(logic_or(stop_2, skip_2))) {
        now_ms_3 = engine_now_ms();
        if (math_compare<">", f64>(now_ms_3, deadline)) {
          missing = true;
          stop_2 = true;
        }
        if (logic_not(logic_or(stop_2, skip_2))) {
          Terrain_build_water_chunk(self, wi, wj, detail);
          key_4 = k_2;
          set_remove(n173, self.dirty_water_, key_4);
        }
      }
      if (logic_not(stop_2)) {
        at_2 += 1;
      }
    }
    if (logic_not(missing)) {
      made = Set<u64>{};
      first = 0;
      last = math_sub<i64>(list_length<u64>(self.want_render_), 1);
      for (auto it175 = flow_for(n175, first, last); it175.next();) {
        set_add(n174, made, list_get<u64>(self.want_render_, n175.index));
      }
      keep = made;
      it = 0;
      while (loop(logic_not(math_compare<"==", i64>(it, map_length<u64, Terrain_RenderTile>(self.render_))))) {
        if (logic_not(num_cast<bool, i64>(num_cast<i64, bool>(set_contains<u64>(keep, map_key_at<u64, Terrain_RenderTile>(self.render_, it)))))) {
          if (num_cast<bool, u32>(self.render_[it].mesh)) {
            engine_renderer_remove(n176, self.r_, self.render_[it].mesh);
            remove = n176.result;
          }
          key_5 = map_key_at<u64, Terrain_RenderTile>(self.render_, it);
          set_remove(n177, self.dirty_render_, key_5);
          at_3 = it;
          map_remove_at(self.render_, at_3);
          it = at_3;
        } else {
          it += 1;
        }
      }
      made_2 = Set<u64>{};
      first_2 = 0;
      last_2 = math_sub<i64>(list_length<u64>(self.want_water_), 1);
      for (auto it179 = flow_for(n179, first_2, last_2); it179.next();) {
        set_add(n178, made_2, list_get<u64>(self.want_water_, n179.index));
      }
      keep = made_2;
      it_2 = 0;
      while (loop(logic_not(math_compare<"==", i64>(it_2, map_length<u64, u32>(self.water_meshes_))))) {
        if (logic_not(num_cast<bool, i64>(num_cast<i64, bool>(set_contains<u64>(keep, map_key_at<u64, u32>(self.water_meshes_, it_2)))))) {
          if (num_cast<bool, u32>(self.water_meshes_[it_2])) {
            engine_renderer_remove(n180, self.r_, self.water_meshes_[it_2]);
            remove_2 = n180.result;
          }
          key_6 = map_key_at<u64, u32>(self.water_meshes_, it_2);
          set_remove(n181, self.dirty_water_, key_6);
          key_7 = map_key_at<u64, u32>(self.water_meshes_, it_2);
          map_remove(n182, self.water_detail_, key_7);
          at_4 = it_2;
          map_remove_at(self.water_meshes_, at_4);
          it_2 = at_4;
        } else {
          it_2 += 1;
        }
      }
    }
    self.stats_.render_tiles = num_cast<u32, i64>(map_length<u64, Terrain_RenderTile>(self.render_));
    self.stats_.dem = self.grid_ready_;
    self.stats_.settled = self.settled_;
    self.stats_.dem_tiles = 0;
    for (auto it183 = flow_for(n183, 0, math_sub<i64>(map_length<u64, Terrain_DemTile>(self.dem_tiles_), 1)); it183.next();) {
      self.stats_.dem_tiles = kern::formula<u32, bool, i32, i32>([](auto dem_tiles, auto a, auto a0, auto a1) { return (dem_tiles + num_cast<u32, i32>((a ? a0 : a1))); }, self.stats_.dem_tiles, math_compare<"==", i64>(list_length<f32>(self.dem_tiles_[n183.index].h), 0), 0, 1);
    }
  }

  void Terrain_collide(Terrain& self, Vector focus = Vector{0.0, 0.0, 0.0}, Vector anchor = Vector{0.0, 0.0, 0.0}, u32 more = 0) {
    bool extra_moved = false;
    u64 i = 0;
    List<Vector> under = List<Vector>{};
    i64 place = 0;
    i32 ci = 0;
    i32 cj = 0;
    bool skip = false;
    u64 k = 0;
    u64 key = 0;
    bool missing = false;
    u32 built = 0;
    i64 at = 0;
    i64 count = 0;
    bool stop = false;
    bool skip_2 = false;
    u64 k_2 = 0;
    u64 key_2 = 0;
    Set<u64> keep = Set<u64>{};
    Set<u64> made = Set<u64>{};
    i64 first = 0;
    i64 last = 0;
    i64 it = 0;
    bool valid = false;
    bool body_valid = false;
    bool body_valid_2 = false;
    bool both = false;
    u64 key_3 = 0;
    i64 at_2 = 0;
    if (logic_or(logic_not(object_valid<kern::physics::World>(self.world_)), logic_not(self.settled_))) {
      return;
    }
    extra_moved = math_compare<"!=", u64>(num_cast<u64, i64>(list_length<Vector>(self.extra)), num_cast<u64, i64>(list_length<Vector>(self.planned_extra_)));
    i = 0;
    while (loop(logic_and(logic_not(extra_moved), math_compare<"<", u64>(i, num_cast<u64, i64>(list_length<Vector>(self.extra)))))) {
      extra_moved = math_compare<">", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(self.extra[num_cast<i64, u64>(i)]).x, vector_break(self.planned_extra_[num_cast<i64, u64>(i)]).x, vector_break(self.extra[num_cast<i64, u64>(i)]).y, vector_break(self.planned_extra_[num_cast<i64, u64>(i)]).y), 40.0f);
      i += 1;
    }
    if (logic_or(logic_or(logic_or(self.replan_, extra_moved), math_compare<">", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(focus).x, vector_break(self.planned_focus_).x, vector_break(focus).y, vector_break(self.planned_focus_).y), 40.0f)), math_compare<">", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, vector_break(anchor).x, vector_break(self.planned_anchor_).x, vector_break(anchor).y, vector_break(self.planned_anchor_).y), 40.0f))) {
      Terrain_want_tiles(self, focus, anchor);
    }
    under = list_make<Vector, "2">(focus, anchor);
    place = list_length<Vector>(under);
    list_insert_list(under, place, self.extra);
    for (auto it184 = flow_for(n184, 0, math_sub<i64>(list_length<Vector>(under), 1)); it184.next();) {
      ci = kern::formula<f32, f32>([](auto x, auto kTile) { return num_cast<i32, f32>(floor((x / kTile))); }, vector_break(under[n184.index]).x, kTile);
      cj = kern::formula<f32, f32>([](auto y, auto kTile) { return num_cast<i32, f32>(floor((y / kTile))); }, vector_break(under[n184.index]).y, kTile);
      for (auto it185 = flow_for(n185, num_cast<i64, i32>(kern::formula<i32>([](auto cj) { return (cj - 1); }, cj)), num_cast<i64, i32>(kern::formula<i32>([](auto cj) { return (cj + 1); }, cj))); it185.next();) {
        for (auto it186 = flow_for(n186, num_cast<i64, i32>(kern::formula<i32>([](auto ci) { return (ci - 1); }, ci)), num_cast<i64, i32>(kern::formula<i32>([](auto ci) { return (ci + 1); }, ci))); it186.next();) {
          skip = false;
          k = key2(num_cast<i32, i64>(n186.index), num_cast<i32, i64>(n185.index)).Result;
          if (logic_and(num_cast<bool, i64>(num_cast<i64, bool>(map_contains<u64, Terrain_CollisionTile>(self.collision_, k))), logic_not(num_cast<bool, i64>(num_cast<i64, bool>(set_contains<u64>(self.dirty_collision_, k)))))) {
            skip = true;
          }
          if (logic_not(skip)) {
            Terrain_build_collision_tile(self, num_cast<i32, i64>(n186.index), num_cast<i32, i64>(n185.index));
            key = k;
            set_remove(n187, self.dirty_collision_, key);
            self.stats_.rebuilds = kern::formula<u32>([](auto rebuilds) { return (rebuilds + 1); }, self.stats_.rebuilds);
          }
        }
      }
    }
    missing = false;
    built = 0;
    count = list_length<u64>(self.want_collision_);
    at = 0;
    stop = false;
    while (loop(logic_and(logic_not(stop), math_compare<"<", i64>(at, count)))) {
      k_2 = self.want_collision_[at];
      skip_2 = false;
      if (logic_and(num_cast<bool, i64>(num_cast<i64, bool>(map_contains<u64, Terrain_CollisionTile>(self.collision_, k_2))), logic_not(num_cast<bool, i64>(num_cast<i64, bool>(set_contains<u64>(self.dirty_collision_, k_2)))))) {
        skip_2 = true;
      }
      if (logic_not(logic_or(stop, skip_2))) {
        if (math_compare<">=", u32>(built, more)) {
          missing = true;
          stop = true;
        }
        if (logic_not(logic_or(stop, skip_2))) {
          Terrain_build_collision_tile(self, kern::formula<u64>([](auto k_2) { return num_cast<i32, u64>((k_2 >> 32)); }, k_2), kern::formula<u64>([](auto k_2) { return num_cast<i32, u64>((k_2 & 4294967295)); }, k_2));
          key_2 = k_2;
          set_remove(n188, self.dirty_collision_, key_2);
          self.stats_.rebuilds = kern::formula<u32>([](auto rebuilds) { return (rebuilds + 1); }, self.stats_.rebuilds);
          built += 1;
        }
      }
      if (logic_not(stop)) {
        at += 1;
      }
    }
    if (logic_and(logic_not(missing), math_compare<">", u64>(num_cast<u64, i64>(map_length<u64, Terrain_CollisionTile>(self.collision_)), num_cast<u64, i64>(list_length<u64>(self.want_collision_))))) {
      made = Set<u64>{};
      first = 0;
      last = math_sub<i64>(list_length<u64>(self.want_collision_), 1);
      for (auto it190 = flow_for(n190, first, last); it190.next();) {
        set_add(n189, made, list_get<u64>(self.want_collision_, n190.index));
      }
      keep = made;
      it = 0;
      while (loop(logic_not(math_compare<"==", i64>(it, map_length<u64, Terrain_CollisionTile>(self.collision_))))) {
        if (logic_not(num_cast<bool, i64>(num_cast<i64, bool>(set_contains<u64>(keep, map_key_at<u64, Terrain_CollisionTile>(self.collision_, it)))))) {
          valid = engine_body_valid(self.collision_[it].body);
          both = valid;
          if (both) {
            body_valid_2 = engine_world_body_valid(self.world_, self.collision_[it].body);
            both = body_valid_2;
          }
          if (both) {
            engine_world_destroy_body(self.world_, self.collision_[it].body);
          }
          key_3 = map_key_at<u64, Terrain_CollisionTile>(self.collision_, it);
          set_remove(n191, self.dirty_collision_, key_3);
          at_2 = it;
          map_remove_at(self.collision_, at_2);
          it = at_2;
        } else {
          it += 1;
        }
      }
    }
    self.stats_.collision_tiles = num_cast<u32, i64>(map_length<u64, Terrain_CollisionTile>(self.collision_));
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
  struct : kern::Node<11> { i64 index = 0; } n11;
  struct : kern::Node<12> { i64 index = 0; } n12;
  struct : kern::Node<13> { i64 index = 0; } n13;
  struct : kern::Node<14> { i64 index = 0; } n14;
  struct : kern::Node<15> { i64 index = 0; } n15;
  struct : kern::Node<16> { i64 index = 0; } n16;
  struct : kern::Node<17> { i64 index = 0; } n17;
  struct : kern::Node<18> { i64 index = 0; } n18;
  struct { bool added = false; i64 index = 0; } n19;
  struct { bool result = false; GeoProjector self = GeoProjector{0.0, 0.0, 1.0}; } n20;
  struct : kern::Node<21> { i64 index = 0; } n21;
  struct : kern::Node<22> { i64 index = 0; } n22;
  struct : kern::Node<23> { i64 index = 0; } n23;
  struct { bool result = false; } n24;
  struct : kern::Node<25> { i64 index = 0; } n25;
  struct { bool result = false; } n26;
  struct : kern::Node<27> { i64 index = 0; } n27;
  struct : kern::Node<28> { i64 index = 0; } n28;
  struct : kern::Node<29> { i64 index = 0; } n29;
  struct : kern::Node<30> { i64 index = 0; } n30;
  struct : kern::Node<31> { i64 index = 0; } n31;
  struct : kern::Node<32> { i64 index = 0; } n32;
  struct : kern::Node<33> { i64 index = 0; } n33;
  struct { bool added = false; i64 index = 0; } n34;
  struct : kern::Node<35> { i64 index = 0; } n35;
  Terrain_plan_demResult n36;
  struct : kern::Node<37> { i64 index = 0; } n37;
  Terrain_plan_windows_matchResult n38;
  struct { Terrain_Grid value = Terrain_Grid{0.0f, 0.0f, 10.0f, 0, List<f32>{}, 0.0f, 0.0f}; } n39;
  struct : kern::Node<40> { i64 index = 0; } n40;
  Terrain_plan_demResult n41;
  struct : kern::Node<42> { i64 index = 0; } n42;
  Terrain_run_job_mineResult n43;
  struct : kern::Node<44> { i64 index = 0; } n44;
  Terrain_run_job_mineResult n45;
  struct { u32 result = 0; } n46;
  struct { bool result = false; List<u8> out = List<u8>{}; } n47;
  struct : kern::Node<48> { i64 index = 0; } n48;
  struct { bool added = false; i64 index = 0; } n49;
  struct : kern::Node<50> { i64 index = 0; } n50;
  struct { bool added = false; i64 index = 0; } n51;
  struct : kern::Node<52> { i64 index = 0; } n52;
  struct { bool added = false; i64 index = 0; } n53;
  struct { Terrain_Grid value = Terrain_Grid{0.0f, 0.0f, 10.0f, 0, List<f32>{}, 0.0f, 0.0f}; } n54;
  struct : kern::Node<55> { i64 index = 0; } n55;
  struct : kern::Node<56> { i64 index = 0; } n56;
  struct : kern::Node<57> { i64 index = 0; } n57;
  struct : kern::Node<58> { i64 index = 0; } n58;
  struct : kern::Node<59> { i64 index = 0; } n59;
  struct : kern::Node<60> { i64 index = 0; } n60;
  struct : kern::Node<61> { i64 index = 0; } n61;
  struct : kern::Node<62> { i64 index = 0; } n62;
  struct { bool added = false; i64 index = 0; } n63;
  struct { List<Terrain_CorridorSet> value = List<Terrain_CorridorSet>{}; } n64;
  struct : kern::Node<65> { i64 index = 0; } n65;
  struct : kern::Node<66> { i64 index = 0; } n66;
  struct : kern::Node<67> { i64 index = 0; } n67;
  struct : kern::Node<68> { i64 index = 0; } n68;
  struct : kern::Node<69> { i64 index = 0; } n69;
  struct { bool added = false; i64 index = 0; } n70;
  struct : kern::Node<71> { i64 index = 0; } n71;
  struct : kern::Node<72> { i64 index = 0; } n72;
  struct : kern::Node<73> { i64 index = 0; } n73;
  struct : kern::Node<74> { i64 index = 0; } n74;
  struct : kern::Node<75> { i64 index = 0; } n75;
  struct : kern::Node<76> { i64 index = 0; } n76;
  struct : kern::Node<77> { i64 index = 0; } n77;
  struct : kern::Node<78> { i64 index = 0; } n78;
  struct { f64 x = 0.0; f64 y = 0.0; } n79;
  struct { f64 x = 0.0; f64 y = 0.0; } n80;
  struct : kern::Node<81> { i64 index = 0; } n81;
  struct : kern::Node<82> { i64 index = 0; } n82;
  struct { bool added = false; i64 index = 0; } n83;
  struct : kern::Node<84> { i64 index = 0; } n84;
  struct { bool added = false; i64 index = 0; } n85;
  Terrain_image_boundsResult n86;
  struct : kern::Node<87> { i64 index = 0; } n87;
  struct : kern::Node<88> { i64 index = 0; } n88;
  struct : kern::Node<89> { i64 index = 0; } n89;
  struct : kern::Node<90> { i64 index = 0; } n90;
  struct : kern::Node<91> { i64 index = 0; } n91;
  struct : kern::Node<92> { i64 index = 0; } n92;
  struct : kern::Node<93> { i64 index = 0; } n93;
  struct : kern::Node<94> { i64 index = 0; } n94;
  struct : kern::Node<95> { i64 index = 0; } n95;
  struct : kern::Node<96> { i64 index = 0; } n96;
  Terrain_image_boundsResult n97;
  struct : kern::Node<98> { i64 index = 0; } n98;
  struct : kern::Node<99> { i64 index = 0; } n99;
  struct : kern::Node<100> { i64 index = 0; } n100;
  struct : kern::Node<101> { i64 index = 0; } n101;
  struct : kern::Node<102> { i64 index = 0; } n102;
  struct : kern::Node<103> { i64 index = 0; } n103;
  struct : kern::Node<104> { i64 index = 0; } n104;
  struct : kern::Node<105> { i64 index = 0; } n105;
  struct : kern::Node<106> { i64 index = 0; } n106;
  struct : kern::Node<107> { i64 index = 0; } n107;
  struct : kern::Node<108> { i64 index = 0; } n108;
  struct : kern::Node<109> { i64 index = 0; } n109;
  struct : kern::Node<110> { i64 index = 0; } n110;
  struct : kern::Node<111> { i64 index = 0; } n111;
  struct { bool added = false; i64 index = 0; } n112;
  struct : kern::Node<113> { i64 index = 0; } n113;
  struct : kern::Node<114> { i64 index = 0; } n114;
  struct : kern::Node<115> { i64 index = 0; } n115;
  struct : kern::Node<116> { i64 index = 0; } n116;
  struct : kern::Node<117> { i64 index = 0; } n117;
  struct : kern::Node<118> { i64 index = 0; } n118;
  Terrain_ground_albedoResult n119;
  struct : kern::Node<120> { i64 index = 0; } n120;
  struct : kern::Node<121> { i64 index = 0; } n121;
  struct : kern::Node<122> { i64 index = 0; } n122;
  struct : kern::Node<123> { i64 index = 0; } n123;
  struct : kern::Node<124> { i64 index = 0; } n124;
  struct : kern::Node<125> { i64 index = 0; } n125;
  struct { MeshData self = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n126;
  struct : kern::Node<127> { i64 index = 0; } n127;
  struct : kern::Node<128> { i64 index = 0; } n128;
  struct : kern::Node<129> { i64 index = 0; } n129;
  struct : kern::Node<130> { i64 index = 0; } n130;
  struct { MeshData self = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n131;
  struct { u32 result = 0; } n132;
  struct { bool result = false; } n133;
  struct { bool added = false; i64 index = 0; } n134;
  struct { bool result = false; } n135;
  struct : kern::Node<136> { i64 index = 0; } n136;
  struct : kern::Node<137> { i64 index = 0; } n137;
  struct { bool added = false; i64 index = 0; } n138;
  struct { physics_Body result = physics_Body{0}; } n139;
  struct { physics_Shape result = physics_Shape{0}; } n140;
  struct : kern::Node<141> { i64 index = 0; } n141;
  struct { MeshData self = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n142;
  struct { MeshData self = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n143;
  struct : kern::Node<144> { i64 index = 0; } n144;
  Terrain_build_water_chunk_wetResult n145;
  Terrain_build_water_chunk_wetResult n147;
  struct : kern::Node<148> { i64 index = 0; } n148;
  struct : kern::Node<149> { i64 index = 0; } n149;
  Terrain_build_water_chunk_wetResult n150;
  struct : kern::Node<151> { i64 index = 0; } n151;
  struct : kern::Node<152> { i64 index = 0; } n152;
  struct : kern::Node<153> { i64 index = 0; } n153;
  struct { MeshData self = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n154;
  struct { MeshData self = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; } n155;
  struct : kern::Node<156> { i64 index = 0; } n156;
  struct : kern::Node<157> { i64 index = 0; } n157;
  Terrain_build_water_chunk_vtxResult n158;
  Terrain_build_water_chunk_vtxResult n159;
  Terrain_build_water_chunk_vtxResult n160;
  Terrain_build_water_chunk_vtxResult n161;
  struct { bool added = false; i64 index = 0; } n162;
  struct { bool added = false; i64 index = 0; } n163;
  struct { bool result = false; } n164;
  struct { u32 result = 0; } n165;
  struct { bool result = false; } n166;
  struct { bool result = false; } n167;
  struct : kern::Node<168> { i64 index = 0; } n168;
  struct : kern::Node<169> { i64 index = 0; } n169;
  Terrain_run_jobResult n170;
  struct { bool removed = false; } n171;
  struct { bool added = false; i64 index = 0; } n172;
  struct { bool removed = false; } n173;
  struct { bool added = false; i64 index = 0; } n174;
  struct : kern::Node<175> { i64 index = 0; } n175;
  struct { bool result = false; } n176;
  struct { bool removed = false; } n177;
  struct { bool added = false; i64 index = 0; } n178;
  struct : kern::Node<179> { i64 index = 0; } n179;
  struct { bool result = false; } n180;
  struct { bool removed = false; } n181;
  struct { bool removed = false; } n182;
  struct : kern::Node<183> { i64 index = 0; } n183;
  struct : kern::Node<184> { i64 index = 0; } n184;
  struct : kern::Node<185> { i64 index = 0; } n185;
  struct : kern::Node<186> { i64 index = 0; } n186;
  struct { bool removed = false; } n187;
  struct { bool removed = false; } n188;
  struct { bool added = false; i64 index = 0; } n189;
  struct : kern::Node<190> { i64 index = 0; } n190;
  struct { bool removed = false; } n191;
};
