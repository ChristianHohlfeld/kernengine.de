// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include "kern/gfx/mesh.h"
#include "kern/gfx/particles.h"
#include "kern/gfx/passes.h"
#include "kern/gfx/program.h"
#include "kern/gfx/renderer.h"
#include "kern/gfx/shader.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct E_AlphaMode { using type = kern::AlphaMode; static constexpr type values[] = {kern::AlphaMode::opaque, kern::AlphaMode::mask, kern::AlphaMode::blend}; };
struct E_AoQuality { using type = kern::AoQuality; static constexpr type values[] = {kern::AoQuality::low, kern::AoQuality::medium, kern::AoQuality::high, kern::AoQuality::ultra}; };
struct E_BloomState { using type = kern::BloomState; static constexpr type values[] = {kern::BloomState::off, kern::BloomState::active, kern::BloomState::disabled_slow_device, kern::BloomState::unsupported}; };
struct S_AoOptions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::AoOptions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::AoOptions& x); };
struct S_BloomOptions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::BloomOptions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::BloomOptions& x); };
struct S_BloomPass_Occlusion { [[maybe_unused]] static Value to(EngineObjects& o, const kern::BloomPass::Occlusion& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::BloomPass::Occlusion& x); };
struct S_Camera { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Camera& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Camera& x); };
struct S_ExtensionAttribute { [[maybe_unused]] static Value to(EngineObjects& o, const kern::ExtensionAttribute& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::ExtensionAttribute& x); };
struct S_Lighting { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Lighting& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Lighting& x); };
struct S_Mat4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Mat4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Mat4& x); };
struct S_Material { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Material& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Material& x); };
struct S_MeshGpu { [[maybe_unused]] static Value to(EngineObjects& o, const kern::MeshGpu& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::MeshGpu& x); };
struct S_MeshInput { [[maybe_unused]] static Value to(EngineObjects& o, const kern::MeshInput& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::MeshInput& x); };
struct S_ParticleSpec { [[maybe_unused]] static Value to(EngineObjects& o, const kern::ParticleSpec& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::ParticleSpec& x); };
struct S_ParticleStats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::ParticleStats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::ParticleStats& x); };
struct S_PickHit { [[maybe_unused]] static Value to(EngineObjects& o, const kern::PickHit& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::PickHit& x); };
struct S_PickQuery { [[maybe_unused]] static Value to(EngineObjects& o, const kern::PickQuery& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::PickQuery& x); };
struct S_PickStats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::PickStats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::PickStats& x); };
struct S_PointLights { [[maybe_unused]] static Value to(EngineObjects& o, const kern::PointLights& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::PointLights& x); };
struct S_Ray { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Ray& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Ray& x); };
struct S_RenderParams { [[maybe_unused]] static Value to(EngineObjects& o, const kern::RenderParams& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::RenderParams& x); };
struct S_RenderStats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::RenderStats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::RenderStats& x); };
struct S_RendererOptions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::RendererOptions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::RendererOptions& x); };
struct S_ShaderSource { [[maybe_unused]] static Value to(EngineObjects& o, const kern::ShaderSource& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::ShaderSource& x); };
struct S_ShadowOptions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::ShadowOptions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::ShadowOptions& x); };
struct S_SkyAtmosphere { [[maybe_unused]] static Value to(EngineObjects& o, const kern::SkyAtmosphere& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::SkyAtmosphere& x); };
struct S_SkyState { [[maybe_unused]] static Value to(EngineObjects& o, const kern::SkyState& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::SkyState& x); };
struct S_Sphere { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Sphere& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Sphere& x); };
struct S_Vec4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Vec4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Vec4& x); };
struct S_Viewport { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Viewport& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Viewport& x); };
Value S_AoOptions::to(EngineObjects& o, const kern::AoOptions& x) { return Value::structure({ev::Conv<ev::Bool>::to(o, x.enabled), ev::Conv<ev::Enum<E_AoQuality>>::to(o, x.quality), ev::Conv<ev::Bool>::to(o, x.half_resolution), ev::Conv<ev::Float>::to(o, x.radius), ev::Conv<ev::Float>::to(o, x.power), ev::Conv<ev::Bool>::to(o, x.adaptive)}); }
void S_AoOptions::from(EngineObjects& o, const Value& v, kern::AoOptions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Bool>::from(o, *f, x.enabled); if (const Value* f = v.field(1)) ev::Conv<ev::Enum<E_AoQuality>>::from(o, *f, x.quality); if (const Value* f = v.field(2)) ev::Conv<ev::Bool>::from(o, *f, x.half_resolution); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.radius); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.power); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.adaptive); }
Value S_BloomOptions::to(EngineObjects& o, const kern::BloomOptions& x) { return Value::structure({ev::Conv<ev::Bool>::to(o, x.enabled), ev::Conv<ev::Bool>::to(o, x.forced), ev::Conv<ev::Bool>::to(o, x.grade)}); }
void S_BloomOptions::from(EngineObjects& o, const Value& v, kern::BloomOptions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Bool>::from(o, *f, x.enabled); if (const Value* f = v.field(1)) ev::Conv<ev::Bool>::from(o, *f, x.forced); if (const Value* f = v.field(2)) ev::Conv<ev::Bool>::from(o, *f, x.grade); }
Value S_BloomPass_Occlusion::to(EngineObjects& o, const kern::BloomPass::Occlusion& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.texture), ev::Conv<ev::Float>::to(o, x.scale_x), ev::Conv<ev::Float>::to(o, x.scale_y), ev::Conv<ev::Struct<S_Mat4>>::to(o, x.projection)}); }
void S_BloomPass_Occlusion::from(EngineObjects& o, const Value& v, kern::BloomPass::Occlusion& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.texture); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.scale_x); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.scale_y); if (const Value* f = v.field(3)) ev::Conv<ev::Struct<S_Mat4>>::from(o, *f, x.projection); }
Value S_Camera::to(EngineObjects& o, const kern::Camera& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.eye), ev::Conv<ev::Vector>::to(o, x.target), ev::Conv<ev::Vector>::to(o, x.up), ev::Conv<ev::Float>::to(o, x.fov), ev::Conv<ev::Float>::to(o, x.near), ev::Conv<ev::Float>::to(o, x.far), ev::Conv<ev::Bool>::to(o, x.orthographic), ev::Conv<ev::Float>::to(o, x.ortho_height), ev::Conv<ev::Bool>::to(o, x.has_projection), ev::Conv<ev::Struct<S_Mat4>>::to(o, x.projection)}); }
void S_Camera::from(EngineObjects& o, const Value& v, kern::Camera& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.eye); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.target); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.up); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.fov); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.near); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.far); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.orthographic); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.ortho_height); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.has_projection); if (const Value* f = v.field(9)) ev::Conv<ev::Struct<S_Mat4>>::from(o, *f, x.projection); }
Value S_ExtensionAttribute::to(EngineObjects& o, const kern::ExtensionAttribute& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.key), ev::Conv<ev::Int>::to(o, x.size), ev::Conv<ev::Text>::to(o, x.glsl)}); }
void S_ExtensionAttribute::from(EngineObjects& o, const Value& v, kern::ExtensionAttribute& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.key); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.size); if (const Value* f = v.field(2)) ev::Conv<ev::Text>::from(o, *f, x.glsl); }
Value S_Lighting::to(EngineObjects& o, const kern::Lighting& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.sun_direction), ev::Conv<ev::Vector>::to(o, x.sun_color), ev::Conv<ev::Vector>::to(o, x.sky_color), ev::Conv<ev::Vector>::to(o, x.ground_color), ev::Conv<ev::Vector>::to(o, x.fog_color), ev::Conv<ev::Float>::to(o, x.fog_density), ev::Conv<ev::Float>::to(o, x.exposure), ev::Conv<ev::Float>::to(o, x.env_intensity), ev::Conv<ev::Vector>::to(o, x.env_zenith), ev::Conv<ev::Vector>::to(o, x.env_horizon), ev::Conv<ev::Vector>::to(o, x.env_ground), ev::Conv<ev::Float>::to(o, x.fog_height_falloff), ev::Conv<ev::Float>::to(o, x.fog_sun_scatter), ev::Conv<ev::Float>::to(o, x.fog_horizon)}); }
void S_Lighting::from(EngineObjects& o, const Value& v, kern::Lighting& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.sun_direction); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.sun_color); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.sky_color); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.ground_color); if (const Value* f = v.field(4)) ev::Conv<ev::Vector>::from(o, *f, x.fog_color); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.fog_density); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.exposure); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.env_intensity); if (const Value* f = v.field(8)) ev::Conv<ev::Vector>::from(o, *f, x.env_zenith); if (const Value* f = v.field(9)) ev::Conv<ev::Vector>::from(o, *f, x.env_horizon); if (const Value* f = v.field(10)) ev::Conv<ev::Vector>::from(o, *f, x.env_ground); if (const Value* f = v.field(11)) ev::Conv<ev::Float>::from(o, *f, x.fog_height_falloff); if (const Value* f = v.field(12)) ev::Conv<ev::Float>::from(o, *f, x.fog_sun_scatter); if (const Value* f = v.field(13)) ev::Conv<ev::Float>::from(o, *f, x.fog_horizon); }
Value S_Mat4::to(EngineObjects& o, const kern::Mat4& x) { return Value::structure({ev::Conv<ev::List<ev::Float>>::to(o, x.m)}); }
void S_Mat4::from(EngineObjects& o, const Value& v, kern::Mat4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.m); }
Value S_Material::to(EngineObjects& o, const kern::Material& x) { return Value::structure({ev::Conv<ev::Struct<S_Vec4>>::to(o, x.color), ev::Conv<ev::Enum<E_AlphaMode>>::to(o, x.alpha), ev::Conv<ev::Float>::to(o, x.alpha_cutoff), ev::Conv<ev::Float>::to(o, x.roughness), ev::Conv<ev::Float>::to(o, x.metalness), ev::Conv<ev::Vector>::to(o, x.emissive), ev::Conv<ev::Bool>::to(o, x.unlit), ev::Conv<ev::Bool>::to(o, x.double_sided), ev::Conv<ev::Bool>::to(o, x.cast_shadow), ev::Conv<ev::Bool>::to(o, x.frustum_culled), ev::Conv<ev::Bool>::to(o, x.viewmodel), ev::Conv<ev::Bool>::to(o, x.linear_colors), ev::Conv<ev::Bool>::to(o, x.lines), ev::Conv<ev::List<ev::Int>>::to(o, x.params)}); }
void S_Material::from(EngineObjects& o, const Value& v, kern::Material& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Struct<S_Vec4>>::from(o, *f, x.color); if (const Value* f = v.field(1)) ev::Conv<ev::Enum<E_AlphaMode>>::from(o, *f, x.alpha); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.alpha_cutoff); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.roughness); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.metalness); if (const Value* f = v.field(5)) ev::Conv<ev::Vector>::from(o, *f, x.emissive); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.unlit); if (const Value* f = v.field(7)) ev::Conv<ev::Bool>::from(o, *f, x.double_sided); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.cast_shadow); if (const Value* f = v.field(9)) ev::Conv<ev::Bool>::from(o, *f, x.frustum_culled); if (const Value* f = v.field(10)) ev::Conv<ev::Bool>::from(o, *f, x.viewmodel); if (const Value* f = v.field(11)) ev::Conv<ev::Bool>::from(o, *f, x.linear_colors); if (const Value* f = v.field(12)) ev::Conv<ev::Bool>::from(o, *f, x.lines); if (const Value* f = v.field(13)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.params); }
Value S_MeshGpu::to(EngineObjects& o, const kern::MeshGpu& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.vao), ev::Conv<ev::Int>::to(o, x.position), ev::Conv<ev::Int>::to(o, x.normal), ev::Conv<ev::Int>::to(o, x.color), ev::Conv<ev::Int>::to(o, x.uv), ev::Conv<ev::Int>::to(o, x.transforms), ev::Conv<ev::Int>::to(o, x.tints), ev::Conv<ev::Int>::to(o, x.instance_normals), ev::Conv<ev::Int>::to(o, x.joints), ev::Conv<ev::Int>::to(o, x.weights), ev::Conv<ev::List<ev::Int>>::to(o, x.extension), ev::Conv<ev::Int>::to(o, x.element), ev::Conv<ev::Int>::to(o, x.element_type), ev::Conv<ev::List<ev::Int>>::to(o, x.capacity)}); }
void S_MeshGpu::from(EngineObjects& o, const Value& v, kern::MeshGpu& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.vao); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.position); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.normal); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.color); if (const Value* f = v.field(4)) ev::Conv<ev::Int>::from(o, *f, x.uv); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.transforms); if (const Value* f = v.field(6)) ev::Conv<ev::Int>::from(o, *f, x.tints); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.instance_normals); if (const Value* f = v.field(8)) ev::Conv<ev::Int>::from(o, *f, x.joints); if (const Value* f = v.field(9)) ev::Conv<ev::Int>::from(o, *f, x.weights); if (const Value* f = v.field(10)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.extension); if (const Value* f = v.field(11)) ev::Conv<ev::Int>::from(o, *f, x.element); if (const Value* f = v.field(12)) ev::Conv<ev::Int>::from(o, *f, x.element_type); if (const Value* f = v.field(13)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.capacity); }
Value S_MeshInput::to(EngineObjects& o, const kern::MeshInput& x) { return Value::structure({ev::Conv<ev::List<ev::Float>>::to(o, x.positions), ev::Conv<ev::List<ev::Float>>::to(o, x.normals), ev::Conv<ev::List<ev::Float>>::to(o, x.colors), ev::Conv<ev::List<ev::Float>>::to(o, x.uvs), ev::Conv<ev::List<ev::Int>>::to(o, x.indices), ev::Conv<ev::List<ev::Float>>::to(o, x.joints), ev::Conv<ev::List<ev::Float>>::to(o, x.weights), ev::Conv<ev::List<ev::Float>>::to(o, x.bones), ev::Conv<ev::List<ev::Row<ev::List<ev::Float>>>>::to(o, x.extension), ev::Conv<ev::List<ev::Float>>::to(o, x.transforms), ev::Conv<ev::List<ev::Float>>::to(o, x.tints), ev::Conv<ev::Struct<S_Material>>::to(o, x.material), ev::Conv<ev::Int>::to(o, x.texture), ev::Conv<ev::Bool>::to(o, x.visible), ev::Conv<ev::Int>::to(o, x.layers), ev::Conv<ev::Bool>::to(o, x.pick_bvh)}); }
void S_MeshInput::from(EngineObjects& o, const Value& v, kern::MeshInput& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.positions); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.normals); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.colors); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.uvs); if (const Value* f = v.field(4)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.indices); if (const Value* f = v.field(5)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.joints); if (const Value* f = v.field(6)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.weights); if (const Value* f = v.field(7)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.bones); if (const Value* f = v.field(8)) ev::Conv<ev::List<ev::Row<ev::List<ev::Float>>>>::from(o, *f, x.extension); if (const Value* f = v.field(9)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.transforms); if (const Value* f = v.field(10)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.tints); if (const Value* f = v.field(11)) ev::Conv<ev::Struct<S_Material>>::from(o, *f, x.material); if (const Value* f = v.field(12)) ev::Conv<ev::Int>::from(o, *f, x.texture); if (const Value* f = v.field(13)) ev::Conv<ev::Bool>::from(o, *f, x.visible); if (const Value* f = v.field(14)) ev::Conv<ev::Int>::from(o, *f, x.layers); if (const Value* f = v.field(15)) ev::Conv<ev::Bool>::from(o, *f, x.pick_bvh); }
Value S_ParticleSpec::to(EngineObjects& o, const kern::ParticleSpec& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Vector>::to(o, x.velocity), ev::Conv<ev::Struct<S_Vec4>>::to(o, x.color), ev::Conv<ev::Float>::to(o, x.start_ms), ev::Conv<ev::Float>::to(o, x.life_ms), ev::Conv<ev::Float>::to(o, x.size), ev::Conv<ev::Float>::to(o, x.gravity), ev::Conv<ev::Float>::to(o, x.grow), ev::Conv<ev::Float>::to(o, x.fade_extra)}); }
void S_ParticleSpec::from(EngineObjects& o, const Value& v, kern::ParticleSpec& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.velocity); if (const Value* f = v.field(2)) ev::Conv<ev::Struct<S_Vec4>>::from(o, *f, x.color); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.start_ms); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.life_ms); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.size); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.gravity); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.grow); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.fade_extra); }
Value S_ParticleStats::to(EngineObjects& o, const kern::ParticleStats& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.active), ev::Conv<ev::Int>::to(o, x.dropped), ev::Conv<ev::Int>::to(o, x.uploaded), ev::Conv<ev::Int>::to(o, x.expired)}); }
void S_ParticleStats::from(EngineObjects& o, const Value& v, kern::ParticleStats& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.active); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.dropped); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.uploaded); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.expired); }
Value S_PickHit::to(EngineObjects& o, const kern::PickHit& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.mesh), ev::Conv<ev::Int>::to(o, x.instance), ev::Conv<ev::Int>::to(o, x.triangle), ev::Conv<ev::Float>::to(o, x.distance), ev::Conv<ev::Vector>::to(o, x.point), ev::Conv<ev::Vector>::to(o, x.normal), ev::Conv<ev::Vector>::to(o, x.barycentric)}); }
void S_PickHit::from(EngineObjects& o, const Value& v, kern::PickHit& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.mesh); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.instance); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.triangle); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.distance); if (const Value* f = v.field(4)) ev::Conv<ev::Vector>::from(o, *f, x.point); if (const Value* f = v.field(5)) ev::Conv<ev::Vector>::from(o, *f, x.normal); if (const Value* f = v.field(6)) ev::Conv<ev::Vector>::from(o, *f, x.barycentric); }
Value S_PickQuery::to(EngineObjects& o, const kern::PickQuery& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.max_distance), ev::Conv<ev::Int>::to(o, x.ignore_id), ev::Conv<ev::Bool>::to(o, x.include_invisible), ev::Conv<ev::Int>::to(o, x.layers)}); }
void S_PickQuery::from(EngineObjects& o, const Value& v, kern::PickQuery& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.max_distance); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.ignore_id); if (const Value* f = v.field(2)) ev::Conv<ev::Bool>::from(o, *f, x.include_invisible); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.layers); }
Value S_PickStats::to(EngineObjects& o, const kern::PickStats& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.triangles_examined), ev::Conv<ev::Int>::to(o, x.bounds_rejected)}); }
void S_PickStats::from(EngineObjects& o, const Value& v, kern::PickStats& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.triangles_examined); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.bounds_rejected); }
Value S_PointLights::to(EngineObjects& o, const kern::PointLights& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.count), ev::Conv<ev::List<ev::Float>>::to(o, x.pos_range), ev::Conv<ev::List<ev::Float>>::to(o, x.color_intensity)}); }
void S_PointLights::from(EngineObjects& o, const Value& v, kern::PointLights& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.count); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.pos_range); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.color_intensity); }
Value S_Ray::to(EngineObjects& o, const kern::Ray& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.origin), ev::Conv<ev::Vector>::to(o, x.direction), ev::Conv<ev::Float>::to(o, x.near), ev::Conv<ev::Float>::to(o, x.far)}); }
void S_Ray::from(EngineObjects& o, const Value& v, kern::Ray& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.origin); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.direction); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.near); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.far); }
Value S_RenderParams::to(EngineObjects& o, const kern::RenderParams& x) { return Value::structure({ev::Conv<ev::Struct<S_Vec4>>::to(o, x.clear_color), ev::Conv<ev::Bool>::to(o, x.has_viewport), ev::Conv<ev::Struct<S_Viewport>>::to(o, x.viewport), ev::Conv<ev::Bool>::to(o, x.page_hidden), ev::Conv<ev::Int>::to(o, x.layers), ev::Conv<ev::Bool>::to(o, x.sky), ev::Conv<ev::Bool>::to(o, x.reuse_shadow), ev::Conv<ev::Int>::to(o, x.framebuffer)}); }
void S_RenderParams::from(EngineObjects& o, const Value& v, kern::RenderParams& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Struct<S_Vec4>>::from(o, *f, x.clear_color); if (const Value* f = v.field(1)) ev::Conv<ev::Bool>::from(o, *f, x.has_viewport); if (const Value* f = v.field(2)) ev::Conv<ev::Struct<S_Viewport>>::from(o, *f, x.viewport); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.page_hidden); if (const Value* f = v.field(4)) ev::Conv<ev::Int>::from(o, *f, x.layers); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.sky); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.reuse_shadow); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.framebuffer); }
Value S_RenderStats::to(EngineObjects& o, const kern::RenderStats& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.draw_calls), ev::Conv<ev::Int>::to(o, x.shadow_draw_calls), ev::Conv<ev::Int>::to(o, x.shadow_resolution), ev::Conv<ev::Bool>::to(o, x.ao), ev::Conv<ev::Bool>::to(o, x.shadow_reused), ev::Conv<ev::Float>::to(o, x.triangles), ev::Conv<ev::Int>::to(o, x.instances), ev::Conv<ev::Int>::to(o, x.culled_meshes), ev::Conv<ev::Int>::to(o, x.visible_meshes), ev::Conv<ev::Float>::to(o, x.cpu_ms), ev::Conv<ev::Float>::to(o, x.gpu_ms), ev::Conv<ev::Enum<E_BloomState>>::to(o, x.bloom)}); }
void S_RenderStats::from(EngineObjects& o, const Value& v, kern::RenderStats& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.draw_calls); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.shadow_draw_calls); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.shadow_resolution); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.ao); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.shadow_reused); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.triangles); if (const Value* f = v.field(6)) ev::Conv<ev::Int>::from(o, *f, x.instances); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.culled_meshes); if (const Value* f = v.field(8)) ev::Conv<ev::Int>::from(o, *f, x.visible_meshes); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.cpu_ms); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.gpu_ms); if (const Value* f = v.field(11)) ev::Conv<ev::Enum<E_BloomState>>::from(o, *f, x.bloom); }
Value S_RendererOptions::to(EngineObjects& o, const kern::RendererOptions& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.width), ev::Conv<ev::Int>::to(o, x.height), ev::Conv<ev::Bool>::to(o, x.shadows), ev::Conv<ev::Struct<S_ShadowOptions>>::to(o, x.shadow), ev::Conv<ev::Struct<S_BloomOptions>>::to(o, x.bloom), ev::Conv<ev::Struct<S_AoOptions>>::to(o, x.ao), ev::Conv<ev::Bool>::to(o, x.grade)}); }
void S_RendererOptions::from(EngineObjects& o, const Value& v, kern::RendererOptions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.width); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.height); if (const Value* f = v.field(2)) ev::Conv<ev::Bool>::from(o, *f, x.shadows); if (const Value* f = v.field(3)) ev::Conv<ev::Struct<S_ShadowOptions>>::from(o, *f, x.shadow); if (const Value* f = v.field(4)) ev::Conv<ev::Struct<S_BloomOptions>>::from(o, *f, x.bloom); if (const Value* f = v.field(5)) ev::Conv<ev::Struct<S_AoOptions>>::from(o, *f, x.ao); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.grade); }
Value S_ShaderSource::to(EngineObjects& o, const kern::ShaderSource& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.vertex), ev::Conv<ev::Text>::to(o, x.fragment)}); }
void S_ShaderSource::from(EngineObjects& o, const Value& v, kern::ShaderSource& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.vertex); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.fragment); }
Value S_ShadowOptions::to(EngineObjects& o, const kern::ShadowOptions& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.resolution), ev::Conv<ev::Float>::to(o, x.extent), ev::Conv<ev::Float>::to(o, x.bias), ev::Conv<ev::Int>::to(o, x.filter)}); }
void S_ShadowOptions::from(EngineObjects& o, const Value& v, kern::ShadowOptions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.resolution); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.extent); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.bias); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.filter); }
Value S_SkyAtmosphere::to(EngineObjects& o, const kern::SkyAtmosphere& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.fog_color), ev::Conv<ev::Vector>::to(o, x.sun_color), ev::Conv<ev::Vector>::to(o, x.sun_direction), ev::Conv<ev::Float>::to(o, x.sun_scatter), ev::Conv<ev::Float>::to(o, x.horizon)}); }
void S_SkyAtmosphere::from(EngineObjects& o, const Value& v, kern::SkyAtmosphere& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.fog_color); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.sun_color); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.sun_direction); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.sun_scatter); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.horizon); }
Value S_SkyState::to(EngineObjects& o, const kern::SkyState& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.sun), ev::Conv<ev::Vector>::to(o, x.moon), ev::Conv<ev::Float>::to(o, x.night), ev::Conv<ev::Bool>::to(o, x.clouds), ev::Conv<ev::Vector>::to(o, x.blast_direction), ev::Conv<ev::Float>::to(o, x.blast_age), ev::Conv<ev::Vector>::to(o, x.wind)}); }
void S_SkyState::from(EngineObjects& o, const Value& v, kern::SkyState& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.sun); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.moon); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.night); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.clouds); if (const Value* f = v.field(4)) ev::Conv<ev::Vector>::from(o, *f, x.blast_direction); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.blast_age); if (const Value* f = v.field(6)) ev::Conv<ev::Vector>::from(o, *f, x.wind); }
Value S_Sphere::to(EngineObjects& o, const kern::Sphere& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.center), ev::Conv<ev::Float>::to(o, x.radius)}); }
void S_Sphere::from(EngineObjects& o, const Value& v, kern::Sphere& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.center); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.radius); }
Value S_Vec4::to(EngineObjects& o, const kern::Vec4& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.x), ev::Conv<ev::Float>::to(o, x.y), ev::Conv<ev::Float>::to(o, x.z), ev::Conv<ev::Float>::to(o, x.w)}); }
void S_Vec4::from(EngineObjects& o, const Value& v, kern::Vec4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.x); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.y); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.z); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.w); }
Value S_Viewport::to(EngineObjects& o, const kern::Viewport& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.x), ev::Conv<ev::Int>::to(o, x.y), ev::Conv<ev::Int>::to(o, x.width), ev::Conv<ev::Int>::to(o, x.height)}); }
void S_Viewport::from(EngineObjects& o, const Value& v, kern::Viewport& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.x); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.y); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.width); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.height); }

