// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/core/base.h"
#include "kern/core/jobs.h"
#include "kern/math/math.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct E_LogLevel { using type = kern::LogLevel; static constexpr type values[] = {kern::LogLevel::debug, kern::LogLevel::info, kern::LogLevel::warn, kern::LogLevel::error}; };
struct S_Aabb { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Aabb& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Aabb& x); };
struct S_Camera { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Camera& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Camera& x); };
struct S_Mat4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Mat4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Mat4& x); };
struct S_Quat { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Quat& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Quat& x); };
struct S_Sphere { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Sphere& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Sphere& x); };
struct S_Vec4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Vec4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Vec4& x); };
Value S_Aabb::to(EngineObjects& o, const kern::Aabb& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.min), ev::Conv<ev::Vector>::to(o, x.max)}); }
void S_Aabb::from(EngineObjects& o, const Value& v, kern::Aabb& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.min); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.max); }
Value S_Camera::to(EngineObjects& o, const kern::Camera& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.eye), ev::Conv<ev::Vector>::to(o, x.target), ev::Conv<ev::Vector>::to(o, x.up), ev::Conv<ev::Num<f32>>::to(o, x.fov), ev::Conv<ev::Num<f32>>::to(o, x.near), ev::Conv<ev::Num<f32>>::to(o, x.far), ev::Conv<ev::Bool>::to(o, x.orthographic), ev::Conv<ev::Num<f32>>::to(o, x.ortho_height), ev::Conv<ev::Bool>::to(o, x.has_projection), ev::Conv<ev::Struct<S_Mat4>>::to(o, x.projection)}); }
void S_Camera::from(EngineObjects& o, const Value& v, kern::Camera& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.eye); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.target); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.up); if (const Value* f = v.field(3)) ev::Conv<ev::Num<f32>>::from(o, *f, x.fov); if (const Value* f = v.field(4)) ev::Conv<ev::Num<f32>>::from(o, *f, x.near); if (const Value* f = v.field(5)) ev::Conv<ev::Num<f32>>::from(o, *f, x.far); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.orthographic); if (const Value* f = v.field(7)) ev::Conv<ev::Num<f32>>::from(o, *f, x.ortho_height); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.has_projection); if (const Value* f = v.field(9)) ev::Conv<ev::Struct<S_Mat4>>::from(o, *f, x.projection); }
Value S_Mat4::to(EngineObjects& o, const kern::Mat4& x) { return Value::structure({ev::Conv<ev::List<ev::Num<f32>>>::to(o, x.m)}); }
void S_Mat4::from(EngineObjects& o, const Value& v, kern::Mat4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Num<f32>>>::from(o, *f, x.m); }
Value S_Quat::to(EngineObjects& o, const kern::Quat& x) { return Value::structure({ev::Conv<ev::Num<f32>>::to(o, x.x), ev::Conv<ev::Num<f32>>::to(o, x.y), ev::Conv<ev::Num<f32>>::to(o, x.z), ev::Conv<ev::Num<f32>>::to(o, x.w)}); }
void S_Quat::from(EngineObjects& o, const Value& v, kern::Quat& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<f32>>::from(o, *f, x.x); if (const Value* f = v.field(1)) ev::Conv<ev::Num<f32>>::from(o, *f, x.y); if (const Value* f = v.field(2)) ev::Conv<ev::Num<f32>>::from(o, *f, x.z); if (const Value* f = v.field(3)) ev::Conv<ev::Num<f32>>::from(o, *f, x.w); }
Value S_Sphere::to(EngineObjects& o, const kern::Sphere& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.center), ev::Conv<ev::Num<f32>>::to(o, x.radius)}); }
void S_Sphere::from(EngineObjects& o, const Value& v, kern::Sphere& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.center); if (const Value* f = v.field(1)) ev::Conv<ev::Num<f32>>::from(o, *f, x.radius); }
Value S_Vec4::to(EngineObjects& o, const kern::Vec4& x) { return Value::structure({ev::Conv<ev::Num<f32>>::to(o, x.x), ev::Conv<ev::Num<f32>>::to(o, x.y), ev::Conv<ev::Num<f32>>::to(o, x.z), ev::Conv<ev::Num<f32>>::to(o, x.w)}); }
void S_Vec4::from(EngineObjects& o, const Value& v, kern::Vec4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<f32>>::from(o, *f, x.x); if (const Value* f = v.field(1)) ev::Conv<ev::Num<f32>>::from(o, *f, x.y); if (const Value* f = v.field(2)) ev::Conv<ev::Num<f32>>::from(o, *f, x.z); if (const Value* f = v.field(3)) ev::Conv<ev::Num<f32>>::from(o, *f, x.w); }

