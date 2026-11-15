// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include "kern/physics/physics.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct E_physics_BodyType { using type = kern::physics::BodyType; static constexpr type values[] = {kern::physics::BodyType::fixed, kern::physics::BodyType::kinematic, kern::physics::BodyType::dynamic}; };
struct E_physics_JointType { using type = kern::physics::JointType; static constexpr type values[] = {kern::physics::JointType::revolute, kern::physics::JointType::distance, kern::physics::JointType::prismatic, kern::physics::JointType::wheel, kern::physics::JointType::weld, kern::physics::JointType::motor, kern::physics::JointType::spherical, kern::physics::JointType::parallel, kern::physics::JointType::filter}; };
struct E_physics_ShapeKind { using type = kern::physics::ShapeKind; static constexpr type values[] = {kern::physics::ShapeKind::box, kern::physics::ShapeKind::sphere, kern::physics::ShapeKind::capsule, kern::physics::ShapeKind::hull, kern::physics::ShapeKind::mesh}; };
struct E_physics_Surface { using type = kern::physics::Surface; static constexpr type values[] = {kern::physics::Surface::none, kern::physics::Surface::cloth, kern::physics::Surface::tpu, kern::physics::Surface::floor}; };
struct S_MeshData { static Value to(EngineObjects& o, const kern::MeshData& x); static void from(EngineObjects& o, const Value& v, kern::MeshData& x); };
struct S_Quat { static Value to(EngineObjects& o, const kern::Quat& x); static void from(EngineObjects& o, const Value& v, kern::Quat& x); };
struct S_physics_Body { static Value to(EngineObjects& o, const kern::physics::Body& x); static void from(EngineObjects& o, const Value& v, kern::physics::Body& x); };
struct S_physics_BodySettings { static Value to(EngineObjects& o, const kern::physics::BodySettings& x); static void from(EngineObjects& o, const Value& v, kern::physics::BodySettings& x); };
struct S_physics_Capsule { static Value to(EngineObjects& o, const kern::physics::Capsule& x); static void from(EngineObjects& o, const Value& v, kern::physics::Capsule& x); };
struct S_physics_ContactPoint { static Value to(EngineObjects& o, const kern::physics::ContactPoint& x); static void from(EngineObjects& o, const Value& v, kern::physics::ContactPoint& x); };
struct S_physics_HitEvent { static Value to(EngineObjects& o, const kern::physics::HitEvent& x); static void from(EngineObjects& o, const Value& v, kern::physics::HitEvent& x); };
struct S_physics_Joint { static Value to(EngineObjects& o, const kern::physics::Joint& x); static void from(EngineObjects& o, const Value& v, kern::physics::Joint& x); };
struct S_physics_JointSettings { static Value to(EngineObjects& o, const kern::physics::JointSettings& x); static void from(EngineObjects& o, const Value& v, kern::physics::JointSettings& x); };
struct S_physics_MotionLocks { static Value to(EngineObjects& o, const kern::physics::MotionLocks& x); static void from(EngineObjects& o, const Value& v, kern::physics::MotionLocks& x); };
struct S_physics_MoverPlane { static Value to(EngineObjects& o, const kern::physics::MoverPlane& x); static void from(EngineObjects& o, const Value& v, kern::physics::MoverPlane& x); };
struct S_physics_Pose { static Value to(EngineObjects& o, const kern::physics::Pose& x); static void from(EngineObjects& o, const Value& v, kern::physics::Pose& x); };
struct S_physics_QueryFilter { static Value to(EngineObjects& o, const kern::physics::QueryFilter& x); static void from(EngineObjects& o, const Value& v, kern::physics::QueryFilter& x); };
struct S_physics_RayHit { static Value to(EngineObjects& o, const kern::physics::RayHit& x); static void from(EngineObjects& o, const Value& v, kern::physics::RayHit& x); };
struct S_physics_Shape { static Value to(EngineObjects& o, const kern::physics::Shape& x); static void from(EngineObjects& o, const Value& v, kern::physics::Shape& x); };
struct S_physics_ShapeMaterial { static Value to(EngineObjects& o, const kern::physics::ShapeMaterial& x); static void from(EngineObjects& o, const Value& v, kern::physics::ShapeMaterial& x); };
struct S_physics_ShapeSettings { static Value to(EngineObjects& o, const kern::physics::ShapeSettings& x); static void from(EngineObjects& o, const Value& v, kern::physics::ShapeSettings& x); };
struct S_physics_TouchEvent { static Value to(EngineObjects& o, const kern::physics::TouchEvent& x); static void from(EngineObjects& o, const Value& v, kern::physics::TouchEvent& x); };
struct S_physics_Version { static Value to(EngineObjects& o, const kern::physics::Version& x); static void from(EngineObjects& o, const Value& v, kern::physics::Version& x); };
struct S_physics_WorldSettings { static Value to(EngineObjects& o, const kern::physics::WorldSettings& x); static void from(EngineObjects& o, const Value& v, kern::physics::WorldSettings& x); };
Value S_MeshData::to(EngineObjects& o, const kern::MeshData& x) { return Value::structure({ev::Conv<ev::List<ev::Float>>::to(o, x.positions), ev::Conv<ev::List<ev::Float>>::to(o, x.normals), ev::Conv<ev::List<ev::Float>>::to(o, x.colors), ev::Conv<ev::List<ev::Float>>::to(o, x.uvs), ev::Conv<ev::List<ev::Float>>::to(o, x.extra), ev::Conv<ev::Int>::to(o, x.extra_size), ev::Conv<ev::List<ev::Int>>::to(o, x.indices)}); }
void S_MeshData::from(EngineObjects& o, const Value& v, kern::MeshData& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.positions); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.normals); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.colors); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.uvs); if (const Value* f = v.field(4)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.extra); if (const Value* f = v.field(5)) ev::Conv<ev::Int>::from(o, *f, x.extra_size); if (const Value* f = v.field(6)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.indices); }
Value S_Quat::to(EngineObjects& o, const kern::Quat& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.x), ev::Conv<ev::Float>::to(o, x.y), ev::Conv<ev::Float>::to(o, x.z), ev::Conv<ev::Float>::to(o, x.w)}); }
void S_Quat::from(EngineObjects& o, const Value& v, kern::Quat& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.x); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.y); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.z); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.w); }
Value S_physics_Body::to(EngineObjects& o, const kern::physics::Body& x) { return Value::structure({ev::Conv<ev::Int64>::to(o, x.id)}); }
void S_physics_Body::from(EngineObjects& o, const Value& v, kern::physics::Body& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int64>::from(o, *f, x.id); }
Value S_physics_BodySettings::to(EngineObjects& o, const kern::physics::BodySettings& x) { return Value::structure({ev::Conv<ev::Enum<E_physics_BodyType>>::to(o, x.type), ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Struct<S_Quat>>::to(o, x.rotation), ev::Conv<ev::Vector>::to(o, x.linear_velocity), ev::Conv<ev::Vector>::to(o, x.angular_velocity), ev::Conv<ev::Float>::to(o, x.linear_damping), ev::Conv<ev::Float>::to(o, x.angular_damping), ev::Conv<ev::Float>::to(o, x.gravity_scale), ev::Conv<ev::Bool>::to(o, x.enable_sleep), ev::Conv<ev::Bool>::to(o, x.awake), ev::Conv<ev::Bool>::to(o, x.enabled), ev::Conv<ev::Bool>::to(o, x.bullet), ev::Conv<ev::Int64>::to(o, x.user)}); }
void S_physics_BodySettings::from(EngineObjects& o, const Value& v, kern::physics::BodySettings& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_physics_BodyType>>::from(o, *f, x.type); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(2)) ev::Conv<ev::Struct<S_Quat>>::from(o, *f, x.rotation); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.linear_velocity); if (const Value* f = v.field(4)) ev::Conv<ev::Vector>::from(o, *f, x.angular_velocity); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.linear_damping); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.angular_damping); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.gravity_scale); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.enable_sleep); if (const Value* f = v.field(9)) ev::Conv<ev::Bool>::from(o, *f, x.awake); if (const Value* f = v.field(10)) ev::Conv<ev::Bool>::from(o, *f, x.enabled); if (const Value* f = v.field(11)) ev::Conv<ev::Bool>::from(o, *f, x.bullet); if (const Value* f = v.field(12)) ev::Conv<ev::Int64>::from(o, *f, x.user); }
Value S_physics_Capsule::to(EngineObjects& o, const kern::physics::Capsule& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.center1), ev::Conv<ev::Vector>::to(o, x.center2), ev::Conv<ev::Float>::to(o, x.radius)}); }
void S_physics_Capsule::from(EngineObjects& o, const Value& v, kern::physics::Capsule& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.center1); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.center2); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.radius); }
Value S_physics_ContactPoint::to(EngineObjects& o, const kern::physics::ContactPoint& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.point), ev::Conv<ev::Vector>::to(o, x.normal), ev::Conv<ev::Float>::to(o, x.separation), ev::Conv<ev::Float>::to(o, x.impulse)}); }
void S_physics_ContactPoint::from(EngineObjects& o, const Value& v, kern::physics::ContactPoint& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.point); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.normal); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.separation); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.impulse); }
Value S_physics_HitEvent::to(EngineObjects& o, const kern::physics::HitEvent& x) { return Value::structure({ev::Conv<ev::Struct<S_physics_Shape>>::to(o, x.shape_a), ev::Conv<ev::Struct<S_physics_Shape>>::to(o, x.shape_b), ev::Conv<ev::Struct<S_physics_Body>>::to(o, x.body_a), ev::Conv<ev::Struct<S_physics_Body>>::to(o, x.body_b), ev::Conv<ev::Vector>::to(o, x.point), ev::Conv<ev::Vector>::to(o, x.normal), ev::Conv<ev::Float>::to(o, x.approach_speed)}); }
void S_physics_HitEvent::from(EngineObjects& o, const Value& v, kern::physics::HitEvent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Struct<S_physics_Shape>>::from(o, *f, x.shape_a); if (const Value* f = v.field(1)) ev::Conv<ev::Struct<S_physics_Shape>>::from(o, *f, x.shape_b); if (const Value* f = v.field(2)) ev::Conv<ev::Struct<S_physics_Body>>::from(o, *f, x.body_a); if (const Value* f = v.field(3)) ev::Conv<ev::Struct<S_physics_Body>>::from(o, *f, x.body_b); if (const Value* f = v.field(4)) ev::Conv<ev::Vector>::from(o, *f, x.point); if (const Value* f = v.field(5)) ev::Conv<ev::Vector>::from(o, *f, x.normal); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.approach_speed); }
Value S_physics_Joint::to(EngineObjects& o, const kern::physics::Joint& x) { return Value::structure({ev::Conv<ev::Int64>::to(o, x.id)}); }
void S_physics_Joint::from(EngineObjects& o, const Value& v, kern::physics::Joint& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int64>::from(o, *f, x.id); }
Value S_physics_JointSettings::to(EngineObjects& o, const kern::physics::JointSettings& x) { return Value::structure({ev::Conv<ev::Enum<E_physics_JointType>>::to(o, x.type), ev::Conv<ev::Struct<S_physics_Body>>::to(o, x.body_a), ev::Conv<ev::Struct<S_physics_Body>>::to(o, x.body_b), ev::Conv<ev::Struct<S_physics_Pose>>::to(o, x.frame_a), ev::Conv<ev::Struct<S_physics_Pose>>::to(o, x.frame_b), ev::Conv<ev::Bool>::to(o, x.collide_connected), ev::Conv<ev::Bool>::to(o, x.enable_spring), ev::Conv<ev::Float>::to(o, x.hertz), ev::Conv<ev::Float>::to(o, x.damping_ratio), ev::Conv<ev::Bool>::to(o, x.enable_limit), ev::Conv<ev::Float>::to(o, x.lower), ev::Conv<ev::Float>::to(o, x.upper), ev::Conv<ev::Bool>::to(o, x.enable_motor), ev::Conv<ev::Float>::to(o, x.motor_speed), ev::Conv<ev::Float>::to(o, x.max_motor_torque), ev::Conv<ev::Float>::to(o, x.max_motor_force), ev::Conv<ev::Vector>::to(o, x.motor_velocity), ev::Conv<ev::Float>::to(o, x.length), ev::Conv<ev::Float>::to(o, x.linear_hertz), ev::Conv<ev::Float>::to(o, x.angular_hertz), ev::Conv<ev::Float>::to(o, x.linear_damping_ratio), ev::Conv<ev::Float>::to(o, x.angular_damping_ratio), ev::Conv<ev::Vector>::to(o, x.linear_velocity), ev::Conv<ev::Vector>::to(o, x.angular_velocity), ev::Conv<ev::Float>::to(o, x.max_velocity_force), ev::Conv<ev::Float>::to(o, x.max_velocity_torque), ev::Conv<ev::Float>::to(o, x.max_spring_force), ev::Conv<ev::Float>::to(o, x.max_spring_torque), ev::Conv<ev::Bool>::to(o, x.enable_steering), ev::Conv<ev::Bool>::to(o, x.enable_steering_limit), ev::Conv<ev::Float>::to(o, x.steering_hertz), ev::Conv<ev::Float>::to(o, x.steering_damping_ratio), ev::Conv<ev::Float>::to(o, x.target_steering), ev::Conv<ev::Float>::to(o, x.max_steering_torque), ev::Conv<ev::Float>::to(o, x.steering_lower), ev::Conv<ev::Float>::to(o, x.steering_upper), ev::Conv<ev::Float>::to(o, x.max_torque), ev::Conv<ev::Float>::to(o, x.constraint_hertz), ev::Conv<ev::Float>::to(o, x.constraint_damping_ratio), ev::Conv<ev::Int64>::to(o, x.user)}); }
void S_physics_JointSettings::from(EngineObjects& o, const Value& v, kern::physics::JointSettings& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_physics_JointType>>::from(o, *f, x.type); if (const Value* f = v.field(1)) ev::Conv<ev::Struct<S_physics_Body>>::from(o, *f, x.body_a); if (const Value* f = v.field(2)) ev::Conv<ev::Struct<S_physics_Body>>::from(o, *f, x.body_b); if (const Value* f = v.field(3)) ev::Conv<ev::Struct<S_physics_Pose>>::from(o, *f, x.frame_a); if (const Value* f = v.field(4)) ev::Conv<ev::Struct<S_physics_Pose>>::from(o, *f, x.frame_b); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.collide_connected); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.enable_spring); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.hertz); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.damping_ratio); if (const Value* f = v.field(9)) ev::Conv<ev::Bool>::from(o, *f, x.enable_limit); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.lower); if (const Value* f = v.field(11)) ev::Conv<ev::Float>::from(o, *f, x.upper); if (const Value* f = v.field(12)) ev::Conv<ev::Bool>::from(o, *f, x.enable_motor); if (const Value* f = v.field(13)) ev::Conv<ev::Float>::from(o, *f, x.motor_speed); if (const Value* f = v.field(14)) ev::Conv<ev::Float>::from(o, *f, x.max_motor_torque); if (const Value* f = v.field(15)) ev::Conv<ev::Float>::from(o, *f, x.max_motor_force); if (const Value* f = v.field(16)) ev::Conv<ev::Vector>::from(o, *f, x.motor_velocity); if (const Value* f = v.field(17)) ev::Conv<ev::Float>::from(o, *f, x.length); if (const Value* f = v.field(18)) ev::Conv<ev::Float>::from(o, *f, x.linear_hertz); if (const Value* f = v.field(19)) ev::Conv<ev::Float>::from(o, *f, x.angular_hertz); if (const Value* f = v.field(20)) ev::Conv<ev::Float>::from(o, *f, x.linear_damping_ratio); if (const Value* f = v.field(21)) ev::Conv<ev::Float>::from(o, *f, x.angular_damping_ratio); if (const Value* f = v.field(22)) ev::Conv<ev::Vector>::from(o, *f, x.linear_velocity); if (const Value* f = v.field(23)) ev::Conv<ev::Vector>::from(o, *f, x.angular_velocity); if (const Value* f = v.field(24)) ev::Conv<ev::Float>::from(o, *f, x.max_velocity_force); if (const Value* f = v.field(25)) ev::Conv<ev::Float>::from(o, *f, x.max_velocity_torque); if (const Value* f = v.field(26)) ev::Conv<ev::Float>::from(o, *f, x.max_spring_force); if (const Value* f = v.field(27)) ev::Conv<ev::Float>::from(o, *f, x.max_spring_torque); if (const Value* f = v.field(28)) ev::Conv<ev::Bool>::from(o, *f, x.enable_steering); if (const Value* f = v.field(29)) ev::Conv<ev::Bool>::from(o, *f, x.enable_steering_limit); if (const Value* f = v.field(30)) ev::Conv<ev::Float>::from(o, *f, x.steering_hertz); if (const Value* f = v.field(31)) ev::Conv<ev::Float>::from(o, *f, x.steering_damping_ratio); if (const Value* f = v.field(32)) ev::Conv<ev::Float>::from(o, *f, x.target_steering); if (const Value* f = v.field(33)) ev::Conv<ev::Float>::from(o, *f, x.max_steering_torque); if (const Value* f = v.field(34)) ev::Conv<ev::Float>::from(o, *f, x.steering_lower); if (const Value* f = v.field(35)) ev::Conv<ev::Float>::from(o, *f, x.steering_upper); if (const Value* f = v.field(36)) ev::Conv<ev::Float>::from(o, *f, x.max_torque); if (const Value* f = v.field(37)) ev::Conv<ev::Float>::from(o, *f, x.constraint_hertz); if (const Value* f = v.field(38)) ev::Conv<ev::Float>::from(o, *f, x.constraint_damping_ratio); if (const Value* f = v.field(39)) ev::Conv<ev::Int64>::from(o, *f, x.user); }
Value S_physics_MotionLocks::to(EngineObjects& o, const kern::physics::MotionLocks& x) { return Value::structure({ev::Conv<ev::List<ev::Bool>>::to(o, x.linear), ev::Conv<ev::List<ev::Bool>>::to(o, x.angular)}); }
void S_physics_MotionLocks::from(EngineObjects& o, const Value& v, kern::physics::MotionLocks& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Bool>>::from(o, *f, x.linear); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Bool>>::from(o, *f, x.angular); }
Value S_physics_MoverPlane::to(EngineObjects& o, const kern::physics::MoverPlane& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.normal), ev::Conv<ev::Float>::to(o, x.offset), ev::Conv<ev::Vector>::to(o, x.point), ev::Conv<ev::Struct<S_physics_Shape>>::to(o, x.shape), ev::Conv<ev::Struct<S_physics_Body>>::to(o, x.body)}); }
void S_physics_MoverPlane::from(EngineObjects& o, const Value& v, kern::physics::MoverPlane& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.normal); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.offset); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.point); if (const Value* f = v.field(3)) ev::Conv<ev::Struct<S_physics_Shape>>::from(o, *f, x.shape); if (const Value* f = v.field(4)) ev::Conv<ev::Struct<S_physics_Body>>::from(o, *f, x.body); }
Value S_physics_Pose::to(EngineObjects& o, const kern::physics::Pose& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Struct<S_Quat>>::to(o, x.rotation)}); }
void S_physics_Pose::from(EngineObjects& o, const Value& v, kern::physics::Pose& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(1)) ev::Conv<ev::Struct<S_Quat>>::from(o, *f, x.rotation); }
Value S_physics_QueryFilter::to(EngineObjects& o, const kern::physics::QueryFilter& x) { return Value::structure({ev::Conv<ev::Int64>::to(o, x.mask_bits), ev::Conv<ev::Int64>::to(o, x.category_bits)}); }
void S_physics_QueryFilter::from(EngineObjects& o, const Value& v, kern::physics::QueryFilter& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int64>::from(o, *f, x.mask_bits); if (const Value* f = v.field(1)) ev::Conv<ev::Int64>::from(o, *f, x.category_bits); }
Value S_physics_RayHit::to(EngineObjects& o, const kern::physics::RayHit& x) { return Value::structure({ev::Conv<ev::Bool>::to(o, x.hit), ev::Conv<ev::Struct<S_physics_Shape>>::to(o, x.shape), ev::Conv<ev::Struct<S_physics_Body>>::to(o, x.body), ev::Conv<ev::Vector>::to(o, x.point), ev::Conv<ev::Vector>::to(o, x.normal), ev::Conv<ev::Float>::to(o, x.fraction)}); }
void S_physics_RayHit::from(EngineObjects& o, const Value& v, kern::physics::RayHit& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Bool>::from(o, *f, x.hit); if (const Value* f = v.field(1)) ev::Conv<ev::Struct<S_physics_Shape>>::from(o, *f, x.shape); if (const Value* f = v.field(2)) ev::Conv<ev::Struct<S_physics_Body>>::from(o, *f, x.body); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.point); if (const Value* f = v.field(4)) ev::Conv<ev::Vector>::from(o, *f, x.normal); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.fraction); }
Value S_physics_Shape::to(EngineObjects& o, const kern::physics::Shape& x) { return Value::structure({ev::Conv<ev::Int64>::to(o, x.id)}); }
void S_physics_Shape::from(EngineObjects& o, const Value& v, kern::physics::Shape& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int64>::from(o, *f, x.id); }
Value S_physics_ShapeMaterial::to(EngineObjects& o, const kern::physics::ShapeMaterial& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.density), ev::Conv<ev::Float>::to(o, x.friction), ev::Conv<ev::Float>::to(o, x.restitution), ev::Conv<ev::Float>::to(o, x.rolling_resistance), ev::Conv<ev::Bool>::to(o, x.sensor), ev::Conv<ev::Int64>::to(o, x.category_bits), ev::Conv<ev::Int64>::to(o, x.mask_bits), ev::Conv<ev::Int>::to(o, x.group_index), ev::Conv<ev::Bool>::to(o, x.contact_events), ev::Conv<ev::Bool>::to(o, x.sensor_events), ev::Conv<ev::Bool>::to(o, x.hit_events), ev::Conv<ev::Int64>::to(o, x.user), ev::Conv<ev::Enum<E_physics_Surface>>::to(o, x.surface)}); }
void S_physics_ShapeMaterial::from(EngineObjects& o, const Value& v, kern::physics::ShapeMaterial& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.density); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.friction); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.restitution); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.rolling_resistance); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.sensor); if (const Value* f = v.field(5)) ev::Conv<ev::Int64>::from(o, *f, x.category_bits); if (const Value* f = v.field(6)) ev::Conv<ev::Int64>::from(o, *f, x.mask_bits); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.group_index); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.contact_events); if (const Value* f = v.field(9)) ev::Conv<ev::Bool>::from(o, *f, x.sensor_events); if (const Value* f = v.field(10)) ev::Conv<ev::Bool>::from(o, *f, x.hit_events); if (const Value* f = v.field(11)) ev::Conv<ev::Int64>::from(o, *f, x.user); if (const Value* f = v.field(12)) ev::Conv<ev::Enum<E_physics_Surface>>::from(o, *f, x.surface); }
Value S_physics_ShapeSettings::to(EngineObjects& o, const kern::physics::ShapeSettings& x) { return Value::structure({ev::Conv<ev::Enum<E_physics_ShapeKind>>::to(o, x.kind), ev::Conv<ev::Vector>::to(o, x.center), ev::Conv<ev::Struct<S_Quat>>::to(o, x.rotation), ev::Conv<ev::Vector>::to(o, x.size), ev::Conv<ev::Float>::to(o, x.radius), ev::Conv<ev::Float>::to(o, x.length), ev::Conv<ev::List<ev::Vector>>::to(o, x.points), ev::Conv<ev::List<ev::Int>>::to(o, x.indices), ev::Conv<ev::Struct<S_physics_ShapeMaterial>>::to(o, x.material)}); }
void S_physics_ShapeSettings::from(EngineObjects& o, const Value& v, kern::physics::ShapeSettings& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Enum<E_physics_ShapeKind>>::from(o, *f, x.kind); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.center); if (const Value* f = v.field(2)) ev::Conv<ev::Struct<S_Quat>>::from(o, *f, x.rotation); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.size); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.radius); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.length); if (const Value* f = v.field(6)) ev::Conv<ev::List<ev::Vector>>::from(o, *f, x.points); if (const Value* f = v.field(7)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.indices); if (const Value* f = v.field(8)) ev::Conv<ev::Struct<S_physics_ShapeMaterial>>::from(o, *f, x.material); }
Value S_physics_TouchEvent::to(EngineObjects& o, const kern::physics::TouchEvent& x) { return Value::structure({ev::Conv<ev::Struct<S_physics_Shape>>::to(o, x.shape_a), ev::Conv<ev::Struct<S_physics_Shape>>::to(o, x.shape_b), ev::Conv<ev::Struct<S_physics_Body>>::to(o, x.body_a), ev::Conv<ev::Struct<S_physics_Body>>::to(o, x.body_b)}); }
void S_physics_TouchEvent::from(EngineObjects& o, const Value& v, kern::physics::TouchEvent& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Struct<S_physics_Shape>>::from(o, *f, x.shape_a); if (const Value* f = v.field(1)) ev::Conv<ev::Struct<S_physics_Shape>>::from(o, *f, x.shape_b); if (const Value* f = v.field(2)) ev::Conv<ev::Struct<S_physics_Body>>::from(o, *f, x.body_a); if (const Value* f = v.field(3)) ev::Conv<ev::Struct<S_physics_Body>>::from(o, *f, x.body_b); }
Value S_physics_Version::to(EngineObjects& o, const kern::physics::Version& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.major), ev::Conv<ev::Int>::to(o, x.minor), ev::Conv<ev::Int>::to(o, x.revision)}); }
void S_physics_Version::from(EngineObjects& o, const Value& v, kern::physics::Version& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.major); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.minor); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.revision); }
Value S_physics_WorldSettings::to(EngineObjects& o, const kern::physics::WorldSettings& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.gravity), ev::Conv<ev::Bool>::to(o, x.enable_sleep), ev::Conv<ev::Bool>::to(o, x.enable_continuous), ev::Conv<ev::Float>::to(o, x.restitution_threshold), ev::Conv<ev::Int>::to(o, x.restitution_iterations), ev::Conv<ev::Bool>::to(o, x.restitution_propagation), ev::Conv<ev::Float>::to(o, x.hit_event_threshold), ev::Conv<ev::Float>::to(o, x.contact_hertz), ev::Conv<ev::Float>::to(o, x.contact_damping_ratio), ev::Conv<ev::Float>::to(o, x.contact_speed), ev::Conv<ev::Float>::to(o, x.maximum_linear_speed)}); }
void S_physics_WorldSettings::from(EngineObjects& o, const Value& v, kern::physics::WorldSettings& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.gravity); if (const Value* f = v.field(1)) ev::Conv<ev::Bool>::from(o, *f, x.enable_sleep); if (const Value* f = v.field(2)) ev::Conv<ev::Bool>::from(o, *f, x.enable_continuous); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.restitution_threshold); if (const Value* f = v.field(4)) ev::Conv<ev::Int>::from(o, *f, x.restitution_iterations); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.restitution_propagation); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.hit_event_threshold); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.contact_hertz); if (const Value* f = v.field(8)) ev::Conv<ev::Float>::from(o, *f, x.contact_damping_ratio); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.contact_speed); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.maximum_linear_speed); }

