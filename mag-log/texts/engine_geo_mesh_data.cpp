struct engine_geo_mesh_data : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(MeshData) { List<Float> positions = List<Float>{}; List<Float> normals = List<Float>{}; List<Float> colors = List<Float>{}; List<Float> uvs = List<Float>{}; List<Float> extra = List<Float>{}; Int extra_size = 0; List<Int> indices = List<Int>{}; };
};
