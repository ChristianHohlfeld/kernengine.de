struct engine_geo_buildings : virtual kern::Library, virtual engine_geo_mesh_data {
  // ---- types ----
  KERN_STRUCT(Rgb) { Float r = 1.0; Float g = 1.0; Float b = 1.0; };
  KERN_STRUCT(GeoChunk) { Int x = 0; Int y = 0; Int items = 0; Int triangles = 0; Int hash = 0; Bool welded = false; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; };
};
