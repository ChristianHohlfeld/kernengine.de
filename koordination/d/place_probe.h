// (Entwurf des Engine-Tests "Platz der Engine" - wird nach test_main.cpp übernommen)
namespace engine_place_probe {

struct Places : kern::Blueprint {
  KERN_STRUCT(MeshData) { List<Float> positions = List<Float>{}; List<Float> normals = List<Float>{}; List<Float> colors = List<Float>{}; List<Float> uvs = List<Float>{}; List<Float> extra = List<Float>{}; Int extra_size = 0; List<Int> indices = List<Int>{}; };
  KERN_STRUCT(GeoChunk) { Int x = 0; Int y = 0; Int items = 0; Int triangles = 0; Int hash = 0; Bool welded = false; MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}; };

  Int Builder = 0;
  Int Length = 0;
  Int At = 0;
  Int X = 0;
  Int Items = 0;
  Float P0 = 0.0;
  Int Beside = 0;
  Int Set = 0;
  List<GeoChunk> Chunks = List<GeoChunk>{};
  Int LengthAfter = 0;
  Bool Removed = false;
  GeoChunk First = GeoChunk{};
  Int Gone = 0;
  Int GoneLength = 0;
  List<GeoChunk> GoneChunks = List<GeoChunk>{};

  struct { Int result = 0; } n1;
  struct { Int index = 0; } n3;
  struct { Bool removed = false; } n4;
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n2;

  void Bind();
  void Change();
  void Release();
  void Read();
  std::string shown() const;
};

inline void Places::Bind() {
  engine_building_builder_make(n1);
  Builder = n1.result;
  engine_building_builder_chunks(n2, Builder);
  Length = list_length(n2.result);
}
inline void Places::Change() {
  list_add(n3, n2.result, GeoChunk{3, -4, 5, 0, 0, true, MeshData{List<Float>{0.1, 2.5}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 2, List<Int>{1, 2}}});
  At = n3.index;
  list_add(n2.result, GeoChunk{});
  n2.result[0].items = 7;
  X = (n2.result[0].x += 3);
  n2.result[1].mesh.positions = List<Float>{1.0 / 3.0, 4.0};
  P0 = n2.result[1].mesh.positions[0];
  n2.result[1].mesh.positions[1] = 0.1;
  Items = n2.result[0].items;
  Beside = n2.result[5].x;
  Set = (n2.result[5].x = 9);
  Chunks = n2.result;
  First = n2.result[0];
  LengthAfter = list_length(n2.result);
  list_insert(n2.result, 0, First);
  list_remove_at(n2.result, 1);
  list_remove(n4, n2.result, First);
  Removed = n4.removed;
}
inline void Places::Release() {
  engine_building_builder_release(Builder);
}
inline void Places::Read() {
  Gone = n2.result[0].x;
  n2.result[0].x = 4;
  list_add(n2.result, GeoChunk{});
  GoneLength = list_length(n2.result);
  GoneChunks = n2.result;
}

}  // namespace engine_place_probe

namespace kern::logic::detail {
using engine_place_probe::Places;
template <> struct Held<Places::MeshData> : HeldBy<Places::MeshData, &Places::MeshData::positions, &Places::MeshData::normals, &Places::MeshData::colors, &Places::MeshData::uvs, &Places::MeshData::extra, &Places::MeshData::extra_size, &Places::MeshData::indices> {};
template <> struct Held<Places::GeoChunk> : HeldBy<Places::GeoChunk, &Places::GeoChunk::x, &Places::GeoChunk::y, &Places::GeoChunk::items, &Places::GeoChunk::triangles, &Places::GeoChunk::hash, &Places::GeoChunk::welded, &Places::GeoChunk::mesh> {};
}  // namespace kern::logic::detail
