struct engine_geo_buildings : virtual kern::Library, virtual engine_geo_mesh_data {
  // ---- types ----
  KERN_STRUCT(Rgb) { f64 r = 1.0; f64 g = 1.0; f64 b = 1.0; };
  KERN_STRUCT(BuildingStyle) { List<Rgb> walls = List<Rgb>{Rgb{0.75, 0.73, 0.7}}; List<Rgb> roofs = List<Rgb>{Rgb{0.3, 0.3, 0.32}}; Rgb trim = Rgb{0.957, 0.937, 0.894}; f64 trim_mix = 0.45; };
  KERN_STRUCT(GeoChunk) { i32 x = 0; i32 y = 0; u32 items = 0; u32 triangles = 0; u32 hash = 0; bool welded = false; MeshData mesh = MeshData{List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, List<f32>{}, 0, List<u32>{}}; };
  KERN_STRUCT(BuildingOptions) { f64 chunk_size = 320.0; u32 max_buildings = 2600; bool cornice = true; bool wall_records = true; BuildingStyle style = BuildingStyle{List<Rgb>{Rgb{0.75, 0.73, 0.7}}, List<Rgb>{Rgb{0.3, 0.3, 0.32}}, Rgb{0.957, 0.937, 0.894}, 0.45}; };
};
