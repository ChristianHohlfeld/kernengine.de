// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/scene/bvh.h"
#include "kern/scene/picking.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct S_Camera { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Camera& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Camera& x); };
struct S_Mat4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Mat4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Mat4& x); };
struct S_Ray { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Ray& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Ray& x); };
Value S_Camera::to(EngineObjects& o, const kern::Camera& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.eye), ev::Conv<ev::Vector>::to(o, x.target), ev::Conv<ev::Vector>::to(o, x.up), ev::Conv<ev::Float>::to(o, x.fov), ev::Conv<ev::Float>::to(o, x.near), ev::Conv<ev::Float>::to(o, x.far), ev::Conv<ev::Bool>::to(o, x.orthographic), ev::Conv<ev::Float>::to(o, x.ortho_height), ev::Conv<ev::Bool>::to(o, x.has_projection), ev::Conv<ev::Struct<S_Mat4>>::to(o, x.projection)}); }
void S_Camera::from(EngineObjects& o, const Value& v, kern::Camera& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.eye); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.target); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.up); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.fov); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.near); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.far); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.orthographic); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.ortho_height); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.has_projection); if (const Value* f = v.field(9)) ev::Conv<ev::Struct<S_Mat4>>::from(o, *f, x.projection); }
Value S_Mat4::to(EngineObjects& o, const kern::Mat4& x) { return Value::structure({ev::Conv<ev::List<ev::Float>>::to(o, x.m)}); }
void S_Mat4::from(EngineObjects& o, const Value& v, kern::Mat4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.m); }
Value S_Ray::to(EngineObjects& o, const kern::Ray& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.origin), ev::Conv<ev::Vector>::to(o, x.direction), ev::Conv<ev::Float>::to(o, x.near), ev::Conv<ev::Float>::to(o, x.far)}); }
void S_Ray::from(EngineObjects& o, const Value& v, kern::Ray& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.origin); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.direction); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.near); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.far); }

// engine.bvh.build: kern::Bvh::build
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Bvh>(o, in, 0, "bvh");
  if (!self) return false;
  std::vector<float> a0;
  ev::take<ev::List<ev::Float>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Count<3u>>>();
  ev::take<ev::Int>(o, in, 2, a1);
  std::vector<unsigned int> a2;
  ev::take<ev::List<ev::Int>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<kern::Count<1u>>>();
  ev::take<ev::Int>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 5, a4);
  if (ev::numbers(a1) > a0.size()) { o.error = "the list positions has fewer entries than vertex_count says"; return false; }
  if (ev::numbers(a3) > a2.size()) { o.error = "the list indices has fewer entries than index_count says"; return false; }
  self->build(a0.empty() ? nullptr : a0.data(), std::move(a1), a2.empty() ? nullptr : a2.data(), std::move(a3), std::move(a4));
  return true;
}

// engine.bvh.empty: kern::Bvh::empty
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Bvh>(o, in, 0, "bvh");
  if (!self) return false;
  decltype(auto) r = self->empty();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.bvh.triangle_count: kern::Bvh::triangle_count
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Bvh>(o, in, 0, "bvh");
  if (!self) return false;
  decltype(auto) r = self->triangle_count();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.screen_ray: kern::screen_ray
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Camera const&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 2, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 3, a3);
  auto a4 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 4, a4);
  auto a5 = ev::blank<std::remove_cvref_t<kern::Ray&>>();
  ev::take<ev::Struct<S_Ray>>(o, in, 5, a5);
  decltype(auto) r = kern::screen_ray(a0, std::move(a1), std::move(a2), std::move(a3), std::move(a4), a5);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Struct<S_Ray>>(o, out, 1, a5);
  return true;
}

// engine.bvh.make: kern::Bvh::Bvh
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::object(o.ref(o.own<kern::Bvh>(std::unique_ptr<kern::Bvh>(new kern::Bvh()))));
  return true;
}

// engine.bvh.release: kern::Bvh::~Bvh
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  ObjectRef r;
  if (!ev::object_ref(o, in, 0, "bvh", r)) return false;
  return ev::release_of<kern::Bvh>(o, r, "bvh", "Bvh");
}

const EngineCall kCalls[] = {
    {"engine.bvh.build", &c0},
    {"engine.bvh.empty", &c1},
    {"engine.bvh.triangle_count", &c2},
    {"engine.screen_ray", &c3},
    {"engine.bvh.make", &c4},
    {"engine.bvh.release", &c5},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

const EngineMaker kMakers[] = {
    {EngineObjects::kind<kern::Bvh>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::Bvh>(handle, std::unique_ptr<kern::Bvh>(new kern::Bvh())); }},
};
const EngineMakers kAllMakers(kMakers, std::size(kMakers));

}  // namespace
}  // namespace kern::logic