// engine.mesh.skinned: kern::Mesh::skinned
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  decltype(auto) r = self->skinned();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.mesh.cull_mode: kern::Mesh::cull_mode
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  decltype(auto) r = self->cull_mode();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.mesh.vertex_count: kern::Mesh::vertex_count
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  decltype(auto) r = self->vertex_count();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.mesh.element_count: kern::Mesh::element_count
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  decltype(auto) r = self->element_count();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.mesh.triangle_count: kern::Mesh::triangle_count
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  decltype(auto) r = self->triangle_count();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.mesh.posed_vertex: kern::Mesh::posed_vertex
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->posed_vertex(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.bounding_sphere: kern::bounding_sphere
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::vector<float> a0;
  ev::take<ev::List<ev::Float>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<3u>>>();
  ev::take<ev::Int>(o, in, 1, a1);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list positions has fewer entries than vertex_count says"; return false; }
  decltype(auto) r = kern::bounding_sphere(a0.empty() ? nullptr : a0.data(), std::move(a1));
  ev::give<ev::Struct<S_Sphere>>(o, out, 0, r);
  return true;
}

// engine.mirror_state: kern::mirror_state
bool c7(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::vector<float> a0;
  ev::take<ev::List<ev::Float>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<16u>>>();
  ev::take<ev::Int>(o, in, 1, a1);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list transforms has fewer entries than instance_count says"; return false; }
  decltype(auto) r = kern::mirror_state(a0.empty() ? nullptr : a0.data(), std::move(a1));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.apply_cull_mode: kern::apply_cull_mode
bool c8(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 0, a0);
  kern::apply_cull_mode(std::move(a0));
  return true;
}

