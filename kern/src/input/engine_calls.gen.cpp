// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/input/actions.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct E_Key { using type = kern::Key; static constexpr type values[] = {kern::Key::unknown, kern::Key::a, kern::Key::b, kern::Key::c, kern::Key::d, kern::Key::e, kern::Key::f, kern::Key::g, kern::Key::h, kern::Key::i, kern::Key::j, kern::Key::k, kern::Key::l, kern::Key::m, kern::Key::n, kern::Key::o, kern::Key::p, kern::Key::q, kern::Key::r, kern::Key::s, kern::Key::t, kern::Key::u, kern::Key::v, kern::Key::w, kern::Key::x, kern::Key::y, kern::Key::z, kern::Key::digit0, kern::Key::digit1, kern::Key::digit2, kern::Key::digit3, kern::Key::digit4, kern::Key::digit5, kern::Key::digit6, kern::Key::digit7, kern::Key::digit8, kern::Key::digit9, kern::Key::left, kern::Key::right, kern::Key::up, kern::Key::down, kern::Key::space, kern::Key::enter, kern::Key::escape, kern::Key::tab, kern::Key::backspace, kern::Key::shift_left, kern::Key::shift_right, kern::Key::control_left, kern::Key::control_right, kern::Key::alt_left, kern::Key::alt_right, kern::Key::count}; };
struct E_PadAxis { using type = kern::PadAxis; static constexpr type values[] = {kern::PadAxis::none, kern::PadAxis::left_x, kern::PadAxis::left_y, kern::PadAxis::right_x, kern::PadAxis::right_y, kern::PadAxis::triggers, kern::PadAxis::count}; };
struct E_PadButton { using type = kern::PadButton; static constexpr type values[] = {kern::PadButton::a, kern::PadButton::b, kern::PadButton::x, kern::PadButton::y, kern::PadButton::lb, kern::PadButton::rb, kern::PadButton::lt, kern::PadButton::rt, kern::PadButton::back, kern::PadButton::start, kern::PadButton::ls, kern::PadButton::rs, kern::PadButton::up, kern::PadButton::down, kern::PadButton::left, kern::PadButton::right, kern::PadButton::home, kern::PadButton::count}; };
struct E_TouchAxis { using type = kern::TouchAxis; static constexpr type values[] = {kern::TouchAxis::none, kern::TouchAxis::stick_x, kern::TouchAxis::stick_y, kern::TouchAxis::count}; };
struct E_TouchZone { using type = kern::TouchZone; static constexpr type values[] = {kern::TouchZone::none, kern::TouchZone::left, kern::TouchZone::right, kern::TouchZone::anywhere, kern::TouchZone::count}; };
struct S_ActionBinding { [[maybe_unused]] static Value to(EngineObjects& o, const kern::ActionBinding& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::ActionBinding& x); };
struct S_AxisBinding { [[maybe_unused]] static Value to(EngineObjects& o, const kern::AxisBinding& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::AxisBinding& x); };
struct S_InputActions { [[maybe_unused]] static Value to(EngineObjects& o, const kern::InputActions& x); [[maybe_unused]] static void from(EngineObjects& o, const Value& v, kern::InputActions& x); };
Value S_ActionBinding::to(EngineObjects& o, const kern::ActionBinding& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::List<ev::Enum<E_Key>>>::to(o, x.keys), ev::Conv<ev::List<ev::Enum<E_PadButton>>>::to(o, x.pad), ev::Conv<ev::List<ev::Int>>::to(o, x.mouse), ev::Conv<ev::Enum<E_TouchZone>>::to(o, x.touch)}); }
void S_ActionBinding::from(EngineObjects& o, const Value& v, kern::ActionBinding& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Enum<E_Key>>>::from(o, *f, x.keys); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Enum<E_PadButton>>>::from(o, *f, x.pad); if (const Value* f = v.field(3)) ev::Conv<ev::List<ev::Int>>::from(o, *f, x.mouse); if (const Value* f = v.field(4)) ev::Conv<ev::Enum<E_TouchZone>>::from(o, *f, x.touch); }
Value S_AxisBinding::to(EngineObjects& o, const kern::AxisBinding& x) { return Value::structure({ev::Conv<ev::Text>::to(o, x.name), ev::Conv<ev::List<ev::Enum<E_Key>>>::to(o, x.negative), ev::Conv<ev::List<ev::Enum<E_Key>>>::to(o, x.positive), ev::Conv<ev::Enum<E_PadAxis>>::to(o, x.pad), ev::Conv<ev::Float>::to(o, x.deadzone), ev::Conv<ev::Enum<E_TouchAxis>>::to(o, x.touch)}); }
void S_AxisBinding::from(EngineObjects& o, const Value& v, kern::AxisBinding& x) { if (const Value* f = v.field(0)) ev::Conv<ev::Text>::from(o, *f, x.name); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Enum<E_Key>>>::from(o, *f, x.negative); if (const Value* f = v.field(2)) ev::Conv<ev::List<ev::Enum<E_Key>>>::from(o, *f, x.positive); if (const Value* f = v.field(3)) ev::Conv<ev::Enum<E_PadAxis>>::from(o, *f, x.pad); if (const Value* f = v.field(4)) ev::Conv<ev::Float>::from(o, *f, x.deadzone); if (const Value* f = v.field(5)) ev::Conv<ev::Enum<E_TouchAxis>>::from(o, *f, x.touch); }
Value S_InputActions::to(EngineObjects& o, const kern::InputActions& x) { return Value::structure({ev::Conv<ev::List<ev::Struct<S_ActionBinding>>>::to(o, x.actions), ev::Conv<ev::List<ev::Struct<S_AxisBinding>>>::to(o, x.axes)}); }
void S_InputActions::from(EngineObjects& o, const Value& v, kern::InputActions& x) { if (const Value* f = v.field(0)) ev::Conv<ev::List<ev::Struct<S_ActionBinding>>>::from(o, *f, x.actions); if (const Value* f = v.field(1)) ev::Conv<ev::List<ev::Struct<S_AxisBinding>>>::from(o, *f, x.axes); }

// engine.key_name: kern::key_name
bool c0(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::Key>>();
  ev::take<ev::Enum<E_Key>>(o, in, 0, a0);
  decltype(auto) r = kern::key_name(std::move(a0));
  ev::give<ev::Text>(o, out, 0, r);
  return true;
}

// engine.parse_key: kern::parse_key
bool c1(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Key&>>();
  ev::take<ev::Enum<E_Key>>(o, in, 1, a1);
  decltype(auto) r = kern::parse_key(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Enum<E_Key>>(o, out, 1, a1);
  return true;
}

// engine.input_to_json: kern::input_to_json
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  auto a0 = ev::blank<std::remove_cvref_t<kern::InputActions const&>>();
  ev::take<ev::Struct<S_InputActions>>(o, in, 0, a0);
  decltype(auto) r = kern::input_to_json(a0);
  out[0] = Value::integer(ev::handle_of<kern::json::Value>(o, std::forward<decltype(r)>(r)));
  return true;
}

const EngineCall kCalls[] = {
    {"engine.key_name", &c0},
    {"engine.parse_key", &c1},
    {"engine.input_to_json", &c2},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

}  // namespace
}  // namespace kern::logic