// engine.physics.surface_name: kern::physics::surface_name
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Surface>>();
  ev::take<ev::Enum<E_physics_Surface>>(o, in, 0, a0);
  decltype(auto) r = kern::physics::surface_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.physics.parse_surface: kern::physics::parse_surface
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::physics::Surface&>>();
  ev::take<ev::Enum<E_physics_Surface>>(o, in, 1, a1);
  decltype(auto) r = kern::physics::parse_surface(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Enum<E_physics_Surface>>(o, out, 1, a1);
  return true;
}

// engine.physics.surface_friction: kern::physics::surface_friction
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Surface>>();
  ev::take<ev::Enum<E_physics_Surface>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::physics::Surface>>();
  ev::take<ev::Enum<E_physics_Surface>>(o, in, 1, a1);
  decltype(auto) r = kern::physics::surface_friction(std::move(a0), std::move(a1));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.physics.frame_from_axis: kern::physics::frame_from_axis
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a2);
  decltype(auto) r = kern::physics::frame_from_axis(std::move(a0), std::move(a1), std::move(a2));
  ev::give<ev::Struct<S_physics_Pose>>(o, out, 0, r);
  return true;
}

// engine.physics.version: kern::physics::version
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::physics::version();
  ev::give<ev::Struct<S_physics_Version>>(o, out, 0, r);
  return true;
}

