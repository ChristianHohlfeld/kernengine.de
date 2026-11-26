// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/geo/buildings.h"
#include "kern/geo/earcut.h"
#include "kern/geo/mesh_data.h"
#include "kern/geo/primitives.h"
#include "kern/geo/roads.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct E_AlphaMode { using type = kern::AlphaMode; static constexpr type values[] = {kern::AlphaMode::opaque, kern::AlphaMode::mask, kern::AlphaMode::blend}; };
struct S_BuildingOptions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::BuildingOptions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::BuildingOptions& x); };
struct S_BuildingStyle { [[maybe_unused]] static Value to(EngineObjects& o, const kern::BuildingStyle& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::BuildingStyle& x); };
struct S_GeoChunk { [[maybe_unused]] static Value to(EngineObjects& o, const kern::GeoChunk& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::GeoChunk& x); };
struct S_Mat4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Mat4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Mat4& x); };
struct S_Material { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Material& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Material& x); };
struct S_MeshData { [[maybe_unused]] static Value to(EngineObjects& o, const kern::MeshData& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::MeshData& x); };
struct S_MeshInput { [[maybe_unused]] static Value to(EngineObjects& o, const kern::MeshInput& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::MeshInput& x); };
struct S_MeshLook { [[maybe_unused]] static Value to(EngineObjects& o, const kern::MeshLook& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::MeshLook& x); };
struct S_Rgb { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Rgb& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Rgb& x); };
struct S_RoadOptions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::RoadOptions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::RoadOptions& x); };
struct S_Vec4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Vec4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Vec4& x); };
struct S_WeldStats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::WeldStats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::WeldStats& x); };
Value S_BuildingOptions::to(EngineObjects& o, const kern::BuildingOptions& x) { return Value::structure({ev::Conv<ev::Num<f64>>::to(o, x.chunk_size), ev::Conv<ev::Num<u32>>::to(o, x.max_buildings), ev::Conv<ev::Bool>::to(o, x.cornice), ev::Conv<ev::Bool>::to(o, x.wall_records), ev::Conv<ev::Struct<S_BuildingStyle>>::to(o, x.style)}); }
void S_BuildingOptions::from(EngineObjects& o, const Value& v, kern::BuildingOptions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<f64>>::from(o, *f, x.chunk_size); if (const Value* f = v.field(1)) ev::Conv<ev::Num<u32>>::from(o, *f, x.max_buildings); if (const Value* f = v.field(2)) ev::Conv<ev::Bool>::from(o, *f, x.cornice); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.wall_records); if (const Value* f = v.field(4)) ev::Conv<ev::Struct<S_BuildingStyle>>::from(o, *f, x.style); }
Value S_BuildingStyle::to(EngineObjects& o, const kern::BuildingStyle& x) { return Value::structure({ev::Conv<ev::List<ev::Struct<S_Rgb>>>::to(o, x.walls), ev::Conv<ev::List<ev::Struct<S_Rgb>>>::to(o, x.roofs), ev::Conv<ev::Struct<S_Rgb>>::to(o, x.trim), ev::Conv<ev::Num<f64>>::to(o, x.trim_mix)}); }
void S_BuildingStyle::from(EngineObjects& o, const Value& v, kern::BuildingStyle& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Struct<S_Rgb>>>::from(o, *f, x.walls); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Struct<S_Rgb>>>::from(o, *f, x.roofs); if (const Value* f = v.field(2)) ev::Conv<ev::Struct<S_Rgb>>::from(o, *f, x.trim); if (const Value* f = v.field(3)) ev::Conv<ev::Num<f64>>::from(o, *f, x.trim_mix); }
Value S_GeoChunk::to(EngineObjects& o, const kern::GeoChunk& x) { return Value::structure({ev::Conv<ev::Num<i32>>::to(o, x.x), ev::Conv<ev::Num<i32>>::to(o, x.y), ev::Conv<ev::Num<u32>>::to(o, x.items), ev::Conv<ev::Num<u32>>::to(o, x.triangles), ev::Conv<ev::Num<u32>>::to(o, x.hash), ev::Conv<ev::Bool>::to(o, x.welded), ev::Conv<ev::Struct<S_MeshData>>::to(o, x.mesh)}); }
void S_GeoChunk::from(EngineObjects& o, const Value& v, kern::GeoChunk& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<i32>>::from(o, *f, x.x); if (const Value* f = v.field(1)) ev::Conv<ev::Num<i32>>::from(o, *f, x.y); if (const Value* f = v.field(2)) ev::Conv<ev::Num<u32>>::from(o, *f, x.items); if (const Value* f = v.field(3)) ev::Conv<ev::Num<u32>>::from(o, *f, x.triangles); if (const Value* f = v.field(4)) ev::Conv<ev::Num<u32>>::from(o, *f, x.hash); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.welded); if (const Value* f = v.field(6)) ev::Conv<ev::Struct<S_MeshData>>::from(o, *f, x.mesh); }
Value S_Mat4::to(EngineObjects& o, const kern::Mat4& x) { return Value::structure({ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.m)}); }
void S_Mat4::from(EngineObjects& o, const Value& v, kern::Mat4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.m); }
Value S_Material::to(EngineObjects& o, const kern::Material& x) { return Value::structure({ev::Conv<ev::Struct<S_Vec4>>::to(o, x.color), ev::Conv<ev::Enum<E_AlphaMode>>::to(o, x.alpha), ev::Conv<ev::Num<f32>>::to(o, x.alpha_cutoff), ev::Conv<ev::Num<f32>>::to(o, x.roughness), ev::Conv<ev::Num<f32>>::to(o, x.metalness), ev::Conv<ev::Vector>::to(o, x.emissive), ev::Conv<ev::Bool>::to(o, x.unlit), ev::Conv<ev::Bool>::to(o, x.double_sided), ev::Conv<ev::Bool>::to(o, x.cast_shadow), ev::Conv<ev::Bool>::to(o, x.frustum_culled), ev::Conv<ev::Bool>::to(o, x.viewmodel), ev::Conv<ev::Bool>::to(o, x.linear_colors), ev::Conv<ev::Bool>::to(o, x.lines), ev::Conv<ev::List<ev::Num<i32>>>::to(o, x.params)}); }
void S_Material::from(EngineObjects& o, const Value& v, kern::Material& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Struct<S_Vec4>>::from(o, *f, x.color); if (const Value* f = v.field(1)) ev::Conv<ev::Enum<E_AlphaMode>>::from(o, *f, x.alpha); if (const Value* f = v.field(2)) ev::Conv<ev::Num<f32>>::from(o, *f, x.alpha_cutoff); if (const Value* f = v.field(3)) ev::Conv<ev::Num<f32>>::from(o, *f, x.roughness); if (const Value* f = v.field(4)) ev::Conv<ev::Num<f32>>::from(o, *f, x.metalness); if (const Value* f = v.field(5)) ev::Conv<ev::Vector>::from(o, *f, x.emissive); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.unlit); if (const Value* f = v.field(7)) ev::Conv<ev::Bool>::from(o, *f, x.double_sided); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.cast_shadow); if (const Value* f = v.field(9)) ev::Conv<ev::Bool>::from(o, *f, x.frustum_culled); if (const Value* f = v.field(10)) ev::Conv<ev::Bool>::from(o, *f, x.viewmodel); if (const Value* f = v.field(11)) ev::Conv<ev::Bool>::from(o, *f, x.linear_colors); if (const Value* f = v.field(12)) ev::Conv<ev::Bool>::from(o, *f, x.lines); if (const Value* f = v.field(13)) ev::Conv<ev::List<ev::Num<i32>>>::from(o, *f, x.params); }
Value S_MeshData::to(EngineObjects& o, const kern::MeshData& x) { return Value::structure({ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.positions), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.normals), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.colors), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.uvs), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.extra), ev::Conv<ev::Num<u32>>::to(o, x.extra_size), ev::Conv<ev::List<ev::Num<u32>>>::to(o, x.indices)}); }
void S_MeshData::from(EngineObjects& o, const Value& v, kern::MeshData& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.positions); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.normals); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.colors); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.uvs); if (const Value* f = v.field(4)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.extra); if (const Value* f = v.field(5)) ev::Conv<ev::Num<u32>>::from(o, *f, x.extra_size); if (const Value* f = v.field(6)) ev::Conv<ev::List<ev::Num<u32>>>::from(o, *f, x.indices); }
Value S_MeshInput::to(EngineObjects& o, const kern::MeshInput& x) { return Value::structure({ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.positions), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.normals), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.colors), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.uvs), ev::Conv<ev::List<ev::Num<u32>>>::to(o, x.indices), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.joints), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.weights), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.bones), ev::Conv<ev::List<ev::Row<ev::List<ev::Num<f32>>>>>::to(o, x.extension), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.transforms), ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.tints), ev::Conv<ev::Struct<S_Material>>::to(o, x.material), ev::Conv<ev::Num<u32>>::to(o, x.texture), ev::Conv<ev::Bool>::to(o, x.visible), ev::Conv<ev::Num<u32>>::to(o, x.layers), ev::Conv<ev::Bool>::to(o, x.pick_bvh)}); }
void S_MeshInput::from(EngineObjects& o, const Value& v, kern::MeshInput& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.positions); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.normals); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.colors); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.uvs); if (const Value* f = v.field(4)) ev::Conv<ev::List<ev::Num<u32>>>::from(o, *f, x.indices); if (const Value* f = v.field(5)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.joints); if (const Value* f = v.field(6)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.weights); if (const Value* f = v.field(7)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.bones); if (const Value* f = v.field(8)) ev::Conv<ev::List<ev::Row<ev::List<ev::Num<f32>>>>>::from(o, *f, x.extension); if (const Value* f = v.field(9)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.transforms); if (const Value* f = v.field(10)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.tints); if (const Value* f = v.field(11)) ev::Conv<ev::Struct<S_Material>>::from(o, *f, x.material); if (const Value* f = v.field(12)) ev::Conv<ev::Num<u32>>::from(o, *f, x.texture); if (const Value* f = v.field(13)) ev::Conv<ev::Bool>::from(o, *f, x.visible); if (const Value* f = v.field(14)) ev::Conv<ev::Num<u32>>::from(o, *f, x.layers); if (const Value* f = v.field(15)) ev::Conv<ev::Bool>::from(o, *f, x.pick_bvh); }
Value S_MeshLook::to(EngineObjects& o, const kern::MeshLook& x) { return Value::structure({ev::Conv<ev::Struct<S_Material>>::to(o, x.material), ev::Conv<ev::Num<u32>>::to(o, x.extra_slot), ev::Conv<ev::Num<u32>>::to(o, x.texture), ev::Conv<ev::Bool>::to(o, x.visible), ev::Conv<ev::Num<u32>>::to(o, x.layers), ev::Conv<ev::Bool>::to(o, x.pick_bvh)}); }
void S_MeshLook::from(EngineObjects& o, const Value& v, kern::MeshLook& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Struct<S_Material>>::from(o, *f, x.material); if (const Value* f = v.field(1)) ev::Conv<ev::Num<u32>>::from(o, *f, x.extra_slot); if (const Value* f = v.field(2)) ev::Conv<ev::Num<u32>>::from(o, *f, x.texture); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.visible); if (const Value* f = v.field(4)) ev::Conv<ev::Num<u32>>::from(o, *f, x.layers); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.pick_bvh); }
Value S_Rgb::to(EngineObjects& o, const kern::Rgb& x) { return Value::structure({ev::Conv<ev::Num<f64>>::to(o, x.r), ev::Conv<ev::Num<f64>>::to(o, x.g), ev::Conv<ev::Num<f64>>::to(o, x.b)}); }
void S_Rgb::from(EngineObjects& o, const Value& v, kern::Rgb& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<f64>>::from(o, *f, x.r); if (const Value* f = v.field(1)) ev::Conv<ev::Num<f64>>::from(o, *f, x.g); if (const Value* f = v.field(2)) ev::Conv<ev::Num<f64>>::from(o, *f, x.b); }
Value S_RoadOptions::to(EngineObjects& o, const kern::RoadOptions& x) { return Value::structure({ev::Conv<ev::Num<f64>>::to(o, x.center_x), ev::Conv<ev::Num<f64>>::to(o, x.center_y), ev::Conv<ev::Num<f64>>::to(o, x.radius), ev::Conv<ev::Num<f64>>::to(o, x.chunk_size), ev::Conv<ev::Num<f64>>::to(o, x.segment_length), ev::Conv<ev::Num<f64>>::to(o, x.curb_extra), ev::Conv<ev::Num<f64>>::to(o, x.curb_z), ev::Conv<ev::Num<f64>>::to(o, x.surface_z), ev::Conv<ev::Num<f64>>::to(o, x.marking_z), ev::Conv<ev::Num<f64>>::to(o, x.marking_width), ev::Conv<ev::Num<f64>>::to(o, x.dash_start), ev::Conv<ev::Num<f64>>::to(o, x.dash_length), ev::Conv<ev::Num<f64>>::to(o, x.dash_period), ev::Conv<ev::Num<f64>>::to(o, x.uv_scale), ev::Conv<ev::Struct<S_Rgb>>::to(o, x.curb_color), ev::Conv<ev::List<ev::Struct<S_Rgb>>>::to(o, x.palette), ev::Conv<ev::Callback<ev::Bool(ev::Num<f64>, ev::Num<f64>, ev::Num<f64>)>>::to(o, x.deformed), ev::Conv<ev::Num<f64>>::to(o, x.fine_step)}); }
void S_RoadOptions::from(EngineObjects& o, const Value& v, kern::RoadOptions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<f64>>::from(o, *f, x.center_x); if (const Value* f = v.field(1)) ev::Conv<ev::Num<f64>>::from(o, *f, x.center_y); if (const Value* f = v.field(2)) ev::Conv<ev::Num<f64>>::from(o, *f, x.radius); if (const Value* f = v.field(3)) ev::Conv<ev::Num<f64>>::from(o, *f, x.chunk_size); if (const Value* f = v.field(4)) ev::Conv<ev::Num<f64>>::from(o, *f, x.segment_length); if (const Value* f = v.field(5)) ev::Conv<ev::Num<f64>>::from(o, *f, x.curb_extra); if (const Value* f = v.field(6)) ev::Conv<ev::Num<f64>>::from(o, *f, x.curb_z); if (const Value* f = v.field(7)) ev::Conv<ev::Num<f64>>::from(o, *f, x.surface_z); if (const Value* f = v.field(8)) ev::Conv<ev::Num<f64>>::from(o, *f, x.marking_z); if (const Value* f = v.field(9)) ev::Conv<ev::Num<f64>>::from(o, *f, x.marking_width); if (const Value* f = v.field(10)) ev::Conv<ev::Num<f64>>::from(o, *f, x.dash_start); if (const Value* f = v.field(11)) ev::Conv<ev::Num<f64>>::from(o, *f, x.dash_length); if (const Value* f = v.field(12)) ev::Conv<ev::Num<f64>>::from(o, *f, x.dash_period); if (const Value* f = v.field(13)) ev::Conv<ev::Num<f64>>::from(o, *f, x.uv_scale); if (const Value* f = v.field(14)) ev::Conv<ev::Struct<S_Rgb>>::from(o, *f, x.curb_color); if (const Value* f = v.field(15)) ev::Conv<ev::List<ev::Struct<S_Rgb>>>::from(o, *f, x.palette); if (const Value* f = v.field(16)) ev::Conv<ev::Callback<ev::Bool(ev::Num<f64>, ev::Num<f64>, ev::Num<f64>)>>::from(o, *f, x.deformed); if (const Value* f = v.field(17)) ev::Conv<ev::Num<f64>>::from(o, *f, x.fine_step); }
Value S_Vec4::to(EngineObjects& o, const kern::Vec4& x) { return Value::structure({ev::Conv<ev::Num<f32>>::to(o, x.x), ev::Conv<ev::Num<f32>>::to(o, x.y), ev::Conv<ev::Num<f32>>::to(o, x.z), ev::Conv<ev::Num<f32>>::to(o, x.w)}); }
void S_Vec4::from(EngineObjects& o, const Value& v, kern::Vec4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<f32>>::from(o, *f, x.x); if (const Value* f = v.field(1)) ev::Conv<ev::Num<f32>>::from(o, *f, x.y); if (const Value* f = v.field(2)) ev::Conv<ev::Num<f32>>::from(o, *f, x.z); if (const Value* f = v.field(3)) ev::Conv<ev::Num<f32>>::from(o, *f, x.w); }
Value S_WeldStats::to(EngineObjects& o, const kern::WeldStats& x) { return Value::structure({ev::Conv<ev::Num<u32>>::to(o, x.source_vertices), ev::Conv<ev::Num<u32>>::to(o, x.unique_vertices), ev::Conv<ev::Num<u64>>::to(o, x.bytes_before), ev::Conv<ev::Num<u64>>::to(o, x.bytes_after), ev::Conv<ev::Bool>::to(o, x.indexed)}); }
void S_WeldStats::from(EngineObjects& o, const Value& v, kern::WeldStats& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<u32>>::from(o, *f, x.source_vertices); if (const Value* f = v.field(1)) ev::Conv<ev::Num<u32>>::from(o, *f, x.unique_vertices); if (const Value* f = v.field(2)) ev::Conv<ev::Num<u64>>::from(o, *f, x.bytes_before); if (const Value* f = v.field(3)) ev::Conv<ev::Num<u64>>::from(o, *f, x.bytes_after); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.indexed); }

