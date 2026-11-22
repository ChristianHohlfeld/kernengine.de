struct world_marks : virtual kern::Library, virtual city_stream, virtual engine_geo_mesh_data {
  // ---- types ----
  KERN_STRUCT(Marks_Scorch) { Vector c = Vector{0.0, 0.0, 0.0}; Float r = 0.0; U32 seed = 0; };
  KERN_STRUCT(Marks_Stats) { U32 scorches = 0; U32 soots = 0; U32 breaches = 0; U32 rubble = 0; U32 craters = 0; };
  KERN_STRUCT(Marks_WallMark) { Vector c = Vector{0.0, 0.0, 0.0}; Vector n = Vector{0.0, 0.0, 0.0}; Vector d = Vector{0.0, 0.0, 0.0}; Float size = 0.0; Float level = 0.0; U32 seed = 0; Bool breach = false; Int64 cell = Int64{0x0}; };
  KERN_STRUCT(Marks_Chunk) { physics_Body body = physics_Body{Int64{0x0}}; Vector half = Vector{0.0, 0.0, 0.0}; Float until = 0.0; };
  KERN_STRUCT(Marks) { List<Marks_Scorch> scorches_ = List<Marks_Scorch>{}; List<Marks_WallMark> soots_ = List<Marks_WallMark>{}; List<Marks_WallMark> breaches_ = List<Marks_WallMark>{}; List<Marks_WallMark> cracks_ = List<Marks_WallMark>{}; U32 crack_cursor_ = 0; List<Marks_Chunk> rubble_ = List<Marks_Chunk>{}; U32 scorch_cursor_ = 0; U32 soot_cursor_ = 0; U32 breach_cursor_ = 0; U32 rubble_cursor_ = 0; List<CityStream_Wall> near_ = List<CityStream_Wall>{}; Fixed<U32, 3> ground_mesh_ = List<U32>{}; U32 wall_mesh_ = 0; Bool dirty_ = false; U32 damage_seen_ = 0; Marks_Stats stats_ = Marks_Stats{0, 0, 0, 0, 0}; Fixed<MeshData, 3> ground_data_ = List<MeshData>{}; MeshData wall_data_ = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; Bool struck_ = false; };
};