// engine.fail: kern::fail
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  decltype(auto) r = kern::fail("%s", a0.c_str());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.last_error: kern::last_error
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::last_error();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.clear_error: kern::clear_error
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::clear_error();
  return true;
}

// engine.log: kern::log
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::LogLevel>>();
  ev::take<ev::Enum<E_LogLevel>>(o, in, 0, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 1, a1);
  kern::log(std::move(a0), "%s", a1.c_str());
  return true;
}

// engine.now_ms: kern::now_ms
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::now_ms();
  ev::give<ev::Num<f64>>(o, out, 0, r);
  return true;
}

// engine.wall_ms: kern::wall_ms
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::wall_ms();
  ev::give<ev::Num<f64>>(o, out, 0, r);
  return true;
}

// engine.hash_u32: kern::hash_u32
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 0, a0);
  decltype(auto) r = kern::hash_u32(std::move(a0));
  ev::give<ev::Num<u32>>(o, out, 0, r);
  return true;
}

// engine.jobs.submit: kern::jobs::submit
bool c7(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<unsigned char, std::allocator<unsigned char> >>>();
  ev::take<ev::List<ev::Num<u8>>>(o, in, 1, a1);
  decltype(auto) r = kern::jobs::submit(std::move(a0), std::move(a1));
  ev::give<ev::Num<u32>>(o, out, 0, r);
  return true;
}

// engine.jobs.take: kern::jobs::take
bool c8(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::vector<unsigned char, std::allocator<unsigned char> >&>>();
  ev::take<ev::List<ev::Num<u8>>>(o, in, 1, a1);
  decltype(auto) r = kern::jobs::take(std::move(a0), a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::List<ev::Num<u8>>>(o, out, 1, a1);
  return true;
}

// engine.jobs.cancel: kern::jobs::cancel
bool c9(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 0, a0);
  kern::jobs::cancel(std::move(a0));
  return true;
}

// engine.jobs.pump: kern::jobs::pump
bool c10(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<f64>>(o, in, 0, a0);
  kern::jobs::pump(std::move(a0));
  return true;
}

// engine.jobs.pending: kern::jobs::pending
bool c11(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::jobs::pending();
  ev::give<ev::Num<u32>>(o, out, 0, r);
  return true;
}

// engine.jobs.set_external: kern::jobs::set_external
bool c12(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 0, a0);
  kern::jobs::set_external(std::move(a0));
  return true;
}

// engine.jobs.external: kern::jobs::external
bool c13(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::jobs::external();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.jobs.set_threads: kern::jobs::set_threads
bool c14(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 0, a0);
  kern::jobs::set_threads(std::move(a0));
  return true;
}

// engine.jobs.threads: kern::jobs::threads
bool c15(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::jobs::threads();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.jobs.reset: kern::jobs::reset
bool c16(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::jobs::reset();
  return true;
}

// engine.jobs.take_queued: kern::jobs::take_queued
bool c17(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::jobs::take_queued();
  ev::give<ev::List<ev::Num<u8>>>(o, out, 0, r);
  return true;
}

// engine.jobs.run: kern::jobs::run
bool c18(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 0, a0);
  std::vector<unsigned char> a1;
  ev::take<ev::List<ev::Num<u8>>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<std::vector<unsigned char, std::allocator<unsigned char> >&>>();
  ev::take<ev::List<ev::Num<u8>>>(o, in, 2, a2);
  kern::jobs::run(std::move(a0), kern::Span<unsigned char const>(a1.data(), a1.size()), a2);
  ev::give<ev::List<ev::Num<u8>>>(o, out, 0, a2);
  return true;
}

// engine.jobs.done: kern::jobs::done
bool c19(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 0, a0);
  std::vector<unsigned char> a1;
  ev::take<ev::List<ev::Num<u8>>>(o, in, 1, a1);
  kern::jobs::done(std::move(a0), kern::Span<unsigned char const>(a1.data(), a1.size()));
  return true;
}

// engine.dot: kern::dot
bool c20(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec2>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec2>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::dot(std::move(a0), std::move(a1));
  ev::give<ev::Num<f32>>(o, out, 0, r);
  return true;
}

// engine.cross: kern::cross
bool c21(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec2>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec2>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::cross(std::move(a0), std::move(a1));
  ev::give<ev::Num<f32>>(o, out, 0, r);
  return true;
}

// engine.length: kern::length
bool c22(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec2>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::length(std::move(a0));
  ev::give<ev::Num<f32>>(o, out, 0, r);
  return true;
}

// engine.dot_2: kern::dot
bool c23(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::dot(std::move(a0), std::move(a1));
  ev::give<ev::Num<f32>>(o, out, 0, r);
  return true;
}