// engine.instanced_bounds: kern::instanced_bounds
bool c9(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Sphere const&>>();
  ev::take<ev::Struct<S_Sphere>>(o, in, 0, a0);
  std::vector<float> a1;
  ev::take<ev::List<ev::Float>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Count<16u>>>();
  ev::take<ev::Int>(o, in, 2, a2);
  if (ev::numbers(a2) > a1.size()) { o.error = "the list transforms has fewer entries than instance_count says"; return false; }
  decltype(auto) r = kern::instanced_bounds(a0, a1.empty() ? nullptr : a1.data(), std::move(a2));
  ev::give<ev::Struct<S_Sphere>>(o, out, 0, r);
  return true;
}

// engine.particles.init: kern::Particles::init
bool c10(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Particles>(o, in, 0, "particles");
  if (!self) return false;
  kern::Renderer* a0 = ev::object<kern::Renderer>(o, in, 1, "renderer");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 3, a2);
  decltype(auto) r = self->init(*a0, std::move(a1), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.particles.emit: kern::Particles::emit
bool c11(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Particles>(o, in, 0, "particles");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::ParticleSpec const&>>();
  ev::take<ev::Struct<S_ParticleSpec>>(o, in, 1, a0);
  decltype(auto) r = self->emit(a0);
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.particles.tick: kern::Particles::tick
bool c12(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Particles>(o, in, 0, "particles");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  decltype(auto) r = self->tick(std::move(a0), std::move(a1));
  ev::give<ev::Struct<S_ParticleStats>>(o, out, 0, r);
  return true;
}

// engine.particles.clear: kern::Particles::clear
bool c13(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Particles>(o, in, 0, "particles");
  if (!self) return false;
  self->clear();
  return true;
}

// engine.particles.mesh_id: kern::Particles::mesh_id
bool c14(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Particles>(o, in, 0, "particles");
  if (!self) return false;
  decltype(auto) r = self->mesh_id();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.particles.active: kern::Particles::active
bool c15(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Particles>(o, in, 0, "particles");
  if (!self) return false;
  decltype(auto) r = self->active();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.particles.stats: kern::Particles::stats
bool c16(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Particles>(o, in, 0, "particles");
  if (!self) return false;
  decltype(auto) r = self->stats();
  ev::give<ev::Struct<S_ParticleStats>>(o, out, 0, r);
  return true;
}

// engine.light_pool.request: kern::LightPool::request
bool c17(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::LightPool>(o, in, 0, "light_pool");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a3);
  decltype(auto) r = self->request(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.light_pool.resolve: kern::LightPool::resolve
bool c18(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::LightPool>(o, in, 0, "light_pool");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  decltype(auto) r = self->resolve(std::move(a0));
  ev::give<ev::Struct<S_PointLights>>(o, out, 0, r);
  return true;
}

// engine.light_pool.clear: kern::LightPool::clear
bool c19(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::LightPool>(o, in, 0, "light_pool");
  if (!self) return false;
  self->clear();
  return true;
}

// engine.sun_shadow_map.init: kern::SunShadowMap::init
bool c20(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SunShadowMap>(o, in, 0, "sun_shadow_map");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::ShadowOptions const&>>();
  ev::take<ev::Struct<S_ShadowOptions>>(o, in, 1, a0);
  decltype(auto) r = self->init(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.sun_shadow_map.destroy: kern::SunShadowMap::destroy
bool c21(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SunShadowMap>(o, in, 0, "sun_shadow_map");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->destroy(std::move(a0));
  return true;
}

// engine.sun_shadow_map.valid: kern::SunShadowMap::valid
bool c22(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SunShadowMap>(o, in, 0, "sun_shadow_map");
  if (!self) return false;
  decltype(auto) r = self->valid();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.sun_shadow_map.focus_key: kern::SunShadowMap::focus_key
bool c23(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SunShadowMap>(o, in, 0, "sun_shadow_map");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  decltype(auto) r = self->focus_key(std::move(a0), std::move(a1), std::move(a2));
  ev::give<ev::Int64>(o, out, 0, r);
  return true;
}

// engine.sun_shadow_map.draw: kern::SunShadowMap::draw
bool c24(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SunShadowMap>(o, in, 0, "sun_shadow_map");
  if (!self) return false;
  std::vector<kern::Mesh*> a0;
  ev::take<ev::List<ev::Int>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 5, a4);
  decltype(auto) r = self->draw(kern::Span<kern::Mesh* const>(a0.data(), a0.size()), std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.sun_shadow_map.matrix: kern::SunShadowMap::matrix
bool c25(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SunShadowMap>(o, in, 0, "sun_shadow_map");
  if (!self) return false;
  decltype(auto) r = self->matrix();
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

// engine.sun_shadow_map.depth_texture: kern::SunShadowMap::depth_texture
bool c26(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SunShadowMap>(o, in, 0, "sun_shadow_map");
  if (!self) return false;
  decltype(auto) r = self->depth_texture();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.sun_shadow_map.options: kern::SunShadowMap::options
bool c27(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SunShadowMap>(o, in, 0, "sun_shadow_map");
  if (!self) return false;
  decltype(auto) r = self->options();
  ev::give<ev::Struct<S_ShadowOptions>>(o, out, 0, r);
  return true;
}

// engine.sun_shadow_map.set_filter: kern::SunShadowMap::set_filter
bool c28(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SunShadowMap>(o, in, 0, "sun_shadow_map");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->set_filter(std::move(a0));
  return true;
}

// engine.sun_shadow_map.extent: kern::SunShadowMap::extent
bool c29(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SunShadowMap>(o, in, 0, "sun_shadow_map");
  if (!self) return false;
  decltype(auto) r = self->extent();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.sky_pass.init: kern::SkyPass::init
bool c30(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SkyPass>(o, in, 0, "sky_pass");
  if (!self) return false;
  decltype(auto) r = self->init();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.sky_pass.destroy: kern::SkyPass::destroy
bool c31(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SkyPass>(o, in, 0, "sky_pass");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->destroy(std::move(a0));
  return true;
}

// engine.sky_pass.draw: kern::SkyPass::draw
bool c32(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::SkyPass>(o, in, 0, "sky_pass");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Camera const&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::SkyState const&>>();
  ev::take<ev::Struct<S_SkyState>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 5, a4);
  auto a5 = ev::blank<std::remove_cvref_t<kern::SkyAtmosphere const&>>();
  ev::take<ev::Struct<S_SkyAtmosphere>>(o, in, 6, a5);
  self->draw(a0, std::move(a1), a2, std::move(a3), std::move(a4), a5);
  return true;
}

// engine.bloom_pass.configure: kern::BloomPass::configure
bool c33(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BloomPass>(o, in, 0, "bloom_pass");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::BloomOptions const&>>();
  ev::take<ev::Struct<S_BloomOptions>>(o, in, 1, a0);
  self->configure(a0);
  return true;
}

// engine.bloom_pass.begin: kern::BloomPass::begin
bool c34(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BloomPass>(o, in, 0, "bloom_pass");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 4, a3);
  decltype(auto) r = self->begin(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.bloom_pass.end: kern::BloomPass::end
bool c35(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BloomPass>(o, in, 0, "bloom_pass");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  std::vector<kern::BloomPass::Occlusion> a1;
  ev::take<ev::List<ev::Struct<S_BloomPass_Occlusion>>>(o, in, 2, a1);
  self->end(std::move(a0), a1.empty() ? nullptr : a1.data());
  return true;
}

// engine.bloom_pass.destroy: kern::BloomPass::destroy
bool c36(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BloomPass>(o, in, 0, "bloom_pass");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->destroy(std::move(a0));
  return true;
}

// engine.bloom_pass.state: kern::BloomPass::state
bool c37(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BloomPass>(o, in, 0, "bloom_pass");
  if (!self) return false;
  decltype(auto) r = self->state();
  ev::give<ev::Enum<E_BloomState>>(o, out, 0, r);
  return true;
}

// engine.bloom_pass.depth_texture: kern::BloomPass::depth_texture
bool c38(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BloomPass>(o, in, 0, "bloom_pass");
  if (!self) return false;
  decltype(auto) r = self->depth_texture();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.bloom_pass.bind_scene: kern::BloomPass::bind_scene
bool c39(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::BloomPass>(o, in, 0, "bloom_pass");
  if (!self) return false;
  self->bind_scene();
  return true;
}

// engine.ambient_occlusion_pass.configure: kern::AmbientOcclusionPass::configure
bool c40(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AmbientOcclusionPass>(o, in, 0, "ambient_occlusion_pass");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::AoOptions const&>>();
  ev::take<ev::Struct<S_AoOptions>>(o, in, 1, a0);
  self->configure(a0);
  return true;
}

// engine.ambient_occlusion_pass.set_radius: kern::AmbientOcclusionPass::set_radius
bool c41(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AmbientOcclusionPass>(o, in, 0, "ambient_occlusion_pass");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->set_radius(std::move(a0));
  return true;
}

// engine.ambient_occlusion_pass.options: kern::AmbientOcclusionPass::options
bool c42(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AmbientOcclusionPass>(o, in, 0, "ambient_occlusion_pass");
  if (!self) return false;
  decltype(auto) r = self->options();
  ev::give<ev::Struct<S_AoOptions>>(o, out, 0, r);
  return true;
}

// engine.ambient_occlusion_pass.compute: kern::AmbientOcclusionPass::compute
bool c43(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AmbientOcclusionPass>(o, in, 0, "ambient_occlusion_pass");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::Mat4 const&>>();
  ev::take<ev::Struct<S_Mat4>>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 5, a4);
  decltype(auto) r = self->compute(std::move(a0), std::move(a1), std::move(a2), a3, std::move(a4));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.ambient_occlusion_pass.texture: kern::AmbientOcclusionPass::texture
bool c44(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AmbientOcclusionPass>(o, in, 0, "ambient_occlusion_pass");
  if (!self) return false;
  decltype(auto) r = self->texture();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.ambient_occlusion_pass.width: kern::AmbientOcclusionPass::width
bool c45(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AmbientOcclusionPass>(o, in, 0, "ambient_occlusion_pass");
  if (!self) return false;
  decltype(auto) r = self->width();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.ambient_occlusion_pass.height: kern::AmbientOcclusionPass::height
bool c46(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AmbientOcclusionPass>(o, in, 0, "ambient_occlusion_pass");
  if (!self) return false;
  decltype(auto) r = self->height();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.ambient_occlusion_pass.destroy: kern::AmbientOcclusionPass::destroy
bool c47(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AmbientOcclusionPass>(o, in, 0, "ambient_occlusion_pass");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->destroy(std::move(a0));
  return true;
}

// engine.ambient_occlusion_pass.quality: kern::AmbientOcclusionPass::quality
bool c48(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AmbientOcclusionPass>(o, in, 0, "ambient_occlusion_pass");
  if (!self) return false;
  decltype(auto) r = self->quality();
  ev::give<ev::Enum<E_AoQuality>>(o, out, 0, r);
  return true;
}

// engine.ambient_occlusion_pass.half_resolution: kern::AmbientOcclusionPass::half_resolution
bool c49(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AmbientOcclusionPass>(o, in, 0, "ambient_occlusion_pass");
  if (!self) return false;
  decltype(auto) r = self->half_resolution();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.program.build: kern::Program::build
bool c50(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Program>(o, in, 0, "program");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  std::string a2;
  ev::take<ev::Text>(o, in, 3, a2);
  decltype(auto) r = self->build(a0, a1, a2.c_str());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.program.destroy: kern::Program::destroy
bool c51(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Program>(o, in, 0, "program");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->destroy(std::move(a0));
  return true;
}

// engine.program.handle: kern::Program::handle
bool c52(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Program>(o, in, 0, "program");
  if (!self) return false;
  decltype(auto) r = self->handle();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.program.valid: kern::Program::valid
bool c53(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Program>(o, in, 0, "program");
  if (!self) return false;
  decltype(auto) r = self->valid();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.program.location: kern::Program::location
bool c54(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Program>(o, in, 0, "program");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->location(a0.c_str());
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.program.use: kern::Program::use
bool c55(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Program>(o, in, 0, "program");
  if (!self) return false;
  self->use();
  return true;
}

// engine.uniform_cache.reset: kern::UniformCache::reset
bool c56(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::UniformCache>(o, in, 0, "uniform_cache");
  if (!self) return false;
  self->reset();
  return true;
}

// engine.uniform_cache.set_int: kern::UniformCache::set_int
bool c57(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::UniformCache>(o, in, 0, "uniform_cache");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  self->set_int(std::move(a0), std::move(a1));
  return true;
}

// engine.uniform_cache.set_float: kern::UniformCache::set_float
bool c58(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::UniformCache>(o, in, 0, "uniform_cache");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  self->set_float(std::move(a0), std::move(a1));
  return true;
}

// engine.uniform_cache.set_vec2: kern::UniformCache::set_vec2
bool c59(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::UniformCache>(o, in, 0, "uniform_cache");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  self->set_vec2(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.uniform_cache.set_vec3: kern::UniformCache::set_vec3
bool c60(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::UniformCache>(o, in, 0, "uniform_cache");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  self->set_vec3(std::move(a0), std::move(a1));
  return true;
}

// engine.uniform_cache.set_vec4: kern::UniformCache::set_vec4
bool c61(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::UniformCache>(o, in, 0, "uniform_cache");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec4>>();
  ev::take<ev::Struct<S_Vec4>>(o, in, 2, a1);
  self->set_vec4(std::move(a0), std::move(a1));
  return true;
}

// engine.uniform_cache.set_vec4_array: kern::UniformCache::set_vec4_array
bool c62(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::UniformCache>(o, in, 0, "uniform_cache");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  std::vector<float> a1;
  ev::take<ev::List<ev::Float>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Count<4u>>>();
  ev::take<ev::Int>(o, in, 3, a2);
  if (ev::numbers(a2) > a1.size()) { o.error = "the list values has fewer entries than count says"; return false; }
  self->set_vec4_array(std::move(a0), a1.empty() ? nullptr : a1.data(), std::move(a2));
  return true;
}

// engine.uniform_cache.set_vec3_array: kern::UniformCache::set_vec3_array
bool c63(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::UniformCache>(o, in, 0, "uniform_cache");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  std::vector<float> a1;
  ev::take<ev::List<ev::Float>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Count<3u>>>();
  ev::take<ev::Int>(o, in, 3, a2);
  if (ev::numbers(a2) > a1.size()) { o.error = "the list values has fewer entries than count says"; return false; }
  self->set_vec3_array(std::move(a0), a1.empty() ? nullptr : a1.data(), std::move(a2));
  return true;
}

// engine.uniform_cache.set_mat4: kern::UniformCache::set_mat4
bool c64(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::UniformCache>(o, in, 0, "uniform_cache");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  std::vector<float> a1;
  ev::take<ev::List<ev::Float>>(o, in, 2, a1);
  self->set_mat4(std::move(a0), a1.empty() ? nullptr : a1.data());
  return true;
}

// engine.uniform_cache.set_mat4_array: kern::UniformCache::set_mat4_array
bool c65(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 0, a0);
  std::vector<float> a1;
  ev::take<ev::List<ev::Float>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Count<16u>>>();
  ev::take<ev::Int>(o, in, 2, a2);
  if (ev::numbers(a2) > a1.size()) { o.error = "the list values has fewer entries than n says"; return false; }
  kern::UniformCache::set_mat4_array(std::move(a0), a1.empty() ? nullptr : a1.data(), std::move(a2));
  return true;
}

// engine.renderer.init: kern::Renderer::init
bool c66(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::RendererOptions const&>>();
  ev::take<ev::Struct<S_RendererOptions>>(o, in, 1, a0);
  std::vector<kern::ShaderExtension*> a1;
  ev::take<ev::List<ev::Int>>(o, in, 2, a1);
  decltype(auto) r = self->init(a0, kern::Span<kern::ShaderExtension* const>(a1.data(), a1.size()));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.shutdown: kern::Renderer::shutdown
bool c67(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  self->shutdown();
  return true;
}

// engine.renderer.resize: kern::Renderer::resize
bool c68(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  decltype(auto) r = self->resize(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.width: kern::Renderer::width
bool c69(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->width();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.renderer.height: kern::Renderer::height
bool c70(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->height();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.renderer.allocate_id: kern::Renderer::allocate_id
bool c71(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->allocate_id();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.renderer.release_id: kern::Renderer::release_id
bool c72(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->release_id(std::move(a0));
  return true;
}

// engine.renderer.id_allocated: kern::Renderer::id_allocated
bool c73(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->id_allocated(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.upsert: kern::Renderer::upsert
bool c74(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::MeshInput const&>>();
  ev::take<ev::Struct<S_MeshInput>>(o, in, 2, a1);
  decltype(auto) r = self->upsert(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.update_instances: kern::Renderer::update_instances
bool c75(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  std::vector<float> a1;
  ev::take<ev::List<ev::Float>>(o, in, 2, a1);
  std::vector<float> a2;
  ev::take<ev::List<ev::Float>>(o, in, 3, a2);
  auto a3 = ev::blank<bool>();
  decltype(auto) r = self->update_instances(std::move(a0), kern::Span<float const>(a1.data(), a1.size()), kern::Span<float const>(a2.data(), a2.size()), &a3);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Bool>(o, out, 1, a3);
  return true;
}

// engine.renderer.update_bones: kern::Renderer::update_bones
bool c76(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  std::vector<float> a1;
  ev::take<ev::List<ev::Float>>(o, in, 2, a1);
  decltype(auto) r = self->update_bones(std::move(a0), kern::Span<float const>(a1.data(), a1.size()));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.set_visible: kern::Renderer::set_visible
bool c77(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  decltype(auto) r = self->set_visible(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.remove: kern::Renderer::remove
bool c78(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->remove(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.set_material: kern::Renderer::set_material
bool c79(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Material const&>>();
  ev::take<ev::Struct<S_Material>>(o, in, 2, a1);
  decltype(auto) r = self->set_material(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.set_layers: kern::Renderer::set_layers
bool c80(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  decltype(auto) r = self->set_layers(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.mesh: kern::Renderer::mesh
bool c81(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->mesh(std::move(a0));
  out[0] = Value::integer(ev::handle_of<kern::Mesh>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.renderer.meshes: kern::Renderer::meshes
bool c82(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->meshes();
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.renderer.mesh_count: kern::Renderer::mesh_count
bool c83(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->mesh_count();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.renderer.create_texture: kern::Renderer::create_texture
bool c84(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  kern::TextureDesc* a0 = ev::object<kern::TextureDesc>(o, in, 1, "desc");
  if (!a0) return false;
  decltype(auto) r = self->create_texture(*a0);
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.renderer.register_external_texture: kern::Renderer::register_external_texture
bool c85(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->register_external_texture(std::move(a0));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.renderer.create_render_target: kern::Renderer::create_render_target
bool c86(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int&>>();
  ev::take<ev::Int>(o, in, 2, a1);
  decltype(auto) r = self->create_render_target(std::move(a0), a1);
  ev::give<ev::Int>(o, out, 0, r);
  ev::give<ev::Int>(o, out, 1, a1);
  return true;
}

// engine.renderer.delete_texture: kern::Renderer::delete_texture
bool c87(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->delete_texture(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.camera: kern::Renderer::camera
bool c88(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->camera();
  ev::give<ev::Struct<S_Camera>>(o, out, 0, r);
  return true;
}

// engine.renderer.set_camera: kern::Renderer::set_camera
bool c89(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Camera const&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 1, a0);
  self->set_camera(a0);
  return true;
}

// engine.renderer.lighting: kern::Renderer::lighting
bool c90(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->lighting();
  ev::give<ev::Struct<S_Lighting>>(o, out, 0, r);
  return true;
}

// engine.renderer.set_lighting: kern::Renderer::set_lighting
bool c91(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Lighting const&>>();
  ev::take<ev::Struct<S_Lighting>>(o, in, 1, a0);
  self->set_lighting(a0);
  return true;
}

// engine.renderer.sky: kern::Renderer::sky
bool c92(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->sky();
  ev::give<ev::Struct<S_SkyState>>(o, out, 0, r);
  return true;
}

// engine.renderer.set_sky: kern::Renderer::set_sky
bool c93(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::SkyState const&>>();
  ev::take<ev::Struct<S_SkyState>>(o, in, 1, a0);
  self->set_sky(a0);
  return true;
}

// engine.renderer.set_point_lights: kern::Renderer::set_point_lights
bool c94(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::PointLights const&>>();
  ev::take<ev::Struct<S_PointLights>>(o, in, 1, a0);
  decltype(auto) r = self->set_point_lights(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.point_lights: kern::Renderer::point_lights
bool c95(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->point_lights();
  ev::give<ev::Struct<S_PointLights>>(o, out, 0, r);
  return true;
}

// engine.renderer.set_shadows: kern::Renderer::set_shadows
bool c96(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  std::vector<kern::ShadowOptions> a0;
  ev::take<ev::List<ev::Struct<S_ShadowOptions>>>(o, in, 1, a0);
  decltype(auto) r = self->set_shadows(a0.empty() ? nullptr : a0.data());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.set_shadow_distance: kern::Renderer::set_shadow_distance
bool c97(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->set_shadow_distance(std::move(a0));
  return true;
}

// engine.renderer.set_shadow_filter: kern::Renderer::set_shadow_filter
bool c98(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->set_shadow_filter(std::move(a0));
  return true;
}

// engine.renderer.set_ambient_occlusion: kern::Renderer::set_ambient_occlusion
bool c99(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::AoOptions const&>>();
  ev::take<ev::Struct<S_AoOptions>>(o, in, 1, a0);
  self->set_ambient_occlusion(a0);
  return true;
}

// engine.renderer.set_ambient_occlusion_radius: kern::Renderer::set_ambient_occlusion_radius
bool c100(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->set_ambient_occlusion_radius(std::move(a0));
  return true;
}

// engine.renderer.ambient_occlusion: kern::Renderer::ambient_occlusion
bool c101(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->ambient_occlusion();
  out[0] = Value::integer(ev::handle_of<kern::AmbientOcclusionPass>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.renderer.shadow: kern::Renderer::shadow
bool c102(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->shadow();
  out[0] = Value::integer(ev::handle_of<kern::SunShadowMap>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.renderer.invalidate: kern::Renderer::invalidate
bool c103(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->invalidate(std::move(a0));
  return true;
}

// engine.renderer.scene_revision: kern::Renderer::scene_revision
bool c104(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->scene_revision();
  ev::give<ev::Int64>(o, out, 0, r);
  return true;
}

// engine.renderer.render: kern::Renderer::render
bool c105(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::RenderParams const&>>();
  ev::take<ev::Struct<S_RenderParams>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::RenderStats&>>();
  ev::take<ev::Struct<S_RenderStats>>(o, in, 2, a1);
  decltype(auto) r = self->render(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_RenderStats>>(o, out, 1, a1);
  return true;
}

// engine.renderer.last_stats: kern::Renderer::last_stats
bool c106(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->last_stats();
  ev::give<ev::Struct<S_RenderStats>>(o, out, 0, r);
  return true;
}

// engine.renderer.screen_ray: kern::Renderer::screen_ray
bool c107(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Ray&>>();
  ev::take<ev::Struct<S_Ray>>(o, in, 3, a2);
  decltype(auto) r = self->screen_ray(std::move(a0), std::move(a1), a2);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_Ray>>(o, out, 1, a2);
  return true;
}

// engine.renderer.pick: kern::Renderer::pick
bool c108(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::PickQuery const&>>();
  ev::take<ev::Struct<S_PickQuery>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::PickHit&>>();
  ev::take<ev::Struct<S_PickHit>>(o, in, 4, a3);
  auto a4 = ev::blank<kern::PickStats>();
  decltype(auto) r = self->pick(std::move(a0), std::move(a1), a2, a3, &a4);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_PickHit>>(o, out, 1, a3);
  ev::give<ev::Struct<S_PickStats>>(o, out, 2, a4);
  return true;
}

// engine.renderer.pick_ray: kern::Renderer::pick_ray
bool c109(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Ray const&>>();
  ev::take<ev::Struct<S_Ray>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::PickQuery const&>>();
  ev::take<ev::Struct<S_PickQuery>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::PickHit&>>();
  ev::take<ev::Struct<S_PickHit>>(o, in, 3, a2);
  auto a3 = ev::blank<kern::PickStats>();
  decltype(auto) r = self->pick_ray(a0, a1, a2, &a3);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_PickHit>>(o, out, 1, a2);
  ev::give<ev::Struct<S_PickStats>>(o, out, 2, a3);
  return true;
}

// engine.renderer.context_lost: kern::Renderer::context_lost
bool c110(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  self->context_lost();
  return true;
}

// engine.renderer.context_restored: kern::Renderer::context_restored
bool c111(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->context_restored();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.lost: kern::Renderer::lost
bool c112(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->lost();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.renderer.set_headless: kern::Renderer::set_headless
bool c113(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  self->set_headless();
  return true;
}

// engine.renderer.extensions: kern::Renderer::extensions
bool c114(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!self) return false;
  decltype(auto) r = self->extensions();
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.shader_extension.name: kern::ShaderExtension::name
bool c115(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  decltype(auto) r = self->name();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.shader_extension.attributes: kern::ShaderExtension::attributes
bool c116(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  decltype(auto) r = self->attributes();
  ev::give<ev::List<ev::Struct<S_ExtensionAttribute>>>(o, out, 0, r);
  return true;
}

// engine.shader_extension.vertex_declarations: kern::ShaderExtension::vertex_declarations
bool c117(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  decltype(auto) r = self->vertex_declarations();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.shader_extension.vertex: kern::ShaderExtension::vertex
bool c118(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  decltype(auto) r = self->vertex();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.shader_extension.fragment_declarations: kern::ShaderExtension::fragment_declarations
bool c119(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  decltype(auto) r = self->fragment_declarations();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.shader_extension.fragment_discard: kern::ShaderExtension::fragment_discard
bool c120(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  decltype(auto) r = self->fragment_discard();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.shader_extension.fragment_input: kern::ShaderExtension::fragment_input
bool c121(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  decltype(auto) r = self->fragment_input();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.shader_extension.fragment_surface: kern::ShaderExtension::fragment_surface
bool c122(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  decltype(auto) r = self->fragment_surface();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.shader_extension.bind: kern::ShaderExtension::bind
bool c123(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  kern::Program* a0 = ev::object<kern::Program>(o, in, 1, "program");
  if (!a0) return false;
  self->bind(*a0);
  return true;
}

// engine.shader_extension.frame_uniforms: kern::ShaderExtension::frame_uniforms
bool c124(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  kern::UniformCache* a0 = ev::object<kern::UniformCache>(o, in, 1, "cache");
  if (!a0) return false;
  kern::Renderer* a1 = ev::object<kern::Renderer>(o, in, 2, "renderer");
  if (!a1) return false;
  self->frame_uniforms(*a0, *a1);
  return true;
}

// engine.shader_extension.material_uniforms: kern::ShaderExtension::material_uniforms
bool c125(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  kern::UniformCache* a0 = ev::object<kern::UniformCache>(o, in, 1, "cache");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::Material const&>>();
  ev::take<ev::Struct<S_Material>>(o, in, 2, a1);
  std::vector<int> a2;
  ev::take<ev::List<ev::Int>>(o, in, 3, a2);
  self->material_uniforms(*a0, a1, a2.empty() ? nullptr : a2.data());
  return true;
}

// engine.build_standard_shader: kern::build_standard_shader
bool c126(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::vector<kern::ShaderExtension*> a0;
  ev::take<ev::List<ev::Int>>(o, in, 0, a0);
  decltype(auto) r = kern::build_standard_shader(kern::Span<kern::ShaderExtension* const>(a0.data(), a0.size()));
  ev::give<ev::Struct<S_ShaderSource>>(o, out, 0, r);
  return true;
}

// engine.ambient_occlusion_pass.make: kern::AmbientOcclusionPass::AmbientOcclusionPass
bool c127(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::AmbientOcclusionPass>(std::unique_ptr<kern::AmbientOcclusionPass>(new kern::AmbientOcclusionPass())));
  return true;
}

// engine.ambient_occlusion_pass.release: kern::AmbientOcclusionPass::~AmbientOcclusionPass
bool c128(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::AmbientOcclusionPass>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no AmbientOcclusionPass that was made";
    return false;
  }
  return true;
}

// engine.bloom_pass.make: kern::BloomPass::BloomPass
bool c129(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::BloomPass>(std::unique_ptr<kern::BloomPass>(new kern::BloomPass())));
  return true;
}

// engine.bloom_pass.release: kern::BloomPass::~BloomPass
bool c130(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::BloomPass>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no BloomPass that was made";
    return false;
  }
  return true;
}

// engine.light_pool.make: kern::LightPool::LightPool
bool c131(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a1);
  out[0] = Value::integer(o.own<kern::LightPool>(std::unique_ptr<kern::LightPool>(new kern::LightPool(std::move(a0), std::move(a1)))));
  return true;
}

// engine.light_pool.release: kern::LightPool::~LightPool
bool c132(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::LightPool>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no LightPool that was made";
    return false;
  }
  return true;
}

// engine.mesh.make: kern::Mesh::Mesh
bool c133(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::Mesh>(std::unique_ptr<kern::Mesh>(new kern::Mesh())));
  return true;
}

// engine.mesh.release: kern::Mesh::~Mesh
bool c134(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::Mesh>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no Mesh that was made";
    return false;
  }
  return true;
}

// engine.mesh.id: kern::Mesh::id
bool c135(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->id);
  return true;
}

// engine.mesh.id@set: what stands at the place kern::Mesh::id gives
bool c135_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->id);
  return true;
}

// engine.mesh.visible: kern::Mesh::visible
bool c136(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Bool>(o, out, 0, self->visible);
  return true;
}

// engine.mesh.visible@set: what stands at the place kern::Mesh::visible gives
bool c136_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Bool>(o, in, 1, self->visible);
  return true;
}

// engine.mesh.layers: kern::Mesh::layers
bool c137(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->layers);
  return true;
}

// engine.mesh.layers@set: what stands at the place kern::Mesh::layers gives
bool c137_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->layers);
  return true;
}

// engine.mesh.pick_bvh: kern::Mesh::pick_bvh
bool c138(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Bool>(o, out, 0, self->pick_bvh);
  return true;
}

// engine.mesh.pick_bvh@set: what stands at the place kern::Mesh::pick_bvh gives
bool c138_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Bool>(o, in, 1, self->pick_bvh);
  return true;
}

// engine.mesh.linear_colors: kern::Mesh::linear_colors
bool c139(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Bool>(o, out, 0, self->linear_colors);
  return true;
}

// engine.mesh.linear_colors@set: what stands at the place kern::Mesh::linear_colors gives
bool c139_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Bool>(o, in, 1, self->linear_colors);
  return true;
}

// engine.mesh.material: kern::Mesh::material
bool c140(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Struct<S_Material>>(o, out, 0, self->material);
  return true;
}

// engine.mesh.material@set: what stands at the place kern::Mesh::material gives
bool c140_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Struct<S_Material>>(o, in, 1, self->material);
  return true;
}

// engine.mesh.texture: kern::Mesh::texture
bool c141(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->texture);
  return true;
}

// engine.mesh.texture@set: what stands at the place kern::Mesh::texture gives
bool c141_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->texture);
  return true;
}

// engine.mesh.positions: kern::Mesh::positions
bool c142(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::List<ev::Float>>(o, out, 0, self->positions);
  return true;
}

// engine.mesh.positions@set: what stands at the place kern::Mesh::positions gives
bool c142_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::List<ev::Float>>(o, in, 1, self->positions);
  return true;
}

// engine.mesh.normals: kern::Mesh::normals
bool c143(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::List<ev::Float>>(o, out, 0, self->normals);
  return true;
}

// engine.mesh.normals@set: what stands at the place kern::Mesh::normals gives
bool c143_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::List<ev::Float>>(o, in, 1, self->normals);
  return true;
}

// engine.mesh.colors: kern::Mesh::colors
bool c144(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::List<ev::Float>>(o, out, 0, self->colors);
  return true;
}

// engine.mesh.colors@set: what stands at the place kern::Mesh::colors gives
bool c144_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::List<ev::Float>>(o, in, 1, self->colors);
  return true;
}

// engine.mesh.uvs: kern::Mesh::uvs
bool c145(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::List<ev::Float>>(o, out, 0, self->uvs);
  return true;
}

// engine.mesh.uvs@set: what stands at the place kern::Mesh::uvs gives
bool c145_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::List<ev::Float>>(o, in, 1, self->uvs);
  return true;
}

// engine.mesh.indices: kern::Mesh::indices
bool c146(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::List<ev::Int>>(o, out, 0, self->indices);
  return true;
}

// engine.mesh.indices@set: what stands at the place kern::Mesh::indices gives
bool c146_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::List<ev::Int>>(o, in, 1, self->indices);
  return true;
}

// engine.mesh.joints: kern::Mesh::joints
bool c147(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::List<ev::Float>>(o, out, 0, self->joints);
  return true;
}

// engine.mesh.joints@set: what stands at the place kern::Mesh::joints gives
bool c147_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::List<ev::Float>>(o, in, 1, self->joints);
  return true;
}

// engine.mesh.weights: kern::Mesh::weights
bool c148(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::List<ev::Float>>(o, out, 0, self->weights);
  return true;
}

// engine.mesh.weights@set: what stands at the place kern::Mesh::weights gives
bool c148_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::List<ev::Float>>(o, in, 1, self->weights);
  return true;
}

// engine.mesh.bones: kern::Mesh::bones
bool c149(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::List<ev::Float>>(o, out, 0, self->bones);
  return true;
}

// engine.mesh.bones@set: what stands at the place kern::Mesh::bones gives
bool c149_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::List<ev::Float>>(o, in, 1, self->bones);
  return true;
}

// engine.mesh.transforms: kern::Mesh::transforms
bool c150(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::List<ev::Float>>(o, out, 0, self->transforms);
  return true;
}

// engine.mesh.transforms@set: what stands at the place kern::Mesh::transforms gives
bool c150_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::List<ev::Float>>(o, in, 1, self->transforms);
  return true;
}

// engine.mesh.tints: kern::Mesh::tints
bool c151(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::List<ev::Float>>(o, out, 0, self->tints);
  return true;
}

// engine.mesh.tints@set: what stands at the place kern::Mesh::tints gives
bool c151_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::List<ev::Float>>(o, in, 1, self->tints);
  return true;
}

// engine.mesh.instance_normals: kern::Mesh::instance_normals
bool c152(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::List<ev::Float>>(o, out, 0, self->instance_normals);
  return true;
}

// engine.mesh.instance_normals@set: what stands at the place kern::Mesh::instance_normals gives
bool c152_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::List<ev::Float>>(o, in, 1, self->instance_normals);
  return true;
}

// engine.mesh.instance_count: kern::Mesh::instance_count
bool c153(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->instance_count);
  return true;
}

// engine.mesh.instance_count@set: what stands at the place kern::Mesh::instance_count gives
bool c153_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->instance_count);
  return true;
}

// engine.mesh.mirror: kern::Mesh::mirror
bool c154(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->mirror);
  return true;
}

// engine.mesh.mirror@set: what stands at the place kern::Mesh::mirror gives
bool c154_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->mirror);
  return true;
}

// engine.mesh.local_bounds: kern::Mesh::local_bounds
bool c155(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Struct<S_Sphere>>(o, out, 0, self->local_bounds);
  return true;
}

// engine.mesh.local_bounds@set: what stands at the place kern::Mesh::local_bounds gives
bool c155_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Struct<S_Sphere>>(o, in, 1, self->local_bounds);
  return true;
}

// engine.mesh.world_bounds: kern::Mesh::world_bounds
bool c156(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Struct<S_Sphere>>(o, out, 0, self->world_bounds);
  return true;
}

// engine.mesh.world_bounds@set: what stands at the place kern::Mesh::world_bounds gives
bool c156_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Struct<S_Sphere>>(o, in, 1, self->world_bounds);
  return true;
}

// engine.mesh.gpu: kern::Mesh::gpu
bool c157(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Struct<S_MeshGpu>>(o, out, 0, self->gpu);
  return true;
}

// engine.mesh.gpu@set: what stands at the place kern::Mesh::gpu gives
bool c157_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Struct<S_MeshGpu>>(o, in, 1, self->gpu);
  return true;
}

// engine.mesh.sort_distance: kern::Mesh::sort_distance
bool c158(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Float>(o, out, 0, self->sort_distance);
  return true;
}

// engine.mesh.sort_distance@set: what stands at the place kern::Mesh::sort_distance gives
bool c158_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Float>(o, in, 1, self->sort_distance);
  return true;
}

// engine.mesh.live_index: kern::Mesh::live_index
bool c159(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->live_index);
  return true;
}

// engine.mesh.live_index@set: what stands at the place kern::Mesh::live_index gives
bool c159_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::Mesh>(o, in, 0, "mesh");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->live_index);
  return true;
}

// engine.particles.make: kern::Particles::Particles
bool c160(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::Particles>(std::unique_ptr<kern::Particles>(new kern::Particles())));
  return true;
}

// engine.particles.release: kern::Particles::~Particles
bool c161(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::Particles>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no Particles that was made";
    return false;
  }
  return true;
}

// engine.program.make: kern::Program::Program
bool c162(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::Program>(std::unique_ptr<kern::Program>(new kern::Program())));
  return true;
}

// engine.program.release: kern::Program::~Program
bool c163(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::Program>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no Program that was made";
    return false;
  }
  return true;
}

// engine.renderer.make: kern::Renderer::Renderer
bool c164(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::Renderer>(std::unique_ptr<kern::Renderer>(new kern::Renderer())));
  return true;
}

// engine.renderer.release: kern::Renderer::~Renderer
bool c165(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::Renderer>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no Renderer that was made";
    return false;
  }
  return true;
}

// engine.shader_extension.material_base: kern::ShaderExtension::material_base
bool c166(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->material_base);
  return true;
}

// engine.shader_extension.material_base@set: what stands at the place kern::ShaderExtension::material_base gives
bool c166_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->material_base);
  return true;
}

// engine.shader_extension.attribute_base: kern::ShaderExtension::attribute_base
bool c167(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->attribute_base);
  return true;
}

// engine.shader_extension.attribute_base@set: what stands at the place kern::ShaderExtension::attribute_base gives
bool c167_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::ShaderExtension>(o, in, 0, "shader_extension");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->attribute_base);
  return true;
}

// engine.sky_pass.make: kern::SkyPass::SkyPass
bool c168(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::SkyPass>(std::unique_ptr<kern::SkyPass>(new kern::SkyPass())));
  return true;
}

// engine.sky_pass.release: kern::SkyPass::~SkyPass
bool c169(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::SkyPass>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no SkyPass that was made";
    return false;
  }
  return true;
}

// engine.sun_shadow_map.make: kern::SunShadowMap::SunShadowMap
bool c170(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::SunShadowMap>(std::unique_ptr<kern::SunShadowMap>(new kern::SunShadowMap())));
  return true;
}

// engine.sun_shadow_map.release: kern::SunShadowMap::~SunShadowMap
bool c171(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::SunShadowMap>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no SunShadowMap that was made";
    return false;
  }
  return true;
}

// engine.texture_desc.make: kern::TextureDesc::TextureDesc
bool c172(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::TextureDesc>(std::unique_ptr<kern::TextureDesc>(new kern::TextureDesc())));
  return true;
}

// engine.texture_desc.release: kern::TextureDesc::~TextureDesc
bool c173(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::TextureDesc>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no TextureDesc that was made";
    return false;
  }
  return true;
}

// engine.texture_desc.width: kern::TextureDesc::width
bool c174(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::TextureDesc>(o, in, 0, "texture_desc");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->width);
  return true;
}

// engine.texture_desc.width@set: what stands at the place kern::TextureDesc::width gives
bool c174_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::TextureDesc>(o, in, 0, "texture_desc");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->width);
  return true;
}

