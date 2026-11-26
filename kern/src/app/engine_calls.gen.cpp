// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/app/app.h"
#include "kern/gfx/renderer.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct E_BloomState { using type = kern::BloomState; static constexpr type values[] = {kern::BloomState::off, kern::BloomState::active, kern::BloomState::disabled_slow_device, kern::BloomState::unsupported}; };
struct E_Key { using type = kern::Key; static constexpr type values[] = {kern::Key::unknown, kern::Key::a, kern::Key::b, kern::Key::c, kern::Key::d, kern::Key::e, kern::Key::f, kern::Key::g, kern::Key::h, kern::Key::i, kern::Key::j, kern::Key::k, kern::Key::l, kern::Key::m, kern::Key::n, kern::Key::o, kern::Key::p, kern::Key::q, kern::Key::r, kern::Key::s, kern::Key::t, kern::Key::u, kern::Key::v, kern::Key::w, kern::Key::x, kern::Key::y, kern::Key::z, kern::Key::digit0, kern::Key::digit1, kern::Key::digit2, kern::Key::digit3, kern::Key::digit4, kern::Key::digit5, kern::Key::digit6, kern::Key::digit7, kern::Key::digit8, kern::Key::digit9, kern::Key::left, kern::Key::right, kern::Key::up, kern::Key::down, kern::Key::space, kern::Key::enter, kern::Key::escape, kern::Key::tab, kern::Key::backspace, kern::Key::shift_left, kern::Key::shift_right, kern::Key::control_left, kern::Key::control_right, kern::Key::alt_left, kern::Key::alt_right, kern::Key::count}; };
struct E_PadAxis { using type = kern::PadAxis; static constexpr type values[] = {kern::PadAxis::none, kern::PadAxis::left_x, kern::PadAxis::left_y, kern::PadAxis::right_x, kern::PadAxis::right_y, kern::PadAxis::triggers, kern::PadAxis::count}; };
struct E_PadButton { using type = kern::PadButton; static constexpr type values[] = {kern::PadButton::a, kern::PadButton::b, kern::PadButton::x, kern::PadButton::y, kern::PadButton::lb, kern::PadButton::rb, kern::PadButton::lt, kern::PadButton::rt, kern::PadButton::back, kern::PadButton::start, kern::PadButton::ls, kern::PadButton::rs, kern::PadButton::up, kern::PadButton::down, kern::PadButton::left, kern::PadButton::right, kern::PadButton::home, kern::PadButton::count}; };
struct E_TouchAxis { using type = kern::TouchAxis; static constexpr type values[] = {kern::TouchAxis::none, kern::TouchAxis::stick_x, kern::TouchAxis::stick_y, kern::TouchAxis::count}; };
struct E_TouchZone { using type = kern::TouchZone; static constexpr type values[] = {kern::TouchZone::none, kern::TouchZone::left, kern::TouchZone::right, kern::TouchZone::anywhere, kern::TouchZone::count}; };
struct S_ActionBinding { [[maybe_unused]] static Value to(EngineObjects& o, const kern::ActionBinding& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::ActionBinding& x); };
struct S_AxisBinding { [[maybe_unused]] static Value to(EngineObjects& o, const kern::AxisBinding& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::AxisBinding& x); };
struct S_Camera { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Camera& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Camera& x); };
struct S_Frame { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Frame& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Frame& x); };
struct S_Gamepad { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Gamepad& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Gamepad& x); };
struct S_InputActions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::InputActions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::InputActions& x); };
struct S_Mat4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Mat4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Mat4& x); };
struct S_RenderStats { [[maybe_unused]] static Value to(EngineObjects& o, const kern::RenderStats& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::RenderStats& x); };
struct S_Touch { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Touch& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Touch& x); };
struct S_Vec4 { [[maybe_unused]] static Value to(EngineObjects& o, const kern::Vec4& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::Vec4& x); };
Value S_ActionBinding::to(EngineObjects& o, const kern::ActionBinding& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::List<ev::Enum<E_Key>>>::to(o, x.keys), ev::Conv<ev::List<ev::Enum<E_PadButton>>>::to(o, x.pad), ev::Conv<ev::List<ev::Int>>::to(o, x.mouse), ev::Conv<ev::Enum<E_TouchZone>>::to(o, x.touch)}); }
void S_ActionBinding::from(EngineObjects& o, const Value& v, kern::ActionBinding& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Enum<E_Key>>>::from(o, *f, x.keys); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Enum<E_PadButton>>>::from(o, *f, x.pad); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.mouse); if (const Value* f = v.field(4)) ev::Conv<ev::Enum<E_TouchZone>>::from(o, *f, x.touch); }
Value S_AxisBinding::to(EngineObjects& o, const kern::AxisBinding& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::List<ev::Enum<E_Key>>>::to(o, x.negative), ev::Conv<ev::List<ev::Enum<E_Key>>>::to(o, x.positive), ev::Conv<ev::Enum<E_PadAxis>>::to(o, x.pad), ev::Conv<ev::Float>::to(o, x.deadzone), ev::Conv<ev::Enum<E_TouchAxis>>::to(o, x.touch)}); }
void S_AxisBinding::from(EngineObjects& o, const Value& v, kern::AxisBinding& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Enum<E_Key>>>::from(o, *f, x.negative); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Enum<E_Key>>>::from(o, *f, x.positive); if (const Value* f = v.field(3)) ev::Conv<ev::Enum<E_PadAxis>>::from(o, *f, x.pad); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.deadzone); if (const Value* f = v.field(5)) ev::Conv<ev::Enum<E_TouchAxis>>::from(o, *f, x.touch); }
Value S_Camera::to(EngineObjects& o, const kern::Camera& x) { return Value::structure({ev::Conv<ev::Vector>::to(o, x.eye), ev::Conv<ev::Vector>::to(o, x.target), ev::Conv<ev::Vector>::to(o, x.up), ev::Conv<ev::Float>::to(o, x.fov), ev::Conv<ev::Float>::to(o, x.near), ev::Conv<ev::Float>::to(o, x.far), ev::Conv<ev::Bool>::to(o, x.orthographic), ev::Conv<ev::Float>::to(o, x.ortho_height), ev::Conv<ev::Bool>::to(o, x.has_projection), ev::Conv<ev::Struct<S_Mat4>>::to(o, x.projection)}); }
void S_Camera::from(EngineObjects& o, const Value& v, kern::Camera& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Vector>::from(o, *f, x.eye); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.target); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.up); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.fov); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.near); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.far); if (const Value* f = v.field(6)) ev::Conv<ev::Bool>::from(o, *f, x.orthographic); if (const Value* f = v.field(7)) ev::Conv<ev::Float>::from(o, *f, x.ortho_height); if (const Value* f = v.field(8)) ev::Conv<ev::Bool>::from(o, *f, x.has_projection); if (const Value* f = v.field(9)) ev::Conv<ev::Struct<S_Mat4>>::from(o, *f, x.projection); }
Value S_Frame::to(EngineObjects& o, const kern::Frame& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.time), ev::Conv<ev::Float>::to(o, x.dt), ev::Conv<ev::Int64>::to(o, x.index)}); }
void S_Frame::from(EngineObjects& o, const Value& v, kern::Frame& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.time); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.dt); if (const Value* f = v.field(2)) ev::Conv<ev::Int64>::from(o, *f, x.index); }
Value S_Gamepad::to(EngineObjects& o, const kern::Gamepad& x) { return Value::structure({ev::Conv<ev::Bool>::to(o, x.connected), ev::Conv<ev::Float>::to(o, x.lx), ev::Conv<ev::Float>::to(o, x.ly), ev::Conv<ev::Float>::to(o, x.rx), ev::Conv<ev::Float>::to(o, x.ry), ev::Conv<ev::Float>::to(o, x.lt), ev::Conv<ev::Float>::to(o, x.rt), ev::Conv<ev::Int>::to(o, x.buttons)}); }
void S_Gamepad::from(EngineObjects& o, const Value& v, kern::Gamepad& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Bool>::from(o, *f, x.connected); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.lx); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.ly); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.rx); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.ry); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.lt); if (const Value* f = v.field(6)) ev::Conv<ev::Float>::from(o, *f, x.rt); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.buttons); }
Value S_InputActions::to(EngineObjects& o, const kern::InputActions& x) { return Value::structure({ev::Conv<ev::List<ev::Struct<S_ActionBinding>>>::to(o, x.actions), ev::Conv<ev::List<ev::Struct<S_AxisBinding>>>::to(o, x.axes)}); }
void S_InputActions::from(EngineObjects& o, const Value& v, kern::InputActions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Struct<S_ActionBinding>>>::from(o, *f, x.actions); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Struct<S_AxisBinding>>>::from(o, *f, x.axes); }
Value S_Mat4::to(EngineObjects& o, const kern::Mat4& x) { return Value::structure({ev::Conv<ev::List<ev::Float>>::to(o, x.m)}); }
void S_Mat4::from(EngineObjects& o, const Value& v, kern::Mat4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Float>>::from(o, *f, x.m); }
Value S_RenderStats::to(EngineObjects& o, const kern::RenderStats& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.draw_calls), ev::Conv<ev::Int>::to(o, x.shadow_draw_calls), ev::Conv<ev::Int>::to(o, x.shadow_resolution), ev::Conv<ev::Bool>::to(o, x.ao), ev::Conv<ev::Bool>::to(o, x.shadow_reused), ev::Conv<ev::Float>::to(o, x.triangles), ev::Conv<ev::Int>::to(o, x.instances), ev::Conv<ev::Int>::to(o, x.culled_meshes), ev::Conv<ev::Int>::to(o, x.visible_meshes), ev::Conv<ev::Float>::to(o, x.cpu_ms), ev::Conv<ev::Float>::to(o, x.gpu_ms), ev::Conv<ev::Enum<E_BloomState>>::to(o, x.bloom)}); }
void S_RenderStats::from(EngineObjects& o, const Value& v, kern::RenderStats& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.draw_calls); if (const Value* f = v.field(1)) ev::Conv<ev::Int>::from(o, *f, x.shadow_draw_calls); if (const Value* f = v.field(2)) ev::Conv<ev::Int>::from(o, *f, x.shadow_resolution); if (const Value* f = v.field(3)) ev::Conv<ev::Bool>::from(o, *f, x.ao); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.shadow_reused); if (const Value* f = v.field(5)) ev::Conv<ev::Float>::from(o, *f, x.triangles); if (const Value* f = v.field(6)) ev::Conv<ev::Int>::from(o, *f, x.instances); if (const Value* f = v.field(7)) ev::Conv<ev::Int>::from(o, *f, x.culled_meshes); if (const Value* f = v.field(8)) ev::Conv<ev::Int>::from(o, *f, x.visible_meshes); if (const Value* f = v.field(9)) ev::Conv<ev::Float>::from(o, *f, x.cpu_ms); if (const Value* f = v.field(10)) ev::Conv<ev::Float>::from(o, *f, x.gpu_ms); if (const Value* f = v.field(11)) ev::Conv<ev::Enum<E_BloomState>>::from(o, *f, x.bloom); }
Value S_Touch::to(EngineObjects& o, const kern::Touch& x) { return Value::structure({ev::Conv<ev::Int>::to(o, x.id), ev::Conv<ev::Vector>::to(o, x.start), ev::Conv<ev::Vector>::to(o, x.position), ev::Conv<ev::Vector>::to(o, x.previous), ev::Conv<ev::Bool>::to(o, x.began)}); }
void S_Touch::from(EngineObjects& o, const Value& v, kern::Touch& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Int>::from(o, *f, x.id); if (const Value* f = v.field(1)) ev::Conv<ev::Vector>::from(o, *f, x.start); if (const Value* f = v.field(2)) ev::Conv<ev::Vector>::from(o, *f, x.position); if (const Value* f = v.field(3)) ev::Conv<ev::Vector>::from(o, *f, x.previous); if (const Value* f = v.field(4)) ev::Conv<ev::Bool>::from(o, *f, x.began); }
Value S_Vec4::to(EngineObjects& o, const kern::Vec4& x) { return Value::structure({ev::Conv<ev::Float>::to(o, x.x), ev::Conv<ev::Float>::to(o, x.y), ev::Conv<ev::Float>::to(o, x.z), ev::Conv<ev::Float>::to(o, x.w)}); }
void S_Vec4::from(EngineObjects& o, const Value& v, kern::Vec4& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Float>::from(o, *f, x.x); if (const Value* f = v.field(1)) ev::Conv<ev::Float>::from(o, *f, x.y); if (const Value* f = v.field(2)) ev::Conv<ev::Float>::from(o, *f, x.z); if (const Value* f = v.field(3)) ev::Conv<ev::Float>::from(o, *f, x.w); }

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
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->button(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.input.button_pressed: kern::Input::button_pressed
bool c6(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  decltype(auto) r = self->button_pressed(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.input.wheel: kern::Input::wheel
bool c7(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  decltype(auto) r = self->wheel();
  ev::give<ev::Float>(o, out, 0, r);
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
  ev::give<ev::Int>(o, out, 0, r);
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

// engine.input.pad_down: kern::Input::pad_down
bool c12(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::PadButton>>();
  ev::take<ev::Enum<E_PadButton>>(o, in, 1, a0);
  decltype(auto) r = self->pad_down(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.input.pad_pressed: kern::Input::pad_pressed
bool c13(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::PadButton>>();
  ev::take<ev::Enum<E_PadButton>>(o, in, 1, a0);
  decltype(auto) r = self->pad_pressed(std::move(a0));
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.input.key: kern::Input::key
bool c14(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c15(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c16(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->pointer_buttons(std::move(a0));
  return true;
}

// engine.input.add_wheel: kern::Input::add_wheel
bool c17(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->add_wheel(std::move(a0));
  return true;
}

// engine.input.touch: kern::Input::touch
bool c18(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  auto a2 = ev::blank<std::remove_cvref_t<kern::Vec2>>();
  ev::take<ev::Vector>(o, in, 3, a2);
  self->touch(std::move(a0), std::move(a1), std::move(a2));
  return true;
}

// engine.input.set_focus: kern::Input::set_focus
bool c19(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_focus(std::move(a0));
  return true;
}

// engine.input.set_gamepad: kern::Input::set_gamepad
bool c20(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Gamepad const&>>();
  ev::take<ev::Struct<S_Gamepad>>(o, in, 1, a0);
  self->set_gamepad(a0);
  return true;
}

// engine.input.end_frame: kern::Input::end_frame
bool c21(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Input>(o, in, 0, "input");
  if (!self) return false;
  self->end_frame();
  return true;
}

// engine.engine.renderer: kern::Engine::renderer
bool c22(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->renderer();
  out[0] = Value::integer(ev::handle_of<kern::Renderer>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.engine.input: kern::Engine::input
bool c23(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->input();
  out[0] = Value::integer(ev::handle_of<kern::Input>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.engine.input_2: kern::Engine::input
bool c24(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->input();
  out[0] = Value::integer(ev::handle_of<kern::Input>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.engine.mixer: kern::Engine::mixer
bool c25(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->mixer();
  out[0] = Value::integer(ev::handle_of<kern::audio::Mixer>(o, std::forward<decltype(r)>(r)));
  return true;
}

// engine.engine.action: kern::Engine::action
bool c26(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->action(a0.c_str());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.action_pressed: kern::Engine::action_pressed
bool c27(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->action_pressed(a0.c_str());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.axis: kern::Engine::axis
bool c28(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->axis(a0.c_str());
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.engine.set_actions: kern::Engine::set_actions
bool c29(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::InputActions const&>>();
  ev::take<ev::Struct<S_InputActions>>(o, in, 1, a0);
  self->set_actions(a0);
  return true;
}

// engine.engine.actions: kern::Engine::actions
bool c30(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->actions();
  ev::give<ev::Struct<S_InputActions>>(o, out, 0, r);
  return true;
}

// engine.engine.set_status: kern::Engine::set_status
bool c31(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  self->set_status(a0.c_str());
  return true;
}

// engine.engine.request_pointer_lock: kern::Engine::request_pointer_lock
bool c32(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->request_pointer_lock(std::move(a0));
  return true;
}

// engine.engine.arg: kern::Engine::arg
bool c33(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c34(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  decltype(auto) r = self->has_arg(a0.c_str());
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.arg_number: kern::Engine::arg_number
bool c35(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<double>>();
  ev::take<ev::Float>(o, in, 2, a1);
  decltype(auto) r = self->arg_number(a0.c_str(), std::move(a1));
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.engine.set_args: kern::Engine::set_args
bool c36(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  std::string a0;
  ev::take<ev::Text>(o, in, 1, a0);
  auto a1 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 2, a1);
  self->set_args(a0.c_str(), std::move(a1));
  return true;
}

// engine.engine.width: kern::Engine::width
bool c37(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->width();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.engine.height: kern::Engine::height
bool c38(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->height();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.engine.aspect: kern::Engine::aspect
bool c39(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->aspect();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.engine.stats: kern::Engine::stats
bool c40(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->stats();
  ev::give<ev::Struct<S_RenderStats>>(o, out, 0, r);
  return true;
}

// engine.engine.clear_color: kern::Engine::clear_color
bool c41(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->clear_color();
  ev::give<ev::Struct<S_Vec4>>(o, out, 0, r);
  return true;
}

// engine.engine.set_clear_color: kern::Engine::set_clear_color
bool c42(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec4>>();
  ev::take<ev::Struct<S_Vec4>>(o, in, 1, a0);
  self->set_clear_color(std::move(a0));
  return true;
}

// engine.engine.xr: kern::Engine::xr
bool c43(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr: kern::Engine::set_xr
bool c44(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_xr(std::move(a0));
  return true;
}

// engine.engine.xr_eye: kern::Engine::xr_eye
bool c45(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_eye();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_eye: kern::Engine::set_xr_eye
bool c46(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->set_xr_eye(std::move(a0));
  return true;
}

// engine.engine.xr_forward: kern::Engine::xr_forward
bool c47(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_forward();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_forward: kern::Engine::set_xr_forward
bool c48(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->set_xr_forward(std::move(a0));
  return true;
}

// engine.engine.xr_room_forward: kern::Engine::xr_room_forward
bool c49(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_room_forward();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_room_forward: kern::Engine::set_xr_room_forward
bool c50(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->set_xr_room_forward(std::move(a0));
  return true;
}

// engine.engine.xr_head_yaw: kern::Engine::xr_head_yaw
bool c51(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_head_yaw();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_head_yaw: kern::Engine::set_xr_head_yaw
bool c52(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->set_xr_head_yaw(std::move(a0));
  return true;
}

// engine.engine.xr_follow_head: kern::Engine::xr_follow_head
bool c53(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_follow_head();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_follow_head: kern::Engine::set_xr_follow_head
bool c54(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_xr_follow_head(std::move(a0));
  return true;
}

// engine.engine.xr_hold_room: kern::Engine::xr_hold_room
bool c55(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_hold_room();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_hold_room: kern::Engine::set_xr_hold_room
bool c56(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_xr_hold_room(std::move(a0));
  return true;
}

// engine.engine.xr_full_room: kern::Engine::xr_full_room
bool c57(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_full_room();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_full_room: kern::Engine::set_xr_full_room
bool c58(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_xr_full_room(std::move(a0));
  return true;
}

// engine.engine.xr_room_up: kern::Engine::xr_room_up
bool c59(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_room_up();
  ev::give<ev::Vector>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_room_up: kern::Engine::set_xr_room_up
bool c60(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Vec3>>();
  ev::take<ev::Vector>(o, in, 1, a0);
  self->set_xr_room_up(std::move(a0));
  return true;
}

// engine.engine.xr_vignette: kern::Engine::xr_vignette
bool c61(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_vignette();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_vignette: kern::Engine::set_xr_vignette
bool c62(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->set_xr_vignette(std::move(a0));
  return true;
}

// engine.engine.xr_vignette_mesh: kern::Engine::xr_vignette_mesh
bool c63(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_vignette_mesh();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_vignette_mesh: kern::Engine::set_xr_vignette_mesh
bool c64(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->set_xr_vignette_mesh(std::move(a0));
  return true;
}

// engine.engine.xr_scope: kern::Engine::xr_scope
bool c65(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_scope();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_scope: kern::Engine::set_xr_scope
bool c66(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_xr_scope(std::move(a0));
  return true;
}

// engine.engine.xr_scope_camera: kern::Engine::xr_scope_camera
bool c67(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_scope_camera();
  ev::give<ev::Struct<S_Camera>>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_scope_camera: kern::Engine::set_xr_scope_camera
bool c68(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<kern::Camera const&>>();
  ev::take<ev::Struct<S_Camera>>(o, in, 1, a0);
  self->set_xr_scope_camera(a0);
  return true;
}

// engine.engine.xr_scope_fbo: kern::Engine::xr_scope_fbo
bool c69(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_scope_fbo();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_scope_fbo: kern::Engine::set_xr_scope_fbo
bool c70(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<unsigned int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->set_xr_scope_fbo(std::move(a0));
  return true;
}

// engine.engine.xr_scope_size: kern::Engine::xr_scope_size
bool c71(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->xr_scope_size();
  ev::give<ev::Int>(o, out, 0, r);
  return true;
}

// engine.engine.set_xr_scope_size: kern::Engine::set_xr_scope_size
bool c72(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<int>>();
  ev::take<ev::Int>(o, in, 1, a0);
  self->set_xr_scope_size(std::move(a0));
  return true;
}

// engine.engine.stereo: kern::Engine::stereo
bool c73(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->stereo();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_stereo: kern::Engine::set_stereo
bool c74(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_stereo(std::move(a0));
  return true;
}

// engine.engine.stereo_ipd: kern::Engine::stereo_ipd
bool c75(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->stereo_ipd();
  ev::give<ev::Float>(o, out, 0, r);
  return true;
}

// engine.engine.set_stereo_ipd: kern::Engine::set_stereo_ipd
bool c76(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<float>>();
  ev::take<ev::Float>(o, in, 1, a0);
  self->set_stereo_ipd(std::move(a0));
  return true;
}

// engine.engine.stereo_cross: kern::Engine::stereo_cross
bool c77(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  decltype(auto) r = self->stereo_cross();
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.engine.set_stereo_cross: kern::Engine::set_stereo_cross
bool c78(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::Engine>(o, in, 0, "engine");
  if (!self) return false;
  auto a0 = ev::blank<std::remove_cvref_t<bool>>();
  ev::take<ev::Bool>(o, in, 1, a0);
  self->set_stereo_cross(std::move(a0));
  return true;
}

// engine.app.extensions: kern::App::extensions
bool c79(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::App>(o, in, 0, "app");
  if (!self) return false;
  decltype(auto) r = self->extensions();
  ev::give<ev::List<ev::Int>>(o, out, 0, r);
  return true;
}

// engine.app.start: kern::App::start
bool c80(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::App>(o, in, 0, "app");
  if (!self) return false;
  kern::Engine* a0 = ev::object<kern::Engine>(o, in, 1, "engine");
  if (!a0) return false;
  decltype(auto) r = self->start(*a0);
  ev::give<ev::Bool>(o, out, 0, r);
  return true;
}

// engine.app.update: kern::App::update
bool c81(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
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
bool c82(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::App>(o, in, 0, "app");
  if (!self) return false;
  kern::Engine* a0 = ev::object<kern::Engine>(o, in, 1, "engine");
  if (!a0) return false;
  self->resized(*a0);
  return true;
}

// engine.app.stop: kern::App::stop
bool c83(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto* self = ev::object<kern::App>(o, in, 0, "app");
  if (!self) return false;
  kern::Engine* a0 = ev::object<kern::Engine>(o, in, 1, "engine");
  if (!a0) return false;
  self->stop(*a0);
  return true;
}

// engine.engine.make: kern::Engine::Engine
bool c84(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  kern::Renderer* a0 = ev::object<kern::Renderer>(o, in, 0, "renderer");
  if (!a0) return false;
  out[0] = Value::integer(o.own<kern::Engine>(std::unique_ptr<kern::Engine>(new kern::Engine(*a0))));
  return true;
}

// engine.engine.release: kern::Engine::~Engine
bool c85(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::Engine>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no Engine that was made";
    return false;
  }
  return true;
}

// engine.input.make: kern::Input::Input
bool c86(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  out[0] = Value::integer(o.own<kern::Input>(std::unique_ptr<kern::Input>(new kern::Input())));
  return true;
}

// engine.input.release: kern::Input::~Input
bool c87(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  if (!o.release<kern::Input>(in.empty() ? 0 : in[0].as_int())) {
    o.error = "the handle names no Input that was made";
    return false;
  }
  return true;
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
    if (kern_given) ev::Conv<ev::List<ev::Int>>::from(o_, kern_out[0], kern_kept_extensions);
    return decltype(std::declval<kern::App&>().extensions())(kern_kept_extensions.data(), kern_kept_extensions.size());
  }
  decltype(std::declval<kern::App&>().start(std::declval<kern::Engine&>())) start(kern::Engine& kern_p0) override {
    const std::vector<Value> kern_in{Value::integer(o_.adopt<kern::Engine>(kern_p0))};
    std::vector<Value> kern_out;
    const bool kern_given = by_->call("start", kern_in, kern_out) && !kern_out.empty();
    decltype(std::declval<kern::App&>().start(std::declval<kern::Engine&>())) kern_r = ev::blank<decltype(std::declval<kern::App&>().start(std::declval<kern::Engine&>()))>();
    if (kern_given) ev::Conv<ev::Bool>::from(o_, kern_out[0], kern_r);
    return kern_r;
  }
  decltype(std::declval<kern::App&>().update(std::declval<kern::Engine&>(), std::declval<kern::Frame const&>())) update(kern::Engine& kern_p0, kern::Frame const& kern_p1) override {
    const std::vector<Value> kern_in{Value::integer(o_.adopt<kern::Engine>(kern_p0)), ev::Conv<ev::Struct<S_Frame>>::to(o_, kern_p1)};
    std::vector<Value> kern_out;
    by_->call("update", kern_in, kern_out);
  }
  decltype(std::declval<kern::App&>().resized(std::declval<kern::Engine&>())) resized(kern::Engine& kern_p0) override {
    if (!by_->has("resized")) return kern::App::resized(kern_p0);
    const std::vector<Value> kern_in{Value::integer(o_.adopt<kern::Engine>(kern_p0))};
    std::vector<Value> kern_out;
    by_->call("resized", kern_in, kern_out);
  }
  decltype(std::declval<kern::App&>().stop(std::declval<kern::Engine&>())) stop(kern::Engine& kern_p0) override {
    if (!by_->has("stop")) return kern::App::stop(kern_p0);
    const std::vector<Value> kern_in{Value::integer(o_.adopt<kern::Engine>(kern_p0))};
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
    {"engine.input.pad_down", &c12},
    {"engine.input.pad_pressed", &c13},
    {"engine.input.key", &c14},
    {"engine.input.pointer_move", &c15},
    {"engine.input.pointer_buttons", &c16},
    {"engine.input.add_wheel", &c17},
    {"engine.input.touch", &c18},
    {"engine.input.set_focus", &c19},
    {"engine.input.set_gamepad", &c20},
    {"engine.input.end_frame", &c21},
    {"engine.engine.renderer", &c22},
    {"engine.engine.input", &c23},
    {"engine.engine.input_2", &c24},
    {"engine.engine.mixer", &c25},
    {"engine.engine.action", &c26},
    {"engine.engine.action_pressed", &c27},
    {"engine.engine.axis", &c28},
    {"engine.engine.set_actions", &c29},
    {"engine.engine.actions", &c30},
    {"engine.engine.set_status", &c31},
    {"engine.engine.request_pointer_lock", &c32},
    {"engine.engine.arg", &c33},
    {"engine.engine.has_arg", &c34},
    {"engine.engine.arg_number", &c35},
    {"engine.engine.set_args", &c36},
    {"engine.engine.width", &c37},
    {"engine.engine.height", &c38},
    {"engine.engine.aspect", &c39},
    {"engine.engine.stats", &c40},
    {"engine.engine.clear_color", &c41},
    {"engine.engine.set_clear_color", &c42},
    {"engine.engine.xr", &c43},
    {"engine.engine.set_xr", &c44},
    {"engine.engine.xr_eye", &c45},
    {"engine.engine.set_xr_eye", &c46},
    {"engine.engine.xr_forward", &c47},
    {"engine.engine.set_xr_forward", &c48},
    {"engine.engine.xr_room_forward", &c49},
    {"engine.engine.set_xr_room_forward", &c50},
    {"engine.engine.xr_head_yaw", &c51},
    {"engine.engine.set_xr_head_yaw", &c52},
    {"engine.engine.xr_follow_head", &c53},
    {"engine.engine.set_xr_follow_head", &c54},
    {"engine.engine.xr_hold_room", &c55},
    {"engine.engine.set_xr_hold_room", &c56},
    {"engine.engine.xr_full_room", &c57},
    {"engine.engine.set_xr_full_room", &c58},
    {"engine.engine.xr_room_up", &c59},
    {"engine.engine.set_xr_room_up", &c60},
    {"engine.engine.xr_vignette", &c61},
    {"engine.engine.set_xr_vignette", &c62},
    {"engine.engine.xr_vignette_mesh", &c63},
    {"engine.engine.set_xr_vignette_mesh", &c64},
    {"engine.engine.xr_scope", &c65},
    {"engine.engine.set_xr_scope", &c66},
    {"engine.engine.xr_scope_camera", &c67},
    {"engine.engine.set_xr_scope_camera", &c68},
    {"engine.engine.xr_scope_fbo", &c69},
    {"engine.engine.set_xr_scope_fbo", &c70},
    {"engine.engine.xr_scope_size", &c71},
    {"engine.engine.set_xr_scope_size", &c72},
    {"engine.engine.stereo", &c73},
    {"engine.engine.set_stereo", &c74},
    {"engine.engine.stereo_ipd", &c75},
    {"engine.engine.set_stereo_ipd", &c76},
    {"engine.engine.stereo_cross", &c77},
    {"engine.engine.set_stereo_cross", &c78},
    {"engine.app.extensions", &c79},
    {"engine.app.start", &c80},
    {"engine.app.update", &c81},
    {"engine.app.resized", &c82},
    {"engine.app.stop", &c83},
    {"engine.engine.make", &c84},
    {"engine.engine.release", &c85},
    {"engine.input.make", &c86},
    {"engine.input.release", &c87},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

const EngineMaker kMakers[] = {
    {EngineObjects::kind<kern::Engine>(), [](EngineObjects& o, i64 handle) -> void* { auto made = std::make_unique<ev::MadeWith<kern::Engine>>(); auto& d0 = made->keep(new kern::Renderer()); made->object.emplace(d0); return o.put_made<kern::Engine>(handle, std::move(made)); }},
    {EngineObjects::kind<kern::Input>(), [](EngineObjects& o, i64 handle) -> void* { return o.put_at<kern::Input>(handle, std::unique_ptr<kern::Input>(new kern::Input())); }},
};
const EngineMakers kAllMakers(kMakers, std::size(kMakers));

const EngineImplement kAdapters[] = {
    {"App", &implements_App},
};
const EngineImplements kAllAdapters(kAdapters, std::size(kAdapters));

}  // namespace
}  // namespace kern::logic
