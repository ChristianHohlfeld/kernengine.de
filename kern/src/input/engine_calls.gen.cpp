// The engine's nodes on the engine itself: the calls of the functions the headers of this component declare
// (written by tools/engine_api.py from the engine's headers - see kern/logic/engine_host.h; not edited by hand).
#include <algorithm>
#include "kern/input/actions.h"
#include "kern/logic/engine_host.h"

namespace kern::logic {
namespace {

namespace ev = engine_values;

struct E_Key { using type = kern::Key; static constexpr type values[] = {kern::Key::unknown, kern::Key::a, kern::Key::b, kern::Key::c, kern::Key::d, kern::Key::e, kern::Key::f, kern::Key::g, kern::Key::h, kern::Key::i, kern::Key::j, kern::Key::k, kern::Key::l, kern::Key::m, kern::Key::n, kern::Key::o, kern::Key::p, kern::Key::q, kern::Key::r, kern::Key::s, kern::Key::t, kern::Key::u, kern::Key::v, kern::Key::w, kern::Key::x, kern::Key::y, kern::Key::z, kern::Key::digit0, kern::Key::digit1, kern::Key::digit2, kern::Key::digit3, kern::Key::digit4, kern::Key::digit5, kern::Key::digit6, kern::Key::digit7, kern::Key::digit8, kern::Key::digit9, kern::Key::left, kern::Key::right, kern::Key::up, kern::Key::down, kern::Key::space, kern::Key::enter, kern::Key::escape, kern::Key::tab, kern::Key::backspace, kern::Key::shift_left, kern::Key::shift_right, kern::Key::control_left, kern::Key::control_right, kern::Key::alt_left, kern::Key::alt_right, kern::Key::count}; };

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

// engine.key_of_code: kern::key_of_code
bool c2(EngineObjects& o, const std::vector<Value>& in, std::vector<Value>& out) {
  std::string a0;
  ev::take<ev::Text>(o, in, 0, a0);
  auto a1 = ev::blank<std::remove_cvref_t<kern::Key&>>();
  ev::take<ev::Enum<E_Key>>(o, in, 1, a1);
  decltype(auto) r = kern::key_of_code(a0, a1);
  ev::give<ev::Bool>(o, out, 0, r);
  ev::give<ev::Enum<E_Key>>(o, out, 1, a1);
  return true;
}

const EngineCall kCalls[] = {
    {"engine.key_name", &c0},
    {"engine.parse_key", &c1},
    {"engine.key_of_code", &c2},
};
const EngineCalls kAll(kCalls, std::size(kCalls));

}  // namespace
}  // namespace kern::logic
