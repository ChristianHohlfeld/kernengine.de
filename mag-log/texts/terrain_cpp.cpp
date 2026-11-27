struct terrain_cpp : virtual kern::Library, virtual common, virtual engine_math_math, virtual terrain, virtual tiles {
  // ---- types ----
  KERN_STRUCT(WantedTile) { f32 d = 0.0f; u64 k = 0; };

  // ---- variables ----
  /// the bare-earth filter's reach from a window's edge (open 160 m + blur): inside it two windows agree
  KERN_CONST f32 kDemMargin = 300.0f;
  /// terrain_craters.mjs WATER_LEVEL_M
  KERN_CONST f32 kWaterLevel = -0.18f;
  KERN_CONST f64 kPiD = 3.141592653589793;
  /// see README: the city ring is ~5 km, bare earth is smooth below 160 m
  KERN_CONST i32 kDemZoom = 13;
  /// terrain_craters.mjs TERRAIN_FIELD_STEP_M
  KERN_CONST f32 kStep = 5.0f;
  /// far render tile
  KERN_CONST f32 kBlock = 640.0f;
  /// collision / near render tile (32 cells of 5 m: terrain_tiles.mjs TILE_CELLS)
  KERN_CONST f32 kTile = 160.0f;
  KERN_CONST f32 kWaterChunk = 320.0f;
  /// world_water.mjs CELL
  KERN_CONST f32 kWaterCell = 4.0f;
  /// 16 x 16 water cells
  KERN_CONST f32 kWaterBlock = 64.0f;

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
    for (auto it32 = flow_for(n32, -2, 2); it32.next();) {
      for (auto it33 = flow_for(n33, -2, 2); it33.next();) {
        w = kern::formula<i64, i64>([](auto index, auto index_2) { return num_cast<f32, i32>(((3 - abs(num_cast<i32, i64>(index))) * (3 - abs(num_cast<i32, i64>(index_2))))); }, n33.index, n32.index);
        sum += kern::formula<f32, f32>([](auto the_result, auto w) { return (the_result * w); }, Terrain_elevation(self, kern::formula<f32, i64>([](auto x, auto index) { return (x + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * 9.0f)); }, x, n33.index), kern::formula<f32, i64>([](auto y, auto index) { return (y + (num_cast<f32, i32>(num_cast<i32, i64>(index)) * 9.0f)); }, y, n32.index)).Result, w);
        wsum += w;
      }
    }
    return {kern::formula<f32, f32>([](auto sum, auto wsum) { return (sum / wsum); }, sum, wsum)};
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
    for (auto it34 = flow_for(n34, 0, math_sub<i64>(map_length<u64, Terrain_RenderTile>(self.render_), 1)); it34.next();) {
      level = 0;
      i = 0;
      j = 0;
      unkey3(level, i, j, map_key_at<u64, Terrain_RenderTile>(self.render_, n34.index));
      size = kern::formula<i32, f32, f32>([](auto level, auto kBlock, auto kTile) { return ((level == 2) ? kBlock : kTile); }, level, kBlock, kTile);
      if (logic_and(logic_and(logic_and(math_compare<"<=", f32>(kern::formula<i32, f32>([](auto i, auto size) { return (num_cast<f32, i32>(i) * size); }, i, size), x1_), math_compare<">=", f32>(kern::formula<i32, f32>([](auto i, auto size) { return (num_cast<f32, i32>((i + 1)) * size); }, i, size), x0_)), math_compare<"<=", f32>(kern::formula<i32, f32>([](auto j, auto size) { return (num_cast<f32, i32>(j) * size); }, j, size), y1_)), math_compare<">=", f32>(kern::formula<i32, f32>([](auto j, auto size) { return (num_cast<f32, i32>((j + 1)) * size); }, j, size), y0_))) {
        key = map_key_at<u64, Terrain_RenderTile>(self.render_, n34.index);
        set_add(n35, self.dirty_render_, key);
      }
    }
    for (auto it36 = flow_for(n36, 0, math_sub<i64>(map_length<u64, Terrain_CollisionTile>(self.collision_), 1)); it36.next();) {
      i_2 = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a >> 32)); }, map_key_at<u64, Terrain_CollisionTile>(self.collision_, n36.index));
      j_2 = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a & 4294967295)); }, map_key_at<u64, Terrain_CollisionTile>(self.collision_, n36.index));
      if (logic_and(logic_and(logic_and(math_compare<"<=", f32>(kern::formula<i32, f32, f32>([](auto i_2, auto kTile, auto kStep) { return ((num_cast<f32, i32>(i_2) * kTile) - kStep); }, i_2, kTile, kStep), x1_), math_compare<">=", f32>(kern::formula<i32, f32, f32>([](auto i_2, auto kTile, auto kStep) { return ((num_cast<f32, i32>((i_2 + 1)) * kTile) + kStep); }, i_2, kTile, kStep), x0_)), math_compare<"<=", f32>(kern::formula<i32, f32, f32>([](auto j_2, auto kTile, auto kStep) { return ((num_cast<f32, i32>(j_2) * kTile) - kStep); }, j_2, kTile, kStep), y1_)), math_compare<">=", f32>(kern::formula<i32, f32, f32>([](auto j_2, auto kTile, auto kStep) { return ((num_cast<f32, i32>((j_2 + 1)) * kTile) + kStep); }, j_2, kTile, kStep), y0_))) {
        key_2 = map_key_at<u64, Terrain_CollisionTile>(self.collision_, n36.index);
        set_add(n37, self.dirty_collision_, key_2);
      }
    }
    for (auto it38 = flow_for(n38, 0, math_sub<i64>(map_length<u64, u32>(self.water_meshes_), 1)); it38.next();) {
      i_3 = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a >> 32)); }, map_key_at<u64, u32>(self.water_meshes_, n38.index));
      j_3 = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a & 4294967295)); }, map_key_at<u64, u32>(self.water_meshes_, n38.index));
      if (logic_and(logic_and(logic_and(math_compare<"<=", f32>(kern::formula<i32, f32>([](auto i_3, auto kWaterChunk) { return (num_cast<f32, i32>(i_3) * kWaterChunk); }, i_3, kWaterChunk), x1_), math_compare<">=", f32>(kern::formula<i32, f32>([](auto i_3, auto kWaterChunk) { return (num_cast<f32, i32>((i_3 + 1)) * kWaterChunk); }, i_3, kWaterChunk), x0_)), math_compare<"<=", f32>(kern::formula<i32, f32>([](auto j_3, auto kWaterChunk) { return (num_cast<f32, i32>(j_3) * kWaterChunk); }, j_3, kWaterChunk), y1_)), math_compare<">=", f32>(kern::formula<i32, f32>([](auto j_3, auto kWaterChunk) { return (num_cast<f32, i32>((j_3 + 1)) * kWaterChunk); }, j_3, kWaterChunk), y0_))) {
        key_3 = map_key_at<u64, u32>(self.water_meshes_, n38.index);
        set_add(n39, self.dirty_water_, key_3);
      }
    }
    if (notify) {
      list_add(self.changes_, engine_math_math::Vec4_new(x0_, y0_, x1_, y1_).Result);
    }
    self.replan_ = true;
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

  struct Terrain_corridor_setResult { i32 Result = 0; };
  KERN_PURE Terrain_corridor_setResult Terrain_corridor_set(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    for (auto it41 = flow_for(n41, 0, math_sub<i64>(list_length<Terrain_CorridorSet_row>(self.corridors_), 1)); it41.next();) {
      if (logic_and(logic_and(logic_and(math_compare<">=", f32>(x, self.corridors_[n41.index].items[0].bounds.x), math_compare<"<", f32>(x, self.corridors_[n41.index].items[0].bounds.z)), math_compare<">=", f32>(y, self.corridors_[n41.index].items[0].bounds.y)), math_compare<"<", f32>(y, self.corridors_[n41.index].items[0].bounds.w))) {
        return {num_cast<i32, i64>(n41.index)};
      }
    }
    return {-1};
  }

  struct Terrain_corridors_atResult { bool Result = false; };
  KERN_PURE Terrain_corridors_atResult Terrain_corridors_at(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f) {
    return {logic_or(logic_not(self.corridors_expected), math_compare<">=", i32>(Terrain_corridor_set(self, x, y).Result, 0))};
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

  void Terrain_set_water_cell(Terrain& self, i32 cx = 0, i32 cy = 0) {
    u32 idx = 0;
    idx = kern::formula<i32, i32>([](auto cy, auto cx) { return num_cast<u32, i32>((((cy & 15) * 16) + (cx & 15))); }, cy, cx);
    if (logic_not(kern::formula<u64, u32>([](auto a, auto idx) { return num_cast<bool, u64>(((a >> (idx & 63)) & 1)); }, self.water_[b_row].bits[num_cast<i64, u32>(kern::formula<u32>([](auto idx) { return (idx >> 6); }, idx))], idx))) {
      self.stats_.water_cells = kern::formula<u32>([](auto water_cells) { return (water_cells + 1); }, self.stats_.water_cells);
    }
    self.water_[b_row].bits[num_cast<i64, u32>(kern::formula<u32>([](auto idx) { return (idx >> 6); }, idx))] = kern::formula<u64, u32>([](auto a, auto idx) { return (a | (1 << (idx & 63))); }, self.water_[b_row].bits[num_cast<i64, u32>(kern::formula<u32>([](auto idx) { return (idx >> 6); }, idx))], idx);
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
    for (auto it45 = flow_for(n45, 0, math_sub<i64>(list_length<Vector>(rings[0].items), 1)); it45.next();) {
      p = rings[0].items[n45.index];
      y_lo = kern::formula<f32, f32>([](auto y_lo, auto y) { return fmin(y_lo, y); }, y_lo, vector_break(p).y);
      y_hi = kern::formula<f32, f32>([](auto y_hi, auto y) { return fmax(y_hi, y); }, y_hi, vector_break(p).y);
      x_lo = kern::formula<f32, f32>([](auto x_lo, auto x) { return fmin(x_lo, x); }, x_lo, vector_break(p).x);
      x_hi = kern::formula<f32, f32>([](auto x_hi, auto x) { return fmax(x_hi, x); }, x_hi, vector_break(p).x);
    }
    xs = List<f32>{};
    j0 = kern::formula<f32, f32>([](auto y_lo, auto kWaterCell) { return num_cast<i32, f32>(floor((y_lo / kWaterCell))); }, y_lo, kWaterCell);
    j1 = kern::formula<f32, f32>([](auto y_hi, auto kWaterCell) { return num_cast<i32, f32>(floor((y_hi / kWaterCell))); }, y_hi, kWaterCell);
    for (auto it46 = flow_for(n46, num_cast<i64, i32>(j0), num_cast<i64, i32>(j1)); it46.next();) {
      y = kern::formula<i64, f32>([](auto index, auto kWaterCell) { return ((num_cast<f32, i32>(num_cast<i32, i64>(index)) + 0.5f) * kWaterCell); }, n46.index, kWaterCell);
      list_clear(xs);
      for (auto it47 = flow_for(n47, 0, math_sub<i64>(list_length<Vector_row>(rings), 1)); it47.next();) {
        a = 0;
        c = kern::formula<u64>([](auto a) { return (a - 1); }, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(rings[n47.index]).items)));
        while (loop(math_compare<"<", u64>(a, num_cast<u64, i64>(list_length<Vector>(struct_break<Vector_row>(rings[n47.index]).items))))) {
          ya = vector_break(rings[n47.index].items[num_cast<i64, u64>(a)]).y;
          yc = vector_break(rings[n47.index].items[num_cast<i64, u64>(c)]).y;
          if (math_compare<"!=", i32>(num_cast<i32, bool>(math_compare<">", f32>(ya, y)), num_cast<i32, bool>(math_compare<">", f32>(yc, y)))) {
            list_add(xs, kern::formula<f32, f32, f32, f32, f32>([](auto x, auto y, auto ya, auto yc, auto x_2) { return (x + (((y - ya) / (yc - ya)) * (x_2 - x))); }, vector_break(rings[n47.index].items[num_cast<i64, u64>(a)]).x, y, ya, yc, vector_break(rings[n47.index].items[num_cast<i64, u64>(c)]).x));
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
        for (auto it48 = flow_for(n48, num_cast<i64, i32>(ia), num_cast<i64, i32>(ib)); it48.next();) {
          Terrain_set_water_cell(self, num_cast<i32, i64>(n48.index), num_cast<i32, i64>(n46.index));
        }
        k += 2;
      }
    }
    Terrain_dirty(self, x_lo, y_lo, x_hi, y_hi, true);
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

  struct Terrain_take_image_requestsResult { Text Result = ""; };
  Terrain_take_image_requestsResult Terrain_take_image_requests(Terrain& self) {
    Text s = "";
    Text b = "";
    s = "[";
    for (auto it49 = flow_for(n49, 0, math_sub<i64>(list_length<TileKey>(self.image_requests_), 1)); it49.next();) {
      b = "";
      b = text_format<"%s[%d,%d,%d]">(63, logic_select<Text>(num_cast<bool, u64>(num_cast<u64, i64>(n49.index)), ",", ""), num_cast<i64, i32>(self.image_requests_[n49.index].z), num_cast<i64, i32>(self.image_requests_[n49.index].x), num_cast<i64, i32>(self.image_requests_[n49.index].y));
      s = text_append(s, b, "");
    }
    list_clear(self.image_requests_);
    return {text_append(s, "]", "")};
  }

  void Terrain_fail_image(Terrain& self, const TileKey& key) {
    i64 it = 0;
    it = kern::formula<i64, i64>([](auto at, auto count) { return at < 0 ? count : at; }, map_find<u64, Terrain_Image>(self.images_, tiles::TileKey_hash(key).Result).index, map_length<u64, Terrain_Image>(self.images_));
    if (logic_not(math_compare<"==", i64>(it, map_length<u64, Terrain_Image>(self.images_)))) {
      self.images_[it].failed = true;
    }
  }

  void Terrain_repaint(Terrain& self, const Vec4& b) {
    i32 level = 0;
    i32 i = 0;
    i32 j = 0;
    f32 size = 0.0f;
    u64 key = 0;
    for (auto it60 = flow_for(n60, 0, math_sub<i64>(map_length<u64, Terrain_RenderTile>(self.render_), 1)); it60.next();) {
      level = 0;
      i = 0;
      j = 0;
      unkey3(level, i, j, map_key_at<u64, Terrain_RenderTile>(self.render_, n60.index));
      size = kern::formula<i32, f32, f32>([](auto level, auto kBlock, auto kTile) { return ((level == 2) ? kBlock : kTile); }, level, kBlock, kTile);
      if (logic_and(logic_and(logic_and(math_compare<"<=", f32>(kern::formula<i32, f32>([](auto i, auto size) { return (num_cast<f32, i32>(i) * size); }, i, size), b.z), math_compare<">=", f32>(kern::formula<i32, f32>([](auto i, auto size) { return (num_cast<f32, i32>((i + 1)) * size); }, i, size), b.x)), math_compare<"<=", f32>(kern::formula<i32, f32>([](auto j, auto size) { return (num_cast<f32, i32>(j) * size); }, j, size), b.w)), math_compare<">=", f32>(kern::formula<i32, f32>([](auto j, auto size) { return (num_cast<f32, i32>((j + 1)) * size); }, j, size), b.y))) {
        key = map_key_at<u64, Terrain_RenderTile>(self.render_, n60.index);
        set_add(n61, self.dirty_render_, key);
      }
    }
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
    for (auto it62 = flow_for(n62, 0, math_sub<i64>(list_length<Terrain_Crater>(self.craters_), 1)); it62.next();) {
      list_add(out, struct_make<Terrain_CraterRec>(vector_break(self.craters_[n62.index].c).x, vector_break(self.craters_[n62.index].c).y, self.craters_[n62.index].r, self.craters_[n62.index].depth));
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
    for (auto it63 = flow_for(n63, 0, math_sub<i64>(list_length<Terrain_CraterRec>(mine), 1)); it63.next();) {
      if (logic_not(set_craters_has(list, mine[n63.index]).Result)) {
        Terrain_dirty(self, kern::formula<f32, f32>([](auto a, auto a_2) { return (a - a_2); }, mine[n63.index].x, mine[n63.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a - a_2); }, mine[n63.index].y, mine[n63.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a + a_2); }, mine[n63.index].x, mine[n63.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a + a_2); }, mine[n63.index].y, mine[n63.index].r), true);
      }
    }
    for (auto it64 = flow_for(n64, 0, math_sub<i64>(list_length<Terrain_CraterRec>(list), 1)); it64.next();) {
      if (logic_not(set_craters_has(mine, list[n64.index]).Result)) {
        Terrain_dirty(self, kern::formula<f32, f32>([](auto a, auto a_2) { return (a - a_2); }, list[n64.index].x, list[n64.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a - a_2); }, list[n64.index].y, list[n64.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a + a_2); }, list[n64.index].x, list[n64.index].r), kern::formula<f32, f32>([](auto a, auto a_2) { return (a + a_2); }, list[n64.index].y, list[n64.index].r), true);
      }
    }
    list_clear(self.craters_);
    for (auto it65 = flow_for(n65, 0, math_sub<i64>(list_length<Terrain_CraterRec>(list), 1)); it65.next();) {
      if (logic_and(logic_and(logic_and(math_compare<">", f32>(list[n65.index].r, 0.5f), kern::formula<f32>([](auto a) { return isfinite(a); }, list[n65.index].x)), kern::formula<f32>([](auto a) { return isfinite(a); }, list[n65.index].y)), kern::formula<f32>([](auto a) { return isfinite(a); }, list[n65.index].depth))) {
        list_add(self.craters_, struct_make<Terrain_Crater>(vector_make(list[n65.index].x, list[n65.index].y, 0.0f), list[n65.index].r, list[n65.index].depth, kern::formula<f32>([](auto a) { return ((a * 7.0f) / 22.0f); }, list[n65.index].depth)));
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
    for (auto it77 = flow_for(n77, 0, math_sub<i64>(list_length<WantedTile>(v), 1)); it77.next();) {
      at = 0;
      end = list_length<u64>(out);
      go = math_compare<"<", i64>(at, end);
      while (loop(go)) {
        if (math_compare<"==", u64>(out[at], v[n77.index].k)) {
          go = false;
        } else {
          at += 1;
          go = math_compare<"<", i64>(at, end);
        }
      }
      if (math_compare<"==", i64>(at, list_length<u64>(out))) {
        list_add(out, v[n77.index].k);
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
    for (auto it66 = flow_for(n66, num_cast<i64, i32>(kern::formula<i32>([](auto bj) { return (bj - 5); }, bj)), num_cast<i64, i32>(kern::formula<i32>([](auto bj) { return (bj + 5); }, bj))); it66.next();) {
      for (auto it67 = flow_for(n67, num_cast<i64, i32>(kern::formula<i32>([](auto bi) { return (bi - 5); }, bi)), num_cast<i64, i32>(kern::formula<i32>([](auto bi) { return (bi + 5); }, bi))); it67.next();) {
        skip = false;
        d = rect_distance(vector_break(focus).x, vector_break(focus).y, kern::formula<i64, f32>([](auto index, auto kBlock) { return (num_cast<f32, i32>(num_cast<i32, i64>(index)) * kBlock); }, n67.index, kBlock), kern::formula<i64, f32>([](auto index, auto kBlock) { return (num_cast<f32, i32>(num_cast<i32, i64>(index)) * kBlock); }, n66.index, kBlock), kern::formula<i64, f32>([](auto index, auto kBlock) { return (num_cast<f32, i32>((num_cast<i32, i64>(index) + 1)) * kBlock); }, n67.index, kBlock), kern::formula<i64, f32>([](auto index, auto kBlock) { return (num_cast<f32, i32>((num_cast<i32, i64>(index) + 1)) * kBlock); }, n66.index, kBlock)).Result;
        if (math_compare<">", f32>(d, 3200.0f)) {
          skip = true;
        }
        if (logic_not(skip)) {
          if (math_compare<"<", f32>(d, 200.0f)) {
            for (auto it68 = flow_for(n68, 0, 3); it68.next();) {
              for (auto it69 = flow_for(n69, 0, 3); it69.next();) {
                ti = kern::formula<i64, i64>([](auto index, auto index_2) { return ((num_cast<i32, i64>(index) * 4) + num_cast<i32, i64>(index_2)); }, n67.index, n69.index);
                tj = kern::formula<i64, i64>([](auto index, auto index_2) { return ((num_cast<i32, i64>(index) * 4) + num_cast<i32, i64>(index_2)); }, n66.index, n68.index);
                td = rect_distance(vector_break(focus).x, vector_break(focus).y, kern::formula<i32, f32>([](auto ti, auto kTile) { return (num_cast<f32, i32>(ti) * kTile); }, ti, kTile), kern::formula<i32, f32>([](auto tj, auto kTile) { return (num_cast<f32, i32>(tj) * kTile); }, tj, kTile), kern::formula<i32, f32>([](auto ti, auto kTile) { return (num_cast<f32, i32>((ti + 1)) * kTile); }, ti, kTile), kern::formula<i32, f32>([](auto tj, auto kTile) { return (num_cast<f32, i32>((tj + 1)) * kTile); }, tj, kTile)).Result;
                list_add(render, struct_make<WantedTile>(td, key3(kern::formula<f32, i32, i32>([](auto td, auto a0, auto a1) { return ((td < 260.0f) ? a0 : a1); }, td, 0, 1), ti, tj).Result));
              }
            }
          } else {
            list_add(render, struct_make<WantedTile>(d, key3(2, num_cast<i32, i64>(n67.index), num_cast<i32, i64>(n66.index)).Result));
          }
        }
      }
    }
    around = list_make<Vector_f32_pair, "2">(struct_make<Vector_f32_pair>(focus, 420.0f), struct_make<Vector_f32_pair>(anchor, 200.0f));
    for (auto it70 = flow_for(n70, 0, math_sub<i64>(list_length<Vector>(self.extra), 1)); it70.next();) {
      list_add(around, struct_make<Vector_f32_pair>(self.extra[n70.index], 300.0f));
    }
    for (auto it71 = flow_for(n71, 0, math_sub<i64>(list_length<Vector_f32_pair>(around), 1)); it71.next();) {
      ci = kern::formula<f32, f32>([](auto x, auto kTile) { return num_cast<i32, f32>(floor((x / kTile))); }, vector_break(around[n71.index].first).x, kTile);
      cj = kern::formula<f32, f32>([](auto y, auto kTile) { return num_cast<i32, f32>(floor((y / kTile))); }, vector_break(around[n71.index].first).y, kTile);
      rr = kern::formula<f32, f32>([](auto a, auto kTile) { return num_cast<i32, f32>(ceil((a / kTile))); }, around[n71.index].second, kTile);
      for (auto it72 = flow_for(n72, num_cast<i64, i32>(kern::formula<i32, i32>([](auto cj, auto rr) { return (cj - rr); }, cj, rr)), num_cast<i64, i32>(kern::formula<i32, i32>([](auto cj, auto rr) { return (cj + rr); }, cj, rr))); it72.next();) {
        for (auto it73 = flow_for(n73, num_cast<i64, i32>(kern::formula<i32, i32>([](auto ci, auto rr) { return (ci - rr); }, ci, rr)), num_cast<i64, i32>(kern::formula<i32, i32>([](auto ci, auto rr) { return (ci + rr); }, ci, rr))); it73.next();) {
          d_2 = rect_distance(vector_break(around[n71.index].first).x, vector_break(around[n71.index].first).y, kern::formula<i64, f32>([](auto index, auto kTile) { return (num_cast<f32, i32>(num_cast<i32, i64>(index)) * kTile); }, n73.index, kTile), kern::formula<i64, f32>([](auto index, auto kTile) { return (num_cast<f32, i32>(num_cast<i32, i64>(index)) * kTile); }, n72.index, kTile), kern::formula<i64, f32>([](auto index, auto kTile) { return (num_cast<f32, i32>((num_cast<i32, i64>(index) + 1)) * kTile); }, n73.index, kTile), kern::formula<i64, f32>([](auto index, auto kTile) { return (num_cast<f32, i32>((num_cast<i32, i64>(index) + 1)) * kTile); }, n72.index, kTile)).Result;
          if (math_compare<"<=", f32>(d_2, around[n71.index].second)) {
            list_add(coll, struct_make<WantedTile>(d_2, key2(num_cast<i32, i64>(n73.index), num_cast<i32, i64>(n72.index)).Result));
          }
        }
      }
    }
    chunks = Set<u64>{};
    for (auto it74 = flow_for(n74, 0, math_sub<i64>(map_length<u64, Terrain_WaterBlock>(self.water_), 1)); it74.next();) {
      bx = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a >> 32)); }, map_key_at<u64, Terrain_WaterBlock>(self.water_, n74.index));
      by = kern::formula<u64>([](auto a) { return num_cast<i32, u64>((a & 4294967295)); }, map_key_at<u64, Terrain_WaterBlock>(self.water_, n74.index));
      ci_2 = kern::formula<i32, f32, f32>([](auto bx, auto kWaterBlock, auto kWaterChunk) { return num_cast<i32, f32>(floor(((num_cast<f32, i32>(bx) * kWaterBlock) / kWaterChunk))); }, bx, kWaterBlock, kWaterChunk);
      cj_2 = kern::formula<i32, f32, f32>([](auto by, auto kWaterBlock, auto kWaterChunk) { return num_cast<i32, f32>(floor(((num_cast<f32, i32>(by) * kWaterBlock) / kWaterChunk))); }, by, kWaterBlock, kWaterChunk);
      key = key2(ci_2, cj_2).Result;
      set_add(n75, chunks, key);
    }
    for (auto it76 = flow_for(n76, 0, math_sub<i64>(set_length<u64>(chunks), 1)); it76.next();) {
      k = set_key_at<u64>(chunks, n76.index);
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
    for (auto it82 = flow_for(n82, num_cast<i64, i32>(kern::formula<i32>([](auto cj) { return (cj - 1); }, cj)), num_cast<i64, i32>(kern::formula<i32>([](auto cj) { return (cj + 1); }, cj))); it82.next();) {
      for (auto it83 = flow_for(n83, num_cast<i64, i32>(kern::formula<i32>([](auto ci) { return (ci - 1); }, ci)), num_cast<i64, i32>(kern::formula<i32>([](auto ci) { return (ci + 1); }, ci))); it83.next();) {
        k = key2(num_cast<i32, i64>(n83.index), num_cast<i32, i64>(n82.index)).Result;
        if (logic_or(logic_not(num_cast<bool, i64>(num_cast<i64, bool>(map_contains<u64, Terrain_CollisionTile>(self.collision_, k)))), num_cast<bool, i64>(num_cast<i64, bool>(set_contains<u64>(self.dirty_collision_, k))))) {
          return {false};
        }
      }
    }
    return {true};
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
  struct : kern::Node<23> { i64 index = 0; } n23;
  struct { bool result = false; } n24;
  struct : kern::Node<25> { i64 index = 0; } n25;
  struct { bool result = false; } n26;
  struct : kern::Node<27> { i64 index = 0; } n27;
  struct : kern::Node<28> { i64 index = 0; } n28;
  struct : kern::Node<29> { i64 index = 0; } n29;
  struct : kern::Node<30> { i64 index = 0; } n30;
  struct : kern::Node<32> { i64 index = 0; } n32;
  struct : kern::Node<33> { i64 index = 0; } n33;
  struct : kern::Node<34> { i64 index = 0; } n34;
  struct { bool added = false; i64 index = 0; } n35;
  struct : kern::Node<36> { i64 index = 0; } n36;
  struct { bool added = false; i64 index = 0; } n37;
  struct : kern::Node<38> { i64 index = 0; } n38;
  struct { bool added = false; i64 index = 0; } n39;
  struct : kern::Node<41> { i64 index = 0; } n41;
  struct : kern::Node<45> { i64 index = 0; } n45;
  struct : kern::Node<46> { i64 index = 0; } n46;
  struct : kern::Node<47> { i64 index = 0; } n47;
  struct : kern::Node<48> { i64 index = 0; } n48;
  struct : kern::Node<49> { i64 index = 0; } n49;
  struct : kern::Node<60> { i64 index = 0; } n60;
  struct { bool added = false; i64 index = 0; } n61;
  struct : kern::Node<62> { i64 index = 0; } n62;
  struct : kern::Node<63> { i64 index = 0; } n63;
  struct : kern::Node<64> { i64 index = 0; } n64;
  struct : kern::Node<65> { i64 index = 0; } n65;
  struct : kern::Node<66> { i64 index = 0; } n66;
  struct : kern::Node<67> { i64 index = 0; } n67;
  struct : kern::Node<68> { i64 index = 0; } n68;
  struct : kern::Node<69> { i64 index = 0; } n69;
  struct : kern::Node<70> { i64 index = 0; } n70;
  struct : kern::Node<71> { i64 index = 0; } n71;
  struct : kern::Node<72> { i64 index = 0; } n72;
  struct : kern::Node<73> { i64 index = 0; } n73;
  struct : kern::Node<74> { i64 index = 0; } n74;
  struct { bool added = false; i64 index = 0; } n75;
  struct : kern::Node<76> { i64 index = 0; } n76;
  struct : kern::Node<77> { i64 index = 0; } n77;
  struct : kern::Node<82> { i64 index = 0; } n82;
  struct : kern::Node<83> { i64 index = 0; } n83;
};
