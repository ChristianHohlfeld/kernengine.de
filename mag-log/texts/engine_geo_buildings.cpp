struct engine_geo_buildings : virtual kern::Library, virtual engine_geo_mesh_data {
  // ---- types ----
  KERN_STRUCT(Rgb) { Float r = 1.0; Float g = 1.0; Float b = 1.0; };
  KERN_STRUCT(GeoChunk) { I32 x = 0; I32 y = 0; U32 items = 0; U32 triangles = 0; U32 hash = 0; Bool welded = false; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; };
};