// engine.building_builder.begin: kern::BuildingBuilder::begin
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BuildingBuilder>(o, in, 0, "building_builder");
  if (!self) return false;
  std::vector<double> a0;
  ev::take<ev::List<ev::Num<f64>>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Num<u32>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::BuildingOptions const&>>();
  ev::take<ev::Struct<S_BuildingOptions>>(o, in, 3, a2);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list stream has fewer entries than length says"; return false; }
  decltype(auto) r = self->begin(a0.empty() ? nullptr : a0.data(), std::move(a1), a2);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.building_builder.step: kern::BuildingBuilder::step
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BuildingBuilder>(o, in, 0, "building_builder");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 1, a0);
  decltype(auto) r = self->step(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.building_builder.done: kern::BuildingBuilder::done
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BuildingBuilder>(o, in, 0, "building_builder");
  if (!self) return false;
  decltype(auto) r = self->done();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.building_builder.chunks: kern::BuildingBuilder::chunks
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BuildingBuilder>(o, in, 0, "building_builder");
  if (!self) return false;
  decltype(auto) r = self->chunks();
  ev::give<ev::List<ev::Struct<S_GeoChunk>>>(o, out, 0, r);
  return true;
}

// engine.building_builder.chunks@set: what stands at the place kern::BuildingBuilder::chunks gives
bool c3_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::BuildingBuilder>(o, in, 0, "building_builder");
  if (!self) return false;
  ev::take<ev::List<ev::Struct<S_GeoChunk>>>(o, in, 1, self->chunks());
  return true;
}