// engine.world.create: kern::physics::World::create
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::WorldSettings const&>>();
  ev::take<ev::Struct<S_physics_WorldSettings>>(o, in, 1, a0);
  decltype(auto) r = self->create(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.world.destroy: kern::physics::World::destroy
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  self->destroy();
  return true;
}

// engine.world.valid: kern::physics::World::valid
bool c7(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->valid();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.world.create_body: kern::physics::World::create_body
bool c8(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::BodySettings const&>>();
  ev::take<ev::Struct<S_physics_BodySettings>>(o, in, 1, a0);
  decltype(auto) r = self->create_body(a0);
  ev::give<ev::Struct<S_physics_Body>>(o, out, 0, r);
  return true;
}

// engine.world.destroy_body: kern::physics::World::destroy_body
bool c9(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  self->destroy_body(std::move(a0));
  return true;
}

// engine.world.body_valid: kern::physics::World::body_valid
bool c10(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  decltype(auto) r = self->body_valid(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.world.create_shape: kern::physics::World::create_shape
bool c11(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::physics::ShapeSettings const&>>();
  ev::take<ev::Struct<S_physics_ShapeSettings>>(o, in, 2, a1);
  decltype(auto) r = self->create_shape(std::move(a0), a1);
  ev::give<ev::Struct<S_physics_Shape>>(o, out, 0, r);
  return true;
}

// engine.world.destroy_shape: kern::physics::World::destroy_shape
bool c12(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Shape>>();
  ev::take<ev::Struct<S_physics_Shape>>(o, in, 1, a0);
  self->destroy_shape(std::move(a0));
  return true;
}

// engine.world.mesh_count: kern::physics::World::mesh_count
bool c13(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->mesh_count();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.world.create_joint: kern::physics::World::create_joint
bool c14(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::JointSettings const&>>();
  ev::take<ev::Struct<S_physics_JointSettings>>(o, in, 1, a0);
  decltype(auto) r = self->create_joint(a0);
  ev::give<ev::Struct<S_physics_Joint>>(o, out, 0, r);
  return true;
}

// engine.world.destroy_joint: kern::physics::World::destroy_joint
bool c15(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  self->destroy_joint(std::move(a0), std::move(a1));
  return true;
}

// engine.world.joint_valid: kern::physics::World::joint_valid
bool c16(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  decltype(auto) r = self->joint_valid(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.world.step: kern::physics::World::step
bool c17(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  self->step(std::move(a0), std::move(a1));
  return true;
}

// engine.world.pose: kern::physics::World::pose
bool c18(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  decltype(auto) r = self->pose(std::move(a0));
  ev::give<ev::Struct<S_physics_Pose>>(o, out, 0, r);
  return true;
}

// engine.world.set_pose: kern::physics::World::set_pose
bool c19(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::physics::Pose const&>>();
  ev::take<ev::Struct<S_physics_Pose>>(o, in, 2, a1);
  self->set_pose(std::move(a0), a1);
  return true;
}

// engine.world.set_target_pose: kern::physics::World::set_target_pose
bool c20(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::physics::Pose const&>>();
  ev::take<ev::Struct<S_physics_Pose>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  self->set_target_pose(std::move(a0), a1, std::move(a2));
  return true;
}

// engine.world.linear_velocity: kern::physics::World::linear_velocity
bool c21(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  decltype(auto) r = self->linear_velocity(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.world.angular_velocity: kern::physics::World::angular_velocity
bool c22(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  decltype(auto) r = self->angular_velocity(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.world.set_linear_velocity: kern::physics::World::set_linear_velocity
bool c23(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  self->set_linear_velocity(std::move(a0), std::move(a1));
  return true;
}

// engine.world.set_angular_velocity: kern::physics::World::set_angular_velocity
bool c24(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  self->set_angular_velocity(std::move(a0), std::move(a1));
  return true;
}

// engine.world.apply_force: kern::physics::World::apply_force
bool c25(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  self->apply_force(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.world.apply_force_to_center: kern::physics::World::apply_force_to_center
bool c26(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  self->apply_force_to_center(std::move(a0), std::move(a1));
  return true;
}

// engine.world.apply_torque: kern::physics::World::apply_torque
bool c27(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  self->apply_torque(std::move(a0), std::move(a1));
  return true;
}

// engine.world.apply_linear_impulse: kern::physics::World::apply_linear_impulse
bool c28(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  self->apply_linear_impulse(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.world.apply_linear_impulse_to_center: kern::physics::World::apply_linear_impulse_to_center
bool c29(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  self->apply_linear_impulse_to_center(std::move(a0), std::move(a1));
  return true;
}

// engine.world.apply_angular_impulse: kern::physics::World::apply_angular_impulse
bool c30(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  self->apply_angular_impulse(std::move(a0), std::move(a1));
  return true;
}

// engine.world.set_awake: kern::physics::World::set_awake
bool c31(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  self->set_awake(std::move(a0), std::move(a1));
  return true;
}

// engine.world.awake: kern::physics::World::awake
bool c32(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  decltype(auto) r = self->awake(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.world.mass: kern::physics::World::mass
bool c33(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  decltype(auto) r = self->mass(std::move(a0));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.world.body_type: kern::physics::World::body_type
bool c34(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  decltype(auto) r = self->body_type(std::move(a0));
  ev::give<ev::Enum<E_physics_BodyType>>(o, out, 0, r);
  return true;
}

// engine.world.set_body_type: kern::physics::World::set_body_type
bool c35(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::physics::BodyType>>();
  ev::take<ev::Enum<E_physics_BodyType>>(o, in, 2, a1);
  self->set_body_type(std::move(a0), std::move(a1));
  return true;
}

// engine.world.body_masks: kern::physics::World::body_masks
bool c36(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<unsigned long, std::allocator<unsigned long> >&>>();
  ev::take<ev::List<ev::Int64>>(o, in, 2, a1);
  self->body_masks(std::move(a0), a1);
  ev::give<ev::List<ev::Int64>>(o, out, 0, a1);
  return true;
}

// engine.world.set_body_masks: kern::physics::World::set_body_masks
bool c37(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  std::vector<unsigned long> a1;
  ev::take<ev::List<ev::Int64>>(o, in, 2, a1);
  self->set_body_masks(std::move(a0), kern::Span<unsigned long const>(a1.data(), a1.size()));
  return true;
}

// engine.world.local_point: kern::physics::World::local_point
bool c38(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  decltype(auto) r = self->local_point(std::move(a0), std::move(a1));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.world.world_point: kern::physics::World::world_point
bool c39(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  decltype(auto) r = self->world_point(std::move(a0), std::move(a1));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.world.world_vector: kern::physics::World::world_vector
bool c40(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  decltype(auto) r = self->world_vector(std::move(a0), std::move(a1));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.world.body_user: kern::physics::World::body_user
bool c41(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  decltype(auto) r = self->body_user(std::move(a0));
  ev::give<ev::Int64>(o, out, 0, r);
  return true;
}

// engine.world.shape_user: kern::physics::World::shape_user
bool c42(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Shape>>();
  ev::take<ev::Struct<S_physics_Shape>>(o, in, 1, a0);
  decltype(auto) r = self->shape_user(std::move(a0));
  ev::give<ev::Int64>(o, out, 0, r);
  return true;
}

// engine.world.shape_body: kern::physics::World::shape_body
bool c43(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Shape>>();
  ev::take<ev::Struct<S_physics_Shape>>(o, in, 1, a0);
  decltype(auto) r = self->shape_body(std::move(a0));
  ev::give<ev::Struct<S_physics_Body>>(o, out, 0, r);
  return true;
}

// engine.world.set_motion_locks: kern::physics::World::set_motion_locks
bool c44(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::physics::MotionLocks const&>>();
  ev::take<ev::Struct<S_physics_MotionLocks>>(o, in, 2, a1);
  self->set_motion_locks(std::move(a0), a1);
  return true;
}

// engine.world.set_mass: kern::physics::World::set_mass
bool c45(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  self->set_mass(std::move(a0), std::move(a1));
  return true;
}

// engine.world.set_mass_data: kern::physics::World::set_mass_data
bool c46(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  std::vector<float> a3;
  ev::take<ev::List<ev::Float>>(o, in, 4, a3);
  self->set_mass_data(std::move(a0), std::move(a1), std::move(a2), a3.empty() ? nullptr : a3.data());
  return true;
}

// engine.world.set_joint_thresholds: kern::physics::World::set_joint_thresholds
bool c47(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  self->set_joint_thresholds(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.world.joint_force: kern::physics::World::joint_force
bool c48(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  decltype(auto) r = self->joint_force(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.world.set_joint_motor: kern::physics::World::set_joint_motor
bool c49(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a3);
  self->set_joint_motor(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  return true;
}

// engine.world.set_wheel_steering: kern::physics::World::set_wheel_steering
bool c50(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  self->set_wheel_steering(std::move(a0), std::move(a1));
  return true;
}

// engine.world.wheel_steering: kern::physics::World::wheel_steering
bool c51(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  decltype(auto) r = self->wheel_steering(std::move(a0));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.world.joint_angle: kern::physics::World::joint_angle
bool c52(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  decltype(auto) r = self->joint_angle(std::move(a0));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.world.joint_speed: kern::physics::World::joint_speed
bool c53(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  decltype(auto) r = self->joint_speed(std::move(a0));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.world.set_joint_spring: kern::physics::World::set_joint_spring
bool c54(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a3);
  self->set_joint_spring(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  return true;
}

// engine.world.hinge_lever: kern::physics::World::hinge_lever
bool c55(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  decltype(auto) r = self->hinge_lever(std::move(a0), std::move(a1));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.world.joint_torque: kern::physics::World::joint_torque
bool c56(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Joint>>();
  ev::take<ev::Struct<S_physics_Joint>>(o, in, 1, a0);
  decltype(auto) r = self->joint_torque(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.world.contact_points: kern::physics::World::contact_points
bool c57(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<kern::physics::ContactPoint, std::allocator<kern::physics::ContactPoint> >&>>();
  ev::take<ev::List<ev::Struct<S_physics_ContactPoint>>>(o, in, 2, a1);
  self->contact_points(std::move(a0), a1);
  ev::give<ev::List<ev::Struct<S_physics_ContactPoint>>>(o, out, 0, a1);
  return true;
}

// engine.world.touching: kern::physics::World::touching
bool c58(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<std::pair<kern::physics::Body, float>, std::allocator<std::pair<kern::physics::Body, float> > >&>>();
  ev::take<ev::List<ev::Pair<ev::Struct<S_physics_Body>, ev::Float>>>(o, in, 2, a1);
  self->touching(std::move(a0), a1);
  ev::give<ev::List<ev::Pair<ev::Struct<S_physics_Body>, ev::Float>>>(o, out, 0, a1);
  return true;
}

// engine.world.cast_ray: kern::physics::World::cast_ray
bool c59(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::physics::QueryFilter>>();
  ev::take<ev::Struct<S_physics_QueryFilter>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 4, a3);
  decltype(auto) r = self->cast_ray(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  ev::give<ev::Struct<S_physics_RayHit>>(o, out, 0, r);
  return true;
}

// engine.world.cast_ray_ignoring: kern::physics::World::cast_ray_ignoring
bool c60(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  std::vector<kern::physics::Body> a2;
  ev::take<ev::List<ev::Struct<S_physics_Body>>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::physics::QueryFilter>>();
  ev::take<ev::Struct<S_physics_QueryFilter>>(o, in, 4, a3);
  decltype(auto) r = self->cast_ray_ignoring(std::move(a0), std::move(a1), kern::Span<kern::physics::Body const>(a2.data(), a2.size()), std::move(a3));
  ev::give<ev::Struct<S_physics_RayHit>>(o, out, 0, r);
  return true;
}

// engine.world.cast_sphere: kern::physics::World::cast_sphere
bool c61(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::physics::QueryFilter>>();
  ev::take<ev::Struct<S_physics_QueryFilter>>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 5, a4);
  decltype(auto) r = self->cast_sphere(std::move(a0), std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::Struct<S_physics_RayHit>>(o, out, 0, r);
  return true;
}

// engine.world.cast_mover: kern::physics::World::cast_mover
bool c62(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::physics::Capsule const&>>();
  ev::take<ev::Struct<S_physics_Capsule>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::physics::QueryFilter>>();
  ev::take<ev::Struct<S_physics_QueryFilter>>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 5, a4);
  decltype(auto) r = self->cast_mover(std::move(a0), a1, std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.world.collide_mover: kern::physics::World::collide_mover
bool c63(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::physics::Capsule const&>>();
  ev::take<ev::Struct<S_physics_Capsule>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::physics::QueryFilter>>();
  ev::take<ev::Struct<S_physics_QueryFilter>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<std::vector<kern::physics::MoverPlane, std::allocator<kern::physics::MoverPlane> >&>>();
  ev::take<ev::List<ev::Struct<S_physics_MoverPlane>>>(o, in, 5, a4);
  self->collide_mover(std::move(a0), a1, std::move(a2), std::move(a3), a4);
  ev::give<ev::List<ev::Struct<S_physics_MoverPlane>>>(o, out, 0, a4);
  return true;
}

// engine.world.explode: kern::physics::World::explode
bool c64(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<unsigned long>>();
  ev::take<ev::Int64>(o, in, 5, a4);
  self->explode(std::move(a0), std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  return true;
}

// engine.world.push_body: kern::physics::World::push_body
bool c65(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 4, a3);
  self->push_body(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  return true;
}

// engine.world.contact_begin: kern::physics::World::contact_begin
bool c66(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->contact_begin();
  ev::give<ev::List<ev::Struct<S_physics_TouchEvent>>>(o, out, 0, r);
  return true;
}

// engine.world.contact_end: kern::physics::World::contact_end
bool c67(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->contact_end();
  ev::give<ev::List<ev::Struct<S_physics_TouchEvent>>>(o, out, 0, r);
  return true;
}

// engine.world.sensor_begin: kern::physics::World::sensor_begin
bool c68(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->sensor_begin();
  ev::give<ev::List<ev::Struct<S_physics_TouchEvent>>>(o, out, 0, r);
  return true;
}

// engine.world.sensor_end: kern::physics::World::sensor_end
bool c69(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->sensor_end();
  ev::give<ev::List<ev::Struct<S_physics_TouchEvent>>>(o, out, 0, r);
  return true;
}

// engine.world.hits: kern::physics::World::hits
bool c70(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->hits();
  ev::give<ev::List<ev::Struct<S_physics_HitEvent>>>(o, out, 0, r);
  return true;
}

// engine.world.joint_events: kern::physics::World::joint_events
bool c71(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->joint_events();
  ev::give<ev::List<ev::Struct<S_physics_Joint>>>(o, out, 0, r);
  return true;
}

// engine.world.body_count: kern::physics::World::body_count
bool c72(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->body_count();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.world.awake_body_count: kern::physics::World::awake_body_count
bool c73(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->awake_body_count();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.world.step_count: kern::physics::World::step_count
bool c74(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->step_count();
  ev::give<ev::Int64>(o, out, 0, r);
  return true;
}

// engine.world.gravity: kern::physics::World::gravity
bool c75(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::World>(o, in, 0, "world");
  if (!self) return false;
  decltype(auto) r = self->gravity();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.mouse_grab.begin: kern::physics::MouseGrab::begin
bool c76(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::MouseGrab>(o, in, 0, "mouse_grab");
  if (!self) return false;
  kern::physics::World* a0 = ev::object<kern::physics::World>(o, in, 1, "world");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 5, a4);
  decltype(auto) r = self->begin(*a0, std::move(a1), std::move(a2), std::move(a3), std::move(a4));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.mouse_grab.set_target: kern::physics::MouseGrab::set_target
bool c77(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::MouseGrab>(o, in, 0, "mouse_grab");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->set_target(std::move(a0));
  return true;
}

// engine.mouse_grab.target: kern::physics::MouseGrab::target
bool c78(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::MouseGrab>(o, in, 0, "mouse_grab");
  if (!self) return false;
  decltype(auto) r = self->target();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.mouse_grab.update: kern::physics::MouseGrab::update
bool c79(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::MouseGrab>(o, in, 0, "mouse_grab");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->update(std::move(a0));
  return true;
}

// engine.mouse_grab.end: kern::physics::MouseGrab::end
bool c80(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::MouseGrab>(o, in, 0, "mouse_grab");
  if (!self) return false;
  self->end();
  return true;
}

// engine.mouse_grab.active: kern::physics::MouseGrab::active
bool c81(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::MouseGrab>(o, in, 0, "mouse_grab");
  if (!self) return false;
  decltype(auto) r = self->active();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.mouse_grab.body: kern::physics::MouseGrab::body
bool c82(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::MouseGrab>(o, in, 0, "mouse_grab");
  if (!self) return false;
  decltype(auto) r = self->body();
  ev::give<ev::Struct<S_physics_Body>>(o, out, 0, r);
  return true;
}

// engine.mouse_grab.anchor: kern::physics::MouseGrab::anchor
bool c83(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::MouseGrab>(o, in, 0, "mouse_grab");
  if (!self) return false;
  decltype(auto) r = self->anchor();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.mouse_grab.max_force: kern::physics::MouseGrab::max_force
bool c84(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::MouseGrab>(o, in, 0, "mouse_grab");
  if (!self) return false;
  decltype(auto) r = self->max_force();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.character_mover.reset: kern::physics::CharacterMover::reset
bool c85(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::CharacterMover>(o, in, 0, "character_mover");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->reset(std::move(a0));
  return true;
}

// engine.character_mover.step: kern::physics::CharacterMover::step
bool c86(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::CharacterMover>(o, in, 0, "character_mover");
  if (!self) return false;
  kern::physics::World* a0 = ev::object<kern::physics::World>(o, in, 1, "world");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 5, a4);
  auto a5 = ev::blank<std::remove_cvref_t<kern::physics::Body>>();
  ev::take<ev::Struct<S_physics_Body>>(o, in, 6, a5);
  decltype(auto) r = self->step(*a0, std::move(a1), std::move(a2), std::move(a3), std::move(a4), std::move(a5));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.character_mover.position: kern::physics::CharacterMover::position
bool c87(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::CharacterMover>(o, in, 0, "character_mover");
  if (!self) return false;
  decltype(auto) r = self->position();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.character_mover.velocity: kern::physics::CharacterMover::velocity
bool c88(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::CharacterMover>(o, in, 0, "character_mover");
  if (!self) return false;
  decltype(auto) r = self->velocity();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.character_mover.set_velocity: kern::physics::CharacterMover::set_velocity
bool c89(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::CharacterMover>(o, in, 0, "character_mover");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->set_velocity(std::move(a0));
  return true;
}

// engine.character_mover.jump: kern::physics::CharacterMover::jump
bool c90(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::CharacterMover>(o, in, 0, "character_mover");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->jump(std::move(a0));
  return true;
}

// engine.character_mover.on_ground: kern::physics::CharacterMover::on_ground
bool c91(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::CharacterMover>(o, in, 0, "character_mover");
  if (!self) return false;
  decltype(auto) r = self->on_ground();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.character_mover.planes: kern::physics::CharacterMover::planes
bool c92(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::CharacterMover>(o, in, 0, "character_mover");
  if (!self) return false;
  decltype(auto) r = self->planes();
  ev::give<ev::List<ev::Struct<S_physics_MoverPlane>>>(o, out, 0, r);
  return true;
}

// engine.character_mover.capsule: kern::physics::CharacterMover::capsule
bool c93(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::physics::CharacterMover>(o, in, 0, "character_mover");
  if (!self) return false;
  decltype(auto) r = self->capsule();
  ev::give<ev::Struct<S_physics_Capsule>>(o, out, 0, r);
  return true;
}

// engine.physics.hull_mesh: kern::physics::hull_mesh
bool c94(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::vector<kern::Vec3> a0;
  ev::take<ev::List<ev::Vector>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::MeshData&>>();
  ev::take<ev::Struct<S_MeshData>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a2);
  decltype(auto) r = kern::physics::hull_mesh(kern::Span<kern::Vec3 const>(a0.data(), a0.size()), a1, std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_MeshData>>(o, out, 1, a1);
  return true;
}

// engine.character_mover.make: kern::physics::CharacterMover::CharacterMover
bool c95(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::physics::CharacterMover>(std::unique_ptr<kern::physics::CharacterMover>(new kern::physics::CharacterMover())));
  return true;
}

// engine.character_mover.release: kern::physics::CharacterMover::~CharacterMover
bool c96(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::physics::CharacterMover>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no CharacterMover that was made";
    return false;
  }
  return true;
}

// engine.mouse_grab.make: kern::physics::MouseGrab::MouseGrab
bool c97(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::physics::MouseGrab>(std::unique_ptr<kern::physics::MouseGrab>(new kern::physics::MouseGrab())));
  return true;
}

// engine.mouse_grab.release: kern::physics::MouseGrab::~MouseGrab
bool c98(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::physics::MouseGrab>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no MouseGrab that was made";
    return false;
  }
  return true;
}

// engine.world.make: kern::physics::World::World
bool c99(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::physics::World>(std::unique_ptr<kern::physics::World>(new kern::physics::World())));
  return true;
}

// engine.world.release: kern::physics::World::~World
bool c100(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::physics::World>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no World that was made";
    return false;
  }
  return true;
}

const EngineCall kCalls[] = {
    {"engine.physics.surface_name", &c0},
    {"engine.physics.parse_surface", &c1},
    {"engine.physics.surface_friction", &c2},
    {"engine.physics.frame_from_axis", &c3},
    {"engine.physics.version", &c4},
    {"engine.world.create", &c5},
    {"engine.world.destroy", &c6},
    {"engine.world.valid", &c7},
    {"engine.world.create_body", &c8},
    {"engine.world.destroy_body", &c9},
    {"engine.world.body_valid", &c10},
    {"engine.world.create_shape", &c11},
    {"engine.world.destroy_shape", &c12},
    {"engine.world.mesh_count", &c13},
    {"engine.world.create_joint", &c14},
    {"engine.world.destroy_joint", &c15},
    {"engine.world.joint_valid", &c16},
    {"engine.world.step", &c17},
    {"engine.world.pose", &c18},
    {"engine.world.set_pose", &c19},
    {"engine.world.set_target_pose", &c20},
    {"engine.world.linear_velocity", &c21},
    {"engine.world.angular_velocity", &c22},
    {"engine.world.set_linear_velocity", &c23},
    {"engine.world.set_angular_velocity", &c24},
    {"engine.world.apply_force", &c25},
    {"engine.world.apply_force_to_center", &c26},
    {"engine.world.apply_torque", &c27},
    {"engine.world.apply_linear_impulse", &c28},
    {"engine.world.apply_linear_impulse_to_center", &c29},
    {"engine.world.apply_angular_impulse", &c30},
    {"engine.world.set_awake", &c31},
    {"engine.world.awake", &c32},
    {"engine.world.mass", &c33},
    {"engine.world.body_type", &c34},
    {"engine.world.set_body_type", &c35},
    {"engine.world.body_masks", &c36},
    {"engine.world.set_body_masks", &c37},
    {"engine.world.local_point", &c38},
    {"engine.world.world_point", &c39},
    {"engine.world.world_vector", &c40},
    {"engine.world.body_user", &c41},
    {"engine.world.shape_user", &c42},
    {"engine.world.shape_body", &c43},
    {"engine.world.set_motion_locks", &c44},
    {"engine.world.set_mass", &c45},
    {"engine.world.set_mass_data", &c46},
    {"engine.world.set_joint_thresholds", &c47},
    {"engine.world.joint_force", &c48},
    {"engine.world.set_joint_motor", &c49},
    {"engine.world.set_wheel_steering", &c50},
    {"engine.world.wheel_steering", &c51},
    {"engine.world.joint_angle", &c52},
    {"engine.world.joint_speed", &c53},
    {"engine.world.set_joint_spring", &c54},
    {"engine.world.hinge_lever", &c55},
    {"engine.world.joint_torque", &c56},
    {"engine.world.contact_points", &c57},
    {"engine.world.touching", &c58},
    {"engine.world.cast_ray", &c59},
    {"engine.world.cast_ray_ignoring", &c60},
    {"engine.world.cast_sphere", &c61},
    {"engine.world.cast_mover", &c62},
    {"engine.world.collide_mover", &c63},
    {"engine.world.explode", &c64},
    {"engine.world.push_body", &c65},
    {"engine.world.contact_begin", &c66},
    {"engine.world.contact_end", &c67},
    {"engine.world.sensor_begin", &c68},
    {"engine.world.sensor_end", &c69},
    {"engine.world.hits", &c70},
    {"engine.world.joint_events", &c71},
    {"engine.world.body_count", &c72},
    {"engine.world.awake_body_count", &c73},
    {"engine.world.step_count", &c74},
    {"engine.world.gravity", &c75},
    {"engine.mouse_grab.begin", &c76},
    {"engine.mouse_grab.set_target", &c77},
    {"engine.mouse_grab.target", &c78},
    {"engine.mouse_grab.update", &c79},
    {"engine.mouse_grab.end", &c80},
    {"engine.mouse_grab.active", &c81},
    {"engine.mouse_grab.body", &c82},
    {"engine.mouse_grab.anchor", &c83},
    {"engine.mouse_grab.max_force", &c84},
    {"engine.character_mover.reset", &c85},
    {"engine.character_mover.step", &c86},
    {"engine.character_mover.position", &c87},
    {"engine.character_mover.velocity", &c88},
    {"engine.character_mover.set_velocity", &c89},
    {"engine.character_mover.jump", &c90},
    {"engine.character_mover.on_ground", &c91},
    {"engine.character_mover.planes", &c92},
    {"engine.character_mover.capsule", &c93},
    {"engine.physics.hull_mesh", &c94},
    {"engine.character_mover.make", &c95},
    {"engine.character_mover.release", &c96},
    {"engine.mouse_grab.make", &c97},
    {"engine.mouse_grab.release", &c98},
    {"engine.world.make", &c99},
    {"engine.world.release", &c100},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

const EngineMaker kMakers[] = {
    {EngineObjects::kind<kern::physics::CharacterMover>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::physics::CharacterMover>(handle, std::unique_ptr<kern::physics::CharacterMover>(new kern::physics::CharacterMover())); }},
    {EngineObjects::kind<kern::physics::MouseGrab>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::physics::MouseGrab>(handle, std::unique_ptr<kern::physics::MouseGrab>(new kern::physics::MouseGrab())); }},
    {EngineObjects::kind<kern::physics::World>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::physics::World>(handle, std::unique_ptr<kern::physics::World>(new kern::physics::World())); }},
};
const EngineMakers kAllMakers(kMakers, std::size(kMakers));

}  // namespace
}  // namespace kern::logic