// engine.texture_desc.height: kern::TextureDesc::height
bool c175(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::TextureDesc>(o, in, 0, "texture_desc");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->height);
  return true;
}

// engine.texture_desc.height@set: what stands at the place kern::TextureDesc::height gives
bool c175_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::TextureDesc>(o, in, 0, "texture_desc");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->height);
  return true;
}

// engine.texture_desc.mipmaps: kern::TextureDesc::mipmaps
bool c176(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::TextureDesc>(o, in, 0, "texture_desc");
  if (!self) return false;
  ev::give<ev::Bool>(o, out, 0, self->mipmaps);
  return true;
}

// engine.texture_desc.mipmaps@set: what stands at the place kern::TextureDesc::mipmaps gives
bool c176_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::TextureDesc>(o, in, 0, "texture_desc");
  if (!self) return false;
  ev::take<ev::Bool>(o, in, 1, self->mipmaps);
  return true;
}

// engine.texture_desc.repeat: kern::TextureDesc::repeat
bool c177(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::TextureDesc>(o, in, 0, "texture_desc");
  if (!self) return false;
  ev::give<ev::Bool>(o, out, 0, self->repeat);
  return true;
}

// engine.texture_desc.repeat@set: what stands at the place kern::TextureDesc::repeat gives
bool c177_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::TextureDesc>(o, in, 0, "texture_desc");
  if (!self) return false;
  ev::take<ev::Bool>(o, in, 1, self->repeat);
  return true;
}

