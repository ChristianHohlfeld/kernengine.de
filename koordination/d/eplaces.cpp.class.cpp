// Kern Blueprint, C++ notation 1: this text and the graph in Kern Studio are one thing in two
// notations, each read from the other without loss (kern/docs/blueprint_cpp.md).
#include "kern/logic/blueprint.h"

struct EnginePlaces : kern::Blueprint {
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
  Int AppendedLength = 0;
  List<GeoChunk> Appended = List<GeoChunk>{};

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

  void Append() {
    engine_building_builder_chunks(n10, Builder);
    engine_building_builder_chunks(n11, Builder);
    list_append(n10.result, Chunks);
    list_add(n10.result, First);
    AppendedLength = list_length<GeoChunk>(n11.result);
    Appended = n11.result;
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
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n10;
  struct { engine_building_builder_chunks_place<List<GeoChunk>> result = {}; } n11;
};
// How the engine reaches the Blueprint EnginePlaces by name (written by logic::native_glue after its
// text): glue, not part of the notation - nothing to edit here.
namespace kern::logic::detail {
template <> struct Held<::EnginePlaces::MeshData> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::List<kern::f64>>(*v.field(0)) && is_value<kern::List<kern::f64>>(*v.field(1)) && is_value<kern::List<kern::f64>>(*v.field(2)) && is_value<kern::List<kern::f64>>(*v.field(3)) && is_value<kern::List<kern::f64>>(*v.field(4)) && is_value<kern::i64>(*v.field(5)) && is_value<kern::List<kern::logic::Whole>>(*v.field(6)); }
  static ::EnginePlaces::MeshData from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::List<kern::f64>>(*v.field(0)), from_value<kern::List<kern::f64>>(*v.field(1)), from_value<kern::List<kern::f64>>(*v.field(2)), from_value<kern::List<kern::f64>>(*v.field(3)), from_value<kern::List<kern::f64>>(*v.field(4)), from_value<kern::i64>(*v.field(5)), from_value<kern::List<kern::logic::Whole>>(*v.field(6))};
  }
  static Value to(const ::EnginePlaces::MeshData& s) { return Value::structure({to_value(s.positions), to_value(s.normals), to_value(s.colors), to_value(s.uvs), to_value(s.extra), to_value(s.extra_size), to_value(s.indices)}); }
};
template <> struct Held<::EnginePlaces::GeoChunk> {
  static constexpr PinType type = PinType::structure;
  static bool is(const Value& v) { return v.type == type && v.fields() == 7 && is_value<kern::i64>(*v.field(0)) && is_value<kern::i64>(*v.field(1)) && is_value<kern::i64>(*v.field(2)) && is_value<kern::i64>(*v.field(3)) && is_value<kern::i64>(*v.field(4)) && is_value<bool>(*v.field(5)) && is_value<::EnginePlaces::MeshData>(*v.field(6)); }
  static ::EnginePlaces::GeoChunk from(const Value& v) {
    if (!is(v)) return {};
    return {from_value<kern::i64>(*v.field(0)), from_value<kern::i64>(*v.field(1)), from_value<kern::i64>(*v.field(2)), from_value<kern::i64>(*v.field(3)), from_value<kern::i64>(*v.field(4)), from_value<bool>(*v.field(5)), from_value<::EnginePlaces::MeshData>(*v.field(6))};
  }
  static Value to(const ::EnginePlaces::GeoChunk& s) { return Value::structure({to_value(s.x), to_value(s.y), to_value(s.items), to_value(s.triangles), to_value(s.hash), to_value(s.welded), to_value(s.mesh)}); }
};
}  // namespace kern::logic::detail
namespace {
struct EnginePlaces_Native final : kern::logic::NativeBlueprint {
  EnginePlaces self;
  void attach(const kern::logic::BlueprintWorld& world) override { self.kern_world = world; }
  void run(int node, int entry) override {
    using kern::logic::native_out;
    (void)entry;
    (void)node;
  }
  bool call(std::string_view function, const std::vector<kern::logic::Value>& args, std::vector<kern::logic::Value>& results) override {
    using kern::logic::native_arg;
    using kern::logic::to_value;
    (void)args;
    results.clear();
    if (function == "Bind") {
      kern::logic::native_call(true);
      self.Bind();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Change") {
      kern::logic::native_call(true);
      self.Change();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Append") {
      kern::logic::native_call(true);
      self.Append();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Release") {
      kern::logic::native_call(true);
      self.Release();
      kern::logic::native_call(false);
      return true;
    }
    if (function == "Read") {
      kern::logic::native_call(true);
      self.Read();
      kern::logic::native_call(false);
      return true;
    }
    return false;
  }
  bool get(std::string_view variable, kern::logic::Value& out) const override {
    (void)out;
    if (variable == "Builder") { out = kern::logic::to_value(self.Builder); return true; }
    if (variable == "Length") { out = kern::logic::to_value(self.Length); return true; }
    if (variable == "At") { out = kern::logic::to_value(self.At); return true; }
    if (variable == "X") { out = kern::logic::to_value(self.X); return true; }
    if (variable == "Items") { out = kern::logic::to_value(self.Items); return true; }
    if (variable == "P0") { out = kern::logic::to_value(self.P0); return true; }
    if (variable == "Beside") { out = kern::logic::to_value(self.Beside); return true; }
    if (variable == "Set") { out = kern::logic::to_value(self.Set); return true; }
    if (variable == "Chunks") { out = kern::logic::to_value(self.Chunks); return true; }
    if (variable == "LengthAfter") { out = kern::logic::to_value(self.LengthAfter); return true; }
    if (variable == "Removed") { out = kern::logic::to_value(self.Removed); return true; }
    if (variable == "First") { out = kern::logic::to_value(self.First); return true; }
    if (variable == "Before") { out = kern::logic::to_value(self.Before); return true; }
    if (variable == "Gone") { out = kern::logic::to_value(self.Gone); return true; }
    if (variable == "GoneLength") { out = kern::logic::to_value(self.GoneLength); return true; }
    if (variable == "GoneChunks") { out = kern::logic::to_value(self.GoneChunks); return true; }
    if (variable == "AppendedLength") { out = kern::logic::to_value(self.AppendedLength); return true; }
    if (variable == "Appended") { out = kern::logic::to_value(self.Appended); return true; }
    return false;
  }
  bool set(std::string_view variable, const kern::logic::Value& value) override {
    (void)value;
    if (variable == "Builder") { kern::logic::native_set(self.Builder, value); return true; }
    if (variable == "Length") { kern::logic::native_set(self.Length, value); return true; }
    if (variable == "At") { kern::logic::native_set(self.At, value); return true; }
    if (variable == "X") { kern::logic::native_set(self.X, value); return true; }
    if (variable == "Items") { kern::logic::native_set(self.Items, value); return true; }
    if (variable == "P0") { kern::logic::native_set(self.P0, value); return true; }
    if (variable == "Beside") { kern::logic::native_set(self.Beside, value); return true; }
    if (variable == "Set") { kern::logic::native_set(self.Set, value); return true; }
    if (variable == "Chunks") { kern::logic::native_set(self.Chunks, value); return true; }
    if (variable == "LengthAfter") { kern::logic::native_set(self.LengthAfter, value); return true; }
    if (variable == "Removed") { kern::logic::native_set(self.Removed, value); return true; }
    if (variable == "First") { kern::logic::native_set(self.First, value); return true; }
    if (variable == "Before") { kern::logic::native_set(self.Before, value); return true; }
    if (variable == "Gone") { kern::logic::native_set(self.Gone, value); return true; }
    if (variable == "GoneLength") { kern::logic::native_set(self.GoneLength, value); return true; }
    if (variable == "GoneChunks") { kern::logic::native_set(self.GoneChunks, value); return true; }
    if (variable == "AppendedLength") { kern::logic::native_set(self.AppendedLength, value); return true; }
    if (variable == "Appended") { kern::logic::native_set(self.Appended, value); return true; }
    return false;
  }
};
[[maybe_unused]] const bool EnginePlaces_registered =
    (kern::logic::register_native_blueprint(0x424e95d9ebe6d77bull, [] { return std::unique_ptr<kern::logic::NativeBlueprint>(new EnginePlaces_Native()); }), true);
}  // namespace
