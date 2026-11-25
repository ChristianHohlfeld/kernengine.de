// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/audio/audio.h"
#include "kern/game/geometry.h"
#include "kern/game/level.h"
#include "kern/game/logic_nodes.h"
#include "kern/game/runtime.h"
#include "kern/game/sound.h"
#include "kern/gfx/renderer.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct E_audio_Bus { using type = kern::audio::Bus; static constexpr type values[] = {kern::audio::Bus::master, kern::audio::Bus::music, kern::audio::Bus::sfx, kern::audio::Bus::ambience, kern::audio::Bus::ui, kern::audio::Bus::voice, kern::audio::Bus::count}; };
struct E_audio_Rolloff { using type = kern::audio::Rolloff; static constexpr type values[] = {kern::audio::Rolloff::inverse, kern::audio::Rolloff::linear, kern::audio::Rolloff::exponential}; };
struct E_game_CameraMode { using type = kern::game::CameraMode; static constexpr type values[] = {kern::game::CameraMode::chase, kern::game::CameraMode::first_person, kern::game::CameraMode::top_down}; };
struct E_game_ColliderShape { using type = kern::game::ColliderShape; static constexpr type values[] = {kern::game::ColliderShape::automatic, kern::game::ColliderShape::box, kern::game::ColliderShape::sphere, kern::game::ColliderShape::capsule, kern::game::ColliderShape::hull}; };
struct E_game_ControlMode { using type = kern::game::ControlMode; static constexpr type values[] = {kern::game::ControlMode::hovercar, kern::game::ControlMode::rolling, kern::game::ControlMode::character, kern::game::ControlMode::flyer, kern::game::ControlMode::vehicle, kern::game::ControlMode::soldier, kern::game::ControlMode::walker}; };
struct E_game_GameGoal { using type = kern::game::GameGoal; static constexpr type values[] = {kern::game::GameGoal::none, kern::game::GameGoal::collect_all, kern::game::GameGoal::reach_goal, kern::game::GameGoal::survive, kern::game::GameGoal::score}; };
struct E_game_GameStatus { using type = kern::game::GameStatus; static constexpr type values[] = {kern::game::GameStatus::inactive, kern::game::GameStatus::playing, kern::game::GameStatus::won, kern::game::GameStatus::lost}; };
struct E_game_JointKind { using type = kern::game::JointKind; static constexpr type values[] = {kern::game::JointKind::hinge, kern::game::JointKind::slider, kern::game::JointKind::ball, kern::game::JointKind::fixed, kern::game::JointKind::distance, kern::game::JointKind::wheel, kern::game::JointKind::motor, kern::game::JointKind::parallel, kern::game::JointKind::no_collision}; };
struct E_game_Motion { using type = kern::game::Motion; static constexpr type values[] = {kern::game::Motion::fixed, kern::game::Motion::kinematic, kern::game::Motion::dynamic}; };
struct E_game_MoverMode { using type = kern::game::MoverMode; static constexpr type values[] = {kern::game::MoverMode::sine, kern::game::MoverMode::ping_pong, kern::game::MoverMode::loop}; };
struct E_game_Shape { using type = kern::game::Shape; static constexpr type values[] = {kern::game::Shape::box, kern::game::Shape::sphere, kern::game::Shape::cylinder, kern::game::Shape::cone, kern::game::Shape::capsule, kern::game::Shape::plane, kern::game::Shape::torus, kern::game::Shape::ramp, kern::game::Shape::mesh}; };
struct E_game_SimMode { using type = kern::game::SimMode; static constexpr type values[] = {kern::game::SimMode::none, kern::game::SimMode::simulate, kern::game::SimMode::play, kern::game::SimMode::replica}; };
struct E_game_TriggerAction { using type = kern::game::TriggerAction; static constexpr type values[] = {kern::game::TriggerAction::goal, kern::game::TriggerAction::hazard, kern::game::TriggerAction::checkpoint, kern::game::TriggerAction::message, kern::game::TriggerAction::launch, kern::game::TriggerAction::teleport, kern::game::TriggerAction::damage, kern::game::TriggerAction::event}; };
struct E_game_WeaponKind { using type = kern::game::WeaponKind; static constexpr type values[] = {kern::game::WeaponKind::rifle, kern::game::WeaponKind::pistol, kern::game::WeaponKind::shotgun, kern::game::WeaponKind::sniper}; };
struct E_logic_TriggerKind { using type = kern::logic::TriggerKind; static constexpr type values[] = {kern::logic::TriggerKind::event, kern::logic::TriggerKind::timer, kern::logic::TriggerKind::condition, kern::logic::TriggerKind::automatic}; };
struct E_physics_ShapeKind { using type = kern::physics::ShapeKind; static constexpr type values[] = {kern::physics::ShapeKind::box, kern::physics::ShapeKind::sphere, kern::physics::ShapeKind::capsule, kern::physics::ShapeKind::hull, kern::physics::ShapeKind::mesh}; };
struct S_Aabb { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Aabb& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Aabb& x); };
struct S_Camera { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Camera& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Camera& x); };
struct S_ExtensionAttribute { [[maybe_unused]] static Value to(EngineObjects& o, const kern::ExtensionAttribute& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::ExtensionAttribute& x); };
struct S_Mat4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Mat4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Mat4& x); };
struct S_MeshData { [[maybe_unused]] static Value to(EngineObjects& o, const kern::MeshData& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::MeshData& x); };
struct S_Quat { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Quat& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Quat& x); };
struct S_game_Actor { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::Actor& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::Actor& x); };
struct S_game_AudioComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::AudioComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::AudioComponent& x); };
struct S_game_BlueprintRunning { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::BlueprintRunning& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::BlueprintRunning& x); };
struct S_game_BodyComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::BodyComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::BodyComponent& x); };
struct S_game_BodyPlan { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::BodyPlan& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::BodyPlan& x); };
struct S_game_CityBox { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::CityBox& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::CityBox& x); };
struct S_game_CityComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::CityComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::CityComponent& x); };
struct S_game_CityGeometry { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::CityGeometry& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::CityGeometry& x); };
struct S_game_ColliderComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::ColliderComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::ColliderComponent& x); };
struct S_game_ControllerComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::ControllerComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::ControllerComponent& x); };
struct S_game_EditorCamera { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::EditorCamera& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::EditorCamera& x); };
struct S_game_GameWorld_ArtStats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::ArtStats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::ArtStats& x); };
struct S_game_GameWorld_DetectorModel { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::DetectorModel& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::DetectorModel& x); };
struct S_game_GameWorld_SpawnChange { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::SpawnChange& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::SpawnChange& x); };
struct S_game_GameWorld_WalkerJoint { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::WalkerJoint& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::WalkerJoint& x); };
struct S_game_GameWorld_WalkerProbe { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::WalkerProbe& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::WalkerProbe& x); };
struct S_game_GameWorld_WalkerSensors { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::WalkerSensors& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::WalkerSensors& x); };
struct S_game_HealthComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::HealthComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::HealthComponent& x); };
struct S_game_Hud { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::Hud& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::Hud& x); };
struct S_game_Issue { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::Issue& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::Issue& x); };
struct S_game_JointComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::JointComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::JointComponent& x); };
struct S_game_Level { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::Level& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::Level& x); };
struct S_game_LightComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::LightComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::LightComponent& x); };
struct S_game_MaterialDesc { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::MaterialDesc& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::MaterialDesc& x); };
struct S_game_MeshComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::MeshComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::MeshComponent& x); };
struct S_game_MotionKey { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::MotionKey& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::MotionKey& x); };
struct S_game_MoverComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::MoverComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::MoverComponent& x); };
struct S_game_PickupComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::PickupComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::PickupComponent& x); };
struct S_game_PlayInput { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::PlayInput& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::PlayInput& x); };
struct S_game_Rng { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::Rng& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::Rng& x); };
struct S_game_RotatorComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::RotatorComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::RotatorComponent& x); };
struct S_game_ShapePlan { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::ShapePlan& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::ShapePlan& x); };
struct S_game_ShotEvent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::ShotEvent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::ShotEvent& x); };
struct S_game_SimStats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::SimStats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::SimStats& x); };
struct S_game_SoldierState { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::SoldierState& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::SoldierState& x); };
struct S_game_SoundCue { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::SoundCue& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::SoundCue& x); };
struct S_game_Transform { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::Transform& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::Transform& x); };
struct S_game_TriggerComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::TriggerComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::TriggerComponent& x); };
struct S_game_WeaponComponent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::WeaponComponent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::WeaponComponent& x); };
struct S_game_WorldSettings { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::WorldSettings& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::WorldSettings& x); };
struct S_logic_CommentBox { [[maybe_unused]] static Value to(EngineObjects& o, const kern::logic::CommentBox& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::logic::CommentBox& x); };
struct S_logic_EnumType { [[maybe_unused]] static Value to(EngineObjects& o, const kern::logic::EnumType& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::logic::EnumType& x); };
struct S_logic_Function { [[maybe_unused]] static Value to(EngineObjects& o, const kern::logic::Function& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::logic::Function& x); };
struct S_logic_Graph { [[maybe_unused]] static Value to(EngineObjects& o, const kern::logic::Graph& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::logic::Graph& x); };
struct S_logic_Link { [[maybe_unused]] static Value to(EngineObjects& o, const kern::logic::Link& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::logic::Link& x); };
struct S_logic_Node { [[maybe_unused]] static Value to(EngineObjects& o, const kern::logic::Node& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::logic::Node& x); };
struct S_logic_State { [[maybe_unused]] static Value to(EngineObjects& o, const kern::logic::State& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::logic::State& x); };
struct S_logic_StateMachine { [[maybe_unused]] static Value to(EngineObjects& o, const kern::logic::StateMachine& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::logic::StateMachine& x); };
struct S_logic_StructType { [[maybe_unused]] static Value to(EngineObjects& o, const kern::logic::StructType& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::logic::StructType& x); };
struct S_logic_Transition { [[maybe_unused]] static Value to(EngineObjects& o, const kern::logic::Transition& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::logic::Transition& x); };
struct S_physics_Body { [[maybe_unused]] static Value to(EngineObjects& o, const kern::physics::Body& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::physics::Body& x); };
struct S_physics_QueryFilter { [[maybe_unused]] static Value to(EngineObjects& o, const kern::physics::QueryFilter& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::physics::QueryFilter& x); };
Value S_Aabb::to(EngineObjects& o, const kern::Aabb& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.min), ev::Conv<ev::Vector>::to(o, x.max)}); }
void S_Aabb::from(EngineObjects& o, const Value& v, kern::Aabb& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.min); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.max); }
Value S_Camera::to(EngineObjects& o, const kern::Camera& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.eye), ev::Conv<ev::Vector>::to(o, x.target), ev::Conv<ev::Vector>::to(o, x.up), ev::Conv<ev::Float>::to(o, x.fov), ev::Conv<ev::Float>::to(o, x.near), ev::Conv<ev::Float>::to(o, x.far), ev::Conv<ev::Bool>::to(o, x.orthographic), ev::Conv<ev::Float>::to(o, x.ortho_height), ev::Conv<ev::Bool>::to(o, x.has_projection), ev::Conv<ev::Struct<S_Mat4>>::to(o, x.projection)}); }
void S_Camera::from(EngineObjects& o, const Value& v, kern::Camera& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.eye); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.target); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.up); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.fov); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.near); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.far); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.orthographic); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.ortho_height); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.has_projection); if (const Value* f = v.field(9)) ev::Conv<ev::Struct<S_Mat4>>::from(o, *f, x.projection); }
Value S_ExtensionAttribute::to(EngineObjects& o, const kern::ExtensionAttribute& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.key), ev::Conv<ev::Int>::to(o, x.size), ev::Conv<ev::Text>::to(o, x.glsl)}); }
void S_ExtensionAttribute::from(EngineObjects& o, const Value& v, kern::ExtensionAttribute& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.key); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.size); if (const Value* f = v.field(2)) ev::Conv<ev::Text>::from(o, *f, x.glsl); }
Value S_Mat4::to(EngineObjects& o, const kern::Mat4& x) { return Value::structure({ev::Conv<ev::List<ev::Float>>::to(o, x.m)}); }
void S_Mat4::from(EngineObjects& o, const Value& v, kern::Mat4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.m); }
Value S_MeshData::to(EngineObjects& o, const kern::MeshData& x) { return Value::structure({ev::Conv<ev::List<ev::Float>>::to(o, x.positions), ev::Conv<ev::List<ev::Float>>::to(o, x.normals), ev::Conv<ev::List<ev::Float>>::to(o, x.colors), ev::Conv<ev::List<ev::Float>>::to(o, x.uvs), ev::Conv<ev::List<ev::Float>>::to(o, x.extra), ev::Conv<ev::Int>::to(o, x.extra_size), ev::Conv<ev::List<ev::Int>>::to(o, x.indices)}); }
void S_MeshData::from(EngineObjects& o, const Value& v, kern::MeshData& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.positions); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.normals); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.colors); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.uvs); if (const Value* f = v.field(4)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.extra); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.extra_size); if (const Value* f = v.field(6)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.indices); }
Value S_Quat::to(EngineObjects& o, const kern::Quat& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.x), ev::Conv<ev::Float>::to(o, x.y), ev::Conv<ev::Float>::to(o, x.z), ev::Conv<ev::Float>::to(o, x.w)}); }
void S_Quat::from(EngineObjects& o, const Value& v, kern::Quat& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.x); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.y); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.z); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.w); }
Value S_game_Actor::to(EngineObjects& o, const kern::game::Actor& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.id), ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::Int>::to(o, x.parent), ev::Conv<ev::Text>::to(o, x.folder), ev::Conv<ev::Bool>::to(o, x.hidden), ev::Conv<ev::Bool>::to(o, x.locked), ev::Conv<ev::Struct<S_game_Transform>>::to(o, x.transform), ev::Conv<ev::List<ev::Text>>::to(o, x.tags), ev::Conv<ev::List<ev::Struct<S_game_MeshComponent>>>::to(o, x.mesh), ev::Conv<ev::List<ev::Struct<S_game_LightComponent>>>::to(o, x.light), ev::Conv<ev::List<ev::Struct<S_game_BodyComponent>>>::to(o, x.body), ev::Conv<ev::List<ev::Struct<S_game_ColliderComponent>>>::to(o, x.colliders), ev::Conv<ev::List<ev::Struct<S_game_JointComponent>>>::to(o, x.joints), ev::Conv<ev::List<ev::Struct<S_game_ControllerComponent>>>::to(o, x.controller), ev::Conv<ev::List<ev::Struct<S_game_PickupComponent>>>::to(o, x.pickup), ev::Conv<ev::List<ev::Struct<S_game_TriggerComponent>>>::to(o, x.trigger), ev::Conv<ev::List<ev::Struct<S_game_RotatorComponent>>>::to(o, x.rotator), ev::Conv<ev::List<ev::Struct<S_game_MoverComponent>>>::to(o, x.mover), ev::Conv<ev::List<ev::Struct<S_game_CityComponent>>>::to(o, x.city), ev::Conv<ev::List<ev::Struct<S_game_AudioComponent>>>::to(o, x.audio), ev::Conv<ev::List<ev::Struct<S_game_WeaponComponent>>>::to(o, x.weapon), ev::Conv<ev::List<ev::Struct<S_game_HealthComponent>>>::to(o, x.health), ev::Conv<ev::List<ev::Struct<S_logic_Graph>>>::to(o, x.logic)}); }
void S_game_Actor::from(EngineObjects& o, const Value& v, kern::game::Actor& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.id); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.parent); if (const Value* f = v.field(3)) ev::Conv<ev::Text>::from(o, *f, x.folder); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.hidden); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.locked); if (const Value* f = v.field(6)) ev::Conv<ev::Struct<S_game_Transform>>::from(o, *f, x.transform); if (const Value* f = v.field(7)) ev::Conv<ev::List<ev::Text>>::from(o, *f, x.tags); if (const Value* f = v.field(8)) ev::Conv<ev::List<ev::Struct<S_game_MeshComponent>>>::from(o, *f, x.mesh); if (const Value* f = v.field(9)) ev::Conv<ev::List<ev::Struct<S_game_LightComponent>>>::from(o, *f, x.light); if (const Value* f = v.field(10)) ev::Conv<ev::List<ev::Struct<S_game_BodyComponent>>>::from(o, *f, x.body); if (const Value* f = v.field(11)) ev::Conv<ev::List<ev::Struct<S_game_ColliderComponent>>>::from(o, *f, x.colliders); if (const Value* f = v.field(12)) ev::Conv<ev::List<ev::Struct<S_game_JointComponent>>>::from(o, *f, x.joints); if (const Value* f = v.field(13)) ev::Conv<ev::List<ev::Struct<S_game_ControllerComponent>>>::from(o, *f, x.controller); if (const Value* f = v.field(14)) ev::Conv<ev::List<ev::Struct<S_game_PickupComponent>>>::from(o, *f, x.pickup); if (const Value* f = v.field(15)) ev::Conv<ev::List<ev::Struct<S_game_TriggerComponent>>>::from(o, *f, x.trigger); if (const Value* f = v.field(16)) ev::Conv<ev::List<ev::Struct<S_game_RotatorComponent>>>::from(o, *f, x.rotator); if (const Value* f = v.field(17)) ev::Conv<ev::List<ev::Struct<S_game_MoverComponent>>>::from(o, *f, x.mover); if (const Value* f = v.field(18)) ev::Conv<ev::List<ev::Struct<S_game_CityComponent>>>::from(o, *f, x.city); if (const Value* f = v.field(19)) ev::Conv<ev::List<ev::Struct<S_game_AudioComponent>>>::from(o, *f, x.audio); if (const Value* f = v.field(20)) ev::Conv<ev::List<ev::Struct<S_game_WeaponComponent>>>::from(o, *f, x.weapon); if (const Value* f = v.field(21)) ev::Conv<ev::List<ev::Struct<S_game_HealthComponent>>>::from(o, *f, x.health); if (const Value* f = v.field(22)) ev::Conv<ev::List<ev::Struct<S_logic_Graph>>>::from(o, *f, x.logic); }
Value S_game_AudioComponent::to(EngineObjects& o, const kern::game::AudioComponent& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.sound), ev::Conv<ev::Float>::to(o, x.volume), ev::Conv<ev::Float>::to(o, x.pitch), ev::Conv<ev::Bool>::to(o, x.loop), ev::Conv<ev::Bool>::to(o, x.autoplay), ev::Conv<ev::Bool>::to(o, x.spatial), ev::Conv<ev::Float>::to(o, x.min_distance), ev::Conv<ev::Float>::to(o, x.max_distance), ev::Conv<ev::Enum<E_audio_Rolloff>>::to(o, x.rolloff), ev::Conv<ev::Enum<E_audio_Bus>>::to(o, x.bus), ev::Conv<ev::Float>::to(o, x.doppler), ev::Conv<ev::Float>::to(o, x.pitch_by_speed), ev::Conv<ev::Float>::to(o, x.delay)}); }
void S_game_AudioComponent::from(EngineObjects& o, const Value& v, kern::game::AudioComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.sound); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.volume); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.pitch); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.loop); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.autoplay); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.spatial); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.min_distance); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.max_distance); if (const Value* f = v.field(8)) ev::Conv<ev::Enum<E_audio_Rolloff>>::from(o, *f, x.rolloff); if (const Value* f = v.field(9)) ev::Conv<ev::Enum<E_audio_Bus>>::from(o, *f, x.bus); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.doppler); if (const Value* f = v.field(11)) ev::Conv<ev::Float>::from(o, *f, x.pitch_by_speed); if (const Value* f = v.field(12)) ev::Conv<ev::Float>::from(o, *f, x.delay); }
Value S_game_BlueprintRunning::to(EngineObjects& o, const kern::game::BlueprintRunning& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::Text>::to(o, x.tier), ev::Conv<ev::Int>::to(o, x.functions), ev::Conv<ev::Int>::to(o, x.nodes), ev::Conv<ev::Bool>::to(o, x.disabled)}); }
void S_game_BlueprintRunning::from(EngineObjects& o, const Value& v, kern::game::BlueprintRunning& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.tier); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.functions); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.nodes); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.disabled); }
Value S_game_BodyComponent::to(EngineObjects& o, const kern::game::BodyComponent& x) { return Value::structure({ev::Conv<ev::Enum<E_game_Motion>>::to(o, x.motion), ev::Conv<ev::Float>::to(o, x.mass), ev::Conv<ev::Float>::to(o, x.linear_damping), ev::Conv<ev::Float>::to(o, x.angular_damping), ev::Conv<ev::Float>::to(o, x.gravity_scale), ev::Conv<ev::Bool>::to(o, x.bullet), ev::Conv<ev::Bool>::to(o, x.start_awake), ev::Conv<ev::Bool>::to(o, x.allow_sleep), ev::Conv<ev::List<ev::Bool>>::to(o, x.lock_linear), ev::Conv<ev::List<ev::Bool>>::to(o, x.lock_angular), ev::Conv<ev::Vector>::to(o, x.linear_velocity), ev::Conv<ev::Vector>::to(o, x.angular_velocity), ev::Conv<ev::Text>::to(o, x.impact_sound), ev::Conv<ev::Float>::to(o, x.impact_threshold), ev::Conv<ev::Vector>::to(o, x.center_of_mass), ev::Conv<ev::List<ev::Float>>::to(o, x.inertia)}); }
void S_game_BodyComponent::from(EngineObjects& o, const Value& v, kern::game::BodyComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_game_Motion>>::from(o, *f, x.motion); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.mass); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.linear_damping); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.angular_damping); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.gravity_scale); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.bullet); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.start_awake); if (const Value* f = v.field(7)) ev::Conv<ev::Bool>::from(o, *f, x.allow_sleep); if (const Value* f = v.field(8)) ev::Conv<ev::List<ev::Bool>>::from(o, *f, x.lock_linear); if (const Value* f = v.field(9)) ev::Conv<ev::List<ev::Bool>>::from(o, *f, x.lock_angular); if (const Value* f = v.field(10)) ev::Conv<ev::Vector>::from(o, *f, x.linear_velocity); if (const Value* f = v.field(11)) ev::Conv<ev::Vector>::from(o, *f, x.angular_velocity); if (const Value* f = v.field(12)) ev::Conv<ev::Text>::from(o, *f, x.impact_sound); if (const Value* f = v.field(13)) ev::Conv<ev::Float>::from(o, *f, x.impact_threshold); if (const Value* f = v.field(14)) ev::Conv<ev::Vector>::from(o, *f, x.center_of_mass); if (const Value* f = v.field(15)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.inertia); }
Value S_game_BodyPlan::to(EngineObjects& o, const kern::game::BodyPlan& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.owner), ev::Conv<ev::Enum<E_game_Motion>>::to(o, x.motion), ev::Conv<ev::Bool>::to(o, x.implicit), ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Struct<S_Quat>>::to(o, x.rotation), ev::Conv<ev::List<ev::Struct<S_game_ShapePlan>>>::to(o, x.shapes)}); }
void S_game_BodyPlan::from(EngineObjects& o, const Value& v, kern::game::BodyPlan& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.owner); if (const Value* f = v.field(1)) ev::Conv<ev::Enum<E_game_Motion>>::from(o, *f, x.motion); if (const Value* f = v.field(2)) ev::Conv<ev::Bool>::from(o, *f, x.implicit); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(4)) ev::Conv<ev::Struct<S_Quat>>::from(o, *f, x.rotation); if (const Value* f = v.field(5)) ev::Conv<ev::List<ev::Struct<S_game_ShapePlan>>>::from(o, *f, x.shapes); }
Value S_game_CityBox::to(EngineObjects& o, const kern::game::CityBox& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.center), ev::Conv<ev::Vector>::to(o, x.size)}); }
void S_game_CityBox::from(EngineObjects& o, const Value& v, kern::game::CityBox& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.center); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.size); }
Value S_game_CityComponent::to(EngineObjects& o, const kern::game::CityComponent& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.seed), ev::Conv<ev::Int>::to(o, x.blocks), ev::Conv<ev::Float>::to(o, x.pitch), ev::Conv<ev::Float>::to(o, x.street), ev::Conv<ev::Float>::to(o, x.max_height), ev::Conv<ev::Bool>::to(o, x.trees), ev::Conv<ev::Int>::to(o, x.traffic), ev::Conv<ev::Bool>::to(o, x.night_windows)}); }
void S_game_CityComponent::from(EngineObjects& o, const Value& v, kern::game::CityComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.seed); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.blocks); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.pitch); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.street); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.max_height); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.trees); if (const Value* f = v.field(6)) ev::Conv<ev::Int>::from(o, *f, x.traffic); if (const Value* f = v.field(7)) ev::Conv<ev::Bool>::from(o, *f, x.night_windows); }
Value S_game_CityGeometry::to(EngineObjects& o, const kern::game::CityGeometry& x) { return Value::structure({ev::Conv<ev::List<ev::Struct<S_MeshData>>>::to(o, x.buildings), ev::Conv<ev::List<ev::Struct<S_MeshData>>>::to(o, x.roads), ev::Conv<ev::Struct<S_MeshData>>::to(o, x.plates), ev::Conv<ev::Struct<S_MeshData>>::to(o, x.trunk), ev::Conv<ev::Struct<S_MeshData>>::to(o, x.crown), ev::Conv<ev::List<ev::Float>>::to(o, x.trunk_transforms), ev::Conv<ev::List<ev::Float>>::to(o, x.crown_transforms), ev::Conv<ev::List<ev::Float>>::to(o, x.crown_tints), ev::Conv<ev::List<ev::Struct<S_game_CityBox>>>::to(o, x.colliders), ev::Conv<ev::Float>::to(o, x.half), ev::Conv<ev::Int>::to(o, x.building_count)}); }
void S_game_CityGeometry::from(EngineObjects& o, const Value& v, kern::game::CityGeometry& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Struct<S_MeshData>>>::from(o, *f, x.buildings); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Struct<S_MeshData>>>::from(o, *f, x.roads); if (const Value* f = v.field(2)) ev::Conv<ev::Struct<S_MeshData>>::from(o, *f, x.plates); if (const Value* f = v.field(3)) ev::Conv<ev::Struct<S_MeshData>>::from(o, *f, x.trunk); if (const Value* f = v.field(4)) ev::Conv<ev::Struct<S_MeshData>>::from(o, *f, x.crown); if (const Value* f = v.field(5)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.trunk_transforms); if (const Value* f = v.field(6)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.crown_transforms); if (const Value* f = v.field(7)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.crown_tints); if (const Value* f = v.field(8)) ev::Conv<ev::List<ev::Struct<S_game_CityBox>>>::from(o, *f, x.colliders); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.half); if (const Value* f = v.field(10)) ev::Conv<ev::Int>::from(o, *f, x.building_count); }
Value S_game_ColliderComponent::to(EngineObjects& o, const kern::game::ColliderComponent& x) { return Value::structure({ev::Conv<ev::Enum<E_game_ColliderShape>>::to(o, x.shape), ev::Conv<ev::Vector>::to(o, x.center), ev::Conv<ev::Vector>::to(o, x.rotation), ev::Conv<ev::Vector>::to(o, x.size), ev::Conv<ev::Float>::to(o, x.radius), ev::Conv<ev::Float>::to(o, x.length), ev::Conv<ev::List<ev::Vector>>::to(o, x.points), ev::Conv<ev::Float>::to(o, x.density), ev::Conv<ev::Float>::to(o, x.friction), ev::Conv<ev::Float>::to(o, x.restitution), ev::Conv<ev::Float>::to(o, x.rolling_resistance), ev::Conv<ev::Text>::to(o, x.surface), ev::Conv<ev::Bool>::to(o, x.sensor), ev::Conv<ev::Int>::to(o, x.category), ev::Conv<ev::Int>::to(o, x.mask), ev::Conv<ev::Int>::to(o, x.group), ev::Conv<ev::Text>::to(o, x.impact_sound)}); }
void S_game_ColliderComponent::from(EngineObjects& o, const Value& v, kern::game::ColliderComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_game_ColliderShape>>::from(o, *f, x.shape); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.center); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.rotation); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.size); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.radius); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.length); if (const Value* f = v.field(6)) ev::Conv<ev::List<ev::Vector>>::from(o, *f, x.points); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.density); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.friction); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.restitution); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.rolling_resistance); if (const Value* f = v.field(11)) ev::Conv<ev::Text>::from(o, *f, x.surface); if (const Value* f = v.field(12)) ev::Conv<ev::Bool>::from(o, *f, x.sensor); if (const Value* f = v.field(13)) ev::Conv<ev::Int>::from(o, *f, x.category); if (const Value* f = v.field(14)) ev::Conv<ev::Int>::from(o, *f, x.mask); if (const Value* f = v.field(15)) ev::Conv<ev::Int>::from(o, *f, x.group); if (const Value* f = v.field(16)) ev::Conv<ev::Text>::from(o, *f, x.impact_sound); }
Value S_game_ControllerComponent::to(EngineObjects& o, const kern::game::ControllerComponent& x) { return Value::structure({ev::Conv<ev::Enum<E_game_ControlMode>>::to(o, x.mode), ev::Conv<ev::Float>::to(o, x.max_speed), ev::Conv<ev::Float>::to(o, x.acceleration), ev::Conv<ev::Float>::to(o, x.turn_rate), ev::Conv<ev::Float>::to(o, x.jump_speed), ev::Conv<ev::Bool>::to(o, x.boost), ev::Conv<ev::Float>::to(o, x.boost_speed), ev::Conv<ev::Float>::to(o, x.hover_height), ev::Conv<ev::Enum<E_game_CameraMode>>::to(o, x.camera), ev::Conv<ev::Float>::to(o, x.camera_distance), ev::Conv<ev::Float>::to(o, x.camera_height), ev::Conv<ev::Float>::to(o, x.camera_fov), ev::Conv<ev::Text>::to(o, x.engine_sound), ev::Conv<ev::Text>::to(o, x.jump_sound), ev::Conv<ev::Text>::to(o, x.boost_sound), ev::Conv<ev::Float>::to(o, x.crouch_speed), ev::Conv<ev::Float>::to(o, x.eye_height), ev::Conv<ev::Float>::to(o, x.look_sensitivity), ev::Conv<ev::Bool>::to(o, x.head_bob), ev::Conv<ev::Bool>::to(o, x.autonomous), ev::Conv<ev::Text>::to(o, x.gait), ev::Conv<ev::Bool>::to(o, x.rollers), ev::Conv<ev::Text>::to(o, x.actions), ev::Conv<ev::Bool>::to(o, x.kern_extras), ev::Conv<ev::Float>::to(o, x.servo_delay)}); }
void S_game_ControllerComponent::from(EngineObjects& o, const Value& v, kern::game::ControllerComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_game_ControlMode>>::from(o, *f, x.mode); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.max_speed); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.acceleration); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.turn_rate); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.jump_speed); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.boost); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.boost_speed); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.hover_height); if (const Value* f = v.field(8)) ev::Conv<ev::Enum<E_game_CameraMode>>::from(o, *f, x.camera); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.camera_distance); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.camera_height); if (const Value* f = v.field(11)) ev::Conv<ev::Float>::from(o, *f, x.camera_fov); if (const Value* f = v.field(12)) ev::Conv<ev::Text>::from(o, *f, x.engine_sound); if (const Value* f = v.field(13)) ev::Conv<ev::Text>::from(o, *f, x.jump_sound); if (const Value* f = v.field(14)) ev::Conv<ev::Text>::from(o, *f, x.boost_sound); if (const Value* f = v.field(15)) ev::Conv<ev::Float>::from(o, *f, x.crouch_speed); if (const Value* f = v.field(16)) ev::Conv<ev::Float>::from(o, *f, x.eye_height); if (const Value* f = v.field(17)) ev::Conv<ev::Float>::from(o, *f, x.look_sensitivity); if (const Value* f = v.field(18)) ev::Conv<ev::Bool>::from(o, *f, x.head_bob); if (const Value* f = v.field(19)) ev::Conv<ev::Bool>::from(o, *f, x.autonomous); if (const Value* f = v.field(20)) ev::Conv<ev::Text>::from(o, *f, x.gait); if (const Value* f = v.field(21)) ev::Conv<ev::Bool>::from(o, *f, x.rollers); if (const Value* f = v.field(22)) ev::Conv<ev::Text>::from(o, *f, x.actions); if (const Value* f = v.field(23)) ev::Conv<ev::Bool>::from(o, *f, x.kern_extras); if (const Value* f = v.field(24)) ev::Conv<ev::Float>::from(o, *f, x.servo_delay); }
Value S_game_EditorCamera::to(EngineObjects& o, const kern::game::EditorCamera& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Float>::to(o, x.yaw), ev::Conv<ev::Float>::to(o, x.pitch)}); }
void S_game_EditorCamera::from(EngineObjects& o, const Value& v, kern::game::EditorCamera& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.yaw); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.pitch); }
Value S_game_GameWorld_ArtStats::to(EngineObjects& o, const kern::game::GameWorld::ArtStats& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.rigs), ev::Conv<ev::Int>::to(o, x.decals), ev::Conv<ev::Int>::to(o, x.tracers), ev::Conv<ev::Int>::to(o, x.chips), ev::Conv<ev::Bool>::to(o, x.view_model)}); }
void S_game_GameWorld_ArtStats::from(EngineObjects& o, const Value& v, kern::game::GameWorld::ArtStats& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.rigs); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.decals); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.tracers); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.chips); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.view_model); }
Value S_game_GameWorld_DetectorModel::to(EngineObjects& o, const kern::game::GameWorld::DetectorModel& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.sigma_abs), ev::Conv<ev::Float>::to(o, x.sigma_rel), ev::Conv<ev::Float>::to(o, x.dropout), ev::Conv<ev::Int64>::to(o, x.seed)}); }
void S_game_GameWorld_DetectorModel::from(EngineObjects& o, const Value& v, kern::game::GameWorld::DetectorModel& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.sigma_abs); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.sigma_rel); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.dropout); if (const Value* f = v.field(3)) ev::Conv<ev::Int64>::from(o, *f, x.seed); }
Value S_game_GameWorld_SpawnChange::to(EngineObjects& o, const kern::game::GameWorld::SpawnChange& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.source), ev::Conv<ev::Int>::to(o, x.id), ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Vector>::to(o, x.rotation)}); }
void S_game_GameWorld_SpawnChange::from(EngineObjects& o, const Value& v, kern::game::GameWorld::SpawnChange& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.source); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.id); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.rotation); }
Value S_game_GameWorld_WalkerJoint::to(EngineObjects& o, const kern::game::GameWorld::WalkerJoint& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::Float>::to(o, x.angle), ev::Conv<ev::Float>::to(o, x.lower), ev::Conv<ev::Float>::to(o, x.upper)}); }
void S_game_GameWorld_WalkerJoint::from(EngineObjects& o, const Value& v, kern::game::GameWorld::WalkerJoint& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.angle); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.lower); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.upper); }
Value S_game_GameWorld_WalkerProbe::to(EngineObjects& o, const kern::game::GameWorld::WalkerProbe& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Struct<S_Quat>>::to(o, x.rotation), ev::Conv<ev::Vector>::to(o, x.velocity), ev::Conv<ev::Vector>::to(o, x.spin), ev::Conv<ev::Vector>::to(o, x.gravity), ev::Conv<ev::List<ev::Bool>>::to(o, x.foot_contact), ev::Conv<ev::List<ev::Float>>::to(o, x.foot_height), ev::Conv<ev::Bool>::to(o, x.self_contact), ev::Conv<ev::Bool>::to(o, x.body_contact), ev::Conv<ev::List<ev::Float>>::to(o, x.joint), ev::Conv<ev::List<ev::Float>>::to(o, x.joint_speed), ev::Conv<ev::List<ev::Float>>::to(o, x.action)}); }
void S_game_GameWorld_WalkerProbe::from(EngineObjects& o, const Value& v, kern::game::GameWorld::WalkerProbe& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(1)) ev::Conv<ev::Struct<S_Quat>>::from(o, *f, x.rotation); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.velocity); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.spin); if (const Value* f = v.field(4)) ev::Conv<ev::Vector>::from(o, *f, x.gravity); if (const Value* f = v.field(5)) ev::Conv<ev::List<ev::Bool>>::from(o, *f, x.foot_contact); if (const Value* f = v.field(6)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.foot_height); if (const Value* f = v.field(7)) ev::Conv<ev::Bool>::from(o, *f, x.self_contact); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.body_contact); if (const Value* f = v.field(9)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.joint); if (const Value* f = v.field(10)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.joint_speed); if (const Value* f = v.field(11)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.action); }
Value S_game_GameWorld_WalkerSensors::to(EngineObjects& o, const kern::game::GameWorld::WalkerSensors& x) { return Value::structure({ev::Conv<ev::List<ev::Float>>::to(o, x.range), ev::Conv<ev::List<ev::Int>>::to(o, x.zone), ev::Conv<ev::Float>::to(o, x.nearest), ev::Conv<ev::Float>::to(o, x.nearest_bearing), ev::Conv<ev::Float>::to(o, x.free_left), ev::Conv<ev::Float>::to(o, x.free_right), ev::Conv<ev::Vector>::to(o, x.gravity), ev::Conv<ev::Bool>::to(o, x.fallen), ev::Conv<ev::Float>::to(o, x.speed), ev::Conv<ev::Float>::to(o, x.tof_time), ev::Conv<ev::Int>::to(o, x.hits), ev::Conv<ev::List<ev::Vector>>::to(o, x.hit), ev::Conv<ev::Int>::to(o, x.forward_observed)}); }
void S_game_GameWorld_WalkerSensors::from(EngineObjects& o, const Value& v, kern::game::GameWorld::WalkerSensors& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.range); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.zone); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.nearest); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.nearest_bearing); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.free_left); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.free_right); if (const Value* f = v.field(6)) ev::Conv<ev::Vector>::from(o, *f, x.gravity); if (const Value* f = v.field(7)) ev::Conv<ev::Bool>::from(o, *f, x.fallen); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.speed); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.tof_time); if (const Value* f = v.field(10)) ev::Conv<ev::Int>::from(o, *f, x.hits); if (const Value* f = v.field(11)) ev::Conv<ev::List<ev::Vector>>::from(o, *f, x.hit); if (const Value* f = v.field(12)) ev::Conv<ev::Int>::from(o, *f, x.forward_observed); }
Value S_game_HealthComponent::to(EngineObjects& o, const kern::game::HealthComponent& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.max_health), ev::Conv<ev::Float>::to(o, x.armor), ev::Conv<ev::Float>::to(o, x.regen_delay), ev::Conv<ev::Float>::to(o, x.regen_rate), ev::Conv<ev::Bool>::to(o, x.respawn), ev::Conv<ev::Float>::to(o, x.respawn_delay), ev::Conv<ev::Float>::to(o, x.points), ev::Conv<ev::Text>::to(o, x.hit_sound), ev::Conv<ev::Text>::to(o, x.death_sound), ev::Conv<ev::Float>::to(o, x.explode_radius), ev::Conv<ev::Float>::to(o, x.explode_damage)}); }
void S_game_HealthComponent::from(EngineObjects& o, const Value& v, kern::game::HealthComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.max_health); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.armor); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.regen_delay); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.regen_rate); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.respawn); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.respawn_delay); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.points); if (const Value* f = v.field(7)) ev::Conv<ev::Text>::from(o, *f, x.hit_sound); if (const Value* f = v.field(8)) ev::Conv<ev::Text>::from(o, *f, x.death_sound); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.explode_radius); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.explode_damage); }
Value S_game_Hud::to(EngineObjects& o, const kern::game::Hud& x) { return Value::structure({ev::Conv<ev::Enum<E_game_GameStatus>>::to(o, x.status), ev::Conv<ev::Text>::to(o, x.title), ev::Conv<ev::Text>::to(o, x.message), ev::Conv<ev::Float>::to(o, x.message_age), ev::Conv<ev::Float>::to(o, x.score), ev::Conv<ev::Int>::to(o, x.pickups), ev::Conv<ev::Int>::to(o, x.pickups_total), ev::Conv<ev::Float>::to(o, x.elapsed), ev::Conv<ev::Float>::to(o, x.remaining), ev::Conv<ev::Float>::to(o, x.speed), ev::Conv<ev::Float>::to(o, x.boost), ev::Conv<ev::Bool>::to(o, x.boosting), ev::Conv<ev::Bool>::to(o, x.interior), ev::Conv<ev::Float>::to(o, x.best), ev::Conv<ev::Bool>::to(o, x.soldier), ev::Conv<ev::Float>::to(o, x.health), ev::Conv<ev::Float>::to(o, x.max_health), ev::Conv<ev::Float>::to(o, x.armor), ev::Conv<ev::Int>::to(o, x.ammo), ev::Conv<ev::Int>::to(o, x.magazine), ev::Conv<ev::Int>::to(o, x.reserve), ev::Conv<ev::Float>::to(o, x.reload), ev::Conv<ev::Float>::to(o, x.spread), ev::Conv<ev::Float>::to(o, x.hit_age), ev::Conv<ev::Bool>::to(o, x.hit_head), ev::Conv<ev::Bool>::to(o, x.hit_kill), ev::Conv<ev::Float>::to(o, x.damage_age), ev::Conv<ev::Float>::to(o, x.damage_yaw), ev::Conv<ev::Float>::to(o, x.respawn_in), ev::Conv<ev::Bool>::to(o, x.aiming), ev::Conv<ev::Float>::to(o, x.fov)}); }
void S_game_Hud::from(EngineObjects& o, const Value& v, kern::game::Hud& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_game_GameStatus>>::from(o, *f, x.status); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.title); if (const Value* f = v.field(2)) ev::Conv<ev::Text>::from(o, *f, x.message); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.message_age); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.score); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.pickups); if (const Value* f = v.field(6)) ev::Conv<ev::Int>::from(o, *f, x.pickups_total); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.elapsed); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.remaining); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.speed); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.boost); if (const Value* f = v.field(11)) ev::Conv<ev::Bool>::from(o, *f, x.boosting); if (const Value* f = v.field(12)) ev::Conv<ev::Bool>::from(o, *f, x.interior); if (const Value* f = v.field(13)) ev::Conv<ev::Float>::from(o, *f, x.best); if (const Value* f = v.field(14)) ev::Conv<ev::Bool>::from(o, *f, x.soldier); if (const Value* f = v.field(15)) ev::Conv<ev::Float>::from(o, *f, x.health); if (const Value* f = v.field(16)) ev::Conv<ev::Float>::from(o, *f, x.max_health); if (const Value* f = v.field(17)) ev::Conv<ev::Float>::from(o, *f, x.armor); if (const Value* f = v.field(18)) ev::Conv<ev::Int>::from(o, *f, x.ammo); if (const Value* f = v.field(19)) ev::Conv<ev::Int>::from(o, *f, x.magazine); if (const Value* f = v.field(20)) ev::Conv<ev::Int>::from(o, *f, x.reserve); if (const Value* f = v.field(21)) ev::Conv<ev::Float>::from(o, *f, x.reload); if (const Value* f = v.field(22)) ev::Conv<ev::Float>::from(o, *f, x.spread); if (const Value* f = v.field(23)) ev::Conv<ev::Float>::from(o, *f, x.hit_age); if (const Value* f = v.field(24)) ev::Conv<ev::Bool>::from(o, *f, x.hit_head); if (const Value* f = v.field(25)) ev::Conv<ev::Bool>::from(o, *f, x.hit_kill); if (const Value* f = v.field(26)) ev::Conv<ev::Float>::from(o, *f, x.damage_age); if (const Value* f = v.field(27)) ev::Conv<ev::Float>::from(o, *f, x.damage_yaw); if (const Value* f = v.field(28)) ev::Conv<ev::Float>::from(o, *f, x.respawn_in); if (const Value* f = v.field(29)) ev::Conv<ev::Bool>::from(o, *f, x.aiming); if (const Value* f = v.field(30)) ev::Conv<ev::Float>::from(o, *f, x.fov); }
Value S_game_Issue::to(EngineObjects& o, const kern::game::Issue& x) { return Value::structure({ev::Conv<ev::Bool>::to(o, x.error), ev::Conv<ev::Int>::to(o, x.actor), ev::Conv<ev::Text>::to(o, x.text)}); }
void S_game_Issue::from(EngineObjects& o, const Value& v, kern::game::Issue& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Bool>::from(o, *f, x.error); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.actor); if (const Value* f = v.field(2)) ev::Conv<ev::Text>::from(o, *f, x.text); }
Value S_game_JointComponent::to(EngineObjects& o, const kern::game::JointComponent& x) { return Value::structure({ev::Conv<ev::Enum<E_game_JointKind>>::to(o, x.kind), ev::Conv<ev::Int>::to(o, x.target), ev::Conv<ev::Vector>::to(o, x.anchor), ev::Conv<ev::Vector>::to(o, x.axis), ev::Conv<ev::Bool>::to(o, x.collide_connected), ev::Conv<ev::Bool>::to(o, x.enable_limit), ev::Conv<ev::Float>::to(o, x.lower), ev::Conv<ev::Float>::to(o, x.upper), ev::Conv<ev::Float>::to(o, x.reference), ev::Conv<ev::Bool>::to(o, x.enable_motor), ev::Conv<ev::Float>::to(o, x.motor_speed), ev::Conv<ev::Float>::to(o, x.max_motor_force), ev::Conv<ev::Bool>::to(o, x.enable_spring), ev::Conv<ev::Float>::to(o, x.spring_hertz), ev::Conv<ev::Float>::to(o, x.spring_damping), ev::Conv<ev::Float>::to(o, x.length), ev::Conv<ev::Float>::to(o, x.break_force), ev::Conv<ev::Float>::to(o, x.break_torque), ev::Conv<ev::Bool>::to(o, x.servo), ev::Conv<ev::Float>::to(o, x.servo_target), ev::Conv<ev::Float>::to(o, x.servo_stiffness), ev::Conv<ev::Float>::to(o, x.servo_damping), ev::Conv<ev::Float>::to(o, x.servo_friction), ev::Conv<ev::Text>::to(o, x.break_sound)}); }
void S_game_JointComponent::from(EngineObjects& o, const Value& v, kern::game::JointComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_game_JointKind>>::from(o, *f, x.kind); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.target); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.anchor); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.axis); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.collide_connected); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.enable_limit); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.lower); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.upper); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.reference); if (const Value* f = v.field(9)) ev::Conv<ev::Bool>::from(o, *f, x.enable_motor); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.motor_speed); if (const Value* f = v.field(11)) ev::Conv<ev::Float>::from(o, *f, x.max_motor_force); if (const Value* f = v.field(12)) ev::Conv<ev::Bool>::from(o, *f, x.enable_spring); if (const Value* f = v.field(13)) ev::Conv<ev::Float>::from(o, *f, x.spring_hertz); if (const Value* f = v.field(14)) ev::Conv<ev::Float>::from(o, *f, x.spring_damping); if (const Value* f = v.field(15)) ev::Conv<ev::Float>::from(o, *f, x.length); if (const Value* f = v.field(16)) ev::Conv<ev::Float>::from(o, *f, x.break_force); if (const Value* f = v.field(17)) ev::Conv<ev::Float>::from(o, *f, x.break_torque); if (const Value* f = v.field(18)) ev::Conv<ev::Bool>::from(o, *f, x.servo); if (const Value* f = v.field(19)) ev::Conv<ev::Float>::from(o, *f, x.servo_target); if (const Value* f = v.field(20)) ev::Conv<ev::Float>::from(o, *f, x.servo_stiffness); if (const Value* f = v.field(21)) ev::Conv<ev::Float>::from(o, *f, x.servo_damping); if (const Value* f = v.field(22)) ev::Conv<ev::Float>::from(o, *f, x.servo_friction); if (const Value* f = v.field(23)) ev::Conv<ev::Text>::from(o, *f, x.break_sound); }
Value S_game_Level::to(EngineObjects& o, const kern::game::Level& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::Struct<S_game_WorldSettings>>::to(o, x.world), ev::Conv<ev::List<ev::Struct<S_game_Actor>>>::to(o, x.actors), ev::Conv<ev::List<ev::Int>>::to(o, x.textures), ev::Conv<ev::List<ev::Int>>::to(o, x.sounds), ev::Conv<ev::List<ev::Int>>::to(o, x.motions), ev::Conv<ev::Struct<S_logic_Graph>>::to(o, x.logic), ev::Conv<ev::Struct<S_game_EditorCamera>>::to(o, x.camera), ev::Conv<ev::Int>::to(o, x.next_id)}); }
void S_game_Level::from(EngineObjects& o, const Value& v, kern::game::Level& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::Struct<S_game_WorldSettings>>::from(o, *f, x.world); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Struct<S_game_Actor>>>::from(o, *f, x.actors); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.textures); if (const Value* f = v.field(4)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.sounds); if (const Value* f = v.field(5)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.motions); if (const Value* f = v.field(6)) ev::Conv<ev::Struct<S_logic_Graph>>::from(o, *f, x.logic); if (const Value* f = v.field(7)) ev::Conv<ev::Struct<S_game_EditorCamera>>::from(o, *f, x.camera); if (const Value* f = v.field(8)) ev::Conv<ev::Int>::from(o, *f, x.next_id); }
Value S_game_LightComponent::to(EngineObjects& o, const kern::game::LightComponent& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.color), ev::Conv<ev::Float>::to(o, x.intensity), ev::Conv<ev::Float>::to(o, x.range)}); }
void S_game_LightComponent::from(EngineObjects& o, const Value& v, kern::game::LightComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.color); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.intensity); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.range); }
Value S_game_MaterialDesc::to(EngineObjects& o, const kern::game::MaterialDesc& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.color), ev::Conv<ev::Float>::to(o, x.roughness), ev::Conv<ev::Float>::to(o, x.metalness), ev::Conv<ev::Vector>::to(o, x.emissive), ev::Conv<ev::Float>::to(o, x.emissive_strength), ev::Conv<ev::Float>::to(o, x.opacity), ev::Conv<ev::Bool>::to(o, x.unlit), ev::Conv<ev::Bool>::to(o, x.cast_shadow), ev::Conv<ev::Bool>::to(o, x.double_sided), ev::Conv<ev::Text>::to(o, x.texture), ev::Conv<ev::Float>::to(o, x.tiling)}); }
void S_game_MaterialDesc::from(EngineObjects& o, const Value& v, kern::game::MaterialDesc& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.color); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.roughness); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.metalness); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.emissive); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.emissive_strength); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.opacity); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.unlit); if (const Value* f = v.field(7)) ev::Conv<ev::Bool>::from(o, *f, x.cast_shadow); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.double_sided); if (const Value* f = v.field(9)) ev::Conv<ev::Text>::from(o, *f, x.texture); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.tiling); }
Value S_game_MeshComponent::to(EngineObjects& o, const kern::game::MeshComponent& x) { return Value::structure({ev::Conv<ev::Enum<E_game_Shape>>::to(o, x.shape), ev::Conv<ev::Vector>::to(o, x.size), ev::Conv<ev::Float>::to(o, x.radius), ev::Conv<ev::Float>::to(o, x.height), ev::Conv<ev::Int>::to(o, x.segments), ev::Conv<ev::Text>::to(o, x.asset), ev::Conv<ev::Bool>::to(o, x.collision), ev::Conv<ev::Float>::to(o, x.friction), ev::Conv<ev::Float>::to(o, x.restitution), ev::Conv<ev::Float>::to(o, x.rolling_resistance), ev::Conv<ev::Text>::to(o, x.surface), ev::Conv<ev::Struct<S_game_MaterialDesc>>::to(o, x.material)}); }
void S_game_MeshComponent::from(EngineObjects& o, const Value& v, kern::game::MeshComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_game_Shape>>::from(o, *f, x.shape); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.size); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.radius); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.height); if (const Value* f = v.field(4)) ev::Conv<ev::Int>::from(o, *f, x.segments); if (const Value* f = v.field(5)) ev::Conv<ev::Text>::from(o, *f, x.asset); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.collision); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.friction); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.restitution); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.rolling_resistance); if (const Value* f = v.field(10)) ev::Conv<ev::Text>::from(o, *f, x.surface); if (const Value* f = v.field(11)) ev::Conv<ev::Struct<S_game_MaterialDesc>>::from(o, *f, x.material); }
Value S_game_MotionKey::to(EngineObjects& o, const kern::game::MotionKey& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.time), ev::Conv<ev::List<ev::Float>>::to(o, x.joints)}); }
void S_game_MotionKey::from(EngineObjects& o, const Value& v, kern::game::MotionKey& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.time); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.joints); }
Value S_game_MoverComponent::to(EngineObjects& o, const kern::game::MoverComponent& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.offset), ev::Conv<ev::Float>::to(o, x.period), ev::Conv<ev::Float>::to(o, x.phase), ev::Conv<ev::Enum<E_game_MoverMode>>::to(o, x.mode)}); }
void S_game_MoverComponent::from(EngineObjects& o, const Value& v, kern::game::MoverComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.offset); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.period); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.phase); if (const Value* f = v.field(3)) ev::Conv<ev::Enum<E_game_MoverMode>>::from(o, *f, x.mode); }
Value S_game_PickupComponent::to(EngineObjects& o, const kern::game::PickupComponent& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.points), ev::Conv<ev::Vector>::to(o, x.burst_color), ev::Conv<ev::Float>::to(o, x.respawn), ev::Conv<ev::Text>::to(o, x.sound), ev::Conv<ev::Float>::to(o, x.heal), ev::Conv<ev::Int>::to(o, x.ammo)}); }
void S_game_PickupComponent::from(EngineObjects& o, const Value& v, kern::game::PickupComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.points); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.burst_color); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.respawn); if (const Value* f = v.field(3)) ev::Conv<ev::Text>::from(o, *f, x.sound); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.heal); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.ammo); }
Value S_game_PlayInput::to(EngineObjects& o, const kern::game::PlayInput& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.move_x), ev::Conv<ev::Float>::to(o, x.move_y), ev::Conv<ev::Float>::to(o, x.up), ev::Conv<ev::Bool>::to(o, x.jump), ev::Conv<ev::Bool>::to(o, x.boost), ev::Conv<ev::Bool>::to(o, x.camera), ev::Conv<ev::Bool>::to(o, x.handbrake), ev::Conv<ev::Int>::to(o, x.robot), ev::Conv<ev::Float>::to(o, x.look_x), ev::Conv<ev::Float>::to(o, x.look_y), ev::Conv<ev::Float>::to(o, x.turn_deg), ev::Conv<ev::Float>::to(o, x.speed_scale), ev::Conv<ev::Float>::to(o, x.zoom), ev::Conv<ev::Float>::to(o, x.view_yaw), ev::Conv<ev::Bool>::to(o, x.has_view_yaw), ev::Conv<ev::Float>::to(o, x.view_pitch), ev::Conv<ev::Bool>::to(o, x.fire), ev::Conv<ev::Bool>::to(o, x.aim), ev::Conv<ev::Bool>::to(o, x.reload), ev::Conv<ev::Bool>::to(o, x.crouch), ev::Conv<ev::Bool>::to(o, x.interact), ev::Conv<ev::Float>::to(o, x.look_stick_x), ev::Conv<ev::Float>::to(o, x.look_stick_y), ev::Conv<ev::Bool>::to(o, x.gamepad), ev::Conv<ev::Float>::to(o, x.rewind)}); }
void S_game_PlayInput::from(EngineObjects& o, const Value& v, kern::game::PlayInput& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.move_x); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.move_y); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.up); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.jump); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.boost); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.camera); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.handbrake); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.robot); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.look_x); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.look_y); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.turn_deg); if (const Value* f = v.field(11)) ev::Conv<ev::Float>::from(o, *f, x.speed_scale); if (const Value* f = v.field(12)) ev::Conv<ev::Float>::from(o, *f, x.zoom); if (const Value* f = v.field(13)) ev::Conv<ev::Float>::from(o, *f, x.view_yaw); if (const Value* f = v.field(14)) ev::Conv<ev::Bool>::from(o, *f, x.has_view_yaw); if (const Value* f = v.field(15)) ev::Conv<ev::Float>::from(o, *f, x.view_pitch); if (const Value* f = v.field(16)) ev::Conv<ev::Bool>::from(o, *f, x.fire); if (const Value* f = v.field(17)) ev::Conv<ev::Bool>::from(o, *f, x.aim); if (const Value* f = v.field(18)) ev::Conv<ev::Bool>::from(o, *f, x.reload); if (const Value* f = v.field(19)) ev::Conv<ev::Bool>::from(o, *f, x.crouch); if (const Value* f = v.field(20)) ev::Conv<ev::Bool>::from(o, *f, x.interact); if (const Value* f = v.field(21)) ev::Conv<ev::Float>::from(o, *f, x.look_stick_x); if (const Value* f = v.field(22)) ev::Conv<ev::Float>::from(o, *f, x.look_stick_y); if (const Value* f = v.field(23)) ev::Conv<ev::Bool>::from(o, *f, x.gamepad); if (const Value* f = v.field(24)) ev::Conv<ev::Float>::from(o, *f, x.rewind); }
Value S_game_Rng::to(EngineObjects& o, const kern::game::Rng& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.s)}); }
void S_game_Rng::from(EngineObjects& o, const Value& v, kern::game::Rng& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.s); }
Value S_game_RotatorComponent::to(EngineObjects& o, const kern::game::RotatorComponent& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.rate)}); }
void S_game_RotatorComponent::from(EngineObjects& o, const Value& v, kern::game::RotatorComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.rate); }
Value S_game_ShapePlan::to(EngineObjects& o, const kern::game::ShapePlan& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.actor), ev::Conv<ev::Enum<E_physics_ShapeKind>>::to(o, x.kind), ev::Conv<ev::Vector>::to(o, x.center), ev::Conv<ev::Struct<S_Quat>>::to(o, x.rotation), ev::Conv<ev::Vector>::to(o, x.size), ev::Conv<ev::Float>::to(o, x.radius), ev::Conv<ev::Float>::to(o, x.length), ev::Conv<ev::List<ev::Vector>>::to(o, x.points), ev::Conv<ev::List<ev::Int>>::to(o, x.indices), ev::Conv<ev::Struct<S_game_ColliderComponent>>::to(o, x.source), ev::Conv<ev::Bool>::to(o, x.automatic)}); }
void S_game_ShapePlan::from(EngineObjects& o, const Value& v, kern::game::ShapePlan& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.actor); if (const Value* f = v.field(1)) ev::Conv<ev::Enum<E_physics_ShapeKind>>::from(o, *f, x.kind); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.center); if (const Value* f = v.field(3)) ev::Conv<ev::Struct<S_Quat>>::from(o, *f, x.rotation); if (const Value* f = v.field(4)) ev::Conv<ev::Vector>::from(o, *f, x.size); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.radius); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.length); if (const Value* f = v.field(7)) ev::Conv<ev::List<ev::Vector>>::from(o, *f, x.points); if (const Value* f = v.field(8)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.indices); if (const Value* f = v.field(9)) ev::Conv<ev::Struct<S_game_ColliderComponent>>::from(o, *f, x.source); if (const Value* f = v.field(10)) ev::Conv<ev::Bool>::from(o, *f, x.automatic); }
Value S_game_ShotEvent::to(EngineObjects& o, const kern::game::ShotEvent& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.from), ev::Conv<ev::Vector>::to(o, x.to), ev::Conv<ev::Int>::to(o, x.shooter), ev::Conv<ev::Int>::to(o, x.hit), ev::Conv<ev::Bool>::to(o, x.impact), ev::Conv<ev::Bool>::to(o, x.head), ev::Conv<ev::Vector>::to(o, x.normal), ev::Conv<ev::Int>::to(o, x.surface)}); }
void S_game_ShotEvent::from(EngineObjects& o, const Value& v, kern::game::ShotEvent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.from); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.to); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.shooter); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.hit); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.impact); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.head); if (const Value* f = v.field(6)) ev::Conv<ev::Vector>::from(o, *f, x.normal); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.surface); }
Value S_game_SimStats::to(EngineObjects& o, const kern::game::SimStats& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.bodies), ev::Conv<ev::Int>::to(o, x.awake), ev::Conv<ev::Int>::to(o, x.shapes), ev::Conv<ev::Int>::to(o, x.joints), ev::Conv<ev::Int>::to(o, x.broken_joints), ev::Conv<ev::Int>::to(o, x.contacts), ev::Conv<ev::Int64>::to(o, x.steps)}); }
void S_game_SimStats::from(EngineObjects& o, const Value& v, kern::game::SimStats& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.bodies); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.awake); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.shapes); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.joints); if (const Value* f = v.field(4)) ev::Conv<ev::Int>::from(o, *f, x.broken_joints); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.contacts); if (const Value* f = v.field(6)) ev::Conv<ev::Int64>::from(o, *f, x.steps); }
Value S_game_SoldierState::to(EngineObjects& o, const kern::game::SoldierState& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.health), ev::Conv<ev::Float>::to(o, x.max_health), ev::Conv<ev::Float>::to(o, x.armor), ev::Conv<ev::Int>::to(o, x.ammo), ev::Conv<ev::Int>::to(o, x.magazine), ev::Conv<ev::Int>::to(o, x.reserve), ev::Conv<ev::Float>::to(o, x.reload), ev::Conv<ev::Float>::to(o, x.spread), ev::Conv<ev::Float>::to(o, x.respawn_in), ev::Conv<ev::Int>::to(o, x.hits), ev::Conv<ev::Int>::to(o, x.damages), ev::Conv<ev::Bool>::to(o, x.hit_head), ev::Conv<ev::Bool>::to(o, x.hit_kill), ev::Conv<ev::Bool>::to(o, x.dead), ev::Conv<ev::Float>::to(o, x.damage_yaw)}); }
void S_game_SoldierState::from(EngineObjects& o, const Value& v, kern::game::SoldierState& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.health); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.max_health); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.armor); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.ammo); if (const Value* f = v.field(4)) ev::Conv<ev::Int>::from(o, *f, x.magazine); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.reserve); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.reload); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.spread); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.respawn_in); if (const Value* f = v.field(9)) ev::Conv<ev::Int>::from(o, *f, x.hits); if (const Value* f = v.field(10)) ev::Conv<ev::Int>::from(o, *f, x.damages); if (const Value* f = v.field(11)) ev::Conv<ev::Bool>::from(o, *f, x.hit_head); if (const Value* f = v.field(12)) ev::Conv<ev::Bool>::from(o, *f, x.hit_kill); if (const Value* f = v.field(13)) ev::Conv<ev::Bool>::from(o, *f, x.dead); if (const Value* f = v.field(14)) ev::Conv<ev::Float>::from(o, *f, x.damage_yaw); }
Value S_game_SoundCue::to(EngineObjects& o, const kern::game::SoundCue& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.sound), ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Float>::to(o, x.volume), ev::Conv<ev::Float>::to(o, x.pitch), ev::Conv<ev::Bool>::to(o, x.spatial), ev::Conv<ev::Enum<E_audio_Bus>>::to(o, x.bus), ev::Conv<ev::Int>::to(o, x.actor), ev::Conv<ev::Bool>::to(o, x.voice), ev::Conv<ev::Bool>::to(o, x.silence)}); }
void S_game_SoundCue::from(EngineObjects& o, const Value& v, kern::game::SoundCue& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.sound); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.volume); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.pitch); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.spatial); if (const Value* f = v.field(5)) ev::Conv<ev::Enum<E_audio_Bus>>::from(o, *f, x.bus); if (const Value* f = v.field(6)) ev::Conv<ev::Int>::from(o, *f, x.actor); if (const Value* f = v.field(7)) ev::Conv<ev::Bool>::from(o, *f, x.voice); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.silence); }
Value S_game_Transform::to(EngineObjects& o, const kern::game::Transform& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Vector>::to(o, x.rotation), ev::Conv<ev::Vector>::to(o, x.scale)}); }
void S_game_Transform::from(EngineObjects& o, const Value& v, kern::game::Transform& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.rotation); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.scale); }
Value S_game_TriggerComponent::to(EngineObjects& o, const kern::game::TriggerComponent& x) { return Value::structure({ev::Conv<ev::Enum<E_game_TriggerAction>>::to(o, x.action), ev::Conv<ev::Text>::to(o, x.message), ev::Conv<ev::Bool>::to(o, x.once), ev::Conv<ev::Text>::to(o, x.sound), ev::Conv<ev::Float>::to(o, x.strength), ev::Conv<ev::Int>::to(o, x.target)}); }
void S_game_TriggerComponent::from(EngineObjects& o, const Value& v, kern::game::TriggerComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_game_TriggerAction>>::from(o, *f, x.action); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.message); if (const Value* f = v.field(2)) ev::Conv<ev::Bool>::from(o, *f, x.once); if (const Value* f = v.field(3)) ev::Conv<ev::Text>::from(o, *f, x.sound); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.strength); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.target); }
Value S_game_WeaponComponent::to(EngineObjects& o, const kern::game::WeaponComponent& x) { return Value::structure({ev::Conv<ev::Enum<E_game_WeaponKind>>::to(o, x.kind), ev::Conv<ev::Float>::to(o, x.damage), ev::Conv<ev::Float>::to(o, x.fire_rate), ev::Conv<ev::Bool>::to(o, x.automatic), ev::Conv<ev::Int>::to(o, x.magazine), ev::Conv<ev::Int>::to(o, x.reserve), ev::Conv<ev::Float>::to(o, x.reload_time), ev::Conv<ev::Float>::to(o, x.spread), ev::Conv<ev::Float>::to(o, x.aim_spread), ev::Conv<ev::Float>::to(o, x.recoil), ev::Conv<ev::Float>::to(o, x.range), ev::Conv<ev::Float>::to(o, x.headshot), ev::Conv<ev::Float>::to(o, x.impulse), ev::Conv<ev::Int>::to(o, x.pellets), ev::Conv<ev::Float>::to(o, x.aim_fov), ev::Conv<ev::Text>::to(o, x.fire_sound), ev::Conv<ev::Text>::to(o, x.reload_sound)}); }
void S_game_WeaponComponent::from(EngineObjects& o, const Value& v, kern::game::WeaponComponent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_game_WeaponKind>>::from(o, *f, x.kind); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.damage); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.fire_rate); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.automatic); if (const Value* f = v.field(4)) ev::Conv<ev::Int>::from(o, *f, x.magazine); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.reserve); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.reload_time); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.spread); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.aim_spread); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.recoil); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.range); if (const Value* f = v.field(11)) ev::Conv<ev::Float>::from(o, *f, x.headshot); if (const Value* f = v.field(12)) ev::Conv<ev::Float>::from(o, *f, x.impulse); if (const Value* f = v.field(13)) ev::Conv<ev::Int>::from(o, *f, x.pellets); if (const Value* f = v.field(14)) ev::Conv<ev::Float>::from(o, *f, x.aim_fov); if (const Value* f = v.field(15)) ev::Conv<ev::Text>::from(o, *f, x.fire_sound); if (const Value* f = v.field(16)) ev::Conv<ev::Text>::from(o, *f, x.reload_sound); }
Value S_game_WorldSettings::to(EngineObjects& o, const kern::game::WorldSettings& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.channels), ev::Conv<ev::Vector>::to(o, x.gravity), ev::Conv<ev::Int>::to(o, x.substeps), ev::Conv<ev::Float>::to(o, x.step_hz), ev::Conv<ev::Float>::to(o, x.time_of_day), ev::Conv<ev::Float>::to(o, x.day_length), ev::Conv<ev::Float>::to(o, x.sun_azimuth), ev::Conv<ev::Float>::to(o, x.fog_density), ev::Conv<ev::Float>::to(o, x.exposure), ev::Conv<ev::Float>::to(o, x.ao_radius), ev::Conv<ev::Float>::to(o, x.shadow_distance), ev::Conv<ev::Bool>::to(o, x.clouds), ev::Conv<ev::Vector>::to(o, x.ground_color), ev::Conv<ev::Bool>::to(o, x.ground), ev::Conv<ev::Float>::to(o, x.ground_friction), ev::Conv<ev::Float>::to(o, x.ground_restitution), ev::Conv<ev::Text>::to(o, x.ground_surface), ev::Conv<ev::Float>::to(o, x.contact_hertz), ev::Conv<ev::Float>::to(o, x.contact_damping), ev::Conv<ev::Float>::to(o, x.contact_speed), ev::Conv<ev::Float>::to(o, x.kill_z), ev::Conv<ev::Text>::to(o, x.title), ev::Conv<ev::Enum<E_game_GameGoal>>::to(o, x.goal), ev::Conv<ev::Float>::to(o, x.time_limit), ev::Conv<ev::Float>::to(o, x.score_target), ev::Conv<ev::Int>::to(o, x.player), ev::Conv<ev::Bool>::to(o, x.observer), ev::Conv<ev::Float>::to(o, x.master_volume), ev::Conv<ev::Text>::to(o, x.music), ev::Conv<ev::Float>::to(o, x.music_volume), ev::Conv<ev::Text>::to(o, x.ambience), ev::Conv<ev::Float>::to(o, x.ambience_volume), ev::Conv<ev::Float>::to(o, x.reverb), ev::Conv<ev::Float>::to(o, x.doppler)}); }
void S_game_WorldSettings::from(EngineObjects& o, const Value& v, kern::game::WorldSettings& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.channels); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.gravity); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.substeps); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.step_hz); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.time_of_day); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.day_length); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.sun_azimuth); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.fog_density); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.exposure); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.ao_radius); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.shadow_distance); if (const Value* f = v.field(11)) ev::Conv<ev::Bool>::from(o, *f, x.clouds); if (const Value* f = v.field(12)) ev::Conv<ev::Vector>::from(o, *f, x.ground_color); if (const Value* f = v.field(13)) ev::Conv<ev::Bool>::from(o, *f, x.ground); if (const Value* f = v.field(14)) ev::Conv<ev::Float>::from(o, *f, x.ground_friction); if (const Value* f = v.field(15)) ev::Conv<ev::Float>::from(o, *f, x.ground_restitution); if (const Value* f = v.field(16)) ev::Conv<ev::Text>::from(o, *f, x.ground_surface); if (const Value* f = v.field(17)) ev::Conv<ev::Float>::from(o, *f, x.contact_hertz); if (const Value* f = v.field(18)) ev::Conv<ev::Float>::from(o, *f, x.contact_damping); if (const Value* f = v.field(19)) ev::Conv<ev::Float>::from(o, *f, x.contact_speed); if (const Value* f = v.field(20)) ev::Conv<ev::Float>::from(o, *f, x.kill_z); if (const Value* f = v.field(21)) ev::Conv<ev::Text>::from(o, *f, x.title); if (const Value* f = v.field(22)) ev::Conv<ev::Enum<E_game_GameGoal>>::from(o, *f, x.goal); if (const Value* f = v.field(23)) ev::Conv<ev::Float>::from(o, *f, x.time_limit); if (const Value* f = v.field(24)) ev::Conv<ev::Float>::from(o, *f, x.score_target); if (const Value* f = v.field(25)) ev::Conv<ev::Int>::from(o, *f, x.player); if (const Value* f = v.field(26)) ev::Conv<ev::Bool>::from(o, *f, x.observer); if (const Value* f = v.field(27)) ev::Conv<ev::Float>::from(o, *f, x.master_volume); if (const Value* f = v.field(28)) ev::Conv<ev::Text>::from(o, *f, x.music); if (const Value* f = v.field(29)) ev::Conv<ev::Float>::from(o, *f, x.music_volume); if (const Value* f = v.field(30)) ev::Conv<ev::Text>::from(o, *f, x.ambience); if (const Value* f = v.field(31)) ev::Conv<ev::Float>::from(o, *f, x.ambience_volume); if (const Value* f = v.field(32)) ev::Conv<ev::Float>::from(o, *f, x.reverb); if (const Value* f = v.field(33)) ev::Conv<ev::Float>::from(o, *f, x.doppler); }
Value S_logic_CommentBox::to(EngineObjects& o, const kern::logic::CommentBox& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.id), ev::Conv<ev::Float>::to(o, x.x), ev::Conv<ev::Float>::to(o, x.y), ev::Conv<ev::Float>::to(o, x.w), ev::Conv<ev::Float>::to(o, x.h), ev::Conv<ev::Text>::to(o, x.text), ev::Conv<ev::Int>::to(o, x.color)}); }
void S_logic_CommentBox::from(EngineObjects& o, const Value& v, kern::logic::CommentBox& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.id); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.x); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.y); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.w); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.h); if (const Value* f = v.field(5)) ev::Conv<ev::Text>::from(o, *f, x.text); if (const Value* f = v.field(6)) ev::Conv<ev::Int>::from(o, *f, x.color); }
Value S_logic_EnumType::to(EngineObjects& o, const kern::logic::EnumType& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::List<ev::Text>>::to(o, x.values), ev::Conv<ev::Text>::to(o, x.help), ev::Conv<ev::Bool>::to(o, x.project), ev::Conv<ev::Text>::to(o, x.from)}); }
void S_logic_EnumType::from(EngineObjects& o, const Value& v, kern::logic::EnumType& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Text>>::from(o, *f, x.values); if (const Value* f = v.field(2)) ev::Conv<ev::Text>::from(o, *f, x.help); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.project); if (const Value* f = v.field(4)) ev::Conv<ev::Text>::from(o, *f, x.from); }
Value S_logic_Function::to(EngineObjects& o, const kern::logic::Function& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::List<ev::Int>>::to(o, x.inputs), ev::Conv<ev::List<ev::Int>>::to(o, x.outputs), ev::Conv<ev::List<ev::Int>>::to(o, x.locals), ev::Conv<ev::Bool>::to(o, x.pure), ev::Conv<ev::Bool>::to(o, x.external), ev::Conv<ev::Text>::to(o, x.remote), ev::Conv<ev::Text>::to(o, x.from), ev::Conv<ev::Text>::to(o, x.exported), ev::Conv<ev::Text>::to(o, x.wire), ev::Conv<ev::Text>::to(o, x.help)}); }
void S_logic_Function::from(EngineObjects& o, const Value& v, kern::logic::Function& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.inputs); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.outputs); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.locals); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.pure); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.external); if (const Value* f = v.field(6)) ev::Conv<ev::Text>::from(o, *f, x.remote); if (const Value* f = v.field(7)) ev::Conv<ev::Text>::from(o, *f, x.from); if (const Value* f = v.field(8)) ev::Conv<ev::Text>::from(o, *f, x.exported); if (const Value* f = v.field(9)) ev::Conv<ev::Text>::from(o, *f, x.wire); if (const Value* f = v.field(10)) ev::Conv<ev::Text>::from(o, *f, x.help); }
Value S_logic_Graph::to(EngineObjects& o, const kern::logic::Graph& x) { return Value::structure({ev::Conv<ev::List<ev::Struct<S_logic_Node>>>::to(o, x.nodes), ev::Conv<ev::List<ev::Struct<S_logic_Link>>>::to(o, x.links), ev::Conv<ev::List<ev::Int>>::to(o, x.variables), ev::Conv<ev::List<ev::Struct<S_logic_StateMachine>>>::to(o, x.machines), ev::Conv<ev::List<ev::Struct<S_logic_Function>>>::to(o, x.functions), ev::Conv<ev::List<ev::Struct<S_logic_EnumType>>>::to(o, x.enums), ev::Conv<ev::List<ev::Struct<S_logic_StructType>>>::to(o, x.structs), ev::Conv<ev::List<ev::Struct<S_logic_CommentBox>>>::to(o, x.comments), ev::Conv<ev::Int>::to(o, x.next_id), ev::Conv<ev::Bool>::to(o, x.library), ev::Conv<ev::List<ev::Text>>::to(o, x.uses), ev::Conv<ev::Text>::to(o, x.implements)}); }
void S_logic_Graph::from(EngineObjects& o, const Value& v, kern::logic::Graph& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Struct<S_logic_Node>>>::from(o, *f, x.nodes); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Struct<S_logic_Link>>>::from(o, *f, x.links); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.variables); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Struct<S_logic_StateMachine>>>::from(o, *f, x.machines); if (const Value* f = v.field(4)) ev::Conv<ev::List<ev::Struct<S_logic_Function>>>::from(o, *f, x.functions); if (const Value* f = v.field(5)) ev::Conv<ev::List<ev::Struct<S_logic_EnumType>>>::from(o, *f, x.enums); if (const Value* f = v.field(6)) ev::Conv<ev::List<ev::Struct<S_logic_StructType>>>::from(o, *f, x.structs); if (const Value* f = v.field(7)) ev::Conv<ev::List<ev::Struct<S_logic_CommentBox>>>::from(o, *f, x.comments); if (const Value* f = v.field(8)) ev::Conv<ev::Int>::from(o, *f, x.next_id); if (const Value* f = v.field(9)) ev::Conv<ev::Bool>::from(o, *f, x.library); if (const Value* f = v.field(10)) ev::Conv<ev::List<ev::Text>>::from(o, *f, x.uses); if (const Value* f = v.field(11)) ev::Conv<ev::Text>::from(o, *f, x.implements); }
Value S_logic_Link::to(EngineObjects& o, const kern::logic::Link& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.from), ev::Conv<ev::Text>::to(o, x.out), ev::Conv<ev::Int>::to(o, x.to), ev::Conv<ev::Text>::to(o, x.in)}); }
void S_logic_Link::from(EngineObjects& o, const Value& v, kern::logic::Link& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.from); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.out); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.to); if (const Value* f = v.field(3)) ev::Conv<ev::Text>::from(o, *f, x.in); }
Value S_logic_Node::to(EngineObjects& o, const kern::logic::Node& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.id), ev::Conv<ev::Text>::to(o, x.type), ev::Conv<ev::Float>::to(o, x.x), ev::Conv<ev::Float>::to(o, x.y), ev::Conv<ev::List<ev::Pair<ev::Text, ev::Int>>>::to(o, x.values), ev::Conv<ev::List<ev::Pair<ev::Text, ev::Text>>>::to(o, x.params), ev::Conv<ev::Text>::to(o, x.comment)}); }
void S_logic_Node::from(EngineObjects& o, const Value& v, kern::logic::Node& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.id); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.type); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.x); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.y); if (const Value* f = v.field(4)) ev::Conv<ev::List<ev::Pair<ev::Text, ev::Int>>>::from(o, *f, x.values); if (const Value* f = v.field(5)) ev::Conv<ev::List<ev::Pair<ev::Text, ev::Text>>>::from(o, *f, x.params); if (const Value* f = v.field(6)) ev::Conv<ev::Text>::from(o, *f, x.comment); }
Value S_logic_State::to(EngineObjects& o, const kern::logic::State& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.id), ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::Float>::to(o, x.x), ev::Conv<ev::Float>::to(o, x.y), ev::Conv<ev::Bool>::to(o, x.initial)}); }
void S_logic_State::from(EngineObjects& o, const Value& v, kern::logic::State& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.id); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.x); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.y); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.initial); }
Value S_logic_StateMachine::to(EngineObjects& o, const kern::logic::StateMachine& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.id), ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::List<ev::Struct<S_logic_State>>>::to(o, x.states), ev::Conv<ev::List<ev::Struct<S_logic_Transition>>>::to(o, x.transitions)}); }
void S_logic_StateMachine::from(EngineObjects& o, const Value& v, kern::logic::StateMachine& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.id); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Struct<S_logic_State>>>::from(o, *f, x.states); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Struct<S_logic_Transition>>>::from(o, *f, x.transitions); }
Value S_logic_StructType::to(EngineObjects& o, const kern::logic::StructType& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::List<ev::Int>>::to(o, x.fields), ev::Conv<ev::Text>::to(o, x.help), ev::Conv<ev::Bool>::to(o, x.project), ev::Conv<ev::Text>::to(o, x.from)}); }
void S_logic_StructType::from(EngineObjects& o, const Value& v, kern::logic::StructType& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.fields); if (const Value* f = v.field(2)) ev::Conv<ev::Text>::from(o, *f, x.help); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.project); if (const Value* f = v.field(4)) ev::Conv<ev::Text>::from(o, *f, x.from); }
Value S_logic_Transition::to(EngineObjects& o, const kern::logic::Transition& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.id), ev::Conv<ev::Int>::to(o, x.from), ev::Conv<ev::Int>::to(o, x.to), ev::Conv<ev::Enum<E_logic_TriggerKind>>::to(o, x.trigger), ev::Conv<ev::Text>::to(o, x.event), ev::Conv<ev::Float>::to(o, x.seconds), ev::Conv<ev::Text>::to(o, x.variable), ev::Conv<ev::Text>::to(o, x.label)}); }
void S_logic_Transition::from(EngineObjects& o, const Value& v, kern::logic::Transition& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.id); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.from); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.to); if (const Value* f = v.field(3)) ev::Conv<ev::Enum<E_logic_TriggerKind>>::from(o, *f, x.trigger); if (const Value* f = v.field(4)) ev::Conv<ev::Text>::from(o, *f, x.event); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.seconds); if (const Value* f = v.field(6)) ev::Conv<ev::Text>::from(o, *f, x.variable); if (const Value* f = v.field(7)) ev::Conv<ev::Text>::from(o, *f, x.label); }
Value S_physics_Body::to(EngineObjects& o, const kern::physics::Body& x) { return Value::structure({ev::Conv<ev::Int64>::to(o, x.id)}); }
void S_physics_Body::from(EngineObjects& o, const Value& v, kern::physics::Body& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int64>::from(o, *f, x.id); }
Value S_physics_QueryFilter::to(EngineObjects& o, const kern::physics::QueryFilter& x) { return Value::structure({ev::Conv<ev::Int64>::to(o, x.mask_bits), ev::Conv<ev::Int64>::to(o, x.category_bits)}); }
void S_physics_QueryFilter::from(EngineObjects& o, const Value& v, kern::physics::QueryFilter& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int64>::from(o, *f, x.mask_bits); if (const Value* f = v.field(1)) ev::Conv<ev::Int64>::from(o, *f, x.category_bits); }