// engine.cross_2: kern::cross
bool c24(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::cross(std::move(a0), std::move(a1));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.length_2: kern::length
bool c25(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::length(std::move(a0));
  ev::give<ev::Num<f32>>(o, out, 0, r);
  return true;
}

// engine.length_sq: kern::length_sq
bool c26(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::length_sq(std::move(a0));
  ev::give<ev::Num<f32>>(o, out, 0, r);
  return true;
}

// engine.distance: kern::distance
bool c27(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::distance(std::move(a0), std::move(a1));
  ev::give<ev::Num<f32>>(o, out, 0, r);
  return true;
}

// engine.distance_sq: kern::distance_sq
bool c28(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::distance_sq(std::move(a0), std::move(a1));
  ev::give<ev::Num<f32>>(o, out, 0, r);
  return true;
}

// engine.finite: kern::finite
bool c29(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::finite(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.lerp: kern::lerp
bool c30(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 2, a2);
  decltype(auto) r = kern::lerp(std::move(a0), std::move(a1), std::move(a2));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.vmin: kern::vmin
bool c31(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::vmin(std::move(a0), std::move(a1));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.vmax: kern::vmax
bool c32(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::vmax(std::move(a0), std::move(a1));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.normalize: kern::normalize
bool c33(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  decltype(auto) r = kern::normalize(std::move(a0), std::move(a1));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.hex_color: kern::hex_color
bool c34(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<u32>>(o, in, 0, a0);
  decltype(auto) r = kern::hex_color(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.vec4.xyz: kern::Vec4::xyz
bool c35(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Vec4>();
  ev::take<ev::Struct<S_Vec4>>(o, in, 0, self);
  decltype(auto) r = self.xyz();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.quat.axis_angle: kern::Quat::axis_angle
bool c36(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 1, a1);
  decltype(auto) r = kern::Quat::axis_angle(std::move(a0), std::move(a1));
  ev::give<ev::Struct<S_Quat>>(o, out, 0, r);
  return true;
}

// engine.quat.rotate: kern::Quat::rotate
bool c37(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Quat>();
  ev::take<ev::Struct<S_Quat>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  decltype(auto) r = self.rotate(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.mat4.identity: kern::Mat4::identity
bool c38(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::Mat4::identity();
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

// engine.mat4.translation: kern::Mat4::translation
bool c39(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Mat4>();
  ev::take<ev::Struct<S_Mat4>>(o, in, 0, self);
  decltype(auto) r = self.translation();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.mat4.is_identity: kern::Mat4::is_identity
bool c40(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Mat4>();
  ev::take<ev::Struct<S_Mat4>>(o, in, 0, self);
  decltype(auto) r = self.is_identity();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.mat4.transform_point: kern::Mat4::transform_point
bool c41(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Mat4>();
  ev::take<ev::Struct<S_Mat4>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  decltype(auto) r = self.transform_point(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.mat4.transform_vector: kern::Mat4::transform_vector
bool c42(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Mat4>();
  ev::take<ev::Struct<S_Mat4>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  decltype(auto) r = self.transform_vector(std::move(a0));
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.mat4.transform: kern::Mat4::transform
bool c43(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Mat4>();
  ev::take<ev::Struct<S_Mat4>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec4>>();
  ev::take<ev::Struct<S_Vec4>>(o, in, 1, a0);
  decltype(auto) r = self.transform(std::move(a0));
  ev::give<ev::Struct<S_Vec4>>(o, out, 0, r);
  return true;
}

// engine.mat4.translate: kern::Mat4::translate
bool c44(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::Mat4::translate(std::move(a0));
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

// engine.mat4.scale: kern::Mat4::scale
bool c45(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  decltype(auto) r = kern::Mat4::scale(std::move(a0));
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

// engine.mat4.trs: kern::Mat4::trs
bool c46(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Quat>>();
  ev::take<ev::Struct<S_Quat>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a2);
  decltype(auto) r = kern::Mat4::trs(std::move(a0), std::move(a1), std::move(a2));
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

// engine.mat4.perspective: kern::Mat4::perspective
bool c47(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 3, a3);
  decltype(auto) r = kern::Mat4::perspective(std::move(a0), std::move(a1), std::move(a2), std::move(a3));
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

// engine.mat4.ortho: kern::Mat4::ortho
bool c48(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 3, a3);
  auto a4 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 4, a4);
  auto a5 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 5, a5);
  decltype(auto) r = kern::Mat4::ortho(std::move(a0), std::move(a1), std::move(a2), std::move(a3), std::move(a4), std::move(a5));
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

// engine.mat4.look_at: kern::Mat4::look_at
bool c49(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<bool&>>();
  ev::take<ev::Bool>(o, in, 3, a3);
  decltype(auto) r = kern::Mat4::look_at(std::move(a0), std::move(a1), std::move(a2), a3);
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  ev::give<ev::Bool>(o, out, 1, a3);
  return true;
}

// engine.mat4.look_at_2: kern::Mat4::look_at
bool c50(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 2, a2);
  decltype(auto) r = kern::Mat4::look_at(std::move(a0), std::move(a1), std::move(a2));
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

// engine.sphere.valid: kern::Sphere::valid
bool c51(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Sphere>();
  ev::take<ev::Struct<S_Sphere>>(o, in, 0, self);
  decltype(auto) r = self.valid();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.aabb.extend: kern::Aabb::extend
bool c52(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Aabb>();
  ev::take<ev::Struct<S_Aabb>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self.extend(std::move(a0));
  ev::give<ev::Struct<S_Aabb>>(o, out, 0, self);
  return true;
}

// engine.aabb.extend_2: kern::Aabb::extend
bool c53(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Aabb>();
  ev::take<ev::Struct<S_Aabb>>(o, in, 0, self);
  auto a0 = ev::blank<std::remove_cvref_t<kern::Aabb const&>>();
  ev::take<ev::Struct<S_Aabb>>(o, in, 1, a0);
  self.extend(a0);
  ev::give<ev::Struct<S_Aabb>>(o, out, 0, self);
  return true;
}

// engine.aabb.valid: kern::Aabb::valid
bool c54(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Aabb>();
  ev::take<ev::Struct<S_Aabb>>(o, in, 0, self);
  decltype(auto) r = self.valid();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.aabb.center: kern::Aabb::center
bool c55(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Aabb>();
  ev::take<ev::Struct<S_Aabb>>(o, in, 0, self);
  decltype(auto) r = self.center();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.aabb.extent: kern::Aabb::extent
bool c56(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto self = ev::blank<kern::Aabb>();
  ev::take<ev::Struct<S_Aabb>>(o, in, 0, self);
  decltype(auto) r = self.extent();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.camera_valid: kern::camera_valid
bool c57(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Camera const&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 0, a0);
  decltype(auto) r = kern::camera_valid(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.projection_matrix: kern::projection_matrix
bool c58(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Camera const&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<f32>>(o, in, 1, a1);
  decltype(auto) r = kern::projection_matrix(a0, std::move(a1));
  ev::give<ev::Struct<S_Mat4>>(o, out, 0, r);
  return true;
}

const EngineCall kCalls[] = {
    {"engine.fail", &c0},
    {"engine.last_error", &c1},
    {"engine.clear_error", &c2},
    {"engine.log", &c3},
    {"engine.now_ms", &c4},
    {"engine.wall_ms", &c5},
    {"engine.hash_u32", &c6},
    {"engine.jobs.submit", &c7},
    {"engine.jobs.take", &c8},
    {"engine.jobs.cancel", &c9},
    {"engine.jobs.pump", &c10},
    {"engine.jobs.pending", &c11},
    {"engine.jobs.set_external", &c12},
    {"engine.jobs.external", &c13},
    {"engine.jobs.set_threads", &c14},
    {"engine.jobs.threads", &c15},
    {"engine.jobs.reset", &c16},
    {"engine.jobs.take_queued", &c17},
    {"engine.jobs.run", &c18},
    {"engine.jobs.done", &c19},
    {"engine.dot", &c20},
    {"engine.cross", &c21},
    {"engine.length", &c22},
    {"engine.dot_2", &c23},
    {"engine.cross_2", &c24},
    {"engine.length_2", &c25},
    {"engine.length_sq", &c26},
    {"engine.distance", &c27},
    {"engine.distance_sq", &c28},
    {"engine.finite", &c29},
    {"engine.lerp", &c30},
    {"engine.vmin", &c31},
    {"engine.vmax", &c32},
    {"engine.normalize", &c33},
    {"engine.hex_color", &c34},
    {"engine.vec4.xyz", &c35},
    {"engine.quat.axis_angle", &c36},
    {"engine.quat.rotate", &c37},
    {"engine.mat4.identity", &c38},
    {"engine.mat4.translation", &c39},
    {"engine.mat4.is_identity", &c40},
    {"engine.mat4.transform_point", &c41},
    {"engine.mat4.transform_vector", &c42},
    {"engine.mat4.transform", &c43},
    {"engine.mat4.translate", &c44},
    {"engine.mat4.scale", &c45},
    {"engine.mat4.trs", &c46},
    {"engine.mat4.perspective", &c47},
    {"engine.mat4.ortho", &c48},
    {"engine.mat4.look_at", &c49},
    {"engine.mat4.look_at_2", &c50},
    {"engine.sphere.valid", &c51},
    {"engine.aabb.extend", &c52},
    {"engine.aabb.extend_2", &c53},
    {"engine.aabb.valid", &c54},
    {"engine.aabb.center", &c55},
    {"engine.aabb.extent", &c56},
    {"engine.camera_valid", &c57},
    {"engine.projection_matrix", &c58},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

}  // namespace
}  // namespace kern::logic
