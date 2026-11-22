struct engine_geo_mesh_data : virtual kern::Library {
  // ---- types ----
  KERN_STRUCT(MeshData) { List<Float> positions = List<Float>{}; List<Float> normals = List<Float>{}; List<Float> colors = List<Float>{}; List<Float> uvs = List<Float>{}; List<Float> extra = List<Float>{}; U32 extra_size = 0; List<U32> indices = List<U32>{}; };
};