// engine.game.shape_mesh: kern::game::shape_mesh
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::MeshComponent const&>>();
  ev::take<ev::Struct<S_game_MeshComponent>>(o, in, 0, a0);
  decltype(auto) r = kern::game::shape_mesh(a0);
  ev::give<ev::Struct<S_MeshData>>(o, out, 0, r);
  return true;
}

// engine.game.shape_bounds: kern::game::shape_bounds
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::MeshComponent const&>>();
  ev::take<ev::Struct<S_game_MeshComponent>>(o, in, 0, a0);
  decltype(auto) r = kern::game::shape_bounds(a0);
  ev::give<ev::Struct<S_Aabb>>(o, out, 0, r);
  return true;
}

// engine.game.fit_collider: kern::game::fit_collider
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::MeshComponent const&>>();
  ev::take<ev::Struct<S_game_MeshComponent>>(o, in, 0, a0);
  decltype(auto) r = kern::game::fit_collider(a0);
  ev::give<ev::Struct<S_game_ColliderComponent>>(o, out, 0, r);
  return true;
}

// engine.game.shape_hull_points: kern::game::shape_hull_points
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::MeshComponent const&>>();
  ev::take<ev::Struct<S_game_MeshComponent>>(o, in, 0, a0);
  decltype(auto) r = kern::game::shape_hull_points(a0);
  ev::give<ev::List<ev::Vector>>(o, out, 0, r);
  return true;
}

