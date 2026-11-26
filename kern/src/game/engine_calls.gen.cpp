// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/audio/audio.h"
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
struct E_game_GameStatus { using type = kern::game::GameStatus; static constexpr type values[] = {kern::game::GameStatus::inactive, kern::game::GameStatus::playing, kern::game::GameStatus::won, kern::game::GameStatus::lost}; };
struct E_game_Shape { using type = kern::game::Shape; static constexpr type values[] = {kern::game::Shape::box, kern::game::Shape::sphere, kern::game::Shape::cylinder, kern::game::Shape::cone, kern::game::Shape::capsule, kern::game::Shape::plane, kern::game::Shape::torus, kern::game::Shape::ramp, kern::game::Shape::mesh}; };
struct E_game_SimMode { using type = kern::game::SimMode; static constexpr type values[] = {kern::game::SimMode::none, kern::game::SimMode::simulate, kern::game::SimMode::play, kern::game::SimMode::replica}; };
struct S_Aabb { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Aabb& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Aabb& x); };
struct S_Camera { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Camera& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Camera& x); };
struct S_ExtensionAttribute { [[maybe_unused]] static Value to(EngineObjects& o, const kern::ExtensionAttribute& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::ExtensionAttribute& x); };
struct S_Mat4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Mat4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Mat4& x); };
struct S_Quat { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Quat& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Quat& x); };
struct S_game_BlueprintRunning { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::BlueprintRunning& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::BlueprintRunning& x); };
struct S_game_GameWorld_ArtStats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::ArtStats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::ArtStats& x); };
struct S_game_GameWorld_DetectorModel { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::DetectorModel& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::DetectorModel& x); };
struct S_game_GameWorld_SpawnChange { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::SpawnChange& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::SpawnChange& x); };
struct S_game_GameWorld_WalkerJoint { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::WalkerJoint& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::WalkerJoint& x); };
struct S_game_GameWorld_WalkerProbe { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::WalkerProbe& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::WalkerProbe& x); };
struct S_game_GameWorld_WalkerSensors { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::GameWorld::WalkerSensors& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::GameWorld::WalkerSensors& x); };
struct S_game_Hud { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::Hud& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::Hud& x); };
struct S_game_PlayInput { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::PlayInput& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::PlayInput& x); };
struct S_game_ShotEvent { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::ShotEvent& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::ShotEvent& x); };
struct S_game_SimStats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::SimStats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::SimStats& x); };
struct S_game_SoldierState { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::SoldierState& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::SoldierState& x); };
struct S_game_SoundCue { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::SoundCue& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::SoundCue& x); };
struct S_game_Transform { [[maybe_unused]] static Value to(EngineObjects& o, const kern::game::Transform& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::game::Transform& x); };
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
Value S_Quat::to(EngineObjects& o, const kern::Quat& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.x), ev::Conv<ev::Float>::to(o, x.y), ev::Conv<ev::Float>::to(o, x.z), ev::Conv<ev::Float>::to(o, x.w)}); }
void S_Quat::from(EngineObjects& o, const Value& v, kern::Quat& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.x); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.y); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.z); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.w); }
Value S_game_BlueprintRunning::to(EngineObjects& o, const kern::game::BlueprintRunning& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::Text>::to(o, x.tier), ev::Conv<ev::Int>::to(o, x.functions), ev::Conv<ev::Int>::to(o, x.nodes), ev::Conv<ev::Bool>::to(o, x.disabled)}); }
void S_game_BlueprintRunning::from(EngineObjects& o, const Value& v, kern::game::BlueprintRunning& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.tier); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.functions); if (const Value* f = v.field(3)) ev::Conv<ev::Int>::from(o, *f, x.nodes); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.disabled); }
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
Value S_game_Hud::to(EngineObjects& o, const kern::game::Hud& x) { return Value::structure({ev::Conv<ev::Enum<E_game_GameStatus>>::to(o, x.status), ev::Conv<ev::Text>::to(o, x.title), ev::Conv<ev::Text>::to(o, x.message), ev::Conv<ev::Float>::to(o, x.message_age), ev::Conv<ev::Float>::to(o, x.score), ev::Conv<ev::Int>::to(o, x.pickups), ev::Conv<ev::Int>::to(o, x.pickups_total), ev::Conv<ev::Float>::to(o, x.elapsed), ev::Conv<ev::Float>::to(o, x.remaining), ev::Conv<ev::Float>::to(o, x.speed), ev::Conv<ev::Float>::to(o, x.boost), ev::Conv<ev::Bool>::to(o, x.boosting), ev::Conv<ev::Bool>::to(o, x.interior), ev::Conv<ev::Float>::to(o, x.best), ev::Conv<ev::Bool>::to(o, x.soldier), ev::Conv<ev::Float>::to(o, x.health), ev::Conv<ev::Float>::to(o, x.max_health), ev::Conv<ev::Float>::to(o, x.armor), ev::Conv<ev::Int>::to(o, x.ammo), ev::Conv<ev::Int>::to(o, x.magazine), ev::Conv<ev::Int>::to(o, x.reserve), ev::Conv<ev::Float>::to(o, x.reload), ev::Conv<ev::Float>::to(o, x.spread), ev::Conv<ev::Float>::to(o, x.hit_age), ev::Conv<ev::Bool>::to(o, x.hit_head), ev::Conv<ev::Bool>::to(o, x.hit_kill), ev::Conv<ev::Float>::to(o, x.damage_age), ev::Conv<ev::Float>::to(o, x.damage_yaw), ev::Conv<ev::Float>::to(o, x.respawn_in), ev::Conv<ev::Bool>::to(o, x.aiming), ev::Conv<ev::Float>::to(o, x.fov)}); }
void S_game_Hud::from(EngineObjects& o, const Value& v, kern::game::Hud& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_game_GameStatus>>::from(o, *f, x.status); if (const Value* f = v.field(1)) ev::Conv<ev::Text>::from(o, *f, x.title); if (const Value* f = v.field(2)) ev::Conv<ev::Text>::from(o, *f, x.message); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.message_age); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.score); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.pickups); if (const Value* f = v.field(6)) ev::Conv<ev::Int>::from(o, *f, x.pickups_total); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.elapsed); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.remaining); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.speed); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.boost); if (const Value* f = v.field(11)) ev::Conv<ev::Bool>::from(o, *f, x.boosting); if (const Value* f = v.field(12)) ev::Conv<ev::Bool>::from(o, *f, x.interior); if (const Value* f = v.field(13)) ev::Conv<ev::Float>::from(o, *f, x.best); if (const Value* f = v.field(14)) ev::Conv<ev::Bool>::from(o, *f, x.soldier); if (const Value* f = v.field(15)) ev::Conv<ev::Float>::from(o, *f, x.health); if (const Value* f = v.field(16)) ev::Conv<ev::Float>::from(o, *f, x.max_health); if (const Value* f = v.field(17)) ev::Conv<ev::Float>::from(o, *f, x.armor); if (const Value* f = v.field(18)) ev::Conv<ev::Int>::from(o, *f, x.ammo); if (const Value* f = v.field(19)) ev::Conv<ev::Int>::from(o, *f, x.magazine); if (const Value* f = v.field(20)) ev::Conv<ev::Int>::from(o, *f, x.reserve); if (const Value* f = v.field(21)) ev::Conv<ev::Float>::from(o, *f, x.reload); if (const Value* f = v.field(22)) ev::Conv<ev::Float>::from(o, *f, x.spread); if (const Value* f = v.field(23)) ev::Conv<ev::Float>::from(o, *f, x.hit_age); if (const Value* f = v.field(24)) ev::Conv<ev::Bool>::from(o, *f, x.hit_head); if (const Value* f = v.field(25)) ev::Conv<ev::Bool>::from(o, *f, x.hit_kill); if (const Value* f = v.field(26)) ev::Conv<ev::Float>::from(o, *f, x.damage_age); if (const Value* f = v.field(27)) ev::Conv<ev::Float>::from(o, *f, x.damage_yaw); if (const Value* f = v.field(28)) ev::Conv<ev::Float>::from(o, *f, x.respawn_in); if (const Value* f = v.field(29)) ev::Conv<ev::Bool>::from(o, *f, x.aiming); if (const Value* f = v.field(30)) ev::Conv<ev::Float>::from(o, *f, x.fov); }
Value S_game_PlayInput::to(EngineObjects& o, const kern::game::PlayInput& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.move_x), ev::Conv<ev::Float>::to(o, x.move_y), ev::Conv<ev::Float>::to(o, x.up), ev::Conv<ev::Bool>::to(o, x.jump), ev::Conv<ev::Bool>::to(o, x.boost), ev::Conv<ev::Bool>::to(o, x.camera), ev::Conv<ev::Bool>::to(o, x.handbrake), ev::Conv<ev::Int>::to(o, x.robot), ev::Conv<ev::Float>::to(o, x.look_x), ev::Conv<ev::Float>::to(o, x.look_y), ev::Conv<ev::Float>::to(o, x.turn_deg), ev::Conv<ev::Float>::to(o, x.speed_scale), ev::Conv<ev::Float>::to(o, x.zoom), ev::Conv<ev::Float>::to(o, x.view_yaw), ev::Conv<ev::Bool>::to(o, x.has_view_yaw), ev::Conv<ev::Float>::to(o, x.view_pitch), ev::Conv<ev::Bool>::to(o, x.fire), ev::Conv<ev::Bool>::to(o, x.aim), ev::Conv<ev::Bool>::to(o, x.reload), ev::Conv<ev::Bool>::to(o, x.crouch), ev::Conv<ev::Bool>::to(o, x.interact), ev::Conv<ev::Float>::to(o, x.look_stick_x), ev::Conv<ev::Float>::to(o, x.look_stick_y), ev::Conv<ev::Bool>::to(o, x.gamepad), ev::Conv<ev::Float>::to(o, x.rewind)}); }
void S_game_PlayInput::from(EngineObjects& o, const Value& v, kern::game::PlayInput& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.move_x); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.move_y); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.up); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.jump); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.boost); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.camera); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.handbrake); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.robot); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.look_x); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.look_y); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.turn_deg); if (const Value* f = v.field(11)) ev::Conv<ev::Float>::from(o, *f, x.speed_scale); if (const Value* f = v.field(12)) ev::Conv<ev::Float>::from(o, *f, x.zoom); if (const Value* f = v.field(13)) ev::Conv<ev::Float>::from(o, *f, x.view_yaw); if (const Value* f = v.field(14)) ev::Conv<ev::Bool>::from(o, *f, x.has_view_yaw); if (const Value* f = v.field(15)) ev::Conv<ev::Float>::from(o, *f, x.view_pitch); if (const Value* f = v.field(16)) ev::Conv<ev::Bool>::from(o, *f, x.fire); if (const Value* f = v.field(17)) ev::Conv<ev::Bool>::from(o, *f, x.aim); if (const Value* f = v.field(18)) ev::Conv<ev::Bool>::from(o, *f, x.reload); if (const Value* f = v.field(19)) ev::Conv<ev::Bool>::from(o, *f, x.crouch); if (const Value* f = v.field(20)) ev::Conv<ev::Bool>::from(o, *f, x.interact); if (const Value* f = v.field(21)) ev::Conv<ev::Float>::from(o, *f, x.look_stick_x); if (const Value* f = v.field(22)) ev::Conv<ev::Float>::from(o, *f, x.look_stick_y); if (const Value* f = v.field(23)) ev::Conv<ev::Bool>::from(o, *f, x.gamepad); if (const Value* f = v.field(24)) ev::Conv<ev::Float>::from(o, *f, x.rewind); }
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
Value S_physics_Body::to(EngineObjects& o, const kern::physics::Body& x) { return Value::structure({ev::Conv<ev::Int64>::to(o, x.id)}); }
void S_physics_Body::from(EngineObjects& o, const Value& v, kern::physics::Body& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int64>::from(o, *f, x.id); }
Value S_physics_QueryFilter::to(EngineObjects& o, const kern::physics::QueryFilter& x) { return Value::structure({ev::Conv<ev::Int64>::to(o, x.mask_bits), ev::Conv<ev::Int64>::to(o, x.category_bits)}); }
void S_physics_QueryFilter::from(EngineObjects& o, const Value& v, kern::physics::QueryFilter& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int64>::from(o, *f, x.mask_bits); if (const Value* f = v.field(1)) ev::Conv<ev::Int64>::from(o, *f, x.category_bits); }

