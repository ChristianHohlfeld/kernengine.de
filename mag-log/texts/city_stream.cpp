struct city_stream : virtual kern::Library, virtual terrain {
  // ---- types ----
  KERN_STRUCT(CityStream_Damage) { Float height = 0.0; Bool leveled = false; };
  KERN_STRUCT(CityStream_DamageRec) { U32 seed = 0; Float height = 0.0; Bool leveled = false; };
  KERN_ENUM(CityStream_Tile_State, requested, decoding, terrain_wait, building, roads, areas, collision, ready, failed);
  KERN_STRUCT(CityStream_Wall) { Vector a = Vector{0.0, 0.0, 0.0}; Vector dir = Vector{0.0, 0.0, 0.0}; Vector normal = Vector{0.0, 0.0, 0.0}; Float len = 0.0; Float base = 0.0; Float top = 0.0; U32 seed = 0; U32 edge = 0; };
  KERN_STRUCT(Lane) { List<Vector> points = List<Vector>{}; Float width = 6.0; Int64 tile = Int64{0x0}; Bool alive = true; };
  KERN_STRUCT(StreamStats) { U32 tiles_ready = 0; U32 tiles_loading = 0; U32 tiles_failed = 0; U32 buildings = 0; U32 colliders = 0; U32 triangles = 0; U32 meshes = 0; Float last_slice_ms = 0.0; Float t_request = -1.0; Float t_data = -1.0; Float t_decoded = -1.0; Float t_terrain = -1.0; Float t_built = -1.0; Float t_collision = -1.0; Float work_ms = 0.0; Float worst_ms = 0.0; Text worst_step = ""; Float worst_step_ms = 0.0; U32 rebuilds = 0; U32 relays = 0; U32 uploads = 0; U32 upload_vertices = 0; };
  KERN_STRUCT(CityStream_NearCollision) { physics_Body body = physics_Body{Int64{0x0}}; physics_Body old_body = physics_Body{Int64{0x0}}; Vector at = Vector{1000000000.0, 1000000000.0, 0.0}; Vector ahead = Vector{1000000000.0, 1000000000.0, 0.0}; Float at_ms = -1000000000.0; List<Int64_Int_pair> list = List<Int64_Int_pair>{}; List<physics_Shape> shapes = List<physics_Shape>{}; Int next = 0; Bool building = false; Bool dirty = true; Bool built = false; List<Vector> mates_at = List<Vector>{}; U32 colliders = 0; };
  KERN_STRUCT(CityStream_Tile) { CityStream_Tile_State state = CityStream_Tile_State::requested; TileKey key = TileKey{14, 0, 0}; U32 tries = 0; Float retry_ms = 0.0; Bool procedural = false; MapFeatures features = MapFeatures{List<MapFeatures_Building>{}, List<MapFeatures_Road>{}, List<MapFeatures_Area>{}, List<MapFeatures_Spot>{}, List<MapFeatures_Spot>{}, false}; Int footprints = 0; Int roads = 0; Int buildings = 0; Int road_builder = 0; List<U32> meshes = List<U32>{}; physics_Body body = physics_Body{Int64{0x0}}; Int collide_next = 0; Int upload_next = 0; U32 colliders = 0; U32 triangles = 0; Vector center = Vector{0.0, 0.0, 0.0}; Vec4 bounds = Vec4{0.0, 0.0, 0.0, 0.0}; List<U32> lay_meshes = List<U32>{}; List<U32> old_lay = List<U32>{}; Bool relay = false; Bool relay_again = false; Vector roads_at = Vector{1000000000.0, 1000000000.0, 0.0}; List<Vector> cent = List<Vector>{}; Map<Int64, List<U32>> cell_items = Map<Int64, List<U32>>{}; Set<Int64> want_cells = Set<Int64>{}; Map<Int64, List<U32>> cell_old = Map<Int64, List<U32>>{}; Bool cell_uploading = false; U32 tile_job = 0; U32 build_job = 0; Map<Int64, List<U32>> cell_ids = Map<Int64, List<U32>>{}; Set<Int64> dirty_cells = Set<Int64>{}; List<Int64> job_cells = List<Int64>{}; U32 cell_job = 0; List<U8> built_bytes = List<U8>{}; Int built_at = 0; Bool prepared = false; I32 skip = -1; physics_Body old_body = physics_Body{Int64{0x0}}; Bool recollide = false; };
  KERN_STRUCT(CityStream) { Vector anchor = Vector{0.0, 0.0, 0.0}; Vector view_lead = Vector{0.0, 0.0, 0.0}; U32 building_budget = 2600; I32 ring = 1; List<Vector> extra_foci = List<Vector>{}; I32 mate_ring = 1; Bool training_offline = false; U32 damage_serial_ = 0; U32 furniture_serial_ = 0; List<Vec4> ground_changes_ = List<Vec4>{}; CityStream_NearCollision near_ = CityStream_NearCollision{physics_Body{Int64{0x0}}, physics_Body{Int64{0x0}}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, -1000000000.0, List<Int64_Int_pair>{}, List<physics_Shape>{}, 0, false, true, false, List<Vector>{}, 0}; StreamStats ms_ = StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}; Float start_ms_ = 0.0; Float worst_ = 0.0; Float worst_at_ = 0.0; Text worst_step_ = ""; Float worst_step_ms_ = 0.0; U32 rebuilds_ = 0; U32 relays_ = 0; U32 upload_count_ = 0; Int64 upload_vertices_ = Int64{0x0}; Vector focus_ = Vector{0.0, 0.0, 0.0}; Vector vis_at_ = Vector{1000000000.0, 1000000000.0, 0.0}; Vector vis_ahead_ = Vector{1000000000.0, 1000000000.0, 0.0}; Float vis_ms_ = -1000000000.0; Map<Int, CityStream_Damage> damage_ = Map<Int, CityStream_Damage>{}; List<U32> damage_log_ = List<U32>{}; Bool damage_quiet_ = false; Int r_ = 0; Int world_ = 0; GeoProjector proj_ = GeoProjector{0.0, 0.0, 1.0}; TileKey origin_tile_ = TileKey{14, 0, 0}; Map<Int64, List<CityStream_Tile>> tiles_ = Map<Int64, List<CityStream_Tile>>{}; List<TileKey> requests_ = List<TileKey>{}; List<Lane> lanes_ = List<Lane>{}; Bool offline_ = false; U32 real_tiles_ = 0; StreamStats stats_ = StreamStats{0, 0, 0, 0, 0, 0, 0, 0.0, -1.0, -1.0, -1.0, -1.0, -1.0, -1.0, 0.0, 0.0, "", 0.0, 0, 0, 0, 0}; Terrain terrain_ = Terrain{List<Vec4>{}, false, Vector{0.0, 0.0, 0.0}, List<Vector>{}, List<Terrain_CorridorSet_row>{}, Map<Int64, Terrain_Image>{}, List<TileKey>{}, Vector{1000000000.0, 1000000000.0, 0.0}, 0, 0, GeoProjector{0.0, 0.0, 1.0}, false, true, false, 0.0, false, Terrain_Grid{0.0, 0.0, 10.0, 0, List<Float>{}, 0.0, 0.0}, List<Terrain_Grid>{}, List<Terrain_Job>{}, Map<Int64, Terrain_DemTile>{}, List<TileKey>{}, 0, 0.0, Map<Int64, Terrain_WaterBlock>{}, List<Terrain_Bridge>{}, List<Terrain_Crater>{}, List<Terrain_Tint>{}, Map<Int64, Terrain_RenderTile>{}, Map<Int64, Terrain_CollisionTile>{}, Map<Int64, Int>{}, Map<Int64, Bool>{}, Set<Int64>{}, Set<Int64>{}, Set<Int64>{}, List<Int64>{}, List<Int64>{}, List<Int64>{}, Vector{1000000000.0, 1000000000.0, 0.0}, Vector{1000000000.0, 1000000000.0, 0.0}, List<Vector>{}, true, List<Vec4>{}, TerrainStats{0, 0, 0, 0, 0, 0, 0, 0, 0.0, 0.0, false, false}}; List<Vec4> terrain_changes_ = List<Vec4>{}; Int64 excl_tile_ = Int64{0x0}; I32 excl_index_ = -1; };

  // ---- functions ----
  struct CityStream_offlineResult { Bool Result = false; };
  KERN_PURE CityStream_offlineResult CityStream_offline(const CityStream& self) {
    return {self.offline_};
  }

  struct CityStream_has_real_dataResult { Bool Result = false; };
  KERN_PURE CityStream_has_real_dataResult CityStream_has_real_data(const CityStream& self) {
    return {math_compare<">">(kern::to<Float>(self.real_tiles_), 0.0)};
  }

  struct CityStream_furniture_serialResult { U32 Result = 0; };
  KERN_PURE CityStream_furniture_serialResult CityStream_furniture_serial(const CityStream& self) {
    return {self.furniture_serial_};
  }

  /// Where the ground changed since the last call (a crater, a new elevation window): what stands on it is put on it again.
  void CityStream_take_ground_changes(CityStream& self, List<Vec4>& out) {
    Int place = 0;
    place = list_length<Vec4>(out);
    list_insert_list(out, place, self.ground_changes_);
    list_clear(self.ground_changes_);
  }

  struct CityStream_damaged_buildingsResult { U32 Result = 0; };
  KERN_PURE CityStream_damaged_buildingsResult CityStream_damaged_buildings(const CityStream& self) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(a); }, kern::to<Float>(map_length<Int, CityStream_Damage>(self.damage_))))};
  }

  struct CityStream_damage_serialResult { U32 Result = 0; };
  /// Counts every height a building lost (marks on the walls drop what hangs in the air: world_marks.cpp).
  KERN_PURE CityStream_damage_serialResult CityStream_damage_serial(const CityStream& self) {
    return {self.damage_serial_};
  }

  struct CityStream_take_damage_logResult { List<U32> Result = List<U32>{}; };
  /// The seeds whose damage changed since the last call (what this machine has to tell).
  CityStream_take_damage_logResult CityStream_take_damage_log(CityStream& self) {
    var_take(n10, self.damage_log_);
    return {n10.value};
  }

  // ---- what nodes give and keep ----
  struct { List<U32> value = List<U32>{}; } n10;
};
