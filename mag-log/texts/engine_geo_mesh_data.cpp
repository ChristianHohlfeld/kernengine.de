struct engine_geo_mesh_data : virtual kern::Library, virtual engine_gfx_mesh {
  // ---- types ----
  KERN_STRUCT(MeshData) { List<Float> positions = List<Float>{}; List<Float> normals = List<Float>{}; List<Float> colors = List<Float>{}; List<Float> uvs = List<Float>{}; List<Float> extra = List<Float>{}; U32 extra_size = 0; List<U32> indices = List<U32>{}; };
  KERN_STRUCT(WeldStats) { U32 source_vertices = 0; U32 unique_vertices = 0; Int bytes_before = 0; Int bytes_after = 0; Bool indexed = false; };
  KERN_STRUCT(MeshLook) { Material material = Material{Vec4{1.0, 1.0, 1.0, 1.0}, AlphaMode::opaque, 0.5, 1.0, 0.0, Vector{0.0, 0.0, 0.0}, false, false, true, true, false, false, false, List<I32>{}}; U32 extra_slot = 0; U32 texture = 0; Bool visible = true; U32 layers = 1; Bool pick_bvh = true; };

  // ---- functions ----
  struct MeshData_vertex_countResult { U32 Result = 0; };
  KERN_PURE MeshData_vertex_countResult MeshData_vertex_count(const MeshData& self) {
    return {kern::to<Int>(kern::formula([](Float a) -> Float { return kern::math::wrap32(kern::math::trunc(kern::math::div(a, 3.0))); }, kern::to<Float>(list_length<Float>(self.positions))))};
  }

  struct MeshData_triangle_countResult { U32 Result = 0; };
  KERN_PURE MeshData_triangle_countResult MeshData_triangle_count(const MeshData& self) {
    return {kern::to<Int>(kern::formula([](Float a, Float a_2, Float a_3) -> Float { return ((a != 0.0) ? kern::math::trunc(kern::math::div(a_2, 3.0)) : kern::math::wrap32(kern::math::trunc(kern::math::div(a_3, 3.0)))); }, kern::to<Float>(math_compare<"==">(kern::to<Float>(list_length<U32>(self.indices)), 0.0)), kern::to<Float>(MeshData_vertex_count(self).Result), kern::to<Float>(list_length<U32>(self.indices))))};
  }
};