// engine.texture_desc.nearest: kern::TextureDesc::nearest
bool c178(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::TextureDesc>(o, in, 0, "texture_desc");
  if (!self) return false;
  ev::give<ev::Bool>(o, out, 0, self->nearest);
  return true;
}

// engine.texture_desc.nearest@set: what stands at the place kern::TextureDesc::nearest gives
bool c178_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::TextureDesc>(o, in, 0, "texture_desc");
  if (!self) return false;
  ev::take<ev::Bool>(o, in, 1, self->nearest);
  return true;
}

// engine.uniform_cache.make: kern::UniformCache::UniformCache
bool c179(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::UniformCache>(std::unique_ptr<kern::UniformCache>(new kern::UniformCache())));
  return true;
}

// engine.uniform_cache.release: kern::UniformCache::~UniformCache
bool c180(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::UniformCache>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no UniformCache that was made";
    return false;
  }
  return true;
}

// kern::ShaderExtension: the object of the engine a Blueprint is (Graph::implements "ShaderExtension")
class Implements_ShaderExtension final : public kern::ShaderExtension {
 public:
  Implements_ShaderExtension(EngineObjects& o, std::shared_ptr<Implementer> by) : o_(o), by_(std::move(by)) {}
  decltype(std::declval<const kern::ShaderExtension&>().name()) name() const override {
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("name", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::ShaderExtension&>().name()) kern_r = "";
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<const kern::ShaderExtension&>().attributes()) attributes() const override {
    if (!by_->has("attributes")) return kern::ShaderExtension::attributes();
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("attributes", kern_in, kern_out) && !kern_out.empty();
    kern_kept_attributes.clear();
    if (kern_given) ev::Conv<ev::List<ev::Struct<S_ExtensionAttribute>>>::from(o_, kern_out[0], kern_kept_attributes);
    return decltype(std::declval<const kern::ShaderExtension&>().attributes())(kern_kept_attributes.data(), kern_kept_attributes.size());
  }
  decltype(std::declval<const kern::ShaderExtension&>().vertex_declarations()) vertex_declarations() const override {
    if (!by_->has("vertex_declarations")) return kern::ShaderExtension::vertex_declarations();
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("vertex_declarations", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::ShaderExtension&>().vertex_declarations()) kern_r = "";
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<const kern::ShaderExtension&>().vertex()) vertex() const override {
    if (!by_->has("vertex")) return kern::ShaderExtension::vertex();
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("vertex", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::ShaderExtension&>().vertex()) kern_r = "";
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<const kern::ShaderExtension&>().fragment_declarations()) fragment_declarations() const override {
    if (!by_->has("fragment_declarations")) return kern::ShaderExtension::fragment_declarations();
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("fragment_declarations", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::ShaderExtension&>().fragment_declarations()) kern_r = "";
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<const kern::ShaderExtension&>().fragment_discard()) fragment_discard() const override {
    if (!by_->has("fragment_discard")) return kern::ShaderExtension::fragment_discard();
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("fragment_discard", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::ShaderExtension&>().fragment_discard()) kern_r = "";
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<const kern::ShaderExtension&>().fragment_input()) fragment_input() const override {
    if (!by_->has("fragment_input")) return kern::ShaderExtension::fragment_input();
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("fragment_input", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::ShaderExtension&>().fragment_input()) kern_r = "";
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<const kern::ShaderExtension&>().fragment_surface()) fragment_surface() const override {
    if (!by_->has("fragment_surface")) return kern::ShaderExtension::fragment_surface();
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("fragment_surface", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<const kern::ShaderExtension&>().fragment_surface()) kern_r = "";
    if (kern_given) ev::Conv<ev::Text>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<kern::ShaderExtension&>().bind(std::declval<kern::Program const&>())) bind(kern::Program const& kern_p0) override {
    if (!by_->has("bind")) return kern::ShaderExtension::bind(kern_p0);
    const std::vector<Value> kern_in{Value::integer(o_.adopt<kern::Program>(kern_p0))};
    std::vector<Value> kern_out;
    by_->call("bind", kern_in, kern_out);
  }
  decltype(std::declval<kern::ShaderExtension&>().frame_uniforms(std::declval<kern::UniformCache&>(), std::declval<kern::Renderer const&>())) frame_uniforms(kern::UniformCache& kern_p0, kern::Renderer const& kern_p1) override {
    if (!by_->has("frame_uniforms")) return kern::ShaderExtension::frame_uniforms(kern_p0, kern_p1);
    const std::vector<Value> kern_in{Value::integer(o_.adopt<kern::UniformCache>(kern_p0)), Value::integer(o_.adopt<kern::Renderer>(kern_p1))};
    std::vector<Value> kern_out;
    by_->call("frame_uniforms", kern_in, kern_out);
  }