// engine.game.build_city: kern::game::build_city
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::CityComponent const&>>();
  ev::take<ev::Struct<S_game_CityComponent>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::CityGeometry&>>();
  ev::take<ev::Struct<S_game_CityGeometry>>(o, in, 1, a1);
  decltype(auto) r = kern::game::build_city(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_game_CityGeometry>>(o, out, 1, a1);
  return true;
}

// engine.game.city_street_point: kern::game::city_street_point
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::CityComponent const&>>();
  ev::take<ev::Struct<S_game_CityComponent>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::Rng&>>();
  ev::take<ev::Struct<S_game_Rng>>(o, in, 1, a1);
  decltype(auto) r = kern::game::city_street_point(a0, a1);
  ev::give<ev::Vector>(o, out, 0, r);
  ev::give<ev::Struct<S_game_Rng>>(o, out, 1, a1);
  return true;
}

// engine.game.euler_to_quat: kern::game::euler_to_quat
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::game::euler_to_quat(std::move(a0));
  ev::give<ev::Struct<S_Quat>>(o, out, 0, r);
  return true;
}

// engine.game.quat_to_euler: kern::game::quat_to_euler
bool c7(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Quat>>();
  ev::take<ev::Struct<S_Quat>>(o, in, 0, a0);
  decltype(auto) r = kern::game::quat_to_euler(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.game.transform_matrix: kern::game::transform_matrix
bool c8(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Transform const&>>();
  ev::take<ev::Struct<S_game_Transform>>(o, in, 0, a0);
  decltype(auto) r = kern::game::transform_matrix(a0);
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

// engine.game.matrix_transform: kern::game::matrix_transform
bool c9(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Mat4 const&>>();
  ev::take<ev::Struct<S_Mat4>>(o, in, 0, a0);
  decltype(auto) r = kern::game::matrix_transform(a0);
  ev::give<ev::Struct<S_game_Transform>>(o, out, 0, r);
  return true;
}

// engine.game.shape_name: kern::game::shape_name
bool c10(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Shape>>();
  ev::take<ev::Enum<E_game_Shape>>(o, in, 0, a0);
  decltype(auto) r = kern::game::shape_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.parse_shape: kern::game::parse_shape
bool c11(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::Shape&>>();
  ev::take<ev::Enum<E_game_Shape>>(o, in, 1, a1);
  decltype(auto) r = kern::game::parse_shape(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Enum<E_game_Shape>>(o, out, 1, a1);
  return true;
}

// engine.game.motion_name: kern::game::motion_name
bool c12(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Motion>>();
  ev::take<ev::Enum<E_game_Motion>>(o, in, 0, a0);
  decltype(auto) r = kern::game::motion_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.collider_shape_name: kern::game::collider_shape_name
bool c13(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::ColliderShape>>();
  ev::take<ev::Enum<E_game_ColliderShape>>(o, in, 0, a0);
  decltype(auto) r = kern::game::collider_shape_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.joint_kind_name: kern::game::joint_kind_name
bool c14(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::JointKind>>();
  ev::take<ev::Enum<E_game_JointKind>>(o, in, 0, a0);
  decltype(auto) r = kern::game::joint_kind_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.control_mode_name: kern::game::control_mode_name
bool c15(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::ControlMode>>();
  ev::take<ev::Enum<E_game_ControlMode>>(o, in, 0, a0);
  decltype(auto) r = kern::game::control_mode_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.camera_mode_name: kern::game::camera_mode_name
bool c16(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::CameraMode>>();
  ev::take<ev::Enum<E_game_CameraMode>>(o, in, 0, a0);
  decltype(auto) r = kern::game::camera_mode_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.weapon_kind_name: kern::game::weapon_kind_name
bool c17(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::WeaponKind>>();
  ev::take<ev::Enum<E_game_WeaponKind>>(o, in, 0, a0);
  decltype(auto) r = kern::game::weapon_kind_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.weapon_preset: kern::game::weapon_preset
bool c18(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::WeaponKind>>();
  ev::take<ev::Enum<E_game_WeaponKind>>(o, in, 0, a0);
  decltype(auto) r = kern::game::weapon_preset(std::move(a0));
  ev::give<ev::Struct<S_game_WeaponComponent>>(o, out, 0, r);
  return true;
}

// engine.game.trigger_action_name: kern::game::trigger_action_name
bool c19(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::TriggerAction>>();
  ev::take<ev::Enum<E_game_TriggerAction>>(o, in, 0, a0);
  decltype(auto) r = kern::game::trigger_action_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.mover_mode_name: kern::game::mover_mode_name
bool c20(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::MoverMode>>();
  ev::take<ev::Enum<E_game_MoverMode>>(o, in, 0, a0);
  decltype(auto) r = kern::game::mover_mode_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.game_goal_name: kern::game::game_goal_name
bool c21(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::GameGoal>>();
  ev::take<ev::Enum<E_game_GameGoal>>(o, in, 0, a0);
  decltype(auto) r = kern::game::game_goal_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.motion_name_ok: kern::game::motion_name_ok
bool c22(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  decltype(auto) r = kern::game::motion_name_ok(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game.default_player: kern::game::default_player
bool c23(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Level const&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, a0);
  decltype(auto) r = kern::game::default_player(a0);
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game.channel_names: kern::game::channel_names
bool c24(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::WorldSettings const&>>();
  ev::take<ev::Struct<S_game_WorldSettings>>(o, in, 0, a0);
  decltype(auto) r = kern::game::channel_names(a0);
  ev::give<ev::List<ev::Text>>(o, out, 0, r);
  return true;
}

// engine.game.channel_bits: kern::game::channel_bits
bool c25(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::WorldSettings const&>>();
  ev::take<ev::Struct<S_game_WorldSettings>>(o, in, 0, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 1, a1);
  decltype(auto) r = kern::game::channel_bits(a0, a1);
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.level.find_2: kern::game::Level::find
bool c26(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::game::Level>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self.find(std::move(a0));
  ev::give<ev::List<ev::Struct<S_game_Actor>>>(o, out, 0, r);
  return true;
}

// engine.level.add: kern::game::Level::add
bool c27(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::game::Level>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Actor>>();
  ev::take<ev::Struct<S_game_Actor>>(o, in, 1, a0);
  decltype(auto) r = self.add(std::move(a0));
  ev::give<ev::Struct<S_game_Actor>>(o, out, 0, r);
  ev::give<ev::Struct<S_game_Level>>(o, out, 1, self);
  return true;
}

// engine.level.children: kern::game::Level::children
bool c28(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::game::Level>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self.children(std::move(a0));
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.level.world_matrix: kern::game::Level::world_matrix
bool c29(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::game::Level>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self.world_matrix(std::move(a0));
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

// engine.level.body_owner: kern::game::Level::body_owner
bool c30(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::game::Level>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self.body_owner(std::move(a0));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.level.is_ancestor: kern::game::Level::is_ancestor
bool c31(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::game::Level>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  decltype(auto) r = self.is_ancestor(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.level.texture: kern::game::Level::texture
bool c32(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::game::Level>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, self);
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self.texture(a0);
  out[0] = Value::integer(ev::handle_of<kern::game::TextureAsset>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.level.sound: kern::game::Level::sound
bool c33(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::game::Level>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, self);
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self.sound(a0);
  out[0] = Value::integer(ev::handle_of<kern::game::SoundAsset>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.level.motion: kern::game::Level::motion
bool c34(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::game::Level>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, self);
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self.motion(a0);
  out[0] = Value::integer(ev::handle_of<kern::game::MotionAsset>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.level.motion_2: kern::game::Level::motion
bool c35(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::game::Level>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, self);
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self.motion(a0);
  out[0] = Value::integer(ev::handle_of<kern::game::MotionAsset>(o, std::forward<decltype(r)>(r)));
  ev::give<ev::Struct<S_game_Level>>(o, out, 1, self);
  return true;
}

// engine.game.to_json: kern::game::to_json
bool c36(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Level const&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, a0);
  decltype(auto) r = kern::game::to_json(a0);
  out[0] = Value::integer(ev::handle_of<kern::json::Value>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.game.to_text: kern::game::to_text
bool c37(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Level const&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a1);
  decltype(auto) r = kern::game::to_text(a0, std::move(a1));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.from_json: kern::game::from_json
bool c38(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::json::Value* a0 = ev::object<kern::json::Value>(o, in, 0, "doc");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::Level&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 1, a1);
  auto a2 = ev::blank<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>();
  decltype(auto) r = kern::game::from_json(*a0, a1, &a2);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_game_Level>>(o, out, 1, a1);
  ev::give<ev::Text>(o, out, 2, a2);
  return true;
}

// engine.game.from_text: kern::game::from_text
bool c39(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::Level&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 1, a1);
  auto a2 = ev::blank<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>();
  decltype(auto) r = kern::game::from_text(a0, a1, &a2);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_game_Level>>(o, out, 1, a1);
  ev::give<ev::Text>(o, out, 2, a2);
  return true;
}

// engine.game.actor_to_json: kern::game::actor_to_json
bool c40(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Actor const&>>();
  ev::take<ev::Struct<S_game_Actor>>(o, in, 0, a0);
  decltype(auto) r = kern::game::actor_to_json(a0);
  out[0] = Value::integer(ev::handle_of<kern::json::Value>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.game.actor_from_json: kern::game::actor_from_json
bool c41(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::json::Value* a0 = ev::object<kern::json::Value>(o, in, 0, "v");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::Actor&>>();
  ev::take<ev::Struct<S_game_Actor>>(o, in, 1, a1);
  auto a2 = ev::blank<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>();
  decltype(auto) r = kern::game::actor_from_json(*a0, a1, &a2);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_game_Actor>>(o, out, 1, a1);
  ev::give<ev::Text>(o, out, 2, a2);
  return true;
}

// engine.game.world_to_json: kern::game::world_to_json
bool c42(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::WorldSettings const&>>();
  ev::take<ev::Struct<S_game_WorldSettings>>(o, in, 0, a0);
  decltype(auto) r = kern::game::world_to_json(a0);
  out[0] = Value::integer(ev::handle_of<kern::json::Value>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.game.world_from_json: kern::game::world_from_json
bool c43(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::json::Value* a0 = ev::object<kern::json::Value>(o, in, 0, "v");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::WorldSettings&>>();
  ev::take<ev::Struct<S_game_WorldSettings>>(o, in, 1, a1);
  auto a2 = ev::blank<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>();
  decltype(auto) r = kern::game::world_from_json(*a0, a1, &a2);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_game_WorldSettings>>(o, out, 1, a1);
  ev::give<ev::Text>(o, out, 2, a2);
  return true;
}

// engine.game.sound_reference_ok: kern::game::sound_reference_ok
bool c44(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Level const&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 1, a1);
  decltype(auto) r = kern::game::sound_reference_ok(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game.validate: kern::game::validate
bool c45(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Level const&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, a0);
  decltype(auto) r = kern::game::validate(a0);
  ev::give<ev::List<ev::Struct<S_game_Issue>>>(o, out, 0, r);
  return true;
}

// engine.game.hex_color: kern::game::hex_color
bool c46(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::game::hex_color(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.parse_hex_color: kern::game::parse_hex_color
bool c47(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3&>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::game::parse_hex_color(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Vector>(o, out, 1, a1);
  return true;
}

// engine.game.srgb_to_linear: kern::game::srgb_to_linear
bool c48(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::game::srgb_to_linear(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.game.logic_node_shapes_registered: kern::game::logic_node_shapes_registered
bool c49(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::game::logic_node_shapes_registered();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game.logic_node_types: kern::game::logic_node_types
bool c50(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::game::logic_node_types();
  ev::give<ev::List<ev::Text>>(o, out, 0, r);
  return true;
}

// engine.game.logic_nodes_registered: kern::game::logic_nodes_registered
bool c51(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::game::logic_nodes_registered();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.night_windows.name: kern::game::NightWindows::name
bool c52(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->name();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.night_windows.attributes: kern::game::NightWindows::attributes
bool c53(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->attributes();
  ev::give<ev::List<ev::Struct<S_ExtensionAttribute>>>(o, out, 0, r);
  return true;
}

// engine.night_windows.vertex_declarations: kern::game::NightWindows::vertex_declarations
bool c54(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->vertex_declarations();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.night_windows.vertex: kern::game::NightWindows::vertex
bool c55(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->vertex();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.night_windows.fragment_declarations: kern::game::NightWindows::fragment_declarations
bool c56(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->fragment_declarations();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.night_windows.fragment_surface: kern::game::NightWindows::fragment_surface
bool c57(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->fragment_surface();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.night_windows.bind: kern::game::NightWindows::bind
bool c58(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  kern::Program* a0 = ev::object<kern::Program>(o, in, 1, "p");
  if (!a0) return false;
  self->bind(*a0);
  return true;
}

// engine.night_windows.frame_uniforms: kern::game::NightWindows::frame_uniforms
bool c59(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  kern::UniformCache* a0 = ev::object<kern::UniformCache>(o, in, 1, "cache");
  if (!a0) return false;
  kern::Renderer* a1 = ev::object<kern::Renderer>(o, in, 2, "value2");
  if (!a1) return false;
  self->frame_uniforms(*a0, *a1);
  return true;
}

// engine.game.plan_bodies: kern::game::plan_bodies
bool c60(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Level const&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 0, a0);
  decltype(auto) r = kern::game::plan_bodies(a0);
  ev::give<ev::List<ev::Struct<S_game_BodyPlan>>>(o, out, 0, r);
  return true;
}

// engine.game.game_status_name: kern::game::game_status_name
bool c61(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::GameStatus>>();
  ev::take<ev::Enum<E_game_GameStatus>>(o, in, 0, a0);
  decltype(auto) r = kern::game::game_status_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game_world.set_layers: kern::game::GameWorld::set_layers
bool c62(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->set_layers(std::move(a0));
  return true;
}

// engine.game_world.sync: kern::game::GameWorld::sync
bool c63(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Level const&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 1, a0);
  self->sync(a0);
  return true;
}

// engine.game_world.clear: kern::game::GameWorld::clear
bool c64(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  self->clear();
  return true;
}

// engine.game_world.actor_for_mesh: kern::game::GameWorld::actor_for_mesh
bool c65(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->actor_for_mesh(std::move(a0));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.actor_matrix: kern::game::GameWorld::actor_matrix
bool c66(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Mat4&>>();
  ev::take<ev::Struct<S_Mat4>>(o, in, 2, a1);
  decltype(auto) r = self->actor_matrix(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_Mat4>>(o, out, 1, a1);
  return true;
}

// engine.game_world.actor_bounds: kern::game::GameWorld::actor_bounds
bool c67(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Aabb&>>();
  ev::take<ev::Struct<S_Aabb>>(o, in, 2, a1);
  decltype(auto) r = self->actor_bounds(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_Aabb>>(o, out, 1, a1);
  return true;
}

// engine.game_world.actor_meshes: kern::game::GameWorld::actor_meshes
bool c68(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<unsigned int, std::allocator<unsigned int> >&>>();
  ev::take<ev::List<ev::Int>>(o, in, 2, a1);
  self->actor_meshes(std::move(a0), a1);
  ev::give<ev::List<ev::Int>>(o, out, 0, a1);
  return true;
}

// engine.game_world.pending_textures: kern::game::GameWorld::pending_textures
bool c69(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->pending_textures();
  ev::give<ev::List<ev::Text>>(o, out, 0, r);
  return true;
}

// engine.game_world.set_texture_pixels: kern::game::GameWorld::set_texture_pixels
bool c70(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 3, a2);
  std::vector<unsigned char> a3;
  ev::take<ev::List<ev::Int>>(o, in, 4, a3);
  decltype(auto) r = self->set_texture_pixels(a0, std::move(a1), std::move(a2), a3.empty() ? nullptr : a3.data());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.apply_lighting: kern::game::GameWorld::apply_lighting
bool c71(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  decltype(auto) r = self->apply_lighting(std::move(a0));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.time_of_day: kern::game::GameWorld::time_of_day
bool c72(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->time_of_day();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.present: kern::game::GameWorld::present
bool c73(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  self->present(std::move(a0), std::move(a1));
  return true;
}

// engine.game_world.start: kern::game::GameWorld::start
bool c74(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Level const&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::SimMode>>();
  ev::take<ev::Enum<E_game_SimMode>>(o, in, 2, a1);
  decltype(auto) r = self->start(a0, std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.stop: kern::game::GameWorld::stop
bool c75(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  self->stop();
  return true;
}

// engine.game_world.running: kern::game::GameWorld::running
bool c76(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->running();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.session: kern::game::GameWorld::session
bool c77(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->session();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.mode: kern::game::GameWorld::mode
bool c78(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->mode();
  ev::give<ev::Enum<E_game_SimMode>>(o, out, 0, r);
  return true;
}

// engine.game_world.set_paused: kern::game::GameWorld::set_paused
bool c79(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_paused(std::move(a0));
  return true;
}

// engine.game_world.paused: kern::game::GameWorld::paused
bool c80(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->paused();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_mode: kern::game::GameWorld::set_mode
bool c81(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::SimMode>>();
  ev::take<ev::Enum<E_game_SimMode>>(o, in, 1, a0);
  decltype(auto) r = self->set_mode(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.step: kern::game::GameWorld::step
bool c82(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::PlayInput const&>>();
  ev::take<ev::Struct<S_game_PlayInput>>(o, in, 2, a1);
  self->step(std::move(a0), a1);
  return true;
}

// engine.game_world.step_once: kern::game::GameWorld::step_once
bool c83(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::PlayInput const&>>();
  ev::take<ev::Struct<S_game_PlayInput>>(o, in, 1, a0);
  self->step_once(a0);
  return true;
}

// engine.game_world.restart: kern::game::GameWorld::restart
bool c84(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->restart();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.update_camera: kern::game::GameWorld::update_camera
bool c85(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Camera&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::game::PlayInput const&>>();
  ev::take<ev::Struct<S_game_PlayInput>>(o, in, 3, a2);
  decltype(auto) r = self->update_camera(a0, std::move(a1), a2);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_Camera>>(o, out, 1, a0);
  return true;
}

// engine.game_world.player: kern::game::GameWorld::player
bool c86(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->player();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.camera_yaw: kern::game::GameWorld::camera_yaw
bool c87(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->camera_yaw();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.interior_view: kern::game::GameWorld::interior_view
bool c88(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->interior_view();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.camera_pitch: kern::game::GameWorld::camera_pitch
bool c89(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->camera_pitch();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.sim_time: kern::game::GameWorld::sim_time
bool c90(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->sim_time();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.add_pawn: kern::game::GameWorld::add_pawn
bool c91(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  decltype(auto) r = self->add_pawn(std::move(a0), std::move(a1));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.remove_pawn: kern::game::GameWorld::remove_pawn
bool c92(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->remove_pawn(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.pawn_source: kern::game::GameWorld::pawn_source
bool c93(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->pawn_source(std::move(a0));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.set_pawn_input: kern::game::GameWorld::set_pawn_input
bool c94(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::PlayInput const&>>();
  ev::take<ev::Struct<S_game_PlayInput>>(o, in, 2, a1);
  self->set_pawn_input(std::move(a0), a1);
  return true;
}

// engine.game_world.set_local_pawn: kern::game::GameWorld::set_local_pawn
bool c95(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->set_local_pawn(std::move(a0));
  return true;
}

// engine.game_world.pawns: kern::game::GameWorld::pawns
bool c96(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->pawns();
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.game_world.take_alive_changes: kern::game::GameWorld::take_alive_changes
bool c97(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::vector<std::pair<unsigned int, bool>, std::allocator<std::pair<unsigned int, bool> > >&>>();
  ev::take<ev::List<ev::Pair<ev::Int, ev::Bool>>>(o, in, 1, a0);
  self->take_alive_changes(a0);
  ev::give<ev::List<ev::Pair<ev::Int, ev::Bool>>>(o, out, 0, a0);
  return true;
}

// engine.game_world.dead_actors: kern::game::GameWorld::dead_actors
bool c98(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->dead_actors();
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.game_world.set_alive: kern::game::GameWorld::set_alive
bool c99(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  self->set_alive(std::move(a0), std::move(a1));
  return true;
}

// engine.game_world.networked_actors: kern::game::GameWorld::networked_actors
bool c100(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::vector<unsigned int, std::allocator<unsigned int> >&>>();
  ev::take<ev::List<ev::Int>>(o, in, 1, a0);
  self->networked_actors(a0);
  ev::give<ev::List<ev::Int>>(o, out, 0, a0);
  return true;
}

// engine.game_world.hud_mut: kern::game::GameWorld::hud_mut
bool c101(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->hud_mut();
  ev::give<ev::Struct<S_game_Hud>>(o, out, 0, r);
  return true;
}

// engine.game_world.hud_mut@set: what stands at the place kern::game::GameWorld::hud_mut gives
bool c101_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::take<ev::Struct<S_game_Hud>>(o, in, 1, self->hud_mut());
  return true;
}

// engine.game_world.start_replica: kern::game::GameWorld::start_replica
bool c102(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Level const&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 1, a0);
  decltype(auto) r = self->start_replica(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_replica_pose: kern::game::GameWorld::set_replica_pose
bool c103(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Quat>>();
  ev::take<ev::Struct<S_Quat>>(o, in, 3, a2);
  self->set_replica_pose(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.game_world.step_replica: kern::game::GameWorld::step_replica
bool c104(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::game::PlayInput const&>>();
  ev::take<ev::Struct<S_game_PlayInput>>(o, in, 3, a2);
  self->step_replica(std::move(a0), std::move(a1), a2);
  return true;
}

// engine.game_world.hud: kern::game::GameWorld::hud
bool c105(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->hud();
  ev::give<ev::Struct<S_game_Hud>>(o, out, 0, r);
  return true;
}

// engine.game_world.aim_blend: kern::game::GameWorld::aim_blend
bool c106(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->aim_blend();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.stats: kern::game::GameWorld::stats
bool c107(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->stats();
  ev::give<ev::Struct<S_game_SimStats>>(o, out, 0, r);
  return true;
}

// engine.game_world.level: kern::game::GameWorld::level
bool c108(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->level();
  ev::give<ev::Struct<S_game_Level>>(o, out, 0, r);
  return true;
}

// engine.game_world.physics: kern::game::GameWorld::physics
bool c109(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->physics();
  out[0] = Value::integer(ev::handle_of<kern::physics::World>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.game_world.actor_body: kern::game::GameWorld::actor_body
bool c110(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->actor_body(std::move(a0));
  ev::give<ev::Struct<S_physics_Body>>(o, out, 0, r);
  return true;
}

// engine.game_world.begin_grab: kern::game::GameWorld::begin_grab
bool c111(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  decltype(auto) r = self->begin_grab(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.move_grab: kern::game::GameWorld::move_grab
bool c112(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->move_grab(std::move(a0));
  return true;
}

// engine.game_world.end_grab: kern::game::GameWorld::end_grab
bool c113(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  self->end_grab();
  return true;
}

// engine.game_world.grabbing: kern::game::GameWorld::grabbing
bool c114(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->grabbing();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.debug_lines: kern::game::GameWorld::debug_lines
bool c115(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<std::vector<float, std::allocator<float> >&>>();
  ev::take<ev::List<ev::Float>>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<std::vector<float, std::allocator<float> >&>>();
  ev::take<ev::List<ev::Float>>(o, in, 5, a4);
  self->debug_lines(std::move(a0), std::move(a1), std::move(a2), a3, a4);
  ev::give<ev::List<ev::Float>>(o, out, 0, a3);
  ev::give<ev::List<ev::Float>>(o, out, 1, a4);
  return true;
}

// engine.game_world.events: kern::game::GameWorld::events
bool c116(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->events();
  ev::give<ev::List<ev::Text>>(o, out, 0, r);
  return true;
}

// engine.game_world.events@set: what stands at the place kern::game::GameWorld::events gives
bool c116_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::take<ev::List<ev::Text>>(o, in, 1, self->events());
  return true;
}

// engine.game_world.sound_cues: kern::game::GameWorld::sound_cues
bool c117(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->sound_cues();
  ev::give<ev::List<ev::Struct<S_game_SoundCue>>>(o, out, 0, r);
  return true;
}

// engine.game_world.sound_cues@set: what stands at the place kern::game::GameWorld::sound_cues gives
bool c117_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::take<ev::List<ev::Struct<S_game_SoundCue>>>(o, in, 1, self->sound_cues());
  return true;
}

// engine.game_world.shots: kern::game::GameWorld::shots
bool c118(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->shots();
  ev::give<ev::List<ev::Struct<S_game_ShotEvent>>>(o, out, 0, r);
  return true;
}

// engine.game_world.shots@set: what stands at the place kern::game::GameWorld::shots gives
bool c118_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::take<ev::List<ev::Struct<S_game_ShotEvent>>>(o, in, 1, self->shots());
  return true;
}

// engine.game_world.damage: kern::game::GameWorld::damage
bool c119(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 5, a4);
  self->damage(std::move(a0), std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  return true;
}

// engine.game_world.health_of: kern::game::GameWorld::health_of
bool c120(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->health_of(std::move(a0));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.set_health: kern::game::GameWorld::set_health
bool c121(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 4, a3);
  self->set_health(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  return true;
}

// engine.game_world.soldier_state: kern::game::GameWorld::soldier_state
bool c122(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::SoldierState&>>();
  ev::take<ev::Struct<S_game_SoldierState>>(o, in, 2, a1);
  decltype(auto) r = self->soldier_state(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_game_SoldierState>>(o, out, 1, a1);
  return true;
}

// engine.game_world.respawn_now: kern::game::GameWorld::respawn_now
bool c123(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->respawn_now(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.apply_soldier_state: kern::game::GameWorld::apply_soldier_state
bool c124(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::SoldierState const&>>();
  ev::take<ev::Struct<S_game_SoldierState>>(o, in, 1, a0);
  self->apply_soldier_state(a0);
  return true;
}

// engine.game_world.set_view: kern::game::GameWorld::set_view
bool c125(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Camera const&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 1, a0);
  self->set_view(a0);
  return true;
}

// engine.game_world.set_view_hand: kern::game::GameWorld::set_view_hand
bool c126(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  std::vector<kern::Mat4> a0;
  ev::take<ev::List<ev::Struct<S_Mat4>>>(o, in, 1, a0);
  self->set_view_hand(a0.empty() ? nullptr : a0.data());
  return true;
}

// engine.game_world.actor_alive: kern::game::GameWorld::actor_alive
bool c127(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->actor_alive(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.last_input: kern::game::GameWorld::last_input
bool c128(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->last_input();
  ev::give<ev::Struct<S_game_PlayInput>>(o, out, 0, r);
  return true;
}

// engine.game_world.logic_runtime: kern::game::GameWorld::logic_runtime
bool c129(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->logic_runtime();
  out[0] = Value::integer(ev::handle_of<kern::logic::Runtime>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.game_world.key_event: kern::game::GameWorld::key_event
bool c130(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  self->key_event(a0, std::move(a1));
  return true;
}

// engine.game_world.take_replicated_changes: kern::game::GameWorld::take_replicated_changes
bool c131(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::vector<kern::game::GameWorld::PropertyChange, std::allocator<kern::game::GameWorld::PropertyChange> >&>>();
  ev::take<ev::List<ev::Int>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<kern::game::GameWorld::SpawnChange, std::allocator<kern::game::GameWorld::SpawnChange> >&>>();
  ev::take<ev::List<ev::Struct<S_game_GameWorld_SpawnChange>>>(o, in, 2, a1);
  self->take_replicated_changes(a0, a1);
  ev::give<ev::List<ev::Int>>(o, out, 0, a0);
  ev::give<ev::List<ev::Struct<S_game_GameWorld_SpawnChange>>>(o, out, 1, a1);
  return true;
}

// engine.game_world.replicated_state: kern::game::GameWorld::replicated_state
bool c132(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::vector<kern::game::GameWorld::PropertyChange, std::allocator<kern::game::GameWorld::PropertyChange> >&>>();
  ev::take<ev::List<ev::Int>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<kern::game::GameWorld::SpawnChange, std::allocator<kern::game::GameWorld::SpawnChange> >&>>();
  ev::take<ev::List<ev::Struct<S_game_GameWorld_SpawnChange>>>(o, in, 2, a1);
  self->replicated_state(a0, a1);
  ev::give<ev::List<ev::Int>>(o, out, 0, a0);
  ev::give<ev::List<ev::Struct<S_game_GameWorld_SpawnChange>>>(o, out, 1, a1);
  return true;
}

// engine.game_world.set_property: kern::game::GameWorld::set_property
bool c133(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 2, a1);
  kern::json::Value* a2 = ev::object<kern::json::Value>(o, in, 3, "value");
  if (!a2) return false;
  decltype(auto) r = self->set_property(std::move(a0), a1, *a2);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_property_number: kern::game::GameWorld::set_property_number
bool c134(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 3, a2);
  decltype(auto) r = self->set_property_number(std::move(a0), a1, std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_property_flag: kern::game::GameWorld::set_property_flag
bool c135(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 3, a2);
  decltype(auto) r = self->set_property_flag(std::move(a0), a1, std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_property_text: kern::game::GameWorld::set_property_text
bool c136(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 3, a2);
  decltype(auto) r = self->set_property_text(std::move(a0), a1, a2);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.spawn_copy: kern::game::GameWorld::spawn_copy
bool c137(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 4, a3);
  decltype(auto) r = self->spawn_copy(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.teleport: kern::game::GameWorld::teleport
bool c138(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  std::vector<kern::Vec3> a2;
  ev::take<ev::List<ev::Vector>>(o, in, 3, a2);
  decltype(auto) r = self->teleport(std::move(a0), std::move(a1), a2.empty() ? nullptr : a2.data());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.actor_velocity: kern::game::GameWorld::actor_velocity
bool c139(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->actor_velocity(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.game_world.set_actor_velocity: kern::game::GameWorld::set_actor_velocity
bool c140(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  decltype(auto) r = self->set_actor_velocity(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.push_actor: kern::game::GameWorld::push_actor
bool c141(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 3, a2);
  decltype(auto) r = self->push_actor(std::move(a0), std::move(a1), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_simulate_physics: kern::game::GameWorld::set_simulate_physics
bool c142(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  decltype(auto) r = self->set_simulate_physics(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_joint_motor: kern::game::GameWorld::set_joint_motor
bool c143(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 5, a4);
  decltype(auto) r = self->set_joint_motor(std::move(a0), std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.find_actor: kern::game::GameWorld::find_actor
bool c144(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->find_actor(a0);
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.actors_with_tag: kern::game::GameWorld::actors_with_tag
bool c145(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<unsigned int, std::allocator<unsigned int> >&>>();
  ev::take<ev::List<ev::Int>>(o, in, 2, a1);
  self->actors_with_tag(a0, a1);
  ev::give<ev::List<ev::Int>>(o, out, 0, a1);
  return true;
}

// engine.game_world.has_tag: kern::game::GameWorld::has_tag
bool c146(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  decltype(auto) r = self->has_tag(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.line_trace: kern::game::GameWorld::line_trace
bool c147(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<unsigned int&>>();
  ev::take<ev::Int>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<kern::Vec3&>>();
  ev::take<ev::Vector>(o, in, 5, a4);
  auto a5 = ev::blank<std::remove_cvref_t<kern::Vec3&>>();
  ev::take<ev::Vector>(o, in, 6, a5);
  auto a6 = ev::blank<std::remove_cvref_t<kern::physics::QueryFilter>>();
  ev::take<ev::Struct<S_physics_QueryFilter>>(o, in, 7, a6);
  decltype(auto) r = self->line_trace(std::move(a0), std::move(a1), std::move(a2), a3, a4, a5, std::move(a6));
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Int>(o, out, 1, a3);
  ev::give<ev::Vector>(o, out, 2, a4);
  ev::give<ev::Vector>(o, out, 3, a5);
  return true;
}

// engine.game_world.add_score: kern::game::GameWorld::add_score
bool c148(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->add_score(std::move(a0));
  return true;
}

// engine.game_world.end_game: kern::game::GameWorld::end_game
bool c149(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 2, a1);
  self->end_game(std::move(a0), a1);
  return true;
}

// engine.game_world.show_message: kern::game::GameWorld::show_message
bool c150(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 1, a0);
  self->show_message(a0);
  return true;
}

// engine.game_world.play_sound: kern::game::GameWorld::play_sound
bool c151(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 5, a4);
  auto a5 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 6, a5);
  self->play_sound(a0, std::move(a1), std::move(a2), std::move(a3), std::move(a4), std::move(a5));
  return true;
}

// engine.game_world.particle_burst: kern::game::GameWorld::particle_burst
bool c152(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  self->particle_burst(std::move(a0), std::move(a1));
  return true;
}

// engine.game_world.shake_camera: kern::game::GameWorld::shake_camera
bool c153(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->shake_camera(std::move(a0));
  return true;
}

// engine.game_world.aim_view_model: kern::game::GameWorld::aim_view_model
bool c154(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 3, a2);
  self->aim_view_model(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.game_world.destroy_actor: kern::game::GameWorld::destroy_actor
bool c155(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->destroy_actor(std::move(a0));
  return true;
}

// engine.game_world.heal: kern::game::GameWorld::heal
bool c156(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  self->heal(std::move(a0), std::move(a1));
  return true;
}

// engine.game_world.give_ammo: kern::game::GameWorld::give_ammo
bool c157(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  self->give_ammo(std::move(a0), std::move(a1));
  return true;
}

// engine.game_world.call_logic_event: kern::game::GameWorld::call_logic_event
bool c158(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  self->call_logic_event(a0, std::move(a1));
  return true;
}

// engine.game_world.walker_sensors: kern::game::GameWorld::walker_sensors
bool c159(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::GameWorld::WalkerSensors&>>();
  ev::take<ev::Struct<S_game_GameWorld_WalkerSensors>>(o, in, 2, a1);
  decltype(auto) r = self->walker_sensors(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_game_GameWorld_WalkerSensors>>(o, out, 1, a1);
  return true;
}

// engine.game_world.walker_camera: kern::game::GameWorld::walker_camera
bool c160(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Camera&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 2, a1);
  decltype(auto) r = self->walker_camera(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_Camera>>(o, out, 1, a1);
  return true;
}

// engine.game_world.walker_command: kern::game::GameWorld::walker_command
bool c161(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 5, a4);
  decltype(auto) r = self->walker_command(std::move(a0), std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_look: kern::game::GameWorld::walker_look
bool c162(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 5, a4);
  decltype(auto) r = self->walker_look(std::move(a0), std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_set_autonomous: kern::game::GameWorld::walker_set_autonomous
bool c163(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  decltype(auto) r = self->walker_set_autonomous(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_quack: kern::game::GameWorld::walker_quack
bool c164(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_quack(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_skill: kern::game::GameWorld::walker_skill
bool c165(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  decltype(auto) r = self->walker_skill(std::move(a0), a1);
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_mouth: kern::game::GameWorld::walker_mouth
bool c166(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  decltype(auto) r = self->walker_mouth(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_holding: kern::game::GameWorld::walker_holding
bool c167(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_holding(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_pet: kern::game::GameWorld::walker_pet
bool c168(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  decltype(auto) r = self->walker_pet(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_hear: kern::game::GameWorld::walker_hear
bool c169(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  decltype(auto) r = self->walker_hear(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_servo_load: kern::game::GameWorld::walker_servo_load
bool c170(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float&>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float&>>();
  ev::take<ev::Float>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float&>>();
  ev::take<ev::Float>(o, in, 4, a3);
  decltype(auto) r = self->walker_servo_load(std::move(a0), a1, a2, a3);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Float>(o, out, 1, a1);
  ev::give<ev::Float>(o, out, 2, a2);
  ev::give<ev::Float>(o, out, 3, a3);
  return true;
}

// engine.game_world.walker_driving: kern::game::GameWorld::walker_driving
bool c171(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_driving(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game_world.set_walker_external: kern::game::GameWorld::set_walker_external
bool c172(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  decltype(auto) r = self->set_walker_external(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_walker_pose_edit: kern::game::GameWorld::set_walker_pose_edit
bool c173(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  decltype(auto) r = self->set_walker_pose_edit(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_walker_pose: kern::game::GameWorld::set_walker_pose
bool c174(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  std::vector<float> a1;
  ev::take<ev::List<ev::Float>>(o, in, 2, a1);
  if (a1.size() < 14) { o.error = "the list joints has fewer than 14 entries"; return false; }
  decltype(auto) r = self->set_walker_pose(std::move(a0), a1.empty() ? nullptr : a1.data());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_walker_action: kern::game::GameWorld::set_walker_action
bool c175(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  std::vector<float> a1;
  ev::take<ev::List<ev::Float>>(o, in, 2, a1);
  if (a1.size() < 14) { o.error = "the list action has fewer than 14 entries"; return false; }
  decltype(auto) r = self->set_walker_action(std::move(a0), a1.empty() ? nullptr : a1.data());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_tick_due: kern::game::GameWorld::walker_tick_due
bool c176(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_tick_due(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_probe: kern::game::GameWorld::walker_probe
bool c177(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::GameWorld::WalkerProbe&>>();
  ev::take<ev::Struct<S_game_GameWorld_WalkerProbe>>(o, in, 2, a1);
  decltype(auto) r = self->walker_probe(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_game_GameWorld_WalkerProbe>>(o, out, 1, a1);
  return true;
}

// engine.game_world.walker_push: kern::game::GameWorld::walker_push
bool c178(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  decltype(auto) r = self->walker_push(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_said: kern::game::GameWorld::walker_said
bool c179(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_said(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_joints: kern::game::GameWorld::walker_joints
bool c180(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_joints(std::move(a0));
  ev::give<ev::List<ev::Struct<S_game_GameWorld_WalkerJoint>>>(o, out, 0, r);
  return true;
}

// engine.game_world.set_walker_play_external: kern::game::GameWorld::set_walker_play_external
bool c181(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  decltype(auto) r = self->set_walker_play_external(std::move(a0), std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_walker_play_action: kern::game::GameWorld::set_walker_play_action
bool c182(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  std::vector<float> a1;
  ev::take<ev::List<ev::Float>>(o, in, 2, a1);
  decltype(auto) r = self->set_walker_play_action(std::move(a0), a1.empty() ? nullptr : a1.data());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_walker_detector: kern::game::GameWorld::set_walker_detector
bool c183(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::GameWorld::DetectorModel const&>>();
  ev::take<ev::Struct<S_game_GameWorld_DetectorModel>>(o, in, 2, a1);
  decltype(auto) r = self->set_walker_detector(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_play_due: kern::game::GameWorld::walker_play_due
bool c184(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_play_due(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_playing: kern::game::GameWorld::walker_playing
bool c185(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_playing(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_sees: kern::game::GameWorld::walker_sees
bool c186(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<bool&>>();
  ev::take<ev::Bool>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float&>>();
  ev::take<ev::Float>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<float&>>();
  ev::take<ev::Float>(o, in, 5, a4);
  decltype(auto) r = self->walker_sees(std::move(a0), std::move(a1), a2, a3, a4);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Bool>(o, out, 1, a2);
  ev::give<ev::Float>(o, out, 2, a3);
  ev::give<ev::Float>(o, out, 3, a4);
  return true;
}

// engine.game_world.walker_state: kern::game::GameWorld::walker_state
bool c187(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_state(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game_world.walkers: kern::game::GameWorld::walkers
bool c188(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->walkers();
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.game_world.art_stats: kern::game::GameWorld::art_stats
bool c189(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->art_stats();
  ev::give<ev::Struct<S_game_GameWorld_ArtStats>>(o, out, 0, r);
  return true;
}

// engine.game.blueprints_running: kern::game::blueprints_running
bool c190(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::logic::Runtime* a0 = ev::object<kern::logic::Runtime>(o, in, 0, "logic");
  if (!a0) return false;
  decltype(auto) r = kern::game::blueprints_running(*a0);
  ev::give<ev::List<ev::Struct<S_game_BlueprintRunning>>>(o, out, 0, r);
  return true;
}

// engine.game.blueprint_count: kern::game::blueprint_count
bool c191(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::logic::Runtime* a0 = ev::object<kern::logic::Runtime>(o, in, 0, "logic");
  if (!a0) return false;
  decltype(auto) r = kern::game::blueprint_count(*a0);
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game.blueprint_name: kern::game::blueprint_name
bool c192(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::logic::Runtime* a0 = ev::object<kern::logic::Runtime>(o, in, 0, "logic");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<std::size_t>>();
  ev::take<ev::Int>(o, in, 1, a1);
  decltype(auto) r = kern::game::blueprint_name(*a0, std::move(a1));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.blueprint_tier: kern::game::blueprint_tier
bool c193(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::logic::Runtime* a0 = ev::object<kern::logic::Runtime>(o, in, 0, "logic");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<std::size_t>>();
  ev::take<ev::Int>(o, in, 1, a1);
  decltype(auto) r = kern::game::blueprint_tier(*a0, std::move(a1));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.blueprint_functions: kern::game::blueprint_functions
bool c194(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::logic::Runtime* a0 = ev::object<kern::logic::Runtime>(o, in, 0, "logic");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<std::size_t>>();
  ev::take<ev::Int>(o, in, 1, a1);
  decltype(auto) r = kern::game::blueprint_functions(*a0, std::move(a1));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game.blueprint_nodes: kern::game::blueprint_nodes
bool c195(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::logic::Runtime* a0 = ev::object<kern::logic::Runtime>(o, in, 0, "logic");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<std::size_t>>();
  ev::take<ev::Int>(o, in, 1, a1);
  decltype(auto) r = kern::game::blueprint_nodes(*a0, std::move(a1));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game.blueprint_disabled: kern::game::blueprint_disabled
bool c196(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::logic::Runtime* a0 = ev::object<kern::logic::Runtime>(o, in, 0, "logic");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<std::size_t>>();
  ev::take<ev::Int>(o, in, 1, a1);
  decltype(auto) r = kern::game::blueprint_disabled(*a0, std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.sound_bank.mixer: kern::game::SoundBank::mixer
bool c197(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  decltype(auto) r = self->mixer();
  out[0] = Value::integer(ev::handle_of<kern::audio::Mixer>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.sound_bank.clip: kern::game::SoundBank::clip
bool c198(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->clip(a0);
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.sound_bank.request: kern::game::SoundBank::request
bool c199(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  self->request(a0);
  return true;
}

// engine.sound_bank.request_level: kern::game::SoundBank::request_level
bool c200(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Level const&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 1, a0);
  self->request_level(a0);
  return true;
}

// engine.sound_bank.sync_assets: kern::game::SoundBank::sync_assets
bool c201(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Level const&>>();
  ev::take<ev::Struct<S_game_Level>>(o, in, 1, a0);
  self->sync_assets(a0);
  return true;
}

// engine.sound_bank.prepare: kern::game::SoundBank::prepare
bool c202(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  decltype(auto) r = self->prepare(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.sound_bank.ready: kern::game::SoundBank::ready
bool c203(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->ready(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.sound_bank.failed: kern::game::SoundBank::failed
bool c204(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->failed(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.sound_bank.pending_imports: kern::game::SoundBank::pending_imports
bool c205(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  decltype(auto) r = self->pending_imports();
  ev::give<ev::List<ev::Text>>(o, out, 0, r);
  return true;
}

// engine.sound_bank.source: kern::game::SoundBank::source
bool c206(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->source(a0);
  ev::give<ev::List<ev::Text>>(o, out, 0, r);
  return true;
}

// engine.sound_bank.set_pcm: kern::game::SoundBank::set_pcm
bool c207(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::vector<float> a1;
  ev::take<ev::List<ev::Float>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 5, a4);
  if (ev::numbers(a2) > a1.size()) { o.error = "the list data has fewer entries than samples says"; return false; }
  decltype(auto) r = self->set_pcm(a0, a1.empty() ? nullptr : a1.data(), std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.sound_bank.set_failed: kern::game::SoundBank::set_failed
bool c208(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>>();
  ev::take<ev::Text>(o, in, 2, a1);
  self->set_failed(a0, std::move(a1));
  return true;
}

// engine.sound_bank.duration: kern::game::SoundBank::duration
bool c209(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->duration(a0);
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.sound_bank.clear: kern::game::SoundBank::clear
bool c210(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  self->clear();
  return true;
}

// engine.sound_bank.queued: kern::game::SoundBank::queued
bool c211(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  decltype(auto) r = self->queued();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_audio.begin: kern::game::GameAudio::begin
bool c212(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  kern::game::GameWorld* a0 = ev::object<kern::game::GameWorld>(o, in, 1, "world");
  if (!a0) return false;
  self->begin(*a0);
  return true;
}

// engine.game_audio.update: kern::game::GameAudio::update
bool c213(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  kern::game::GameWorld* a0 = ev::object<kern::game::GameWorld>(o, in, 1, "world");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::Camera const&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  self->update(*a0, a1, std::move(a2));
  return true;
}

// engine.game_audio.end: kern::game::GameAudio::end
bool c214(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->end(std::move(a0));
  return true;
}

// engine.game_audio.active: kern::game::GameAudio::active
bool c215(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  decltype(auto) r = self->active();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_audio.apply_settings: kern::game::GameAudio::apply_settings
bool c216(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::WorldSettings const&>>();
  ev::take<ev::Struct<S_game_WorldSettings>>(o, in, 1, a0);
  self->apply_settings(a0);
  return true;
}

// engine.game_audio.set_muted: kern::game::GameAudio::set_muted
bool c217(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_muted(std::move(a0));
  return true;
}

// engine.game_audio.muted: kern::game::GameAudio::muted
bool c218(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  decltype(auto) r = self->muted();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_audio.cues_played: kern::game::GameAudio::cues_played
bool c219(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  decltype(auto) r = self->cues_played();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_audio.cues_dropped: kern::game::GameAudio::cues_dropped
bool c220(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  decltype(auto) r = self->cues_dropped();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game.engine_sound_for: kern::game::engine_sound_for
bool c221(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::ControllerComponent const&>>();
  ev::take<ev::Struct<S_game_ControllerComponent>>(o, in, 0, a0);
  decltype(auto) r = kern::game::engine_sound_for(a0);
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game_audio.make: kern::game::GameAudio::GameAudio
bool c222(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::audio::Mixer* a0 = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!a0) return false;
  kern::game::SoundBank* a1 = ev::object<kern::game::SoundBank>(o, in, 1, "bank");
  if (!a1) return false;
  out[0] = Value::integer(o.own<kern::game::GameAudio>(std::unique_ptr<kern::game::GameAudio>(new kern::game::GameAudio(*a0, *a1))));
  return true;
}

// engine.game_audio.release: kern::game::GameAudio::~GameAudio
bool c223(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::GameAudio>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no GameAudio that was made";
    return false;
  }
  return true;
}

// engine.game_world.make: kern::game::GameWorld::GameWorld
bool c224(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::Renderer* a0 = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!a0) return false;
  kern::game::NightWindows* a1 = ev::maybe<kern::game::NightWindows>(o, in, 1, "windows");
  if (!o.error.empty()) return false;
  out[0] = Value::integer(o.own<kern::game::GameWorld>(std::unique_ptr<kern::game::GameWorld>(new kern::game::GameWorld(*a0, a1))));
  return true;
}

// engine.game_world.release: kern::game::GameWorld::~GameWorld
bool c225(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::GameWorld>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no GameWorld that was made";
    return false;
  }
  return true;
}

// engine.game_world.logic_native: kern::game::GameWorld::logic_native
bool c226(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::give<ev::Bool>(o, out, 0, self->logic_native);
  return true;
}

// engine.game_world.logic_native@set: what stands at the place kern::game::GameWorld::logic_native gives
bool c226_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::take<ev::Bool>(o, in, 1, self->logic_native);
  return true;
}

// engine.property_change.make: kern::game::GameWorld::PropertyChange::PropertyChange
bool c227(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::game::GameWorld::PropertyChange>(std::unique_ptr<kern::game::GameWorld::PropertyChange>(new kern::game::GameWorld::PropertyChange())));
  return true;
}

// engine.property_change.release: kern::game::GameWorld::PropertyChange::~PropertyChange
bool c228(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::GameWorld::PropertyChange>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no PropertyChange that was made";
    return false;
  }
  return true;
}

// engine.property_change.actor: kern::game::GameWorld::PropertyChange::actor
bool c229(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld::PropertyChange>(o, in, 0, "property_change");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->actor);
  return true;
}

// engine.property_change.actor@set: what stands at the place kern::game::GameWorld::PropertyChange::actor gives
bool c229_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld::PropertyChange>(o, in, 0, "property_change");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->actor);
  return true;
}

// engine.property_change.path: kern::game::GameWorld::PropertyChange::path
bool c230(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld::PropertyChange>(o, in, 0, "property_change");
  if (!self) return false;
  ev::give<ev::Text>(o, out, 0, self->path);
  return true;
}

// engine.property_change.path@set: what stands at the place kern::game::GameWorld::PropertyChange::path gives
bool c230_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld::PropertyChange>(o, in, 0, "property_change");
  if (!self) return false;
  ev::take<ev::Text>(o, in, 1, self->path);
  return true;
}

// engine.motion_asset.make: kern::game::MotionAsset::MotionAsset
bool c231(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::game::MotionAsset>(std::unique_ptr<kern::game::MotionAsset>(new kern::game::MotionAsset())));
  return true;
}

// engine.motion_asset.release: kern::game::MotionAsset::~MotionAsset
bool c232(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::MotionAsset>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no MotionAsset that was made";
    return false;
  }
  return true;
}

// engine.motion_asset.name: kern::game::MotionAsset::name
bool c233(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::MotionAsset>(o, in, 0, "motion_asset");
  if (!self) return false;
  ev::give<ev::Text>(o, out, 0, self->name);
  return true;
}

// engine.motion_asset.name@set: what stands at the place kern::game::MotionAsset::name gives
bool c233_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::MotionAsset>(o, in, 0, "motion_asset");
  if (!self) return false;
  ev::take<ev::Text>(o, in, 1, self->name);
  return true;
}

// engine.motion_asset.duration: kern::game::MotionAsset::duration
bool c234(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::MotionAsset>(o, in, 0, "motion_asset");
  if (!self) return false;
  ev::give<ev::Float>(o, out, 0, self->duration);
  return true;
}

// engine.motion_asset.duration@set: what stands at the place kern::game::MotionAsset::duration gives
bool c234_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::MotionAsset>(o, in, 0, "motion_asset");
  if (!self) return false;
  ev::take<ev::Float>(o, in, 1, self->duration);
  return true;
}

// engine.motion_asset.keys: kern::game::MotionAsset::keys
bool c235(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::MotionAsset>(o, in, 0, "motion_asset");
  if (!self) return false;
  ev::give<ev::List<ev::Struct<S_game_MotionKey>>>(o, out, 0, self->keys);
  return true;
}

// engine.motion_asset.keys@set: what stands at the place kern::game::MotionAsset::keys gives
bool c235_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::MotionAsset>(o, in, 0, "motion_asset");
  if (!self) return false;
  ev::take<ev::List<ev::Struct<S_game_MotionKey>>>(o, in, 1, self->keys);
  return true;
}

// engine.night_windows.make: kern::game::NightWindows::NightWindows
bool c236(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::game::NightWindows, kern::ShaderExtension>(std::unique_ptr<kern::game::NightWindows>(new kern::game::NightWindows())));
  return true;
}

// engine.night_windows.release: kern::game::NightWindows::~NightWindows
bool c237(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::NightWindows>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no NightWindows that was made";
    return false;
  }
  return true;
}

// engine.night_windows.night: kern::game::NightWindows::night
bool c238(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  ev::give<ev::Float>(o, out, 0, self->night);
  return true;
}

// engine.night_windows.night@set: what stands at the place kern::game::NightWindows::night gives
bool c238_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  ev::take<ev::Float>(o, in, 1, self->night);
  return true;
}

// engine.soldier_art_delete.make: kern::game::SoldierArtDelete::SoldierArtDelete
bool c239(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::game::SoldierArtDelete>(std::unique_ptr<kern::game::SoldierArtDelete>(new kern::game::SoldierArtDelete())));
  return true;
}

// engine.soldier_art_delete.release: kern::game::SoldierArtDelete::~SoldierArtDelete
bool c240(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::SoldierArtDelete>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no SoldierArtDelete that was made";
    return false;
  }
  return true;
}

// engine.sound_asset.make: kern::game::SoundAsset::SoundAsset
bool c241(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::game::SoundAsset>(std::unique_ptr<kern::game::SoundAsset>(new kern::game::SoundAsset())));
  return true;
}

// engine.sound_asset.release: kern::game::SoundAsset::~SoundAsset
bool c242(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::SoundAsset>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no SoundAsset that was made";
    return false;
  }
  return true;
}

// engine.sound_asset.id: kern::game::SoundAsset::id
bool c243(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundAsset>(o, in, 0, "sound_asset");
  if (!self) return false;
  ev::give<ev::Text>(o, out, 0, self->id);
  return true;
}

// engine.sound_asset.id@set: what stands at the place kern::game::SoundAsset::id gives
bool c243_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::SoundAsset>(o, in, 0, "sound_asset");
  if (!self) return false;
  ev::take<ev::Text>(o, in, 1, self->id);
  return true;
}

// engine.sound_asset.name: kern::game::SoundAsset::name
bool c244(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundAsset>(o, in, 0, "sound_asset");
  if (!self) return false;
  ev::give<ev::Text>(o, out, 0, self->name);
  return true;
}

// engine.sound_asset.name@set: what stands at the place kern::game::SoundAsset::name gives
bool c244_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::SoundAsset>(o, in, 0, "sound_asset");
  if (!self) return false;
  ev::take<ev::Text>(o, in, 1, self->name);
  return true;
}

// engine.sound_asset.duration: kern::game::SoundAsset::duration
bool c245(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundAsset>(o, in, 0, "sound_asset");
  if (!self) return false;
  ev::give<ev::Float>(o, out, 0, self->duration);
  return true;
}

// engine.sound_asset.duration@set: what stands at the place kern::game::SoundAsset::duration gives
bool c245_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::SoundAsset>(o, in, 0, "sound_asset");
  if (!self) return false;
  ev::take<ev::Float>(o, in, 1, self->duration);
  return true;
}

// engine.sound_bank.make: kern::game::SoundBank::SoundBank
bool c246(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::audio::Mixer* a0 = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!a0) return false;
  out[0] = Value::integer(o.own<kern::game::SoundBank>(std::unique_ptr<kern::game::SoundBank>(new kern::game::SoundBank(*a0))));
  return true;
}

// engine.sound_bank.release: kern::game::SoundBank::~SoundBank
bool c247(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::SoundBank>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no SoundBank that was made";
    return false;
  }
  return true;
}

// engine.texture_asset.make: kern::game::TextureAsset::TextureAsset
bool c248(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::game::TextureAsset>(std::unique_ptr<kern::game::TextureAsset>(new kern::game::TextureAsset())));
  return true;
}

// engine.texture_asset.release: kern::game::TextureAsset::~TextureAsset
bool c249(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::TextureAsset>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no TextureAsset that was made";
    return false;
  }
  return true;
}

// engine.texture_asset.id: kern::game::TextureAsset::id
bool c250(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::TextureAsset>(o, in, 0, "texture_asset");
  if (!self) return false;
  ev::give<ev::Text>(o, out, 0, self->id);
  return true;
}

// engine.texture_asset.id@set: what stands at the place kern::game::TextureAsset::id gives
bool c250_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::TextureAsset>(o, in, 0, "texture_asset");
  if (!self) return false;
  ev::take<ev::Text>(o, in, 1, self->id);
  return true;
}

// engine.texture_asset.name: kern::game::TextureAsset::name
bool c251(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::TextureAsset>(o, in, 0, "texture_asset");
  if (!self) return false;
  ev::give<ev::Text>(o, out, 0, self->name);
  return true;
}

// engine.texture_asset.name@set: what stands at the place kern::game::TextureAsset::name gives
bool c251_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::TextureAsset>(o, in, 0, "texture_asset");
  if (!self) return false;
  ev::take<ev::Text>(o, in, 1, self->name);
  return true;
}

// engine.texture_asset.width: kern::game::TextureAsset::width
bool c252(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::TextureAsset>(o, in, 0, "texture_asset");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->width);
  return true;
}

// engine.texture_asset.width@set: what stands at the place kern::game::TextureAsset::width gives
bool c252_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::TextureAsset>(o, in, 0, "texture_asset");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->width);
  return true;
}

// engine.texture_asset.height: kern::game::TextureAsset::height
bool c253(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::TextureAsset>(o, in, 0, "texture_asset");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->height);
  return true;
}

// engine.texture_asset.height@set: what stands at the place kern::game::TextureAsset::height gives
bool c253_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::TextureAsset>(o, in, 0, "texture_asset");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->height);
  return true;
}

const EngineCall kCalls[] = {
    {"engine.game.shape_mesh", &c0},
    {"engine.game.shape_bounds", &c1},
    {"engine.game.fit_collider", &c2},
    {"engine.game.shape_hull_points", &c3},
    {"engine.game.build_city", &c4},
    {"engine.game.city_street_point", &c5},
    {"engine.game.euler_to_quat", &c6},
    {"engine.game.quat_to_euler", &c7},
    {"engine.game.transform_matrix", &c8},
    {"engine.game.matrix_transform", &c9},
    {"engine.game.shape_name", &c10},
    {"engine.game.parse_shape", &c11},
    {"engine.game.motion_name", &c12},
    {"engine.game.collider_shape_name", &c13},
    {"engine.game.joint_kind_name", &c14},
    {"engine.game.control_mode_name", &c15},
    {"engine.game.camera_mode_name", &c16},
    {"engine.game.weapon_kind_name", &c17},
    {"engine.game.weapon_preset", &c18},
    {"engine.game.trigger_action_name", &c19},
    {"engine.game.mover_mode_name", &c20},
    {"engine.game.game_goal_name", &c21},
    {"engine.game.motion_name_ok", &c22},
    {"engine.game.default_player", &c23},
    {"engine.game.channel_names", &c24},
    {"engine.game.channel_bits", &c25},
    {"engine.level.find_2", &c26},
    {"engine.level.add", &c27},
    {"engine.level.children", &c28},
    {"engine.level.world_matrix", &c29},
    {"engine.level.body_owner", &c30},
    {"engine.level.is_ancestor", &c31},
    {"engine.level.texture", &c32},
    {"engine.level.sound", &c33},
    {"engine.level.motion", &c34},
    {"engine.level.motion_2", &c35},
    {"engine.game.to_json", &c36},
    {"engine.game.to_text", &c37},
    {"engine.game.from_json", &c38},
    {"engine.game.from_text", &c39},
    {"engine.game.actor_to_json", &c40},
    {"engine.game.actor_from_json", &c41},
    {"engine.game.world_to_json", &c42},
    {"engine.game.world_from_json", &c43},
    {"engine.game.sound_reference_ok", &c44},
    {"engine.game.validate", &c45},
    {"engine.game.hex_color", &c46},
    {"engine.game.parse_hex_color", &c47},
    {"engine.game.srgb_to_linear", &c48},
    {"engine.game.logic_node_shapes_registered", &c49},
    {"engine.game.logic_node_types", &c50},
    {"engine.game.logic_nodes_registered", &c51},
    {"engine.night_windows.name", &c52},
    {"engine.night_windows.attributes", &c53},
    {"engine.night_windows.vertex_declarations", &c54},
    {"engine.night_windows.vertex", &c55},
    {"engine.night_windows.fragment_declarations", &c56},
    {"engine.night_windows.fragment_surface", &c57},
    {"engine.night_windows.bind", &c58},
    {"engine.night_windows.frame_uniforms", &c59},
    {"engine.game.plan_bodies", &c60},
    {"engine.game.game_status_name", &c61},
    {"engine.game_world.set_layers", &c62},
    {"engine.game_world.sync", &c63},
    {"engine.game_world.clear", &c64},
    {"engine.game_world.actor_for_mesh", &c65},
    {"engine.game_world.actor_matrix", &c66},
    {"engine.game_world.actor_bounds", &c67},
    {"engine.game_world.actor_meshes", &c68},
    {"engine.game_world.pending_textures", &c69},
    {"engine.game_world.set_texture_pixels", &c70},
    {"engine.game_world.apply_lighting", &c71},
    {"engine.game_world.time_of_day", &c72},
    {"engine.game_world.present", &c73},
    {"engine.game_world.start", &c74},
    {"engine.game_world.stop", &c75},
    {"engine.game_world.running", &c76},
    {"engine.game_world.session", &c77},
    {"engine.game_world.mode", &c78},
    {"engine.game_world.set_paused", &c79},
    {"engine.game_world.paused", &c80},
    {"engine.game_world.set_mode", &c81},
    {"engine.game_world.step", &c82},
    {"engine.game_world.step_once", &c83},
    {"engine.game_world.restart", &c84},
    {"engine.game_world.update_camera", &c85},
    {"engine.game_world.player", &c86},
    {"engine.game_world.camera_yaw", &c87},
    {"engine.game_world.interior_view", &c88},
    {"engine.game_world.camera_pitch", &c89},
    {"engine.game_world.sim_time", &c90},
    {"engine.game_world.add_pawn", &c91},
    {"engine.game_world.remove_pawn", &c92},
    {"engine.game_world.pawn_source", &c93},
    {"engine.game_world.set_pawn_input", &c94},
    {"engine.game_world.set_local_pawn", &c95},
    {"engine.game_world.pawns", &c96},
    {"engine.game_world.take_alive_changes", &c97},
    {"engine.game_world.dead_actors", &c98},
    {"engine.game_world.set_alive", &c99},
    {"engine.game_world.networked_actors", &c100},
    {"engine.game_world.hud_mut", &c101},
    {"engine.game_world.start_replica", &c102},
    {"engine.game_world.set_replica_pose", &c103},
    {"engine.game_world.step_replica", &c104},
    {"engine.game_world.hud", &c105},
    {"engine.game_world.aim_blend", &c106},
    {"engine.game_world.stats", &c107},
    {"engine.game_world.level", &c108},
    {"engine.game_world.physics", &c109},
    {"engine.game_world.actor_body", &c110},
    {"engine.game_world.begin_grab", &c111},
    {"engine.game_world.move_grab", &c112},
    {"engine.game_world.end_grab", &c113},
    {"engine.game_world.grabbing", &c114},
    {"engine.game_world.debug_lines", &c115},
    {"engine.game_world.events", &c116},
    {"engine.game_world.sound_cues", &c117},
    {"engine.game_world.shots", &c118},
    {"engine.game_world.damage", &c119},
    {"engine.game_world.health_of", &c120},
    {"engine.game_world.set_health", &c121},
    {"engine.game_world.soldier_state", &c122},
    {"engine.game_world.respawn_now", &c123},
    {"engine.game_world.apply_soldier_state", &c124},
    {"engine.game_world.set_view", &c125},
    {"engine.game_world.set_view_hand", &c126},
    {"engine.game_world.actor_alive", &c127},
    {"engine.game_world.last_input", &c128},
    {"engine.game_world.logic_runtime", &c129},
    {"engine.game_world.key_event", &c130},
    {"engine.game_world.take_replicated_changes", &c131},
    {"engine.game_world.replicated_state", &c132},
    {"engine.game_world.set_property", &c133},
    {"engine.game_world.set_property_number", &c134},
    {"engine.game_world.set_property_flag", &c135},
    {"engine.game_world.set_property_text", &c136},
    {"engine.game_world.spawn_copy", &c137},
    {"engine.game_world.teleport", &c138},
    {"engine.game_world.actor_velocity", &c139},
    {"engine.game_world.set_actor_velocity", &c140},
    {"engine.game_world.push_actor", &c141},
    {"engine.game_world.set_simulate_physics", &c142},
    {"engine.game_world.set_joint_motor", &c143},
    {"engine.game_world.find_actor", &c144},
    {"engine.game_world.actors_with_tag", &c145},
    {"engine.game_world.has_tag", &c146},
    {"engine.game_world.line_trace", &c147},
    {"engine.game_world.add_score", &c148},
    {"engine.game_world.end_game", &c149},
    {"engine.game_world.show_message", &c150},
    {"engine.game_world.play_sound", &c151},
    {"engine.game_world.particle_burst", &c152},
    {"engine.game_world.shake_camera", &c153},
    {"engine.game_world.aim_view_model", &c154},
    {"engine.game_world.destroy_actor", &c155},
    {"engine.game_world.heal", &c156},
    {"engine.game_world.give_ammo", &c157},
    {"engine.game_world.call_logic_event", &c158},
    {"engine.game_world.walker_sensors", &c159},
    {"engine.game_world.walker_camera", &c160},
    {"engine.game_world.walker_command", &c161},
    {"engine.game_world.walker_look", &c162},
    {"engine.game_world.walker_set_autonomous", &c163},
    {"engine.game_world.walker_quack", &c164},
    {"engine.game_world.walker_skill", &c165},
    {"engine.game_world.walker_mouth", &c166},
    {"engine.game_world.walker_holding", &c167},
    {"engine.game_world.walker_pet", &c168},
    {"engine.game_world.walker_hear", &c169},
    {"engine.game_world.walker_servo_load", &c170},
    {"engine.game_world.walker_driving", &c171},
    {"engine.game_world.set_walker_external", &c172},
    {"engine.game_world.set_walker_pose_edit", &c173},
    {"engine.game_world.set_walker_pose", &c174},
    {"engine.game_world.set_walker_action", &c175},
    {"engine.game_world.walker_tick_due", &c176},
    {"engine.game_world.walker_probe", &c177},
    {"engine.game_world.walker_push", &c178},
    {"engine.game_world.walker_said", &c179},
    {"engine.game_world.walker_joints", &c180},
    {"engine.game_world.set_walker_play_external", &c181},
    {"engine.game_world.set_walker_play_action", &c182},
    {"engine.game_world.set_walker_detector", &c183},
    {"engine.game_world.walker_play_due", &c184},
    {"engine.game_world.walker_playing", &c185},
    {"engine.game_world.walker_sees", &c186},
    {"engine.game_world.walker_state", &c187},
    {"engine.game_world.walkers", &c188},
    {"engine.game_world.art_stats", &c189},
    {"engine.game.blueprints_running", &c190},
    {"engine.game.blueprint_count", &c191},
    {"engine.game.blueprint_name", &c192},
    {"engine.game.blueprint_tier", &c193},
    {"engine.game.blueprint_functions", &c194},
    {"engine.game.blueprint_nodes", &c195},
    {"engine.game.blueprint_disabled", &c196},
    {"engine.sound_bank.mixer", &c197},
    {"engine.sound_bank.clip", &c198},
    {"engine.sound_bank.request", &c199},
    {"engine.sound_bank.request_level", &c200},
    {"engine.sound_bank.sync_assets", &c201},
    {"engine.sound_bank.prepare", &c202},
    {"engine.sound_bank.ready", &c203},
    {"engine.sound_bank.failed", &c204},
    {"engine.sound_bank.pending_imports", &c205},
    {"engine.sound_bank.source", &c206},
    {"engine.sound_bank.set_pcm", &c207},
    {"engine.sound_bank.set_failed", &c208},
    {"engine.sound_bank.duration", &c209},
    {"engine.sound_bank.clear", &c210},
    {"engine.sound_bank.queued", &c211},
    {"engine.game_audio.begin", &c212},
    {"engine.game_audio.update", &c213},
    {"engine.game_audio.end", &c214},
    {"engine.game_audio.active", &c215},
    {"engine.game_audio.apply_settings", &c216},
    {"engine.game_audio.set_muted", &c217},
    {"engine.game_audio.muted", &c218},
    {"engine.game_audio.cues_played", &c219},
    {"engine.game_audio.cues_dropped", &c220},
    {"engine.game.engine_sound_for", &c221},
    {"engine.game_audio.make", &c222},
    {"engine.game_audio.release", &c223},
    {"engine.game_world.make", &c224},
    {"engine.game_world.release", &c225},
    {"engine.game_world.logic_native", &c226},
    {"engine.property_change.make", &c227},
    {"engine.property_change.release", &c228},
    {"engine.property_change.actor", &c229},
    {"engine.property_change.path", &c230},
    {"engine.motion_asset.make", &c231},
    {"engine.motion_asset.release", &c232},
    {"engine.motion_asset.name", &c233},
    {"engine.motion_asset.duration", &c234},
    {"engine.motion_asset.keys", &c235},
    {"engine.night_windows.make", &c236},
    {"engine.night_windows.release", &c237},
    {"engine.night_windows.night", &c238},
    {"engine.soldier_art_delete.make", &c239},
    {"engine.soldier_art_delete.release", &c240},
    {"engine.sound_asset.make", &c241},
    {"engine.sound_asset.release", &c242},
    {"engine.sound_asset.id", &c243},
    {"engine.sound_asset.name", &c244},
    {"engine.sound_asset.duration", &c245},
    {"engine.sound_bank.make", &c246},
    {"engine.sound_bank.release", &c247},
    {"engine.texture_asset.make", &c248},
    {"engine.texture_asset.release", &c249},
    {"engine.texture_asset.id", &c250},
    {"engine.texture_asset.name", &c251},
    {"engine.texture_asset.width", &c252},
    {"engine.texture_asset.height", &c253},
    {"engine.game_world.hud_mut@get", &c101},
    {"engine.game_world.hud_mut@set", &c101_set},
    {"engine.game_world.events@get", &c116},
    {"engine.game_world.events@set", &c116_set},
    {"engine.game_world.sound_cues@get", &c117},
    {"engine.game_world.sound_cues@set", &c117_set},
    {"engine.game_world.shots@get", &c118},
    {"engine.game_world.shots@set", &c118_set},
    {"engine.game_world.logic_native@get", &c226},
    {"engine.game_world.logic_native@set", &c226_set},
    {"engine.property_change.actor@get", &c229},
    {"engine.property_change.actor@set", &c229_set},
    {"engine.property_change.path@get", &c230},
    {"engine.property_change.path@set", &c230_set},
    {"engine.motion_asset.name@get", &c233},
    {"engine.motion_asset.name@set", &c233_set},
    {"engine.motion_asset.duration@get", &c234},
    {"engine.motion_asset.duration@set", &c234_set},
    {"engine.motion_asset.keys@get", &c235},
    {"engine.motion_asset.keys@set", &c235_set},
    {"engine.night_windows.night@get", &c238},
    {"engine.night_windows.night@set", &c238_set},
    {"engine.sound_asset.id@get", &c243},
    {"engine.sound_asset.id@set", &c243_set},
    {"engine.sound_asset.name@get", &c244},
    {"engine.sound_asset.name@set", &c244_set},
    {"engine.sound_asset.duration@get", &c245},
    {"engine.sound_asset.duration@set", &c245_set},
    {"engine.texture_asset.id@get", &c250},
    {"engine.texture_asset.id@set", &c250_set},
    {"engine.texture_asset.name@get", &c251},
    {"engine.texture_asset.name@set", &c251_set},
    {"engine.texture_asset.width@get", &c252},
    {"engine.texture_asset.width@set", &c252_set},
    {"engine.texture_asset.height@get", &c253},
    {"engine.texture_asset.height@set", &c253_set},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

const EngineMaker kMakers[] = {
    {EngineObjects::kind<kern::game::GameAudio>(), [](EngineObjects& o, i64 handle) -> void* { auto made = std::make_unique<ev::MadeWith<kern::game::GameAudio>>(); auto& d0 = made->keep(new kern::audio::Mixer()); auto& d1 = made->keep(new kern::audio::Mixer()); auto& d2 = made->keep(new kern::game::SoundBank(d1)); made->object.emplace(d0, d2); return o.put_made<kern::game::GameAudio>(handle, std::move(made)); }},
    {EngineObjects::kind<kern::game::GameWorld>(), [](EngineObjects& o, i64 handle) -> void* { auto made = std::make_unique<ev::MadeWith<kern::game::GameWorld>>(); auto& d0 = made->keep(new kern::Renderer()); made->object.emplace(d0); return o.put_made<kern::game::GameWorld>(handle, std::move(made)); }},
    {EngineObjects::kind<kern::game::GameWorld::PropertyChange>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::game::GameWorld::PropertyChange>(handle, std::unique_ptr<kern::game::GameWorld::PropertyChange>(new kern::game::GameWorld::PropertyChange())); }},
    {EngineObjects::kind<kern::game::MotionAsset>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::game::MotionAsset>(handle, std::unique_ptr<kern::game::MotionAsset>(new kern::game::MotionAsset())); }},
    {EngineObjects::kind<kern::game::NightWindows>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::game::NightWindows, kern::ShaderExtension>(handle, std::unique_ptr<kern::game::NightWindows>(new kern::game::NightWindows())); }},
    {EngineObjects::kind<kern::game::SoldierArtDelete>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::game::SoldierArtDelete>(handle, std::unique_ptr<kern::game::SoldierArtDelete>(new kern::game::SoldierArtDelete())); }},
    {EngineObjects::kind<kern::game::SoundAsset>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::game::SoundAsset>(handle, std::unique_ptr<kern::game::SoundAsset>(new kern::game::SoundAsset())); }},
    {EngineObjects::kind<kern::game::SoundBank>(), [](EngineObjects& o, i64 handle) -> void* { auto made = std::make_unique<ev::MadeWith<kern::game::SoundBank>>(); auto& d0 = made->keep(new kern::audio::Mixer()); made->object.emplace(d0); return o.put_made<kern::game::SoundBank>(handle, std::move(made)); }},
    {EngineObjects::kind<kern::game::TextureAsset>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::game::TextureAsset>(handle, std::unique_ptr<kern::game::TextureAsset>(new kern::game::TextureAsset())); }},
};
const EngineMakers kAllMakers(kMakers, std::size(kMakers));

}  // namespace
}  // namespace kern::logic
