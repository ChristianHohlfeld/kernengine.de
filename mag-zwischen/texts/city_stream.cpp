struct city_stream : virtual kern::Library, virtual terrain {
  // ---- types ----
  KERN_STRUCT(CityStream_AreaRef) { u64 key = 0; u64 tile = 0; u32 index = 0; };
  KERN_STRUCT(CityStream_BuildingRef) { u64 tile = 0; u32 index = 0; };
  KERN_STRUCT(CityStream_Damage) { f32 height = 0.0f; bool leveled = false; };
  KERN_STRUCT(CityStream_DamageRec) { u32 seed = 0; f32 height = 0.0f; bool leveled = false; };
  KERN_ENUM(CityStream_Tile_State, requested, decoding, terrain_wait, building, roads, areas, collision, ready, failed);
  KERN_STRUCT(Lane) { List<Vector> points = List<Vector>{}; f32 width = 6.0f; u64 tile = 0; bool alive = true; };
  KERN_STRUCT(StreamStats) { u32 tiles_ready = 0; u32 tiles_loading = 0; u32 tiles_failed = 0; u32 buildings = 0; u32 colliders = 0; u32 triangles = 0; u32 meshes = 0; f64 last_slice_ms = 0.0; f64 t_request = -1.0; f64 t_data = -1.0; f64 t_decoded = -1.0; f64 t_terrain = -1.0; f64 t_built = -1.0; f64 t_collision = -1.0; f64 work_ms = 0.0; f64 worst_ms = 0.0; Text worst_step = ""; f64 worst_step_ms = 0.0; u32 rebuilds = 0; u32 relays = 0; u32 uploads = 0; u32 upload_vertices = 0; };
  KERN_STRUCT(CityStream_NearCollision) { physics_Body body = physics_Body{0}; physics_Body old_body = physics_Body{0}; Vector at = Vector{1e+09, 1e+09, 0.0}; Vector ahead = Vector{1e+09, 1e+09, 0.0}; u32 steps = 1000000; List<u64_u32_pair> list = List<u64_u32_pair>{}; List<physics_Shape> shapes = List<physics_Shape>{}; u64 next = 0; bool building = false; bool dirty = true; bool built = false; List<Vector> mates_at = List<Vector>{}; u32 colliders = 0; };
  KERN_STRUCT(CityStream_Tile) { CityStream_Tile_State state = CityStream_Tile_State::requested; TileKey key = TileKey{14, 0, 0}; u32 tries = 0; f64 retry_ms = 0.0; bool procedural = false; MapFeatures features = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}; Object<kern::FootprintStream> footprints = Object<kern::FootprintStream>{}; Object<kern::RoadStream> roads = Object<kern::RoadStream>{}; Object<kern::BuildingBuilder> buildings = Object<kern::BuildingBuilder>{}; Object<kern::RoadBuilder> road_builder = Object<kern::RoadBuilder>{}; List<u32> meshes = List<u32>{}; physics_Body body = physics_Body{0}; u64 collide_next = 0; u64 upload_next = 0; u32 colliders = 0; u32 triangles = 0; Vector center = Vector{0.0, 0.0, 0.0}; Vec4 bounds = Vec4{0.0f, 0.0f, 0.0f, 0.0f}; List<u32> lay_meshes = List<u32>{}; List<u32> old_lay = List<u32>{}; bool relay = false; bool relay_again = false; Vector roads_at = Vector{1e+09, 1e+09, 0.0}; List<Vector> cent = List<Vector>{}; Map<u64, List<u32>> cell_items = Map<u64, List<u32>>{}; Set<u64> want_cells = Set<u64>{}; Map<u64, List<u32>> cell_old = Map<u64, List<u32>>{}; bool cell_uploading = false; u32 tile_job = 0; u32 build_job = 0; Map<u64, List<u32>> cell_ids = Map<u64, List<u32>>{}; Set<u64> dirty_cells = Set<u64>{}; List<u64> job_cells = List<u64>{}; u32 cell_job = 0; List<u8> built_bytes = List<u8>{}; u64 built_at = 0; bool prepared = false; i32 skip = -1; physics_Body old_body = physics_Body{0}; bool recollide = false; };
  KERN_STRUCT(CityStream) { Vector anchor = Vector{0.0, 0.0, 0.0}; Vector view_lead = Vector{0.0, 0.0, 0.0}; u32 building_budget = 2600; i32 ring = 1; List<Vector> extra_foci = List<Vector>{}; i32 mate_ring = 1; bool training_offline = false; u32 damage_serial_ = 0; u32 furniture_serial_ = 0; List<Vec4> ground_changes_ = List<Vec4>{}; CityStream_NearCollision near_ = CityStream_NearCollision{physics_Body{0}, physics_Body{0}, Vector{1e+09, 1e+09, 0.0}, Vector{1e+09, 1e+09, 0.0}, 1000000, List<u64_u32_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0}; StreamStats ms_ = StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}; f64 start_ms_ = 0.0; f64 worst_ = 0.0; f64 worst_at_ = 0.0; Text worst_step_ = ""; f64 worst_step_ms_ = 0.0; u32 rebuilds_ = 0; u32 relays_ = 0; u32 upload_count_ = 0; u64 upload_vertices_ = 0; Vector focus_ = Vector{0.0, 0.0, 0.0}; Vector vis_at_ = Vector{1e+09, 1e+09, 0.0}; Vector vis_ahead_ = Vector{1e+09, 1e+09, 0.0}; f64 vis_ms_ = -1000000000.0; Map<u32, CityStream_Damage> damage_ = Map<u32, CityStream_Damage>{}; List<u32> damage_log_ = List<u32>{}; bool damage_quiet_ = false; Object<kern::Renderer> r_ = Object<kern::Renderer>{}; Object<kern::physics::World> world_ = Object<kern::physics::World>{}; GeoProjector proj_ = GeoProjector{0.0, 0.0, 1.0}; TileKey origin_tile_ = TileKey{14, 0, 0}; Map<u64, List<CityStream_Tile>> tiles_ = Map<u64, List<CityStream_Tile>>{}; List<TileKey> requests_ = List<TileKey>{}; List<Lane> lanes_ = List<Lane>{}; bool offline_ = false; u32 real_tiles_ = 0; StreamStats stats_ = StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}; Terrain terrain_ = Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<u64, Terrain_Image>{}, List<TileKey>{}, Vector{1e+09, 1e+09, 0.0}, Object<kern::Renderer>{}, Object<kern::physics::World>{}, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0f, false, Terrain_Grid{0.0f, 0.0f, 10.0f, 0, List<f32>{}, 0.0f, 0.0f}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<u64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<u64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<u64, Terrain_RenderTile>{}, Map<u64, Terrain_CollisionTile>{}, Map<u64, u32>{}, Map<u64, bool>{}, Set<u64>{}, Set<u64>{}, Set<u64>{}, List<u64>{}, List<u64>{}, List<u64>{}, Vector{1e+09, 1e+09, 0.0}, Vector{1e+09, 1e+09, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0f, 0.0f, false, false}}; List<Vec4> terrain_changes_ = List<Vec4>{}; u64 excl_tile_ = 0; i32 excl_index_ = -1; };

  // ---- variables ----
  /// 250 ms at the world's 240 Hz (world_building_collisions.mjs: 250 ms)
  KERN_CONST u32 kNearSteps = 60;
  /// buildings of a new set a step (480 in 30 steps, an eighth of a second)
  KERN_CONST u32 kNearPerStep = 16;

  // ---- functions ----
  struct CityStream_offlineResult { bool Result = false; };
  KERN_PURE CityStream_offlineResult CityStream_offline(const CityStream& self) {
    return {self.offline_};
  }

  struct CityStream_has_real_dataResult { bool Result = false; };
  KERN_PURE CityStream_has_real_dataResult CityStream_has_real_data(const CityStream& self) {
    return {math_compare<">", u32>(self.real_tiles_, 0)};
  }

  struct CityStream_furniture_serialResult { u32 Result = 0; };
  KERN_PURE CityStream_furniture_serialResult CityStream_furniture_serial(const CityStream& self) {
    return {self.furniture_serial_};
  }

  /// Where the ground changed since the last call (a crater, a new elevation window): what stands on it is put on it again.
  void CityStream_take_ground_changes(CityStream& self, List<Vec4>& out) {
    i64 place = 0;
    place = list_length<Vec4>(out);
    list_insert_list(out, place, self.ground_changes_);
    list_clear(self.ground_changes_);
  }

  struct CityStream_damaged_buildingsResult { u32 Result = 0; };
  KERN_PURE CityStream_damaged_buildingsResult CityStream_damaged_buildings(const CityStream& self) {
    return {num_cast<u32, i64>(map_length<u32, CityStream_Damage>(self.damage_))};
  }

  struct CityStream_damage_serialResult { u32 Result = 0; };
  /// Counts every height a building lost (marks on the walls drop what hangs in the air: world_marks.cpp).
  KERN_PURE CityStream_damage_serialResult CityStream_damage_serial(const CityStream& self) {
    return {self.damage_serial_};
  }

  struct CityStream_take_damage_logResult { List<u32> Result = List<u32>{}; };
  /// The seeds whose damage changed since the last call (what this machine has to tell).
  CityStream_take_damage_logResult CityStream_take_damage_log(CityStream& self) {
    var_take(n10, self.damage_log_);
    return {n10.value};
  }

  // ---- what nodes give and keep ----
  struct { List<u32> value = List<u32>{}; } n10;
};