// engine.game.euler_to_quat: kern::game::euler_to_quat
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::game::euler_to_quat(std::move(a0));
  ev::give<ev::Struct<S_Quat>>(o, out, 0, r);
  return true;
}

// engine.game.quat_to_euler: kern::game::quat_to_euler
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Quat>>();
  ev::take<ev::Struct<S_Quat>>(o, in, 0, a0);
  decltype(auto) r = kern::game::quat_to_euler(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.game.transform_matrix: kern::game::transform_matrix
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Transform const&>>();
  ev::take<ev::Struct<S_game_Transform>>(o, in, 0, a0);
  decltype(auto) r = kern::game::transform_matrix(a0);
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

// engine.game.matrix_transform: kern::game::matrix_transform
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Mat4 const&>>();
  ev::take<ev::Struct<S_Mat4>>(o, in, 0, a0);
  decltype(auto) r = kern::game::matrix_transform(a0);
  ev::give<ev::Struct<S_game_Transform>>(o, out, 0, r);
  return true;
}

// engine.game.shape_name: kern::game::shape_name
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::Shape>>();
  ev::take<ev::Enum<E_game_Shape>>(o, in, 0, a0);
  decltype(auto) r = kern::game::shape_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.parse_shape: kern::game::parse_shape
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::Shape&>>();
  ev::take<ev::Enum<E_game_Shape>>(o, in, 1, a1);
  decltype(auto) r = kern::game::parse_shape(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Enum<E_game_Shape>>(o, out, 1, a1);
  return true;
}

