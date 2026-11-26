// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/app/app.h"
#include "kern/app/project.h"
#include "kern/app/storage.h"
#include "kern/gfx/renderer.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct E_AoQuality { using type = kern::AoQuality; static constexpr type values[] = {kern::AoQuality::low, kern::AoQuality::medium, kern::AoQuality::high, kern::AoQuality::ultra}; };
struct E_BloomState { using type = kern::BloomState; static constexpr type values[] = {kern::BloomState::off, kern::BloomState::active, kern::BloomState::disabled_slow_device, kern::BloomState::unsupported}; };
struct E_Key { using type = kern::Key; static constexpr type values[] = {kern::Key::unknown, kern::Key::a, kern::Key::b, kern::Key::c, kern::Key::d, kern::Key::e, kern::Key::f, kern::Key::g, kern::Key::h, kern::Key::i, kern::Key::j, kern::Key::k, kern::Key::l, kern::Key::m, kern::Key::n, kern::Key::o, kern::Key::p, kern::Key::q, kern::Key::r, kern::Key::s, kern::Key::t, kern::Key::u, kern::Key::v, kern::Key::w, kern::Key::x, kern::Key::y, kern::Key::z, kern::Key::digit0, kern::Key::digit1, kern::Key::digit2, kern::Key::digit3, kern::Key::digit4, kern::Key::digit5, kern::Key::digit6, kern::Key::digit7, kern::Key::digit8, kern::Key::digit9, kern::Key::left, kern::Key::right, kern::Key::up, kern::Key::down, kern::Key::space, kern::Key::enter, kern::Key::escape, kern::Key::tab, kern::Key::backspace, kern::Key::shift_left, kern::Key::shift_right, kern::Key::control_left, kern::Key::control_right, kern::Key::alt_left, kern::Key::alt_right, kern::Key::count}; };
struct S_AoOptions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::AoOptions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::AoOptions& x); };
struct S_BloomOptions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::BloomOptions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::BloomOptions& x); };
struct S_Camera { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Camera& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Camera& x); };
struct S_Frame { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Frame& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Frame& x); };
struct S_Gamepad { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Gamepad& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Gamepad& x); };
struct S_Mat4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Mat4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Mat4& x); };
struct S_RenderStats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::RenderStats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::RenderStats& x); };
struct S_RendererOptions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::RendererOptions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::RendererOptions& x); };
struct S_ShadowOptions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::ShadowOptions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::ShadowOptions& x); };
struct S_Touch { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Touch& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Touch& x); };
struct S_Vec4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Vec4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Vec4& x); };
Value S_AoOptions::to(EngineObjects& o, const kern::AoOptions& x) { return Value::structure({ev::Conv<ev::Bool>::to(o, x.enabled), ev::Conv<ev::Enum<E_AoQuality>>::to(o, x.quality), ev::Conv<ev::Bool>::to(o, x.half_resolution), ev::Conv<ev::Num<::kern::f32>>::to(o, x.radius), ev::Conv<ev::Num<::kern::f32>>::to(o, x.power), ev::Conv<ev::Bool>::to(o, x.adaptive)}); }
void S_AoOptions::from(EngineObjects& o, const Value& v, kern::AoOptions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Bool>::from(o, *f, x.enabled); if (const Value* f = v.field(1)) ev::Conv<ev::Enum<E_AoQuality>>::from(o, *f, x.quality); if (const Value* f = v.field(2)) ev::Conv<ev::Bool>::from(o, *f, x.half_resolution); if (const Value* f = v.field(3)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.radius); if (const Value* f = v.field(4)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.power); if (const Value* f = v.field(5)) ev::Conv<ev::Bool>::from(o, *f, x.adaptive); }
Value S_BloomOptions::to(EngineObjects& o, const kern::BloomOptions& x) { return Value::structure({ev::Conv<ev::Bool>::to(o, x.enabled), ev::Conv<ev::Bool>::to(o, x.forced), ev::Conv<ev::Bool>::to(o, x.grade)}); }
void S_BloomOptions::from(EngineObjects& o, const Value& v, kern::BloomOptions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Bool>::from(o, *f, x.enabled); if (const Value* f = v.field(1)) ev::Conv<ev::Bool>::from(o, *f, x.forced); if (const Value* f = v.field(2)) ev::Conv<ev::Bool>::from(o, *f, x.grade); }
Value S_Camera::to(EngineObjects& o, const kern::Camera& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.eye), ev::Conv<ev::Vector>::to(o, x.target), ev::Conv<ev::Vector>::to(o, x.up), ev::Conv<ev::Num<::kern::f32>>::to(o, x.fov), ev::Conv<ev::Num<::kern::f32>>::to(o, x.near), ev::Conv<ev::Num<::kern::f32>>::to(o, x.far), ev::Conv<ev::Bool>::to(o, x.orthographic), ev::Conv<ev::Num<::kern::f32>>::to(o, x.ortho_height), ev::Conv<ev::Bool>::to(o, x.has_projection), ev::Conv<ev::Struct<S_Mat4>>::to(o, x.projection)}); }
void S_Camera::from(EngineObjects& o, const Value& v, kern::Camera& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.eye); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.target); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.up); if (const Value* f = v.field(3)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.fov); if (const Value* f = v.field(4)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.near); if (const Value* f = v.field(5)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.far); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.orthographic); if (const Value* f = v.field(7)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.ortho_height); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.has_projection); if (const Value* f = v.field(9)) ev::Conv<ev::Struct<S_Mat4>>::from(o, *f, x.projection); }
Value S_Frame::to(EngineObjects& o, const kern::Frame& x) { return Value::structure({ev::Conv<ev::Num<::kern::f64>>::to(o, x.time), ev::Conv<ev::Num<::kern::f32>>::to(o, x.dt), ev::Conv<ev::Num<::kern::u64>>::to(o, x.index)}); }
void S_Frame::from(EngineObjects& o, const Value& v, kern::Frame& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<::kern::f64>>::from(o, *f, x.time); if (const Value* f = v.field(1)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.dt); if (const Value* f = v.field(2)) ev::Conv<ev::Num<::kern::u64>>::from(o, *f, x.index); }
Value S_Gamepad::to(EngineObjects& o, const kern::Gamepad& x) { return Value::structure({ev::Conv<ev::Bool>::to(o, x.connected), ev::Conv<ev::Num<::kern::f32>>::to(o, x.lx), ev::Conv<ev::Num<::kern::f32>>::to(o, x.ly), ev::Conv<ev::Num<::kern::f32>>::to(o, x.rx), ev::Conv<ev::Num<::kern::f32>>::to(o, x.ry), ev::Conv<ev::Num<::kern::f32>>::to(o, x.lt), ev::Conv<ev::Num<::kern::f32>>::to(o, x.rt), ev::Conv<ev::Num<::kern::u32>>::to(o, x.buttons)}); }
void S_Gamepad::from(EngineObjects& o, const Value& v, kern::Gamepad& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Bool>::from(o, *f, x.connected); if (const Value* f = v.field(1)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.lx); if (const Value* f = v.field(2)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.ly); if (const Value* f = v.field(3)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.rx); if (const Value* f = v.field(4)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.ry); if (const Value* f = v.field(5)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.lt); if (const Value* f = v.field(6)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.rt); if (const Value* f = v.field(7)) ev::Conv<ev::Num<::kern::u32>>::from(o, *f, x.buttons); }
Value S_Mat4::to(EngineObjects& o, const kern::Mat4& x) { return Value::structure({ev::Conv<ev::List<ev::Num<::kern::f32>>>::to(o, x.m)}); }
void S_Mat4::from(EngineObjects& o, const Value& v, kern::Mat4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Num<::kern::f32>>>::from(o, *f, x.m); }
Value S_RenderStats::to(EngineObjects& o, const kern::RenderStats& x) { return Value::structure({ev::Conv<ev::Num<::kern::u32>>::to(o, x.draw_calls), ev::Conv<ev::Num<::kern::u32>>::to(o, x.shadow_draw_calls), ev::Conv<ev::Num<::kern::u32>>::to(o, x.shadow_resolution), ev::Conv<ev::Bool>::to(o, x.ao), ev::Conv<ev::Bool>::to(o, x.shadow_reused), ev::Conv<ev::Num<::kern::f64>>::to(o, x.triangles), ev::Conv<ev::Num<::kern::u32>>::to(o, x.instances), ev::Conv<ev::Num<::kern::u32>>::to(o, x.culled_meshes), ev::Conv<ev::Num<::kern::u32>>::to(o, x.visible_meshes), ev::Conv<ev::Num<::kern::f64>>::to(o, x.cpu_ms), ev::Conv<ev::Num<::kern::f64>>::to(o, x.gpu_ms), ev::Conv<ev::Enum<E_BloomState>>::to(o, x.bloom)}); }
void S_RenderStats::from(EngineObjects& o, const Value& v, kern::RenderStats& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<::kern::u32>>::from(o, *f, x.draw_calls); if (const Value* f = v.field(1)) ev::Conv<ev::Num<::kern::u32>>::from(o, *f, x.shadow_draw_calls); if (const Value* f = v.field(2)) ev::Conv<ev::Num<::kern::u32>>::from(o, *f, x.shadow_resolution); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.ao); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.shadow_reused); if (const Value* f = v.field(5)) ev::Conv<ev::Num<::kern::f64>>::from(o, *f, x.triangles); if (const Value* f = v.field(6)) ev::Conv<ev::Num<::kern::u32>>::from(o, *f, x.instances); if (const Value* f = v.field(7)) ev::Conv<ev::Num<::kern::u32>>::from(o, *f, x.culled_meshes); if (const Value* f = v.field(8)) ev::Conv<ev::Num<::kern::u32>>::from(o, *f, x.visible_meshes); if (const Value* f = v.field(9)) ev::Conv<ev::Num<::kern::f64>>::from(o, *f, x.cpu_ms); if (const Value* f = v.field(10)) ev::Conv<ev::Num<::kern::f64>>::from(o, *f, x.gpu_ms); if (const Value* f = v.field(11)) ev::Conv<ev::Enum<E_BloomState>>::from(o, *f, x.bloom); }
Value S_RendererOptions::to(EngineObjects& o, const kern::RendererOptions& x) { return Value::structure({ev::Conv<ev::Num<::kern::i32>>::to(o, x.width), ev::Conv<ev::Num<::kern::i32>>::to(o, x.height), ev::Conv<ev::Bool>::to(o, x.shadows), ev::Conv<ev::Struct<S_ShadowOptions>>::to(o, x.shadow), ev::Conv<ev::Struct<S_BloomOptions>>::to(o, x.bloom), ev::Conv<ev::Struct<S_AoOptions>>::to(o, x.ao), ev::Conv<ev::Bool>::to(o, x.grade)}); }
void S_RendererOptions::from(EngineObjects& o, const Value& v, kern::RendererOptions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<::kern::i32>>::from(o, *f, x.width); if (const Value* f = v.field(1)) ev::Conv<ev::Num<::kern::i32>>::from(o, *f, x.height); if (const Value* f = v.field(2)) ev::Conv<ev::Bool>::from(o, *f, x.shadows); if (const Value* f = v.field(3)) ev::Conv<ev::Struct<S_ShadowOptions>>::from(o, *f, x.shadow); if (const Value* f = v.field(4)) ev::Conv<ev::Struct<S_BloomOptions>>::from(o, *f, x.bloom); if (const Value* f = v.field(5)) ev::Conv<ev::Struct<S_AoOptions>>::from(o, *f, x.ao); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.grade); }
Value S_ShadowOptions::to(EngineObjects& o, const kern::ShadowOptions& x) { return Value::structure({ev::Conv<ev::Num<::kern::i32>>::to(o, x.resolution), ev::Conv<ev::Num<::kern::f32>>::to(o, x.extent), ev::Conv<ev::Num<::kern::f32>>::to(o, x.bias), ev::Conv<ev::Num<::kern::i32>>::to(o, x.filter)}); }
void S_ShadowOptions::from(EngineObjects& o, const Value& v, kern::ShadowOptions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<::kern::i32>>::from(o, *f, x.resolution); if (const Value* f = v.field(1)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.extent); if (const Value* f = v.field(2)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.bias); if (const Value* f = v.field(3)) ev::Conv<ev::Num<::kern::i32>>::from(o, *f, x.filter); }
Value S_Touch::to(EngineObjects& o, const kern::Touch& x) { return Value::structure({ev::Conv<ev::Num<::kern::i32>>::to(o, x.id), ev::Conv<ev::Vector>::to(o, x.start), ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Vector>::to(o, x.previous), ev::Conv<ev::Bool>::to(o, x.began)}); }
void S_Touch::from(EngineObjects& o, const Value& v, kern::Touch& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<::kern::i32>>::from(o, *f, x.id); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.start); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.previous); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.began); }
Value S_Vec4::to(EngineObjects& o, const kern::Vec4& x) { return Value::structure({ev::Conv<ev::Num<::kern::f32>>::to(o, x.x), ev::Conv<ev::Num<::kern::f32>>::to(o, x.y), ev::Conv<ev::Num<::kern::f32>>::to(o, x.z), ev::Conv<ev::Num<::kern::f32>>::to(o, x.w)}); }
void S_Vec4::from(EngineObjects& o, const Value& v, kern::Vec4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.x); if (const Value* f = v.field(1)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.y); if (const Value* f = v.field(2)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.z); if (const Value* f = v.field(3)) ev::Conv<ev::Num<::kern::f32>>::from(o, *f, x.w); }

