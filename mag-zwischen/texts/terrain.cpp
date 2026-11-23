struct terrain : virtual kern::Library, virtual engine_geo_roads, virtual engine_physics_physics, virtual tiles {
  // ---- types ----
  KERN_STRUCT(TerrainStats) { U32 windows = 0; U32 dem_tiles = 0; U32 dem_failed = 0; U32 render_tiles = 0; U32 collision_tiles = 0; U32 water_cells = 0; U32 craters = 0; U32 rebuilds = 0; Float min_height = 0.0; Float max_height = 0.0; Bool dem = false; Bool settled = false; };
  KERN_STRUCT(Terrain_Bridge) { Vector c = Vector{0.0, 0.0, 0.0}; Float hl = 0.0; Float hw = 0.0; Float yaw = 0.0; };
  KERN_STRUCT(Terrain_CorridorSeg) { Float ax = 0.0; Float ay = 0.0; Float dx = 0.0; Float dy = 0.0; Float l2 = 0.0; Float ha = 0.0; Float hb = 0.0; Float half = 0.0; };
  KERN_STRUCT(Terrain_Crater) { Vector c = Vector{0.0, 0.0, 0.0}; Float r = 0.0; Float depth = 0.0; Float rim = 0.0; };
  KERN_STRUCT(Terrain_CraterRec) { Float x = 0.0; Float y = 0.0; Float r = 0.0; Float depth = 0.0; };
  KERN_STRUCT(Terrain_Grid) { Float x0 = 0.0; Float y0 = 0.0; Float step = 10.0; U32 n = 0; List<Float> h = List<Float>{}; Float cx = 0.0; Float cy = 0.0; };
  KERN_STRUCT(Terrain_RenderTile) { U32 mesh = 0; Bool skirt = true; };
  KERN_STRUCT(Terrain_WaterBlock) { Fixed<Int64, 4> bits = List<Int64>{}; List<Vector> flow = List<Vector>{}; List<Float> speed = List<Float>{}; };
  KERN_STRUCT(Terrain_CollisionTile) { physics_Body body = physics_Body{Int64{0x0}}; };
  KERN_STRUCT(Terrain_CorridorSet) { Int64 owner = Int64{0x0}; Vec4 bounds = Vec4{0.0, 0.0, 0.0, 0.0}; Vec4 box = Vec4{0.0, 0.0, 0.0, 0.0}; Map<Int64, List<Terrain_CorridorSeg>> grid = Map<Int64, List<Terrain_CorridorSeg>>{}; };
  KERN_STRUCT(Terrain_DemTile) { TileKey key = TileKey{14, 0, 0}; List<Float> h = List<Float>{}; U32 w = 0; U32 hgt = 0; Bool failed = false; Bool requested = false; };
  KERN_STRUCT(Terrain_Image) { TileKey key = TileKey{14, 0, 0}; List<Float> albedo = List<Float>{}; Bool requested = false; Bool failed = false; };
  KERN_STRUCT(Terrain_Job) { Terrain_Grid g = Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}; U32 id = 0; Bool side = false; I32 x0 = 0; I32 y0 = 0; I32 x1 = 0; I32 y1 = 0; };
  KERN_STRUCT(Terrain_Tint) { Int64 owner = Int64{0x0}; Vec4 box = Vec4{0.0, 0.0, 0.0, 0.0}; Vector lin = Vector{0.0, 0.0, 0.0}; List<Vector_row> rings = List<Vector_row>{}; };
  KERN_STRUCT(Terrain_CorridorSet_row) { List<Terrain_CorridorSet> items = List<Terrain_CorridorSet>{}; };
  KERN_STRUCT(Terrain) { List<Vec4> needs = List<Vec4>{}; Bool corridors_expected = false; Vector lead = Vector{0.0, 0.0, 0.0}; List<Vector> extra = List<Vector>{}; List<Terrain_CorridorSet_row> corridors_ = List<Terrain_CorridorSet_row>{}; Map<Int64, Terrain_Image> images_ = Map<Int64, Terrain_Image>{}; List<TileKey> image_requests_ = List<TileKey>{}; Vector image_at_ = Vector{1000000000.0, 1000000000.0, 0.0}; Int r_ = 0; Int world_ = 0; GeoProjector proj_ = GeoProjector{0.0, 0.0, 1.0}; Bool dem_ = false; Bool settled_ = true; Bool grid_ready_ = false; Float ref_height_ = 0.0; Bool ref_ready_ = false; Terrain_Grid grid_ = Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}; List<Terrain_Grid> side_ = List<Terrain_Grid>{}; List<Terrain_Job> job_ = List<Terrain_Job>{}; Map<Int64, Terrain_DemTile> dem_tiles_ = Map<Int64, Terrain_DemTile>{}; List<TileKey> requests_ = List<TileKey>{}; U32 dem_failures_ = 0; Float retry_at_ = 0.0; Map<Int64, Terrain_WaterBlock> water_ = Map<Int64, Terrain_WaterBlock>{}; List<Terrain_Bridge> bridges_ = List<Terrain_Bridge>{}; List<Terrain_Crater> craters_ = List<Terrain_Crater>{}; List<Terrain_Tint> tints_ = List<Terrain_Tint>{}; Map<Int64, Terrain_RenderTile> render_ = Map<Int64, Terrain_RenderTile>{}; Map<Int64, Terrain_CollisionTile> collision_ = Map<Int64, Terrain_CollisionTile>{}; Map<Int64, Int> water_meshes_ = Map<Int64, Int>{}; Map<Int64, Bool> water_detail_ = Map<Int64, Bool>{}; Set<Int64> dirty_render_ = Set<Int64>{}; Set<Int64> dirty_collision_ = Set<Int64>{}; Set<Int64> dirty_water_ = Set<Int64>{}; List<Int64> want_render_ = List<Int64>{}; List<Int64> want_collision_ = List<Int64>{}; List<Int64> want_water_ = List<Int64>{}; Vector planned_focus_ = Vector{1000000000.0, 1000000000.0, 0.0}; Vector planned_anchor_ = Vector{1000000000.0, 1000000000.0, 0.0}; List<Vector> planned_extra_ = List<Vector>{}; Bool replan_ = true; List<Vec4> changes_ = List<Vec4>{}; TerrainStats stats_ = TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false}; };

  // ---- functions ----
  struct Terrain_settledResult { Bool Result = false; };
  /// The ground's shape is decided (elevation loaded, or flat because there is none).
  KERN_PURE Terrain_settledResult Terrain_settled(const Terrain& self) {
    return {self.settled_};
  }

  struct Terrain_has_demResult { Bool Result = false; };
  KERN_PURE Terrain_has_demResult Terrain_has_dem(const Terrain& self) {
    return {self.grid_ready_};
  }

  struct Terrain_deformedResult { Bool Result = false; };
  /// Whether a circle (x, y, r) touches a crater (the roads drape finely there: city_stream.cpp).
  KERN_PURE Terrain_deformedResult Terrain_deformed(const Terrain& self, Float x = 0.0, Float y = 0.0, Float r = 0.0) {
    for (auto it10 = flow_for(n10, 0, kern::to<Int>(kern::formula([](Float a) -> Float { return a - 1.0; }, kern::to<Float>(list_length<Terrain_Crater>(self.craters_))))); it10.next();) {
      if (math_compare<"<">(kern::formula([](Float x, Float x_2, Float y, Float y_2) -> Float { return kern::math::sqrt(((x - x_2) * (x - x_2)) + ((y - y_2) * (y - y_2))); }, x, vector_break(self.craters_[n10.index].c).x, y, vector_break(self.craters_[n10.index].c).y), kern::formula([](Float a, Float r) -> Float { return ((a + r) + 2.5); }, self.craters_[n10.index].r, r))) {
        return {true};
      }
    }
    return {false};
  }

  struct Terrain_crater_countResult { U32 Result = 0; };
  KERN_PURE Terrain_crater_countResult Terrain_crater_count(const Terrain& self) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(list_length<Terrain_Crater>(self.craters_))))};
  }

  struct CraterRec_equalResult { Bool Result = false; };
  /// One crater as it goes over the network (destruction sync: game_destruction_sync.cpp).
  KERN_PURE CraterRec_equalResult CraterRec_equal(const Terrain_CraterRec& self, const Terrain_CraterRec& o) {
    return {logic_and(logic_and(logic_and(math_compare<">">(kern::formula([](Float x, Float x_2) -> Float { return (x == x_2); }, self.x, o.x), 0.5), math_compare<">">(kern::formula([](Float y, Float y_2) -> Float { return (y == y_2); }, self.y, o.y), 0.5)), math_compare<">">(kern::formula([](Float r, Float r_2) -> Float { return (r == r_2); }, self.r, o.r), 0.5)), math_compare<">">(kern::formula([](Float depth, Float depth_2) -> Float { return (depth == depth_2); }, self.depth, o.depth), 0.5))};
  }

  /// Regions changed since the last call (city roads/areas re-lay themselves on them).
  void Terrain_take_changes(Terrain& self, List<Vec4>& out) {
    Int place = 0;
    place = list_length<Vec4>(out);
    list_insert_list(out, place, self.changes_);
    list_clear(self.changes_);
  }

  struct terrarium_heightResult { Float Result = 0.0; };
  /// Terrarium RGB -> metres.
  KERN_PURE terrarium_heightResult terrarium_height(U8 r = 0, U8 g = 0, U8 b = 0) {
    return {kern::formula([](Float r, Float g, Float b) -> Float { return ((((r * 256.0) + g) + kern::math::div(b, 256.0)) - 32768.0); }, kern::to<Float>(r), kern::to<Float>(g), kern::to<Float>(b))};
  }

  // ---- what nodes give and keep ----
  struct : kern::Node<10> { Int index = 0; } n10;
};
