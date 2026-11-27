struct terrain : virtual kern::Library, virtual engine_geo_roads, virtual engine_physics_physics, virtual tiles {
  // ---- types ----
  KERN_STRUCT(TerrainStats) { u32 windows = 0; u32 dem_tiles = 0; u32 dem_failed = 0; u32 render_tiles = 0; u32 collision_tiles = 0; u32 water_cells = 0; u32 craters = 0; u32 rebuilds = 0; f32 min_height = 0.0f; f32 max_height = 0.0f; bool dem = false; bool settled = false; };
  KERN_STRUCT(Terrain_Bridge) { Vector c = Vector{0.0, 0.0, 0.0}; f32 hl = 0.0f; f32 hw = 0.0f; f32 yaw = 0.0f; };
  KERN_STRUCT(Terrain_CorridorSeg) { f32 ax = 0.0f; f32 ay = 0.0f; f32 dx = 0.0f; f32 dy = 0.0f; f32 l2 = 0.0f; f32 ha = 0.0f; f32 hb = 0.0f; f32 half = 0.0f; };
  KERN_STRUCT(Terrain_Crater) { Vector c = Vector{0.0, 0.0, 0.0}; f32 r = 0.0f; f32 depth = 0.0f; f32 rim = 0.0f; };
  KERN_STRUCT(Terrain_CraterRec) { f32 x = 0.0f; f32 y = 0.0f; f32 r = 0.0f; f32 depth = 0.0f; };
  KERN_STRUCT(Terrain_Grid) { f32 x0 = 0.0f; f32 y0 = 0.0f; f32 step = 10.0f; u32 n = 0; List<f32> h = List<f32>{}; f32 cx = 0.0f; f32 cy = 0.0f; };
  KERN_STRUCT(Terrain_RenderTile) { u32 mesh = 0; bool skirt = true; };
  KERN_STRUCT(Terrain_WaterBlock) { Fixed<u64, 4> bits = List<u64>{}; List<Vector> flow = List<Vector>{}; List<f32> speed = List<f32>{}; };
  KERN_STRUCT(Terrain_CollisionTile) { physics_Body body = physics_Body{0}; List<Vector> pts = List<Vector>{}; };
  KERN_STRUCT(Terrain_CorridorSet) { u64 owner = 0; Vec4 bounds = Vec4{0.0f, 0.0f, 0.0f, 0.0f}; Vec4 box = Vec4{0.0f, 0.0f, 0.0f, 0.0f}; Map<u64, List<Terrain_CorridorSeg>> grid = Map<u64, List<Terrain_CorridorSeg>>{}; };
  KERN_STRUCT(Terrain_DemTile) { TileKey key = TileKey{14, 0, 0}; List<f32> h = List<f32>{}; u32 w = 0; u32 hgt = 0; bool failed = false; bool requested = false; };
  KERN_STRUCT(Terrain_Image) { TileKey key = TileKey{14, 0, 0}; List<f32> albedo = List<f32>{}; bool requested = false; bool failed = false; };
  KERN_STRUCT(Terrain_Job) { Terrain_Grid g = Terrain_Grid{0.0f, 0.0f, 10.0f, 0, List<f32>{}, 0.0f, 0.0f}; u32 id = 0; bool side = false; i32 x0 = 0; i32 y0 = 0; i32 x1 = 0; i32 y1 = 0; };
  KERN_STRUCT(Terrain_Tint) { u64 owner = 0; Vec4 box = Vec4{0.0f, 0.0f, 0.0f, 0.0f}; Vector lin = Vector{0.0, 0.0, 0.0}; List<Vector_row> rings = List<Vector_row>{}; };
  KERN_STRUCT(Terrain_CorridorSet_row) { List<Terrain_CorridorSet> items = List<Terrain_CorridorSet>{}; };
  KERN_STRUCT(Terrain) { List<Vec4> needs = List<Vec4>{}; bool corridors_expected = false; Vector lead = Vector{0.0, 0.0, 0.0}; List<Vector> extra = List<Vector>{}; List<Terrain_CorridorSet_row> corridors_ = List<Terrain_CorridorSet_row>{}; Map<u64, Terrain_Image> images_ = Map<u64, Terrain_Image>{}; List<TileKey> image_requests_ = List<TileKey>{}; Vector image_at_ = Vector{1e+09, 1e+09, 0.0}; Object<kern::Renderer> r_ = Object<kern::Renderer>{}; Object<kern::physics::World> world_ = Object<kern::physics::World>{}; GeoProjector proj_ = GeoProjector{0.0, 0.0, 1.0}; bool dem_ = false; bool settled_ = true; bool grid_ready_ = false; f32 ref_height_ = 0.0f; bool ref_ready_ = false; Terrain_Grid grid_ = Terrain_Grid{0.0f, 0.0f, 10.0f, 0, List<f32>{}, 0.0f, 0.0f}; List<Terrain_Grid> side_ = List<Terrain_Grid>{}; List<Terrain_Job> job_ = List<Terrain_Job>{}; Map<u64, Terrain_DemTile> dem_tiles_ = Map<u64, Terrain_DemTile>{}; List<TileKey> requests_ = List<TileKey>{}; u32 dem_failures_ = 0; f64 retry_at_ = 0.0; Map<u64, Terrain_WaterBlock> water_ = Map<u64, Terrain_WaterBlock>{}; List<Terrain_Bridge> bridges_ = List<Terrain_Bridge>{}; List<Terrain_Crater> craters_ = List<Terrain_Crater>{}; List<Terrain_Tint> tints_ = List<Terrain_Tint>{}; Map<u64, Terrain_RenderTile> render_ = Map<u64, Terrain_RenderTile>{}; Map<u64, Terrain_CollisionTile> collision_ = Map<u64, Terrain_CollisionTile>{}; Map<u64, u32> water_meshes_ = Map<u64, u32>{}; Map<u64, bool> water_detail_ = Map<u64, bool>{}; Set<u64> dirty_render_ = Set<u64>{}; Set<u64> dirty_collision_ = Set<u64>{}; Set<u64> dirty_water_ = Set<u64>{}; List<u64> want_render_ = List<u64>{}; List<u64> want_collision_ = List<u64>{}; List<u64> want_water_ = List<u64>{}; Vector planned_focus_ = Vector{1e+09, 1e+09, 0.0}; Vector planned_anchor_ = Vector{1e+09, 1e+09, 0.0}; List<Vector> planned_extra_ = List<Vector>{}; bool replan_ = true; List<Vec4> changes_ = List<Vec4>{}; TerrainStats stats_ = TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, false, false}; };

  // ---- variables ----
  /// the elevation grid job (kern/core/jobs.h; city jobs are 1, 2)
  KERN_CONST u32 kDemJob = 3;
  /// see README: the city ring is ~5 km, bare earth is smooth below 160 m
  KERN_CONST i32 kDemZoom = 13;
  /// grid spacing of the sampled elevation (m)
  KERN_CONST f32 kDemStep = 10.0f;
  /// the smallest elevation window (m); it grows with the map tiles it must hold
  KERN_CONST f32 kDemSize = 8000.0f;
  /// and the largest (low latitudes: a map tile is 2.4 km wide there)
  KERN_CONST f32 kDemMaxSize = 24000.0f;
  /// the bare-earth filter's reach from a window's edge (open 160 m + blur): inside it two windows agree
  KERN_CONST f32 kDemMargin = 300.0f;
  /// terrain_elevation.mjs OPEN_M, BLUR_M
  KERN_CONST f32 kOpen = 160.0f;
  /// terrain_elevation.mjs OPEN_M, BLUR_M
  KERN_CONST f32 kBlur = 30.0f;
  /// terrain_craters.mjs TERRAIN_FIELD_STEP_M
  KERN_CONST f32 kStep = 5.0f;
  /// terrain_craters.mjs WATER_LEVEL_M
  KERN_CONST f32 kWaterLevel = -0.18f;
  /// terrain_craters.mjs WATER_BED_M
  KERN_CONST f32 kWaterBed = 2.6f;
  /// world_water.mjs CELL
  KERN_CONST f32 kWaterCell = 4.0f;
  /// world_water.mjs WATER_RENDER_Z (above the level)
  KERN_CONST f32 kWaterRenderZ = 0.052f;
  /// terrain_tiles.mjs SKIRT_DROP_M
  KERN_CONST f32 kSkirtDrop = 0.05f;
  /// ---- ground colour (world_ground.mjs): aerial imagery of the 1600 m around the player, one albedo per 5 m node (never shown as a texture); rebuilt after 350 m of travel ----
  KERN_CONST i32 kImageZoom = 15;
  /// ---- ground colour (world_ground.mjs): aerial imagery of the 1600 m around the player, one albedo per 5 m node (never shown as a texture); rebuilt after 350 m of travel ----
  KERN_CONST i32 kImagePixels = 64;

  // ---- functions ----
  struct Terrain_settledResult { bool Result = false; };
  /// The ground's shape is decided (elevation loaded, or flat because there is none).
  KERN_PURE Terrain_settledResult Terrain_settled(const Terrain& self) {
    return {self.settled_};
  }

  struct Terrain_has_demResult { bool Result = false; };
  KERN_PURE Terrain_has_demResult Terrain_has_dem(const Terrain& self) {
    return {self.grid_ready_};
  }

  struct Terrain_deformedResult { bool Result = false; };
  /// Whether a circle (x, y, r) touches a crater (the roads drape finely there: city_stream.cpp).
  KERN_PURE Terrain_deformedResult Terrain_deformed(const Terrain& self, f32 x = 0.0f, f32 y = 0.0f, f32 r = 0.0f) {
    for (auto it10 = flow_for(n10, 0, math_sub<i64>(list_length<Terrain_Crater>(self.craters_), 1)); it10.next();) {
      if (math_compare<"<", f32>(kern::formula<f32, f32, f32, f32>([](auto x, auto x_2, auto y, auto y_2) { return hypot((x - x_2), (y - y_2)); }, x, vector_break(self.craters_[n10.index].c).x, y, vector_break(self.craters_[n10.index].c).y), kern::formula<f32, f32>([](auto a, auto r) { return ((a + r) + 2.5f); }, self.craters_[n10.index].r, r))) {
        return {true};
      }
    }
    return {false};
  }

  struct Terrain_crater_countResult { u32 Result = 0; };
  KERN_PURE Terrain_crater_countResult Terrain_crater_count(const Terrain& self) {
    return {num_cast<u32, i64>(list_length<Terrain_Crater>(self.craters_))};
  }

  struct CraterRec_equalResult { bool Result = false; };
  /// One crater as it goes over the network (destruction sync: game_destruction_sync.cpp).
  KERN_PURE CraterRec_equalResult CraterRec_equal(const Terrain_CraterRec& self, const Terrain_CraterRec& o) {
    return {logic_and(logic_and(logic_and(math_compare<"==", f32>(self.x, o.x), math_compare<"==", f32>(self.y, o.y)), math_compare<"==", f32>(self.r, o.r)), math_compare<"==", f32>(self.depth, o.depth))};
  }

  /// Regions changed since the last call (city roads/areas re-lay themselves on them).
  void Terrain_take_changes(Terrain& self, List<Vec4>& out) {
    i64 place = 0;
    place = list_length<Vec4>(out);
    list_insert_list(out, place, self.changes_);
    list_clear(self.changes_);
  }

  struct terrarium_heightResult { f32 Result = 0.0f; };
  /// Terrarium RGB -> metres.
  KERN_PURE terrarium_heightResult terrarium_height(u8 r = 0, u8 g = 0, u8 b = 0) {
    return {kern::formula<u8, u8, u8>([](auto r, auto g, auto b) { return ((((num_cast<f32, u8>(r) * 256.0f) + num_cast<f32, u8>(g)) + (num_cast<f32, u8>(b) / 256.0f)) - 32768.0f); }, r, g, b)};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { i64 index = 0; } n10;
};