// engine.input.down: kern::Input::down
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Key>>();
  ev::take<ev::Enum<E_Key>>(o, in, 1, a0);
  decltype(auto) r = self->down(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.input.pressed: kern::Input::pressed
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Key>>();
  ev::take<ev::Enum<E_Key>>(o, in, 1, a0);
  decltype(auto) r = self->pressed(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.input.released: kern::Input::released
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Key>>();
  ev::take<ev::Enum<E_Key>>(o, in, 1, a0);
  decltype(auto) r = self->released(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.input.pointer: kern::Input::pointer
bool c3(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  decltype(auto) r = self->pointer();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.input.pointer_delta: kern::Input::pointer_delta
bool c4(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  decltype(auto) r = self->pointer_delta();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.input.button: kern::Input::button
bool c5(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<::kern::u32>>(o, in, 1, a0);
  decltype(auto) r = self->button(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.input.button_pressed: kern::Input::button_pressed
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<::kern::u32>>(o, in, 1, a0);
  decltype(auto) r = self->button_pressed(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.input.wheel: kern::Input::wheel
bool c7(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  decltype(auto) r = self->wheel();
  ev::give<ev::Num<::kern::f32>>(o, out, 0, r);
  return true;
}

// engine.input.touches: kern::Input::touches
bool c8(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  decltype(auto) r = self->touches();
  ev::give<ev::List<ev::Struct<S_Touch>>>(o, out, 0, r);
  return true;
}

// engine.input.touch_count: kern::Input::touch_count
bool c9(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  decltype(auto) r = self->touch_count();
  ev::give<ev::Num<::kern::u32>>(o, out, 0, r);
  return true;
}

// engine.input.focused: kern::Input::focused
bool c10(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  decltype(auto) r = self->focused();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.input.gamepad: kern::Input::gamepad
bool c11(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  decltype(auto) r = self->gamepad();
  ev::give<ev::Struct<S_Gamepad>>(o, out, 0, r);
  return true;
}

// engine.input.key: kern::Input::key
bool c12(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Key>>();
  ev::take<ev::Enum<E_Key>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 2, a1);
  self->key(std::move(a0), std::move(a1));
  return true;
}

// engine.input.pointer_move: kern::Input::pointer_move
bool c13(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec2>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Vec2>>();
  ev::take<ev::Vector>(o, in, 2, a1);
  self->pointer_move(std::move(a0), std::move(a1));
  return true;
}

// engine.input.pointer_buttons: kern::Input::pointer_buttons
bool c14(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<::kern::u32>>(o, in, 1, a0);
  self->pointer_buttons(std::move(a0));
  return true;
}

// engine.input.add_wheel: kern::Input::add_wheel
bool c15(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 1, a0);
  self->add_wheel(std::move(a0));
  return true;
}

// engine.input.touch: kern::Input::touch
bool c16(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Num<::kern::i32>>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Num<::kern::i32>>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec2>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  self->touch(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.input.set_focus: kern::Input::set_focus
bool c17(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_focus(std::move(a0));
  return true;
}

// engine.input.set_gamepad: kern::Input::set_gamepad
bool c18(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Gamepad const&>>();
  ev::take<ev::Struct<S_Gamepad>>(o, in, 1, a0);
  self->set_gamepad(a0);
  return true;
}

// engine.input.end_frame: kern::Input::end_frame
bool c19(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  self->end_frame();
  return true;
}

// engine.app_host.pointer_lock: kern::AppHost::pointer_lock
bool c20(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AppHost>(o, in, 0, "app_host");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->pointer_lock(std::move(a0));
  return true;
}

// engine.app_host.widgets: kern::AppHost::widgets
bool c21(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::AppHost>(o, in, 0, "app_host");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  self->widgets(a0);
  return true;
}

// engine.platform_app_host: kern::platform_app_host
bool c22(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  decltype(auto) r = kern::platform_app_host();
  out[0] = Value::object(o.ref(ev::handle_of<kern::AppHost>(o, std::forward<decltype(r)>(r))));
  return true;
}

// engine.engine.host: kern::Engine::host
bool c23(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->host();
  out[0] = Value::object(o.ref(ev::handle_of<kern::AppHost>(o, std::forward<decltype(r)>(r))));
  return true;
}

// engine.engine.renderer: kern::Engine::renderer
bool c24(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->renderer();
  out[0] = Value::object(o.ref(ev::handle_of<kern::Renderer>(o, std::forward<decltype(r)>(r))));
  return true;
}

// engine.engine.input: kern::Engine::input
bool c25(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->input();
  out[0] = Value::object(o.ref(ev::handle_of<kern::Input>(o, std::forward<decltype(r)>(r))));
  return true;
}

// engine.engine.input_2: kern::Engine::input
bool c26(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->input();
  out[0] = Value::object(o.ref(ev::handle_of<kern::Input>(o, std::forward<decltype(r)>(r))));
  return true;
}

// engine.engine.profile: kern::Engine::profile
bool c27(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->profile();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.engine.set_profile: kern::Engine::set_profile
bool c28(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>>();
  ev::take<ev::Text>(o, in, 1, a0);
  self->set_profile(std::move(a0));
  return true;
}

// engine.engine.mixer: kern::Engine::mixer
bool c29(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->mixer();
  out[0] = Value::object(o.ref(ev::handle_of<kern::audio::Mixer>(o, std::forward<decltype(r)>(r))));
  return true;
}

// engine.engine.action: kern::Engine::action
bool c30(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->action(a0.c_str());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.action_pressed: kern::Engine::action_pressed
bool c31(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->action_pressed(a0.c_str());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.axis: kern::Engine::axis
bool c32(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->axis(a0.c_str());
  ev::give<ev::Num<::kern::f32>>(o, out, 0, r);
  return true;
}

// engine.engine.save_int: kern::Engine::save_int
bool c33(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::i64>>();
  ev::take<ev::Num<::kern::i64>>(o, in, 3, a2);
  decltype(auto) r = self->save_int(a0.c_str(), a1.c_str(), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.save_float: kern::Engine::save_float
bool c34(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<::kern::f64>>(o, in, 3, a2);
  decltype(auto) r = self->save_float(a0.c_str(), a1.c_str(), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.save_bool: kern::Engine::save_bool
bool c35(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 3, a2);
  decltype(auto) r = self->save_bool(a0.c_str(), a1.c_str(), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.save_text: kern::Engine::save_text
bool c36(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  std::string a2;
  ev::take<ev::Text>(o, in, 3, a2);
  decltype(auto) r = self->save_text(a0.c_str(), a1.c_str(), a2.c_str());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.load_int: kern::Engine::load_int
bool c37(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::i64>>();
  ev::take<ev::Num<::kern::i64>>(o, in, 3, a2);
  decltype(auto) r = self->load_int(a0.c_str(), a1.c_str(), std::move(a2));
  ev::give<ev::Num<::kern::i64>>(o, out, 0, r);
  return true;
}

// engine.engine.load_float: kern::Engine::load_float
bool c38(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<::kern::f64>>(o, in, 3, a2);
  decltype(auto) r = self->load_float(a0.c_str(), a1.c_str(), std::move(a2));
  ev::give<ev::Num<::kern::f64>>(o, out, 0, r);
  return true;
}

// engine.engine.load_bool: kern::Engine::load_bool
bool c39(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 3, a2);
  decltype(auto) r = self->load_bool(a0.c_str(), a1.c_str(), std::move(a2));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.load_text: kern::Engine::load_text
bool c40(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  std::string a2;
  ev::take<ev::Text>(o, in, 3, a2);
  decltype(auto) r = self->load_text(a0.c_str(), a1.c_str(), a2.c_str());
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.engine.saved: kern::Engine::saved
bool c41(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  decltype(auto) r = self->saved(a0.c_str(), a1.c_str());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.erase_saved: kern::Engine::erase_saved
bool c42(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->erase_saved(a0.c_str());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_storage: kern::Engine::set_storage
bool c43(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  kern::StorageHost* a0 = ev::object<kern::StorageHost>(o, in, 1, "host");
  if (!a0) return false;
  self->set_storage(*a0);
  return true;
}

// engine.engine.set_status: kern::Engine::set_status
bool c44(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  self->set_status(a0.c_str());
  return true;
}

// engine.engine.ui_text: kern::Engine::ui_text
bool c45(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 4, a3);
  self->ui_text(a0.c_str(), a1.c_str(), std::move(a2), std::move(a3));
  return true;
}

// engine.engine.ui_bar: kern::Engine::ui_bar
bool c46(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 4, a3);
  auto a4 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 5, a4);
  auto a5 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 6, a5);
  self->ui_bar(a0.c_str(), a1.c_str(), std::move(a2), std::move(a3), std::move(a4), std::move(a5));
  return true;
}

// engine.engine.ui_button: kern::Engine::ui_button
bool c47(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 4, a3);
  decltype(auto) r = self->ui_button(a0.c_str(), a1.c_str(), std::move(a2), std::move(a3));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.ui_menu: kern::Engine::ui_menu
bool c48(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 4, a3);
  decltype(auto) r = self->ui_menu(a0.c_str(), a1.c_str(), std::move(a2), std::move(a3));
  ev::give<ev::Num<::kern::i32>>(o, out, 0, r);
  return true;
}

// engine.engine.ui_json: kern::Engine::ui_json
bool c49(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->ui_json();
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.engine.ui_event: kern::Engine::ui_event
bool c50(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Num<::kern::i32>>(o, in, 2, a1);
  self->ui_event(a0, std::move(a1));
  return true;
}

// engine.engine.ui_begin: kern::Engine::ui_begin
bool c51(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  self->ui_begin();
  return true;
}

// engine.engine.ui_end: kern::Engine::ui_end
bool c52(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  self->ui_end();
  return true;
}

// engine.engine.request_pointer_lock: kern::Engine::request_pointer_lock
bool c53(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->request_pointer_lock(std::move(a0));
  return true;
}

// engine.engine.arg: kern::Engine::arg
bool c54(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  decltype(auto) r = self->arg(a0.c_str(), a1.c_str());
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.engine.has_arg: kern::Engine::has_arg
bool c55(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->has_arg(a0.c_str());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.arg_number: kern::Engine::arg_number
bool c56(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Num<::kern::f64>>(o, in, 2, a1);
  decltype(auto) r = self->arg_number(a0.c_str(), std::move(a1));
  ev::give<ev::Num<::kern::f64>>(o, out, 0, r);
  return true;
}

// engine.engine.set_args: kern::Engine::set_args
bool c57(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<::kern::u32>>(o, in, 2, a1);
  self->set_args(a0.c_str(), std::move(a1));
  return true;
}

// engine.engine.width: kern::Engine::width
bool c58(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->width();
  ev::give<ev::Num<::kern::i32>>(o, out, 0, r);
  return true;
}

// engine.engine.height: kern::Engine::height
bool c59(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->height();
  ev::give<ev::Num<::kern::i32>>(o, out, 0, r);
  return true;
}

// engine.engine.aspect: kern::Engine::aspect
bool c60(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->aspect();
  ev::give<ev::Num<::kern::f32>>(o, out, 0, r);
  return true;
}

// engine.engine.stats: kern::Engine::stats
bool c61(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->stats();
  ev::give<ev::Struct<S_RenderStats>>(o, out, 0, r);
  return true;
}

// engine.engine.clear_color: kern::Engine::clear_color
bool c62(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->clear_color();
  ev::give<ev::Struct<S_Vec4>>(o, out, 0, r);
  return true;
}

// engine.engine.set_clear_color: kern::Engine::set_clear_color
bool c63(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec4>>();
  ev::take<ev::Struct<S_Vec4>>(o, in, 1, a0);
  self->set_clear_color(std::move(a0));
  return true;
}

// engine.engine.xr: kern::Engine::xr
bool c64(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr: kern::Engine::set_xr
bool c65(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_xr(std::move(a0));
  return true;
}

// engine.engine.xr_eye: kern::Engine::xr_eye
bool c66(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_eye();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_eye: kern::Engine::set_xr_eye
bool c67(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->set_xr_eye(std::move(a0));
  return true;
}

// engine.engine.xr_forward: kern::Engine::xr_forward
bool c68(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_forward();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_forward: kern::Engine::set_xr_forward
bool c69(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->set_xr_forward(std::move(a0));
  return true;
}

// engine.engine.xr_room_forward: kern::Engine::xr_room_forward
bool c70(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_room_forward();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_room_forward: kern::Engine::set_xr_room_forward
bool c71(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->set_xr_room_forward(std::move(a0));
  return true;
}

// engine.engine.xr_head_yaw: kern::Engine::xr_head_yaw
bool c72(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_head_yaw();
  ev::give<ev::Num<::kern::f32>>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_head_yaw: kern::Engine::set_xr_head_yaw
bool c73(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 1, a0);
  self->set_xr_head_yaw(std::move(a0));
  return true;
}

// engine.engine.xr_follow_head: kern::Engine::xr_follow_head
bool c74(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_follow_head();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_follow_head: kern::Engine::set_xr_follow_head
bool c75(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_xr_follow_head(std::move(a0));
  return true;
}

// engine.engine.xr_hold_room: kern::Engine::xr_hold_room
bool c76(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_hold_room();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_hold_room: kern::Engine::set_xr_hold_room
bool c77(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_xr_hold_room(std::move(a0));
  return true;
}

// engine.engine.xr_full_room: kern::Engine::xr_full_room
bool c78(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_full_room();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_full_room: kern::Engine::set_xr_full_room
bool c79(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_xr_full_room(std::move(a0));
  return true;
}

// engine.engine.xr_room_up: kern::Engine::xr_room_up
bool c80(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_room_up();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_room_up: kern::Engine::set_xr_room_up
bool c81(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->set_xr_room_up(std::move(a0));
  return true;
}

// engine.engine.xr_vignette: kern::Engine::xr_vignette
bool c82(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_vignette();
  ev::give<ev::Num<::kern::f32>>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_vignette: kern::Engine::set_xr_vignette
bool c83(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 1, a0);
  self->set_xr_vignette(std::move(a0));
  return true;
}

// engine.engine.xr_vignette_mesh: kern::Engine::xr_vignette_mesh
bool c84(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_vignette_mesh();
  ev::give<ev::Num<::kern::u32>>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_vignette_mesh: kern::Engine::set_xr_vignette_mesh
bool c85(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<::kern::u32>>(o, in, 1, a0);
  self->set_xr_vignette_mesh(std::move(a0));
  return true;
}

// engine.engine.xr_scope: kern::Engine::xr_scope
bool c86(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_scope();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_scope: kern::Engine::set_xr_scope
bool c87(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_xr_scope(std::move(a0));
  return true;
}

// engine.engine.xr_scope_camera: kern::Engine::xr_scope_camera
bool c88(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_scope_camera();
  ev::give<ev::Struct<S_Camera>>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_scope_camera: kern::Engine::set_xr_scope_camera
bool c89(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Camera const&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 1, a0);
  self->set_xr_scope_camera(a0);
  return true;
}

// engine.engine.xr_scope_fbo: kern::Engine::xr_scope_fbo
bool c90(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_scope_fbo();
  ev::give<ev::Num<::kern::u32>>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_scope_fbo: kern::Engine::set_xr_scope_fbo
bool c91(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Num<::kern::u32>>(o, in, 1, a0);
  self->set_xr_scope_fbo(std::move(a0));
  return true;
}

// engine.engine.xr_scope_size: kern::Engine::xr_scope_size
bool c92(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_scope_size();
  ev::give<ev::Num<::kern::i32>>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_scope_size: kern::Engine::set_xr_scope_size
bool c93(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Num<::kern::i32>>(o, in, 1, a0);
  self->set_xr_scope_size(std::move(a0));
  return true;
}

// engine.engine.stereo: kern::Engine::stereo
bool c94(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->stereo();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_stereo: kern::Engine::set_stereo
bool c95(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_stereo(std::move(a0));
  return true;
}

// engine.engine.stereo_ipd: kern::Engine::stereo_ipd
bool c96(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->stereo_ipd();
  ev::give<ev::Num<::kern::f32>>(o, out, 0, r);
  return true;
}

// engine.engine.set_stereo_ipd: kern::Engine::set_stereo_ipd
bool c97(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 1, a0);
  self->set_stereo_ipd(std::move(a0));
  return true;
}

// engine.engine.stereo_cross: kern::Engine::stereo_cross
bool c98(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->stereo_cross();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_stereo_cross: kern::Engine::set_stereo_cross
bool c99(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_stereo_cross(std::move(a0));
  return true;
}

// engine.app.extensions: kern::App::extensions
bool c100(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::App>(o, in, 0, "app");
  if (!self) return false;
  decltype(auto) r = self->extensions();
  ev::give<ev::List<ev::Handle>>(o, out, 0, r);
  return true;
}

// engine.app.start: kern::App::start
bool c101(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::App>(o, in, 0, "app");
  if (!self) return false;
  kern::Engine* a0 = ev::object<kern::Engine>(o, in, 1, "engine");
  if (!a0) return false;
  decltype(auto) r = self->start(*a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.app.update: kern::App::update
bool c102(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::App>(o, in, 0, "app");
  if (!self) return false;
  kern::Engine* a0 = ev::object<kern::Engine>(o, in, 1, "engine");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::Frame const&>>();
  ev::take<ev::Struct<S_Frame>>(o, in, 2, a1);
  self->update(*a0, a1);
  return true;
}

// engine.app.resized: kern::App::resized
bool c103(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::App>(o, in, 0, "app");
  if (!self) return false;
  kern::Engine* a0 = ev::object<kern::Engine>(o, in, 1, "engine");
  if (!a0) return false;
  self->resized(*a0);
  return true;
}

// engine.app.stop: kern::App::stop
bool c104(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::App>(o, in, 0, "app");
  if (!self) return false;
  kern::Engine* a0 = ev::object<kern::Engine>(o, in, 1, "engine");
  if (!a0) return false;
  self->stop(*a0);
  return true;
}

// engine.project_json.num: kern::project_json::num
bool c105(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Num<::kern::f32>>(o, in, 0, a0);
  decltype(auto) r = kern::project_json::num(std::move(a0));
  out[0] = Value::object(o.ref(ev::handle_of<kern::json::Value>(o, std::forward<decltype(r)>(r))));
  return true;
}

// engine.project_json.quality_name: kern::project_json::quality_name
bool c106(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::AoQuality>>();
  ev::take<ev::Enum<E_AoQuality>>(o, in, 0, a0);
  decltype(auto) r = kern::project_json::quality_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.project_json.quality_of: kern::project_json::quality_of
bool c107(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::AoQuality>>();
  ev::take<ev::Enum<E_AoQuality>>(o, in, 1, a1);
  decltype(auto) r = kern::project_json::quality_of(a0, std::move(a1));
  ev::give<ev::Enum<E_AoQuality>>(o, out, 0, r);
  return true;
}

// engine.project_json.shadow_to: kern::project_json::shadow_to
bool c108(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::ShadowOptions const&>>();
  ev::take<ev::Struct<S_ShadowOptions>>(o, in, 0, a0);
  decltype(auto) r = kern::project_json::shadow_to(a0);
  out[0] = Value::object(o.ref(ev::handle_of<kern::json::Value>(o, std::forward<decltype(r)>(r))));
  return true;
}

// engine.project_json.shadow_from: kern::project_json::shadow_from
bool c109(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::json::Value* a0 = ev::object<kern::json::Value>(o, in, 0, "v");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::ShadowOptions&>>();
  ev::take<ev::Struct<S_ShadowOptions>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 2, a2);
  decltype(auto) r = kern::project_json::shadow_from(*a0, a1, a2);
  ev::give<ev::Text>(o, out, 0, r);
  ev::give<ev::Struct<S_ShadowOptions>>(o, out, 1, a1);
  return true;
}

// engine.project_json.ao_to: kern::project_json::ao_to
bool c110(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::AoOptions const&>>();
  ev::take<ev::Struct<S_AoOptions>>(o, in, 0, a0);
  decltype(auto) r = kern::project_json::ao_to(a0);
  out[0] = Value::object(o.ref(ev::handle_of<kern::json::Value>(o, std::forward<decltype(r)>(r))));
  return true;
}

// engine.project_json.ao_from: kern::project_json::ao_from
bool c111(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::json::Value* a0 = ev::object<kern::json::Value>(o, in, 0, "v");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::AoOptions&>>();
  ev::take<ev::Struct<S_AoOptions>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 2, a2);
  decltype(auto) r = kern::project_json::ao_from(*a0, a1, a2);
  ev::give<ev::Text>(o, out, 0, r);
  ev::give<ev::Struct<S_AoOptions>>(o, out, 1, a1);
  return true;
}

// engine.project_json.renderer_to: kern::project_json::renderer_to
bool c112(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::RendererOptions const&>>();
  ev::take<ev::Struct<S_RendererOptions>>(o, in, 0, a0);
  decltype(auto) r = kern::project_json::renderer_to(a0);
  out[0] = Value::object(o.ref(ev::handle_of<kern::json::Value>(o, std::forward<decltype(r)>(r))));
  return true;
}

// engine.project_json.renderer_from: kern::project_json::renderer_from
bool c113(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::json::Value* a0 = ev::object<kern::json::Value>(o, in, 0, "r");
  if (!a0) return false;
  auto a1 = ev::blank<std::remove_cvref_t<kern::RendererOptions&>>();
  ev::take<ev::Struct<S_RendererOptions>>(o, in, 1, a1);
  auto a2 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> > const&>>();
  ev::take<ev::Text>(o, in, 2, a2);
  decltype(auto) r = kern::project_json::renderer_from(*a0, a1, a2);
  ev::give<ev::Text>(o, out, 0, r);
  ev::give<ev::Struct<S_RendererOptions>>(o, out, 1, a1);
  return true;
}

// engine.storage_host.read: kern::StorageHost::read
bool c114(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::StorageHost>(o, in, 0, "storage_host");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>>();
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<bool&>>();
  ev::take<ev::Bool>(o, in, 3, a2);
  auto a3 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>>();
  ev::take<ev::Text>(o, in, 4, a3);
  decltype(auto) r = self->read(a0, a1, a2, a3);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Text>(o, out, 1, a1);
  ev::give<ev::Bool>(o, out, 2, a2);
  ev::give<ev::Text>(o, out, 3, a3);
  return true;
}

// engine.storage_host.write: kern::StorageHost::write
bool c115(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::StorageHost>(o, in, 0, "storage_host");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  std::string a1;
  ev::take<ev::Text>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>>();
  ev::take<ev::Text>(o, in, 3, a2);
  decltype(auto) r = self->write(a0, a1, a2);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Text>(o, out, 1, a2);
  return true;
}

// engine.storage_host.remove: kern::StorageHost::remove
bool c116(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::StorageHost>(o, in, 0, "storage_host");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>>();
  ev::take<ev::Text>(o, in, 2, a1);
  decltype(auto) r = self->remove(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Text>(o, out, 1, a1);
  return true;
}

// engine.platform_storage: kern::platform_storage
bool c117(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<std::basic_string<char, std::char_traits<char>, std::allocator<char> >>>();
  ev::take<ev::Text>(o, in, 0, a0);
  decltype(auto) r = kern::platform_storage(std::move(a0));
  out[0] = Value::object(o.ref(ev::handle_of<kern::StorageHost>(o, std::forward<decltype(r)>(r))));
  return true;
}

// engine.storage_name_ok: kern::storage_name_ok
bool c118(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  decltype(auto) r = kern::storage_name_ok(a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.device_profile.make: kern::DeviceProfile::DeviceProfile
bool c119(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::object(o.ref(o.own<kern::DeviceProfile>(std::unique_ptr<kern::DeviceProfile>(new kern::DeviceProfile()))));
  return true;
}

// engine.device_profile.release: kern::DeviceProfile::~DeviceProfile
bool c120(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  ObjectRef r;
  if (!ev::object_ref(o, in, 0, "device_profile", r)) return false;
  return ev::release_of<kern::DeviceProfile>(o, r, "device_profile", "DeviceProfile");
}

// engine.device_profile.name: kern::DeviceProfile::name
bool c121(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::DeviceProfile>(o, in, 0, "device_profile");
  if (!self) return false;
  ev::give<ev::Text>(o, out, 0, self->name);
  return true;
}

// engine.device_profile.name@set: what stands at the place kern::DeviceProfile::name gives
bool c121_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::DeviceProfile>(o, in, 0, "device_profile");
  if (!self) return false;
  ev::take<ev::Text>(o, in, 1, self->name);
  return true;
}

// engine.device_profile.renderer: kern::DeviceProfile::renderer
bool c122(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::DeviceProfile>(o, in, 0, "device_profile");
  if (!self) return false;
  ev::give<ev::Struct<S_RendererOptions>>(o, out, 0, self->renderer);
  return true;
}

// engine.device_profile.renderer@set: what stands at the place kern::DeviceProfile::renderer gives
bool c122_set(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>&) {
  auto* self = ev::object<kern::DeviceProfile>(o, in, 0, "device_profile");
  if (!self) return false;
  ev::take<ev::Struct<S_RendererOptions>>(o, in, 1, self->renderer);
  return true;
}

// engine.engine.make: kern::Engine::Engine
bool c123(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::Renderer* a0 = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!a0) return false;
  out[0] = Value::object(o.ref(o.own<kern::Engine>(std::unique_ptr<kern::Engine>(new kern::Engine(*a0)))));
  return true;
}

// engine.engine.make_2: kern::Engine::Engine
bool c124(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::Renderer* a0 = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!a0) return false;
  kern::AppHost* a1 = ev::object<kern::AppHost>(o, in, 1, "host");
  if (!a1) return false;
  out[0] = Value::object(o.ref(o.own<kern::Engine>(std::unique_ptr<kern::Engine>(new kern::Engine(*a0, *a1)))));
  return true;
}

// engine.engine.release: kern::Engine::~Engine
bool c125(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  ObjectRef r;
  if (!ev::object_ref(o, in, 0, "engine", r)) return false;
  return ev::release_of<kern::Engine>(o, r, "engine", "Engine");
}

// engine.input.make: kern::Input::Input
bool c126(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::object(o.ref(o.own<kern::Input>(std::unique_ptr<kern::Input>(new kern::Input()))));
  return true;
}

// engine.input.release: kern::Input::~Input
bool c127(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  ObjectRef r;
  if (!ev::object_ref(o, in, 0, "input", r)) return false;
  return ev::release_of<kern::Input>(o, r, "input", "Input");
}

// kern::AppHost: the object of the engine a Blueprint is (Graph::implements "AppHost")
class Implements_AppHost final : public kern::AppHost {
 public:
  Implements_AppHost(EngineObjects& o, std::shared_ptr<Implementer> by) : o_(o), by_(std::move(by)) {}
  decltype(std::declval<kern::AppHost&>().pointer_lock(std::declval<bool>())) pointer_lock(bool kern_p0) override {
    const std::vector<Value> kern_in{ev::Conv<ev::Bool>::to(o_, kern_p0)};
    std::vector<Value> kern_out;
    by_->call("pointer_lock", kern_in, kern_out);
  }
  decltype(std::declval<kern::AppHost&>().widgets(std::declval<std::basic_string_view<char, std::char_traits<char> >>())) widgets(std::basic_string_view<char, std::char_traits<char> > kern_p0) override {
    const std::vector<Value> kern_in{ev::Conv<ev::Text>::to(o_, kern_p0)};
    std::vector<Value> kern_out;
    by_->call("widgets", kern_in, kern_out);
  }

 private:
  EngineObjects& o_;
  std::shared_ptr<Implementer> by_;
};
i64 implements_AppHost(EngineObjects& o, std::shared_ptr<Implementer> by) {
  return o.own<Implements_AppHost, kern::AppHost>(std::make_unique<Implements_AppHost>(o, std::move(by)));
}

// kern::App: the object of the engine a Blueprint is (Graph::implements "App")
class Implements_App final : public kern::App {
 public:
  Implements_App(EngineObjects& o, std::shared_ptr<Implementer> by) : o_(o), by_(std::move(by)) {}
  decltype(std::declval<kern::App&>().extensions()) extensions() override {
    if (!by_->has("extensions")) return kern::App::extensions();
    const std::vector<Value> kern_in;
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("extensions", kern_in, kern_out) && !kern_out.empty();
    kern_kept_extensions.clear();
    if (kern_given) ev::Conv<ev::List<ev::Handle>>::from(o_, kern_out[0], kern_kept_extensions);
    return decltype(std::declval<kern::App&>().extensions())(kern_kept_extensions.data(), kern_kept_extensions.size());
  }
  decltype(std::declval<kern::App&>().start(std::declval<kern::Engine&>())) start(kern::Engine& kern_p0) override {
    const std::vector<Value> kern_in{Value::object(o_.ref(o_.adopt<kern::Engine>(kern_p0)))};
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("start", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<kern::App&>().start(std::declval<kern::Engine&>())) kern_r = ev::blank<decltype(std::declval<kern::App&>().start(std::declval<kern::Engine&>()))>();
    if (kern_given) ev::Conv<ev::Bool>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<kern::App&>().update(std::declval<kern::Engine&>(), std::declval<kern::Frame const&>())) update(kern::Engine& kern_p0, kern::Frame const& kern_p1) override {
    const std::vector<Value> kern_in{Value::object(o_.ref(o_.adopt<kern::Engine>(kern_p0))), ev::Conv<ev::Struct<S_Frame>>::to(o_, kern_p1)};
    std::vector<Value> kern_out;
    by_->call("update", kern_in, kern_out);
  }
  decltype(std::declval<kern::App&>().resized(std::declval<kern::Engine&>())) resized(kern::Engine& kern_p0) override {
    if (!by_->has("resized")) return kern::App::resized(kern_p0);
    const std::vector<Value> kern_in{Value::object(o_.ref(o_.adopt<kern::Engine>(kern_p0)))};
    std::vector<Value> kern_out;
    by_->call("resized", kern_in, kern_out);
  }
  decltype(std::declval<kern::App&>().stop(std::declval<kern::Engine&>())) stop(kern::Engine& kern_p0) override {
    if (!by_->has("stop")) return kern::App::stop(kern_p0);
    const std::vector<Value> kern_in{Value::object(o_.ref(o_.adopt<kern::Engine>(kern_p0)))};
    std::vector<Value> kern_out;
    by_->call("stop", kern_in, kern_out);
  }

 private:
  EngineObjects& o_;
  std::shared_ptr<Implementer> by_;
  mutable std::vector<std::remove_cv_t<std::remove_pointer_t<decltype(std::declval<decltype(std::declval<kern::App&>().extensions())>().data)>>> kern_kept_extensions;
};
i64 implements_App(EngineObjects& o, std::shared_ptr<Implementer> by) {
  return o.own<Implements_App, kern::App>(std::make_unique<Implements_App>(o, std::move(by)));
}

// kern::StorageHost: the object of the engine a Blueprint is (Graph::implements "StorageHost")
class Implements_StorageHost final : public kern::StorageHost {
 public:
  Implements_StorageHost(EngineObjects& o, std::shared_ptr<Implementer> by) : o_(o), by_(std::move(by)) {}
  decltype(std::declval<kern::StorageHost&>().read(std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>(), std::declval<bool&>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>())) read(std::basic_string_view<char, std::char_traits<char> > kern_p0, std::basic_string<char, std::char_traits<char>, std::allocator<char> >& kern_p1, bool& kern_p2, std::basic_string<char, std::char_traits<char>, std::allocator<char> >& kern_p3) override {
    const std::vector<Value> kern_in{ev::Conv<ev::Text>::to(o_, kern_p0), ev::Conv<ev::Text>::to(o_, kern_p1), ev::Conv<ev::Bool>::to(o_, kern_p2), ev::Conv<ev::Text>::to(o_, kern_p3)};
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("read", kern_in, kern_out) && !kern_out.empty();
    if (kern_given && kern_out.size() > 1) ev::Conv<ev::Text>::from(o_, kern_out[1], kern_p1);
    if (kern_given && kern_out.size() > 2) ev::Conv<ev::Bool>::from(o_, kern_out[2], kern_p2);
    if (kern_given && kern_out.size() > 3) ev::Conv<ev::Text>::from(o_, kern_out[3], kern_p3);
    decltype(std::declval<kern::StorageHost&>().read(std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>(), std::declval<bool&>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>())) kern_r = ev::blank<decltype(std::declval<kern::StorageHost&>().read(std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>(), std::declval<bool&>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>()))>();
    if (kern_given) ev::Conv<ev::Bool>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<kern::StorageHost&>().write(std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>())) write(std::basic_string_view<char, std::char_traits<char> > kern_p0, std::basic_string_view<char, std::char_traits<char> > kern_p1, std::basic_string<char, std::char_traits<char>, std::allocator<char> >& kern_p2) override {
    const std::vector<Value> kern_in{ev::Conv<ev::Text>::to(o_, kern_p0), ev::Conv<ev::Text>::to(o_, kern_p1), ev::Conv<ev::Text>::to(o_, kern_p2)};
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("write", kern_in, kern_out) && !kern_out.empty();
    if (kern_given && kern_out.size() > 1) ev::Conv<ev::Text>::from(o_, kern_out[1], kern_p2);
    decltype(std::declval<kern::StorageHost&>().write(std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>())) kern_r = ev::blank<decltype(std::declval<kern::StorageHost&>().write(std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>()))>();
    if (kern_given) ev::Conv<ev::Bool>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<kern::StorageHost&>().remove(std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>())) remove(std::basic_string_view<char, std::char_traits<char> > kern_p0, std::basic_string<char, std::char_traits<char>, std::allocator<char> >& kern_p1) override {
    const std::vector<Value> kern_in{ev::Conv<ev::Text>::to(o_, kern_p0), ev::Conv<ev::Text>::to(o_, kern_p1)};
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("remove", kern_in, kern_out) && !kern_out.empty();
    if (kern_given && kern_out.size() > 1) ev::Conv<ev::Text>::from(o_, kern_out[1], kern_p1);
    decltype(std::declval<kern::StorageHost&>().remove(std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>())) kern_r = ev::blank<decltype(std::declval<kern::StorageHost&>().remove(std::declval<std::basic_string_view<char, std::char_traits<char> >>(), std::declval<std::basic_string<char, std::char_traits<char>, std::allocator<char> >&>()))>();
    if (kern_given) ev::Conv<ev::Bool>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }

 private:
  EngineObjects& o_;
  std::shared_ptr<Implementer> by_;
};
i64 implements_StorageHost(EngineObjects& o, std::shared_ptr<Implementer> by) {
  return o.own<Implements_StorageHost, kern::StorageHost>(std::make_unique<Implements_StorageHost>(o, std::move(by)));
}

const EngineCall kCalls[] = {
    {"engine.input.down", &c0},
    {"engine.input.pressed", &c1},
    {"engine.input.released", &c2},
    {"engine.input.pointer", &c3},
    {"engine.input.pointer_delta", &c4},
    {"engine.input.button", &c5},
    {"engine.input.button_pressed", &c6},
    {"engine.input.wheel", &c7},
    {"engine.input.touches", &c8},
    {"engine.input.touch_count", &c9},
    {"engine.input.focused", &c10},
    {"engine.input.gamepad", &c11},
    {"engine.input.key", &c12},
    {"engine.input.pointer_move", &c13},
    {"engine.input.pointer_buttons", &c14},
    {"engine.input.add_wheel", &c15},
    {"engine.input.touch", &c16},
    {"engine.input.set_focus", &c17},
    {"engine.input.set_gamepad", &c18},
    {"engine.input.end_frame", &c19},
    {"engine.app_host.pointer_lock", &c20},
    {"engine.app_host.widgets", &c21},
    {"engine.platform_app_host", &c22},
    {"engine.engine.host", &c23},
    {"engine.engine.renderer", &c24},
    {"engine.engine.input", &c25},
    {"engine.engine.input_2", &c26},
    {"engine.engine.profile", &c27},
    {"engine.engine.set_profile", &c28},
    {"engine.engine.mixer", &c29},
    {"engine.engine.action", &c30},
    {"engine.engine.action_pressed", &c31},
    {"engine.engine.axis", &c32},
    {"engine.engine.save_int", &c33},
    {"engine.engine.save_float", &c34},
    {"engine.engine.save_bool", &c35},
    {"engine.engine.save_text", &c36},
    {"engine.engine.load_int", &c37},
    {"engine.engine.load_float", &c38},
    {"engine.engine.load_bool", &c39},
    {"engine.engine.load_text", &c40},
    {"engine.engine.saved", &c41},
    {"engine.engine.erase_saved", &c42},
    {"engine.engine.set_storage", &c43},
    {"engine.engine.set_status", &c44},
    {"engine.engine.ui_text", &c45},
    {"engine.engine.ui_bar", &c46},
    {"engine.engine.ui_button", &c47},
    {"engine.engine.ui_menu", &c48},
    {"engine.engine.ui_json", &c49},
    {"engine.engine.ui_event", &c50},
    {"engine.engine.ui_begin", &c51},
    {"engine.engine.ui_end", &c52},
    {"engine.engine.request_pointer_lock", &c53},
    {"engine.engine.arg", &c54},
    {"engine.engine.has_arg", &c55},
    {"engine.engine.arg_number", &c56},
    {"engine.engine.set_args", &c57},
    {"engine.engine.width", &c58},
    {"engine.engine.height", &c59},
    {"engine.engine.aspect", &c60},
    {"engine.engine.stats", &c61},
    {"engine.engine.clear_color", &c62},
    {"engine.engine.set_clear_color", &c63},
    {"engine.engine.xr", &c64},
    {"engine.engine.set_xr", &c65},
    {"engine.engine.xr_eye", &c66},
    {"engine.engine.set_xr_eye", &c67},
    {"engine.engine.xr_forward", &c68},
    {"engine.engine.set_xr_forward", &c69},
    {"engine.engine.xr_room_forward", &c70},
    {"engine.engine.set_xr_room_forward", &c71},
    {"engine.engine.xr_head_yaw", &c72},
    {"engine.engine.set_xr_head_yaw", &c73},
    {"engine.engine.xr_follow_head", &c74},
    {"engine.engine.set_xr_follow_head", &c75},
    {"engine.engine.xr_hold_room", &c76},
    {"engine.engine.set_xr_hold_room", &c77},
    {"engine.engine.xr_full_room", &c78},
    {"engine.engine.set_xr_full_room", &c79},
    {"engine.engine.xr_room_up", &c80},
    {"engine.engine.set_xr_room_up", &c81},
    {"engine.engine.xr_vignette", &c82},
    {"engine.engine.set_xr_vignette", &c83},
    {"engine.engine.xr_vignette_mesh", &c84},
    {"engine.engine.set_xr_vignette_mesh", &c85},
    {"engine.engine.xr_scope", &c86},
    {"engine.engine.set_xr_scope", &c87},
    {"engine.engine.xr_scope_camera", &c88},
    {"engine.engine.set_xr_scope_camera", &c89},
    {"engine.engine.xr_scope_fbo", &c90},
    {"engine.engine.set_xr_scope_fbo", &c91},
    {"engine.engine.xr_scope_size", &c92},
    {"engine.engine.set_xr_scope_size", &c93},
    {"engine.engine.stereo", &c94},
    {"engine.engine.set_stereo", &c95},
    {"engine.engine.stereo_ipd", &c96},
    {"engine.engine.set_stereo_ipd", &c97},
    {"engine.engine.stereo_cross", &c98},
    {"engine.engine.set_stereo_cross", &c99},
    {"engine.app.extensions", &c100},
    {"engine.app.start", &c101},
    {"engine.app.update", &c102},
    {"engine.app.resized", &c103},
    {"engine.app.stop", &c104},
    {"engine.project_json.num", &c105},
    {"engine.project_json.quality_name", &c106},
    {"engine.project_json.quality_of", &c107},
    {"engine.project_json.shadow_to", &c108},
    {"engine.project_json.shadow_from", &c109},
    {"engine.project_json.ao_to", &c110},
    {"engine.project_json.ao_from", &c111},
    {"engine.project_json.renderer_to", &c112},
    {"engine.project_json.renderer_from", &c113},
    {"engine.storage_host.read", &c114},
    {"engine.storage_host.write", &c115},
    {"engine.storage_host.remove", &c116},
    {"engine.platform_storage", &c117},
    {"engine.storage_name_ok", &c118},
    {"engine.device_profile.make", &c119},
    {"engine.device_profile.release", &c120},
    {"engine.device_profile.name", &c121},
    {"engine.device_profile.renderer", &c122},
    {"engine.engine.make", &c123},
    {"engine.engine.make_2", &c124},
    {"engine.engine.release", &c125},
    {"engine.input.make", &c126},
    {"engine.input.release", &c127},
    {"engine.device_profile.name@get", &c121},
    {"engine.device_profile.name@set", &c121_set},
    {"engine.device_profile.renderer@get", &c122},
    {"engine.device_profile.renderer@set", &c122_set},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

const EngineMaker kMakers[] = {
    {EngineObjects::kind<kern::DeviceProfile>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::DeviceProfile>(handle, std::unique_ptr<kern::DeviceProfile>(new kern::DeviceProfile())); }},
    {EngineObjects::kind<kern::Engine>(), [](EngineObjects& o, i64 handle) -> void* { auto made = std::make_unique<ev::MadeWith<kern::Engine>>(); auto& d0 = made->keep(new kern::Renderer()); made->object.emplace(d0); return o.put_made<kern::Engine>(handle, std::move(made)); }},
    {EngineObjects::kind<kern::Input>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::Input>(handle, std::unique_ptr<kern::Input>(new kern::Input())); }},
};
const EngineMakers kAllMakers(kMakers, std::size(kMakers));

const EngineImplement kAdapters[] = {
    {"AppHost", &implements_AppHost},
    {"App", &implements_App},
    {"StorageHost", &implements_StorageHost},
};
const EngineImplements kAllAdapters(kAdapters, std::size(kAdapters));

}  // namespace
}  // namespace kern::logic
