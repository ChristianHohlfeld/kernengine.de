struct engine_geo_buildings : virtual kern::Library, virtual engine_geo_mesh_data {
  // ---- types ----
  KERN_STRUCT(Rgb) { Float r = 1.0; Float g = 1.0; Float b = 1.0; };
  KERN_STRUCT(BuildingStyle) { List<Rgb> walls = List<Rgb>{Rgb{0.75, 0.73, 0.7}}; List<Rgb> roofs = List<Rgb>{Rgb{0.3, 0.3, 0.32}}; Rgb trim = Rgb{0.957, 0.937, 0.894}; Float trim_mix = 0.45; };
  KERN_STRUCT(GeoChunk) { I32 x = 0; I32 y = 0; U32 items = 0; U32 triangles = 0; U32 hash = 0; Bool welded = false; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<U32>{}}; };
  KERN_STRUCT(BuildingOptions) { Float chunk_size = 320.0; U32 max_buildings = 2600; Bool cornice = true; Bool wall_records = true; BuildingStyle style = BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}; };
};
