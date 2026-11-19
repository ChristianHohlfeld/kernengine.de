struct engine_geo_mesh_data : virtual kern::Library, virtual engine_gfx_mesh {
  // ---- types ----
  KERN_STRUCT(MeshData) { List<Float> positions = List<Float>{}; List<Float> normals = List<Float>{}; List<Float> colors = List<Float>{}; List<Float> uvs = List<Float>{}; List<Float> extra = List<Float>{}; Int extra_size = 0; List<Int> indices = List<Int>{}; };
  KERN_STRUCT(MeshLook) { Material material = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<Int>{}}; Int extra_slot = 0; Int texture = 0; Bool visible = true; Int layers = 1; Bool pick_bvh = true; };
};