// engine.building_builder.processed: kern::BuildingBuilder::processed
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BuildingBuilder>(o, in, 0, "building_builder");
  if (!self) return false;
  decltype(auto) r = self->processed();
  ev::give<ev::Num<u32>>(o, out, 0, r);
  return true;
}

// engine.building_builder.built: kern::BuildingBuilder::built
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BuildingBuilder>(o, in, 0, "building_builder");
  if (!self) return false;
  decltype(auto) r = self->built();
  ev::give<ev::Num<u32>>(o, out, 0, r);
  return true;
}

// engine.building_builder.skipped: kern::BuildingBuilder::skipped
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BuildingBuilder>(o, in, 0, "building_builder");
  if (!self) return false;
  decltype(auto) r = self->skipped();
  ev::give<ev::Num<u32>>(o, out, 0, r);
  return true;
}

// engine.building_builder.triangles: kern::BuildingBuilder::triangles
bool c7(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BuildingBuilder>(o, in, 0, "building_builder");
  if (!self) return false;
  decltype(auto) r = self->triangles();
  ev::give<ev::Num<u32>>(o, out, 0, r);
  return true;
}

// engine.footprint_stream.begin: kern::FootprintStream::begin
bool c8(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::FootprintStream>(o, in, 0, "footprint_stream");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 3, a2);
  self->begin(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.footprint_stream.begin_2: kern::FootprintStream::begin
bool c9(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::FootprintStream>(o, in, 0, "footprint_stream");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 5, a4);
  self->begin(std::move(a0), std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  return true;
}

// engine.footprint_stream.empty: kern::FootprintStream::empty
bool c10(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::FootprintStream>(o, in, 0, "footprint_stream");
  if (!self) return false;
  decltype(auto) r = self->empty();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.footprint_stream.ring: kern::FootprintStream::ring
bool c11(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::FootprintStream>(o, in, 0, "footprint_stream");
  if (!self) return false;
  std::vector<kern::Vec2> a0;
  ev::take<ev::List<ev::Vector>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Num<u32>>(o, in, 2, a1);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list points has fewer entries than count says"; return false; }
  self->ring(a0.empty() ? nullptr : a0.data(), std::move(a1));
  return true;
}

// engine.footprint_stream.data: kern::FootprintStream::data
bool c12(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::FootprintStream>(o, in, 0, "footprint_stream");
  if (!self) return false;
  decltype(auto) r = self->data();
  ev::give<ev::List<ev::Num<f64>>>(o, out, 0, r);
  return true;
}

// engine.footprint_stream.size: kern::FootprintStream::size
bool c13(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::FootprintStream>(o, in, 0, "footprint_stream");
  if (!self) return false;
  decltype(auto) r = self->size();
  ev::give<ev::Num<u64>>(o, out, 0, r);
  return true;
}

// engine.footprint_stream.values: kern::FootprintStream::values
bool c14(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::FootprintStream>(o, in, 0, "footprint_stream");
  if (!self) return false;
  decltype(auto) r = self->values();
  ev::give<ev::List<ev::Num<f64>>>(o, out, 0, r);
  return true;
}

// engine.clean_ring: kern::clean_ring
bool c15(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<std::vector<double, std::allocator<double> >&>>();
  ev::take<ev::List<ev::Num<f64>>>(o, in, 0, a0);
  kern::clean_ring(a0);
  ev::give<ev::List<ev::Num<f64>>>(o, out, 0, a0);
  return true;
}

// engine.ring_area: kern::ring_area
bool c16(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::vector<double> a0;
  ev::take<ev::List<ev::Num<f64>>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<2u>>>();
  ev::take<ev::Num<u32>>(o, in, 1, a1);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list xy has fewer entries than count says"; return false; }
  decltype(auto) r = kern::ring_area(a0.empty() ? nullptr : a0.data(), std::move(a1));
  ev::give<ev::Num<f64>>(o, out, 0, r);
  return true;
}

// engine.earcut.triangulate: kern::Earcut::triangulate
bool c17(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Earcut>(o, in, 0, "earcut");
  if (!self) return false;
  std::vector<double> a0;
  ev::take<ev::List<ev::Num<f64>>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<2u>>>();
  ev::take<ev::Num<u32>>(o, in, 2, a1);
  std::vector<unsigned int> a2;
  ev::take<ev::List<ev::Num<u32>>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Num<u32>>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<std::vector<unsigned int, std::allocator<unsigned int> >&>>();
  ev::take<ev::List<ev::Num<u32>>>(o, in, 5, a4);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list xy has fewer entries than vertex_count says"; return false; }
  if (ev::numbers(a3) > a2.size()) { o.error = "the list hole_starts has fewer entries than hole_count says"; return false; }
  self->triangulate(a0.empty() ? nullptr : a0.data(), std::move(a1), a2.empty() ? nullptr : a2.data(), std::move(a3), a4);
  ev::give<ev::List<ev::Num<u32>>>(o, out, 0, a4);
  return true;
}

// engine.earcut.deviation: kern::Earcut::deviation
bool c18(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::vector<double> a0;
  ev::take<ev::List<ev::Num<f64>>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<2u>>>();
  ev::take<ev::Num<u32>>(o, in, 1, a1);
  std::vector<unsigned int> a2;
  ev::take<ev::List<ev::Num<u32>>>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Num<u32>>(o, in, 3, a3);
  std::vector<unsigned int> a4;
  ev::take<ev::List<ev::Num<u32>>>(o, in, 4, a4);
  auto a5 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Num<u32>>(o, in, 5, a5);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list xy has fewer entries than vertex_count says"; return false; }
  if (ev::numbers(a3) > a2.size()) { o.error = "the list hole_starts has fewer entries than hole_count says"; return false; }
  if (ev::numbers(a5) > a4.size()) { o.error = "the list triangles has fewer entries than index_count says"; return false; }
  decltype(auto) r = kern::Earcut::deviation(a0.empty() ? nullptr : a0.data(), std::move(a1), a2.empty() ? nullptr : a2.data(), std::move(a3), a4.empty() ? nullptr : a4.data(), std::move(a5));
  ev::give<ev::Num<f64>>(o, out, 0, r);
  return true;
}

// engine.mesh_data.as_input: kern::MeshData::as_input
bool c19(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::MeshData>();
  ev::take<ev::Struct<S_MeshData>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 1, a0);
  decltype(auto) r = self.as_input(std::move(a0));
  ev::give<ev::Struct<S_MeshInput>>(o, out, 0, r);
  return true;
}

// engine.upsert: kern::upsert
bool c20(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::Renderer* a0 = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::MeshData const&>>();
  ev::take<ev::Struct<S_MeshData>>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::MeshLook const&>>();
  ev::take<ev::Struct<S_MeshLook>>(o, in, 3, a3);
  std::vector<float> a4;
  ev::take<ev::List<ev::Num<f32>>>(o, in, 4, a4);
  std::vector<float> a5;
  ev::take<ev::List<ev::Num<f32>>>(o, in, 5, a5);
  decltype(auto) r = kern::upsert(*a0, std::move(a1), a2, a3, kern::Span<float const>(a4.data(), a4.size()), kern::Span<float const>(a5.data(), a5.size()));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.weld: kern::weld
bool c21(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::MeshData&>>();
  ev::take<ev::Struct<S_MeshData>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::size_t>>();
  ev::take<ev::Num<u64>>(o, in, 1, a1);
  decltype(auto) r = kern::weld(a0, std::move(a1));
  ev::give<ev::Struct<S_WeldStats>>(o, out, 0, r);
  ev::give<ev::Struct<S_MeshData>>(o, out, 1, a0);
  return true;
}

// engine.content_hash: kern::content_hash
bool c22(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::MeshData const&>>();
  ev::take<ev::Struct<S_MeshData>>(o, in, 0, a0);
  decltype(auto) r = kern::content_hash(a0);
  ev::give<ev::Num<u32>>(o, out, 0, r);
  return true;
}

// engine.primitives.box: kern::primitives::box
bool c23(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::primitives::box(std::move(a0), std::move(a1));
  ev::give<ev::Struct<S_MeshData>>(o, out, 0, r);
  return true;
}

// engine.primitives.box_on_ground: kern::primitives::box_on_ground
bool c24(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::primitives::box_on_ground(std::move(a0), std::move(a1));
  ev::give<ev::Struct<S_MeshData>>(o, out, 0, r);
  return true;
}

// engine.primitives.sphere: kern::primitives::sphere
bool c25(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a3);
  decltype(auto) r = kern::primitives::sphere(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  ev::give<ev::Struct<S_MeshData>>(o, out, 0, r);
  return true;
}

// engine.primitives.capsule: kern::primitives::capsule
bool c26(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 3, a3);
  auto a4 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 4, a4);
  decltype(auto) r = kern::primitives::capsule(std::move(a0), std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::Struct<S_MeshData>>(o, out, 0, r);
  return true;
}

// engine.primitives.torus: kern::primitives::torus
bool c27(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 3, a3);
  auto a4 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 4, a4);
  decltype(auto) r = kern::primitives::torus(std::move(a0), std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::Struct<S_MeshData>>(o, out, 0, r);
  return true;
}

