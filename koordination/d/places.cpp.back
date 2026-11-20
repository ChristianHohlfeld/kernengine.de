// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct Places : kern::Blueprint {
  // ---- types ----
  KERN_STRUCT(MeshData) {
    List<Float> positions = List<Float>{};
    List<Float> normals = List<Float>{};
    List<Float> colors = List<Float>{};
    List<Float> uvs = List<Float>{};
    List<Float> extra = List<Float>{};
    Int extra_size = 0;
    List<Int> indices = List<Int>{};
  };
  KERN_STRUCT(GeoChunk) {
    Int x = 0;
    Int y = 0;
    Int items = 0;
    Int triangles = 0;
    Int hash = 0;
    Bool welded = false;
    MeshData mesh = MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}};
  };

  // ---- variables ----
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
  GeoChunk First = GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}};
  Int Before = 0;
  Int Gone = 0;
  Int GoneLength = 0;
  List<GeoChunk> GoneChunks = List<GeoChunk>{};

  // ---- functions ----
  void Bind() {
    engine_building_builder_make(n1);
    Builder = n1.result;
    engine_building_builder_chunks(n2, Builder);
    Length = list_length<GeoChunk>(n2.result);
  }

  void Change() {
    engine_building_builder_chunks(n7, Builder);
    list_add(n3, n7.result, GeoChunk{3, -4, 5, 0, 0, true, MeshData{List<Float>{0.1, 2.5}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 2, List<Int>{1, 2}}});
    At = n3.index;
    list_add(n7.result, GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}});
    n7.result[0].items = 7;
    n5.value = (n7.result[0].x += 3);
    X = n5.value;
    n7.result[1].mesh.positions = List<Float>{0.3333333333333333, 4.0};
    P0 = n7.result[1].mesh.positions[0];
    n7.result[1].mesh.positions[1] = 0.1;
    Items = n7.result[0].items;
    Removed = list_contains<GeoChunk>(n7.result, First);
    First = list_get<GeoChunk>(n7.result, 1);
    P0 = math_mul(n7.result[1].mesh.positions[0], 2.0);
    LengthAfter = list_length<Float>(n7.result[1].mesh.positions);
    Beside = n7.result[5].x;
    n6.value = (n7.result[5].x = 9);
    Set = n6.value;
    Chunks = n7.result;
    First = n7.result[0];
    LengthAfter = list_length<GeoChunk>(n7.result);
    list_insert(n7.result, 0, First);
    list_remove_at(n7.result, 1);
    list_remove(n4, n7.result, First);
    Removed = n4.removed;
  }

  void Release() {
    engine_building_builder_chunks(n8, Builder);
    Before = n8.result[0].x;
    engine_building_builder_release(Builder);
    Gone = n8.result[0].x;
    n8.result[0].x = 4;
    list_add(n8.result, GeoChunk{0, 0, 0, 0, 0, false, MeshData{List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, List<Float>{}, 0, List<Int>{}}});
    GoneLength = list_length<GeoChunk>(n8.result);
    GoneChunks = n8.result;
  }

  void Read() {
    engine_building_builder_chunks(n9, Builder);
    Gone = n9.result[1].x;
    n9.result[0].items = 4;
    GoneLength = list_length<GeoChunk>(n9.result);
  }

  // ---- what nodes give and keep ----
  struct { Int result = 0; } n1;
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n2;
  struct { Int index = 0; } n3;
  struct { Bool removed = false; } n4;
  struct { Int value = 0; } n5;
  struct { Int value = 0; } n6;
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n7;
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n8;
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n9;
};