 private:
  EngineObjects& o_;
  std::shared_ptr<Implementer> by_;
  mutable std::vector<std::remove_cv_t<std::remove_pointer_t<decltype(std::declval<decltype(std::declval<const kern::ShaderExtension&>().attributes())>().data)>>> kern_kept_attributes;
};
i64 implements_ShaderExtension(EngineObjects& o, std::shared_ptr<Implementer> by) {
  return o.own<Implements_ShaderExtension, kern::ShaderExtension>(std::make_unique<Implements_ShaderExtension>(o, std::move(by)));
}

const EngineCall kCalls[] = {
    {"engine.mesh.skinned", &c0},
    {"engine.mesh.cull_mode", &c1},
    {"engine.mesh.vertex_count", &c2},
    {"engine.mesh.element_count", &c3},
    {"engine.mesh.triangle_count", &c4},
    {"engine.mesh.posed_vertex", &c5},
    {"engine.bounding_sphere", &c6},
    {"engine.mirror_state", &c7},
    {"engine.apply_cull_mode", &c8},
    {"engine.instanced_bounds", &c9},
    {"engine.particles.init", &c10},
    {"engine.particles.emit", &c11},
    {"engine.particles.tick", &c12},
    {"engine.particles.clear", &c13},
    {"engine.particles.mesh_id", &c14},
    {"engine.particles.active", &c15},
    {"engine.particles.stats", &c16},
    {"engine.light_pool.request", &c17},
    {"engine.light_pool.resolve", &c18},
    {"engine.light_pool.clear", &c19},
    {"engine.sun_shadow_map.init", &c20},
    {"engine.sun_shadow_map.destroy", &c21},
    {"engine.sun_shadow_map.valid", &c22},
    {"engine.sun_shadow_map.focus_key", &c23},
    {"engine.sun_shadow_map.draw", &c24},
    {"engine.sun_shadow_map.matrix", &c25},
    {"engine.sun_shadow_map.depth_texture", &c26},
    {"engine.sun_shadow_map.options", &c27},
    {"engine.sun_shadow_map.set_filter", &c28},
    {"engine.sun_shadow_map.extent", &c29},
    {"engine.sky_pass.init", &c30},
    {"engine.sky_pass.destroy", &c31},
    {"engine.sky_pass.draw", &c32},
    {"engine.bloom_pass.configure", &c33},
    {"engine.bloom_pass.begin", &c34},
    {"engine.bloom_pass.end", &c35},
    {"engine.bloom_pass.destroy", &c36},
    {"engine.bloom_pass.state", &c37},
    {"engine.bloom_pass.depth_texture", &c38},
    {"engine.bloom_pass.bind_scene", &c39},
    {"engine.ambient_occlusion_pass.configure", &c40},
    {"engine.ambient_occlusion_pass.set_radius", &c41},
    {"engine.ambient_occlusion_pass.options", &c42},
    {"engine.ambient_occlusion_pass.compute", &c43},
    {"engine.ambient_occlusion_pass.texture", &c44},
    {"engine.ambient_occlusion_pass.width", &c45},
    {"engine.ambient_occlusion_pass.height", &c46},
    {"engine.ambient_occlusion_pass.destroy", &c47},
    {"engine.ambient_occlusion_pass.quality", &c48},
    {"engine.ambient_occlusion_pass.half_resolution", &c49},
    {"engine.program.build", &c50},
    {"engine.program.destroy", &c51},
    {"engine.program.handle", &c52},
    {"engine.program.valid", &c53},
    {"engine.program.location", &c54},
    {"engine.program.use", &c55},
    {"engine.uniform_cache.reset", &c56},
    {"engine.uniform_cache.set_int", &c57},
    {"engine.uniform_cache.set_float", &c58},
    {"engine.uniform_cache.set_vec2", &c59},
    {"engine.uniform_cache.set_vec3", &c60},
    {"engine.uniform_cache.set_vec4", &c61},
    {"engine.uniform_cache.set_vec4_array", &c62},
    {"engine.uniform_cache.set_vec3_array", &c63},
    {"engine.uniform_cache.set_mat4", &c64},
    {"engine.uniform_cache.set_mat4_array", &c65},
    {"engine.renderer.init", &c66},
    {"engine.renderer.shutdown", &c67},
    {"engine.renderer.resize", &c68},
    {"engine.renderer.width", &c69},
    {"engine.renderer.height", &c70},
    {"engine.renderer.allocate_id", &c71},
    {"engine.renderer.release_id", &c72},
    {"engine.renderer.id_allocated", &c73},
    {"engine.renderer.upsert", &c74},
    {"engine.renderer.update_instances", &c75},
    {"engine.renderer.update_bones", &c76},
    {"engine.renderer.set_visible", &c77},
    {"engine.renderer.remove", &c78},
    {"engine.renderer.set_material", &c79},
    {"engine.renderer.set_layers", &c80},
    {"engine.renderer.mesh", &c81},
    {"engine.renderer.meshes", &c82},
    {"engine.renderer.mesh_count", &c83},
    {"engine.renderer.create_texture", &c84},
    {"engine.renderer.register_external_texture", &c85},
    {"engine.renderer.create_render_target", &c86},
    {"engine.renderer.delete_texture", &c87},
    {"engine.renderer.camera", &c88},
    {"engine.renderer.set_camera", &c89},
    {"engine.renderer.lighting", &c90},
    {"engine.renderer.set_lighting", &c91},
    {"engine.renderer.sky", &c92},
    {"engine.renderer.set_sky", &c93},
    {"engine.renderer.set_point_lights", &c94},
    {"engine.renderer.point_lights", &c95},
    {"engine.renderer.set_shadows", &c96},
    {"engine.renderer.set_shadow_distance", &c97},
    {"engine.renderer.set_shadow_filter", &c98},
    {"engine.renderer.set_ambient_occlusion", &c99},
    {"engine.renderer.set_ambient_occlusion_radius", &c100},
    {"engine.renderer.ambient_occlusion", &c101},
    {"engine.renderer.shadow", &c102},
    {"engine.renderer.invalidate", &c103},
    {"engine.renderer.scene_revision", &c104},
    {"engine.renderer.render", &c105},
    {"engine.renderer.last_stats", &c106},
    {"engine.renderer.screen_ray", &c107},
    {"engine.renderer.pick", &c108},
    {"engine.renderer.pick_ray", &c109},
    {"engine.renderer.context_lost", &c110},
    {"engine.renderer.context_restored", &c111},
    {"engine.renderer.lost", &c112},
    {"engine.renderer.set_headless", &c113},
    {"engine.renderer.extensions", &c114},
    {"engine.shader_extension.name", &c115},
    {"engine.shader_extension.attributes", &c116},
    {"engine.shader_extension.vertex_declarations", &c117},
    {"engine.shader_extension.vertex", &c118},
    {"engine.shader_extension.fragment_declarations", &c119},
    {"engine.shader_extension.fragment_discard", &c120},
    {"engine.shader_extension.fragment_input", &c121},
    {"engine.shader_extension.fragment_surface", &c122},
    {"engine.shader_extension.bind", &c123},
    {"engine.shader_extension.frame_uniforms", &c124},
    {"engine.shader_extension.material_uniforms", &c125},
    {"engine.build_standard_shader", &c126},
    {"engine.ambient_occlusion_pass.make", &c127},
    {"engine.ambient_occlusion_pass.release", &c128},
    {"engine.bloom_pass.make", &c129},
    {"engine.bloom_pass.release", &c130},
    {"engine.light_pool.make", &c131},
    {"engine.light_pool.release", &c132},
    {"engine.mesh.make", &c133},
    {"engine.mesh.release", &c134},
    {"engine.mesh.id", &c135},
    {"engine.mesh.visible", &c136},
    {"engine.mesh.layers", &c137},
    {"engine.mesh.pick_bvh", &c138},
    {"engine.mesh.linear_colors", &c139},
    {"engine.mesh.material", &c140},
    {"engine.mesh.texture", &c141},
    {"engine.mesh.positions", &c142},
    {"engine.mesh.normals", &c143},
    {"engine.mesh.colors", &c144},
    {"engine.mesh.uvs", &c145},
    {"engine.mesh.indices", &c146},
    {"engine.mesh.joints", &c147},
    {"engine.mesh.weights", &c148},
    {"engine.mesh.bones", &c149},
    {"engine.mesh.transforms", &c150},
    {"engine.mesh.tints", &c151},
    {"engine.mesh.instance_normals", &c152},
    {"engine.mesh.instance_count", &c153},
    {"engine.mesh.mirror", &c154},
    {"engine.mesh.local_bounds", &c155},
    {"engine.mesh.world_bounds", &c156},
    {"engine.mesh.gpu", &c157},
    {"engine.mesh.sort_distance", &c158},
    {"engine.mesh.live_index", &c159},
    {"engine.particles.make", &c160},
    {"engine.particles.release", &c161},
    {"engine.program.make", &c162},
    {"engine.program.release", &c163},
    {"engine.renderer.make", &c164},
    {"engine.renderer.release", &c165},
    {"engine.shader_extension.material_base", &c166},
    {"engine.shader_extension.attribute_base", &c167},
    {"engine.sky_pass.make", &c168},
    {"engine.sky_pass.release", &c169},
    {"engine.sun_shadow_map.make", &c170},
    {"engine.sun_shadow_map.release", &c171},
    {"engine.texture_desc.make", &c172},
    {"engine.texture_desc.release", &c173},
    {"engine.texture_desc.width", &c174},
    {"engine.texture_desc.height", &c175},
    {"engine.texture_desc.mipmaps", &c176},
    {"engine.texture_desc.repeat", &c177},
    {"engine.texture_desc.nearest", &c178},
    {"engine.uniform_cache.make", &c179},
    {"engine.uniform_cache.release", &c180},
    {"engine.mesh.id@get", &c135},
    {"engine.mesh.id@set", &c135_set},
    {"engine.mesh.visible@get", &c136},
    {"engine.mesh.visible@set", &c136_set},
    {"engine.mesh.layers@get", &c137},
    {"engine.mesh.layers@set", &c137_set},
    {"engine.mesh.pick_bvh@get", &c138},
    {"engine.mesh.pick_bvh@set", &c138_set},
    {"engine.mesh.linear_colors@get", &c139},
    {"engine.mesh.linear_colors@set", &c139_set},
    {"engine.mesh.material@get", &c140},
    {"engine.mesh.material@set", &c140_set},
    {"engine.mesh.texture@get", &c141},
    {"engine.mesh.texture@set", &c141_set},
    {"engine.mesh.positions@get", &c142},
    {"engine.mesh.positions@set", &c142_set},
    {"engine.mesh.normals@get", &c143},
    {"engine.mesh.normals@set", &c143_set},
    {"engine.mesh.colors@get", &c144},
    {"engine.mesh.colors@set", &c144_set},
    {"engine.mesh.uvs@get", &c145},
    {"engine.mesh.uvs@set", &c145_set},
    {"engine.mesh.indices@get", &c146},
    {"engine.mesh.indices@set", &c146_set},
    {"engine.mesh.joints@get", &c147},
    {"engine.mesh.joints@set", &c147_set},
    {"engine.mesh.weights@get", &c148},
    {"engine.mesh.weights@set", &c148_set},
    {"engine.mesh.bones@get", &c149},
    {"engine.mesh.bones@set", &c149_set},
    {"engine.mesh.transforms@get", &c150},
    {"engine.mesh.transforms@set", &c150_set},
    {"engine.mesh.tints@get", &c151},
    {"engine.mesh.tints@set", &c151_set},
    {"engine.mesh.instance_normals@get", &c152},
    {"engine.mesh.instance_normals@set", &c152_set},
    {"engine.mesh.instance_count@get", &c153},
    {"engine.mesh.instance_count@set", &c153_set},
    {"engine.mesh.mirror@get", &c154},
    {"engine.mesh.mirror@set", &c154_set},
    {"engine.mesh.local_bounds@get", &c155},
    {"engine.mesh.local_bounds@set", &c155_set},
    {"engine.mesh.world_bounds@get", &c156},
    {"engine.mesh.world_bounds@set", &c156_set},
    {"engine.mesh.gpu@get", &c157},
    {"engine.mesh.gpu@set", &c157_set},
    {"engine.mesh.sort_distance@get", &c158},
    {"engine.mesh.sort_distance@set", &c158_set},
    {"engine.mesh.live_index@get", &c159},
    {"engine.mesh.live_index@set", &c159_set},
    {"engine.shader_extension.material_base@get", &c166},
    {"engine.shader_extension.material_base@set", &c166_set},
    {"engine.shader_extension.attribute_base@get", &c167},
    {"engine.shader_extension.attribute_base@set", &c167_set},
    {"engine.texture_desc.width@get", &c174},
    {"engine.texture_desc.width@set", &c174_set},
    {"engine.texture_desc.height@get", &c175},
    {"engine.texture_desc.height@set", &c175_set},
    {"engine.texture_desc.mipmaps@get", &c176},
    {"engine.texture_desc.mipmaps@set", &c176_set},
    {"engine.texture_desc.repeat@get", &c177},
    {"engine.texture_desc.repeat@set", &c177_set},
    {"engine.texture_desc.nearest@get", &c178},
    {"engine.texture_desc.nearest@set", &c178_set},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

const EngineMaker kMakers[] = {
    {EngineObjects::kind<kern::AmbientOcclusionPass>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::AmbientOcclusionPass>(handle, std::unique_ptr<kern::AmbientOcclusionPass>(new kern::AmbientOcclusionPass())); }},
    {EngineObjects::kind<kern::BloomPass>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::BloomPass>(handle, std::unique_ptr<kern::BloomPass>(new kern::BloomPass())); }},
    {EngineObjects::kind<kern::LightPool>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::LightPool>(handle, std::unique_ptr<kern::LightPool>(new kern::LightPool())); }},
    {EngineObjects::kind<kern::Mesh>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::Mesh>(handle, std::unique_ptr<kern::Mesh>(new kern::Mesh())); }},
    {EngineObjects::kind<kern::Particles>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::Particles>(handle, std::unique_ptr<kern::Particles>(new kern::Particles())); }},
    {EngineObjects::kind<kern::Program>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::Program>(handle, std::unique_ptr<kern::Program>(new kern::Program())); }},
    {EngineObjects::kind<kern::Renderer>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::Renderer>(handle, std::unique_ptr<kern::Renderer>(new kern::Renderer())); }},
    {EngineObjects::kind<kern::SkyPass>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::SkyPass>(handle, std::unique_ptr<kern::SkyPass>(new kern::SkyPass())); }},
    {EngineObjects::kind<kern::SunShadowMap>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::SunShadowMap>(handle, std::unique_ptr<kern::SunShadowMap>(new kern::SunShadowMap())); }},
    {EngineObjects::kind<kern::TextureDesc>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::TextureDesc>(handle, std::unique_ptr<kern::TextureDesc>(new kern::TextureDesc())); }},
    {EngineObjects::kind<kern::UniformCache>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::UniformCache>(handle, std::unique_ptr<kern::UniformCache>(new kern::UniformCache())); }},
};
const EngineMakers kAllMakers(kMakers, std::size(kMakers));

const EngineImplement kAdapters[] = {
    {"ShaderExtension", &implements_ShaderExtension},
};
const EngineImplements kAllAdapters(kAdapters, std::size(kAdapters));

}  // namespace
}  // namespace kern::logic