// engine.primitives.cylinder: kern::primitives::cylinder
bool c28(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a3);
  decltype(auto) r = kern::primitives::cylinder(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  ev::give<ev::Struct<S_MeshData>>(o, out, 0, r);
  return true;
}

// engine.primitives.cone: kern::primitives::cone
bool c29(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a3);
  decltype(auto) r = kern::primitives::cone(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  ev::give<ev::Struct<S_MeshData>>(o, out, 0, r);
  return true;
}

// engine.primitives.plane: kern::primitives::plane
bool c30(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 3, a3);
  decltype(auto) r = kern::primitives::plane(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  ev::give<ev::Struct<S_MeshData>>(o, out, 0, r);
  return true;
}

// engine.primitives.append: kern::primitives::append
bool c31(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::MeshData&>>();
  ev::take<ev::Struct<S_MeshData>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::MeshData const&>>();
  ev::take<ev::Struct<S_MeshData>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Mat4 const&>>();
  ev::take<ev::Struct<S_Mat4>>(o, in, 2, a2);
  kern::primitives::append(a0, a1, a2);
  ev::give<ev::Struct<S_MeshData>>(o, out, 0, a0);
  return true;
}

// engine.road_builder.begin: kern::RoadBuilder::begin
bool c32(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::RoadBuilder>(o, in, 0, "road_builder");
  if (!self) return false;
  std::vector<double> a0;
  ev::take<ev::List<ev::Num<f64>>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Num<u32>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::RoadOptions const&>>();
  ev::take<ev::Struct<S_RoadOptions>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::HeightSampler>>();
  ev::take<ev::Callback<ev::Num<f64>(ev::Num<f64>, ev::Num<f64>)>>(o, in, 4, a3);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list stream has fewer entries than length says"; return false; }
  decltype(auto) r = self->begin(a0.empty() ? nullptr : a0.data(), std::move(a1), a2, std::move(a3));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.road_builder.step: kern::RoadBuilder::step
bool c33(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::RoadBuilder>(o, in, 0, "road_builder");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 1, a0);
  decltype(auto) r = self->step(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.road_builder.done: kern::RoadBuilder::done
bool c34(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::RoadBuilder>(o, in, 0, "road_builder");
  if (!self) return false;
  decltype(auto) r = self->done();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.road_builder.failed: kern::RoadBuilder::failed
bool c35(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::RoadBuilder>(o, in, 0, "road_builder");
  if (!self) return false;
  decltype(auto) r = self->failed();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.road_builder.chunks: kern::RoadBuilder::chunks
bool c36(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::RoadBuilder>(o, in, 0, "road_builder");
  if (!self) return false;
  decltype(auto) r = self->chunks();
  ev::give<ev::List<ev::Struct<S_GeoChunk>>>(o, out, 0, r);
  return true;
}

// engine.road_builder.chunks@set: what stands at the place kern::RoadBuilder::chunks gives
bool c36_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::RoadBuilder>(o, in, 0, "road_builder");
  if (!self) return false;
  ev::take<ev::List<ev::Struct<S_GeoChunk>>>(o, in, 1, self->chunks());
  return true;
}

// engine.road_builder.roads: kern::RoadBuilder::roads
bool c37(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::RoadBuilder>(o, in, 0, "road_builder");
  if (!self) return false;
  decltype(auto) r = self->roads();
  ev::give<ev::Num<u32>>(o, out, 0, r);
  return true;
}

// engine.road_builder.triangles: kern::RoadBuilder::triangles
bool c38(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::RoadBuilder>(o, in, 0, "road_builder");
  if (!self) return false;
  decltype(auto) r = self->triangles();
  ev::give<ev::Num<u32>>(o, out, 0, r);
  return true;
}

// engine.road_stream.road: kern::RoadStream::road
bool c39(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::RoadStream>(o, in, 0, "road_stream");
  if (!self) return false;
  std::vector<kern::Vec2> a0;
  ev::take<ev::List<ev::Vector>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Num<u32>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 5, a4);
  auto a5 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Num<i32>>(o, in, 6, a5);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list points has fewer entries than count says"; return false; }
  self->road(a0.empty() ? nullptr : a0.data(), std::move(a1), std::move(a2), std::move(a3), std::move(a4), std::move(a5));
  return true;
}

// engine.road_stream.data: kern::RoadStream::data
bool c40(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::RoadStream>(o, in, 0, "road_stream");
  if (!self) return false;
  decltype(auto) r = self->data();
  ev::give<ev::List<ev::Num<f64>>>(o, out, 0, r);
  return true;
}

// engine.road_stream.size: kern::RoadStream::size
bool c41(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::RoadStream>(o, in, 0, "road_stream");
  if (!self) return false;
  decltype(auto) r = self->size();
  ev::give<ev::Num<u64>>(o, out, 0, r);
  return true;
}

// engine.build_terrain_tile: kern::build_terrain_tile
bool c42(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 3, a3);
  auto a4 = ev::blank<std::remove_cvref_t<kern::Rgb>>();
  ev::take<ev::Struct<S_Rgb>>(o, in, 4, a4);
  auto a5 = ev::blank<std::remove_cvref_t<kern::HeightSampler>>();
  ev::take<ev::Callback<ev::Num<f64>(ev::Num<f64>, ev::Num<f64>)>>(o, in, 5, a5);
  auto a6 = ev::blank<std::remove_cvref_t<kern::MeshData&>>();
  ev::take<ev::Struct<S_MeshData>>(o, in, 6, a6);
  auto a7 = ev::blank<double>();
  auto a8 = ev::blank<double>();
  decltype(auto) r = kern::build_terrain_tile(std::move(a0), std::move(a1), std::move(a2), std::move(a3), std::move(a4), std::move(a5), a6, &a7, &a8);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_MeshData>>(o, out, 1, a6);
  ev::give<ev::Num<f64>>(o, out, 2, a7);
  ev::give<ev::Num<f64>>(o, out, 3, a8);
  return true;
}

// engine.building_builder.make: kern::BuildingBuilder::BuildingBuilder
bool c43(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::object(o.ref(o.own<kern::BuildingBuilder>(std::unique_ptr<kern::BuildingBuilder>(new kern::BuildingBuilder()))));
  return true;
}

// engine.building_builder.release: kern::BuildingBuilder::~BuildingBuilder
bool c44(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  ObjectRef r;
  if (!ev::object_ref(o, in, 0, "building_builder", r)) return false;
  return ev::release_of<kern::BuildingBuilder>(o, r, "building_builder", "BuildingBuilder");
}

// engine.earcut.make: kern::Earcut::Earcut
bool c45(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::object(o.ref(o.own<kern::Earcut>(std::unique_ptr<kern::Earcut>(new kern::Earcut()))));
  return true;
}

// engine.earcut.release: kern::Earcut::~Earcut
bool c46(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  ObjectRef r;
  if (!ev::object_ref(o, in, 0, "earcut", r)) return false;
  return ev::release_of<kern::Earcut>(o, r, "earcut", "Earcut");
}

// engine.footprint_stream.make: kern::FootprintStream::FootprintStream
bool c47(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::object(o.ref(o.own<kern::FootprintStream>(std::unique_ptr<kern::FootprintStream>(new kern::FootprintStream()))));
  return true;
}

// engine.footprint_stream.release: kern::FootprintStream::~FootprintStream
bool c48(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  ObjectRef r;
  if (!ev::object_ref(o, in, 0, "footprint_stream", r)) return false;
  return ev::release_of<kern::FootprintStream>(o, r, "footprint_stream", "FootprintStream");
}

// engine.road_builder.make: kern::RoadBuilder::RoadBuilder
bool c49(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::object(o.ref(o.own<kern::RoadBuilder>(std::unique_ptr<kern::RoadBuilder>(new kern::RoadBuilder()))));
  return true;
}

// engine.road_builder.release: kern::RoadBuilder::~RoadBuilder
bool c50(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  ObjectRef r;
  if (!ev::object_ref(o, in, 0, "road_builder", r)) return false;
  return ev::release_of<kern::RoadBuilder>(o, r, "road_builder", "RoadBuilder");
}

// engine.road_stream.make: kern::RoadStream::RoadStream
bool c51(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::object(o.ref(o.own<kern::RoadStream>(std::unique_ptr<kern::RoadStream>(new kern::RoadStream()))));
  return true;
}

// engine.road_stream.release: kern::RoadStream::~RoadStream
bool c52(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  ObjectRef r;
  if (!ev::object_ref(o, in, 0, "road_stream", r)) return false;
  return ev::release_of<kern::RoadStream>(o, r, "road_stream", "RoadStream");
}

const EngineCall kCalls[] = {
    {"engine.building_builder.begin", &c0},
    {"engine.building_builder.step", &c1},
    {"engine.building_builder.done", &c2},
    {"engine.building_builder.chunks", &c3},
    {"engine.building_builder.processed", &c4},
    {"engine.building_builder.built", &c5},
    {"engine.building_builder.skipped", &c6},
    {"engine.building_builder.triangles", &c7},
    {"engine.footprint_stream.begin", &c8},
    {"engine.footprint_stream.begin_2", &c9},
    {"engine.footprint_stream.empty", &c10},
    {"engine.footprint_stream.ring", &c11},
    {"engine.footprint_stream.data", &c12},
    {"engine.footprint_stream.size", &c13},
    {"engine.footprint_stream.values", &c14},
    {"engine.clean_ring", &c15},
    {"engine.ring_area", &c16},
    {"engine.earcut.triangulate", &c17},
    {"engine.earcut.deviation", &c18},
    {"engine.mesh_data.as_input", &c19},
    {"engine.upsert", &c20},
    {"engine.weld", &c21},
    {"engine.content_hash", &c22},
    {"engine.primitives.box", &c23},
    {"engine.primitives.box_on_ground", &c24},
    {"engine.primitives.sphere", &c25},
    {"engine.primitives.capsule", &c26},
    {"engine.primitives.torus", &c27},
    {"engine.primitives.cylinder", &c28},
    {"engine.primitives.cone", &c29},
    {"engine.primitives.plane", &c30},
    {"engine.primitives.append", &c31},
    {"engine.road_builder.begin", &c32},
    {"engine.road_builder.step", &c33},
    {"engine.road_builder.done", &c34},
    {"engine.road_builder.failed", &c35},
    {"engine.road_builder.chunks", &c36},
    {"engine.road_builder.roads", &c37},
    {"engine.road_builder.triangles", &c38},
    {"engine.road_stream.road", &c39},
    {"engine.road_stream.data", &c40},
    {"engine.road_stream.size", &c41},
    {"engine.build_terrain_tile", &c42},
    {"engine.building_builder.make", &c43},
    {"engine.building_builder.release", &c44},
    {"engine.earcut.make", &c45},
    {"engine.earcut.release", &c46},
    {"engine.footprint_stream.make", &c47},
    {"engine.footprint_stream.release", &c48},
    {"engine.road_builder.make", &c49},
    {"engine.road_builder.release", &c50},
    {"engine.road_stream.make", &c51},
    {"engine.road_stream.release", &c52},
    {"engine.building_builder.chunks@get", &c3},
    {"engine.building_builder.chunks@set", &c3_set},
    {"engine.road_builder.chunks@get", &c36},
    {"engine.road_builder.chunks@set", &c36_set},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

const EngineMaker kMakers[] = {
    {EngineObjects::kind<kern::BuildingBuilder>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::BuildingBuilder>(handle, std::unique_ptr<kern::BuildingBuilder>(new kern::BuildingBuilder())); }},
    {EngineObjects::kind<kern::Earcut>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::Earcut>(handle, std::unique_ptr<kern::Earcut>(new kern::Earcut())); }},
    {EngineObjects::kind<kern::FootprintStream>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::FootprintStream>(handle, std::unique_ptr<kern::FootprintStream>(new kern::FootprintStream())); }},
    {EngineObjects::kind<kern::RoadBuilder>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::RoadBuilder>(handle, std::unique_ptr<kern::RoadBuilder>(new kern::RoadBuilder())); }},
    {EngineObjects::kind<kern::RoadStream>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::RoadStream>(handle, std::unique_ptr<kern::RoadStream>(new kern::RoadStream())); }},
};
const EngineMakers kAllMakers(kMakers, std::size(kMakers));

}  // namespace
}  // namespace kern::logic