// engine.game.motion_name_ok: kern::game::motion_name_ok
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  decltype(auto) r = kern::game::motion_name_ok(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game.hex_color: kern::game::hex_color
bool c7(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::game::hex_color(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game.parse_hex_color: kern::game::parse_hex_color
bool c8(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c9(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::game::srgb_to_linear(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.game.logic_node_shapes_registered: kern::game::logic_node_shapes_registered
bool c10(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::game::logic_node_shapes_registered();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game.logic_node_types: kern::game::logic_node_types
bool c11(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::game::logic_node_types();
  ev::give<ev::List<ev::Text>>(o, out, 0, r);
  return true;
}

// engine.game.logic_nodes_registered: kern::game::logic_nodes_registered
bool c12(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::game::logic_nodes_registered();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.night_windows.name: kern::game::NightWindows::name
bool c13(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->name();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.night_windows.attributes: kern::game::NightWindows::attributes
bool c14(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->attributes();
  ev::give<ev::List<ev::Struct<S_ExtensionAttribute>>>(o, out, 0, r);
  return true;
}

// engine.night_windows.vertex_declarations: kern::game::NightWindows::vertex_declarations
bool c15(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->vertex_declarations();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.night_windows.vertex: kern::game::NightWindows::vertex
bool c16(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->vertex();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.night_windows.fragment_declarations: kern::game::NightWindows::fragment_declarations
bool c17(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->fragment_declarations();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.night_windows.fragment_surface: kern::game::NightWindows::fragment_surface
bool c18(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  decltype(auto) r = self->fragment_surface();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.night_windows.bind: kern::game::NightWindows::bind
bool c19(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  kern::Program* a0 = ev::object<kern::Program>(o, in, 1, "p");
  if (!a0) return false;
  self->bind(*a0);
  return true;
}

// engine.night_windows.frame_uniforms: kern::game::NightWindows::frame_uniforms
bool c20(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  kern::UniformCache* a0 = ev::object<kern::UniformCache>(o, in, 1, "cache");
  if (!a0) return false;
  kern::Renderer* a1 = ev::object<kern::Renderer>(o, in, 2, "value2");
  if (!a1) return false;
  self->frame_uniforms(*a0, *a1);
  return true;
}

// engine.game.game_status_name: kern::game::game_status_name
bool c21(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::GameStatus>>();
  ev::take<ev::Enum<E_game_GameStatus>>(o, in, 0, a0);
  decltype(auto) r = kern::game::game_status_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.fixed_step.fixed_step: kern::game::FixedStep::fixed_step
bool c22(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::FixedStep>(o, in, 0, "fixed_step");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->fixed_step(std::move(a0));
  return true;
}

// engine.game_world.set_layers: kern::game::GameWorld::set_layers
bool c23(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->set_layers(std::move(a0));
  return true;
}

// engine.game_world.clear: kern::game::GameWorld::clear
bool c24(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  self->clear();
  return true;
}

// engine.game_world.actor_for_mesh: kern::game::GameWorld::actor_for_mesh
bool c25(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->actor_for_mesh(std::move(a0));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.actor_matrix: kern::game::GameWorld::actor_matrix
bool c26(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c27(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c28(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c29(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->pending_textures();
  ev::give<ev::List<ev::Text>>(o, out, 0, r);
  return true;
}

// engine.game_world.set_texture_pixels: kern::game::GameWorld::set_texture_pixels
bool c30(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c31(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  decltype(auto) r = self->apply_lighting(std::move(a0));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.time_of_day: kern::game::GameWorld::time_of_day
bool c32(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->time_of_day();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.present: kern::game::GameWorld::present
bool c33(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  self->present(std::move(a0), std::move(a1));
  return true;
}

// engine.game_world.start_level: kern::game::GameWorld::start_level
bool c34(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::game::SimMode>>();
  ev::take<ev::Enum<E_game_SimMode>>(o, in, 2, a1);
  decltype(auto) r = self->start_level(a0, std::move(a1));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_fixed_step: kern::game::GameWorld::set_fixed_step
bool c35(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  kern::game::FixedStep* a0 = ev::maybe<kern::game::FixedStep>(o, in, 1, "step");
  if (!o.error.empty()) return false;
  self->set_fixed_step(a0);
  return true;
}

// engine.game_world.fixed_step_handler: kern::game::GameWorld::fixed_step_handler
bool c36(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->fixed_step_handler();
  out[0] = Value::integer(ev::handle_of<kern::game::FixedStep>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.game_world.stop: kern::game::GameWorld::stop
bool c37(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  self->stop();
  return true;
}

// engine.game_world.running: kern::game::GameWorld::running
bool c38(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->running();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.session: kern::game::GameWorld::session
bool c39(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->session();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.mode: kern::game::GameWorld::mode
bool c40(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->mode();
  ev::give<ev::Enum<E_game_SimMode>>(o, out, 0, r);
  return true;
}

// engine.game_world.set_paused: kern::game::GameWorld::set_paused
bool c41(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_paused(std::move(a0));
  return true;
}

// engine.game_world.paused: kern::game::GameWorld::paused
bool c42(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->paused();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.set_mode: kern::game::GameWorld::set_mode
bool c43(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::SimMode>>();
  ev::take<ev::Enum<E_game_SimMode>>(o, in, 1, a0);
  decltype(auto) r = self->set_mode(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.step: kern::game::GameWorld::step
bool c44(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c45(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::PlayInput const&>>();
  ev::take<ev::Struct<S_game_PlayInput>>(o, in, 1, a0);
  self->step_once(a0);
  return true;
}

// engine.game_world.restart: kern::game::GameWorld::restart
bool c46(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->restart();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.update_camera: kern::game::GameWorld::update_camera
bool c47(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c48(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->player();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.camera_yaw: kern::game::GameWorld::camera_yaw
bool c49(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->camera_yaw();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.interior_view: kern::game::GameWorld::interior_view
bool c50(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->interior_view();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.camera_pitch: kern::game::GameWorld::camera_pitch
bool c51(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->camera_pitch();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.sim_time: kern::game::GameWorld::sim_time
bool c52(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->sim_time();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.add_pawn: kern::game::GameWorld::add_pawn
bool c53(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c54(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->remove_pawn(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.pawn_source: kern::game::GameWorld::pawn_source
bool c55(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->pawn_source(std::move(a0));
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.set_pawn_input: kern::game::GameWorld::set_pawn_input
bool c56(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c57(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->set_local_pawn(std::move(a0));
  return true;
}

// engine.game_world.pawns: kern::game::GameWorld::pawns
bool c58(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->pawns();
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.game_world.take_alive_changes: kern::game::GameWorld::take_alive_changes
bool c59(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::vector<std::pair<unsigned int, bool>, std::allocator<std::pair<unsigned int, bool> > >&>>();
  ev::take<ev::List<ev::Pair<ev::Int, ev::Bool>>>(o, in, 1, a0);
  self->take_alive_changes(a0);
  ev::give<ev::List<ev::Pair<ev::Int, ev::Bool>>>(o, out, 0, a0);
  return true;
}

// engine.game_world.dead_actors: kern::game::GameWorld::dead_actors
bool c60(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->dead_actors();
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.game_world.set_alive: kern::game::GameWorld::set_alive
bool c61(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c62(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::vector<unsigned int, std::allocator<unsigned int> >&>>();
  ev::take<ev::List<ev::Int>>(o, in, 1, a0);
  self->networked_actors(a0);
  ev::give<ev::List<ev::Int>>(o, out, 0, a0);
  return true;
}

// engine.game_world.hud_mut: kern::game::GameWorld::hud_mut
bool c63(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->hud_mut();
  ev::give<ev::Struct<S_game_Hud>>(o, out, 0, r);
  return true;
}

// engine.game_world.hud_mut@set: what stands at the place kern::game::GameWorld::hud_mut gives
bool c63_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::take<ev::Struct<S_game_Hud>>(o, in, 1, self->hud_mut());
  return true;
}

// engine.game_world.set_replica_pose: kern::game::GameWorld::set_replica_pose
bool c64(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c65(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c66(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->hud();
  ev::give<ev::Struct<S_game_Hud>>(o, out, 0, r);
  return true;
}

// engine.game_world.aim_blend: kern::game::GameWorld::aim_blend
bool c67(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->aim_blend();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.stats: kern::game::GameWorld::stats
bool c68(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->stats();
  ev::give<ev::Struct<S_game_SimStats>>(o, out, 0, r);
  return true;
}

// engine.game_world.physics: kern::game::GameWorld::physics
bool c69(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->physics();
  out[0] = Value::integer(ev::handle_of<kern::physics::World>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.game_world.actor_body: kern::game::GameWorld::actor_body
bool c70(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->actor_body(std::move(a0));
  ev::give<ev::Struct<S_physics_Body>>(o, out, 0, r);
  return true;
}

// engine.game_world.begin_grab: kern::game::GameWorld::begin_grab
bool c71(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c72(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->move_grab(std::move(a0));
  return true;
}

// engine.game_world.end_grab: kern::game::GameWorld::end_grab
bool c73(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  self->end_grab();
  return true;
}

// engine.game_world.grabbing: kern::game::GameWorld::grabbing
bool c74(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->grabbing();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.debug_lines: kern::game::GameWorld::debug_lines
bool c75(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c76(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->events();
  ev::give<ev::List<ev::Text>>(o, out, 0, r);
  return true;
}

// engine.game_world.events@set: what stands at the place kern::game::GameWorld::events gives
bool c76_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::take<ev::List<ev::Text>>(o, in, 1, self->events());
  return true;
}

// engine.game_world.sound_cues: kern::game::GameWorld::sound_cues
bool c77(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->sound_cues();
  ev::give<ev::List<ev::Struct<S_game_SoundCue>>>(o, out, 0, r);
  return true;
}

// engine.game_world.sound_cues@set: what stands at the place kern::game::GameWorld::sound_cues gives
bool c77_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::take<ev::List<ev::Struct<S_game_SoundCue>>>(o, in, 1, self->sound_cues());
  return true;
}

// engine.game_world.shots: kern::game::GameWorld::shots
bool c78(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->shots();
  ev::give<ev::List<ev::Struct<S_game_ShotEvent>>>(o, out, 0, r);
  return true;
}

// engine.game_world.shots@set: what stands at the place kern::game::GameWorld::shots gives
bool c78_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::take<ev::List<ev::Struct<S_game_ShotEvent>>>(o, in, 1, self->shots());
  return true;
}

// engine.game_world.damage: kern::game::GameWorld::damage
bool c79(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c80(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->health_of(std::move(a0));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.game_world.set_health: kern::game::GameWorld::set_health
bool c81(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c82(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c83(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->respawn_now(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.apply_soldier_state: kern::game::GameWorld::apply_soldier_state
bool c84(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::game::SoldierState const&>>();
  ev::take<ev::Struct<S_game_SoldierState>>(o, in, 1, a0);
  self->apply_soldier_state(a0);
  return true;
}

// engine.game_world.set_view: kern::game::GameWorld::set_view
bool c85(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Camera const&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 1, a0);
  self->set_view(a0);
  return true;
}

// engine.game_world.set_view_hand: kern::game::GameWorld::set_view_hand
bool c86(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  std::vector<kern::Mat4> a0;
  ev::take<ev::List<ev::Struct<S_Mat4>>>(o, in, 1, a0);
  self->set_view_hand(a0.empty() ? nullptr : a0.data());
  return true;
}

// engine.game_world.actor_alive: kern::game::GameWorld::actor_alive
bool c87(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->actor_alive(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.last_input: kern::game::GameWorld::last_input
bool c88(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->last_input();
  ev::give<ev::Struct<S_game_PlayInput>>(o, out, 0, r);
  return true;
}

// engine.game_world.logic_runtime: kern::game::GameWorld::logic_runtime
bool c89(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->logic_runtime();
  out[0] = Value::integer(ev::handle_of<kern::logic::Runtime>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.game_world.key_event: kern::game::GameWorld::key_event
bool c90(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c91(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c92(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c93(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c94(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c95(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c96(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c97(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c98(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c99(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->actor_velocity(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.game_world.set_actor_velocity: kern::game::GameWorld::set_actor_velocity
bool c100(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c101(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c102(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c103(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c104(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->find_actor(a0);
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_world.actors_with_tag: kern::game::GameWorld::actors_with_tag
bool c105(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c106(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c107(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c108(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->add_score(std::move(a0));
  return true;
}

// engine.game_world.end_game: kern::game::GameWorld::end_game
bool c109(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c110(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 1, a0);
  self->show_message(a0);
  return true;
}

// engine.game_world.play_sound: kern::game::GameWorld::play_sound
bool c111(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c112(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c113(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->shake_camera(std::move(a0));
  return true;
}

// engine.game_world.aim_view_model: kern::game::GameWorld::aim_view_model
bool c114(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c115(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->destroy_actor(std::move(a0));
  return true;
}

// engine.game_world.heal: kern::game::GameWorld::heal
bool c116(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c117(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c118(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c119(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c120(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c121(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c122(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c123(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c124(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_quack(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_skill: kern::game::GameWorld::walker_skill
bool c125(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c126(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c127(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_holding(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_pet: kern::game::GameWorld::walker_pet
bool c128(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c129(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c130(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c131(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_driving(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game_world.set_walker_external: kern::game::GameWorld::set_walker_external
bool c132(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c133(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c134(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c135(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c136(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_tick_due(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_probe: kern::game::GameWorld::walker_probe
bool c137(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c138(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c139(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_said(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_joints: kern::game::GameWorld::walker_joints
bool c140(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_joints(std::move(a0));
  ev::give<ev::List<ev::Struct<S_game_GameWorld_WalkerJoint>>>(o, out, 0, r);
  return true;
}

// engine.game_world.set_walker_play_external: kern::game::GameWorld::set_walker_play_external
bool c141(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c142(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c143(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c144(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_play_due(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_playing: kern::game::GameWorld::walker_playing
bool c145(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_playing(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_world.walker_sees: kern::game::GameWorld::walker_sees
bool c146(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c147(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->walker_state(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.game_world.walkers: kern::game::GameWorld::walkers
bool c148(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->walkers();
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.game_world.art_stats: kern::game::GameWorld::art_stats
bool c149(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  decltype(auto) r = self->art_stats();
  ev::give<ev::Struct<S_game_GameWorld_ArtStats>>(o, out, 0, r);
  return true;
}

// engine.game.blueprints_running: kern::game::blueprints_running
bool c150(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::logic::Runtime* a0 = ev::object<kern::logic::Runtime>(o, in, 0, "logic");
  if (!a0) return false;
  decltype(auto) r = kern::game::blueprints_running(*a0);
  ev::give<ev::List<ev::Struct<S_game_BlueprintRunning>>>(o, out, 0, r);
  return true;
}

// engine.sound_bank.mixer: kern::game::SoundBank::mixer
bool c151(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  decltype(auto) r = self->mixer();
  out[0] = Value::integer(ev::handle_of<kern::audio::Mixer>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.sound_bank.clip: kern::game::SoundBank::clip
bool c152(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->clip(a0);
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.sound_bank.request: kern::game::SoundBank::request
bool c153(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  self->request(a0);
  return true;
}

// engine.sound_bank.prepare: kern::game::SoundBank::prepare
bool c154(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 1, a0);
  decltype(auto) r = self->prepare(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.sound_bank.ready: kern::game::SoundBank::ready
bool c155(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->ready(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.sound_bank.failed: kern::game::SoundBank::failed
bool c156(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->failed(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.sound_bank.pending_imports: kern::game::SoundBank::pending_imports
bool c157(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  decltype(auto) r = self->pending_imports();
  ev::give<ev::List<ev::Text>>(o, out, 0, r);
  return true;
}

// engine.sound_bank.source: kern::game::SoundBank::source
bool c158(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->source(a0);
  ev::give<ev::List<ev::Text>>(o, out, 0, r);
  return true;
}

// engine.sound_bank.set_pcm: kern::game::SoundBank::set_pcm
bool c159(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c160(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c161(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->duration(a0);
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.sound_bank.clear: kern::game::SoundBank::clear
bool c162(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  self->clear();
  return true;
}

// engine.sound_bank.queued: kern::game::SoundBank::queued
bool c163(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::SoundBank>(o, in, 0, "sound_bank");
  if (!self) return false;
  decltype(auto) r = self->queued();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_audio.begin: kern::game::GameAudio::begin
bool c164(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  kern::game::GameWorld* a0 = ev::object<kern::game::GameWorld>(o, in, 1, "world");
  if (!a0) return false;
  self->begin(*a0);
  return true;
}

// engine.game_audio.update: kern::game::GameAudio::update
bool c165(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c166(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->end(std::move(a0));
  return true;
}

// engine.game_audio.active: kern::game::GameAudio::active
bool c167(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  decltype(auto) r = self->active();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_audio.set_muted: kern::game::GameAudio::set_muted
bool c168(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_muted(std::move(a0));
  return true;
}

// engine.game_audio.muted: kern::game::GameAudio::muted
bool c169(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  decltype(auto) r = self->muted();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.game_audio.cues_played: kern::game::GameAudio::cues_played
bool c170(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  decltype(auto) r = self->cues_played();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_audio.cues_dropped: kern::game::GameAudio::cues_dropped
bool c171(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameAudio>(o, in, 0, "game_audio");
  if (!self) return false;
  decltype(auto) r = self->cues_dropped();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.game_audio.make: kern::game::GameAudio::GameAudio
bool c172(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::audio::Mixer* a0 = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!a0) return false;
  kern::game::SoundBank* a1 = ev::object<kern::game::SoundBank>(o, in, 1, "bank");
  if (!a1) return false;
  out[0] = Value::integer(o.own<kern::game::GameAudio>(std::unique_ptr<kern::game::GameAudio>(new kern::game::GameAudio(*a0, *a1))));
  return true;
}

// engine.game_audio.release: kern::game::GameAudio::~GameAudio
bool c173(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::GameAudio>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no GameAudio that was made";
    return false;
  }
  return true;
}

// engine.game_world.make: kern::game::GameWorld::GameWorld
bool c174(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::Renderer* a0 = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!a0) return false;
  kern::game::NightWindows* a1 = ev::maybe<kern::game::NightWindows>(o, in, 1, "windows");
  if (!o.error.empty()) return false;
  out[0] = Value::integer(o.own<kern::game::GameWorld>(std::unique_ptr<kern::game::GameWorld>(new kern::game::GameWorld(*a0, a1))));
  return true;
}

// engine.game_world.release: kern::game::GameWorld::~GameWorld
bool c175(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::GameWorld>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no GameWorld that was made";
    return false;
  }
  return true;
}

// engine.game_world.logic_native: kern::game::GameWorld::logic_native
bool c176(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::give<ev::Bool>(o, out, 0, self->logic_native);
  return true;
}

// engine.game_world.logic_native@set: what stands at the place kern::game::GameWorld::logic_native gives
bool c176_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld>(o, in, 0, "game_world");
  if (!self) return false;
  ev::take<ev::Bool>(o, in, 1, self->logic_native);
  return true;
}

// engine.property_change.make: kern::game::GameWorld::PropertyChange::PropertyChange
bool c177(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::game::GameWorld::PropertyChange>(std::unique_ptr<kern::game::GameWorld::PropertyChange>(new kern::game::GameWorld::PropertyChange())));
  return true;
}

// engine.property_change.release: kern::game::GameWorld::PropertyChange::~PropertyChange
bool c178(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::GameWorld::PropertyChange>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no PropertyChange that was made";
    return false;
  }
  return true;
}

// engine.property_change.actor: kern::game::GameWorld::PropertyChange::actor
bool c179(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld::PropertyChange>(o, in, 0, "property_change");
  if (!self) return false;
  ev::give<ev::Int>(o, out, 0, self->actor);
  return true;
}

// engine.property_change.actor@set: what stands at the place kern::game::GameWorld::PropertyChange::actor gives
bool c179_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld::PropertyChange>(o, in, 0, "property_change");
  if (!self) return false;
  ev::take<ev::Int>(o, in, 1, self->actor);
  return true;
}

// engine.property_change.path: kern::game::GameWorld::PropertyChange::path
bool c180(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::GameWorld::PropertyChange>(o, in, 0, "property_change");
  if (!self) return false;
  ev::give<ev::Text>(o, out, 0, self->path);
  return true;
}

// engine.property_change.path@set: what stands at the place kern::game::GameWorld::PropertyChange::path gives
bool c180_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::GameWorld::PropertyChange>(o, in, 0, "property_change");
  if (!self) return false;
  ev::take<ev::Text>(o, in, 1, self->path);
  return true;
}

// engine.night_windows.make: kern::game::NightWindows::NightWindows
bool c181(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::game::NightWindows, kern::ShaderExtension>(std::unique_ptr<kern::game::NightWindows>(new kern::game::NightWindows())));
  return true;
}

// engine.night_windows.release: kern::game::NightWindows::~NightWindows
bool c182(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::NightWindows>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no NightWindows that was made";
    return false;
  }
  return true;
}

// engine.night_windows.night: kern::game::NightWindows::night
bool c183(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  ev::give<ev::Float>(o, out, 0, self->night);
  return true;
}

// engine.night_windows.night@set: what stands at the place kern::game::NightWindows::night gives
bool c183_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::game::NightWindows>(o, in, 0, "night_windows");
  if (!self) return false;
  ev::take<ev::Float>(o, in, 1, self->night);
  return true;
}

// engine.sound_bank.make: kern::game::SoundBank::SoundBank
bool c184(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::audio::Mixer* a0 = ev::object<kern::audio::Mixer>(o, in, 0, "mixer");
  if (!a0) return false;
  out[0] = Value::integer(o.own<kern::game::SoundBank>(std::unique_ptr<kern::game::SoundBank>(new kern::game::SoundBank(*a0))));
  return true;
}

// engine.sound_bank.release: kern::game::SoundBank::~SoundBank
bool c185(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::game::SoundBank>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no SoundBank that was made";
    return false;
  }
  return true;
}

// kern::game::FixedStep: the object of the engine a Blueprint is (Graph::implements "game_FixedStep")
class Implements_game_FixedStep final : public kern::game::FixedStep {
 public:
  Implements_game_FixedStep(EngineObjects& o, std::shared_ptr<Implementer> by) : o_(o), by_(std::move(by)) {}
  decltype(std::declval<kern::game::FixedStep&>().fixed_step(std::declval<double>())) fixed_step(double kern_p0) override {
    const std::vector<Value> kern_in{ev::Conv<ev::Float>::to(o_, kern_p0)};
    std::vector<Value> kern_out;
    by_->call("fixed_step", kern_in, kern_out);
  }

 private:
  EngineObjects& o_;
  std::shared_ptr<Implementer> by_;
};
i64 implements_game_FixedStep(EngineObjects& o, std::shared_ptr<Implementer> by) {
  return o.own<Implements_game_FixedStep, kern::game::FixedStep>(std::make_unique<Implements_game_FixedStep>(o, std::move(by)));
}

const EngineCall kCalls[] = {
    {"engine.game.euler_to_quat", &c0},
    {"engine.game.quat_to_euler", &c1},
    {"engine.game.transform_matrix", &c2},
    {"engine.game.matrix_transform", &c3},
    {"engine.game.shape_name", &c4},
    {"engine.game.parse_shape", &c5},
    {"engine.game.motion_name_ok", &c6},
    {"engine.game.hex_color", &c7},
    {"engine.game.parse_hex_color", &c8},
    {"engine.game.srgb_to_linear", &c9},
    {"engine.game.logic_node_shapes_registered", &c10},
    {"engine.game.logic_node_types", &c11},
    {"engine.game.logic_nodes_registered", &c12},
    {"engine.night_windows.name", &c13},
    {"engine.night_windows.attributes", &c14},
    {"engine.night_windows.vertex_declarations", &c15},
    {"engine.night_windows.vertex", &c16},
    {"engine.night_windows.fragment_declarations", &c17},
    {"engine.night_windows.fragment_surface", &c18},
    {"engine.night_windows.bind", &c19},
    {"engine.night_windows.frame_uniforms", &c20},
    {"engine.game.game_status_name", &c21},
    {"engine.fixed_step.fixed_step", &c22},
    {"engine.game_world.set_layers", &c23},
    {"engine.game_world.clear", &c24},
    {"engine.game_world.actor_for_mesh", &c25},
    {"engine.game_world.actor_matrix", &c26},
    {"engine.game_world.actor_bounds", &c27},
    {"engine.game_world.actor_meshes", &c28},
    {"engine.game_world.pending_textures", &c29},
    {"engine.game_world.set_texture_pixels", &c30},
    {"engine.game_world.apply_lighting", &c31},
    {"engine.game_world.time_of_day", &c32},
    {"engine.game_world.present", &c33},
    {"engine.game_world.start_level", &c34},
    {"engine.game_world.set_fixed_step", &c35},
    {"engine.game_world.fixed_step_handler", &c36},
    {"engine.game_world.stop", &c37},
    {"engine.game_world.running", &c38},
    {"engine.game_world.session", &c39},
    {"engine.game_world.mode", &c40},
    {"engine.game_world.set_paused", &c41},
    {"engine.game_world.paused", &c42},
    {"engine.game_world.set_mode", &c43},
    {"engine.game_world.step", &c44},
    {"engine.game_world.step_once", &c45},
    {"engine.game_world.restart", &c46},
    {"engine.game_world.update_camera", &c47},
    {"engine.game_world.player", &c48},
    {"engine.game_world.camera_yaw", &c49},
    {"engine.game_world.interior_view", &c50},
    {"engine.game_world.camera_pitch", &c51},
    {"engine.game_world.sim_time", &c52},
    {"engine.game_world.add_pawn", &c53},
    {"engine.game_world.remove_pawn", &c54},
    {"engine.game_world.pawn_source", &c55},
    {"engine.game_world.set_pawn_input", &c56},
    {"engine.game_world.set_local_pawn", &c57},
    {"engine.game_world.pawns", &c58},
    {"engine.game_world.take_alive_changes", &c59},
    {"engine.game_world.dead_actors", &c60},
    {"engine.game_world.set_alive", &c61},
    {"engine.game_world.networked_actors", &c62},
    {"engine.game_world.hud_mut", &c63},
    {"engine.game_world.set_replica_pose", &c64},
    {"engine.game_world.step_replica", &c65},
    {"engine.game_world.hud", &c66},
    {"engine.game_world.aim_blend", &c67},
    {"engine.game_world.stats", &c68},
    {"engine.game_world.physics", &c69},
    {"engine.game_world.actor_body", &c70},
    {"engine.game_world.begin_grab", &c71},
    {"engine.game_world.move_grab", &c72},
    {"engine.game_world.end_grab", &c73},
    {"engine.game_world.grabbing", &c74},
    {"engine.game_world.debug_lines", &c75},
    {"engine.game_world.events", &c76},
    {"engine.game_world.sound_cues", &c77},
    {"engine.game_world.shots", &c78},
    {"engine.game_world.damage", &c79},
    {"engine.game_world.health_of", &c80},
    {"engine.game_world.set_health", &c81},
    {"engine.game_world.soldier_state", &c82},
    {"engine.game_world.respawn_now", &c83},
    {"engine.game_world.apply_soldier_state", &c84},
    {"engine.game_world.set_view", &c85},
    {"engine.game_world.set_view_hand", &c86},
    {"engine.game_world.actor_alive", &c87},
    {"engine.game_world.last_input", &c88},
    {"engine.game_world.logic_runtime", &c89},
    {"engine.game_world.key_event", &c90},
    {"engine.game_world.take_replicated_changes", &c91},
    {"engine.game_world.replicated_state", &c92},
    {"engine.game_world.set_property", &c93},
    {"engine.game_world.set_property_number", &c94},
    {"engine.game_world.set_property_flag", &c95},
    {"engine.game_world.set_property_text", &c96},
    {"engine.game_world.spawn_copy", &c97},
    {"engine.game_world.teleport", &c98},
    {"engine.game_world.actor_velocity", &c99},
    {"engine.game_world.set_actor_velocity", &c100},
    {"engine.game_world.push_actor", &c101},
    {"engine.game_world.set_simulate_physics", &c102},
    {"engine.game_world.set_joint_motor", &c103},
    {"engine.game_world.find_actor", &c104},
    {"engine.game_world.actors_with_tag", &c105},
    {"engine.game_world.has_tag", &c106},
    {"engine.game_world.line_trace", &c107},
    {"engine.game_world.add_score", &c108},
    {"engine.game_world.end_game", &c109},
    {"engine.game_world.show_message", &c110},
    {"engine.game_world.play_sound", &c111},
    {"engine.game_world.particle_burst", &c112},
    {"engine.game_world.shake_camera", &c113},
    {"engine.game_world.aim_view_model", &c114},
    {"engine.game_world.destroy_actor", &c115},
    {"engine.game_world.heal", &c116},
    {"engine.game_world.give_ammo", &c117},
    {"engine.game_world.call_logic_event", &c118},
    {"engine.game_world.walker_sensors", &c119},
    {"engine.game_world.walker_camera", &c120},
    {"engine.game_world.walker_command", &c121},
    {"engine.game_world.walker_look", &c122},
    {"engine.game_world.walker_set_autonomous", &c123},
    {"engine.game_world.walker_quack", &c124},
    {"engine.game_world.walker_skill", &c125},
    {"engine.game_world.walker_mouth", &c126},
    {"engine.game_world.walker_holding", &c127},
    {"engine.game_world.walker_pet", &c128},
    {"engine.game_world.walker_hear", &c129},
    {"engine.game_world.walker_servo_load", &c130},
    {"engine.game_world.walker_driving", &c131},
    {"engine.game_world.set_walker_external", &c132},
    {"engine.game_world.set_walker_pose_edit", &c133},
    {"engine.game_world.set_walker_pose", &c134},
    {"engine.game_world.set_walker_action", &c135},
    {"engine.game_world.walker_tick_due", &c136},
    {"engine.game_world.walker_probe", &c137},
    {"engine.game_world.walker_push", &c138},
    {"engine.game_world.walker_said", &c139},
    {"engine.game_world.walker_joints", &c140},
    {"engine.game_world.set_walker_play_external", &c141},
    {"engine.game_world.set_walker_play_action", &c142},
    {"engine.game_world.set_walker_detector", &c143},
    {"engine.game_world.walker_play_due", &c144},
    {"engine.game_world.walker_playing", &c145},
    {"engine.game_world.walker_sees", &c146},
    {"engine.game_world.walker_state", &c147},
    {"engine.game_world.walkers", &c148},
    {"engine.game_world.art_stats", &c149},
    {"engine.game.blueprints_running", &c150},
    {"engine.sound_bank.mixer", &c151},
    {"engine.sound_bank.clip", &c152},
    {"engine.sound_bank.request", &c153},
    {"engine.sound_bank.prepare", &c154},
    {"engine.sound_bank.ready", &c155},
    {"engine.sound_bank.failed", &c156},
    {"engine.sound_bank.pending_imports", &c157},
    {"engine.sound_bank.source", &c158},
    {"engine.sound_bank.set_pcm", &c159},
    {"engine.sound_bank.set_failed", &c160},
    {"engine.sound_bank.duration", &c161},
    {"engine.sound_bank.clear", &c162},
    {"engine.sound_bank.queued", &c163},
    {"engine.game_audio.begin", &c164},
    {"engine.game_audio.update", &c165},
    {"engine.game_audio.end", &c166},
    {"engine.game_audio.active", &c167},
    {"engine.game_audio.set_muted", &c168},
    {"engine.game_audio.muted", &c169},
    {"engine.game_audio.cues_played", &c170},
    {"engine.game_audio.cues_dropped", &c171},
    {"engine.game_audio.make", &c172},
    {"engine.game_audio.release", &c173},
    {"engine.game_world.make", &c174},
    {"engine.game_world.release", &c175},
    {"engine.game_world.logic_native", &c176},
    {"engine.property_change.make", &c177},
    {"engine.property_change.release", &c178},
    {"engine.property_change.actor", &c179},
    {"engine.property_change.path", &c180},
    {"engine.night_windows.make", &c181},
    {"engine.night_windows.release", &c182},
    {"engine.night_windows.night", &c183},
    {"engine.sound_bank.make", &c184},
    {"engine.sound_bank.release", &c185},
    {"engine.game_world.hud_mut@get", &c63},
    {"engine.game_world.hud_mut@set", &c63_set},
    {"engine.game_world.events@get", &c76},
    {"engine.game_world.events@set", &c76_set},
    {"engine.game_world.sound_cues@get", &c77},
    {"engine.game_world.sound_cues@set", &c77_set},
    {"engine.game_world.shots@get", &c78},
    {"engine.game_world.shots@set", &c78_set},
    {"engine.game_world.logic_native@get", &c176},
    {"engine.game_world.logic_native@set", &c176_set},
    {"engine.property_change.actor@get", &c179},
    {"engine.property_change.actor@set", &c179_set},
    {"engine.property_change.path@get", &c180},
    {"engine.property_change.path@set", &c180_set},
    {"engine.night_windows.night@get", &c183},
    {"engine.night_windows.night@set", &c183_set},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

const EngineMaker kMakers[] = {
    {EngineObjects::kind<kern::game::GameAudio>(), [](EngineObjects& o, i64 handle) -> void* { auto made = std::make_unique<ev::MadeWith<kern::game::GameAudio>>(); auto& d0 = made->keep(new kern::audio::Mixer()); auto& d1 = made->keep(new kern::audio::Mixer()); auto& d2 = made->keep(new kern::game::SoundBank(d1)); made->object.emplace(d0, d2); return o.put_made<kern::game::GameAudio>(handle, std::move(made)); }},
    {EngineObjects::kind<kern::game::GameWorld>(), [](EngineObjects& o, i64 handle) -> void* { auto made = std::make_unique<ev::MadeWith<kern::game::GameWorld>>(); auto& d0 = made->keep(new kern::Renderer()); made->object.emplace(d0); return o.put_made<kern::game::GameWorld>(handle, std::move(made)); }},
    {EngineObjects::kind<kern::game::GameWorld::PropertyChange>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::game::GameWorld::PropertyChange>(handle, std::unique_ptr<kern::game::GameWorld::PropertyChange>(new kern::game::GameWorld::PropertyChange())); }},
    {EngineObjects::kind<kern::game::NightWindows>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::game::NightWindows, kern::ShaderExtension>(handle, std::unique_ptr<kern::game::NightWindows>(new kern::game::NightWindows())); }},
    {EngineObjects::kind<kern::game::SoundBank>(), [](EngineObjects& o, i64 handle) -> void* { auto made = std::make_unique<ev::MadeWith<kern::game::SoundBank>>(); auto& d0 = made->keep(new kern::audio::Mixer()); made->object.emplace(d0); return o.put_made<kern::game::SoundBank>(handle, std::move(made)); }},
};
const EngineMakers kAllMakers(kMakers, std::size(kMakers));

const EngineImplement kAdapters[] = {
    {"game_FixedStep", &implements_game_FixedStep},
};
const EngineImplements kAllAdapters(kAdapters, std::size(kAdapters));

}  // namespace
}  // namespace kern::logic
