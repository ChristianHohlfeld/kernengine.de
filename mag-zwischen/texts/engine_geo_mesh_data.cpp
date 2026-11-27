struct engine_geo_mesh_data : virtual kern::Library, virtual engine_gfx_mesh {
  // ---- types ----
  KERN_STRUCT(MeshData) { List<f32> positions = List<f32>{}; List<f32> normals = List<f32>{}; List<f32> colors = List<f32>{}; List<f32> uvs = List<f32>{}; List<f32> extra = List<f32>{}; u32 extra_size = 0; List<u32> indices = List<u32>{}; };
  KERN_STRUCT(WeldStats) { u32 source_vertices = 0; u32 unique_vertices = 0; u64 bytes_before = 0; u64 bytes_after = 0; bool indexed = false; };
  KERN_STRUCT(MeshLook) { Material material = Material{Vec4{1.0f, 1.0f, 1.0f, 1.0f}, AlphaMode::opaque, 0.5f, 1.0f, 0.0f, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<i32>{}}; u32 extra_slot = 0; u32 texture = 0; bool visible = true; u32 layers = 1; bool pick_bvh = true; };
};
